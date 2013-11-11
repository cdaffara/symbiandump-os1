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
// ssmcommandlistfactory.cpp
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code  
*/


#include <ssm/ssmswp.h>
#include <ssm/ssmcommand.h>

#include "ssmcommandfactory.h"
#include "ssmpanic.h"
#include "ssmcommandlistimpl.h"
#include "ssmcommandutilprovider.h"
#include "tcmd_mock_commandfactory.h"

static MSsmCommandFactory* EXTERNALFACTORY = NULL;

void SsmCommandFactory::DelegateTo(MSsmCommandFactory* aExternalFactory)
	{
	EXTERNALFACTORY = aExternalFactory;
	}

void CleanupCommand(TAny* aCmd)
	{
	if (aCmd)
		{
		MSsmCommand* const cmd = static_cast<MSsmCommand*>(aCmd);
		cmd->Release();
		}
	}

MSsmCommand* SsmCommandFactory::ConstructWaitForApparcInitCommandLC(TCmdErrorSeverity aSeverity)
	{
	if(EXTERNALFACTORY)
		return EXTERNALFACTORY->ConstructWaitForApparcInitCommandLC(aSeverity);
	
	MSsmCommand* cmd = NULL;
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

MSsmCommand* SsmCommandFactory::ConstructMultipleWaitCommandLC(TInt32 aTimeout)
	{
	if(EXTERNALFACTORY)
		return EXTERNALFACTORY->ConstructMultipleWaitCommandLC(aTimeout);
	
	MSsmCommand* cmd = NULL;
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

MSsmCommand* SsmCommandFactory::ConstructAMAStarterCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TUid& aDscId)
	{
	if(EXTERNALFACTORY)
		return EXTERNALFACTORY->ConstructAMAStarterCommandLC(aSeverity, aExecutionBehaviour, aDscId);
	
	MSsmCommand* cmd = NULL;
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

MSsmCommand* SsmCommandFactory::ConstructStartApplicationCommandLC(TCmdErrorSeverity aSeverity, CSsmStartupProperties* aStartupProperties)
	{
	if(EXTERNALFACTORY)
		return EXTERNALFACTORY->ConstructStartApplicationCommandLC(aSeverity, aStartupProperties);
	
	MSsmCommand* cmd = NULL;
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

MSsmCommand* SsmCommandFactory::ConstructStartProcessCommandLC(TCmdErrorSeverity aSeverity, CSsmStartupProperties* aStartupProperties)
	{
	if(EXTERNALFACTORY)
		return EXTERNALFACTORY->ConstructStartProcessCommandLC(aSeverity, aStartupProperties);
	
	MSsmCommand* cmd = NULL;
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

MSsmCommand* SsmCommandFactory::ConstructCustomCommandCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, CSsmCustomCommandInfo& aCustomInfo)
	{
	if(EXTERNALFACTORY)
		return EXTERNALFACTORY->ConstructCustomCommandCommandLC(aSeverity, aExecutionBehaviour, aCustomInfo);
	
	MSsmCommand* cmd = NULL;
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

MSsmCommand* SsmCommandFactory::ConstructFinaliseDrivesCommandLC(TCmdErrorSeverity aSeverity)
	{
	if(EXTERNALFACTORY)
		return EXTERNALFACTORY->ConstructFinaliseDrivesCommandLC(aSeverity);
	
	MSsmCommand* cmd = NULL;
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

MSsmCommand* SsmCommandFactory::ConstructLoadSupCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TInt aRetries, const TSsmSupInfo& aInfo)
	{
	if(EXTERNALFACTORY)
		return EXTERNALFACTORY->ConstructLoadSupCommandLC(aSeverity, aExecutionBehaviour, aRetries, aInfo);
	
	MSsmCommand* cmd = NULL;
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

MSsmCommand* SsmCommandFactory::ConstructPersistHalAttributesCommandLC(TCmdErrorSeverity aSeverity)
	{
	if(EXTERNALFACTORY)
		return EXTERNALFACTORY->ConstructPersistHalAttributesCommandLC(aSeverity);
	
	MSsmCommand* cmd = NULL;
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

MSsmCommand* SsmCommandFactory::ConstructPowerOffCommandLC(TCmdErrorSeverity aSeverity, TPowerState aPowerEvent)
	{
	if(EXTERNALFACTORY)
		return EXTERNALFACTORY->ConstructPowerOffCommandLC(aSeverity, aPowerEvent);
	
	MSsmCommand* cmd = NULL;
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

MSsmCommand* SsmCommandFactory::ConstructPublishSwpCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwp)
	{
	if(EXTERNALFACTORY)
		return EXTERNALFACTORY->ConstructPublishSwpCommandLC(aSeverity, aExecutionBehaviour, aSwp);
	
	MSsmCommand* cmd = NULL;
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

MSsmCommand* SsmCommandFactory::ConstructReqSwpChangeCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TSsmSwp& aSwpInfo)
	{
	if(EXTERNALFACTORY)
		return EXTERNALFACTORY->ConstructReqSwpChangeCommandLC(aSeverity, aExecutionBehaviour, aSwpInfo);
	
	MSsmCommand* cmd = NULL;
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

MSsmCommand* SsmCommandFactory::ConstructPublishSystemStateCommandLC(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmPublishSystemStateInfo& aInfo)
	{
	if(EXTERNALFACTORY)
		return EXTERNALFACTORY->ConstructPublishSystemStateCommandLC(aSeverity, aExecutionBehaviour, aInfo);
	
	MSsmCommand* cmd = NULL;
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

MSsmCommand* SsmCommandFactory::ConstructCreateSwpCommandLC(TCmdErrorSeverity aSeverity, const TSsmSwp& aSwp, TDesC& aFilename)
	{
	if(EXTERNALFACTORY)
		return EXTERNALFACTORY->ConstructCreateSwpCommandLC(aSeverity, aSwp, aFilename);
	
	MSsmCommand* cmd = NULL;
	CleanupStack::PushL(TCleanupItem(CleanupCommand, cmd));
	return cmd;
	}

//Factory function to create a command from a MSsmCommand object.
CSsmCommandBase* SsmCommandFactory::ConstructCommandFromCommandLC(const MSsmCommand& aSsmCommandBase, TArray<MSsmCommand*>& aDeferredList, CSsmCommandUtilProvider* aUtilProvider)
    {
    if(EXTERNALFACTORY)
        return EXTERNALFACTORY->ConstructCommandFromCommandLC(aSsmCommandBase, aDeferredList, aUtilProvider);
    
    CSsmCommandBase* cmd = NULL;
    CleanupStack::PushL(cmd);
    return cmd;
    }
//
// Add a command to the command list with data from a RReadStream
//
CSsmCommandBase* SsmCommandFactory::ConstructCommandFromStreamLC(TSsmCommandType aType, RReadStream& aReadStream, TArray<MSsmCommand*>& aDeferredList)
	{
	if(EXTERNALFACTORY)
		return EXTERNALFACTORY->ConstructCommandFromStreamLC(aType, aReadStream, aDeferredList);
	
	CSsmCommandBase* cmd = NULL;
	CleanupStack::PushL(cmd);
	return cmd;
	}


//
// Add a command to the command list with data from a RResourceReader
//
CSsmCommandBase* SsmCommandFactory::ConstructCommandFromResourceLC(TSsmCommandType aType, TSsmCommandParameters& aCommandParameters)
	{ 
	if(EXTERNALFACTORY)
		return EXTERNALFACTORY->ConstructCommandFromResourceLC(aType, aCommandParameters);
	
	CSsmCommandBase* cmd = NULL;
	CleanupStack::PushL(cmd);
	return cmd;
	}

