// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <dns_qry.h>

#include <comms-infras/SOCKMES.H>
#include <comms-infras/es_config.h>
#include <es_enum_internal.h>
#include <es_sock_internal.h>
#include <comms-infras/RConfigDaemonMess.h>
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
	TInt iSSRes;

	TInt do_execute(Int2Type<EHRCreate>);
	TInt do_execute(Int2Type<EHRCreateWithConnection>);
	TInt do_execute(Int2Type<EHRGetByName>);
	TInt do_execute(Int2Type<EHRNext>);
	TInt do_execute(Int2Type<EHRGetByAddress>);
	TInt do_execute(Int2Type<EHRGetHostName>);
	TInt do_execute(Int2Type<EHRSetHostName>);
	TInt do_execute(Int2Type<EHRCancel>);
	TInt do_execute(Int2Type<EHRClose>);
	TInt do_execute(Int2Type<EHrQuery>);
	TInt do_execute(Int2Type<EHrQueryNext>);

	TInt do_execute(Int2Type<ESRCreate>);
    TInt do_execute(Int2Type<ESRGetByName>);
    TInt do_execute(Int2Type<ESRGetByNumber>);
    TInt do_execute(Int2Type<ESRRegisterService>);
    TInt do_execute(Int2Type<ESRRemoveService>);
	TInt do_execute(Int2Type<ESRClose>);

    TInt do_execute(Int2Type<ESSNumProtocols>);
    TInt do_execute(Int2Type<ESSProtocolInfo>);
    TInt do_execute(Int2Type<ESSProtocolInfoByName>);
    TInt do_execute(Int2Type<ESSProtocolStart>);
    TInt do_execute(Int2Type<ESSProtocolStop>);
    TInt do_execute(Int2Type<ESSInstallExtension>);
	TInt do_execute(Int2Type<ESSExclusiveMode>);
	TInt do_execute(Int2Type<ESSClearExclusiveMode>);

    TInt CreateConnection();
	TInt CreateStartDefaultConnection();
	TInt CloseConnection();
    TInt StopCloseConnection();

    TInt ConnectionControl(TUint aOptionLevel, TUint aOptionName, TDes8& aOption);
	TInt ConnectionControl(TUint aOptionLevel, TUint aOptionName, TInt aOption);

    TInt test_KConnGetInterfaceName();
    TInt test_KConnDisableTimers();
    TInt test_KCoEnumerateConnectionClients();
    TInt test_KCoGetConnectionClientInfo();
    TInt test_KCoEnumerateConnectionSockets();
    TInt test_KCoGetConnectionSocketInfo();

    TInt test_KCOLInterface();
    TInt test_KCOLAgent();

    TInt test_RConfigDaemon();
    TInt OpenHostResolver();
    TInt OpenServiceResolver();


    };

#endif


