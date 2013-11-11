// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the interface to Bca & flow control.
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "BcaControllerTraces.h"
#endif

#include <e32uid.h>
#include <nifmbuf.h>
#include <es_ini.h>
#include <e32svr.h>
#include <u32hal.h>
#include "BcaController.h"

//In order not to flow off SPUD everytime we set the default to 1
const TUint KDefaultBufferSize=1;

#ifdef __EABI__
// Patch data is used and KMaxTxIPPacketSize and KMaxRxIPPacketSize can be modified to a different value in RawIpNif.iby file
extern const TInt KMaxSendQueueLen = KDefaultBufferSize;
extern const TInt KMaxTxIPPacketSize = KMaxIPPacket + KIPTagHeaderLength;
extern const TInt KMaxRxIPPacketSize = KMaxIPPacket + KIPTagHeaderLength;
#endif

CBcaController::CBcaController(MControllerObserver& aObserver,
	CBttLogger* aTheLogger)
/**
 * Constructor. 
 */
	: iObserver(aObserver),  
	  iTxFlowControl(EFlowControlOff), 
	  iTxContextActive(ETrue), 
	  iSendState(EIdle),
	  iMaxSendQueueLen(KDefaultBufferSize),
	  iNumPacketsInSendQueue(0)
	{
	iSendQueue.Init();
	}


CBcaController::~CBcaController()
/**
 * Destructor.
 */
	{
	iSendQueue.Free();
	iNumPacketsInSendQueue = 0;

#ifdef RAWIP_HEADER_APPENDED_TO_PACKETS
	delete iIPTagHeader;
#endif // RAWIP_HEADER_APPENDED_TO_PACKETS
	}

void CBcaController::BaseConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_BASECONSTRUCTL_1, "CBcaController::BaseConstructL");
	
    #ifdef RAWIP_HEADER_APPENDED_TO_PACKETS         
	iIPTagHeader = new (ELeave) CIPTagHeader();        
    #endif // RAWIP_HEADER_APPENDED_TO_PACKETS
	
	iIPTagHeader = new (ELeave) CIPTagHeader(iTheLogger);
    
#if defined (__EABI__)
    // Default value for queue length
    iMaxSendQueueLen = KMaxSendQueueLen;
	// Default value for Tx and Rx packet size
	iMaxTxPacketSize = KMaxTxIPPacketSize;
	iMaxRxPacketSize = KMaxRxIPPacketSize;
#else // WINS
	// Set default values in case patch is not present in epocrawip.ini
	iMaxSendQueueLen = KDefaultBufferSize;    
	iMaxTxPacketSize = KMaxIPPacket + KIPTagHeaderLength;
	iMaxRxPacketSize = KMaxIPPacket + KIPTagHeaderLength;
	       
	// for the emulator process is patched via the epocrawip.ini file
	UserSvr::HalFunction(EHalGroupEmulator,EEmulatorHalIntProperty,(TAny*)"rawip_KMaxSendQueueLen",&iMaxSendQueueLen);
	UserSvr::HalFunction(EHalGroupEmulator,EEmulatorHalIntProperty,(TAny*)"rawip_KMaxTxIPPacketSize",&iMaxTxPacketSize);
	UserSvr::HalFunction(EHalGroupEmulator,EEmulatorHalIntProperty,(TAny*)"rawip_KMaxRxIPPacketSize",&iMaxRxPacketSize);
#endif
		OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_BASECONSTRUCTL_2, "RawIp ini file %S not found. Default values will be used.", KRawIpIniFile);
	}

void CBcaController::UpdateInternalFlowFlag(TFlowControl aValue)
/**
 *  Updates Internal Flow flag and resumes the data flow if
 *  necessary.
 *
 * @param aValue the new state of iInternalFlow
 */
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_UPDATEINTERNALFLOWFLAG_1, "CBcaController::UpdateInternalFlowFlag[NewValue=%d, iSendState=%d]",aValue, iSendState);

	if(iTxFlowControl == aValue)
		{
		// C32 Sent the same indication signal twice. Nif will ignore it.
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_UPDATEINTERNALFLOWFLAG_2, "WARNING CBcaController: Received same indication twice");
		return;
		}
	
	// Update the flag value.
	iTxFlowControl = aValue;

	if(iTxFlowControl == EFlowControlOff)
		{
		// If the indication received turned flow control off...
		if (IsTxPossible() && (iSendState == EIdle))
			{
			// ... if the NIF is in the EWaiting state
			// then the data flow can be resumed. 
			ResumeSending();
			}
		}		
	else
		{
		// if the Flow Control is on we can remove all queued write requests
		EmptySendQueue();
		}
	}

