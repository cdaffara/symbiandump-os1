// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Provide an alternative to the apgrfx to avoid a dependency on middleware

#include "apgrfxfacade.h"

TApaTaskFacade::TApaTaskFacade(RWsSession& aSession) :
    iSession(aSession), iWgId(0)
    {
    }

TApaTaskFacade::~TApaTaskFacade()
    {
    }

TApaTaskFacade::TApaTaskFacade(const TApaTaskFacade& aTaskFacade) :
	iSession(aTaskFacade.iSession), iWgId(aTaskFacade.iWgId)
	{
	}

void TApaTaskFacade::BringToForeground()
    {
    iSession.SetWindowGroupOrdinalPosition(iWgId, 0);
    }

void TApaTaskFacade::SetWgId(TInt aWgId)
    {
    iWgId = aWgId;
    }

TApaTaskListFacade::TApaTaskListFacade(RWsSession& aSession) :
    iSession(aSession)
    {
    }

TApaTaskListFacade::~TApaTaskListFacade()
    {
    }

TApaTaskFacade TApaTaskListFacade::FindByPos(TInt aPos)
	{
	TApaTaskFacade task(iSession);
	TRAP_IGNORE(FindByPosL(task, aPos));
	return task;
	}

void TApaTaskListFacade::FindByPosL(TApaTaskFacade& aTask,TInt aPos)
    {
    TInt wgId;
    const TInt windowGroups=iSession.NumWindowGroups(0);
    if (aPos < 0 || windowGroups < 0 || aPos > windowGroups)
        {
        wgId = 0;
        }
    else
        {
        CArrayFixFlat<TInt>* wgIdArray=new(ELeave) CArrayFixFlat<TInt>(windowGroups);
        CleanupStack::PushL(wgIdArray);
        User::LeaveIfError(iSession.WindowGroupList(0, wgIdArray));
        wgId=(*wgIdArray)[aPos];
        CleanupStack::PopAndDestroy(1);
        }
    aTask.SetWgId(wgId);
    }
