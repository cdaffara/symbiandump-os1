// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// mecunittestrefcountowner.cpp
//

/**
@file
*/

#include "mecunittestrefcountowner.h"

CMecUnitTestRefCountOwnerBase::CMecUnitTestRefCountOwnerBase(const TAny* aPtr)
    : iPtr(const_cast<TAny*>(aPtr)), iRefCount(0)
    {
    }

CMecUnitTestRefCountOwnerBase::~CMecUnitTestRefCountOwnerBase()
    {
    }

void CMecUnitTestRefCountOwnerBase::Close()
    {
    // LockedDec() returns the value prior to decrement
    if (User::LockedDec(iRefCount) == 1)
        {
        DeletePtr();
        delete this;
        }
    }

void CMecUnitTestRefCountOwnerBase::Open()
    {
    // LockedInc() returns the value prior to increment
    User::LockedInc(iRefCount);
    }

