// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the state machine which drives backup and restore support for the 
// Bluetooth Manager server.
//

/**
 @file
 @internalAll
*/

#ifndef BTMANSERVERBURSTATEMACHINE_H
#define BTMANSERVERBURSTATEMACHINE_H

/**
TBTBURState - records the state of the BUR manager.
**/
enum TBTBURState
	{
	EBTBURStateNormal,			// Waiting for a BUR event

	EBTBURStateBackupRequest,		// Processing a backup request
	EBTBURStateBackupOngoing, 		// Taking part in a backup operation
	EBTBURStateBackupIgnore,		// Not taking part in a backup operation (e.g. operation may not affect us)

	EBTBURStateRestoreRequest,		// Processing a restore request
	EBTBURStateRestoreOngoing, 		// Taking part in a restore operation
	EBTBURStateRestoreIgnore,		// Not taking part in a restore operation (e.g. operation may not affect us)

	EBTBURStateProcessRestoreFile,		// Processing an available restore file.
	EBTBURStateRestoreFileProcessingComplete,	// Processing of restore file has completed and registry has been updated
	
	EBTBURStateUnknown,			// Used to indicate an invalid transition
	};

/**
TBTBUREvent - Events which can cause transitions in the state of the BUR manager
**/
enum TBTBUREvent
	{
	// Events relating to P&S key changes from BUR engine
	EBTBUREventUnset,			// Unset notification event received from the Secure Backup Engine
	EBTBUREventNormal,			// Normal mode notification event received from the Secure Backup Engine
	EBTBUREventBackup,			// Backup notification event received from the Secure Backup Engine
	EBTBUREventRestore,			// Restore notification event received from the Secure Backup Engine

	// Internally generated events
	EBTBUREventBackupProceed,		// BUR manager should proceed with backup operation
	EBTBUREventBackupReject,		// BUR manager should not proceed with backup operation
	EBTBUREventRestoreProceed,		// BUR manager should proceed with restore operation
	EBTBUREventRestoreReject,		// BUR manager should not proceed with restore operation
	EBTBUREventProcessRestoreFile,		// Request to process an available restore file
	EBTBUREventProcessRestoreFileComplete,	// Parsing of restore file has completed
	EBTBUREventRestoreFileTransitionNormal,	// Parsing of restore file has been completed, transition should be made to normal state.

	EBTBUREventAbortStateAction,		// A state action has encountered an error which requires an abort back to normal state.
	};

/**
State handling mechanisms - these classes support the state machine used by the BUR manager.
**/

class CBTManServerBURMgr;
class CBTManServerBURMgrStateAction;

NONSHARABLE_CLASS(CBTManServerBURMgrStateFactory) : public CBase
	{
public:
	static CBTManServerBURMgrStateFactory* NewL(CBTManServerBURMgr& aBURMgr);
	~CBTManServerBURMgrStateFactory();

	inline TBTBURState GetCurrentState();
	TInt TransitionState(TBTBUREvent aTransitionEvent);
	void ExecuteAction();

private:
	CBTManServerBURMgrStateFactory(CBTManServerBURMgr& aBURMgr);
	void ConstructL();

	void ConstructStateMachineL();
	CBTManServerBURMgrStateAction* ConstructStateLC(TBTBURState aState);

private:
	RPointerArray<CBTManServerBURMgrStateAction> iStateActions;
	CBTManServerBURMgr& iBURMgr;
	TBTBURState iState;

	};

NONSHARABLE_CLASS(CBTManServerBURMgrStateAction) : public CBase
	{
public:
	~CBTManServerBURMgrStateAction();

	inline TBTBURState GetThisState();
	virtual TBTBURState GetNextState(TBTBUREvent aTransitionEvent)=0;
	virtual void ExecuteActionL(CBTManServerBURMgr& aBURMgr)=0;
	virtual void HandleActionError(CBTManServerBURMgr& aBURMgr, TInt aError);

protected:
	CBTManServerBURMgrStateAction(TBTBURState aState);

protected:
	TBTBURState iState;

	};

/**
Normal state
**/

NONSHARABLE_CLASS(CBTManServerBURMgrStateNormal) : public CBTManServerBURMgrStateAction
	{
public:
	static CBTManServerBURMgrStateNormal* NewLC();
	~CBTManServerBURMgrStateNormal();

	virtual TBTBURState GetNextState(TBTBUREvent aTransitionEvent);
	virtual void ExecuteActionL(CBTManServerBURMgr& aBURMgr);

protected:
	CBTManServerBURMgrStateNormal();

	};

/**
Backup states
**/

