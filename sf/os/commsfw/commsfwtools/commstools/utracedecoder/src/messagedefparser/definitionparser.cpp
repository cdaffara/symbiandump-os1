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


#include <cassert>
#include <string>
#include <iostream>
#include <iomanip>
#include <direct.h>

#include "messagedefparser\statebase.h"
#include "messagedefparser\definitionparser.h"
#include "messagedefparser\constidentifier.h"
#include "messagedefparser\contextidentifier.h"
#include "messagedefparser\enumidentifier.h"
#include "messagedefparser\integeridentifier.h"
#include "messagedefparser\messageidentifier.h"
#include "messagedefparser\padidentifier.h"
#include "messagedefparser\signatureidentifier.h"
#include "messagedefparser\structidentifier.h"
#include "messagedefparser\initialstate.h"
#include "messagedefparser\messageidtypeidentifier.h"
#include "util.h"
#include "version.h"

namespace Parser
{

// aFilename must be an absolute filename
CDefinitionFile::CDefinitionFile(const std::string& aFilename)
    {
    iTokenizer = new Tokens::CDefinitionTokenizer();
    iFilename = aFilename;
    size_t sep = iFilename.find_last_of("\\");

    assert(sep != std::string::npos && iFilename[0] != '\\');
    iFilePath = iFilename.substr(0, sep);
    }

CDefinitionFile::~CDefinitionFile()
    {
    delete iTokenizer;
    }



CDefinitionParser::CDefinitionParser()
    {
    iTesting = false;
    iAllIdentifiers.push_back(new CIntegerTypeIdentifier("UINT32", 4));
    iAllIdentifiers.push_back(new CIntegerTypeIdentifier("UINT16", 2));
    iAllIdentifiers.push_back(new CIntegerTypeIdentifier("UINT8", 1));
    iAllIdentifiers.push_back(new CIntegerTypeIdentifier("INT32", 4, true));
    iAllIdentifiers.push_back(new CIntegerTypeIdentifier("INT16", 2, true));
    iAllIdentifiers.push_back(new CIntegerTypeIdentifier("INT8", 1, true));
    iAllIdentifiers.push_back(new CPadTypeIdentifier("PAD"));

    // Complex built-in types
    iAllIdentifiers.push_back(new CMessageIdTypeIdentifier(*this));
    }

CDefinitionParser::~CDefinitionParser()
    {
    while (!iFileStack.empty())
        {
        delete iFileStack.back();
        iFileStack.pop_back();
        }
    }


bool CDefinitionParser::IncludeDefinitionFile(const char* aDefinitionFile)
    {
    std::string includeDefFile;

    if (aDefinitionFile[1] == ':') // consider it an absolute file path
        {
        includeDefFile = aDefinitionFile;
        }
    else if (iFileStack.empty())
        {
        char* cwd = _getcwd(NULL, 0);
        if (aDefinitionFile[0] == '\\') // consider it relative to the root of the current drive
            {
            includeDefFile = std::string(cwd).substr(0, 2);
            }
        else
            {
            includeDefFile = cwd;
            includeDefFile += "\\";
            }
        includeDefFile += aDefinitionFile;
        free(cwd);
        }
    else
        {
        if (aDefinitionFile[0] == '\\')
            {
            // needs a drive
            includeDefFile = CurrentFile().FilePath().substr(0, 2);
            }
        else
            {
            includeDefFile = CurrentFile().FilePath();
            }
        includeDefFile += "\\";
        includeDefFile += aDefinitionFile;
        }
    
    // TODO: get absolute filename for aDefinitionfile based on original def file
    for (unsigned int i=0; i<iFileStack.size(); i++)
        {
        std::string includedFile = iFileStack.at(i)->FilePath() + "\\" + iFileStack.at(i)->Filename();
        if (!_strcmpi(includedFile.c_str(), includeDefFile.c_str()))
            {
            std::cerr << "Error: Circular inclusion of " << includeDefFile << " from ";
            std::cerr << CurrentFile().FilePath() << "\\" << CurrentFile().Filename() << " at line ";
            std::cerr << CurrentFile().Tokenizer().Line() << std::endl;
            iError = Parser::ECircularInclusion;
            return false;
            }
        }

    bool loadSuccess;
    CDefinitionFile* defFile = new CDefinitionFile(includeDefFile);
    loadSuccess = (defFile->Tokenizer().LoadDefinitionFile(includeDefFile) == Tokens::ENoError);
    if (!loadSuccess)
        {
        // if loading the first (the specified initial definition file) and it failed
        // and the file isn't root relative or absolute try looking in the exe dir
        if (iFileStack.empty() && aDefinitionFile[0] != '\\' && aDefinitionFile[1] != ':') 
            {
            includeDefFile = ::__argv[0];
            size_t sep;
            sep = includeDefFile.find_last_of('\\');
            if (sep != std::string::npos)
                {
                includeDefFile.erase(sep + 1);
                includeDefFile += aDefinitionFile;
                }

            delete defFile;
            defFile = new CDefinitionFile(includeDefFile);
            loadSuccess = (defFile->Tokenizer().LoadDefinitionFile(includeDefFile) == Tokens::ENoError);
            }

        if (!loadSuccess)
            {
            delete defFile;
            if (iFileStack.empty())
                {
                std::cerr << "Error loading definition file: " << aDefinitionFile << std::endl;
                }
            else
                {
                std::cerr << "Error loading definition file: " << includeDefFile
                    << " included from "
                    << CurrentFile().FilePath() << "\\" << CurrentFile().Filename()
                    << " at line "
                    << CurrentFile().Tokenizer().Line() << std::endl;
                }

            iError = EFileError;
            return false;
            }
        }
    
    iFileStack.push_back(defFile);
    return true;
    }


void CDefinitionParser::CloseCurrentFile()
    {
    assert(!iFileStack.empty());
    CDefinitionFile* defFile = iFileStack.back();
    iFileStack.pop_back();
    delete defFile;
    }


CDefinitionFile& CDefinitionParser::CurrentFile()
    {
    assert(!iFileStack.empty());
    return *(iFileStack.back());
    }


Parser::TResult CDefinitionParser::ParseDefinitionFile(const char* aDefinitionFile)
    {
    iError = ENoError;
    if (!IncludeDefinitionFile(aDefinitionFile))
        {
        return iError;
        }

    SetState(new CInitialState(*this));

    do
        {
        while (CurrentFile().Tokenizer().GetNextToken() == Tokens::ETokenFound)
            {
            State().ProcessState();

            if (iError != Parser::ENoError && !iTesting)
                {
                return iError;
                }
            }

        if (State().StateId() != Parser::EStateInitial)
            {
            std::cerr << "Error: Unexpected end of file ";
            std::cerr << CurrentFile().Filename() << " at line ";
            std::cerr << CurrentFile().Tokenizer().Line() << std::endl;
            if (!iTesting)
                {
                return Parser::EUnexpectedEndOfFile;
                }
            }

        // finished with current file
        CloseCurrentFile();

        } while (iFileStack.size() > 0);

     return iError;
    }


void CDefinitionParser::DumpIdentifiers() const
    {
    unsigned int i;
    std::cout << "utracedecoder " APPVERSION << std::endl;
    std::cout << "Identifier List" << std::endl << "===============" << std::endl;
    for (i=0; i<iAllIdentifiers.size(); i++)
        {
        std::cout << std::endl;

        switch (iAllIdentifiers.at(i)->Type())
            {
            case EConstIdentifier:
                {
                const CConstIdentifier* ident = static_cast<const CConstIdentifier*>(iAllIdentifiers.at(i));
                std::cout << "CONST " << ident->Name() << " = " << ident->iValue << std::endl;
                }
                break;

            case EEnumTypeIdentifier:
                {
                const CEnumTypeIdentifier* ident = static_cast<const CEnumTypeIdentifier*>(iAllIdentifiers.at(i));
                std::cout << "ENUM " << ident->Name() << "(size=";
                std::cout << ident->Size();
                std::cout << ")" << std::endl;
                unsigned int j;
                for (j=0; j<ident->iEnumValues.size(); j++)
                    {
                    std::cout << "   " << ident->iEnumValues.at(j)->Name() << " = " << ident->iEnumValues.at(j)->iValue << std::endl;
                    }
                std::cout << std::endl;
                }
                break;

            case EStructIdentifier:
            case EMessageIdTypeIdentifier:
                {
                const CStructIdentifier* ident = static_cast<const CStructIdentifier*>(iAllIdentifiers.at(i));
                std::cout << "STRUCT " << ident->Name() << "(size=";
                std::cout << ident->TotalSize();
                std::cout << ")" << std::endl;
                unsigned int j;
                for (j=0; j<ident->iMembers.size(); j++)
                    {
                    std::cout << "   " << ident->iMembers.at(j)->iOffset << ": ";
                    std::cout << "   " << ident->iMembers.at(j)->iMemberType->Name() << " "
                        << ident->iMembers.at(j)->iMemberName
                        << " (" << ident->iMembers.at(j)->iArraySize * ident->iMembers.at(j)->iMemberType->TotalSize()
                        << ")" << std::endl;
                    }
                std::cout << std::endl;
                }
                break;

            case ESignatureIdentifier:
                {
                if (iAllIdentifiers.at(i)->Flags() & KAliasIdentifier)
                    {
                    const CSignatureAliasIdentifier* ident = static_cast<const CSignatureAliasIdentifier*>(iAllIdentifiers.at(i));
                    std::cout << "ALIAS SIGNATURE " << ident->Name()
                        << " = " << ident->RealName() << std::endl;
                    }
                else
                    {
                    const CSignatureIdentifier* ident = static_cast<const CSignatureIdentifier*>(iAllIdentifiers.at(i));
                    std::cout << "SIGNATURE " << ident->Name() << "(size=";
                    std::cout << ident->TotalSize();
                    std::cout << ")" << std::endl;
                    if (ident->BaseSignature())
                        {
                        std::cout << "   BASE SIGNATURE = " << ident->BaseSignature()->Name()
                            << " (size=" << ident->BaseSignature()->TotalSize() << ")" << std::endl;
                        }
                    std::cout << "   STYPEID = (0x" << std::setw(8) << std::noshowbase
                        << std::nouppercase << std::hex << std::setfill('0') << ident->TypeId().iUid
                        << ", " << std::dec << ident->TypeId().iType << ")" << std::endl;

                    unsigned int j;
                    for (j=0; j<ident->iMembers.size(); j++)
                        {
                        std::cout << "   " << ident->iMembers.at(j)->iOffset << ": ";
                        std::cout << ident->iMembers.at(j)->iMemberType->Name() << " "
                            << ident->iMembers.at(j)->iMemberName
                            << " (size=" << ident->iMembers.at(j)->iArraySize * ident->iMembers.at(j)->iMemberType->TotalSize()
                            << ")" << std::endl;
                        }
                    }
                std::cout << std::endl;
                }
                break;

            case EContextIdentifier:
                {
                if (iAllIdentifiers.at(i)->Flags() & KAliasIdentifier)
                    {
                    const CContextAliasIdentifier* ident = static_cast<const CContextAliasIdentifier*>(iAllIdentifiers.at(i));
                    std::cout << "ALIAS CONTEXT " << ident->Name()
                        << " = " << ident->RealName() << std::endl << std::endl;
                    }
                else
                    {
                    const CContextIdentifier* ident = static_cast<const CContextIdentifier*>(iAllIdentifiers.at(i));
                    std::cout << "CONTEXT " << ident->Name() << "(size=";
                    std::cout << ident->TotalSize();
                    std::cout << ")" << std::endl;
                    if (ident->BaseContext())
                        {
                        std::cout << "   BASE CONTEXT = " << ident->BaseContext()->Name()
                            << " (size=" << ident->BaseContext()->TotalSize() << ")" << std::endl;
                        }

                    unsigned int j;
                    for (j=0; j<ident->iMembers.size(); j++)
                        {
                        std::cout << "   " << ident->iMembers.at(j)->iOffset << ": ";
                        std::cout << ident->iMembers.at(j)->iMemberType->Name() << " " 
                            << ident->iMembers.at(j)->iMemberName
                            << " (size=" << ident->iMembers.at(j)->iArraySize * ident->iMembers.at(j)->iMemberType->TotalSize()
                            << ")" << std::endl;
                        }
                    }
                std::cout << std::endl;
                }
                break;

            case EMessageIdentifier:
                {
                if (iAllIdentifiers.at(i)->Flags() & KAliasIdentifier)
                    {
                    const CMessageAliasIdentifier* ident = static_cast<const CMessageAliasIdentifier*>(iAllIdentifiers.at(i));
                    std::cout << "ALIAS MESSAGE " << ident->Name()
                        << " = " << ident->RealName() << std::endl << std::endl;
                    }
                else
                    {
                    const CMessageIdentifier* ident = static_cast<const CMessageIdentifier*>(iAllIdentifiers.at(i));
                    std::cout << "MESSAGE " << ident->Name() << std::endl;
                    if (ident->iSignature)
                        {
                        std::cout << "   SIGNATURE = " << ident->iSignature->Name() << std::endl;
                        }
                    std::cout << "   ID = (" << ident->iMessageId.iRealmId << ", " << ident->iMessageId.iMessageId << ")" << std::endl;
                    }
                std::cout << std::endl;
                }
                break;

            default:
                std::cout << iAllIdentifiers.at(i)->Name() << " (identifier type=" << iAllIdentifiers.at(i)->Type() << ")" << std::endl;
                break;
            }
        }
    }

const CIdentifierBase* CDefinitionParser::FindIdentifier(const char* aIdentifierName) const
    {
    unsigned int i;
    const CIdentifierBase* ident;
    for (i=0; i<iAllIdentifiers.size(); i++)
        {
        ident = iAllIdentifiers.at(i);
        if (!_strcmpi(ident->Name(), aIdentifierName))
            {
            return ident;
            }
        }
    return NULL;
    }


void CDefinitionParser::AddIdentifier(const CIdentifierBase* aIdentifier)
    {
    if (FindIdentifier(aIdentifier->Name()))
        {
        std::cerr << "ERROR: **** Adding duplicate identifier : " << aIdentifier->Name() << " ****" << std::endl;
        }
    assert(!FindIdentifier(aIdentifier->Name()));
    iAllIdentifiers.push_back(aIdentifier);
    }

const CContextIdentifier* CDefinitionParser::FindContextBySize(unsigned int aSize) const
    {
    unsigned int i;
    const CContextIdentifier* context;
    for (i=0; i<iAllIdentifiers.size(); i++)
        {
        context = static_cast<const CContextIdentifier*>(iAllIdentifiers.at(i));
        if (context->Type() == Parser::EContextIdentifier
            && context->TotalSize() == aSize)
            {
            return context;
            }   
        }
    return NULL;
    }

const CSignatureIdentifier* CDefinitionParser::FindSignatureByTypeId(const STypeId& aTypeId) const
    {
    unsigned int i;
    const CSignatureIdentifier* signature;
    for (i=0; i<iAllIdentifiers.size(); i++)
        {
        signature = static_cast<const CSignatureIdentifier*>(iAllIdentifiers.at(i));
        if (signature->Type() == Parser::ESignatureIdentifier
            && signature->TypeId().iUid == aTypeId.iUid
            && signature->TypeId().iType == aTypeId.iType) 
            {
            return signature;
            }   
        }
    return NULL;
    }


const CMessageIdentifier* CDefinitionParser::FindMessageByMessageId(const TMessageId& aMessageId) const
    {
    unsigned int i;
    const CMessageIdentifier* message;
    for (i=0; i<iAllIdentifiers.size(); i++)
        {
        message = static_cast<const CMessageIdentifier*>(iAllIdentifiers.at(i));
        if (message->Type() == Parser::EMessageIdentifier
            && message->iMessageId.iRealmId == aMessageId.iRealmId
            && message->iMessageId.iMessageId == aMessageId.iMessageId)
            {
            return message;
            }       
        }
    return NULL;
    }

void CDefinitionParser::SetError(TResult aError)
    {
    iError = aError;
    switch (iError)
        {
        case ENoError:
            return;
            //break;

        case EDuplicateIdentifier:
            std::cerr << "Error: Duplicate identifier \"" << CurrentFile().Tokenizer().Token() << "\" in ";
            break;

        case EValueOutOfRange:
            std::cerr << "Error: Value out of range \"" << CurrentFile().Tokenizer().Token() << "\" in ";
            break;

        case EUnexpectedToken:
            std::cerr << "Error: Unexpected token \"" << CurrentFile().Tokenizer().Token() << "\" in ";
            break;

        case EUnknownIdentifier:
            std::cerr << "Error: Unknown identifier \"" << CurrentFile().Tokenizer().Token() << "\" in ";
            break;

        default:
            std::cerr << "Error: (" << iError << ") in ";
        }

    std::cerr << CurrentFile().Filename() << " at line ";
    std::cerr << CurrentFile().Tokenizer().Line() << std::endl;            
    }

} // namespace Parser

