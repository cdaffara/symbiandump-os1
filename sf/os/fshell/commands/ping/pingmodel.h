// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// pingmodel.h - icmp echo client engine
// This software has been implemented in the 6PACK
// project at the Mobile Networks Laboratory (MNW)
// http://www.research.nokia.com/projects/6pack/
//

#if !defined __PINGMODEL_H
#define __PINGMODEL_H

#include <es_sock.h>
#include <in_sock.h>

#include <nifmbuf.h>
#include "in_icmp.h"
#include <icmp6_hdr.h>

#include "const.h"

#ifdef IAPSETTING
#include <agentclient.h>
#endif

// constants

#define TIMESTAMP_SIZE  sizeof(TInt64)  //Timestamp Size

#define DEFDATALEN      64 - TIMESTAMP_SIZE     //default data length


#define SECOND 1000000  // One second is 1000000 us. (1 us. per tick)

//ICMPv6 codes not defined in .h's
#define KInet6ICMP_HopLimitExceeded		0
#define KInet6ICMP_FragReassExceeded	1

#define KInet6ICMP_NoRoute				0
#define KInet6ICMP_AdminProhibition		1
#define KInet6ICMP_NotNeighbour			2
#define KInet6ICMP_AddrUnreach			3
#define KInet6ICMP_PortUnreach			4

#define KInet6ICMP_ErrHdrField			0
#define KInet6ICMP_NextHdrUnknown		1
#define KInet6ICMP_OptionUnkown			2


#define MAX_PATTERN_LENGTH	16	//Must match the value of the pattern in the RSS file!!!
#ifdef IAPSETTING
#define MAX_IAP_LENGTH	5	//Must match the value of the pattern in the RSS file!!!
#endif
class CPingContainer;
class CPingSender;


const TUint KPingQuiet			= 0x01;
const TUint KPingVerbose		= 0x02;
const TUint KPingPackLimit		= 0x04;
const TUint KPingDebug			= 0x08;

//Contains engine's saved preferences
class TPreferences
{
public:
	TUint iFlags;
	//Contains:
		//Bit 0 TBool   iQuiet;                 //No packet info, just statistics
		//Bit 1 TBool   iVerbose;               //Verbose Output. All ICMP packets, not only Echo reply
		//Bit 2 TBool   iPackLimit;             //Indicates if unlimited packet number
		//Bit 3 TBool   iDebug;                 //Set the SO_DEBUG flag in the socket

	TUint   iSecWait;			//Time between sent packets (Default 1 second)
	TUint   iPacketDataSize;	//Default Data Size (not including ICMP header)
	TUint   iTotalPackets;		//Number of ICMP Echo Request packets to send
	TUint   iLastSecWait;		//Time to wait for the Last packet. Default 2
	TBuf<KHostNameLimit> iHostname;
	TBuf<MAX_PATTERN_LENGTH> iPattern;	//Pattern to fill the packet.Default 0xFF (Stored as text)

#ifdef IAPSETTING
	TUint iIAP;	
#endif
	//Not used yet
	//TUint   iHopLimit;              //Time-to-live in hops default 255 (0 means not set)
	//TBool   iNumericOutput; //Resolve adresses by default	
};

class CPingSender;
class CPingSingleSender;
//Ping Utility class. Contains All the ping model and create active objects to
//                                              send and receive from the socket
class CPing : public CActive
{
friend class CPingSender;
friend class CPingSingleSender;
public:
	CPing();
	~CPing();
	
	void ConstructL(TPreferences aPref);

	//return the current preferences
	void GetPreferences(TPreferences &aPref);
	static void DefaultPreferences(TPreferences &aPref);
	void SetConsole(CPingContainer *aConsole);
	TDes* GetHostName();
	void SetHostName(const TDesC& ahostname);
	
	void EndPingL();
	TBool IsRunning();
	void BeginL();

#ifdef IAPSETTING
	void StartConnection();
#endif

protected:
	
	TUint16 in_chksum(TUint16 *data, TUint len);
	void ComposeFirstICMPPacket();  //Compose a whole ICMPPacket
	void ComposeICMPPacket();               //Only composes timestamp and sequence number
	void StampPacket(); // Modify the composed packet with fresh timestamp and checksum
	void NextSeq();

	void CloseAll();        //Closes all the resources
	void PrintICMPData(const TDesC8& adata);

