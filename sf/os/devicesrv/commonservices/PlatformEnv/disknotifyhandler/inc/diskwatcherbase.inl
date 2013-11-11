/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Inline methods of base class for disk watchers.
*
*/



inline RFs& CDiskWatcherBase::Fs()
    {
    return iFs;
    }

inline MDiskNotifyHandlerCallback& CDiskWatcherBase::Callback()
    {
    return iCallback;
    }

template < class T >
inline TInt CDiskWatcherBase::Compare( const T& aItem1, const T& aItem2 )
    {
    if ( aItem1 < aItem2 )
        {
        return -1; 
        }
    if ( aItem1 > aItem2 )
        {
        return 1; 
        }
    return 0;
    }


// End of File
