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
//

#ifndef __CSIM_PUB_SUB_CHANGE_H__
#define __CSIM_PUB_SUB_CHANGE_H__

#include <e32base.h>
#include <e32property.h>
#include "csimtimer.h"
#include "CSimPubSub.h"
#include "simtsyglobalproperties.h"
#include "SimTsy.h"

enum TNotificationType
	{
	ETimer,
	EPublishAndSubscribe
	};

class MPubSubChangeCallback
	{
	public:
	virtual void PubSubCallback(TInt aId) = 0;
	};
	
class CSimPubSubChange : public CActive
	{
	public:
	static CSimPubSubChange* NewL(MPubSubChangeCallback* aCallback, CSimPubSub::TPubSubProperty aProperty);
	static CSimPubSubChange* NewL(MPubSubChangeCallback* aCallback);//, CSimPubSub::TPubSubProperty aProperty);
	void ConstructL();
	~CSimPubSubChange();
	
	private:
	CSimPubSubChange(MPubSubChangeCallback* aCallback, CSimPubSub::TPubSubProperty aProperty);
	
	void RunL();
	void DoCancel();
	
	private:
	MPubSubChangeCallback* iCallback;
	RProperty iProp;
	const CSimPubSub::TPubSubProperty iPSProperty;
	};
	
#endif //__CSIM_PUB_SUB_CHANGE_H__
