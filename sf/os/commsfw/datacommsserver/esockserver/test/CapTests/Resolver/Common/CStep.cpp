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
//static const TInt KWaitPeriod = 3000000;


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

	iStepCap			= (TUint32)PARAM_MESS_CAPMASK;

	//Get a unique thread name
	ChildThread_SR.Format(_L("ChildThread_%S_%d"),&SR_ServerName,SR_MESSAGE_ID);

	}


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

//------------------------------------------------------------------------------------------------------
// testing
// EHRCreate
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<EHRCreate>)
{
    TInt result;

    TPckgBuf<TInt> session(iSSRes);

    result=SendReceive(EHRCreate,TIpcArgs(KAfInet, KProtocolInetUdp, 0, &session));
    if(result != KErrNone) return result;

	iSSRes = session();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// EHRCreateWithConnection
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<EHRCreateWithConnection>)
{
	TInt result;

    //-- 1. Create and start a connection.
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;

    TNifProgressBuf nifProgrBuf;
    TRequestStatus  rqStat;
    const TSubConnectionUniqueId subConnId = 1;

    //-- subscribe to the progress notification
    SendReceive(ESCPSProgressNotification,TIpcArgs(subConnId, &nifProgrBuf, KConnProgressDefault, iSSRef), rqStat);
    result = rqStat.Int();
    if((result != KErrNone) && (result != KRequestPending))
        return result;

    TPckgBuf<TInt> session(iSSRes);

    //-- now cancel the progress notification
    result = SendReceive(EHRCreateWithConnection,TIpcArgs(KAfInet, KProtocolInetUdp, iSSRef, &session));
    iSSRes = session();

    //-- stop and close connection
    StopCloseConnection();
    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// EHRGetByName
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<EHRGetByName>)
{
	TInt result;

    //-- 1. Create and start a connection.
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;


	result = OpenHostResolver();
    if(result != KErrNone)
    {
    //-- stop and close connection
    StopCloseConnection();
    return result;
    }


	_LIT(KName, "www.yahoo.com");
	TBufC<20> aName(KName);

    TNameEntry aResult;

	result = SendReceive(EHRGetByName,TIpcArgs(&aName,&aResult,0, iSSRes));

    if(result != KErrNone) return result;

    //-- stop and close connection
    StopCloseConnection();

    return result;
}


//------------------------------------------------------------------------------------------------------
// testing
// EHRNext
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<EHRNext>)
{
	TInt result;

    //-- 1. Create and start a connection.
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;


	result = OpenHostResolver();
    if(result != KErrNone)
    {
    //-- stop and close connection
    StopCloseConnection();
    return result;
    }

	_LIT(KName, "www.yahoo.com");
	TBufC<20> aName(KName);

    TNameEntry aResult;

	result = SendReceive(EHRGetByName,TIpcArgs(&aName,&aResult,0, iSSRes));

	result = SendReceive(EHRNext,TIpcArgs(0,&aResult,0,iSSRes));

    if(result != KErrNone) return result;

    //-- stop and close connection
    StopCloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// EHRGetByAddress
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<EHRGetByAddress>)
{
    TInt result;


    //-- 1. Create  a connection.

	result = OpenHostResolver();
    if(result != KErrNone)
    {
    //-- close connection
    SendReceive(EHRClose,TIpcArgs(0,0,0,iSSRes));
    return result;
    }

	_LIT(KName, "127.0.0.1");
	TBufC<20> aName(KName);

    TInetAddr addr;
    addr.Input(aName);
    TNameEntry aResult;

    result =SendReceive(EHRGetByAddress,TIpcArgs(&addr,&aResult,0, iSSRes));
    if(result != KErrNone)
        return result;

    //-- stop  connection
    result = SendReceive(EHRClose,TIpcArgs(0,0,0,iSSRes));

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// EHRGetHostName
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<EHRGetHostName>)
{
    TInt result;

    //-- 1. Create and start a connection.




	result = OpenHostResolver();
    if(result != KErrNone)
    {
    //-- stop and close connection
    SendReceive(EHRClose,TIpcArgs(0,0,0,iSSRes));
    return result;
    }

    THostName aResult;

    result =SendReceive(EHRGetHostName,TIpcArgs(&aResult,0,0,iSSRes));
    if(result != KErrNone)
        return result;

    //-- stop and close connection
    result = SendReceive(EHRClose,TIpcArgs(0,0,0,iSSRes));

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// EHRSetHostName
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<EHRSetHostName>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;

    result = OpenHostResolver();
    if(result != KErrNone)
    {
    //-- stop and close connection
    StopCloseConnection();
    return result;
    }

    _LIT(KName, "localhost");
	TBufC<20> aName(KName);


    result =SendReceive(EHRSetHostName,TIpcArgs(&aName,0,0,iSSRes));
    if(result != KErrNotSupported)
        return result;

    //-- stop and close connection
    result = StopCloseConnection();

    return result;
}


