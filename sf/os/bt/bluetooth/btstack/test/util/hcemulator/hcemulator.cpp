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
// BT test code to emulate certain aspects of host controller over null modem
// for testing stack behaviour on invalid events etc from HC
// 
//

#include <e32test.h>
#include <es_sock.h>
#include <f32file.h>
#include <e32test.h>
#include <c32comm.h>
#include <bluetooth/hci/hciopcodes.h>
#include <bluetooth/hci/hciconsts.h>
#include <bluetooth/hci/aclpacketconsts.h>

#include "hcemulator.h"

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else  // __GCC32__
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
// #define ETNA_PDD_NAME _L("EUART2") // for debugging over Com2
#endif

#define _EMULATOR_HWFC
#undef  _EMULATOR_HWFC

//The #define below was introduced for DEF074193. 
//This defect was subsumed by INC075303.
//#define DISCONNECT_ON_ACCEPT //Tweak HCEmulator to disconnect instead of completing an Accept_Connection_Request cmd


GLDEF_D RTest test(_L("Host Controller Emulator"));



CHCEmulator* CHCEmulator::NewL(CConsoleBase& aConsole)
	{
	CHCEmulator* e = new (ELeave) CHCEmulator(aConsole);
	CleanupStack::PushL(e);
	e->ConstructL(aConsole);
	CleanupStack::Pop(e);
	return e;
	}


CHCEmulator::CHCEmulator(CConsoleBase& aConsole) : iConsole(aConsole), iPacket(NULL,0), iHCAddress(KEmulatedHCAddress)
	{
	}


CHCEmulator::~CHCEmulator()
	{
	delete iReader;
	delete iWriter;
	delete iInquiryTimer;
	delete iActiveConsole;
	delete iConnectionsConsole;
	delete iPacketBuffer;
	
	iLocalName.Close();

	iConnections.Close();

	iPort.Close();
	iCommServer.Close();
	}


void CHCEmulator::ConstructL(CConsoleBase& aConsole)
	{
	User::LeaveIfError(iCommServer.Connect());
	User::LeaveIfError(iCommServer.LoadCommModule(_L("ECUART")));
	
	iConsole.Printf(_L("Select COM port that NULL modem lead is connected to (0/1/2/3/4/5/6/7/8/9):"));

	TChar port;
	
	do
		{
		port = iConsole.Getch();
		}
	while (port != '0' && 
		   port != '1' && 
		   port != '2' && 
		   port != '3' && 
		   port != '4' && 
		   port != '5' && 
		   port != '6' && 
		   port != '7' && 
		   port != '8' && 
		   port != '9');

	TBuf<7> portName;

	portName = _L("COMM::");
	portName.Append(port);

	TInt ret=iPort.Open(iCommServer,portName,ECommExclusive);
	
	if (ret!=KErrNone)
		User::Leave(ret);

	iPort.ResetBuffers();

	TCommConfig config;
	iPort.Config(config);
	config().iRate=EBps115200;					// 115200 for CSR, 57600 for EBDK builds of stack
	config().iDataBits=EData8;
	config().iParity=EParityNone;			    // no parity
	config().iStopBits=EStop1;                 // 1 stop bit
#ifdef _EMULATOR_HWFC
	config().iHandshake=KConfigObeyCTS | KConfigFreeRTS | KConfigFreeDTR;        // no handshaking;
#else
	config().iHandshake=0;
#endif
	User::LeaveIfError(iPort.SetConfig(config));

//	iPort.SetSignals(KSignalDTEInputs,0);			// allow other side to send
#ifdef _EMULATOR_HWFC
	iPort.SetSignals(KSignalRTS,KSignalDTR);
#endif

	iReader = CActiveReader::NewL(aConsole, *this, iPort);
	iWriter = CActiveWriter::NewL(aConsole, *this, iPort);
	iInquiryTimer = CInquiryTimer::NewL(*this);

	iActiveConsole = CActiveConsole::NewL(aConsole, *this);
	iConnectionsConsole = AutoSizeNewL(_L("Emulated Connections"), TSize(60,20));

	iPacketBuffer = HBufC8::NewL(KWriteBufSize);
	iPacket.Set(iPacketBuffer->Des());

	iInquiryAddresses.Copy(&KDeviceAddresses[0], KEmulatedInquiryNumberDevicesFound);
	iInquiryNames.Copy(&KDeviceNames[0], KEmulatedInquiryNumberDevicesFound);
	
	iLocalName.Create(248);
	iLocalName.Append(KDefaultLocalName);
	}


CConsoleBase* CHCEmulator::AutoSizeNewL(const TDesC& aTitle, TSize aSize)
	{
	// Try to create a console of our preferred size, otherwise we
	// guess the screen is too small and use its entirety.
	CConsoleBase* console = NULL;
	TRAPD(err, console = Console::NewL(aTitle, aSize));
	if (err != KErrNone)
		{
		// If we leave now it is not because of offscreen drawing.
		console = Console::NewL(aTitle, TSize(KConsFullScreen, KConsFullScreen));
		}
	return console;
	}


void CHCEmulator::Start()
	{
	CActiveScheduler::Start();
	}


void CHCEmulator::Stop()
	{
	CActiveScheduler::Stop();
	}


void CHCEmulator::CommandStatusEvent(TUint16 aOpcode, TUint8 aError)
	{
	iConsole.Printf(_L("Sending C.S.E (opcode 0x%04x, error 0x%02x)...\n"), aOpcode, aError);
	BuildAndWriteCommandPacket(ECommandStatusEvent, aOpcode, aError);
	}


void CHCEmulator::InquiryStartEvent(TUint8 aError)
	{
	iConsole.Printf(_L("Starting inquiry...")); //FIXME IAC

	iCurrentInquiryResponse = 0;
	iInquiryTimer->Start();
	
	BuildAndWriteCommandPacket(ECommandCompleteEvent, KInquiryOpcode, aError);
	}


void CHCEmulator::DefaultCommandCompleteEvent(TUint16 aOpcode, TUint8 aError)
	{
	iConsole.Printf(_L("Sending C.C.E (opcode 0x%04x, error 0x%02x)...\n"), aOpcode, aError);
	BuildAndWriteCommandPacket(ECommandCompleteEvent, aOpcode, aError);
	}


void CHCEmulator::ReadBufferSizeCompleteEvent(TUint8 aError)
	{
	iConsole.Printf(_L("Sending C.C.E (opcode ReadBufSize, error 0x%02x)...\n"), aError);
	BuildAndWriteCommandPacket(ECommandCompleteEvent, KReadBufferSizeOpcode, aError);
	}


void CHCEmulator::ReadBDAddrCompleteEvent(TUint8 aError)
	{
	iConsole.Printf(_L("Sending C.C.E (opcode ReadBDAddr, error 0x%02x)...\n"), aError);
	BuildAndWriteCommandPacket(ECommandCompleteEvent, KReadBdaddrOpcode, aError);
	}
	

void CHCEmulator::ReadLocalNameCompleteEvent(TUint8 aError)
	{
	iConsole.Printf(_L("Sending C.C.E (opcode ReadLocalName, error 0x%02x)...\n"), aError);
	BuildAndWriteCommandPacket(ECommandCompleteEvent, KReadLocalNameOpcode, aError);
	}


void CHCEmulator::InquiryCompleteEvent(TUint8 aError)
	{
	ClearPacket();

	iConsole.Printf(_L("Sending InquiryComplete Event (error 0x%02x)...\n"), aError);

	PutBytes(KHCIUARTEventHeader);
	PutBytes(KHCIInquiryCompleteEventCode);
	PutBytes(KHCIInquiryCompleteEventLength);
	PutBytes(aError);
	
	WritePacket();
	}


