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

#ifndef SIGNATUREIDENTIFIER_H
#define SIGNATUREIDENTIFIER_H

#include <vector>

#include "identifierbase.h"
#include "nodemessagetypes.h"
#include "aliasidentifier.h"

namespace Parser
{

class CSignatureIdentifier : public CIdentifierBase
    {
    public:
        explicit CSignatureIdentifier(const char* aName)
            : CIdentifierBase(Parser::ESignatureIdentifier, aName), iBaseSignature(NULL)
            {
            iSize = 0;
            }

        void SetBaseSignature(const CSignatureIdentifier* aBaseSignature);
        void AddMember(TMember* aMember);
        const TMember* FindMember(const std::string& aName);

        bool IsNodeSignalDerived() const;
        void MessageId(TMessageId& aMessageId) const;

        inline STypeId TypeId() const
            {
            return iHeader.iTypeId;
            }

        inline STypeId& TypeId()
            {
            return iHeader.iTypeId;
            }

        inline const CSignatureIdentifier* BaseSignature() const
            {
            return iBaseSignature;
            }

        virtual void Describe(const unsigned char* aData, unsigned int aLength, const void* aOptions, std::ostream& aDescription) const;
        virtual int TotalSize() const;

    public:
        std::vector<TMember*> iMembers;

    private:
        SMessageHeader iHeader;
        const CSignatureIdentifier* iBaseSignature;
    };

typedef CAliasIdentifier<CSignatureIdentifier> CSignatureAliasIdentifier;

} // namespace Parser

#endif
// SIGNATUREIDENTIFIER_H

