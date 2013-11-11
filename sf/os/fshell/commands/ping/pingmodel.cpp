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
// Accenture - minor mods to work as an fshell command and fixes to broken logic
//
// Description:
// pingmodel.cpp - icmp echo client engine
// This software has been implemented in the 6PACK
// project at the Mobile Networks Laboratory (MNW)
// http://www.research.nokia.com/projects/6pack/
// REMARKS:
// The application is prepared to admit the option Hop Limit and
// Numeric output (no name resolution), but these options are not used
// because the socket is open in ICMP mode.  To use them need an IP
// socket and IP packets to set the options. Some code is commented and
// can be used in case of need
//



#ifdef IAPSETTING
#include <commdb.h>
#endif

#include <e32math.h>
#include <e32std.h>
//#include <eikenv.h>
//#include <netdial.h>

// IPv6 changes CPing::iType, ComposeFirstIcmpPacket()- ICMPType, Socket Open()

#include "pingmodel.h"

//#include <pingapp.rsg> //resource is not needed as console app

#include "ping_misc.h"


CPing::CPing():CActive(EPriorityStandard),iPacket(0,0),iReceivedData(0,0)
{
	CActiveScheduler::Add(this);	//Adds itself to the scheduler only the first time
}



//Sets all default values. Actually it's no more a L function
void CPing::ConstructL(TPreferences aPref)
{

	// Base class second-phase construction.

	//iHostname=_L("bart.research.nokia.com");	//default initial addr
	//iHostname=_L("127.0.0.1");	//default initial addr
	//iHostname=_L("dead::beef");	//default initial addr
	iHostname=aPref.iHostname;
	iPackLimit = aPref.iFlags & KPingPackLimit;		//Unlimited packets
	iTotalPackets=aPref.iTotalPackets;	//Default packet number when limited number
	
	iSecWait=aPref.iSecWait;		//Default 1 second
	iPacketDataSize=aPref.iPacketDataSize;	//Default Size
	iQuiet=aPref.iFlags & KPingQuiet;	//No packet info, just statistics
		
	iVerbose=aPref.iFlags & KPingVerbose;	//Verbose Output. All ICMP packets, not only Echo reply. Default no
	iPattern.Copy(aPref.iPattern);			//Pattern 0xFF (1-filled byte)
	
	iDebug=aPref.iFlags & KPingDebug;		//Set SO_DEBUG flag
	iLastSecWait=aPref.iLastSecWait;	//Default time to wait for the last packet

#ifdef IAPSETTING
	iIAP = aPref.iIAP;		
#endif

	//Not used by now
	iNumericOutput=EFalse;	//Resolve adresses by default
	iHopLimit=0;			//Time-to-live in hops default 255 ( 1 to 255) (0 means not set)

	iDupPackets=0;			//Duplicated packets
	iRunning=EFalse;		// Tells if there's an Instance of Ping Running


	
/*
	iReceivedDataBuffer= HBufC8::NewL(iPacketDataSize + ICMP_ECHO_HEADER_SIZE);	//Maximum size of a return packet
	TPtr8 auxPtr(iReceivedDataBuffer->Des());	//must be used here because the buffer changes
	iReceivedData.Set(auxPtr);			//we use an aux var because can't use Des() directly Why??
*/
}

//return the current preferences
void CPing::GetPreferences(TPreferences &aPref)
{	
	aPref.iFlags=0;
	if (iQuiet)
		aPref.iFlags|=KPingQuiet;
	if (iVerbose)
		aPref.iFlags|=KPingVerbose;
	if (iPackLimit)
		aPref.iFlags|=KPingPackLimit;
	if (iDebug)
		aPref.iFlags|=KPingDebug;

	aPref.iSecWait=iSecWait;               //Time between sent packets (Default 1 second)
	aPref.iPacketDataSize=iPacketDataSize;//Default Data Size (not including ICMP header)
	aPref.iTotalPackets=iTotalPackets;  //Number of ICMP Echo Request packets to send
	aPref.iLastSecWait=iLastSecWait;   //Time to wait for the Last packet. Default 2
	aPref.iHostname=iHostname;		//Address to Ping
	aPref.iPattern.Copy(iPattern);

#ifdef IAPSETTING	
	aPref.iIAP = iIAP;
#endif
		
}

void CPing::DefaultPreferences(TPreferences &aPref)
{	
	aPref.iFlags=KPingVerbose|KPingPackLimit;

	aPref.iSecWait=1;               //Time between sent packets (Default 1 second)
	aPref.iPacketDataSize=56;//Default Data Size (not including ICMP header)
	aPref.iTotalPackets=5;  //Number of ICMP Echo Request packets to send
	aPref.iLastSecWait=2;   //Time to wait for the Last packet. Default 2
	aPref.iPattern=_L("FF");
	aPref.iHostname=_L("127.0.0.1");
#ifdef IAPSETTING
	aPref.iIAP=1;
#endif
}

