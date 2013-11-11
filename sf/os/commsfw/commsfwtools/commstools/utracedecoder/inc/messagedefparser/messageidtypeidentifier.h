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

#ifndef MESSAGEIDTYPEIDENTIFIER_H
#define MESSAGEIDTYPEIDENTIFIER_H

#include "structidentifier.h"
#include "integeridentifier.h"
#include "nodemessagetypes.h"


namespace Parser
{

class CDefinitionParser;
class CSignatureIdentifier;

class CMessageIdTypeIdentifier : public CStructIdentifier
    {
    public:
        explicit CMessageIdTypeIdentifier(CDefinitionParser& aParser);

        virtual void Describe(const unsigned char* aData, unsigned int aLength, const void* aOptions, std::ostream& aDescription) const;

    private:
        CDefinitionParser& iParser;
    };
    
} // namespace Parser

#endif
// MESSAGEIDTYPEIDENTIFIER_H

