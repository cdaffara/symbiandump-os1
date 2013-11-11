/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Implementation for the CDosEventSender class.
*
*/


// INCLUDE FILES
#include "doseventsender.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDosEventSender::CDosEventSender
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CDosEventSender::CDosEventSender()
{
	iDosServer = NULL;
	iDestroyDosServer = EFalse;
}

// Destructor
EXPORT_C CDosEventSender::~CDosEventSender()
{
	iEventSender.Close();
	if (iDestroyDosServer)
	{
		iDosServer->Close();
		delete iDosServer;
	}
}

// ---------------------------------------------------------
// CDosEventSender::NewL (Not owned Session)
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CDosEventSender* CDosEventSender::NewL(RDosServer& aDosServer)
{
	CDosEventSender* result = new (ELeave) CDosEventSender;
	CleanupStack::PushL(result);
	result->ConstructL(&aDosServer);
	CleanupStack::Pop();

	return result;
}

// ---------------------------------------------------------
// CDosEventSender::NewL (Owned Session)
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CDosEventSender* CDosEventSender::NewL()
{
	CDosEventSender* result = new (ELeave) CDosEventSender;
	CleanupStack::PushL(result);
	result->iDosServer = new RDosServer;
	if(result->iDosServer)
	{
		result->iDosServer->Connect();
		result->iDestroyDosServer = ETrue;
		result->ConstructL(result->iDosServer);
	}
	else
		User::Leave(KErrNoMemory);
	CleanupStack::Pop();

	return result;
}

// ---------------------------------------------------------
// CDosEventSender::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CDosEventSender::ConstructL(RDosServer* aDosServer)
{
	//Construction
	iDosServer = aDosServer;
	iEventSender.Register(*iDosServer);
}

// ========================== OTHER EXPORTED FUNCTIONS =========================

// ---------------------------------------------------------
// CDosEventSender::GeneralEvent
// ---------------------------------------------------------
//
EXPORT_C void CDosEventSender::GeneralEvent(TInt32 aEvent,TAny* aParameter,TInt aParSize)
{
	TPtr8 ptr(REINTERPRET_CAST(TText8*,aParameter),aParSize,aParSize);
	TIpcArgs args(aEvent, &ptr, aParSize);
	iEventSender.SendEvent(args);
}
