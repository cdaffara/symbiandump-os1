// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "CSimPsyNmeaPosRequester.h"
#include "CSimPsyMultiNmeaPosRequester.h"
#include "EPos_CPosSimulationPositioner.h"          
#include "s32file.h"
#include "EPos_SimPsyConstants.h"


// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSimPsyNmeaPosRequester::CSimPsyNmeaPosRequester(CSimPsyMultiNmeaPosRequester* aParent) 
: CActive(EPriorityStandard), 
    iParent(aParent)
    {
    CActiveScheduler::Add(this);
    }
     
// Destructor
CSimPsyNmeaPosRequester::~CSimPsyNmeaPosRequester()
    {
    // RDebug::Print(_L("NmeaPosRequester::dtor"));
    Cancel();
    iPosition = NULL;
    delete iPositioner;
    }

void CSimPsyNmeaPosRequester::ConstructL()
    {
	TUid uid; 
	uid.iUid = KPosSimulationPsyImplUid;

	iConstructParams.iImplementationUid = uid;
	iConstructParams.iStatusObserver = this;
	iConstructParams.iParamObserver = this;

    iPositioner = CPosSimulationPositioner::NewL(&iConstructParams);
    }

CSimPsyNmeaPosRequester* CSimPsyNmeaPosRequester::NewL(CSimPsyMultiNmeaPosRequester* aParent)
    {
    CSimPsyNmeaPosRequester* self = new (ELeave) CSimPsyNmeaPosRequester(aParent); //i==1
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    
    }

void CSimPsyNmeaPosRequester::MakeRequest(TPositionInfo* aPositionInfo)
    {
    // RDebug::Print(_L("NmeaRequester::MakeRequest"));

    iStartTime.UniversalTime();
	iPosition = aPositionInfo;
    iPositioner->NotifyPositionUpdate(*iPosition, iStatus);
    SetActive();  //pang
    }

void CSimPsyNmeaPosRequester::CancelRequest()
    {
    iPositioner->CancelNotifyPositionUpdate();
    }

void CSimPsyNmeaPosRequester::RunL()
    {    
    // RDebug::Print(_L("NmeaRequester::RunL"));
    TTime now;
    now.UniversalTime();
    iRequestTime = now.MicroSecondsFrom(iStartTime);
    iParent->HandleRequestDone();    
    }

void CSimPsyNmeaPosRequester::DoCancel()
    {
    // RDebug::Print(_L("NmeaRequester::DoCancel"));
    iPositioner->CancelNotifyPositionUpdate();
    TTime now;
    now.UniversalTime();
    iRequestTime = now.MicroSecondsFrom(iStartTime);    
    iParent->HandleRequestDone();
    }

void CSimPsyNmeaPosRequester::GetResult(
    TInt& aStatus, 
	TTimeIntervalMicroSeconds& aRequestTime)
    {    
    aStatus = iStatus.Int();
    aRequestTime = iRequestTime;
    }

void CSimPsyNmeaPosRequester::SetPartialUpdateRestriction(
    TBool aRestriction)
    {
    iPartialUpdateRestriction = aRestriction;
    }

void CSimPsyNmeaPosRequester::ReportStatus(
        /* IN  */   const TPositionModuleId& /*aImplementationUid*/,
        /* IN  */   const TPositionModuleStatus& /*aStatus*/)
	{
	;
	}
		
// From MPosParameterObserver
TPositionInfoClassType CSimPsyNmeaPosRequester::RequestedClasses() const
	{
	return 1;	
	}

TInt CSimPsyNmeaPosRequester::GetRequiredPositionQuality(TPositionQuality& /*aRequestedPosQuality*/) const
	{
	return KErrNone;
	}

TInt CSimPsyNmeaPosRequester::QueryId() const
	{
	return 1;
	}

void CSimPsyNmeaPosRequester::GetMaxAge(TTime& /*aMaxAge*/) const
	{
	;
	}

TBool CSimPsyNmeaPosRequester::IsPartialUpdateAllowed() const
	{
	return iPartialUpdateRestriction;
	}

// End of File
