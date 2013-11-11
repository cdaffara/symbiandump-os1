// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <lbspositioninfo.h>
#include <lbssatellite.h>
#include "ctlbstestPsy4RequestHandler.h"

// ---------------------------------------------------------
// CT_LbsPsy4RequestHandler::InstanceL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//  
CT_LbsPsy4RequestHandler* CT_LbsPsy4RequestHandler::InstanceL()
    {
    CT_LbsPsy4RequestHandler* self;
    
	if (Dll::Tls())
		{
		self = reinterpret_cast<CT_LbsPsy4RequestHandler*> (Dll::Tls());
		}
    else
        {
		self = new (ELeave) CT_LbsPsy4RequestHandler;
        CleanupStack::PushL(self);
        self->ConstructL();
        CleanupStack::Pop(); // self
		Dll::SetTls(reinterpret_cast<TAny*>(self));
        }

	self->iReferenceCounter++;
	return self;
    }

// ---------------------------------------------------------
// ::Release
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPsy4RequestHandler::Release()
	{
	if (--iReferenceCounter == 0)
		{
        Dll::SetTls(NULL);
		delete this;
		}
	}

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsPsy4RequestHandler::CT_LbsPsy4RequestHandler() :
	CT_LbsTestPsyBase(NULL)
    {
	
	}

// Destructor
CT_LbsPsy4RequestHandler::~CT_LbsPsy4RequestHandler()
    {

	}

// ---------------------------------------------------------
// CT_LbsPsy4RequestHandler::CancelGetPositionInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPsy4RequestHandler::CancelNotifyPositionUpdate(TRequestStatus* aStatus)
	{
	TInt index = FindStatus(aStatus);
    if(index == KErrNone)
		{
		CompleteRequest(index, KErrCancel);
		}
	else if(index == KErrNotFound)
		{
		_LIT(KTestDes, "No preceding request");
		User::Panic(KTestDes, KErrNotFound);	
		}
	if(RequestsStatusCount() == 0)
		{
		Cancel();
        }
	}
