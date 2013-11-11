// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __SMSPENUM_H__
#define __SMSPENUM_H__

#include <e32base.h>
#include <etelmm.h>
#include "gsmuSmsSlot.h"
#include "smspcomm.h"
#include "smspproc.h"
#include "smsuset.h"

#include <mmretrieve.h>
#include "smsuact.h"
#include "smspqueue.h"

#include "es_ini.h"

class CSmsSegmentationStore;
class CFacadeSmsReassemblyStore;
class CQueuedSmsMessage;


/**
 *  
 *  Class CSmsStoreRead for reading messages from the store. Messages can be read
 *  to the list all at once or one by one, depending on capabilities of the Tsy.
 *  @internalComponent
 */
class CSmsStoreRead : public CSmsuActiveBase
	{
private:
        enum
            {
            KDefaultEnumerateTimeoutSeconds=255000000 //<255 sec
            };
public:
	static CSmsStoreRead* NewL(MSmsComm& aSmsComm,
							   const TSmsSettings& aSmsSettings,
							   RMobileSmsMessaging& aSmsMessaging,
							   CFacadeSmsReassemblyStore& aReassemblyStore,
							   CSmsSegmentationStore& aSegmentationStore,
							   TInt aPriority,
							   CSmsMonitorDiskSpace& aSmsMonitorDiskSpace);
	~CSmsStoreRead();

	void Start(TRequestStatus& aStatus,TInt aStoreIndex);

	void CloseStore();

protected:
	void DoCancel();
	void  ReadConfigurableSmsSettingsL();
private:
	CSmsStoreRead(MSmsComm& aSmsComm,
				  const TSmsSettings& aSmsSettings,
				  RMobileSmsMessaging& aSmsMessaging,
				  CFacadeSmsReassemblyStore& aReassemblyStore,
				  CSmsSegmentationStore& aSegmentationStore,
				  TInt aPriority,
				  CSmsMonitorDiskSpace& aSmsMonitorDiskSpace);

	void ConstructL();
	void DoRunL();

	void Complete(TInt aStatus);

	void ReadNextSlotL();
	void ProcessPduL();

private:
	enum TSmsStoreReadState
		{
		ESmsStoreReadIdle,					//< Idle
		ESmsStoreReadGettingStoreInfo,		//< Getting message store info
		ESmsStoreReadReadingList,			//< Reading message list at once
		ESmsStoreReadReadingSlot,			//< Reading message list one message at the time
		ESmsStoreReadReadingSlotCancel		//< Reading message list cancel
		};

private:
	TSmsStoreReadState iState;				//< The current state of the state machine
	MSmsComm& iSmsComm;
	const TSmsSettings& iSmsSettings;
	RMobileSmsMessaging& iSmsMessaging;
	CFacadeSmsReassemblyStore& iReassemblyStore;
	CSmsSegmentationStore& iSegmentationStore;
	CSmsMonitorDiskSpace& iSmsMonitorDiskSpace;
	RMobileSmsStore iSmsStorage;			//< Phone side message store

	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg iStoreInfoPckg;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 iStoreInfo;		//< Capabilities of the current store
	CRetrieveMobilePhoneSmsList* iRetrieveMobilePhoneSmsList;	//< Pointer for getting SMS list from TSY
	CMobilePhoneGsmSmsList* iMobilePhoneGsmSmsList;		//< Pointer to a SMS list
	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg iEntryV1Pckg;
	RMobileSmsStore::TMobileGsmSmsEntryV1 iSmsEntry;	//< Data of a SMS entry

	TGsmSmsSlot iSlot;					//< Data of a SMS entry including store name

	TInt iCurrentSlotIndex;				//< Index to a slot, identifies the index of iSlot
    TBool  iConfigAutomaticDeletionForClass2;
    TBool  iConfigDiscardingType0Class2;
    TBool  iConfigDiscardingType0Class0;
	};


/**
 *  Class CSmsPhoneEnumeration for enumerating all stores.
 *  @internalComponent
 */
