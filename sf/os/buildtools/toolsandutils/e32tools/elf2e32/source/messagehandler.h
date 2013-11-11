// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Message Handler Class for elf2e32 tool
// @internalComponent
// @released
// 
//


#ifndef _MESSAGE_HANDLER_
#define _MESSAGE_HANDLER_

#include "messageimplementation.h"

/**
Class for Message Handler which will be used for getting instance of Message Implementationin
and start logging, creating message file, initializing messages.
@internalComponent
@released
*/
class MessageHandler
{
    public:
		static Message *GetInstance();
		static void CleanUp();
		static void StartLogging(char *filename);
		static void CreateMessageFile(char *fileName);
		static void InitializeMessages(char *fileName);

    private:
		static Message* iInstance;
};



#endif

