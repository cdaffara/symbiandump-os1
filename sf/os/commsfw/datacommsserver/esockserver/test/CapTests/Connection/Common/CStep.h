// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#ifndef PARAM_MESS_NAME_H_
#define PARAM_MESS_NAME_H_

//Include the suite header
#include "CSuite.h"
#include <ss_std.h>

#include <nifman.h>
#include <CommDbConnPref.h>


#include "comms-infras/SOCKMES.H"
#include "comms-infras/es_config.h"
#include <es_enum_internal.h>
#include <es_sock_internal.h>
#include "comms-infras/RConfigDaemonMess.h"
#include <rsshared.h>


PARAM_FOREACH_LIST_BEGIN
#include "PARAM_SVR_LISTHEADERFILE"
PARAM_FOREACH_LIST_END


class CPARAM_MESS_NAMEStep: public CCapabilityTestStep
	{

public:

	//Get the version of the server to be called
	TVersion Version()
	{
		return TVersion(PARAM_SVR_MAJORNAME, PARAM_SVR_MINORNAME, PARAM_SVR_BUILDNAME);
	}


	//Constructor called from the respective Suite.cpp from their "AddTestStep" function
	CPARAM_MESS_NAMEStep(TBool aUpsAuthorisationGranted);

	//Always clean your mess
	~CPARAM_MESS_NAMEStep()
	{
		tChildThread.Close();
	}

	//This is the Function called from "doTestStepL" by the test Suite,and it creates an
	//child thread which internally calls the corresponding Exec_SendReceive_SERVERNAME fn.
	TVerdict MainThread();

	//Here's where the connection and testing the message takes place
	TInt Exec_SendReceive();

private:
	TInt iSSRef;

	TInt do_execute(Int2Type<ECNIoctl>);
	TInt do_execute(Int2Type<ECNControl>);
	TInt do_execute(Int2Type<ECNProgressNotification>);
	TInt do_execute(Int2Type<ECNServiceChangeNotification>);
	TInt do_execute(Int2Type<ECNAllInterfaceNotification>);
	TInt do_execute(Int2Type<ECNAllSubConnectionNotification>);
	TInt do_execute(Int2Type<ESCPSDataTransferred>);
	TInt do_execute(Int2Type<ESCPSDataSentNotificationRequest>);
	TInt do_execute(Int2Type<ESCPSDataReceivedNotificationRequest>);
	TInt do_execute(Int2Type<ESCPSIsSubConnectionActiveRequest>);
	TInt do_execute(Int2Type<ECNEnumerateSubConnections>);
	TInt do_execute(Int2Type<ESCPSGetSubConnectionInfo>);
	TInt do_execute(Int2Type<ECNGetConnectionInfo>);
	TInt do_execute(Int2Type<ECNProgress>);
	TInt do_execute(Int2Type<ECNLastProgressError>);
	TInt do_execute(Int2Type<ECNReference>);
	TInt do_execute(Int2Type<ECNCancelIoctl>);
	TInt do_execute(Int2Type<ECNSetStartPrefs>);
    TInt do_execute(Int2Type<ECNGetIntSetting>);
    TInt do_execute(Int2Type<ECNGetBoolSetting>);
    TInt do_execute(Int2Type<ECNGetDes8Setting>);
    TInt do_execute(Int2Type<ECNGetDes16Setting>);
    TInt do_execute(Int2Type<ECNGetLongDesSetting>);
    TInt do_execute(Int2Type<ESCPSStop>);
    TInt do_execute(Int2Type<ESCPSProgressNotification>);
    TInt do_execute(Int2Type<ECNEnumerateConnections>);
	TInt do_execute(Int2Type<ECNStart>);
	TInt do_execute(Int2Type<ECNStop>);
	TInt do_execute(Int2Type<ECNCreate>);
	TInt do_execute(Int2Type<ECNCreateWithName>);

    TInt CreateConnection();
	TInt CreateStartDefaultConnection();
	TInt CloseConnection();
    TInt StopCloseConnection();
    TInt WaitForDisconnection();
	void WaitForDisconnectionL(RConnection& aMon);

    TInt ConnectionControl(TUint aOptionLevel, TUint aOptionName, TDes8& aOption);
	TInt ConnectionSetOpt(TUint aOptionLevel, TUint aOptionName, TInt aOption);

    TInt test_KConnGetInterfaceName();
    TInt test_KConnDisableTimers();
    TInt test_KCoEnumerateConnectionClients();
    TInt test_KCoGetConnectionClientInfo();
    TInt test_KCoEnumerateConnectionSockets();
    TInt test_KCoGetConnectionSocketInfo();

    TInt test_KCOLInterface();
    TInt test_KCOLAgent();

    TInt test_RConfigDaemon();



    };

#endif


