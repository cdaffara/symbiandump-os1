// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Base class for calculation tests
// 
//

#include "ctlbscalculationstepbase.h"
#include "ctlbscalculationserver.h"
#include "tlbscalculationcheck.h"

// CONSTANTS
const TInt KMaxLineLength = 512;
const TInt KMaxEmtpyLineLength = 31; // \t == number of columns

TInt ReadLn(RFile& aFile, TDes& aDes)
    {
    aDes = KNullDesC;
    TBuf8<KMaxLineLength> line;

    TInt err = aFile.Read(line);
    if (line.Length() == 0)
        {
        return KErrEof;
        }
    if (err)
        {
        return err;
        }

    // Find line feed
    TInt i = 0;
    TLex8 fileData;
    fileData.Assign (line );
    while (fileData.Peek ( )!= '\n' && !(fileData.Peek()).Eos())
        {
        fileData.Inc();
        if (++i == KMaxLineLength)
            {
            // In case no carriage return or end of string characters are present.
            break;
            }
        }

    // Update file position
    TInt diff = -(line.Length() - i - 1);
    aFile.Seek (ESeekCurrent, diff);

    // Convert to 16-bit
    line.SetLength(i);
    aDes.Copy(line);

    return KErrNone;
    }


CT_LbsCalculationStepBase::~CT_LbsCalculationStepBase()
	{
	delete iScheduler;
    iFileServer.Close();
    iCalculationChecks.Reset();
	}

CT_LbsCalculationStepBase::CT_LbsCalculationStepBase(CT_LbsCalculationServer& aParent)
: iParent(aParent)
	{
	}

void CT_LbsCalculationStepBase::ConstructL()
	{
	iScheduler = new ( ELeave ) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
    User::LeaveIfError(iFileServer.Connect());
	}

/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsCalculationStepBase::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsCalculationStepBase::doTestStepPreambleL()"));
	SetTestStepResult(EPass);

	return TestStepResult();
	}


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsCalculationStepBase::doTestStepPostambleL()
	{
	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsCalculationStepBase::doTestStepPostabmleL()"));

	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}

TInt CT_LbsCalculationStepBase::ReadValues()
    {
    if (iCalculationChecks.Count() == 0 )
        {
        _LIT(KFileTestValues, "c:\\system\\data\\testvaluescalculation.txt");

        RFile file;
    
        TInt err;
        err=file.Open(iFileServer, KFileTestValues,  EFileRead);
        if (err)
            {
            file.Close();
            return err;
            }

        TBuf<KMaxLineLength> line;  
        for (TInt i=0; i<4; i++) //Skip first 4 lines
            {
            ReadLn(file, line);
            }
    
        while (ReadLn(file, line) != KErrEof)
            {
            if (line.Length() > KMaxEmtpyLineLength)
                {     
                T_LbsCalculationCheck c(line);
                err = iCalculationChecks.Append(c);
                if (err != KErrNone)
                    {
                    file.Close();
                    return err;
                    }
                }
            }
        file.Close();
        }
    
    return KErrNone;
    }

void CT_LbsCalculationStepBase::AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode)
    {
    if (!aCondition)
        {
        TBuf<100> buf;
        buf.Format(aErrorMsg, aErrorCode);
        ERR_PRINTF1(aErrorMsg);
        User::Leave(aErrorCode);
        }
    }    

void CT_LbsCalculationStepBase::LogError(const TBool aCondition, 
                                         const T_LbsCalculationCheck& aCalc, 
                                         const TInt aErrorCode,
                                         const TInt aRow)
    {
    if (!aCondition)
        {
        SetTestStepResult(EFail);
        LogTestError(aCalc, aErrorCode, aRow);
        }
    }

void CT_LbsCalculationStepBase::LogErrorAndLeaveL(const TDesC& aErrorMsg)
    {
    ERR_PRINTF1(aErrorMsg);
    User::Leave(KErrGeneral);
    }

//  End of File
