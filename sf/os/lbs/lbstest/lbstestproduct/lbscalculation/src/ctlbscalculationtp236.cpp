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



//  INCLUDES
#include "ctlbscalculationtp236.h"
#include "tlbscalculationcheck.h"

// C++ constructor 
CT_LbsCalculationTP236::CT_LbsCalculationTP236(CT_LbsCalculationServer& aParent) : CT_LbsCalculationStepBase(aParent)
    {
    _LIT(KTestName, "Calculation TP236 - Coordinate Move");
    SetTestStepName(KTestName);
    }

CT_LbsCalculationTP236::~CT_LbsCalculationTP236()
    {
    }

CT_LbsCalculationTP236* CT_LbsCalculationTP236::New(CT_LbsCalculationServer& aParent)
    {
    CT_LbsCalculationTP236* testStep = new CT_LbsCalculationTP236(aParent);
    // Note the lack of ELeave.
    // This means that having insufficient memory will return NULL;

    if (testStep)
        {
        TInt err = KErrNone;

        TRAP(err, testStep->ConstructL());
        if (err)
            {
            delete testStep;
            testStep = NULL;
            }
        }
    return testStep;
    }

TVerdict CT_LbsCalculationTP236::doTestStepL()
    {
    TestValuesL();
    return TestStepResult();
    }

void CT_LbsCalculationTP236::TestValuesL()
    {   
    _LIT(KOpenFailed, "File open failed with %d");
    
    TInt err=0; 
    err=ReadValues();
    AssertTrueSecL(err == KErrNone, KOpenFailed, err);

    TInt index=5;
    for (TInt i=0; i < iCalculationChecks.Count(); i++)
        {  
        
        T_LbsCalculationCheck cc = iCalculationChecks[i];    
        
        if (index==10) 
            cc.CheckMove();

        err=cc.CheckMove();
        LogError(err==KErrNone, cc, err, index);
         
        ++index;
        if (index == 21 || index == 26 || index == 33 || index == 38|| index == 40 || index == 45 || 
            index == 50 || index == 73 || index == 78 || index == 85 || index == 92 || index == 96 || 
            index == 33 || index == 106 || index == 123 || index == 128 || index == 133)
            {
            ++index;
            }
        }

    // Checks that KErrArgumenet is returned
    
    T_LbsCalculationCheck c;
    err=c.CheckMove();
    LogError(err==KErrNone, c, err);

    //
    // Test error report SBUY-65KBZD TCoordinate::Move returns wrong results
    // Note that the same calculation error is found in the calculations in the excel sheet
    // Therefore we need to test it here
    TCoordinate source(0, 0);
    TCoordinate target(0, 99);
    err=source.Move(90, 11000000);

    
    _LIT(KInfoMsg1, "Testing SBUY-65KBZD");
    INFO_PRINTF1(KInfoMsg1);

    // Sert accuracy to very low
    const TReal KAccuracy = 0.5;

    if (!Equals(source.Latitude(), target.Latitude(), KAccuracy) ||
        !Equals(source.Longitude(), target.Longitude(), KAccuracy))
        {
        _LIT(KErrorMsg1, "Move of TC(0,0) bearing 90, 11000000 meters returned wrong coord");
        LogErrorAndLeaveL(KErrorMsg1);
        }

    //
    TCoordinate source2(0, 0);
    TCoordinate target2(0, -90);
    err=source2.Move(90, 30000000);

    if (!Equals(source2.Latitude(), target2.Latitude(), KAccuracy) ||
        !Equals(source2.Longitude(), target2.Longitude(), KAccuracy))
        {
        _LIT(KErrorMsg2, "Move of TC(0,0) bearing 90, 30000000 meters returned wrong coord");
        LogErrorAndLeaveL(KErrorMsg2);
        }        
    //

    }

TBool CT_LbsCalculationTP236::Equals(const TReal aValue1, const TReal aValue2, const TReal aAccuracy)
    {
    TReal maxAllowedError = aAccuracy;

    TReal absError = Abs(aValue1-aValue2);
    
    return (absError < maxAllowedError); 

    }

void CT_LbsCalculationTP236::TestNotSupportedL()
    {
    T_LbsCalculationCheck c;
    c.SetMoveErrorCode(KErrNotSupported);
      
    TInt err=c.CheckMove();
    LogError(err==KErrNone, c, err);     
    }

void CT_LbsCalculationTP236::LogTestError(const T_LbsCalculationCheck& aCalc, 
                                     const TInt aError,
                                     const TInt aRow)
    {
    TBuf<KMaxLogLineLength> buf;
    TPosition source;
    TPosition target;
    aCalc.GetSourcePosition(source);
    aCalc.GetTargetPosition(target);
    
    if (aError==KErrCalculationCheckMove)
        {
        TCoordinate calcCoordinate;
        aCalc.GetCalcCoordinate(calcCoordinate);

        _LIT(KSpeedError, "Calculated coordinate is not equal to read target coordinate, values:");
        _LIT(KValues, "Read coordinate: Lat= %f, Long2= %f, calulated coordinate: Lat= %f, Long= %f, row %d"); 
            
		buf.Format(KValues, target.Latitude(), 
                            target.Longitude(), 
                            calcCoordinate.Latitude(), 
                            calcCoordinate.Longitude(),
                            aRow);
         
        ERR_PRINTF1(KSpeedError);
        }
     else if(aError==KErrCalculationCheckErrorCode)
        {
        _LIT(KIncorrectErrorCode, "Incorrect error code returned from Speed calculation lib:");
        _LIT(KCodes, "Lat1= %f, Long1= %f, Lat2= %f, Long2= %f, Returned = %d, Expected = %d, row= %d");
        
        buf.Format(KCodes, source.Latitude(), 
                           source.Longitude(), 
                           target.Latitude(), 
                           target.Longitude(), 
                           aCalc.ReturnedErrorCode(), 
                           aCalc.SpeedErrorCode(), aRow);

        ERR_PRINTF1(KIncorrectErrorCode);
        }
        
    ERR_PRINTF1(buf);
    }

//  End of File
