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

#include <ssm/ssmcommandlist.h>
#include <ssm/ssmcommand.h>
#include <ssm/ssmcommandfactory.h>

#include "ssmdebug.h"
#include "ssmpanic.h"
#include "tcmd_cmdlist.h"
#include "ssmcommandbase.h"
#include "ssmcommandlistinterface.h"
#include "ssmcommandutilprovider.h"
#include "cmdwaitforapparcinit.h"
#include "cmdmultiplewait.h"
#include "cmdpublishswp.h"
#include "cmdcustomcommand.h"
#include "cmdpublishsystemstate.h"
#include "cmdpoweroff.h"
#include "cmdfinalisedrives.h"
#include "cmdloadsup.h"
#include "cmdpersisthalattributes.h"
#include "cmdpublishswp.h"
#include "cmdreqswpchange.h"
#include "cmdamastarter.h"
#include "cmdstartprocess.h"
#include "cmdstartapp.h"
#include "cmdcreateswp.h"
#include "cmdsetpandskey.h"
#include "s32mem.h"

//
// Create the object
//
CCmdTestList* CCmdTestList::NewLC(TCmdCallback aCallback, TAny* aParam)
	{
	CCmdTestList* self = new(ELeave) CCmdTestList();
	CleanupStack::PushL(self);
	self->ConstructL(&aCallback, aParam);
	return self;
	}

CCmdTestList* CCmdTestList::NewL(TCmdCallback aCallback, TAny* aParam)
	{
	CCmdTestList* self = CCmdTestList::NewLC(aCallback, aParam);
	CleanupStack::Pop(self);
	return self;
	}


//
// Destroy the object
//
CCmdTestList::~CCmdTestList()
	{
	RDebug::Printf("CCmdTestList::~CCmdTestList");
	CActive::Cancel();
	delete iUtilProvider;
	iDeferredList.Close();
	if (iCommandList)
		{
		iCommandList->DeferredDelete();
		}
	iCommandList = NULL;
	}

//
// Prepare commands
//
void CCmdTestList::AddWaitForApparcInitCommandL(TCmdErrorSeverity aSeverity)
	{
	RDebug::Printf("CCmdTestList::AddWaitForApparcInitCommandL");
	SSMLOGLEAVEIFNULL(iCommandList);	
	CSsmCommandBase* cmd = CCmdWaitForApparcInit::NewL(aSeverity);
	CleanupStack::PushL(cmd);
	cmd->SetUtilProvider(*iUtilProvider);
	static_cast<MSsmCommandList&>(*iCommandList).AppendL(cmd);
	if (cmd->IsDeferred() && (ESsmDeferredWaitForSignal == cmd->ExecutionBehaviour()))
		{
		RDebug::Printf("Test Adding deferred command to list...");
		iDeferredList.AppendL(cmd);
		}
	CleanupStack::Pop(cmd);
	}

void CCmdTestList::AddMultipleWaitCommandL(TInt32 aTimeout)
	{
	RDebug::Printf("CCmdTestList::AddMultipleWaitCommandL");
	SSMLOGLEAVEIFNULL(iCommandList);
	//
	const TInt KTempDataLength = 1024;
	CBufFlat* inputBuffer = CBufFlat::NewL(KTempDataLength);
	CleanupStack::PushL(inputBuffer);
	RBufWriteStream writeStream(*inputBuffer);
	CleanupClosePushL(writeStream);
	writeStream.WriteInt32L(aTimeout);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	writeStream.WriteUint16L(KDefaultCommandPriority);
#endif //SYMBIAN_SSM_FLEXIBLE_MERGE
	writeStream.CommitL();
	RDesReadStream readStream(inputBuffer->Ptr(0));
	CleanupClosePushL(readStream);
	TArray<MSsmCommand*> deferredList = iDeferredList.Array();
	CSsmCommandBase* cmd = CCmdMultipleWait::NewL(readStream, deferredList);
	CleanupStack::PushL(cmd);
	static_cast<MSsmCommandList&>(*iCommandList).AppendL(cmd);
	iDeferredList.Reset();
	CleanupStack::Pop(cmd);

	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(inputBuffer);
	}

