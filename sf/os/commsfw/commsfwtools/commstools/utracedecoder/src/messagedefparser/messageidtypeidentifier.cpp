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
// messageidtypeidentifer.cpp
// 
//

#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "messagedefparser\messageidtypeidentifier.h"
#include "messagedefparser\messageidentifier.h"
#include "messagedefparser\definitionparser.h"

namespace Parser
{

CMessageIdTypeIdentifier::CMessageIdTypeIdentifier(Parser::CDefinitionParser& aParser)
    : CStructIdentifier(Parser::EMessageIdTypeIdentifier, "TMESSAGEID"), iParser(aParser)
    {
    TMember* member;
    member = new TMember();
    member->iMemberName = "iMessageId";
    member->iMemberType = aParser.FindIdentifier("UINT16");
    AddMember(member);

    member = new TMember();
    member->iArraySize = 2;
    member->iMemberType = aParser.FindIdentifier("PAD");
    AddMember(member);

    member = new TMember();
    member->iMemberName = "iRealmId";
    member->iMemberType = aParser.FindIdentifier("UINT32");
    AddMember(member);
    }

void CMessageIdTypeIdentifier::Describe(const unsigned char* aData, unsigned int aLength, const void* aOptions, std::ostream& aOutput) const
    {
    TMessageId messageId;
    messageId.iMessageId = (0xffff & *reinterpret_cast<const unsigned int*>(aData));
    messageId.iRealmId = (0xffffffff & *reinterpret_cast<const unsigned int*>(aData+4));

    iMembers.at(2)->iMemberType->Describe(aData + 4, 4, NULL, aOutput);
    aOutput << ":";
    const CMessageIdentifier* message = iParser.FindMessageByMessageId(messageId);
    if (message)
        {
        aOutput << message->Name();
        }
    else
        {
        iMembers.at(0)->iMemberType->Describe(aData, 2, NULL, aOutput);
        }

    //aOutput << " ";
    }

} // namespace Parser

