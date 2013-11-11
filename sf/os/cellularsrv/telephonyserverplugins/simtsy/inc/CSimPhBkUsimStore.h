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

#ifndef __SIMPHBKUSIMSTORE_H_
#define __SIMPHBKUSIMSTORE_H_

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
#include "CSimPhBkStore.h"

/**
 * The main Phonebook Store class which simulates a Phonebook Store.
 */
class CSimPhone;
class CTestConfigSection;
class CTestConfigItem;
class CSimPhBkUSimStore : public CSimPhone, public MTimerCallBack
	{
public:
	
	static CSimPhBkUSimStore* NewL(CSimPhone* aPhone, const TDesC8& aStore, const TDesC8& aName, TInt aMaxNumSlots, TInt aMaxNumLen, TInt aMaxTextLen, TInt aMaxAdditional, TInt aMaxEmail, TInt aMaxAdditionalTelNumLen, TInt aMaxAdditionalTextLen);
	CSimPhBkUSimStore(CSimPhone* aPhone);
	~CSimPhBkUSimStore();

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
	void PopulatePhBkStoreInfo(RMobilePhoneStore::TMobilePhoneStoreInfoV1* aStoreInfo);

//coverity[bad_override]    	
	TPtrC8 Name();
	TPtrC8 PhBkStore();
	TPtrC8 AID();
	TInt UsedEntries();
	TInt MaxSlots();
	TInt CheckAndSwitchUSimApps();

protected:
	void ConstructL(const TDesC8& aStore, const TDesC8& aName, TInt aMaxNumSlots, TInt aMaxNumLen, TInt aMaxTextLen, TInt aAdditional, TInt aMaxEmail, TInt aMaxAdditionalTelNumLen, TInt aMaxAdditionalTextLen);

private:
	TInt GetInfo(TTsyReqHandle aReqHandle, TDes8* aPckg);
	TInt Read(TTsyReqHandle aReqHandle, TDes8* aPckg1, TDes8* aPckg2);
	TInt Write(TTsyReqHandle aReqHandle,TDes8* aPckg1, TDes8* aPckg2);
	TInt Delete(TTsyReqHandle aReqHandle,TDes8* aPckg);
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
	TBool FindIpcErrorMatch(TInt& aError);
	const CTestConfigSection* CfgFile();
	
	TInt GetPhBkAdditionalEntry(TPtrC8& aEntry, TInt& aItemIndex,
								 TPtrC8& aTelNum, TPtrC8& aAlphaTag, TUint8& aNpiTon);

	TInt GetPhBkUSimEntry(const CTestConfigItem* aItem, TInt& aItemIndex,
								 TPtrC8& aPhonebookStore, TPtrC8& aPhonebookName, 
								 TPtrC8& aAid,TInt& aIndex, TPtrC8& aTelNum, 
								 TPtrC8& aAlphaTag, TUint8& aNpiTon,
								 TPtrC8& aAlphaTag2);
	void PopulateOOBWrite();
	void PopulateOOBDelete();

private:
	CPhoneBase* iDummyPhoneBaseRef;

	CSimPhone* iPhone;				// < A pointer to the "owner" phone class.
	CSimTimer* iReqTimer;			// < The timer used to delay request completions.
	CSimTimer* iOOBWriteTimer;		// < The timer used to time an OOB store event.
	CSimTimer* iOOBDeleteTimer;		// < The timer used to time an OOB delete event.

	TInt iPhBkMaxNumSlots;			// < The maximum number of slots in this Phonebook store.
	TInt iPhBkMaxTelNumLen;			// < The maximum size of a telephone number that can be stored in this Phonebook.
	TInt iPhBkMaxTextLen;			// < The maximum size of an alpha tag that can be stored in this Phonebook.
	TInt iPhBkAdditional;			// < The maximum number of additional numbers that can be stored in this Phonebook per entry.
	TInt iPhBkMaxEmail;				// < The maximum number of emails that can be stored in this Phonebook per entry.
	
	TInt iPhBkMaxAdditionalTelNumLen; // < The maximum size of an additional telephone number that can be stored in this Phonebook.
	TInt iPhBkMaxAdditionalTextLen;   // < The maximum size of an additional alpha tag that can be stored in this Phonebook.
	
	struct TPhBkStoreEntry
		{
		TBuf<KPhBkMaxAlphaTagSize> iAlphaTag;	// < The phonebook entry's alpha tag.
		TUint8 iTonNpi;							// < The phonebook entry's Type of Number and Number Plan Identifier.
		TBuf<KPhBkMaxTelNumSize> iTelNum;		// < The phonebook entry's telephone number.
		};

	typedef TBuf<KPhBkMaxAlphaTagSize> TPhBkUSimEmail;
	typedef TBuf<KPhBkMaxAlphaTagSize> TPhBkUSimGroup;

	class TPhBkUSimStoreEntry
		{
	public:
		TBuf<KPhBkMaxAlphaTagSize> iAlphaTag;	// < The phonebook entry's alpha tag.
		TUint8 iTonNpi;							// < The phonebook entry's Type of Number and Number Plan Identifier.
		TBuf<KPhBkMaxTelNumSize> iTelNum;		// < The phonebook entry's telephone number.
		TBuf<KPhBkMaxAlphaTagSize> iAlphaTag2;	// < The phonebook entry's name2 alpha tag.
		CArrayFixFlat<TPhBkStoreEntry>* iAdditional; //< The phonebook entry's additional numbers
		CArrayFixFlat<TPhBkUSimEmail>* iEmails;				//< The phonebook entry's list of email addresses
		CArrayFixFlat<TPhBkUSimGroup>* iGroups;			   //< The phonebook entry's list groups definition
		TBool iHiddenEntry;
		};
	
	TPhBkUSimStoreEntry* iPhBkUSimStoreEntries; // < Gsm Phonebook only
	TPhBkUSimStoreEntry* iPhBkUSim1StoreEntries; // < USim PhoneBook1
	TPhBkUSimStoreEntry* iPhBkUSim2StoreEntries; // < USim PhoneBook2
	
	struct TPhBkError
		{
		TInt iCount;				// < The IPC count of the request that is to be errored.
		TInt iError;				// < The error number that must be used with that request.
		};
	CArrayFixFlat<TPhBkError>* iPhBkError;	// < Array of indicating where to simulate phonebook request errors.
	TInt iIpcCnt;							// < The current count of IPC client requests.

	TBuf8<KMaxName> iPhBkStore;		// < The Phonebook Store.
	TBuf8<KMaxName> iPhBkStoreName;		// < The name of the Phonebook Store.
	TInt iPhBkUSimCounter;
	RMobilePhone::TAID iPhBkAid;
	RMobilePhone::TAID iPhBkAidUSim1;
	RMobilePhone::TAID iPhBkAidUSim2;

	TUint iPhBkStoreCaps;				// < The phone level caps of the PhBk Store

	CPhoneBookBuffer* iPhBkRwBuffer;	// < The Phonebook read/write "streaming" class.

	TBool iEvOutstandingReq;		// < A flag indicating an outstanding Phonebook Store notification request.
	TTsyReqHandle iEvReqHandle;		// < The TSY request handle associated with an outstanding Phonebook Store notification request.
	TUint32* iEvEvent;				// < The event flags associated with an outstanding Phonebook Store notification request.
	TInt* iEvIndex;					// < The index value associated with an outstanding Phonebook Store notification request.

	TInt iPhBkIndividualPause;		// < The pause value used to delay the completion of an individual Phonebook Store manipulation request.
	TInt iPhBkBatchPause;			// < The pause value used to delay the completion of a batch Phonebook Store manipulation request.

	TTsyReqHandle iPendingReqCompletion;	// < The TSY request handle for the request that is awaiting a timer event before completion.
	TStoreEvent iPendingEvent;				// < The type of store event related to the request that is awaiting a timer event before completion.
	TInt iPendingIndex;						// < The index related to the request that is awaiting a timer event before completion.

	TInt iPhBkOOBWriteDuration;				// < The pause before the "out of band" write will be executed.
	TInt iPhBkOOBWriteIndex;				// < The index number associated with the "out of band" write.
	TPhBkUSimStoreEntry iPhBkOOBWrite;			// < The phonebook entry to be written in the "out of band" write.

	TInt iPhBkOOBDeleteDuration;			// < The pause before the "out of band" delete.
	TInt iPhBkOOBDeleteIndex;				// < The inex number associated with the "out of band" delete.
	
	struct TTriggerEventIPC					
		{
		TInt iIPC;							
		TInt iIPCCnt;						
		RMobilePhone::TMobilePhoneSecurityEvent iEvent;	
		};
	TTriggerEventIPC iTriggerEventIPC;		// <  If .iIPC is called .iIpcCnt times, .iEvent is triggered
	TInt iTriggerCnt;						// < the counter used to count the number of times iTriggerEventIPC.iIpc has been called				

	};

#endif
