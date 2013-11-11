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
#include <sstream>
#include <iomanip>

#include "utraceframe.h"
#include "e32btrace.h"
#include "util.h"

CUTraceFrame::CUTraceFrame(const unsigned char* aFrameData, unsigned int aEntryNumber)
    {
    SetFrameBuffer(aFrameData);
    iEventEntryNumber = aEntryNumber;
    }

CUTraceFrame::~CUTraceFrame()
    {
    delete [] iFrameBuffer;
    }


void CUTraceFrame::SetFrameBuffer(const unsigned char *aData)
    {
	int length = aData[0];
	iFrameBuffer = new unsigned char[length + 4];
	memset(iFrameBuffer, 0, length + 4); //need to do this so payload() doesnt blow out when used as string
	memcpy((char *)iFrameBuffer, (const char*)aData, length);
    }


unsigned char* CUTraceFrame::Data() const
    {
    // Get a pointer to the data after the header
	unsigned char *p = (unsigned char*)(iFrameBuffer + CalculateEntryOffset(0));

    // Make adjustments for any extra fields such as multipart information
    // and the arg1 value
    if (IsMultiPart())
        {
        /*
	    The structure of the data part of each trace record in a multipart trace is described
	    below. In this description, the following labels are used.
	    -	A is the initial 4 bytes of data; the a1 argument of BTraceBig.
	    -	D is the array of bytes of additional data; the aData argument of BTraceBig.
	    -	N is the size of D; the aDataSize argument of BTraceBig
	    -	X is the maximum number of additional bytes which will fit into a trace record. 
		    This is usually KMaxBTraceDataArray but this should not be assumed, instead
		    the size and other information present in each trace record should be examined.

	    For the first part of a multipart trace, the data in a trace record has the following
	    structure:
	    -	4 bytes containing N.
	    -	4 bytes containing A.
	    -	X bytes containing D[0..X-1]
    
	    If the parts are numbered 0 through to 'j', then a middle part of a multipart trace
	    is numbered 'i' where 0<i<j. The data in these parts has the structure:
	    -	4 bytes containing N.
	    -	4 bytes containing X*i. I.e. the offset within D for the data in this trace record.
	    -	X bytes containing D[X*i..X*i+X-1]
	
	    For the last part of a multipart trace, the data has the structure:
	    -	4 bytes containing N.
	    -	4 bytes containing X*j. I.e. the offset within D for the data in this trace record.
	    -	N modulo X bytes containing D[X*j..N-1]. I.e. the final bytes of the trace data.*/
        
        p += 8; // trace arg1 and the offset to where this piece of data should go
        }
    else
        {
        p += 4; // trace arg1
        }

    return p;
    }


unsigned int CUTraceFrame::EventEntryNumber() const
    {
    return iEventEntryNumber;
    }

unsigned int CUTraceFrame::DataLength() const
    {
    unsigned char* data = Data();
    unsigned int dataLength = (unsigned int)((iFrameBuffer + FrameLength()) - data);
    return dataLength;
    }

unsigned int CUTraceFrame::DataFullLength() const
    {
    if (MultiPartType() == BTrace::EMultiPartFirst)
        {
	    unsigned char* p = (unsigned char*)(iFrameBuffer + CalculateEntryOffset(0));
        return *((unsigned int*)(p));
        }
    else if (MultiPartType() == BTrace::ENotMultiPart)
        {
        return DataLength();
        }

#ifdef _DEBUG
    assert(0); // called out of context
#endif
    std::cerr << "Error: CUTraceFrame::DataFullLength() called out of context" << std::endl;
    return 0;
    }


unsigned int CUTraceFrame::FrameLength() const
    {
    return unsigned int(iFrameBuffer[BTrace::ESizeIndex]);
    }


unsigned int CUTraceFrame::Flags() const
    {
    return int(iFrameBuffer[BTrace::EFlagsIndex]);
    }


unsigned int CUTraceFrame::PrimaryFilter() const
    {
    return int(iFrameBuffer[BTrace::ECategoryIndex]);
    }


unsigned int CUTraceFrame::SecondaryFilter() const
    {
    // TODO:
    std::cerr << "Error: CUTraceFrame::SecondaryFilter() not implemented" << std::endl;
    assert(0);
	return 0;
    }


