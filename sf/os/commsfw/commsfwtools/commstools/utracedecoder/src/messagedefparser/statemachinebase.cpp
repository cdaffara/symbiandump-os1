// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <stddef.h>

#include "messagedefparser\statemachinebase.h"
#include "messagedefparser\statebase.h"

CStateMachineBase::CStateMachineBase()
    {
    iCurrentState = NULL;
    iLastState = NULL;
    }

CStateMachineBase::~CStateMachineBase()
    {
    delete iCurrentState;
    delete iLastState;
    }

void CStateMachineBase::SetState(CStateBase* aNewState)
    {
    delete iLastState;
    iLastState = iCurrentState;
    iCurrentState = aNewState;
    iCurrentState->EnterState();
    }

