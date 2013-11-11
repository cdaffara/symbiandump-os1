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

#ifndef DEFINITIONPARSER_H
#define DEFINITIONPARSER_H

// Disable "identifier was truncated to '255' characters" warning in VC6
#pragma warning (disable : 4786)

#include <string>
#include <vector>

#include "statemachinebase.h"
#include "definitiontokenizer.h"
#include "nodemessagetypes.h"

namespace Parser
{

class CIdentifierBase;
class CSignatureIdentifier;
class CMessageIdentifier;
class CContextIdentifier;

enum TResult 
    {
    ENoError,
    EFileError,
    EUnexpectedToken,
    EUnexpectedEndOfFile,
    ECircularInclusion,
    EDuplicateIdentifier,
    EValueOutOfRange,
    EUnknownIdentifier,
    EInvalidType,
    };

class CDefinitionFile
    {
    public:
        explicit CDefinitionFile(const std::string& aFilename);
        ~CDefinitionFile();

        inline Tokens::CDefinitionTokenizer& Tokenizer()
            {
            return *iTokenizer;
            }

        inline const std::string& Filename()
            {
            return iFilename;
            }

        inline const std::string& FilePath()
            {
            return iFilePath;
            }

    private:
        Tokens::CDefinitionTokenizer* iTokenizer;
        std::string iFilename;
        std::string iFilePath;
    };


class CDefinitionParser : public CStateMachineBase
    {
    public:
        CDefinitionParser();
        ~CDefinitionParser();

        TResult ParseDefinitionFile(const char* aDefinitionFile);
        void SetError(TResult aError);

        bool IncludeDefinitionFile(const char* aDefinationFile);
        CDefinitionFile& CurrentFile();

        const CIdentifierBase* FindIdentifier(const char* aIdentifierName) const;
        void AddIdentifier(const CIdentifierBase* aIdentifier);
        void DumpIdentifiers() const;

        const CContextIdentifier* FindContextBySize(unsigned int aSize) const;
        const CSignatureIdentifier* FindSignatureByTypeId(const STypeId& aTypeId) const;
        const CMessageIdentifier* FindMessageByMessageId(const TMessageId& aMessageId) const;

        inline void SetTestMode()
            {
            iTesting = true;
            }

    private:
        void CloseCurrentFile();
        
    private:
        bool iTesting;
        TResult iError;
        std::vector<CDefinitionFile*> iFileStack;
        std::vector<const CIdentifierBase*> iAllIdentifiers; 
    };

} // namespace Parser

#endif
// DEFINITIONPARSER_H

