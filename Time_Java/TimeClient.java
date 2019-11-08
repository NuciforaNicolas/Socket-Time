import java.net.*;
import java.io.*;

public class TimeClient{
    private static final int PORT = 9999;

    public static void main(String[] args) throws IOException{
        Socket remoteHost = new Socket(InetAddress.getByName("127.0.0.1"), PORT);
        BufferedReader in = null;
        BufferedReader input = null;
        PrintWriter out = null;
        String msg = null;

        try{
            in = new BufferedReader(new InputStreamReader(remoteHost.getInputStream()));
            input = new BufferedReader(new InputStreamReader(System.in));
            out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(remoteHost.getOutputStream())), true);
            System.out.println("---Connection established with server---\nSend TIME to obtain current time [CTRL+C to quit]");

            while((msg = input.readLine()).length() > 0 ){
                out.println(msg);
                System.out.println("---Message sent---");
                String answer = in.readLine();
                System.out.println(answer);
            }
        }catch(IOException e){
            e.printStackTrace();
            System.exit(-1);
        }
        remoteHost.close();
    }
}