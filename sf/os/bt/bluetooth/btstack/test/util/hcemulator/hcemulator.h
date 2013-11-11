// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef HCEMULATOR_H
#define HCEMULATOR_H

// forward declarations
#include <e32test.h>
#include <es_sock.h>
#include <f32file.h>
#include <bt_sock.h>
#include <c32comm.h>
#include <bluetooth/hci/hcierrors.h>
#include <bluetooth/hci/hcitypes.h>

const TInt KReadBufSize			=258;
const TInt KWriteBufSize		=258;

// properties of emulated host controller

const TBTDevAddr KEmulatedHCAddress(MAKE_TINT64(0xfeed, 0xdeadbeef));						
_LIT8(KDefaultLocalName, "HCEmuDevice");

const TUint16 KEmulatedHCNumberACLBuffers			=0x0010;
const TUint16 KEmulatedHCSizeACLBuffers				=0x02A0;
const TUint16 KEmulatedHCNumberSCOBuffers			=0x0000;
const TUint8 KEmulatedHCSizeSCOBuffers				=0x00;
const TEncryptMode KEmulatedEncryptMode				=EDisabled;
const TUint16 KEmulatedClockOffset					=0xfd5d;

// emulated devices found

const TBTDevAddr KDeviceAddresses []={
									MAKE_TINT64(0x1111, 0x11111111),
									MAKE_TINT64(0x2222, 0x22222222),
									MAKE_TINT64(0x3333, 0x33333333),
									MAKE_TINT64(0x4444, 0x44444444),
									MAKE_TINT64(0x5555, 0x55555555),
									MAKE_TINT64(0x6666, 0x66666666),
									MAKE_TINT64(0x7777, 0x77777777),
									MAKE_TINT64(0x8888, 0x88888888),
									MAKE_TINT64(0x9999, 0x99999999),
									MAKE_TINT64(0xaaaa, 0xaaaaaaaa),
									MAKE_TINT64(0xbbbb, 0xbbbbbbbb),
									MAKE_TINT64(0xcccc, 0xcccccccc),
									MAKE_TINT64(0xdddd, 0xdddddddd),
									MAKE_TINT64(0xeeee, 0xeeeeeeee),
									MAKE_TINT64(0xffff, 0xffffffff),
									MAKE_TINT64(0x1111, 0x22222222),
									MAKE_TINT64(0x2222, 0x33333333),
									MAKE_TINT64(0x3333, 0x44444444),
									MAKE_TINT64(0x4444, 0x55555555),
									MAKE_TINT64(0x5555, 0x66666666),
									MAKE_TINT64(0x6666, 0x77777777),
									MAKE_TINT64(0x7777, 0x88888888),
									MAKE_TINT64(0x8888, 0x99999999),
									MAKE_TINT64(0x9999, 0xaaaaaaaa)
									};
const TBuf8<20> KDeviceNames [] ={
								_L8("Emulated Device 1"),
								_L8("Emulated Device 2"),
								_L8("Emulated Device 3"),
								_L8("Emulated Device 4"),
								_L8("Emulated Device 5"),
								_L8("Emulated Device 6"),
								_L8("Emulated Device 7"),
								_L8("Emulated Device 8"),
								_L8("Emulated Device 9"),
								_L8("Emulated Device a"),
								_L8("Emulated Device b"),
								_L8("Emulated Device c"),
								_L8("Emulated Device d"),
								_L8("Emulated Device e"),
								_L8("Emulated Device f"),
								_L8("Emulated Device 12"),
								_L8("Emulated Device 23"),
								_L8("Emulated Device 34"),
								_L8("Emulated Device 45"),
								_L8("Emulated Device 56"),
								_L8("Emulated Device 67"),
								_L8("Emulated Device 78"),
								_L8("Emulated Device 89"),
								_L8("Emulated Device 9a")
								};


const TUint8  KEmulatedInquiryPageScanRepetionMode	= 0x01;
const TUint8  KEmulatedInquiryPageScanPeriodMode	= 0x02;
const TUint8  KEmulatedInquiryPageScanMode			= 0x03;
const TUint	  KEmulatedInquiryClassOfDevice			= 0xcc00dd;
const TUint	  KEmulatedInquiryNullClassOfDevice		= 0x0;
const TUint16 KEmulatedInquiryClockOffset			= 0xabcd;
const TUint	  KEmulatedLocalSupportedFeatures1	    = 0x000fffff;
const TUint   KEmulatedLocalSupportedFeatures2		= 0x00000000;
const TUint	  KEmulatedLocalVersion1			    = 0x01020d01;
const TUint   KEmulatedLocalVersion2				= 0x020d000a;
const TUint	  KEmulatedCoD							= 0x000000;
const TUint	  KEmulatedRemoteSupportedFeatures1		= 0x0031ea04;
const TUint	  KEmulatedRemoteSupportedFeatures2		= 0x00000000;
const TUint	  KEmulatedRemoteVersionInfo1			= 0x01000000;
const TUint8  KEmulatedRemoteVersionInfo2			= 0x04;
const TUint16 KEmulatedChangedPacketType			= 0x0018;

