package net.daporkchop.dsstore;

import com.sun.net.httpserver.HttpServer;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.util.Scanner;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * @author DaPorkchop_
 */
public class Main {
    public static final AtomicBoolean running = new AtomicBoolean(true);
    private static final char[] HEX = "0123456789abcdef".toCharArray();
    private static final File ROOT_FOLDER = new File(".", "games");
    private static final byte[] ERROR_404 = "404 not found".getBytes();

    static {
        if (!ROOT_FOLDER.exists()) {
            ROOT_FOLDER.mkdirs();
        }
    }

    public static void main(String... args) throws IOException {
        HttpServer server = HttpServer.create(new InetSocketAddress(8236), 4);
        server.createContext("/", exchange -> {
            exchange.getResponseHeaders().set("Content-Type", "text/html; charset=windows-1252");
            byte[] bytes = null;
            InputStream data = null;
            int len = -1;
            final String path = exchange.getRequestURI().getPath();
            System.out.println(path);
            if (path.startsWith("/updates")) {
                char[] text = path.replaceFirst("/updates=", "").toCharArray();
                int index = 0;
                while (index < text.length) {
                }
            }
            switch (path) {
                case "/index.txt":
                    ByteArrayOutputStream baos = new ByteArrayOutputStream();
                    for (File file : ROOT_FOLDER.listFiles()) {
                        byte[] name = file.getName().getBytes();
                        if (name.length >= 256) {
                            throw new IllegalStateException("Name longer than 256 bytes: " + file.getName());
                        }
                        baos.write(name.length & 0xFF);
                        baos.write(name);
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
                //os.write(len & 0xFF);
                //os.write((len >> 8) & 0xFF);
                //os.write((len >> 16) & 0xFF);
                //os.write((len >> 24) & 0xFF);
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
        server.start();

        new Thread(() -> {
            Scanner s = new Scanner(System.in);
            s.nextLine();
            running.set(false);
            s.close();
            server.stop(0);
        }).start();
        /*ServerSocket socket = new ServerSocket(8236);
        Socket connection = socket.accept();
        connection.getOutputStream().write("hello world".getBytes());
        connection.getOutputStream().close();
        connection.close();
        socket.close();*/
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
}
