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

#ifndef __SMSPREASSEMBLYSTOR_H__
#define __SMSPREASSEMBLYSTOR_H__

#include <e32base.h>
#include "gsmustor.h"
#include "smsprot.h"

/**
 *  @internalComponent
 */
const TDriveNumber KStoreDrive = EDriveC;
_LIT(KStoreSubDir, "sms\\");

// Flat array object's granularity.
const TInt KFlatArrayGranularity = 8;

/**
 *  Provides Utility functions for re-assembly store.
 *  
 *  @internalComponent
 */
class CReassemblyStoreUtility : public CBase
	{
public:
	static void PrivatePath(RFs& aFs, TDes& aPath);
	static void PopulateEntry(TSmsReassemblyEntry& aEntry,const CSmsMessage& aSmsMessage,TInt aNumSmss);
	};

/**
 *  Provides generic information useful for the reassembly of SMS messages.
 *  
 *  This class should reflect parts of a complete SMS message to be stored in
 *  the reassembly store.
 *  @internalComponent
 */
class TReassemblyEntry : public TSmsReassemblyEntry
	{
	private:
	/*
	In TSmsReassemblyEntry, the below 2 functions are public.
	In this class main intention is to make these 2 function private
	so that client can't use those 2 functions. This class will be used in
	CReassemblyStore class which does not deal with storing the data in
	permanent store file. So there is no need of these 2 functions.
	*/
	TStreamId DataStreamId() const;
	void SetDataStreamId(TStreamId aStreamId);
	};

/**
 *  This class is based on template design pattern. The re-assembly store logic is divided
 *	into invariant & variant part. The invariant part logic is specified here.
 *	Variant part logic is left to sub-class to implement. 
 *	Invariant part deals with addition/updation/deletion of SMS message in re-assembly store
 *	without knowing where or how the message is stored.	Variant part logic
 *	mainly deals with storing/reading SMS messages in/from persistent file. So in future if 
 *	situation demand then storing part can be changed without any changes in invariant part.
 *	In that case new class has to be derived from this class & all pure virtual function has
 *	to be implemented.
 *  @internalComponent
 */
class CReassemblyStore : public CBase
	{
public:
	void InitializeL();
	void PurgeL(const TTimeIntervalMinutes& aTimeIntervalMinutes,TBool aPurgeIncompleteOnly);
	void DeleteEnumeratedSIMEntries();
	TInt NumberOfCompleteMessages();

	void AddSegmentToReassemblyStoreL(CSmsMessage& aSmsMessage,const TGsmSms& aGsmSms, TInt& aIndex, TBool& aIsComplete, TBool aIsEnumeration, TInt& aCount, TInt& aTotal);
	void DeleteMessageL(const CSmsMessage& aSmsMessage, TBool aPassed);
	void UpdateLogServerIdOfMessageL(const CSmsMessage& aSmsMessage, TInt aIndex);
	void SetMessagePassedToClientL(const CSmsMessage& aSmsMessage, TBool aPassed=ETrue);
	void GetMessageL(TInt aIndex, CSmsMessage& aSmsMessage);

protected:
	CReassemblyStore(RFs& aFs);
	virtual ~CReassemblyStore();

	virtual void NewMessagePDUL(TInt& aIndex,CSmsMessage& aSmsMessage,const TGsmSms& aGsmSms);
	void MatchPDUToExistingMessage(const CSmsMessage& aSmsMessage, TInt& aIndex);
	virtual void UpdateExistingMessageL(CSmsMessage& aSmsMessage,
								const TGsmSms& aGsmSms, TInt aIndex,
								TBool& aComplete,
								TBool& aDuplicateMsgRef,
								TBool& aDuplicateSlot);

	TBool FindMessageL(const CSmsMessage& aSmsMessage, TBool aPassed, TInt& aIndex);
	const CArrayFix<TReassemblyEntry>& Entries() const {return iEntryArray;}

public:
	/*
	This function will open the file where SMS messages are stored
	*/
	virtual void OpenStoreL()=0;
	/*
	This function will close the file where SMS messages are stored
	*/
	virtual void Close()=0;

private:
	/*
	This function will fill-up the entries for all the messages stored in re-assembly store.
	*/
	virtual void PopulateEntryArrayL(CArrayFix<TReassemblyEntry>& aEntryArray) = 0;
	/*
	This function will add a new message in the persistent file.
	*/
	virtual void AddNewMessageL(CSmsMessage& aSmsMessage,const TGsmSms& aGsmSms) = 0;
	/*
	This function will update an existing message in the persistent file.
	*/
	virtual void UpdateExistingMessageL(CSmsMessage& aSmsMessage,const TGsmSms& aGsmSms, TBool& aDuplicateMsgRef, TBool& aDuplicateSlot) = 0;
	/*
	This function will return an SMS message based on the provided entry information.
	*/
	virtual void RetrieveMessageL(const TReassemblyEntry& aEntry, CSmsMessage& aSmsMessage) = 0;
	/*
	This function will delete a SMS message based on the provided entry information.
	*/
	virtual void DeleteEntryL(const TReassemblyEntry& aEntry) = 0;
	/*
	This function will update the Set Passed To Client field of the provided entry.
	*/
	virtual void SetPassedToClientL(const TReassemblyEntry& aEntry, TBool aBool) = 0;
	/*
	This function will update the log server id of the provided entry.
	*/
	virtual void UpdateLogServerIdL(const TReassemblyEntry& aEntry, TLogId aLogServerId) = 0;
	/*
	This function will begin the transaction.
	*/
	virtual void BeginTransactionLC() = 0;
	/*
	This function will commit the transaction.
	*/
	virtual void CommitTransactionL() = 0;

protected:
	/*
	Ideally RFs should not be a member variable of this class.
	But at the time of creating CSmsMessage it needs reference to RFs
	That is the reason why it is here.
	*/
	RFs& iFs;
	CArrayFixFlat<TReassemblyEntry>	iEntryArray;
	TTime iLastReceivedTime;
	TTime iLastRealTime;
	};

#endif // !defined __SMSPREASSEMBLYSTOR_H__
