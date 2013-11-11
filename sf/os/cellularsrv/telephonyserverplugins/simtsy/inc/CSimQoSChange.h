// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CSimNetStatChange.H
// 
//

#ifndef __CSIMNETSTATCHANGE_H__
#define __CSIMNETSTATCHANGE_H__

#include <e32base.h>
#include <e32property.h>
#include "CSimTimer.h"
#include "CSimPubSub.h"

class MQoSChangeCallBack
	{
	public:
	virtual void QoSChangeCallBack(TInt aId) = 0;
	};
	
class CSimQoSChange : public CActive
	{
	public:
	static CSimQoSChange* NewL(MQoSChangeCallBack* aCallback, CSimPubSub::TPubSubProperty aProperty);
	void ConstructL();
	~CSimQoSChange();
	
	private:
	CSimQoSChange(MQoSChangeCallBack* aCallback, CSimPubSub::TPubSubProperty aProperty);
	
	void RunL();
	void DoCancel();
	
	private:
	RTimer iTimer;
	TBool iNetStat;
	MQoSChangeCallBack* iCallback;
	RProperty iProp;
	const CSimPubSub::TPubSubProperty iPSProperty;
	};
	
#endif //__CSIMNETSTATCHANGE_H__
