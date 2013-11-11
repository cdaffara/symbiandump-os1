// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ServerConsole.h"


/**
 *
 * Static constructor for CServerConsole.
 *
 * @param	"const TDesC& aName"
 *			Display name for console
 *
 * @return	"CServerConsole*"
 *			The constructed CServerConsole
 *
 * @xxxx 
 *
 */
CServerConsole* CServerConsole::NewL(const TDesC& aName)
	{
	CServerConsole* s = new(ELeave) CServerConsole;
	CleanupStack::PushL(s);
	s->ConstructL(aName);
	CleanupStack::Pop();
	return s;
	}

/**
 *
 * Destructor for CServerConsole.
 * Destroys display console and its reader
 *
 * @xxxx
 *
 */
CServerConsole::~CServerConsole()
	{
	delete iWindowName;
	delete iInstructions;
	delete iConsole;
	}

/**
 *
 * First-phase constructor for CServerConsole.
 * Adds itself to the Active Scheduler.
 *
 * @xxxx
 *
 */
CServerConsole::CServerConsole()
	{
	}

/**
 *
 * Second-phase constructor for CServerConsole.
 * Creates the display console and its reader.
 *
 * @param	"const TDesC& aName"
 *			Display name of console.
 *
 * @xxxx
 *
 */
void CServerConsole::ConstructL(const TDesC& aName)
	{
	iWindowName = aName.AllocL();
	iConsole =  Console::NewL(*iWindowName, TSize(KConsFullScreen,KConsFullScreen));
	}

/**
 *
 * Accessor for base console.
 *
 * @return	"CConsoleBase*"
 *			The base console.
 *
 * @xxxx 
 *
 */
CConsoleBase* CServerConsole::Console() const
	{
	return iConsole;
	}

/**
 *
 * Sets and shows displayable instructions.
 *
 * @param	"const TDesC& aInstructions"
 *			Displayable instruction string.
 *
 * @xxxx 
 *
 */
void CServerConsole::SetInstructionsL(const TDesC& aInstructions)
	{
	if (iInstructions)
		{
		delete iInstructions;
		iInstructions = NULL;
		}
	iInstructions = aInstructions.AllocL();
	iConsole->ClearScreen();
	iConsole->Write(*iInstructions);
	}