unsigned int CUTraceFrame::SubCategory() const
    {
    return iFrameBuffer[BTrace::ESubCategoryIndex];
    }


bool CUTraceFrame::Header2Present() const
    {
    return iFrameBuffer[BTrace::EFlagsIndex] & BTrace::EHeader2Present;
    }

bool CUTraceFrame::Timestamp1Present() const
    {
    return (iFrameBuffer[BTrace::EFlagsIndex] & BTrace::ETimestampPresent) == BTrace::ETimestampPresent;
    }

bool CUTraceFrame::Timestamp2Present() const
    {
    return (iFrameBuffer[BTrace::EFlagsIndex] & BTrace::ETimestamp2Present) == BTrace::ETimestamp2Present;
    }

bool CUTraceFrame::ContextIdPresent() const
    {
    return (iFrameBuffer[BTrace::EFlagsIndex] & BTrace::EContextIdPresent) == BTrace::EContextIdPresent;
    }

bool CUTraceFrame::PcPresent() const
    {
    return (iFrameBuffer[BTrace::EFlagsIndex] & BTrace::EPcPresent) == BTrace::EPcPresent;
    }

bool CUTraceFrame::ExtraPresent() const
    {
    return (iFrameBuffer[BTrace::EFlagsIndex] & BTrace::EExtraPresent) == BTrace::EExtraPresent;
    }

bool CUTraceFrame::RecordTruncated() const
    {
    return (iFrameBuffer[BTrace::EFlagsIndex] & BTrace::ERecordTruncated) == BTrace::ERecordTruncated;
    }

bool CUTraceFrame::MissingRecord() const
    {
    return (iFrameBuffer[BTrace::EFlagsIndex] & BTrace::EMissingRecord) == BTrace::EMissingRecord;
    }


unsigned int CUTraceFrame::Header2() const
    {
    if (Header2Present())
        {
        return BytesToInt(iFrameBuffer + CalculateEntryOffset(BTrace::EHeader2Present));
	    }

#ifdef _DEBUG
    assert(0); // Test for presence
#endif
    std::cerr << "Error: CUTraceFrame::Header2() called out of context" << std::endl;
    return 0;
    }


unsigned int CUTraceFrame::Timestamp1() const
    {
	if (Timestamp1Present())
        {
        return BytesToInt(iFrameBuffer  + CalculateEntryOffset(BTrace::ETimestampPresent));
	    }

#ifdef _DEBUG
    assert(0); // Test for presence
#endif
    std::cerr << "Error: CUTraceFrame::Timestamp1() called out of context" << std::endl;
    return 0;
    }


unsigned int CUTraceFrame::Timestamp2() const
    {
    if (Timestamp2Present())
	    {
        return BytesToInt(iFrameBuffer  + CalculateEntryOffset(BTrace::ETimestamp2Present));
	    }

#ifdef _DEBUG
    assert(0); // Test for presence
#endif
    std::cerr << "Error: CUTraceFrame::Timestamp2() called out of context" << std::endl;
    return 0;
    }


unsigned int CUTraceFrame::ContextType() const
    {
    if (ContextIdPresent())
        {
        return BytesToInt(iFrameBuffer  + CalculateEntryOffset(BTrace::EContextIdPresent)) & (3UL);
        }

    #ifdef _DEBUG
    assert(0); // Test for presence
#endif
    std::cerr << "Error: CUTraceFrame::ContextType() called out of context" << std::endl;
    return 0;
    }


unsigned int CUTraceFrame::ContextId() const
    {
    if (ContextIdPresent())
	    {
        return BytesToInt(iFrameBuffer  + CalculateEntryOffset(BTrace::EContextIdPresent)) & (~3UL);
	    }

#ifdef _DEBUG
    assert(0); // Test for presence
#endif
    std::cerr << "Error: CUTraceFrame::ContextId() called out of context" << std::endl;
    return 0;
    }


unsigned int CUTraceFrame::ProgramCounter() const
    {
    if (PcPresent())
        {
        return BytesToInt(iFrameBuffer  + CalculateEntryOffset(BTrace::EPcPresent));
	    }

#ifdef _DEBUG
    assert(0); // Test for presence
#endif
    std::cerr << "Error: CUTraceFrame::ProgramCounter() called out of context" << std::endl;
    return 0;
    }


