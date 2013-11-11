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

#ifndef MESSAGEIDENTIFIER_H
#define MESSAGEIDENTIFIER_H

#include "identifierbase.h"
#include "nodemessagetypes.h"
#include "aliasidentifier.h"

namespace Parser
{

class CSignatureIdentifier;

class CMessageIdentifier : public CIdentifierBase
    {
    public:
        explicit CMessageIdentifier(const char* aName)
            : CIdentifierBase(Parser::EMessageIdentifier, aName), iSignature(NULL)
            {
            }

        virtual void Describe(const unsigned char* aData, unsigned int aLength, const void* aOptions, std::ostream& aDescription) const;

    public:
        const CSignatureIdentifier* iSignature;
        TMessageId iMessageId;
    };
    
typedef CAliasIdentifier<CMessageIdentifier> CMessageAliasIdentifier;

} // namespace Parser

#endif
// MESSAGEIDENTIFIER_H

