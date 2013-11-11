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

#include "messagedefparser\contextprocessor.h"
#include "messagedefparser\initialstate.h"
#include "messagedefparser\definitiontokenizer.h"
#include "messagedefparser\integeridentifier.h"
#include "util.h"

namespace Parser
{

void CContextProcessor::ProcessState()
    {
    Tokens::TTokenType tokenType = CurrentTokenType();
    iResult = Parser::EUnexpectedToken;
    const CIdentifierBase* ident = NULL;

    switch (iInternalState)
        {
        case EStateExpectContextIdentifier:
            if (tokenType == Tokens::EIdentifier)
                {
                CreateTempContext();
                }
            break;

        case EStateExpectBaseOrFirstMember:
            iResult = Parser::ENoError; // default to ok - most cases here will succeed
            switch (tokenType)
                {
                case Tokens::EColon:
                    iInternalState = EStateExpectBaseIdentifier;
                    break;

                case Tokens::EIdentifier:
                case Tokens::EIntType:
                case Tokens::EPadType:
                case Tokens::EMessageIdType:
                    CreateTempMember();
                    break;

                default:
                    iResult = Parser::EUnexpectedToken;
                }
            break;

        case EStateExpectBaseIdentifier:
            iInternalState = EStateExpectMoreMembersOrEnd;
            if (tokenType == Tokens::EIdentifier)
                {
                ident = ParserSM().FindIdentifier(CurrentToken());
                if (!ident)
                    {
                    iResult = Parser::EUnknownIdentifier;
                    }
                else if (ident->Type() == Parser::EContextIdentifier)
                    {
                    iIdentifier->SetBaseContext(static_cast<const CContextIdentifier*>(ident));
                    iResult = Parser::ENoError;
                    }
                else
                    {
                    iResult = Parser::EInvalidType;
                    }
                }
            break;

        case EStateExpectMemberNameOrFormat:
            switch (tokenType)
                {
                case Tokens::EIdentifier:
                    if (iIdentifier->FindMember(CurrentToken()))
                        {
                        iResult = Parser::EDuplicateIdentifier;
                        delete iTempMember;
                        }
                    else
                        {
                        iTempMember->iMemberName = _strdup(CurrentToken());
                        iIdentifier->AddMember(iTempMember);
                        iResult = Parser::ENoError;
                        }
                    iInternalState = EStateExpectMoreMembersOrEnd;
                    iTempMember = NULL;
                    break;

                case Tokens::EDisplayDec:
                case Tokens::EDisplayHex:
                    iTempMember->iIdentifierOptions = new TIntegerIdentifierOptions(tokenType == Tokens::EDisplayHex);
                    iInternalState = EStateExpectMemberName;
                    iResult = Parser::ENoError;
                    break;
                }
            break;

        case EStateExpectMemberName:
            if (tokenType == Tokens::EIdentifier)
                {
                if (iIdentifier->FindMember(CurrentToken()))
                    {
                    iResult = Parser::EDuplicateIdentifier;
                    delete iTempMember;
                    }
                else
                    {
                    iTempMember->iMemberName = _strdup(CurrentToken());
                    iIdentifier->AddMember(iTempMember);
                    iResult = Parser::ENoError;
                    }
                iInternalState = EStateExpectMoreMembersOrEnd;
                iTempMember = NULL;
                }
            break;

        case EStateExpectMoreMembersOrEnd:
            iResult = Parser::ENoError;
            switch (tokenType)
                {
                case Tokens::EEnd:
                    if (iIdentifier->iMembers.size() > 0)
                        {
                        iInternalState = EStateExpectEndContext;
                        }
                    else
                        {
                        iResult = Parser::EUnexpectedToken;
                        }
                    break;

                case Tokens::EIdentifier:
                case Tokens::EIntType:
                case Tokens::EPadType:
                case Tokens::EMessageIdType:
                    CreateTempMember();
                    break;

                default:
                    iResult = Parser::EUnexpectedToken;
                }
            break;

        case EStateExpectEndContext:
            if (tokenType == Tokens::EContext)
                {
                if (ParserSM().FindContextBySize(iIdentifier->TotalSize()))
                    {
                    iResult = Parser::EDuplicateIdentifier;
                    }
                else
                    {
                    iResult = Parser::ENoError;
                    ParserSM().AddIdentifier(iIdentifier);
                    iIdentifier = NULL;
                    }
                }
            ParserSM().SetState(new CInitialState(ParserSM()));
            break;

        case EStateExpectPadSize:
            {
            iResult = Parser::ENoError;
            iInternalState = EStateExpectMoreMembersOrEnd;
            unsigned int v = 0;
            if (tokenType == Tokens::ENumberHex)
                {
                v = HexToVal(CurrentToken());
                }
            else if (tokenType == Tokens::ENumberDec)
                {
                v = atol(CurrentToken());
                }
            else
                {
                iResult = Parser::EUnexpectedToken;
                delete iTempMember;
                }
            if (iResult == Parser::ENoError)
                {
                if (v >= 1)
                    {
                    iTempMember->iArraySize = v;
                    iIdentifier->AddMember(iTempMember);
                    }
                else
                    {
                    iResult = Parser::EValueOutOfRange;
                    delete iTempMember;
                    }
                }
            iTempMember = NULL;
            }
            break;
        }

    if (iResult != Parser::ENoError)
        {
        ParserSM().SetError(iResult);
        }
    }

void CContextProcessor::CreateTempContext()
    {
    if (!ParserSM().FindIdentifier(CurrentToken()))
        {
        iIdentifier = new CContextIdentifier(CurrentToken());
        iInternalState = EStateExpectBaseOrFirstMember;
        iResult = Parser::ENoError;
        }
    else
        {
        iResult = Parser::EDuplicateIdentifier;
        }
    }

void CContextProcessor::CreateTempMember()
    {
    // Expect type of the member - can be a builtin (PAD, UINTx, INTx, TMESSAGEID)
    // or an identifier of a predefined struct/enum
    const CIdentifierBase* ident = ParserSM().FindIdentifier(CurrentToken());
    if (!ident) 
        {
        iResult = EUnknownIdentifier;
        }
    else if (ident->Type() == EEnumTypeIdentifier || ident->Type() == EStructIdentifier
        || ident->Type() == EIntegerTypeIdentifier || ident->Type() == Parser::EMessageIdTypeIdentifier)
        {
        iTempMember = new TMember();
        iTempMember->iMemberType = ident;

        if (ident->Type() == EIntegerTypeIdentifier)
            {
            iInternalState = EStateExpectMemberNameOrFormat;
            }
        else
            {
            iInternalState = EStateExpectMemberName;
            }
        }
    else if (ident->Type() == EPadTypeIdentifier)
        {
        iTempMember = new TMember();
        iTempMember->iMemberType = ident;
        iInternalState = EStateExpectPadSize;
        }
    else
        {
        iResult = Parser::EInvalidType;
        }
    }

} // namespace Parser

