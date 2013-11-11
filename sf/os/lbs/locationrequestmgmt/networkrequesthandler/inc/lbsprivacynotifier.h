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

/**
 @file
 @internalTechnology
 @released
*/

#ifndef __LBSPRIVACYNOTIFIER_H__
#define __LBSPRIVACYNOTIFIER_H__

#include <e32std.h>
#include <e32def.h>
#include <e32base.h>
#include <lbs/lbsprivacyextnotifiers.h>

//forward declaration
class CLbsPrivacyNotifierImpl;


/**
Privacy Notification dialog type

@internalTechnology
@released
*/
enum TPrivacyDialogType
	{
	EPrivacyDialogNotification,
	EPrivacyDialogVerification
	};	

/**
Observer class for Privacy Notification Server

@internalTechnology
@released
*/
class MLbsPrivacyNotifierObserver
{
public:
	//Pure virtual implemented by classes which inherit from MLbsPrivacyNotifierObserver
	virtual void OnNotificationDialogResponse(TInt aErr, const TLbsPrivacyNotifierResponse& aResponse) = 0;
};


/**
Privacy Notification Dialog API

@internalTechnology
@released
*/
class CLbsPrivacyNotifier : public CBase
	{
public:
	IMPORT_C static CLbsPrivacyNotifier* NewL(TPrivacyDialogType aDlg);
	IMPORT_C static CLbsPrivacyNotifier* NewL(TPrivacyDialogType aDlg, TTimeIntervalMicroSeconds32 aTimeout);
	IMPORT_C ~CLbsPrivacyNotifier();
	IMPORT_C void SetObserver(MLbsPrivacyNotifierObserver* aObserver);
	IMPORT_C void DisplayL(TInt& aErr, const TLbsExternalRequestInfo& aRequestInfo);
	IMPORT_C void Cancel();

protected:
	CLbsPrivacyNotifier();
	void ConstructL(TPrivacyDialogType aDlg);
	void ConstructL(TPrivacyDialogType aDlg, TTimeIntervalMicroSeconds32 aTimeout);
private:
	//handle to dialog implementation
	CLbsPrivacyNotifierImpl* iBody;
	};


#endif // __LBSPRIVACYNOTIFIER_H__
