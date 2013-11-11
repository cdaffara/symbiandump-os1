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

#ifndef INTEGERIDENTIFIER_H
#define INTEGERIDENTIFIER_H

#include "identifierbase.h"

namespace Parser
{
struct TIntegerIdentifierOptions
    {
    public:
        TIntegerIdentifierOptions()
            : iFormatAsHex(true)
            {
            }

        TIntegerIdentifierOptions(bool aFormatAsHex)
            : iFormatAsHex(aFormatAsHex)
            {
            }

        bool iFormatAsHex;
    };

class CIntegerTypeIdentifier : public CIdentifierBase
    {
    public:
        explicit CIntegerTypeIdentifier(const char* aName, int aSize)
            : CIdentifierBase(Parser::EIntegerTypeIdentifier, aName), iSigned(false)
            {
            iSize = aSize;
            }

        explicit CIntegerTypeIdentifier(const char* aName, int aSize, bool aSigned)
            : CIdentifierBase(Parser::EIntegerTypeIdentifier, aName), iSigned(aSigned)
            {
            iSize = aSize;
            }

        virtual void Describe(const unsigned char* aData, unsigned int aLength, const void* aOptions, std::ostream& aDescription) const;

    protected:
        explicit CIntegerTypeIdentifier(Parser::TIdentifierType aType, const char* aName)
            : CIdentifierBase(aType, aName), iSigned(false)
            {
            }

        explicit CIntegerTypeIdentifier(Parser::TIdentifierType aType, const char* aName, bool aSigned)
            : CIdentifierBase(aType, aName), iSigned(aSigned)
            {
            }

    public:
        bool ValueInRange(int aValue) const;
        bool ValueInRange(unsigned int aValue) const;
        static bool ValueInRange(unsigned int aSize, int aValue);
        static bool ValueInRange(unsigned int aSize, unsigned int aValue);

        inline void SetSigned(bool aSigned)
            {
            iSigned = aSigned;
            }

        inline bool Signed() const
            {
            return iSigned;
            }

    protected:
        bool iSigned;
    };
    
} // namespace Parser

#endif
// INTEGERIDENTIFIER_H