void CPing::Statistics()
{
	//TBuf<300> aux(_L("==========================================\n"));
	TBuf<300> aux(_L("\n"));
	
	TReal rLoss=iSentPackets-iRecvPackets-iChksumErrors;	// An error is a received but wrong packet
										//is not masked as received because of time calculations
	if (rLoss<0) 
		rLoss=0;
	TReal rSent=iSentPackets;
	TReal r;
	if (rSent>0)
		r=rLoss/rSent;
	else
		r=0;

	aux.AppendFormat(_L("Lost: %.1f%% Bad: %d"), r*100, iChksumErrors);

	//If there's a timestamp and data received
	if ((iRecvPackets>0) && (iPacketDataSize >= TIMESTAMP_SIZE))
	{
		r=(iTimeSum/iRecvPackets);	//average in ms
		aux.AppendFormat(_L(" Max: %d Min: %d Avg: "),iMaxTime,iMinTime);
		TRealFormat format;	//no decimals
		format.iType|=KDoNotUseTriads;	// no thousands separatorlast
		aux.AppendNum(r,format);
		aux.Append(_L(" ms"));
	}

	if (iDupPackets>0)
		aux.AppendFormat(_L(" Dup: %d"),iDupPackets);

	//aux.Append(_L("\n==========================================\n"));
	iConsole->WriteLine(aux);
}

// Use when data is not going to be displayed in Quiet mode
void CPing::WriteLineIfNotQuiet(const TDesC& abuf)
{
	if (!iQuiet)
		iConsole->WriteLine(abuf);
}

TDes* CPing::GetHostName()
{
	return &iHostname;
}

void CPing::SetHostName(const TDesC& ahostname)
{
	iHostname=ahostname;
}

void CPing::SetConsole(CPingContainer* aConsole)
{
	iConsole=aConsole;
}

CPing::~CPing()
	{
	Cancel();
	delete iPacketData;		// If something leaves.
	delete iReceivedDataBuffer;
	CloseAll();
	}

//Shows the error and set the application as not running. 
//Requires a return after calling it!
void CPing::ErrorL(const TDesC& string,TInt error)
{
	
	TBuf<150> aux;
	TBuf<100> errtxt;

	//CEikonEnv::Static()->GetErrorText( errtxt,error);
	aux.Format(string);	
	aux.Append(_L(": "));
	aux.Append(errtxt);
	aux.AppendFormat(_L(" (%d)\n"), error);
	WriteLineIfNotQuiet(aux);
	//iRunning=EFalse;

	iSockErrors++;

	if (error==KErrAbort)	//Critical Error
	{

/*		RNetDial xnetdial;
		TBool active;

		xnetdial.Open();
		TInt err=xnetdial.NetworkActive(active);
	
		if (!active)
		{
			xnetdial.Start();
			err=xnetdial.NetworkActive(active);
		}
*/
		EndPingL();
		User::Leave(0);	// NOT SURE IF IT'S THE BEST WAY!!!
	}

	if (iSockErrors>5)	//To avoid a chain of errors that blocks everything. Should never happen though 
	{
		EndPingL();
		User::Leave(0);	// NOT SURE IF IT'S THE BEST WAY!!!
	}
	
}


// checksum of the ICMP packet (ICMP header + data)
TUint16 CPing::in_chksum(TUint16* data, TUint len)
{
	
	TUint16 *d=data;
	TUint left=len;
	TUint32 sum=0;
	TUint16 aux=0;
	
	while (left > 1)
	{
		sum += *d++;
		left -= 2;		//because adding 16 bits numbers (2 bytes)
	}

	if (left==1)	//If odd length
	{
		*(TUint8*) &aux = *(TUint8*) d;
		sum += aux;
	}
	
	sum = (sum >> 16) + (sum & 0x0ffff);
	sum += (sum >> 16);
	aux = (TUint16) ~sum;

	return aux;
	
}

// Clears a bit from the iDup table

void CPing::CLR(TUint16 num)
{
	TInt num256 = num % 256;		//num modulus 256
	TInt pos = num256 >> 5;		//Position of vector iDup
	iDup[pos] &= ~((1 << num % 32 ));
}

// Sets a bit from the iDup table

void CPing::SET(TUint16 num)
{
	TInt num256= num % 256;		//num modulus 256
	TInt pos=num256 >> 5;		//Position of vector iDup
	iDup[pos] |= (1 << num % 32 );
}

// Tests if the bit is set or not

TBool CPing::TEST(TUint16 num)
{
	TUint num256= num % 256;		//num modulus 256
	TInt pos=num256 >> 5;		//Position of vector iDup
	return ((iDup[pos] & (1 << num % 32)) == (TUint32)(1 << num % 32));
}

//Generates a random number using iSeed
TUint16 CPing::RandomNumber()
{
	if (iSeed==0)	//Initialize seed randomly with time
	{
		TTime time;
        time.HomeTime();
		iSeed=time.Int64();
    }
    return ((TUint16)Math::Rand(iSeed));	//Just take the lowest 16 bits)
}

// Composes the whole ICMP packet except time stamp and checksum