unsigned int CUTraceFrame::Extra() const
    {
    if (ExtraPresent())
	    {
        return BytesToInt(iFrameBuffer  + CalculateEntryOffset(BTrace::EExtraPresent));
	    }

#ifdef _DEBUG
    assert(0); // Test for presence
#endif
    std::cerr << "Error: CUTraceFrame::Extra() called out of context" << std::endl;
    return 0;
    }


BTrace::TMultiPart CUTraceFrame::MultiPartType() const
    {
    if (Header2Present())
        {
        return (BTrace::TMultiPart)(Header2() & BTrace::EMultiPartFlagMask);
        }

    return BTrace::ENotMultiPart;
    }


unsigned int CUTraceFrame::MultiPartSequenceNumber() const
    {
    if (MultiPartType() == BTrace::ENotMultiPart)
        {
#ifdef _DEBUG
        assert(0); // called out of context
#endif
        std::cerr << "Error: CUTraceFrame::MultiPartSequenceNumber() called out of context" << std::endl;
        return 0;
        }

    return Extra();
    }


unsigned int CUTraceFrame::Arg1() const
    {
    BTrace::TMultiPart partType = MultiPartType();

    assert(partType == BTrace::ENotMultiPart || partType ==  BTrace::EMultiPartFirst);
    return *((unsigned int*)(Data() - 4)); // Arg1 always 4 bytes before the start of the data
    }


unsigned int CUTraceFrame::Truncated() const
    {
    if (RecordTruncated())
        {
		return BytesToInt(iFrameBuffer  + CalculateEntryOffset(BTrace::ERecordTruncated));
	    }

#ifdef _DEBUG
    assert(0); // Test for presence
#endif
    return 0;
    }


unsigned int CUTraceFrame::Missing() const
    {
    if (MissingRecord())
        {
    	return BytesToInt(iFrameBuffer  + CalculateEntryOffset(BTrace::EMissingRecord));
	    }
    
#ifdef _DEBUG
    assert(0); // Test for presence
#endif
    return 0;
    }


/**
Calculates the offset from the start of the frame to either the data after the header,
or to the data for the specified flag.

@param aEntry - Either a value from BTrace::TFlags or zero to retrieve the offset to the data after the header.
The requested flag must be set in the flags field of the frame.
The requested flag must not be BTrace::ERecordTruncated or BTrace::EMissingRecord which have no associated data
*/
unsigned int CUTraceFrame::CalculateEntryOffset(unsigned char aEntry) const
    {
	assert(aEntry == 0 || (aEntry & Flags()) > 0);
	assert(aEntry != BTrace::ERecordTruncated);
	assert(aEntry != BTrace::EMissingRecord);

    int offset = 4; // size, flags, 1ry category, subcategory
    unsigned char mask = aEntry - 1;
	mask &= ~(BTrace::ERecordTruncated | BTrace::EMissingRecord);

    unsigned char n = Flags() & mask;    
	while (n)
	    {
		offset += ((n & 1UL) * 4) ;
		n >>= 1 ;
	    }

	return offset;
    }


void CUTraceFrame::DumpFlags(std::ostream& aStream) const
    {
    if (Flags() & BTrace::EHeader2Present)
        {
        aStream << "Header2Present ";
        }

    if (Flags() & BTrace::ETimestampPresent)
        {
        aStream << "TimestampPresent ";
        }

    if (Flags() & BTrace::ETimestamp2Present)
        {
        aStream << "Timestamp2Present ";
        }

    if (Flags() & BTrace::EContextIdPresent)
        {
        aStream << "ContextIdPresent ";
        }

    if (Flags() & BTrace::EPcPresent)
        {
        aStream << "PcPresent ";
        }

    if (Flags() & BTrace::EExtraPresent)
        {
        aStream << "ExtraPresent ";
        }

    if (Flags() & BTrace::ERecordTruncated)
        {
        aStream << "RecordTruncated ";
        }

    if (Flags() & BTrace::EMissingRecord)
        {
        aStream << "MissingRecord ";
        }
    }

