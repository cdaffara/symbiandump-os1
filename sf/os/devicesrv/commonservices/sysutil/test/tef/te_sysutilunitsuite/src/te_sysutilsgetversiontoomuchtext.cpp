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

#include "te_sysutilsgetversiontoomuchtext.h"

/**
Destructor
 */
CSysUtilsGetVersionTooMuchTextStep::~CSysUtilsGetVersionTooMuchTextStep()
	{
	}

/**
Constructor
 */
CSysUtilsGetVersionTooMuchTextStep::CSysUtilsGetVersionTooMuchTextStep()

	{
	SetTestStepName(KSysUtilsGetVersionTooMuchTextStep);
	}

/**
@SYMTestCaseID			SYSLIB-BAFL-UT-4024
@SYMREQ					REQ8170
@SYMTestCaseDesc		GetVersionTooMuchText
@SYMTestPriority		HIGH
@SYMTestStatus			Implemented
@SYMTestActions			Use the SysUtil::GetSWVersion method, with a buffer of 
@SYMTestActions			sixty six characters, to get the version information 
@SYMTestActions			from the relevant text file in C:\versions\. The file 
@SYMTestActions			should contain sixty six characters.
@SYMTestActions			Repeat if correct error is returned for 
@SYMTestActions			SysUtil::GetLangSWVersion and SysUtil::GetLangVersion.
@SYMTestExpectedResults KErrTooBig is returned and the buffer is truncated 
@SYMTestExpectedResults because the software version text is limited to sixty 
@SYMTestExpectedResults four characters.
*/
TVerdict CSysUtilsGetVersionTooMuchTextStep::doTestStepL()
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
	
	const TInt KBufSize = 66;
	
	_LIT16(KDummy,"xxxxx");

	TBuf16<KBufSize> version;
	version.Insert(0,KDummy);

	err = SysUtil::GetSWVersion( version );
	ProcessError(err);
	err = SysUtil::GetLangSWVersion( version );
	ProcessError(err);
	err = SysUtil::GetLangVersion( version );
	ProcessError(err);
	err = SysUtil::GetPRInformation( version );
	ProcessError(err);
	    
	return TestStepResult();
	}

//Process the errorcode returned.
void CSysUtilsGetVersionTooMuchTextStep::ProcessError(TInt aErr)
	{
	if ( aErr == KErrTooBig )
		{
		INFO_PRINTF1(_L("KErrorToBig this is exepected."));
		} 
	else
		{
		ERR_PRINTF2(_L("other error code %d"), aErr);
		SetTestStepResult(EFail);
		}
	}


