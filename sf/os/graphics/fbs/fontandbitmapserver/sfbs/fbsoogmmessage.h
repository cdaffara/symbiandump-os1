// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Message class defining the action desired in response to a notification from
// FBserv's Out-of-Graphics-Memory plug-in.
//
#ifndef FBSOOGMMESSAGE_H
#define FBSOOGMMESSAGE_H


class TFbsOogmMessage
    {
public:

    enum TOogmAction
        {
        EFbsOogmNoAction = 0,
        EFbsOogmLowNotification,
        EFbsOogmOkayNotification
        };

public:
    inline TFbsOogmMessage();
    inline TFbsOogmMessage( TOogmAction aOogmAction, TInt aBytesToFree, TInt aFlags );

public:
    TOogmAction iOogmNotification;
    TInt iBytesToFree;
    TInt iFlags;
    };


#include <graphics/fbsoogmmessage.inl>
#endif // FBSOOGMMESSAGE_H
