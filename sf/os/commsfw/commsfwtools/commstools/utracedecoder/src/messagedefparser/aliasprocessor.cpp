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

#include "messagedefparser\aliasprocessor.h"
#include "messagedefparser\initialstate.h"
#include "messagedefparser\definitiontokenizer.h"
#include "messagedefparser\signatureidentifier.h"
#include "messagedefparser\messageidentifier.h"
#include "messagedefparser\contextidentifier.h"
#include "util.h"

namespace Parser
{
#if 1

void CAliasProcessor::ProcessState()
    {
    Tokens::TTokenType tokenType = CurrentTokenType();
    iResult = Parser::EUnexpectedToken;

    switch (iInternalState)
        {
        case EStateExpectAliasTargetType:
            iResult = Parser::ENoError;
            switch (tokenType)
                {
                case Tokens::EContext:
                    iAliasTargetType = Parser::EContextIdentifier;
                    break;

                case Tokens::ESignature:
                    iAliasTargetType = Parser::ESignatureIdentifier;
                    break;

                case Tokens::EMessage:
                    iAliasTargetType = Parser::EMessageIdentifier;
                    break;
                
                default:
                    iResult = Parser::EUnexpectedToken;
                    break;
                }
            iInternalState = EStateExpectAliasName;
            break;

        case EStateExpectAliasName:
            if (tokenType == Tokens::EIdentifier)
                {
                if (!ParserSM().FindIdentifier(CurrentToken()))
                    {
                    iName = _strdup(CurrentToken());
                    iInternalState = EStateExpectEquals;
                    iResult = Parser::ENoError;
                    }
                else 
                    {
                    iResult = Parser::EDuplicateIdentifier;
                    }
                }
            break;

        case EStateExpectEquals:
            if (tokenType == Tokens::EEquals)
                {
                iInternalState = EStateExpectAliasTarget;
                iResult = Parser::ENoError;
                }
            break;

        case EStateExpectAliasTarget:
            if (tokenType == Tokens::EIdentifier)
                {
                const CIdentifierBase* realIdentifier;
                realIdentifier = ParserSM().FindIdentifier(CurrentToken());
                if (!realIdentifier || realIdentifier->Type() != iAliasTargetType)
                    {
                    iResult = Parser::EUnknownIdentifier;
                    }
                else
                    {
                    switch (iAliasTargetType)
                        {
                        case Parser::EContextIdentifier:
                            iIdentifier = new CContextAliasIdentifier(
                                iName, static_cast<const CContextIdentifier*>(realIdentifier));
                            break;

                        case Parser::ESignatureIdentifier:
                            iIdentifier = new CSignatureAliasIdentifier(
                                iName, static_cast<const CSignatureIdentifier*>(realIdentifier));
                            break;

                        case Parser::EMessageIdentifier:
                            iIdentifier = new CMessageAliasIdentifier(
                                iName, static_cast<const CMessageIdentifier*>(realIdentifier));
                            break;
                        }

                    if (iIdentifier)
                        {
                        ParserSM().AddIdentifier(iIdentifier);
                        iIdentifier = NULL;
                        iResult = Parser::ENoError;
                        ParserSM().SetState(new CInitialState(ParserSM()));
                        }
                    }
                }
            break;
        }

    if (iResult != Parser::ENoError)
        {
        ParserSM().SetError(iResult);
        }
    }


CAliasProcessor::~CAliasProcessor()
    {
    delete iName;
    }

#endif


} // namespace Parser

