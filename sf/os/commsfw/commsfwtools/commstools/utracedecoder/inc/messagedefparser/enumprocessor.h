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

#ifndef ENUMPROCESSOR_H
#define ENUMPROCESSOR_H

#include "keywordprocessorbase.h"
#include "enumidentifier.h"

namespace Parser
{
class CEnumProcessor : public CKeywordProcessor<Parser::EStateEnum, Parser::CEnumTypeIdentifier>
    {
    public:
        explicit CEnumProcessor(CDefinitionParser& aStateMachine)
            : CKeywordProcessor<Parser::EStateEnum, Parser::CEnumTypeIdentifier>(aStateMachine),
            iTempEnumValue(NULL)
            {
            }

    public:
        //virtual void EnterState();
        virtual void ProcessState();

    private:
        CEnumValueIdentifier* iTempEnumValue;
        TIntegerIdentifierOptions iFormatOptions;

        enum
            {
            EStateExpectEnumIdentifierOrFormat = CKeywordProcessorBase::EStateInitialState,
            EStateExpectEnumIdentifier,
            EStateExpectColon,
            EStateExpectBaseIntType,
            EStateExpectEnumValueIdentifierOrEnd,
            EStateExpectEquals,
            EStateExpectEnumValue,
            EStateExpectEndEnum,
            };
    };

} // namespace Parser

#endif
// ENUMPROCESSOR_H

