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


#include <ssm/ssmcommandlist.h>
#include <ssm/ssmcmd.hrh>

#include "ssmcommandlistutils.h"
#include <ssm/ssmcommand.h>
#include "ssmdebug.h"
#include "ssmpanic.h"


EXPORT_C TBool CSsmCommandListUtils::IsValidStateList(const CSsmCommandList& aCommandList)
	{
	// Validation 1: A command list used to transition to a system state must
	// contain exactly one publish system state command
	// and must not contain any publish swp commands
	
	//Validation 2:In a command list,  if there are one or more commands with their
	//execution behaviour as ESsmDeferredWaitForSignal that should be followed by a
	//SSM_MULTIPLE_WAIT command  
	
	
	return IsValidList(aCommandList, ESsmCmdPublishSystemState) && ValidateDeferredWaitCommand(aCommandList);
	}

EXPORT_C TBool CSsmCommandListUtils::IsValidSwpList(const CSsmCommandList& aCommandList)
	{
	//Validation 1: A command list used to change the value of a swp must
	// contain exactly one publish swp command
	// and must not contain any publish system state commands
	
	//Validation 2:In a command list,  if there are one or more commands with their
	//execution behaviour as ESsmDeferredWaitForSignal that should be followed by a
	//SSM_MULTIPLE_WAIT command 
		
	return IsValidList(aCommandList, ESsmCmdPublishSwp)  && ValidateDeferredWaitCommand(aCommandList);
	}

TBool CSsmCommandListUtils::IsValidList(const CSsmCommandList& aCommandList, TSsmCommandType aRequiredType)
 	{
	TBool ret = EFalse;
	TInt publishSystemStateCount = 0;
	TInt publishSwpCount = 0;
 	const TInt KRequiredCount = 1;
	
 	//From Commandlist fetch 1.) no. of publish system state command 
 	//2.) no. of publish swp command

	aCommandList.GetDataToValidateCommandlist(publishSystemStateCount,publishSwpCount);
	
	 if (aRequiredType == ESsmCmdPublishSystemState)	//if commandlist has a Publish System State command
 		{
		if (publishSwpCount != 0)	//and a Publish SWP command also
 			{
			DEBUGPRINT1A("PublishSwp command illegal in PublishSystemState command list");
 			}
		else if (publishSystemStateCount != KRequiredCount)	//number of publish System State commands is not equal to 1
 			{
			DEBUGPRINT2A("Command list incorrectly contains %d PublishSystemState command ", publishSystemStateCount);
 			}
 		else
 			{
			ret = ETrue;
 			}
 		}
  	else if (aRequiredType == ESsmCmdPublishSwp)	//if commandlist has a Publish SWP command
 		{
		if (publishSystemStateCount != 0)	//and a Publish System state command also
			{
			DEBUGPRINT1A("PublishSystemState command illegal in PublishSwp command list");
			}
		else if (publishSwpCount != KRequiredCount)	//number of publish SWP commands is not equal to 1
 			{
			DEBUGPRINT2A("Command list incorrectly contains %d PublishSwp command ", publishSwpCount);
 			}
		else
 			{
			ret = ETrue;
 			}
 		}
	 
	 return ret;
 	}

/**
This method runs through the command list from start to end checks for existence
of command with execution behaviour ESsmDeferredWaitForSignal
If one or more such commands found then it checks for ESsmCmdMultipleWait type command.
If ESsmCmdMultipleWait command not found corresponding to one or more ESsmDeferredWaitForSignal
returns EFalse.
*/  

TBool CSsmCommandListUtils::ValidateDeferredWaitCommand(const CSsmCommandList& aCommandList)
    {
    TBool ret = ETrue;
    TBool deferredWaitFound = EFalse;
    const TInt CommandsCount = aCommandList.Count();
    
    TSsmCommandType commandType; 
        
    for (TInt count = 0 ; count < CommandsCount ; count++)
        {
        if(deferredWaitFound)
            {
            commandType = aCommandList[count]->Type();
            if(commandType == ESsmCmdMultipleWait)
                {
                deferredWaitFound = EFalse;                 
                }
            }
        if(aCommandList[count]->ExecutionBehaviour() == ESsmDeferredWaitForSignal)  
            {
            deferredWaitFound = ETrue;              
            }
        }
        
    if(deferredWaitFound)
        {
        DEBUGPRINT1A("Command list incorrectly contains deferredWaitSignal command and no later Multiple_Wait");        
        ret = EFalse;                   
        }

    return ret;
    }  
