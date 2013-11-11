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
// This file defines the class for the MO-LR protocol state handler
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __CMOLRSTATEHANDLER_H__
#define __CMOLRSTATEHANDLER_H__

#include <e32base.h>
#include "cstatehandlerbase.h"

class CStateMachineBase;

/** Class that defines the MO-LR protocol state handler
This class implements pure virtual methods defined by CStateHandlerBase
@see CStateHandlerBase
*/
NONSHARABLE_CLASS(CMoLrStateHandler) : public CStateHandlerBase
	{
public:

	static CMoLrStateHandler* NewL(CStateMachineBase& aMachine);
	virtual ~CMoLrStateHandler();
	
	// CStateHandlerBase methods
	
	void Initialise();
	void EntryActions();
	void ExitActions();
	
protected:

	CMoLrStateHandler(CStateMachineBase& aMachine);
	
private:
	
	void ConstructL();

	};

#endif // __CMOLRSTATEHANDLER_H__

