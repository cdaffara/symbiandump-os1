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

#include "te_sysutilsgetversionunreadable.h"

/**
@SYMTestCaseID			SYSLIB-BAFL-UT-4026
@SYMREQ					REQ8170
@SYMTestCaseDesc		GetSWVersionUnreadable
@SYMTestPriority		HIGH
@SYMTestStatus			Implemented
@SYMTestActions			Use the SysUtil::GetSWVersion method to get the 
@SYMTestActions			software version from C:\versions\sw.txt. The text file 
@SYMTestActions			should be using ANSI encoding.
@SYMTestExpectedResults The test does not panic, but KerrCorrupted is returned 
@SYMTestExpectedResults because the sw.txt file is unreadable.
*/

/**
Destructor
 */
CSysUtilsGetSWVersionUnreadableStep::~CSysUtilsGetSWVersionUnreadableStep()
	{
	}

/**
Constructor
 */
CSysUtilsGetSWVersionUnreadableStep::CSysUtilsGetSWVersionUnreadableStep()
	{
	SetTestStepName(KSysUtilsGetSWVersionUnreadableStep);
	}

TVerdict CSysUtilsGetSWVersionUnreadableStep::doTestStepL()
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
	
	if ( err == KErrCorrupt)
		{
		INFO_PRINTF1(_L("KErrCorrupt returned this is expected"));
		}
	else 
		{
		ERR_PRINTF2(_L("Other error code  = %d"), err);
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}
	

/**
@SYMTestCaseID			SYSLIB-BAFL-UT-4028
@SYMREQ					REQ1802
@SYMTestCaseDesc		GetLangVersionUnreadable
@SYMTestPriority		HIGH
@SYMTestStatus			Implemented
@SYMTestActions			Use the SysUtil::GetLangVersion method to get the 
@SYMTestActions			language software version from C:\versions\lang.txt. 
@SYMTestActions			The text file should be using ANSI encoding.
@SYMTestExpectedResults The test does not panic, but KerrCorrupted is returned 
@SYMTestExpectedResults because the lang.txt file is unreadable.
*/

/**
Destructor
 */
CSysUtilsGetLangVersionUnreadableStep::~CSysUtilsGetLangVersionUnreadableStep()
	{
	}

/**
Constructor
 */
CSysUtilsGetLangVersionUnreadableStep::CSysUtilsGetLangVersionUnreadableStep()
	{
	SetTestStepName(KSysUtilsGetLangVersionUnreadableStep);
	}

TVerdict CSysUtilsGetLangVersionUnreadableStep::doTestStepL()
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
	
	if ( err == KErrCorrupt)
		{
		INFO_PRINTF1(_L("KErrCorrupt returned this is expected"));
		}
	else 
		{
		ERR_PRINTF2(_L("Other error code  = %d"), err);
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

/**
@SYMTestCaseID			SYSLIB-BAFL-UT-4030
@SYMREQ					REQ1802
@SYMTestCaseDesc		GetLangSWVersionUnreadable
@SYMTestPriority		HIGH
@SYMTestStatus			Implemented
@SYMTestActions			Use the SysUtil::GetLangVersion method to get the 
@SYMTestActions			language version from C:\versions\langsw.txt. The text 
@SYMTestActions			file should be using ANSI encoding.
@SYMTestExpectedResults The test does not panic, but KerrCorrupt is returned 
@SYMTestExpectedResults because the langsw.txt file is unreadable.
*/

/**
Destructor
 */
CSysUtilsGetLangSWVersionUnreadableStep::~CSysUtilsGetLangSWVersionUnreadableStep()
	{
	}

/**
Constructor
 */
CSysUtilsGetLangSWVersionUnreadableStep::CSysUtilsGetLangSWVersionUnreadableStep()
	{
	SetTestStepName(KSysUtilsGetLangSWVersionUnreadableStep);
	}

TVerdict CSysUtilsGetLangSWVersionUnreadableStep::doTestStepL()
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
	
	if ( err == KErrCorrupt)
		{
		INFO_PRINTF1(_L("KErrCorrupt returned this is expected"));
		}
	else 
		{
		ERR_PRINTF2(_L("Other error code  = %d"), err);
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}


/**
@SYMTestCaseID          SYSLIB-BAFL-UT-4039
@SYMDEF		          DEF141094
@SYMTestCaseDesc        GetPRInformationUnreadable
@SYMTestPriority        HIGH
@SYMTestStatus          Implemented
@SYMTestActions         Use the SysUtil::GetPRInformation method to get the 
@SYMTestActions         pr information from C:\versions\purpose.txt. The text file 
@SYMTestActions         should be using ANSI encoding.
@SYMTestExpectedResults The test does not panic, but KErrCorrupt is returned 
@SYMTestExpectedResults because the purpose.txt file is unreadable.
*/

/**
Destructor
 */
CSysUtilsGetPRInformationUnreadableStep::~CSysUtilsGetPRInformationUnreadableStep()
    {
    }

/**
Constructor
 */
CSysUtilsGetPRInformationUnreadableStep::CSysUtilsGetPRInformationUnreadableStep()
    {
    SetTestStepName(KSysUtilsGetPRInformationUnreadableStep);
    }

TVerdict CSysUtilsGetPRInformationUnreadableStep::doTestStepL()
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
    
    if ( err == KErrCorrupt)
        {
        INFO_PRINTF1(_L("KErrCorrupt returned this is expected"));
        }
    else 
        {
        ERR_PRINTF2(_L("Other error code  = %d"), err);
        SetTestStepResult(EFail);
        }
    return TestStepResult();
    }
    
