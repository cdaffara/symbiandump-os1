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

#include <ctype.h>
#include <memory.h>
#include <string.h>

#include "messagedefparser\definitiontokenizer.h"

namespace Tokens
{

struct TTokenTypeText 
    {
    const char* iText;
    TTokenType iType;
    };


static const TTokenTypeText keywords[] =
    {
        { "enum", EEnum },
        { "message", EMessage },
        { "struct", EStruct },
        { "signature", ESignature }, 
        { "context", EContext },
        { "end", EEnd },
        { "const", EConst },
        { "include", EInclude },
        { "alias", EAlias },

        { "decimal", EDisplayDec },
        { "hex", EDisplayHex },

        { "uint8", EIntType },
        { "uint16", EIntType },
        { "uint32", EIntType },
        { "int8", EIntType },
        { "int16", EIntType },
        { "int32", EIntType },
        { "tmessageid", EMessageIdType },
        { "pad", EPadType },

        { "typeid", ETypeId },
        { "messageid", EMessageId },

        { NULL, EUnknown }
    };


static const TTokenTypeText tokenTypes[] =
    {
        { "EColon", EColon },
        { "EEquals", EEquals },
        { "EInclude", EInclude },
        { "EAlias", EAlias },
        { "EConst", EConst },
        { "EEnd", EEnd },
        { "EEnum", EEnum },
        { "EStruct", EStruct },
        { "ESignature", ESignature },
        { "EContext", EContext },
        { "EMessage", EMessage },
        { "EIdentifier", EIdentifier },
        { "ENumberDec", ENumberDec },
        { "ENumberHex", ENumberHex },
        { "EIntType", EIntType },
        { "EPadType", EPadType },
        { "EString", EString },
        { "ETypeId", ETypeId },
        { "EMessageId", EMessageId },
        { "EMessageIdType", EMessageIdType },
        { "EUnknown", EUnknown }
    };


const char* TokenTypeToString(Tokens::TTokenType aType)
    {
    int i = 0;
    while (tokenTypes[i].iType != EUnknown)
        {
        if (aType == tokenTypes[i].iType)
            {
            break;
            }
        ++i;
        }
    return tokenTypes[i].iText;
    }


CDefinitionTokenizer::CDefinitionTokenizer()
    {
    this->iToken = new char[KMaxTokenSize];
    this->iInputBuffer = new char[KBufferSize];
    this->iTokenOffset = 0;
    this->iLastBufferOffset = 0;
    this->iBufferValid = false;
    this->iBufferOffset = 0;
    this->iLine = 1;
    }


CDefinitionTokenizer::~CDefinitionTokenizer()
    {
    iInputFile.close();
    delete iToken;
    delete iInputBuffer;
    }


Tokens::TResult CDefinitionTokenizer::LoadDefinitionFile(const std::string& aFilename)
    {
    iInputFile.open(aFilename.c_str(), std::ios::in);
    iLine = 1;
    if (iInputFile.is_open())
        {
        RefillBuffer();
        iError = ENoError;
        }
    else
        {
        iError = EFileNotFound;
        }
    
    return iError;
    }


Tokens::TResult CDefinitionTokenizer::GetNextToken()
    {
    iTokenOffset = 0;
    iLastBufferOffset = iBufferOffset;
    iTokenType = EUnknown;
//    TTokenState state = EStateStartToken;
    iState = EStateStartToken;
    iError = EUnexpectedToken;

    while (iBufferValid)
        {
        // start to collect the token
        while (iBufferOffset < iBufferSize)
            {
            if (iTokenOffset == KMaxTokenSize)
                {
                iError = ETokenTooBig;
                return ETokenTooBig;
                }

            iToken[iTokenOffset] = iInputBuffer[iBufferOffset];
            if (iToken[iTokenOffset] == '\r')
                {
                ++iBufferOffset;
                continue;
                }
            
            switch (iState)
                {
                case EStateStartToken:
                    iState = ProcessStateStartToken();
                    break;
                
                case EStateMaybeComment:
                    iState = ProcessStateMaybeComment();
                    break;

                case EStateMultiLineComment:
                case EStateMaybeEndMultiLineComment:
                    iState = ProcessStateMultiLineComment();
                    break;

                case EStateComment:
                    iState = ProcessStateComment();
                    break;

                case EStateNumber:
                    iState = ProcessStateNumber();
                    break;

                case EStateDecimalNumber:
                    iState = ProcessStateDecimalNumber();
                    break;

                case EStateMaybeHexNumber:
                    iState = ProcessStateMaybeHexNumber();
                    break;

                case EStateHexNumber:
                    iState = ProcessStateHexNumber();
                    break;

                case EStateIdentifier:
                    iState = ProcessStateIdentifier();
                    break;

                case EStateMaybeString:
                    iState = ProcessStateMaybeString();
                    break;

                case EStateMaybeNegativeNumber:
                    iState = ProcessStateMaybeNegativeNumber();
                    break;

                default:
                    iState = EStateError;
                    iError = EUnknownState;
                    break;
                }

            
            // New state processing
            switch (iState)
                {
                case EStateError:
                    iTokenType = EUnknown;
                    iToken[++iTokenOffset] = 0; // include the invalid character in the token
                    iBufferOffset++;
                    return iError;
                    break;

                case EStateStartToken:
                    iTokenOffset = 0; 
                    iLastBufferOffset = iBufferOffset;
                    iBufferOffset++;
                    break;
                
                case EStateComplete:
                    // TODO: clear error
                    iLastBufferOffset = iBufferOffset;
                    iToken[iTokenOffset] = 0;
                    if (iTokenType == EIdentifier)
                        {
                        ExamineIdentifierForKeyword();
                        }
                    iError = ETokenFound;
                    return ETokenFound;
                    //break;

                case EStateComment:
                case EStateMultiLineComment:
                case EStateMaybeEndMultiLineComment:
                    iTokenOffset = 0;
                    iLastBufferOffset = iBufferOffset;
                    iBufferOffset++;
                    break;

                default:
                    ++iBufferOffset;
                    ++iTokenOffset;
                    break;
                }
            }
        
        RefillBuffer();
        }

    iToken[iTokenOffset] = 0;
    if (iTokenOffset == 0)
        {
        iError = EEndOfFile;
        }
    else
        {
        if (iTokenType == EIdentifier)
            {
            ExamineIdentifierForKeyword();
            }
        iError = ETokenFound;
        }

    return iError;
    }


void CDefinitionTokenizer::ExamineIdentifierForKeyword()
    {
    int i = 0;
    while (keywords[i].iText != NULL)
        {
        if (!_strcmpi(keywords[i].iText, iToken))
            {
            iTokenType = keywords[i].iType;
            break;
            }
        ++i;
        }
    }


void CDefinitionTokenizer::RefillBuffer()
    {
    int bufferSpace = KBufferSize;
    int bufferInUse = 0;

    if (iLastBufferOffset)
        {
        bufferInUse = iBufferSize - iLastBufferOffset;
        bufferSpace = KBufferSize - bufferInUse;
        memcpy(iInputBuffer, &iInputBuffer[iLastBufferOffset], bufferInUse);
        }

    memset(&iInputBuffer[bufferInUse], 0, bufferSpace);
    iInputFile.read(&iInputBuffer[bufferInUse], bufferSpace);
    iBufferSize = iInputFile.gcount();
    iBufferValid = (iBufferSize > 0);
    iBufferSize += bufferInUse;

    iBufferOffset = bufferInUse;
    iLastBufferOffset = 0;
    }


CDefinitionTokenizer::TTokenState CDefinitionTokenizer::ProcessStateStartToken()
    {
    TTokenState nextState = EStateError;
    iTokenType = EUnknown;

    switch (iToken[0])
        {
        // single char tokens
        case '=':
            iToken[++iTokenOffset] = 0;
            ++iBufferOffset;
            nextState = EStateComplete;
            iTokenType = EEquals;
            break;

        case ':':
            iToken[++iTokenOffset] = 0;
            ++iBufferOffset;
            nextState = EStateComplete;
            iTokenType = EColon;
            break;

        case ' ':
        case '\t':
            // consume leading whitespace
            nextState = EStateStartToken;
            break;

        case '\n':
            ++iLine;
            nextState = EStateStartToken;
            break;

        case '\"':
            nextState = EStateMaybeString;
            --iTokenOffset; // don't include the quotes in the token
            break;

        case '/':
            nextState = EStateMaybeComment; 
            break;

        case '-':
            nextState = EStateMaybeNegativeNumber;
            break;

        case '0':
            nextState = EStateNumber;
            iTokenType = ENumberDec;
            break;

        default:
            if (iToken[iTokenOffset] >= '1' && iToken[iTokenOffset] <= '9')
                {
                nextState = EStateDecimalNumber;
                iTokenType = ENumberDec;
                }
            else if (tolower(iToken[iTokenOffset]) >= 'a'&& tolower(iToken[iTokenOffset]) <= 'z')
                {
                nextState = EStateIdentifier;
                iTokenType = EIdentifier;
                }
            break;
        }

    return nextState;
    }


CDefinitionTokenizer::TTokenState CDefinitionTokenizer::ProcessStateMaybeNegativeNumber()
    {
    TTokenState nextState = EStateError;

    switch (iToken[iTokenOffset])
        {
        case '0':
            nextState = EStateNumber;
            iTokenType = ENumberDec;
            break;

        default:
            if (iToken[iTokenOffset] >= '1' && iToken[iTokenOffset] <= '9')
                {
                nextState = EStateDecimalNumber;
                iTokenType = ENumberDec;
                }
            break;
        }

    return nextState;
    }


CDefinitionTokenizer::TTokenState CDefinitionTokenizer::ProcessStateMaybeComment()
    {
    TTokenState nextState = EStateError;

    if (iToken[iTokenOffset] == '/')
        {
        nextState = EStateComment; 
        }
    else if (iToken[iTokenOffset] == '*')
        {
        nextState = EStateMultiLineComment;
        }

    return nextState;
    }


CDefinitionTokenizer::TTokenState CDefinitionTokenizer::ProcessStateComment()
    {
    TTokenState nextState = EStateComment;

    if (iToken[iTokenOffset] == '\n')
        {
        --iBufferOffset; // because the behaviour of moving to EStateStartToken is
                         // to progress to the next byte - we want to process the '\n'
        nextState = EStateStartToken; 
        iTokenType = EUnknown;
        }

    return nextState;
    }


CDefinitionTokenizer::TTokenState CDefinitionTokenizer::ProcessStateMultiLineComment()
    {
    TTokenState nextState = EStateMultiLineComment;

    if (iState == EStateMultiLineComment)
        {
        if (iToken[iTokenOffset] == '*')
            {
            // Started multi line comment
            nextState = EStateMaybeEndMultiLineComment;
            }
        }
    else if (iState == EStateMaybeEndMultiLineComment)
        {
        if (iToken[iTokenOffset] == '/')
            {
            nextState = EStateStartToken; 
            iTokenType = EUnknown;
            }
        }

    return nextState;
    }


CDefinitionTokenizer::TTokenState CDefinitionTokenizer::ProcessStateNumber()
    {
    TTokenState nextState = EStateError;

    if (IsTerminalChar())
        {
        nextState = EStateComplete;
        // iTokenType = ENumberDec; 
        }
    else
        {
        if (tolower(iToken[iTokenOffset]) == 'x')
            {
            nextState = EStateMaybeHexNumber;
            iTokenType = EUnknown;
            }
        else if (iToken[iTokenOffset] >= '0' && iToken[iTokenOffset] <= '9' )
            {
            nextState = EStateDecimalNumber;
            // iTokenType = ENumberDec;
            }
        }

    return nextState;
    }


CDefinitionTokenizer::TTokenState CDefinitionTokenizer::ProcessStateDecimalNumber()
    {
    TTokenState nextState = EStateError;

    if (IsTerminalChar())
        {
        nextState = EStateComplete;
        // iTokenType = ENumberDec;
        }
    else
        {
        if (iToken[iTokenOffset] >= '0' && iToken[iTokenOffset] <= '9')
            {
            nextState = EStateDecimalNumber;
            // iTokenType = ENumberDec;
            }
        }
    return nextState;
    }


CDefinitionTokenizer::TTokenState CDefinitionTokenizer::ProcessStateMaybeHexNumber()
    {
    TTokenState nextState = EStateError;
    
    char c = tolower(iToken[iTokenOffset]);
    if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))
        {
        nextState = EStateHexNumber;
        iTokenType = ENumberHex;
        }

    return nextState;
    }


