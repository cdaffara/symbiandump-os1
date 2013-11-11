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

#ifndef __CMDMULTIPLEWAIT_H__
#define __CMDMULTIPLEWAIT_H__

#include "ssmcommandbase.h"

class RReadStream;
class TSsmCommandParameters;

/**
A command object responsible for checking that all deferred commands in the list preceding 
this multiple wait command have finished.  
Commandlist must be externalized and internalized in order to populate the deferred list 
of MULTIPLE_WAIT command.

@internalComponent
@released
*/
NONSHARABLE_CLASS(CCmdMultipleWait) : public CSsmCommandBase
	{
public:
	static CCmdMultipleWait* NewL(RReadStream& aReadStream, TArray<MSsmCommand*>& aDeferredCommands);
	static CCmdMultipleWait* NewL(TInt32 aTimeout);
	static CCmdMultipleWait* NewL(TSsmCommandParameters& aCommandParameters);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	static CCmdMultipleWait* NewL(TInt32 aTimeout,const TUint16 aPriority);
#endif
	//Must be used only by CLE to reconstruct the command
	static CCmdMultipleWait* NewLC(const CCmdMultipleWait& aCmdMultipleWait, TArray<MSsmCommand*>& aDeferredCommands);
	~CCmdMultipleWait();

	// from MSsmCommand
	void Execute(TRequestStatus& aStatus);
	void Release();
	void ExecuteCancel();
	TSsmCommandType Type() const;
	void ExternalizeL(RWriteStream& aWriteStream) const;

	void HandleTimeoutCallback();
	inline TInt32 Timeout() const {return iTimeout;}

private:
	enum TStates
		{
		EStateCheckCompletionStatus = 0,
		EStateTakeAction,
		EStateTimerExpired
		};

	//CTOR's
	CCmdMultipleWait();
	CCmdMultipleWait(TInt32 aTimeout);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CCmdMultipleWait(TInt32 aTimeout, const TUint16 aPriority);
#endif

	//ConstructL's
	void ConstructL(TArray<MSsmCommand*>&  aDeferredCommands, RReadStream& aReadStream);
	void ConstructL(TSsmCommandParameters& aCommandParameters);
	void ConstructL();
	void ConstructL(const CCmdMultipleWait& aCmdMultipleWait, TArray<MSsmCommand*>& aDeferredCommands);
	//helper functions
	void CheckCompletion();
	void TakeProperAction();
	void MoveToNextState(TStates aNextState);
	void InternalizeL(RReadStream& aReadStream);
	void Cleanup();
	static TInt MaxSupportedVersion();

	//AO to handle multiple wait timeout
	class CMultipleWaitTimer : public CTimer
		{
		public:
			static CMultipleWaitTimer* NewL(CCmdMultipleWait& aMultipleWaitCmd);
			~CMultipleWaitTimer();
			void StartTimer(TInt32 aTimeout);

		protected:
			// from CActive
			void RunL();

		private:
			void ConstructL();
			CMultipleWaitTimer(CCmdMultipleWait& aMultipleWaitCmd);

		private:
			CCmdMultipleWait& iMultipleWaitCmd;
		};

protected:
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	RPointerArray<MSsmCommand> iDeferredCommands;
	TInt32 iTimeout;
	CMultipleWaitTimer* iMultipleWaitTimer;

	// data members for maintaining internal state of multiple wait
	// index of the current deferred command
	TInt iCommandIndex;
	// internal state maintained by MULTIPLE_WAIT command
	TStates iInternalState;
	TInt iCommandError;
	};

#endif // __CMDMULTIPLEWAIT_H__