void CPing::ComposeFirstICMPPacket()
{
	ThdrICMP *hdr;
	TUint firstPos = 0;
	
	hdr = (ThdrICMP *)&iPacket[0];	// Can use this one for IPv6 because is the same format

	if (iType == IPv4)
		hdr->NetSetType(KICMPTypeEchoRequest);
	else
		hdr->NetSetType(KInet6ICMP_EchoRequest);

	hdr->NetSetCode(KICMPCode);

	iId = RandomNumber();
	((ThdrICMP_Echo *)hdr)->NetSetId(iId);	
	((ThdrICMP_Echo *)hdr)->NetSetSeq(0);
	CLR(0);	// Clears the bit in the received/dup buffer
	
	// Jump over possible timestamp

	if (iPacketDataSize >= TIMESTAMP_SIZE)
	{
		firstPos = TIMESTAMP_SIZE;
	}

	// The rest is filled with a pattern
	// The last part of the packet may only be a part of the pattern

	TInt i;
	TInt j;

	//First transform the pattern from text to Hexadecimal
	TLex lex;
	//TUint8 hex_num;
	TBuf<2> hex_digit;
	TBuf<2 * MAX_PATTERN_LENGTH> text_pattern;
	TUint8 hex_pattern[MAX_PATTERN_LENGTH];
	if (iPattern.Length() % 2 == 0)
		text_pattern.Copy(iPattern);
	else	//odd size so the pattern is doubled and we have a even size
	{
		text_pattern.Copy(iPattern);
		text_pattern.Append(iPattern);
	}

	for (i = 0; i < text_pattern.Length(); i += 2)
	{
		hex_digit.Copy(text_pattern.Ptr() + i, 2);	//Copy 2 text digits 1 byte
		lex.Assign(hex_digit);
		lex.Val(hex_pattern[i/2], EHex);		//Can't fail because only 2 digits
		//hex_pattern.Append((TUint8 *)&hex_num, sizeof(TUint8));	//Append the hex digit (byte)
	}


	
	for (j = 0, i = firstPos; (TUint)i < iPacketDataSize; i++, j++)
	{
		iPacket[ICMP_ECHO_HEADER_SIZE + i] = !text_pattern.Length()
			? (TUint8)(i % 256)
			: hex_pattern[j % (text_pattern.Length()/2)];	//hex_pattern is half the size of text pattern
	}
//	TBuf8<150> prova;
//	prova.Copy(iPacket);
	StampPacket();
}

// Only composes timestamp and sequence number and calculates checksum

void CPing::ComposeICMPPacket()
{
	ThdrICMP *hdr = (ThdrICMP *)&iPacket[0];
	TUint16 seq = (TUint16)(((ThdrICMP_Echo *)hdr)->NetGetSeq() + 1);

	((ThdrICMP_Echo *)hdr)->NetSetSeq(seq);	// Next seq. Number
	CLR(seq);	// Clears the bit in the received/dup buffer

	StampPacket();
}

void CPing::StampPacket()
{
	ThdrICMP *hdr = (ThdrICMP *)&iPacket[0];

	if (iPacketDataSize >= TIMESTAMP_SIZE)
	{
		TTime time;
		time.UniversalTime();
		*(TInt64*)&iPacket[8] = time.Int64();	// Converts the time to a Int64 value
	}

	hdr->NetSetChecksum(0);
	hdr->NetSetChecksum(in_chksum((TUint16 *)&iPacket[0], iPacketDataSize + ICMP_ECHO_HEADER_SIZE));
}

//Puts the next seq number in the packet (iPacket)
void CPing::NextSeq()
{
	ThdrICMP_Echo *hdr;

	hdr=(ThdrICMP_Echo *)&iPacket[8];
	hdr->SetSeq((TUint16)((hdr->NetGetSeq()) + 1));
}

