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

#ifndef KEYWORDPROCESSORBASE_H
#define KEYWORDPROCESSORBASE_H

#include "messagedefparser\definitiontokenizer.h"
#include "messagedefparser\definitionparser.h"
#include "messagedefparser\statebase.h"

namespace Parser
{

enum
    {
    EStateInitial,
    EStateInclude,
    EStateConst,
    EStateStruct,
    EStateSignature,
    EStateMessage,
    EStateEnum,
    EStateContext,
    EStateAlias,

    EStateError,
    EStateComplete
    };


class CKeywordProcessorBase : public CStateBase
    {
    protected:
        enum
            {
            EStateInitialState = 0
            };

        explicit CKeywordProcessorBase(int aStateId, CDefinitionParser& aStateMachine)
            : CStateBase(aStateId, aStateMachine), iInternalState(EStateInitialState)
            {
            }

        inline CDefinitionParser& ParserSM()
            {
            return static_cast<CDefinitionParser&>(StateMachine());
            }

        inline const char* CurrentToken()
            {
            return ParserSM().CurrentFile().Tokenizer().Token();
            }

        inline Tokens::TTokenType CurrentTokenType()
            {
            return ParserSM().CurrentFile().Tokenizer().TokenType();
            }

        Parser::TResult ResolveNumericToken(Tokens::TTokenType aType, const char* aToken, int aMaxSize, unsigned short int& aValue);
        Parser::TResult ResolveNumericToken(Tokens::TTokenType aType, const char* aToken, int aMaxSize, unsigned int& aValue);

    protected:
        int iInternalState;
    };

template<int STATEID, typename IDENTIFIERTYPE>
class CKeywordProcessor : public CKeywordProcessorBase
    {
    protected:
        explicit CKeywordProcessor<STATEID, IDENTIFIERTYPE>(CDefinitionParser& aStateMachine)
            : CKeywordProcessorBase(STATEID, aStateMachine)
            {
            iIdentifier = NULL;
            }

        virtual ~CKeywordProcessor<STATEID, IDENTIFIERTYPE>()
            {
            delete iIdentifier;
            }
        
    public:
        bool AddCompletedIdentifier()
            {
            if (ParserSM().FindIdentifier(iIdentifier->Name()))
                {
                return false;
                }

            ParserSM().AddIdentifier(iIdentifier);
            iIdentifier = NULL;
            return true;
            }

    protected:
        IDENTIFIERTYPE* iIdentifier;
    };

} // namespace Parser

#endif
// KEYWORDPROCESSORBASE_H

