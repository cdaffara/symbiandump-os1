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

#ifndef PADIDENTIFIER_H
#define PADIDENTIFIER_H

#include "identifierbase.h"

namespace Parser
{

class CPadTypeIdentifier : public CIdentifierBase
    {
    public:
        explicit CPadTypeIdentifier(const char* aName)
            : CIdentifierBase(Parser::EPadTypeIdentifier, aName)
            {
            iSize = 1;
            }

        virtual void Describe(const unsigned char* aData, unsigned int aLength, const void* aOptions, std::ostream& aDescription) const;
    };
    
} // namespace Parser

#endif
// PADIDENTIFIER_H

