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

const TUint KMagicConfigDaemonTestLevel  = 777;
const TUint KMagicConfigDaemonTestOption = 778;

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
	
	// ECNControl level and name.
	iControlLevel		= PARAM_MESS_OPTLEVEL;
	iControlOption		= PARAM_MESS_OPTNAME;

	if(PARAM_MESS_OPTLEVEL == KMagicConfigDaemonTestLevel && PARAM_MESS_OPTNAME == KMagicConfigDaemonTestOption)
		{
		// For this particular case, NetworkServices capability is not controlled by the UPS (it is enforced
		// by the DHCP server).
		aUpsAuthorisationGranted = EFalse;
		}

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

		// This is essentially an RSocketServ, so must be closed
		Close();
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
// RConnection::ESCPSStop
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESCPSStop>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;

    result =SendReceive(ESCPSStop,TIpcArgs(1, 0, 0, iSSRef));

    //-- stop and close connection
    StopCloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ESCPSProgressNotification and ESCPSCancelProgressNotification
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESCPSProgressNotification>)
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
        {
		StopCloseConnection();
        return result;
	    }

    //-- now cancel the progress notification
    result = SendReceive(ESCPSCancelProgressNotification,TIpcArgs(subConnId, 0, 0, iSSRef));
    User::WaitForRequest(rqStat);

    //-- stop and close connection
    StopCloseConnection();

    return result;
}


//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ECNGetIntSetting
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNGetIntSetting>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;

    // read an integer value from commdb (IAP\Id chosen)
	_LIT(KName, "IAP\\id");
	TBufC<7> name(KName);
	TUint32 intval= 0;
    TPckg<TUint32> intPckg(intval);

    result =SendReceive(ECNGetIntSetting,TIpcArgs(&name, &intPckg, 0, iSSRef));

    //-- stop and close connection
    StopCloseConnection();

    return result;
}


//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ECNGetBoolSetting
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNGetBoolSetting>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;

	// read a bool value from commdb (DialOutISP\IfPromptForAuth chosen)
	_LIT(KName, "LANService\\IpAddrFromServer");
	TBufC<27> name(KName);
	TBool boolval=EFalse;
    TPckg<TBool> boolPckg(boolval);

    result =SendReceive(ECNGetBoolSetting,TIpcArgs(&name, &boolPckg, 0, iSSRef));

    //-- stop and close connection
    StopCloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ECNGetDes8Setting
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNGetDes8Setting>)
{
    TInt result;


    //-- 1. Create and start a connection.
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;

	_LIT(KName, "LANBearer\\Agent");
	TBufC<21> name(KName);
	TBuf8<128> desval8;

    result =SendReceive(ECNGetDes8Setting,TIpcArgs(&name, &desval8, 0, iSSRef));

    //-- stop and close connection
    StopCloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ECNGetDes16Setting
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNGetDes16Setting>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;

	_LIT(KName, "LANBearer\\Agent");
	TBufC<21> name(KName);
	TBuf16<128> desval16;

    result =SendReceive(ECNGetDes16Setting,TIpcArgs(&name, &desval16, 0, iSSRef));

    //-- stop and close connection
    StopCloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ECNGetLongDesSetting
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNGetLongDesSetting>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;

	_LIT(KName, "LANBearer\\Agent");
	TBufC<23> name(KName);
	TBuf<128> longdesval;

    result =SendReceive(ECNGetLongDesSetting,TIpcArgs(&name, &longdesval, 0, iSSRef));

    //-- stop and close connection
    StopCloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ECNStart
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNCreate>)
{
    TInt result;

    //-- 1. Create (Open) a connection
    const TUint KConnectionType = KAfInet;
    TPckgBuf<TInt> session(iSSRef);

    result = SendReceive(ECNCreate,TIpcArgs(KConnectionType, 0, 0, &session));
    iSSRef = session(); //-- this is the esock subsession handle

    //-- close connection
    CloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ECNCreate
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNCreateWithName>)
{
    TInt result;

    //-- 1. Create a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

    TName connName;
    result =SendReceive(ECNReference,TIpcArgs(&connName, 0, 0, iSSRef));
	if (result != KErrNone)
	    return result;

	result = SendReceive(ECNCreateWithName,TIpcArgs(&connName, 0, 0, iSSRef));

    //-- close connection
    CloseConnection();

	return result;
}
//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ECNStart
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNStart>)
{
    TInt result;

    //-- 1. Create a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

    //-- 2. start the connection
    result = SendReceive(ECNStart,TIpcArgs(0,0,0,iSSRef));

    //-- stop and close connection
    StopCloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ECNSetStartPrefs
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNSetStartPrefs>)
{
    TInt result;

    //-- 1. Create a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

    //-- 2. start connection with preferences
    TCommDbConnPref prefs;
    prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);

    result =SendReceive(ECNSetStartPrefs,TIpcArgs(&prefs, 0, 0, iSSRef));
	if (result == KErrNone)
		{
		// Continue starting connection so that session is not left in a half-started state
		// Note this may fail, but the test succeeds based on "result" since that is the IPC being tested
		TInt err = SendReceive(ECNStart,TIpcArgs(0,0,0,iSSRef));
		}

    //-- stop and close connection
    StopCloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ECNStop
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNStop>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;

    //-- 2. start the connection
    result = SendReceive(ECNStop,TIpcArgs(RConnection::EStopAuthoritative,0,0,iSSRef));

    //-- stop and close connection
    CloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ECNReference
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNCancelIoctl>)
{
    TInt result;

    //-- 1. Create a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

    result =SendReceive(ECNCancelIoctl,TIpcArgs(0, 0, 0, iSSRef));

    //-- close connection
    CloseConnection();

    return result;
}


