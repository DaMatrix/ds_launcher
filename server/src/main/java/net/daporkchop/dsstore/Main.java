package net.daporkchop.dsstore;

import com.sun.net.httpserver.HttpServer;
import sun.misc.IOUtils;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.util.Scanner;
import java.util.concurrent.Executors;
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
            if (path.startsWith("/download")) {
                File file = new File(ROOT_FOLDER, path.replaceFirst("/download/", ""));
                System.out.println(file.getAbsolutePath() + ' ' + file.exists());
                len = (int) file.length();
                data = new FileInputStream(file);
            }
            /*if (path.startsWith("/updates")) {
                String[] names = path.replaceFirst("/updates=", "").split("&");
                List<String> toUpdate = new ArrayList<>();
                for (String s : names)   {
                    String[] split = s.split(":");
                    int version = decodeHex(split[1].toCharArray(), 0);
                    String name = split[0];
                    int localVersion = getVersion(name);
                    if (localVersion > version) {
                        toUpdate.add(name);
                    }
                }
                exchange.sendResponseHeaders(200, 1);
                OutputStream os = exchange.getResponseBody();
                os.write(encodeHex(toUpdate.size()));
                for (String name : toUpdate)    {
                    os.write(encodeHex(getVersion(name)));
                    {
                        byte[] b = name.getBytes();
                        os.write(b.length);
                        os.write(b);
                    }
                    File file = new File(ROOT_FOLDER, name + ".nds");
                    os.write(encodeHex((int) file.length()));
                    FileInputStream fis = new FileInputStream(file);
                    byte[] buf = new byte[256];
                    while (true) {
                        int read = fis.read(buf);
                        if (read == -1) {
                            break;
                        }
                        os.write(buf, 0, read);
                    }
                    fis.close();
                }
                os.flush();
                os.close();
                exchange.close();
                return;
            }*/
            switch (path) {
                case "/index.txt":
                    ByteArrayOutputStream baos = new ByteArrayOutputStream();
                    baos.write(encodeHex(ROOT_FOLDER.listFiles().length));
                    for (File file : ROOT_FOLDER.listFiles()) {
                        byte[] name = file.getName()/*.replaceAll(".nds", "")*/.getBytes();
                        baos.write(encodeHex(name.length));
                        baos.write(name);
                        baos.write(encodeHex(1)); //version
                        //baos.write(encodeHex(file.getName().hashCode() & 0xFFFFFF)); //id
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
        server.start();

        new Thread(() -> {
            Scanner s = new Scanner(System.in);
            s.nextLine();
            running.set(false);
            s.close();
            server.stop(0);
        }).start();
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
        FileInputStream fis = new FileInputStream(new File(ROOT_FOLDER, name + ".version"));
        int localVersion = Integer.parseInt(new String(IOUtils.readFully(fis, -1, false)));
        fis.close();
        return localVersion;
    }
}