class CSmsPhoneEnumeration : public CSmsuActiveBase
	{
public:
	static CSmsPhoneEnumeration* NewL(MSmsComm& aSmsComm,const TSmsSettings& aSmsSettings, CFacadeSmsReassemblyStore& aReassemblyStore, CSmsSegmentationStore& aSegmentationStore,RMobilePhone& aGsmPhone, TInt aPriority, CSmsMonitorDiskSpace& aSmsMonitorDiskSpace);
	~CSmsPhoneEnumeration();

	void Start();

protected:

	void DoRunL();
	void DoCancel();

private:

	CSmsPhoneEnumeration(MSmsComm& aSmsComm,const TSmsSettings& aSmsSettings, RMobilePhone& aGsmPhone, TInt aPriority);
	void ConstructL(CFacadeSmsReassemblyStore& aReassemblyStore,CSmsSegmentationStore& aSegmentationStore,CSmsMonitorDiskSpace& aSmsMonitorDiskSpace);

	void Complete(TInt aStatus);

private:
	enum TSmsPhoneEnumerationState
		{
		ESmsPhoneEnumerationIdle,				//< Idle
		ESmsPhoneEnumerationInitializing,		//< Initializing mobile phone
		ESmsPhoneEnumerationReadingStores		//< Reading all stores one at the time
		};

private:
	TSmsPhoneEnumerationState iState;		//< The current state of the state machine
	MSmsComm& iSmsComm;						//< The reference to the observer
	const TSmsSettings& iSmsSettings;		//< Global SMS Protocol settings
	RMobilePhone& iGsmPhone;				//< Etel phone susession
	RMobileSmsMessaging iSmsMessaging;		//< ETEL phone subsession for messaging services
	TInt iNumStores;						//< Number of the phone side stores
	TInt iCurrentStore;						//< Index to a store
	CSmsStoreRead* iSmsStoreRead;			//< Pointer for reading store from the phone side
	};





/**
 *  Class CSmsMessageWrite for writing messages to the store.
 *  @internalComponent
 */
class CSmsMessageWrite : public CSmsuActiveBase
	{
	friend class CSmspWriteQueue;

private:

	static CSmsMessageWrite* NewL(MSmsComm& aSmsComm, const TSmsSettings& aSmsSettings,
								  RMobilePhone& aGsmPhone,
								  CSmsSegmentationStore& aSegmentationStore
								  );
	~CSmsMessageWrite();

	void Start(CSmsMessage* aSmsMessage, TRequestStatus& aStatus);

private:

	void DoRunL();
	void DoCancel();

private:
	CSmsMessageWrite(MSmsComm& aSmsComm, const TSmsSettings& aSmsSettings,
					 RMobilePhone& aGsmPhone,
					 CSmsSegmentationStore& aSegmentationStore
					 );

	void ConstructL();
	void SegmentMessageL();
	void WriteNextSlot();
	void Complete(TInt aStatus);

private:
	enum TSmsMessageWriteState
		{
		ESmsMessageWriteIdle,				//< Idle
		ESmsMessageWriteInitializing,		//< Initializing mobile phone
		ESmsMessageWriteGettingStoreInfo,	//< Getting message store info
		ESmsMessageWriteWritingPDUs,		//< Writing slot to the store
		};

private:

	MSmsComm& iSmsComm;
	TSmsMessageWriteState iState;		//< The current state of the state machine
	const TSmsSettings& iSmsSettings;	//< Global SMS Protocol settings
	RMobilePhone& iGsmPhone;			//< Etel phone susession
	RMobileSmsMessaging iSmsMessaging;	//< ETEL phone subsession for messaging services
	RMobileSmsStore iSmsStorage;		//< Phone side message store

	RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg iStoreInfoPckg;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 iStoreInfo;	//< Capabilities of the current store

	RMobileSmsStore::TMobileGsmSmsEntryV1Pckg iWriteEntryV1Pckg;
	RMobileSmsStore::TMobileGsmSmsEntryV1 iEntryToWrite;	//< Given entry for writing

	CSmsSegmentationStore& iSegmentationStore;	//< Segmentation store, used to get next 8 or 16bit reference
	CArrayFixFlat<TGsmSms> iSmsArray;	//< Array for Pdus
	CSmsMessage* iSmsMessage;		//< The Message to write
	TInt iNumStores;				//< Number of the phone side stores
	TInt iCurrentStore;				//< Index to a store
	TGsmSmsSlot iSlot;				//< a slot
	};



#endif // !defined __SMSPENUM_H__
