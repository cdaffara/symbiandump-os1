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

#ifndef ALIASIDENTIFIER_H
#define ALIASIDENTIFIER_H

#include "nodemessagetypes.h"

namespace Parser
{
const int KAliasIdentifier = 0x0001;

template<typename BASECLASS>
class CAliasIdentifier : public BASECLASS
    {
    public:
        explicit CAliasIdentifier<BASECLASS>(const char* aName, const BASECLASS* aIdentifier)
            : BASECLASS(aName), iIdentifier(aIdentifier)
            {
            iFlags |= KAliasIdentifier;
            }

        virtual void Describe(const unsigned char* aData, unsigned int aLength, const void* aOptions, std::ostream& aDescription) const;
        virtual int TotalSize() const;

        const char* RealName() const;

    private:
        const BASECLASS* iIdentifier;
    };


template<typename BASECLASS>
void CAliasIdentifier<BASECLASS>::Describe(const unsigned char *aData, unsigned int aLength, const void *aOptions, std::ostream &aDescription) const
    {
    iIdentifier->Describe(aData, aLength, aOptions, aDescription);
    }


template<typename BASECLASS>
int CAliasIdentifier<BASECLASS>::TotalSize() const
    {
    return iIdentifier->TotalSize();
    }

template<typename BASECLASS>
const char* CAliasIdentifier<BASECLASS>::RealName() const
    {
    return iIdentifier->Name();
    }

} // namespace Parser

#endif
// ALIASIDENTIFIER_H

