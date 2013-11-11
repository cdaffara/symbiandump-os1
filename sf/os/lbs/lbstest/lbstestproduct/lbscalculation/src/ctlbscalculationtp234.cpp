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
#include "ctlbscalculationtp234.h"
#include "tlbscalculationcheck.h"


// C++ constructor 
CT_LbsCalculationTP234::CT_LbsCalculationTP234(CT_LbsCalculationServer& aParent) : CT_LbsCalculationStepBase(aParent)
    {
    _LIT(KTestName, "Calculation TP234 - Calculate Bearing");
    SetTestStepName(KTestName);
    }

CT_LbsCalculationTP234::~CT_LbsCalculationTP234()
    {
    }

CT_LbsCalculationTP234* CT_LbsCalculationTP234::New(CT_LbsCalculationServer& aParent)
    {
    CT_LbsCalculationTP234* testStep = new CT_LbsCalculationTP234(aParent);
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


TVerdict CT_LbsCalculationTP234::doTestStepL()
    {
    TestValuesL();
    return TestStepResult();
    }

void CT_LbsCalculationTP234::TestValuesL()
    {   
    _LIT(KOpenFailed, "File open failed with %d");
    
    TInt err=0; 
    err=ReadValues();
    AssertTrueSecL(err == KErrNone, KOpenFailed, err);

    TInt index=5;
    for (TInt i=0; i< iCalculationChecks.Count(); i++)
        {  
        T_LbsCalculationCheck cc = iCalculationChecks[i];    
        
        TInt err=cc.CheckBearing();
        LogError(err==KErrNone, cc, err, index);
        
        err=cc.CheckBearingCoordinate();
        LogError(err==KErrNone, cc, err, index);
       
        err=cc.CheckBearingDelta();
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
    err=c.CheckBearing();
    LogError(err==KErrNone, c, err);

    err=c.CheckBearingCoordinate();
    LogError(err==KErrNone, c, err);

    err=c.CheckBearingDelta();
    LogError(err==KErrNone, c, err);
   
    }

void CT_LbsCalculationTP234::LogTestError(const T_LbsCalculationCheck& aCalc, 
                                     const TInt aError,
                                     const TInt aRow)
    {
    
    TBuf<KMaxLogLineLength> buf;
    TPosition source;
    TPosition target;
    aCalc.GetSourcePosition(source);
    aCalc.GetTargetPosition(target);

    if (aError==KErrCalculationCheckBearing)
        {
        _LIT(KDistanceError, "Calculated bearing is not equal to read bearing, values:");
        _LIT(KValues, "Lat1= %f, Long1= %f, Lat2= %f, Long2= %f, calculated bearing= %f, read bearing= %f, row= %d"); 
            
		buf.Format(KValues, source.Latitude(), 
                            source.Longitude(), 
                            target.Latitude(), 
                            target.Longitude(),
                            aCalc.Calculated(),
                            aCalc.Bearing(),
                            aRow);
         
        ERR_PRINTF1(KDistanceError);
        }
     else if(aError==KErrCalculationCheckBearingDelta)
        {
        _LIT(KDeltaError, "Calculated bearing delta is not equal to read bearing delta:");
        _LIT(KValues, "Lat1= %f, Long1= %f, Lat2= %f, Long2= %f, calculated delta= %f, read delta= %f, row= %d"); 
        
        buf.Format(KValues, source.Latitude(), 
                            source.Longitude(), 
                            target.Latitude(), 
                            target.Longitude(),
                            aCalc.Calculated(),
                            aCalc.BearingDelta(),
                            aRow);
        ERR_PRINTF1(KDeltaError);


        }
     else if(aError==KErrCalculationCheckErrorCode)
        {
        _LIT(KIncorrectErrorCode, "Incorrect error code returned from Bearing calculation lib:");
        _LIT(KCodes, "Lat1= %f, Long1= %f, Lat2= %f, Long2= %f, Returned = %d, Expected = %d, row= %d");
        
        buf.Format(KCodes, source.Latitude(), 
                           source.Longitude(), 
                           target.Latitude(), 
                           target.Longitude(), 
                           aCalc.ReturnedErrorCode(), 
                           aCalc.BearingErrorCode(), aRow);

        ERR_PRINTF1(KIncorrectErrorCode);
        
        }
    ERR_PRINTF1(buf);
    }

//  End of File