CDefinitionTokenizer::TTokenState CDefinitionTokenizer::ProcessStateHexNumber()
    {
    TTokenState nextState = EStateError;

    if (IsTerminalChar())
        {
        nextState = EStateComplete;
        // iTokenType = ENumberHex;
        }
    else
        {
        char c = tolower(iToken[iTokenOffset]);
        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))
            {
            nextState = EStateHexNumber;
            // iTokenType = ENumberHex;
            }
        }
    return nextState;
    }


CDefinitionTokenizer::TTokenState CDefinitionTokenizer::ProcessStateIdentifier()
    {
    TTokenState nextState = EStateError;

    if (IsTerminalChar())
        {
        nextState = EStateComplete;
        // iTokenType = EIdentifier;
        }
    else
        {
        char c = tolower(iToken[iTokenOffset]);
        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c == '_'))
            {
            nextState = EStateIdentifier;
            // iTokenType = EIdentifier;
            }
        }
    return nextState;
    }


CDefinitionTokenizer::TTokenState CDefinitionTokenizer::ProcessStateMaybeString()
    {
    TTokenState nextState = EStateMaybeString;

    if (iToken[iTokenOffset] == '\"')
        {
        nextState = EStateComplete;
        ++iBufferOffset; // don't want to process the quote again
        iTokenType = EString;
        }
    else if (iToken[iTokenOffset] == '\n')
        {
        nextState = EStateError;
        --iTokenOffset; // don't include the \n in the bad token
        iError = EUnterminatedString;
        // iTokenType = EUnknown;
        }

    return nextState;
    }


bool CDefinitionTokenizer::IsTerminalChar()
    {
    char c = iToken[iTokenOffset];
    if (c == ' ' || c == '\t' || c == '\n' || c == ':' || c == '=' || c == '/')
        {
        return true;
        }
    return false;
    }

} // namespace Tokens

