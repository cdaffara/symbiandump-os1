// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SMSPFACADESTOR_H__
#define __SMSPFACADESTOR_H__

#include "smspstor.h"
#include "smspclass0stor.h"

/**
CFacadeSmsReassemblyStore.

This class will act as a container class for both class 0 & non-class 0 
reassembly store. It will provide generic interface through which client 
of this class will add, delete or process the SMS message.
It also provides few class 0 specific functions because sometimes class 0 messsages 
needs to be handled differently.
Internally it will store the SMS message in its respective reassembly store.

@internalComponent
*/
class CFacadeSmsReassemblyStore : public CBase
	{
public:
	static CFacadeSmsReassemblyStore* NewL(RFs& aFs, MSmsComm& aSmsComm);
	~CFacadeSmsReassemblyStore();

	void OpenStoreL();
	void Close();
	RFs& FileSession() const;

	void InitL();
	void PurgeL(const TTimeIntervalMinutes& aTimeIntervalMinutes,TBool aPurgeIncompleteOnly);
	TBool IsFull();
	void DeleteEnumeratedSIMEntries();
	TInt ExternalizeEnumeratedMessagesL(CSmsProvider& aProvider,TInt& aCount);
	void ProcessCompleteSmsMessagesL(MSmsComm& aSmsComm, const CSmsMessage* aCurrentSmsMessage);

	void AddSegmentToReassemblyStoreL(CSmsMessage& aSmsMessage,const TGsmSms& aGsmSms, TInt& aIndex, TBool& aIsComplete, TBool aIsEnumeration, TInt& aCount, TInt& aTotal);
	void DeleteMessageL(const CSmsMessage& aSmsMessage, TBool aPassed);
	void UpdateLogServerIdL(const CSmsMessage& aSmsMessage, TInt aIndex);
	void SetMessagePassedToClientL(const CSmsMessage& aSmsMessage, TBool aPassed=ETrue);
	TBool IsSeparateClass0StoreSupported();
	//Class 0 Specific function.
	void ProcessMessageIfExceedLimitationL(MSmsComm& aSmsComm);
	void ForwardCompleteClass0SmsMessagesL(MSmsComm& aSmsComm, const CSmsMessage& aSmsMessage, const TSmsAddr* aOriginalSmsAddr,const CSmsMessage* aOriginalSmsMessage,TDes& aDes);
	void SetIncompleteMessageForwardedToClientL(const CSmsMessage& aSmsMessage);
	void SetDiskSpaceState(TSmsDiskSpaceMonitorStatus aDiskSpaceStatus);
	TBool IsWapSMS(const CSmsMessage& aSmsMessage);

private:
	CFacadeSmsReassemblyStore(RFs& aFs, MSmsComm& aSmsComm);
	void ConstructL();

private:
	void InitializeNonClass0StoreL();
	void DeleteNonClass0EnumeratedSIMEntries();
	void DeleteNonClass0MessageL(const CSmsMessage& aSmsMessage, TBool aPassed);
	void AddSegmentToNonClass0ReassemblyStoreL(CSmsMessage& aSmsMessage,const TGsmSms& aGsmSms, TInt& aIndex, TBool& aIsComplete, TBool aIsEnumeration, TInt& aCount, TInt& aTotal);
	TInt ExternalizeEnumeratedNonClass0SmsMessagesL(CSmsProvider& aProvider,TInt& aCount);
	TInt ExternalizeEnumeratedClass0SmsMessagesL(CSmsProvider& aProvider,TInt& aCount);
	void ProcessCompleteNonClass0SmsMessagesL(MSmsComm& aSmsComm, const CSmsMessage* aCurrentSmsMessage);
	void ProcessCompleteClass0SmsMessagesL(MSmsComm& aSmsComm, const CSmsMessage* aCurrentSmsMessage);
	TInt NumberOfCompleteNonClass0Messages();
	void SetNonClass0MessagePassedToClientL(const CSmsMessage& aSmsMessage, TBool aPassed=ETrue);
	void UpdateLogServerIdOfNonClass0MessageL(const CSmsMessage& aSmsMessage, TInt aIndex);
	TBool IsForClass0ReassemblyStore(const CSmsMessage& aSmsMessage);

private:
	RFs& iFs;
	MSmsComm& iSmsComm;
	/** This re-assembly store will contain non-class 0 SMS messages
	& also WAP class 0 messages if SMS stack is configured for handling 
	class 0 messages in out of disk condition. 
	In other case it will contain all type (class 0 & non-class 0) of SMS messages.
	*/

	CSmsReassemblyStore*  iReassemblyStore;

	/** This re-assembly store will contain only class 0 sms messages if 
	SMS stack is configured for handling class 0 messages in out of disk condition.
	In other case it will not be used. */

	CClass0SmsReassemblyStore*  iClass0ReassemblyStore;

	/** Maximum number of comlete SMS messages that can be stored in reassembly store before
	it (re-assembly store) considered to be full. IsFull() function returns TRUE if 
	re-assembly store contains  more than this defined number of complete SMS messages. */
	TInt iMaxmumNumberOfCompleteMessagesInReassemblyStore;
	};

#endif //__SMSPFACADESTOR_H__