const TTimeIntervalSeconds KEmulatedInterDeviceInquiryTime			=0x05;	// seconds

// emulated results
const TUint8 KEmulatedInquiryNumberDevicesFound		=0x18;
const THCIConnHandle KEmulatedBaseConnectionHandle	=0x0001;
const TUint8 KHCIEmulatedDisconnectionReason		=0x13;

// HCI Interface events

const TInt KHCIUARTCommandHeader				=0x01;
const TInt KHCIUARTACLDataHeader				=0x02;
const TInt KHCIUARTEventHeader					=0x04;

const TUint8 KHCIInquiryCompleteEventCode		=0x01;
const TUint8 KHCIInquiryResultEventCode			=0x02;
const TUint8 KHCIConnectionCompleteEventCode	=0x03;
const TUint8 KHCIConnectionRequestEventCode		=0x04;
const TUint8 KHCICommandCompleteEventCode		=0x0e;
const TUint8 KHCICommandStatusEventCode			=0x0f;
const TUint8 KHCIDisconnectionCompleteEventCode	=0x05;
const TUint8 KHCIRemoteNameRequestCompleteEventCode = 0x07;
const TUint8 KHCINumCompletedPacketsEventCode	=0x13;
const TUint8 KHCIReadClockOffsetCompleteEventCode = 0x1c;
const TUint8 KHCIReadRemoteSupportedFeaturesCode = 0x0b;
const TUint8 KHCIReadRemoteVersionInfoCode 		= 0x0c;
const TUint8 KHCIConnectionPacketTypeChangedCode = 0x1d;
const TUint8 KHCIHardwareErrorEventCode			=0x10;
const TUint8 KHCISpecificVendorDebugEvent		=0xff;
// opcodes
const TUint16 KNoOpcode						=0x0000;

// lengths
const TUint8 KHCIDefaultCommandCompleteEventLength			=0x04;
const TUint8 KHCICommandStatusEventLength					=0x04;
const TUint8 KHCIReadBufferSizeCommandCompleteEventLength	=0x0b;
const TUint8 KHCIReadBdaddrCommandCompleteEventLength		=0x0a;
const TUint8 KHCIReadLocalNameCommandCompleteEventLength	=0xfc;
const TUint8 KHCIConnectionCompleteEventLength				=0x0b;
const TUint8 KHCIInquiryCompleteEventLength					=0x01;
const TUint8 KHCIDisconnectionCompleteEventLength			=0x04;
const TUint8 KHCINumCompletedPacketsEventLength				=0x05;
const TUint8 KHCIInquiryResultEventLength					=0x0f;
const TUint8 KHCIConnectionRequestEventLength				=0x0a;
const TUint8 KHCIReadLocalSupportedFeaturesLength			=0x0c;
const TUint8 KHCIReadLocalVersionInformationLength			=0x0c;
const TUint8 KHCIReadCoDLength								=0x07;
const TUint8 KHCIWriteLinkPolicySettingsCompleteLength		=0x06;
const TUint8 KHCIReadClockOffsetCompleteLength				=0x05;
const TUint8 KHCIReadRemoteSupportedFeaturesLength			=0x0b;
const TUint8 KHCIReadRemoteVersionInfoLength				=0x08;
const TUint8 KHCIConnectionPacketTypeChangedLength			=0x05;
const TUint8 KHCIHardwareErrorEventLength					=0x01;

// errors
const TUint8 KHCIOK								=0x00;
const TUint8 KHCINoConnection					=0x02;
const TUint8 KHCIPageTimeout					=0x04;
const TUint8 KHCICommandDisallowed				=0x0c;


// forward declarations
class CActiveConsole;
class CActiveReader;
class CActiveWriter;
class CInquiryTimer;

