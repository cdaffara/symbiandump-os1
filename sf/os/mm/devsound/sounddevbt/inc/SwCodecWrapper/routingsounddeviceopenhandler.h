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

#ifndef __ROUTINGSOUNDDEVICEOPENHANDLER_H__
#define __ROUTINGSOUNDDEVICEOPENHANDLER_H__

#include <mmfbthwdevice2.h>

class CMMFSwCodecDataPath; //forward reference

class CRoutingSoundPlayDevice;
class CRoutingSoundRecordDevice;
class CMMFSwCodecWrapper;

/**
AO to handle asynch initialisation of the routing sound device

@internalComponent
*/ 
class CRoutingSoundDeviceOpenHandler : public CActive
	{
public:
	static CRoutingSoundDeviceOpenHandler* NewL(CMMFSwCodecWrapper* aObserver);
	~CRoutingSoundDeviceOpenHandler();
	void Start();
private:
	void RunL();
	void DoCancel();
	CRoutingSoundDeviceOpenHandler(CMMFSwCodecWrapper* aObserver);
	void ConstructL();
private:
	CMMFSwCodecWrapper* iObserver;
	};

#endif //__ROUTINGSOUNDDEVICEOPENHANDLER_H__

