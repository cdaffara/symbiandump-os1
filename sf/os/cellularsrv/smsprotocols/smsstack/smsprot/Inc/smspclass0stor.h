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

#if !defined (__SMSPCLASS0STOR_H__)
#define __SMSPCLASS0STOR_H__

#include "smspreassemblystore.h"

/**
 *  @internalComponent
 */
_LIT(KPreAllocatedFileName,"smsclass0preallocated.dat");	//Pre-allocated File Name
_LIT(KClass0ReassemblyStoreName,"smsclass0reast.dat");	//reassembly store name

const TInt KClass0ReassemblyStoreUidValue=0x102835A1;
const TUid KClass0ReassemblyStoreUid={KClass0ReassemblyStoreUidValue};  //  Used for Third uid of class 0 reassembly store

class TSmsPreAllocatedFileStoreReassemblyEntry;
class CClass0SmsReassemblyStore;

/**
CSmsPermanentFileStore.

This class is based on Symbian Permanent file store mechanism.
This class stores the data as streams. Like CSmsReassemblyStore's implementation, 
the root stream will contain an array (header info) of TSmsReassemblyEntry. 
There is one entry in the array for every sms messages which is being stored. 
The entry contains the generic information related to the SMS message. 
Each SMS is also allocated its own stream inside the file. This stream is primarily 
used to store the SMS message's user data. The stream id is also stored in the 
corresponding entry in the header to identify on which stream the SMS user data is stored.

It provides interface to add, update, delete the SMS messages. It also provides the interface
to modify the entry information (update log server id, message passed to client, etc.)

It also provides few cleanup functions to clean the stored messages from this permanent store file.
(CleanupEntriesWithCompactL, CleanupEntriesL)
This requirement is very specific to class 0 re-assembly store implementation.
These functions are needed because it is not possible to delete the message in out-of-disk condition
from permanent store file. But after getting the disk space it is required to clean up the entries.
In Class 0 re-assembly store it is possible that few PDUs of a concatenated message are stored 
in this file. Others are stored in pre-allocated file. There might be a scenario where user has 
deleted a message but the corresponding message has not been deleted from permanent store file 
due to out-of-disk condition. And also at the time of forwarding an incomplete message a 
forwarded message has not been deleted due to above reason. But the entry/PDU is invalid 
because it is no more in the pre-allocated file which contains master header info.

@internalComponent
*/
class CSmsPermanentFileStore : public CBase
	{
friend void CSmsPermanentStoreCloseObject(TAny* aObj);

public:
	static CSmsPermanentFileStore* NewL(RFs& aFs, const TDesC& aFileName, const TUid& aThirdUid);
	~CSmsPermanentFileStore();

	void CreateL();
	void OpenL();
	void Close();
	TBool IsFileOK();

	const CArrayFix<TSmsReassemblyEntry>& Entries() const {return iEntryArray;}

	void AddNewMessageL(TInt& aIndex, CSmsMessage& aSmsMessage,const TGsmSms& aGsmSms);
	void UpdateExistingMessageL(TInt aIndex, const CSmsMessage& aSmsMessage, const CArrayFix<TInt>& aIndexArray, const CArrayFix<TGsmSms>& aSmsArray);
	void DeleteEntryL(TInt aIndex);

	void MatchEntryToExistingMessage(const TReassemblyEntry& aEntry,	TInt& aIndex);
	void UpdateLogServerIdL(TInt& aIndex, TLogId aLogServerId);
	void SetPassedToClientL(TInt aIndex, TBool aBool);

	void BeginTransactionL();
	void CommitTransactionL();
	void Revert();

	void InternalizeEntryL(const TInt aIndex, CSmsMessage& aSmsMessage, CArrayFix<TInt>& aIndexArray, CArrayFix<TGsmSms>& aSmsArray);

	void RemovePDUsL(TInt aIndex, TInt aStartPos, TInt aEndPos);
	void CleanupEntriesWithCompactL(const CArrayFix<TSmsPreAllocatedFileStoreReassemblyEntry>& aEntryArray);
	void CleanupEntriesL(const CArrayFix<TSmsPreAllocatedFileStoreReassemblyEntry>& aEntryArray);

private:
	CSmsPermanentFileStore(RFs& aFs, const TUid& aThirdUid);
	void ConstructL(const TDesC& aFileName);

	void AddEntryL(TSmsReassemblyEntry& aEntry);
	void ChangeEntryL(TInt aIndex, const TSmsReassemblyEntry& aNewEntry);

	void ExternalizeEntryArrayL();
	void InternalizeEntryArrayL();

	void ExternalizeEntryL(TStreamId& aStreamId,const CSmsMessage& aSmsMessage,const CArrayFix<TInt>& aIndexArray,const CArrayFix<TGsmSms>& aSmsArray);
	void DoCommitAndCompactL();
	void RemoveDeletedEntries();
	void ReinstateDeletedEntries();

private:
	enum
		{
		KNumStoreCommitsBeforeCompaction=16
		};

private:
	RFs& iFs;
	HBufC* iFileName;
	TUid iThirdUid;
	CFileStore* iFileStore;
	CArrayFixFlat<TSmsReassemblyEntry>	iEntryArray;
	TBool iInTransaction;
	TInt iCommitCount;
	TBool iCompact;
	};

