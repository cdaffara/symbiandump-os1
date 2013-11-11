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
#include <sstream>

#include "e32btrace.h"
#include "utraceframe.h"
#include "util.h"
#include "eventoutputhandler.h"
#include "logevents\traceeventbase.h"
#include "utracedecoderapp.h"

CTraceEventBase::CTraceEventBase(const CMultiPartFrameCollection& aFrames, MEventOutputHandler& aOutputHandler)
    : iOutputHandler(aOutputHandler)
    {
    CUTraceFrame* currentFrame = aFrames.at(0);
    Initialise(*currentFrame); 

    // Initialise copied the data from the first frame so now we continue from the next frame
    unsigned int offset = currentFrame->DataLength();
    unsigned int i;
    for (i=1; i<aFrames.size(); i++)
        {
        currentFrame = aFrames.at(i);
        assert(offset + currentFrame->DataLength() <= iDataLength);
        memcpy(iData + offset, currentFrame->Data(), currentFrame->DataLength());
        offset += currentFrame->DataLength();
        }
    }


CTraceEventBase::CTraceEventBase(const CUTraceFrame& aFrame, MEventOutputHandler& aOutputHandler)
    : iOutputHandler(aOutputHandler)
    {
    Initialise(aFrame);
    }


void CTraceEventBase::Initialise(const CUTraceFrame& aFrame)
    {
    iPrefix = "";

    iFlags = 0;
    iPrimaryFilter = 0;
    iSecondaryFilter = 0;
    iSubCategory = 0;
    iArg1 = 0;
    iHeader2 = 0;
    iTimestamp1 = 0;
    iTimestamp2 = 0;
    iContext = 0;
    iProgramCounter = 0;
    iExtra = 0;

    iData = NULL;
    iDataLength = 0;

    iData = new unsigned char[aFrame.DataFullLength() + 4];
    memset(iData, 0, aFrame.DataFullLength() + 4);
    
    iFlags = aFrame.Flags();
    iPrimaryFilter = aFrame.PrimaryFilter();
    //iSecondaryFilter = aFrame.SecondaryFilter();
    iSubCategory = aFrame.SubCategory();
    iArg1 = aFrame.Arg1();

    if (Header2Present())
        {
        iHeader2 = aFrame.Header2();
        }

    if (Timestamp1Present())
        {
        iTimestamp1 = aFrame.Timestamp1();
        }

    if (Timestamp2Present())
        {
        iTimestamp2 = aFrame.Timestamp2();
        }

    if (ContextIdPresent())
        {
        iContext = aFrame.ContextId() | aFrame.ContextType();
        }

    if (PcPresent())
        {
        iProgramCounter = aFrame.ProgramCounter();
        }

    if (ExtraPresent())
        {
        iExtra = aFrame.Extra();
        }

    iDataLength = aFrame.DataFullLength();
    
    memcpy(iData, aFrame.Data(), aFrame.DataLength());
    iEventEntryNumber = aFrame.EventEntryNumber();
    }

CTraceEventBase::~CTraceEventBase()
    {
    delete [] iData;
    }

unsigned char* CTraceEventBase::Data() const
    {
    return iData;
    }


unsigned int CTraceEventBase::DataLength() const
    {
    return iDataLength;
    }


unsigned int CTraceEventBase::Flags() const
    {
    return iFlags;
    }


unsigned int CTraceEventBase::PrimaryFilter() const
    {
    return iPrimaryFilter;
    }


unsigned int CTraceEventBase::SecondaryFilter() const
    {
    // TODO:
    std::cerr << "Error: CUTraceFrame::SecondaryFilter() not implemented" << std::endl;
    assert(0);
	return iSecondaryFilter;
    }


unsigned int CTraceEventBase::SubCategory() const
    {
    return iSubCategory;
    }


bool CTraceEventBase::Header2Present() const
    {
    return (iFlags & BTrace::EHeader2Present) == BTrace::EHeader2Present;
    }

bool CTraceEventBase::Timestamp1Present() const
    {
    return (iFlags & BTrace::ETimestampPresent) == BTrace::ETimestampPresent;
    }

bool CTraceEventBase::Timestamp2Present() const
    {
    return (iFlags & BTrace::ETimestamp2Present) == BTrace::ETimestamp2Present;
    }

bool CTraceEventBase::ContextIdPresent() const
    {
    return (iFlags & BTrace::EContextIdPresent) == BTrace::EContextIdPresent;
    }

bool CTraceEventBase::PcPresent() const
    {
    return (iFlags & BTrace::EPcPresent) == BTrace::EPcPresent;
    }

bool CTraceEventBase::ExtraPresent() const
    {
    return (iFlags & BTrace::EExtraPresent) == BTrace::EExtraPresent;
    }

unsigned int CTraceEventBase::Header2() const
    {
    if (Header2Present())
        {
        return iHeader2;
	    }

#ifdef _DEBUG
    assert(0); // Test for presence - don't call this method blindly
#endif
    std::cerr << "Error: CTraceEventBase::Header2() called out of context" << std::endl;
    return 0;
    }


unsigned int CTraceEventBase::Timestamp1() const
    {
	if (Timestamp1Present())
        {
        return iTimestamp1;
	    }

#ifdef _DEBUG
    assert(0); // Test for presence - don't call this method blindly
#endif
    std::cerr << "Error: CTraceEventBase::Timestamp1() called out of context" << std::endl;
    return 0;
    }