//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ECNReference
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNReference>)
{
    TInt result;

    //-- 1. Create a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

    TName connName;
    result =SendReceive(ECNReference,TIpcArgs(&connName, 0, 0, iSSRef));

    //-- close connection
    CloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ECNProgress
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNProgress>)
{
    TInt result;

    //-- 1. Create a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

    TNifProgress progr;
    TPckg<TNifProgress> progrPckg(progr);

    result =SendReceive(ECNProgress,TIpcArgs(&progrPckg, 0, 0, iSSRef));

    //-- close connection
    CloseConnection();

    return result;

}


//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ECNLastProgressError
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNLastProgressError>)
{
    TInt result;

    //-- 1. Create a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

    TNifProgress progr;
    TPckg<TNifProgress> progrPckg(progr);

    result =SendReceive(ECNLastProgressError,TIpcArgs(&progrPckg, 0, 0, iSSRef));

    //-- close connection
    CloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ECNEnnumerateConnections
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNEnumerateConnections>)
{
    TInt result;

    //-- 1. Create (Open) a connection
    result = CreateConnection();

    if(result != KErrNone)
        return result;

    TUint  count=0;
    TPckg<TUint> countPckg(count);

    //-- enumerate connections
    result = SendReceive(ECNEnumerateConnections,TIpcArgs(&countPckg, 0, 0, iSSRef));

    //-- close connection
    CloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ECNGetConnectionInfo
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNGetConnectionInfo>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;

    TPckgBuf<TConnectionInfo> connInfoBuf;

    //-- get connection info
    result =SendReceive(ECNGetConnectionInfo,TIpcArgs(1, &connInfoBuf, 0, iSSRef));

    //-- stop and close connection
    StopCloseConnection();

    return result;

}

//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ESCPSGetSubConnectionInfo
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESCPSGetSubConnectionInfo>)
{

    TInt result;

    //-- 1. Create and start a connection.
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;

    TPckgBuf<TSubConnectionInfo> subConnInfoBuf;

    //-- get subconnection info
    result =SendReceive(ESCPSGetSubConnectionInfo,TIpcArgs(1, &subConnInfoBuf, 0, iSSRef));

    //-- stop and close connection
    StopCloseConnection();

    return result;
}



//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ECNEnumerateSubConnections
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNEnumerateSubConnections>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;

    TUint  count=0;
    TPckg<TUint> countPckg(count);

    //-- enumerate subconnections
    result =SendReceive(ECNEnumerateSubConnections,TIpcArgs(&countPckg, 0, 0, iSSRef));

    //-- stop and close connection
    StopCloseConnection();

    return result;
}


