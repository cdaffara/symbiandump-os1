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
// LbsModStatus.h
// 
//

#ifndef __LBSMODSTATUSHANDLER_H__
#define __LBSMODSTATUSHANDLER_H__

#include <e32base.h>
#include "LbsInternalInterface.h"


class MModuleStatusObserver;

class CModStatusHandler :	public CActive
	{
public:
	static CModStatusHandler* NewL(TUid aModuleId, MModuleStatusObserver* aObserver);
	
	~CModStatusHandler();
	void StartGettingStatusChanges(TRequestStatus& aStatus,
						TPositionModuleStatusEventBase& aStatusEvent);
	TUid ModuleId();
	TRequestStatus*& RequestStatusOfClient();
protected:
	CModStatusHandler(TUid aModuleId, MModuleStatusObserver* aObserver); 
	void ConstructL(TUid aModuleId);
	
	// From CActive
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);
	
private:
	RLbsModuleStatus 				iModStatus;
	TUid 							iModuleId;
	TPositionModuleStatusEventBase*	iStatusEvent;
    TRequestStatus* 				iRequestStatusOfClient;
    MModuleStatusObserver* 			iObserver;
	};



#endif //__LBSMODSTATUSHANDLER_H__
