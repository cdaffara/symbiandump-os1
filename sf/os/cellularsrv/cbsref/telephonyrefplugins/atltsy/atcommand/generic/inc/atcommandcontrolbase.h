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
// This contains CATCommandControlBase which is composite class of a number classes
// 

#ifndef ATCOMMANDCONTROLBASE_H_
#define ATCOMMANDCONTROLBASE_H_

//system include
#include <e32base.h>

//user include 
#include "atcommandtype.h"
#include "atcommandexecuteobserver.h"
#include "requestbase.h"
#include <ctsy/ltsy/cctsydispatchercallback.h>


//forward class
class CAtCommandBase;
class CRequestBase;

/**
 * This class is a composite class which is composed of a number of AT commands
 * 
 */
class CATCommandControlBase : public CRequestBase, public MAtCommandExecuteObserver
	{
public:
    /**
     * Destructor
     */
	virtual ~CATCommandControlBase();
public:
    /**
     * Add an AT command
     * 
     * @param aAtCommand The AT command which is to be added in
     */
	TInt AddAtCommand(const CAtCommandBase* aAtCommand);
	
	/**
	 * Remove the AT command
	 */
	void RemoveAtCommand(const CAtCommandBase* aAtCommand);
	
	/**
	 * Remove the AT command by its AT type
	 */
	void RemoveAtCommandByAtType(TLtsyATCommandType aType);
	
	/**
	 * Get the AT command by its AT type
	 * 
	 * @param aType @see TLtsyATCommandType
	 */
	CAtCommandBase& GetAtCommandByAtTypeL(TLtsyATCommandType aType);
	
	/**
	 * Remove all the AT commands
	 */
	void CleanAllAtCommands();
	
	/**
	 * Remove all the AT commands and destroy them also
	 */
	void CleanAndDeleteAllAtCommands();	
public:
    /**
     * Define the type to delete the AT command
     */
	enum TAutoDeleteAtCommand
		{
		/**
		 * Delete all at command instance contained in the RPointArray when destructor
		 */
		EAutoDelete,
		/**
		 * Only close RPointArray, not delete at command instance contained in the RPonintArray
		 */
		ENoDelete
		};
	/**
	 * Set the type of auto delete type
	 */
	void SetIsAutoDelete(TAutoDeleteAtCommand aIsDel);
	
	/**
	 * Get the auto delete type
	 */
	TAutoDeleteAtCommand GetIsAutoDelete();
protected:
    /**
     * Constructor
     */
	CATCommandControlBase(CGlobalPhonemanager& aGloblePhone, 
		                  CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
protected:
    /**
     * The dispatcher call back
     */
	CCtsyDispatcherCallback& iCtsyDispatcherCallback;
private:
    /**
     * Auto delete type
     */
    TAutoDeleteAtCommand iIsDel;
    
    /**
     * AT commands store
     */
    RPointerArray<CAtCommandBase> iArray;	
	};

#endif /*ATCOMMANDCONTROLBASE_H_*/
