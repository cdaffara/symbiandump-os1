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
#include "ctlbscalculationtp235.h"
#include "tlbscalculationcheck.h"


// C++ constructor 
CT_LbsCalculationTP235::CT_LbsCalculationTP235(CT_LbsCalculationServer& aParent) : CT_LbsCalculationStepBase(aParent)
    {
    _LIT(KTestName, "Calculation TP235 - Calculate Speed");
    SetTestStepName(KTestName);
    }

CT_LbsCalculationTP235::~CT_LbsCalculationTP235()
    {
    }

CT_LbsCalculationTP235* CT_LbsCalculationTP235::New(CT_LbsCalculationServer& aParent )
    {
    CT_LbsCalculationTP235* testStep = new CT_LbsCalculationTP235(aParent);
    // Note the lack of ELeave.
    // This means that having insufficient memory will return NULL;

    if (testStep )
        {
        TInt err = KErrNone;

        TRAP(err, testStep->ConstructL());
        if (err )
            {
            delete testStep;
            testStep = NULL;
            }
        }
    return testStep;
    }

TVerdict CT_LbsCalculationTP235::doTestStepL()
    {
    TestValuesL();
    return TestStepResult();
    }

void CT_LbsCalculationTP235::TestValuesL()
    {   
    _LIT(KOpenFailed, "File open failed with %d");
    
    TInt err=0; 
    err=ReadValues();
    AssertTrueSecL(err == KErrNone, KOpenFailed, err);

    TInt index=5;
    for (TInt i=0; i < iCalculationChecks.Count(); i++)
        {      
        T_LbsCalculationCheck cc = iCalculationChecks[i];    
        
        err=cc.CheckSpeed();
        LogError(err==KErrNone, cc, err, index);
        
        err=cc.CheckSpeedDelta();
        LogError(err==KErrNone, cc, err, index);
        
        ++index;
        if (index == 21 || index == 26 || index == 33 || index == 38|| index == 40 || index == 45 || 
            index == 50 || index == 73 || index == 78 || index == 85 || index == 92 || index == 96 || 
            index == 33 || index == 106 || index == 123 || index == 128 || index == 133)
            {
            ++index;
            }
        }

    TTime ttime1(0);
    TTime ttime2(2);

    TPosition source;
    TPosition target;

    source.SetCoordinate(33, 33, 10);
    source.SetTime(ttime1);
    target.SetCoordinate(33, 34, 1);
    target.SetTime(ttime2);

    T_LbsCalculationCheck c(source, target);
    err=c.CheckSpeed();
    LogError(err==KErrNone, c, err);

    // Checks that KErrArgumenet is returned
    T_LbsCalculationCheck c2;
    err=c2.CheckSpeed();
    LogError(err==KErrNone, c2, err);

    err=c2.CheckSpeedDelta();
    LogError(err==KErrNone, c2, err);
    
    }

void CT_LbsCalculationTP235::LogTestError(const T_LbsCalculationCheck& aCalc, 
                                     const TInt aError,
                                     const TInt aRow)
    {
    
    TBuf<KMaxLogLineLength> buf;
    TPosition source;
    TPosition target;
    aCalc.GetSourcePosition(source);
    aCalc.GetTargetPosition(target);
    
    if (aError==KErrCalculationCheckSpeed)
        {
        _LIT(KSpeedError, "Calculated speed is not equal to read speed, values:");
        _LIT(KValues, "Lat1= %f, Long1= %f, Lat2= %f, Long2= %f, calculated speed= %f, read speed= %f, row= %d"); 
            
		buf.Format(KValues, source.Latitude(), 
                            source.Longitude(), 
                            target.Latitude(), 
                            target.Longitude(),
                            aCalc.Calculated(),
                            aCalc.Speed(),
                            aRow);
         
        ERR_PRINTF1(KSpeedError);
        }
    else if(aError==KErrCalculationCheckSpeedDelta)
        {
        _LIT(KDeltaError, "Calculated bearing delta is not equal to read bearing delta:");
        _LIT(KValues, "Lat1= %f, Long1= %f, Lat2= %f, Long2= %f, calculated delta= %f, read delta= %f, row= %d"); 
        
        buf.Format(KValues, source.Latitude(), 
                            source.Longitude(), 
                            target.Latitude(), 
                            target.Longitude(),
                            aCalc.Calculated(),
                            aCalc.SpeedDelta(),
                            aRow);
        ERR_PRINTF1(KDeltaError);


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
                           aCalc.SpeedErrorCode(), 
                           aRow);

        ERR_PRINTF1(KIncorrectErrorCode);
        }
        
    ERR_PRINTF1(buf);
    }

//  End of File
