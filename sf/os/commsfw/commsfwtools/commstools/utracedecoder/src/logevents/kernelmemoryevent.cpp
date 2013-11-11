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

#include "logevents\kernelmemoryevent.h"
#include "e32btrace.h"

CKernelMemoryEvent::CKernelMemoryEvent(const CUTraceFrame& aFrame, MEventOutputHandler& aOutputHandler)
    : CTraceEventBase(aFrame, aOutputHandler)
    {
    SetPrefix("EKernelMemory");
    }

CKernelMemoryEvent::CKernelMemoryEvent(const CMultiPartFrameCollection& aFrameCollection, MEventOutputHandler& aOutputHandler)
    : CTraceEventBase(aFrameCollection, aOutputHandler)
    {
    SetPrefix("EKernelMemory");
    }

void CKernelMemoryEvent::Describe(std::ostream& aOutput) const
    {
    int subcat = SubCategory();

    switch (subcat)
        {
        case BTrace::EKernelMemoryInitialFree:
            aOutput << std::noshowbase << std::nouppercase << std::hex 
                << "Kernel Memory Initial Free: [Size=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] ";
            break;

        case BTrace::EKernelMemoryCurrentFree:
            aOutput << std::noshowbase << std::nouppercase << std::hex 
                << "Kernel Memory Current Free: [Size=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] ";
            break;
        
        case BTrace::EKernelMemoryMiscAlloc:
            aOutput << std::noshowbase << std::nouppercase << std::hex 
                << "Kernel Memory Misc Alloc: [Size=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] ";
            break;
    
        case BTrace::EKernelMemoryMiscFree:
            aOutput << std::noshowbase << std::nouppercase << std::hex 
                << "Kernel Memory Misc Free: [Size=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] ";
            break;
    
        case BTrace::EKernelMemoryDemandPagingCache:
            aOutput << std::noshowbase << std::nouppercase << std::hex 
                << "Kernel Memory Demand Paging Cache: [MinSize=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] ";
            break;
    
        case BTrace::EKernelMemoryDrvPhysAlloc:
		    // NB: The prime function logs a EKernelMemoryDrvPhysAlloc record where the physical 
		    // address is -1 and should be ignored.
            if ((int)BytesToInt(Data()) != -1)
                {
                aOutput << std::noshowbase << std::nouppercase << std::hex 
                    << "Kernel Memory Driver Physical Alloc: [Size=0x" << std::setw(8) << std::setfill('0') << Arg1()
                    << "] [PhysicalBase=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data())
                    << "] ";
                }
            break;
    
        case BTrace::EKernelMemoryDrvPhysFree:
            aOutput << std::noshowbase << std::nouppercase << std::hex 
                << "Kernel Memory Driver Physical Free: [Size=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] [PhysicalBase=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data())
                << "] ";
            break;    
        
        default:
            aOutput << "Unknown Event ";
            break;			
        }
    }