//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ESCPSIsSubConnectionActiveRequest and ESCPSIsSubConnectionActiveCancel
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESCPSIsSubConnectionActiveRequest>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;

    TRequestStatus  rqStat;
    TBool           state = EFalse;
    TPckg<TBool>    stateDes(state);
    const TUint     secs=2;
    const TUint     KSubConnectionId = KAfInet;

    //-- make a request
    SendReceive(ESCPSIsSubConnectionActiveRequest,TIpcArgs(KSubConnectionId, secs, &stateDes, iSSRef), rqStat);
    result = rqStat.Int();
    if((result != KErrNone) && (result != KRequestPending))
        {
		StopCloseConnection();
        return result;
	    }

    //-- now cancel the notification subscription
    result = SendReceive(ESCPSIsSubConnectionActiveCancel,TIpcArgs(KSubConnectionId, 0, 0, iSSRef));
    User::WaitForRequest(rqStat);

    //-- stop and close connection
    StopCloseConnection();

    return result;
}


//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ESCPSDataReceivedNotificationRequest and ESCPSDataReceivedNotificationCancel
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESCPSDataReceivedNotificationRequest>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;

    TRequestStatus  rqStat;
    TUint           vol   = 0;
    TPckg<TUint>    volPckg(vol);
    const TUint     KLowThresholdSize = 1024;
    const TUint     KSubConnectionId = KAfInet;

    //-- request data transferred values.
    SendReceive(ESCPSDataReceivedNotificationRequest,TIpcArgs(KSubConnectionId, KLowThresholdSize, &volPckg, iSSRef), rqStat);
    result = rqStat.Int();
    if((result != KErrNone) && (result != KRequestPending))
        {
		StopCloseConnection();
        return result;
	    }

    //-- now cancel the notification subscription
    result = SendReceive(ESCPSDataReceivedNotificationCancel,TIpcArgs(KSubConnectionId, 0, 0, iSSRef));
    User::WaitForRequest(rqStat);

    //-- stop and close connection
    StopCloseConnection();

    return result;

}



//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ESCPSDataSentNotificationRequest and ESCPSDataSentNotificationCancel
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESCPSDataSentNotificationRequest>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;

    TRequestStatus  rqStat;
    TUint           vol   = 0;
    TPckg<TUint>    volPckg(vol);
    const TUint     KLowThresholdSize = 1024;
    const TUint     KSubConnectionId = KAfInet;

    //-- request data sent notification.
    SendReceive(ESCPSDataSentNotificationRequest,TIpcArgs(KSubConnectionId, KLowThresholdSize, &volPckg, iSSRef), rqStat);
    result = rqStat.Int();
    if((result != KErrNone) && (result != KRequestPending))
        {
		StopCloseConnection();
        return result;
	    }

    //-- now cancel the notification subscription
    result = SendReceive(ESCPSDataSentNotificationCancel,TIpcArgs(KSubConnectionId, 0, 0, iSSRef));
    User::WaitForRequest(rqStat);

    //-- stop and close connection
    StopCloseConnection();

    return result;

}

