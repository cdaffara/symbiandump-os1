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
// Implementation of Location Based Services Administration API
// 
//

/**
 @file
 @internalTecnology
 @released
*/

#ifndef LBS_ADMIN_SUBSCRIBER_H
#define LBS_ADMIN_SUBSCRIBER_H

#include <e32base.h>
#include <f32file.h>
#include <s32file.h>
#include <e32property.h>
#include <lbs/lbsadmin.h>
#include <centralrepository.h>
#include <e32std.h>

// Internal values that specify the protocol array should be used 
const TLbsAdminSetting KLbsProtocolModulesHome		= KLbsSettingHomeProtocolModule + 1;
const TLbsAdminSetting KLbsProtocolModulesRoaming	= KLbsSettingRoamingProtocolModule + 1;

// Base central repository keys for the additional protocol modules
const TUint32 KLbsProtocolModulesHomeBase = 0x00000080;
const TUint32 KLbsProtocolModulesRoamingBase = 0x00000100;

//********************************************************************************************************
NONSHARABLE_CLASS(CAdminSettingSubscriber) : public CActive
	{
public:
	static CAdminSettingSubscriber* NewL(TUid aCategory, TLbsAdminSetting aSetting, MLbsAdminObserver& aObserver);
	static CAdminSettingSubscriber* NewLC(TUid aCategory, TLbsAdminSetting aSetting, MLbsAdminObserver& aObserver);
	~CAdminSettingSubscriber();
	
	inline TLbsAdminSetting GetSetting();
private:
	CAdminSettingSubscriber(TUid aCategory, TLbsAdminSetting aSetting, MLbsAdminObserver& aObserver);
	void ConstructL();
	

	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);

private:
	TUid					iCategory;
	TLbsAdminSetting		iSetting;
	MLbsAdminObserver&		iObserver;
	RProperty				iProperty;
	};


//********************************************************************************************************
NONSHARABLE_CLASS(CLbsAdminImpl) : public CBase
	{
public:		
	static CLbsAdminImpl* NewL();
	static CLbsAdminImpl* NewL(MLbsAdminObserver& aObserver, const TLbsAdminSettingGroup& aSettingsMask);
	virtual ~CLbsAdminImpl();
	
	void SetNotificationMaskL(const TLbsAdminSettingGroup& aSettingsMask);
	void ClearNotificationMask();
	
	TInt ResetToDefault();

	TInt SetSettingValidated(TLbsAdminSetting aSetting, const TInt& aValue);
	TInt SetInfoBaseValidated(TLbsAdminSetting aSetting, const TLbsAdminInfoBase& aInfo);
	TInt GetSettingValidated(TLbsAdminSetting aSetting, TInt& aValue);
	
private:
	CLbsAdminImpl(MLbsAdminObserver* aObserver);
	void ConstructL();
	void ConstructL(const TLbsAdminSettingGroup& aSettingsMask);

	void CheckCenRepL();
	void DefinePropertiesL();
	void DeleteSubscribers();
	void DeleteSubscribersNotInMask(TLbsAdminSettingGroup& aMask);
	TInt DefineArrayProperties(const TLbsAdminSetting aAdminSetting, TUint32 aKeyBase);
	TInt GetProtocolModulesValidated(TLbsAdminSetting aSetting, TInt& aValue, const TLbsAdminSetting aDefault);
	TInt SyncCenrep(TLbsAdminSetting aSetting);

private:
	MLbsAdminObserver* iObserver;
	RPointerArray<CAdminSettingSubscriber> iSubscribers;
	CRepository* iCenRep;
	RMutex iMutex;
	};

//********************************************************************************************************
NONSHARABLE_CLASS(LbsAdminUtils)
	{
public:
	static TUint32 BitPosToNumber(TLbsAdminSetting aSetting);
	//static TLbsAdminSetting NumberToBitPos(TUint32 aNumber);
	static TInt ConfirmSettingPluginExists(TLbsAdminSetting aSetting, TInt aValue);
	static TInt ConfirmPmPluginsExist(TLbsAdminSetting aSetting, TLbsAdminProtocolModulesInfo::TLbsProtocolModuleIdArray aArray, TInt aCount);
	static TInt ValidateValue(TLbsAdminSetting aSetting, const TInt& aValue);
	};


#include "lbsadminimpl.inl"

#endif //LBS_ADMIN_SUBSCRIBER_H
