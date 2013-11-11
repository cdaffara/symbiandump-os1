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
static const TInt KWaitPeriod = 3000000;
static const TInt KListenQLength = 2;
const TUint32 KInetAddrTestNetwork = INET_ADDR(10,22,64,10);  //(127,0,0,1);

CPARAM_MESS_NAMEStep::CPARAM_MESS_NAMEStep(TBool aUpsAuthorisationGranted)
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
		if ((!aUpsAuthorisationGranted || cap[i] != ECapabilityNetworkServices) &&
			!(info.iCaps.HasCapability(cap[i])))
		{
			result=ETrue;

		}

	}


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
/*TInt CPARAM_MESS_NAMEStep::Exec_SendReceive()
	{
	iResult_Server = CreateSession(SR_ServerName,Version(),2);

	if (iResult_Server!=KErrNone)
		{

		iResult_Server=StartServer();
		if (iResult_Server!=KErrNone)
			return(iResult_Server);

		iResult_Server = CreateSession(SR_ServerName,TVersion(),2);
		}
	if(iResult_Server == 0)
		{
		iSessionCreated = ETrue;
		if(SR_MESSAGE_ID >= 0)
			iResult_SR	=	SendReceive(SR_MESSAGE_ID,TIpcArgs(0,0,0,0));
		}

	return iResult_Server;
	}*/

TInt CPARAM_MESS_NAMEStep::Exec_SendReceive()
	{
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
	iResult_Server=CreateSession(SOCKET_SERVER_NAME,Version());
	// Because ESock is now loaded by the Comms Root Server which is generally started during
	// the boot this should commonly succeed; however for test code this is still a possibility
	// Hence here we try starting it; this is an atomic operation (and cheap if it's already started)
	if (iResult_Server==KErrNotFound)
		{
		r=StartC32();
		if (r==KErrNone || r==KErrAlreadyExists)
			{
			iResult_Server=CreateSession(SOCKET_SERVER_NAME,Version());
			}
		}
	mutex.Signal();
	mutex.Close();

	if (iResult_Server == KErrNone)
		{
		iSessionCreated = ETrue;

		const TInt test_id = dispatch_num<PARAM_MESS_VALUE>::result;

		iResult_SR = do_execute(Int2Type<test_id>());
		}
	else
		{
		iSessionCreated = EFalse;
		return 0;
		}
	return r;
	}



// Use ONLY for ESoConnect
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoConnect>)
	{
	// Open and Bind socket
	TInt result = do_execute(Int2Type<ESoBind>());

	if (result == KErrNone)
		{
		const TUint32 KInetAddr = PARAM_MESS_REMIP4ADDR;
		TInetAddr Addr(KInetAddr, PARAM_MESS_LOCALPORT);

		result = do_async_sr(ESoConnect, ESoCancelAll,
				TIpcArgs(&Addr, NULL, NULL,iSSRef));
		}
	return result;
	}

// Use ONLY for ESoConnect
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoShutdown>)
	{
	// Open and Bind socket
	TInt result = do_execute(Int2Type<ESoBind>());

	if (result == KErrNone)
		{

		result = do_async_sr(ESoShutdown, ESoCancelAll,
				TIpcArgs(0,0,0,iSSRef)); //RSocket::ENormal = 0
		}
	return result;
	}
/*
		TRequestStatus status;
		SendReceive(ESoIoctl,
			TIpcArgs(PARAM_MESS_OPTNAME,NULL,PARAM_MESS_OPTLEVEL,iSSRef),status);
		User::WaitForRequest(status);
		return status.Int();

*/

// Use ONLY for ESoCancelConnect
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoCancelConnect>)
	{
	// Open and Bind socket
	TInt result = do_execute(Int2Type<ESoBind>());

	if (result == KErrNone)
		{
		const TUint32 KInetAddr = PARAM_MESS_REMIP4ADDR;
		TInetAddr Addr(KInetAddr, PARAM_MESS_LOCALPORT);

		result = do_sync_cancel(ESoConnect, ESoCancelConnect,
				TIpcArgs(&Addr, NULL, NULL,iSSRef));
		}
	return result;
	}
// Use ONLY for ESoBind
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoBind>)
	{
	TInt result = MakeSubsession();
	// Do binding
	if (result == KErrNone)
		{
		TInetAddr Addr(PARAM_MESS_LOCALPORT);
		return SendReceive(ESoBind,TIpcArgs(&Addr, 0, 0, iSSRef));
		}
	return result;
	}
