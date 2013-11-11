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

#ifndef NODEMESSAGEEVENT_H
#define NODEMESSAGEEVENT_H

#include "simplelogevent.h"

class CNodeMessageEvent : public CSimpleLogEvent
    {
    friend class CTraceEventFactory;

    protected:
        enum TLogEntryType
            {
            ELogBinary,
            ELogInfo,
            ELogBlockStart,
            ELogBlockEnd,
            ELogNodeCreate,
            ELogNodeDestroy,
            ELogAddress,
            ELogMessage,
            ELogNode,
            };

        explicit CNodeMessageEvent(const CUTraceFrame& aFrame, MEventOutputHandler& aOutputEventHandler);
        explicit CNodeMessageEvent(const CMultiPartFrameCollection& aFrameCollection, MEventOutputHandler& aOutputEventHandler);

    public:
        //virtual void ProcessEvent() const;
        virtual void Describe(std::ostream& aOutput) const;
    };


#endif
// NODEMESSAGEEVENT_H