void CCmdTestList::AddAMAStarterCommandL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, const TUid aDscId)
	{
	RDebug::Printf("CCmdTestList::AddAMAStarterCommandL");
	SSMLOGLEAVEIFNULL(iCommandList);	
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	MSsmCommand* cmd = SsmCommandFactory::ConstructAMAStarterCommandLC(aSeverity, aExecutionBehaviour, aDscId, KDefaultCommandPriority);
#else
	CSsmCommandBase* cmd = CCmdAmaStarter::NewL(aSeverity, aExecutionBehaviour, aDscId);
	CleanupStack::PushL(cmd);
#endif
	static_cast<MSsmCommandList&>(*iCommandList).AppendL(cmd);
	if (cmd->IsDeferred() && (ESsmDeferredWaitForSignal == cmd->ExecutionBehaviour()))
		{
		RDebug::Printf("Test Adding deferred command to list...");
		iDeferredList.AppendL(cmd);
		}
	CleanupStack::Pop(cmd);
	}

void CCmdTestList::AddStartApplicationCommandL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties* aStartupProperties)
	{
	RDebug::Printf("CCmdTestList::AddStartApplicationCommandL");
	SSMLOGLEAVEIFNULL(iCommandList);	
	CSsmCommandBase* cmd = CCmdStartApp::NewL(aSeverity, aStartupProperties);
	CleanupStack::PushL(cmd);
	cmd->SetUtilProvider(*iUtilProvider);
	static_cast<MSsmCommandList&>(*iCommandList).AppendL(cmd);
	if (cmd->IsDeferred() && (ESsmDeferredWaitForSignal == cmd->ExecutionBehaviour()))
		{
		RDebug::Printf("Test Adding deferred command to list...");
		iDeferredList.AppendL(cmd);
		}
	CleanupStack::Pop(cmd);
	}

void CCmdTestList::AddStartProcessCommandL(TCmdErrorSeverity aSeverity, const CSsmStartupProperties* aStartupProperties)
	{
	RDebug::Printf("CCmdTestList::AddStartProcessCommandL");
	SSMLOGLEAVEIFNULL(iCommandList);	
	CSsmCommandBase* cmd = CCmdStartProcess::NewL(aSeverity, aStartupProperties);
	CleanupStack::PushL(cmd);
	cmd->SetUtilProvider(*iUtilProvider);
	static_cast<MSsmCommandList&>(*iCommandList).AppendL(cmd);
	if (cmd->IsDeferred() && (ESsmDeferredWaitForSignal == cmd->ExecutionBehaviour()))
		{
		RDebug::Printf("Test Adding deferred command to list...");
		iDeferredList.AppendL(cmd);
		}
	CleanupStack::Pop(cmd);
	}

void CCmdTestList::AddCustomCommandCommandL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, CSsmCustomCommandInfo& aInfo)
	{
	RDebug::Printf("CCmdTestList::AddCustomCommandCommandL");
	SSMLOGLEAVEIFNULL(iCommandList);
	RDebug::Printf("iCommandList is not NULL");
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	MSsmCommand* mcmd = SsmCommandFactory::ConstructCustomCommandCommandLC(aSeverity, aExecutionBehaviour, aInfo, KDefaultCommandPriority);
	RDebug::Printf("Constructed mcmd");
	CSsmCommandBase* cmd = dynamic_cast<CSsmCommandBase*>(mcmd);
	RDebug::Printf("Casted to cmd");
	if (cmd == NULL)
		{
		// reaching here means dynamic_casting has failed; so use the work around
		RDebug::Printf("it is NULL so re-construct");
		CleanupStack::PopAndDestroy(mcmd);
		delete cmd;
		cmd = NULL;
		cmd = CCmdCustomCommand::NewL(aSeverity, aExecutionBehaviour, aInfo);
		CleanupStack::PushL(cmd);
		}
#else
	CSsmCommandBase* cmd = CCmdCustomCommand::NewL(aSeverity, aExecutionBehaviour, aInfo);
	CleanupStack::PushL(cmd);
#endif
	cmd->SetUtilProvider(*iUtilProvider);
	RDebug::Printf("After Setting util provider");
	static_cast<MSsmCommandList&>(*iCommandList).AppendL(cmd);
	RDebug::Printf("After AppendL");
	if (cmd->IsDeferred() && (ESsmDeferredWaitForSignal == cmd->ExecutionBehaviour()))
		{
		RDebug::Printf("Test Adding deferred command to list...");
		iDeferredList.AppendL(cmd);
		}
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	CleanupStack::Pop();
#else
	CleanupStack::Pop(cmd);
#endif
	}

