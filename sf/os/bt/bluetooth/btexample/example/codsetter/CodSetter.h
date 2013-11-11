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

#ifndef DISC_H
#define DISC_H


static const TBTMajorDeviceClass KMajorDeviceClass = EMajorDevicePhone;
static const TBTMinorDeviceClassPhone KMinorDeviceClass = EMinorDevicePhoneCellular;


class CCodSetter : public CActive
/**
Example subscriber
@internalComponent
*/
	{
public:
	static CCodSetter* NewL(RTest& aTest);
	void Start();

	void RunL();
	void DoCancel();

	~CCodSetter();

private:
	CCodSetter(RTest&);
	void ConstructL();
private:
	RTest&					iTest;
	RProperty				iProperty;
	TBTMajorServiceClass	iServiceClassToSet; //run BTProperties to see result
	};

#endif //DISC