class CHCEmulator : public CBase
	{
	public:

		static CHCEmulator* NewL(CConsoleBase& aConsole);
		void CommandStatusEvent(TUint16 aOpcode, TUint8 aError);
		void DefaultCommandCompleteEvent(TUint16 aOpcode, TUint8 aError);
		void ReadBufferSizeCompleteEvent(TUint8 aError);
		void ReadBDAddrCompleteEvent(TUint8 aError);
		void ReadLocalNameCompleteEvent(TUint8 aError);
		void InquiryCompleteEvent(TUint8 aError);
		void ConnectionCompleteEvent(TUint8 aError, TLinkType aType);
		void ConnectionRequestEvent(const TBTDevAddr& aAddr, TUint aCoD, TLinkType aType);
		void DisconnectionCompleteEvent(TUint8 aError, THCIConnHandle aConnH, TUint8 aReason);
		void InquiryStartEvent(TUint8 aError);
		void InquiryResultEvent();
		void NumberCompletedPacketsEvent(/*const TDesC8& aConnHandleFlagsBuf*/THCIConnHandle aConnH, TUint8 aNumPackets, TUint8 aError);
		void InquiryCancelCompleteEvent(TUint8 aError);
		void ReadRemoteNameCompleteEvent(TUint8 aError, const TBTDevAddr& aAddr, const TDesC8& aName);
		void ReadLocalSupportedFeaturesCompleteEvent(TUint8 aError);
		void ReadRemoteSupportedFeaturesCompleteEvent(TUint8 aError, THCIConnHandle aConnH);
		void ReadRemoteVersionInfoCompleteEvent(TUint8 aError, THCIConnHandle aConnH);
		void ReadLocalVersionInfoCompleteEvent(TUint8 aError);
		void WriteVoiceSettingCompleteEvent(TUint8 aError);
		void WriteCurrentIACLAPCompleteEvent(TUint8 aError);
	    void ReadClassOfDeviceCompleteEvent(TUint8 aError);
		void WriteLinkPolicySettingsCompleteEvent(TUint8 aError, THCIConnHandle aConnh);
		void ReadClockOffsetCompleteEvent(TUint8 aError, THCIConnHandle aConnH);
		void VendorSpecificDebugCompleteEvent(TUint16 aOpCode, const TDesC8& aRemainderBuf);
		void HardwareErrorEvent();
		void SpecificVendorDebugEvent();
		
		void CommandReceived(const TDesC8& aOpcodeBuf, const TDesC8& aRemainderBuf);
		void ACLDataReceived(const TDesC8& aConnHandleFlagsBuf, const TDesC8& aRemainderBuf);
		void SCODataReceived(const TDesC8& /*aConnHandleBuf*/, const TDesC8& /*aRemainderBuf*/) {}; // not yet!
		void ConnectionPacketTypeChangedEvent(TUint8 aError, THCIConnHandle aConnH);

		static CConsoleBase* AutoSizeNewL(const TDesC& aTitle, TSize aSize);

		void Start();
		void Stop();
		~CHCEmulator();

	private:
		CHCEmulator(CConsoleBase& aConsole);
		void ConstructL(CConsoleBase& aConsole);

		enum TPacketType
			{
			ECommandStatusEvent,
			ECommandCompleteEvent,
			EInquiryCompleteEvent,
			EACLConnectionCompleteEvent,
			ESCOConnectionCompleteEvent,
			EDisconnectCompleteEvent
			};

		void BuildAndWriteCommandPacket(TPacketType aType, TUint16 aOpcode, TUint8 aError);
		void BuildAndWriteDataPacket(const TDesC8& aConnHandleFlagsBuf, /*THCIConnHandle aConnH,*/ const TDesC8& aRemainderBuf);

		void PutBytes(TUint aNum, TUint8 aNumOfBytes=1);
		void PutAddress(const TBTDevAddr& aAddress);
		void PutName(const TDesC8& aName);
		
// util functions
		void GetBigEndianDevAddr(TBTDevAddr& aAddresS, const TUint8* aPacket);
		void GetConnHandle(THCIConnHandle& aConnH, const TUint8* aPacket);
		void GetByte(TUint8& aByte, const TUint8* aPacket);

		TBool Decode(const TDesC8& aOpcodeBuf, const TDesC8& aRemainderBuf);	
		void ResetEmulator();
		void WritePacket();
		void ClearPacket();
		TInt FindConnection(THCIConnHandle aConnH);
		TInt GetRemoteName(const TBTDevAddr& aAddr, TPtrC8& aName) const;

		
	private:
		CActiveConsole*	iActiveConsole;
		CConsoleBase&	iConsole;
		CConsoleBase*	iConnectionsConsole;
		CActiveWriter*	iWriter;
		CActiveReader*	iReader;
		CInquiryTimer*	iInquiryTimer;
		TUint8			iCurrentInquiryResponse;
		RCommServ		iCommServer;
		RComm			iPort;
		HBufC8*			iPacketBuffer;
		TPtr8			iPacket;
		TUint			iParsePtr;		// points to current parse point

	private:
		// emulated stuff
		RArray<TBTConnect>	iConnections; // competed once a createconnection command appears
		TFixedArray<TBTDevAddr, KEmulatedInquiryNumberDevicesFound>	iInquiryAddresses;	// for emulated inquiries
		TFixedArray<TBuf8<20>, KEmulatedInquiryNumberDevicesFound>	iInquiryNames;		// for emulated name lookups
		THCIScanEnable		iScanEnable;
		const TBTDevAddr	iHCAddress;
		RBuf8				iLocalName;
	};

