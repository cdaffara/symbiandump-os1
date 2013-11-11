/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MLBSLOCMONITORAREAINFOFINDEROBSERVER_H
#define MLBSLOCMONITORAREAINFOFINDEROBSERVER_H

#include "tserverstartparams.h"
#include "LbsInternalInterface.h"
#include "LbsLogger.h"
#include "lbslocmonitorareainfodefs.h"


/**
Mix-in class that all observers of a CLbsLocMonitorAreaInfoFinder
objects will derive from in order to obtain updates on area information
(e.g, global cell id)

@internalComponent
@released
*/
class MLbsLocMonitorAreaInfoFinderObserver
	{
	
public:

	virtual void AreaInfoUpdate(const TLbsLocMonitorAreaInfoBase& aAreaInfo) = 0;

	};
	
#endif //MLBSLOCMONITORAREAINFOFINDEROBSERVER_H
