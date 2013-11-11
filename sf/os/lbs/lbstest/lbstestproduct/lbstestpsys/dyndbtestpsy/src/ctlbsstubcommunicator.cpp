// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "ctlbsstubcommunicator.h"
#include "dyndbtestpsy.hrh"
#include <lbspositioninfo.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CT_LbsStubCommunicator::Close
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsStubCommunicator::Close()
	{
	if (--iReferenceCounter == 0)
		{
        Dll::SetTls(NULL);
		delete this;
		}
	}

// ---------------------------------------------------------
// CT_LbsStubCommunicator::GetInstanceL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
CT_LbsStubCommunicator* CT_LbsStubCommunicator::GetInstanceL()
	{
    CT_LbsStubCommunicator* self;
    TAny* tlsptr = Dll::Tls();
	if (tlsptr == NULL)
		{
        self = CT_LbsStubCommunicator::NewL();
		Dll::SetTls(REINTERPRET_CAST(TAny*, self));
		}
    else
        {
        self = REINTERPRET_CAST(CT_LbsStubCommunicator*, Dll::Tls());
        }
	self->iReferenceCounter++;
	return self;
	}

// ---------------------------------------------------------
// CT_LbsStubCommunicator::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
void CT_LbsStubCommunicator::NotifyPositionUpdate(
	TPositionInfoBase& aPosInfo, 
	TRequestStatus& aStatus)
	{
    iRequestStatus = &aStatus;
	iPosition = static_cast<TPositionInfo*> (&aPosInfo);

	TPosition position;

    switch (iRequestCounter++)
        {
        case 0:
			position.SetAccuracy(25.0, 0); //Horizontal is set
            iTimeout = 4250000;
            break;
        case 1:
			position.SetAccuracy(30.0, 0); //Horizontal is set
            iTimeout = 5250000;
            break;
        case 2:
			position.SetAccuracy(15.0, 0); //Horizontal is set
            iTimeout = 3250000;
            iRequestCounter = 0;
            break;
        default:
            break;
        }
	
	position.SetCoordinate(1, 2, 3);

	TInt64 myint64(iTimeout);
	TTimeIntervalMicroSeconds timeint(myint64);
    TTime t;
    //t.HomeTime();
    t.UniversalTime();
    t+=timeint;
	position.SetTime(t);
	iPosition->SetPosition(position);
	
	TUid implUid = { KPosImplementationUid };
	iPosition->SetModuleId(implUid);

    iTimer.After(iStatus, iTimeout);
    SetActive();
	}

// ---------------------------------------------------------
// CT_LbsStubCommunicator::CancelNotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsStubCommunicator::CancelNotifyPositionUpdate()
	{
    Cancel();    
	}

// Two-phased constructor.
CT_LbsStubCommunicator* CT_LbsStubCommunicator::NewL()
    {
    CT_LbsStubCommunicator* self = CT_LbsStubCommunicator::NewLC();    
    CleanupStack::Pop();
    return self;
    }

// Two-phased constructor.
CT_LbsStubCommunicator* CT_LbsStubCommunicator::NewLC()
    {
    CT_LbsStubCommunicator* self = new (ELeave) CT_LbsStubCommunicator;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsStubCommunicator::CT_LbsStubCommunicator() 
: CActive(1),  
iReferenceCounter(0), iTimeout(3000000) 
    {
    CActiveScheduler::Add(this);
    }

// EPOC default constructor can leave.
void CT_LbsStubCommunicator::ConstructL()
    {
    User::LeaveIfError(iTimer.CreateLocal());
    }

// Destructor
CT_LbsStubCommunicator::~CT_LbsStubCommunicator()
    {
    Cancel();
    iTimer.Close();
	iPosition = NULL;
    }

// ---------------------------------------------------------
// CT_LbsStubCommunicator::RunL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsStubCommunicator::RunL()
    {
    User::RequestComplete(iRequestStatus, KErrNone);
    }


// ---------------------------------------------------------
// CT_LbsStubCommunicator::DoCancel
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsStubCommunicator::DoCancel()
    {
    iTimer.Cancel();
    User::RequestComplete(iRequestStatus, KErrCancel);
    }

//  End of File  
