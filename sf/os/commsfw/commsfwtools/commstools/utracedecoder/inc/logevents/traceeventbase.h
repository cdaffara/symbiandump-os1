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

#ifndef TRACEEVENTBASE_H
#define TRACEEVENTBASE_H

#include <string>
#include "traceevent.h"

class CTraceEventFactory;
class MEventOutputHandler;
class CMultiPartFrameCollection;
class CUTraceFrame;

class CTraceEventBase : public MTraceEvent
    {
    friend class CTraceEventFactory;

    protected:
        CTraceEventBase(const CMultiPartFrameCollection& aFrames, MEventOutputHandler& aOutputHandler);
        CTraceEventBase(const CUTraceFrame& aFrame, MEventOutputHandler& aOutputHandler);

    public:
        virtual ~CTraceEventBase();

    // From MLogEvent
    public:
        // Flags from the frame header
        virtual unsigned int Flags() const;

        // Primary filter (Category) of the frame
        virtual unsigned int PrimaryFilter() const;

        // Secondary filter of the frame
        virtual unsigned int SecondaryFilter() const;

        // Subcategory of the frame (meaning dependant on the Category)
        virtual unsigned int SubCategory() const;

        // Flag accessors
        virtual bool Header2Present() const;
        virtual bool Timestamp1Present() const;
        virtual bool Timestamp2Present() const;
        virtual bool ContextIdPresent() const;
        virtual bool PcPresent() const;
        virtual bool ExtraPresent() const;

        // These return values from the header if they are present
        virtual unsigned int Header2() const;
        virtual unsigned int Timestamp1() const;
        virtual unsigned int Timestamp2() const;
        virtual unsigned int ContextType() const;
        virtual unsigned int ContextId() const;
        virtual unsigned int ProgramCounter() const;
        virtual unsigned int Extra() const;

        virtual unsigned int Arg1() const;
        virtual unsigned char* Data() const;
        virtual unsigned int DataLength() const;

        virtual unsigned int EventEntryNumber() const;

    public:
        virtual void ProcessEvent() const;
        virtual void Describe(std::ostream& aOutput) const = 0;

        void DumpEvent(std::ostream& aStream) const;
        
        void WriteContextInfo(std::ostream& aOutput) const;
        void WriteEventPrefix(std::ostream& aOutput) const;

        void SetPrefix(const std::string& aPrefix);
        void SetPrefix(const char* aPrefix);

    protected:
        inline MEventOutputHandler& EventOutputHandler() const
            {
            return iOutputHandler;
            }

        virtual void Initialise(const CUTraceFrame& aFrame);
    
    private:
        mutable MEventOutputHandler& iOutputHandler;

        unsigned int iFlags;
        unsigned int iPrimaryFilter;
        unsigned int iSecondaryFilter;
        unsigned int iContext;
        unsigned int iSubCategory;

        unsigned int iArg1;
        
        unsigned int iHeader2;
        unsigned int iTimestamp1;
        unsigned int iTimestamp2;
        unsigned int iProgramCounter;
        unsigned int iExtra;

        unsigned char* iData;
        unsigned int iDataLength;

        unsigned int iEventEntryNumber;

        std::string iPrefix;
    };

#endif
// TRACEEVENTBASE_H