void CCmdTestList::AddFinaliseDrivesCommandL(TCmdErrorSeverity aSeverity)
	{
	RDebug::Printf("CCmdTestList::AddCustomCommandCommandL");
	SSMLOGLEAVEIFNULL(iCommandList);	
	CSsmCommandBase* cmd = CCmdFinaliseDrives::NewL(aSeverity);
	CleanupStack::PushL(cmd);
	static_cast<MSsmCommandList&>(*iCommandList).AppendL(cmd);
	if (cmd->IsDeferred() && (ESsmDeferredWaitForSignal == cmd->ExecutionBehaviour()))
		{
		RDebug::Printf("Test Adding deferred command to list...");
		iDeferredList.AppendL(cmd);
		}
	CleanupStack::Pop(cmd);
	}

void CCmdTestList::AddLoadSupCommandL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TInt16 aRetries, const TSsmSupInfo& aInfo)
	{
	RDebug::Printf("CCmdTestList::AddLoadSupCommandL");
	SSMLOGLEAVEIFNULL(iCommandList);	
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	MSsmCommand* cmd = SsmCommandFactory::ConstructLoadSupCommandLC(aSeverity, aExecutionBehaviour, aRetries, aInfo, KDefaultCommandPriority); 
#else
	CSsmCommandBase* cmd = CCmdLoadSup::NewL(aSeverity, aExecutionBehaviour, aRetries, aInfo);
	CleanupStack::PushL(cmd);
#endif
	static_cast<MSsmCommandList&>(*iCommandList).AppendL(cmd);
	if (cmd->IsDeferred() && (ESsmDeferredWaitForSignal == cmd->ExecutionBehaviour()))
		{
		RDebug::Printf("Test Adding deferred command to list...");
		iDeferredList.AppendL(cmd);
		}
	CleanupStack::Pop(cmd);
	}

void CCmdTestList::AddPersistHalAttributesCommandL(TCmdErrorSeverity aSeverity)
	{
	RDebug::Printf("CCmdTestList::AddPersistHalAttributesCommandL");
	SSMLOGLEAVEIFNULL(iCommandList);	
	CSsmCommandBase* cmd = CCmdPersistHalAttributes::NewL(aSeverity);
	CleanupStack::PushL(cmd);
	static_cast<MSsmCommandList&>(*iCommandList).AppendL(cmd);
	if (cmd->IsDeferred() && (ESsmDeferredWaitForSignal == cmd->ExecutionBehaviour()))
		{
		RDebug::Printf("Test Adding deferred command to list...");
		iDeferredList.AppendL(cmd);
		}
	CleanupStack::Pop(cmd);
	}

void CCmdTestList::AddPowerOffCommandL(TCmdErrorSeverity aSeverity, TPowerState aPowerEvent)
	{
	RDebug::Printf("CCmdTestList::AddPowerOffCommandL");
	SSMLOGLEAVEIFNULL(iCommandList);	
	CSsmCommandBase* cmd = CCmdPowerOff::NewL(aSeverity, aPowerEvent);
	CleanupStack::PushL(cmd);
	static_cast<MSsmCommandList&>(*iCommandList).AppendL(cmd);
	if (cmd->IsDeferred() && (ESsmDeferredWaitForSignal == cmd->ExecutionBehaviour()))
		{
		RDebug::Printf("Test Adding deferred command to list...");
		iDeferredList.AppendL(cmd);
		}
	CleanupStack::Pop(cmd);
	}

void CCmdTestList::AddPublishSwpCommandL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmSwp& aSwp)
	{
	RDebug::Printf("CCmdTestList::AddPublishSwpCommandL");
	SSMLOGLEAVEIFNULL(iCommandList);	
	CSsmCommandBase* cmd = CCmdPublishSwp::NewL(aSeverity, aExecutionBehaviour, aSwp);
	CleanupStack::PushL(cmd);
	static_cast<MSsmCommandList&>(*iCommandList).AppendL(cmd);
	if (cmd->IsDeferred() && (ESsmDeferredWaitForSignal == cmd->ExecutionBehaviour()))
		{
		RDebug::Printf("Test Adding deferred command to list...");
		iDeferredList.AppendL(cmd);
		}
	CleanupStack::Pop(cmd);
	}

