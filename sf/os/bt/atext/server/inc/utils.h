/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines utility functions
*
*/


#ifndef UTILS_H
#define UTILS_H

#include <e32base.h>

template <class T>
class CleanupResetDestroyClose
    {
public:
    inline static void PushL(T& aRef)
        {
        CleanupStack::PushL(TCleanupItem(&ResetDestroyClose,&aRef));
        }
private:
    static void ResetDestroyClose(TAny *aPtr)
        {
        static_cast<T*>(aPtr)->ResetAndDestroy();
        static_cast<T*>(aPtr)->Close();
        }
    };

/**
 * Pushes an object into CleanupStack and specifies the cleanup
 * function as ResetAndDestroy() and Close().
*/
template <class T>
inline void CleanupResetDestroyClosePushL(T& aRef)
    {CleanupResetDestroyClose<T>::PushL(aRef);}

template <class T>
static TBool OperatorEqual(const T& aT1, const T& aT2)
    {
    return aT1 == aT2;
    }

#endif // UTILS_H
