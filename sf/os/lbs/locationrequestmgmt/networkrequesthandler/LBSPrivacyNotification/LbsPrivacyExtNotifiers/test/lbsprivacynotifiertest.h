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

#ifndef LBSPRIVACYNOTIFIERTEST_H_
#define LBSPRIVACYNOTIFIERTEST_H_

class CLbsPrivacyNotifierTest : public CBase, public MLbsPrivacyNotifierObserver
	{
public:
	static CLbsPrivacyNotifierTest* NewL();
	
	~CLbsPrivacyNotifierTest();
	
	void DoDialogTestL();
	
	// From MLbsPrivacyNotifierObserver
	void OnNotificationDialogResponse(TInt aErr, const TLbsPrivacyNotifierResponse& aResponse);
	
private:
	CLbsPrivacyNotifierTest();
	void ConstructL();
	
private:
	CLbsPrivacyNotifier* iNotifyNotifier;
	CLbsPrivacyNotifier* iVerifyNotifier;
	};
	
#endif // LBSPRIVACYNOTIFIERTEST_H_
