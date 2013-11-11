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

#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

class CSubscriber : public CActive
/**
Example subscriber
@internalComponent
*/
	{
public:
	static CSubscriber* NewL(RTest& aTest, TUint aKey, const TDesC& aString);
	void Start();

	void RunL();
	void DoCancel();

	~CSubscriber();

protected:
	CSubscriber(RTest&, const TDesC&);
	void ConstructL(TUint aKey);
protected:
	RProperty	iProperty;
	RTest&		iTest;
	const TDesC&	iString;
	};

class CDeviceNameSubscriber : public CSubscriber
/**
Example subscriber for DeviceName
@internalComponent
*/
	{
public:
	static CDeviceNameSubscriber* NewL(RTest& aTest, TUint aKey, const TDesC& aString);

	void RunL();

	~CDeviceNameSubscriber();

protected:
	CDeviceNameSubscriber(RTest&, const TDesC&);

	};

#endif //SUBSCRIBER_H