//TPtrC CPing::PacketTypev6(TInet6HeaderICMP_Echo *aHdr)
void CPing::PacketTypev6(TDes& buf,ThdrICMP *aHdr)
{
	//TBuf<40> buf;
	TInet6HeaderICMP *hdr = (TInet6HeaderICMP *)aHdr;
	//TInet6HeaderICMP_Echo *echoHdr;

	TInt8 code=hdr->Code();
	switch(hdr->Type())
	{
		//Errors 0-127
		case KInet6ICMP_Unreachable: 
			//buf.Format(_L("type= "));
			buf.Append(_L("Dest. Unreachable: "));
			//buf.Format(_L("type=%d "),type);
			switch (code)
			{
			case KInet6ICMP_NoRoute:
				//buf.Append(_L("code= "));
				buf.Append(_L("No Route "));
				break;
			case KInet6ICMP_AdminProhibition:
				//buf.Append(_L("code= "));
				buf.Append(_L("Admin. Prohibition "));
				break;
			case KInet6ICMP_NotNeighbour:
				//buf.Append(_L("code= "));
				buf.Append(_L("Not a Neighbour "));
				break;
			case KInet6ICMP_AddrUnreach:
				//buf.Append(_L("code= "));
				buf.Append(_L("Addr. Unreachable "));
				break;
			case KInet6ICMP_PortUnreach:
				//buf.Append(_L("code= "));
				buf.Append(_L("Port Unreachable "));
				break;
			default: buf.AppendFormat(_L("code=%d "),code);
			}
			break;
		case KInet6ICMP_PacketTooBig:
			//buf.Format(_L("type= "));
			buf.Append(_L("Pack. Too big "));
			break;
		case KInet6ICMP_TimeExceeded:
			//buf.Format(_L("type= "));
			buf.Append(_L("Time exceeded: "));
			switch (code)
			{
			case KInet6ICMP_HopLimitExceeded:
				//buf.Append(_L("code= "));
				buf.Append(_L("Hop Limit "));
				break;
			case KInet6ICMP_FragReassExceeded:
				//buf.Append(_L("code= "));
				buf.Append(_L("Frag. Reassembly "));
				break;
			default: buf.AppendFormat(_L("code=%d "),code);
			}
			break;
		case KInet6ICMP_ParameterProblem:
			//buf.Format(_L("type= "));
			buf.Append(_L("Parameter problem: "));
			switch (code)
			{
			case KInet6ICMP_ErrHdrField:
				//buf.Append(_L("code= "));
				buf.Append(_L("Bad header filed"));
				break;
			case KInet6ICMP_NextHdrUnknown:
				//buf.Append(_L("code= "));
				buf.Append(_L("Unknown Next Header "));
				break;
			case KInet6ICMP_OptionUnkown:
				//buf.Append(_L("code= "));
				buf.Append(_L("Unknown Option"));
				break;
			default: buf.AppendFormat(_L("code=%d "),code);
			}
			break;

		//Information 128-255
		case KInet6ICMP_EchoRequest:
			//echoHdr=(TInet6HeaderICMP_Echo *)hdr;
			buf.Append(_L("Echo Request "));
			//buf.AppendFormat(_L("id= %d "),echoHdr->Identifier());
			//buf.AppendFormat(_L("Seq= %d "),echoHdr->Sequence());
			break;
		case KInet6ICMP_EchoReply:
			//echoHdr=(TInet6HeaderICMP_Echo *)hdr;
			buf.Append(_L("Echo Reply "));
			//buf.AppendFormat(_L("id= %d "),echoHdr->Identifier());
			//buf.AppendFormat(_L("Seq= %d "),echoHdr->Sequence());
			break;
	
		case KInet6ICMP_Redirect:
 			buf.Append(_L("Redirect "));
			break;

		case KICMPTypeRouterAdvert: 
		case KInet6ICMP_RouterAdv:
			buf.Append(_L("Router advertisement "));
			break;

		case KICMPTypeRouterSolicit: 
		case KInet6ICMP_RouterSol:
			buf.Append(_L("Router solicitation "));
			break;

		case KInet6ICMP_GroupQuery:
			buf.Append(_L("KInet6ICMP_GroupQuery "));
			break;

		case KInet6ICMP_GroupReport:
			buf.Append(_L("KInet6ICMP_GroupReport "));
			break;

		case KInet6ICMP_GroupDone:
			buf.Append(_L("KInet6ICMP_GroupDone "));
			break;

		case KInet6ICMP_NeighborSol:
			buf.Append(_L("Neighbor Solicitation "));
			break;

		case KInet6ICMP_NeighborAdv:
			buf.Append(_L("Neighbor Advertisement "));
			break;

		default: //buf.Format(_L("Unknown ICMP Type"));
			buf.Format(_L("Unknown ICMP Type"));
			//buf.Format(_L("type=%d "),hdr->Type());
	}
	//buf.Append(_L("\n"));

}

TPtrC CPing::PacketType(ThdrICMP *aHdr)
{
	switch(aHdr->NetGetType())
	{
		case KICMPTypeEchoReply: return _L("ICMP Echo reply");
		case KICMPTypeUnreachable: 
			switch (aHdr->NetGetCode())
			{
				case KICMPCodeUnreachNet: return _L("Network Unreachable");
				case KICMPCodeUnreachHost: return _L("Host Unreachable");
				case KICMPCodeUnreachProtocol: return _L("Protocol Unreachable");
				case KICMPCodeUnreachPort: return _L("Port Unreachable");
				case KICMPCodeUnreachNeedFrag: return _L("Message too long. Fragmentation needed");
				case KICMPCodeUnreachSrcRouteFail: return _L("Source Route Failed");
				case KICMPCodeUnreachNetUnknown: return _L("Destination Network Unknown");
				case KICMPCodeUnreachHostUnknown: return _L("Destination Host Unknown");
				case KICMPCodeUnreachSrcHostIsolated: return _L("Source host isolated");
				case KICMPCodeUnreachNetProhibited: return _L("Destination Network Administatively prohibited");
				case KICMPCodeUnreachHostProhibited: return _L("Destination Host Administatively prohibited");
				case KICMPCodeUnreachNetTOS: return _L("Network Unreachable for TOS");
				case KICMPCodeUnreachHostTOS: return _L("Host Unreachable for TOS");
				case KICMPCodeUnreachProhibited: return _L("Communication Administatively prohibited");
				case KICMPCodeUnreachPrecVolation: return _L("Host Precedence violation");
				case KICMPCodeUnreachPrecCutoff: return _L("Precedence cutoff in effect");
				default: return _L("Unknown code for Destination Unreachable");
			}
		case KICMPTypeSourceQuench: return _L("Source Quench");
		case KICMPTypeRedirect: 
			switch (aHdr->NetGetCode())
			{
				case KICMPCodeRedirectNet: return _L("Redirect for network");
				case KICMPCodeRedirectHost: return _L("Redirect for Host");
				case KICMPCodeRedirectNetTOS: return _L("Redirect for TOS and Network");
				case KICMPCodeRedirectHostTOS: return _L("Redirect for TOS and Host");
				default: return _L("Unknown code for ICMP Redirect");
			}
		case KICMPTypeEchoRequest: return _L("Echo Request");
		case KICMPTypeRouterAdvert: return _L("Router advertisement");
		case KICMPTypeRouterSolicit: return _L("Router solicitation");
		case KICMPTypeTimeExceeded: 
			switch (aHdr->NetGetCode())
			{
				case KICMPCodeExceedInTransit: return _L("TTL 0 during Transit");
				case KICMPCodeExceedInReasm: return _L("TTL 0 during Reassembly");
				default: return _L("Unknown Code for Time exceeded type");
			}
		case KICMPTypeBadParameter: return _L("Parameter Problem");
		case KICMPTypeTimeRequest: return _L("Timestamp Request");
		case KICMPTypeTimeReply: return _L("Timestamp Reply");
		case KICMPTypeInfoRequest: return _L("Information Request");
		case KICMPTypeInfoReply: return _L("Information Reply");
		case KICMPTypeMaskRequest: return _L("Adress Mask Request");
		case KICMPTypeMaskReply: return _L("Adress Mask Reply");
		default: return _L("Unknown ICMP Type");
	}
}

