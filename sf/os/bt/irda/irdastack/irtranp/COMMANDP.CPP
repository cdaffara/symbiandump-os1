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

#include "debug.h"
#include <e32base.h>
#include "COMMANDP.H"
#include "ABSTRACT.H"
#include "GLOBAL.H"

CommandProcessor::CommandProcessor()
	{
	m_iCommands = 0;
	m_iMaxCommands = 20;
	for(TInt i=0;i<m_iMaxCommands;i++)
		{
		m_CommandQueue[i] = NULL;
		}
	}

CommandProcessor::~CommandProcessor()
	{
	for(TInt i=0;i<m_iMaxCommands;i++)
		{
		//if(m_CommandQueue[i]!=NULL)
		//	{
			delete m_CommandQueue[i]; // Get rid of old commands!
		//	}
		}
	}

/*
* Method description:	Called from SCEP with a new command to add into the outgoing queue
*
* Parameter:			a_oCommand - pointer to a command to send
*/

void CommandProcessor::Add(AbstractCommand* a_oCommand)
	{	
	SCEPPRINT(_L("CommandProcessor: Add Command\n"));

	m_CommandQueue[m_iCommands++] = a_oCommand;
	if(m_iCommands >= m_iMaxCommands)
		{
		m_iCommands = 0; // yeah, sorry. We do leak old stuff for now.
		}
	}

/*
* Method description:	Called from SCEP when the application "pumps" the module for incoming/outgoing data processing
*
*/

void CommandProcessor::DoCommand()
	{	
	if(m_CommandQueue[0] != NULL)
		{
		SCEPPRINT(_L("CommandProcessor: DoCommand\n"));
		m_CommandQueue[0]->DoIt(); // we always execute the first command in the queue!
		
		delete m_CommandQueue[0];
		m_CommandQueue[0] = NULL; // well, it gets a new value below .. but anyway
		ShiftQueue();
		}
	}

	/*
	* Method description:	Private method called to shift new commands to the beginning of the queue
	*
*/

void CommandProcessor::ShiftQueue()
	{
	for(TInt i=1;i<m_iCommands;i++)
		{
		m_CommandQueue[i-1] = m_CommandQueue[i];
		}
	m_CommandQueue[m_iCommands] = NULL; // obsolete pointer
	m_iCommands--;
	if(m_iCommands<0) // can this happen?
		{
		m_iCommands=0;
		}
	}
