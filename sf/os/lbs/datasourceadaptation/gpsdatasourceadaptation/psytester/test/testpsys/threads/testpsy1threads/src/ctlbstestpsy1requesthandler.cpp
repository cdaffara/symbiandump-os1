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
#include <e32std.h>
#include "ctlbstestpsy1requesthandler.h"
  
CT_LbsTestPsy1RequestHandler* CT_LbsTestPsy1RequestHandler::InstanceL()
    {
    CT_LbsTestPsy1RequestHandler* self;
    
	if (Dll::Tls())
		{
		self = reinterpret_cast<CT_LbsTestPsy1RequestHandler*> (Dll::Tls());
		}
    else
        {
		self = new (ELeave) CT_LbsTestPsy1RequestHandler;
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
void CT_LbsTestPsy1RequestHandler::Release()
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
CT_LbsTestPsy1RequestHandler::CT_LbsTestPsy1RequestHandler() :
    CT_LbsTestPsyBase(NULL)
    {

	}

// Destructor
CT_LbsTestPsy1RequestHandler::~CT_LbsTestPsy1RequestHandler()
    {
        
	}

void CT_LbsTestPsy1RequestHandler::NotifyPositionUpdate(TPositionInfo* aPosInfo, TRequestStatus* aStatus)
    {
    ++iRequests;

    if(iRequests == 1)
        {
        TInt err;
        
        _LIT(KNewProcessName, "Micke's_PSY TESTER");
                    
        err = User::RenameProcess(KNewProcessName);
        
        if(KErrNone == err)
        	{
        	RProcess().SetPriority(::EPriorityHigh);
        	}
        }
            
	CT_LbsTestPsyBase::NotifyPositionUpdate(aPosInfo, aStatus);
	}

