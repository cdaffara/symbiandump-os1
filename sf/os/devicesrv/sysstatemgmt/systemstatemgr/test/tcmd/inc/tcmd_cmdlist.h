/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* // This class wraps up all the functionality of a command list as an Active Object.  
* // Built In Command Tests that want to run within a CommandList environment
* // should use this class.
* 
*
*/



/**
 @file
*/
#ifndef __TCMD_CMDLIST_H__
#define __TCMD_CMDLIST_H__

#include <ssm/ssmcmd.hrh>
#include <e32base.h>
#include <e32power.h>
#include <ssm/ssmswp.h>

class RFs;
class TSsmSupInfo;
class CSsmCommandList;
class CSsmStartupProperties;
class TSsmCommandParameters;
class CSsmCustomCommandInfo;
class SsmCommandFactory;
class TSsmPublishSystemStateInfo;
class CSsmCommandUtilProvider;

// function pointer to callback function
typedef TInt (*TCmdCallback)(TAny* aParam);

NONSHARABLE_CLASS (CCmdTestList) : public CActive
	{
public: 
	static CCmdTestList* NewLC(TCmdCallback aCallback, TAny* aParam);
	static CCmdTestList* NewL(TCmdCallback aCallback, TAny* aParam);
	~CCmdTestList();

	void AddWaitForApparcInitCommandL(TCmdErrorSeverity aSeverity);
	void AddMultipleWaitCommandL(TInt32 aTimeout);
	void AddAMAStarterCommandL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TUid aDscId);
	void AddStartApplicationCommandL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties* aStartupProperties);
	void AddStartProcessCommandL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties* aStartupProperties);
	void AddCustomCommandCommandL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, CSsmCustomCommandInfo& aInfo);
	void AddFinaliseDrivesCommandL(TCmdErrorSeverity aSeverity);
	void AddLoadSupCommandL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TInt16 aRetries, const TSsmSupInfo& aInfo);
	void AddPersistHalAttributesCommandL(TCmdErrorSeverity aSeverity);
	void AddPowerOffCommandL(TCmdErrorSeverity aSeverity, TPowerState aPowerEvent);
	void AddPublishSwpCommandL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmSwp& aSwp);
	void AddReqSwpChangeCommandL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmSwp& aSwp);
	void AddPublishSystemStateCommandL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmPublishSystemStateInfo& aInfo);
	void AddCreateSwpCommandL(TCmdErrorSeverity aSeverity, TSsmSwp& aSwpInfo, TDesC& aFilename);
	void AddSetPAndSKeyCommandL(TCmdErrorSeverity aSeverity, TUid aCategory, TUint aKey, TInt aValue);
	void AddCommandL(TSsmCommandType aType, RReadStream& aReadStream, RFs* aRfs=NULL);
	void AddCommandFromResourceL(TSsmCommandType aType, TSsmCommandParameters& aCommandParameters);

	void CancelAllCommands();
	TInt Errorcode() { return iErrorcode; }

protected:
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
private:
	CCmdTestList();
	void ConstructL(TCmdCallback* aCallback, TAny* aParam);
	void CompleteExecuteRequest(TInt aReason);

private:
	TInt iCommandIndex;				// the command currently being started
	TInt iErrorcode;				// the errorcode of the last started command
	CSsmCommandList* iCommandList;	// our list of commands
	RPointerArray<MSsmCommand> iDeferredList;	//list of deferred commands
	CSsmCommandUtilProvider* iUtilProvider;
	TCmdCallback iCallback;
	TAny* iParam;
	TBool iAddStop;
	};

#endif // __TCMD_CMDLIST_H__