void CCmdTestList::AddReqSwpChangeCommandL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmSwp& aSwp)
	{
	RDebug::Printf("CCmdTestList::AddReqSwpChangeCommandL");
	SSMLOGLEAVEIFNULL(iCommandList);	
	CSsmCommandBase* cmd = CCmdReqSwpChange::NewL(aSeverity, aExecutionBehaviour, aSwp);
	CleanupStack::PushL(cmd);
	static_cast<MSsmCommandList&>(*iCommandList).AppendL(cmd);
	if (cmd->IsDeferred() && (ESsmDeferredWaitForSignal == cmd->ExecutionBehaviour()))
		{
		RDebug::Printf("Test Adding deferred command to list...");
		iDeferredList.AppendL(cmd);
		}
	CleanupStack::Pop(cmd);
	}

void CCmdTestList::AddPublishSystemStateCommandL(TCmdErrorSeverity aSeverity, TSsmExecutionBehaviour aExecutionBehaviour, TSsmPublishSystemStateInfo& aInfo)
	{
	RDebug::Printf("CCmdTestList::AddPublishSystemStateCommandL");
	SSMLOGLEAVEIFNULL(iCommandList);	
	CSsmCommandBase* cmd = CCmdPublishSystemState::NewL(aSeverity, aExecutionBehaviour, aInfo);
	CleanupStack::PushL(cmd);
	static_cast<MSsmCommandList&>(*iCommandList).AppendL(cmd);
	if (cmd->IsDeferred() && (ESsmDeferredWaitForSignal == cmd->ExecutionBehaviour()))
		{
		RDebug::Printf("Test Adding deferred command to list...");
		iDeferredList.AppendL(cmd);
		}
	CleanupStack::Pop(cmd);
	}

void CCmdTestList::AddCreateSwpCommandL(TCmdErrorSeverity aSeverity, TSsmSwp& aSwpInfo, TDesC& aFilename)
	{
	RDebug::Printf("CCmdTestList::AddCreateSwpCommandL");
	SSMLOGLEAVEIFNULL(iCommandList);	

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	MSsmCommand* cmd = SsmCommandFactory::ConstructCreateSwpCommandLC(aSeverity, aSwpInfo, aFilename, KDefaultCommandPriority);
#else
	CSsmCommandBase* cmd = CCmdCreateSwp::NewL(aSeverity, aSwpInfo, aFilename);
	CleanupStack::PushL(cmd);
#endif
	static_cast<MSsmCommandList&>(*iCommandList).AppendL(cmd);
	if (cmd->IsDeferred() && (ESsmDeferredWaitForSignal == cmd->ExecutionBehaviour()))
		{
		RDebug::Printf("Test Adding deferred command to list...");
		iDeferredList.AppendL(cmd);
		}
	CleanupStack::Pop(cmd);
	}

void CCmdTestList::AddSetPAndSKeyCommandL(TCmdErrorSeverity aSeverity, TUid aCategory, TUint aKey, TInt aValue)
	{
	RDebug::Printf("CCmdTestList::AddSetPAndSKeyCommandL");
	SSMLOGLEAVEIFNULL(iCommandList);	

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	MSsmCommand* cmd = SsmCommandFactory::ConstructSetPAndSKeyCommandLC(aSeverity, aCategory, aKey, aValue, KDefaultCommandPriority);
#else
	CSsmCommandBase* cmd = CCmdSetPAndSKey::NewL(aSeverity, aCategory, aKey, aValue);
	CleanupStack::PushL(cmd);
#endif
	static_cast<MSsmCommandList&>(*iCommandList).AppendL(cmd);
	if (cmd->IsDeferred() && (ESsmDeferredWaitForSignal == cmd->ExecutionBehaviour()))
		{
		RDebug::Printf("Test Adding deferred command to list...");
		iDeferredList.AppendL(cmd);
		}
	CleanupStack::Pop(cmd);
	}

