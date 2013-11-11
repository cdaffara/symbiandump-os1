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

#include "messagedefparser\constprocessor.h"
#include "messagedefparser\initialstate.h"
#include "messagedefparser\definitiontokenizer.h"
#include "util.h"

namespace Parser
{

void CConstProcessor::ProcessState()
    {
    Tokens::TTokenType tokenType = CurrentTokenType();
    iResult = Parser::EUnexpectedToken;

    switch (iInternalState)
        {
        case EStateExpectConstIdentifier:
            // Expect an identifier for the new const
            if (tokenType == Tokens::EIdentifier)
                {
                CreateTempConst();
                }
            break;

        case EStateExpectEquals:
            if (tokenType == Tokens::EEquals)
                {
                iInternalState = EStateExpectValue;
                iResult = Parser::ENoError;
                }
            break;

        case EStateExpectValue:
            {
            // Expect a value formatted as decimal or hex
            if (tokenType == Tokens::ENumberHex)
                {
                iIdentifier->iValue = HexToVal(CurrentToken());
                CommitConst();
                }
            else if (tokenType == Tokens::ENumberDec)
                {
                iIdentifier->iValue = atol(CurrentToken());
                CommitConst();
                }
            break;
            }
        }

    if (iResult != Parser::ENoError)
        {
        ParserSM().SetError(iResult);
        }
    }

void CConstProcessor::CreateTempConst()
    {
    if (!ParserSM().FindIdentifier(CurrentToken()))
        {
        iIdentifier = new CConstIdentifier(CurrentToken());
        iInternalState = EStateExpectEquals;
        iResult = Parser::ENoError;
        }
    else
        {
        iResult = Parser::EDuplicateIdentifier;
        }
    }

void CConstProcessor::CommitConst()
    {
    ParserSM().AddIdentifier(iIdentifier);
    iIdentifier = NULL;
    ParserSM().SetState(new CInitialState(ParserSM()));
    iResult = Parser::ENoError;
    }

} // namespace Parser