class CActiveConsole : public CActive
	{
	public:
		static CActiveConsole* NewL(CConsoleBase& aConsole, CHCEmulator& aEmulator);

		void RunL();
		void DoCancel();
		void TestMenu();
		void DrawCursor();
		~CActiveConsole();

	private:
		CActiveConsole(CConsoleBase& aConsole, CHCEmulator& aEmulator);
		void ConstructL();
		void RequestKey();

	private:
		CConsoleBase&	iConsole;
		CHCEmulator&	iEmulator;
	};

class CActiveReader : public CActive
	{
	public:
		static CActiveReader* NewL(CConsoleBase& aConsole, CHCEmulator& aEmulator, RComm& iPort);
		void RunL();
		void DoCancel();
		~CActiveReader();

	private:
		CActiveReader(CConsoleBase& aConsole, CHCEmulator& aEmulator, RComm& iPort);
		void ConstructL();
		void DoRead();
		TInt DecodeHeader();
		TInt DecodeOpcode();

	private:
		enum TReadState
			{
			EReadHeader,
			EReadOpcode,
			EReadCommandLength,
			EReadDataLength,
			EReadCommandRemainder,
			EReadDataRemainder,
			EReadConnectionHandleFlags,
			};

	private:
		CConsoleBase&	iConsole;
		CHCEmulator&	iEmulator;
		RComm&			iPort;
		CConsoleBase*	iReaderOutputConsole;
		TBool			iAuto;
		TReadState		iReadState;
		TBuf8<1>		iHeaderBuf;
		TBuf8<2>		iOpcodeBuf;
		TBuf8<1>		iCommandLengthBuf;
		TBuf8<2>		iDataLengthBuf;
		TBuf8<2>		iConnectionHandleFlagsBuf;
		TBuf8<255>		iRemainderBuf;
		TBool			iReadComplete;
	};

class CActiveWriter : public CActive
	{
	public:
		static CActiveWriter* NewL(CConsoleBase& aConsole, CHCEmulator& aEmulator, RComm& iPort);
		void RunL();
		void DoCancel();
		void WriteL(const TDesC8& aPacket);
		~CActiveWriter();

	private:
		CActiveWriter(CConsoleBase& aConsole, CHCEmulator& aEmulator, RComm& iPort);
		void ConstructL();
		void DoWrite();

	private:
		CConsoleBase&	iConsole;
		CHCEmulator&	iEmulator;
		RComm&			iPort;
		RPointerArray<HBufC8>	iWriteBuffers;
	};


class CInquiryTimer : public CTimer
/**
	Simulates the finite time to find stuff - handy for testing inquiry cancelling
	RunL asks emulator to send up one InquiryStatus with n devices in
**/
	{
	public:
		static CInquiryTimer* NewL(CHCEmulator& aEmulator);
		void RunL();
		void Start();

	private:
		CInquiryTimer(CHCEmulator& aEmulator);
		void ConstructL();

	private:
		CHCEmulator& iEmulator;
	};

/**
	@internalComponent
	class TInputManager
	It manage the input from the keyboard and the output on the console.
**/
class TInputManager
	{
	public:
		TInputManager(CConsoleBase& aConsole);
		
		TUint ReadString(TDes8& aStr);
		TInt  String2Uint(TDes8& aStr, TUint& aResult, TBool aHex=EFalse);
	private:
		void ConsoleBackspace(TUint aNumberOfChars);
		TUint GetCipher(TChar aChar);
	private:
		CConsoleBase& iConsole;
	};

#endif // HCEMULATOR_H