void CCmdTestList::AddCommandL(TSsmCommandType aType, RReadStream& aReadStream, RFs* /*aRfs*/)
	{
	RDebug::Printf("CCmdTestList::AddCommand");
	SSMLOGLEAVEIFNULL(iCommandList);	
	TArray<MSsmCommand*> deferredList = iDeferredList.Array();
	CSsmCommandBase* cmd = SsmCommandFactory::ConstructCommandFromStreamLC(aType, aReadStream, deferredList);
	cmd->SetUtilProvider(*iUtilProvider);
	static_cast<MSsmCommandList&>(*iCommandList).AppendL(cmd);
	if (cmd->IsDeferred() && (ESsmDeferredWaitForSignal == cmd->ExecutionBehaviour()))
		{
		RDebug::Printf("Test Adding deferred command to list...");
		iDeferredList.AppendL(cmd);
		}
	CleanupStack::Pop(cmd);
	}

void CCmdTestList::AddCommandFromResourceL(TSsmCommandType aType, TSsmCommandParameters& aCommandParameters)
	{
	RDebug::Printf("CCmdTestList::AddCommandFromResourceL");
	SSMLOGLEAVEIFNULL(iCommandList);	
	CSsmCommandBase* cmd = SsmCommandFactory::ConstructCommandFromResourceLC(aType, aCommandParameters);
	cmd->SetUtilProvider(*iUtilProvider);
	static_cast<MSsmCommandList&>(*iCommandList).AppendL(cmd);
	if (cmd->IsDeferred() && (ESsmDeferredWaitForSignal == cmd->ExecutionBehaviour()))
		{
		RDebug::Printf("Test Adding deferred command to list...");
		iDeferredList.AppendL(cmd);
		}
	CleanupStack::Pop(cmd);
	}

void CCmdTestList::CancelAllCommands()
	{
	RDebug::Printf("CCmdTestList::CancelAllCommands");

	TInt count = iCommandList->Count();
	for (TInt i=0;i<count;i++)
		{
		iCommandList->ExecuteCancel(i);
		}
	}


//
// From CActive
//
void CCmdTestList::RunL()
	{
	 RDebug::Printf("CCmdTestList::RunL");
	
	// set the failure code and exit processing
	if (iStatus.Int() != KErrNone)
		{
		iErrorcode = iStatus.Int();
		DEBUGPRINT2A("Error during processing: %d", iErrorcode);
		}	
		
	if (iCommandIndex < iCommandList->Count())
		{
		DEBUGPRINT2A("Call Execute on cmd %d", iCommandIndex);
		TSsmCommandType type = (*iCommandList)[iCommandIndex]->Type();	
		iCommandList->Execute(iCommandIndex++, iStatus);	// execute the current command
		SetActive();
		}
	else
		{
		RDebug::Printf("No more tests");
		if (iAddStop && iCallback)
			{
			RDebug::Printf("Test Adding Stop Scheduler callback...");
			(iCallback)(iParam);
			}
		}
	}

void CCmdTestList::DoCancel()
	{
	RDebug::Printf("CCmdTestList::DoCancel");

	TInt count = iCommandList->Count();
	for (TInt i=0;i<count;i++)
		{
		DEBUGPRINT2A("Cancelling command %d", i);
		iCommandList->ExecuteCancel(i);
		}
	iCommandList->DeferredDelete();
	CompleteExecuteRequest(KErrCancel);
	}

TInt CCmdTestList::RunError(TInt aError)
	{
	DEBUGVERBOSE2A("CCmdTestList::RunError called with %d", aError);
	CompleteExecuteRequest(aError);
	return KErrNone;
	}


//
// PRIVATE METHODS
//
CCmdTestList::CCmdTestList()
	: CActive(EPriorityHigh), iAddStop(EFalse)
	{
	CActiveScheduler::Add(this);
	}

void CCmdTestList::ConstructL(TCmdCallback* aCallback, TAny* aParam)
	{
	RDebug::Printf("CCmdTestList::ConstructL");
	iCommandList = CSsmCommandList::NewL();
	iCommandIndex = 0;
	iErrorcode = KErrNone;
	iUtilProvider = CSsmCommandUtilProvider::NewL();

	if (aCallback)
		{
		iCallback = *aCallback;
		iParam = aParam;
		iAddStop = ETrue;
		}

	TRequestStatus* trs = &iStatus;
	User::RequestComplete(trs, KErrNone);
	SetActive();
	}

void CCmdTestList::CompleteExecuteRequest(TInt aReason)
	{
	if (iStatus.Int() == KRequestPending)
		{
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, aReason);
		}

	iErrorcode = aReason;
	}
