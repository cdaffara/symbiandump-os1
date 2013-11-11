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
// mecunittestrefcountowner.h
//

/**
@file
*/

#ifndef MECUNITTESTREFCOUNTOWNER_H
#define MECUNITTESTREFCOUNTOWNER_H

#include <e32base.h>

class CMecUnitTestRefCountOwnerBase : public CBase
    {
public:
    void Close();
    void Open();

protected:
    CMecUnitTestRefCountOwnerBase(const TAny* aPtr);
    ~CMecUnitTestRefCountOwnerBase();

    virtual void DeletePtr() = 0;

protected:
    TAny*       iPtr;

private:
    TInt        iRefCount;
    };

typedef CMecUnitTestRefCountOwnerBase CRefCountOwnerBase;

#endif
// MECUNITTESTREFCOUNTOWNER_H

