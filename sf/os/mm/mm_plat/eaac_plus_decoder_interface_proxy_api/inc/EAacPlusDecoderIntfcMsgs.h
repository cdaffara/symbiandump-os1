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


#ifndef EAACPLUSDECODERINTFCMSGS_H
#define EAACPLUSDECODERINTFCMSGS_H

// INCLUDES
#include <EAacPlusDecoderIntfc.h>

// CONSTANTS

// MACROS

// DATA TYPES
class TEAacPlusDecoderConfig
    {
    public:

    CEAacPlusDecoderIntfc::TAudioObjectType iAudioObjectType;
    TUint iInputSamplingFrequency;
    TUint iNumOfChannels;
    TBool iSbrEnabled;
    TBool iDsmEnabled;
    };


enum TEAacPlusDecoderIntfcMsgs
	{
	EEaacpdimApplyConfig
	};

#endif      // EAACPLUSDECODERINTFCMSGS_H

// End of File
