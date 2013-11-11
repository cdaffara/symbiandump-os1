// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>

#include "physicallinkmetrics.h"
#include "physicallinks.h"
#include "linkutil.h"
#include "ProxySAP.h"
#include "btsockettimer.h"

#include <bluetooth/hci/hciutil.h>
#include <bluetooth/hci/readfailedcontactcountercommand.h>
#include <bluetooth/hci/readlinkqualitycommand.h>
#include <bluetooth/hci/readrssicommand.h>
#include <bluetooth/hci/readtransmitpowerlevelcommand.h>
#include <bluetooth/hci/readrssicompleteevent.h>
#include <bluetooth/hci/readlinkqualitycompleteevent.h>
#include <bluetooth/hci/readfailedcontactcountercompleteevent.h>
#include <bluetooth/hci/readtransmitpowerlevelcompleteevent.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_LINKMGR);
#endif




TPLMManager::TPLMManager()
	:iPLMQue(_FOFF(CBTProxySAP, iPLMLink))
	{
	LOG_FUNC
	}
void TPLMManager::UpdatePLMState(THciCommandState aPLMState)
	{
	LOG_FUNC
	FTRACE(FPrint(_L("TPLMManager::UpdatePLMState, Previous state :: %d\n"), iPLMState));
	iPLMState = aPLMState;
	FTRACE(FPrint(_L("TPLMManager::UpdatePLMState, Changed state :: %d\n"), iPLMState));
	}

void TPLMManager::RegisterProxySAP(CBTProxySAP& aSAP)
	{
	LOG_FUNC
	iPLMQue.AddLast(aSAP);
	}

void TPLMManager::UpdatePLMValue(TInt aPLMValue)
	{
	LOG_FUNC
	FTRACE(FPrint(_L("TPLMManager::UpdatePLMValue, Previous PLM value :: %d\n"), (PLMValue())() ));
	iPLMValue() = aPLMValue;
	FTRACE(FPrint(_L("TPLMManager::UpdatePLMValue, Changed PLM value :: %d\n"), (PLMValue())() ));
	}

TPckgBuf<TInt>& TPLMManager::PLMValue()
	{
	LOG_FUNC
	return iPLMValue;
	}

THciCommandState TPLMManager::PLMState() const
	{
	LOG_FUNC
	return iPLMState;
	}

void TPLMManager::NotifyQueuedProxySAPs(TInt aErr, TUint aName, TDesC8* aBuf)
	{
	LOG_FUNC
	TDblQueIter<CBTProxySAP> iter(iPLMQue);
	CBTProxySAP* thisSAP;
	
	// Iterate through all ProxySAPs that have requested this PLM, notifying them
	while ((thisSAP = iter++) != NULL)
		{
		thisSAP->IoctlComplete(aErr, KSolBtLMProxy, aName, aBuf);
		thisSAP->iPLMLink.Deque();
		}
	}

/*Check if we have any outstanding requests registered by the clients*/
TBool TPLMManager::AnyOutstandingClientRequest() const
	{
	LOG_FUNC
	if(iPLMQue.IsEmpty())
		{
		return EFalse;
		}
	else
		{
		return ETrue;
		}
	}

// ------------------------------------------------------------------------
// class CPhysicalLinkMetrics
// ------------------------------------------------------------------------


