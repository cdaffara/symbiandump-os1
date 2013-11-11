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

#ifndef CONSTPROCESSOR_H
#define CONSTPROCESSOR_H

#include "messagedefparser\constidentifier.h"
#include "messagedefparser\keywordprocessorbase.h"

namespace Parser
{
class CConstProcessor : public CKeywordProcessor<Parser::EStateConst, CConstIdentifier>
    {
    public:
        explicit CConstProcessor(CDefinitionParser& aStateMachine)
            : CKeywordProcessor<Parser::EStateConst, CConstIdentifier>(aStateMachine), iInternalState(0)
            {
            }

    public:
        //virtual void EnterState();
        virtual void ProcessState();

    private:
        void CreateTempConst();
        void CommitConst();

    private:
        int iInternalState;
        Parser::TResult iResult;

        enum 
            {
            EStateExpectConstIdentifier = CKeywordProcessorBase::EStateInitialState,
            EStateExpectEquals,
            EStateExpectValue
            };
    };

} // namespace Parser

#endif
// CONSTPROCESSOR_H

