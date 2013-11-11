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

#include "utraceframe.h"
#include "logevents\traceeventfactory.h"
#include "logevents\codesegsevent.h"
#include "logevents\kernelmemoryevent.h"
#include "logevents\clientserverevent.h"
#include "logevents\threadidentificationevent.h"
#include "logevents\chunksevent.h"
#include "logevents\simplelogevent.h"
#include "logevents\nodemessageevent.h"
#include "logevents\rdebuglogevent.h"
#include "logevents\metatraceevent.h"

CTraceEventBase* CTraceEventFactory::CreateTraceEvent(const CUTraceFrame& aFrame, MEventOutputHandler &aOutputEventHandler)
    {
    CTraceEventBase* newLogEvent = NULL;

    switch (aFrame.PrimaryFilter())
        {
       case BTrace::ECodeSegs:
            newLogEvent = new CCodeSegsEvent(aFrame, aOutputEventHandler);
            break;

        case BTrace::EChunks:
            newLogEvent = new CChunksEvent(aFrame, aOutputEventHandler);
            break;

        case BTrace::EThreadIdentification:
            newLogEvent = new CThreadIdentificationEvent(aFrame, aOutputEventHandler);
            break;

        case BTrace::EKernelMemory:
            newLogEvent = new CKernelMemoryEvent(aFrame, aOutputEventHandler);
            break;

        case BTrace::EClientServer:
            newLogEvent = new CClientServerEvent(aFrame, aOutputEventHandler);
            break;

		case BTrace::EMetaTrace:
            newLogEvent = new CMetaTraceEvent(aFrame, aOutputEventHandler);
            break;

        case ECustomMMF1:
            newLogEvent = new CSimpleLogEvent("ECustomMMF1", aFrame, aOutputEventHandler);
            break;

        case EMeshMachine:
            newLogEvent = new CSimpleLogEvent("EMeshMachine", aFrame, aOutputEventHandler);
            break;

        case ENodeMessages:
            newLogEvent = new CNodeMessageEvent(aFrame, aOutputEventHandler);
            break;

        case ESimpleLogging:
            newLogEvent = new CSimpleLogEvent(aFrame, aOutputEventHandler);
            break;

        case BTrace::ERDebugPrintf:
            newLogEvent = new CRDebugLogEvent(aFrame, aOutputEventHandler);
            break;

        case BTrace::EKernPrintf:
            newLogEvent = new CRDebugLogEvent("EKernPrintf", aFrame, aOutputEventHandler);
            break;

        case BTrace::EPlatsecPrintf:
            newLogEvent = new CRDebugLogEvent("EPlatsecPrintf", aFrame, aOutputEventHandler);
            break;
        }    

    return newLogEvent;
    }


CTraceEventBase* CTraceEventFactory::CreateTraceEvent(const CMultiPartFrameCollection& aFrameCollection, MEventOutputHandler &aOutputEventHandler)
    {
    CTraceEventBase* newLogEvent = NULL;
    CUTraceFrame* frame;

    frame = aFrameCollection.at(0);
    switch (frame->PrimaryFilter())
        {
       case BTrace::ECodeSegs:
            newLogEvent = new CCodeSegsEvent(aFrameCollection, aOutputEventHandler);
            break;

        case BTrace::EChunks:
            newLogEvent = new CChunksEvent(aFrameCollection, aOutputEventHandler);
            break;

        case BTrace::EThreadIdentification:
            newLogEvent = new CThreadIdentificationEvent(aFrameCollection, aOutputEventHandler);
            break;

        case BTrace::EKernelMemory:
            newLogEvent = new CKernelMemoryEvent(aFrameCollection, aOutputEventHandler);
            break;

        case ECustomMMF1:
            newLogEvent = new CSimpleLogEvent("ECustomMMF1", aFrameCollection, aOutputEventHandler);
            break;

        case EMeshMachine:
            newLogEvent = new CSimpleLogEvent("EMeshMachine", aFrameCollection, aOutputEventHandler);
            break;

        case ENodeMessages:
            newLogEvent = new CNodeMessageEvent(aFrameCollection, aOutputEventHandler);
            break;

        case ESimpleLogging:
            newLogEvent = new CSimpleLogEvent(aFrameCollection, aOutputEventHandler);
            break;

        case BTrace::ERDebugPrintf:
            newLogEvent = new CRDebugLogEvent(aFrameCollection, aOutputEventHandler);
            break;

        case BTrace::EKernPrintf:
            newLogEvent = new CRDebugLogEvent("EKernPrintf", aFrameCollection, aOutputEventHandler);
            break;

        case BTrace::EPlatsecPrintf:
            newLogEvent = new CRDebugLogEvent("EPlatsecPrintf", aFrameCollection, aOutputEventHandler);
            break;
        }

    return newLogEvent;
    }

