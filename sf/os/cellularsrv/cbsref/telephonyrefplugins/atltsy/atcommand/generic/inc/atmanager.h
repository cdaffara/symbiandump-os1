// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file atmanager.h
// This contains CAtManager which manage AT commands
//

#ifndef ATMANAGER_H_
#define ATMANAGER_H_

// system include
#include <e32base.h>

// user include
#include "mcommobserver.h"
#include "atcommandbase.h"

//class forward
class CActiveCommandStore;

/**
 * This class is acting an interface between Comms Engine and AT commands, which manage to find active
 * AT command and send the Event to it.
 * 
 */
class CAtManager : public CBase, 
				   public MCommReadLineNotifier, 
				   public MCommEngineObserver
	{
public:
	/**
	 * Factory function
	 * 
	 */
	static CAtManager* NewL();
	
	/**
	 * Factory function
	 */
	static CAtManager* NewLC();
	
	/**
	 * Destructor
	 */
	virtual ~CAtManager();
	
public:
	/**
	 * Set current active solicited AT Command.
	 * By default, one one solicited AT command should be currently Active
	 * 
	 * @param aSolicitedCommand The solicited AT command
	 */
	void SetSolicitedAtCommand(CAtCommandBase* aSolicitedCommand);
	
	/**
	 * Get the Active Commands store: only one solicited and number of unsoclited AT Commands
	 * 
	 * CActiveCommandStore The active commands store
	 */
	CActiveCommandStore* GetActiveCommandStore() const;
	
	/**
	 * From MCommEngineObserver
	 * This function would be called when the event @see TAtEventSource happened.
	 * 
	 */
	virtual void ReadOrWriteComplete(TAtEventSource aEvent, TInt aStatus);
	
	/**
	 * From MCommReadLineNotifier
	 * This function would be called when one line string were found 
	 */
	virtual TBool NotifyOneLineFoundL(TInt aStatus, const TDesC8& aLineBuf);
	
protected:
	/**
	 * Constructor
	 * 
	 */
	CAtManager();
	
	/**
	 * 2nd constructor
	 * 
	 */
	void ConstructL();
	
private:
	/**
	 * The active solicited AT command
	 * Not Own
	 */
	CAtCommandBase* iSolicitedCommand; 
	
	/**
	 * The active command store
	 * Own
	 */
	CActiveCommandStore* iActiveCommandStore;
	};

#endif /*ATMANAGER_H_*/