// Shows ICMP data in the Packet and calculates time related info

void CPing::PrintICMPData(const TDesC8& data)
{
	TBuf<300> aux;
	ThdrICMP *hdr;	// Use only this one because the packet format for ICMP_Echo_reply is 
					// identical in both ICMPv4 and ICMPv6

	hdr=(ThdrICMP*)&data[0];	// ICMP packet format

	TUint type;
	if (iType==IPv4)
		type=KICMPTypeEchoReply;
	else	// ICMPv6
		type=KInet6ICMP_EchoReply;

	if (hdr->NetGetType()!=type)
	{
		// We want to list other packets than ICMP Echo Reply
		if (iVerbose)
		{
			if (iType==IPv4)
				aux.Format(PacketType(hdr)); // Return a description of the packet Type and Code
			else
			{
				TBuf<40> auxBuf;
				PacketTypev6(auxBuf,hdr);
				aux.Format(auxBuf);	// Return a description of the packet Type and Code
			}
			aux.Append(_L("\n"));
			WriteLineIfNotQuiet(aux);
		}	// else we ignore them
		return;
	}
	
	// Checks if it's a the packet have a correct Id
	// Useful if two instances if Ping running (not possible in this version)

	if (((ThdrICMP_Echo *)hdr)->NetGetId()!=iId)
	{
		aux.Append(_L("Packet with wrong id received\n"));
		WriteLineIfNotQuiet(aux);
		return;
	}

	// Correct packet type and code

	aux.AppendFormat(_L("Seq: %u"),((ThdrICMP_Echo *)hdr)->NetGetSeq());

	// Checks if chksum is correct must be 0 (because includes the checksum field)
	// else there's something wrong

	if (iType==IPv4)
	{
		if (in_chksum((TUint16 *)&data[0], iPacketDataSize + ICMP_ECHO_HEADER_SIZE)!=0)
		{
			aux.Append(_L(" Checksum Error\n"));
			WriteLineIfNotQuiet(aux);
			iChksumErrors++;
			return;
		}
	}
	// ICMPv6 checks checksum internally

	// Timestamp calculation

	if (iPacketDataSize >= TIMESTAMP_SIZE)
	{
		TTime now;
		TTime time(*(TInt64*)&data[8]);
		now.UniversalTime();

		TTimeIntervalMicroSeconds interval;
		interval = now.MicroSecondsFrom(time);
#ifdef I64LOW
		TUint num = I64LOW(interval.Int64()) / 1000;
#else
		TUint num = interval.Int64().GetTInt() / 1000;
#endif

		if (num > iMaxTime)
			iMaxTime = num;

		if (num < iMinTime)
			iMinTime = num;

		iTimeSum += num;

		aux.AppendFormat(_L("\tTime: %d ms"),num);
	}
	
	iRecvPackets++;

	// Test if duplicated

	if (TEST(((ThdrICMP_Echo *)hdr)->NetGetSeq()))
	{
		aux.Append(_L("\tDUPLICATED"));
		iRecvPackets--;	// because duplicated
		iDupPackets++;  // to show it in statistics
	}
	else
	{
		SET(((ThdrICMP_Echo *)hdr)->NetGetSeq());	// Marks the packet as received
	}

	//aux.Append(_L("\n"));
	WriteLineIfNotQuiet(aux);
}

void CPing::Stop()
{
	iRunning=EFalse;
	iConsole->OnEnd();

	//CEikonEnv::Static()->BusyMsgCancel();
	/*
	delete iPacketData;
	iPacketData=NULL;
	delete iReceivedDataBuffer;
	iReceivedDataBuffer=NULL;
	*/
}

