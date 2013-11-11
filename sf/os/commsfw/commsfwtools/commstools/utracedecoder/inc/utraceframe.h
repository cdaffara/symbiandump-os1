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

#ifndef UTRACEFRAME_H
#define UTRACEFRAME_H

#include <vector>

#include "e32btrace.h"
#include "logevents\traceevent.h"

class CUTraceFrame : public MTraceEvent
    {
    public:
        explicit CUTraceFrame(const unsigned char* aFrameData, unsigned int aEntryNumber);
        virtual ~CUTraceFrame();

        void DumpFrame(std::ostream& aStream) const;
        void DumpFrame(std::string& aOutput) const;

        // Length of this btrace frame
        virtual unsigned int FrameLength() const;

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
        virtual bool RecordTruncated() const;
        virtual bool MissingRecord() const;

        // These return values from the header if they are present
        virtual unsigned int Header2() const;
        virtual unsigned int Timestamp1() const;
        virtual unsigned int Timestamp2() const;
        virtual unsigned int ContextType() const;
        virtual unsigned int ContextId() const;
        virtual unsigned int ProgramCounter() const;
        virtual unsigned int Extra() const;
        virtual unsigned int Truncated() const;
        virtual unsigned int Missing() const;

        virtual unsigned int Arg1() const;
        virtual unsigned char* Data() const;
        virtual unsigned int DataLength() const;
        
        virtual unsigned int EventEntryNumber() const;
        
        unsigned int DataFullLength() const;

        // MultiPart log methods
        // MultiPartSequenceNumber() will assert if it the frame is not a multipart
        BTrace::TMultiPart MultiPartType() const;
        unsigned int MultiPartSequenceNumber() const;
        inline bool IsMultiPart() const
            {
            return MultiPartType() != BTrace::ENotMultiPart;
            }


    private:
        unsigned int CalculateEntryOffset(unsigned char aEntry) const;
        void SetFrameBuffer(const unsigned char *aData);
        void DumpFlags(std::ostream& aStream) const;

    private:
	    unsigned char* iFrameBuffer; //a buffer to store a single frame
        unsigned int iEventEntryNumber;
    };


class CMultiPartFrameCollection : public std::vector<CUTraceFrame*>
    {
    public:
        explicit CMultiPartFrameCollection(CUTraceFrame* aFrame);
        ~CMultiPartFrameCollection();

        unsigned int MultiPartId() const;
        void AddFrame(CUTraceFrame* aFrame);
    };

#endif
// UTRACEFRAME_H

