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
// testexensions.h
//

/**
@file
*/

#ifndef TESTEXTENSIONS_H
#define TESTEXTENSIONS_H

#include <elements/metadata.h>

const TUint KExtVersion1 = 1;
const TUint KExtVersion2 = 2;
const TUint KExtVersion3 = 3;
const TUint KExtVersion4 = 3;

class TTestExtension1 : public Meta::SMetaData
    {
public:
    // Not an allocated UID, but its contained within the test code so doesn't matter
    enum { EUid = 0x1FFFFFF, ETypeId = 1 };
    
    TTestExtension1()
        : iVersion(KExtVersion1)
        {
        }

    TTestExtension1(TUint aVersion)
        : iVersion(aVersion)
        {
        }

    DATA_VTABLE
    
    TUint iVersion;
    };


class TTestExtension2 : public Meta::SMetaData
    {
public:
    // Not an allocated UID, but its contained within the test code so doesn't matter
    enum { EUid = 0x1FFFFFF, ETypeId = 2 };
    
    TTestExtension2()
        : iVersion(KExtVersion1)
        {
        }

    TTestExtension2(TUint aVersion)
        : iVersion(aVersion)
        {
        }
    
    DATA_VTABLE

    TUint iVersion;
    };


class TTestExtension3 : public Meta::SMetaData
    {
public:
    // Not an allocated UID, but its contained within the test code so doesn't matter
    enum { EUid = 0x1FFFFFF, ETypeId = 3 };
    
    TTestExtension3()
        : iVersion(KExtVersion1)
        {
        }

    TTestExtension3(TUint aVersion)
        : iVersion(aVersion)
        {
        }
    
    DATA_VTABLE

    TUint iVersion;
    };

#endif
// TESTEXTENSIONS