/*
It will contain forwarded message's bit-map information.
The max size of SMS message can be 256 PDUs. Due to this
reason 32 bytes character is reserved to store the forwarded
message information.

@internalComponent
*/
const TInt KBitMapLen = 32;
typedef TBuf8<KBitMapLen> TForwardedPDUBitMap;


/**
SMS reassembly store entry for incoming SMS messages in Pre-allocated file.

NOTE:
	Previously these 4 fields (iStatus, iTimeOffset, iDecodedOnSim, iForwardToClient) 
	was not part of entry information. Because it was part of CSmsMesssage 
	& was being stored in the permanent store file. But in the raw data file, 
	there is no	provision to store CSmsMessage, only PDUs are stored so the given 
	4 fields are added for this purpose.

@internalComponent
 */
class TSmsPreAllocatedFileStoreReassemblyEntry : public TSmsReassemblyEntry
	{
public:
	TSmsPreAllocatedFileStoreReassemblyEntry();
	inline TInt PreAllocatedStorageId () const {return iPreAllocatedStorageId;}
	inline void SetPreAllocatedStorageId (TInt aStorageId) {iPreAllocatedStorageId = aStorageId;}

	inline RMobileSmsStore::TMobileSmsStoreStatus Status() const {return iStatus;}
	inline void SetStatus(RMobileSmsStore::TMobileSmsStoreStatus aStatus) {iStatus=aStatus;}

	inline TTimeIntervalSeconds UTCOffset() const {return iTimeOffset;}
	inline void SetUTCOffset(TTimeIntervalSeconds aTimeOffset) {iTimeOffset = aTimeOffset;}

	inline TBool DecodedOnSIM() const { return iDecodedOnSim; }
	inline void SetDecodedOnSIM(TBool aBool) { iDecodedOnSim = aBool; }

	inline TBool ForwardToClient() const { return iForwardToClient; }
	inline void SetForwardToClient(TBool aBool) { iForwardToClient = aBool; }

	inline TInt NumberOfPDUsForwardToClient() const { return iForwardedCount; }
	inline void SetNumberOfPDUsForwardToClient(TInt aCount) { iForwardedCount = aCount; }

	void GetBitMap(TUint8 aIndex, TUint8& aBitMap);
	void SetBitMap(TUint8 aIndex, TUint8 aBitMap);

private:
	/*
	In TSmsReassemblyEntry, the below 2 functions are public.
	In this class main intention is to make these 2 function private
	so that client can't use those 2 functions. This class will be used in
	CPreallocatedFile class which does not deal with storing the data in
	permanent store file. So there is no need of these 2 functions.
	*/
	TStreamId DataStreamId() const;
	void SetDataStreamId(TStreamId aStreamId);

private:
	/**
	This is the unique identifier to identify the PDUs.
	*/
	TInt iPreAllocatedStorageId;
	RMobileSmsStore::TMobileSmsStoreStatus iStatus;
	TTimeIntervalSeconds iTimeOffset;
	TBool iDecodedOnSim;
	TBool iForwardToClient;
	TInt iForwardedCount;     //< number of PDUs forwarded to client
	TForwardedPDUBitMap iBitMap;	// Forwarded Message's bit-map information
	};