void CHCEmulator::InquiryResultEvent()
	{
	TInt resultsInPacket = Min(((iCurrentInquiryResponse % 5) + 1) , (KEmulatedInquiryNumberDevicesFound - iCurrentInquiryResponse));
	
	ClearPacket();

	TInt error = KHCIOK;

	iConsole.Printf(_L("Sending InquiryResult Event (error 0x%02x)...\n"), error);

	PutBytes(KHCIUARTEventHeader);
	PutBytes(KHCIInquiryResultEventCode);
	PutBytes(1 + (14 * resultsInPacket));
	PutBytes(resultsInPacket);
	
	for (TInt i = 0; i < resultsInPacket; i++)
		{
		// stick them all in one event!
		
		PutAddress(iInquiryAddresses[iCurrentInquiryResponse + i]);
		PutBytes(KEmulatedInquiryPageScanRepetionMode);
		PutBytes(KEmulatedInquiryPageScanPeriodMode);
		PutBytes(KEmulatedInquiryPageScanMode);
		PutBytes(KEmulatedInquiryClassOfDevice,3);
		PutBytes(KEmulatedInquiryClockOffset,2);
		}

	WritePacket();
	
	iCurrentInquiryResponse += resultsInPacket;
	
	// Wait for another result if we've not done them all, else complete inquiry
	if(iCurrentInquiryResponse < KEmulatedInquiryNumberDevicesFound)
		{
		iInquiryTimer->Start();
		}
	else
		{
		InquiryCompleteEvent(KHCIOK);
		}
	}


void CHCEmulator::ConnectionCompleteEvent(TUint8 aError, TLinkType /*aType*/)
	{
	ClearPacket();

	iConsole.Printf(_L("Sending ConnectionComplete Event (error 0x%02x)...\n"), aError);

	const TBTConnect& newConnection = iConnections[iConnections.Count()-1];

	PutBytes(KHCIUARTEventHeader);
	PutBytes(KHCIConnectionCompleteEventCode);
	PutBytes(KHCIConnectionCompleteEventLength);

	// test for what happens if we createconnection while inquiry is active
	if (iInquiryTimer->IsActive())
		PutBytes(KHCICommandDisallowed);
	else
		PutBytes(aError);

	PutBytes(newConnection.iConnH,2);
	PutAddress(newConnection.iBdaddr); // <-- test signaller by changing this address
	PutBytes(newConnection.iLinkType);
	PutBytes(newConnection.iEncryptMode);
	
	WritePacket();
	}


void CHCEmulator::InquiryCancelCompleteEvent(TUint8 aError)
	{
	iInquiryTimer->Cancel();

	DefaultCommandCompleteEvent(KInquiryCancelOpcode, aError); 
	}


void CHCEmulator::ConnectionRequestEvent(const TBTDevAddr& aAddr, TUint aCoD, TLinkType aType)
	{
	ClearPacket();

	iConsole.Printf(_L("Sending Connection Request event...\n"));

	PutBytes(KHCIUARTEventHeader);
	PutBytes(KHCIConnectionRequestEventCode);
	PutBytes(KHCIConnectionRequestEventLength);
	PutAddress(aAddr);
	PutBytes(aCoD,3);
	PutBytes(aType);
	
	WritePacket();
	}


void CHCEmulator::DisconnectionCompleteEvent(TUint8 aError, THCIConnHandle aConnH, TUint8 aReason)
	{
	ClearPacket();

	iConsole.Printf(_L("Sending DisconnectComplete Event (error 0x%02x)...\n"), aError);

	PutBytes(KHCIUARTEventHeader);
	PutBytes(KHCIDisconnectionCompleteEventCode);
	PutBytes(KHCIDisconnectionCompleteEventLength);
	PutBytes(aError);
	PutBytes(aConnH, 2);
	PutBytes(aReason);
	
	WritePacket();
	}


void CHCEmulator::WriteLinkPolicySettingsCompleteEvent(TUint8 aError, THCIConnHandle aConnH)
	{
	ClearPacket();

	iConsole.Printf(_L("Sending WriteLinkPolicySettingsComplete Event (error 0x%02x)...\n"), aError);

	PutBytes(KHCIUARTEventHeader);
	PutBytes(KHCICommandCompleteEventCode);
	PutBytes(KHCIWriteLinkPolicySettingsCompleteLength);
	PutBytes(1); //1 command token
	PutBytes(KWriteLinkPolicySettingsOpcode,2);
	PutBytes(aError);
	PutBytes(aConnH, 2);

	WritePacket();	
	}


void CHCEmulator::ReadClockOffsetCompleteEvent(TUint8 aError, THCIConnHandle aConnH)
	{
	ClearPacket();

	iConsole.Printf(_L("Sending ReadClockOffsetComplete Event (error 0x%02x)...\n"), aError);

	PutBytes(KHCIUARTEventHeader);
	PutBytes(KHCIReadClockOffsetCompleteEventCode);
	PutBytes(KHCIReadClockOffsetCompleteLength);
	PutBytes(aError);
	PutBytes(aConnH, 2);
	PutBytes(KEmulatedClockOffset,2);

	WritePacket();	
	}


void CHCEmulator::ConnectionPacketTypeChangedEvent(TUint8 aError, THCIConnHandle aConnH)
	{
	ClearPacket();

	iConsole.Printf(_L("Sending ConnectionPacketTypeChanged Event (error 0x%02x)...\n"), aError);

	PutBytes(KHCIUARTEventHeader);
	PutBytes(KHCIConnectionPacketTypeChangedCode);
	PutBytes(KHCIConnectionPacketTypeChangedLength);
	PutBytes(aError);
	PutBytes(aConnH, 2);
	PutBytes(KEmulatedChangedPacketType,2);

	WritePacket();	
	}


void CHCEmulator::ReadRemoteSupportedFeaturesCompleteEvent(TUint8 aError, THCIConnHandle aConnH)
	{
	ClearPacket();

	iConsole.Printf(_L("Sending ReadRemoteSupportedFeaturesComplete Event (error 0x%02x)...\n"), aError);

	PutBytes(KHCIUARTEventHeader);
	PutBytes(KHCIReadRemoteSupportedFeaturesCode);
	PutBytes(KHCIReadRemoteSupportedFeaturesLength);
	PutBytes(aError);
	PutBytes(aConnH, 2);
	PutBytes(KEmulatedRemoteSupportedFeatures1,4);
	PutBytes(KEmulatedRemoteSupportedFeatures2,4);

	WritePacket();	
	}


void CHCEmulator::ReadRemoteVersionInfoCompleteEvent(TUint8 aError, THCIConnHandle aConnH)
	{
	ClearPacket();

	iConsole.Printf(_L("Sending ReadRemoteVersionInfoComplete Event (error 0x%02x)...\n"), aError);

	PutBytes(KHCIUARTEventHeader);
	PutBytes(KHCIReadRemoteVersionInfoCode);
	PutBytes(KHCIReadRemoteVersionInfoLength);
	PutBytes(aError);
	PutBytes(aConnH, 2);
	PutBytes(KEmulatedRemoteVersionInfo1,4);
	PutBytes(KEmulatedRemoteVersionInfo2,1);

	WritePacket();	
	}

	