NONSHARABLE_CLASS(CBTManServerBURMgrStateBackupRequest) : public CBTManServerBURMgrStateAction
	{
public:
	static CBTManServerBURMgrStateBackupRequest* NewLC();
	~CBTManServerBURMgrStateBackupRequest();

	virtual TBTBURState GetNextState(TBTBUREvent aTransitionEvent);
	virtual void ExecuteActionL(CBTManServerBURMgr& aBURMgr);
	virtual void HandleActionError(CBTManServerBURMgr& aBURMgr, TInt aError);

protected:
	CBTManServerBURMgrStateBackupRequest();

	};

NONSHARABLE_CLASS(CBTManServerBURMgrStateBackupOngoing) : public CBTManServerBURMgrStateAction
	{
public:
	static CBTManServerBURMgrStateBackupOngoing* NewLC();
	~CBTManServerBURMgrStateBackupOngoing();

	virtual TBTBURState GetNextState(TBTBUREvent aTransitionEvent);
	virtual void ExecuteActionL(CBTManServerBURMgr& aBURMgr);
	virtual void HandleActionError(CBTManServerBURMgr& aBURMgr, TInt aError);

protected:
	CBTManServerBURMgrStateBackupOngoing();

	};


NONSHARABLE_CLASS(CBTManServerBURMgrStateBackupIgnore) : public CBTManServerBURMgrStateAction
	{
public:
	static CBTManServerBURMgrStateBackupIgnore* NewLC();
	~CBTManServerBURMgrStateBackupIgnore();

	virtual TBTBURState GetNextState(TBTBUREvent aTransitionEvent);
	virtual void ExecuteActionL(CBTManServerBURMgr& aBURMgr);

protected:
	CBTManServerBURMgrStateBackupIgnore();

	};

/**
Restore states
**/

NONSHARABLE_CLASS(CBTManServerBURMgrStateRestoreRequest) : public CBTManServerBURMgrStateAction
	{
public:
	static CBTManServerBURMgrStateRestoreRequest* NewLC();
	~CBTManServerBURMgrStateRestoreRequest();

	virtual TBTBURState GetNextState(TBTBUREvent aTransitionEvent);
	virtual void ExecuteActionL(CBTManServerBURMgr& aBURMgr);
	virtual void HandleActionError(CBTManServerBURMgr& aBURMgr, TInt aError);

protected:
	CBTManServerBURMgrStateRestoreRequest();

	};

NONSHARABLE_CLASS(CBTManServerBURMgrStateRestoreOngoing) : public CBTManServerBURMgrStateAction
	{
public:
	static CBTManServerBURMgrStateRestoreOngoing* NewLC();
	~CBTManServerBURMgrStateRestoreOngoing();

	virtual TBTBURState GetNextState(TBTBUREvent aTransitionEvent);
	virtual void ExecuteActionL(CBTManServerBURMgr& aBURMgr);
	virtual void HandleActionError(CBTManServerBURMgr& aBURMgr, TInt aError);

protected:
	CBTManServerBURMgrStateRestoreOngoing();

	};

NONSHARABLE_CLASS(CBTManServerBURMgrStateRestoreIgnore) : public CBTManServerBURMgrStateAction
	{
public:
	static CBTManServerBURMgrStateRestoreIgnore* NewLC();
	~CBTManServerBURMgrStateRestoreIgnore();

	virtual TBTBURState GetNextState(TBTBUREvent aTransitionEvent);
	virtual void ExecuteActionL(CBTManServerBURMgr& aBURMgr);

protected:
	CBTManServerBURMgrStateRestoreIgnore();

	};

/**
Restore file processing states
**/

NONSHARABLE_CLASS(CBTManServerBURMgrStateProcessRestoreFile) : public CBTManServerBURMgrStateAction
	{
public:
	static CBTManServerBURMgrStateProcessRestoreFile* NewLC();
	~CBTManServerBURMgrStateProcessRestoreFile();

	virtual TBTBURState GetNextState(TBTBUREvent aTransitionEvent);
	virtual void ExecuteActionL(CBTManServerBURMgr& aBURMgr);
	virtual void HandleActionError(CBTManServerBURMgr& aBURMgr, TInt aError);

protected:
	CBTManServerBURMgrStateProcessRestoreFile();

	};

NONSHARABLE_CLASS(CBTManServerBURMgrStateRestoreFileProcessingComplete) : public CBTManServerBURMgrStateAction
	{
public:
	static CBTManServerBURMgrStateRestoreFileProcessingComplete* NewLC();
	~CBTManServerBURMgrStateRestoreFileProcessingComplete();

	virtual TBTBURState GetNextState(TBTBUREvent aTransitionEvent);
	virtual void ExecuteActionL(CBTManServerBURMgr& aBURMgr);

protected:
	CBTManServerBURMgrStateRestoreFileProcessingComplete();

	};

#endif // BTMANSERVERBURSTATEMACHINE_H
