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
* Description: 
*    Implementation for the CDosEventSndService class
*
*/


#include "doseventsndservice.h"
#include "doseventmanagerbase.h"
#include "dosclientserver.h"
#include "dosserver.h"

#include "dos_debug.h"

#include <e32svr.h>

//
// ---------------------------------------------------------
// CDosEventSndService::NewL
// ---------------------------------------------------------
//  

CDosEventSndService* CDosEventSndService::NewL(CEventManagerBase* aEventManager)
{
	CDosEventSndService* result=new (ELeave) CDosEventSndService;
	CleanupStack::PushL(result);
	result->ConstructL(aEventManager);
	CleanupStack::Pop();

	return result;
}

//
// ---------------------------------------------------------
// CDosEventSndService::ConstructL
// ---------------------------------------------------------
//  

void CDosEventSndService::ConstructL(CEventManagerBase* aEventManager)
{
	iEventManager = aEventManager;
}

//
// ---------------------------------------------------------
// CDosEventSndService ExecuteMessageL
// ---------------------------------------------------------
//  

TInt CDosEventSndService::ExecuteMessageL(const RMessage2& aMessage)
{
	if(aMessage.Function()==EDosEventFiring)
	{
		return iEventManager->RaiseEvent(aMessage);
	}

    COM_TRACE_( "CDosEventSndService ExecuteMessageL - PLANNING TO PANIC CLIENT!!!" );

	PanicClient(aMessage,EPanicIllegalFunction);
	return KErrNone;
}

