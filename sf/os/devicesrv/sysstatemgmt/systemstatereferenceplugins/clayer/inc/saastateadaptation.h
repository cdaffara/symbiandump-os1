// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SAASTATEADAPTATION_H__
#define __SAASTATEADAPTATION_H__

#include <ssm/ssmadaptation.h>
#include "adaptationbase.h"

class CStartupAdaptationAdapter;

/*
 * Provides a concrete class to provide the appropriate adaptation interface
 * 
 * @internalComponent
 */
NONSHARABLE_CLASS(CSaaStateAdaptation) : public CAdaptationBase, public MStateAdaptation
{
public:
	static CSaaStateAdaptation* NewL(CStartupAdaptationAdapter* aAdapter);
	~CSaaStateAdaptation();

	void ProcessEventL(TSsmCoopSysEventType aEventType);
	
private:

	//from MStateAdaptation
	virtual void Release();
	virtual void RequestCoopSysStateChange(TSsmState aState, TRequestStatus& aStatus);
	virtual void RequestCoopSysSelfTest(TRequestStatus& aStatus);
	virtual void RequestCoopSysPerformRestartActions(TInt aReason, TRequestStatus& aStatus);
	virtual void RequestCoopSysPerformShutdownActions(TInt aReason, TRequestStatus& aStatus);
	virtual void RequestCoopSysPerformRfsActions(TSsmRfsType aRfsType, TRequestStatus& aStatus);
	virtual void RequestCancel();

	virtual void NotifyCoopSysEvent(TDes8& aEvent, TRequestStatus& aStatus);
	virtual void NotifyCancel();
	
	//from CAdaptationBase
	virtual void RequestComplete(const StartupAdaptation::TCommand aCommandId, TDesC8& aRetPckg); 
	virtual TDesC8* ParameterPckg();
	
private:
	CSaaStateAdaptation(CStartupAdaptationAdapter* aAdapter);
	void ConstructL();
	
	static StartupAdaptation::TGlobalState MapToStartupAdaptationState(TSsmState aState);
	
private:
	TBuf8<1> iNullBuf;
	StartupAdaptation::TGlobalStateChangePckg iGlobalStatePckg;
	StartupAdaptation::TRFSReasonPckg iRfsReasonPckg;
	StartupAdaptation::TResetReasonPckg iResetReasonPckg;
	TDes8* iEventOutputBuffer;
	TRequestStatus* iEventStatus;
	RArray<TSsmCoopSysEventType> iEventQueue;
};

#endif // __SAASTATEADAPTATION_H__
