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

#ifndef STRUCTIDENTIFIER_H
#define STRUCTIDENTIFIER_H

#include <vector>

#include "identifierbase.h"

namespace Parser
{

class CStructIdentifier : public CIdentifierBase
    {
    public:
        explicit CStructIdentifier(const char* aName)
            : CIdentifierBase(Parser::EStructIdentifier, aName)
            {
            }

        void AddMember(TMember* aMember);
        const TMember* FindMember(const std::string& aName);

        virtual void Describe(const unsigned char* aData, unsigned int aLength, const void* aOptions, std::ostream& aDescription) const;

    protected:
        explicit CStructIdentifier(TIdentifierType aType, const char* aName)
            : CIdentifierBase(aType, aName)
            {
            }

    public:
        std::vector<TMember*> iMembers;
    };
    
} // namespace Parser

#endif
// STRUCTIDENTIFIER_H