void CHCEmulator::ReadRemoteNameCompleteEvent(TUint8 aError, const TBTDevAddr& aAddr, const TDesC8& aName)
	{
	ClearPacket();
	
	iConsole.Printf(_L("Sending C.C.E (opcode ReadRemoteName, error 0x%02x)...\n"), aError);

	PutBytes(KHCIUARTEventHeader);
	PutBytes(KHCIRemoteNameRequestCompleteEventCode);
	PutBytes(KHCIRemoteNameReqCompleteEventPacketLength);
	PutBytes(aError);
	PutAddress(aAddr);
	PutName(aName);
	
	WritePacket();
	}


void CHCEmulator::ReadLocalSupportedFeaturesCompleteEvent(TUint8 aError)
	{	
	iConsole.Printf(_L("Sending C.C.E (opcode ReadLocalSupportedFeatures, error 0x%02x)...\n"), aError);
	BuildAndWriteCommandPacket(ECommandCompleteEvent, KReadLocalSupportedFeaturesOpcode, aError);
	}


void CHCEmulator::ReadLocalVersionInfoCompleteEvent(TUint8 aError)
	{	
	iConsole.Printf(_L("Sending C.C.E (opcode ReadLocalVersionInformation, error 0x%02x)...\n"), aError);
	BuildAndWriteCommandPacket(ECommandCompleteEvent, KReadLocalVersionInfoOpcode, aError);
	}


void CHCEmulator::WriteVoiceSettingCompleteEvent(TUint8 aError)
	{
	iConsole.Printf(_L("Sending C.C.E (opcode WriteVoiceSetting, error 0x%02x)...\n"), aError);
	BuildAndWriteCommandPacket(ECommandCompleteEvent, KWriteVoiceSettingOpcode, aError);	
	}


void CHCEmulator::WriteCurrentIACLAPCompleteEvent(TUint8 aError)
	{
	iConsole.Printf(_L("Sending C.C.E (opcode WriteVoiceSetting, error 0x%02x)...\n"), aError);
	BuildAndWriteCommandPacket(ECommandCompleteEvent, KWriteCurrentIACLAPOpcode, aError);	
	}

	
void CHCEmulator::ReadClassOfDeviceCompleteEvent(TUint8 aError)
	{
	iConsole.Printf(_L("Sending C.C.E (opcode ReadClassOfDevice, error 0x%02x)...\n"), aError);
	BuildAndWriteCommandPacket(ECommandCompleteEvent, KReadClassOfDeviceOpcode, aError);	
	}


void CHCEmulator::VendorSpecificDebugCompleteEvent(TUint16 aOpCode, const TDesC8& aRemainderBuf)
	{
	ClearPacket();
	
	//  Just send back the payload we got in Vendor Specific Command. This is used by
	//  the tproxy HCIProxy test program, to be found in HCIProxy's directory.

	const TUint KFixedParamSize = 3;
	const TUint KMaxUserParamSize  = 255 - KFixedParamSize;
	TUint userParamSize = (aRemainderBuf.Length() < KMaxUserParamSize) ?
						   aRemainderBuf.Length() : KMaxUserParamSize;

	PutBytes(KHCIUARTEventHeader);
	PutBytes(KHCICommandCompleteEventCode);
	PutBytes(KFixedParamSize + userParamSize);
	PutBytes(1);	// numPackets
	PutBytes(aOpCode, 2);
	for (int i = 0; i < userParamSize; i++)
		PutBytes(aRemainderBuf[i]);

	iConsole.Printf(_L("Sending C.C.E (opcode VendorSpecificDebug), user param size %d...\n"),
					userParamSize);
	WritePacket();
	}


void CHCEmulator::ACLDataReceived(const TDesC8& aConnHandleFlagsBuf, const TDesC8& aRemainderBuf)
	{
	// send it back!
	// RFCOMM might need to tweak the data to allow SC n to connect to (say) SC (n+1)
	BuildAndWriteDataPacket(aConnHandleFlagsBuf, aRemainderBuf);

	// send ACL completed packets event

	// test link manager by varying this (e.g. comment it out!)

	THCIConnHandle connh;
	GetConnHandle(connh, aConnHandleFlagsBuf.Ptr());

//	NumberCompletedPacketsEvent(connh+0x0f00, 1, KHCIOK);	//invalid behvaiour
	NumberCompletedPacketsEvent(connh, 1, KHCIOK);	//valid behvaiour
	}


void CHCEmulator::NumberCompletedPacketsEvent(/*const TDesC8& aConnHandleFlagsBuf*/THCIConnHandle aConnH, TUint8 aNumPackets, TUint8 aError)
	{
	ClearPacket();

	iConsole.Printf(_L("Sending NumPacketsComplete Event (error 0x%02x)...\n"), aError);
	
	PutBytes(KHCIUARTEventHeader);
	PutBytes(KHCINumCompletedPacketsEventCode);
	PutBytes(KHCINumCompletedPacketsEventLength);
	PutBytes(0x01);				// FIXME we'll do one at a time for the moment
	PutBytes(aConnH,2);
//	iPacket.Append(aConnHandleFlagsBuf);
	PutBytes(aNumPackets,2);				// FIXME we'll do one at a time for the moment
	
	WritePacket();
	}

	
void CHCEmulator::HardwareErrorEvent()
	{
	ClearPacket();

	TUint8 error = KHCIOK;

	iConsole.Printf(_L("Sending HardwareError Event (error 0x%02x)...\n"), error);

	PutBytes(KHCIUARTEventHeader);
	PutBytes(KHCIHardwareErrorEventCode);
	PutBytes(KHCIHardwareErrorEventLength);	
	PutBytes(error);

	WritePacket();
	}


void CHCEmulator::SpecificVendorDebugEvent()
	{
	TInputManager manager(iConsole);
	
	iConsole.Printf(_L("Data length (0-99): "));
	
	TBuf8<255> str;
	manager.ReadString(str);
	TUint count;
	
	if (manager.String2Uint(str, count) != KErrNone)
		{
		iConsole.Printf(_L("Invalid data length\n"), count);
		return;
		}
	
	iConsole.Printf(_L("Fill in %d bytes\n"), count);
	
	TBuf8<255> data(count);
	TUint value;
	
	for(TUint index=0;index<count;index++)
		{
		iConsole.Printf(_L("byte %d: 0x"), index);
		// TUint value = manager.ReadInt(ETrue);
		manager.ReadString(str);
		if (manager.String2Uint(str, value, ETrue) != KErrNone)
			{
			iConsole.Printf(_L("Invalid byte value (must be hex)\n"), count);
			return;
			}
		data[index] = value;
		}
	
	ClearPacket();
	PutBytes(KHCIUARTEventHeader);
	PutBytes(KHCISpecificVendorDebugEvent);
	PutBytes(count);
	for(TUint index=0;index<count;index++)
		{
		PutBytes(data[index]);
		}
	
	TRAPD(error,iWriter->WriteL(iPacket));

	if (error)
		iConsole.Printf(_L("ERROR - Write failed\n"));
	}


void CHCEmulator::BuildAndWriteDataPacket(const TDesC8& aConnHandleBuf /* THCIConnHandle aConnH*/, const TDesC8& aRemainderBuf)
	{
	ClearPacket();

	PutBytes(KHCIUARTACLDataHeader);
//	PutBytes(aConnH,2);

	iPacket.Append(aConnHandleBuf);
	PutBytes(aRemainderBuf.Length(), 2);
	iPacket.Append(aRemainderBuf);

	TRAPD(error,iWriter->WriteL(iPacket));

	if (error)
		iConsole.Printf(_L("ERROR - Write failed\n"));
	}


