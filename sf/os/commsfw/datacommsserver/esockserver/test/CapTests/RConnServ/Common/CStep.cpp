// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// PARAM_MESS_NAME_CStep.CPP
// 
//



//Test Step header
#include "PARAM_MESS_NAME_CStep.h"


//TO BE SAFE
IMPORT_C TInt StartDialogThread();

CPARAM_MESS_NAMEStep::CPARAM_MESS_NAMEStep()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	//DEF iTestStepName = _L("CPARAM_MESS_NAMEStep");

	//The server name and IPC number is obtained and all messages are checked Sync
	SR_ServerName		= _L("PARAM_SVR_NAME");
	SR_MESSAGE_TYPE		=	2;
	SR_MESSAGE_ID		= PARAM_MESS_VALUE;
	SR_MESSAGE_MASK		= PARAM_MESS_CAPMASK;

	//The iServer_Panic is a unique name from Server,but always truncated to KMaxExitCategoryName
	
	iServer_Panic		=	_L("PARAM_SVR_T16_PANIC");

	TCapability cap[] = {ECapabilityPARAM_MESS_NAMECAP, ECapability_Limit};
	
	TSecurityInfo info;
	info.Set(RProcess());
	TBool result = EFalse;
	
	for (TInt i = 0; cap[i] != ECapability_Limit; i++) 
	{
	
		if (!(info.iCaps.HasCapability(cap[i])))
		{
			result=ETrue;
		
		}
		
	}
	
	iQueryBundle = CConnectionServParameterBundle::NewL();
	iQueryBuffer.Create(1024);
	iResultBuffer.Create(1024);
	iBadBuffer.Create(4);
	
	iExpect_Rejection = result;
	iStepCap			= PARAM_MESS_CAPMASK;

	//Get a unique thread name
	ChildThread_SR.Format(_L("ChildThread_%S_%d"),&SR_ServerName,SR_MESSAGE_ID);

	}

/*
Exec_SendReceive():
	This Fn is called by the Child Thread
1.	Create a session with the server
2.	Test an SendReceive call
3.	Informs the main thread about the status of the call using
	a.	iSessionCreated, if the a connection is established
	b.	iResult_Server, holds the return value for connection 
	c.	iResult_SR, the return value of SendReceive	call
*/
TInt CPARAM_MESS_NAMEStep::Exec_SendReceive()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	_LIT(KEsockSessStartMutex,"KESSMTX"); //Keep the descriptor short
	RMutex mutex;
	TInt r;
	// Protect the openning of the session
	// with a mutex to stop thread collision
	FOREVER
		{
		r = mutex.CreateGlobal(KEsockSessStartMutex());
		if (r == KErrNone)
			break;
		if (r != KErrAlreadyExists)
			return r;
		r=mutex.OpenGlobal(KEsockSessStartMutex());
		if (r == KErrNone)
			break;
		if (r != KErrNotFound)
			return r;
		}
	mutex.Wait(); // the exclusion ensures the session is started atomically
	iResult_Server=CreateSession(SR_ServerName,TVersion(),2);
	// Because ESock is now loaded by the Comms Root Server which is generally started during
	// the boot this should commonly succeed; however for test code this is still a possibility
	// Hence here we try starting it; this is an atomic operation (and cheap if it's already started)
	if (iResult_Server==KErrNotFound)
		{
		r=StartC32();
		if (r==KErrNone || r==KErrAlreadyExists)
			{
			iResult_Server=CreateSession(SR_ServerName,Version(),2);
			}
		}
	
	mutex.Signal();
	mutex.Close();
	
	if(iResult_Server==KErrNone)
		{
		iSessionCreated = ETrue;
		const TInt test_id = dispatch_num<PARAM_MESS_VALUE>::result;
		if(SR_MESSAGE_ID >= 0)
			iResult_SR = do_execute(Int2Type<test_id>());
		}
	delete cleanup;
	return iResult_Server;
	}


TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECMAccessPointStatusQuery_Cancel>)
// testing the ECMAccessPointStatusQuery_Cancel IPC message
	{
	return SendReceive(ECMAccessPointStatusQuery_Cancel,TIpcArgs(0,0,0,0));
	}
	
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECMAccessPointNotification_SetupThenAwaitThenGetResultOrSize>)
// testing the ECMAccessPointNotification_SetupThenAwaitThenGetResultOrSize IPC message
	{
	return SendReceive(ECMAccessPointNotification_SetupThenAwaitThenGetResultOrSize,TIpcArgs(0,0,0,0));
	}
	
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECMAccessPointNotification_AwaitThenGetResultOrSize>)
// testing the ECMAccessPointNotification_AwaitThenGetResultOrSize IPC message
	{
	// connect with server
	TInt err = ConnectToServer();
	if(err != KErrNone)
		{
		return err; 
		}
	TRequestStatus status;
	// Make Notification Query reques, this is the setup for the ECMAccessPointNotification_AwaitThenGetResultOrSize
	TRAP(err,NotificationQueryL(status,iResultBuffer, ENotificationStepTwo));
	if(err != KErrNone)
		{
		return err;
		}
	User::WaitForRequest(status);
	if(status.Int() != KErrNone)
		{
		return status.Int();		
		}
	// load the result buffer
	TRAP(err, CConnectionServParameterBundle *bundle = CConnectionServParameterBundle::LoadL(iResultBuffer));
	if(err != KErrNone)
		{
		return err;
		}
	// make ECMAccessPointNotification_AwaitThenGetResultOrSize request then start a connection in order for request to complete
    SendReceive(ECMAccessPointNotification_AwaitThenGetResultOrSize,TIpcArgs(0,&iResultBuffer,0,iSessionRef), status);
	OpenConn();
	err = iConn.Start();
	if(err != KErrNone)
		{
		return err;
		}
	User::WaitForRequest(status);
	// make ECMAccessPointNotification_AwaitThenGetResultOrSize request then cancel the notification request
	SendReceive(ECMAccessPointNotification_AwaitThenGetResultOrSize,TIpcArgs(0,&iBadBuffer,0,iSessionRef), status);
	SendReceive(ECMAccessPointNotification_Cancel,TIpcArgs(0,0,0,iSessionRef), status);
	User::WaitForRequest(status);
	return status.Int();
	}
	
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECMAccessPointNotification_GetResult>)
	{
	// connect to server 
	TInt err = ConnectToServer(271064565);
	if(err != KErrNone)
		{
		return err;
		}
	TRequestStatus status;
	// Make Notification Query request, this is the setup for the ECMAccessPointNotification_AwaitThenGetResultOrSize
	// using result buffer that is too small, will cause error after ECMAccessPointNotification_AwaitThenGetResultOrSize
	TRAP(err,NotificationQueryL(status,iBadBuffer, ENotificationStepTwo));
	if(err != KErrNone)
		{
		return err;
		}
	User::WaitForRequest(status);
	// make ECMAccessPointNotification_AwaitThenGetResultOrSize request then start a connection in order for request to complete
	SendReceive(ECMAccessPointNotification_AwaitThenGetResultOrSize,TIpcArgs(0,&iBadBuffer,0,iSessionRef), status);
	OpenConn();
	err = iConn.Start();
	if(err != KErrNone)
		{
		return err;
		}
	User::WaitForRequest(status);
	iConn.Stop();
	// should return an error due to had buffer
	TRAP(err, CConnectionServParameterBundle *bundle = CConnectionServParameterBundle::LoadL(iResultBuffer));
	if(err != KErrNone)
		{
		return err;
		}
	// Now get result with a buffer of correct size
	SendReceive(ECMAccessPointNotification_GetResult,TIpcArgs(0,&iResultBuffer,0,iSessionRef), status);
	User::WaitForRequest(status);
	if(status.Int() != KErrNone )
		{
		return status.Int();
		}
	// cancel notification request
	SendReceive(ECMAccessPointNotification_AwaitThenGetResultOrSize,TIpcArgs(0,&iBadBuffer,0,iSessionRef), status);
	SendReceive(ECMAccessPointNotification_Cancel,TIpcArgs(0,0,0,iSessionRef), status);
	User::WaitForRequest(status);
	return status.Int();
	}
	
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECMAccessPointNotification_Cancel>)
	{
	// connect to server 
	TInt err = ConnectToServer();
	if(err != KErrNone)
		{
		return err;	
		}
	TRequestStatus status;
	// Make Notification Query request, this is the setup for the ECMAccessPointNotification_AwaitThenGetResultOrSize
	TRAP(err, NotificationQueryL(status, iResultBuffer));
	if(err != KErrNone)
		{
		return err;
		}
	User::WaitForRequest(status);
	if(status.Int()!= KErrNone)
		{
		return err;	
		}
	// cancel the request
	SendReceive(ECMAccessPointNotification_Cancel,TIpcArgs(0,0,0,iSessionRef), status);
	User::WaitForRequest(status);
	return status.Int(); 
	}
	
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECMCreate>)
// testing the ECMCreate IPC message
	{
	return SendReceive(ECMCreate,TIpcArgs(0,0,0,0));
	}
	
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECMClose>)
// testing the ECMClose IPC message
	{
	return SendReceive(ECMClose,TIpcArgs(0,0,0,0));
	}
	
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECMAttachToTierManager>)
// testing the ECMAttachToTierManager IPC message
	{
	return SendReceive(ECMAttachToTierManager,TIpcArgs(0,0,0,0));
	}
	
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECMAccessPointStatusQuery_DoThenGetResultOrSize>)
// testing the ECMAccessPointStatusQuery_DoThenGetResultOrSize IPC message
	{ 
	return SendReceive(ECMAccessPointStatusQuery_DoThenGetResultOrSize,TIpcArgs(0,0,0,0));
	}
	
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECMAccessPointStatusQuery_GetResult>)
	{
	// connect to server with specified tier
	TInt err = ConnectToServer(271064565);
	if(err != KErrNone)
		{
		return err;
		}
	
	// make an access point query with a buffer that is too small to hold the result
	TRAP(err, 
		AccessPointQuery(iBadBuffer);
		CConnectionServParameterBundle *bundle = CConnectionServParameterBundle::LoadL(iResultBuffer);
		);
	if(err != KErrNone)
		{
		return err;
		}
	TRequestStatus status;
	// get result with a buffer of sufficient size
	SendReceive(ECMAccessPointStatusQuery_GetResult,TIpcArgs(&iResultBuffer), status);
	User::WaitForRequest(status);
	return status.Int();
	}


