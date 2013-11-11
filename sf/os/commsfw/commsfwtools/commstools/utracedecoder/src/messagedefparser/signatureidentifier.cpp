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
#include <sstream>
#include <iomanip>

#include "messagedefparser\signatureidentifier.h"

namespace Parser
{

void CSignatureIdentifier::AddMember(TMember* aMember)
    {
    assert(aMember);
    assert(aMember->iMemberType->Type() == Parser::EPadTypeIdentifier || aMember->iMemberName[0] != 0);
    assert(!FindMember(aMember->iMemberName));
    iMembers.push_back(aMember);
    aMember->iOffset = iSize;
    iSize += aMember->iMemberType->TotalSize() * aMember->iArraySize;
    }


void CSignatureIdentifier::SetBaseSignature(const CSignatureIdentifier* aBaseSignature)
    {
    iBaseSignature = aBaseSignature;
    }


const TMember* CSignatureIdentifier::FindMember(const std::string& aName)
    {
    unsigned int i;
    TMember* tmp;

    if (aName[0] == 0)
        {
        return NULL;
        }

    for (i=0; i<iMembers.size(); i++)
        {
        tmp = iMembers.at(i);
        if (!_strcmpi(tmp->iMemberName.c_str(), aName.c_str()))
            {
            return tmp;
            }
        }
    return NULL;
    }


bool CSignatureIdentifier::IsNodeSignalDerived() const
    {
    const CSignatureIdentifier* baseSig = this;
    while (baseSig)
        {
        if (!_strcmpi(baseSig->Name(), "TNodeSignal"))
            {
            return true;
            }
        baseSig = baseSig->iBaseSignature;
        }
    return false;
    }

int CSignatureIdentifier::TotalSize() const
    {
    if (iBaseSignature)
        {
        return Size() + iBaseSignature->TotalSize();
        }
    else
        {
        return Size();
        }
    }


void CSignatureIdentifier::Describe(const unsigned char* aData, unsigned int aLength, const void* aOptions, std::ostream& aOutput) const
    {
    if (iBaseSignature)
        {
        iBaseSignature->Describe(aData + Size(), aLength - Size(), NULL, aOutput);
        }

    unsigned int i;
    TMember* member;
    for (i=0; i<iMembers.size(); i++)
        {
        member = iMembers.at(i);
        if (member->iMemberName[0] != 0) // padding members have no name
            {
            aOutput << "[" + std::string(member->iMemberName) + "=";
            member->iMemberType->Describe(aData + member->iOffset, member->iMemberType->Size(), member->iIdentifierOptions, aOutput);
            aOutput << "] ";
            }
        }
    }

} // namespace Parser

