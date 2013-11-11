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

#ifndef STATEMACHINEBASE_H
#define STATEMACHINEBASE_H

class CStateBase;

class CStateMachineBase
    {
    public:
        CStateMachineBase();
        ~CStateMachineBase();

        void SetState(CStateBase* aNewState);
        inline CStateBase& State()
            {
            return *iCurrentState;
            }

    private:
        CStateBase* iCurrentState;
        CStateBase* iLastState;
    };

#endif
//STATEMACHINEBASE_H


