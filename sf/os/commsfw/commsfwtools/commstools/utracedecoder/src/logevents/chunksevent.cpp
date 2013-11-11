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

#include "logevents\chunksevent.h"
#include "e32btrace.h"

CChunksEvent::CChunksEvent(const CUTraceFrame& aFrame, MEventOutputHandler& aOutputHandler)
    : CTraceEventBase(aFrame, aOutputHandler)
    {
    SetPrefix("EChunks");
    }

CChunksEvent::CChunksEvent(const CMultiPartFrameCollection& aFrameCollection, MEventOutputHandler& aOutputHandler)
    : CTraceEventBase(aFrameCollection, aOutputHandler)
    {
    SetPrefix("EChunks");
    }

void CChunksEvent::Describe(std::ostream& aOutput) const
    {
	int subcat = SubCategory();
	
	switch (subcat)
        {
        case BTrace::EChunkCreated:
            aOutput << "Chunk Create: [ChunkId=0x"
                << std::noshowbase << std::setw(8) << std::setfill('0') << std::nouppercase << std::hex
                << Arg1() << "] [MaxSize=0x"
                << std::noshowbase << std::setw(8) << std::setfill('0') << std::nouppercase << std::hex
                << BytesToInt(Data()) << "] [Name="
                << (Data() + 4) << "] ";
            break;
		
        case BTrace::EChunkDestroyed:
            aOutput << "Chunk Destroy: [ChunkId=0x"
                << std::noshowbase << std::setw(8) << std::setfill('0') << std::nouppercase << std::hex
                << Arg1() << "] ";
            break;
	
        case BTrace::EChunkInfo:
            aOutput << "Chunk Info: [ChunkId=0x"
                << std::noshowbase << std::setw(8) << std::setfill('0') << std::nouppercase << std::hex
                << Arg1() << "] [Type=0x"
                << std::noshowbase << std::setw(8) << std::setfill('0') << std::nouppercase << std::hex
                << BytesToInt(Data()) << "] [Attr=0x"
                << std::noshowbase << std::setw(8) << std::setfill('0') << std::nouppercase << std::hex
                << BytesToInt(Data()+4) << "] ";
            break;
	
        case BTrace::EChunkMemoryAdded:
            aOutput << "Chunk Memory Added: [ChunkId=0x"
                << std::noshowbase << std::setw(8) << std::setfill('0') << std::nouppercase << std::hex
                << Arg1() << "] [Offset=0x"
                << std::noshowbase << std::setw(8) << std::setfill('0') << std::nouppercase << std::hex
                << BytesToInt(Data()) << "] [Size=0x"
                << std::noshowbase << std::setw(8) << std::setfill('0') << std::nouppercase << std::hex
                << BytesToInt(Data()+4) << "] ";
            break;
		
        case BTrace::EChunkMemoryAllocated:
            aOutput << "Chunk Memory Allocated: [ChunkId=0x"
                << std::noshowbase << std::setw(8) << std::setfill('0') << std::nouppercase << std::hex
                << Arg1() << "] [Offset=0x"
                << std::noshowbase << std::setw(8) << std::setfill('0') << std::nouppercase << std::hex
                << BytesToInt(Data()) << "] [Size=0x"
                << std::noshowbase << std::setw(8) << std::setfill('0') << std::nouppercase << std::hex
                << BytesToInt(Data()+4) << "] ";
            break;
		
        case BTrace::EChunkMemoryDeallocated:
            aOutput << "Chunk Memory Deallocated: [ChunkId=0x"
                << std::noshowbase << std::setw(8) << std::setfill('0') << std::nouppercase << std::hex
                << Arg1() << "] [Offset=0x"
                << std::noshowbase << std::setw(8) << std::setfill('0') << std::nouppercase << std::hex
                << BytesToInt(Data()) << "] [Size=0x"
                << std::noshowbase << std::setw(8) << std::setfill('0') << std::nouppercase << std::hex
                << BytesToInt(Data()+4) << "] ";
            break;
	
        case BTrace::EChunkMemoryRemoved:
            aOutput << "Chunk Memory Removed: [ChunkId=0x"
                << std::noshowbase << std::setw(8) << std::setfill('0') << std::nouppercase << std::hex
                << Arg1() << "] [Offset=0x"
                << std::noshowbase << std::setw(8) << std::setfill('0') << std::nouppercase << std::hex
                << BytesToInt(Data()) << "] [Size=0x"
                << std::noshowbase << std::setw(8) << std::setfill('0') << std::nouppercase << std::hex
                << BytesToInt(Data()+4) << "] ";
            break;
	
        case BTrace::EChunkOwner:
            aOutput << "Chunk Owner: [ChunkId=0x"
                << std::noshowbase << std::setw(8) << std::setfill('0') << std::nouppercase << std::hex
                << Arg1() << "] [DProcess=0x"
                << std::noshowbase << std::setw(8) << std::setfill('0') << std::nouppercase << std::hex
                << BytesToInt(Data()) << "] ";
            break;

        default:
            aOutput << "Unknown Event ";
            break;
        }
    }

