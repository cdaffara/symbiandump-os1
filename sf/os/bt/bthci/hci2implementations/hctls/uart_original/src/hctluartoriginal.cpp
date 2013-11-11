// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#include "hctluartoriginal.h"

#include "hctluartoriginalsender.h"
#include "hctluartoriginalreceiver.h"
#include "controllermanager.h"

#include "uartoriginalutils.h"

#include <bluetooth/hci/hctleventobserver.h>
#include <bluetooth/hci/hctldataobserver.h>
#include <bluetooth/hci/hctlchannelobserver.h>
#include <bluetooth/logger.h>


#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_HCTL_UART_ORIGINAL);
#endif

CHCTLUartOriginal::CHCTLUartOriginal()
	{
	LOG_FUNC
	}

CHCTLUartOriginal::~CHCTLUartOriginal()
	{
	LOG_FUNC
	HCI_LOG_UNLOAD(this);
	delete iReceiver;
	delete iSender; 
	delete iControllerMan;
	}

CHCTLUartOriginal* CHCTLUartOriginal::NewL()
	{
	LOG_STATIC_FUNC

	CHCTLUartOriginal* self = new(ELeave) CHCTLUartOriginal();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CHCTLUartOriginal::ConstructL()
	{
	LOG_FUNC
	HCI_LOG_LOADL(this, KHCILoggerDatalinkTypeH4);

	// Initialises iSender and iReceiver via the PortOpenedL method.
	BaseConstructL(KIniFileName());

	iControllerMan = CControllerManager::NewL(*this);
	}

TAny* CHCTLUartOriginal::Interface(TUid aUid)
	{
	LOG_FUNC
	
	TAny* ret = NULL;
	switch(aUid.iUid)
		{
		case KHCTLInterfaceUid:
			ret = reinterpret_cast<TAny*>(static_cast<MHCTLInterface*>(this));
			break;

		case KHCTLPowerInterfaceUid:
			ret = reinterpret_cast<TAny*>(static_cast<MHCTLPowerInterface*>(iControllerMan));
			break;
         
		case KHCHardResetUid:
			ret = reinterpret_cast<TAny*>(static_cast<MHardResetInitiator*>(this));   
			break;
		default:
			break;
		}

	return ret;
	}

void CHCTLUartOriginal::DoConfigL()
	{
	LOG_FUNC

	Port().ResetBuffers();
	TCommConfig conf;
	Port().Config(conf);
	
	// Get reference to TCommConfig config
	TCommConfigV01& config = conf(); 
	
	// Set Config to 8bit char, 1 stop bit and no parity
	config.iDataBits = EData8;			
	config.iStopBits = EStop1;			
	config.iParity = EParityNone;			
	config.iParityError = KConfigParityErrorIgnore;
	config.iHandshake = KConfigObeyCTS;
	
	LEAVEIFERRORL(Port().SetConfig(conf));

	// allows HC to talk back to us!
	Port().SetSignals(KSignalRTS, KSignalDTEInputs); 
	}

//Implementation of pure virtuals from MHCTLInterface
TInt CHCTLUartOriginal::MhiWriteAclData(const TDesC8& aData)
	{
	LOG_FUNC

	TInt rerr = KErrNotReady;
	// Send if the power is on.
	if(iCurrentPowerState == EBTOn)
		{
		// Add the packet indicator to the first byte of the buffer.
		SetPacketIndicator(EACLDataPacket, aData);
		rerr = iSender->Write(aData);
		HCI_LOG_FRAME_IF_NO_ERROR(rerr, this, KHCILoggerHostToController | KHCILoggerACLDataFrame, aData);
		}
		
	return rerr;
	}

TInt CHCTLUartOriginal::MhiWriteSynchronousData(const TDesC8& aData)
	{
	LOG_FUNC
	
	TInt rerr = KErrNotReady;
	// Send if the power is on.
	if(iCurrentPowerState == EBTOn)
		{
		// Add the packet indicator to the first byte of the buffer.
		SetPacketIndicator(ESynchronousDataPacket, aData);
		rerr = iSender->Write(aData);
		HCI_LOG_FRAME_IF_NO_ERROR(rerr, this, KHCILoggerHostToController | KHCILoggerSynchronousDataFrame, aData);
		}
		
	return rerr;
	}

TInt CHCTLUartOriginal::MhiWriteCommand(const TDesC8& aData)
	{
	LOG_FUNC

	TInt rerr = KErrNotReady;
	// Send if the power is on.
	if(iCurrentPowerState == EBTOn)
		{
		// Add the packet indicator to the first byte of the buffer.
		SetPacketIndicator(ECommandPacket, aData);
		rerr = iSender->Write(aData);
		HCI_LOG_FRAME_IF_NO_ERROR(rerr, this, KHCILoggerHostToController | KHCILoggerCommandOrEvent, aData);
		}
		
	return rerr;
	}
	
void CHCTLUartOriginal::MhiSetQdpPluginInterfaceFinder(MQdpPluginInterfaceFinder& aQdpPluginInterfaceFinder)
	{
	iQdpPluginInterfaceFinder = &aQdpPluginInterfaceFinder;
	}
	
void CHCTLUartOriginal::MhriStartHardReset()
	{
	__ASSERT_DEBUG(iControllerMan, PANIC(KUartOriginalPanic, ENoControllerManager));
	iControllerMan->HardReset();
	}
	
void CHCTLUartOriginal::MhiGetAclDataTransportOverhead(TUint& aHeaderSize, TUint& aTrailerSize) const
	{
	// Return the transport overhead for ACL data.
	aHeaderSize = KHCTLAclDataHeaderSize;
	aTrailerSize = KHCTLAclDataTrailerSize;
	}
	

void CHCTLUartOriginal::MhiGetSynchronousDataTransportOverhead(TUint& aHeaderSize, TUint& aTrailerSize) const
	{
	// Return the transport overhead for Synchronous data.
	aHeaderSize = KHCTLSynchronousDataHeaderSize;
	aTrailerSize = KHCTLSynchronousDataTrailerSize;
	}

void CHCTLUartOriginal::MhiGetCommandTransportOverhead(TUint& aHeaderSize, TUint& aTrailerSize) const
	{
	// Return the transport overhead for HCI commands data.
	aHeaderSize = KHCTLCommandHeaderSize;
	aTrailerSize = KHCTLCommandTrailerSize;
	}


/**
This function is used by the receiver for informing HCI that ACL data has been received
The receiver doesn't have reference to iDataObserver. So this is merely a wrapper for iDataObserver
*/
void CHCTLUartOriginal::ProcessACLData(const TDesC8& aData)
	{
	iDataObserver->MhdoProcessAclData(aData);
	}

/**
This function is used by the receiver for informing HCI that Synchronous data has been received
The receiver doesn't have reference to iDataObserver. So this is merely a wrapper for iDataObserver
*/
void CHCTLUartOriginal::ProcessSynchronousData(const TDesC8& aData)
	{
	iDataObserver->MhdoProcessSynchronousData(aData);
	}

/**
This function is used by the receiver for informing HCI that event has been received
The receiver doesn't have reference to iEventObserver. So this is merely a wrapper for iDataObserver
*/	
void CHCTLUartOriginal::ProcessEvent(const TDesC8& aEvent)
	{
	iEventObserver->MheoProcessEvent(aEvent);
	}

/**
QdpPluginInterfaceFinder getter.

@return returns iQdpPluginInterfaceFinder which could be NULL 
if it has not been given one.
*/
MQdpPluginInterfaceFinder* CHCTLUartOriginal::QdpPluginInterfaceFinder()
	{
	return iQdpPluginInterfaceFinder;
	}
	
/**
Sets the packet type indicator at the begining of the HCI packet.

Hence the HC can recognise whether this packet is a command, ACL/SCO data.
The packet preamble/indication is different between different HCTL 
implementations and are totally dependent upon the meduim used (UART, R2232 etc).
*/
/*static*/ void CHCTLUartOriginal::SetPacketIndicator(THctlPacketType aType, const TDesC8& aData)
	{
	LOG_STATIC_FUNC
	
	TUint8* ptr = const_cast<TUint8*>(aData.Ptr());
	*ptr = static_cast<TUint8>(aType);
	}
	
// Implementation of pure virtual from CHCTLUartBase
void CHCTLUartOriginal::PortOpenedL()
	{
	LOG_FUNC

	__ASSERT_DEBUG(Port().Handle(), PANIC(KUartOriginalPanic, EPortNotOpen));

	if (iSender || iReceiver)
		{
		LEAVEIFERRORL(KErrAlreadyExists);
		}
	
	DoConfigL();
	iCurrentPowerState = EBTOn;	
	// The sender Active Object must be added to the Active Scheduler before 
	// the receiver Active Object so that it gets preferential treatment. It 
	// is reported that otherwise the response from a command can come in 
	// before the sending client is told that the send has completed!
	iSender	= CHCTLUartOriginalSender::NewL(Port());
	iReceiver = CHCTLUartOriginalReceiver::NewL(*this, Port());

	// Start reading from the UART  
	iReceiver->Start();
	}


void CHCTLUartOriginal::MhiSetDataObserver(MHCTLDataObserver& aDataObserver)
	{
	iDataObserver = &aDataObserver;
	}

void CHCTLUartOriginal::MhiSetEventObserver(MHCTLEventObserver& aEventObserver)
	{
	iEventObserver = &aEventObserver;
	}

void CHCTLUartOriginal::MhiSetChannelObserver(MHCTLChannelObserver& aChannelObserver)
	{
	iChannelObserver = &aChannelObserver;
	iSender->SetChannelObserver(aChannelObserver); 
	}

void CHCTLUartOriginal::MhiSetControllerStateObserver(MControllerStateObserver& aControllerStateObserver)
	{
	iControllerStateObserver = &aControllerStateObserver;
	iControllerMan->SetControllerStateObserver(aControllerStateObserver);
	}

void CHCTLUartOriginal::HandlePowerOff()
	{
	iCurrentPowerState = EBTOff;
	// Close all channels and cancel the sender and receiver.
	iSender->Cancel();
	iReceiver->Cancel();

	iChannelObserver->MhcoChannelClosed(KHCITransportAllChannels);
	}

void CHCTLUartOriginal::HandlePowerOn()
	{
	iCurrentPowerState = EBTOn;
	iChannelObserver->MhcoChannelOpen(KHCITransportAllChannels);

	// Clear any spurious data from the buffer.
	Port().ResetBuffers();
	
	iReceiver->Start();
	// The sender will be activated when the first frame needs to be sent.
	}

TBTPowerState CHCTLUartOriginal::CurrentPowerState() const
	{
	return iCurrentPowerState;
	}
