
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
//

/**
 @internalTechnology
 @file
*/

#include "te_sysutilscheckdriveletter.h"

/**
Destructor
*/
CSysUtilsCheckDriveLetterFFSStep::~CSysUtilsCheckDriveLetterFFSStep()
	{
	}

/**
Constructor
 */
CSysUtilsCheckDriveLetterFFSStep::CSysUtilsCheckDriveLetterFFSStep()
	{
	SetTestStepName(KSysUtilsCheckDriveLetterFFSStep);
	}

TVerdict CSysUtilsCheckDriveLetterFFSStep::doTestStepL()
	{
	TInt drive_letter = -1;
	
//Get drive number from ini file
#ifdef __WINSCW__
	
	TInt expected_drive_letter;
	TESTL( GetIntFromConfig(ConfigSection(),_L("WINSFFS")  , expected_drive_letter) );	
#else	
	TInt expected_drive_letter;
	TESTL( GetIntFromConfig(ConfigSection(),_L("OTHERFFS")  , expected_drive_letter) );
#endif

	RFs session;	// This is not NULL
	session.Connect();

	drive_letter = SysUtil::GetFFSDriveLetter( session );

	if( expected_drive_letter == drive_letter )
		{
		INFO_PRINTF1(_L("GetFFSDriveLetter passed when checking against known value"));
		SetTestStepResult( EPass );
		}
	else
		{
		INFO_PRINTF1(_L("GetFFSDriveLetter failed when checking against known value"));
		SetTestStepResult( EFail );
		}

	session.Close();
	return TestStepResult();
	}


/**
Destructor
*/
CSysUtilsCheckDriveLetterMMCStep::~CSysUtilsCheckDriveLetterMMCStep()
	{
	}

/**
Constructor
 */
CSysUtilsCheckDriveLetterMMCStep::CSysUtilsCheckDriveLetterMMCStep()
	{
	SetTestStepName(KSysUtilsCheckDriveLetterMMCStep);
	}

TVerdict CSysUtilsCheckDriveLetterMMCStep::doTestStepL()
	{
	TInt drive_letter = -1;
	
//Get drive number from ini file
#ifdef __WINSCW__
	TInt expected_drive_letter;
	TESTL( GetIntFromConfig(ConfigSection(),_L("WINSMMC")  , expected_drive_letter) );	
#else	
	TInt expected_drive_letter;
	TESTL( GetIntFromConfig(ConfigSection(),_L("OTHERMMC")  , expected_drive_letter) );
#endif

	RFs session;	// This is not NULL
	session.Connect();

	drive_letter = SysUtil::GetMMCDriveLetter( session );

	if( expected_drive_letter == drive_letter )
		{
		INFO_PRINTF1(_L("GetMMCDriveLetter passed when checking against known value"));
		SetTestStepResult( EPass );
		}
	else
		{
		INFO_PRINTF2(_L("GetMMCDriveLetter failed when checking against known value %d"), drive_letter);
		SetTestStepResult( EFail );
		}

	session.Close();
	return TestStepResult();
	}

