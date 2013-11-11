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

#ifndef __SAAEMERGENCYCALLRFADAPTATION_H__
#define __SAAEMERGENCYCALLRFADAPTATION_H__

#include <ssm/ssmadaptation.h>
#include "adaptationbase.h"

class CStartupAdaptationAdapter;

/*
 * Provides a concrete class to provide the appropriate adaptation interface
 * 
 * @internalComponent
 */
NONSHARABLE_CLASS(CSaaEmergencyCallRfAdaptation) : public CAdaptationBase, public MEmergencyCallRfAdaptation
	{
public:
	static CSaaEmergencyCallRfAdaptation* NewL(CStartupAdaptationAdapter* aAdapter);
	~CSaaEmergencyCallRfAdaptation();
	
public:
	//from MEmergencyCallRfAdaptation
	virtual void Release();

	virtual void ActivateRfForEmergencyCall(TRequestStatus& aStatus);
	virtual void DeactivateRfForEmergencyCall(TRequestStatus& aStatus);

	virtual void Cancel();
	
	//from CAdaptationBase
	virtual void RequestComplete(const StartupAdaptation::TCommand aCommandId, TDesC8& aRetPckg); 
	virtual TDesC8* ParameterPckg();
	
private:
	CSaaEmergencyCallRfAdaptation(CStartupAdaptationAdapter* aAdapter);
	
private:
	TBuf8<1> iNullBuf;
};

#endif // __SAAEMERGENCYCALLRFADAPTATION_H__
