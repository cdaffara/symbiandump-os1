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
// Get Language Software Version
// 
//

/**
 @file 
 @internalTechnology
*/

#include "te_sysutilsgetlangswversion.h"

/**
Destructor
 */
CSysUtilsGetLangSWVersionStep::~CSysUtilsGetLangSWVersionStep()
	{
    SetTestPath(EFalse);
	}

/**
Constructor
 */
CSysUtilsGetLangSWVersionStep::CSysUtilsGetLangSWVersionStep()
	{
	SetTestStepName(KSysUtilsGetLangSWVersionStep);
	}

/**
@SYMTestCaseID			SYSLIB-BAFL-CT-4002
@SYMREQ					REQ1802
@SYMTestCaseDesc		GetLangSWVersion
@SYMTestPriority		HIGH
@SYMTestStatus			Implemented
@SYMTestActions			Get the software language version from Z:\resource\versions\langsw.txt 
@SYMTestActions			using the SysUtil::GetLangSWVersion method and compare 
@SYMTestActions			it against a string literal set in an ini file. The ini 
@SYMTestActions			file is located in z:\testdata\inifiles\.
@SYMTestExpectedResults The language software version information obtained is a 
@SYMTestExpectedResults correctly formatted version of the information in 
@SYMTestExpectedResults Z:\resource\versions\langsw.txt.
*/
TVerdict CSysUtilsGetLangSWVersionStep::doTestStepL()
	{
	TInt err=KErrNone;

	_LIT16(KDummy,"xxxxx");
	
	TBuf16<KSysUtilVersionTextLength> version;

	version.Insert(0,KDummy);

	TInt fileErr = CheckIsFileExist( _L("C:\\versions\\langsw.txt"));

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
		
		err = SysUtil::GetLangSWVersion( version );
		if ( err != KErrNone )
			{
			ERR_PRINTF2(_L("Get lang SW version failed. Error = %d"), err);
			SetTestStepResult(EFail);
			}
		else
			{
			// Get the information from the ini file
			_LIT(KLangSW, "LangSW");
			TPtrC16 line;
			
			TESTL( GetStringFromConfig(ConfigSection(), KLangSW, line) );
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
				ERR_PRINTF1(_L("software language formatted incorrectly"));
				ERR_PRINTF1(_L("From ini file: "));
				ERR_PRINTF1(testBuf);
				ERR_PRINTF1(_L("From txt file: "));
				ERR_PRINTF1(version);
				ERR_PRINTF2(_L("Comparison returned value %d"), version.Compare(testBuf));
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
		ERR_PRINTF1(_L("Langsw.txt is missing, should be included to base packets"));
		SetTestStepResult(EFail);
		}
		
	SetTestPath(EFalse);
	return TestStepResult(); 	
	}

