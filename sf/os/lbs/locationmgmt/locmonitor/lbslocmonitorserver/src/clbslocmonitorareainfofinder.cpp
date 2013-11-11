/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

	
	
#include <e32base.h>
#include "lbsdevloggermacros.h"
#include "clbslocmonitorareainfofinder.h"

CLbsLocMonitorAreaInfoFinder::CLbsLocMonitorAreaInfoFinder():
							  CActive(EPriorityStandard)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorAreaInfoFinder::CLbsLocMonitorAreaInfoFinder");
	}

CLbsLocMonitorAreaInfoFinder::~CLbsLocMonitorAreaInfoFinder()
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorAreaInfoFinder::~CLbsLocMonitorAreaInfoFinder");
	iObservers.Reset();
	}

void CLbsLocMonitorAreaInfoFinder::RegisterObserverL(const MLbsLocMonitorAreaInfoFinderObserver& aObserver)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorAreaInfoFinder::RegisterObserverL");
	iObservers.AppendL(&aObserver);
	}


/*
This methods sends the aAreaUpdate to all of the observers. It is called by derived classes when
they have new area information to pass on to the observers.
*/
void CLbsLocMonitorAreaInfoFinder::NotifyObservers(const TLbsLocMonitorAreaInfoBase& aAreaUpdate)
	{
	LBSLOG(ELogP1,"->CLbsLocMonitorAreaInfoFinder::NotifyObservers");
	TInt index(0);
	TInt count(iObservers.Count());
	while (index < count)
		{
		iObservers[index]->AreaInfoUpdate(aAreaUpdate);
		index++;
		}
	}

