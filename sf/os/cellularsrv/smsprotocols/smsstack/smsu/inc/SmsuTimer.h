// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Declares CSmsuTimeout
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SMSU_TIMER_H__
#define SMSU_TIMER_H__

#include <e32base.h>

#include "smsuact.h"


/**
 *  Asynchronous utility class that times out
 *  another active object given during construction.
 *  @internalComponent
 */
class CSmsuTimeout : public CTimer
	{
public:
	static CSmsuTimeout* NewL(CSmsuActiveBase& aActive);
	~CSmsuTimeout();

	void Start(const TTimeIntervalMicroSeconds32& aTimeIntervalMicroSeconds32);

	inline TBool TimedOut() const {return iTimedOut;};

	void RequestCompleteMyselfAfterCancel(TInt aStatus);

private:
	CSmsuTimeout(CSmsuActiveBase& aActive);
	void RunL();

protected:
	CSmsuActiveBase&  iActive;        //< reference to the timer observer
	TBool  iTimedOut;                 //< ETrue if it actually timed out
	TBool  iCompleteMyselfRequested;  //< ETrue if a restart after cancel needed
	TInt  iCompleteMyselfStatus;      //< Restart status value.
	};

#endif // SMSU_TIMER_H__