void CUTraceFrame::DumpFrame(std::ostream& aStream) const
    {
    aStream
        << "Size: " << std::dec << FrameLength() << std::endl
        << "Flags: 0x" << std::noshowbase << std::setw(2)
        << std::setfill('0') << std::nouppercase << std::hex
        << Flags() << " (";

    DumpFlags(aStream);

    aStream << ")" << std::endl
        << "Primary Filter: 0x" << std::noshowbase << std::setw(2)
        << std::setfill('0') << std::nouppercase << std::hex
        << PrimaryFilter() << std::endl
        << "Subcategory: 0x" << std::noshowbase << std::setw(2)
        << std::setfill('0') << std::nouppercase << std::hex
        << SubCategory() << std::endl;

    if (Header2Present())
        {
        aStream << "Header2: 0x" << std::noshowbase << std::setw(8)
            << std::setfill('0') << std::nouppercase << std::hex
            << Header2() << std::endl;
        }

    if (Timestamp1Present())
        {
        aStream << "Timestamp1: " << std::noshowbase << std::dec
            << Timestamp1() << std::endl;
        }

    if (Timestamp2Present())
        {
        aStream << "Timestamp2: " << std::noshowbase << std::dec
            << Timestamp2() << std::endl;
        }
    if (ContextIdPresent())
        {
        aStream << "Context Type: ";
        switch (ContextType())
            {
            case 0:
                aStream << "NThread" << std::endl;
                aStream << "Context ID: 0x" << std::noshowbase << std::hex << std::nouppercase
                    << std::setw(8) << std::setfill('0') << ContextId() << std::endl;
                break;

            case 1:
                aStream << "Fast Interrupt (FIQ)" << std::endl;
                break;

            case 2:
                aStream << "Interrupt (IRQ)" << std::endl;                    
                break;

            case 3:
                aStream << "Immediate Delayed Function Call (IDFC)" << std::endl;
                break;
            }
        }

    if (PcPresent())
        {
        aStream << "PC: 0x" << std::noshowbase << std::setw(8)
            << std::setfill('0') << std::nouppercase << std::hex
            << ProgramCounter() << std::endl;
        }

    if (IsMultiPart())
        {
        aStream << "Multipart Frame: ";
        switch (Header2() & BTrace::EMultiPartFlagMask)
            {
            case 1:
                aStream << "EMultiPartFirst";
                break;

            case 2:
                aStream << "EMultiPartMiddle";
                break;

            case 3:
                aStream << "EMultiPartLast";
                break;
            }
        aStream << std::endl;
        }

    if (ExtraPresent())
        {
        if (IsMultiPart())
            {
            aStream << "MultiPart Event Id: 0x";
            }
        else
            {
            aStream << "Extra: 0x";
            }

        aStream << std::noshowbase << std::setw(8)
            << std::setfill('0') << std::nouppercase << std::hex
            << Extra() << std::endl;
        }

    if (!IsMultiPart() || MultiPartType() == BTrace::EMultiPartFirst)
        {
        aStream << "Arg1: 0x" << std::noshowbase << std::setw(8)
            << std::setfill('0') << std::nouppercase << std::hex
            << Arg1() << std::endl;
        }

    unsigned int zones[1] = { (unsigned int)(Data() - iFrameBuffer) };
    DumpBytes(aStream, iFrameBuffer, FrameLength(), zones, 1);
    aStream << std::endl;
    aStream.flush();
    }


void CUTraceFrame::DumpFrame(std::string& aOutput) const
    {
    std::ostringstream ss;
    DumpFrame(ss);
    aOutput += ss.str();
    }


CMultiPartFrameCollection::CMultiPartFrameCollection(CUTraceFrame* aFrame)
    {
    assert(aFrame->IsMultiPart());
    push_back(aFrame);
    }

CMultiPartFrameCollection::~CMultiPartFrameCollection()
    {
    while (!empty())
        {
        CUTraceFrame* frame = this->back();
        this->pop_back();
        delete frame;
        }
    }

void CMultiPartFrameCollection::AddFrame(CUTraceFrame* aFrame)
    {
    assert(aFrame->IsMultiPart() && aFrame->MultiPartSequenceNumber() == MultiPartId());
    push_back(aFrame);
    }

unsigned int CMultiPartFrameCollection::MultiPartId() const
    {
    return at(0)->MultiPartSequenceNumber();
    }

