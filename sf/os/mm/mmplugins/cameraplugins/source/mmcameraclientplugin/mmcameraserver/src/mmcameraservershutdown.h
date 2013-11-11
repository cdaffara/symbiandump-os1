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

/**
 @file
 @internalTechnology
*/

#ifndef MMCAMERASERVERSHUTDOWN_H
#define MMCAMERASERVERSHUTDOWN_H

#include <e32base.h>

class CMMCameraServerShutdown : public CTimer
	{
	enum {KMMServerShutdownDelay = 0x200000};  // approx 2s
	
public:
	static CMMCameraServerShutdown* NewL();
	~CMMCameraServerShutdown();

	void Start();
	
private:
	CMMCameraServerShutdown();
	void ConstructL();
	void RunL();
	};

#endif // MMCAMERASERVERSHUTDOWN_H
