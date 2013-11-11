// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SIMONSTORE_H_
#define __SIMONSTORE_H_

/**
 * @file
 *
 * Contains the class definitions required for the Phonebook Store.
 * @internalAll
 */

#include <etelmm.h>
#include <et_phone.h>
#include <mpbutil.h>
#include "csimtimer.h"
#include "CSimPhone.h"
#include "CSimPhoneSecurity.h"

const TInt KONMaxTextSize=241;
const TInt KONMaxTelNumSize=100;

/**
 * The class which simulates a Own Number Store.
 */
class CSimPhone;
class CTestConfigSection;
class CTestConfigItem;
NONSHARABLE_CLASS(CSimONStore) : public CSimPhone, public MTimerCallBack
	{
public:
	static CSimONStore* NewL(CSimPhone* aPhone, TInt aMaxNumSlots, TInt aMaxNumLen, TInt aMaxTextLen);
	CSimONStore(CSimPhone* aPhone);
	~CSimONStore();

	static void ClosePhone(TAny* aObj);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIpc);
// MTelObjectTSY pure virtual
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual void Init();
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
	virtual TInt CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle);

// MSubSessionExtBaseTSY pure virtual
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);

// Other public functions.
	void PopulateStoreFromConfigFileL();
	void PopulateONStoreInfo(RMobileONStore::TMobileONStoreInfoV1* aStoreInfo);

	TInt UsedEntries();

protected:
	void ConstructL(TInt aMaxNumSlots, TInt aMaxNumLen, TInt aMaxTextLen);

private:
	TInt GetInfo(TTsyReqHandle aReqHandle, TDes8* aPckg);
	TInt Read(TTsyReqHandle aReqHandle, TDes8* aPckg1);

	void DelayCompletion(TInt aDelayDuration,TTsyReqHandle aReqHandle);
	void TimerCallBack(TInt aId);

	TBool FindIpcErrorMatch(TInt& aError);
	const CTestConfigSection* CfgFile();
	TInt GetONEntry(const CTestConfigItem* aItem, TInt aItemIndex, TInt& aIndex,
			 TPtrC8& aTelNum, TPtrC8& aText, RMobilePhone::TMobileTON& aTypeOfNumber,
			 RMobilePhone::TMobileService& aService, RMobilePhone::TMobilePhoneNetworkMode& aMode,
			 RMobilePhone::TMobileNPI& aNumberPlan);

private:
	CPhoneBase*	iDummyPhoneBaseRef;

	CSimPhone* iPhone;				// < A pointer to the "owner" phone class.
	CSimTimer* iReqTimer;			// < The timer used to delay request completions.

	TInt iONMaxNumSlots;			// < The maximum number of slots in this Own Number store.
	TInt iONMaxTelNumLen;			// < The maximum size of a telephone number that can be stored in this Own Number store.
	TInt iONMaxTextLen;			// < The maximum size of an alpha tag that can be stored in this Own Number store.

	struct TONStoreEntry
		{
		RMobilePhone::TMobilePhoneNetworkMode iMode;
		RMobilePhone::TMobileService iService;
		RMobilePhone::TMobileTON iTypeOfNumber;	// < The Own Number entry's Type of Number and Number Plan Identifier.
		RMobilePhone::TMobileNPI iNumberPlan;
		TBuf<KONMaxTelNumSize> iTelNum;		// < The Own Number entry's telephone number.
		TBuf<KONMaxTelNumSize> iText;		// < The Own Number entry's telephone number.
		};

	TONStoreEntry* iONStoreEntries;	// < A pointer to the Own Number Store array.

	TUint iONStoreCaps;				// < The phone level caps of the Own Number Store
	CPhoneBookBuffer* iONRwBuffer;	// < The Own Number read/write "streaming" class.

	TBool iEvOutstandingReq;		// < A flag indicating an outstanding Phonebook Store notification request.
	TInt iONIndividualPause;		// < The pause value used to delay the completion of an individual Phonebook Store manipulation request.
	TTsyReqHandle iPendingReqCompletion;	// < The TSY request handle for the request that is awaiting a timer event before completion.

	};

#endif
