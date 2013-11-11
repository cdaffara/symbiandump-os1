/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Defines messages sent by the proxy to the message handler.
*
*/


#ifndef WMADECODERINTFCMSGS_H
#define WMADECODERINTFCMSGS_H

// INCLUDES
#include <WmaDecoderIntfc.h>

// CONSTANTS

// MACROS

// DATA TYPES
class TAllParams
	{
	public:
		CWmaDecoderIntfc::TFormat iCurrentFormat;
		TInt  iCurrentFormatStatus;
        TUint iCurrentBitsPerSample;
        TInt  iCurrentBitsPerSampleStatus;       
        TUint iCurrentNumChannelsIn;
        TInt iCurrentNumChannelsInStatus;        
        TUint iCurrentSamplesPerSec;
        TInt iCurrentSamplesPerSecStatus;
        TUint iCurrentAvgBytesPerSec;
        TInt iCurrentAvgBytesPerSecStatus;
        TUint iCurrentBlockAlign;
        TInt iCurrentBlockAlignStatus;
        TUint iCurrentEncodeOptions;
        TInt iCurrentEncodeOptionsStatus;
		TUint iCurrentEncodeOptions1;
		TInt iCurrentEncodeOptions1Status;
		TUint iCurrentEncodeOptions2;		
		TInt iCurrentEncodeOptions2Status;		
        TUint iCurrentChannelMask;				
        TInt iCurrentChannelMaskStatus;
        TBool iCurrentToolOutPut32Bit;
        TInt iCurrentToolOutPut32BitStatus;
        TBool iCurrentToolDownMixToStereo;
        TInt iCurrentToolDownMixToStereoStatus;
        TBool iCurrentToolLostDataConcealment;
        TInt iCurrentToolLostDataConcealmentStatus;              
	};

class TWmaDecoderConfig
    {
    public:

		CWmaDecoderIntfc::TFormat iFormat;
        TUint iBitsPerSample;
        TUint iNumChannelsIn;
        TUint iSamplesPerSec;
        TUint iAvgBytesPerSec;
        TUint iBlockAlign;
        TUint iEncodeOptions;
		TUint iEncodeOptions1;
		TUint iEncodeOptions2;		
        TUint iChannelMask;
        TBool iToolOutPut32Bit;
        TBool iToolDownMixToStereo;
        TBool iToolLostDataConcealment;
    };


enum TWmaDecoderIntfcMsgs
	{
	EWmaGetNumOfSupportedFormats,
	EWmaGetSupportedFormats,
	EWmaGetNumOfSupportedTools,
	EWmaGetSupportedTools,
	EWmaGetSupportedMaxChannels,
	EWmaGetSupportedMaxBitrate,
	EWmaGetSupportedMaxSampleRate,
	EWmaGetNumOfControllableTools,
	EWmaGetControllableTools,
	EWmaGetAllMsg,
	EWmaApplyConfig
	};

#endif      // WMADECODERINTFCMSGS_H

// End of File
