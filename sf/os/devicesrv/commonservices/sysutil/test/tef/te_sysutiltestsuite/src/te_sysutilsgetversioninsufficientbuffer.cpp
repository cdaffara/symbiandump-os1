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

#include "te_sysutilsgetversioninsufficientbuffer.h"

/**
Destructor
 */
CSysUtilsGetVersionInsufficienBufferStep::~CSysUtilsGetVersionInsufficienBufferStep()

	{
	}

/**
Constructor
 */
CSysUtilsGetVersionInsufficienBufferStep::CSysUtilsGetVersionInsufficienBufferStep()

	{
	SetTestStepName(KSysUtilsGetVersionInsufficienBufferStep);
	}


/**
@SYMTestCaseID		SYSLIB-BAFL-CT-4022 
@SYMREQ  	    	REQ8170
@SYMDEF			DEF141094
@SYMTestCaseDesc    GetVersionInsufficientBuffer
@SYMTestPriority    HIGH
@SYMTestStatus      Implemented
@SYMTestActions    	Use  the SysUtil::GetSWVersion method with its descriptor 
@SYMTestActions    	parameter’s size smaller than the length of the software version text 
@SYMTestActions    	Repeat for SysUtil::GetLangSWVersion 
@SYMTestExpectedResults KErrTooBig is returned because not all the software version text can 
@SYMTestExpectedResults	be read into the buffer supplied. 
*/
TVerdict CSysUtilsGetVersionInsufficienBufferStep::doTestStepL()
	{
	TInt err=KErrNone;
	const TInt bufSize = 6;
	
	_LIT16(kDummy,"xxxxx");

	TBuf16<bufSize> version;
	version.Insert(0,kDummy);
	
	err = SysUtil::GetSWVersion( version );
	ProcessError(err);
	err = SysUtil::GetLangSWVersion( version );
	ProcessError(err);
	err = SysUtil::GetPRInformation( version );
	ProcessError(err);
	
	return TestStepResult();
	}

void CSysUtilsGetVersionInsufficienBufferStep::ProcessError(TInt aErr)
	{
	if ( aErr == KErrTooBig )
		{
		INFO_PRINTF1(_L("KErrorToBig this is expected."));	  	
		} 
	else
		{
		ERR_PRINTF2(_L("other error code %d"), aErr);
		SetTestStepResult(EFail);
		}
	}


