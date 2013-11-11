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
// Dial In State Machine
// 
//

/**
 @file Nd_dlin.cpp 
*/

#include "ND_DLIN.H"
#include "ND_ETEL.H"
#include "ND_STD.H"
#include "Nd_dlinStates.h"
#include "ND_DBACC.H"

// Dial In state machine

CDlInAgXSM* CDlInAgXSM::NewL(MAgentNotify& aObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess)
/**
2 phased constructor, first phase.

@param aObserver a reference to state machine observer.
@param aDlgPrc a pointer to dialog processor.
@param aDbAccess a referecen to CommDB accessor
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDlInAgXSM object.
*/
	{
	CDlInAgXSM* r=new(ELeave) CDlInAgXSM(aObserver,aDlgPrc,aDbAccess);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDlInAgXSM::CDlInAgXSM(MAgentNotify& aObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess)
	: CNetdialSM(aObserver,aDlgPrc,aDbAccess)
/**
Private constructor used in the first phase of construction.

@param aObserver a reference to state machine observer.
@param aDlgPrc a pointer to the dialog processor.
@param aDbAccess a reference to the database accessor.
*/
	{}

CDlInAgXSM::~CDlInAgXSM()
/**
Destructor. Cancels active requests.
*/
	{
	Cancel();
	}

void CDlInAgXSM::ConstructL()
/**
Private constructor used in the first phase of construction.
Calls base call constructor CNetdialSM::ConstructL() and calls CDlInInit::NewL().

@exception Leaves if CNetdialSM::ConstructL() or dial in init state creation leaves.
*/
	{
	CNetdialSM::ConstructL();
	//Start off the state machine with an Initial state which contains no functionality
	iState=CDlInInit::NewL(*this,*this);
	}

TBool CDlInAgXSM::UseScript() const
/**
Returns EFalse.

@return EFalse.
*/
	{
	return EFalse;
	}

void CDlInAgXSM::SetUpScriptL()
/**
Panics if called.

@exception Panics with EScriptingUnsupportedInDialIn.
*/
	{
	NetDialPanic(EScriptingUnsupportedInDialIn);
	}

TBool CDlInAgXSM::IsReconnect() const
/**
Returns EFalse.

@return EFalse.
*/
	{
	return EFalse;
	}

TInt CDlInAgXSM::Notification(TNifToAgentEventType aEvent, TAny* /*aInfo*/)
/**
Act on / reply to NifMan's notification requests
*/
	{
	switch (aEvent)
		{
	case ENifToAgentEventTypePPPCallbackGranted:
		return KErrNotSupported;
	case ENifToAgentEventTypeQueryIsDialIn:
		return ENDDialTypeDialIn;
	default:
		return KErrNone;
		}
	}

TInt CDlInAgXSM::IncomingConnectionReceived()
/**
Informs observer about incoming connection with IncomingConnectionReceived().
*/
	{
	return iControllerObserver->IncomingConnectionReceived();
	}