void CPing::BeginL()
{
	TInt err=0;

	if (IsRunning())	// There's another instance running
		return;
	else
		iRunning=ETrue;

	//INITIALIZATION
	iSentPackets=0;		//ICMP Echo Request Packets sent
	iRecvPackets=0;		//ICMP Echo Reply Packets received
	iChksumErrors=0;	//Packets with errors when checking checksum
	iSockErrors=0;		//Errors when writing/reading to/from the sockets
	iDupPackets=0;		//Duplicated packets

	iMaxTime=0;			//Time-related vars
	iMinTime=KMaxTUint32;
	iTimeSum=0;

	if (iPacketDataSize > MAX_ICMP_PACKETSIZE)  //Is prevented in .rss file
		iPacketDataSize = MAX_ICMP_PACKETSIZE;

	
	delete iPacketData;
	iPacketData= HBufC8::NewL(iPacketDataSize + ICMP_HDRLEN); 
						//Allocates space for the ICMP packet
	TPtr8 aux(iPacketData->Des());	//weird but necessary. Cannot use Des() directly in iPacket
	iPacket.Set(aux);
	//iPacket.SetMax();	//because it'll be written directly using [] in Compose...() and it would crash.
	//HUOM!!! Cannot be SetMax because sometimes the reserved size is slightly bigger. Because of block size?
	iPacket.SetLength(iPacketDataSize + ICMP_HDRLEN);	//because it'll be written directly using [] in Compose...() and it would crash.


	delete iReceivedDataBuffer;
	iReceivedDataBuffer= HBufC8::NewL(iPacketDataSize + ICMP_ECHO_HEADER_SIZE);	//Maximum size of a return packet
	TPtr8 auxPtr(iReceivedDataBuffer->Des());	//must be used here because the buffer changes
	iReceivedData.Set(auxPtr);			//we use an aux var because can't use Des() directly Why??
	
	iConsole->WriteHostL(iHostname);
	iConsole->WriteLine(_L("Connecting...\n"));	
	iConsole->UpdateStatisticsL();
	
	
#ifdef IAPSETTING
	//CStoreableOverrideSettings *settings = NULL;
	//CCommDbOverrideSettings::TParamList ParamList = CCommDbOverrideSettings::TParamList::EParamListPartial;
	//CCommDbOverrideSettings::TParamList ParamList = CCommDbOverrideSettings::TParamList::EParamListPartial;
	TCommDbDatabaseType Type = EDatabaseTypeIAP;
	CStoreableOverrideSettings *settings = CStoreableOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial,Type);
	
	CleanupStack::PushL(settings);
	
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref conPref;
	conPref.iRanking = 1;
	conPref.iDirection = ECommDbConnectionDirectionOutgoing;
	CCommsDbConnectionPrefTableView::TCommDbIapBearer bearer;
	bearer.iIapId = iIAP;
	conPref.iBearer = bearer;

	err = settings->SetConnectionPreferenceOverride(conPref);
	User::LeaveIfError(err);
	User::LeaveIfError(iGenericAgent.Open());
    iGenericAgent.StartOutgoing(*settings,iStatus);
	
	err = iGenericAgent.DisableTimers();
    if (err != KErrAlreadyExists)
        User::LeaveIfError(err);
	CleanupStack::PopAndDestroy();	
	User::WaitForAnyRequest();
#endif


	//connecting the Socket Server
	err=iSockServ.Connect();	//KESockDefaultMessageSlots
	if (err!=KErrNone)
	{
		ErrorL(_L("Socket Server Error (Connect)"),err);
		Stop();
		return;
	}

	err=iHostResolv.Open(iSockServ, KAfInet, KProtocolInetIcmp);	// Address Resolver 
	
	if (err!=KErrNone)
	{
		ErrorL(_L("Resolver Error (Open)"),err);

		Stop();
		return;
	}



	//Report(_L("Resolver Open"));
	iConsole->WriteLine(_L("Resolving..."));	
	iHostResolv.GetByName(iHostname,iEntry,iStatus);
	//Report(_L("Resolver GetByName"));
	iStage=0;
	//CEikonEnv::Static()->BusyMsgL(R_RESOLVING_NAME);

	
//-------------------------------------------------
	//DO NOT REMOVE the next commented code!!!!!!!!!!!!!!!!!!
	/*
	//Never will be used because the socket is opened as ICMP not IP so is not usable 
	//unless some changes are made to the socket and packet format.
	
	
	if (iHopLimit!=0)	// 0 means value not set
	{	//Only setable for multicast adresses
		err=iSocket.SetOpt(KSoIpTTL,KSOLSocket,iHopLimit);	//Set TTL (max. hops)
		
		
		if (err==KErrNotSupported)
			iConsole->WriteLine(_L("TTL can only be set with multicast adress (Not used)\n"));
		else
			if (err<0)
			{
				ErrorL(_L("Socket Error (SetOpt)"),err);
				return;
			}
		
	}
--------------------------------------------------------	*/
	IssueRequest();
/*	iStage=0;
	TRequestStatus *xst=&iStatus;
	User::RequestComplete(xst,KErrNone);*/
}

void CPing::CreateSocketAOL()
{
	
	iPingSender=new(ELeave) CPingSender;
    iPingSender->ConstructL(this);	//2nd phase
	iPingSender->FirstRunL();	//Starts packet sending
}

//Issues next RunL execution
void CPing::IssueRequest()
{
	SetActive();	//Sets the object as Active.
					//RunL will be executed when iStatus!=KRequestPending (set by CPingSender)
}


