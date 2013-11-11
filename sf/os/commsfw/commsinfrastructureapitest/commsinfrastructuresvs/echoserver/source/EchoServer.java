/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


import java.net.*;
import java.io.*;

public class EchoServer extends Thread{

	// Constants
	
	// Commands available
	private final String		iCommandShutdown = "Shutdown.";
	private final String		iCommandShutdownNow = "ShutdownNow.";
	private final String		iCommandStopReturning = "StopReturning.";
	private final String		iCommandStartReturning = "StartReturning.";
	private final String		iCommandSleep = "sleep=";
	private final String		iCommandSendBufferSize = "sendbufsize=";
	private final String		iCommandReceiveBufferSize = "recvbufsize=";

	private final int			iBufferSize = 256;

	// Static variables
	private static final int	sDefaultTcpPort = 10007;
	private static int			sPort;
	
	// Member variables
	private Socket				iClientSocket;
	private boolean				iUdpSocket;
	private int					iUdpPort;
	
	private String				iClientHost;
	private int					iClientPort;
	
	private int					iSendBufferLength = 0;
	private int					iReceiveBufferLength = 0;
	
	private boolean				iEchoData = true;
	private boolean				iPortSet = false;

	public static void main(String[] aArgs) throws IOException{
	
		ServerSocket lServerSocket = null;
		sPort = sDefaultTcpPort;
		boolean lUdpSocket = false;
		if (aArgs.length > 0){
			try{
				sPort = Integer.parseInt(aArgs[0]);
			}
    		catch(NumberFormatException nfe){
 				System.out.println("Usage:\n\tjava EchoServer [port [protocol]]\n\tIf port is not specified, port: " + sDefaultTcpPort + " will be used\n\tProtocol is TCP or UDP if not specified or incorrect TCP is default");		
 				System.exit(1);
    		}
    		if (aArgs.length > 1){
    			if (aArgs[1].equalsIgnoreCase("UDP")){
					lUdpSocket = true;
				}
    		}
		}
		// UDP Version
		if (lUdpSocket){
			System.out.println("Connection Socket Created for UDP connection on port: " + sPort);
			while (true){
				try{
					java.lang.Thread.sleep(1000);
				}
				catch(java.lang.InterruptedException ie){
					System.err.println("Sleep inturrupted.");
				}
				System.out.println("Waiting for Connection");
				EchoServer lEchoServer = new EchoServer(sPort);
				lEchoServer.handleUDP();
			}
		}

		// TCP version
		else{
		
			try{
				lServerSocket = new ServerSocket(sPort);
				System.out.println("Connection Socket Created for TCP connection on port: " + sPort);

				try{
					while (true){
						System.out.println("Waiting for Connection");
						new EchoServer(lServerSocket.accept());
					}
				}
				catch(IOException e){
					System.err.println("Accept failed.");
					System.exit(1);
				}
			}
			catch(IOException e){
				System.err.println("Could not listen on port: " + sPort + ".");
				System.exit(1);
			}
			finally{
				try{
					lServerSocket.close();
				}
				catch(IOException e){
					System.err.println("Could not close port: " + sPort + ".");
					System.exit(1);
				}
			}
		}
	}

	private EchoServer(Socket aClientSoc){
	
		iClientSocket = aClientSoc;
		iUdpSocket = false;
		
		InetAddress lIpAddress = iClientSocket.getInetAddress();
		iClientHost = lIpAddress.getHostAddress();
		System.out.println("Accepted from IP Address: " + iClientHost);

		start();
	}
	
	private EchoServer(int aPort){
	
		iUdpSocket = true;
		iUdpPort = aPort;
	}
	
	// returns true if iClientPort successfully parsed
	private boolean parsePort(String aMessage){
	
		// If the text is formatted like this:  
		// [2024]
		// parse the client port so that connection back to this preference is possible
		int lFirstIndex = aMessage.indexOf('[');
		int lSecondIndex = aMessage.indexOf(']');
		
		if ( (lFirstIndex == 0) && (lSecondIndex > 1) ){
		
			String lPortString = aMessage.substring(1, lSecondIndex);
			int lPort = 0;
						
			try{
				lPort = Integer.parseInt(lPortString);
			}
    		catch(NumberFormatException nfe){
 				System.out.println("Failed to parse port to integer");
 				return false;
    		}
    		
    		iClientPort = lPort;
    		iPortSet = true;
    		return true;
		}
    	return false;
	}
	
