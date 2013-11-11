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

#include "messagedefparser\structprocessor.h"
#include "messagedefparser\integeridentifier.h"
#include "messagedefparser\initialstate.h"
#include "messagedefparser\definitiontokenizer.h"
#include "util.h"

namespace Parser
{

void CStructProcessor::ProcessState()
    {
    Tokens::TTokenType tokenType = CurrentTokenType();
    iResult = Parser::EUnexpectedToken;
    const CIdentifierBase* ident = NULL;

    switch (iInternalState)
        {
        case EStateExpectStructIdentifier:
            // name of new struct
            if (tokenType == Tokens::EIdentifier)
                {
                if (!ParserSM().FindIdentifier(CurrentToken()))
                    {
                    iIdentifier = new CStructIdentifier(CurrentToken());
                    iInternalState = EStateExpectMoreMembersOrEnd;
                    iResult = Parser::ENoError;
                    }
                else
                    {
                    iResult = Parser::EDuplicateIdentifier;
                    }
                }
            break;

        case EStateExpectMoreMembersOrEnd:
            iResult = Parser::ENoError;
            switch (tokenType)
                {
                case Tokens::EPadType:
                case Tokens::EIntType:
                case Tokens::EIdentifier:
                case Tokens::EMessageIdType:
                    ident = ParserSM().FindIdentifier(CurrentToken());
                    if (!ident)
                        {
                        iResult = Parser::EUnknownIdentifier;
                        }
                    else if (ident->Type() == Parser::EEnumTypeIdentifier || ident->Type() == Parser::EStructIdentifier
                        || ident->Type() == Parser::EIntegerTypeIdentifier || ident->Type() == Parser::EContextIdentifier
                        || ident->Type() == Parser::EMessageIdTypeIdentifier)
                        {
                        iTempMember = new TMember();
                        iTempMember->iMemberType = ident;

                        if (ident->Type() == Parser::EIntegerTypeIdentifier)
                            {
                            iInternalState = EStateExpectMemberNameOrFormat;
                            }
                        else
                            {
                            iInternalState = EStateExpectMemberName;
                            }
                        }
                    else if (ident->Type() == Parser::EPadTypeIdentifier)
                        {
                        iTempMember = new TMember();
                        iTempMember->iMemberType = ident;
                        iInternalState = EStateExpectPadSize;
                        break;
                        }
                    else 
                        {
                        iResult = Parser::EInvalidType;
                        }
                    break;

                case Tokens::EEnd:
                    iInternalState = EStateExpectEndStruct;
                    if (iIdentifier->iMembers.size() == 0)
                        {
                        iResult = Parser::EUnexpectedToken;
                        }
                    break;

                default:
                    iResult = Parser::EUnexpectedToken;
                    break;
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

        case EStateExpectEndStruct:
            if (tokenType == Tokens::EStruct)
                {
                ParserSM().AddIdentifier(iIdentifier);
                iIdentifier = NULL;
                iResult = Parser::ENoError;
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


} // namespace Parser

