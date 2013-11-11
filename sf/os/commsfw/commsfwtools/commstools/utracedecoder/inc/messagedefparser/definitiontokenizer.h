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

#ifndef DEFINITIONTOKENIZER_H
#define DEFINITIONTOKENIZER_H

#include <iostream>
#include <fstream>
#include <string>

namespace Tokens
{

const int KBufferSize = 2048;
const int KMaxTokenSize = 128;

enum TTokenType
    {
    EUnknown,

    EColon,
    EEquals,

    EInclude,
    EConst,
    EEnd,
    EEnum,
    EStruct,
    ESignature,
    EContext,
    EMessage,
    ETypeId,
    EMessageId,
    EAlias,

    EIntType,
    EPadType,
    EMessageIdType,

    EIdentifier,
    ENumberDec,
    ENumberHex,
    EString,

    EDisplayDec,
    EDisplayHex,
    };

 enum TResult
    {
    ENoError,
    ETokenFound = ENoError,

    EEndOfFile,
    EUnexpectedToken,
    ETokenTooBig,
    EUnterminatedString,
    EUnknownState,
    EFileNotFound
    };

 const char* TokenTypeToString(TTokenType aType);

 class CDefinitionTokenizer
    {
    public:
        CDefinitionTokenizer();
        ~CDefinitionTokenizer();

        TResult LoadDefinitionFile(const std::string& aFilename);
        TResult GetNextToken();

        inline const char* Token()
            {
            return iToken;
            }
        
        inline TTokenType TokenType()
            {
            return iTokenType;
            }

        inline int Line()
            {
            return iLine;
            }

    private:
        enum TTokenState
            {
            EStateComplete,
            EStateError,
            EStateStartToken,
            EStateMaybeComment,
            EStateComment,
            EStateMultiLineComment,
            EStateMaybeEndMultiLineComment,
            EStateNumber,
            EStateDecimalNumber,
            EStateMaybeHexNumber,
            EStateHexNumber,
            EStateIdentifier,
            EStateMaybeString,
            EStateMaybeNegativeNumber
            };

        void RefillBuffer();
        bool IsTerminalChar();
        void ExamineIdentifierForKeyword();

        TTokenState ProcessStateStartToken();
        TTokenState ProcessStateMaybeComment();
        TTokenState ProcessStateComment();
        TTokenState ProcessStateMultiLineComment();
        TTokenState ProcessStateDecimalNumber();
        TTokenState ProcessStateNumber();
        TTokenState ProcessStateMaybeHexNumber();
        TTokenState ProcessStateHexNumber();
        TTokenState ProcessStateIdentifier();
        TTokenState ProcessStateMaybeString();
        TTokenState ProcessStateMaybeNegativeNumber();

    private:
        std::ifstream iInputFile;
        char* iInputBuffer;
        int iBufferOffset;
        int iLastBufferOffset;
        int iBufferSize;
        bool iBufferValid;
        char* iToken;
        int iTokenOffset;
        TTokenType iTokenType;
        int iLine;
        TTokenState iState;
        TResult iError;
    };

} // namespace Tokens

#endif
// DEFINITIONTOKENIZER_H

