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

#ifndef __SAAMISCADAPTATION_H__
#define __SAAMISCADAPTATION_H__

#include <ssm/ssmadaptation.h>
#include "adaptationbase.h"

class CStartupAdaptationAdapter;

/*
 * Provides a concrete class to provide the appropriate adaptation interface
 * 
 * @internalComponent
 */
NONSHARABLE_CLASS(CSaaMiscAdaptation) : public CAdaptationBase, public MMiscAdaptation
{
public:
	static CSaaMiscAdaptation* NewL(CStartupAdaptationAdapter* aAdapter);
	~CSaaMiscAdaptation();
	
public:
	//from MMiscAdaptation
	virtual void Release();

	virtual void SecurityStateChange(TInt aState, TDes8& aResponsePckg, TRequestStatus& aStatus);
	virtual void GetGlobalStartupMode(TDes8& aModePckg, TRequestStatus& aRequest);
	virtual void PrepareSimLanguages(TSsmLanguageListPriority aPriority, TDes8& aSizePckg, TRequestStatus& aStatus);
	virtual void GetSimLanguagesL(CBufBase* aBuf, TInt aCount);
	virtual void GetHiddenReset(TDes8& aHiddenResetPckg, TRequestStatus& aStatus);
	virtual void Cancel();
	
	//from CAdaptationBase
	virtual void RequestComplete(const StartupAdaptation::TCommand aCommandId, TDesC8& aRetPckg); 
	virtual TDesC8* ParameterPckg();

private:
	CSaaMiscAdaptation(CStartupAdaptationAdapter* aAdapter);
	
private:
	TBuf8<1> iNullBuf;
	StartupAdaptation::TSecurityStateChangePckg iSecurityStateChangePckg;
	StartupAdaptation::TLanguageListPriorityPckg iLanguageListPriorityPckg;
	TDes8* iReturnPckg;
	StartupAdaptation::RLanguageList iLanguageList;
};

#endif // __SAAMISCADAPTATION_H__
