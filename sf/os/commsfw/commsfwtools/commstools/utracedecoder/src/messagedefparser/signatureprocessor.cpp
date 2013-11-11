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

#include "messagedefparser\signatureprocessor.h"
#include "messagedefparser\initialstate.h"
#include "messagedefparser\definitiontokenizer.h"
#include "messagedefparser\integeridentifier.h"
#include "util.h"

namespace Parser
{

void CSignatureProcessor::ProcessState()
    {
    Tokens::TTokenType tokenType = CurrentTokenType();
    Parser::TResult result = Parser::EUnexpectedToken;
    const CIdentifierBase* ident = NULL;

    switch (iInternalState)
        {
        case EStateExpectSignatureIdentifier:
            if (tokenType == Tokens::EIdentifier)
                {
                if (!ParserSM().FindIdentifier(CurrentToken()))
                    {
                    iIdentifier = new CSignatureIdentifier(CurrentToken());
                    iInternalState = EStateExpectTypeIdOrBaseColon;
                    result = Parser::ENoError;
                    }
                else
                    {
                    result = Parser::EDuplicateIdentifier;
                    }
                }
            break;

        case EStateExpectTypeIdOrBaseColon:
            if (tokenType == Tokens::ETypeId)
                {
                result = Parser::ENoError;
                iInternalState = EStateExpectTypeIdEquals;
                }
            else if (tokenType == Tokens::EColon)
                {
                result = Parser::ENoError;
                iInternalState = EStateExpectSignatureBase;
                }
            break;

        case EStateExpectTypeIdEquals:
            if (tokenType == Tokens::EEquals)
                {
                result = Parser::ENoError;
                iInternalState = EStateExpectSignatureUidValue;
                }
            break;

        case EStateExpectSignatureUidValue:
            result = ResolveNumericToken(tokenType, CurrentToken(), 32, iIdentifier->TypeId().iUid);
            if (result == Parser::ENoError)
                {
                iInternalState = EStateExpectSignatureIdColon;
                }
            break;

        case EStateExpectSignatureIdColon:
            if (tokenType == Tokens::EColon)
                {
                result = Parser::ENoError;
                iInternalState = EStateExpectSignatureIdValue;
                }
            break;

        case EStateExpectSignatureIdValue:
            result = ResolveNumericToken(tokenType, CurrentToken(), 32, iIdentifier->TypeId().iType);
            if (result == Parser::ENoError)
                {
                iInternalState = EStateExpectMoreMembersOrEnd;
                }
            break;

        case EStateExpectMoreMembersOrEnd:
            result = Parser::ENoError;
            switch (tokenType)
                {
                case Tokens::EEnd:
                    iInternalState = EStateExpectEndSignature;
                    break;

                case Tokens::EIdentifier:
                case Tokens::EIntType:
                case Tokens::EPadType:
                case Tokens::EMessageIdType:
                    ident = ParserSM().FindIdentifier(CurrentToken());
                    if (!ident) 
                        {
                        result = EUnknownIdentifier;
                        }
                    else if (ident->Type() == EEnumTypeIdentifier || ident->Type() == EStructIdentifier
                        || ident->Type() == EIntegerTypeIdentifier || ident->Type() == EContextIdentifier
                        || ident->Type() == Parser::EMessageIdTypeIdentifier)
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
                        result = Parser::EInvalidType;
                        }
                    break;
                
                default:
                    result = Parser::EUnexpectedToken;
                }
            break;

        case EStateExpectMemberNameOrFormat:
            switch (tokenType)
                {
                case Tokens::EIdentifier:
                    if (iIdentifier->FindMember(CurrentToken()))
                        {
                        result = Parser::EDuplicateIdentifier;
                        delete iTempMember;
                        }
                    else
                        {
                        iTempMember->iMemberName = _strdup(CurrentToken());
                        iIdentifier->AddMember(iTempMember);
                        result = Parser::ENoError;
                        }
                    iTempMember = NULL;
                    iInternalState = EStateExpectMoreMembersOrEnd;
                    break;

                case Tokens::EDisplayDec:
                case Tokens::EDisplayHex:
                    iTempMember->iIdentifierOptions = new TIntegerIdentifierOptions(tokenType == Tokens::EDisplayHex);
                    iInternalState = EStateExpectMemberName;
                    result = Parser::ENoError;
                    break;
                }
            break;

        case EStateExpectMemberName:
            if (tokenType == Tokens::EIdentifier)
                {
                if (iIdentifier->FindMember(CurrentToken()))
                    {
                    result = Parser::EDuplicateIdentifier;
                    delete iTempMember;
                    }
                else
                    {
                    iTempMember->iMemberName = _strdup(CurrentToken());
                    iIdentifier->AddMember(iTempMember);
                    result = Parser::ENoError;
                    }
                iTempMember = NULL;
                iInternalState = EStateExpectMoreMembersOrEnd;
                }
            break;

        case EStateExpectEndSignature:
            if (tokenType == Tokens::ESignature)
                {
                result = Parser::ENoError;
                ParserSM().AddIdentifier(iIdentifier);
                iIdentifier = NULL;
                }
            ParserSM().SetState(new CInitialState(ParserSM()));
            break;

        case EStateExpectSignatureBase:
            iInternalState = EStateExpectTypeId;
            if (tokenType == Tokens::EIdentifier)
                {
                ident = ParserSM().FindIdentifier(CurrentToken());
                if (!ident)
                    {
                    result = Parser::EUnknownIdentifier;
                    }
                else if (ident->Type() == Parser::ESignatureIdentifier)
                    {
                    iIdentifier->SetBaseSignature(static_cast<const CSignatureIdentifier*>(ident));
                    result = Parser::ENoError;
                    }
                else
                    {
                    result = Parser::EInvalidType;
                    }
                }
            break;

        case EStateExpectTypeId:
            if (tokenType == Tokens::ETypeId)
                {
                result = Parser::ENoError;
                iInternalState = EStateExpectTypeIdEquals;
                }
            break;

        case EStateExpectPadSize:
            {
            result = Parser::ENoError;
            iInternalState = 6;
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
                result = Parser::EUnexpectedToken;
                delete iTempMember;
                }
            if (result == Parser::ENoError)
                {
                if (v >= 1)
                    {
                    iTempMember->iArraySize = v;
                    iIdentifier->AddMember(iTempMember);
                    }
                else
                    {
                    result = Parser::EValueOutOfRange;
                    delete iTempMember;
                    }
                }
            iTempMember = NULL;
            }
            break;
        }

    if (result != Parser::ENoError)
        {
        ParserSM().SetError(result);
        }
    }

} // namespace Parser

