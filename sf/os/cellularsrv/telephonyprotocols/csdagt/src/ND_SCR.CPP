// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Script Control definitions
// 
//

/**
 @file Nd_scr.cpp 
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ND_SCRTraces.h"
#endif

#include "ND_SCR.H"
#include "ND_STD.H"
#include "SSCREXEC.H"
#include "ND_DBACC.H"


_LIT(KLoginNameVar,"login_name$");
_LIT(KLoginPassVar,"login_pass$");
_LIT(KLoginIpaddrVar,"login_ipaddr$");

// CNetDialScript definitions

CNetDialScript* CNetDialScript::NewL(CCommsDbNetDialAccess* aDb,CDialogProcessor* aDlgPrc,RCall::TCommPort& aCommPort,TCommRole aRole,TInt aLength)
/**
2 phased constructor for CNetDialScript, first phase.

@param aDb a pointer to CommDB accessor.
@param aDlgPrc a pointer to dialog processor.
@param aCommPort is loaned COMM port from ETel server.
@param aLength is lenght for used script.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CNetDialScript object.
*/
	{
	CNetDialScript* r=new(ELeave) CNetDialScript(aDb,aDlgPrc,aLength);
	CleanupStack::PushL(r);
	r->ConstructL(aCommPort,aRole);
	CleanupStack::Pop();
	return r;
	}

CNetDialScript::CNetDialScript(CCommsDbNetDialAccess* aDb,CDialogProcessor* aDlgPrc,TInt aLength) 
	: CActive(EPriorityStandard), iState(EIdle), iDb(aDb), iDlgPrc(aDlgPrc), iUsePct(EFalse), iLength(aLength)
/**
Private constructor for CNetDialScript, used in the first phase of construction.

@param aDb a pointer to CommDB accessor.
@param aDlgPrc is pointer to dialog processor.
@param aLength is length of the used script.
*/
	{}

void CNetDialScript::ConstructL(RCall::TCommPort& aCommPort,TCommRole aRole)
/**
Instantiate Member variable.
Add this object into active scheduler.
Create script executor.

@param aCommPort a loaned COMM port from ETel server.
@exception Leaves if CScriptExecutor::NewL() leaves.
*/
	{
	CActiveScheduler::Add(this);
	iScriptExecutor=CScriptExecutor::NewL(this,aCommPort.iPort,iLength);
	(void) aRole;
	GetBcaStack();
	GetIapIdL();
	}

CNetDialScript::~CNetDialScript()
/**
Destructor.
Cancel active requests.
If script executor is used, call Disconnect() to close it.
Delete script executor.
*/
	{
	Cancel();
	if(iScriptExecutor!=NULL)
		iScriptExecutor->Disconnect();
	delete iScriptExecutor;
	}

void CNetDialScript::ReConfigureAndCancelCommPort(TRequestStatus& aStatus)
/**
Request script executor to reconfigure COMM port.
*/
	{
	iScriptExecutor->ReConfigureAndCancelCommPort(aStatus);
	}

void CNetDialScript::DropSignals(TRequestStatus& aStatus)
/**
Request script executor to Drop signals on COMM port.
*/
	{
	iScriptExecutor->DropSignals(aStatus);
	}

void CNetDialScript::GetBcaStack()
	{
	TRAPD(ret, iDb->GetBcaStackL(iBcaStack););
	if (ret != KErrNone || iBcaStack.Length() == 0) //Well it looks like there was not BcaStack therefore
		{
		_LIT(KDefaultBcaStack, "c32bca");
		iBcaStack.Copy(KDefaultBcaStack());
		}
	}

void CNetDialScript::GetIapIdL()
	{
	TInt iapOrError = iDb->GetIapId();
	if (iapOrError == 0)
		{
		User::Leave(KErrNetDialDatabaseDefaultUndefined);
		}
	else
	if (iapOrError > 0)
		{
		iIapId = iapOrError;
		}
	else
		{
		User::Leave(iapOrError);
		}
	}

void CNetDialScript::CreateChannel(TRequestStatus& aStatus)
	{
	ASSERT(iScriptExecutor);
	iScriptExecutor->CreateChannel(aStatus);
	}

void CNetDialScript::CancelCreateChannel()
	{
	ASSERT(iScriptExecutor);
	iScriptExecutor->CancelCreateChannel();
	}

void CNetDialScript::ShutdownChannel(TRequestStatus& aStatus)
	{
	ASSERT(iScriptExecutor);
	iScriptExecutor->ShutdownChannel(aStatus);
	}


void CNetDialScript::Scan(MNetDialScriptObserver& aObserver)
/**
Starts script scanning.
Scan is synchronous, so just go straight to the RunL() and do it.

@param aObserver a reference to script observer.
*/
	{
	__ASSERT_DEBUG(iLength>0, NetDialPanic(EZeroLengthScript));
	__ASSERT_DEBUG(iState==EIdle, User::Invariant());

	iCurrentObserver=&aObserver;
	iStatus=KRequestPending;
	SetActive();
	iState=EScanScript;
	ScriptOperationComplete(KErrNone);
	}

