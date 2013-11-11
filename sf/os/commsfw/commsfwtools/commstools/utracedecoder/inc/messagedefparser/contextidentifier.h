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

#ifndef CONTEXTIDENTIFIER_H
#define CONTEXTIDENTIFIER_H

#include <vector>

#include "identifierbase.h"
#include "aliasidentifier.h"

namespace Parser
{

class CContextIdentifier : public CIdentifierBase
    {
    public:
        explicit CContextIdentifier(const char* aName)
            : CIdentifierBase(Parser::EContextIdentifier, aName), iBaseContext(NULL)
            {
            iSize = 0;
            }

        void SetBaseContext(const CContextIdentifier* aBaseContext);
        void AddMember(TMember* aMember);
        const TMember* FindMember(const std::string& aName);

        inline const CContextIdentifier* BaseContext() const
            {
            return iBaseContext;
            }

        virtual void Describe(const unsigned char* aData, unsigned int aLength, const void* aOptions, std::ostream& aDescription) const;
        virtual int TotalSize() const;

    public:
        std::vector<TMember*> iMembers;

    private:
        const CContextIdentifier* iBaseContext;
    };

typedef CAliasIdentifier<CContextIdentifier> CContextAliasIdentifier;

} // namespace Parser

#endif
// CONTEXTIDENTIFIER_H

