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
// ao wrapper around the a-gps mgr function NotifyPositionUpdate
// 
//

/**
 @file ctlbsx3pdummyao.cpp
*/

#include <lbs/lbsx3p.h>
#include "ctlbsx3pdummyao.h"


CT_LbsX3PDummyAO* CT_LbsX3PDummyAO::NewL(MT_LbsX3PDummyObserver* aObserver)
/**
 *	'public constructor' may leave
 */
	{
	CT_LbsX3PDummyAO* dummyAO = new(ELeave)CT_LbsX3PDummyAO(aObserver);
	return dummyAO;
	}


CT_LbsX3PDummyAO::CT_LbsX3PDummyAO(MT_LbsX3PDummyObserver* aObserver) : iObserver(aObserver), CActive(EPriorityStandard)
/**
 * 	Constructor - will not leave
 */
	{ 	
	CActiveScheduler::Add(this); 
	//Since it is dummy active object, Set it active now so that active scheduler can start it immediately.
	SetActive(); 
	}

CT_LbsX3PDummyAO::~CT_LbsX3PDummyAO()
/**
 * 	Destructor
 */
	{
	DoCancel();

	}

void CT_LbsX3PDummyAO::DoCancel()
	{
	Cancel();		//Not sure about this...might have to change

	}


void CT_LbsX3PDummyAO::RunL()
	{
	TInt err = iStatus.Int();
	User::LeaveIfError(err);	// remove if we wish to process the err

	// iStatus will contain error code
	// async request completed. Notify caller via callback:
	if (iObserver)
		{
		iObserver->MT_LbsX3PDummyCallbackHandler(iStatus);
		}
	}

TInt CT_LbsX3PDummyAO::RunError(TInt aError)
	{
	return aError;
	}

