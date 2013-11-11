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

#include <e32uid.h>
#include "te_sysutilsdummyapis.h"

const TInt KSysUtilDllUid3 = 0x10005943;

/**
 Destructor
 */
CSysUtilsDummyAPIsStep::~CSysUtilsDummyAPIsStep()

	{
	}

/**
 Constructor
 */
CSysUtilsDummyAPIsStep::CSysUtilsDummyAPIsStep()

	{
	SetTestStepName (KSysUtilsDummyAPIsStep);
	}

/**
@SYMTestCaseID		SYSLIB-BAFL-CT-4023    
@SYMREQ  	    	REQ8170, REQ8172		
@SYMTestCaseDesc    DummyAPIs
@SYMTestPriority    HIGH
@SYMTestStatus      Implemented
@SYMTestActions    	An attempt should be made to call each remove method as if it was still 
@SYMTestActions    	present in the System Utils dummy apis 3, 7, 8, 9, 11 – 20, 25 - 40 for winsc 
@SYMTestActions    	and 1 – 8, 12, 16 – 20, 25 - 40 for eabi.					 
@SYMTestExpectedResults Trace should be logged explaining that unsupported method calls have 
@SYMTestExpectedResults	been made, but no errors should occur. Trace is only logged if 
@SYMTestExpectedResults	_DEBUG is  defined
*/
TVerdict CSysUtilsDummyAPIsStep::doTestStepL()
	{

	TUidType uidType(KDynamicLibraryUid, KSharedLibraryUid,
			TUid::Uid (KSysUtilDllUid3));

	RLibrary library;
	TInt err = library.Load (_L("sysutil.dll"),uidType);
	if ( err == KErrNone)
		{
		__ASSERT_ALWAYS(err==KErrNone,User::Panic(_L("Failed to load sysutil.dll, error: %d."),err));
#ifdef __WINSCW__

		TLibraryFunction ordinalfunction = library.Lookup (3);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (7);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (8);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (9);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (11);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (12);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (13);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (14);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (15);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (16);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (17);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (18);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (19);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (20);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();
		
		ordinalfunction = library.Lookup (15);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (16);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (17);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (18);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (19);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (20);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();
		
		ordinalfunction = library.Lookup (25);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (26);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (27);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (28);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (29);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (30);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();
		
		ordinalfunction = library.Lookup (31);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (32);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (33);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (34);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (35);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (36);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();
		
		ordinalfunction = library.Lookup (37);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (38);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (39);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (40);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

#else 
		//eabi
		TLibraryFunction ordinalfunction = library.Lookup(1);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction)();

		ordinalfunction = library.Lookup(2);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction)();

		ordinalfunction = library.Lookup(3);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction)();

		ordinalfunction = library.Lookup(4);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction)();

		ordinalfunction = library.Lookup(5);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction)();

		ordinalfunction = library.Lookup(6);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction)();

		ordinalfunction = library.Lookup(7);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction)();

		ordinalfunction = library.Lookup(8);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction)();

		ordinalfunction = library.Lookup(12);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction)();

		ordinalfunction = library.Lookup(16);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction)();

		ordinalfunction = library.Lookup(17);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction)();

		ordinalfunction = library.Lookup(18);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction)();

		ordinalfunction = library.Lookup(19);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction)();

		ordinalfunction = library.Lookup(20);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction)();
		
		ordinalfunction = library.Lookup (25);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (26);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (27);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (28);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (29);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (30);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();
		
		ordinalfunction = library.Lookup (31);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (32);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (33);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (34);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (35);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (36);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();
		
		ordinalfunction = library.Lookup (37);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (38);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (39);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();

		ordinalfunction = library.Lookup (40);
		__ASSERT_ALWAYS(ordinalfunction!=NULL,User::Panic(_L("Failed to find ordinal in sysutil.dll."),KErrGeneral));
		(*ordinalfunction) ();
#endif

		}
	else
		{
		ERR_PRINTF1 (_L("DummyAPI failed"));
		SetTestStepResult (EFail);
		}

	return TestStepResult ();
	}