	void IssueRequest();
	void RunL();    //Segment of code to execute
	void DoCancel();        //Stops Execution of Ping

private:
	void CreateSocketAOL();
	void Statistics();
	void WriteLineIfNotQuiet(const TDesC& abuf);
	void ErrorL(const TDesC& string,TInt error);
	void CLR(TUint16 num);
	void SET(TUint16 num);
	TBool TEST(TUint16 num);
	TPtrC PacketType(ThdrICMP *hdr);
	//TPtrC PacketTypev6(TInet6HeaderICMP_Echo *aHdr);
	void PacketTypev6(TDes& buf,ThdrICMP *aHdr);
	void Stop();
	TUint16 RandomNumber();

private:
	
    CPingContainer *iConsole;
	RSocketServ iSockServ;  // Socket Server
	RSocket iSocket;                                // Socket
	RHostResolver iHostResolv;      //Resolver
	//TRequestStatus status;		// To wait for socket operations completion

	CPingSender *iPingSender;
	TUint32 iDup[8];                //Calculates duplicate packets up to 256 packets, after
						//it begins again using modulus (%, MOD)

	HBufC8 *iPacketData;
	TPtr8 iPacket;
	TUint16 iId;

	TUint iMaxTime;                         //Maximum time 
	TUint iMinTime;                         //Minimum time
	TUint iTimeSum;                         //all times sum
	TBuf<KHostNameLimit> iHostname;         // Contains hostname to ping
	
	TUint iDupPackets;						//Duplicated packets
	TBool iRunning;
	TUint iStage;		//Stages to use non-blocking resolver in RunL
	TNameEntry iEntry;	//contains address resolution data

	HBufC8 *iReceivedDataBuffer;
	TPtr8 iReceivedData;

	TUint iType;

	TInt64 iSeed;	//Used for random iId generation

public:
	TBool   iNumericOutput; //Resolve adresses by default
	TUint   iSecWait;               //Time between sent packets (Default 1 second)
	TUint   iPacketDataSize;//Default Data Size (not including ICMP header)
	TBool   iQuiet;                 //No packet info, just statistics
	TUint   iHopLimit;              //Time-to-live in hops default 255 (0 means not set)
	TBool   iVerbose;               //Verbose Output. All ICMP packets, not only Echo reply
	TBool   iPackLimit;             //Indicates if unlimited packet number
	TUint   iSentPackets;   //ICMP Echo Request Packets sent til now
	TUint   iRecvPackets;   //ICMP Echo Reply Packets received til now
	TUint   iTotalPackets;  //Number of ICMP Echo Request packets to send
	TBuf<MAX_PATTERN_LENGTH> iPattern;               //Pattern to fill the packet.Default 0xFF
	TBool   iDebug;                 //Set the SO_DEBUG flag in the socket
	TUint   iLastSecWait;   //Time to wait for the Last packet. Default 2
	TUint   iChksumErrors;
	TUint   iSockErrors;
	TInetAddr iHostAddr;		//Address to Ping
#ifdef IAPSETTING
	TUint iIAP;		
	RGenericAgent iGenericAgent;     // To establish the connection with the ISP.
#endif
};

// Used by CPing
// Send packets using a CTimer (CActive) object
// Not intended to run alone!!!

class CPingSender : public CTimer
{
public:
	//constructor
	CPingSender();

	//destructor
	~CPingSender();
	
	//second phase constructor
	void ConstructL(CPing *aPingModel);
	void FirstRunL();

protected:
	//Issues next RunL execution
	void IssueRequest();

	//Issues last RunL execution
	void IssueLastRequest();

	// will send all the packets
	void RunL();

	//Cancel Packet Sending
	void DoCancel();
	
	void SendFirstPacketL();
	void SendPacket();

private:
	CPing *iPingModel;
	CPingSingleSender *iSender;
	

};


// Used by CPingSender
// Sends packets. Cannot be done directly by CPingSender because there are conflicts with
//					diferent TRequestStatus.


class CPingSingleSender : public CActive
{
public:
	//constructor
	CPingSingleSender(CPing *aPingModel);

	//destructor
	~CPingSingleSender();
	
	void FirstRunLD();
	void NextPacket();

protected:
	//Issues next RunL execution
	void IssueRequest();

	//Issues last RunL execution
	void IssueLastRequest();

	// will send all the packets
	void RunL();

	//Cancel Packet Sending
	void DoCancel();


private:
	CPing *iPingModel;
	TUint iUnsent;

};

#endif
