/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file te_lbsstatusstep.h
*/

#ifndef TE_LBSSTATUSSTEP_H
#define TE_LBSSTATUSSTEP_H

#include <e32base.h>
#include <lbs/lbspositioningstatus.h>

class CTe_LbsPositioningStatusStep;
class CLbsPositioningStatus;

class CTe_LbsStatusTestManager : public CActive, public MLbsPositioningStatusObserver
    {
private:
    enum TTestStep
        {
        EStepInitial,
        EStepFirstRequest,
        EStepSecondRequest,
        EStepThirdRequest,
        EStepFourthRequest,
        EStepFifthRequest,
        EStepStop
        };
public:
    static CTe_LbsStatusTestManager* NewL(CTe_LbsPositioningStatusStep& aTest);
    ~CTe_LbsStatusTestManager();
    
    void StartTest();
    
private: // from MLbsPositioningStatusObserver
    void OnPositioningStatusUpdate(const CLbsPositioningStatus::TLbsPositioningStatus& aPositioningStatus);
    
private:
    CTe_LbsStatusTestManager(CTe_LbsPositioningStatusStep& aTest);
    void ConstructL();
    void RunL();
    TInt RunError(TInt aError);
	void DoCancel();
    void CompleteSelf();
    
private:
    CTe_LbsPositioningStatusStep& iTest;
    TTestStep iStep;
    
    CLbsPositioningStatus* iPositioningStatus;
    RTimer iTimer;
    TUid    iMoPosStatusCategory;
    TUid    iNiPosStatusCategory;
    
    TBool iPosStatusUpdateReceived;
    CLbsPositioningStatus::TLbsPositioningStatus iReceivedPosStatus;
    TBool iUpdateExpected;
    TBool iTestFailed;

    };

#endif // TE_LBSSTATUSSTEP_H