// ESoRecv
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoRecv>)
	{
	// Open, Bind and Connect to socket
	TInt result = do_execute(Int2Type<ESoSend>());

	return result;
	}
// ESoCancelRecv , cancel ESoRecv prematually
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoCancelRecv>)
	{
	// Open, Bind and Connect to socket
	TInt result = do_execute(Int2Type<ESoConnect>());

	// Send data
	if (result == KErrNone)
		{
		TPtr8 bufPtr = (HBufC8::NewL(0))->Des();
		TSockXfrLength length = 0;

		result = do_sync_cancel(ESoRecv, ESoCancelRecv,
				TIpcArgs(NULL,&length,&bufPtr,iSSRef));
		}
	return result;
	}
// ESoRecvOneOrMore
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoRecvOneOrMore>)
	{
	// Open, Bind and Connect to socket
	TInt result = do_execute(Int2Type<ESoSend>());

	return result;
	}
// ESoRecvOneOrMoreNoLength
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoRecvOneOrMoreNoLength>)
	{
	// Open, Bind and Connect to socket
	TInt result = do_execute(Int2Type<ESoSend>());

	return result;
	}
// ESoRead
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoRead>)
	{
	// Open, Bind and Connect to socket
	TInt result = do_execute(Int2Type<ESoSend>());

	return result;
	}
// ESoWrite
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoWrite>)
	{
	// Open, Bind and Connect to socket
	TInt result = do_execute(Int2Type<ESoSend>());

	return result;
	}
// used for ESoSend and being called by ESoSendNoLength,
// ESoRecv, ESoRecvNoLength, ESoRecvOneOrMore, ESoRecvOneOrMoreNoLength
// ESoRead and ESoWrite
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoSend>)
	{
	// Open, Bind and Connect to socket
	TInt result = do_execute(Int2Type<ESoConnect>());

	// Send data
	if (result == KErrNone)
		{
		TPtr8 bufPtr = (HBufC8::NewL(0))->Des();
		TSockXfrLength length = 0;

		if(PARAM_MESS_VALUE == ESoSendTo || PARAM_MESS_VALUE == ESoRecvFrom)
			{
			result = do_async_sr(PARAM_MESS_VALUE, ESoCancelAll,
					TIpcArgs(&length,NULL,&bufPtr,iSSRef));
			}
		else
			{
			result = do_async_sr(PARAM_MESS_VALUE, ESoCancelAll,
					TIpcArgs(NULL,&length,&bufPtr,iSSRef));
			}
		}
	return result;
	}

// ESoCancelSend, used by ESoCancelAll
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoCancelSend>)
	{
	// Open, Bind and Connect to socket
	TInt result = do_execute(Int2Type<ESoConnect>());

	// Send data
	if (result == KErrNone)
		{
		TPtr8 bufPtr = (HBufC8::NewL(0))->Des();
		TSockXfrLength length = 0;

		result = do_sync_cancel(ESoSend, PARAM_MESS_VALUE,
				TIpcArgs(NULL,&length,&bufPtr,iSSRef));
		}
	return result;
	}
// ESoCancelAll, cancel action prematually
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoCancelAll>)
	{
	// Open, Bind and Connect to socket
	TInt result = do_execute(Int2Type<ESoCancelSend>());

	return result;
	}
// used for ESoSendTo and ESoSendToNoLength,
// ESoRecvFrom and ESoRecvFromNoLength
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoSendTo>)
	{
	TInt result = MakeSubsession();
	// Use unconnected/unbound socket
	// Send data
	if (result == KErrNone)
		{
		TPtr8 bufPtr = (HBufC8::NewL(0))->Des();

		const TUint32 KInetAddr = PARAM_MESS_REMIP4ADDR;
		TInetAddr Addr(KInetAddr, PARAM_MESS_LOCALPORT);

		TSockXfrLength length = 0;
		result = do_async_sr(PARAM_MESS_VALUE, ESoCancelAll,
				TIpcArgs(&length,&Addr,&bufPtr,iSSRef));
		}
	return result;
	}
// ESoRecvFrom
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoRecvFrom>)
	{
	TInt result = do_execute(Int2Type<ESoSendTo>());
	return result;
	}
// Use ONLY for ESoAccept, ESoCancelAccept being tested as well
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoAccept>)
	{
	// Open, bind and listen
	TInt result = do_execute(Int2Type<ESoListen>());
	// Open a new blank subsession
	TInt reply = 0;
	TPckgBuf<TInt> session(reply);
	if (result == KErrNone)
		{
		result = SendReceive(ESoCreateNull,
			TIpcArgs(NULL,NULL,NULL, &session));
		reply = session();
		}

	// Call accept
	if (result == KErrNone)
		{
		result = do_async_sr(ESoAccept, ESoCancelAccept,
				TIpcArgs(NULL,reply,NULL,iSSRef));
		}
	return result;
	}
