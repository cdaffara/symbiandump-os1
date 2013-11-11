// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "messagedefparser\integeridentifier.h"

namespace Parser
{


bool CIntegerTypeIdentifier::ValueInRange(unsigned int aSize, int aValue)
    {
    return false;
    // TODO: This wont work - update for signed ints
    // return ((((~0L) >> (32 - aSize)) & aValue) == aValue);
    }


bool CIntegerTypeIdentifier::ValueInRange(unsigned int aSize, unsigned int aValue)
    {
    return ((((~0UL) >> (32 - aSize)) & aValue) == aValue);
    }


bool CIntegerTypeIdentifier::ValueInRange(int aValue) const
    {
    return CIntegerTypeIdentifier::ValueInRange(iSize << 3, aValue);
    }


bool CIntegerTypeIdentifier::ValueInRange(unsigned int aValue) const
    {
    return CIntegerTypeIdentifier::ValueInRange(iSize << 3, aValue);
    }


void CIntegerTypeIdentifier::Describe(const unsigned char* aData, unsigned int aLength, const void* aOptions, std::ostream& aOutput) const
    {
#ifdef _DEBUG    
    assert (aLength == iSize);
#endif

    const TIntegerIdentifierOptions* options = reinterpret_cast<const TIntegerIdentifierOptions*>(aOptions);
    
    if (!options || options->iFormatAsHex)
        {
        if (iSize == 1)
            {
            aOutput << "0x" << std::noshowbase << std::setw(2)
                << std::setfill('0') << std::nouppercase << std::hex
                << (0xff & *reinterpret_cast<const unsigned int*>(aData));
            }
        else if (iSize == 2)
            {
            aOutput << "0x" << std::noshowbase << std::setw(4)
              << std::setfill('0') << std::nouppercase << std::hex
              << (0xffff & *reinterpret_cast<const unsigned int*>(aData));
            }
        else if (iSize == 4)
            {
            aOutput << "0x" << std::noshowbase << std::setw(8)
              << std::setfill('0') << std::nouppercase << std::hex
              << *reinterpret_cast<const unsigned int*>(aData);
            }
        }
    else if (iSigned)
        {
        // Signed and format as decimal
        int v;
        if (iSize == 1)
            {
            char n = (char)(*aData);
            v = n;
            }
        else if (iSize == 2)
            {
            short int n = *reinterpret_cast<const short int*>(aData);
            v = n;
            }
        else if (iSize == 4)
            {
            v = *reinterpret_cast<const int*>(aData);
            }
        aOutput << std::noshowbase << std::dec << v;
        }
    else
        {
        // Unsigned and format as decimal
        unsigned int v;
        if (iSize == 1)
            {
            unsigned char n = (unsigned char)(*aData);
            v = n;
            }
        else if (iSize == 2)
            {
            unsigned short int n = *reinterpret_cast<const unsigned short int*>(aData);
            v = n;
            }
        else if (iSize == 4)
            {
            v = *reinterpret_cast<const unsigned int*>(aData);
            }
        aOutput << std::noshowbase << std::dec << v;
        }
    }


} // namespace Parser

