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

#include "messagedefparser\messageprocessor.h"
#include "messagedefparser\initialstate.h"
#include "messagedefparser\definitiontokenizer.h"
#include "messagedefparser\signatureidentifier.h"
#include "util.h"

namespace Parser
{

void CMessageProcessor::ProcessState()
    {
    Tokens::TTokenType tokenType = CurrentTokenType();
    Parser::TResult result = Parser::EUnexpectedToken;
    const CIdentifierBase* ident = NULL;

    switch (iInternalState)
        {
        case EStateExpectMessageIdentifier:
            if (tokenType == Tokens::EIdentifier)
                {
                if (!ParserSM().FindIdentifier(CurrentToken()))
                    {
                    iIdentifier = new CMessageIdentifier(CurrentToken());
                    iInternalState = EStateExpectSignature;
                    result = Parser::ENoError;
                    }
                else
                    {
                    result = Parser::EDuplicateIdentifier;
                    }
                }
            break;

        case EStateExpectSignature:
            if (tokenType == Tokens::ESignature)
                {
                result = Parser::ENoError;
                iInternalState = EStateExpectSignatureEquals;
                }
            break;

        case EStateExpectSignatureEquals:
            if (tokenType == Tokens::EEquals)
                {
                result = Parser::ENoError;
                iInternalState = EStateExpectSignatureIdentifier;
                }
            break;

        case EStateExpectSignatureIdentifier:
            iInternalState = EStateExpectMessageId;
            if (tokenType == Tokens::EIdentifier)
                {
                ident = ParserSM().FindIdentifier(CurrentToken());
                if (!ident)
                    {
                    result = Parser::EUnknownIdentifier;
                    }
                else if (ident->Type() == Parser::ESignatureIdentifier)
                    {
                    iIdentifier->iSignature = static_cast<const CSignatureIdentifier*>(ident);
                    result = Parser::ENoError;
                    }
                else
                    {
                    result = Parser::EInvalidType;
                    }
                }
            break;

        case EStateExpectMessageId:
            if (tokenType == Tokens::EMessageId)
                {
                result = Parser::ENoError;
                iInternalState = EStateExpectMessageIdEquals;
                }
            break;

        case EStateExpectMessageIdEquals:
            if (tokenType == Tokens::EEquals)
                {
                result = Parser::ENoError;
                iInternalState = EStateExpectMessageRealmValue;
                }
            break;

        case EStateExpectMessageRealmValue:
            result = ResolveNumericToken(tokenType, CurrentToken(), 32, iIdentifier->iMessageId.iRealmId);
            if (result == Parser::ENoError)
                {
                iInternalState = EStateExpectMessageColon;
                }
            break;

        case EStateExpectMessageColon:
            if (tokenType == Tokens::EColon)
                {
                result = Parser::ENoError;
                iInternalState = EStateExpectMessageIdValue;
                }
            break;

        case EStateExpectMessageIdValue:
            result = ResolveNumericToken(tokenType, CurrentToken(), 16, iIdentifier->iMessageId.iMessageId);
            if (result == Parser::ENoError)
                {
                iInternalState = EStateExpectEnd;
                }
            break;

        case EStateExpectEnd:
            if (tokenType == Tokens::EEnd)
                {
                result = Parser::ENoError;
                iInternalState = EStateExpectEndMessage;
                }
            break;

        case EStateExpectEndMessage:
            if (tokenType == Tokens::EMessage)
                {
                result = Parser::ENoError;
                ParserSM().AddIdentifier(iIdentifier);
                iIdentifier = NULL;
                }
            ParserSM().SetState(new CInitialState(ParserSM()));
            break;
        }

    if (result != Parser::ENoError)
        {
        ParserSM().SetError(result);
        }
    }

} // namespace Parser

