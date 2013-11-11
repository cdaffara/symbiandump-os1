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

#ifndef UTRACEDECODERAPP_H
#define UTRACEDECODERAPP_H

#include <string>
#include <iostream>
#include <fstream>

#include "globals.h"
#include "eventoutputhandler.h"
#include "messagedefparser\definitionparser.h"

class CFrameSorter;


class CUTraceDecoderApp : private MEventOutputHandler
    {
    public:
        CUTraceDecoderApp();
        ~CUTraceDecoderApp();

        bool Initialise(int aArgc, char** aArgv);
        void ShowUsage();
        void ProcessLog();

		static bool UseTimes()
			{
			return iUseTimes;
			}
		enum TTimestampPeriod { EUnknown, ESplit, E64Bit };
		static void SetTimestampPeriod(TTimestampPeriod aType, double aPeriod1, double aPeriod2);
		static inline TTimestampPeriod PeriodType() 
			{
			return iPeriodType;
			}
		static double Period1() 
			{
			return iPeriod1;
			}
		static double Period2() 
			{
			return iPeriod2;
			}

        inline bool TestMode() const
            {
            return iTesting;
            }

    private:
        virtual unsigned int CurrentFrame() const;
        virtual void WriteEvent(const CTraceEventBase& aEvent) const;

        bool LoadUTraceLog();
        void RefillUTraceBuffer();

    private:
        mutable unsigned int iCurrentFrame;
        bool iDumpIdentifiers;
        std::string iLogFile;
        unsigned char* iInputBuffer;
        std::ifstream iInputFile;
        std::string iMsgDefFile;
        int iBufferOffset;
        int iBufferSize;
        bool iBufferValid;
        bool iTesting;
        bool iUseQuotes;
        bool iTail;
		CFrameSorter* iFrameSorter;
		static bool iUseTimes;
		static TTimestampPeriod iPeriodType;
		static double iPeriod1;
		static double iPeriod2;
    };

#endif
// UTRACEDECODERAPP_H

