// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SIMPHONEUSIMAPP_H_
#define __SIMPHONEUSIMAPP_H_

/**
 * @file
 *
 * Contains the class definitions required for the Phone USim applications.
 * @internalAll
 */

#include <etelmm.h>
#include <et_phone.h>
#include <mpbutil.h>
#include "csimtimer.h"
#include "CSimPhone.h"

const TInt KMaxUSimApps = 3;

/**
 * The main Phone USim applications class which simulates USim applications features.
 */
class CSimPhone;
class CTestConfigSection;
class CTestConfigItem;
class CSimPhoneUSimApp : public CSubSessionExtBase, public MTimerCallBack
	{
public:
	static CSimPhoneUSimApp* NewL(CSimPhone* aPhone);
	CSimPhoneUSimApp(CSimPhone* aPhone);
	~CSimPhoneUSimApp();

	virtual CTelObject::TReqMode ReqModeL(const TInt aIpc);
// MTelObjectTSY pure virtual
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual void Init();
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);

// MSubSessionExtBaseTSY pure virtual
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual void TimerCallBack(TInt aId);
	const CTestConfigSection* CfgFile();
	RMobilePhone::TAID GetActiveUSim();
	
	TBool FoundUSimAppTags();

protected:
	void ConstructL();
	
private:
	class TUSimAppEvent 
	{
	public:
		TUint32 iDuration;
		RMobilePhone::TAID iAid;
	};
	
	class TUSimNotifyData
	{	
	public:
		TBool iNotifyPending;  //indicates if a notification is pending
		TTsyReqHandle iNotifyHandle; //the TSY notification handle
		TAny* iNotifyData;  //Holds the data for the notification pending
		TAny* iNotifyDataXtra;  //Holds the data for the notification pending
	};

	TInt EnumerateUSimApplications(const TTsyReqHandle aTsyReqHandle, TInt* aCount, RMobilePhone::TAID* aActiveAID);
	TInt EnumerateUSimApplicationsCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetUSimApplicationInfo(const TTsyReqHandle aTsyReqHandle, TInt* aUSimAppIndex, TDes8* aInfo);
	TInt GetUSimApplicationInfoCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyUSimApplicationsInfoChange(const TTsyReqHandle aTsyReqHandle, TInt* aCount, RMobilePhone::TAID* aActiveAID);
	TInt NotifyUSimApplicationsInfoChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetUSimApplicationStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TAID* aAID, RMobilePhone::TUSimAppAction* aUsimAppAction);
	TInt SetUSimApplicationStatusCancel(const TTsyReqHandle aTsyReqHandle);
	TInt SetUSimAppsSelectionMode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TUSimSelectionMode* aUSimSelectionMode);
	TInt GetUSimAppsSelectionMode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TUSimSelectionMode* aUSimSelectionMode);
	TInt NotifyUSIMAppsSelectionModeChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TUSimSelectionMode* aUSimSelectionMode);
	TInt NotifyUSIMAppsSelectionModeChangeCancel(const TTsyReqHandle aTsyReqHandle);

	
private:
	CSimPhone* iPhone;
	CSimTimer* iTimer;
	RMobilePhone::TUSimSelectionMode iCurrentSelection;
	RMobilePhone::TAID iActiveAID;
	CArrayFixFlat<TUSimAppEvent>* iUSimAppEventList;
	TInt iEventPointer;
	CArrayFixFlat<RMobilePhone::TUSimApplicationInfoV2>* iUSimAppInfo;
	TUSimNotifyData iNotifyInfo;
	TNotifyData iNotifyMode;
	TBool iTimerStarted;
	TBool iFoundUSimAppTags;
	};

#endif
