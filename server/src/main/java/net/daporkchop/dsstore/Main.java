package net.daporkchop.dsstore;

import io.netty.bootstrap.ServerBootstrap;
import io.netty.buffer.ByteBuf;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandler;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioServerSocketChannel;
import net.daporkchop.lib.binary.stream.StreamUtil;
import net.daporkchop.lib.common.util.PorkUtil;
import net.daporkchop.lib.nds.RomNDS;
import net.daporkchop.lib.nds.header.RomIcon;
import sun.misc.IOUtils;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.Map;
import java.util.Queue;
import java.util.Scanner;
import java.util.Set;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * @author DaPorkchop_
 */
public class Main {
    public static final AtomicBoolean running = new AtomicBoolean(true);
    private static final char[] HEX = "0123456789abcdef".toCharArray();
    private static final File ROOT_FOLDER = new File(".", "games");
    private static final File INFO_FOLDER = new File(".", "info");
    private static byte[] FILE_LIST;
    private static final int DEFAULT_VERSION = 4;

    static {
        if (!ROOT_FOLDER.exists()) {
            ROOT_FOLDER.mkdirs();
        }
    }

    public static void main(String... args) throws IOException {
        if (args.length == 1 && "cleanup".equals(args[0])) {
            File[] files = ROOT_FOLDER.listFiles();
            Map<String, Character> finishedGames = new Hashtable<>();
            for (File file : files) {
                RENAME:
                {
                    String name = file.getName();
                    try {
                        Integer.parseInt(name.substring(0, 4));
                        if (name.substring(4, 7).equals(" - ")) {
                            name = name.substring(7);
                        } else if (name.substring(4, 7).equals("_-_")) {
                            break RENAME;
                        }
                    } catch (NumberFormatException e) {
                    }
                    if (!(name.contains("(U)") || name.contains("(E)") || name.contains("(US)") || name.contains("(EU)"))) {
                        break RENAME;
                    }
                    char[] text = name.toCharArray();
                    char region = text[0];
                    int i;
                    for (i = 1; i < text.length; i++) {
                        if (region == '(' && text[i + 1] == ')') {
                            region = text[i];
                            break;
                        }
                        if (region == '(' && text[i + 2] == ')') {
                            if (text[i] == 'U' && text[i + 1] == 'S') {
                                region = 'U';
                                break;
                            }
                            if (text[i] == 'E' && text[i + 1] == 'U') {
                                region = 'E';
                                break;
                            }
                            break RENAME;
                        }
                        if (region == '(' && text[i + 3] == ')') {
                            if (text[i] == 'U' && text[i + 1] == 'S' && text[i + 2] == 'A') {
                                region = 'U';
                                break;
                            }
                            break RENAME;
                        }
                        region = text[i];
                    }
                    if (!(region == 'U' || region == 'E')) {
                        break RENAME;
                    }
                    String trimmed = name.substring(0, i - 1);
                    System.out.println(trimmed);
                    CHECKREGION:
                    if (finishedGames.containsKey(trimmed)) {
                        if (region == 'U' && finishedGames.get(trimmed) == 'E') {
                            break CHECKREGION;
                        } else {
                            break RENAME;
                        }
                    }
                    //strip extra things
                    if (trimmed.contains("(")) {
                        trimmed = trimmed.substring(0, trimmed.indexOf("(")).trim();
                        if (file.getName().toLowerCase().contains("(DSi Enhanced)".toLowerCase())) {
                            trimmed += " (DSi Enhanced)";
                        }
                    }
                    file.renameTo(new File(ROOT_FOLDER, trimmed.trim() + ".nds"));
                    finishedGames.put(trimmed, region);
                    continue;
                }
                System.out.println("Deleting " + file.getName());
                file.delete();
            }
            return;
        }

        EventLoopGroup group = new NioEventLoopGroup(Runtime.getRuntime().availableProcessors(), PorkUtil.DEFAULT_EXECUTOR);
        ServerBootstrap bootstrap = new ServerBootstrap();
        bootstrap.group(group, group);
        bootstrap.channel(NioServerSocketChannel.class);
        bootstrap.childHandler(new PHandler());

        Channel channel = bootstrap.bind(8234).syncUninterruptibly().channel();
        try (Scanner scanner = new Scanner(System.in))  {
            scanner.nextLine();
        }
        channel.close().syncUninterruptibly();
        group.shutdownGracefully();
    }

    @ChannelHandler.Sharable
    protected static class PHandler extends ChannelInboundHandlerAdapter    {
        @Override
        public void channelRegistered(ChannelHandlerContext ctx) throws Exception {
            System.out.printf("Incoming connection: %s\n", ctx.channel().remoteAddress());
            try (RomNDS rom = new RomNDS(new File("/home/daporkchop/192.168.1.119/Torrents/ROMs/New Super Mario Bros.nds")))    {
                RomIcon icon = rom.getHeaders().getIconTitle().getIcon();
                ByteBuf buf = ctx.alloc().ioBuffer();
                if (false) {
                    for (short s : icon.getPalette()) {
                        buf.writeByte(((s >>> 10) & 0x1F) | (s == 0 ? 0 : 0x80));
                        buf.writeByte((s >>> 5) & 0x1F);
                        buf.writeByte(s & 0x1F);
                    }
                    ctx.channel().writeAndFlush(buf);
                    buf = ctx.alloc().ioBuffer();
                    buf.writeBytes(icon.getPixels());
                    ctx.channel().writeAndFlush(buf);
                } else if (true)    {
                    for (int x = 31; x >= 0; x--)   {
                        for (int y = 31; y >= 0; y--)   {
                            short s = icon.getColor(x, y);
                            buf.writeByte(((s >>> 10) & 0x1F) | (s == 0 ? 0 : 0x80));
                            buf.writeByte((s >>> 5) & 0x1F);
                            buf.writeByte(s & 0x1F);
                        }
                    }
                    ctx.channel().writeAndFlush(buf);
                }
                //ctx.close();
            }
            super.channelRegistered(ctx);
        }

        @Override
        public void channelRead(ChannelHandlerContext ctx, Object msg) throws Exception {
            super.channelRead(ctx, msg);
        }
    }
}