	// returns true if integer successfully parsed
	private int parseIntCommand(String aMessage, String aCommandName){
	
		// If the text is formatted like this:  
		// command=x
		// parse the integer x
		int lResult = -1;
		
		if (aMessage.startsWith(aCommandName)){
		
			int lDotIndex = aMessage.indexOf('.');
			
			if (lDotIndex < 0){
				lDotIndex = aMessage.length();
			}
			
			String lResultString = aMessage.substring(aCommandName.length(), lDotIndex);
						
			try{
				lResult = Integer.parseInt(lResultString);
			}
    		catch(NumberFormatException nfe){
 				System.out.println("Failed to parse integer");
    		}
		}
		return lResult;
	}

	// TCP Handling	
	public void run(){
	
		System.out.println("New TCP Communication Thread Started");
		try{
		
			PrintWriter lOut = null;
			Socket lClientSocketConn = null;
			BufferedReader lIn = new BufferedReader(new InputStreamReader(iClientSocket.getInputStream()));
			String lInputLine;
			boolean lExit = false;
			int lTempResult = -1;

			while (lExit == false){
			
				char lDataReceived[] = new char[iBufferSize];
				
				int lLengthOfData = lIn.read(lDataReceived);

				if (lLengthOfData <= 0){
					lExit = true;
					continue;
				}
				
				lInputLine = new String(lDataReceived, 0, lLengthOfData);
				
				System.out.println("Server: " + lInputLine);

				lTempResult = parseIntCommand(lInputLine, iCommandReceiveBufferSize);
				if (lTempResult != -1){		// Message parsed
					if (lTempResult > 0){
						iReceiveBufferLength = lTempResult;
						iClientSocket.setReceiveBufferSize(iReceiveBufferLength);
						System.out.println("RECEIVE BUFFER LENGTH SET TO " + iClientSocket.getReceiveBufferSize());
					}
					else{
						System.out.println("INVALID RECEIVE BUFFER LENGTH: " + iReceiveBufferLength);
					}
				}
				
				lTempResult = parseIntCommand(lInputLine, iCommandSendBufferSize);
				if (lTempResult != -1){		// Message parsed
					if (lTempResult > 0){
						iSendBufferLength = lTempResult;
					}
					else{
						System.out.println("INVALID SEND BUFFER LENGTH: " + lTempResult);
					}
				}
				if (iSendBufferLength > 0){
					lClientSocketConn.setSendBufferSize(iSendBufferLength);
					System.out.println("SEND BUFFER LENGTH SET TO " + lClientSocketConn.getSendBufferSize());
				}
				
				if (parsePort(lInputLine)){
					try{
						lClientSocketConn = new Socket(iClientHost, iClientPort);
        				lOut = new PrintWriter(lClientSocketConn.getOutputStream(), true);
					}
					catch(UnknownHostException e){
						System.err.println("Don't know about host: " + iClientHost);
						lExit = true;
						continue;
					}
					catch(IOException e){
						System.err.println("Couldn't get I/O for the connection to: " + iClientHost);
						lExit = true;
						continue;
    				}
				}
				
				if (lInputLine.startsWith(iCommandStartReturning)){
					System.out.println("ECHOING ACTIVATED");
					iEchoData = true;
				}
								
				if (lInputLine.startsWith(iCommandShutdownNow)){
					System.out.println("SHUTTING DOWN");
					break;					
				}
				
				if ( (lOut != null) && iEchoData ){
					System.out.println("Echo to host: " + iClientHost + ":" + iClientPort);
					lOut.print(lInputLine);
					lOut.flush();
				}

				if (lInputLine.startsWith(iCommandShutdown)){
					System.out.println("SHUTTING DOWN");
					break;					
				}
				
				if (lInputLine.startsWith(iCommandStopReturning)){
					System.out.println("ECHOING DEACTIVATED");
					iEchoData = false;
				}
				
				int lSleepLength = parseIntCommand(lInputLine, iCommandSleep);
				if (lSleepLength != -1){		// Message parsed
					if (lSleepLength > 0){
						System.out.println("SLEEPING FOR " + lSleepLength + " MILLISECONDS");
						try{
							java.lang.Thread.sleep(lSleepLength);
						}
						catch(java.lang.InterruptedException ie){
							System.err.println("Sleep inturrupted.");
						}
					}
					else{
						System.out.println("INVALID SLEEP LENGTH: " + lSleepLength + " MILLISECONDS");
					}
				}
			}
			
			if (lOut != null){
				lOut.close();
				lClientSocketConn.close();
			}
			
			lIn.close();
			iClientSocket.close();
			System.out.println("Communication Thread Ended");	
		}
		catch(IOException e){
			System.err.println("Problem with Communication Server");
		}
	}
	
