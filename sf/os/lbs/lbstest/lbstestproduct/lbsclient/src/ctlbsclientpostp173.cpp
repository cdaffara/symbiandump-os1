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




//  INCLUDES
#include "ctlbsclientpostp173.h"
#include <badesca.h>
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>
#include <lbscommon.h>
#include <lbspositioninfo.h>
#include "tctlbsclientmodulesobserver.h"


void MyStopActive(TAny* /*aAny*/)
    {
    CActiveScheduler::Stop();
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp173::CT_LbsClientPosTp173(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{
	_LIT(KTestName, "TP173 - Notify Module Events");
	SetTestStepName(KTestName); 
	}


// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp173::~CT_LbsClientPosTp173()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp173::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp173::StartL()
    {
    iNoEvents = EFalse;
    iError = EFalse;
    // This test listens for events.
    // There is no data in the event, so all that is needed to be done is
    // to see that an event is generated. This test doesn't fail if unsuccessful,
    // it hangs.

    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);

    db->SetObserverL(*this);

    TInt prio = db->PriorityL(iUidTestPsy2);
    if (prio > 0)
        {
        prio = 0;
        }
    else
        {
        prio = 1; // this assumes that more that one PSY is installed
        }

    CPosModuleUpdate* visibileUpdate = CPosModuleUpdate::NewLC();
    visibileUpdate->SetUpdateVisibility(ETrue);
    db->UpdateModuleL(iUidTestPsy2, *visibileUpdate);
    
    db->SetModulePriorityL(iUidTestPsy2, prio);
    CleanupStack::PopAndDestroy(visibileUpdate);
    

    // Wait for event
    DoEventHandlingL();

    CPosModuleUpdate* availUpdate = CPosModuleUpdate::NewLC();


	TPositionModuleInfo info;
	db -> GetModuleInfoL(iUidTestPsy2, info);
	TBool available = info.IsAvailable();

    availUpdate->SetUpdateAvailability(/*ETrue*/ !available);
    db->UpdateModuleL(iUidTestPsy2, *availUpdate);
    
    CleanupStack::PopAndDestroy(availUpdate);

    // Wait for event
    DoEventHandlingL();

    CPosModuleUpdate* costUpdate = CPosModuleUpdate::NewLC();
	
	TPositionQuality quality;
	info.GetPositionQuality(quality);
	TPositionQuality::TCostIndicator indicator = quality.CostIndicator();
    switch (indicator)
    {
        case TPositionQuality::ECostUnknown:
            indicator = TPositionQuality::ECostZero;
            break;
        case TPositionQuality::ECostZero:
            indicator = TPositionQuality::ECostPossible;
            break;
        case TPositionQuality::ECostPossible:
            indicator = TPositionQuality::ECostCharge;
            break;
        case TPositionQuality::ECostCharge:
            indicator = TPositionQuality::ECostPossible;
            break;
        default:
        	_LIT(KErrorAndLeave, "quality indicator does not exist");
            LogErrorAndLeaveL(KErrorAndLeave);
    }

    costUpdate->SetUpdateCost(/*TPositionQuality::ECostCharge*/ indicator);
    db->UpdateModuleL(iUidTestPsy2, *costUpdate);
    
    CleanupStack::PopAndDestroy(costUpdate);

    // Wait for event
    DoEventHandlingL();

    //

    CPosModuleUpdate* visibilityUpdate = CPosModuleUpdate::NewLC();
    visibilityUpdate->SetUpdateVisibility(EFalse);
    db->UpdateModuleL(iUidTestPsy2, *visibilityUpdate);
    CleanupStack::PopAndDestroy(visibilityUpdate);
    
    DoEventHandlingL();
    
    //
    
    CMyNotificationTimer* timer = CMyNotificationTimer::NewLC(this);
	
    TTimeIntervalMicroSeconds ttfUpdate(1000000);
    TPositionQuality dynQualityUpdate;
    dynQualityUpdate.SetHorizontalAccuracy(30.0);
    dynQualityUpdate.SetTimeToNextFix(ttfUpdate);

    timer->StartNotificationTimer();
    iNoEvents = ETrue;
    // Wait for event
    DoEventHandlingL();
    
//
   
   
    
    
    db->RemoveObserver();
    CleanupStack::PopAndDestroy(2, db);
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp173::DoEventHandlingL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp173::DoEventHandlingL()
    {
	//User::After(20);
	CActiveScheduler::Start();
    CheckErrorL();
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp173::CheckErrorL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp173::CheckErrorL()
    {
    if (iError)
        {
        LogErrorAndLeaveL(iErrorText);
        }
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp173::HandleDatabaseChangeL
//
// (other items were commented in a header).
// ---------------------------------------------------------
void CT_LbsClientPosTp173::HandleSettingsChangeL(TPosModulesEvent /*aModuleEvent*/)
    {
       
    CleanupStack::PushL(TCleanupItem(MyStopActive,NULL));
    if (iNoEvents)
        {
        _LIT(KErrorText, "Event received when no event should have been received");
        iErrorText = KErrorText;
        iError = ETrue;
        }

    CleanupStack::PopAndDestroy();
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp173::AssertTrueSecL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp173::AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode)
    {
    if (!aCondition)
        {
        TBuf<100> buf;
        buf.Format(aErrorMsg, aErrorCode);
        LogErrorAndLeaveL(buf);
        }
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp173::NotifyTimeout
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp173::NotifyTimeout()
	{
    CleanupStack::PushL(TCleanupItem(MyStopActive, NULL));
    if (!iNoEvents)
        {
        _LIT(KErrorText, "No event received when event should have been received");
        iErrorText = KErrorText;
        iError = ETrue;
        }

    CleanupStack::PopAndDestroy();
	}

// ---------------------------------------------------------
// CNotificationTimer::~CNotificationTimer
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
//Implementation of a CNotificationTimer

// destructor
CMyNotificationTimer::~CMyNotificationTimer()
	{
	Cancel();
	iMainControl = NULL;
	}

// ---------------------------------------------------------
// CNotificationTimer::NewLC
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
// Two-phased constructor.
CMyNotificationTimer* CMyNotificationTimer::NewLC(CT_LbsClientPosTp173* aMainControl)
	{
	CMyNotificationTimer* self=new (ELeave) CMyNotificationTimer(aMainControl);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------
// CNotificationTimer::NewL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
// Two-phased constructor.
CMyNotificationTimer* CMyNotificationTimer::NewL(CT_LbsClientPosTp173* aMainControl)
	{
	CMyNotificationTimer* self = NewLC(aMainControl);
	CleanupStack::Pop();
	return self;
	}
	
// ---------------------------------------------------------
// CNotificationTimer::StartNotificationTimer
//
// (other items were commented in a header).
// ---------------------------------------------------------
void CMyNotificationTimer::StartNotificationTimer()
	{
	CTimer::After(iNotificationTimout);
	}

// ---------------------------------------------------------
// CNotificationTimer::DoCancel
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CMyNotificationTimer::DoCancel()
	{
	CTimer::DoCancel();
	}

// ---------------------------------------------------------
// CNotificationTimer::ConstructL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CMyNotificationTimer::ConstructL()
	{
	CTimer::ConstructL();
	iNotificationTimout = 5000000;
	//add this object to the active scheduler
	CActiveScheduler::Add(this); 
	}

// ---------------------------------------------------------
// CNotificationTimer::CNotificationTimer
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
//c++ default constructor
CMyNotificationTimer::CMyNotificationTimer(CT_LbsClientPosTp173* aMainControl)
:CTimer(CActive::EPriorityStandard)
	{
	iMainControl = aMainControl;
	}
        
// ---------------------------------------------------------
// CNotificationTimer::RunL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CMyNotificationTimer::RunL()
	{
	iMainControl->NotifyTimeout();
	}


