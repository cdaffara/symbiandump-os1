// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SSMADAPTATIONCLI_H__
#define __SSMADAPTATIONCLI_H__

#include <e32std.h>
#include <e32base.h>

#include <ssm/ssmstate.h>
#include <ssm/ssmadaptation.hrh>


/**
 Base class for Adaptation server client interface classes.
 RSsmAdaptationBase serves as base for the following classes which are published 
 as client interface for Adaptation server.
 @publishedPartner
 @released
*/
// NONSHARABLE_CLASS(RSsmAdaptationBase) : public RSessionBase
class RSsmAdaptationBase: public RSessionBase
	{
public:
	IMPORT_C TInt Connect();
	IMPORT_C TInt Connect(TInt aAsyncMessageSlotCount);
	//Used by testcode only
#ifdef TEST_SSM_SERVER
    TInt Connect(const TDesC& aServerName);
#endif //TEST_SSM_SERVER
private:
    TInt DoConnect(const TDesC& aServerName, TInt aAsyncMessageSlotCount);
	TVersion Version() const;
	};


/**
 Client interface for State Adaptation part of the Adaptation server.	
 @publishedPartner
 @released
 */
// NONSHARABLE_CLASS(RSsmStateAdaptation) : public RSsmAdaptationBase
class RSsmStateAdaptation: public RSsmAdaptationBase
	{ 
public:
	IMPORT_C void RequestCoopSysStateChange(TSsmState aState, TRequestStatus& aStatus);
	IMPORT_C void RequestCoopSysSelfTest(TRequestStatus& aStatus);
	IMPORT_C void RequestCoopSysPerformRestartActions(TInt aReason, TRequestStatus& aStatus);
	IMPORT_C void RequestCoopSysPerformShutdownActions(TInt aReason, TRequestStatus& aStatus);
	IMPORT_C void RequestCoopSysPerformRfsActions(TSsmRfsType aType, TRequestStatus& aStatus);
	IMPORT_C void RequestCancel();
	IMPORT_C void NotifyCoopSysEvent(TDes8& aTypePckg, TRequestStatus& aStatus);
	IMPORT_C void NotifyCancel();
	IMPORT_C void GetLastCoopSysEvent(TDes8& aTypePckg);
	};

/**
 Client interface for Sim Adaptation part of the Adaptation server.	
 @publishedPartner
 @released
 */
// NONSHARABLE_CLASS(RSsmSimAdaptation) : public RSsmAdaptationBase
class RSsmSimAdaptation: public RSsmAdaptationBase
	{
public:
	IMPORT_C void GetSimOwned(TDes8& aOwnedPckg, TRequestStatus& aStatus);
	IMPORT_C void GetCancel();
	IMPORT_C void NotifySimEvent(TDes8& aTypePckg, TRequestStatus& aStatus);
	IMPORT_C void NotifyCancel();
	IMPORT_C void GetLastSimEvent(TDes8& aTypePckg);
	};
/**
 Client interface for RTC Adaptation part of the Adaptation server.	
 @publishedPartner
 @released
 */
// NONSHARABLE_CLASS(RSsmRtcAdaptation) : public RSsmAdaptationBase
class RSsmRtcAdaptation: public RSsmAdaptationBase
	{
public:
	IMPORT_C void ValidateRtc(TDes8& aValidityPckg, TRequestStatus& aStatus);
	IMPORT_C void SetWakeupAlarm(TDesC8& aAlarmTimePckg, TRequestStatus& aStatus);
	IMPORT_C void UnsetWakeupAlarm(TRequestStatus& aStatus);
	IMPORT_C void Cancel();
	};
	
/**
 Client interface for Miscellaneous Adaptation part of the Adaptation server.	
 @publishedPartner
 @released
 */
// NONSHARABLE_CLASS(RSsmMiscAdaptation) : public RSsmAdaptationBase
class RSsmMiscAdaptation: public RSsmAdaptationBase
	{
public:
	IMPORT_C void SecurityStateChange(TInt aState, TDes8& aResponsePckg, TRequestStatus& aStatus);
	IMPORT_C void GetGlobalStartupMode(TDes8& aModePckg, TRequestStatus& aRequest);
	IMPORT_C void PrepareSimLanguages(TSsmLanguageListPriority aPriority, TDes8& aSizePckg, TRequestStatus& aStatus);
	IMPORT_C CArrayFix<TInt32>* SimLanguagesArrayL(TInt aSimLanguageCount);
	IMPORT_C void Cancel();
	IMPORT_C void GetHiddenReset(TDes8& aHiddenResetPckg, TRequestStatus& aStatus);
	};

/**
 Client interface for EmergencyCallRf Adaptation part of the Adaptation server.	
 @publishedPartner
 @released
 */
// NONSHARABLE_CLASS(RSsmEmergencyCallRfAdaptation) : public RSsmAdaptationBase
class RSsmEmergencyCallRfAdaptation: public RSsmAdaptationBase
	{
public:
    IMPORT_C TInt SetAsPriorityClient();
	IMPORT_C void ActivateRfForEmergencyCall(TRequestStatus& aStatus);
	IMPORT_C void DeactivateRfForEmergencyCall(TRequestStatus& aStatus);
	IMPORT_C void Cancel();
	};

	
#endif	// __SSMSUSCLI_H__
