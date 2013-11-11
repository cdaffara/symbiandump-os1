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

#ifndef SIMPLELOGEVENT_H
#define SIMPLELOGEVENT_H

#include <string>
#include "traceeventbase.h"

class CSimpleLogEvent : public CTraceEventBase
    {
    friend class CTraceEventFactory;

    protected:
        explicit CSimpleLogEvent(const CUTraceFrame& aFrame, MEventOutputHandler& aOutputHandler);
        explicit CSimpleLogEvent(const CMultiPartFrameCollection& aFrameCollection, MEventOutputHandler& aOutputHandler);
        explicit CSimpleLogEvent(const std::string& aPrefix, const CUTraceFrame& aFrame, MEventOutputHandler& aOutputHandler);
        explicit CSimpleLogEvent(const std::string& aPrefix, const CMultiPartFrameCollection& aFrameCollection, MEventOutputHandler& aOutputHandler);

    public:
        virtual void Describe(std::ostream& aOutput) const;
        void DescribeText(std::ostream& aOutput) const;
        void DescribeBinary(std::ostream& aOutput) const;
    };


#endif
// SIMPLELOGEVENT_H

