package net.daporkchop.dsstore;

import io.netty.bootstrap.ServerBootstrap;
import io.netty.buffer.ByteBuf;
import io.netty.channel.Channel;
import io.netty.channel.ChannelHandler;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.ChannelOption;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.nio.NioServerSocketChannel;
import lombok.NonNull;
import lombok.RequiredArgsConstructor;
import net.daporkchop.lib.common.function.io.IOBiConsumer;
import net.daporkchop.lib.common.util.PorkUtil;
import net.daporkchop.lib.nds.RomNDS;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.nio.charset.Charset;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.Map;
import java.util.Scanner;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.function.BiConsumer;
import java.util.function.Consumer;
import java.util.stream.Collectors;

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
    @SuppressWarnings("unchecked")
    public static BiConsumer<Channel, ByteBuf>[] HANDLERS = (BiConsumer<Channel, ByteBuf>[]) new BiConsumer[256];
    public static Collection<Channel> CHANNELS = Collections.synchronizedCollection(new HashSet<>());



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
        bootstrap.childHandler(new ChannelInitializer<Channel>() {
            @Override
            protected void initChannel(Channel ch) throws Exception {
                ch.pipeline().addLast("pork", new PHandler(ch));
            }
        });
        bootstrap.option(ChannelOption.SO_BACKLOG, 256);
        bootstrap.childOption(ChannelOption.SO_KEEPALIVE, true);

        setHandlers();
        Channel channel = bootstrap.bind(8234).syncUninterruptibly().channel();
        {
            Thread t = new Thread(() -> {
                try {
                    while (true) {
                        Thread.sleep(1000L);
                        CHANNELS.stream()
                                .map(ch -> ch.pipeline().get(PHandler.class))
                                .filter(handler -> handler.lastActive + TimeUnit.MINUTES.toMillis(2L) < System.currentTimeMillis())
                                .collect(Collectors.toList()).forEach(handler -> handler.channel.close());
                    }
                } catch (InterruptedException e)     {
                    throw new RuntimeException(e);
                }
            });
            t.setDaemon(true);
            t.start();
        }
        try (Scanner scanner = new Scanner(System.in)) {
            LOOP:
            while (true) {
                switch (scanner.nextLine().trim()) {
                    case "reload": {
                        System.out.println("Reloading handlers...");
                        setHandlers();
                    }
                    break;
                    case "kick": {
                        CHANNELS.forEach(Channel::close);
                    }
                    break;
                    default: {
                        System.out.println("Exiting...");
                        break LOOP;
                    }
                }
            }
        }
        channel.close().syncUninterruptibly();
        group.shutdownGracefully();
    }

    protected static void setHandlers() {
        Arrays.fill(HANDLERS, null);

        HANDLERS[127] = (ch, buf) -> {
            System.out.printf("Closing connection %s...\n", ch.remoteAddress());
            ch.close();
        };
        HANDLERS[1] = (ch, buf) -> {
            System.out.printf("Received printable message from %s:\n%s\n", ch.remoteAddress(), buf.toString(Charset.forName("ASCII")));
        };
        HANDLERS[2] = (ch, buf) -> {
            System.out.printf("Sending some placeholder data back to %s...\n", ch.remoteAddress());
            send(ch, 1, "Hello client!".getBytes(Charset.forName("ASCII")));
        };
        HANDLERS[3] = (IOBiConsumer<Channel, ByteBuf>) (ch, buf) -> {
            System.out.printf("Sending an image to %s!\n", ch.remoteAddress());
            //BufferedImage img = ImageIO.read(new File("/media/daporkchop/TooMuchStuff/PortableIDE/Web/www.daporkchop.net/toembed/profilepic-128p.png"));
            File[] roms = new File(/*"/media/daporkchop/TooMuchStuff/Misc/ds"*/ "/home/daporkchop/192.168.1.119/Torrents/ROMs").listFiles((file, name) -> name.endsWith(".nds"));
            BufferedImage img;
            try (RomNDS rom = new RomNDS(roms[buf.readIntLE() % roms.length]))    {
                img = rom.getHeaders().getIconTitle().getIcon().getAsBufferedImage();
            }
            send(ch, 3, out -> {
                out.writeIntLE(img.getWidth()).writeIntLE(img.getHeight());
                for (int x = img.getWidth() - 1; x >= 0; x--)   {
                    for (int y = img.getHeight() - 1; y >= 0; y--)  {
                        int col = img.getRGB(x, y);
                        int a = (col >>> 24) & 0xFF;
                        if (a != 255)   {
                            out.writeShortLE(0);
                            continue;
                        }
                        int r = (col >>> 16) & 0xFF;
                        int g = (col >>> 8) & 0xFF;
                        int b = col & 0xFF;
                        out.writeShortLE(0x8000 | ((b >>> 3) << 10) | ((g >>> 3) << 5) | (r >>> 3));
                    }
                }
            });
        };
    }

    public static void send(@NonNull Channel channel, int id, @NonNull byte[] data) {
        channel.writeAndFlush(
                channel.alloc().ioBuffer()
                        .writeByte(id)
                        .writeIntLE(data.length)
                        .writeBytes(data).writeByte(0)
        );
    }

    public static void send(@NonNull Channel channel, int id, @NonNull Consumer<ByteBuf> dataWriter) {
        ByteBuf buf = channel.alloc().ioBuffer();
        buf.writeByte(id).writeIntLE(-1);
        int start = buf.writerIndex();
        dataWriter.accept(buf);
        buf.writeByte(0);
        buf.setIntLE(1, buf.writerIndex() - start - 1);
        System.out.printf("%d bytes\n", buf.getIntLE(1));
        channel.writeAndFlush(buf);
    }

    @RequiredArgsConstructor
    protected static class PHandler extends ChannelInboundHandlerAdapter {
        protected final File[] roms = new File("/media/daporkchop/TooMuchStuff/Misc/ds").listFiles((file, name) -> name.endsWith(".nds"));
        protected long lastActive = System.currentTimeMillis();

        @NonNull
        protected final Channel channel;

        @Override
        public void channelRegistered(ChannelHandlerContext ctx) throws Exception {
            System.out.printf("Incoming connection: %s\n", ctx.channel().remoteAddress());
            CHANNELS.add(ctx.channel());

            super.channelRegistered(ctx);
        }

        @Override
        public void channelUnregistered(ChannelHandlerContext ctx) throws Exception {
            System.out.printf("Connection %s closed!\n", ctx.channel().remoteAddress());
            CHANNELS.remove(ctx.channel());
            super.channelUnregistered(ctx);
        }

        @Override
        public void channelInactive(ChannelHandlerContext ctx) throws Exception {
            //System.out.printf("Closing channel %s for inactivity...\n", ctx.channel().remoteAddress());
            //ctx.close();
            super.channelInactive(ctx);
        }

        @Override
        public void channelRead(ChannelHandlerContext ctx, Object o) throws Exception {
            ByteBuf msg = (ByteBuf) o;

            while (msg.isReadable()) {
                System.out.println(msg);
                int id = msg.readByte() & 0xFF;
                if (id == 0)    {
                    continue;
                }
                BiConsumer<Channel, ByteBuf> handler = HANDLERS[id];
                if (handler == null) {
                    System.err.printf("Received unknown packet id %d from %s!\n", id, ctx.channel().remoteAddress());
                    ctx.close();
                    return;
                } else {
                    int len = msg.readIntLE();
                    handler.accept(ctx.channel(), msg.slice(msg.readerIndex(), len));
                    msg.skipBytes(len + 1);
                }
            }

            this.lastActive = System.currentTimeMillis();
            super.channelRead(ctx, msg);
        }
    }
}
