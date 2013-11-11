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

/**
 @file 
 @test 
 @internalComponent - Internal Symbian test code  
*/

#ifndef __TCMD_MOCK_SSMCOMMANDFACTORY_H__
#define __TCMD_MOCK_SSMCOMMANDFACTORY_H__

#include <ssm/ssmcmd.hrh>
#include <e32power.h>
#include <ssm/ssmswp.h>

class MSsmCommand;
class CSsmCommandBase;
class TSsmCommandParameters;
class CSsmStartupProperties;
class CSsmCustomCommandInfo;
class TSsmSupInfo;
class TSsmPublishSystemStateInfo;
class CSsmCommandUtilProvider;

class MSsmCommandFactory
	{
public:
	virtual	MSsmCommand* ConstructWaitForApparcInitCommandLC(TCmdErrorSeverity /*aSeverity*/) { return NULL; }
	virtual	MSsmCommand* ConstructMultipleWaitCommandLC(TInt32 /*aTimeout*/) { return NULL; }
	virtual	MSsmCommand* ConstructAMAStarterCommandLC(TCmdErrorSeverity /*aSeverity*/, TSsmExecutionBehaviour /*aExecutionBehaviour*/, const TUid& /*aDscId*/) { return NULL; }
	virtual	MSsmCommand* ConstructStartApplicationCommandLC(TCmdErrorSeverity /*aSeverity*/, CSsmStartupProperties* /*aStartupProperties*/) { return NULL; }
	virtual	MSsmCommand* ConstructStartProcessCommandLC(TCmdErrorSeverity /*aSeverity*/, CSsmStartupProperties* /*aStartupProperties*/) { return NULL; }
	virtual	MSsmCommand* ConstructCustomCommandCommandLC(TCmdErrorSeverity /*aSeverity*/, TSsmExecutionBehaviour /*aExecutionBehaviour*/, CSsmCustomCommandInfo& /*aCustomInfo*/) { return NULL; }
	virtual	MSsmCommand* ConstructFinaliseDrivesCommandLC(TCmdErrorSeverity /*aSeverity*/) { return NULL; }
	virtual	MSsmCommand* ConstructLoadSupCommandLC(TCmdErrorSeverity /*aSeverity*/, TSsmExecutionBehaviour /*aExecutionBehaviour*/, TInt /*aRetries*/, const TSsmSupInfo& /*aInfo*/) { return NULL; }
	virtual	MSsmCommand* ConstructPersistHalAttributesCommandLC(TCmdErrorSeverity /*aSeverity*/) { return NULL; }
	virtual	MSsmCommand* ConstructPowerOffCommandLC(TCmdErrorSeverity /*aSeverity*/, TPowerState /*aPowerEvent*/) { return NULL; }
	virtual	MSsmCommand* ConstructPublishSwpCommandLC(TCmdErrorSeverity /*aSeverity*/, TSsmExecutionBehaviour /*aExecutionBehaviour*/, const TSsmSwp& /*aSwp*/) { return NULL; }
	virtual	MSsmCommand* ConstructReqSwpChangeCommandLC(TCmdErrorSeverity /*aSeverity*/, TSsmExecutionBehaviour /*aExecutionBehaviour*/, const TSsmSwp& /*aSwp*/) { return NULL; }
	virtual	MSsmCommand* ConstructPublishSystemStateCommandLC(TCmdErrorSeverity /*aSeverity*/, TSsmExecutionBehaviour /*aExecutionBehaviour*/, TSsmPublishSystemStateInfo& /*aInfo*/) { return NULL; }	
	virtual	MSsmCommand* ConstructCreateSwpCommandLC(TCmdErrorSeverity /*aSeverity*/, const TSsmSwp& /*aSwp*/, TDesC& /*aFilename*/){ return NULL; }
	virtual	MSsmCommand* ConstructSetPAndSKeyCommandLC(TCmdErrorSeverity /*aSeverity*/, TUid /*aCategory*/, TUint /*aKey*/, TInt /*aValue*/) { return NULL; }
	virtual	CSsmCommandBase* ConstructCommandFromStreamLC(TSsmCommandType /*aType*/, RReadStream& /*aReadStream*/, TArray<MSsmCommand*>& /*aDeferredList*/) { return NULL; }
	virtual	CSsmCommandBase* ConstructCommandFromResourceLC(TSsmCommandType /*aType*/, TSsmCommandParameters& /*aCommandParameters*/) { return NULL; }
	virtual CSsmCommandBase* ConstructCommandFromCommandLC(const MSsmCommand& /*aSsmCommandBase*/, TArray<MSsmCommand*>& /*aDeferredList*/,CSsmCommandUtilProvider* /*aUtilProvider*/) {return NULL;}
	};


	
#endif // __TCMD_MOCK_SSMCOMMANDFACTORY_H__
