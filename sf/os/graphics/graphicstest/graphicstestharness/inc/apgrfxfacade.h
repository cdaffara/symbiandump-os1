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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __APGRFXFACADE_H__
#define __APGRFXFACADE_H__

#include <e32std.h>
#include <w32std.h>

class TApaTaskFacade : public CBase
    {   
public:
    TApaTaskFacade(RWsSession& aSession);
	TApaTaskFacade(const TApaTaskFacade& aTaskFacade);
    ~TApaTaskFacade();
    void BringToForeground();
    void SetWgId(TInt aWgId);
private:
    RWsSession& iSession;
    TInt iWgId;
    };

class TApaTaskListFacade : public CBase
    {
public:
    TApaTaskListFacade(RWsSession& aSession);
    ~TApaTaskListFacade();
    TApaTaskFacade FindByPos(TInt aPos);
private:
    void FindByPosL(TApaTaskFacade& aTask,TInt aPos);
private:
    RWsSession& iSession;
    };

#endif // __APGRFXFACADE_H__
