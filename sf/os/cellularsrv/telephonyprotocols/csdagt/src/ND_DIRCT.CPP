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
// Direct Connection State Machine
// 
//

/**
 @file Nd_dirct.cpp
*/

#include "ND_DIRCT.H"
#include "ND_SCR.H"
#include "ND_DBACC.H"
#include "ND_STD.H"
#include "ND_DIRECTSTATES.H"

CDirCtAgXSM* CDirCtAgXSM::NewL(MAgentNotify& aControllerObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess)
/**
2 phased constructor, first phase.

@param aControllerObserver a reference to state machine observer.
@param aDlgPrc a pointer to dialog processor.
@param aDbAccess a referecen to CommDB accessor
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CDirCtAgXSM object.
*/
	{
	CDirCtAgXSM* r=new(ELeave) CDirCtAgXSM(aControllerObserver,aDlgPrc,aDbAccess);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CDirCtAgXSM::~CDirCtAgXSM()
/**
Destructor. Cancels active requests.
*/
	{
	Cancel();
	}

CDirCtAgXSM::CDirCtAgXSM(MAgentNotify& aControllerObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess)
	: CNetdialSM(aControllerObserver, aDlgPrc,aDbAccess)
/**
Private constructor used in the first phase of construction.

@param aControllerObserver a reference to state machine observer.
@param aDlgPrc a pointer to the dialog processor.
@param aDbAccess a reference to the database accessor.
*/
	{}

void CDirCtAgXSM::ConstructL()
/**
Private constructor used in the first phase of construction.

@exception Leaves if CommDB accessor or direct connection state machine creation leaves.
*/
	{
	//Instantiate Member variables
	iDbAccess=CCommsDbNetDialAccess::NewL(iDb);

	//Start off the state machine with an Initial state which contains no functionality
	iState=CDirCtInit::NewL(*this,*this);
	}

void CDirCtAgXSM::SetUpScriptL()
/**
Sets up script for direct connection

@exception Leaves if GetCommPortForDirectConnectL(), GetScriptDetailsL() or script creation leaves.
*/
	{
	RCall::TCommPort commPort;
	iDbAccess->GetCommPortForDirectConnectL(commPort);
	TCommRole role;
	iDbAccess->GetCommPortRoleL(role);
	TInt len;
	iDbAccess->GetScriptDetailsL(iUseScript,len);
	
	if (!iUseScript || len==0)	
		{		// set them consistently
		iUseScript=EFalse;
		}
	
	if (iNdScript != NULL)
	    {
        delete iNdScript;
        iNdScript = NULL;
	    }
	
	iNdScript=CNetDialScript::NewL(iDbAccess,iDlgPrc,commPort,role,len);	// create it whatever because we need other stuff
	}

TInt CDirCtAgXSM::Notification(TNifToAgentEventType /*aEvent*/, TAny* /*aInfo*/)
/**
Not supported by direct connection state machine.
*/
	{
	return KErrNotSupported;
	}

TInt CDirCtAgXSM::IncomingConnectionReceived()
/**
Not supported by direct connection state machine.
*/
	{
	return KErrNotSupported;
	}

