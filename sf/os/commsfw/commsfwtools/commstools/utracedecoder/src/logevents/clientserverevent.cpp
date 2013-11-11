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

#include "logevents\clientserverevent.h"
#include "e32btrace.h"

CClientServerEvent::CClientServerEvent(const CUTraceFrame& aFrame, MEventOutputHandler& aOutputHandler)
    : CTraceEventBase(aFrame, aOutputHandler)
    {
    SetPrefix("EClientServer");
    }

CClientServerEvent::CClientServerEvent(const CMultiPartFrameCollection& aFrameCollection, MEventOutputHandler& aOutputHandler)
    : CTraceEventBase(aFrameCollection, aOutputHandler)
    {
    SetPrefix("EClientServer");
    }

void CClientServerEvent::Describe(std::ostream& aOutput) const
    {
    int subcat = SubCategory();

    switch (subcat)
        {
        case BTrace::EServerCreate:
            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Server Create: [DServer=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] [Name=" << (Data() + 4)
                << "] ";
            break;

        case BTrace::EServerDestroy:
            aOutput << std::noshowbase << std::nouppercase << std::hex 
                << "Server Destroy: [DServer=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] ";
            break;
        
        case BTrace::ESessionAttach:
            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Session Attach: [DSession=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] [DServer=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data())
                << "] ";
            break;
    
        case BTrace::ESessionDetach:
            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Session Detach: [DSession=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] [Reason=" << std::dec << BytesToInt(Data())
                << "] ";
            break;
    
        case BTrace::EMessageSend:
            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Message Send: [Handle=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] [Function=" << std::dec << BytesToInt(Data())
                << "] [DSession=0x" << std::hex << BytesToInt(Data() + 4)
                << "] ";
            break;
    
        case BTrace::EMessageReceive:
            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Message Receive: [Handle=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] [int0=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data())
                << "] [int3=0x" << std::setw(8) << std::setfill('0') << BytesToInt(Data() + 4)
                << "] ";
            break;
    
        case BTrace::EMessageComplete:
            aOutput << std::noshowbase << std::nouppercase << std::hex
                << "Message Complete: [Handle=0x" << std::setw(8) << std::setfill('0') << Arg1()
                << "] [Reason=" << std::dec << BytesToInt(Data())
                << "] ";
            break;    
        
        default:
            aOutput << "Unknown Event ";
            break;			
        }
    }

