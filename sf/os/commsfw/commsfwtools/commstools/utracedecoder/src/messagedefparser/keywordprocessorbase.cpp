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

#include "messagedefparser\keywordprocessorbase.h"
#include "messagedefparser\definitiontokenizer.h"
#include "messagedefparser\identifierbase.h"
#include "messagedefparser\constidentifier.h"
#include "messagedefparser\enumidentifier.h"
#include "messagedefparser\integeridentifier.h"
#include "util.h"

namespace Parser
{

Parser::TResult CKeywordProcessorBase::ResolveNumericToken(Tokens::TTokenType aType, const char* aToken, int aMaxSize, unsigned short int& aValue)
    {
    unsigned int v;
    Parser::TResult result = ResolveNumericToken(aType, aToken, aMaxSize, v);
    if (result == Parser::ENoError)
        {
        aValue = (unsigned short int)v;
        }

    return result;
    }

Parser::TResult CKeywordProcessorBase::ResolveNumericToken(Tokens::TTokenType aType, const char* aToken, int aMaxSize, unsigned int& aValue)
    {
    TResult result = Parser::ENoError;
    unsigned int value = 0;
    const CIdentifierBase* ident = NULL;

    switch (aType)
        {
        case Tokens::ENumberDec:
            value = atol(CurrentToken());
            break;

        case Tokens::ENumberHex:
            value = HexToVal(CurrentToken());
            break;

        case Tokens::EIdentifier:
            ident = ParserSM().FindIdentifier(aToken);
            if (!ident)
                {
                result = EUnknownIdentifier;
                }
            else if (ident->Type() == Parser::EConstIdentifier)
                {
                value = static_cast<const CConstIdentifier*>(ident)->iValue;
                }
            //else if (ident->Type() == Parser::EEnumValueIdentifier)
            //    {
            //    value = static_cast<const CEnumValueIdentifier*>(ident)->iValue;
            //    }
            else
                {
                result = Parser::EInvalidType;
                }
            break;

        default:
            result = Parser::EUnexpectedToken;
        }

    if (result == Parser::ENoError)
        {
        if (CIntegerTypeIdentifier::ValueInRange(aMaxSize, value))
            {
            aValue = value;
            }
        else
            {
            result = Parser::EValueOutOfRange;
            }
        }

    return result;
    }

} // namespace Parser

