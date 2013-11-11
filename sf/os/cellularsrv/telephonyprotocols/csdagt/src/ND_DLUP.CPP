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
// Dial Up State Machine
// 
//

/**
 @file Nd_dlup.cpp 
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ND_DLUPTraces.h"
#endif

#include "ND_DLUP.H"
#include "ND_ETEL.H"
#include "ND_SCR.H"
#include <comms-infras/dialogprocessor.h>
#include <comms-infras/eventlogger.h>
#include "ND_DBACC.H"
#include "ND_STD.H"
#include "Nd_dlupStates.h"
#include <comms-infras/nifprvar.h>

CDlUpAgXSM* CDlUpAgXSM::NewL(MAgentNotify& aObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess)
/**
Dial Up state machine

2 phased constructor, first phase.

@param aObserver a reference to state machine observer.
@param aDlgPrc a pointer to dialog processor.
@param aDbAccess a referecen to CommDB accessor
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDlUpAgXSM object.
*/
	{
	CDlUpAgXSM* r=new(ELeave) CDlUpAgXSM(aObserver,aDlgPrc,aDbAccess);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDlUpAgXSM::CDlUpAgXSM(MAgentNotify& aObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess)
	: CNetdialSM(aObserver,aDlgPrc,aDbAccess)
/**
Private constructor used in the first phase of construction.

@param aObserver a reference to state machine observer.
@param aDlgPrc a pointer to the dialog processor.
@param aDbAccess a reference to the database accessor.
*/
	{}

CDlUpAgXSM::~CDlUpAgXSM()
/**
Destructor. Cancels active requests.
*/
	{
	Cancel();
	}

void CDlUpAgXSM::ConstructL()
/**
Private constructor used in the first phase of construction.
Calls base call constructor CNetdialSM::ConstructL() and calls CDlUpInit::NewL().

@exception Leaves if CNetdialSM::ConstructL() or dial up init state creation leaves.
*/
	{
	CNetdialSM::ConstructL();
	//Start off the state machine by instantiating an Initial state
	//which contains little functionality
	iState=CDlUpInit::NewL(*this,*this);
	}

void CDlUpAgXSM::SetUpScriptL()
/**
Setups scripts for dialup connection
Loans dataport from ETel server.
Gets script details from CommDB accessor by calling GetScriptDetailsL().
If script is not used, sets iUseScript to EFalse.
Creates CNetDialScript.

@exception Leaves if GetCommPortL(), GetScriptDetailsL() or NewL() leaves.
*/
	{
	RCall::TCommPort commPort;
	iTelServer->GetCommPortL(commPort);
	TCommRole role;
	iDbAccess->GetCommPortRoleL(role);
	TInt len;
	iDbAccess->GetScriptDetailsL(iUseScript,len);
	if (!iUseScript || len==0)	
		{		// set them consistently
		iUseScript=EFalse;
		}
		iNdScript=CNetDialScript::NewL(iDbAccess,iDlgPrc,commPort,role,len);	// create it whatever because we need other stuff
	}

TInt CDlUpAgXSM::Notification(TNifToAgentEventType aEvent, TAny* /*aInfo*/)
/**
Act on / reply to NifMan's notification requests
*/
	{
	switch (aEvent)
		{
	case ENifToAgentEventTypePPPCallbackGranted:
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CDLUPAGXSM_NOTIFICATION_1, "NetDial:\tCallback Request Granted");
		return KErrNone;
		}
	case ENifToAgentEventTypeQueryIsDialIn:
		{
		if(iCallBack)
			{
			return ENDDialTypeCallBackDialIn;
			}
		else
			{
			return ENDDialTypeDialOut;
			}
		}
	case ENifToAgentEventTypeLinkLayerDown:
		{
		// stop the logging, the connection has been dropped
		Logger()->LogDataUpdateEvent(R_LOG_CON_DISCONNECTED, KLogDataEventTypeUid);
		// does nothing if logging not started
		return KErrNone;
		}
	default:
		return KErrNone;
		}
	}

TInt CDlUpAgXSM::IncomingConnectionReceived()
/**
Incoming connection is not supported in dial up.
*/
	{	
	return KErrNotSupported;
	}