TInt CPARAM_MESS_NAMEStep::ConnectToServer(TUint aTierId)
// connect to the RConnServer with specified Tier Uid
	{
	TInt result;
    TPckgBuf<TInt> session(iSessionRef);
    result = SendReceive(ECMAttachToTierManager,TIpcArgs(aTierId, &session));
    iSessionRef = session(); //-- this is the subsession handle
    return result;
	}
	
TInt CPARAM_MESS_NAMEStep::AccessPointQuery(RBuf8& aBuf)
// Make access point status query
	{
	// build a query
	TAccessPointStatusFilter filter = BuildQuery();
	TRAPD(err, AddFilterToQueryL(*iQueryBundle, filter));
	if(err != KErrNone)
		{
		return err;
		}
	err = iQueryBundle->Store(iQueryBuffer);
	if(err != KErrNone)
		{
		return err;
		}
	
	TRequestStatus status;
	SendReceive(ECMAccessPointStatusQuery_DoThenGetResultOrSize, TIpcArgs(&iQueryBuffer,  &aBuf, 0,iSessionRef),status);
	User::WaitForRequest(status);
	return status.Int();
	}
	
TAccessPointStatusFilter CPARAM_MESS_NAMEStep::BuildQuery(TFilterType aType)
// Build a query
	{
	TAccessPointStatusFilter filter;
	switch(aType)
		{
		case ENotificationStepTwo :
			filter.Configured(EAccessPointFlagIgnore);
			filter.Restricted(EAccessPointFlagIgnore);
			filter.Available(EAccessPointFlagIgnore);  
			filter.Started(EAccessPointFlagMatchTrue);
			break;
		default :
			filter.Configured(EAccessPointFlagMatchTrue);
			filter.Restricted(EAccessPointFlagMatchAny);
			filter.Available(EAccessPointFlagIgnore);
			filter.Started(EAccessPointFlagIgnore);
			break;	
		};
	return filter;
	}
	
void CPARAM_MESS_NAMEStep::AddFilterToQueryL(CConnectionServParameterBundle& aBundle,TAccessPointStatusFilter& aFilter )
// Add a filter to the query
	{
	aBundle.AddMatchConditionL(aFilter);
	aBundle.AddParameterSetToReturnL(XAccessPointGenericParameterSet::Type());
	}
	
void CPARAM_MESS_NAMEStep::NotificationQueryL(TRequestStatus& aStatus, RBuf8& aBuf, TFilterType aFilterType)
// Make access point Notification query
	{
	TAccessPointStatusFilter filter = BuildQuery(aFilterType);
	AddFilterToQueryL(*iQueryBundle, filter);
	User::LeaveIfError(iQueryBundle->Store(iQueryBuffer));
	SendReceive(ECMAccessPointNotification_SetupThenAwaitThenGetResultOrSize, TIpcArgs(&iQueryBuffer,  &aBuf, 0,iSessionRef),aStatus);
	}

TInt CPARAM_MESS_NAMEStep::OpenConn()
// Open an rconnection
	{
	TInt err = iSockServ.Connect();
	if(err != KErrNone)
		{
		return err;
		}
	return iConn.Open(iSockServ);
	}

