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

#include <s32strm.h>
#include "ssmcommandparameters.h"
#include "ssmcommandutilprovider.h"
#include <ssm/ssmstartupproperties.h>
#include "ssmdebug.h"
#include "ssmpanic.h"

#include "cmdstartprocess.h"
#include "cmdstartapp.h"

/**
Destructor
*/
CCmdStarterBase::~CCmdStarterBase()
	{
	Cancel();
	delete iAppInfo;
	iProcess.Close();
	}

/**
Execute this command
@param aStatus 	Completed to indicate execution of the next command may begin.	
@panic ECmdNullPtr if the information used to create command is null	
*/
void CCmdStarterBase::Execute(TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iUtilProvider, PanicNow(KPanicCmdStarterBase, ECmdNullPtr));
	__ASSERT_ALWAYS(iAppInfo, PanicNow(KPanicCmdStarterBase, ECmdNullPtr));

	aStatus = KRequestPending;
	iExecuteRequest = &aStatus;
	TPtrC fileName = iAppInfo->FileName();
	TPtrC args = iAppInfo->Args();
	iExecutionBehaviour = iAppInfo->ExecutionBehaviour();
	DEBUGPRINT4(_L("Starting : %S with arguments : %S and Execution behaviour : %d"), &fileName, &args, iExecutionBehaviour);

	iUtilProvider->StartSafe().Start(*iAppInfo, iProcess, iStatus, iCallerIndex);
	if (iExecutionBehaviour == ESsmDeferredWaitForSignal)
		{
		// complete the command request and initiate execution of next command
		CompleteExecuteRequest(KErrNone);
		}
	SetActive();
	}

/**
Releases resources associated with this BIC
*/
void CCmdStarterBase::Release()
	{
	delete this;
	}

/**
Cancels the BIC
*/
void CCmdStarterBase::ExecuteCancel()
	{
	//cancel the outstanding request
	Cancel();
	//and complete the request with KErrCancel
	CompleteDeferredExecuteRequest(KErrCancel);
	}

/**
Returns the type of the BIC
@return Type of BIC
*/
TSsmCommandType CCmdStarterBase::Type() const
	{
	return iCommandType;
	}
/**
*/
void CCmdStarterBase::ExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteInt16L(iSeverity);
	iAppInfo->ExternalizeL(aWriteStream);
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	aWriteStream.WriteUint16L(iPriority);
#endif
	}

/**
Handles command's request completion event.
Request for ESsmDeferredWaitForSignal & ESsmFireAndForget is completed in 
Execute() but completion code will be completed here.
*/
void CCmdStarterBase::RunL()
	{
	TInt err = iStatus.Int();
	TPtrC fileName = iAppInfo->FileName();
	DEBUGPRINT3(_L("%S started with err : %d "), &fileName, err);
	SSMLOGLEAVEIFERROR(err);

	// start monitor if required, for cmds with execution behaviour = ESsmWaitForSignal/ESsmDeferredWaitForSignal
	// Monitor can not be started for ESsmFireAndForget
	if ((iAppInfo->ExecutionBehaviour() != ESsmFireAndForget) && iAppInfo->IsMonitoringRequired())
		{
		// sysmon doesn't monitor the applications with deferredwaitforsignal
		// execution behaviour, so changing it to waitforsignal 
		if (iAppInfo->ExecutionBehaviour() == ESsmDeferredWaitForSignal)
			{
			iAppInfo->SetExecutionBehaviour(ESsmWaitForSignal);
			}
		DEBUGPRINT2(_L("Starting monitor for %S"), &fileName);
		iUtilProvider->SysMonSessionL().MonitorL(*iAppInfo, iProcess);
		}

	CompleteDeferredExecuteRequest(err);
	}

/**
Cancellation of an outstanding request.
*/
void CCmdStarterBase::DoCancel()
	{
	iUtilProvider->StartSafe().StartCancel(iCallerIndex);
	}

/**
Complete the command request with aError. It gets called when RunL leaves.
@param aError Error code with which RunL has left.
@return KErrNone as error is handled in this function.
*/
TInt CCmdStarterBase::RunError(TInt aError)
	{
	DEBUGPRINT2A("CCmdStarterBase::RunError called with reason %d", aError);
	CompleteDeferredExecuteRequest(aError);
	return KErrNone;
	}

