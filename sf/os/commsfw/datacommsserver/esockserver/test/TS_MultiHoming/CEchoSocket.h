// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// This is the header file for the CEchoSocket class, used in
// the TS_MultiHoming test suite
//
//

/**
 @file
*/

#if (!defined __CECHOSOCKET_H__)
#define __CECHOSOCKET_H__

#include "TS_MultiHomingStep.h"






// String Literals
_LIT(KErrConnectFailed, "Could not connect/bind %S:%d, error %i");
_LIT(KErrSendFailed, "Send of packet failed, %S:%d, error %i");
_LIT(KErrRecvFailed, "Receive error on %S:%d, error %S");
_LIT(KErrPacketsDiff, "Packet received did not match packet sent on %S:%d");
_LIT(KTxtCEchoSocket, "CEchoSocket");
_LIT(KTxtWhatHappened, "State Error: CEchoSocket");
_LIT(KSockDetails, "%S:%d, Destination: %S:%i, %i Packets of %i bytes");


enum EEchSockState
/**
 * Enum for internal state of the socket
 */
	{
	EInitialised,		//< Socket initialised
	EReady,				//< Socket bound (and connected for TCP)
	ESentData,			//< SendData() was called last
	ERecvdData,			//< RecvData() was last called
	ERecvFail,			//< RecvData() did not read all that was expected
	EComplete			//< All data has been sent on the socket, results available
	};

class CSafety;
class CEchoSocket : public CActive
	{
	public:
		static CEchoSocket* NewL();
		virtual ~CEchoSocket();

		// From CActive
		void RunL();
		void DoCancel();


		// Request an echo socket implicit connections
        virtual TInt Echo(const TInetAddr& aDest,   const TInetAddr& aSrc,
					      const TInt& aSize,		const TInt& aPackets,
					      const TUint& aProto,		const TInt& aTol,
					      TConnDetails *aConn,      const TInt& aSockNum,
					      const TBool aIsListener,  CTS_MultiHomingStep* aStep);

		void LogTimeout( );
		TInt LogReport(TBool& aPassed, TInt& aNumSent, TInt& aUdpRate);	//< Returns the socket's outcome
		void SafetyCall();		//< Allows the safety timer to cancel socket operations

		TInt iSockErr;
		enum { SAFETY_TIMEOUT = 10000000,    //< 10 second safety timer
			  MAX_CONSEC_TIMEOUT = 5   };	//< no more than 5 consecutive timeouts (ie 50 seconds)
											//< Based on 30 second RIP updates, expect 3 udp packets to be lost
									//<
protected:
		CEchoSocket();
		void ConstructL();
		void AcceptUdp();				//< Accepts Udp traffic
		void ConnectTcp();				//< Performs a TCP Connect
		void BindUdp();					//< Binds a UDP Socket
		void SendData();				//< Sends a packet
		virtual void RecvData();		//< Receives a packet
		void SetComplete();				//< Closes the socket.
		void RecvDataTcpListen();		//< handles client data
		void SendDataToClient();		//< Echos data back to client
		RSocket		iSocket;			//< The actual socket

		TConnDetails *iTConnection;  //< The connection that has been started
		TInetAddr	iSrcAddr;			//< Source IP address optional
		TInetAddr	iDestAddr;			//< Destination IP address
		TInt		iPacketSize;		//< Packet size in Bytes
		TInt		iPackets;			//< Packets to send
		TInt		iPacketsToSend;		//< Packets still to send
		TInt		iRecvdGood;			//< Number of Packets received
		TUint		iProtocol;			//< Protocol used TCP/UDP
		TInt		iUDPTolerance;		//< Percentage allowed to drop
		EEchSockState iState;			//< Current state of the socket
		CSafety*	iSafety;			//< The Safety timer AO
		HBufC8*		iWritebuf;			//< Output data buffer
		HBufC8*		iReadbufThisRead;   //< Input data buffer for this read
		HBufC8*		iReadbuf;			//< Input data buffer
		TPtr8		iPtrRecvd;			//< Read buffer pointer total received
		TPtr8		iPtrRecvdThisRead;  //< Read buffer pointer for each recv call
		TPtr8		iPtrWritn;			//< Read buffer pointer

		CTS_MultiHomingStep* iOwnerStep;	//< Pointer to owner step, logging use.
		TName		iName;				//< Socket Name, for logging reference
		TInt		iConsecRecvTimeOuts;//< number of consecutive read timeouts
		TInt        iSockNum;           //< Socket number as assigned by the test suite
		TBool       iIsListener;		//< Is a listener
		RSocket	    iAcceptedConnection;
	};



class CSafety : public CActive
/**
 * Class to implement a safety timeout for the CEchoSocket class
 */
	{
	public:
		static CSafety* NewL(CEchoSocket* aOwner);
		virtual ~CSafety();

		// From CActive
		void RunL();
		void DoCancel();

		// Set the timer
		void SetSafety();

	private:
		CSafety();
		void ConstructL(CEchoSocket* aOwner);

		CEchoSocket*					myOwner;//< The owner AO for this option
		RTimer							iSafety;//< Safety Timer for socket operations
		TTimeIntervalMicroSeconds32		iTime;	//< Timeout in microseconds
	};

// Used for the incomming connection tests for multiple connections
// This class will do only one side of the connection, rather than managing the whole thing.
NONSHARABLE_CLASS(CSplitEchoSocket) : public CEchoSocket
	{
	public:
		static CSplitEchoSocket* NewL();
		 ~CSplitEchoSocket();

		// Override the behaviour...
		TInt Echo(const TInetAddr& aDest,       const TInetAddr& aSrc,
							const TInt& aSize,			const TInt& aPackets,
							const TUint& aProto,		const TInt& aTol,
							TConnDetails *aConn,        const TInt& aSockNum,
							const TBool aIsListener,    CTS_MultiHomingStep* aStep);
		void RunL();

	private:
		CSplitEchoSocket();

		// Override the behaviour...
		void RecvData();				//< Receives a packet
	};

#endif //__CECHOSOCKET_H__

