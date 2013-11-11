/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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



#ifndef MEDIATORTESTDATA_H
#define MEDIATORTESTDATA_H

#include <e32cmn.h>

struct TTestItemList
    {
    TUid iDomain;  // domain of the list
    TUid iCategory; // category of the list
    TInt iFirst; // id of the first item in the list 
    TInt iLast;
    };
    
const TTestItemList TestItemLists[] =
    { 
         { 0xEF00000, 0xEF00000, 50, 80 },
         { 0xEF00001, 0xEF00001, 81, 100 },
         { 0xEF00001, 0xEF00002, 10, 600 },
         { 0xEF00001, 0xEF00003, 1, 9 }
    };

const TInt KMaxCountOfLists = 10;

struct TTestItemListSet
    {
    TInt iCount;
    TTestItemList iTestLists[KMaxCountOfLists];
    };

/**
 *  Communication channel between tests and mediator test plugin.
 */
const TUid KMediatorStifTestPsKeys = { 0x010099ff };
/**
 * Index of the current test list in use. KErrNotSupported if list is not used.
 */
const TUint32 KCurrentTestList = 0x01; 

/**
 *  Result of verification. KErrNone if OK, otherwise a error code.
 */
const TUint32 KCurrentTestListVerificationVerdict = 0x2;

/**
 * Response data from notification listener. 
 */
typedef TPckgBuf<TTestItemListSet> TTestItemListSetPckgBuf;

const TUint32 KReceivedTestLists = 0x03;

#endif      // MEDIATORTESTDATA_H

// End of File
