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

#ifndef CONTEXTPROCESSOR_H
#define CONTEXTPROCESSOR_H

#include "messagedefparser\contextidentifier.h"
#include "messagedefparser\keywordprocessorbase.h"

namespace Parser
{

class CContextProcessor : public CKeywordProcessor<Parser::EStateContext, Parser::CContextIdentifier>
    {
    public:
        explicit CContextProcessor(CDefinitionParser& aStateMachine)
            : CKeywordProcessor<Parser::EStateContext, Parser::CContextIdentifier>(aStateMachine), iTempMember(NULL)
            {
            }

    public:
        //virtual void EnterState();
        virtual void ProcessState();

    private:
        void CreateTempContext();
        void CreateTempMember();

    private:
        TMember* iTempMember;
        Parser::TResult iResult;

        enum
            {
            EStateExpectContextIdentifier = CKeywordProcessorBase::EStateInitialState,
            EStateExpectBaseOrFirstMember,
            EStateExpectBaseIdentifier,
            EStateExpectMemberName,
            EStateExpectMemberNameOrFormat,
            EStateExpectMoreMembersOrEnd,
            EStateExpectEndContext,
            EStateExpectPadSize,
            };
    };


} // namespace Parser

#endif
// CONTEXTPROCESSOR_H

