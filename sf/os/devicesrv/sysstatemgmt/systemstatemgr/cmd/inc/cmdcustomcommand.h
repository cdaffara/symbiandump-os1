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

#ifndef __CMDCUSTOMCOMMAND_H__
#define __CMDCUSTOMCOMMAND_H__

#include "ssmdeferrablecommand.h"

class TSsmCommandParameters;
class RReadStream;
class MSsmCustomCommand;
class CSsmCustomCommandInfo;
class CSsmCustomCommandEnv;

/**
Function prototype for the custom command ordinal
@internalComponent
@released
*/
typedef MSsmCustomCommand* (*CustomCmdFunctionType)(void);

/**
Built-in command for executing a custom command
@internalComponent
@released
@panic KPanicCmdCustomCommand, EInvalidRunLAction The internal state of the dll execution became corrupted during processing of the command
@panic KPanicCmdCustomCommand, ENoCustomCommandObject The internal custom command object was not loaded or initialised properly before being used
@see CSsmCustomCommandInfo
@see MSsmCustomCommand
*/
NONSHARABLE_CLASS (CCmdCustomCommand) : public CSsmDeferrableCommand
	{
public: 
	static CCmdCustomCommand* NewL(RReadStream& aReadStream);
	static CCmdCustomCommand* NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const CSsmCustomCommandInfo& aInfo);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	static CCmdCustomCommand* NewL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const CSsmCustomCommandInfo& aInfo, const TUint16 aPriority);
#endif
	static CCmdCustomCommand* NewL(TSsmCommandParameters& aCommandParameters);
	//Must be used only by CLE to reconstruct the command
	static CCmdCustomCommand* NewLC(const CCmdCustomCommand& aCmdCustomCommand, CSsmCommandUtilProvider* aUtilProvider);
	~CCmdCustomCommand();

	// from MSsmCommand
	TSsmCommandType Type() const;
	void ExternalizeL(RWriteStream& aWriteStream) const;
	void Execute(TRequestStatus& aStatus);
	void ExecuteCancel();
	void Release();
	CSsmCustomCommandInfo& SsmCustomCommandInfo() const {return *iInfo;}
	
private:
	// Custom command state enumerations
	enum TCustomCmdAction
		{
		EPrepareDll = 0,
		EInitialiseDll,
		EExecuteMethod,
		EClose
		};

	CCmdCustomCommand();
	CCmdCustomCommand(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CCmdCustomCommand(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TUint16 aPriority);
#endif
	void InternalizeL(RReadStream& aReadStream);
	void ConstructL(RReadStream& aReadStream);
	void ConstructL(const CSsmCustomCommandInfo& aInfo);
	void ConstructL(TSsmCommandParameters& aCommandParameters);
	void ConstructL(const CCmdCustomCommand& aCmdCustomCommand, CSsmCommandUtilProvider* aUtilProvider);
	void PrepareCustomCmdL();
	void InitialiseCmdL();
	void MoveToNextStateL(TInt aError, TCustomCmdAction aNextState, TBool aCompleteRequest=ETrue);
	void ValidateL();
	static TInt MaxSupportedVersion();
	void WriteHandleToFileL(TInt aHandle);
	
protected:
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	CSsmCustomCommandInfo* iInfo;			// custom command plugin info
	TCustomCmdAction iAction;				// internal state iterator
	RLibrary iLibrary;						// interface to the load library API
	MSsmCustomCommand* iCustomCommand;		// the actual interface to the custom command
	TInt iAttempts;							// count of unsuccessful attempts
	TBool iLoaded;							// whether custom command dll loaded or not
	CSsmCustomCommandEnv* iCmdEnv;			// custom command environment passed during initialisation
	};

#endif // __CMDCUSTOMCOMMAND_H__
