/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Header for signal strength retreival object
* 
*
*/



/**
 @file SIGSTRGTH.H
 @internalComponent
*/

#ifndef __SIGSTRGTH_H__
#define __SIGSTRGTH_H__

#include <e32base.h>
#include <etelmm.h>

NONSHARABLE_CLASS(CAgentSignalStrengthWatcher) : public CActive
/**
This class watch the signal strength of the Agent.

@internalComponent
*/
	{
public:
	static CAgentSignalStrengthWatcher* NewL(RMobilePhone& aPhone);
	~CAgentSignalStrengthWatcher();
	TInt CurrentSignalStrength(TInt32& aSigStrength);
private:
	CAgentSignalStrengthWatcher(RMobilePhone& aPhone);
	void ConstructL();
private: 
	// From CActive 
	virtual void RunL();

	// From CActive
	virtual void DoCancel();
private:
	RMobilePhone& iPhone;
	enum {EInitialising,EUpdating} iState;
	TInt32 iSigStrengthInDBm;
	TInt8 iSigStrengthInBars;
	};


#endif

