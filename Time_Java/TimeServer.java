import java.net.*;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.io.*;

public class TimeServer{
    private static final int PORT = 9999;
    private static List<InetAddress> ipClients = new ArrayList<InetAddress>();

    public static void main(String[] args) throws IOException{
        ServerSocket server = new ServerSocket(PORT);
        Socket client = null;
        BufferedReader in = null;
        PrintWriter out = null;
        String msg = null;

        System.out.println("---Server Ready---");

        try{
            while(true){
                client = server.accept();
                in = new BufferedReader(new InputStreamReader(client.getInputStream()));
                out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(client.getOutputStream())), true);

                ipClients.add(client.getLocalAddress());
                System.out.format("Connection established: [%s, %d]\n", client.getLocalAddress(), client.getLocalPort());

                while(((msg = in.readLine()).length() > 0)){
                    if(ipClients.contains(client.getLocalAddress())){
                        System.out.format("Received message from [%s]: %s\n", client.getLocalAddress(), msg);
                        if(msg.equals("TIME")){
                            out.println(System.currentTimeMillis());
                        }
                        else{
                            out.println("Wrong message");
                        }
                        System.out.println("---Message sent---");
                        ipClients.remove(client.getLocalAddress());
                    }
                    else{
                        out.println("BANNED");
                    }
                }

                out.close();
                in.close();
                client.close();
            }
        }catch(IOException e){
            System.out.println("Connection closed by client");
            System.exit(-1);
        }
        server.close();
    }
}