unsigned int CTraceEventBase::Timestamp2() const
    {
    if (Timestamp2Present())
	    {
        return iTimestamp2;
	    }

#ifdef _DEBUG
    assert(0); // Test for presence - don't call this method blindly
#endif
    std::cerr << "Error: CTraceEventBase::Timestamp2() called out of context" << std::endl;
    return 0;
    }


unsigned int CTraceEventBase::ContextType() const
    {
    if (ContextIdPresent())
        {
        return iContext & (3UL);
        }

    #ifdef _DEBUG
    assert(0); // Test for presence - don't call this method blindly
#endif
    std::cerr << "Error: CTraceEventBase::ContextType() called out of context" << std::endl;
    return 0;
    }


unsigned int CTraceEventBase::ContextId() const
    {
    if (ContextIdPresent())
	    {
        return iContext & (~3UL);
	    }

#ifdef _DEBUG
    assert(0); // Test for presence - don't call this method blindly
#endif
    std::cerr << "Error: CTraceEventBase::ContextId() called out of context" << std::endl;
    return 0;
    }


unsigned int CTraceEventBase::ProgramCounter() const
    {
    if (PcPresent())
        {
        return iProgramCounter;
	    }

#ifdef _DEBUG
    assert(0); // Test for presence - don't call this method blindly
#endif
    std::cerr << "Error: CTraceEventBase::ProgramCounter() called out of context" << std::endl;
    return 0;
    }


unsigned int CTraceEventBase::Extra() const
    {
    if (ExtraPresent())
	    {
        return iExtra;
	    }

#ifdef _DEBUG
    assert(0); // Test for presence - don't call this method blindly
#endif
    std::cerr << "Error: CTraceEventBase::Extra() called out of context" << std::endl;
    return 0;
    }


unsigned int CTraceEventBase::Arg1() const
    {
    return iArg1;
    }


unsigned int CTraceEventBase::EventEntryNumber() const
    {
    return iEventEntryNumber;
    }


void CTraceEventBase::WriteContextInfo(std::ostream& aOutput) const
    {
    unsigned int contextId = 0;
    unsigned int ts1 = 0;
    unsigned int ts2 = 0;
    
    if (ContextIdPresent())
        {
        contextId = ContextId();
        }

    if (Timestamp1Present())
        {
        ts1 = Timestamp1();
        }

    if (Timestamp2Present())
        {
        ts2 = Timestamp2();
        }

	// sorry, life is too short to figure out how to make ostream format floats nicely
	char timeStr[32];
	if(CUTraceDecoderApp::UseTimes())
		{
		switch(CUTraceDecoderApp::PeriodType())
			{
		case CUTraceDecoderApp::ESplit:
			{
			// guessing - need to test on Uni
			double sec = (double) ts1 * CUTraceDecoderApp::Period1() + (double) ts2 * CUTraceDecoderApp::Period2();
			sprintf_s(timeStr, sizeof(timeStr), "%1.6f,0", sec);
			break;
			}
		case CUTraceDecoderApp::E64Bit:
			{
			double sec = ((double) ((__int64) ts2 << 32) + ts1) * CUTraceDecoderApp::Period1();
			sprintf_s(timeStr, sizeof(timeStr), "%1.6f,0", sec);
			break;
			}
		default:
			sprintf_s(timeStr, sizeof(timeStr), "-0,0");
			break;
			}
		}
	else
		{
		sprintf_s(timeStr, sizeof(timeStr), "%d,%d", ts1, ts2);
		}

    aOutput << std::noshowbase << std::dec
        << iOutputHandler.CurrentFrame() << "," << PrimaryFilter() << ","
        << SubCategory() << "," 
		<< timeStr << ",0x"
        << std::noshowbase << std::setw(8) << std::setfill('0')
        << std::nouppercase << std::hex << contextId << "," ;
    }

void CTraceEventBase::WriteEventPrefix(std::ostream& aOutput) const
    {
    if (iPrefix.length() > 0)
        {
        aOutput << iPrefix << ": ";
        }
    }

void CTraceEventBase::SetPrefix(const std::string& aPrefix)
    {
    iPrefix = aPrefix;
    }

void CTraceEventBase::SetPrefix(const char* aPrefix)
    {
    iPrefix = aPrefix;
    }
        
void CTraceEventBase::ProcessEvent() const
    {
    iOutputHandler.WriteEvent(*this);
    }

void CTraceEventBase::DumpEvent(std::ostream& aStream) const
    {
    aStream
        << "Size: " << std::dec << DataLength() << std::endl
        << "Flags: 0x" << std::noshowbase << std::setw(2)
        << std::setfill('0') << std::nouppercase << std::hex
        << Flags() << std::endl
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

    if (ExtraPresent())
        {
        aStream << "Extra: 0x"
            << std::noshowbase << std::setw(8)
            << std::setfill('0') << std::nouppercase << std::hex
            << Extra() << std::endl;
        }

    DumpBytes(aStream, Data(), DataLength());
    aStream << std::endl;
    aStream.flush();
    }



unsigned int MTraceEvent::BytesToInt(const unsigned char *aBytes) const
    {
	unsigned int num = ((aBytes[3]<<24)&0xFF000000) |
	              ((aBytes[2]<<16)&0xFF0000) |
	              ((aBytes[1]<<8)&0xFF00) |
	              (aBytes[0]&0xFF);
	
	return num;
    }


unsigned int MTraceEvent::BytesToInt(const char *aBytes) const
    {
	return BytesToInt((const unsigned char *)aBytes);
    }
        

