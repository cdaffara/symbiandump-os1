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

#ifndef ALIASPROCESSOR_H
#define ALIASPROCESSOR_H

#include "messagedefparser\keywordprocessorbase.h"
#include "messagedefparser\identifierbase.h"

namespace Parser
{
#if 1
class CAliasProcessor : public CKeywordProcessor<Parser::EStateAlias, Parser::CIdentifierBase>
    {
    public:
        explicit CAliasProcessor(CDefinitionParser& aStateMachine)
            : CKeywordProcessor<Parser::EStateAlias, Parser::CIdentifierBase>(aStateMachine), iInternalState(0)
            {
            }

    public:
        //virtual void EnterState();
        virtual void ProcessState();
        virtual ~CAliasProcessor();

    private:
        int iInternalState;
        Parser::TResult iResult;
        int iAliasTargetType;
        char* iName;

        enum 
            {
            EStateExpectAliasTargetType = CKeywordProcessorBase::EStateInitialState,
            EStateExpectAliasName,
            EStateExpectEquals,
            EStateExpectAliasTarget
            };
    };
#endif
} // namespace Parser

#endif
// ALIASPROCESSOR_H

