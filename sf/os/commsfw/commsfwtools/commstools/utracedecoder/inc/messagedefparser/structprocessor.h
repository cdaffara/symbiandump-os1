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

#ifndef STRUCTPROCESSOR_H
#define STRUCTPROCESSOR_H

#include "keywordprocessorbase.h"
#include "structidentifier.h"

namespace Parser
{

class CStructProcessor : public CKeywordProcessor<Parser::EStateStruct, Parser::CStructIdentifier>
    {
    public:
        explicit CStructProcessor(CDefinitionParser& aStateMachine)
            : CKeywordProcessor<Parser::EStateStruct, Parser::CStructIdentifier>(aStateMachine), iTempMember(NULL)
            {
            }

    public:
        //virtual void EnterState();
        virtual void ProcessState();

    private:
        enum
            {
            EStateExpectStructIdentifier = CKeywordProcessorBase::EStateInitialState,
            EStateExpectMoreMembersOrEnd,
            EStateExpectMemberName,
            EStateExpectMemberNameOrFormat,
            EStateExpectEndStruct,
            EStateExpectPadSize,
            };

        TMember* iTempMember;
        Parser::TResult iResult;
    };
        
} // namespace Parser
 
#endif
// STRUCTPROCESSOR_H

