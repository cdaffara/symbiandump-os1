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


#ifndef CLBSLOCMONITORAREAINFOFINDER_H
#define CLBSLOCMONITORAREAINFOFINDER_H

#include "tserverstartparams.h"
#include "LbsInternalInterface.h"
#include "LbsLogger.h"
#include "mlbslocmonitorareainfofinderobserver.h"

class TLbsLocMonitorAreaInfoBase;

/**
Base class for all the classes used by the Location Monitor to 
find area information (e.g. one such subclass is the 
CLbsLocMonitorCellInfoFinder that obtains cell information 
from ETEL)

@internalComponent
@released
*/
class CLbsLocMonitorAreaInfoFinder : public CActive
	{

public:
	void RegisterObserverL(const MLbsLocMonitorAreaInfoFinderObserver& aObserver);
	
	virtual TInt RunError(TInt aError) = 0;
	virtual void RunL() = 0;
	virtual void DoCancel() = 0;
	
	~CLbsLocMonitorAreaInfoFinder();

protected:
	CLbsLocMonitorAreaInfoFinder();
	
	void NotifyObservers(const TLbsLocMonitorAreaInfoBase& aAreaUpdate);
	
private:

	RPointerArray<MLbsLocMonitorAreaInfoFinderObserver> iObservers;
	};
	
#endif //CLBSLOCMONITORAREAINFOFINDER_H
