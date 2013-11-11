// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of class used to shut down a server after a specified time
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef __LBSSHUTDOWNTIMER_H__
#define __LBSSHUTDOWNTIMER_H__

#include <e32base.h>

/**
@internalTechnology
@released

 CTimer base class, it is used to shut down the server in specified time span
 */
class CShutdownTimer : public CTimer
	{
public:
	CShutdownTimer();
	~CShutdownTimer();
	void ConstructL();
	void Start(const TTimeIntervalMicroSeconds32 aDelay);
	void Stop();
protected: 
	// from CActive
	void RunL();
	};
	
#endif // __LBSSHUTDOWNTIMER_H__