/**
Constructs the object through read stream.
@param aReadStream A read stream containing BIC data
*/
void CCmdStarterBase::ConstructL(RReadStream& aReadStream)
	{
	iAppInfo = CSsmStartupProperties::NewL();
	InternalizeL(aReadStream);
	ValidateL();	//validate command's data
	}

/**
Constructs an object from resource file.
@param aCommandParameters Object data from a resource file
*/
void CCmdStarterBase::ConstructL(TSsmCommandParameters& aCommandParameters)
	{
	RResourceReader& reader = aCommandParameters.MainReader();
	iSeverity = static_cast<TCmdErrorSeverity>(reader.ReadInt16L());
	iAppInfo = CSsmStartupProperties::NewL(reader, aCommandParameters.ResourceFile(), Type());
	iExecutionBehaviour = iAppInfo->ExecutionBehaviour();
	ValidateL();	//validate command's data
	}


/**
Constructs an object from aAppInfo.
@param aAppInfo Startup properties through which command will be constructed.
*/
void CCmdStarterBase::ConstructL(const CSsmStartupProperties* aAppInfo)
	{
	SSMLOGLEAVEIFTRUE(aAppInfo == NULL, KErrArgument);
	iAppInfo = CSsmStartupProperties::NewL(*aAppInfo);
	iExecutionBehaviour = iAppInfo->ExecutionBehaviour();
	ValidateL();	//validate command's data
	}
/**
Constructs an object from aAppInfo, Must be used only by CLE
@param aAppInfo Startup properties through which command will be constructed.
@param aFalse is dummy variable to bypass the validation of command
*/
void CCmdStarterBase::ConstructL(const CSsmStartupProperties *aAppInfo, TBool /*aFalse*/)
    {
    SSMLOGLEAVEIFTRUE(aAppInfo == NULL, KErrArgument);
    iAppInfo = CSsmStartupProperties::NewL(*aAppInfo);
    iExecutionBehaviour = iAppInfo->ExecutionBehaviour();
    }

/**
Overloaded CTOR
@param aCommandType Command type, It can be either ESsmCmdStartApp or ESsmCmdStartProcess
*/
CCmdStarterBase::CCmdStarterBase(TSsmCommandType aCommandType)
	: iCommandType(aCommandType)
	{
	}

#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
/**
Overloaded CTOR
@param aCommandType Command type, It can be either ESsmCmdStartApp or ESsmCmdStartProcess
@param aSeverity Severity of the command
@param aPriority The priority of the command in the list
*/
CCmdStarterBase::CCmdStarterBase(TSsmCommandType aCommandType, TCmdErrorSeverity aSeverity, const TUint16 aPriority)
	: CSsmDeferrableCommand(aSeverity, aPriority), iCommandType(aCommandType)
	{
	}
#endif

/**
Overloaded CTOR
@param aCommandType Command type, It can be either ESsmCmdStartApp or ESsmCmdStartProcess
@param aSeverity Severity of the command
*/
CCmdStarterBase::CCmdStarterBase(TSsmCommandType aCommandType, TCmdErrorSeverity aSeverity)
	: CSsmDeferrableCommand(aSeverity), iCommandType(aCommandType)
	{
	}

/**
Configures the BIC using data contained in a ReadStream
@param aReadStream 	A read stream containing BIC data
*/
void CCmdStarterBase::InternalizeL(RReadStream& aReadStream)
	{
	iSeverity = static_cast<TCmdErrorSeverity>(aReadStream.ReadInt16L());
	iAppInfo->InternalizeL(aReadStream);
	iExecutionBehaviour = iAppInfo->ExecutionBehaviour();
#ifdef SYMBIAN_SSM_FLEXIBLE_MERGE
	iPriority = aReadStream.ReadUint16L();
#endif
	}

/**
Helper function to validate commands data.
@leave KErrArgument If execution behaviour is wrong or FileName() of AppInfo is NULL.
*/
void CCmdStarterBase::ValidateL()
	{
	CSsmDeferrableCommand::ValidateL();
	SSMLOGLEAVEIFTRUE(iAppInfo->Retries() < 0, KErrArgument);
	SSMLOGLEAVEIFTRUE(iAppInfo->FileName() == KNullDesC, KErrArgument);
	}
