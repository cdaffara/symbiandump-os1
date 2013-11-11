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
#include <sstream>
#include <string>

#include <windows.h>

#include "utracedecoderapp.h"
#include "utraceframe.h"
#include "framesorter.h"
#include "util.h"
#include "logevents\traceeventbase.h"
#include "version.h"

const int KLogBufferSize = 16384;

Parser::CDefinitionParser* gMsgDefParser;
bool gQuiet;

bool CUTraceDecoderApp::iUseTimes = false;
CUTraceDecoderApp::TTimestampPeriod CUTraceDecoderApp::iPeriodType = CUTraceDecoderApp::EUnknown;
double CUTraceDecoderApp::iPeriod1;
double CUTraceDecoderApp::iPeriod2;

CUTraceDecoderApp::CUTraceDecoderApp()
    {
    iFrameSorter = new CFrameSorter(*this);

    iDumpIdentifiers = false;
    iTesting = false;
    SetShowContextInfo(true);
    iCurrentFrame = 0;
    iUseQuotes = false;
    iTail = false;

    iInputBuffer = new unsigned char[KLogBufferSize];
    iBufferOffset = 0;
    iBufferSize = 0;
    iBufferValid = false;

    // There will only ever be one CUTraceDecoderApp so this isn't so bad
    // Needs refactoring though. gMsgDefParser is used by the Node Message Event
    ::gMsgDefParser = new Parser::CDefinitionParser();
	::gQuiet = false;
    }


CUTraceDecoderApp::~CUTraceDecoderApp()
    {
    delete iInputBuffer;
    }


bool CUTraceDecoderApp::Initialise(int aArgc, char **aArgv) 
    {
    if (aArgc < 2)
	    {
	    return false;
	    }
    
    // defaults
    SetShowContextInfo(true);
    SetNewLineEnabled(true);
    
    int currentArg = 1;
    while (currentArg < aArgc)
        {
        if (aArgv[currentArg][0] == '-' && aArgv[currentArg][1] == '-')
            {
            if (!_strcmpi(&aArgv[currentArg][2], "payload-only"))
                {
                SetShowContextInfo(false);
                }
            else if (!_strcmpi(&aArgv[currentArg][2], "use-quotes"))
                {
                iUseQuotes = true;
                }
            else if (!_strcmpi(&aArgv[currentArg][2], "use-times"))
                {
                iUseTimes = true;
                }
            else if (!_strcmpi(&aArgv[currentArg][2], "tail"))
                {
                // EXPERIMENTAL: This is an experimental feature at the moment
                iTail = true;
                }
			else if (!_strcmpi(&aArgv[currentArg][2], "quiet"))
				{
				::gQuiet = true;
				}
            else if (!_strcmpi(&aArgv[currentArg][2], "message-def"))
                {
                currentArg++;
                iMsgDefFile = aArgv[currentArg];
                }
            else if (!_strcmpi(&aArgv[currentArg][2], "dump-frame"))
                {
                currentArg++;
                char* startFrame = aArgv[currentArg];

                char* endFrame = startFrame;
                while (*endFrame != ',' && *endFrame != 0)
                    {
                    if (*endFrame < '0' || *endFrame > '9')
                        {
                        return false;
                        }
                    ++endFrame;
                    }
                if (*endFrame == 0)
                    {
                    return false;
                    }
                *endFrame = 0;
                if (*startFrame == 0)
                    {
                    return false;
                    }
                ++endFrame;
                
                char* tmp = endFrame;
                while (*tmp != 0)
                    {
                    if (*tmp < '0' || *tmp > '9')
                        {
                        return false;
                        }
                    ++tmp;
                    }

                iFrameSorter->SetDumpFrameRange(atol(startFrame), atol(endFrame));
                }
            else if (!_strcmpi(&aArgv[currentArg][2], "dump-event"))
                {
                currentArg++;
                char* startEvent = aArgv[currentArg];

                char* endEvent = startEvent;
                while (*endEvent != ',' && *endEvent != 0)
                    {
                    if (*endEvent < '0' || *endEvent > '9')
                        {
                        return false;
                        }
                    ++endEvent;
                    }
                if (*endEvent == 0)
                    {
                    return false;
                    }
                *endEvent = 0;
                if (*startEvent == 0)
                    {
                    return false;
                    }
                ++endEvent;
                
                char* tmp = endEvent;
                while (*tmp != 0)
                    {
                    if (*tmp < '0' || *tmp > '9')
                        {
                        return false;
                        }
                    ++tmp;
                    }

                iFrameSorter->SetDumpEventRange(atol(startEvent), atol(endEvent));
                }
            else if (!_strcmpi(&aArgv[currentArg][2], "dump-message-def"))
                {
                iDumpIdentifiers = true;
                }
            else if (!_strcmpi(&aArgv[currentArg][2], "test-message-def"))
                {
                iTesting = true;
                }
            }
        else
            {
            if (iLogFile.length() > 0)
                {
                return false;
                }
            iLogFile = aArgv[currentArg];
            }

        currentArg++;
        }

    if (iMsgDefFile.length() > 0)
        {
        if (iTesting)
            {
            gMsgDefParser->SetTestMode();
            }

        if (gMsgDefParser->ParseDefinitionFile(iMsgDefFile.c_str()) != Parser::ENoError)
            {
//            std::cerr << "Error in message definition file" << std::endl;
            return false;
            }
        }

    if (iDumpIdentifiers)
        {
        gMsgDefParser->DumpIdentifiers();
        }
    
    if (iTesting)
        {
        if (iMsgDefFile.length() == 0)
            {
            std::cerr << "No message definition file specifed to check" << std::endl;
            }
        return true;
        }

    if (iLogFile.length() == 0)
        {
        return false;
        }
        
    return true;
    }


