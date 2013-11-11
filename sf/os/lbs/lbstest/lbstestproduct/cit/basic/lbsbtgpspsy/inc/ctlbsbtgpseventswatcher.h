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
// Watcher for BT GPS events
//

#ifndef __CT_LBSBTGPSEVENTSWATCHER_H__
#define __CT_LBSBTGPSEVENTSWATCHER_H__

#include <bt_sock.h>
#include <BtGpsPsyPrivatePSKeys.h>

/** Event observer interface 
*/
class MBtGpsEventsEventObserver
	{
public:
	virtual void OnEventsUpdated(const TUint aMask, TInt aValue) = 0;
	};

/** Event watcher 
*/
class CBtGpsEventsWatcher : public CActive
	{
public:
	static CBtGpsEventsWatcher* NewL(MBtGpsEventsEventObserver& aObserver, TUint aMask);
	~CBtGpsEventsWatcher();
	
protected:
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
		
private:
	CBtGpsEventsWatcher(MBtGpsEventsEventObserver& aObserver, TUint aMask);
	void ConstructL();
	
private:
	RProperty iProperty;
	MBtGpsEventsEventObserver& iObserver;
	TUint iEventsMask;
	};


#endif	//__CT_LBSBTGPSEVENTSWATCHER_H__
