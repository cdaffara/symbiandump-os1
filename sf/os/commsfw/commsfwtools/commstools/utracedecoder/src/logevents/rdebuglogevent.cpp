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
#include "logevents\rdebuglogevent.h"
#include "e32btrace.h"

CRDebugLogEvent::CRDebugLogEvent(const CUTraceFrame& aFrame, MEventOutputHandler& aOutputHandler)
    : CSimpleLogEvent(aFrame, aOutputHandler)
    {
    SetPrefix("ERDebugPrintf");
    }

CRDebugLogEvent::CRDebugLogEvent(const std::string& aPrefix, const CUTraceFrame& aFrame, MEventOutputHandler& aOutputHandler)
    : CSimpleLogEvent(aFrame, aOutputHandler)
    {
    SetPrefix(aPrefix);
    }

CRDebugLogEvent::CRDebugLogEvent(const CMultiPartFrameCollection& aFrameCollection, MEventOutputHandler& aOutputHandler)
    : CSimpleLogEvent(aFrameCollection, aOutputHandler)
    {
    SetPrefix("ERDebugPrintf");
    }


CRDebugLogEvent::CRDebugLogEvent(const std::string& aPrefix, const CMultiPartFrameCollection& aFrameCollection, MEventOutputHandler& aOutputHandler)
    : CSimpleLogEvent(aFrameCollection, aOutputHandler)
    {
    SetPrefix(aPrefix);
    }

void CRDebugLogEvent::Describe(std::ostream& aOutput) const
    {
    DescribeText(aOutput);
    }


