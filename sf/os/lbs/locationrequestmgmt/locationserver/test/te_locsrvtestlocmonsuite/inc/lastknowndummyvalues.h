/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file includes all the dummy data used by the test server to
* complete client requests. The file should be included by both the
* test server and the unit test code to ensure that the API implementation
* works.
*
*/


#ifndef __TESTLASTKNOWNDUMMYVALUES_H__
#define __TESTLASTKNOWNDUMMYVALUES_H__

/**
 * @file 
 * @internalTechnology
 * @test
 * 
 */

// NotifyPositionUpdate data
#define DUMMY_NOTIFY_POS_UPDATE_LATITUDE1               TReal64(57.1)
#define DUMMY_NOTIFY_POS_UPDATE_LONGITUDE1              TReal64(11.3)
#define DUMMY_NOTIFY_POS_UPDATE_ALTITUDE1               TReal32(32.5)
#define DUMMY_NOTIFY_POS_UPDATE_HORIZONTAL_ACCURACY1    TReal32(40.0)
#define DUMMY_NOTIFY_POS_UPDATE_VERTICAL_ACCURACY1      TReal32(40.0)

#define DUMMY_LAST_KNOWN_LATITUDE1                      DUMMY_NOTIFY_POS_UPDATE_LATITUDE1
#define DUMMY_LAST_KNOWN_LONGITUDE1                     DUMMY_NOTIFY_POS_UPDATE_LONGITUDE1
#define DUMMY_LAST_KNOWN_ALTITUDE1                      DUMMY_NOTIFY_POS_UPDATE_ALTITUDE1
#define DUMMY_LAST_KNOWN_HORIZONTAL_ACCURACY1           DUMMY_NOTIFY_POS_UPDATE_HORIZONTAL_ACCURACY1
#define DUMMY_LAST_KNOWN_VERTICAL_ACCURACY1             DUMMY_NOTIFY_POS_UPDATE_VERTICAL_ACCURACY1

#endif // __TESTDUMMYVALUES_H__
