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
#ifndef FBSOOGMMESSAGE_INL
#define FBSOOGMMESSAGE_INL

const TInt KFbsOogmBytesToFreeNone = 0;
const TInt KFbsOogmFlagsClear = 0;


inline TFbsOogmMessage::TFbsOogmMessage()
: iOogmNotification( EFbsOogmNoAction ), iBytesToFree( KFbsOogmBytesToFreeNone ), iFlags( KFbsOogmFlagsClear )
    {
    }


inline TFbsOogmMessage::TFbsOogmMessage( TOogmAction aOogmAction, TInt aBytesToFree, TInt aFlags )
: iOogmNotification( aOogmAction ), iBytesToFree( aBytesToFree), iFlags( aFlags )
    {
    }

#endif // FBSOOGMMESSAGE_INL
