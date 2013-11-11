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

#ifndef STATEBASE_H
#define STATEBASE_H

class CStateMachineBase;

class CStateBase
    {
    protected:
        explicit CStateBase(int aStateId, CStateMachineBase& aStateMachine);
        inline CStateMachineBase& StateMachine()
            {
            return iStateMachine;
            }

    public:
        inline int StateId()
            {
            return iStateId;
            }

        virtual ~CStateBase();
        virtual void EnterState();
        virtual void ProcessState() = 0;

    private:
        CStateMachineBase& iStateMachine;
        int iStateId;
    };

#endif
// STATEBASE_H

