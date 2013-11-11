// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tcustcmd_swppolicy_customcmd.h"      
#include <ssm/ssmcustomcommandinfo.h>
#include <ssm/ssmcommandlist.h>
#include <e32debug.h>
#include <e32property.h>
/**
This dll will be used by integration test code of custom command for suporting the command transition.
*/	

enum TestSwpValForCustCmd
	{
	//create and execute ActivateEmergencyCall custom command
	EValForActivateEmergencyCall = 11,
	//create and execute DeActivateEmergencyCall custom command
	EValForDeActivateEmergencyCall = 22,
	//create and execute security custom command with execution behaviour WaitForSignal
	EValForSecCustCmdWaitForSignal = 33,
	//create and execute security custom command with execution behaviour DeferredWaitForSignal
	EValForSecCustCmdDeferrdWaitForSignal = 44,
	//create and execute security custom command with execution behaviour fireandforget
	EValForSecCustCmdFireAndForget = 55,
	//create and execute Device security custom command with execution behaviour WaitForSignal
	EValForDeviceSecCustCmdWaitForSignal = 333,
	//create and execute Device security custom command with execution behaviour DeferredWaitForSignal
	EValForDeviceSecCustCmdDeferredWaitForSignal = 444,
	//create and execute Device security custom command with execution behaviour fireandforget
	EValForDeviceSecCustCmdFireAndForget = 555,
	//create and execute CmdCheckUserDrive custom command
	EValForCmdCheckUserDrive = 66,
	//create and execute CmdInitRamDrive custom command
	EValForCmdInitRamDrive = 77,
	//create and execute CmdDeleteTempFiles custom command
	EValForCmdDeleteTempFiles = 88,
	//create and execute CmdClearStartupReason custom command
	EValForCmdCmdClearStartupReason = 99,
	//create and execute CmdValidateRTC custom command
	EValForCmdCmdValidateRTC = 100
	};
	
EXPORT_C MSsmSwpPolicy* CTestSsmSwpCustomCmd::NewL()
	{
	CTestSsmSwpCustomCmd* self = new (ELeave) CTestSsmSwpCustomCmd;
	return self;
	}
 
CTestSsmSwpCustomCmd::CTestSsmSwpCustomCmd()
	{
	}

CTestSsmSwpCustomCmd::~CTestSsmSwpCustomCmd()
	{
	delete iCommandList;
	}

void CTestSsmSwpCustomCmd::Initialize(TRequestStatus& aStatus)
	{
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}

void CTestSsmSwpCustomCmd::InitializeCancel()
	{
	}

void CTestSsmSwpCustomCmd::Release()
	{
	delete this;
	}

MSsmSwpPolicy::TResponse CTestSsmSwpCustomCmd::TransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage)
	{
	(void)aSwp;
	(void)aMessage; 
	return EAllowed; 
	}

void CTestSsmSwpCustomCmd::PrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus)
	{
	TRAPD(err, doPrepareCommandListL(aSwp));

	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, err);
	}

