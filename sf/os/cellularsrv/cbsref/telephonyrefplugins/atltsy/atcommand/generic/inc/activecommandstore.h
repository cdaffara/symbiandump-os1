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
// This contains CActiveCommandStore which store one and only one solicited active at command and a number of active 
// unsolicited at commands
// 

#ifndef ACTIVECOMMANDSTORE_H_
#define ACTIVECOMMANDSTORE_H_

#include "atcommandbase.h"

//Class forward

/**
 * This class is used to manager a list of active AT commands
 * 
 */
class CActiveCommandStore : public CBase
	{
public:
    /**
     * Factory function
     */
	static CActiveCommandStore* NewL();
	
	/**
	 * Factory function
	 */
	static CActiveCommandStore* NewLC();
	
	/**
	 * Destructor
	 */
	virtual ~CActiveCommandStore();
	
	/**
	 * Add unsolicited AT command
	 */
	TInt AddUnsolicitedAtCommand(CAtCommandBase *aAtCommand);
	
	/**
	 * Add the solicited AT command
	 */
	TInt AddSolicitedAtCommand(CAtCommandBase *aAtCommand);
	
	/**
	 * Remove the At command
	 */
	TInt RemoveActiveAtCommand(const CAtCommandBase *aAtCommand);
	
	/**
	 * Get the active command list
	 */
	RPointerArray<CAtCommandBase>& GetActiveCommandArray();
protected:
    /**
     * constructor
     */
	CActiveCommandStore();
	
	/**
	 * 2nd constructor
	 */
	void ConstructL();
private:
    /**
     * The list store the acitve AT command solicited and unsolicited
     */
	RPointerArray<CAtCommandBase> iArray;
	};

#endif /*ACTIVECOMMANDSTORE_H_*/
