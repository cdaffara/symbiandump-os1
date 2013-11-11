// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "te_sysutilsgetversionnonewlines.h"

/**
@SYMTestCaseID			SYSLIB-BAFL-UT-4034
@SYMREQ					REQ8170
@SYMTestCaseDesc		GetSWVersionNoNewLines
@SYMTestPriority		HIGH
@SYMTestStatus			Implemented
@SYMTestActions			Use the SysUtil::GetSWVersion method to get the 
@SYMTestActions			software version from C:\versions\sw.txt. The text file 
@SYMTestActions			should contain no newlines (\n).
@SYMTestExpectedResults There is no error returned the contents of the file is printed out.
*/

/**
Destructor
 */
CSysUtilsGetSWVersionNoNewLinesStep::~CSysUtilsGetSWVersionNoNewLinesStep()
	{
	}

/**
Constructor
 */
CSysUtilsGetSWVersionNoNewLinesStep::CSysUtilsGetSWVersionNoNewLinesStep()
	{
	SetTestStepName(KSysUtilsGetSWVersionNoNewLinesStep);
	}

TVerdict CSysUtilsGetSWVersionNoNewLinesStep::doTestStepL()
	{
	TInt err = SetTestPath(ETrue);
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("Could not turn test path on. Error = %d"), err);
		SetTestStepResult(EAbort);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("Test path turned on."));
	
	err = DeletePSProperties();
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("Could not delete P&S properties. Error = %d"), err);
		SetTestStepResult(EAbort);
		}
	INFO_PRINTF1(_L("Deleted P&S properties successfully."));
	
	_LIT16(KDummy,"xxxxx");

	TBuf16<KSysUtilVersionTextLength> version;
	version.Insert(0,KDummy);

	err = SysUtil::GetSWVersion( version );
	
	if ( err == KErrNone)
		{
		_LIT(KSw, "SW");
		TPtrC16 line;
		TESTL( GetStringFromConfig(ConfigSection(), KSw, line) );
		TBuf16<KSysUtilVersionTextLength> testBuf(line);
		if (version.Compare(testBuf)==0)
			{
			INFO_PRINTF1(_L("Got version "));
			INFO_PRINTF1(version);
			}
		else
			{
			ERR_PRINTF1(_L("Version not correct"));
			SetTestStepResult(EFail);
			}
		}
	else 
		{
		ERR_PRINTF2(_L("Error code = %d"), err);
		SetTestStepResult(EFail);
		}
	
		
	return TestStepResult();
	}



/**
@SYMTestCaseID			SYSLIB-BAFL-UT-4035
@SYMREQ					REQ1802
@SYMTestCaseDesc		GetLangSWVersionNoNewLines
@SYMTestPriority		HIGH
@SYMTestStatus			Implemented
@SYMTestActions			Use the SysUtil::GetLangSWVersion method to get the 
@SYMTestActions			language software version from C:\versions\langsw.txt. 
@SYMTestActions			The text file should contain no newlines (\n).
@SYMTestExpectedResults There is no error returned the contents of the file is printed out.
*/

/**
Destructor
 */
CSysUtilsGetLangSWVersionNoNewLinesStep::~CSysUtilsGetLangSWVersionNoNewLinesStep()
	{
	}

/**
Constructor
 */
CSysUtilsGetLangSWVersionNoNewLinesStep::CSysUtilsGetLangSWVersionNoNewLinesStep()
	{
	SetTestStepName(KSysUtilsGetLangSWVersionNoNewLinesStep);
	}

TVerdict CSysUtilsGetLangSWVersionNoNewLinesStep::doTestStepL()
	{
	TInt err = SetTestPath(ETrue);
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("Could not turn test path on. Error = %d"), err);
		SetTestStepResult(EAbort);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("Test path turned on."));
	
	err = DeletePSProperties();
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("Could not delete P&S properties. Error = %d"), err);
		SetTestStepResult(EAbort);
		return TestStepResult();
		}
	INFO_PRINTF1(_L("Deleted P&S properties successfully."));

	_LIT16(KDummy,"xxxxx");

	TBuf16<KSysUtilVersionTextLength> version;
	version.Insert(0,KDummy);

	err = SysUtil::GetLangSWVersion( version );
	
	if ( err == KErrNone)
		{
		_LIT(KLangSw, "LangSW");
		TPtrC16 line;
		TESTL( GetStringFromConfig(ConfigSection(), KLangSw, line) );
		TBuf16<KSysUtilVersionTextLength> testBuf(line);
		if (version.Compare(testBuf)==0)
			{
			INFO_PRINTF1(_L("Got version "));
			INFO_PRINTF1(version);
			}
		else
			{
			ERR_PRINTF1(_L("Version not correct"));
			INFO_PRINTF1(version);
			SetTestStepResult(EFail);
			}

		}
	else 
		{
		ERR_PRINTF2(_L("Error code = %d"), err);
		SetTestStepResult(EFail);
		}

	return TestStepResult();
	}


/**
@SYMTestCaseID          SYSLIB-BAFL-UT-4037
@SYMDEF		          DEF141094
@SYMTestCaseDesc        GetPRInformationNoNewLines
@SYMTestPriority        HIGH
@SYMTestStatus          Implemented
@SYMTestActions         Use the SysUtil::GetPRInformation method to get the 
@SYMTestActions         pr information from C:\versions\purpose.txt. The text file 
@SYMTestActions         should contain no newlines (\n).
@SYMTestExpectedResults Returns KErrNone and Prints the file contents.
*/

/**
Destructor
 */
CSysUtilsGetPRInformationNoNewLinesStep::~CSysUtilsGetPRInformationNoNewLinesStep()
    {
    }

/**
Constructor
 */
CSysUtilsGetPRInformationNoNewLinesStep::CSysUtilsGetPRInformationNoNewLinesStep()
    {
    SetTestStepName(KSysUtilsGetPRInformationnNoNewLinesStep);
    }

TVerdict CSysUtilsGetPRInformationNoNewLinesStep::doTestStepL()
    {
    TInt err = SetTestPath(ETrue);
    if( err != KErrNone )
        {
        ERR_PRINTF2(_L("Could not turn test path on. Error = %d"), err);
        SetTestStepResult(EAbort);
        return TestStepResult();
        }
    INFO_PRINTF1(_L("Test path turned on."));
    
    err = DeletePSProperties();
    if( err != KErrNone )
        {
        ERR_PRINTF2(_L("Could not delete P&S properties. Error = %d"), err);
        SetTestStepResult(EAbort);
        }
    INFO_PRINTF1(_L("Deleted P&S properties successfully."));
    
    _LIT16(KDummy,"xxxxx");

    TBuf16<KSysUtilVersionTextLength> version;
    version.Insert(0,KDummy);

    err = SysUtil::GetPRInformation( version );
    
    if ( err == KErrNone)
        {
        _LIT(KPRInfo, "PRInfo");
        TPtrC16 line;
        TESTL( GetStringFromConfig(ConfigSection(), KPRInfo, line) );
        TBuf16<KSysUtilVersionTextLength> testBuf(line);
        if (version.Compare(testBuf)==0)
            {
            INFO_PRINTF1(_L("Got Product Release Info "));
            INFO_PRINTF1(version);
            }
        else
            {
            ERR_PRINTF1(_L("Product Release Info not correct"));
            SetTestStepResult(EFail);
            }
        }
    else 
        {
        ERR_PRINTF2(_L("Error code = %d"), err);
        SetTestStepResult(EFail);
        }
    
        
    return TestStepResult();
    }
