// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CMVSViewTicker.cpp
// Part of the MVS Application for TechView
//



//
//#include files
//
#include "MVSViewTicker.h"
#include "MVSApp.hrh"




//
//NewL(...)              *** This method can LEAVE ***
//
// Factory constructor, sets up the ticker for the Set Position dialog
//
CMVSViewTicker* CMVSViewTicker::NewL(CMVSAppUi* aAppUi)
	{
	CMVSViewTicker* self = new(ELeave) CMVSViewTicker(aAppUi);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}



//
//CMVSViewTicker(...)
//
// Initialises the ticker with an interval and the dialog. This is where the
// ticker interval is set.
//
CMVSViewTicker::CMVSViewTicker(CMVSAppUi* aAppUi)
	{
	iTickInterval = 200000;
    iAppUi = aAppUi;
	}



//
//~CMVSViewTicker()
//
// Destructor
//
CMVSViewTicker::~CMVSViewTicker()
	{
	if (iPeriodic)
 		{
 		iPeriodic->Cancel();
 		}
	delete iPeriodic;
	}



//
//ConstructL()
//
// Second-phase constructor, initialises the periodic member, and starts it
// running.
//
void CMVSViewTicker::ConstructL()
	{
	iPeriodic = CPeriodic::NewL(0);
	iPeriodic->Start(iTickInterval,iTickInterval,TCallBack(Tick, this));
	}



//
//Tick(...)
//
// The callback function, which is called upon a periodic timer event. This
// method calls the UpdatePositionViewL() in the AppUi
//
TInt CMVSViewTicker::Tick(TAny* aObject)
    {
    TRAPD(err,((CMVSViewTicker*)aObject)->iAppUi->UpdatePositionViewL());
    return err;
    }