void CHCEmulator::BuildAndWriteCommandPacket(TPacketType aType, TUint16 aOpcode, TUint8 aError)
/**
	For 'easy' packets
**/
	{
	iPacket.Zero();

	TUint8 numpackets = 1;					// <--- adjustable

	switch (aType)
		{
		case ECommandStatusEvent:
			{
			PutBytes(KHCIUARTEventHeader);
			PutBytes(KHCICommandStatusEventCode);
			PutBytes(KHCICommandStatusEventLength);
			PutBytes(aError);
			PutBytes(numpackets);
			PutBytes(aOpcode,2);
			break;
			}
		case ECommandCompleteEvent:
			{
			switch (aOpcode)
				{
				case KReadBufferSizeOpcode:
					{
					PutBytes(KHCIUARTEventHeader);
					PutBytes(KHCICommandCompleteEventCode);
					PutBytes(KHCIReadBufferSizeCommandCompleteEventLength);
					PutBytes(numpackets);
					PutBytes(aOpcode,2);
					PutBytes(aError);
					PutBytes(KEmulatedHCSizeACLBuffers,2);
					PutBytes(KEmulatedHCSizeSCOBuffers,1);
					PutBytes(KEmulatedHCNumberACLBuffers,2);
					PutBytes(KEmulatedHCNumberSCOBuffers,2);
					break;
					}
				case KReadBdaddrOpcode:
					{
					PutBytes(KHCIUARTEventHeader);
					PutBytes(KHCICommandCompleteEventCode);
					PutBytes(KHCIReadBdaddrCommandCompleteEventLength);
					PutBytes(numpackets);
					PutBytes(aOpcode,2);
					PutBytes(aError);
					PutAddress(KEmulatedHCAddress);
					break;
					}
				case KReadLocalNameOpcode:
					{
					PutBytes(KHCIUARTEventHeader);
					PutBytes(KHCICommandCompleteEventCode);
					PutBytes(KHCIReadLocalNameCommandCompleteEventLength);
					PutBytes(numpackets);
					PutBytes(aOpcode,2);
					PutBytes(aError);
					PutName(iLocalName);
					break;
					}
				case KResetOpcode:
				case KSetHostControllerToHostFlowOpcode:
				case KWritePageTimeoutOpcode:
				case KHostBufferSizeOpcode:
				case KWriteScanEnableOpcode:
				case KWriteConnectionAcceptTimeoutOpcode:
				case KChangeLocalNameOpcode:
				case KInquiryCancelOpcode:
				case KWriteVoiceSettingOpcode:
				case KWriteCurrentIACLAPOpcode:
				case KSetEventMaskOpcode:
				case KNopOpcode:
					{
					PutBytes(KHCIUARTEventHeader);
					PutBytes(KHCICommandCompleteEventCode);
					PutBytes(KHCIDefaultCommandCompleteEventLength);
					PutBytes(numpackets);
					PutBytes(aOpcode,2);
					PutBytes(aError);
					}
					break;
					
				case KReadLocalSupportedFeaturesOpcode:
					{
					PutBytes(KHCIUARTEventHeader);
					PutBytes(KHCICommandCompleteEventCode);
					PutBytes(KHCIReadLocalSupportedFeaturesLength);
					PutBytes(numpackets);
					PutBytes(aOpcode,2);
					PutBytes(aError);
					PutBytes(KEmulatedLocalSupportedFeatures1,4);
					PutBytes(KEmulatedLocalSupportedFeatures2,4);
					}
					break;
					
				case KReadLocalVersionInfoOpcode:
					{
					PutBytes(KHCIUARTEventHeader);
					PutBytes(KHCICommandCompleteEventCode);
					PutBytes(KHCIReadLocalVersionInformationLength);
					PutBytes(numpackets);
					PutBytes(aOpcode,2);
					PutBytes(aError);
					PutBytes(KEmulatedLocalVersion1,4);
					PutBytes(KEmulatedLocalVersion2,4);
					}
					break;
					
				case KReadClassOfDeviceOpcode:
					{
					PutBytes(KHCIUARTEventHeader);
					PutBytes(KHCICommandCompleteEventCode);
					PutBytes(KHCIReadCoDLength);
					PutBytes(numpackets);
					PutBytes(aOpcode,2);
					PutBytes(aError);
					PutBytes(KEmulatedCoD,3);	
					}
					break;
					
				}
			break;
			}
		}
	WritePacket();
	}


void CHCEmulator::ClearPacket()
	{
	iPacket.Zero();
	}


void CHCEmulator::WritePacket()
	{
	TRAPD(error, iWriter->WriteL(iPacket));
	if (error)
		iConsole.Printf(_L("ERROR - Write failed\n"));
	}


void CHCEmulator::CommandReceived(const TDesC8& aOpcodeBuf, const TDesC8& aRemainderBuf)
	{
	iParsePtr = 0;
	Decode(aOpcodeBuf, aRemainderBuf);
	}


void CHCEmulator::ResetEmulator()
	{
	iConnections.Reset();
	}


