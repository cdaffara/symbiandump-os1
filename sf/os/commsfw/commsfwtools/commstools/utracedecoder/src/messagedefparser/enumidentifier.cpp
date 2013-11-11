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

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "messagedefparser\enumidentifier.h"

namespace Parser
{
void CEnumTypeIdentifier::AddValue(CEnumValueIdentifier* aEnumValue)
    {
    iEnumValues.push_back(aEnumValue);
    }


void CEnumTypeIdentifier::Describe(const unsigned char* aData, unsigned int aLength, const void* aOptions, std::ostream& aOutput) const
    {
    unsigned int i;
    union 
        {
        unsigned int u32;
        signed int i32;
        } value;

    if (iSigned)
        {
        if (iSize == 1)
            {
            char n = (char)(*aData);
            value.i32 = n;
            }
        else if (iSize == 2)
            {
            short int n = *reinterpret_cast<const short int*>(aData);
            value.i32 = n;
            }
        else if (iSize == 4)
            {
            value.i32 = *reinterpret_cast<const int*>(aData);
            }
        }
    else
        {
        unsigned int mask = ~0UL >> (32 - (iSize << 3));
        value.u32 = *reinterpret_cast<const unsigned int*>(aData) & mask;
        }

    for (i=0; i<iEnumValues.size(); i++)
        {
        if (iEnumValues.at(i)->iValue == value.u32)
            {
            iEnumValues.at(i)->Describe(NULL, 0, aOptions, aOutput);
            return;
            }
        }

    CIntegerTypeIdentifier::Describe(aData, aLength, &iFormatOptions, aOutput);    
    }

/*
void CEnumValueIdentifier::Describe(const unsigned char* aData, unsigned int aLength, const void* aOptions, std::ostream& aOutput) const
    {
    (void)aData;
    (void)aLength;
    (void)aOptions;
    aOutput << Name();
    }
*/

}
// namespace Parser

