/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <e32std.h>
#include <e32base.h>
#include <lbspositioninfo.h>
#include <lbscriteria.h>

#include "lbstestlocmonitormessageenums.h"

#include "lbstestlocmonitorserver.h"
#include "lbstestlocmonitorsession.h"
#include "lbstestlocmonitorsubsession.h"
#include "Utility.h"

CTe_LocMonitorServerSession::CTe_LocMonitorServerSession()
:	CSession2()
	{
	__DECLARE_NAME(_S("CTe_LocMonitorServerSession"));
	}



void CTe_LocMonitorServerSession::CreateL()
//
// 2nd phase construct for sessions - called by the CServer framework
//
	{
	iObjectIx = CObjectIx::NewL();
	iContainerIx = CObjectConIx::NewL();
	iObjectCon = iContainerIx->CreateL();
	Server().AddSession();
	}


CTe_LocMonitorServerSession::~CTe_LocMonitorServerSession()
	{
	if (iObjectIx)
		{
		TInt count = iObjectIx->Count();

		// Close any objects that the client did not close
		for(TInt i=0; i<count; i++)
			{
			CObject* theObj = (*iObjectIx)[i];
			if (theObj)
				{
				TInt handle = iObjectIx->At(theObj);
				iObjectIx->Remove(handle);
				}
			}
		delete iObjectIx;
		}

	if (iContainerIx)
		{
		iContainerIx->Remove(iObjectCon);
		delete iContainerIx;
		}


	Server().DropSession();
	}

void CTe_LocMonitorServerSession::ServiceL(const RMessage2& aMessage)
	{
	// Only requests that leave when they are setting up are completed
	// here. Cannot complete all requests as a couple are asynchronous.

	TRAPD(error, DispatchL(aMessage))
	if (error)
		aMessage.Complete(error);
	}

void CTe_LocMonitorServerSession::DispatchL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
        {

	case EOpenPositioner:
		NewPositionerL(aMessage);
		break;

	case EClosePositioner:
		ClosePositionerL(aMessage);
		break;
	//
	// Requests for sub-sessions
	default:
		CTe_LocMonitorServerSubSession* theObj = static_cast<CTe_LocMonitorServerSubSession*>(iObjectIx->At(aMessage.Int3()));
		theObj->ServiceL(aMessage);
		return;
        }
	}


void CTe_LocMonitorServerSession::NewPositionerL(const RMessage2& aMessage)
	{
	CTe_LocMonitorServerSubSession* newObject = CTe_LocMonitorServerSubSession::NewL();
	CleanupClosePushL(*newObject);
	iObjectCon->AddL(newObject);
	TInt handle = iObjectIx->AddL(newObject);
	TPckgC<TInt> pH(handle);
	Utility::Write(aMessage, 3, pH);
	CleanupStack::Pop(newObject);
	aMessage.Complete(KErrNone);
	}

void CTe_LocMonitorServerSession::ClosePositionerL(const RMessage2& aMessage)
	{
	CObject* theObj = reinterpret_cast<CObject*>(iObjectIx->At(aMessage.Int3()));

	if (theObj == NULL)
		aMessage.Complete(KErrBadHandle);
	else
		{
		CObject* theObject = reinterpret_cast<CObject*>(theObj->Owner());
		iObjectIx->Remove(aMessage.Int3());
		if (theObject)
			theObject->Close();
		aMessage.Complete(KErrNone);
		}
	}