//------------------------------------------------------------------------------------------------------
// testing
// EHRCancel
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<EHRCancel>)
{
    TInt result;

    //-- 1. Create a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

    result = OpenHostResolver();
    if(result != KErrNone)
    {
    //-- stop and close connection
    StopCloseConnection();
    return result;
    }

	_LIT(KName, "www.yahoo.com");
	TBufC<20> aName(KName);

    TNameEntry aResult;

	result = SendReceive(EHRGetByName,TIpcArgs(&aName,&aResult,0, iSSRes));

    result =SendReceive(EHRCancel,TIpcArgs(0,0,0,iSSRes));
    if(result != KErrNone)
        return result;

    //-- stop and close connection
    result = StopCloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// EHRClose
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<EHRClose>)
{
    TInt result;

    //-- 1. Create a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

    result = OpenHostResolver();
    if(result != KErrNone)
    {
    //-- stop and close connection
    StopCloseConnection();
    return result;
    }

    result =SendReceive(EHRClose,TIpcArgs(0, 0, 0, iSSRes));
    if(result != KErrNone)
        return result;

    //-- close connection
    result = CloseConnection();

    return result;
}


//------------------------------------------------------------------------------------------------------
// testing
// EHrQuery
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<EHrQuery>)
{
    TInt result;

    //-- 1. Create a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

    result = OpenHostResolver();
    if(result != KErrNone)
    {
    //-- stop and close connection
    StopCloseConnection();
    return result;
    }

    TDnsQueryBuf      dnsQryBuf;
    TDnsRespABuf      dnsRespABuf;

    dnsQryBuf().SetType(KDnsRRTypeA);
    dnsQryBuf().SetData(_L8("http://www.sample.net/"));


    TName connName;
    result =SendReceive(EHrQuery,TIpcArgs(&dnsQryBuf,&dnsRespABuf,0,iSSRes));
    if(result != KErrNone)
        return result;

    //-- close connection
    result = CloseConnection();

    return result;
}


//------------------------------------------------------------------------------------------------------
// testing
// EHrQueryNext
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<EHrQueryNext>)
{
    TInt result;

    //-- 1. Create a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

    result = OpenHostResolver();
    if(result != KErrNone)
    {
    //-- stop and close connection
    StopCloseConnection();
    return result;
    }

    TDnsQueryBuf      dnsQryBuf;
    TDnsRespABuf      dnsRespABuf;

    dnsQryBuf().SetType(KDnsRRTypeA);
    dnsQryBuf().SetData(_L8("http://www.sample.net/"));

    result =SendReceive(EHrQuery,TIpcArgs(&dnsQryBuf,&dnsRespABuf,0,iSSRes));
    if(result != KErrNone)
        return result;

    result =SendReceive(EHrQueryNext,TIpcArgs(&dnsRespABuf,0,0,iSSRes));
    if(result != KErrNone)
        return result;

    //-- close connection
    result = CloseConnection();

    return result;
}


//------------------------------------------------------------------------------------------------------
// testing
// ESSInstallExtension
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESSInstallExtension>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

	// bad extension args to pass to SS
	_LIT(KBadExtensionName, "bad extension");
	_LIT(KBadArguments, "bad Arguments");
	TPtrC aArgs(KBadArguments);
	result=SendReceive(ESSInstallExtension,TIpcArgs(&KBadExtensionName,&aArgs,aArgs.Length(),iSSRes));

	// The API is current unsupported so we are expecting to see KErrNotSupported as a result.
    if(result != KErrNotSupported)
        return result;

    //-- stop and close connection
    StopCloseConnection();

    return KErrNone;
}

