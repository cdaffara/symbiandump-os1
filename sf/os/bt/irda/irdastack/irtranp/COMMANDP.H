// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

class AbstractCommand;

/*
 * Name:		CommandProcessor
 *
 * Description:	Contains a queue of commands to be sent to CComReadWrite
 *
 */

NONSHARABLE_CLASS(CommandProcessor)
{
private:
	// Array of pointers to the commands - commandqueue that is. Note the number - same as m_iMaxCommands
	AbstractCommand* m_CommandQueue[20];
	// Where we are in the queue
	int m_iCommands;
	// Maximum number of commands that we can hold
	int m_iMaxCommands;

public:
	CommandProcessor();
	~CommandProcessor();
	void Add(AbstractCommand* a_oCommand);
	void DoCommand();
	void ShiftQueue();
};

#endif // COMMANDPROCESSOR_H