TBool CNetDialScript::GetLoginParamsL(MNetDialScriptObserver& aObserver, TBool aIsReconnect)
/**
Get login parameters. Assumes that there is a non-zero length script.
Check from CommDB accessor if login dialog is shown, what are the login name and password and IP address.
If the login params are in script, do not request them from user.
If dialog is used, get login parameters from dialog processor, set object active and return EFalse.
Else set state idle and return ETrue as login params should 

@param aObserver a reference to login observer.
@return EFalse if the request is not complete and ETrue if the request is already complete.
@exception Leaves if GetLoginParamsL() leaves, panics if script length is zero or iState is not idle.
*/
	{
	__ASSERT_DEBUG(iLength>0, NetDialPanic(EZeroLengthScript));
	__ASSERT_DEBUG(iState==EIdle, User::Invariant());

	iCurrentObserver=&aObserver;
	iDb->GetLoginParamsL(iUseLoginDialog,iLoginName,iLoginPass,iIPAddress);
	if (iScriptExecutor->ReadFound())
		iUseLoginDialog=EFalse;			// don't use prompt for password there is a READ command
	if (iUseLoginDialog)
		{
		iDlgPrc->Login(*this,iLoginName,iLoginPass,aIsReconnect);
		iStatus=KRequestPending;
		SetActive();
		iState=EGetLogin;
		return EFalse;
		}
	else
		{
		iState=EIdle;
		return ETrue;
		}
	}

void CNetDialScript::ExecuteL(MNetDialScriptObserver& aObserver)
/**
Set up login script variables and execute script (will be valid if called).
Get script from CommDB accessor and set it for script executor.
Get login name, password and IP address from CommDB.
If PCT is used, call OpenPct(), call DestroyPctNotification() to end notifications and set login params for script executor.
Call RunScript() to start the script.
Set object active and iState to EExecute.

@param aObserver a reference to script observer.
@exception Leaves if GetAndSetScriptL() or SetLoginVarsL() leaves, panics if script length is zero or iState is not idle.
*/
	{
	__ASSERT_DEBUG(iLength>0, NetDialPanic(EZeroLengthScript));
	__ASSERT_DEBUG(iState==EIdle, User::Invariant());
	
	iCurrentObserver=&aObserver;
	GetAndSetScriptL();
	SetLoginVarsL();
	if (iUsePct)
		{
		User::LeaveIfError(OpenPct());
		DestroyPctNotification();
		iScriptExecutor->SetLoginParams(iLoginName,iLoginPass);		// required for test dialog server
		}	
	iScriptExecutor->RunScript();
	iStatus=KRequestPending;
	SetActive();
	iState=EExecute;
	}

void CNetDialScript::CloseScript()
/**
Close script.
Call CloseScript() from script executor.
*/
	{
	iScriptExecutor->CloseScript();
	}

void CNetDialScript::CleanupScript()
/**
Cleanup script.
Call Close() from script executor.
*/
	{
	iScriptExecutor->Close();
	}

void CNetDialScript::ScriptOperationComplete(TInt aError)
/**
Completes script request.
*/
	{
	TRequestStatus* statusPtr=&iStatus;
	User::RequestComplete(statusPtr,aError);
	}

void CNetDialScript::ConfigureCommPort(TRequestStatus& aStatus, const TCommConfig& aConfig)
/**
Configures COMM port.
Call ConfigureCommPort() from script executor.

@param aConfig a reference to COMM port.
@return error code for configuration.
*/
	{
	iScriptExecutor->ConfigureCommPort(aStatus, aConfig);
	}

void CNetDialScript::CancelConfigureCommPort()
/**
Cancels Configures COMM port.
Call CancelConfigureCommPort() from script executor.
*/
	{
	iScriptExecutor->CancelConfigureCommPort();
	}


TInt CNetDialScript::GetExcessData(TDes8& aBuffer)
/**
Get excess data.
Call GetExcessData() from script executor.

@param aBuffer a reference for excess data.
@return error code for GetExcessData() request.
*/
	{
	return iScriptExecutor->GetExcessData(aBuffer);
	}

void CNetDialScript::DropDTR(TRequestStatus* aStatusPtr)
/**
Drop DTR.
Call DropDTR() from script executor.
*/
	{
	iScriptExecutor->DropDTR(aStatusPtr);
	}