// Use ONLY for ESoCancelAccept for prematually stop action
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoCancelAccept>)
	{
	// Open, bind and listen
	TInt result = do_execute(Int2Type<ESoListen>());
	// Open a new blank subsession
	TInt reply = 0;
	TPckgBuf<TInt> session(reply);
	if (result == KErrNone)
		{
		result = SendReceive(ESoCreateNull,
			TIpcArgs(NULL,NULL,NULL, &session));  // another session to accept
		reply = session();
		}

	// Call accept
	if (result == KErrNone)
		{
		result = do_async_cancel(ESoAccept, ESoCancelAccept,
				TIpcArgs(NULL,reply,NULL,iSSRef));
		}
	return result;
	}

// Use ONLY for ESoListen
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoListen>)
	{
	// Needs to be open and bound
	TInt result = do_execute(Int2Type<ESoBind>());
	// Call Listen with arbitraty queue length
	if (result == KErrNone)
		{
		return SendReceive(ESoListen,TIpcArgs(KListenQLength,NULL,NULL,iSSRef));
		}
	return result;
	}
// Use only for ESoSetOpt and ESoGetOpt and ESoIoctl
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoSetOpt>)
	{
	TInt result = MakeSubsession();

	if (result == KErrNone)
		{
		TInt optionVal = PARAM_MESS_OPTVAL;
		TPtr8 optionDes((TUint8*)&optionVal,sizeof(TInt),sizeof(TInt));
		result = SendReceive(ESoSetOpt,
			TIpcArgs(PARAM_MESS_OPTNAME,&optionDes,PARAM_MESS_OPTLEVEL,iSSRef));
		}
	return result;
	}
// Use only for ESoSetOpt and ESoGetOpt and ESoIoctl
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoGetOpt>)
	{
	TInt result = do_execute(Int2Type<ESoSetOpt>());

	if (result == KErrNone)
		{
		TInt optionVal;  //expected return value packeted in a descriptor
		TPtr8 optionDes((TUint8*)&optionVal,sizeof(TInt),sizeof(TInt));
		result = SendReceive(ESoGetOpt,
			TIpcArgs(PARAM_MESS_OPTNAME,&optionDes,PARAM_MESS_OPTLEVEL,iSSRef));
		}
	return result;
	}

// Use for ESoIoctl
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoIoctl>)
	{
	TInt result = MakeSubsession();

	if (result == KErrNone)
		{
		TRequestStatus status;
		SendReceive(ESoIoctl,
			TIpcArgs(PARAM_MESS_OPTNAME,NULL,PARAM_MESS_OPTLEVEL,iSSRef),status);
		User::WaitForRequest(status);
		return status.Int();
		}
	return result;
	}
// Use for ESoCancelIoctl
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoCancelIoctl>)
	{
	TInt result = MakeSubsession();

	if (result == KErrNone)
		{
		result = do_sync_cancel(ESoIoctl, ESoCancelIoctl,
			TIpcArgs(PARAM_MESS_OPTNAME,NULL,PARAM_MESS_OPTLEVEL,iSSRef));
		}
	return result;
	}

// Use for ESoGetDiscData, ESoGetLocalName, ESoGetRemoteName
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoGetDiscData>)
	{
	TInt result = do_execute(Int2Type<ESoConnect>());
	if (result == KErrNone)
		{
		TSockAddr addr;
		result = do_async_sr(PARAM_MESS_VALUE, ESoCancelConnect,
				TIpcArgs(&addr,NULL,NULL,iSSRef));
		}

	return result;
	}
// Use for ESoGetLocalName
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoGetLocalName>)
	{
	TInt result = do_execute(Int2Type<ESoBind>());

	if (result == KErrNone)
		{
		// we can use local loopback for testing ESoGetLocalName/ESoGetRemoteName
		const TUint32 KInetAddr = PARAM_MESS_LOCALIP4ADDR;
		TInetAddr Addr(KInetAddr, PARAM_MESS_LOCALPORT);

		result = do_async_sr(ESoConnect, ESoCancelAll,
				TIpcArgs(&Addr, NULL, NULL,iSSRef));

		if (result == KErrNone)
			{
			// This is where the actual test of ESoGetLocalName/ESoGetRemoteName takes place
			TSockAddr addr;
			result = do_async_sr(PARAM_MESS_VALUE, ESoCancelAll,
					TIpcArgs(&addr,NULL,NULL,iSSRef));
			}
		}

	return result;
	}

