/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/
#ifndef LBSTESTLOCMONITORAPIDEFS_H_
#define LBSTESTLOCMONITORAPIDEFS_H_

#include <e32cmn.h>

// Values used to vary the behaviour of the Test Location Monitor Interface
// for different unit test cases in the Location Server
/*
 * Key values for test control properties
 */
const TUint KLbsLocMonitorTestLKPAreaKey 		= 0x10000000;
const TUint KLbsLocMonitorTestLKPosKey 			= 0x20000000;
const TUint KLbsLocMonitorTestSessionKey 		= 0x30000000; // For EmptyLastKnownPositionStore requests
const TUint KLbsLocMonitorTestSubSessionKey 	= 0x40000000;

// Dummy Test Component UIDs
const TInt KLocSrvTestSuiteUid	= 0x102869E6;
const TUid KLocSrvTestSuite	= {KLocSrvTestSuiteUid};


// These values should match the test case id found in te_locsrvsuite.ini
// These values are used to vary the behaviour of the location monitor for
// different test cases.
enum TLocServerTestCase
	{
	// Function calls on the Session handle with Location Server  
	EClearDBPendingCloseSubSession=1,
	EClearDBLocMonitorOpenSessionFail = 2,
	EClearDB2Clients,
	EClearDBAndCancel,
	EClearDBAndCancel2Clients,
	EClearDBReqTimedOut=6,
	EClearDBReq10Clients,

	// Should correspond to te_locsrvsuite.ini
	// Function calls on the SubSession handle with Location Server
	EGetLKPArea = 1,
	EGetLKPAreaCancel =2,
	EGetLKPAreaExtended,
	EGetLKPAreaAndClearDB,
	EGetLKPAreaCancel3Clients,
	EGetLKPAreaAndClearDB3Clients,
	EGetLKPosAreaNotFound = 7,
	EGetLKPAreaExtendedIncorrectArg,
	EGetLKPAreaLocMonitorOpenSubsessionFail = 9,
	EGetLKPAreaPendingCloseSubSession,
	EGetLKPAreaTimedOut=11,
	EGetLKPArea10Clients,
	
	EGetLKPos=21,
	EGetLKPosCancel,
	EGetLKPosCancelAndClearDB,
	EGetLKPosCancel3Clients,
	EGetLKPosAndClearDB3Clients,
	EGetLKPosNotFound = 26,
	EGetLKPosAndLKPArea3Clients,
	EGetLKPosIncorrectArg,
	EGetLKPosLocMonitorOpenSubsessionFail = 29,
	EGetLKPosPendingCloseSubSession,
	EGetLKPosTimedOut=31,
	EGetLKPos10Clients,
	
	EGetLKPosAndLKPAreaAndClearDB3Clients=40,
	};




// Dummy values for the last known position area request
#define DUMMY_LAST_KNOWN_POS_LATITUDE1               TReal64(57.1)
#define DUMMY_LAST_KNOWN_POS_LONGITUDE1              TReal64(11.3)
#define DUMMY_LAST_KNOWN_POS_ALTITUDE1               TReal32(32.5)
#define DUMMY_LAST_KNOWN_POS_HORIZONTAL_ACCURACY1    TReal32(40.0)
#define DUMMY_LAST_KNOWN_POS_VERTICAL_ACCURACY1      TReal32(40.0)


//_LIT(KFailedOpenLocMonSession, "Failed to open default psy");
//_LIT(KFailedOpenLocMonSubSession, "Check failed");
//_LIT(KFailedEqualCheck, "Equal check failed");

#endif /*LBSTESTLOCMONITORAPIDEFS_H_*/
