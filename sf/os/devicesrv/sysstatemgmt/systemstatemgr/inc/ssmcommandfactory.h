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

#ifndef __SSMCOMMANDFACTORY_H__
#define __SSMCOMMANDFACTORY_H__

#include <e32power.h>
#include <ssm/ssmcmd.hrh>
#include <ssm/ssmswp.h>

class CSsmCustomCommandInfo;
class TSsmSupInfo;
class TSsmPublishSystemStateInfo;
class CSsmStartupProperties;
class RResourceReader;
class CSsmCommandUtilProvider;
class CSsmCommandBase;
class MSsmCommand;
class MSsmCommandList;
class TSsmCommandParameters;
class MSsmCommandFactory;
class TSsmSwp;

/**
@publishedPartner
@released
*/
class SsmCommandFactory
	{
public:
	// Construct a specific type of command
	IMPORT_C static MSsmCommand* ConstructWaitForApparcInitCommandLC(TCmdErrorSeverity aSeverity);
	IMPORT_C static MSsmCommand* ConstructMultipleWaitCommandLC(TInt32 aTimeout);
	IMPORT_C static MSsmCommand* ConstructAMAStarterCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TUid& aDscId);
	IMPORT_C static MSsmCommand* ConstructStartApplicationCommandLC(TCmdErrorSeverity aSeverity, CSsmStartupProperties* aStartupProperties);
	IMPORT_C static MSsmCommand* ConstructStartProcessCommandLC(TCmdErrorSeverity aSeverity, CSsmStartupProperties* aStartupProperties);
	IMPORT_C static MSsmCommand* ConstructCustomCommandCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, CSsmCustomCommandInfo& aCustomInfo);
	IMPORT_C static MSsmCommand* ConstructFinaliseDrivesCommandLC(TCmdErrorSeverity aSeverity);
	IMPORT_C static MSsmCommand* ConstructLoadSupCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TInt aRetries, const TSsmSupInfo& aInfo);
	IMPORT_C static MSsmCommand* ConstructPersistHalAttributesCommandLC(TCmdErrorSeverity aSeverity);
	IMPORT_C static MSsmCommand* ConstructPowerOffCommandLC(TCmdErrorSeverity aSeverity, TPowerState aPowerEvent);
	IMPORT_C static MSsmCommand* ConstructPublishSwpCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwp);
	IMPORT_C static MSsmCommand* ConstructReqSwpChangeCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwp);
	IMPORT_C static MSsmCommand* ConstructPublishSystemStateCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmPublishSystemStateInfo& aInfo);
	IMPORT_C static MSsmCommand* ConstructCreateSwpCommandLC(TCmdErrorSeverity aSeverity, const TSsmSwp& aSwp, TDesC& aFilename);
	IMPORT_C static MSsmCommand* ConstructSetPAndSKeyCommandLC(TCmdErrorSeverity aSeverity, const TUid& aCategory, TUint aKey, TInt aValue);

	// Construct a command of the specified type from a RReadStream
	static CSsmCommandBase* ConstructCommandFromStreamLC(TSsmCommandType aType, RReadStream& aReadStream, TArray<MSsmCommand*>& aDeferredList);

	// Construct a command of the specified type from resource
	static CSsmCommandBase* ConstructCommandFromResourceLC(TSsmCommandType aType, TSsmCommandParameters& aCommandParameters);
	
	// Construct a command of the specified type from MSsmCommand object
    static CSsmCommandBase* ConstructCommandFromCommandLC(const MSsmCommand& aSsmCommand, TArray<MSsmCommand*>& aDeferredList, CSsmCommandUtilProvider* aUtilProvider);

#ifdef SSM_CMD_TESTFRAMEWORK
	static void DelegateTo(MSsmCommandFactory* aExternalFactory); 
#endif //SSM_CMD_TESTFRAMEWORK
	
	//SYMBIAN_SSM_FLEXIBLE_MERGE
	IMPORT_C static MSsmCommand* ConstructWaitForApparcInitCommandLC(TCmdErrorSeverity aSeverity, const TUint16 aPriority);
	IMPORT_C static MSsmCommand* ConstructAMAStarterCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TUid& aDscId, const TUint16 aPriority);
	IMPORT_C static MSsmCommand* ConstructStartApplicationCommandLC(TCmdErrorSeverity aSeverity, CSsmStartupProperties* aStartupProperties, const TUint16 aPriority);
	IMPORT_C static MSsmCommand* ConstructStartProcessCommandLC(TCmdErrorSeverity aSeverity, CSsmStartupProperties* aStartupProperties, const TUint16 aPriority);
	IMPORT_C static MSsmCommand* ConstructCustomCommandCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, CSsmCustomCommandInfo& aCustomInfo, const TUint16 aPriority);
	IMPORT_C static MSsmCommand* ConstructLoadSupCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TInt aRetries, const TSsmSupInfo& aInfo, const TUint16 aPriority);
	IMPORT_C static MSsmCommand* ConstructPublishSwpCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwp, const TUint16 aPriority);
	IMPORT_C static MSsmCommand* ConstructReqSwpChangeCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwp, const TUint16 aPriority);
	IMPORT_C static MSsmCommand* ConstructPublishSystemStateCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmPublishSystemStateInfo& aInfo, const TUint16 aPriority);
	IMPORT_C static MSsmCommand* ConstructCreateSwpCommandLC(TCmdErrorSeverity aSeverity, const TSsmSwp& aSwp, TDesC& aFilename, const TUint16 aPriority);
	IMPORT_C static MSsmCommand* ConstructSetPAndSKeyCommandLC(TCmdErrorSeverity aSeverity, const TUid& aCategory, TUint aKey, TInt aValue, const TUint16 aPriority);
	IMPORT_C static MSsmCommand* ConstructMultipleWaitCommandLC(TInt32 aTimeout, const TUint16 aPriority);
	
	//SYMBIAN_SSM_FLEXIBLE_MERGE

private:
	SsmCommandFactory();
	};
	
#endif // __SSMCOMMANDFACTORY_H__