void CPing::RunL()
{
	TInt err=KErrNone;
	TBuf<39> textIPaddress;	//text address to be displayed
	TBuf<356> aux;
	TInetAddr anyaddr;
	TRequestStatus xStatus;

	switch (iStage)
	{
	case 0:
		//Report(_L("Resolver GetByName end"));
		if (iStatus==KErrNotFound)
		{//The Nameserver couldn't find the Host. 
			TBuf<100> warn(iHostname);
			warn.Append(_L(" not found!\n"));
			iConsole->WriteLine(warn);
			iHostResolv.Close();
			Stop();
			return;
		}	
		if (iStatus!=KErrNone)
		{
			ErrorL(_L("Resolver Error (GetByName)"),iStatus.Int());
			iHostResolv.Close();
			//iSockServ.Close();
			Stop();
			return;
		}

		iHostResolv.Close();
		iHostAddr = TInetAddr::Cast(iEntry().iAddr);	//host address

		// The type of PING (ICMPv4 or ICMPv6) depens purely on the destination address.
		// ...for IPv4 (or IPv4 mapped), use ICMPv4
		// ...for true IPv6, use ICMPv6
		iType = (iHostAddr.Family() == KAfInet6 && !iHostAddr.IsV4Mapped()) ? IPv6 : IPv4;

		iHostAddr.Output(textIPaddress);
		/*aux.Append(iEntry().iName);   // maybe the main name is not the entered
		aux.Append(_L(" is "));
		aux.Append(textIPaddress);

		aux.AppendFormat(_L("\nUsing %d data bytes"), iPacketDataSize);
		*/
		aux.AppendFormat(_L("Pinging %S [%S] with %d bytes of data:\n"), &iEntry().iName, &textIPaddress, iPacketDataSize);
		if (iPacketDataSize < TIMESTAMP_SIZE)
			aux.AppendFormat(_L("timestamps disabled (min %d bytes)\n"), TIMESTAMP_SIZE);

		//aux.Append(_L("\n\n"));
		iConsole->WriteLine(aux);

		if (iType==IPv4)
		{
			err=iSocket.Open(iSockServ,_L("icmp"));
		}
		else
		{
			err=iSocket.Open(iSockServ,_L("icmp6"));
		}

		if (err!=KErrNone)
		{
			ErrorL(_L("Socket Error (Open)"),err);
			Stop();
			return;
		}
		//iStage++;
		
		anyaddr.SetAddress(KInetAddrAny);	//Sniffs all packets
		anyaddr.SetPort(KProtocolInetIcmp);	//Sniffs all packets

		err=iSocket.Bind(anyaddr);
		if (err!=KErrNone)
		{	
			ErrorL(_L("Socket Error (Bind)"),err);
			Stop();
			return;
		}

		iSocket.Connect(iHostAddr,xStatus);	//must be wait here or panic esock14
		User::WaitForRequest(xStatus);
		if (xStatus.Int()!=KErrNone)
		{
			ErrorL(_L("Socket Error (Connect)"),xStatus.Int());
			Stop();
			return;
		}
	
		// Socket Options setting
		if (iDebug)
			err=iSocket.SetOpt(KSODebug,KSOLSocket,1);	//Enable debugging
		else
			err=iSocket.SetOpt(KSODebug,KSOLSocket,0);	//disable debugging (DEFAULT)
            
		if (err!=KErrNone)
		{
			ErrorL(_L("Socket Error (SetOpt)"),err);
			//iHostResolv.Close();
			iSocket.Close();
			//iSockServ.Close();
#ifdef IAPSETTING			
			iGenericAgent.Close();
#endif 			
			Stop();
			return;
		}
		//CEikonEnv::Static()->BusyMsgCancel();	//Cancel the resolving name msg
		CreateSocketAOL();	//Creates the send A.O.
		iSocket.Read(iReceivedData,iStatus);
		iStage++;
		IssueRequest();	//Prepares to receive it in RunL()
		break;

	case 1:
		//Report(_L("Socket Read end"));
		if (iStatus==KErrNone)
		{
			PrintICMPData(iReceivedData);	//The previous packet
		}
		else
			ErrorL(_L("Read (Recv)"),iStatus.Int());

		iConsole->UpdateStatisticsL();
		
		iSocket.Read(iReceivedData,iStatus);	// NEXT Packet
		IssueRequest();	// Prepares to receive it in RunL()
		// No more stages!
		break;

	default:
		//CEikonEnv::Static()->InfoMsg(_L("Bad Stage!!!"));
		EndPingL();
	}

}

void CPing::DoCancel()
	{
	if (iStage==0)
		{
		iHostResolv.Cancel();
		}
	else if (iStage==1)	
		{
		//RSocket::Read has been called, so need to cancel this outstanding Read
		iSocket.CancelRead();
		}
	}

void CPing::CloseAll()
	{
	iHostResolv.Close();
	//iSocket.CancelAll();	//Cancel all outstanding requests 
	iSocket.Close();	
	iSockServ.Close();
#ifdef IAPSETTING
	iGenericAgent.Close();
#endif
}

// Stops Ping

void CPing::EndPingL()
{	
	//CEikonEnv::Static()->BusyMsgCancel();	// Cancel the resolving name msg in case it wasn't

	Statistics();
	iConsole->UpdateStatisticsL();
	
	if (iPingSender!=0)	// Not needed if control dimmed because can't launch 2 pings
	{
		delete iPingSender;
		iPingSender=0;
	}

	Cancel();

	CloseAll();
	iRunning = EFalse;
	iConsole->OnEnd();
}

