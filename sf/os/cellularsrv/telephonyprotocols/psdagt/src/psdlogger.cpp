// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// PSD-specific logging class
// 
//

/**
 @file PsdLogger.cpp
*/

#include "psdlogger.h"
#include "psdagt.h"
#include <comms-infras/cagentsmbase.h>
#include <logengevents.h>

CDataLogger* CDataLogger::NewL(MPsdEnv* aPsdSM,MAgentNotify* aObserver)
/**
Constructing an object of class CDataLogger, pushing it into the clean up stack and popping it.

@return a pointer to class CDataLogger.
*/
	{
	CDataLogger* log = new (ELeave) CDataLogger(aPsdSM,aObserver);
	CleanupStack::PushL(log);
	log->ConstructL();
	CleanupStack::Pop();
	return log;
	}

CDataLogger::CDataLogger(MPsdEnv* aPsdSM,MAgentNotify* aObserver)
: iSM(aPsdSM), iSMObserver(aObserver)
/**
Constructor
*/
	{
	}

void CDataLogger::ConstructL()
	{
	iRegularUpdater = CPeriodic::NewL(EPriorityLow);
	}

CDataLogger::~CDataLogger()
	{
	delete iRegularUpdater;
	}

void CDataLogger::Start()
/**
This must not log anything immediately because the asynchronous log start will probably
still be outstanding (started by the Open state) we must therefore wait for this start to complete
we will therefore wait for the time KRegularUpdateInterval before commencing logging otherwise
the log will not be open once we begin writing to it.

Assumption: log start takes less time than KRegularUpdateInterval.
We use the Active-Objectness of CDataLogger to cope with the asynchronous logging of
data transferred, with the same assumption as above.
*/
	{
	__ASSERT_DEBUG(iSM,User::Invariant());

	TCallBack callbackfn(CDataLogger::Update, this);
	iRegularUpdater->Start(KRegularUpdateInterval,KRegularUpdateInterval,callbackfn);
	}

void CDataLogger::StopL()
/**
No longer an active object
LogDataUpdateEvent will put the request in a queue
and process when available
*/
  	{
		iRegularUpdater->Cancel();
	
		TInt ret=KErrNone;
		if(iSM->Config().TsySupportsDataTransferInfoL(iSM->PacketNetwork()))
			{
			ret = iSM->Context().GetDataVolumeTransferred(iVolumeTransferred);
			}
		else	// retrieve last values from PPP
			{
			TPckg<RPacketContext::TDataVolume> dataPackage(iVolumeTransferred);
			ret = iSMObserver->Notification(EAgentToNifEventTypeGetDataTransfer,&dataPackage);
			}
		if (ret==KErrNone)
			LogData();
	}	

TInt CDataLogger::Update(TAny* aContext)
/**
This function is called every minute
*/
	{
	CDataLogger* datalogger = REINTERPRET_CAST(CDataLogger*,aContext);
	TRAPD(ret,datalogger->RegularUpdateL());
	return ret;
	}

void CDataLogger::RegularUpdateL()
/**
Only logs if there has been change.
*/
	{

	__ASSERT_DEBUG(iSM,User::Invariant());
	__ASSERT_DEBUG(iSMObserver,User::Invariant());

	RPacketContext::TDataVolume newVolume;
	if(iSM->Config().TsySupportsDataTransferInfoL(iSM->PacketNetwork()))
		// does the phone support counting the data volume transfer?
		{
		User::LeaveIfError(iSM->Context().GetDataVolumeTransferred(newVolume));
		}
	else	// if not request values from PPP instead
		{
		TPckg<RPacketContext::TDataVolume> dataPackage(newVolume);
		User::LeaveIfError(iSMObserver->Notification(EAgentToNifEventTypeGetDataTransfer,&dataPackage));
		}
	
	if (IsVolumeChanged(newVolume))
		{
		iVolumeTransferred = newVolume;
		LogData();
		}
	}

void CDataLogger::LogData() 
	{
	TInt64 sent = MAKE_TINT64(iVolumeTransferred.iOverflowCounterSent,iVolumeTransferred.iBytesSent);
	TInt64 recvd = MAKE_TINT64(iVolumeTransferred.iOverflowCounterReceived,iVolumeTransferred.iBytesReceived);
	iSM->Logger()->LogDataUpdateEvent(CEventLogger::KConnectionStatusIdNotAvailable, KLogPacketDataEventTypeUid, sent,recvd);
	}

TBool CDataLogger::IsVolumeChanged(RPacketContext::TDataVolume& aNewVolume) const
	{
	return (aNewVolume.iBytesReceived!=iVolumeTransferred.iBytesReceived ||
				aNewVolume.iBytesSent!=iVolumeTransferred.iBytesSent);
	}

