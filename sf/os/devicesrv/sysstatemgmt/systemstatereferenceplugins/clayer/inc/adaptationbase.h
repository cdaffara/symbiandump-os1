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
//

#ifndef __ADAPTATIONBASE_H__
#define __ADAPTATIONBASE_H__

#include <ssm/ssmadaptation.h>

#include <ssm/startupadaptationcommands.h>

class CStartupAdaptationAdapter;

/*
 * Provides an abstract class for the various adaptation classes
 * 
 * @internalComponent
 */
NONSHARABLE_CLASS(CAdaptationBase) : public CBase
{
public:
	
	~CAdaptationBase();
	
public:

	void SetDispatched(TBool aDispatched);
	TBool Dispatched();

	void SetCommandId(StartupAdaptation::TCommand aCommandId);
	StartupAdaptation::TCommand CommandId();
	
	/**
	 * Callback function called by the adaptation adapter when the command from this adaptation has been completed 
	 * 
	 * @internalComponent
	 */
	virtual void RequestComplete(const StartupAdaptation::TCommand aCommandId, TDesC8& aRetPckg) = 0; 
	
	/**
	 * Returns a pointer to the package descriptor to pass as the parameters
	 * to the startup adaptation for this command
	 * 
	 * @internalComponent
	 */ 
	virtual TDesC8* ParameterPckg() = 0;
		
	void CompleteRequestStatus(TInt aValue);

protected:
	CAdaptationBase(CStartupAdaptationAdapter* aAdapter);
	
	TBool Busy();
		
	void SetRequestStatus(TRequestStatus* aStatus);
	
	void CancelRequest();

protected:
	CStartupAdaptationAdapter* iAdapter;
private:
	TRequestStatus* iRequestStatus;
	TBool iDispatched;
	StartupAdaptation::TCommand iCommandId;
};

#endif // __ADAPTATIONBASE_H__