	// UDP Handling
	public void handleUDP(){
	
		DatagramSocket lSock;
		try{
			lSock = new DatagramSocket(iUdpPort);
		}
		catch(SocketException e){
			System.out.println(e);
			return;
		}
		int lTempResult = -1;
		// echo back everything to new socket
		while (true){
		
			try{
				DatagramPacket lPack = new DatagramPacket(new byte[iBufferSize], iBufferSize);

				lSock.receive(lPack);
				
				// Set IP address of remote socket
				InetAddress lIpAddress = lPack.getAddress();
				iClientHost = lIpAddress.getHostAddress();
				System.out.println("Received from IP Address: " + iClientHost);
				
				int lLengthOfData = lPack.getLength();

				if (lLengthOfData <= 0){
					continue;
				}
				
				byte[] lData = lPack.getData();
				String lMessage = new String(lData, 0, lLengthOfData);
				System.out.println("Server: " + lMessage);

				lTempResult = parseIntCommand(lMessage, iCommandReceiveBufferSize);
				if (lTempResult != -1){		// Message parsed
					if (lTempResult > 0){
						iReceiveBufferLength = lTempResult;
						lSock.setReceiveBufferSize(iReceiveBufferLength);
						System.out.println("RECEIVE BUFFER LENGTH SET TO " + lSock.getReceiveBufferSize());
					}
					else{
						System.out.println("INVALID RECEIVE BUFFER LENGTH: " + iReceiveBufferLength);
					}
				}
				
				lTempResult = parseIntCommand(lMessage, iCommandSendBufferSize);
				if (lTempResult != -1){		// Message parsed
					if (lTempResult > 0){
						iSendBufferLength = lTempResult;
					}
					else{
						System.out.println("INVALID SEND BUFFER LENGTH: " + lTempResult);
					}
				}
				
				parsePort(lMessage);
				
				if (lMessage.startsWith(iCommandStartReturning)){
					System.out.println("ECHOING ACTIVATED");
					iEchoData = true;
				}
				
				if (lMessage.startsWith(iCommandShutdownNow)){
					System.out.println("SHUTTING DOWN");
					lSock.close();
					return;					
				}			

				if (iPortSet && iEchoData){
					sendUDP(lMessage);					
				}
				
				if (lMessage.startsWith(iCommandShutdown)){
					System.out.println("SHUTTING DOWN");
					lSock.close();
					return;					
				}			

				if (lMessage.startsWith(iCommandStopReturning)){
					System.out.println("ECHOING DEACTIVATED");
					iEchoData = false;
				}
				
				int lSleepLength = parseIntCommand(lMessage, iCommandSleep);
				if (lSleepLength != -1){		// Message parsed
					if (lSleepLength > 0){
						System.out.println("SLEEPING FOR " + lSleepLength + " MILLISECONDS");
						try{
							java.lang.Thread.sleep(lSleepLength);
						}
						catch(java.lang.InterruptedException ie){
							System.err.println("Sleep inturrupted.");
						}
					}
					else{
						System.out.println("INVALID SLEEP LENGTH: " + lSleepLength + " MILLISECONDS");
					}
				}
			}
			catch(IOException ioe){
				System.out.println(ioe);
				try{
					lSock.close();
				}
				catch(Exception e){
					System.err.println(e);
				}
				return;
			}
		}
	}
	
	private void sendUDP(String aMessage){
	
		try{
			byte[] lMessage = aMessage.getBytes();
			System.out.println("Echo to host: " + iClientHost + ":" + iClientPort);

			InetAddress lAddress = InetAddress.getByName(iClientHost);
			// Initialize a datagram packet with data and address
			DatagramPacket lPacket = new DatagramPacket(lMessage, lMessage.length, lAddress, iClientPort);

			// Create a datagram socket, send the packet through it, close it.
			DatagramSocket lDSocket = new DatagramSocket();
			
			if (iSendBufferLength > 0){
				lDSocket.setSendBufferSize(iSendBufferLength);
				System.out.println("SEND BUFFER LENGTH SET TO " + lDSocket.getSendBufferSize());
			}

			lDSocket.send(lPacket);
			lDSocket.close();
		}
		catch(Exception e){
			System.err.println(e);
		}		
	}
}
