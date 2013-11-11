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

#ifndef __SAARTCADAPTATION_H__
#define __SAARTCADAPTATION_H__

#include <ssm/ssmadaptation.h>
#include "adaptationbase.h"

class CStartupAdaptationAdapter;

/*
 * Provides a concrete class to provide the appropriate adaptation interface
 * 
 * @internalComponent
 */
NONSHARABLE_CLASS(CSaaRtcAdaptation) : public CAdaptationBase, public MRtcAdaptation
{
public:
	static CSaaRtcAdaptation* NewL(CStartupAdaptationAdapter* aAdapter);
	~CSaaRtcAdaptation();
	
public:
	//from MRtcAdaptation
	virtual void Release();

	virtual void ValidateRtc(TDes8& aValidityPckg, TRequestStatus& aStatus);
	virtual void SetWakeupAlarm(TDesC8& aAlarmTimePckg, TRequestStatus& aStatus);
	virtual void UnsetWakeupAlarm(TRequestStatus& aStatus);
	virtual void Cancel();
	
	//from CAdaptationBase
	virtual void RequestComplete(const StartupAdaptation::TCommand aCommandId, TDesC8& aRetPckg); 
	virtual TDesC8* ParameterPckg();
	
private:
	CSaaRtcAdaptation(CStartupAdaptationAdapter* aAdapter);
	
private:
	TBuf8<1> iNullBuf;
	TDes8* iReturnDes;
	StartupAdaptation::TWakeupAlarmPckg iWakeupAlarmPckg;
};

#endif // __SAARTCADAPTATION_H__
