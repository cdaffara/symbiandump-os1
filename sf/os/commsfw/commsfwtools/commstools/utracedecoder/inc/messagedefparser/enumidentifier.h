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

#ifndef ENUMIDENTIFIER_H
#define ENUMIDENTIFIER_H

#include <vector>

#include "integeridentifier.h"
#include "constidentifier.h"

namespace Parser
{

//class CEnumValueIdentifier;
typedef CConstIdentifier CEnumValueIdentifier;

class CEnumTypeIdentifier : public CIntegerTypeIdentifier
    {
    public:
        explicit CEnumTypeIdentifier(const char* aName)
            : CIntegerTypeIdentifier(Parser::EEnumTypeIdentifier, aName)
            {
            iSize = 4;
            }

    public:
        void AddValue(CEnumValueIdentifier* aEnumValue);
        CIdentifierBase* FindValue(const char* aName);

        inline void SetSize(int aSize)
            {
            iSize = aSize;
            }

        virtual void Describe(const unsigned char* aData, unsigned int aLength, const void* aOptions, std::ostream& aDescription) const;

    public:
        std::vector<CEnumValueIdentifier*> iEnumValues;
        TIntegerIdentifierOptions iFormatOptions;
    };


/*
class CEnumValueIdentifier : public CIdentifierBase
    {
    public:
        explicit CEnumValueIdentifier(const char* aName, const CEnumTypeIdentifier& aEnumType)
            : CIdentifierBase(Parser::EEnumValueIdentifier, aName), iEnumType(aEnumType), iValue(0)
            {
            }

        virtual void Describe(const unsigned char* aData, unsigned int aLength, const void* aOptions, std::ostream& aDescription) const;

    public:
        const CEnumTypeIdentifier& iEnumType;
        unsigned int iValue;
    };
*/

} // namespace Parser

#endif
// ENUMIDENTIFIER_H

