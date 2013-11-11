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

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "utracedecoderapp.h"
#include "logevents\nodemessageevent.h"
#include "messagedefparser\definitionparser.h"
#include "messagedefparser\signatureidentifier.h"
#include "messagedefparser\contextidentifier.h"
#include "messagedefparser\messageidentifier.h"

CNodeMessageEvent::CNodeMessageEvent(const CUTraceFrame& aFrame, MEventOutputHandler& aOutputHandler)
    : CSimpleLogEvent(aFrame, aOutputHandler)
    {
    SetPrefix("ENodeMessages");
    }

CNodeMessageEvent::CNodeMessageEvent(const CMultiPartFrameCollection& aFrameCollection, MEventOutputHandler& aOutputHandler)
    : CSimpleLogEvent(aFrameCollection, aOutputHandler)
    {
    SetPrefix("ENodeMessages");
    }

void CNodeMessageEvent::Describe(std::ostream& aOutput) const
    {
    unsigned char* payload = Data();
    unsigned int length = DataLength();
    unsigned int subCategory = SubCategory();
    unsigned int textLength = Arg1();

    if (subCategory == ELogMessage || subCategory == ELogAddress || subCategory == ELogNode)
        {
        if (textLength > length)
            {
            aOutput << "**** ERROR: Corrupt event. Embedded text length > Event length **** ";
            return;
            }

        if (textLength > 0)
            {
            std::string embeddedText = std::string(reinterpret_cast<char*>(Data())).substr(0, textLength);
            aOutput << embeddedText << " ";
            payload += textLength;
            length -= textLength;
            }

        if (length <= 0)
            {
            aOutput << "**** ERROR: Corrupt event. No data after embedded text **** ";
            return;
            }
        }

    switch (subCategory)
        {
        case ELogMessage:
            {
            const SMessageHeader& header = *reinterpret_cast<const SMessageHeader*>(payload);
            const Parser::CSignatureIdentifier* msgSig = ::gMsgDefParser->FindSignatureByTypeId(header.iTypeId);
            if (msgSig)
                {
                aOutput << "[Signature=" + std::string(msgSig->Name()) + "] ";

                //if (msgSig->IsNodeSignalDerived())
                //    {
                //    const TMessageId& msgId = *reinterpret_cast<const TMessageId*>(payload + header.iLength - sizeof(TMessageId));
            
                //    const Parser::CMessageIdentifier* msg = ::gMsgDefParser->FindMessageByMessageId(msgId);
                //    if (msg)
                //        {
                //        aOutput << "[Message=" + std::string(msg->Name()) + "] ";
                //        }
                //    }
                if (header.iLength - sizeof(header) == msgSig->TotalSize())
                    {
                    msgSig->Describe(payload + sizeof(header), header.iLength - sizeof(header), NULL, aOutput);
                    }
                else
                    {
                    aOutput << "**** ERROR: Corrupt event or incorrect signature definition - "
                        << "[Signature=0x" << std::noshowbase << std::setw(8) << std::setfill('0')
                        << std::nouppercase << std::hex << header.iTypeId.iUid
                        << ":0x" << std::noshowbase << std::setw(8) << std::setfill('0')
                        << std::nouppercase << std::hex << header.iTypeId.iType << "] **** ";
                    }

                }
            else
                {
                aOutput << "**** ERROR: Undefined signature - [Signature=0x"
                    << std::noshowbase << std::setw(8) << std::setfill('0') << std::nouppercase << std::hex << header.iTypeId.iUid
                    << ":0x"
                    << std::noshowbase << std::setw(8) << std::setfill('0') << std::nouppercase << std::hex << header.iTypeId.iType
                    << "] **** ";
                }
            }
            break;

        case ELogBlockStart:
            EventOutputHandler().PushNewLineEnabled(false);
            aOutput << "[STARTBLOCK=";
            CSimpleLogEvent::DescribeText(aOutput);
            aOutput << "] ";
            break;

        case ELogBlockEnd:
            aOutput << "[ENDBLOCK=";
            CSimpleLogEvent::DescribeText(aOutput);
            aOutput << "] ";
            EventOutputHandler().PopNewLineEnabled();
            break;

        case ELogNodeCreate:
        case ELogNodeDestroy:
        case ELogInfo:
            CSimpleLogEvent::DescribeText(aOutput);
            break;

        case ELogAddress:
            {
            const SRuntimeContextBase& rtCtx = *reinterpret_cast<const SRuntimeContextBase*>(payload);
            if (rtCtx.iSize & 0x80) // top bit set indicates not null
                {
                aOutput << "[Address=";
                const Parser::CContextIdentifier* context = ::gMsgDefParser->FindContextBySize(rtCtx.iSize & 0x7f); // top bit indicates NULL
                if (context)
                    {
                    if (length == context->TotalSize())
                        {
                        context->Describe(payload, length, NULL, aOutput);
                        }
                    else
                        {
                        aOutput << "**** ERROR: ELogAddress - Event data size and context size don't match **** ";
                        }
                    }
                else
                    {
                    aOutput << "**** ERROR: ELogAddress - Logged address uses undefined context type **** ";
                    }
                aOutput << "] ";
                }
            else
                {
                aOutput << "[Address=Null] ";
                }
            }
            break;

        case ELogNode:
            {
            aOutput << "[ANode=0x" << std::hex << std::setw(8)
                << std::setfill('0') << std::nouppercase  << std::noshowbase
                << BytesToInt(payload)
                << "] ";
            }
            break;

        default:
            aOutput << "**** ERROR: Unknown or unsupported log statement **** ";
            break;
        }
    }

