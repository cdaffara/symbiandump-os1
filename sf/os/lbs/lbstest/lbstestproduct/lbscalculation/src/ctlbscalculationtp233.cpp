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
#include "ctlbscalculationtp233.h"
#include "tlbscalculationcheck.h"

// C++ constructor 
CT_LbsCalculationTP233::CT_LbsCalculationTP233(CT_LbsCalculationServer& aParent) : CT_LbsCalculationStepBase(aParent)
    {
    _LIT(KTestName, "Calculation TP233 - Calculate Distance");
    SetTestStepName(KTestName);
    }

CT_LbsCalculationTP233::~CT_LbsCalculationTP233()
    {
    }

CT_LbsCalculationTP233* CT_LbsCalculationTP233::New(CT_LbsCalculationServer& aParent)
    {
    CT_LbsCalculationTP233* testStep = new CT_LbsCalculationTP233(aParent);
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


TVerdict CT_LbsCalculationTP233::doTestStepL()
    {
    TestValuesL();
    return TestStepResult();
    }

void CT_LbsCalculationTP233::TestValuesL()
    {   
    _LIT(KOpenFailed, "File open failed with %d");
    
    TInt err=0; 
    err=ReadValues();
    AssertTrueSecL(err == KErrNone, KOpenFailed, err);

    TInt index=5;
    for (TInt i=0; i< iCalculationChecks.Count(); i++)
        {
        T_LbsCalculationCheck cc =iCalculationChecks[i];    
        
        TInt err=cc.CheckDistance();
        LogError(err==KErrNone, cc, err, index);
            
        err=cc.CheckDistanceCoordinate();
        LogError(err==KErrNone, cc, err, index);

        err=cc.CheckDistanceDelta();
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
    err=c.CheckDistance();
    LogError(err==KErrNone, c, err);

    err=c.CheckDistanceCoordinate();
    LogError(err==KErrNone, c, err);

    err=c.CheckDistanceDelta();
    LogError(err==KErrNone, c, err);

    }

void CT_LbsCalculationTP233::LogTestError(const T_LbsCalculationCheck& aCalc, 
                                     const TInt aError,
                                     const TInt aRow)
    {
    TBuf<KMaxLogLineLength> buf;
    TPosition source;
    TPosition target;
    aCalc.GetSourcePosition(source);
    aCalc.GetTargetPosition(target);

    if (aError==KErrCalculationCheckDistance)
        {
        _LIT(KDistanceError, "Calculated distance is not equal to read distance, values:");
        _LIT(KValues, "Lat1= %f, Long1= %f, Lat2= %f, Long2= %f, calculated distance= %f, read distance= %f, row= %d"); 
            
		buf.Format(KValues, source.Latitude(), 
                            source.Longitude(), 
                            target.Latitude(), 
                            target.Longitude(),
                            aCalc.Calculated(),
                            aCalc.Distance(),
                            aRow);
         
        ERR_PRINTF1(KDistanceError);
        }
    else if(aError==KErrCalculationCheckDistanceDelta)
        {
        _LIT(KDeltaError, "Calculated delta is not equal to read delta:");
        _LIT(KValues, "Lat1= %f, Long1= %f, Lat2= %f, Long2= %f, calculated delta= %f, read delta= %f, row= %d"); 
        
        buf.Format(KValues, source.Latitude(), 
                            source.Longitude(), 
                            target.Latitude(), 
                            target.Longitude(),
                            aCalc.Calculated(),
                            aCalc.Delta(),
                            aRow);
        ERR_PRINTF1(KDeltaError);
        }
    else if(aError==KErrCalculationCheckErrorCode)
        {
        _LIT(KIncorrectErrorCode, "Incorrect error code returned from calculation lib:");
        _LIT(KCodes, "Returned = %d, Expected = %d, row= &d");
        
        buf.Format(KCodes, aCalc.ReturnedErrorCode(), aCalc.ErrorCode(), aRow);
        ERR_PRINTF1(KIncorrectErrorCode);
        }
    ERR_PRINTF1(buf);
    }

//  End of File