void CBcaController::UpdateContextStateFlag(TBool aValue)
/**
 *  Updates Context State flag and resumes the data flow if
 *  necessary.
 *
 * @param aValue the new state of iTxContextState
 */
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_UPDATECONTEXTSTATEFLAG_1, "CBcaController::UpdateContextStateFlag[NewValue=%d, OldValue=%d]",aValue, iTxContextActive);
	
	if(iTxContextActive == aValue)
		{
		return;
		}

	// Update the flag value.
	iTxContextActive = aValue;

	if(iTxContextActive)
		{
		// If the PDP context is active and...
		if (IsTxPossible() && (iSendState == EIdle))
			{
			// ... if the NIF is in the EWaiting state
			// then the data flow can be resumed. 
			ResumeSending();
			}
		}
	else
		{
		// if the PDP context is suspended we can remove all queued write requests
		EmptySendQueue();
		}
	}

TInt CBcaController::Send(RMBufChain& aPdu)
/**
 *  This method is called by CRawIPNifMain in order to send a packet down
 * to the BCA. 
 *
 *	@param aPdu a data packet
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_SEND_1, ">>CBcaController::Send");

	// Check if NIF is shutting down
	if (iSendState == EShuttingDown)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_SEND_2, "    ERROR: Nif is shutting down");
		
		aPdu.Free();

		return KErrDisconnected;
		}
	
	// check that this packet isnt too big - If it is, we dont want to send it or
	// add it to our queue
	if ((aPdu.Length() - aPdu.First()->Length()) > BcaSendBufferLength())
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_SEND_3, "Packet is too large - discarding");
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_SEND_4, "<<CSender::Send -> Error");

		aPdu.Free();
		return KErrArgument;
		}
	
	if (iSendState == ESending)
		// If this happens, it means that TCP/IP has sent us an IP packet
		// while we're still sending the previous one. 
		{
		// check that the queue isnt full NB. this check should not be needed as when the 
		// queue becomes full the IP layer shouldnt send any more packets until it is told to
		if (!IsSendQueueFull())
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_SEND_5, "    Sender busy, appending packet to queue");
			//We know that flow control is off and context isnt suspended so can add to queue
			AppendToSendQueue(aPdu);
			
			return IsSendQueueFull() ? KStopSending : KContinueSending;
			}
			
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_SEND_6, "    Queue is full, upper layer is still sending packets, potential memory problems.");
		AppendToSendQueue(aPdu);
		return KStopSending;
		}

	// If we have got here then a write isnt currently happening
	// We dont need to check flow control is off and context isnt suspended as the BCA always
	// has room for one packet, so send the packet

	if(!IsSendQueueEmpty())
		{
		//make sure that we don't change the order of packets!
		//first send what has already been lined up
		RMBufChain tmpPdu;
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_SEND_7, "    Packet removed from queue to send");
		RemoveFromSendQueue(tmpPdu);
		AppendToSendQueue(aPdu);
		
		// Update module state
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_SEND_8, "     set State to ESending");
		iSendState = ESending;
		
		BcaSend(tmpPdu);
		}
	else
		{
		// Update module state
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_SEND_9, "     set State to ESending");
		iSendState = ESending;
		 
		BcaSend(aPdu);
		}
		
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_SEND_10, "<<CBcaController::Send - return StopSending/ContinueSending");
	return IsSendQueueFull() ? KStopSending : KContinueSending;
	}

void CBcaController::SendComplete()	
/**
 *  This method is called after a packet was sent to the board. 
 *  If allowed by flow contol flags the NIF can signal the TCP/IP
 *  protocol indicating that is available to send more packets.
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_SENDCOMPLETE_1, "CBcaController::SendComplete");
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_SENDCOMPLETE_2, "     set State to EIdle");
	
	iSendState = EIdle;

	if (IsTxPossible())
		ResumeSending();
	}

TBool CBcaController::IsTxPossible()
/**
 *  This method returns ETrue if both TX flags are set to ETrue
 *
 * @return The Flow control state
 */
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_ISTXPOSSIBLE_1, "CBcaController::IsTxPossible (contextActive %d, flowcontrol %d)", iTxContextActive, iTxFlowControl);

	if(iTxContextActive && (iTxFlowControl == EFlowControlOff))
		return ETrue;
	else
		return EFalse;
	}

