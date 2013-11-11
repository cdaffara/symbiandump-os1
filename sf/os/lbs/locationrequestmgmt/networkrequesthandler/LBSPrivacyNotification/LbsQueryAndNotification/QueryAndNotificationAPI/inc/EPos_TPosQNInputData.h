/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
 @released
*/

#ifndef TPOSQNINPUTDATA_H
#define TPOSQNINPUTDATA_H

//  INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS

// CLASS DECLARATION

const TInt KPosQNInputDataClassSize = 36;
typedef TInt32 TPosQNRequestId;

/**
* Data sent in Query/Notification communication.
*/
class TPosQNInputData
    {
    public:
        enum TPosRequestType { EQuery, ENotification, ECancel, ECancelAll };
        TPosRequestType iType;
        TPosQNRequestId iId;
        TInt iDataSize;
        TAny* iDataPtr;
        TPosRequestSource iRequestSource;
        TPosRequestDecision iTimeoutStrategy;
        TPosRequestDecision iRequestDecision;
        TPosNotificationReason iNotificationReason;
        TPosVerifyCancelReason iCancelReason;
    };

#endif      // TPOSQNINPUTDATA_H

// End of File