void CUTraceDecoderApp::ProcessLog()
    {
    // csv header
    if (ShowContextInfo())
        {
        std::cout << "Sequence,Primary Filter,Sub Category,Time Stamp1,Time Stamp2,Context ID,Description" << std::endl;
        } 

    // Open the log file and fill the buffer
    if(!LoadUTraceLog()) 
        { // file couldn't be opened
        std::cerr << "Error: file " << iLogFile.c_str() << " could not be opened." << std::endl;
        return;
        }

    while (iBufferValid)
        {
        while (iBufferOffset < iBufferSize)
            {
            //grab a hunk of log
            int length = iInputBuffer[iBufferOffset];
            length = (length + 3) & ~3;

            // Minimum length is actually 4, but this would mean only the header was
            // present, so why was it logged
            if (length < 8 || length > KMaxBTraceRecordSize)
                {
                if (!iInputFile.eof())
                    {
                    std::cerr << "Error: Frame " << iCurrentFrame << " is corrupt - size is out of range (8-116)" << std::endl;
                    
                    // Dump what we have in the buffer, up to the max size of a frame
                    DumpBytes(std::cerr, &iInputBuffer[iBufferOffset], (KMaxBTraceRecordSize < (iBufferSize - iBufferOffset)) ? KMaxBTraceRecordSize : (iBufferSize - iBufferOffset));
                    }
                return;
                }

            if (length + iBufferOffset > iBufferSize)
                {
                break;
                }
	   		
            CUTraceFrame* frame = new CUTraceFrame(&iInputBuffer[iBufferOffset], iCurrentFrame);
            if (frame)
                {
                iFrameSorter->ProcessFrame(frame);
                frame = NULL;
                iCurrentFrame++;
                }
            else
                {
                std::cerr << "Failed to create frame instance" << std::endl;
                }
	   		
            iBufferOffset += length;
            }
		   
        RefillUTraceBuffer();
        }
    
    iInputFile.close();
	
	std::cerr << "Processed " << iCurrentFrame << " frames" << std::endl;
    }


unsigned int CUTraceDecoderApp::CurrentFrame() const
    {
    return iCurrentFrame;
    }


