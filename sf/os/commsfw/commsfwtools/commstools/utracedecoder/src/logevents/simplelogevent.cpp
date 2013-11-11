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

#include "eventoutputhandler.h"
#include "util.h"
#include "logevents\simplelogevent.h"
#include "e32btrace.h"

CSimpleLogEvent::CSimpleLogEvent(const CUTraceFrame& aFrame, MEventOutputHandler& aOutputHandler)
    : CTraceEventBase(aFrame, aOutputHandler)
    {
    }


CSimpleLogEvent::CSimpleLogEvent(const std::string& aPrefix, const CUTraceFrame& aFrame, MEventOutputHandler& aOutputHandler)
    : CTraceEventBase(aFrame, aOutputHandler)
    {
    SetPrefix(aPrefix);
    }


CSimpleLogEvent::CSimpleLogEvent(const CMultiPartFrameCollection& aFrameCollection, MEventOutputHandler& aOutputHandler)
    : CTraceEventBase(aFrameCollection, aOutputHandler)
    {
    }


CSimpleLogEvent::CSimpleLogEvent(const std::string& aPrefix, const CMultiPartFrameCollection& aFrameCollection, MEventOutputHandler& aOutputHandler)
    : CTraceEventBase(aFrameCollection, aOutputHandler)
    {
    SetPrefix(aPrefix);
    }



void CSimpleLogEvent::Describe(std::ostream& aOutput) const
    {
    if (SubCategory() == UTrace::KText)
        {
        DescribeText(aOutput);
        }
    else
        {
        DescribeBinary(aOutput);
        }
    }

void CSimpleLogEvent::DescribeText(std::ostream& aOutput) const
    {
    aOutput << Data();
    }

void CSimpleLogEvent::DescribeBinary(std::ostream& aOutput) const
    {
    aOutput << "===BEGIN DUMP===" << std::endl;
    DumpBytes(aOutput, Data(), DataLength());
    aOutput << "===END DUMP===" << std::endl;
    }

