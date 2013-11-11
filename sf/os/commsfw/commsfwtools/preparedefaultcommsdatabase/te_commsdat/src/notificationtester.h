/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file notificationtester.h
*/
 
#if (!defined __NOTIFICATION_TESTER_H__)
#define __NOTIFICATION_TESTER_H__

#include <e32base.h>
#include <e32std.h>
#include "MetaDatabase.h"
#include "CommsDat.h"

using namespace CommsDat;
using namespace Meta;

class CNotificationTester:public CActive
	{
public:
	static CNotificationTester* NewL(const TDesC &aThreadName,CommsDat::CMDBSession* aSession, CommsDat::CMDBElement* aElement );
	~CNotificationTester();
	void SetTester();
	void LogOn(TRequestStatus &aStatus);
	void RequestStatus(TRequestStatus &aRequestStatus);
	void CancelNotification(TRequestStatus &aStatus);
private:
	CNotificationTester(CommsDat::CMDBSession* aSession, CommsDat::CMDBElement* aElement);
	void ConstructL(const TDesC &aThreadName);
	
	void RunL();
	void DoCancel();
	static TInt ThreadFunction(TAny *aPtr);
	static TInt DoThreadFunctionL(TAny *aPtr);
private:
	CommsDat::CMDBSession *iSession;
	CommsDat::CMDBElement *iElement;
	RThread iThread;
	TBool iThreadRunning;
	TInt iReason;
	};

#endif//__NOTIFICATION_TESTER_H__
