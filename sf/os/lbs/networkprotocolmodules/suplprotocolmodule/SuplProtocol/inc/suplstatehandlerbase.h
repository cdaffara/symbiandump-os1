// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// employed by the SUPL Protocol Module
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/

#ifndef __CSUPLSTATEHANDLERBASE_H__
#define __CSUPLSTATEHANDLERBASE_H__

#include <e32base.h>
#include "suplfsmsessionbase.h"
#include "suplmessagecommon.h"


/** Class that defines the base class for protocol state handlers.
*/
NONSHARABLE_CLASS(CSuplStateHandlerBase) : public CBase
	{

public:
	
	/** Destructor
	*/
	virtual ~CSuplStateHandlerBase();
	
	// Pure virtual methods to be implemented in derived classes
	
	/** Perform entry actions
	*/
	virtual TBool EntryActionsL() = 0;

protected:

	/** Standard constructor
	@param aMachine Reference to the parent state machine.
	*/
	CSuplStateHandlerBase(CSuplFsmSessionBase& aMachine);
	
	TBool BuildLocationIdL(CSuplLocationId*& aLocationId);
	TBool EntryActionsFromPositioningInProgressStateL();
	TBool EntryActionsFromPositionReceivedStateL();
	TBool EntryActionsFromSuplSessionEndedStateL();
	TBool EntryActionsFromLbsSessionEndedStateL();
	
	
	void ConvertToBcd(TDes8& aBcdString);

protected:

	/** Parent state machine.
	*/
	CSuplFsmSessionBase& iMachine;
	
	};

#endif // __CSUPLSTATEHANDLERBASE_H__