TBool CHCEmulator::Decode(const TDesC8& aOpcodeBuf, const TDesC8& aRemainderBuf)
	{
	TUint16 opcode = static_cast<TUint16>((aOpcodeBuf[1] << 8) + aOpcodeBuf[0]);

	TBuf<60> command;
	
	// Separate if to catch all Vendor Specific opcodes.
	if ((opcode & KOGFMask) == KVendorDebugOGF)
		{
		command = _L("VendorSpecificDebug");
		const TUint16 KOCFMask = KMaxTUint16 & ~KOGFMask;
		if((opcode &  KOCFMask) != 0) //OCF = 0 is used by tproxy to indicate that we do not want a reply
			{
			VendorSpecificDebugCompleteEvent(opcode, aRemainderBuf);
			}
		else
			{
			DefaultCommandCompleteEvent(KNopOpcode, KHCIOK);
			}
		}
	else
		{
		switch (opcode)
			{
			case KResetOpcode:
				command = _L("Reset");
	
				ResetEmulator();
	
				DefaultCommandCompleteEvent(KResetOpcode, KHCIOK);
				CommandStatusEvent(KNoOpcode, KHCIOK);	// for CSR
				break;
			case KSetHostControllerToHostFlowOpcode:
				command = _L("SetHostControllerToHostFlow");
				DefaultCommandCompleteEvent(KSetHostControllerToHostFlowOpcode, KHCIOK);
				break;
			case KWritePageTimeoutOpcode:
				command = _L("WritePageTimeout");
				DefaultCommandCompleteEvent(KWritePageTimeoutOpcode, KHCIOK);
				break;
			case KHostBufferSizeOpcode:
				command = _L("HostBufferSize");
				DefaultCommandCompleteEvent(KHostBufferSizeOpcode, KHCIOK);
				break;
			case KWriteScanEnableOpcode:
				command = _L("WriteScanEnable");
				DefaultCommandCompleteEvent(KWriteScanEnableOpcode, KHCIOK);
				break;
			case KWriteConnectionAcceptTimeoutOpcode:
				command = _L("WriteConnectionAcceptTimeout");
				DefaultCommandCompleteEvent(KWriteConnectionAcceptTimeoutOpcode, KHCIOK);
				break;
			case KChangeLocalNameOpcode:
				command = _L("ChangeLocalName");
				iLocalName.SetLength(248);
				iLocalName.Replace(0, aRemainderBuf.Length(), aRemainderBuf);
				DefaultCommandCompleteEvent(KChangeLocalNameOpcode, KHCIOK);
				break;
			case KReadBdaddrOpcode:
				command = _L("ReadBDAddr");
				ReadBDAddrCompleteEvent(KHCIOK);
				break;
			case KReadLocalNameOpcode:
				command = _L("ReadLocalName");
				ReadLocalNameCompleteEvent(KHCIOK);
				break;
			case KReadBufferSizeOpcode:
				command = _L("ReadBufferSize");
				ReadBufferSizeCompleteEvent(KHCIOK);
				break;
			case KCreateACLConnectionOpcode:
				{
				// decode this and store in emulated connections
				command = _L("CreateConnection");
				CommandStatusEvent(KCreateACLConnectionOpcode, KHCIOK);
	
				TBTDevAddr remoteaddr;
				GetBigEndianDevAddr(remoteaddr, aRemainderBuf.Ptr());
	#ifdef _CONNECT_TYPE_1
				ConnectionRequestEvent(remoteaddr, KEmulatedInquiryClassOfDevice, EACLLink);
	#else
				TBTConnect newConnection;
				
				//GetBigEndianDevAddr(newConnection.iBdaddr, aRemainderBuf.Ptr());
	
				newConnection.iBdaddr=remoteaddr;
				newConnection.iLinkType=EACLLink;
				newConnection.iEncryptMode=KEmulatedEncryptMode;
				newConnection.iConnH = static_cast<TUint16>(KEmulatedBaseConnectionHandle + iConnections.Count());
	
				//update connection model - stick in separate class that wraps RArray/Console
				iConnections.Append(newConnection);
				TBuf<30> readableaddr;
				newConnection.iBdaddr.GetReadable(readableaddr);
				iConnectionsConsole->Printf(_L("Connection added: Handle 0x%04x, Remote address %S, Type ACL\n"), newConnection.iConnH, &readableaddr);
	
				ConnectionCompleteEvent(KHCIOK, EACLLink);
	#endif
				// CommandStatusEvent(KCreateACLConnectionOpcode, KHCICommandDisallowed); 
				}
				break;
	
			case KAcceptConnectionRequestOpcode:
				{
				TBTConnect newConnection;
				
				//GetBigEndianDevAddr(newConnection.iBdaddr, aRemainderBuf.Ptr());
	
				newConnection.iLinkType=EACLLink;
				newConnection.iEncryptMode=KEmulatedEncryptMode;
				newConnection.iConnH = static_cast<TUint16>(KEmulatedBaseConnectionHandle + iConnections.Count());
	
				//update connection model - stick in separate class that wraps RArray/Console
				iConnections.Append(newConnection);
				TBuf<30> readableaddr;
				newConnection.iBdaddr.GetReadable(readableaddr);
				iConnectionsConsole->Printf(_L("Connection added: Handle 0x%04x, Remote address %S, Type ACL\n"), newConnection.iConnH, &readableaddr);
	
	#ifdef DISCONNECT_ON_ACCEPT
				//This was first used for reproducing DEF074193 
				//we need to send a disconnect to a SCO socket
				//that is in state TSCOLinkStateAccepting.
				DisconnectionCompleteEvent(KHCIOK, 1, KHCINoConnection);
	#else
				ConnectionCompleteEvent(KHCIOK, EACLLink);
	#endif
				}
				break;
	
			case KDisconnectOpcode:
				{
				command = _L("Disconnect");
				CommandStatusEvent(KDisconnectOpcode, KHCIOK);
				THCIConnHandle connh;
				GetConnHandle(connh, aRemainderBuf.Ptr());
				
				TUint8 reason;
				GetByte(reason, aRemainderBuf.Ptr()+2); //+2 or else first byte of connection handle will be read
	
				TInt index = FindConnection(connh);
				if (index!=KErrNotFound)
					{
					iConnections.Remove(index);				// delete from emulator's connections
					iConnectionsConsole->Printf(_L("Connection removed: Handle 0x%04x\n"), connh);
					DisconnectionCompleteEvent(KHCIOK, connh, reason);
					}
				else
					DisconnectionCompleteEvent(KHCIOK, connh, KHCINoConnection);
	
	
				}
				break;
	
			case KInquiryOpcode:
				command = _L("Inquiry");
				CommandStatusEvent(KInquiryOpcode, KHCIOK);
				InquiryStartEvent(KHCIOK);
				break;
	
			case KInquiryCancelOpcode:
				command = _L("InquiryCancel");
				InquiryCancelCompleteEvent(KHCIOK);
				break;
				
			case KRemoteNameRequestOpcode:
				{
				command = _L("RemoteNameRequest");
				CommandStatusEvent(KRemoteNameRequestOpcode, KHCIOK);
				
				TBTDevAddr remoteAddr;
				GetBigEndianDevAddr(remoteAddr, aRemainderBuf.Ptr());
				
				TPtrC8 namePtr;
				TInt err = GetRemoteName(remoteAddr, namePtr);
				
				ReadRemoteNameCompleteEvent(err, remoteAddr, namePtr);
				break;
				}
	
			case KReadLocalSupportedFeaturesOpcode:
				{
				command = _L("ReadLocalSupportedFeatures");			
				ReadLocalSupportedFeaturesCompleteEvent(KHCIOK);
				break;
				}
				
			case KReadLocalVersionInfoOpcode:
				{
				command = _L("ReadLocalVersionInformation");
				ReadLocalVersionInfoCompleteEvent(KHCIOK);
				break;		
				}
				
			case KWriteVoiceSettingOpcode:
				{
				command = _L("WriteVoiceSetting");	
				WriteVoiceSettingCompleteEvent(KHCIOK);
				break;
				}
			
			case KWriteCurrentIACLAPOpcode:
				{
				command = _L("WriteCurrentIACLAP");	
				WriteCurrentIACLAPCompleteEvent(KHCIOK);
				break;	
				}
				
			case KReadClassOfDeviceOpcode:
				{
				command = _L("ReadClassOfDevice");	
				ReadClassOfDeviceCompleteEvent(KHCIOK);
				break;	
				}
		
			case KWriteLinkPolicySettingsOpcode:
				{
				command = _L("WriteLinkPolicySettings");
				CommandStatusEvent(KWriteLinkPolicySettingsOpcode, KHCIOK);
				
				THCIConnHandle connh;
				GetConnHandle(connh, aRemainderBuf.Ptr());
	
				WriteLinkPolicySettingsCompleteEvent(KHCIOK, connh);
				}
				break;
				
			case KReadClockOffsetOpcode:
				{
				command = _L("ReadClockOffset");
				CommandStatusEvent(KReadClockOffsetOpcode, KHCIOK);
						
				THCIConnHandle connh;
				GetConnHandle(connh, aRemainderBuf.Ptr());
	
				ReadClockOffsetCompleteEvent(KHCIOK, connh);		
				}
				break;
			
			case KReadRemoteSupportedFeaturesOpcode:
				{
				command = _L("ReadRemoteSupportedFeatures");
				CommandStatusEvent(KReadRemoteSupportedFeaturesOpcode, KHCIOK);
						
				THCIConnHandle connh;
				GetConnHandle(connh, aRemainderBuf.Ptr());
	
				ReadRemoteSupportedFeaturesCompleteEvent(KHCIOK, connh);	
				}
				break;
			
			case KReadRemoteVersionInfoOpcode:
				{
				command = _L("ReadRemoteVersionInfo");
				CommandStatusEvent(KReadRemoteVersionInfoOpcode, KHCIOK);
						
				THCIConnHandle connh;
				GetConnHandle(connh, aRemainderBuf.Ptr());
	
				ReadRemoteVersionInfoCompleteEvent(KHCIOK, connh);				
				}
				break;
			
			case KChangeConnectionPacketTypeOpcode:
				{
				command = _L("ChangeConnectPacketType");
				CommandStatusEvent(KChangeConnectionPacketTypeOpcode, KHCIOK);
						
				THCIConnHandle connh;
				GetConnHandle(connh, aRemainderBuf.Ptr());
	
				ConnectionPacketTypeChangedEvent(KHCIOK, connh);		
				}
				break;
			case KNopOpcode:
				{
				command = _L("NoOperationOpcode");
				DefaultCommandCompleteEvent(KNopOpcode, KHCIOK);
				}
				break;
			case KSetEventMaskOpcode:
				{
				command = _L("SetEventMaskOpcode");
				DefaultCommandCompleteEvent(KSetEventMaskOpcode, KHCIOK);
				}
				break;
			default:
				command = _L("Unknown");
				break;
			} // switch
		} // opcode OGF is not Vendor Debug
	iConsole.Printf(_L("%S command received - opcode 0x%x\n"), &command, opcode); //FIXME

	return ETrue;
	}