// Just checks if sending packets from a previous ping

TBool CPing::IsRunning()
{
	return iRunning;
}

// Sends packets with an active Timer
// Created by CPing
// Not intended to run alone

CPingSender::CPingSender():CTimer(EPriorityStandard)//,iSentData(0,0)
{
	CActiveScheduler::Add(this);	//Adds itself to the scheduler only the first time
}

CPingSender::~CPingSender()
	{
	Cancel();
	delete iSender;
	}

void CPingSender::ConstructL(CPing *aPingModel)
{
	//Base class 2nd phase constructor
	CTimer::ConstructL();

	iPingModel=aPingModel;	// Poiter to the model which contains data
	//iSentDataBuffer= HBufC8::NewL(iPingModel->iPacketDataSize + ICMP_HDRLEN);
							//Allocates the maximum space needed the size chosen + ICMP Header
	iSender=new(ELeave) CPingSingleSender(iPingModel);
}


//Issues next RunL execution
void CPingSender::IssueRequest()
{
	After(iPingModel->iSecWait*SECOND);	//Also sets the object as Active
}

//Issues last RunL execution
void CPingSender::IssueLastRequest()
{
	After(iPingModel->iLastSecWait*SECOND);	//Also sets the object as Active
}


void CPingSender::FirstRunL()
{	
	
	SendFirstPacketL();
	
	if ((iPingModel->iSentPackets==iPingModel->iTotalPackets) && (iPingModel->iPackLimit))
		IssueLastRequest();	//Last RunL	have a special waiting time. to receive the last reply
	else
		IssueRequest();	//First RunL
}



// will send all the packets. One packet each Time
void CPingSender::RunL()
{
	if ((iPingModel->iSentPackets>=iPingModel->iTotalPackets) && (iPingModel->iPackLimit))
	{
		iPingModel->EndPingL();
		return;
	}
	else	//There are packets to send or number unlimited
	{		
		SendPacket();

		if ((iPingModel->iSentPackets>=iPingModel->iTotalPackets) && (iPingModel->iPackLimit))
			IssueLastRequest();	//Last RunL	have a special waiting time. to receive the last reply
		else
			IssueRequest();	//Next RunL
	}

}


//Creates a AO that sends a packets waits for it to be send and dies
void CPingSender::SendFirstPacketL()
{
	iSender->FirstRunLD();	//Starts packet sending. Destroys itself
}


//Creates a AO that sends a packets waits for it to be send and dies
void CPingSender::SendPacket()
{
	iSender->NextPacket();
}

//Cancel Packet Sending
void CPingSender::DoCancel()
	{
	CTimer::DoCancel();
	}




// Used by CPingSender
// Sends packets. Cannot be done directly by CPingSender because there are conflicts with
//					diferent TRequestStatus.

CPingSingleSender::CPingSingleSender(CPing *aPingModel):CActive(EPriorityStandard)
{
	iPingModel=aPingModel;	// Pointer to the model which contains data
	CActiveScheduler::Add(this);	//Adds itself to the scheduler only the first time
}

CPingSingleSender::~CPingSingleSender()
	{
	Cancel();
	}


// Issues next RunL execution

void CPingSingleSender::IssueRequest()
{
	SetActive();	// Sets the object as Active.
}

void CPingSingleSender::FirstRunLD()
{	
	iPingModel->ComposeFirstICMPPacket();
	iPingModel->iSocket.Write(iPingModel->iPacket, iStatus); // iStatus used by CTimer
	iPingModel->iSentPackets++;
	iUnsent=0;
	IssueRequest();
}

void CPingSingleSender::NextPacket()
{	
	if (IsActive())		// Still a packet being sent
		iUnsent++;		// Cannot sent here because iSatus would be overwritten
	else
	{
		iPingModel->ComposeICMPPacket();
		iPingModel->iSocket.Write(iPingModel->iPacket, iStatus); // No other request waiting
		iPingModel->iSentPackets++;
		IssueRequest();
	}
}

// will send all the packets. One packet each Time
// when entering this function, 
// it means either one packet has actually been sent, or failed
void CPingSingleSender::RunL()
{
//	TBuf<50> aux;
	
//	if (iStatus==KErrNone)
//		iPingModel->iSentPackets++;
	//else if (iStatus!=KRequestPending)
	if (iStatus!=KErrNone)
	{
		iPingModel->iSentPackets--;	//Packet sending failed
		iPingModel->ErrorL(_L("Write"),iStatus.Int());
	}

	//aux.Format(_L("Write end (st=%d, sent=%d)"),iStatus,iPingModel->iSentPackets);
	//iPingModel->WriteLineIfNotQuiet(aux);
	iPingModel->iConsole->UpdateStatisticsL();
	//Ignores the timer request because there are packets that should already be sent
	if (iUnsent)
	{
		iPingModel->ComposeICMPPacket();
		iPingModel->iSocket.Write(iPingModel->iPacket, iStatus); // iStatus used by CTimer
		iPingModel->iSentPackets++;
		iUnsent--;
		IssueRequest();
	}
}

// Cancel Packet Sending

void CPingSingleSender::DoCancel()
	{
	iPingModel->iSocket.CancelWrite();
	}
