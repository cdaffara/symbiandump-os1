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

#ifndef __GSASTATEPOLICYSTARTUP_H__
#define __GSASTATEPOLICYSTARTUP_H__

#include <e32base.h>
#include <ssm/ssmstatepolicy.h>
#include <ssm/ssmconditionalcallback.h>

class CSsmCommandListResourceReader;

/**
Startup state policy plug-in for GSA. This is a polymorphic dll.
Implements MSsmStatePolicy to initialize, prepare and return command list using CSsmCommandListResourceReader.
Implements MSsmConditionalCallback to decide whether to include a command in a command list or not.

For internal use, implements a fallback mechanism to launch 'sysstart.exe'  when static command list for 'start-up' state is not found.
This is required during migration from 'sysstart' to 'ssma start-up' and is not required for writing a new 'start-up' state policy.

@internalComponent
@released

@see MSsmStatePolicy
@see MSsmConditionalCallback
*/
NONSHARABLE_CLASS(CGsaStatePolicyStartup)  :  public CBase,  public MSsmStatePolicy, public MSsmConditionalCallback
	{
public:
	IMPORT_C static MSsmStatePolicy* NewL();

	//from MSsmStatePolicy
	TResponse TransitionAllowed(const TSsmStateTransition& aRequest, TSsmStateTransition const* aCurrent, 
								TSsmStateTransition const* aQueued, const RMessagePtr2& aMessage);
	void Initialize(TRequestStatus& aStatus);
	void InitializeCancel();
	void PrepareCommandList(TSsmState aState, TInt aReason, TRequestStatus& aStatus);
	void PrepareCommandListCancel();
	CSsmCommandList* CommandList();
	TBool GetNextState(TSsmState aCurrentTransition, TInt aReason, TInt aError, TInt aSeverity, TSsmState& aNextState);
	void Release();

	// from MSsmConditionalCallback
	TBool ConditionalCommandAllowedL(CResourceFile& aResourceFile, TInt aResourceId);

private:
	~CGsaStatePolicyStartup();
	CGsaStatePolicyStartup();

	void ConstructL();
	TBool TransitionSupported(const TSsmState& aRequestedState) const;	
	void GetCommandListPath(TUint aBootMode, TDes& aCmdListPath);
	void GetStartupCountAndMaxbootLimitL(TInt &aBootCount, TInt &aResetLimit);

private:
	TInt iHardwareReason;
	TInt iSoftwareReason;
	TInt iSubStatesCount;
	TUint16 iRequestedSubState;
	TBool iLaunchSysStart;
	TBool iLaunchSysAgt2SrvAndWServ;

	RArray<TInt> iSubStates;
	RArray<TSsmState> iCurrentlySupportedTransitions;	
	CSsmCommandListResourceReader* iCommandListResourceReader;
	RFs iFs;

	// Possible panic codes within Startup policy
	enum TStartupPanicCodes
		{
		/**
		 Panic when command list resource reader is NULL
		*/
		EInvalidResourceReader = 1,
		/**
		 Panic when Main state supplied is not 'ESsmStartup'
		*/
		EInvalidStartupState,
		/**
		 Panic when ConditionalCommandAllowedL is called for a command which hasn't implemented conditional_information
		*/
		EConditionalInfoNotImplemented,
		/**
		 Panic on Emulator (__WINS__) instead of moving to Fail state when CLE returns an error during the execution of a command with a severity other than ECmdIgnoreFailure.
		 Fail policy would issue a re-start(TPowerState::EPwRestart) which is not supported on emulator. On H4hrp a re-start is issued by Fail policy.
		*/
		EEmulatorPowerOff,
		/**
		 Substate for transition is not found in the command list id.	
		*/
		ESubStateIndexNotFound,
		/**
		 File for persisting reboot counts not found	
		*/
		EBootupCountFileNotFound
		};
	};

#endif	//__GSASTATEPOLICYSTARTUP_H__
