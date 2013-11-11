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
*
*/




/**
 @file
 @publishedPartner
 @released
*/
#ifndef MMGSMWCDMAUTILS_H
#define MMGSMWCDMAUTILS_H

//  INCLUDES
#include <etelmm.h>

/** Voice Call related structure definitions */

/*  SCallGsmReAddress */     
struct SCallGsmReAddress
    {
    TUint8 iAddressType;
    TUint8 iPresentationIndicator;
    //check length
    HBufC16* iAddress;
    };

/* SCallGsmCugInfo */
struct SCallGsmCugInfo
    {
    TUint8 iPreferentialCug;
    TUint8 iCugOutgoingAccess;
    TUint16 iCugIndex;
    };
/* SCallOriginInfo */
struct SCallOriginInfo
    {
    TUint8 iPresentationIndicator;
    TUint8 iNumberSource;
    TUint8 iNumberType;
    TUint8 iNumberGroup;
    HBufC16* iName;
    };

/* SCallGsmNoInd */
struct SCallGsmNoInd
    {
    TUint8 iTransactionId;
    TUint8 iValidData;
    TUint8 iNotificationInd;
    TUint16 iMmiSsCode;
    TUint8 iSsStatus;
    TUint8 iSsNotification;
    TUint8 iSsIndicator;
    TUint8 iSsHoldIndicator;
    TUint8 iSsEctIndicator;
    SCallGsmReAddress* iCallGsmRemoteAddress;
    SCallGsmCugInfo* iCallGsmCugInfo;
    SCallGsmReAddress* iCallGsmAddressOfB;
    SCallOriginInfo* iCallOriginInfo;
    TUint8 iAlertPattern;
    };


/** SIM access and capabilities related structure definitions */

/* SSSTFileData */
struct SSSTFileData
    {
    TUint8 iTransactionId;
    TBuf8<11> iSSTData;
    };

/* SStoreInfo */
struct SStoreInfo
    {
    TUint8 iTransactionId;
    TUint8 iTotalEntries;
    TUint8 iUsedEntries;
    };

#endif // MMGSMWCDMAUTILS_H

// End of File