void CBcaController::Process(TDesC8& aPdu)
/**
 *  This method will pass on the received data to CBttNifMain. 
 *
 * @param aPdu a data packet
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_PROCESS_1, ">>CBcaController::Process");

	TInt ret;

	// Create a packet object.
	RMBufPacket packet;
	TRAP(ret, packet.CreateL(aPdu));
	if (ret != KErrNone)
		{
		// Couldn't create package. Packet will be ignored...
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_PROCESS_2, "<<CBcaController::Process couldn't create MBuf [ret=%d]", ret);
		return;
		}
	else
		// Package created...
		{
#ifdef RAWIP_HEADER_APPENDED_TO_PACKETS
		TUint16 protocolCode = iIPTagHeader->RemoveHeader(packet);
#else
		TUint16 protocolCode = 0;
#endif // RAWIP_HEADER_APPENDED_TO_PACKETS

		packet.Pack();
		// Process the packet
		GetObserver().Process(packet, protocolCode);
		}

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_PROCESS_3, "<<CBcaController::Process");
	}

void CBcaController::ResumeSending()
/**
 *  Used to indicate to the TCP/IP protocol layer that the NIF is ready to 
 *	process more packets.
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_RESUMESENDING_1, "CBcaIoController::ResumeSending");

	// If there are still some packets in the queue to be sent, then carry
	// on sending them.
	// NB. we only want to send more packets from the queue if we are currently EIdle
	if (iSendState == EIdle) 
		{
		if(!IsSendQueueEmpty())
			{
			RMBufChain tmpPdu;
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_RESUMESENDING_2, "    Packet removed from queue to send");
			RemoveFromSendQueue(tmpPdu);
			
			// Update module state
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_RESUMESENDING_3, "     set State to ESending");
			iSendState = ESending;
			
			BcaSend(tmpPdu);
			}
		if(iFullQueueFlag && IsSendQueueEmpty())
			{
			iObserver.ResumeSending();
			}
		}
	// if iSendState = ESending - do nothing
	}
	
#ifdef RAWIP_HEADER_APPENDED_TO_PACKETS
void CBcaController::SetType(TUint16 aType)
	{
/**
 *  Used to specify the type of the IP header.
 */
 	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_SETTYPE_1, "CBcaController::SetType");
 	
 	iIPTagHeader->SetType(aType);	
	}

void CBcaController::AddHeader(TDes8& aDes)
/**
 *  Used to add the IP header to the packet before sending to the BCA.
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_ADDHEADER_1, "CBcaController::AddHeader");

	iIPTagHeader->AddHeader(aDes);
	}

TUint16 CBcaController::RemoveHeader(RMBufChain& aPdu)
/**
 *  Used to remove the IP header from the received the packet before sending to the 
 *  TCP/IP layer.  
 * @return The IP header that has been removed from the packet
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_REMOVEHEADER_1, "CBcaController::RemoveHeader");

	return (iIPTagHeader->RemoveHeader(aPdu));
	}	
#endif // RAWIP_HEADER_APPENDED_TO_PACKETS

TBool CBcaController::IsSendQueueEmpty()
/**
 *  Indicator of whether the BufferQueue is empty
 * @return TBool.  ETrue if bufferQueue is emtpy, EFalse if queue is not empty
 */
	{
	return iSendQueue.IsEmpty();
	}
	
TBool CBcaController::IsSendQueueFull()
/**
 *  Indicator of whether the BufferQueue is full
 * @return TBool.  ETrue if bufferQueue is full, EFalse if queue is not full
 */
	{    
	iFullQueueFlag = iNumPacketsInSendQueue >= iMaxSendQueueLen;
	return iFullQueueFlag;
	}	

void CBcaController::AppendToSendQueue(RMBufChain& aPdu)
/**
 * Appends the packet aPdu to the queue. 
 * Increments the packet count. Doesn't do error checking.
 * @param aChain buffer chain to be added
 */
	{
	iSendQueue.Append(aPdu);
	iNumPacketsInSendQueue++;
	}
	
TBool CBcaController::RemoveFromSendQueue(RMBufChain& aPdu)
/**
 * Removes the packet aPdu from the queue. 
 * Decrements the packet count.
 * @param aChain buffer chain to be added
 * @return False if chain is empty
 */
	{
	TBool ret = iSendQueue.Remove(aPdu);
	if(ret)
		{
		iNumPacketsInSendQueue--;
		}
	return ret;
	}
	
void CBcaController::EmptySendQueue()
/**
 * Removes all the packets from the send queue. Initializes the
 * send queue and sets the packet count to 0.
 */
	{
	iSendQueue.Free();
	iSendQueue.Init();
	iNumPacketsInSendQueue = 0;
	}
