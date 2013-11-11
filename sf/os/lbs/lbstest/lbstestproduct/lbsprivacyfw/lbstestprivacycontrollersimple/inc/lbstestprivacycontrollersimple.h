/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file lbstestprivacycontrollersimple.h
*/
#ifndef __LBS_TEST_PRIVACY_CONTROLLER_SIMPLE_H__
#define __LBS_TEST_PRIVACY_CONTROLLER_SIMPLE_H__

#include <lbs/lbsprivacycontroller.h>

class CTestPrivacyControllerSimple : public CBase, public MLbsPrivacyObserver
	{
public:
	static CTestPrivacyControllerSimple* NewL();
	~CTestPrivacyControllerSimple();

	//From MLbsPrivacyObserver
	void ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType);
	void ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo);
	void ProcessRequestComplete(TUint aRequestId, TInt aReason);	
	
private:
	CTestPrivacyControllerSimple();
	void ConstructL();

private:
	// the lbs privacy controller interface:
	CLbsPrivacyController* iLbsPrivacyController;

	};

#endif // __LBS_TEST_PRIVACY_CONTROLLER_SIMPLE_H__