/**
CPreallocatedFile.

This class will store the incoming PDU in out-of-disk condition. It will create a normal data 
file (based on Symbian RFile) when SMS stack boots for the 1st time. It will be a fixed size 
file dedicated to store fixed number (configurable) of class 0 PDUs in out of disk condition 
& also contain all the stored SMS Message's generic information. At the time of creation of 
this file, the size of the file will be set (RFile::SetSize) to guarantee that the entire 
file is allocated. Since Pre-allocated file has a fixed size and known layout, SMS 
Stack will use RFile::Write() to write data to a fixed position in the file. 
And the update will be guaranteed to succeed even if there is no memory left on the drive.
When a class 0 SMS PDU is received and the disk space is exhausted, then class 0 
reassembly store will store the segment in this file. It will take 2 configuration 
elements to create this file:

1.	Number of PDU segments that could be stored in out of disk condition.
2.	Maximum number of class 0 messages that could be stored at one instance

This file will have 4 sections: Version Number, Header Section, Duplicate Header Section & Data Section

1.	Version Number - Interger Value.
2.	Header Section: Number of entries, Array of Entries, Array of PDU identifier section, Checksum.
3.	Duplicate Header Section: Same as Header Section.
4.	Data Section: Array of Container. Each container contains Sms slot informatiuon, index number & PDU.

It provides interface to add, update, delete the SMS messages. It also provides the interface
to modify the entry information (update log server id, message passed to client, etc.)

@internalComponent
*/
class CPreallocatedFile : public CBase
	{
public:
/**
 *  Pre-allocated file versions
 */
	enum TPreAllocatedFileVersion
		{
	/** The default value. */
		EPreAllocatedFileV1
		};

public:
	static CPreallocatedFile* NewL(RFs& aFs, const TDesC& aFileName, TInt aMaxClass0Msg, TInt aMaxPDUSeg, TPreAllocatedFileVersion aVersion=EPreAllocatedFileV1);
	~CPreallocatedFile();

	void CreateL();
	void OpenL();
	void Close();
	TBool IsFileOK();

	void AddNewMessageL(TInt& aIndex, CSmsMessage& aSmsMessage,const TGsmSms& aGsmSms);
	void UpdateExistingMessageL(TInt aIndex, const CSmsMessage& aSmsMessage, TInt aPduIndex, const TGsmSms& aSms);
	void UpdateLogServerIdL(TInt& aIndex, TLogId aLogServerId);
	void SetPassedToClientL(TInt aIndex, TBool aBool);
	void MatchEntryToExistingMessage(const TReassemblyEntry& aEntry, TInt& aIndex);
	void AddEntryL(TSmsPreAllocatedFileStoreReassemblyEntry& aEntry);
	void DeleteEntryL(TInt aIndex);

	void InternalizeEntryL(const TInt aIndex, CSmsMessage& aSmsMessage, CArrayFix<TInt>& aIndexArray, CArrayFix<TGsmSms>& aSmsArray);

	void BeginTransactionL();
	void CommitTransactionL();
	void Revert();
	void CopyDuplicateToMasterL();
	TInt NumberOfPDUStored();
	TInt GetOldestMessageEntryIndex();

	void RemovePDUsL(TInt aIndex, TInt aStartPos, TInt aEndPos);
	void StoreForwardedPDUsInfoL(TInt aIndex, TInt aStartPos, TInt aEndPos);

	const CArrayFix<TSmsPreAllocatedFileStoreReassemblyEntry>& Entries() const {return iEntryArray;}

private:
	CPreallocatedFile(RFs& aFs, TInt aMaxClass0Msg, TInt aMaxPDUSeg, TPreAllocatedFileVersion aVersion);
	void ConstructL(const TDesC& aFileName);

	void InternalizeEntryArrayL();
	void ExternalizeEntryArray();

	void ChangeEntryL(TInt aIndex, const TSmsPreAllocatedFileStoreReassemblyEntry& aNewEntry);
	void AddStorageIdL(TInt aIndex, TInt aStorageId);
	void ExternalizeEntry(TInt aContainerId, const TGsmSmsSlotEntry& aSmsSlot, TInt aIndex, const TGsmSms& aGsmSms);
	void ExternalizeEntry(TInt aContainerId, TInt aIndex, const TGsmSms& aGsmSms);
	TInt GetFreeContainer();
	TInt GetFreeStorageId();
	void CheckDataL();
	TUint32 ChecksumValue();
	void CopyMasterToDuplicateL();
	void PutChecksumValueL();
	void ClearEntryL(TInt aStorageId, TInt aNumberOfPDUs);

	void RemoveDeletedEntries();
	void ReinstateDeletedEntries();
	void ReinstateEntries();

private:
	enum TReinstateFlag
		{
		EEntryIsDeleted=1, //< indicates that this entry is deleted
		EEntryIsAdded=2    //< indicated that this entry is added
		};

	class TReinstateEntryInfo
		{
		public:
			TInt iPreAllocatedStorageId;
			TInt iContainerId;
			TReinstateFlag iFlag;
		};

private:
	RFs& iFs;
	RFile	iFile;
	HBufC* iFileName;
	CArrayFixFlat<TSmsPreAllocatedFileStoreReassemblyEntry>	iEntryArray;
	TBool iInTransaction;
	/*
	The below data structure (iReinstateEntryInfo) is there for revert 
	operation to succeed. e.g, If after removing the PDUs from pre-allocated 
	file it is needed to restore, then using this data structure the lost 
	information can be retrieved. In the same way after adding the PDU if 
	it is needed to be deleted then using this data structure it can be 
	found where	the PDU is added.
	*/
	CArrayFixFlat<TReinstateEntryInfo>	iReinstateEntryInfo;

	TInt iMaxClass0Msg;
	TInt iMaxPDUSeg;
	TPreAllocatedFileVersion iVersion;

	TInt iSizeOfEntrySection;
	TInt iSizeOfStorageIdentifierSection;
	TInt iSizeOfFile;
	TInt iBeginOfDuplicateHeaderSection;
	TInt iBeginOfDataSection;
	};

