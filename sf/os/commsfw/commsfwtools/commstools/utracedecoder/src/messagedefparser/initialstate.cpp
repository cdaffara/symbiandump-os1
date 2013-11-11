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

#include "messagedefparser\initialstate.h"
#include "messagedefparser\constprocessor.h"
#include "messagedefparser\contextprocessor.h"
#include "messagedefparser\enumprocessor.h"
#include "messagedefparser\includeprocessor.h"
#include "messagedefparser\messageprocessor.h"
#include "messagedefparser\signatureprocessor.h"
#include "messagedefparser\structprocessor.h"
#include "messagedefparser\aliasprocessor.h"


namespace Parser
{
void CInitialState::ProcessState()
    {
    CStateBase* newState = NULL;
    switch (CurrentTokenType())
        {
        case Tokens::EInclude:
            newState = new CIncludeProcessor(ParserSM());
            break;

        case Tokens::EConst:
            newState = new CConstProcessor(ParserSM());
            break;

        case Tokens::EEnum:
            newState = new CEnumProcessor(ParserSM());
            break;

        case Tokens::EStruct:
            newState = new CStructProcessor(ParserSM());
            break;

        case Tokens::ESignature:
            newState = new CSignatureProcessor(ParserSM());
            break;

        case Tokens::EContext:
            newState = new CContextProcessor(ParserSM());
            break;

        case Tokens::EMessage:
            newState = new CMessageProcessor(ParserSM());
            break;
        
        case Tokens::EAlias:
            newState = new CAliasProcessor(ParserSM());
            break;
        
        default:
            ParserSM().SetError(Parser::EUnexpectedToken);
            break;
        }

    if (newState != NULL)
        {
        StateMachine().SetState(newState);
        }
    }

} // namespace Parser

