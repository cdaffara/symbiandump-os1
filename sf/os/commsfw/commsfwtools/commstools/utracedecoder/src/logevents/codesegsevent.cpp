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

#include "logevents\codesegsevent.h"
#include "e32btrace.h"

CCodeSegsEvent::CCodeSegsEvent(const CUTraceFrame& aFrame, MEventOutputHandler& aOutputHandler)
    : CTraceEventBase(aFrame, aOutputHandler)
    {
    SetPrefix("ECodeSegs");
    }

CCodeSegsEvent::CCodeSegsEvent(const CMultiPartFrameCollection& aFrameCollection, MEventOutputHandler& aOutputHandler)
    : CTraceEventBase(aFrameCollection, aOutputHandler)
    {
    SetPrefix("ECodeSegs");
    }

void CCodeSegsEvent::Describe(std::ostream& aOutput) const
    {
    int subcat = SubCategory();
	
    switch (subcat)
        {
        case BTrace::ECodeSegCreated:
            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Code Seg Create: [DCodeSeg=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] [Name="
                << (Data()) << "] ";
            break;
	
        case BTrace::ECodeSegDestroyed:
            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Code Seg Destroy: [DCodeSeg=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] ";
            break;
		
        case BTrace::ECodeSegInfo:
            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Code Seg Info: [DCodeSeg=0x" << std::setw(8) << std::setfill('0')  << Arg1()
                << "] [Attr=0x" << std::setw(8) << std::setfill('0')  << BytesToInt(Data())
                << "] [CodeSectionBase=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data()+4)
                << "] [CodeSectionSize=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data()+8)
                << "] [ConstDataSectionBase=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data()+12)
                << "] [ConstDataSectionSize=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data()+16)
                << "] [InitialisedDataSectionBase=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data()+20)
                << "] [InitialisedDataSectionSize=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data()+24)
                << "] [UninitialisedDataSectionBase=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data()+28)
                << "] [UninitialisedDataSectionSize=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data()+32)
                << "] ";
                break;
	
        case BTrace::ECodeSegMapped:
            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Code Seg Mapped: [DCodeSeg=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] [DProcess=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data())
                << "] ";
            break;

        case BTrace::ECodeSegUnmapped:
            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Code Seg Unmapped: [DCodeSeg=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] [DProcess=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data())
                << "] ";
            break;
	
        case BTrace::ECodeSegMemoryAllocated:
            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Code Seg Memory Allocated: [DCodeSeg=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] [Size=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data())
                << "] ";
            break;
	
        case BTrace::ECodeSegMemoryDeallocated:
            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Code Seg Memory Deallocated: [DCodeSeg=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] [Size=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data())
                << "] ";
            break;

        default:
            aOutput << "Unknown Event ";
            break;
        }
    }