TInt CNetDialScript::OpenPct()
/**
Open PCT.
Call OpenPct() from dialog processor.

@return error code for OpenPct() request.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CNETDIALSCRIPT_OPENPCT_1, "NetDial:\tOpening PCT");

	const TInt ret = iDlgPrc->OpenPct();
	if (ret == KErrNone)
		iPctOpen = ETrue;
	return ret;
	}

void CNetDialScript::DestroyPctNotification()
/**
Destroy PCT notification.
Call DestroyPctNotification() from dialog processor.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CNETDIALSCRIPT_DESTROYPCTNOTIFICATION_1, "NetDial:\tRequesting Destroy PCT Notification");
	
	iDlgPrc->DestroyPctNotification(*this);
	}	

TInt CNetDialScript::WritePct(TDes& aBuffer)
/**
Write incoming data into PCT.

@param aBuffer a reference for incoming data.
@return error code for WritePct() request.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CNETDIALSCRIPT_WRITEPCT_1, "NetDial:\tWriting To PCT");
	
	return iDlgPrc->WritePct(aBuffer);
	}	

void CNetDialScript::ReadPct(TDes& aBuffer)
/**
Read data from PCT.

@param aBuffer a reference for read data.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CNETDIALSCRIPT_READPCT_1, "NetDial:\tReading From PCT");
	
	iDlgPrc->ReadPct(*this,aBuffer);
	}

void CNetDialScript::ClosePct()
/**
Close PCT.
If PCT is open, call ClosePct() from dialog processor 
*/
	{
	if (iPctOpen)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CNETDIALSCRIPT_CLOSEPCT_1, "NetDial:\tClosing PCT");
	
		iDlgPrc->ClosePct();
		iPctOpen = EFalse;
		iUsePct = EFalse;
		}
	}

void CNetDialScript::CancelDialogServer()
/**
Cancel requests.
Call CancelEverything() from dialog processor.
*/
	{
	iDlgPrc->CancelEverything();
	}

void CNetDialScript::MDPOLoginComplete(TInt aError)
/**
Login complete.
Complete request with aError.

@param aError a error code for completition.
*/
	{
	TRequestStatus* statusPtr=&iStatus;
	User::RequestComplete(statusPtr,aError);
	}

void CNetDialScript::MDPOReadPctComplete(TInt aError)
/**
PCT read complete.
Call ReadPctComplete() from script executor with aError to complete the ReadPct() request.

@param aError a error code for completition.
*/
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS,CNETDIALSCRIPT_MDPOREADPCTCOMPLETE_1,"NetDial:\tRead PCT Complete with Error %d",aError);
	iScriptExecutor->ReadPctComplete(aError);
	}
void CNetDialScript::MDPODestroyPctComplete(TInt aError)
/**
Destroy PCT complete.
Call DestroyPctNotificationReceived() from script executor with aError to complete the DestroyPctNotification() request.

@param aError a error code for completition.
*/
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CNETDIALSCRIPT_MDPODESTROYPCTCOMPLETE_1, "NetDial:\tDestroy PCT Notification Complete with Error %d",aError);

	iScriptExecutor->DestroyPctNotificationReceived(aError);
	}

void CNetDialScript::RunL()
/**
RunL - Request completed.
If iState is EScanScript, call DoScanScriptL() and trap error.
Else copy iStatus.Int() into ret variable.
Call ScriptFunctionComplete() with ret from current observer.
Set state to EIdle.
*/
	{
	__ASSERT_DEBUG(iCurrentObserver!=NULL, User::Invariant());
	__ASSERT_DEBUG(((iState==EScanScript) || (iState==EExecute) || (iState==EGetLogin)), User::Invariant());

	TInt ret=KErrNone;
	if (iState==EScanScript)
		{
		TRAP(ret,DoScanScriptL());
		}
	else
		{
		ret=iStatus.Int();
		}

	iCurrentObserver->ScriptFunctionComplete(ret);
	iState=EIdle;
	}

void CNetDialScript::DoCancel()
/**
Cancels active requests.
Call Cancel() from script executor.
If request is pending, call ScriptFunctionComplete() with KErrCancel from current observer.
*/
	{
	iScriptExecutor->Cancel();
	if(iStatus==KRequestPending)
		{
		iCurrentObserver->ScriptFunctionComplete(KErrCancel);
		}
	}

void CNetDialScript::DoScanScriptL()
/**
Scan the script for a READ command.
Call GetUsePctL() from CommDB accessor to find out if PCT is used.
Call GetAndSetScript() to get script from CommDB accessor and set it for script executor.
Call ScanScriptL() from script executor to start scanning.
If PCT is not used and ReadFound() returns ETrue, set iUsePct to ETrue.
We need to know if there is a READ whether or not the PCT flag is set to see if we need the login dialog.

@exception Leaves if GetUsePctL(), GetAndSetScriptL() or ScanScriptL() leaves.
*/
	{
	iDb->GetUsePctL(iUsePct);
	GetAndSetScriptL();
	iScriptExecutor->ScanScriptL();
	if (!iUsePct && iScriptExecutor->ReadFound())
		iUsePct = ETrue;
	}

void CNetDialScript::GetAndSetScriptL()
/**
Get script from database and set script in script executor.
*/
	{
	if (!iScriptExecutor->IsScriptSet())
		{
		HBufC* buffer=iDb->GetScriptLC();
		TPtr script(buffer->Des());
		iScriptExecutor->SetScript(script);
		CleanupStack::PopAndDestroy();
		}
	}

void CNetDialScript::SetLoginVarsL()
/**
Set up values read from the database as variables.
*/
	{
	iScriptExecutor->SetVariableL(KLoginNameVar,iLoginName);
	iScriptExecutor->SetVariableL(KLoginPassVar,iLoginPass);
	iScriptExecutor->SetVariableL(KLoginIpaddrVar,iIPAddress);
	}	

