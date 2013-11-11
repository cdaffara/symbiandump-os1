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
// This file provides the implementation of the base class for
// protocol states used in the Test Protocol Module.
// 
//

#include "cstatehandlerbase.h"


/** Standard constructor.
@param aMachine A reference to the parent state machine.
*/  
CStateHandlerBase::CStateHandlerBase(CStateMachineBase& aMachine)
: iMachine(aMachine)
	{
	}


/** Standard destructor.
*/  
CStateHandlerBase::~CStateHandlerBase()
	{
	}


