// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef CMULTIPLESURFACES_H
#define CMULTIPLESURFACES_H


#include "multiplesurfacesapp.h"
#include "surfaceutility.h"

#include <e32msgqueue.h>


class CTWindow;


/**
 *  CMultipleSurfaces
 * 
 */
class CMultipleSurfaces : public CBase
	{
public:
	~CMultipleSurfaces();
	static CMultipleSurfaces* NewL(RWsSession& aWs, RWindowGroup& aWindowGroup, TPtrC aBackgroundColor, 
	        CMultipleSurfacesAppUi* aMultipleSurfacesAppUi, TPtrC aParentQueueName);
	void Start();
	void Stop();

private:
	CMultipleSurfaces(RWsSession& aWs, RWindowGroup& aWindowGroup, CMultipleSurfacesAppUi* aMultipleSurfacesAppUi);
	void ConstructL(TPtrC aBackgroundColor, TPtrC aParentQueueName);
	void RenderL();
	static TInt TimerCallBack(TAny* aMultipleSurfaces);

private:
    RWsSession& iWs;
    RWindowGroup& iWindowGroup;
    CMultipleSurfacesAppUi* iMultipleSurfacesAppUi;
    
	CPeriodic* iTimer;
	RArray<CTWindow*>  iSmallWindows;
	RMsgQueue<TInt> iMsgQueue;
	RMsgQueue<TInt> iParentMsgQueue;
	};

#endif // CMULTIPLESURFACES_H
