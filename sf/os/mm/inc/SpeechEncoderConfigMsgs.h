/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Messages sent by proxy to the message handler.
*
*/



#ifndef SPEECHENCODERCONFIGMSGS_H
#define SPEECHENCODERCONFIGMSGS_H

// INCLUDES

// CONSTANTS

// MACROS

// DATA TYPES
enum TSpeechEncoderConfigMsgs
	{
	ESecmGetNumOfSupportedBitrates,
	ESecmGetSupportedBitrates,
	ESecmSetBitrate,
	ESecmGetBitrate,
	ESecmSetVadMode,
	ESecmGetVadMode
	};

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

#endif      // SPEECHENCODERCONFIGMSGS_H
            
// End of File
