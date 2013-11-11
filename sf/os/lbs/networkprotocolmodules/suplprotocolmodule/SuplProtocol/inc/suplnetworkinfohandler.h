// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of network information handler sub-component of
// SUPL Protocol Module
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/

#ifndef SUPLNEWORKTINFOHANDLER_H
#define SUPLNEWORKTINFOHANDLER_H

#include <etelmm.h>
#include <e32property.h>
#include <commsdattypesv1_1.h>
#include "suplcellinfohandler.h"

class MSuplNetworkInfoObserver
	{
public:
	virtual void NetInfoResults(const RMobilePhone::TMobilePhoneNetworkInfoV1& aNetworkInfo,
						   const RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea) = 0;

	virtual void HomeMccMncResult(TUint aMcc, TUint aMnc) = 0;
	
	virtual void MsisdnResult(const TDesC& aTelNumber) = 0;
	
	virtual void CellInfoResults(const RMobilePhone::TMobilePhoneCellInfoV9& aCellInfo) = 0;
	};


/** CSuplNetworkInfoHandler

This class uses ETEL's RMobilePhone subsession to obtain current network and 
subscriber information needed by SUPL (e.g. Mobile Country Code, Local Area Code,
CellId, IMSI, Timing Advance...).

This class is an Active Object. A number of states (@see TSuplNetInfoHandlerState) have
been defined to conduct all the necessary asynchronous calls to ETEL that provide the
network and subscriber information needed by the SUPL Protocol Module.

This AO will eventually monitor changes to network information using ETEL's method
NotifyCurrentNetworkChange().

This class also owns an object of type CSuplCellInfoHandler used to monitor changes 
to Timing Advance with NotifyCellInfoChange() which is only instantiated when the TSY
supports such method.
*/
NONSHARABLE_CLASS(CSuplNetworkInfoHandler) : public CActive
	{

public:

	/** CSuplNetworkInfoHandler states.
	*/
	enum TSuplNetInfoHandlerState
		{
		/** Not valid */
		EStateNull = 0,
		/** Waiting for Comms RootServer to be up and running */
		EStateInitializing = 1,
		/** Getting network info from ETEL with GetCurrentNetwork */
		EStateGettingNetInfo = 2,
		/** Getting the home MCC and MNC from ETEL */
		EStateGettingMccMnc = 3,
		/** Getting the MSISDN - getting store info*/
		EStateGettingMsisdnStoreInfo = 4,
		/** Getting the MSISDN - getting store entry*/
		EStateGettingMsisdnStoreEntry = 5,
		/** Monitoring changes to current network info
		with ETEL's NotifyCurrentNetwork */	
		EStateNetInfoChangeMon = 6
		};

public:
	static CSuplNetworkInfoHandler* NewL(MSuplNetworkInfoObserver& aObserver);
	~CSuplNetworkInfoHandler();
	
	void StartGettingNetworkInfoL();

private:

	CSuplNetworkInfoHandler(MSuplNetworkInfoObserver& aObserver);
	void ConstructL();

	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

	void GetCommDbTSYnameL(TDes& aTsyName);
	void InitialisePhonesL();
	
	void GetNetworkInfoL();
	void GetMccMnc();
	void GetMsisdnStoreInfo();
	void GetMsisdnStoreEntry();
	void MonitorNetworkChange();
	
private:
	TBool ValidateNetInfo();

private:

	MSuplNetworkInfoObserver& iObserver;

	RTelServer iTelServer;
	RProperty iConfigurationProperty;
	
	/** iNetPhone monitors changes to Network Info */
	RMobilePhone iNetPhone;
	/** iCellPhone monitors changes to Cell Info (timing advance)*/
	RMobilePhone iCellPhone;
	
	TBool iTimingAdvSupport;
	TBool iHomeNetworkInfoSupport;
	
	CSuplCellInfoHandler* iCellInfoHandler;
	
	TSuplNetInfoHandlerState iState;
	
	RMobilePhone::TMobilePhoneNetworkInfoV1 iNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg iNetworkInfoPckg;
	RMobilePhone::TMobilePhoneLocationAreaV1 iLocArea;
	
	
	RMobileONStore iOnStore;
	RMobileONStore::TMobileONStoreInfoV1 iOnStoreInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg iOnStoreInfoPckg;
	RMobileONStore::TMobileONEntryV1 iOnStoreEntry;
	RMobileONStore::TMobileONEntryV1Pckg iOnStoreEntryPckg;
	TBool iMsisdnSupport;
	};

#endif // SUPLNEWORKTINFOHANDLER_H

