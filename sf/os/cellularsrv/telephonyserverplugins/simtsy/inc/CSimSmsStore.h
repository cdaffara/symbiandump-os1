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

#ifndef __SIMSMSSTORE_H_
#define __SIMSMSSTORE_H_

/**
 * @file
 * @internalAll
 * Contains the class definitions required for the SMS Store.
 */

#include <etelmm.h>
#include <et_phone.h>
#include "csimtimer.h"
#include <mmretrieve.h>

/**
 * This class is used to hold the details of batch read requests between the first and
 * second phases of a "two phase read".  During the first phase, an instance of this class
 * is created containing a unique identifier to the request and the resulting information.
 * Once the second phase operation has identified this instance from a list and completed
 * the client request, this instance will be deleted.
 */
class CListReadAllAttempt : public CBase
	{
public:
	static CListReadAllAttempt* NewL(RMobilePhone::TClientId& aId, TTsyReqHandle aReqHandle);
	~CListReadAllAttempt();

protected:
	CListReadAllAttempt(RMobilePhone::TClientId& aId, TTsyReqHandle aReqHandle);

private:
	void ConstructL();

public:
	CBufBase* iListBuf;					// < The result of the read operation.
	RMobilePhone::TClientId iClient;	// < A unique identifier of the read request.
	TTsyReqHandle iReqHandle;			// < The first phase request handle.
	};
	
/**
 * The main SMS Store class which simulates an SMS Store.
 */
class CSimSmsMessaging;
class CTestConfigSection;
class CSimSmsStore : public CSubSessionExtBase, public MTimerCallBack
	{
public:
	static CSimSmsStore* NewL(CSimSmsMessaging* aSimSmsMess, const TDesC8& aName, TInt aMaxNumSlots, CSimPhone* aPhone);
	CSimSmsStore(CSimSmsMessaging* aSimSmsMess, CSimPhone* aPhone);
	~CSimSmsStore();

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
	void DeleteAllCancel();
	void ReadAllCancel();

// MSubSessionExtBaseTSY pure virtual
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);

// Other public functions.
	void PopulateStoreFromConfigFile();
	TPtrC8 Name();
	TInt UsedEntries();
	TInt MaxSlots();
	TUint StoreCaps();
	TInt StoreIncomingMessage(RMobileSmsStore::TMobileGsmSmsEntryV1* aSms);
	

protected:
	void ConstructL(const TDesC8& aName, TInt aMaxNumSlots);

private:
	TInt GetInfo(TTsyReqHandle aReqHandle, TDes8* aPckg);
	TInt Read(TTsyReqHandle aReqHandle, TDes8* aPckg);
	TInt Write(TTsyReqHandle aReqHandle,TDes8* aPckg);
	TInt Delete(TTsyReqHandle aReqHandle,TDes8* aPckg);
	TInt ReadAllPhase1(TTsyReqHandle aReqHandle,TDes8* aPckg1,TDes8* aPckg2);
	void ReadAllPhase1L(TTsyReqHandle aReqHandle,CRetrieveMobilePhoneSmsList::TBatchRequestData& aClientId,TInt& aBufSize);


	TInt ReadAllPhase2(TTsyReqHandle aReqHandle,TDes8* aPckg1,TDes8* aBuffer);
	TInt DeleteAll(TTsyReqHandle aReqHandle);
	TInt NotifyStoreEvent(TTsyReqHandle aReqHandle,TDes8* aPckg1,TDes8* aPckg2);
	void NotifyStoreEventCancel();

	enum TStoreEvent			// < Store event identifier.
		{
		EStoreEventNoEvent,
		EStoreEventAdded,
		EStoreEventDeleted,
		EStoreEventChanged
		};

	void StoreEvent(TStoreEvent aEvent,TInt aIndex);
	void DelayCompletion(TInt aDelayDuration,TTsyReqHandle aReqHandle);
	void DelayCompletion(TInt aDelayDuration,TTsyReqHandle aReqHandle,TStoreEvent aEvent,TInt aIndex);
	void TimerCallBack(TInt aId);
	const CTestConfigSection* CfgFileSection();
	void RecordSca(const TDesC8& aAsciiScaAddr, TInt aIndex);	
	void LogRequest(TBool aEntering, TInt aIpc, TInt aError);

private:
	CSimSmsMessaging* iSmsMessaging;	// < A pointer to the "owner" SMS messaging class.
	CSimTimer* iTimer;					// < The timer used to delay request completions.
	CSimTimer* iIncomingTimer;					// < The timer used to delay the storage of incoming messages

	TInt iSmsMaxNumSlots;				// < The maximum number of slots in this SMS store.

	RMobileSmsStore::TMobileGsmSmsEntryV1* iGsmSmsStoreEntries;		// Array for GSM SMS entries
	

	TBuf8<KMaxName> iSmsStoreName;	// < The name of the SMS Store.

	CArrayPtrFlat<CListReadAllAttempt>* iSmsReadAll;			// < A list of the oustanding two phase batch read requests.

	TBool iEvOutstandingReq;		// < A flag indicating an outstanding SMS Store notification request.
	TTsyReqHandle iEvReqHandle;		// < The TSY request handle associated with an outstanding SMS Store notification request.
	TUint32* iEvEvent;				// < The event flags associated with an outstanding SMS Store notification request.
	TInt* iEvIndex;					// < The index value associated with an outstanding SMS Store notification request.

	TInt iSmsIndividualPause;		// < The pause value used to delay the completion of an individual SMS Store manipulation request.
	TInt iSmsBatchPause;			// < The pause value used to delay the completion of a batch SMS Store manipulation request.
	
	TUint iSmsStoreCaps;
	
	TTsyReqHandle iPendingReqCompletion;	// < The TSY request handle for the request that is awaiting a timer event before completion.
	TStoreEvent iPendingEvent;				// < The type of store event related to the request that is awaiting a timer event before completion.
	TInt iPendingIndex;						// < The index related to the request that is awaiting a timer event before completion.
	
	CSimPhone* iPhone;
	};

#endif