//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ESCPSDataTransferred and ESCPSDataTransferredCancel
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ESCPSDataTransferred>)
{
    TInt result;

    //-- 1. Create and start a connection.
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;

    TRequestStatus  rqStat;
    TUint           uplinkVol   = 0;
    TUint           downlinkVol = 0;

    TPckg<TUint>  uplinkVolPckg(uplinkVol);
    TPckg<TUint>  downlinkVolPckg(downlinkVol);

    //-- request data transferred values.
    SendReceive(ESCPSDataTransferred,TIpcArgs(0, &uplinkVolPckg, &downlinkVolPckg, iSSRef), rqStat);
    result = rqStat.Int();
    if((result != KErrNone) && (result != KRequestPending))
	    {
		StopCloseConnection();
        return result;
	    }

    //-- now cancel the notification subscription
    result = SendReceive(ESCPSDataTransferredCancel,TIpcArgs(0, 0, 0, iSSRef));
    User::WaitForRequest(rqStat);

    //-- stop and close connection
    StopCloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ECNAllSubConnectionNotification and ECNCancelAllSubConnectionNotification
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNAllSubConnectionNotification>)
{
    TInt result;

    //-- 1. Create a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

    TRequestStatus  rqStat;
    TSubConnectionNotificationBuf info;

    //-- subscribe to the all interfaces notification
    SendReceive(ECNAllSubConnectionNotification,TIpcArgs(&info, 0, 0, iSSRef), rqStat);
    result = rqStat.Int();
    if((result != KErrNone) && (result != KRequestPending))
        {
		CloseConnection();
        return result;
	    }

    //-- now cancel the notification subscription
    result = SendReceive(ECNCancelAllSubConnectionNotification,TIpcArgs(0, 0, 0, iSSRef));
    User::WaitForRequest(rqStat);

    //-- close connection
    CloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ECNAllInterfaceNotification and ECNCancelServiceChangeNotification
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNAllInterfaceNotification>)
{
    TInt result;

    //-- 1. Create a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

    TRequestStatus  rqStat;
    TInterfaceNotificationBuf info;

    //-- subscribe to the all interfaces notification
    SendReceive(ECNAllInterfaceNotification,TIpcArgs(&info, 0, 0, iSSRef), rqStat);
    result = rqStat.Int();
    if((result != KErrNone) && (result != KRequestPending))
        {
		CloseConnection();
        return result;
	    }

    //-- now cancel the notification subscription
    result = SendReceive(ECNCancelAllInterfaceNotification,TIpcArgs(0, 0, 0, iSSRef));
    User::WaitForRequest(rqStat);

    //-- close connection
    CloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ECNServiceChangeNotification and ECNCancelServiceChangeNotification
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNServiceChangeNotification>)
{
    TInt result;

    //-- 1. Create a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

    TRequestStatus  rqStat;
    TBuf<20>        newServiceType;
    TUint32 isp=0;
    TPckg<TUint> ispPckg(isp);

    //-- subscribe to the service change notification
    SendReceive(ECNServiceChangeNotification,TIpcArgs(&ispPckg, &newServiceType, 0, iSSRef), rqStat);
    result = rqStat.Int();
    if((result != KErrNone) && (result != KRequestPending))
        {
		CloseConnection();
        return result;
	    }

    //-- now cancel the service change notification
    result = SendReceive(ECNCancelServiceChangeNotification,TIpcArgs(0, 0, 0, iSSRef));
    User::WaitForRequest(rqStat);

    //-- close connection
    CloseConnection();

    return result;
}

//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ECNProgressNotification and ECNCancelProgressNotification,
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNProgressNotification>)
{
    TInt result;

    //-- 1. Create a connection.
    result = CreateConnection();
    if(result != KErrNone)
        return result;

    TNifProgressBuf nifProgrBuf;
    TRequestStatus  rqStat;

    //-- subscribe to the progress notification
    SendReceive(ECNProgressNotification,TIpcArgs(&nifProgrBuf, KConnProgressDefault, 0, iSSRef), rqStat);
    result = rqStat.Int();
    if((result != KErrNone) && (result != KRequestPending))
        {
		CloseConnection();
        return result;
	    }

    //-- now cancel the progress notification
    result = SendReceive(ECNCancelProgressNotification,TIpcArgs(0, 0, 0, iSSRef));
    User::WaitForRequest(rqStat);

    //-- close connection
    CloseConnection();

    return result;
}


//------------------------------------------------------------------------------------------------------
// testing
// RConnection::ECNIoctl (DHCP stuff) at KCOLConfiguration level.
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNIoctl>)
{

    TInt result;

    //-- 1. Create and start default connection
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
    	{
    	return result;
    	}

    //------------------------------------------------------
    //-- this part is for testing RConfigDaemon functionality.
    //-- Tests direct IOCTL calls to the DHCP server.
    //-- this is distinguished by using magin numbers in the message.csv file, look below.
    if(PARAM_MESS_OPTLEVEL == KMagicConfigDaemonTestLevel && PARAM_MESS_OPTNAME == KMagicConfigDaemonTestOption)
    {
    	StopCloseConnection();
        return test_RConfigDaemon();
    }
    //------------------------------------------------------

    //-- 2. perform the RConnection::IOCTL
	TBuf8<128> paramBuf; //-- buffer receiving IOCTL result data. No one cares about the actual result, so the buffer is generic.

    result = SendReceive(ECNIoctl,TIpcArgs(PARAM_MESS_OPTLEVEL, PARAM_MESS_OPTNAME, &paramBuf, iSSRef));

    //-- 3. close the connection
    StopCloseConnection();

    return result;

}

