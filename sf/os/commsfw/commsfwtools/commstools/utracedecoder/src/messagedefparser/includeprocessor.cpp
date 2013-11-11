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

#include "messagedefparser\includeprocessor.h"
#include "messagedefparser\initialstate.h"
#include "messagedefparser\definitiontokenizer.h"

namespace Parser
{

void CIncludeProcessor::ProcessState()
    {
    if (CurrentTokenType() == Tokens::EString)
        {
        if (ParserSM().IncludeDefinitionFile(CurrentToken()))
            {
            ParserSM().SetState(new CInitialState(ParserSM()));
            }
        }
    else
        {
        ParserSM().SetError(Parser::EUnexpectedToken);
        }
    }

} // namespace Parser

