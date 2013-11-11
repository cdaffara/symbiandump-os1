/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file TDBT5Step.h
 @internalTechnology
*/
#ifndef __TDBT5_STEP_H__
#define __TDBT5_STEP_H__
#include <test/TestExecuteStepBase.h>
#include "Te_LbsBtGpsPsyStepBase.h"
#include <lbs.h>
#include <lbsCommon.h>


//
// ---------------------------------------------------------
// Async wait
class CAsyncWait : public CBase
    {
    public:
        static CAsyncWait* NewL();
        ~CAsyncWait();
        static void WaitL(TRequestStatus& aStatus);
        static TInt IdleCallBack(TAny *aAny);
        TInt IdleCall();
    private:
        CAsyncWait();
        void ConstructL();
    private:
        CActiveSchedulerWait* iWait;
        CIdle* iIdle;
        TRequestStatus* iStatus;
    };


class CTDBT5Step : public CTe_LbsBtGpsPsyStepBase
	{
public:
	CTDBT5Step();
	~CTDBT5Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	void LogL(TRefByValue<const TDesC> aMsg, ...);
	void LogL(const TDesC& aLog);
	void ShowDisplayResult(
            TInt aErr, 
            TPositionInfo& aPosInfo,
            TBool aShowDialog = EFalse);
	void StartL(TInt aIndex);
    //Location req
    void BasicLocationRequestL();
    //HGenericInfo
    void HGenericInfoTestL();
    //NMEA sentences log
    void NMEALogTestL();
    //NMEA partial long log
    void NMEALogPartialLongTestL();
    //NMEA multiple clients
    void NMEALogMultipleClientsL();
      
    void LocationRequestTestL(TBool aShowResult = EFalse);
    void LocReqByEComTestL();

    void FormatCoordinates(TDes& aDes, const TPositionInfo& aPosInfo);
    void FormatToDegrees(TDes& aDes, TReal64 aCoordinate);
    void GetNmeaData(TDes&aDes,const TPositionInfo& aPosInfo);
	};

_LIT(KTDBT5Step,"TDBT5Step");

#endif