//------------------------------------------------------------------------------------------------------
// testing
// ESSNumProtocols
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESSNumProtocols>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

    TUint count;
	TPtr8 n((TUint8 *)&count,sizeof(count));

    result = SendReceive(ESSNumProtocols,TIpcArgs(&n,0,0,iSSRes));
    if(result != KErrNone)
        return result;

    //-- stop and close connection
    StopCloseConnection();

    return KErrNone;
}

//------------------------------------------------------------------------------------------------------
// testing
// ESSProtocolInfo
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESSProtocolInfo>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

	TUint index=1;
	TProtocolDesc protoInfo;
	TPckg<TProtocolDesc> protDesc(protoInfo);
    result = SendReceive(ESSProtocolInfo,TIpcArgs(&protDesc,index,0,iSSRes));
    if(result != KErrNone)
        return result;

    //-- stop and close connection
    StopCloseConnection();

    return KErrNone;
}

//------------------------------------------------------------------------------------------------------
// testing
// ESSProtocolInfoByName
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESSProtocolInfoByName>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

	const TDesC& protName = _L("tcp");
	TProtocolDesc protoInfo;
	TPckg<TProtocolDesc> protDesc(protoInfo);
    result = SendReceive(ESSProtocolInfoByName,TIpcArgs(&protDesc,&protName,0,iSSRes));
    if(result != KErrNone)
        return result;

    //-- stop and close connection
    StopCloseConnection();

    return KErrNone;
}

//------------------------------------------------------------------------------------------------------
// testing
// ESSProtocolInfoByName
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESSProtocolStart>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

	result = SendReceive(ESSProtocolStart,TIpcArgs(KAfInet,KSockDatagram,KProtocolInetUdp,iSSRes));

    if(result != KErrNone)
        return result;

    //-- stop and close connection
    StopCloseConnection();

    return KErrNone;
}

//------------------------------------------------------------------------------------------------------
// testing
// ESSProtocolStop
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESSProtocolStop>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

	result = SendReceive(ESSProtocolStop,TIpcArgs(KAfInet,KSockDatagram,KProtocolInetUdp,iSSRes));

    if(result != KErrNone)
        return result;

    //-- stop and close connection
    StopCloseConnection();

    return KErrNone;
}

//------------------------------------------------------------------------------------------------------
// testing
// ESSProtocolStop
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESSExclusiveMode>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

	result = SendReceive(ESSExclusiveMode,TIpcArgs(0,0,0,iSSRes));

    if(result != KErrNone)
        return result;

    //-- stop and close connection
    StopCloseConnection();

    return KErrNone;
}

//------------------------------------------------------------------------------------------------------
// testing
// ESSProtocolStop
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESSClearExclusiveMode>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

	result = SendReceive(ESSClearExclusiveMode,TIpcArgs(0,0,0,iSSRes));

    if(result != KErrNone)
        return result;

    //-- stop and close connection
    StopCloseConnection();

    return KErrNone;
}

