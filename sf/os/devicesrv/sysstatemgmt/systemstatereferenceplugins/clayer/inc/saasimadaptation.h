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

#ifndef __SAASIMADAPTATION_H__
#define __SAASIMADAPTATION_H__

#include <ssm/ssmadaptation.h>
#include "adaptationbase.h"

class CStartupAdaptationAdapter;

/*
 * Provides a concrete class to provide the appropriate adaptation interface
 * 
 * @internalComponent
 */
NONSHARABLE_CLASS(CSaaSimAdaptation) : public CAdaptationBase, public MSimAdaptation
{
public:
	static CSaaSimAdaptation* NewL(CStartupAdaptationAdapter* aAdapter);
	~CSaaSimAdaptation();

	void ProcessEventL(TSsmSimEventType aEventType);
	
private:
	//from MSimAdaptation
	virtual void Release();
	virtual void GetSimOwned(TDes8& aOwnedPckg, TRequestStatus& aStatus);
	virtual void GetCancel();

	virtual void NotifySimEvent(TDes8& aTypePckg, TRequestStatus& aStatus);
	virtual void NotifyCancel();
	
	//from CAdaptationBase
	virtual void RequestComplete(const StartupAdaptation::TCommand aCommandId, TDesC8& aRetPckg); 
	virtual TDesC8* ParameterPckg();
	
private:
	CSaaSimAdaptation(CStartupAdaptationAdapter* aAdapter);
	void ConstructL();
	
private:
	TBuf8<1> iNullBuf;
	TDes8* iReturnDes;
	TDes8* iEventOutputBuffer;
	TRequestStatus* iEventStatus;
	RArray<TSsmSimEventType> iEventQueue;
};

#endif // __SAASIMADAPTATION_H__
