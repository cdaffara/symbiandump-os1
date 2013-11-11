// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TIMEOUTWAITER_H__
#define __TIMEOUTWAITER_H__

#include <e32base.h>
#include "timeoutwaiternotification.h"



NONSHARABLE_CLASS( CTimeoutWaiter ) : public CTimer
	{
public:
	static CTimeoutWaiter* NewL( TInt aTimeout, MTimeoutWaiterNotification& aTimed );
	static CTimeoutWaiter* NewLC( TInt aTimeout, MTimeoutWaiterNotification& aTimed );
	~CTimeoutWaiter();
	
	void ActuateTimer();
	
protected:
	void RunL();

private:
	CTimeoutWaiter( TInt aTimeout, MTimeoutWaiterNotification& aTimed );
	void ConstructL();
	
private:
	TInt iTimeout;
	MTimeoutWaiterNotification& iTimed;
	};



#endif // __TIMEOUTWAITER_H__