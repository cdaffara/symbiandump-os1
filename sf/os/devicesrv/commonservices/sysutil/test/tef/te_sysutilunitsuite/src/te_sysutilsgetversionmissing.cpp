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

#include "te_sysutilsgetversionmissing.h"


/**
@SYMTestCaseID			SYSLIB-BAFL-UT-4025
@SYMREQ					REQ8170
@SYMTestCaseDesc		GetSWVersionMissing
@SYMTestPriority		HIGH
@SYMTestStatus			Implemented
@SYMTestActions			Run the SysUtil::GetSWVersion with the path pointing to 
@SYMTestActions			a non existent file in C:\versions\.
@SYMTestExpectedResults KErrNotFound is returned.
*/

/**
Destructor
 */
CSysUtilsGetSWVersionMissingStep::~CSysUtilsGetSWVersionMissingStep()
	{
	}

/**
Constructor
 */
CSysUtilsGetSWVersionMissingStep::CSysUtilsGetSWVersionMissingStep()
	{
	SetTestStepName(KSysUtilsGetSWVersionMissingStep);
	}

TVerdict CSysUtilsGetSWVersionMissingStep::doTestStepL()
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

	err = SysUtil::GetSWVersion( version );
	
	if ( err == KErrNotFound )
		{
		INFO_PRINTF1(_L("KErrNotFound this is expected"));
		}
	else 
		{
		ERR_PRINTF2(_L("Other error code = %d"), err);
		SetTestStepResult(EFail);
		}

	return TestStepResult();
	}

/**
@SYMTestCaseID			SYSLIB-BAFL-UT-4027
@SYMREQ					REQ1802
@SYMTestCaseDesc		GetLangVersionMissing
@SYMTestPriority		HIGH
@SYMTestStatus			Implemented
@SYMTestActions			Run the SysUtil::GetLangVersion with the path pointing 
@SYMTestActions			a non existent file in C:\versions\.
@SYMTestExpectedResults KErrNotFound is returned.
*/

/**
Destructor
 */
CSysUtilsGetLangVersionMissingStep::~CSysUtilsGetLangVersionMissingStep()
	{
	}

/**
Constructor
 */
CSysUtilsGetLangVersionMissingStep::CSysUtilsGetLangVersionMissingStep()
	{
	SetTestStepName(KSysUtilsGetLangVersionMissingStep);
	}

TVerdict CSysUtilsGetLangVersionMissingStep::doTestStepL()
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
	
	if ( err == KErrNotFound )
		{
		INFO_PRINTF1(_L("KErrNotFound this is expected"));
		}
	else 
		{
		ERR_PRINTF2(_L("Other error code = %d"), err);
		SetTestStepResult(EFail);
		}

	return TestStepResult();
	}

/**
@SYMTestCaseID			SYSLIB-BAFL-UT-4029
@SYMREQ					REQ1802
@SYMTestCaseDesc		GetLangSWVersionMissing
@SYMTestPriority		HIGH
@SYMTestStatus			Implemented
@SYMTestActions			Run the SysUtil::GetLangSWVersion with the path 
@SYMTestActions			pointing a non existent file in C:\versions\.
@SYMTestExpectedResults KErrNotFound is returned.
*/

/**
Destructor
 */
CSysUtilsGetLangSWVersionMissingStep::~CSysUtilsGetLangSWVersionMissingStep()
	{
	}

/**
Constructor
 */
CSysUtilsGetLangSWVersionMissingStep::CSysUtilsGetLangSWVersionMissingStep()
	{
	SetTestStepName(KSysUtilsGetLangSWVersionMissingStep);
	}

TVerdict CSysUtilsGetLangSWVersionMissingStep::doTestStepL()
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
	
	if ( err == KErrNotFound )
		{
		INFO_PRINTF1(_L("KErrNotFound this is expected"));
		}
	else 
		{
		ERR_PRINTF2(_L("Other error code = %d"), err);
		SetTestStepResult(EFail);
		}

	return TestStepResult();
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-UT-4036
@SYMDEF		          DEF141094
@SYMTestCaseDesc        GetPRInformationMissing
@SYMTestPriority        HIGH
@SYMTestStatus          Implemented
@SYMTestActions         Run the SysUtil::GetPRInformation with the path pointing to 
@SYMTestActions         a non existent file in C:\versions\.
@SYMTestExpectedResults KErrNotFound is returned.
*/

/**
Destructor
 */
CSysUtilsGetPRInformationMissingStep::~CSysUtilsGetPRInformationMissingStep()
    {
    }

/**
Constructor
 */
CSysUtilsGetPRInformationMissingStep::CSysUtilsGetPRInformationMissingStep()
    {
    SetTestStepName(KSysUtilsGetPRInformationMissingStep);
    }

TVerdict CSysUtilsGetPRInformationMissingStep::doTestStepL()
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

    TBuf16<KSysUtilVersionTextLength> prinfo;
    prinfo.Insert(0,KDummy);

    err = SysUtil::GetPRInformation( prinfo );
    
    if ( err == KErrNotFound )
        {
        INFO_PRINTF1(_L("KErrNotFound this is expected"));
        }
    else 
        {
        ERR_PRINTF2(_L("Other error code = %d"), err);
        SetTestStepResult(EFail);
        }

    return TestStepResult();
    }

