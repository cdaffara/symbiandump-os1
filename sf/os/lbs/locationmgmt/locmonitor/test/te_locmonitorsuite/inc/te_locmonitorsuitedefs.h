/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* This file define all the common values thoughout your test project
*
*/


/**
 @file te_locmonitorsuitedefs.h
*/
#if (!defined TE_LOCMONITORSUITEDEFS_H)
#define TE_LOCMONITORSUITEDEFS_H

_LIT(KTe_locMonitorSuiteString,"TheString");
_LIT(KTe_locMonitorSuiteInt,"TheInt");
_LIT(KTe_locMonitorSuiteBool,"TheBool");

const TUid KTestLocMonUid = {0x102869E4};

// Values that the test DB will set in its 
// responses to GetPosition...
// ...the first time a Get method is called 
const TInt KLatGetPos1(15); // KLatGetPos1
const TInt KLongGetPos1(25); // KLongGetPos1
//...the second and later times.
const TInt KLatGetPos2(16); // KLatGetPos2
const TInt KLongGetPos2(26); // KLongGetPos2


// Values that the test DB will set in its 
// responses to GetLast...
// ...the first time a Get method is called 
const TInt KLatGetLast1(17);
const TInt KLongGetLast1(27);
//...the second and later times.
const TInt KLatGetLast2(18);
const TInt KLongGetLast2(28);


// Define properties keys needed for testing
enum TTestPropertyKeys{EUnitLocMonDbDelay,
					   EUnitLocMonDbFeedback};

// For test step panics
_LIT(KTe_locMonitorSuitePanic,"Te_locMonitorSuite");


enum TTestDbSetUpInstructions
	{
	EDbSetupShortDelay = 0,
	EDbSetupLongDelay,
	EDbSetupFail,
	EDbKillProcess	
	};

class TDbFeedback
{

public:
	
	// For the test DB to report back to the test step what
	// position the Location Monitor asked the DB to store.
	TPosition iPosition;
	
	// For the DB to report back to the test step what Global
	// Cel Id values the Location Monitor has sent to it.
	// Location Monitor.
	TUint	iMcc;
	TUint	iMnc;
	TUint	iLac;
	TUint	iCellId;
	
	// For the test DB to report back to the test step that
	// it has received an instruction to delete the DB file. 
	TBool iDbDeleted;
};


#endif
