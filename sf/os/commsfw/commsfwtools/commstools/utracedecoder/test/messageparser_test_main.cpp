// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ostream>
#include "definitiontokenizer.h"
#include "definitionparser.h"

int testTokenizer(int argc, char* argv[]);

int main(int argc, char* argv[])
    {
  //  return testTokenizer(argc, argv);

    if (argc < 2)
        {
        return -1;
        }

    Parser::CDefinitionParser parser;
    parser.ParseDefinitionFile(argv[1]);
    parser.DumpIdentifiers();
    return 0;
    }


int testTokenizer(int argc, char* argv[])
    {
    Tokens::CDefinitionTokenizer tokenizer;
    if (argc < 2)
        {
        return -1;
        }
    tokenizer.LoadDefinitionFile(argv[1]);

    Tokens::TResult result;
    result = tokenizer.GetNextToken();
    while (result != Tokens::EEndOfFile)
        {
        std::cout << "result=" << result << " : " << tokenizer.Token() << " : ";
        std::cout << Tokens::TokenTypeToString(tokenizer.TokenType());
        if (result != Tokens::ETokenFound)
            {
            std::cout << " - line " << tokenizer.Line();
            }
        std::cout << std::endl;
        result = tokenizer.GetNextToken();
        }

    return 0;
    }

