/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Declaration of TStrtSimStatusMgr class.
*
*/




/**
 @file
 @internalComponent
 @released
*/

#ifndef __CSIMSTATUSPOLICY_H__
#define __CSIMSTATUSPOLICY_H__

#include <ssm/ssmswppolicy.h>
#include <e32property.h>


NONSHARABLE_CLASS(CSimStatuspolicy) : public CBase, public MSsmSwpPolicy
	{
public:
	IMPORT_C static MSsmSwpPolicy* NewL();
	
	//from MSsmSwpPolicy
	void ConstructL();
	void Initialize(TRequestStatus& aStatus);
	void InitializeCancel();
	void Release();
	TResponse TransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage);
	void PrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus);
	void PrepareCommandListCancel();
	CSsmCommandList* CommandList();
	void HandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus);
	void HandleCleReturnValueCancel();
	
private:
	//from CBase
	CSimStatuspolicy();
	~CSimStatuspolicy();

	void DoPrepareCommandListL(const TSsmSwp& aSwp);
	MSsmSwpPolicy::TResponse IsSimReadableTransitionAllowed();
	MSsmSwpPolicy::TResponse IsSimUsableTransitionAllowed();
	MSsmSwpPolicy::TResponse IsSimNotSupportedTransitionAllowed();
	MSsmSwpPolicy::TResponse IsSimRemovedTransitionAllowed();

private:
	CSsmCommandList* iCommandList;
	RProperty iProperty;
	
    /** Indicates whether the SIM has ever been readable since the device was started. */
    TBool iSimHasBeenReadable;

    /** Indicates whether the SIM has ever been usable since the device was started. */
    TBool iSimHasBeenUsable;
	};

#endif //__CSIMSTATUSPOLICY_H__
