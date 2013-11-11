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

#ifndef PHYSICALLINKMETRICS_H
#define PHYSICALLINKMETRICS_H

#include <bt_sock.h>
#include <bluetooth/hcicommandqueueclient.h>
#include <bluetooth/hcicmdqcontroller.h>

#include "internaltypes.h"
#include "notification.h"

class CPhysicalLink;
class CBTProxySAP;

class THCICommandCompleteEvent;
class TReadFailedContactCounterCompleteEvent;
class TReadRSSICompleteEvent;
class TReadLinkQualityCompleteEvent;
class TReadTransmitPowerLevelCompleteEvent;

const TUint KPLMPollInterval = 2000000; // 2 Seconds


enum THciCommandState
	{
	ECommandIdle,
	ECommandIssued,
	ECommandBlockedForTimer
	};

class TPLMManager
	{
public:
	TPLMManager();
	void RegisterProxySAP(CBTProxySAP& aSAP);	
	void UpdatePLMValue(TInt aPLMValue);
	void UpdatePLMState(THciCommandState aPLMState);
	TPckgBuf<TInt>& PLMValue();
	THciCommandState PLMState() const;
	void NotifyQueuedProxySAPs(TInt aErr, TUint aName, TDesC8* aBuf);
	TBool AnyOutstandingClientRequest() const;

private:	
	TDblQue<CBTProxySAP>	iPLMQue;
	THciCommandState		iPLMState;
	TPckgBuf<TInt>			iPLMValue;
	};

/**
	This class handles CPhysicalLinkMetrics commands to and from the HCI
	It is owned, constructed and destructed by CPhysicalLink
**/
NONSHARABLE_CLASS(CPhysicalLinkMetrics) : public CBase, public MHCICommandQueueClient,
										  public MPhysicalLinkObserver
	{
public:
	static CPhysicalLinkMetrics* NewL(CPhysicalLink& aLink, MHCICommandQueue& aController);
	~CPhysicalLinkMetrics();
	void ReadNewPhysicalLinkMetricValue(TUint aIoctlName, CBTProxySAP& aSAP, TInt aCurrentValue);		
	
private:
	CPhysicalLinkMetrics(CPhysicalLink& aLink, MHCICommandQueue& aController);
	void ConstructL();
	void HandleReadFailedContactCounterCompleteEvent(const TReadFailedContactCounterCompleteEvent& aEvent);
	void HandleReadLinkQualityCompleteEvent(const TReadLinkQualityCompleteEvent& aEvent);
	void HandleReadRssiCompleteEvent(const TReadRSSICompleteEvent& aEvent);
	void HandleReadTransmitPowerLevelCompleteEvent(const TReadTransmitPowerLevelCompleteEvent& aEvent);
	
	void ReadRssiCommand();
	void ReadRssiCommandL();
	void ReadLinkQualityCommand();
	void ReadLinkQualityCommandL();
	void ReadFailedContactCounterCommand();
	void ReadFailedContactCounterCommandL();
	void ReadTransmitPowerLevelCommand();
	void ReadTransmitPowerLevelCommandL();
	
	void RemovePLMCommands();	
	void CommandCompleteEvent(const THCICommandCompleteEvent& aEvent);
	
	static TInt PLMEventReceived(TAny* aThis);
	void DoRssiTimerEvent();
	void DoLinkQualityTimerEvent();
	void DoFailedContactCounterTimerEvent();
	void DoTransmitPowerLevelTimerEvent();
	
	void QueueNextPLMPollIfNotAlreadyQueued();
	void RemovePLMPoll();	

	// From MHCICommandQueueClient
	virtual void MhcqcCommandEventReceived(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	virtual void MhcqcCommandErrored(TInt aErrorCode, const CHCICommandBase* aCommand);

	// From MPhysicalLinkObserver
	virtual void PhysicalLinkChange(const TBTBasebandEventNotification& aEvent, CPhysicalLink& aPhysicalLink);
	virtual TPhysicalLinkObserverQLink& ObserverQLink();
	
private:
	CPhysicalLink&			iParent;
	MHCICommandQueue&		iCmdController;
	
	TPhysicalLinkObserverQLink		iQueLink;
	
	TPckgBuf<TInt>			iDummyResult;
	
	TPLMManager				iRssi;
	TPLMManager				iLinkQuality;
	TPLMManager				iTransmitPowerLevel;
	TPLMManager				iFailedContactCounter;
 
	TDeltaTimerEntry		iPLMTimer;	
	TBool					iPLMTimerQueued;
	};

#endif //PHYSICALLINKMETRICS_H
