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
// Declares the CSimSysAgent class.
// 
//

/**
 @file
 @internalAll
*/

#ifndef __CSIMPUBSUB_H__
#define __CSIMPUBSUB_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include <e32property.h>

//forward declarations
class MPSSimObserver;

/**
class CSimPubSub

Requests notification of changes to call status using Publish&Subscribe

This class is a replica of CSimSysAgent but uses the Publish&Subscribe notification mechanism instead of SysAgent

The class is a simple active object derived state machine.
*/
class CSimPubSub : public CActive
	{
public:
	class TPubSubProperty
	/**
	Capture the uids required to subscribe to a property.
	*/
		{
	public:
		TPubSubProperty();
		TPubSubProperty(TUid aCategory, TInt aKey,	TInt aType);
		TBool operator==(const TPubSubProperty& aProperty) const;
	public:
		TUid iCategory;
		TInt iKey;
		TInt iType;
		};

public:
	static CSimPubSub* NewL(MPSSimObserver* aPSSimCallBack, const TPubSubProperty aProperty);
	~CSimPubSub();
	void Start();

protected:
	void RunL();
	void DoCancel();	
	
private:
	CSimPubSub(MPSSimObserver* aPSSimCallBack, const TPubSubProperty aProperty);
	void ConstructL();


private:
	const CSimPubSub::TPubSubProperty iProperty;
	MPSSimObserver* iPSSimCallBack;	
	RProperty iSubscribe;		
	};

class MPSSimObserver
	{
public:
	// Callback when a call event has occurred.
	virtual void SimPSEvent(const CSimPubSub::TPubSubProperty aProperty, TInt aStatus)=0;
	};

#endif // !defined __CSIMPUBSUB_H__