/**
This class provides the interface to enable & disable the guard timer. 

When class 0 re-assembly store opens, it enable the guard timer.
When the timer expires, it requests re-assembly store to forward & purge all
those messages whose time have expired. Then it again enables the guard timer.
It always enables the guard timer against the oldest message. When the timer expires
it again re-calculate expire time on the basis of time of old message. If there is no
message then it set the timer against configured timeout settings.

@internalComponent
 */
class CGuardTimer : public CTimer
	{
public:
	static CGuardTimer* NewL(CClass0SmsReassemblyStore& aClass0ReassemblyStore, TInt aGuardTimeout);
	~CGuardTimer();
	void EnableGuardTimer();
	void DisableGuardTimer();

private:
	CGuardTimer(CClass0SmsReassemblyStore& aClass0ReassemblyStore, TInt aGuardTimeout);
	void RunL();

private:
	CClass0SmsReassemblyStore& iClass0ReassemblyStore;
	const TTimeIntervalSeconds   iTimeoutInSecs;
	};

/*
This class is primarily responsible for storing incoming class 0 SMS PDU's & assembling them.
It provides the same functionality as CSmsReassemblyStore class but augmented the 
functionality by adding the fallback mechanism to store class 0 SMS message in out of 
disk condition. It maintains reference to CPermanentStoreFile & CPreAllocatedFile
to store incoming PDU in normal & out of disk condition respectively. It also contains a 
timer object which will be used as guard timer.

This class is derived from CReassemblyStore & has implemented all the pure virtual functions.

It also adds few new functions which is required for handling specific class 0 message requirements
like handling of incomplete class 0 messages, guard time out, etc.

@internalComponent
*/
class CClass0SmsReassemblyStore : public CReassemblyStore
	{
friend class CFacadeSmsReassemblyStore;
friend void CClass0StoreCloseObject(TAny* aObj);

public:
	static CClass0SmsReassemblyStore* NewL(RFs& aFs, MSmsComm& aSmsComm);
	virtual ~CClass0SmsReassemblyStore();

	//Function related to incomplete message
	void ProcessMessageIfExceedLimitationL(MSmsComm& aSmsComm);
	void ForwardCompleteClass0SmsMessagesL(MSmsComm& aSmsComm, const CSmsMessage& aSmsMessage,const TSmsAddr* aOriginalSmsAddr,const CSmsMessage* aOriginalSmsMessage,TDes& aDes);
	void SetIncompleteMessageForwardedToClientL(const CSmsMessage& aSmsMessage);
	//Function which set the disk space state
	void SetDiskSpaceState(TSmsDiskSpaceMonitorStatus aDiskSpaceStatus);
	//Function related to guard timer
	void GetNextGuardTimeout(TTime& aNextTimeout);
	void ProcessTimeoutMessageL();
	//Function related to clean-up
	TInt CleanReassemblyEntries();

public:
	//Implementation of public pure virtual function of CReassemblyStore
	virtual void OpenStoreL();
	virtual void Close();

private:
	CClass0SmsReassemblyStore(RFs& aFs, MSmsComm& aSmsComm);
	void ConstructL();
	void Revert();
	void ReadConfigurableClass0SmsSettingsL(TInt& aMaxClass0Msg, TInt& aMaxPDUSeg, TInt& aGuardTimeOut);

	//Function related for reserved space full.
	TBool IsExceedLimitation();
	TInt GetIndexOfOldestMessage();
	TInt GetIndexOfOldestMessageFromReservedFileL();
	void GetSmsEntriesL(const TReassemblyEntry& aEntry, CSmsMessage& aSmsMessage, CArrayFix<TInt>& aIndexArray, CArrayFix<TGsmSms>& aSmsArray);
	void SortPDUsL(CArrayFix<TInt>& aIndexArray, CArrayFix<TGsmSms>& aSmsArray);

private:
	//Implementation of private pure virtual function of CReassemblyStore
	virtual void PopulateEntryArrayL(CArrayFix<TReassemblyEntry>& aEntryArray);
	virtual void AddNewMessageL(CSmsMessage& aSmsMessage,const TGsmSms& aGsmSms);
	virtual void UpdateExistingMessageL(CSmsMessage& aSmsMessage,const TGsmSms& aGsmSms, TBool& aDuplicateMsgRef, TBool& aDuplicateSlot);
	virtual void RetrieveMessageL(const TReassemblyEntry& aEntry, CSmsMessage& aSmsMessage);
	virtual void DeleteEntryL(const TReassemblyEntry& aEntry);
	virtual void SetPassedToClientL(const TReassemblyEntry& aEntry, TBool aBool);
	virtual void UpdateLogServerIdL(const TReassemblyEntry& aEntry, TLogId aLogServerId);
	virtual void BeginTransactionLC();
	virtual void CommitTransactionL();

private:
	enum
		{
		KMaxNumberOfClass0MessagesInReassemblyStore = 10
		};

	enum
		{
		KNumberOfPDUSegmentsStoredInOODCondition = 20
		};

	enum
		{
		KGuardTimeOut = 12
		};

private:
	MSmsComm& iSmsComm;
	CSmsPermanentFileStore*	iPermanentFileStore;
	CPreallocatedFile*	iPreallocatedFile;
	CGuardTimer*		iGuardTimer;
	TBool iInTransaction;
	TSmsDiskSpaceMonitorStatus iDiskSpaceStatus;

	TInt iMaxClass0Msg;
	TInt iMaxPDUSeg;
	TInt iGuardTimeOut;
	};

#endif // !defined __SMSPCLASS0STOR_H__

