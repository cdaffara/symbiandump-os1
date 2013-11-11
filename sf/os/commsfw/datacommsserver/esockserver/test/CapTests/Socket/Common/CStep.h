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
#include "comms-infras/SOCKMES.H"
#include <es_enum_internal.h>
#include <es_sock_internal.h>
#include <in_sock_internal.h>
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
	TInt do_async_sr(const TInt aMess, const TInt aCancelMess, const TIpcArgs& aArgs);
	// Cancel action prematurely
	TInt do_sync_cancel(const TInt aMess, const TInt aCancelMess, const TIpcArgs& aArgs);
	TInt do_async_cancel(const TInt aMess, const TInt aCancelMess, const TIpcArgs& aArgs);

	TInt MakeSubsession();

	TInt do_execute(Int2Type<ESoCreate>);
	TInt do_execute(Int2Type<ESoRecv>);
	TInt do_execute(Int2Type<ESoCancelRecv>);
	TInt do_execute(Int2Type<ESoRecvOneOrMore>);
	TInt do_execute(Int2Type<ESoRecvOneOrMoreNoLength>);
	TInt do_execute(Int2Type<ESoRead>);  //new
    TInt do_execute(Int2Type<ESoWrite>);  //new
	TInt do_execute(Int2Type<ESoSend>);
	TInt do_execute(Int2Type<ESoCancelSend>);  //new
	TInt do_execute(Int2Type<ESoCancelAll>);
	TInt do_execute(Int2Type<ESoSendTo>);
	TInt do_execute(Int2Type<ESoRecvFrom>);
	TInt do_execute(Int2Type<ESoConnect>); // done
	TInt do_execute(Int2Type<ESoShutdown>);
    TInt do_execute(Int2Type<ESoCancelConnect>);  //new
	TInt do_execute(Int2Type<ESoBind>);  // done
	TInt do_execute(Int2Type<ESoAccept>);
	TInt do_execute(Int2Type<ESoCancelAccept>);  //new
	TInt do_execute(Int2Type<ESoListen>);

	TInt do_execute(Int2Type<ESoSetOpt>);
	TInt do_execute(Int2Type<ESoGetOpt>);  //new
	TInt do_execute(Int2Type<ESoIoctl>);
	TInt do_execute(Int2Type<ESoCancelIoctl>);  //new
	TInt do_execute(Int2Type<ESoGetDiscData>);
	TInt do_execute(Int2Type<ESoGetLocalName>);
	TInt do_execute(Int2Type<ESoGetRemoteName>);
	TInt do_execute(Int2Type<ESoReference>);  //new
	TInt do_execute(Int2Type<ESoSocketInfo>); // new

	TInt do_execute(Int2Type<ESoCreateWithSubConnection>);

	};

#endif

