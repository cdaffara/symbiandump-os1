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
//

#ifndef LASMANAGER_H
#define LASMANAGER_H

#include <e32std.h>

class CLbsLocationSourceGpsObserver;
class CLbsLocationSourceGpsBase;
class CLasRequestHandler;

class CLocationManagerGps : CBase
	{
public:
	static CLocationManagerGps* NewL(TInt aModuleNum);
	~CLocationManagerGps();
	
	void NotifyServiceToClose(TRequestStatus* aStatus);

protected:
	CLocationManagerGps(TInt aModuleNum);
	void ConstructL();

protected:
	const TInt iModuleNum;
	CLbsLocationSourceGpsObserver*   iMonitor;
	CLbsLocationSourceGpsBase*			iLasModule;
	CLasRequestHandler*					iRequestHandler;
	};
	
#endif //LASMANAGER_H