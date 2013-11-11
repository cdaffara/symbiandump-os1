// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file defines the base class for protocol state handlers
// employed by the Test Protocol Module
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __CSTATEHANDLERBASE_H__
#define __CSTATEHANDLERBASE_H__

#include <e32base.h>
#include "cstatemachinebase.h"


/** Class that defines the base class for protocol state handlers.
*/
NONSHARABLE_CLASS(CStateHandlerBase) : public CBase
	{

public:
	
	/** Destructor
	*/
	virtual ~CStateHandlerBase();
	
	// Pure virtual methods to be implemented in derived classes
	
	/** Initialise state attributes
	*/
	virtual void Initialise() = 0;
	
	/** Perform entry actions
	*/
	virtual void EntryActions() = 0;
	
	/** Perform exit actions
	*/
	virtual void ExitActions() = 0;

protected:

	/** Standard constructor
	@param aMachine Reference to the parent state machine.
	*/
	CStateHandlerBase(CStateMachineBase& aMachine);
	
	
protected:

	/** Parent state machine.
	*/
	CStateMachineBase& iMachine;
	
	};

#endif // __CSTATEHANDLERBASE_H__