void CUTraceDecoderApp::WriteEvent(const CTraceEventBase& aEvent) const
    {
    if (NewLineEnabled())
        {
        if (ShowContextInfo())
            {
            aEvent.WriteContextInfo(std::cout);
            }

        if (iUseQuotes)
            {
            std::cout << "\"";
            }
        aEvent.WriteEventPrefix(std::cout);
        }

    if (iUseQuotes)
        {
        std::stringstream ss;
        aEvent.Describe(ss);
        std::string s = ss.str();
        // todo: replace " with ""
        std::cout << s;
        }
    else
        {
        aEvent.Describe(std::cout);
        }

    if (NewLineEnabled())
        {
        if (iUseQuotes)
            {
            std::cout << "\"";
            }
        std::cout << std::endl;
        }
    else
        {
        std::cout << " ";
        }
    };


bool CUTraceDecoderApp::LoadUTraceLog()
    {
    iInputFile.open(iLogFile.c_str(), std::ios::in | std::ios::binary);
    if (iInputFile.is_open())
        {
        RefillUTraceBuffer();
        return true;
        }

    return false;
    }


void CUTraceDecoderApp::RefillUTraceBuffer()
    {
    int bufferSpace = KLogBufferSize;
    int bufferInUse = 0;

    if (iBufferOffset)
        {
        bufferInUse = iBufferSize - iBufferOffset;
        bufferSpace = KLogBufferSize - bufferInUse;
        memcpy(iInputBuffer, &iInputBuffer[iBufferOffset], bufferInUse);
        }

    memset(&iInputBuffer[bufferInUse], 0, bufferSpace);
    iInputFile.read((char*)&iInputBuffer[bufferInUse], bufferSpace);
    iBufferSize = iInputFile.gcount();

#ifdef _DEBUG
    std::cerr << std::dec << std::noshowbase << iBufferSize << " bytes read from log file" << std::endl;
#endif
    iBufferValid = (iBufferSize > 0);

    if (!iBufferValid && iTail)
        {
        // EXPERIMENTAL: This is an experimental feature at the moment
        HANDLE fileChangeHandle;
        fileChangeHandle = FindFirstChangeNotificationA(iLogFile.c_str(), FALSE, FILE_NOTIFY_CHANGE_SIZE);
        if (fileChangeHandle != INVALID_HANDLE_VALUE)
            {
            iInputFile.read((char*)&iInputBuffer[bufferInUse], bufferSpace);
            iBufferSize = iInputFile.gcount();

            std::cerr << std::dec << std::noshowbase << iBufferSize << " bytes read from log file" << std::endl;
            iBufferValid = (iBufferSize > 0);
            FindCloseChangeNotification(fileChangeHandle);
            }
        }

    iBufferSize += bufferInUse;
    iBufferOffset = 0;
    }


void CUTraceDecoderApp::ShowUsage()
    {
	std::cerr
        << "utracedecoder " APPVERSION ":" << std::endl
	    << "Log parser for utrace logs, converts a utrace generated log file into human readable and perl processable csv format" << std::endl
	    << "utracedecoder [options] <path_to_log>" << std::endl
        << "    --payload-only                 Show only the message part of the trace" << std::endl
        << "    --use-quotes                   Wrap the description in quotes" << std::endl
        << "    --use-times                    \"time-in-seconds,0\" substituted for \"timestamp1,timestamp\" pair" << std::endl
        << "    --message-def <message_definition_file>  The file from which to load message and signature definition" << std::endl
        << "    --test-message-def             Tests the message definition file showing all errors then exits" << std::endl
        << std::endl << "Debugging options" << std::endl 
        << "    --dump-message-def             Dumps the parsed definition file details" << std::endl
        << "    --dump-frame <start_frame>,<end_frame>  Dumps the frames in the given range to stderr" << std::endl
        << "    --dump-event <first_event>,<last_event>  Dumps the events in the given range to stderr" << std::endl
		<< "    --quiet                        Only output urgent warnings" << std::endl
        << std::endl;
    }

void CUTraceDecoderApp::SetTimestampPeriod(CUTraceDecoderApp::TTimestampPeriod aType, double aPeriod1, double aPeriod2)
	{
	iPeriodType = aType;
	iPeriod1 = aPeriod1;
	iPeriod2 = aPeriod2;
	}

