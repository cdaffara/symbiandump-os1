 
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
// Examine the space left on the given disk
// 
//

/**
 @file 
 @internalTechnology
*/

#include "te_sysutilscheckrfsconnection.h"


/**
Destructor
*/
CSysUtilsCheckRFsConnectionDSStep::~CSysUtilsCheckRFsConnectionDSStep()
	{
	}

/**
Constructor
 */
CSysUtilsCheckRFsConnectionDSStep::CSysUtilsCheckRFsConnectionDSStep()
	{
	SetTestStepName(KSysUtilsCheckRFsConnectionDSStep);
	}

TVerdict CSysUtilsCheckRFsConnectionDSStep::doTestStepL()
	{
	TInt err=KErrNone;

	RFs session;	// This is not NULL
	session.Connect();
	
	// Test the case when the RFs session is connected. 
	// Expected result: err = KErrNone
	TRAP(err, SysUtil::DiskSpaceBelowCriticalLevelL( &session, 0, EDriveZ ) );

	if( err == KErrNone )
		{
		INFO_PRINTF1(_L("DiskSpaceBelowCriticalLevel passed when passing a connected RFs pointer"));
		}
	
	session.Close();
	
	// Test the case when the RFs session is NOT connected
	// Expected result: PANIC KEN-EXEC 0
	// There is no way of logging the result of this TRAP as the case will panic.
	TRAP(err, SysUtil::DiskSpaceBelowCriticalLevelL( &session, 0, EDriveZ ) );

	return EFail ;
	}


/**
Destructor
*/
CSysUtilsCheckRFsConnectionFFSStep::~CSysUtilsCheckRFsConnectionFFSStep()
	{
	}

/**
Constructor
 */
CSysUtilsCheckRFsConnectionFFSStep::CSysUtilsCheckRFsConnectionFFSStep()
	{
	SetTestStepName(KSysUtilsCheckRFsConnectionFFSStep);
	}

TVerdict CSysUtilsCheckRFsConnectionFFSStep::doTestStepL()
	{
	TInt err=KErrNone;

	RFs session;	// This is not NULL
	session.Connect();
	
	// Test the case when the RFs session is connected. 
	// Expected result: err = KErrNone
	TRAP(err, SysUtil::FFSSpaceBelowCriticalLevelL( &session, 0 ) );

	if( err == KErrNone )
		{
		INFO_PRINTF1(_L("FFSSpaceBelowCriticalLevelL passed when passing a connected RFs pointer"));
		}
	
	session.Close();
	
	// Test the case when the RFs session is NOT connected
	// Expected result: PANIC KEN-EXEC 0
	// There is no way of logging the result of this TRAP as the case will panic.
	TRAP(err, SysUtil::FFSSpaceBelowCriticalLevelL( &session, 0 ) );

	return EFail ;
	}


/**
Destructor
*/
CSysUtilsCheckRFsConnectionMMCStep::~CSysUtilsCheckRFsConnectionMMCStep()
	{
	}

/**
Constructor
 */
CSysUtilsCheckRFsConnectionMMCStep::CSysUtilsCheckRFsConnectionMMCStep()
	{
	SetTestStepName(KSysUtilsCheckRFsConnectionMMCStep);
	}

TVerdict CSysUtilsCheckRFsConnectionMMCStep::doTestStepL()
	{
	TInt err=KErrNone;

	RFs session;	// This is not NULL
	session.Connect();
	
	// Test the case when the RFs session is connected. 
	// Expected result: err = KErrNone
	TRAP(err, SysUtil::MMCSpaceBelowCriticalLevelL( &session, 0 ) );

	if( err == KErrNone )
		{
		INFO_PRINTF1(_L("MMCSpaceBelowCriticalLevelL passed when passing a connected RFs pointer"));
		}
	
	session.Close();
	
	// Test the case when the RFs session is NOT connected
	// Expected result: PANIC KEN-EXEC 0
	// There is no way of logging the result of this TRAP as the case will panic.
	TRAP(err, SysUtil::MMCSpaceBelowCriticalLevelL( &session, 0 ) );

	return EFail ;
	}