//------------------------------------------------------------------------------------------------------
// testing
// RConnection::SetOpt() at KCOLProvider   level.
// RConnection:Control() at KCOLConnection level.
// Requires alive connection. Uses Ethernet one
//------------------------------------------------------------------------------------------------------
TInt CPARAM_MESS_NAMEStep::do_execute(Int2Type<ECNControl>)
{

    TInt result = KErrNone;

    //-- 1. Create and start default connection. Also performs connections enumeration
    result = CreateStartDefaultConnection();
    if(result != KErrNone)
        return result;

    //-- call test function depending on the option level and name.
    //-- this looks messy because opt. values for differrent opt names can be the same.
    if(PARAM_MESS_OPTLEVEL == KCOLProvider)
    switch(PARAM_MESS_OPTNAME)
    {

        case KConnDisableTimers:
            result = test_KConnDisableTimers();
        break;

        case KConnGetInterfaceName:
            result = test_KConnGetInterfaceName();
        break;

        default:
            TESTL(EFalse);
        break;

    }

    if(PARAM_MESS_OPTLEVEL == KCOLConnection)
    switch(PARAM_MESS_OPTNAME)
    {
        case KCoEnumerateConnectionClients:
            result = test_KCoEnumerateConnectionClients();
        break;

        case KCoGetConnectionClientInfo:
            result = test_KCoGetConnectionClientInfo();
        break;

        case KCoEnumerateConnectionSockets:
            result = test_KCoEnumerateConnectionSockets();
        break;

        case KCoGetConnectionSocketInfo:
            result = test_KCoGetConnectionSocketInfo();
        break;

        default:
            TESTL(EFalse);
        break;

    }

    if(PARAM_MESS_OPTLEVEL == KCOLInterface)
        result = test_KCOLInterface();

    if(PARAM_MESS_OPTLEVEL == KCOLAgent)
        result = test_KCOLAgent();



    //-- close the connection
    StopCloseConnection();


    return result;
}

//------------------------------------------------------------------------------------------------------

TInt CPARAM_MESS_NAMEStep::test_KConnDisableTimers()
{
    return ConnectionSetOpt(KCOLProvider, KConnDisableTimers, 1);
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
	return SendReceive(ECNControl, TIpcArgs(aOptionLevel, aOptionName, &aOption, iSSRef));
}


/**
    Performing ECNControl actions.
*/
TInt CPARAM_MESS_NAMEStep::ConnectionSetOpt(TUint aOptionLevel, TUint aOptionName, TInt aOption)
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
        {
		CloseConnection();
        return result;
	    }


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
#if 0
	// Stopping the connection is not strictly necessary.  The test may run faster if
	// the interface remains up (in short timer mode) across tests.
    TInt result;
    result = SendReceive(ECNStop,TIpcArgs(RConnection::EStopAuthoritative,0,0,iSSRef));
    if(result != KErrNone)
           return result;
#endif

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

	// Before starting a new connection wait for any old ones to go away
//	TInt ignore = WaitForDisconnection();

    return result;
}

/**
	After a call to close, this can be called to wait for the connection to really close down
*/
TInt CPARAM_MESS_NAMEStep::WaitForDisconnection()
	{
	TInt error = KErrNone;
	if (Handle())
		{
		RSocketServ ss;
		ss.SetHandle(Handle());
		ss.Duplicate(RThread());
		RConnection mon;
		error = mon.Open(ss, KAfInet);
		if (error == KErrNone)
			{
			TRAP(error, WaitForDisconnectionL(mon));
			// Clean up
			mon.Close();
			}
		}
	return error;
	}

void CPARAM_MESS_NAMEStep::WaitForDisconnectionL(RConnection& aMon)
	{
	TUint count;
	User::LeaveIfError(aMon.EnumerateConnections(count));
	if (count > 0)
		{
		// Get the connection info if there are any connections
		TConnectionInfoBuf info;
		User::LeaveIfError(aMon.GetConnectionInfo(1,info));

		// Attach a monitoring connection
		User::LeaveIfError(aMon.Attach(info, RConnection::EAttachTypeMonitor));
	 	TNifProgressBuf progBuf;
	 	TRequestStatus progStatus;
	 	aMon.ProgressNotification(progBuf, progStatus, KConnectionUninitialised);

		// Wait for KLinkLayerClosed stage (or error)
		User::WaitForRequest(progStatus);
		User::LeaveIfError(progStatus.Int());
		}
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

    RTestDaemonClient   testDaemon;

    //-- create session to the DHCP server
    result = testDaemon.CreateSession();
	testDaemon.Close();
    return result;

}



//------------------------------------------------------------------------------------------------------