TInt CHCEmulator::GetRemoteName(const TBTDevAddr& aAddr, TPtrC8& aName) const
	{
	TInt index = 0;
	while(index < iInquiryAddresses.Count())
		{
		if (iInquiryAddresses[index] == aAddr)
			{
			aName.Set(iInquiryNames[index]);
			return KHCIOK;
			}
		index++;
		}
		
	aName.Set(KNullDesC8);
	return KHCIPageTimeout;
	}


TInt CHCEmulator::FindConnection(THCIConnHandle aConnH)
	{
	TInt index = 0;
	while (index<iConnections.Count())
		{
		if (iConnections[index].iConnH == aConnH)
			return index;
		else
			++index;
		}
	return KErrNotFound;
	}


void CHCEmulator::PutBytes(TUint aNum, TUint8 aNumOfBytes)
	{
	TInt length = iPacket.Length();
	iPacket.SetLength(length+aNumOfBytes);

    switch (aNumOfBytes)
        {
		// deliberate fallthroughs...
    case 4:
        iPacket[length+3]=TUint8(aNum>>24);
    case 3:
        iPacket[length+2]=TUint8(aNum>>16 & 0xff);
    case 2:
		iPacket[length+1]=TUint8(aNum>>8 & 0xff);
    case 1:
        iPacket[length]=TUint8(aNum & 0xff);
        break;
		}
	}


void CHCEmulator::PutAddress(const TBTDevAddr& aAddress)
	{
	TInt currentLength = iPacket.Length();
	
	iPacket.SetLength(currentLength + KBTMaxDevAddrSize);

    for (TInt i=0;i<KBTMaxDevAddrSize;i++)
		{
        iPacket[currentLength+i]=aAddress[KBTMaxDevAddrSize-1-i];  // little endian-ify
		}
	}


void CHCEmulator::PutName(const TDesC8& aName)
	{
	// The length of a name parameter is 248.  Any bytes not used by
	// the name should be set to zero.
	TInt padding = 248 - aName.Length();
	
	iPacket.Append(aName);
	
	while(padding--)
		{
		iPacket.Append(0x0);
		}
	}


//FIXME make static

void CHCEmulator::GetBigEndianDevAddr(TBTDevAddr& aAddress, const TUint8* aPacket)
// big endian get of dev addr
    {
    TUint8* ptr=const_cast<TUint8*>(aPacket);

    for (TInt i=0; i<KBTMaxDevAddrSize; i++)
        aAddress[KBTMaxDevAddrSize-1-i]=*ptr++;  // reading bdaddr from iEventBufPtr
    
	iParsePtr+=6;
	}


void CHCEmulator::GetByte(TUint8& aByte, const TUint8* aPacket)
	{
//	aByte = const_cast<TUint8>(aPacket[iParsePtr++]);
	aByte = aPacket[iParsePtr++];
	}


void CHCEmulator::GetConnHandle(THCIConnHandle& aConnH, const TUint8* aPacket)
	{
	// ignore the ACL flags
    aConnH = TUint16(((aPacket[1]&~KPacketDataFlagMask)<<8) | aPacket[0]);
//	iParsePtr+=2;
	}


// -------------------------------------------

CInquiryTimer* CInquiryTimer::NewL(CHCEmulator& aEmulator)
	{
	CInquiryTimer* t = new (ELeave) CInquiryTimer(aEmulator);
	CleanupStack::PushL(t);
	t->ConstructL();
	CleanupStack::Pop(t);
	return t;
	}


CInquiryTimer::CInquiryTimer(CHCEmulator& aEmulator) : CTimer(EPriorityStandard), iEmulator(aEmulator)
	{
	}


void CInquiryTimer::Start()
	{
	After(KEmulatedInterDeviceInquiryTime.Int() * 1000000);
	}


void CInquiryTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}


void CInquiryTimer::RunL()
	{
	iEmulator.InquiryResultEvent();
	}


// -------------------------------------------

CActiveReader* CActiveReader::NewL(CConsoleBase& aConsole, CHCEmulator& aEmulator, RComm& aPort)
	{
	CActiveReader* r = new (ELeave) CActiveReader(aConsole, aEmulator, aPort);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop(r);
	return r;
	}


CActiveReader::CActiveReader(CConsoleBase& aConsole, CHCEmulator& aEmulator, RComm& aPort)
: CActive(EPriorityHigh), iConsole(aConsole), iEmulator(aEmulator),
  iPort(aPort), iAuto(ETrue), iReadState(EReadHeader)
	{
	}


CActiveReader::~CActiveReader()
	{
	Cancel();
	delete iReaderOutputConsole;
	}


void CActiveReader::ConstructL()
	{
	User::LeaveIfNull(iReaderOutputConsole = CHCEmulator::AutoSizeNewL(_L("Input"), TSize(60,20)));

	CActiveScheduler::Add(this);
	
	// start reading
	DoRead();
	}


void CActiveReader::DoRead()
	{
#ifdef _EMULATOR_HWFC
	iPort.SetSignals(KSignalRTS,KSignalDTR);		//FIXME
#endif
	switch (iReadState)
		{
		case EReadHeader:
			iPort.Read(iStatus,iHeaderBuf, 1);
			break;

		case EReadOpcode:
			iPort.Read(iStatus,iOpcodeBuf, 2);
			break;

		case EReadCommandLength:
			iPort.Read(iStatus,iCommandLengthBuf, 1);
			break;

		case EReadDataLength:
			iPort.Read(iStatus,iDataLengthBuf, 2);
			break;

		case EReadConnectionHandleFlags:
			iPort.Read(iStatus,iConnectionHandleFlagsBuf, 2);
			break;

		case EReadCommandRemainder:
			{
			const TInt len = iCommandLengthBuf[0];
			iPort.Read(iStatus,iRemainderBuf, len);
			}
			break;

		case EReadDataRemainder:
			{
			const TUint16 len = static_cast<TUint16>((iDataLengthBuf[1] << 8) + iDataLengthBuf[0]);
			iPort.Read(iStatus,iRemainderBuf, len);
			}
			break;

		default:
			__DEBUGGER();
		}


	SetActive();
	}