void CTestSsmSwpCustomCmd::doPrepareCommandListL(TSsmSwp aSwp)
	{
	const TUid KPropertyCategory={0x2000D75B};				// tcustomcmd_server SID = KSsmServerName SID (changed in tcustomcmd_server.mmp file)
	const TUid KCustomcmdServerSID = KPropertyCategory;

	TInt value = 0;
	TInt err = RProperty::Get(KCustomcmdServerSID, aSwp.Key(), value);

	// reset the command list
	delete iCommandList;
	iCommandList = CSsmCommandList::NewL();
	
	_LIT(KDllName, "customcmds.dll");
	TPtrC filename = KDllName();
	TInt32 ordinal = 0;
	const TInt KSecurityCustCmdOrdinal = 1;
	const TInt KDeviceSecurityCustCmdOrdinal = 11;
	const TInt KActivateEmergencyCallOrdinal = 4;
	const TInt KDeActivateEmergencyCallOrdinal = 6;
	const TInt KCmdCheckUserDrive = 13;
	const TInt KCmdInitRamDrive = 14;
	const TInt KCmdDeleteTempFiles = 15;
	const TInt KCmdClearStartupReason = 16;
	const TInt KCmdValidateRTC = 12;
	
	switch(value)
		{
		case EValForActivateEmergencyCall:
			ordinal = KActivateEmergencyCallOrdinal;		
			iExecutionBehaviour = ESsmWaitForSignal;
		break;
		case EValForDeActivateEmergencyCall:
			ordinal = KDeActivateEmergencyCallOrdinal;		
			iExecutionBehaviour = ESsmWaitForSignal;		
		break;
		case EValForSecCustCmdWaitForSignal:
			ordinal = KSecurityCustCmdOrdinal;		
			iExecutionBehaviour = ESsmWaitForSignal;
		break;
		case EValForSecCustCmdDeferrdWaitForSignal:
			ordinal = KSecurityCustCmdOrdinal;		
			iExecutionBehaviour = ESsmDeferredWaitForSignal;	
		break;
		case EValForSecCustCmdFireAndForget:
			ordinal = KSecurityCustCmdOrdinal;		
			iExecutionBehaviour = ESsmFireAndForget;	
		break;
		case EValForDeviceSecCustCmdWaitForSignal:
			ordinal = KDeviceSecurityCustCmdOrdinal;		
			iExecutionBehaviour = ESsmWaitForSignal;
		break;
		case EValForDeviceSecCustCmdDeferredWaitForSignal:
			ordinal = KDeviceSecurityCustCmdOrdinal;		
			iExecutionBehaviour = ESsmDeferredWaitForSignal;	
		break;
		case EValForDeviceSecCustCmdFireAndForget:
			ordinal = KDeviceSecurityCustCmdOrdinal;		
			iExecutionBehaviour = ESsmFireAndForget;	
		break;
		case EValForCmdCheckUserDrive:
			ordinal = KCmdCheckUserDrive;		
			iExecutionBehaviour = ESsmWaitForSignal;	
		break;
		case EValForCmdInitRamDrive:
		     ordinal = KCmdInitRamDrive;       
		     iExecutionBehaviour = ESsmWaitForSignal;    
		break;
		case EValForCmdDeleteTempFiles:
		     ordinal = KCmdDeleteTempFiles;       
		     iExecutionBehaviour = ESsmWaitForSignal;    
		break;	
		case EValForCmdCmdClearStartupReason:
		     ordinal = KCmdClearStartupReason;       
		     iExecutionBehaviour = ESsmWaitForSignal;    
		break; 
		case EValForCmdCmdValidateRTC :
		     ordinal = KCmdValidateRTC ;       
		     iExecutionBehaviour = ESsmWaitForSignal;    
		break;  

		default:
		break;	
		}

	TPtrC8 params = KNullDesC8();
	TCmdCustomCommandLibUnloading unloading = EUnloadOnCommandCompletion;
	TInt retries = 1;

	CSsmCustomCommandInfo* customInfo = new (ELeave) CSsmCustomCommandInfo();
	CleanupStack::PushL(customInfo);
	customInfo->SetL(filename, ordinal, unloading, retries, params);

	// Make sure that the publish swp command is the first one in the command list (Symbian's recomendation, but not important here)
	MSsmCommand* cmd1 = SsmCommandFactory::ConstructPublishSwpCommandLC(ECmdCriticalSeverity, iExecutionBehaviour, aSwp);
	MSsmCommand* cmd2 = SsmCommandFactory::ConstructCustomCommandCommandLC(ECmdIgnoreFailure, iExecutionBehaviour, *customInfo);
	iCommandList->AppendL(cmd1);
	iCommandList->AppendL(cmd2);
	if(iExecutionBehaviour == ESsmDeferredWaitForSignal)
		{
		const TInt timeout = 10000; //some random timeout for the multiple_wait cmd
		MSsmCommand* cmd3 = SsmCommandFactory::ConstructMultipleWaitCommandLC(timeout);
		iCommandList->AppendL(cmd3);
		CleanupStack::Pop(cmd3);
		}
	CleanupStack::Pop(2);	//cmd2,cmd1		
	// clean-up
	CleanupStack::PopAndDestroy(customInfo);
	}

void CTestSsmSwpCustomCmd::PrepareCommandListCancel()
	{
 	delete iCommandList;
	}

CSsmCommandList* CTestSsmSwpCustomCmd::CommandList()
	{
	CSsmCommandList* list = iCommandList;
	iCommandList = NULL;
	return list;
	}

void CTestSsmSwpCustomCmd::HandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus)
	{
	(void)aSwp;
	(void)aSeverity;

   	RDebug::Printf("--- CTestSsmSwpCustomCmd::HandleCleReturnValue received value %d from Cle", aError);

	TRequestStatus* rs = &aStatus;
	const TUid KPropertyCategory={0x2000D75B};				// tcustomcmd_server SID = KSsmServerName SID (changed in tcustomcmd_server.mmp file)
	const TUid KCustomcmdServerSID = KPropertyCategory;
	const TUint KCustomcmdServerPropertyKey = 0x0012AC;
	if(iExecutionBehaviour == ESsmWaitForSignal )
		{
		// Set this property key to indicate to stop the active scheduler
 		TInt err = RProperty::Set(KCustomcmdServerSID, KCustomcmdServerPropertyKey, ESwpValForStopSchedulerCmdWaitForSignal);		
		}
	else if(iExecutionBehaviour == ESsmDeferredWaitForSignal )
		{
		// Set this swp to indicate to stop the active scheduler
		TInt err = RProperty::Set(KCustomcmdServerSID, KCustomcmdServerPropertyKey, ESwpValForStopSchedulerCmdDeferredWaitForSignal);
		}	
	User::RequestComplete(rs, KErrNone);	
	}

void CTestSsmSwpCustomCmd::HandleCleReturnValueCancel()
	{
	}	

