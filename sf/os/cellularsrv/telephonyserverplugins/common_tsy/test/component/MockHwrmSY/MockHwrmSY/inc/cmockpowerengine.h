// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// MockSY engine for the Power plugin
// 
//

#ifndef CMOCKPOWERENGINE_H_
#define CMOCKPOWERENGINE_H_

#include "cmockhwrmpluginengine.h"
#include "tmockhwrmplugindata.h"
#include "cmocksyengine.h"
#include "cmockpowersy.h"


class CMockPowerEngine : public CMockSYEngine
	{	
public:
	virtual void DoQueueEventL(TMockSYEvent::TEventType aType, TInt aCmdId, HBufC8* aData, 
		TInt aResultCode, TBool aLeave, TInt aDelay);
	TInt ExecuteProcessCommandL(const TInt aCommandId, const TUint8 aTransId, TDesC8& aData);
	TInt ExecuteCancelCommandL(const TUint8 aTransId, const TInt aCommandId);

	virtual void DoCompleteEventL(const TMockSYEvent& aEvent);
	void DoCompleteEventFurther(const TMockSYEvent& aEvent);
	static CMockPowerEngine* NewL(CMockPowerSY& aMockHwrmPluginSY);
private:
	CMockPowerEngine(CMockPowerSY& aMockHwrmPluginSY);
protected:
	virtual void LogRequest(TInt aCmdId, const MMockSyEngineData& aData,TInt aResultCode);
	virtual void LogCompletion(TInt aCmdId, const TDesC8& aData,TInt aResultCode);
	virtual void LogExpectError(TInt aCmdId, const MMockSyEngineData& aData,TInt aExpectedCmd,const TDesC8& aExpectedData);	
	virtual void QueueProcessedCompletionL(const TInt aCommandId, const TUint8 aTransId, RBuf8& aBuffer, TDesC8& /*aData*/);
	virtual void FurtherProcessEventL(const TInt aCommandId, TDesC8& aData);	
	
	void QueueProcessedEventL(const TInt aCommandId, const TUint8 aTransId, TDesC8& aData);
	virtual void Reset();

private:
	void DoLogError(TInt aCmdId, const TMockHwrmPluginData& aData,TInt aExpectedCmd,const TMockHwrmPluginData& aExpectedData);
	void DoLogCommandData(TInt aCmdId, const TMockHwrmPluginData& aCmdData, const TDesC& aComment);
	const TDesC& CommandName(TInt aCmdId);
private:
    TUint iPowerNotificationPeriod;
    TUint iPowerVoltageNotificationCounter;
    TUint iPowerCurrentNotificationCounter;
    TBool iPowerReporting;
	TInt iLatestTransId;
	TBool iStarted;
	CMockPowerSY& iMockPowerSY;
#ifdef SYMBIAN_HWRM_EXTPOWERINFO
    TUint iChargingCurrentNotificationPeriod;
    TUint iChargeTimeNotificationPeriod;
    TUint iChargingCurrentCounter;
    TUint iChargeTimeCounter;
    TBool iChargingCurrentReporting;
    TBool iChargingTimeReporting;
#endif //SYMBIAN_HWRM_EXTPOWERINFO
	};
	
#endif /*CMOCKPOWERENGINE_H_*/
