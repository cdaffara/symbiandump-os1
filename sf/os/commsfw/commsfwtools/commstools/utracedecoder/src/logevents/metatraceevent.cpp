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

#include <iostream>
#include <iomanip>
#include <math.h>

#include "utracedecoderapp.h"
#include "logevents\metatraceevent.h"
#include "e32btrace.h"

CMetaTraceEvent::CMetaTraceEvent(const CUTraceFrame& aFrame, MEventOutputHandler& aOutputHandler)
    : CTraceEventBase(aFrame, aOutputHandler)
    {
    SetPrefix("EMetaTrace");
    }

CMetaTraceEvent::CMetaTraceEvent(const CMultiPartFrameCollection& aFrameCollection, MEventOutputHandler& aOutputHandler)
    : CTraceEventBase(aFrameCollection, aOutputHandler)
    {
    SetPrefix("EMetaTrace");
    }

void CMetaTraceEvent::Describe(std::ostream& aOutput) const
    {
    int subcat = SubCategory();

    switch (subcat)
        {
        case BTrace::EMetaTraceTimestampsInfo:
			{
			int val = Arg1();
			int exponent = (signed char)(val>>24);
			int mantissa = val&0xffffff;
			double period1 = (double)mantissa * ::pow((double) 2, exponent);
			val = *((int*)Data() + 0);
			exponent = (signed char)(val>>24);
			mantissa = val&0xffffff;
			double period2 = (double)mantissa * ::pow((double) 2, exponent);
			int flags = *((int*)Data() + 1);
			CUTraceDecoderApp::SetTimestampPeriod((flags & 1)? CUTraceDecoderApp::E64Bit: CUTraceDecoderApp::ESplit, period1, period2);

            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Server Create: [DServer=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] [Name=" << (Data() + 4)
                << "] ";
			}
            break;

		default:
			;
		}
	}

