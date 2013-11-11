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
#include "ctlbspsytestpositioner.h"
#include "ctlbspsytestconstants.h"
#include "testpsy.hrh"

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsPsyTestPositioner::CT_LbsPsyTestPositioner()
    {
    }

// EPOC default constructor can leave.
void CT_LbsPsyTestPositioner::ConstructL(TAny* aConstructionParameters)
    {
    BaseConstructL(aConstructionParameters);
	iTtfSimulator = CT_LbsPsyTtfSimulator::NewL(KPsyTtf);
    }

// Two-phased constructor.
CT_LbsPsyTestPositioner* CT_LbsPsyTestPositioner::NewLC(TAny* aConstructionParameters)
    {
    CT_LbsPsyTestPositioner* self = new (ELeave) CT_LbsPsyTestPositioner;
    CleanupStack::PushL(self); 
    self->ConstructL(aConstructionParameters);
    return self;
    }

// Two-phased constructor.
CT_LbsPsyTestPositioner* CT_LbsPsyTestPositioner::NewL(TAny* aConstructionParameters)
    {
	CT_LbsPsyTestPositioner* self = NewLC(aConstructionParameters);
    CleanupStack::Pop();
    return self;
    }

// Destructor
CT_LbsPsyTestPositioner::~CT_LbsPsyTestPositioner()
    {
    delete iTtfSimulator;
    }

// ---------------------------------------------------------
// CT_LbsPsyTestPositioner::NotifyPositionUpdate
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPsyTestPositioner::NotifyPositionUpdate(TPositionInfoBase& aPosInfo,
										 TRequestStatus& aStatus)
	{
    TPositionInfo* position = static_cast<TPositionInfo*> (&aPosInfo);
	TUid implUid = { KPosImplementationUid };   
	// Start the timer
    iTtfSimulator->Start(&aStatus, position, implUid);

	}


// ---------------------------------------------------------
// CT_LbsPsyTestPositioner::CancelNotifyPositionUpdate
//
// (other items were commentedin -a header).
// ---------------------------- ----------------------------
//
void CT_LbsPsyTestPositioner::CancelNotifyPositionUpdate()
	{
    iTtfSimulator->Cancel();
	}

//
//
//
// CT_LbsPsyTtfSimulator - Time to Fix simulator :)
//
//
//


// C++ default constructor
CT_LbsPsyTtfSimulator::CT_LbsPsyTtfSimulator() : CActive(EPriorityStandard), iTimer()
    {
    }

// EPOC default constructor
void CT_LbsPsyTtfSimulator::ConstructL(TInt aTimeout)
    {
    iTimeout = aTimeout;
    User::LeaveIfError(iTimer.CreateLocal());
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------
// CT_LbsPsyTtfSimulator::Start
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPsyTtfSimulator::Start(TRequestStatus* aStatus,
                             TPositionInfo* aPosition,
                             TUid aImplUid)
    {	
	aPosition->SetModuleId(aImplUid);
	
    TPosition position;
	position.SetAccuracy(KPsyHAcc, KPsyVAcc);
	position.SetCurrentTime();
	aPosition->SetPosition(position);

    iPositionStatus = aStatus;
    *iPositionStatus = KRequestPending;
    iTimer.After(iStatus, iTimeout);
    SetActive();
    }

// Two-phased constructor.
CT_LbsPsyTtfSimulator* CT_LbsPsyTtfSimulator::NewL(TInt aTimeout)
    {
    CT_LbsPsyTtfSimulator* self = NewLC(aTimeout);
    CleanupStack::Pop();
    return self;
    }

// Two-phased constructor.
CT_LbsPsyTtfSimulator* CT_LbsPsyTtfSimulator::NewLC(TInt aTimeout)
    {
    CT_LbsPsyTtfSimulator* self = new (ELeave) CT_LbsPsyTtfSimulator;
    CleanupStack::PushL(self);
    self->ConstructL(aTimeout);
    return self;
    }

// Destructor
CT_LbsPsyTtfSimulator::~CT_LbsPsyTtfSimulator()
    {
    Cancel();
    iTimer.Close();
    }

// ---------------------------------------------------------
// CT_LbsPsyTtfSimulator::RunL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPsyTtfSimulator::RunL()
   {
   // Flag that the request is finished
   User::RequestComplete(iPositionStatus, KErrNone);
   }

// ---------------------------------------------------------
// CT_LbsPsyTtfSimulator::DoCancel
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsPsyTtfSimulator::DoCancel()
   {
   iTimer.Cancel();
   User::RequestComplete(iPositionStatus, KErrCancel);
   }

//  End of File