CPhysicalLinkMetrics* CPhysicalLinkMetrics::NewL(CPhysicalLink& aLink, MHCICommandQueue& aCmdController)
	{
	LOG_STATIC_FUNC
	
	CPhysicalLinkMetrics* self = new(ELeave) CPhysicalLinkMetrics(aLink, aCmdController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPhysicalLinkMetrics::CPhysicalLinkMetrics(CPhysicalLink& aLink, MHCICommandQueue& aCmdController)
	: iParent(aLink),
	  iCmdController(aCmdController),
	  iQueLink(this)
	{
	TCallBack plmCb(PLMEventReceived, this);
	iPLMTimer.Set(plmCb);
	
	iParent.SubscribeLinkObserver(*this);
	}

void CPhysicalLinkMetrics::ConstructL()
	{
	LOG_FUNC
	}

CPhysicalLinkMetrics::~CPhysicalLinkMetrics()
	{
	RemovePLMCommands();
	}

void CPhysicalLinkMetrics::PhysicalLinkChange(const TBTBasebandEventNotification& aEvent, CPhysicalLink& /* aPhysicalLink */)
	{
	LOG_FUNC;
	if (aEvent.EventType() == ENotifyPhysicalLinkDown || aEvent.EventType() == ENotifyPhysicalLinkError)
		{
		RemovePLMPoll();
		iRssi.NotifyQueuedProxySAPs(KErrDisconnected, KLMReadRssiIoctl, &iDummyResult);
		iLinkQuality.NotifyQueuedProxySAPs(KErrDisconnected, KLMReadLinkQualityIoctl, &iDummyResult);
		iFailedContactCounter.NotifyQueuedProxySAPs(KErrDisconnected, KLMReadFailedContactCounterIoctl, &iDummyResult);
		iTransmitPowerLevel.NotifyQueuedProxySAPs(KErrDisconnected, KLMReadCurrentTransmitPowerLevelIoctl, &iDummyResult);
		}
	}

TPhysicalLinkObserverQLink& CPhysicalLinkMetrics::ObserverQLink()
	{
	return iQueLink;
	}

void CPhysicalLinkMetrics::ReadNewPhysicalLinkMetricValue(TUint aIoctlName, CBTProxySAP& aSAP, TInt aCurrentValue)
	{
	LOG_FUNC;
	switch (aIoctlName)
		{
		case KLMReadRssiIoctl:
			{
			// If the command has not been issued, we issue now
			if (iRssi.PLMState() == ECommandIdle)
				{
				iRssi.UpdatePLMValue(aCurrentValue);
				iRssi.RegisterProxySAP(aSAP);
				//Issue the command and change the state to ECommandIssued
				ReadRssiCommand();
				}
			// Otherwise, if the command is waiting for the timer to expire, and the current value the user has is
			// different to the currently cached value, we complete straight away
			else if (iRssi.PLMState() == ECommandBlockedForTimer && aCurrentValue != iRssi.PLMValue()())
				{
				aSAP.IoctlComplete(KErrNone, KSolBtLMProxy, KLMReadRssiIoctl, &iRssi.PLMValue());
				}
			// Otherwise, if we are waiting for the command to complete, or we are waiting for the timer to expire,
			// and the value the user has is the same as the cached valuewe just queue the SAP.
			else
				{
				iRssi.RegisterProxySAP(aSAP);
				}
			}
			break;
		case KLMReadLinkQualityIoctl:
			{
			// If the command has not been issued, we issue now
			if (iLinkQuality.PLMState() == ECommandIdle)
				{
				iLinkQuality.UpdatePLMValue(aCurrentValue);
				iLinkQuality.RegisterProxySAP(aSAP);
				//Issue the command and change the state to ECommandIssued
				ReadLinkQualityCommand();
				}
			// Otherwise, if the command is waiting for the timer to expire, and the current value the user has is
			// different to the currently cached value, we complete straight away
			else if (iLinkQuality.PLMState() == ECommandBlockedForTimer && aCurrentValue != iLinkQuality.PLMValue()())
				{
				aSAP.IoctlComplete(KErrNone, KSolBtLMProxy, KLMReadLinkQualityIoctl, &iLinkQuality.PLMValue());
				}
			// Otherwise, if we are waiting for the command to complete, or we are waiting for the timer to expire,
			// and the value the user has is the same as the cached valuewe just queue the SAP.
			else
				{
				iLinkQuality.RegisterProxySAP(aSAP);
				}
			}
			break;
		case KLMReadFailedContactCounterIoctl:
			{
			// If the command has not been issued, we issue now
			if (iFailedContactCounter.PLMState() == ECommandIdle)
				{
				iFailedContactCounter.UpdatePLMValue(aCurrentValue);
				iFailedContactCounter.RegisterProxySAP(aSAP);
				//Issue the command and change the state to ECommandIssued
				ReadFailedContactCounterCommand();
				}
			// Otherwise, if the command is waiting for the timer to expire, and the current value the user has is
			// different to the currently cached value, we complete straight away
			else if (iFailedContactCounter.PLMState() == ECommandBlockedForTimer && aCurrentValue != iFailedContactCounter.PLMValue()())
				{
				aSAP.IoctlComplete(KErrNone, KSolBtLMProxy, KLMReadFailedContactCounterIoctl, &iFailedContactCounter.PLMValue());
				}
			// Otherwise, if we are waiting for the command to complete, or we are waiting for the timer to expire,
			// and the value the user has is the same as the cached valuewe just queue the SAP.
			else
				{
				iFailedContactCounter.RegisterProxySAP(aSAP);
				}
			}
			break;
		case KLMReadCurrentTransmitPowerLevelIoctl:
			{
			// If the command has not been issued, we issue now
			if (iTransmitPowerLevel.PLMState() == ECommandIdle)
				{
				iTransmitPowerLevel.UpdatePLMValue(aCurrentValue);
				iTransmitPowerLevel.RegisterProxySAP(aSAP);
				//Issue the command and change the state to ECommandIssued
				ReadTransmitPowerLevelCommand();
				}
			// Otherwise, if the command is waiting for the timer to expire, and the current value the user has is
			// different to the currently cached value, we complete straight away
			else if (iTransmitPowerLevel.PLMState() == ECommandBlockedForTimer && aCurrentValue != iTransmitPowerLevel.PLMValue()())
				{
				aSAP.IoctlComplete(KErrNone, KSolBtLMProxy, KLMReadCurrentTransmitPowerLevelIoctl, &iTransmitPowerLevel.PLMValue());
				}
			// Otherwise, if we are waiting for the command to complete, or we are waiting for the timer to expire,
			// and the value the user has is the same as the cached valuewe just queue the SAP.
			else
				{
				iTransmitPowerLevel.RegisterProxySAP(aSAP);
				}
			}
			break;
		default:
			__ASSERT_DEBUG(EFalse, Panic(EBTProxySAPInvalidIoctl));
		}
	}

void CPhysicalLinkMetrics::ReadRssiCommand()
	{
	LOG_FUNC
	TRAPD(err, ReadRssiCommandL());
	if (err != KErrNone)
		{
		iRssi.NotifyQueuedProxySAPs(err, KLMReadRssiIoctl, &iDummyResult);
		}
	}

void CPhysicalLinkMetrics::ReadRssiCommandL()
	{
	LOG_FUNC
	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	CReadRSSICommand* cmd = CReadRSSICommand::NewL(iParent.Handle());
	iCmdController.MhcqAddCommandL(cmd, *this);
	iRssi.UpdatePLMState(ECommandIssued);
	}

void CPhysicalLinkMetrics::ReadLinkQualityCommand()
	{
	LOG_FUNC
	TRAPD(err, ReadLinkQualityCommandL());
	if (err != KErrNone)
		{
		iLinkQuality.NotifyQueuedProxySAPs(err, KLMReadLinkQualityIoctl, &iDummyResult);
		}
	}

void CPhysicalLinkMetrics::ReadLinkQualityCommandL()
	{
	LOG_FUNC
	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	CReadLinkQualityCommand* cmd = CReadLinkQualityCommand::NewL(iParent.Handle());
	iCmdController.MhcqAddCommandL(cmd, *this);
	iLinkQuality.UpdatePLMState(ECommandIssued);
	}

void CPhysicalLinkMetrics::ReadFailedContactCounterCommand()
	{
	LOG_FUNC
	TRAPD(err, ReadFailedContactCounterCommandL());
	if (err != KErrNone)
		{
		iFailedContactCounter.NotifyQueuedProxySAPs(err, KLMReadFailedContactCounterIoctl, &iDummyResult);
		}
	}

void CPhysicalLinkMetrics::ReadFailedContactCounterCommandL()
	{
	LOG_FUNC
	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	CReadFailedContactCounterCommand* cmd = CReadFailedContactCounterCommand::NewL(iParent.Handle());
	iCmdController.MhcqAddCommandL(cmd, *this);
	iFailedContactCounter.UpdatePLMState(ECommandIssued);
	}

void CPhysicalLinkMetrics::ReadTransmitPowerLevelCommand()
	{
	LOG_FUNC
	TRAPD(err, ReadTransmitPowerLevelCommandL());
	if (err != KErrNone)
		{
		iTransmitPowerLevel.NotifyQueuedProxySAPs(err, KLMReadCurrentTransmitPowerLevelIoctl, &iDummyResult);
		}
	}

void CPhysicalLinkMetrics::ReadTransmitPowerLevelCommandL()
	{
	LOG_FUNC
	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	CReadTransmitPowerLevelCommand* cmd = CReadTransmitPowerLevelCommand::NewL(iParent.Handle(), ECurrentTransmitPowerLevel);
	iCmdController.MhcqAddCommandL(cmd, *this);
	iTransmitPowerLevel.UpdatePLMState(ECommandIssued);
	}

void CPhysicalLinkMetrics::MhcqcCommandErrored(TInt aErrorCode, const CHCICommandBase* aCommand)
	{
	LOG_FUNC
	__ASSERT_DEBUG(aCommand, Panic(EHCIUnknownCommandEvent));   // this should never happen
	LOG2(_L("error code:%d opcode:0x%x"), aErrorCode, aCommand->Opcode());
	switch (aCommand->Opcode())
		{
		case KReadFailedContactCounterOpcode:
			iFailedContactCounter.UpdatePLMState(ECommandIdle);			
			iFailedContactCounter.NotifyQueuedProxySAPs(aErrorCode, KLMReadFailedContactCounterIoctl, &iDummyResult);
			break;
		
		case KReadLinkQualityOpcode:
			iLinkQuality.UpdatePLMState(ECommandIdle);
			iLinkQuality.NotifyQueuedProxySAPs(aErrorCode, KLMReadLinkQualityIoctl, &iDummyResult);
			break;
		
		case KReadRSSIOpcode:
			iRssi.UpdatePLMState(ECommandIdle);
			iRssi.NotifyQueuedProxySAPs(aErrorCode, KLMReadRssiIoctl, &iDummyResult);			
			break;
		
		case KReadTransmitPowerLevelOpcode:
			iTransmitPowerLevel.UpdatePLMState(ECommandIdle);
			iTransmitPowerLevel.NotifyQueuedProxySAPs(aErrorCode, KLMReadCurrentTransmitPowerLevelIoctl, &iDummyResult);						
			break;
			
		default:
			//LOG1(_L("physicallinkmetrics.cpp: Invalid command opcode, expecting only PLM commands but got %d command"),aCommand->Opcode());
			__ASSERT_DEBUG(EFalse, Panic(EHCICommandBadArgument));
		}
	}


// From MHCICommandQueueClient
void CPhysicalLinkMetrics::MhcqcCommandEventReceived(const THCIEventBase& aEvent,
													 const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	switch(aEvent.EventCode())
		{
		case ECommandCompleteEvent:
			{
			const THCICommandCompleteEvent& completeEvent = THCICommandCompleteEvent::Cast(aEvent);
			CommandCompleteEvent(completeEvent);
			break;
			}
		
		default:
			{
			LOG1(_L("Warning!! Unexpected Event Received (event code:%d)"), aEvent.EventCode());
			__ASSERT_DEBUG(EFalse, Panic(EHCIUnexpectedEvent));
			break;
			}
		}	
	}

void CPhysicalLinkMetrics::CommandCompleteEvent(const THCICommandCompleteEvent& aEvent)
	{
	LOG_FUNC
	THCIOpcode opcode = aEvent.CommandOpcode();

	switch (opcode)
		{
		case KReadFailedContactCounterOpcode:
			{
			const TReadFailedContactCounterCompleteEvent& event = TReadFailedContactCounterCompleteEvent::Cast(aEvent);
			HandleReadFailedContactCounterCompleteEvent(event);
			}
			break;
		
		case KReadLinkQualityOpcode:
			{
			const TReadLinkQualityCompleteEvent& event = TReadLinkQualityCompleteEvent::Cast(aEvent);
			HandleReadLinkQualityCompleteEvent(event);
			}
			break;
		
		case KReadRSSIOpcode:
			{
			const TReadRSSICompleteEvent& event = TReadRSSICompleteEvent::Cast(aEvent);
			HandleReadRssiCompleteEvent(event);
			}
			break;
		
		case KReadTransmitPowerLevelOpcode:
			{
			const TReadTransmitPowerLevelCompleteEvent& event = TReadTransmitPowerLevelCompleteEvent::Cast(aEvent);
			HandleReadTransmitPowerLevelCompleteEvent(event);
			}
			break;
		
		default:
			LOG2(_L("Warning: Unexpected Command complete event! Opcode %d error code %d"), opcode, aEvent.ErrorCode());
			__ASSERT_DEBUG(EFalse, Panic(EHCIUnknownCommandCompleteOpcode));
			break;
		}
	}

void CPhysicalLinkMetrics::HandleReadFailedContactCounterCompleteEvent(const TReadFailedContactCounterCompleteEvent& aEvent)
	{
	LOG_FUNC
	TUint8 result = 0;
	TInt err = CHciUtil::SymbianErrorCode(aEvent.ErrorCode());
	if (err == KErrNone)
		{
		result = aEvent.FailedContactCounter();
		}
	
	if (err != KErrNone || static_cast<TInt>(result) != iFailedContactCounter.PLMValue()())
		{
		iFailedContactCounter.UpdatePLMValue(result);
		// Notify all SAPs via IoctlComplete, with the result or an error code
		iFailedContactCounter.NotifyQueuedProxySAPs(err, KLMReadFailedContactCounterIoctl, &iFailedContactCounter.PLMValue());
		}
	iFailedContactCounter.UpdatePLMState(ECommandBlockedForTimer);		
	QueueNextPLMPollIfNotAlreadyQueued();	
	}

void CPhysicalLinkMetrics::HandleReadLinkQualityCompleteEvent(const TReadLinkQualityCompleteEvent& aEvent)
	{
	LOG_FUNC
	TUint8 result = 0;
	TInt err = CHciUtil::SymbianErrorCode(aEvent.ErrorCode());
	if (err == KErrNone)
		{
		result = aEvent.LinkQuality();
		}
	
	// Notify all SAPs via IoctlComplete, with the result or an error code

	if (err != KErrNone || static_cast<TInt>(result) != iLinkQuality.PLMValue()())
		{
		iLinkQuality.UpdatePLMValue(result);
		// Notify all SAPs via IoctlComplete, with the result or an error code
		iLinkQuality.NotifyQueuedProxySAPs(err, KLMReadLinkQualityIoctl, &iLinkQuality.PLMValue());
		}
	iLinkQuality.UpdatePLMState(ECommandBlockedForTimer);
	QueueNextPLMPollIfNotAlreadyQueued();
	}

void CPhysicalLinkMetrics::HandleReadRssiCompleteEvent(const TReadRSSICompleteEvent& aEvent)
	{
	LOG_FUNC
	TUint8 result = 0;
	TInt err = CHciUtil::SymbianErrorCode(aEvent.ErrorCode());
	if (err == KErrNone)
		{
		result = aEvent.RSSI().RSSI();
		}
	
	if (err != KErrNone || static_cast<TInt>(result) != iRssi.PLMValue()())
		{
		iRssi.UpdatePLMValue(result);
		// Notify all SAPs via IoctlComplete, with the result or an error code
		iRssi.NotifyQueuedProxySAPs(err, KLMReadRssiIoctl, &iRssi.PLMValue());
		}
	iRssi.UpdatePLMState(ECommandBlockedForTimer);
	QueueNextPLMPollIfNotAlreadyQueued();
	}

void CPhysicalLinkMetrics::HandleReadTransmitPowerLevelCompleteEvent(const TReadTransmitPowerLevelCompleteEvent& aEvent)
	{
	LOG_FUNC
	TUint8 result = 0;
	TInt err = CHciUtil::SymbianErrorCode(aEvent.ErrorCode());
	if (err == KErrNone)
		{
		result = aEvent.TransmitPowerLevel();
		}
	
	if (err != KErrNone || static_cast<TInt>(result) != iTransmitPowerLevel.PLMValue()())
		{
		iTransmitPowerLevel.UpdatePLMValue(result);
		// Notify all SAPs via IoctlComplete, with the result or an error code
		iTransmitPowerLevel.NotifyQueuedProxySAPs(err, KLMReadCurrentTransmitPowerLevelIoctl, &iTransmitPowerLevel.PLMValue());
		}
	iTransmitPowerLevel.UpdatePLMState(ECommandBlockedForTimer);
	QueueNextPLMPollIfNotAlreadyQueued();
	}

/*static*/ TInt CPhysicalLinkMetrics::PLMEventReceived(TAny* aThis)
	{
	static_cast<CPhysicalLinkMetrics*>(aThis)->iPLMTimerQueued = EFalse;	
	
	/*Send the PLM commands if there are any outstanding requests*/	
	static_cast<CPhysicalLinkMetrics*>(aThis)->DoRssiTimerEvent();
	static_cast<CPhysicalLinkMetrics*>(aThis)->DoLinkQualityTimerEvent();
	static_cast<CPhysicalLinkMetrics*>(aThis)->DoFailedContactCounterTimerEvent();
	static_cast<CPhysicalLinkMetrics*>(aThis)->DoTransmitPowerLevelTimerEvent();
	
	return KErrNone;
	}


void CPhysicalLinkMetrics::DoRssiTimerEvent()
	{
	LOG_FUNC
	iRssi.UpdatePLMState(ECommandIdle);
	if (iRssi.AnyOutstandingClientRequest())
		{
		ReadRssiCommand();
		}
	}

void CPhysicalLinkMetrics::DoLinkQualityTimerEvent()
	{
	LOG_FUNC
	iLinkQuality.UpdatePLMState(ECommandIdle);
	if (iLinkQuality.AnyOutstandingClientRequest())
		{
		ReadLinkQualityCommand();
		}
	}

void CPhysicalLinkMetrics::DoFailedContactCounterTimerEvent()
	{
	LOG_FUNC
	iFailedContactCounter.UpdatePLMState(ECommandIdle);
	if (iFailedContactCounter.AnyOutstandingClientRequest())
		{
		ReadFailedContactCounterCommand();
		}
	}

void CPhysicalLinkMetrics::DoTransmitPowerLevelTimerEvent()
	{
	LOG_FUNC
	iTransmitPowerLevel.UpdatePLMState(ECommandIdle);
	if (iTransmitPowerLevel.AnyOutstandingClientRequest())
		{
		ReadTransmitPowerLevelCommand();
		}
	}

void CPhysicalLinkMetrics::RemovePLMCommands()
	{
	LOG_FUNC
	iCmdController.MhcqRemoveAllCommands(*this);
	}


/* A common timer is used for all the 4 PLMs. Which ever PLM command completes first will start the timer  
 * for the subsequent requests. When the timer completes we will send all the PLM commands (for which there are 
 * outstanding requests) and start the timer again with the first completed PLM command.
 */ 
void CPhysicalLinkMetrics::QueueNextPLMPollIfNotAlreadyQueued()
	{	
	LOG_FUNC
	TUint sniffInterval = iParent.GetSniffInterval();
	if(!iPLMTimerQueued)
		{
		//If we are in sniff mode (i.e. sniff interval is greater than zero) and the sniff interval is greater 
		// then default PLM poll interval then PLM commands should be sent after every sniff interval only  
		if( sniffInterval > KPLMPollInterval)
			{
			BTSocketTimer::Queue(sniffInterval, iPLMTimer);
			}
		else
			{
			BTSocketTimer::Queue(KPLMPollInterval, iPLMTimer);
			}
		iPLMTimerQueued = ETrue;
		}
	}

void CPhysicalLinkMetrics::RemovePLMPoll()
	{
	LOG_FUNC
	//It is safe to call remove even if the timer is already removed.
	BTSocketTimer::Remove(iPLMTimer);
	iPLMTimerQueued = EFalse;
	}

