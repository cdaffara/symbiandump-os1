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

#include <cassert>
#include "messagedefparser\enumprocessor.h"
#include "messagedefparser\initialstate.h"
#include "messagedefparser\definitiontokenizer.h"
#include "util.h"

namespace Parser
{

void CEnumProcessor::ProcessState()
    {
    Tokens::TTokenType tokenType = CurrentTokenType();
    Parser::TResult result = Parser::EUnexpectedToken;
    const CIntegerTypeIdentifier* intType;

    switch (iInternalState)
        {
        case EStateExpectEnumIdentifierOrFormat:
            switch (tokenType)
                {
                case Tokens::EIdentifier:
                    if (!ParserSM().FindIdentifier(CurrentToken()))
                        {
                        iIdentifier = new CEnumTypeIdentifier(CurrentToken());
                        iInternalState = EStateExpectColon;
                        result = Parser::ENoError;
                        }
                    else
                        {
                        result = Parser::EDuplicateIdentifier;
                        }
                    break;

                case Tokens::EDisplayDec:
                case Tokens::EDisplayHex:
                    iFormatOptions.iFormatAsHex = (tokenType == Tokens::EDisplayHex);
                    iInternalState = EStateExpectEnumIdentifier;
                    result = Parser::ENoError;
                    break;
                }

        case EStateExpectEnumIdentifier:
            if (tokenType == Tokens::EIdentifier)
                {
                if (!ParserSM().FindIdentifier(CurrentToken()))
                    {
                    iIdentifier = new CEnumTypeIdentifier(CurrentToken());
                    iInternalState = EStateExpectColon;
                    result = Parser::ENoError;
                    }
                else
                    {
                    result = Parser::EDuplicateIdentifier;
                    }
                }
            break;

        case EStateExpectColon:
            if (tokenType == Tokens::EColon)
                {
                iInternalState = EStateExpectBaseIntType;
                result = Parser::ENoError;
                }
            break;

        case EStateExpectBaseIntType:
            result = Parser::ENoError;
            iInternalState = EStateExpectEnumValueIdentifierOrEnd;
            if (tokenType == Tokens::EIntType)
                {
                intType = static_cast<const CIntegerTypeIdentifier*>(ParserSM().FindIdentifier(CurrentToken()));
                assert(intType && intType->Type() == Parser::EIntegerTypeIdentifier);
                iIdentifier->SetSize(intType->Size());
                iIdentifier->SetSigned(intType->Signed());
                }
            else
                {
                result = Parser::EInvalidType;
                }
            break;
        
        case EStateExpectEnumValueIdentifierOrEnd:
            if (tokenType == Tokens::EIdentifier)
                {
                if (!ParserSM().FindIdentifier(CurrentToken()))
                    {
                    iTempEnumValue = new CEnumValueIdentifier(CurrentToken());
                    result = Parser::ENoError;
                    iInternalState = EStateExpectEquals;
                    }
                else
                    {
                    result = Parser::EDuplicateIdentifier;
                    }
                }
            else if (tokenType == Tokens::EEnd && iIdentifier->iEnumValues.size() > 0)
                {
                iInternalState = EStateExpectEndEnum;
                result = Parser::ENoError;
                }
            break;

        case EStateExpectEquals:
            if (tokenType == Tokens::EEquals)
                {
                result = Parser::ENoError;
                iInternalState = EStateExpectEnumValue;
                }
            break;

        case EStateExpectEnumValue:
            iInternalState = EStateExpectEnumValueIdentifierOrEnd;
            if (tokenType == Tokens::ENumberDec || tokenType == Tokens::ENumberHex)
                {
                if (tokenType == Tokens::ENumberDec)
                    {
                    iTempEnumValue->iValue = atol(CurrentToken());
                    }
                else 
                    {
                    iTempEnumValue->iValue = HexToVal(CurrentToken());
                    }

                if (iIdentifier->ValueInRange(iTempEnumValue->iValue))
                    {
                    ParserSM().AddIdentifier(iTempEnumValue);
                    iIdentifier->AddValue(iTempEnumValue);
                    iTempEnumValue = NULL;
                    result = Parser::ENoError;
                    }
                else
                    {
                    result = Parser::EValueOutOfRange;
                    }
                }
            break;

        case EStateExpectEndEnum:
            if (tokenType == Tokens::EEnum)
                {
                result = Parser::ENoError;
                iIdentifier->iFormatOptions = iFormatOptions;
                ParserSM().AddIdentifier(iIdentifier);
                iIdentifier = NULL;
                ParserSM().SetState(new CInitialState(ParserSM()));
                }
            else
                {
                ParserSM().SetState(new CInitialState(ParserSM()));
                }
            break;
        }

    if (result != Parser::ENoError)
        {
        ParserSM().SetError(result);
        }
    }

} // namespace Parser

