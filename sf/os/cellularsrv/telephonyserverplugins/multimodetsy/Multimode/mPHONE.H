// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalAll 
*/

#ifndef __MPHONE_H__
#define __MPHONE_H__

#include "PHONE.H"
#include <etelmm.h>
#include "mnetwork.h"
#include "Msignal.h"

class CMobileSmsMessaging;
class CATPhoneBookInfo;
class CATGetBattery;
class CATGetSignal;
class CATNetworkInfo;
class CATSubscriberId;
class CATOwnNumbers;
class CATDetectNetwork;
class CGprs;


class CListReadAllAttempt : public CBase
	{
public:
	static CListReadAllAttempt* NewL(RMobilePhone::TClientId* aId, TTsyReqHandle aTsyReqHandle);
	~CListReadAllAttempt();

protected:
	CListReadAllAttempt(RMobilePhone::TClientId* aId, TTsyReqHandle aTsyReqHandle);

private:
	void ConstructL();

public:
	CBufBase* iListBuf;
	RMobilePhone::TClientId iClient;
	TTsyReqHandle iReqHandle;
	};


class CPhoneMobile : public CPhoneHayes
	{
public:
	static CPhoneMobile* NewL();
	~CPhoneMobile();

	static void ClosePhone(TAny* aObj);

	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle);
	virtual TReqMode ReqModeL(const TInt aIpc);
	virtual CTelObject* OpenNewObjectL(TDes& aName);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual TInt ControlledInitialisation(const TTsyReqHandle aTsyReqHandle);
	
	RMobilePhone::TMobilePhoneRegistrationStatus RegistrationStatus() const;
	void CurrentNetworkInfo(CATNetworkInfo::TTsyNetworkInfo* aNetworkInfo) const;
	// Callback function for CATDetectNetworks to signal cancel completion
	void CompleteDetectedNetworksCancel(const TTsyReqHandle aTsyReqHandle);
	TInt MultimodeInitL(TBool aExplicit);


	RMobilePhone::TMMTableSettings iTableSettings;

private:
	void ConstructL();
	//
	// Multimode extension
	//
	TInt GetPhoneId(const TTsyReqHandle aTsyReqHandle,RMobilePhone::TMobilePhoneIdentityV1* aId);
	TInt GetIdentityCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aIdentityCaps);
	TInt GetSignalCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt GetSignalStrength(const TTsyReqHandle aTsyReqHandle, TInt32* aSignalStrength, TInt8* aBar); 
	TInt GetSignalStrengthCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetMultimodeCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aMultimodeCaps);
	TInt GetPhoneStoreInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aInfo,TDesC* aName);
	TInt GetPhoneStoreInfo(const TTsyReqHandle aTsyReqHandle, TDes8* aStoreNameIccType,TDes8* aInfo);
	TInt GetNetworkRegistrationStatus(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneRegistrationStatus* aRegistrationStatus);
	TInt NotifyNetworkRegistrationStatusChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneRegistrationStatus* aRegistrationStatus);

	TInt NotifyNetworkRegistrationStatusChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetNetworkCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aNetworkCaps);
	TInt GetCurrentMode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneNetworkMode* aNetworkMode);
	TInt GetCurrentNetwork(const TTsyReqHandle aTsyReqHandle, TDes8* aNetworkInfoPckg,
						   RMobilePhone::TMobilePhoneLocationAreaV1* aArea);
	TInt NotifyCurrentNetworkChange(const TTsyReqHandle aTsyReqHandle,TDes8* aNetworkInfoPckg,
						   RMobilePhone::TMobilePhoneLocationAreaV1* aArea);

	TInt NotifyCurrentNetworkChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt GetDetectedNetworksPhase1L(TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSiz);
	TInt GetDetectedNetworksPhase1(TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TInt* aBufSiz); //hs
	TInt GetDetectedNetworksPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf);
	TInt GetBatteryCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aBatteryCaps);

	TInt GetBatteryInfo(const TTsyReqHandle aTsyReqHandle,RMobilePhone::TMobilePhoneBatteryInfoV1* aBatteryInfo);
	TInt GetBatteryInfoCancel(const TTsyReqHandle aTsyReqHandle);
	TBool IsListedPhoneBook(const TDesC& aName,TDes8& aStorageType,TBool& aSupported) const;
	TInt GetSubscriberId(TTsyReqHandle aTsyReqHandle,RMobilePhone::TMobilePhoneSubscriberId* aId);
	RMobilePhone::TMobilePhoneNetworkMode GetMode();
	TInt ExplicitInitialisationL(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMMTableSettings* aSettings);
	TInt ExplicitInitialisationCancel(const TTsyReqHandle aTsyReqHandle);

private:
	CGprs* iGprsService;
	CMobileSmsMessaging* iSmsMessaging;
	CATPhoneBookInfo* iPhoneBookInfo;
	CATGetBattery* iATBattery;
	CATGetSignal* iATSignal;
	CATNetworkInfo* iATNetworkInfo;
	CATSubscriberId* iATSubscriberId;
	CATOwnNumbers* iATOwnNumbers;
	CATDetectNetwork* iATDetectNetwork;
	CArrayPtrFlat<CListReadAllAttempt>* iGetDetectedNetworks;

	// Keep structure holding pointers to ETel server memory
	CATGetSignal::TTsySignalInfo iSignalPtrs;
	CATNetworkInfo::TTsyNetworkInfo iNetworkInfoPtrs;
	CATDetectNetwork::TTsyDetectedNetworks iNetworkListInfoPtrs;
	};

#endif



