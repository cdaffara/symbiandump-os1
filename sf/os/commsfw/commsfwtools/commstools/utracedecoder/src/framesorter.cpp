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

#include <cassert>
#include <iostream>
#include <iomanip>

#include "globals.h"
#include "utraceframe.h"
#include "framesorter.h"
#include "eventoutputhandler.h"
#include "logevents\traceeventfactory.h"
#include "logevents\traceeventbase.h"

CFrameSorter::CFrameSorter(MEventOutputHandler& aOutputHandler)
    : iOutputHandler(aOutputHandler),
    iDumpFirstEvent(-1), iDumpLastEvent(-1),
    iDumpFirstFrame(-1), iDumpLastFrame(-1)
    {
    }


CFrameSorter::~CFrameSorter()
    {
    while (!iFragmentedTraceEvents.empty())
        {
        delete iFragmentedTraceEvents.front();
        iFragmentedTraceEvents.pop_front();
        }

    while (!iHeldTraceEvents.empty())
        {
        delete iHeldTraceEvents.front();
        iHeldTraceEvents.pop_front();
        }
    }


void CFrameSorter::ProcessFrame(CUTraceFrame* aFrame)
    {
    MaybeDumpFrame(*aFrame);

	if (aFrame->Flags() & BTrace::ERecordTruncated)
		{
        std::cerr << "Frame " << std::dec << iOutputHandler.CurrentFrame() << " frame was oversized and truncation occured - skipping unusable frame" << std::endl;
        delete aFrame;
        return;
		}

	if (aFrame->Flags() & BTrace::EMissingRecord)
		{
		std::cerr << "Frame " << std::dec << iOutputHandler.CurrentFrame() << " indicates missing frames - this can happen if the trace buffer was full" << std::endl;
		}

    if (aFrame->IsMultiPart())
        {
        // Multi Part frames will either be the first frame in a new collection
        // or be added to the collection for an existing incomplete multi part event
        // The unique multi part sequence number is used to identify the collection

        CMultiPartFrameCollection* frameCollection;
        if (aFrame->MultiPartType() == BTrace::EMultiPartFirst)
            {
            // A new event
            frameCollection = new CMultiPartFrameCollection(aFrame);
            if (!frameCollection)
                {
                std::cerr << "Frame " << std::dec << iOutputHandler.CurrentFrame() << " Error creating multipart frame collection" << std::endl;
                delete aFrame;
                return;
                }

            iFragmentedTraceEvents.push_back(frameCollection);
            }
        else
            {
            // An existing incomplete event
            unsigned int partIndex;
            CMultiPartFrameCollection* parts = NULL;
            for (partIndex=0; partIndex<iFragmentedTraceEvents.size(); partIndex++)
                {
                if (iFragmentedTraceEvents.at(partIndex)->MultiPartId() == aFrame->MultiPartSequenceNumber())
                    {
                    parts = iFragmentedTraceEvents.at(partIndex);
                    break;
                    }
                }

            if (!parts)
                {
                // Didn't find a matching collection for a middle or last multipart frame
                std::cerr << "Frame " << std::dec << iOutputHandler.CurrentFrame() << " Unknown or corrupt trace event" << std::endl;
                aFrame->DumpFrame(std::cerr);
                delete aFrame;
                return;
                }

            parts->AddFrame(aFrame);
            if (aFrame->MultiPartType() == BTrace::EMultiPartLast)
                {
                // We got the last part of an event, we try to construct an actual event from the available fragments
                // this can fail for OOM, if any of the parts are missing, or if the factory doesn't know about this
                // type of event
                CTraceEventBase* traceEvent = CTraceEventFactory::CreateTraceEvent(*parts, iOutputHandler);
                iFragmentedTraceEvents.erase(iFragmentedTraceEvents.begin() + partIndex);
                if (!traceEvent)
                    {
                    std::cerr << "Frame " << std::dec << iOutputHandler.CurrentFrame() << " Unknown or corrupt trace event" << std::endl;
                    
                    unsigned int i;
                    for (i=0; i<parts->size(); i++)
                        {
                        std::cerr << "Multi Part Frame Number: " << std::dec << i << " of " << std::dec << parts->size() << std::endl;
                        parts->at(i)->DumpFrame(std::cerr);
                        }
                    std::cerr << std::endl;

                    delete parts;
                    DrainEventQueue();
                    return;
                    }

                delete parts;

                MaybeDumpEvent(*traceEvent);

                // Now we have a defragmented log event insert it into the held events queue
                unsigned int i;
                for (i=0; i<iHeldTraceEvents.size(); i++)
                    {
                    if (iHeldTraceEvents.at(i)->Timestamp1() > traceEvent->Timestamp1())
                        {
                        iHeldTraceEvents.insert(iHeldTraceEvents.begin() + i, traceEvent);
                        break;
                        }
                    }

                if (i == iHeldTraceEvents.size())
                    {
                    iHeldTraceEvents.push_back(traceEvent);
                    }

                DrainEventQueue();
                }
            }
        }
    else
        {
        CTraceEventBase* traceEvent = CTraceEventFactory::CreateTraceEvent(*aFrame, iOutputHandler);
        if (!traceEvent)
            {
			if (!::gQuiet)
				{
				std::cerr << "Frame " << std::dec << iOutputHandler.CurrentFrame() << " Unknown trace event" << std::endl;
				aFrame->DumpFrame(std::cerr);
				}
            delete aFrame;
            return;
            }
        delete aFrame;

        MaybeDumpEvent(*traceEvent);

        // This frame isn't a multi part frame - we can process it immediately
        // so long as there are no other queued logs to process
        if (iFragmentedTraceEvents.empty())
            {
            // If there are no fragmented log events waiting for more parts
            // there should be no other log events being held
            assert(iHeldTraceEvents.empty());

            traceEvent->ProcessEvent();
            delete traceEvent;
            }
        else
            {
            // Fragmented events exist, queue the event until the other parts for preceding events show up
            iHeldTraceEvents.push_back(traceEvent);
            }
        }
    }

void CFrameSorter::DrainEventQueue()
    {
    if (iFragmentedTraceEvents.empty())
        {
        while (!iHeldTraceEvents.empty())
            {
            CTraceEventBase* traceEvent = iHeldTraceEvents.front();
            iHeldTraceEvents.pop_front();

            traceEvent->ProcessEvent();
            delete traceEvent;
            }
        }
    }

void CFrameSorter::MaybeDumpFrame(const CUTraceFrame& aFrame) const
    {
    if (iDumpFirstFrame >= 0 && iDumpLastFrame >= 0
        && (int)aFrame.EventEntryNumber() >= iDumpFirstFrame
        && (int)aFrame.EventEntryNumber() <= iDumpLastFrame)
        {
        std::cerr << "Dumping Frame " << std::dec << aFrame.EventEntryNumber() << std::endl;
        aFrame.DumpFrame(std::cerr);
        }
    }

void CFrameSorter::MaybeDumpEvent(const CTraceEventBase& aEvent) const
    {
    if (iDumpFirstEvent >= 0 && iDumpLastEvent >= 0
        && (int)aEvent.EventEntryNumber() >= iDumpFirstEvent
        && (int)aEvent.EventEntryNumber() <= iDumpLastEvent)
        {
        std::cerr << "Dumping Event " << std::dec << aEvent.EventEntryNumber() << std::endl;
        aEvent.DumpEvent(std::cerr);
        }
    }

