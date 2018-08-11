package net.daporkchop.dsstore;

import net.daporkchop.lib.binary.stream.StreamUtil;
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
import java.util.Collections;
import java.util.HashSet;
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
    private static final byte[] ERROR_404 = "404 not found".getBytes();

    static {
        if (!ROOT_FOLDER.exists()) {
            ROOT_FOLDER.mkdirs();
        }
    }

    public static void main(String... args) throws IOException {
        ServerSocket server = new ServerSocket(8236);
        new Thread(() -> {
            Scanner s = new Scanner(System.in);
            s.nextLine();
            running.set(false);
            s.close();
            try {
                server.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }).start();

        final Set<WriteThread> threads = Collections.synchronizedSet(new HashSet<>());

        new Thread(() -> {
            Queue<WriteThread> toRemove = new ConcurrentLinkedQueue<>();
            while (!server.isClosed()) {
                long minTime = System.currentTimeMillis() - 20000L;
                threads.forEach(t -> {
                    if (t.lastWrite < minTime) {
                        try {
                            t.socket.close();
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                        t.interrupt();
                        toRemove.add(t);
                    } else if (!t.isAlive())    {
                        toRemove.add(t);
                    }
                });
                toRemove.forEach(threads::remove);
                toRemove.clear();
                try {
                    Thread.sleep(1000L);
                } catch (InterruptedException e) {
                }
            }
        }).start();

        while (!server.isClosed()) {
            try {
                Socket socket = server.accept();
                WriteThread thread = new WriteThread(socket);
                thread.start();
                threads.add(thread);
            } catch (SocketException e) {
                break;
            }
        }

        /*HttpServer server = HttpServer.create(new InetSocketAddress(8236), 4);
        server.createContext("/", exchange -> {
            exchange.getResponseHeaders().set("Content-Type", "text/html; charset=windows-1252");
            byte[] bytes = null;
            InputStream data = null;
            int len = -1;
            final String path = exchange.getRequestURI().getPath();
            System.out.println(path);
            if (path.startsWith("/download")) {
                File file = new File(ROOT_FOLDER, path.replaceFirst("/download/", "").replaceAll("~", " "));
                System.out.println(file.getAbsolutePath() + ' ' + file.exists());
                len = (int) file.length();
                data = new FileInputStream(file);
            }
            switch (path) {
                case "/index.txt":
                    ByteArrayOutputStream baos = new ByteArrayOutputStream();
                    baos.write(encodeHex(ROOT_FOLDER.listFiles().length));
                    for (File file : ROOT_FOLDER.listFiles()) {
                        byte[] name = file.getName().getBytes();
                        baos.write(encodeHex(name.length));
                        baos.write(name);
                        baos.write(encodeHex(getVersion(file.getName()))); //version
                        baos.write(encodeHex(file.getName().hashCode() & 0xFFFFFFF)); //id
                    }
                    bytes = baos.toByteArray();
                    break;
                case "/a":
                    bytes = "hello AAA".getBytes();
                    break;
                case "/b":
                    bytes = "hello BBB".getBytes();
                    break;
            }
            if (len == -1L && bytes != null) {
                len = bytes.length;
            }
            if (data == null && bytes != null) {
                data = new ByteArrayInputStream(bytes);
                len = bytes.length;
            }
            if (data == null) {
                data = new ByteArrayInputStream(ERROR_404);
                exchange.sendResponseHeaders(404, ERROR_404.length);
            } else {
                exchange.sendResponseHeaders(200, len + 9L); //add 9 bytes for start padding
            }
            OutputStream os = exchange.getResponseBody();
            {
                //start padding
                os.write(127);
                os.write(encodeHex(len));
            }
            byte[] buf = new byte[256];
            while (true) {
                int read = data.read(buf);
                if (read == -1) {
                    break;
                }
                os.write(buf, 0, read);
            }
            data.close();
            os.flush();
            os.close();
            exchange.close();
        });
        server.setExecutor(null);
        server.start();*/
    }

    public static byte[] encodeHex(int val) {
        char[] letters = new char[8];
        for (int i = 0; i < 8; i++) {
            letters[i] = HEX[(val >> (i << 2)) & 0xF];
        }
        return new String(letters).getBytes();
    }

    public static int decodeHex(char[] in, int fromIndex) {
        int val = 0;
        for (int i = 0; i < 8; i++) {
            char c = in[i + fromIndex];
            for (int j = 0; j < 16; j++) {
                if (HEX[j] == c) {
                    val |= j << (i << 2);
                    break;
                }
            }
        }
        return val;
    }

    public static int getVersion(String name) throws IOException {
        File file = new File(INFO_FOLDER, name + ".info");
        if (file.exists()) {
            FileInputStream fis = new FileInputStream(file);
            int localVersion = Integer.parseInt(new String(IOUtils.readFully(fis, -1, false)).trim());
            fis.close();
            return localVersion;
        } else {
            return 2;
        }
    }

    public static class WriteThread extends Thread {
        public final Socket socket;
        public volatile long lastWrite = System.currentTimeMillis();

        public WriteThread(Socket socket) {
            this.socket = socket;
        }

        @Override
        public void run() {
            try {
                byte[] bytes = null;
                InputStream data = null;
                int len = -1;
                final String path;
                {
                    byte[] hex = new byte[8];
                    InputStream is = socket.getInputStream();
                    StreamUtil.read(is, hex, 0, 8);
                    System.out.println(new String(hex) + " (" + decodeHex(new String(hex).toCharArray(), 0) + ')');
                    byte[] text = new byte[decodeHex(new String(hex).toCharArray(), 0)];
                    StreamUtil.read(is, text, 0, text.length);
                    path = new String(text);
                }
                System.out.println(path);
                if (!path.startsWith("/")) {
                    File file = new File(ROOT_FOLDER, path/*.replaceFirst("/download/", "")*/.replaceAll("~", " "));
                    System.out.println(file.getAbsolutePath() + ' ' + file.exists());
                    len = (int) file.length();
                    data = new FileInputStream(file);
                }
                switch (path) {
                    case "/index.txt":
                        ByteArrayOutputStream baos = new ByteArrayOutputStream();
                        baos.write(encodeHex(ROOT_FOLDER.listFiles().length));
                        for (File file : ROOT_FOLDER.listFiles()) {
                            byte[] name = file.getName().getBytes();
                            baos.write(encodeHex(name.length));
                            baos.write(name);
                            baos.write(encodeHex(getVersion(file.getName()))); //version
                            //baos.write(encodeHex(file.getName().hashCode() & 0xFFFFFFF)); //id
                        }
                        bytes = baos.toByteArray();
                        break;
                }
                if (len == -1L && bytes != null) {
                    len = bytes.length;
                }
                if (data == null && bytes != null) {
                    data = new ByteArrayInputStream(bytes);
                    len = bytes.length;
                }
                OutputStream os = socket.getOutputStream();
                {
                    //start padding
                    os.write(127);
                    os.write(encodeHex(len));
                }
                byte[] buf = new byte[256];
                while (true) {
                    int read = data.read(buf);
                    if (read == -1) {
                        break;
                    }
                    os.write(buf, 0, read);
                    lastWrite = System.currentTimeMillis();
                }
                data.close();
                os.flush();
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                try {
                    socket.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
