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

#ifndef EVENTOUTPUTHANDLER_H
#define EVENTOUTPUTHANDLER_H

#include <vector>

class CTraceEventBase;

class MEventOutputHandler
    {
    public:
        inline bool ShowContextInfo() const
            {
            return iShowContextInfo;
            }

        inline bool SetShowContextInfo(bool aShowContextInfo)
            {
            bool lastValue = iShowContextInfo;
            iShowContextInfo = aShowContextInfo;
            return lastValue;
            }

        inline bool NewLineEnabled() const
            {
			return iNewLineEnabledStack.size()? iNewLineEnabledStack.back(): false;
            }

        inline void PushNewLineEnabled(bool aNewLineEnabled)
            {
            iNewLineEnabledStack.push_back(aNewLineEnabled);
            }

        inline void PopNewLineEnabled()
            {
            iNewLineEnabledStack.pop_back();
            }

        inline void SetNewLineEnabled(bool aNewLineEnabled)
            {
            while (!iNewLineEnabledStack.empty())
                {
                iNewLineEnabledStack.pop_back();
                }
            iNewLineEnabledStack.push_back(aNewLineEnabled);
            }

        virtual unsigned int CurrentFrame() const = 0;
        virtual void WriteEvent(const CTraceEventBase& aEvent) const = 0;

    private:
        std::vector<bool> iNewLineEnabledStack;
        bool iShowContextInfo;
    };

#endif
// EVENTOUTPUTHANDLER_H

