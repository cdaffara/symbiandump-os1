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
// Wait for first one incoming call

#ifndef ATRING_H_
#define ATRING_H_

//system include

//user include
#include "atcommandbase.h"

class CATRing : public CAtCommandBase
	{
public:
	/**
	 * Factory function
	 * 
	 */
	static CATRing* NewL(CGlobalPhonemanager& aGloblePhone,
			             CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 *
	 * Factory function
	 * 
	 */ 
	static CATRing* NewLC(CGlobalPhonemanager& aGloblePhone,
			              CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 
	 * Destructor
	 * 
	 */
	virtual ~CATRing();
	
	/**
	 * Run this command
	 * 
	 */
	virtual void ExecuteCommand(){};
		
protected:
	/**
	 * 
	 * Constructor
	 * 
	 */
	CATRing(CGlobalPhonemanager& aGloblePhone,
			CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	 * 
	 * 2nd constructor
	 * 
	 */
	void ConstructL();
	
	};

#endif /*ATRING_H_*/