void CActiveReader::RunL()
	{
	// the read has completed - for the moment we are not really interested at the moment

	if (iAuto)
		{
		switch (iReadState)
			{
			case EReadHeader:
				iReaderOutputConsole->Printf(_L("Header byte recvd\n"));
				iReadState = (DecodeHeader() == KHCIUARTCommandHeader ? EReadOpcode : EReadConnectionHandleFlags);
				break;
			case EReadOpcode:
				{
				iReaderOutputConsole->Printf(_L("Opcode bytes recvd\n"));
				TInt opcode = DecodeOpcode();
				if (opcode == 0)
					//NoOpCommand has no more parameters
					{
					iReadComplete = ETrue;
					iReadState = EReadHeader; // start again
					}
				else
					{
					iReadState = EReadCommandLength;
					}
				}
				break;
			case EReadCommandLength:
				iReaderOutputConsole->Printf(_L("Length byte recvd\n"));
				iReadState = EReadCommandRemainder;
				break;
			case EReadDataLength:
				iReaderOutputConsole->Printf(_L("Length byte recvd\n"));
				iReadState = EReadDataRemainder;
				break;
			case EReadConnectionHandleFlags:
				iReaderOutputConsole->Printf(_L("ConnectionHandle bytes recvd\n"));
				iReadState = EReadDataLength;
				break;
			case EReadCommandRemainder:
			case EReadDataRemainder:
				iReaderOutputConsole->Printf(_L("Remainder bytes recvd\n"));
				iReadComplete = ETrue;
				iReadState = EReadHeader; // start again
				break;

			default:
				__DEBUGGER();
			}
		
		if (iReadComplete)
			{
			iReaderOutputConsole->Printf(_L("Decoding...\n"));

			if (DecodeHeader() == KHCIUARTCommandHeader)
				{
				iReaderOutputConsole->Printf(_L("Decode: Command\n"));
				iEmulator.CommandReceived(iOpcodeBuf, iRemainderBuf);
				}
			else
				{
				iReaderOutputConsole->Printf(_L("Decode: ACL Data\n"));
				iEmulator.ACLDataReceived(iConnectionHandleFlagsBuf, iRemainderBuf);
				}
			iReadComplete = EFalse;
			}
				

		}

	// queue another one
	DoRead();
	}


TInt CActiveReader::DecodeHeader()
	{
	TUint8 header = iHeaderBuf[0];
	return header;
	}


TInt CActiveReader::DecodeOpcode()
	{
	TUint16 opcode = static_cast<TUint16>((iOpcodeBuf[1] << 8) + iOpcodeBuf[0]);
	return opcode;
	}


void CActiveReader::DoCancel()
	{
	iPort.ReadCancel();
	}


// -------------------------------------------

CActiveWriter* CActiveWriter::NewL(CConsoleBase& aConsole, CHCEmulator& aEmulator, RComm& aPort)
	{
	CActiveWriter* w = new (ELeave) CActiveWriter(aConsole, aEmulator, aPort);
	CleanupStack::PushL(w);
	w->ConstructL();
	CleanupStack::Pop(w);
	return w;
	}


CActiveWriter::CActiveWriter(CConsoleBase& aConsole, CHCEmulator& aEmulator, RComm& aPort)
: CActive(EPriorityStandard), iConsole(aConsole), iEmulator(aEmulator), iPort(aPort)
	{
	}


CActiveWriter::~CActiveWriter()
	{
	Cancel();

	iWriteBuffers.ResetAndDestroy();
	iWriteBuffers.Close();
	}


void CActiveWriter::ConstructL()
	{
	CActiveScheduler::Add(this);
	}


void CActiveWriter::DoWrite()
	{
	TPtr8 currentbuffer = (iWriteBuffers[0])->Des();

	iPort.Write(iStatus,currentbuffer);
	SetActive();

	// let other side try to send - this handshaking is a pain!
#ifdef _EMULATOR_HWFC
	iPort.SetSignals(KSignalCTS,KSignalDSR);		//FIXME
#endif
	}


void CActiveWriter::WriteL(const TDesC8& aPacket)
	{
	HBufC8* newbuf = HBufC8::NewL(KWriteBufSize);
	newbuf->Des() = aPacket;
	iWriteBuffers.Append(newbuf);
	if (IsActive())
		return; //try again later

	DoWrite();
	}


void CActiveWriter::RunL()
	{
	TPtr8 currentbuffer = (iWriteBuffers[0])->Des();

	iConsole.Printf(_L("Write of %d bytes (including header) returned result %d.\n"), currentbuffer.Length(), iStatus.Int());
	
	// the write has completed - clear up the buffer
	delete iWriteBuffers[0];
	iWriteBuffers.Remove(0);

	if (iWriteBuffers.Count())
		// more to send
		DoWrite();
	}


void CActiveWriter::DoCancel()
	{
	iPort.WriteCancel();
	}



// ----------------------------------------------------------------------------
//							Active Console
// ----------------------------------------------------------------------------


CActiveConsole::CActiveConsole(CConsoleBase& aConsole, CHCEmulator& aEmulator)
: CActive(EPriorityStandard), iConsole(aConsole), iEmulator(aEmulator)
	{
	}


CActiveConsole::~CActiveConsole()
	{
	// must cancel
	Cancel();
	}


CActiveConsole* CActiveConsole::NewL(CConsoleBase& aConsole, CHCEmulator& aEmulator)
	{
	CActiveConsole* console = new (ELeave) CActiveConsole(aConsole, aEmulator);
	CleanupStack::PushL(console);
	console->ConstructL();
	CleanupStack::Pop();
	return console;
	}


void CActiveConsole::ConstructL()
	{
	CActiveScheduler::Add(this);
	TestMenu();
	}


void CActiveConsole::DoCancel()
	{
	iConsole.ReadCancel();
	}


void CActiveConsole::RequestKey()
	{
	DrawCursor();
	iConsole.Read(iStatus);
	SetActive();
	}


void CActiveConsole::DrawCursor()
	{
	iConsole.Printf(_L(">>"));
	}


void CActiveConsole::TestMenu()
	{
	iConsole.Printf(_L("Choose action...\n\n"));
	iConsole.Printf(_L("s.\t\tCommand status event (connect, error)\n"));
	iConsole.Printf(_L("n.\t\tCommand Status event (NOP, no error) - CSR-reset\n"));
	iConsole.Printf(_L("p.\t\tWritePageTimeout complete vent (no error)\n"));
	iConsole.Printf(_L("r.\t\tReset Complete event (no error)\n"));
	iConsole.Printf(_L("b.\t\tReadBufferSize Complete event (no error)\n"));
	iConsole.Printf(_L("B.\t\tHostBufferSize Complete event (no error)\n"));
	iConsole.Printf(_L("a.\t\tReadBdAddr Complete event (no error)\n"));
	iConsole.Printf(_L("A.\t\tReadLocalName Complete event (no error)\n"));
	iConsole.Printf(_L("e.\t\tWriteScanEnable Complete event (no error)\n"));
	iConsole.Printf(_L("c.\t\tWriteConnectionAcceptTimeout complete event (no error)\n"));
	iConsole.Printf(_L("h.\t\tHostToHostControllerFC Complete Event (no error)\n"));
	iConsole.Printf(_L("l.\t\tSetLocalName Complete Event (no error)\n"));
	iConsole.Printf(_L("i.\t\tStartInquiry Complete Event (no error)\n"));
	iConsole.Printf(_L("I.\t\tInquiryComplete Event (no error)\n"));
	iConsole.Printf(_L("@.\t\tInquiryResultComplete Event (no error)\n"));
	iConsole.Printf(_L("C.\t\tConnectionCompleteEvent (no error)\n"));
	iConsole.Printf(_L("d.\t\tDisconnectionCompleteEvent (no error)\n"));
	iConsole.Printf(_L("k.\t\tConnectionRequestEvent (no error)\n"));
	iConsole.Printf(_L("K.\t\tConnectionRequestEvent (NULL CoD, no error)\n"));
	iConsole.Printf(_L("X.\t\tHardware Error Event (error 0)\n"));
	iConsole.Printf(_L("V.\t\tVendor Specific Debug Event\n"));
	iConsole.Printf(_L("Esc.\tStop\n"));
	RequestKey();
	}


