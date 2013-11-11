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

#ifndef MESSAGEPROCESSOR_H
#define MESSAGEPROCESSOR_H

#include "keywordprocessorbase.h"
#include "messageidentifier.h"

namespace Parser
{

class CMessageProcessor : public CKeywordProcessor<Parser::EStateMessage, Parser::CMessageIdentifier>
    {
    public:
        explicit CMessageProcessor(CDefinitionParser& aStateMachine)
            : CKeywordProcessor<Parser::EStateMessage, Parser::CMessageIdentifier>(aStateMachine)
            {
            }

    public:
        //virtual void EnterState();
        virtual void ProcessState();

    private:
        enum
            {
            EStateExpectMessageIdentifier = CKeywordProcessorBase::EStateInitialState,
            EStateExpectSignature,
            EStateExpectSignatureEquals,
            EStateExpectSignatureIdentifier,
            EStateExpectMessageId,
            EStateExpectMessageIdEquals,
            EStateExpectMessageRealmValue,
            EStateExpectMessageColon,
            EStateExpectMessageIdValue,
            EStateExpectEnd,
            EStateExpectEndMessage,
            };

    };

} // namespace Parser

#endif
// MESSAGEPROCESSOR_H

