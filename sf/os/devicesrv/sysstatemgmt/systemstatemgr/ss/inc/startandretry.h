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

#ifndef __STARTANDRETRY_H__
#define __STARTANDRETRY_H__

#include <e32base.h>

#include "startandretrynotifications.h"
#include "rvobservernotification.h"
#include "ssconst.h"



class CApaStarter;
class CRvObserver;
class CSsmStartupProperties;

NONSHARABLE_CLASS(  CStartAndRetry ) : public CBase, public MRvObserverNotification
	{
public:
	static CStartAndRetry* NewL( MStartAndRetryNotifications& aStartSafeNotify, const CSsmStartupProperties& aStartupProperties, RProcess& aProcess, TInt aCallerIndex, CApaStarter* aApaStarter );
	static CStartAndRetry* NewLC( MStartAndRetryNotifications& aStartSafeNotify, const CSsmStartupProperties& aStartupProperties, RProcess& aProcess, TInt aCallerIndex, CApaStarter* aApaStarter );
	~CStartAndRetry();
	
	void Actuate();
	// From MRvObserverNotification
	void NotifyCompletion( const TInt aCompletion );
	void NotifyCancellation( TInt aCompletion );
	
private:
	CStartAndRetry( MStartAndRetryNotifications& aStartSafeNotify, RProcess& aProcess, TInt aCallerIndex, CApaStarter* aApaStarter );
	void ConstructL( const CSsmStartupProperties& aStartupProperties );
	void DoFireAndForgetL();
	void DoWaitForStartL();
	
private:
	TInt iCallerIndex;
	RProcess& iProcess;
	CRvObserver* iRvObserver;
	MStartAndRetryNotifications& iStartSafeNotify; // We are owned by
	CApaStarter* iApaStarter; // Not owning. Might be NULL, therefore ptr.
	CSsmStartupProperties* iStartupProperties;
	};



#endif // __STARTANDRETRY_H__
