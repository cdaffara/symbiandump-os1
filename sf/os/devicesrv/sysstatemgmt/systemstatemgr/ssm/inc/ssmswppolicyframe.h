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



#ifndef __SSMSWPPOLICYFRAME_H__
#define __SSMSWPPOLICYFRAME_H__

#include <e32base.h>
#include <ssm/ssmswppolicy.h>
#include <ssm/ssmswp.h>
#include "ssmswppolicycli.h"
#include "ssmswppolicyproxy.h"

class CSsmCommandList;

/**
The CSsmSwpPolicyFrame class is responsible for loading and discarding the policy DLL. 
It provides accessor functions that call policy functions through the swp policy interface.
The CSsmSwpPolicyFrame also takes ownership of the RLibrary class that was used to load the DLL
in order to discard the DLL when deleted

@internalComponent
@released
*/
class CSsmSwpPolicyFrame : public CBase
  	{
public:
	static CSsmSwpPolicyFrame* NewL(TLibraryFunction aNewLFunc);
	~CSsmSwpPolicyFrame();
	
	TUint SwpKey() const;
	void  SetSwpKey(TUint aSwpKey);
	void  SetLibrary(const RLibrary& aLibrary);
	void SetSsmSwpPolicySession(CSsmSwpPolicyCliSession* aSsmSwpPolicySession);

	void CallInitialize(TRequestStatus& aStatus);
	TInt CallInitializeCancel();
	MSsmSwpPolicy::TResponse CallTransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage);
	void CallPrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus);
	TInt CallPrepareCommandListCancel();
	CSsmCommandList* CallCommandList();
	void CallHandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus);
	TInt CallHandleCleReturnValueCancel();

private:
	CSsmSwpPolicyFrame();
	void ConstructL(TLibraryFunction aNewLFunc);

private:
	enum TInternalStates
		{
		EWaitForInitialize,
		EWaitForPrepare,
		EWaitForGet,
		EWaitForCommandListExecution
		};
	/** used to track the internal state of the object for extra integrity */
	TInternalStates iInternalState;
	/** the policy that has been loaded */
	MSsmSwpPolicy* iSwpPolicy;
	/** the swp key that this object represents */
	TUint iSwpKey;
	/** track whether the object has been initialised */
	TBool iInitializeCalled;	
	/** the library object used to load the DLL */
	RLibrary iLibrary;
	/** the interface used to communicate with the SsmSwpPolicyServer */	
	CSsmSwpPolicyCliSession* iSsmSwpPolicySession;	//not owned by me
	};
	
#endif
