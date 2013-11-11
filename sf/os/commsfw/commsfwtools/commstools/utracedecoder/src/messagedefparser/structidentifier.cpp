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

#include "messagedefparser\structidentifier.h"

namespace Parser
{

void CStructIdentifier::AddMember(TMember* aMember)
    {
    assert(aMember);
    assert(aMember->iMemberType->Type() == Parser::EPadTypeIdentifier || aMember->iMemberName[0] != 0);
    assert(!FindMember(aMember->iMemberName));
    iMembers.push_back(aMember);
    aMember->iOffset = iSize;
    iSize += aMember->iMemberType->TotalSize() * aMember->iArraySize;
    }


const TMember* CStructIdentifier::FindMember(const std::string& aName)
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

void CStructIdentifier::Describe(const unsigned char* aData, unsigned int aLength, const void* aOptions, std::ostream& aOutput) const
    {
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

