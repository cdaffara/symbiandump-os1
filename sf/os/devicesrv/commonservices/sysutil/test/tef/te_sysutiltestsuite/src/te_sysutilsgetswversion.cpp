// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Get the software version
// 
//

/**
 @file 
 @internalTechnology
*/

#include "te_sysutilsgetswversion.h"

/**
Destructor
 */
CSysUtilsGetSWVersionStep::~CSysUtilsGetSWVersionStep()
	{
    SetTestPath(EFalse);
	}

/**
Constructor
 */
CSysUtilsGetSWVersionStep::CSysUtilsGetSWVersionStep()
	{
	SetTestStepName(KSysUtilsGetSWVersionStep);
	}

/**
@SYMTestCaseID			SYSLIB-BAFL-CT-4001
@SYMREQ					REQ1802
@SYMTestCaseDesc		GetSWVersion
@SYMTestPriority		HIGH
@SYMTestStatus			Implemented
@SYMTestActions			Get the software version from Z:\resource\versions\sw.txt 
@SYMTestActions			using the SysUtil::GetSWVersion method and compare it 
@SYMTestActions			file against a string literal set in an ini file. The 
@SYMTestActions			ini is located in z:\testdata\inifiles\.
@SYMTestExpectedResults The software version information obtained is a correctly 
@SYMTestExpectedResults formatted version of the information in 
@SYMTestExpectedResults Z:\resource\versions\sw.txt.
*/

TVerdict CSysUtilsGetSWVersionStep::doTestStepL()
	{
	TInt err=KErrNone;
	
	_LIT16(KDummy,"xxxxx");

	TBuf16<KSysUtilVersionTextLength> version;
	version.Insert(0,KDummy);
	
	TInt fileErr = CheckIsFileExist( _L("C:\\versions\\sw.txt"));

	if(fileErr == KErrNone)
		{
		err = DeletePSProperties();
		if(err != KErrNone)
			{
			ERR_PRINTF2(_L("Could not delete P&S properties. Error = %d"), err);
			SetTestStepResult(EAbort);
			return TestStepResult();
			}
		INFO_PRINTF1(_L("Deleted P&S properties successfully."));
		
		err = SetTestPath(ETrue);
		if( err != KErrNone )
			{
			ERR_PRINTF2(_L("Could not turn test path off. Error = %d"), err);
			SetTestStepResult(EAbort);
			return TestStepResult();
			}
		INFO_PRINTF1(_L("Test path turned off."));
		
		err = SysUtil::GetSWVersion( version );
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("Get SW version failed. Error = %d"), err);
			SetTestStepResult(EFail);
			}
		else
			{
			//Get the information from the ini file
			_LIT(KSw, "SW");
			TPtrC16 line;

			TESTL( GetStringFromConfig(ConfigSection(), KSw, line) );
			TBuf16<KSysUtilVersionTextLength> testBuf(line);
		    //Parse the information from the ini file
		    // Replace new-line patterns with real ones
			_LIT(KNewLinePattern, "\\n");
			_LIT( KNewline, "\n" );
			
			err = testBuf.Find(KNewLinePattern);
	        while (err != KErrNotFound)
	            {
	            // err is a position
	            testBuf.Replace(err, KNewLinePattern().Length(), KNewline );
	            err = testBuf.Find(KNewLinePattern);
	            }
	        
	        if (version.Compare(testBuf)!=0)
				{
				ERR_PRINTF1(_L("software version formatted incorrectly"));
				INFO_PRINTF1(_L("From ini file: "));
                INFO_PRINTF1(testBuf);
                INFO_PRINTF1(_L("From txt file: "));
                INFO_PRINTF1(version);
                INFO_PRINTF2(_L("Comparison returned value %d"), version.Compare(testBuf));
				SetTestStepResult(EFail);
				}
			else
				{
				INFO_PRINTF1(_L("software version formatted correctly"));
				INFO_PRINTF1(_L("from ini file "));
				INFO_PRINTF1(testBuf);
				INFO_PRINTF1(_L("from txt file "));
				INFO_PRINTF1(version);
				}
			}
		}
	else
		{
		ERR_PRINTF1(_L("sw.txt is missing, should be included to base packets"));
		SetTestStepResult(EFail);
		}
	
	SetTestPath(EFalse);
	return TestStepResult();
	}

