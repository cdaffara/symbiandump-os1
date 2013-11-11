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
#include <comms-infras\es_connectionservermessages.h>
#include <comms-infras/es_connectionserv.h>
#include <comms-infras/cs_connservparams.h>
#include <comms-infras/cs_connservparams_internal.h>
#include <comms-infras/es_availability.h>
#include <comms-infras/es_accesspointstatus.h>
#include <nifman.h>
#include <CommDbConnPref.h>

PARAM_FOREACH_LIST_BEGIN
#include "PARAM_SVR_LISTHEADERFILE"
PARAM_FOREACH_LIST_END

using namespace ConnectionServ;

class CPARAM_MESS_NAMEStep: public CCapabilityTestStep
	{

public:

	//Get the version of the server to be called
	TVersion Version()
		{
		return TVersion(PARAM_SVR_MAJORNAME, PARAM_SVR_MINORNAME, PARAM_SVR_BUILDNAME);
		}


	//Constructor called from the respective Suite.cpp from their "AddTestStep" function
	CPARAM_MESS_NAMEStep() ;

	//Always clean your mess
	~CPARAM_MESS_NAMEStep()
		{
		tChildThread.Close();
		iQueryBuffer.Close();
		iResultBuffer.Close();
		iBadBuffer.Close();
		//delete iQueryBundle;
		}

	//This is the Function called from "doTestStepL" by the test Suite,and it creates an
	//child thread which internally calls the corresponding Exec_SendReceive_SERVERNAME fn.
	TVerdict MainThread();

	//Here's where the connection and testing the message takes place
	TInt Exec_SendReceive();
	TInt iSessionRef;
	
private:
	TInt do_execute(Int2Type<ECMAccessPointStatusQuery_Cancel>);
	TInt do_execute(Int2Type<ECMAccessPointNotification_SetupThenAwaitThenGetResultOrSize>);
	TInt do_execute(Int2Type<ECMAccessPointNotification_AwaitThenGetResultOrSize>);
	TInt do_execute(Int2Type<ECMAccessPointNotification_GetResult>);
	TInt do_execute(Int2Type<ECMAccessPointNotification_Cancel>);
	TInt do_execute(Int2Type<ECMCreate>);
	TInt do_execute(Int2Type<ECMClose>);
	TInt do_execute(Int2Type<ECMAttachToTierManager>);
	TInt do_execute(Int2Type<ECMAccessPointStatusQuery_DoThenGetResultOrSize>);
	TInt do_execute(Int2Type<ECMAccessPointStatusQuery_GetResult>);
	TInt ConnectToServer(TUint aTierId = 271064560);
	TInt AccessPointQuery(RBuf8& aBuf);
	TAccessPointStatusFilter BuildQuery(TFilterType aType = ERegular);
   	void AddFilterToQueryL(CConnectionServParameterBundle& aBundle, TAccessPointStatusFilter& aFilter);
    void NotificationQueryL(TRequestStatus& aStatus, RBuf8& aBuf, TFilterType aType = ERegular);
   	TInt OpenConn();
   	
private:
	RConnection iConn;
	RSocketServ iSockServ;
	CConnectionServParameterBundle* iQueryBundle;
	RBuf8 iQueryBuffer;
	RBuf8 iResultBuffer;
	RBuf8 iBadBuffer;
	};

#endif


