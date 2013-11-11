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


#ifndef FRAMESORTER_H
#define FRAMESORTER_H

// Disable "identifier was truncated to '255' characters" warning in VC6
#pragma warning (disable : 4786)

#include <deque>

class MEventOutputHandler;
class CUTraceFrame;
class CMultiPartFrameCollection;
class CTraceEventBase;

class CFrameSorter
    {
    public:
        explicit CFrameSorter(MEventOutputHandler& aOutputHandler);
        ~CFrameSorter();

        void ProcessFrame(CUTraceFrame* aFrame);
        inline void SetDumpEventRange(int aFirst, int aLast)
            {
            iDumpFirstEvent = aFirst;
            iDumpLastEvent = aLast;
            }
        
        inline void SetDumpFrameRange(int aFirst, int aLast)
            {
            iDumpFirstFrame = aFirst;
            iDumpLastFrame = aLast;
            }

    private:
        void DrainEventQueue();
        void CFrameSorter::MaybeDumpFrame(const CUTraceFrame& aFrame) const;
        void CFrameSorter::MaybeDumpEvent(const CTraceEventBase& aEvent) const;

    private:
        std::deque<CMultiPartFrameCollection*> iFragmentedTraceEvents;
        std::deque<CTraceEventBase*> iHeldTraceEvents;
        MEventOutputHandler& iOutputHandler;
        int iDumpFirstEvent;
        int iDumpLastEvent;
        int iDumpFirstFrame;
        int iDumpLastFrame;
    };


#endif
// FRAMESORTER_H

