// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Watcher for BT GPS Simulation test server
//

#ifndef __CT_LBSBTGPSSIMWATCHER_H__
#define __CT_LBSBTGPSSIMWATCHER_H__

#include <bt_sock.h>
#include "btgpssim.h"


/** Event observer interface 
*/
class MBtGpsSimEventObserver
	{
public:
	virtual void OnDeviceConnect(const TBTSockAddr& aAddr) = 0;
	virtual void OnDeviceDisconnect(const TBTSockAddr& aAddr) = 0;
	virtual void OnDeviceError(const TBTSockAddr& aAddr) = 0;
	};


/** Event watcher 
*/
class CBtGpsSimWatcher : public CActive
	{
public:
	static CBtGpsSimWatcher* NewL(MBtGpsSimEventObserver& aObserver);
	~CBtGpsSimWatcher();

	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
		
private:
	CBtGpsSimWatcher(MBtGpsSimEventObserver& aObserver);
	void ConstructL();
	
private:
	RBtGpsSim iSim;
	MBtGpsSimEventObserver& iObserver;
	TBtGpsSimEventInfoPckg iEventInfo;
	};


#endif	//__CT_LBSBTGPSSIMWATCHER_H__
