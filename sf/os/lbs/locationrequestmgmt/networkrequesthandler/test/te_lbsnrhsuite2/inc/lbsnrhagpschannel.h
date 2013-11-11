/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file lbsnrhagpschannel.h
*/
 
#ifndef LBSNRHAGPSCHANNEL_H_
#define LBSNRHAGPSCHANNEL_H_

#include "lbsinternalinterface.h"
#include <lbs/lbsgpsmeasurement.h>


class CTestExecuteLogger;

/* Class that handles messages to and from the A-GPS manager from the NRH
   
   It is an active object because it needs to wait for the
   acknowledgement signal to ensure the message has been
   read by the NRH.
*/
class CAgpsChannel : public CActive
	{
public:
	static CAgpsChannel* NewL(CTestExecuteLogger& aLogger);
	~CAgpsChannel();
	
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
	void WaitForRequestL(TTimeIntervalSeconds aTimeout);
	TLbsPositionUpdateRequestBase::TType GetNextRequestType();
	void GetNextRequestL(TLbsPositionUpdateRequestBase& aMessage);
	
	TInt SendGpsLocationUpdate(
		TInt aReason, 
		const TPositionInfoBase& aPosInfo,
		const TTime& aTargetTime);

	TInt SendGpsMeasurementUpdate(
		TInt aReason,
		const TPositionGpsMeasurementInfo& aMeasurementInfo);
	
private:
	CAgpsChannel(CTestExecuteLogger& aLogger);
	void ConstructL();

	void BufferRequestL(const TLbsPositionUpdateRequestBase& aRequest);
	
	CTestExecuteLogger& Logger() { return iLogger; }
	void LogRequestUpdate(TLbsPositionUpdateRequestBase& aRequest);
	void LogPowerAdviceMode(TLbsPositionUpdateRequestBase::TPowerAdvice aPowerAdvice);
	
private:
	CTestExecuteLogger& iLogger;
	CActiveSchedulerWait iWait;
	RLbsPositionUpdateRequests iAgpsRequestChannel;
	RPointerArray<TLbsPositionUpdateRequestBase> iRequestBuffer;
	
	RLbsPositionUpdates iAgpsPositionUpdateBus;
	RLbsGpsMeasurementUpdates iAgpsMeasurementUpdateBus;
	};

#endif /*LBSNRHAGPSCHANNEL_H_*/
