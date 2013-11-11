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

#include "te_sysutilsgetversionnotext.h"


/**
@SYMTestCaseID			SYSLIB-BAFL-UT-4031
@SYMREQ					REQ8170
@SYMTestCaseDesc		GetSWVersionNoText
@SYMTestPriority		HIGH
@SYMTestStatus			Implemented
@SYMTestActions			Use the SysUtil::GetSWVersion method to get the 
@SYMTestActions			software version from C:\versions\sw.txt. The text file 
@SYMTestActions			should contain no text.
@SYMTestExpectedResults KErrEof is returned.
*/

/**
Destructor
 */
CSysUtilsGetSWVersionNoTextStep::~CSysUtilsGetSWVersionNoTextStep()
	{
	}

/**
Constructor
 */
CSysUtilsGetSWVersionNoTextStep::CSysUtilsGetSWVersionNoTextStep()
	{
	SetTestStepName(KSysUtilsGetSWVersionNoTextStep);
	}

TVerdict CSysUtilsGetSWVersionNoTextStep::doTestStepL()
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
	
	if ( err == KErrEof)
		{
		INFO_PRINTF1(_L("KErrEof expected"));
		}
	else 
		{
		ERR_PRINTF2(_L("Other error code  = %d"), err);
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}
	

/**
@SYMTestCaseID			SYSLIB-BAFL-UT-4032
@SYMREQ					REQ1802
@SYMTestCaseDesc		GetLangVersionNoText
@SYMTestPriority		HIGH
@SYMTestStatus			Implemented
@SYMTestActions			Use the SysUtil::GetLangVersion method to get the 
@SYMTestActions			language version from C:\versions\lang.txt. The text 
@SYMTestActions			file should contain no text.
@SYMTestExpectedResults KErrEof is returned.
*/

/**
Destructor
 */
CSysUtilsGetLangVersionNoTextStep::~CSysUtilsGetLangVersionNoTextStep()
	{
	}

/**
Constructor
 */
CSysUtilsGetLangVersionNoTextStep::CSysUtilsGetLangVersionNoTextStep()
	{
	SetTestStepName(KSysUtilsGetLangVersionNoTextStep);
	}

TVerdict CSysUtilsGetLangVersionNoTextStep::doTestStepL()
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

	err = SysUtil::GetLangVersion( version );
	
	if ( err == KErrEof)
		{
		INFO_PRINTF1(_L("KErrEof expected"));
		}
	else 
		{
		ERR_PRINTF2(_L("Other error code  = %d"), err);
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

/**
@SYMTestCaseID			SYSLIB-BAFL-UT-4033
@SYMREQ					REQ1802
@SYMTestCaseDesc		GetLangSWVersionNoText
@SYMTestPriority		HIGH
@SYMTestStatus			Implemented
@SYMTestActions			Use the SysUtil::GetLangSWVersion method to get the 
@SYMTestActions			language software version from C:\versions\langsw.txt. 
@SYMTestActions			The text file should contain no text.
@SYMTestExpectedResults KErrEof is returned.
*/

/**
Destructor
 */
CSysUtilsGetLangSWVersionNoTextStep::~CSysUtilsGetLangSWVersionNoTextStep()
	{
	}

/**
Constructor
 */
CSysUtilsGetLangSWVersionNoTextStep::CSysUtilsGetLangSWVersionNoTextStep()
	{
	SetTestStepName(KSysUtilsGetLangSWVersionNoTextStep);
	}

TVerdict CSysUtilsGetLangSWVersionNoTextStep::doTestStepL()
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
	
	if ( err == KErrEof)
		{
		INFO_PRINTF1(_L("KErrEof expected"));
		}
	else 
		{
		ERR_PRINTF2(_L("Other error code  = %d"), err);
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}


/**
@SYMTestCaseID          SYSLIB-BAFL-UT-4038
@SYMDEF		          DEF141094
@SYMTestCaseDesc        GetPRInformationNoText
@SYMTestPriority        HIGH
@SYMTestStatus          Implemented
@SYMTestActions         Use the SysUtil::GetPRInformation method to get the 
@SYMTestActions         PR Information from C:\versions\purpose.txt. The text file 
@SYMTestActions         should contain no text.
@SYMTestExpectedResults KErrEof is returned.
*/

/**
Destructor
 */
CSysUtilsGetPRInformationNoTextStep::~CSysUtilsGetPRInformationNoTextStep()
    {
    }

/**
Constructor
 */
CSysUtilsGetPRInformationNoTextStep::CSysUtilsGetPRInformationNoTextStep()
    {
    SetTestStepName(KSysUtilsGetPRInformationNoTextStep);
    }

TVerdict CSysUtilsGetPRInformationNoTextStep::doTestStepL()
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
    
    if ( err == KErrEof)
        {
        INFO_PRINTF1(_L("KErrEof expected"));
        }
    else 
        {
        ERR_PRINTF2(_L("Other error code  = %d"), err);
        SetTestStepResult(EFail);
        }
    return TestStepResult();
    }
    

