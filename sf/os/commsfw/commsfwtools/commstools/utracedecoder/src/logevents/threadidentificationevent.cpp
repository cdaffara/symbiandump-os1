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

#include <sstream>
#include <iomanip>

#include "e32btrace.h"
#include "eventoutputhandler.h"
#include "logevents\threadidentificationevent.h"

CThreadIdentificationEvent::CThreadIdentificationEvent(const CUTraceFrame& aFrame, MEventOutputHandler& aOutputHandler)
    : CTraceEventBase(aFrame, aOutputHandler)
    {
    SetPrefix("EThreadIdentification");
    }

CThreadIdentificationEvent::CThreadIdentificationEvent(const CMultiPartFrameCollection& aFrameCollection, MEventOutputHandler& aOutputHandler)
    : CTraceEventBase(aFrameCollection, aOutputHandler)
    {
    SetPrefix("EThreadIdentification");
    }


void CThreadIdentificationEvent::ProcessEvent() const
    {
    EventOutputHandler().SetNewLineEnabled(true);
    CTraceEventBase::ProcessEvent();
    }

void CThreadIdentificationEvent::Describe(std::ostream& aOutput) const
    {
	int subcat = SubCategory();

	switch (subcat)
        {
        case BTrace::EThreadCreate:
            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Thread Create: [NThread=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] [DProcess=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data())
                << "] [Name=" << (Data() + 4)
                << "] ";
            break;
	
        case BTrace::EThreadDestroy:
            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Thread Destroy: [NThread=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] [DProcess=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data())
                << "] ";
            break;
	
        case BTrace::EThreadName:
            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Thread Name: [NThread=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] [DProcess=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data())
                << "] [Name=" << (Data() + 4)
                << "] ";
            break;
	
        case BTrace::EProcessName:
            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Process Name: [NThread=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] [DProcess=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data())
                << "] [Name=" << (Data() + 4)
                << "] ";
            break;
	
        case BTrace::EThreadId:
            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Thread Id: [NThread=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] [DProcess=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data())
                << "] [TID=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data()+4)
                << "] ";
            break;
				
        case BTrace::ENanoThreadCreate:
            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Nano Thread Create: [NThread=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] ";
	        break;

        case BTrace::ENanoThreadDestroy:
            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Nano Thread Destroy: [NThread=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] ";
	        break;
	
        case BTrace::EProcessCreate:
            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Process Create: [DProcess=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] ";
            break;
	
        case BTrace::EProcessDestroy:
            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Process Destroy: [DProcess=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] ";
            break;
	
        default:
            aOutput << "Unknown Event ";
            break;
        }
    }

