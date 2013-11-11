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

#ifndef SIGNATUREPROCESSOR_H
#define SIGNATUREPROCESSOR_H

#include "keywordprocessorbase.h"
#include "signatureidentifier.h"

namespace Parser
{

class CSignatureProcessor : public CKeywordProcessor<Parser::EStateSignature, Parser::CSignatureIdentifier>
    {
    public:
        explicit CSignatureProcessor(CDefinitionParser& aStateMachine)
            : CKeywordProcessor<Parser::EStateSignature, Parser::CSignatureIdentifier>(aStateMachine), iTempMember(NULL)
            {
            }

    public:
        //virtual void EnterState();
        virtual void ProcessState();

    private:
        enum
            {
            EStateExpectSignatureIdentifier = CKeywordProcessorBase::EStateInitialState,
            EStateExpectTypeIdOrBaseColon,
            EStateExpectTypeIdEquals,
            EStateExpectSignatureUidValue,
            EStateExpectSignatureIdColon,
            EStateExpectSignatureIdValue,
            EStateExpectMoreMembersOrEnd,
            EStateExpectMemberName,
            EStateExpectMemberNameOrFormat,
            EStateExpectEndSignature,
            EStateExpectTypeId,
            EStateExpectPadSize,
            EStateExpectSignatureBase,
            };

        void AddMember(TMember* aMember);
        const TMember* FindMember(const char* aName);

    private:
        TMember* iTempMember;
    };

} // namespace Parser

#endif
// SIGNATUREPROCESSOR_H