//------------------------------------------------------------------------------------------------------
// testing
// ESRCreate
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESRCreate>)
{
    TInt result;

    TPckgBuf<TInt> session(iSSRes);

    result=SendReceive(ESRCreate,TIpcArgs(KAfInet, KProtocolInetUdp, 0, &session));
    if(result != KErrNone) return result;

	iSSRes = session();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// ESRGetByName
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESRGetByName>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;


	result = OpenServiceResolver();
    if(result != KErrNone)
    {
    //-- stop and close connection
    StopCloseConnection();
    return result;
    }

	// "echo" service is on port 7, or we could try "http" on port 80.
	_LIT(KName, "echo");
	TBufC<20> aName(KName);

    TNameEntry aResult;

	result = SendReceive(ESRGetByName,TIpcArgs(&aName,&aResult,0, iSSRes));

    if(result != KErrNone) return result;

    //-- stop and close connection
    StopCloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// ESRGetByNumber
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESRGetByNumber>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;


	result = OpenServiceResolver();
    if(result != KErrNone)
    {
    //-- stop and close connection
    StopCloseConnection();
    return result;
    }

	// "echo" service is on port 7, or we could try "http" on port 80.
	TUint aPort = 7;
    TNameEntry aName;

	result = SendReceive(ESRGetByNumber,TIpcArgs(&aName,aPort,0, iSSRes));

    if(result != KErrNone) return result;

    //-- stop and close connection
    StopCloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// ESRRegisterService
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESRRegisterService>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;


	result = OpenServiceResolver();
    if(result != KErrNone)
    {
    //-- stop and close connection
    StopCloseConnection();
    return result;
    }

	// "echo" service is on port 7, or we could try "http" on port 80.
	TUint aPort = 7;
	_LIT(KName, "echo");
	TBufC<20> aName(KName);

	result = SendReceive(ESRRegisterService,TIpcArgs(&aName,aPort,0, iSSRes));

    if(result != KErrNone) return result;

    //-- stop and close connection
    StopCloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// ESRRemoveService
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESRRemoveService>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;

	result = OpenServiceResolver();
    if(result != KErrNone)
    {
    //-- stop and close connection
    StopCloseConnection();
    return result;
    }

	// "echo" service is on port 7, or we could try "http" on port 80.
	TUint aPort = 7;
	_LIT(KName, "echo");
	TBufC<20> aName(KName);

	result = SendReceive(ESRRemoveService,TIpcArgs(&aName,aPort,0, iSSRes));

    if(result != KErrNone) return result;

    //-- stop and close connection
    StopCloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// ESRCancel
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESRClose>)
{
    TInt result;

    //-- 1. Create a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

	result = OpenServiceResolver();
    if(result != KErrNone)
		{
		//-- stop and close connection
		StopCloseConnection();
		return result;
		}

    result =SendReceive(ESRClose,TIpcArgs(0, 0, 0, iSSRes));
    if(result != KErrNone)
        return result;

    //-- close connection
    result = CloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------

TInt CPARAM_MESS_NAMEStep::test_KConnDisableTimers()
{
    return ConnectionControl(KCOLProvider, KConnDisableTimers, 1);
}

TInt CPARAM_MESS_NAMEStep::test_KConnGetInterfaceName()
{
    TConnInterfaceName name;
    name.iIndex = 1;
    TPckg<TConnInterfaceName> namePkg(name);

    return ConnectionControl(KCOLProvider, KConnGetInterfaceName, namePkg);
}


TInt CPARAM_MESS_NAMEStep::test_KCoEnumerateConnectionClients()
{
    TConnEnumArgBuf connEnumBuf;
   	connEnumBuf().iIndex = 1;
    return  ConnectionControl(KCOLConnection, KCoEnumerateConnectionClients, connEnumBuf);
}

TInt CPARAM_MESS_NAMEStep::test_KCoGetConnectionClientInfo()
{
    TConnGetClientInfoArgBuf  connCliInfoBuf;
    connCliInfoBuf().iIndex = 1;
    return ConnectionControl(KCOLConnection, KCoGetConnectionClientInfo, connCliInfoBuf);
}

TInt CPARAM_MESS_NAMEStep::test_KCoEnumerateConnectionSockets()
{
    TConnEnumArgBuf connEnumBuf;
    connEnumBuf().iIndex = 1;
    return ConnectionControl(KCOLConnection, KCoEnumerateConnectionSockets, connEnumBuf);
}

TInt CPARAM_MESS_NAMEStep::test_KCoGetConnectionSocketInfo()
{
    TConnGetSocketInfoArgBuf    connSockInfoBuf;

    connSockInfoBuf().iIndex = 1;
    TInt  result = ConnectionControl(KCOLConnection, KCoGetConnectionSocketInfo, connSockInfoBuf);
    if(result == KErrNotFound)
        result = KErrNone; //-- no sockets found, but this is a positive result anyway.

    return result;
}

TInt CPARAM_MESS_NAMEStep::test_KCOLInterface()
{
	TInt   value = 0;
    TPckg<TInt> valueBuf(value);

	TInt nRes = ConnectionControl(KCOLInterface, 0, valueBuf);
    if(nRes == KErrNotSupported)
        nRes = KErrNone; //-- if thist option is not supported by nif and there is no capabilities conflict, then it's OK.

    return nRes;

}

TInt CPARAM_MESS_NAMEStep::test_KCOLAgent()
{
	TInt   value = 0;
    TPckg<TInt> valueBuf(value);

	TInt nRes = ConnectionControl(KCOLAgent, 0, valueBuf);
    if(nRes == KErrNotSupported)
        nRes = KErrNone; //-- if thist option is not supported by nif and there is no capabilities conflict, then it's OK.

    return nRes;
}

//------------------------------------------------------------------------------------------------------

/**
    Performing ECNControl actions.
*/
TInt CPARAM_MESS_NAMEStep::ConnectionControl(TUint aOptionLevel, TUint aOptionName, TDes8& aOption)
{
	TSockOpt arg;
	arg.optionName = aOptionName;
	arg.optionVal = &aOption;	//Note that optionVal will be useless on the server side
	arg.optionLen = aOption.MaxLength();
	TPckgC<TSockOpt> buf(arg);

	return SendReceive(ECNControl, TIpcArgs(&buf, aOptionLevel, &aOption,iSSRef ));
}


/**
    Performing ECNControl actions.
*/
TInt CPARAM_MESS_NAMEStep::ConnectionControl(TUint aOptionLevel, TUint aOptionName, TInt aOption)
{
	TPtr8 optionDes((TUint8*)&aOption, sizeof(TInt), sizeof(TInt));
	aOptionName &= ~KConnWriteUserDataBit;
	return ConnectionControl(aOptionLevel, aOptionName, optionDes);

}



/**
    Create a connection.
*/
TInt CPARAM_MESS_NAMEStep::CreateConnection()
{
    TInt result;

    //-- 1. Create (Open) a connection
    const TUint KConnectionType = KAfInet;
    TPckgBuf<TInt> session(iSSRef);

    result = SendReceive(ECNCreate,TIpcArgs(KConnectionType, 0, 0, &session));
    iSSRef = session(); //-- this is the esock subsession handle

    return result;
}


/**
    Create and start a default connection
    Also enumerates existing connections.
*/
TInt CPARAM_MESS_NAMEStep::CreateStartDefaultConnection()
{
    TInt result;

    //-- 1. Create (Open) a connection
    result = CreateConnection();

    if(result != KErrNone)
        return result;

    //-- 2. start the connection
    result = SendReceive(ECNStart,TIpcArgs(0,0,0,iSSRef));
    if(result != KErrNone)
        return result;


    //-- 3. Enumerate connections, necessarily for further activity (e.g. KCoGetConnectionSocketInfo)
    //-- and anyway we need to test it.
    TUint cntConn = 0;
   	TPckg<TUint> count(cntConn);
   	result =SendReceive(ECNEnumerateConnections, TIpcArgs(&count,0,0,iSSRef));

    return result;
}


/**
Stops and closes the active connection
*/
TInt CPARAM_MESS_NAMEStep::StopCloseConnection()
{
    TInt result;

    result = SendReceive(ECNStop,TIpcArgs(RConnection::EStopAuthoritative,0,0,iSSRef));
    if(result != KErrNone)
           return result;

    return CloseConnection();
}

/**
    Close the connection
*/
TInt CPARAM_MESS_NAMEStep::CloseConnection()
{
    TInt result;

    //-- close the connection
    result = SendReceive(ECNClose,TIpcArgs(0,0,0,iSSRef));

    return result;
}

//------------------------------------------------------------------------------------------------------

/**
  Test DHCP server API policing.
  Connects to the DHCP server (which is supposed to be started by opening a Ethernet-oriented connection).
  Then tries to perform IOCTL calls, expecting KErrPermissionDenied.

  @return standard error code.
*/
TInt CPARAM_MESS_NAMEStep::test_RConfigDaemon()
{
    TInt result = KErrNone;

    /**
    define a local class to talk to the DHCP daemon. We need this becase
    files like \networking\dhcp\include\DHCP_Std.h are not exported.
    This class represents RConfigDaemon functionality and makes sense only within this function scope.
    */
    class RTestDaemonClient : public RSessionBase
	{
    public:
    	/**
    	Create a session to the DHCP server. beware: we had to use local definition of the KDHCPServerName because
    	DHCP_Std.h isn't accessible here

    	@return standard error code.
    	*/
    	TInt CreateSession()
    	{
    	    _LIT(KDHCPServerName,"!DHCPServ"); // ugly!
    	    return RSessionBase::CreateSession(KDHCPServerName, TVersion());
    	}

    	/**
    	Configure DHCP server. We need this for IOCTL calls.
    	@param aStatus  request status
    	@param apDes    pointer to the TConnectionInfoBuf with connection properties.
    	*/
    	void Configure(TRequestStatus& aStatus, TDes8* apDes)
    	{
    	    SendReceive(EConfigDaemonConfigure, TIpcArgs(apDes, 0, 0, 0), aStatus);
    	}

    	/**
    	IOCTL call to the DHCP server.
    	@param aOptionLevel Option level
    	@param aOptionName  Option Name
    	@param aStatus      request status
    	@param apDes        pointer to the packaged IOCTL data descriptor
    	*/
    	void Ioctl(TUint aOptionLevel, TUint aOptionName, TRequestStatus& aStatus, TDes8* apDes=NULL)
    	{
    	    SendReceive(EConfigDaemonIoctl, TIpcArgs(aOptionLevel, aOptionName, apDes, apDes ? apDes->MaxLength() : 0), aStatus);
    	}

	}; //RTestDaemonClient

    //-- get connection info to configure DHCP daemon later
    TConnectionInfoBuf connInfoBuf;
    //TConnectionInfo &ci = connInfoBuf();
    result =SendReceive(ECNGetConnectionInfo,TIpcArgs(1, &connInfoBuf, 0, iSSRef));
    if(result != KErrNone)
        return result;


    RTestDaemonClient   testDaemon;
    TRequestStatus      rqStat;

    //-- create session to the DHCP server
    result = testDaemon.CreateSession();
    if(result != KErrNone)
        return result;

    //-- configure DHCP server
    testDaemon.Configure(rqStat, &connInfoBuf);
    User::WaitForRequest(rqStat);
    result = rqStat.Int();
    if(result != KErrNone)
        return result;

    //-- test DHCP daemon IOCTL calls.
    //-- The expected result is KErrPermissionDenied (after DHCP daemon calls are policed).
    const TInt KExpectedResult = KErrPermissionDenied;


    TConnectionAddrBuf connAddrBuf;
    //TConnectionAddress &connAddr = connAddrBuf();

    //--
    testDaemon.Ioctl(KCOLConfiguration, KConnGetCurrentAddr, rqStat, &connAddrBuf);
    User::WaitForRequest(rqStat);
    result = rqStat.Int();
    if(result != KExpectedResult)
        return KErrGeneral;
    //--
    testDaemon.Ioctl(KCOLConfiguration, KConnGetServerAddr, rqStat, &connAddrBuf);
    User::WaitForRequest(rqStat);
    result = rqStat.Int();
    if(result != KExpectedResult)
        return KErrGeneral;
    //--
    testDaemon.Ioctl(KCOLConfiguration, KConnGetAddrLeaseTimeRemain, rqStat, &connAddrBuf);
    User::WaitForRequest(rqStat);
    result = rqStat.Int();
    if(result != KExpectedResult)
        return KErrGeneral;
    //--
    testDaemon.Ioctl(KCOLConfiguration, KConnAddrRelease, rqStat, &connAddrBuf);
    User::WaitForRequest(rqStat);
    result = rqStat.Int();
    if(result != KExpectedResult)
        return KErrGeneral;
    //--
    testDaemon.Ioctl(KCOLConfiguration, KConnAddrRenew, rqStat, &connAddrBuf);
    User::WaitForRequest(rqStat);
    result = rqStat.Int();
    if(result != KExpectedResult)
        return KErrGeneral;

    //--
    testDaemon.Ioctl(KCOLConfiguration, KConnGetDhcpRawOptionData, rqStat, &connAddrBuf);
    User::WaitForRequest(rqStat);
    result = rqStat.Int();
    if(result != KExpectedResult)
        return KErrGeneral;


    return KErrNone;
}

TInt CPARAM_MESS_NAMEStep::OpenHostResolver()
{
    TInt result;

    TPckgBuf<TInt> session(iSSRes);

    result=SendReceive(EHRCreate,TIpcArgs(KAfInet, KProtocolInetUdp, 0, &session));
    if(result != KErrNone) return result;

	iSSRes = session();

    return result;
}

//------------------------------------------------------------------------------------------------------

TInt CPARAM_MESS_NAMEStep::OpenServiceResolver()
{
    TInt result;

    TPckgBuf<TInt> session(iSSRes);

    result=SendReceive(ESRCreate,TIpcArgs(KAfInet, KProtocolInetUdp, 0, &session));
    if(result != KErrNone) return result;

	iSSRes = session();

    return result;
}

//------------------------------------------------------------------------------------------------------