void CActiveConsole::RunL()
	{
	// key has been pressed
	TChar ch = iConsole.KeyCode();

	TUint8 error = KHCIOK;
	TUint16 opcode;

	TBTDevAddr remoteaddr(MAKE_TINT64(0x0002, 0x5bff0a1e)); // an example addr
	TBTDevAddr remoteaddr2(MAKE_TINT64(0x0002, 0x5bff0a1e)); // an example addr

	// Change the errors for interesting tests of stack behaviour

	switch (ch)
		{
	case 's':
		error = KHCICommandDisallowed;
		opcode = KCreateACLConnectionOpcode;
		iEmulator.CommandStatusEvent(opcode, error);
		break;

	case 'n':									
		opcode = KNoOpcode;
		iEmulator.CommandStatusEvent(opcode, error);
		break;

	case 'a':
		opcode = KReadBdaddrOpcode;
		iEmulator.ReadBDAddrCompleteEvent(error);
		break;
		
	case 'A':
		opcode = KReadLocalNameOpcode;
		iEmulator.ReadLocalNameCompleteEvent(error);
		break;

	case 'b':
		opcode = KReadBufferSizeOpcode;
		iEmulator.ReadBufferSizeCompleteEvent(error);
		break;

	case 'B':
		opcode = KHostBufferSizeOpcode;
		iEmulator.DefaultCommandCompleteEvent(opcode, error);
		break;

	case 'p':
		opcode = KWritePageTimeoutOpcode;
		iEmulator.DefaultCommandCompleteEvent(opcode, error);
		break;

	case 'e':
		opcode = KWriteScanEnableOpcode;
		iEmulator.DefaultCommandCompleteEvent(opcode, error);
		break;

	case 'h':
		opcode = KSetHostControllerToHostFlowOpcode;
		iEmulator.DefaultCommandCompleteEvent(opcode, error);
		break;

	case 'c':
		opcode = KWriteConnectionAcceptTimeoutOpcode;
		iEmulator.DefaultCommandCompleteEvent(opcode, error);
		break;

	case 'r':
		opcode = KResetOpcode;
		iEmulator.DefaultCommandCompleteEvent(opcode, error);
		break;

	case 'l':
		opcode = KChangeLocalNameOpcode;
		iEmulator.DefaultCommandCompleteEvent(opcode, error);
		break;

	case 'i':
		opcode = KInquiryOpcode;
		iEmulator.DefaultCommandCompleteEvent(opcode, error);
		break;

	case 'I':
		iEmulator.InquiryCompleteEvent(error);
		break;

	case 'C':
		iEmulator.ConnectionCompleteEvent(error, EACLLink);
		break;

	case 'd':
		iEmulator.DisconnectionCompleteEvent(error, KEmulatedBaseConnectionHandle, KHCIEmulatedDisconnectionReason);
		break;
	case 'k':
		iEmulator.ConnectionRequestEvent(remoteaddr, KEmulatedInquiryClassOfDevice, EACLLink);
		break;

	case 'K':
		iEmulator.ConnectionRequestEvent(remoteaddr2, KEmulatedInquiryNullClassOfDevice, EACLLink);
		break;		
	
	case 'x':
	case 'X':
		iEmulator.HardwareErrorEvent();
		break;

	case 'V':
	case 'v':
		iEmulator.SpecificVendorDebugEvent();
		break;
	case '1':
		//First used for reproducing DEF074193
		//Sends a SCO connection request. A DisconnectComplete event will 
		//be sent in response to the resulting 
		//HCI_Accept_Connection_Request command.
		iEmulator.ConnectionRequestEvent(KDeviceAddresses[0], KEmulatedInquiryNullClassOfDevice, ESCOLink);
		break;		
		
	case EKeyEscape:
		{
		iEmulator.Stop();
		goto end;
		}


	default:
		iConsole.Printf(_L("Unknown command\r\n"));
		}

	// schedule another key press
	RequestKey();

end:
	return;
	}


// -----------------------------------------------

TInputManager::TInputManager(CConsoleBase& aConsole) : 
	iConsole(aConsole)
	{
	}


void TInputManager::ConsoleBackspace(TUint aNumberOfChars)
	{
	TPoint cur = iConsole.CursorPos();
	cur.iX -= aNumberOfChars;
	iConsole.SetCursorPosAbs(cur);
	iConsole.ClearToEndOfLine();
	}


TUint TInputManager::ReadString(TDes8& aStr)
	{
	TKeyCode key;
	
	aStr.Zero();
	
	while( (key = iConsole.Getch()) != EKeyEnter)
		{
		if (aStr.Length() == aStr.MaxLength())
			return aStr.MaxLength();
		
		if (key == EKeyBackspace)
			{
			if (aStr.Length() > 0)
				{
				aStr.Delete(aStr.Length()-1, 1);
				ConsoleBackspace(1);
				}
			}
		else
			{
			TUint8 keyChar(key);
			aStr.Append(keyChar);
			iConsole.Printf(_L("%c"), keyChar);
			}
		}
	iConsole.Printf(_L("\n"));
	return aStr.Length();
	}


TUint TInputManager::GetCipher(TChar aChar)
	{
	TUint value;
	switch(aChar)
		{
		case 'a': value = 10; break;
		case 'b': value = 11; break;
		case 'c': value = 12; break;
		case 'd': value = 13; break;
		case 'e': value = 14; break;
		case 'f': value = 15; break;
		default: value = aChar.GetNumericValue(); 
		}
	return value;
	}


TInt TInputManager::String2Uint(TDes8& aStr, TUint& aResult, TBool aHex)
	{
		TInt result = 0;
		for(TUint index=0;index<aStr.Length();index++)
			{
			TChar keyChar(aStr[index]);
			TBool isDigit = (aHex ? keyChar.IsHexDigit() : keyChar.IsDigit());
			if (!isDigit)
				return KErrArgument;
			TUint mult = aHex ? 16 : 10;
			result = result * mult + GetCipher(keyChar);
			}
		aResult = result;
		return KErrNone;
	}


// -----------------------------------------------

void StartL()
	{
	CHCEmulator* emulator= CHCEmulator::NewL(*test.Console());

	emulator->Start();

	delete emulator;
	//finished
	}


void LoadLDD_PDD()
	{
	TInt r;
	r=StartC32();
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		test.Printf(_L("Failed %d!\n\r"),r);
		test(r==KErrNone);
		}
	else
		test.Printf(_L("Started C32\n"));
	test.Printf(_L("Loading PDD\n"));
	r=User::LoadPhysicalDevice(PDD_NAME);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		test.Printf(_L("Failed %d!\n\r"),r);
		test(r==KErrNone);
		}
	else 
		test.Printf(_L("Loaded LDD\n"));
	test.Printf(_L("Loading LDD\n"));
	r=User::LoadLogicalDevice(LDD_NAME);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		test.Printf(_L("Failed %d!\n\r"),r);
		test(r==KErrNone);
		}
	else
		test.Printf(_L("Loaded PDD\n"));
	}


TInt E32Main()
	{
	test.SetLogged(EFalse);
	test.Title();

// For some reason, you have to do the following to
// ensure that the file server behaves properly.
	RFs fs;
	fs.Connect();
	fs.Close();

	LoadLDD_PDD();

	__UHEAP_MARK;
	CTrapCleanup* cleanupStack=CTrapCleanup::New();	// Get CleanupStack

	CActiveScheduler* activescheduler=new CActiveScheduler;
	CActiveScheduler::Install(activescheduler);

	TRAPD(err, StartL());
	if (err)
		{
		test.Printf(_L("StartL failed with error: %d!\n"), err);
		}
		
	delete activescheduler;
	delete cleanupStack;

	__UHEAP_MARKEND;

	return 0; // and return
	}

