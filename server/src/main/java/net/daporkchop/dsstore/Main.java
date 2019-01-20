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
                    } else if (!t.isAlive()) {
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

        new Thread(() -> {
            do {
                try {
                    ByteArrayOutputStream baos = new ByteArrayOutputStream();
                    File[] files = ROOT_FOLDER.listFiles();
                    String[] names = new String[files.length];
                    for (int i = 0; i < files.length; i++)  {
                        names[i] = files[i].getName();
                    }
                    Arrays.sort(names);
                    baos.write(encodeHex(files.length));
                    for (String s : names) {
                        byte[] name = s.getBytes();
                        baos.write(encodeHex(name.length));
                        baos.write(name);
                        baos.write(encodeHex(getVersion(s))); //version
                    }
                    FILE_LIST = baos.toByteArray();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                for (int i = 0; i < 60 && !server.isClosed(); i++) {
                    try {
                        Thread.sleep(1000L);
                    } catch (InterruptedException e) {
                    }
                }
            } while (!server.isClosed());
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
            return DEFAULT_VERSION;
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
                        bytes = FILE_LIST;
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
