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
//


#ifndef PHYSICALLINKMETRICHELPERS_H
#define PHYSICALLINKMETRICHELPERS_H

#include <bt_sock.h>
#include <bluetooth/lmoptions.h>

NONSHARABLE_CLASS(CBluetoothPhysicalLinkMetricSubscriber): public CActive
	{
public:
	static CBluetoothPhysicalLinkMetricSubscriber* NewL(
								CBluetoothPhysicalLinkMetrics& aParent,
								RSocketServ& aSockServ,
								RSocket& aConnectedSocket);
	
	static CBluetoothPhysicalLinkMetricSubscriber* NewL(
									CBluetoothPhysicalLinkMetrics& aParent,
									RSocketServ& aSockServ,
									const TBTDevAddr& aAddr);

	~CBluetoothPhysicalLinkMetricSubscriber();
	
	void SubscribePhysicalLinkMetric(TBTLMIoctls aIoctl);
	
private:
	CBluetoothPhysicalLinkMetricSubscriber(CBluetoothPhysicalLinkMetrics& aParent);
	
	void ConstructL(RSocketServ& aSockServ,	RSocket& aConnectedSocket);
	void ConstructL(RSocketServ& aSockServ,	const TBTDevAddr& aAddr);
	
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();

private:
	RBTBaseband iBaseband;
	CBluetoothPhysicalLinkMetrics& iParent;
	TBTLMIoctls iPhysicalLinkMetric;
	TPckgBuf<TInt> iValue;
	};

#endif // PHYSICALLINKMETRICHELPERS_H
