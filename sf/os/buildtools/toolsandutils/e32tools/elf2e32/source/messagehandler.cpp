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
// Error Message Interface Operations for elf2e32 tool
// @internalComponent
// @released
// 
//

#include "messagehandler.h"
#include "messageimplementation.h"

Message* MessageHandler::iInstance=0;

/**
Function Get Instance of class Message Implementation and initializing messages.

@internalComponent
@released

@return Instance of MessageImplementation
*/
Message * MessageHandler::GetInstance()
{
    if(iInstance == 0)
	{
		iInstance = new MessageImplementation();
		iInstance->InitializeMessages(NULL);
	}
	return iInstance;
}

/**
Function to call StartLogging function of class Message Implementation.

@internalComponent
@released

@param aFileName
Name of the Log File
*/
void MessageHandler::StartLogging(char *aFileName)
{
    GetInstance()->StartLogging(aFileName);
}

/**
Function to call InitializeMessages function of class Message Implementation.

@internalComponent
@released

@param aFileName
Name of the Message file 
*/
void MessageHandler::InitializeMessages(char *aFileName)
{
	GetInstance()->InitializeMessages(aFileName);
}

/**
Function to delete instance of class MessageImplementation

@internalComponent
@released
*/
void MessageHandler::CleanUp()
{
	delete iInstance;
}