// Use for ESoGetRemoteName
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoGetRemoteName>)
	{
	TInt result = do_execute(Int2Type<ESoGetLocalName>());
	return result;
	}

// Use for ESoSocketInfo
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoSocketInfo>)
	{
	// Needs to be open
	TInt result = MakeSubsession();
	TProtocolDesc protoInfo;
	TPckg<TProtocolDesc> protDesc(protoInfo);
	// Call ESoSocketInfo
	if (result == KErrNone)
		{
		result = SendReceive(ESoSocketInfo,TIpcArgs(&protDesc,0,0,iSSRef));
		}
	return result;
	}
// Use for ESoReference
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoReference>)
	{
	// Needs to be open
	TInt result = MakeSubsession();
	TName aName;
	// Call RSocket::Name()
	if (result == KErrNone)
		{
		result = SendReceive(ESoReference,TIpcArgs(&aName,0,0,iSSRef));
		}
	return result;
	}
// Use for ESoCreateWithSubConnection
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoCreateWithSubConnection>)
   {
	const TDesC& protName = _L("PARAM_MESS_PROTNAME");
	TProtocolDesc protoInfo;
	TPckg<TProtocolDesc> protDesc(protoInfo);
	TInt result = SendReceive(ESSProtocolInfoByName,TIpcArgs(&protDesc,&protName));
	if (result == KErrNone)
		{
		TPckgBuf<TInt> session(iSSRef);
	    // Opens socket of the given type
		result = SendReceive(ESoCreate,
			TIpcArgs(protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol, &session));
		iSSRef = session();
		}
	// Clean up
	if (result == KErrNone)
		{
		result = SendReceive(ESoClose,TIpcArgs(0,0,0,iSSRef));
		}
	return result;
   }
// Use for ESoCreate
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESoCreate>)
	{
	// Needs to be open
	TInt result = MakeSubsession();
	// Call RSocket::Close()
	if (result == KErrNone)
		{
		result = SendReceive(ESoClose,TIpcArgs(0,0,0,iSSRef));
		}
	return result;
	}

// Being called by many other as it does ESoCreate
TInt CPARAM_MESS_NAMEStep::MakeSubsession()
	{
	const TDesC& protName = _L("PARAM_MESS_PROTNAME");
	TProtocolDesc protoInfo;
	TPckg<TProtocolDesc> protDesc(protoInfo);
	TInt err = SendReceive(ESSProtocolInfoByName,TIpcArgs(&protDesc,&protName));
	if (err == KErrNone)
		{
		TPckgBuf<TInt> session(iSSRef);
	// Opens socket of the given type
		err = SendReceive(ESoCreate,
			TIpcArgs(protoInfo.iAddrFamily, protoInfo.iSockType, protoInfo.iProtocol, &session));
		iSSRef = session();
		}
	return err;
	}


TInt CPARAM_MESS_NAMEStep::do_async_sr(const TInt aMess, const TInt aCancelMess, const TIpcArgs& aArgs)
	{
	TRequestStatus stat, timerStatus;

	RTimer timer;
	timer.CreateLocal();
	timer.After(timerStatus, KWaitPeriod);

	SendReceive(aMess, aArgs, stat);
	User::WaitForRequest(stat, timerStatus);

	// if timer has completed
	if (timerStatus.Int() == KErrNone)
		{
		// cancel the operation
		SendReceive(aCancelMess,TIpcArgs(0,0,0,iSSRef));
		}
	else
		timer.Cancel();

	timer.Close();

	return stat.Int();
	}

// To be used for cancel sync action prematurely
TInt CPARAM_MESS_NAMEStep::do_sync_cancel(const TInt aMess, const TInt aCancelMess, const TIpcArgs& aArgs)
	{

		TInt result = SendReceive(aMess, aArgs);

	// Cancel the action immidiately
	if (result == KErrNone)
		{
		// cancel the operation
		result = SendReceive(aCancelMess,TIpcArgs(0,0,0,iSSRef));
		}

	return result;
	}
// To be used for cancel async action prematurely
TInt CPARAM_MESS_NAMEStep::do_async_cancel(const TInt aMess, const TInt aCancelMess, const TIpcArgs& aArgs)
	{

	TRequestStatus stat;

	SendReceive(aMess, aArgs, stat);
		TInt result = SendReceive(aCancelMess,TIpcArgs(0,0,0,iSSRef));

	return result;
	}

