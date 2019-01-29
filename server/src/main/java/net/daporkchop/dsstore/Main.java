package net.daporkchop.dsstore;

import io.netty.bootstrap.ServerBootstrap;
import io.netty.buffer.ByteBuf;
import io.netty.buffer.Unpooled;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandler;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import io.netty.channel.ChannelOption;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.nio.NioServerSocketChannel;
import net.daporkchop.lib.common.util.PorkUtil;
import net.daporkchop.lib.nds.RomNDS;
import net.daporkchop.lib.nds.header.RomIcon;
import net.daporkchop.lib.nds.header.RomLanguage;
import net.daporkchop.lib.nds.header.RomTitle;

import java.io.File;
import java.io.IOException;
import java.util.Hashtable;
import java.util.Map;
import java.util.Scanner;
import java.util.concurrent.ThreadLocalRandom;
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
        bootstrap.childOption(ChannelOption.SO_KEEPALIVE, true);

        Channel channel = bootstrap.bind(8234).syncUninterruptibly().channel();
        try (Scanner scanner = new Scanner(System.in)) {
            scanner.nextLine();
        }
        channel.close().syncUninterruptibly();
        group.shutdownGracefully();
    }

    @ChannelHandler.Sharable
    protected static class PHandler extends ChannelInboundHandlerAdapter {
        protected final File[] roms = new File("/media/daporkchop/TooMuchStuff/Misc/ds").listFiles((file, name) -> name.endsWith(".nds"));

        @Override
        public void channelRegistered(ChannelHandlerContext ctx) throws Exception {
            System.out.printf("Incoming connection: %s\n", ctx.channel().remoteAddress());

            super.channelRegistered(ctx);
        }

        @Override
        public void channelUnregistered(ChannelHandlerContext ctx) throws Exception {
            System.out.printf("Connection closed: %s\n", ctx.channel().remoteAddress());
            super.channelUnregistered(ctx);
        }

        @Override
        public void channelRead(ChannelHandlerContext ctx, Object msg) throws Exception {
            if (true)   {
                ByteBuf buf = ctx.alloc().ioBuffer();
                for (int i = 0; i < 128; i++)   {
                    buf.writeByte(ThreadLocalRandom.current().nextInt(256));
                }
                ctx.writeAndFlush(buf);
                return;
            }

            int id = ((ByteBuf) msg).readIntLE();
            System.out.printf("Sending content for ROM #%d\n", id);
            try (RomNDS rom = new RomNDS(this.roms[id])) {
                RomIcon icon = rom.getHeaders().getIconTitle().getIcon();
                RomTitle title = rom.getHeaders().getIconTitle().getTitle(RomLanguage.ENGLISH);
                ByteBuf buf = ctx.alloc().ioBuffer();
                for (int x = 31; x >= 0; x--) {
                    for (int y = 31; y >= 0; y--) {
                        short s = icon.getColor(x, y);
                        buf.writeByte(((s >>> 10) & 0x1F) | (s == 0 ? 0 : 0x80));
                        buf.writeByte((s >>> 5) & 0x1F);
                        buf.writeByte(s & 0x1F);
                    }
                }
                ctx.channel().write(buf);
                byte[] b = new byte[256];
                buf = Unpooled.wrappedBuffer(b).writerIndex(0);
                byte[] bytesTitle = title.getTitle().getBytes();
                byte[] bytesSubtitle = title.getSubtitle().getBytes();
                byte[] bytesManufacturer = title.getManufacturer().getBytes();
                buf.writeBytes(bytesTitle).writeByte(0);
                buf.writeBytes(bytesSubtitle).writeByte(0);
                buf.writeBytes(bytesManufacturer).writeByte(0);
                ctx.channel().writeAndFlush(Unpooled.wrappedBuffer(b));
            }
            super.channelRead(ctx, msg);
        }
    }
}
