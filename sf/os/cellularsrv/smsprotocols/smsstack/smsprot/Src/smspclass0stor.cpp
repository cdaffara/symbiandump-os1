// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements CPreallocatedFile.
// 
//

/**
 @file
*/

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "smspclass0storTraces.h"
#endif

#include "smspclass0stor.h"
#include "gsmubuf.h"
#include "gsmunonieoperations.h"

const TInt KSizeOfPreallocatedFileVersion = sizeof(CPreallocatedFile::TPreAllocatedFileVersion);
const TInt KSizeOfNumberOfEntrySection = sizeof(TInt);
//size of 32-bit checksum
const TInt KSizeOfChecksum = sizeof(TUint32);
const TInt KSizeOfPreAllocatedStoreEntry = sizeof(TSmsPreAllocatedFileStoreReassemblyEntry);
const TInt KSizeOfGsmSmsSlotEntry = sizeof(TGsmSmsSlotEntry);
const TInt KSizeOfIndexEntry = sizeof(TInt);
const TInt KSizeOfSmsGsmPDU = sizeof(RMobileSmsMessaging::TMobileSmsGsmTpdu);
const TInt KSizeOfAContainer = KSizeOfGsmSmsSlotEntry + KSizeOfIndexEntry + KSizeOfSmsGsmPDU;
const TInt KBeginOfMasterHeaderSection = KSizeOfPreallocatedFileVersion;

/**
Static factory constructor. Uses two phase 
construction and leaves nothing on the CleanupStack.

@param aFs  File Server handle.
@param aFileName  Permanent file store name.
@param aThirdUid  Uid of file.
@leave KErrNoMemory

@return A pointer to the newly created CSmsPermanentFileStore object.

@pre A connected file server session must be passed as parameter.
@post CSmsPermanentFileStore object is now fully initialised

@internalComponent
*/
CSmsPermanentFileStore* CSmsPermanentFileStore::NewL(RFs& aFs, const TDesC& aFileName, const TUid& aThirdUid)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_NEWL_1, "CSmsPermanentFileStore::NewL()");
	CSmsPermanentFileStore*  self = new (ELeave) CSmsPermanentFileStore(aFs, aThirdUid);
	CleanupStack::PushL(self);
	self->ConstructL(aFileName);
	CleanupStack::Pop(self);

	return self;
	}

void CSmsPermanentFileStore::ConstructL(const TDesC& aFileName)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_CONSTRUCTL_1, "CSmsPermanentFileStore::ConstructL()");
	iFileName = aFileName.AllocL();
	}

/**
 *  Constructor.
*/
CSmsPermanentFileStore::CSmsPermanentFileStore(RFs& aFs, const TUid& aThirdUid)
							: iFs(aFs), iFileName(NULL), iThirdUid(aThirdUid), iFileStore(NULL), iEntryArray(KFlatArrayGranularity)
	{
	}

/**
 *  Destructor. It destroys all the member variables.
*/
CSmsPermanentFileStore::~CSmsPermanentFileStore()
	{
	delete iFileName;
	}

/**
It checks & returns whether the file exist or not. If file is there whether
it is corrupted or not.

@internalComponent
*/
TBool CSmsPermanentFileStore::IsFileOK()
	{
	TBool retBool(EFalse);
	TUidType uidtype(KPermanentFileStoreLayoutUid,KSARStoreUid,iThirdUid);
	TEntry entry;
	TInt ret=iFs.Entry(iFileName->Des(), entry);
	//  Check file existence & corruption
	if ((ret == KErrNone) && (entry.iType == uidtype))
		{
		retBool = ETrue;
		}
	return retBool;
	}

/**
It creates a permanent store file.

@internalComponent
*/
void CSmsPermanentFileStore::CreateL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_CREATEL_1, "CSmsPermanentFileStore::CreateL()");
	TUidType uidtype(KPermanentFileStoreLayoutUid,KSARStoreUid,iThirdUid);
	iFileStore=CPermanentFileStore::ReplaceL(iFs, iFileName->Des(), EFileShareExclusive|EFileStream|EFileRead|EFileWrite);
	iFileStore->SetTypeL(uidtype);
	iEntryArray.Reset();
	ExternalizeEntryArrayL();
	iFileStore->CommitL();
	// Close it to make sure that file is created correctly (defensive approach).
	Close();
	//Again Open the file
	OpenL();
	}

/**
It opens the permanent store file and internalizes the entries.

@internalComponent
*/
void CSmsPermanentFileStore::OpenL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_OPENL_1, "CSmsPermanentFileStore::OpenL()");
	iFileStore=CPermanentFileStore::OpenL(iFs,iFileName->Des(),EFileShareExclusive|EFileStream|EFileRead|EFileWrite);
	InternalizeEntryArrayL();
	}

/**
It closes the permanent store file.

@internalComponent
*/
void CSmsPermanentFileStore::Close()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_CLOSE_1, "CSmsPermanentFileStore::Close()");
	delete iFileStore;
	iFileStore = NULL;
	iEntryArray.Reset();
	}

/*
This function cleans its entries against the passed entries.
This is needed because there might be a scenario where user has deleted a message
but the corresponding message is not deleted from permanent store file
due to out-of-disk condition. But the entry is invalid because
it is no more in the pre-allocated file which contains master header info.
And also at the time of forwarding an incomplete message a forwarded 
message has not been deleted due to above reason.
This function also compacts the store after deletion.

@param aEntryArray entray array against whose clean up is carried out.

@internalComponent
*/
void CSmsPermanentFileStore::CleanupEntriesWithCompactL(const CArrayFix<TSmsPreAllocatedFileStoreReassemblyEntry>& aEntryArray)
	{
    // Ignore in code coverage - a previous CleanupEntries would need to have failed with KErrDiskFull
	BULLSEYE_OFF
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_CLEANUPENTRIESWITHCOMPACTL_1, "CSmsPermanentFileStore::CleanupEntriesWithCompactL()");

	iCompact = ETrue;
	CleanupEntriesL(aEntryArray);
	BULLSEYE_RESTORE	
	}

/*
This function cleans its entries against the passed entries.
This is needed because there migth be a scenario where user has deleted a message
but the corresponding message is not deleted from permanent store file
due to out-of-disk condition. But the entry is invalid because
it is no more in the pre-allocated file which contains master header info.
And also at the time of forwarding an incomplete message a forwarded 
message has not been deleted due to above reason.

@param aEntryArray entray array against whose clean up is carried out.

@internalComponent
*/
void CSmsPermanentFileStore::CleanupEntriesL(const CArrayFix<TSmsPreAllocatedFileStoreReassemblyEntry>& aEntryArray)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_CLEANUPENTRIESL_1, "CSmsPermanentFileStore::CleanupEntriesL()");

	TInt reassemblyCount = iEntryArray.Count();
	TInt index, index2;

	for (index = 0;  index < reassemblyCount;  index++)
		{
		for (index2 = 0;  index2 < aEntryArray.Count();  index2++)
			{
			if (iEntryArray[index].Reference() == aEntryArray[index2].Reference()  &&
			    iEntryArray[index].Total() == aEntryArray[index2].Total()  &&
			    iEntryArray[index].PduType() == aEntryArray[index2].PduType()  &&
				iEntryArray[index].Storage() == aEntryArray[index2].Storage()  &&
				iEntryArray[index].Description2() == aEntryArray[index2].Description2())
				{
				if (aEntryArray[index2].NumberOfPDUsForwardToClient() > 0)
					{
					//Internalize the entries.
					CSmsBuffer*  buffer = CSmsBuffer::NewL();
					CSmsMessage*  smsMessage = CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver, buffer);
					CleanupStack::PushL(smsMessage);

					CArrayFix<TInt>*  indexArray = new(ELeave) CArrayFixFlat<TInt>(KFlatArrayGranularity);
					CleanupStack::PushL(indexArray);
					CArrayFixFlat<TGsmSms>*  smsArray = new(ELeave) CArrayFixFlat<TGsmSms>(KFlatArrayGranularity);
					CleanupStack::PushL(smsArray);

					InternalizeEntryL(index, *smsMessage, *indexArray, *smsArray);
					TInt noOfForwardedEntries = 0;
					for (TInt j=indexArray->Count(); j>0 ;j--)
						{
						TUint8 bitMapIndex = (indexArray->At(j-1)-1)/8;
						TUint8 bitPos = (indexArray->At(j-1)-1)%8;
						TUint8 bitMap;
						TSmsPreAllocatedFileStoreReassemblyEntry entry;
						entry = aEntryArray[index2];
						entry.GetBitMap(bitMapIndex, bitMap);
						TUint8 tmpBitMap = 1;
						tmpBitMap <<= bitPos;
						if (tmpBitMap == (bitMap & tmpBitMap))
							{
							noOfForwardedEntries++;
							indexArray->Delete(j-1);
							smsArray->Delete(j-1);
							}
						}
					if (noOfForwardedEntries > 0)
						{
						TStreamId streamId = iEntryArray[index].DataStreamId();
						ExternalizeEntryL(streamId, *smsMessage, *indexArray, *smsArray);
						}
					CleanupStack::PopAndDestroy(3, smsMessage); // smsMessage, indexArray, smsArray
					}
				break;
				}
			}
		if (index2 == aEntryArray.Count())
			{
			DeleteEntryL(index);
			}
		}
	}

/**
It internalizes its entry array.

@internalComponent
*/
void CSmsPermanentFileStore::InternalizeEntryArrayL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_INTERNALIZEENTRYARRAYL_1, "CSmsPermanentFileStore::InternalizeEntryArrayL()");

	iEntryArray.Reset();
	TStreamId headerid=iFileStore->Root();
	RStoreReadStream stream;
	stream.OpenLC(*iFileStore,headerid);
	TInt count=stream.ReadInt32L();
	for (TInt i=0; i<count; i++)
		{
		TSmsReassemblyEntry sarentry;
		stream >> sarentry;
		iEntryArray.AppendL(sarentry);
		}
	CleanupStack::PopAndDestroy();  //  stream
	}

/**
It externalizes its entry array.

@internalComponent
*/
void CSmsPermanentFileStore::ExternalizeEntryArrayL()
	{
	OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_EXTERNALIZEENTRYARRAYL_1, "CSmsPermanentFileStore::ExternalizeEntryArrayL(): this=0x%08X count=%d headerid=%u]",(TUint) this, iEntryArray.Count(), (TUint)iFileStore->Root().Value());

	TStreamId headerid=iFileStore->Root();
	RStoreWriteStream stream;
	if (headerid==KNullStreamId)
		{
		headerid=stream.CreateLC(*iFileStore);
		iFileStore->SetRootL(headerid);
		}
	else
		{
		stream.ReplaceLC(*iFileStore,headerid);
		}

	TInt count1=iEntryArray.Count();
	TInt count2=0;
	TInt i=0;

	for (; i<count1; i++)
		{
		if (!iEntryArray[i].IsDeleted())
			{
			count2++;
			}
		}
	stream.WriteInt32L(count2);
	for (i=0; i<count1; i++)
		{
		if (!iEntryArray[i].IsDeleted())
			{
			stream << iEntryArray[i];
			}
		}

	stream.CommitL();
	CleanupStack::PopAndDestroy(&stream);
	}

/*
It adds the new message in permanent store file.

@param aIndex (output) index number on which message is added.
@param aSmsMessage reference to sms message to be added.
@param aGsmSms reference to GsmSms object to be added.

@internalComponent
*/
void CSmsPermanentFileStore::AddNewMessageL(TInt& aIndex, CSmsMessage& aSmsMessage,const TGsmSms& aGsmSms)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_ADDNEWMESSAGEL_1, "CSmsPermanentFileStore::AddNewMessageL");

	CArrayFix<TInt>* indexArray=new(ELeave) CArrayFixFlat<TInt>(KFlatArrayGranularity);
	CleanupStack::PushL(indexArray);
	CArrayFixFlat<TGsmSms>* smsArray=new(ELeave) CArrayFixFlat<TGsmSms>(KFlatArrayGranularity);
	CleanupStack::PushL(smsArray);
	//Index must initialize to 0.
	TInt index = 0;
	if (!aSmsMessage.IsDecoded())
		{
		index = aSmsMessage.SmsPDU().ConcatenatedMessagePDUIndex();
		}
	indexArray->AppendL(index);
	smsArray->AppendL(aGsmSms);

	TStreamId streamid = KNullStreamId;
	ExternalizeEntryL(streamid, aSmsMessage, *indexArray, *smsArray);

	TSmsReassemblyEntry tmpEntry;
	//Fill-up entry information...
	CReassemblyStoreUtility::PopulateEntry(tmpEntry, aSmsMessage, 1);
	// Update Data stream id.
	tmpEntry.SetDataStreamId(streamid);

	aIndex = iEntryArray.Count();
	AddEntryL(tmpEntry);
	CleanupStack::PopAndDestroy(2);	//indexArray, smsArray
	}

/*
It updates the existing message in permanent store file.

@param aIndex index number on which message is to be updated.
@param aSmsMessage reference to sms message to be updated.
@param aIndexArray array of index of all the PDUs.
@param aSmsArray array of sms of all the PDUs.

@internalComponent
*/
void CSmsPermanentFileStore::UpdateExistingMessageL(TInt aIndex, const CSmsMessage& aSmsMessage,const CArrayFix<TInt>& aIndexArray,const CArrayFix<TGsmSms>& aSmsArray)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_UPDATEEXISTINGMESSAGEL_1, "CSmsPermanentFileStore::UpdateExistingMessageL()");
	TStreamId  streamid = iEntryArray[aIndex].DataStreamId();
	ExternalizeEntryL(streamid, aSmsMessage, aIndexArray, aSmsArray);
	TSmsReassemblyEntry entry;
	CReassemblyStoreUtility::PopulateEntry(entry, aSmsMessage, aSmsArray.Count());
	entry.SetDataStreamId(streamid);
	ChangeEntryL(aIndex, entry);
	}

/*
It matches the entry with entry in permanent store file & returns
the index. If not found it returns KErrNotFound.

@param aEntry reference to entry information.
@param aIndex (output) returns the index number.

@internalComponent
*/
void CSmsPermanentFileStore::MatchEntryToExistingMessage(const TReassemblyEntry& aEntry,
													TInt& aIndex)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_MATCHENTRYTOEXISTINGMESSAGE_1, "CSmsPermanentFileStore::MatchEntryToExistingMessage()");

	aIndex = KErrNotFound;

	//
	// Search the reassembly store for a matching entry...
	//
	TInt reassemblyCount = iEntryArray.Count();
	for (TInt  index = 0;  index < reassemblyCount;  index++)
		{
		TSmsReassemblyEntry&  entry = iEntryArray[index];

		if (entry.Reference() == aEntry.Reference()  &&
			entry.Total() == aEntry.Total()  &&
			entry.PduType() == aEntry.PduType()  &&
			entry.Storage() == aEntry.Storage()  &&
			entry.Description2() == aEntry.Description2())
			{
			//
			// Found it!
			//
			aIndex = index;
			break;
			}
		}

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_MATCHENTRYTOEXISTINGMESSAGE_2, "CSmsPermanentFileStore::MatchEntryToExistingMessage(): aIndex=%d", aIndex);
	}

/*
It updates the log server id of the message.

@param aIndex index number of the message to be updated.
@param aLogServerId log server id.

@internalComponent
*/
void CSmsPermanentFileStore::UpdateLogServerIdL(TInt& aIndex, TLogId aLogServerId)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_UPDATELOGSERVERIDL_1, "CSmsPermanentFileStore::UpdateLogServerIdL");

	TSmsReassemblyEntry entry;
	entry = iEntryArray[aIndex];

	if (entry.LogServerId() != aLogServerId)
		{
		entry.SetLogServerId(aLogServerId);
		ChangeEntryL(aIndex, entry);
		}
	}

/*
It sets the message passed to client or not.

@param aIndex index number of the message to be updated.
@param aBool boolean value indicating whether message is passes or not.

@internalComponent
*/
void CSmsPermanentFileStore::SetPassedToClientL(TInt aIndex, TBool aBool)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_SETPASSEDTOCLIENTL_1, "CSmsPermanentFileStore::SetPassedToClientL(): aIndex=%d", aIndex);

	TSmsReassemblyEntry entry;
	entry = iEntryArray[aIndex];

	if (entry.PassedToClient() != aBool)
		{
		entry.SetPassedToClient(aBool);
		ChangeEntryL(aIndex, entry);
		}
	}

/*
It adds the new entry in the existing entry array.

@param aEntry entry to be added.

@internalComponent
*/
void CSmsPermanentFileStore::AddEntryL(TSmsReassemblyEntry& aEntry)
	{
	iEntryArray.AppendL(aEntry);
	iEntryArray[iEntryArray.Count()-1].SetIsAdded(ETrue);
	}

/*
It changes the existing entry with new entry.

@param aIndex index number of the entry which will be changed.
@param aNewEntry entry to be updated.

@internalComponent
*/
void CSmsPermanentFileStore::ChangeEntryL(TInt aIndex,const TSmsReassemblyEntry& aNewEntry)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_CHANGEENTRYL_1, "CSmsPermanentFileStore::ChangeEntryL(): aIndex=%d", aIndex);

	iEntryArray[aIndex].SetIsDeleted(ETrue);
	iEntryArray.InsertL(aIndex,aNewEntry);
	iEntryArray[aIndex].SetIsAdded(ETrue);
	}

/*
It deletes the entry.

@param aIndex index number of the entry which will be deleted.

@internalComponent
*/
void CSmsPermanentFileStore::DeleteEntryL(TInt aIndex)
	{
	iFileStore->DeleteL(iEntryArray[aIndex].DataStreamId());
	iEntryArray[aIndex].SetIsDeleted(ETrue);
	}

/*
It externalizes(writes) the entry in permanent store file.

@param aStreamId stream id which needs to externalized.
@param aSmsMessage reference to sms message which needs to be externalized.
@param aIndexArray refence to array of index which needs to be externalized.
@param aSmsArray refence to array of sms which needs to be externalized.

@internalComponent
*/
void CSmsPermanentFileStore::ExternalizeEntryL(TStreamId& aStreamId,const CSmsMessage& aSmsMessage,const CArrayFix<TInt>& aIndexArray,const CArrayFix<TGsmSms>& aSmsArray)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_EXTERNALIZEENTRYL_1, "CSmsPermanentFileStore::ExternalizeEntryL Start [sid=%d]", aStreamId.Value());

	RStoreWriteStream writestream;
	if (aStreamId==KNullStreamId)
		aStreamId=writestream.CreateLC(*iFileStore);
	else
		writestream.ReplaceLC(*iFileStore,aStreamId);
	writestream << aSmsMessage;
	TInt count=aIndexArray.Count();
	writestream.WriteInt32L(count);
	TInt i=0;
	for (; i<count; i++)
		writestream.WriteInt32L(aIndexArray[i]);
	count=aSmsArray.Count();
	writestream.WriteInt32L(count);
	for (i=0; i<count; i++)
		{
		RMobileSmsMessaging::TMobileSmsGsmTpdu pdu;
		pdu=aSmsArray[i].Pdu();
		writestream << pdu;
		}
	writestream.CommitL();
	CleanupStack::PopAndDestroy();

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_EXTERNALIZEENTRYL_2, "CClass0PermanentFileStore::ExternalizeEntryL End [count=%d]", count);
	}

/*
It internalizes(reads) the entry from permanent store file.

@param aIndex index number of the message to be internalized.
@param aSmsMessage (output) reference to sms message.
@param aIndexArray (output) refence to array of index.
@param aSmsArray (output) refence to array of sms.

@internalComponent
*/
void CSmsPermanentFileStore::InternalizeEntryL(const TInt aIndex, CSmsMessage& aSmsMessage, CArrayFix<TInt>& aIndexArray, CArrayFix<TGsmSms>& aSmsArray)
	{
	TSmsReassemblyEntry&  entry = iEntryArray[aIndex];
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_INTERNALIZEENTRYL_1, "CSmsPermanentFileStore::InternalizeEntryL Start [sid=%d]", entry.DataStreamId().Value());
	RStoreReadStream readstream;
	readstream.OpenLC(*iFileStore, entry.DataStreamId());
	readstream >> aSmsMessage;
	TInt count=readstream.ReadInt32L();
	TInt i;
	for (i=0; i<count; i++)
		{
		TInt index=readstream.ReadInt32L();
		aIndexArray.AppendL(index);
		}
	count=readstream.ReadInt32L();
	if(count!=aIndexArray.Count())
		{
		User::Leave(KErrCorrupt);
		}
	for (i=0; i<count; i++)
		{
		RMobileSmsMessaging::TMobileSmsGsmTpdu pdu;
		readstream >> pdu;
		TGsmSms sms;
		sms.SetPdu(pdu);
		aSmsArray.AppendL(sms);
		}
	CleanupStack::PopAndDestroy();
	//Set other properties of CSmsMessage
	aSmsMessage.SetStorage(entry.Storage());
	aSmsMessage.SetLogServerId(entry.LogServerId());
	aSmsMessage.SetTime(entry.Time());
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_INTERNALIZEENTRYL_2, "CSmsPermanentFileStore::InternalizeEntryL End [count=%d]", count);
	}

/*
It removes the PDUs from permanent store file.
This function is needed because after forwarding the incomplete message
to client, the corresponding PDUs needs to be be removed from permanent
store file.
This functionality is specific to class 0 re-assembly store.

@param aIndex index number of the message to be removed.
@param aStartPos starting pos of pdu to be removed.
@param aEndPos end pos of pdu to be removed.

@internalComponent
*/
void CSmsPermanentFileStore::RemovePDUsL(TInt aIndex, TInt aStartPos, TInt aEndPos)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_REMOVEPDUSL_1, "CSmsPermanentFileStore::RemovePDUsL");

	CSmsBuffer*  buffer = CSmsBuffer::NewL();
	CSmsMessage*  smsMessage = CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver, buffer);
	CleanupStack::PushL(smsMessage);

	CArrayFix<TInt>*  indexArray = new(ELeave) CArrayFixFlat<TInt>(KFlatArrayGranularity);
	CleanupStack::PushL(indexArray);
	CArrayFixFlat<TGsmSms>*  smsArray = new(ELeave) CArrayFixFlat<TGsmSms>(KFlatArrayGranularity);
	CleanupStack::PushL(smsArray);

	InternalizeEntryL(aIndex, *smsMessage, *indexArray, *smsArray);

	TInt count = indexArray->Count();
	for (TInt i=count; i>0; i--)
		{
		if ((indexArray->At(i-1) >= aStartPos) && (indexArray->At(i-1) <= aEndPos))
			{
			indexArray->Delete(i-1);
			smsArray->Delete(i-1);
			}
		}

	/*
	There are 3 scenarios in this case:
	1. If all the entries are removed,
	then there is no need to store the entry in this permanent file.
	2. If few entries are removed,
	then externalize the remaining entries. Update count field also.
	3. If no entries are removed, then do nothing.
	*/
	if (indexArray->Count()==0)
		{
		DeleteEntryL(aIndex);
		}
	else if (count!=indexArray->Count())
		{
		TStreamId  streamid = iEntryArray[aIndex].DataStreamId();
		ExternalizeEntryL(streamid, *smsMessage, *indexArray, *smsArray);

		TSmsReassemblyEntry entry;
		entry = iEntryArray[aIndex];
		entry.SetCount(indexArray->Count());
		ChangeEntryL(aIndex, entry);
		}
	CleanupStack::PopAndDestroy(3, smsMessage); // smsMessage, indexArray, smsArray
	}

/**
 *  Sets the permanent store as in-transaction.
 *  
 *  The function checks the validity of the call and leaves KErrAccessDenied if
 *  invalid.
 *  @capability None
 */
void CSmsPermanentFileStore::BeginTransactionL()
	{
    OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_BEGINTRANSACTIONL_1, "CSmsPermanentFileStore::BeginTransactionL [this=0x%08X iInTransaction=%d iFileStore=0x%08X]", (TUint)this, iInTransaction, (TUint)iFileStore);

	if (iFileStore == NULL || iInTransaction)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_BEGINTRANSACTIONL_2, "WARNING CSmsPermanentFileStore::BeginTransactionL leaving with KErrAccessDenied");
		User::Leave(KErrAccessDenied);
		}

	iInTransaction = ETrue;
	} // CSmsPermanentFileStore::BeginTransactionL

/**
 *  It reverts the transaction.
 */
void CSmsPermanentFileStore::Revert()
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_REVERT_1, "CSmsPermanentFileStore::Revert(): this=0x%08X, iInTransaction=%d",(TUint)this, iInTransaction);

	iFileStore->Revert();
	iInTransaction = EFalse;
	ReinstateDeletedEntries();
	} // CSmsPermanentFileStore::Revert

/**
 *  It commits the transaction. Then it compact the permanent store file.
 */
void CSmsPermanentFileStore::DoCommitAndCompactL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_DOCOMMITANDCOMPACTL_1, "CSmsPermanentFileStore::DoCommitAndCompactL()");

#if (OST_TRACE_CATEGORY & OST_TRACE_CATEGORY_DEBUG) 
    TBuf<40> timestamp;
    SmsTimeStampL(timestamp);
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS,CSMSPERMANENTFILESTORE_DOCOMMITANDCOMPACTL_2, "%S",timestamp);
#endif
	iFileStore->CommitL();
#if (OST_TRACE_CATEGORY & OST_TRACE_CATEGORY_DEBUG) 
    SmsTimeStampL(timestamp);
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS,CSMSPERMANENTFILESTORE_DOCOMMITANDCOMPACTL_3, "%S",timestamp);
#endif

	iCommitCount--;
	if ((iCommitCount < 0) || (iCompact))
		{
		iCommitCount = KNumStoreCommitsBeforeCompaction;
		iFileStore->CompactL();
		iFileStore->CommitL();
		iCompact = EFalse;
		}
	} // CSmsPermanentFileStore::DoCommitAndCompactL

/**
 *  It commits the transaction.
 */
void CSmsPermanentFileStore::CommitTransactionL()
	{
	OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_COMMITTRANSACTIONL_1, "CSmsPermanentFileStore::CommitTransactionL(): this=0x%08X iInTransaction=%d iFileStore=0x%08X",(TUint)this, iInTransaction, (TUint)iFileStore);

	ExternalizeEntryArrayL();

#ifdef OST_TRACE_COMPLIER_IN_USE
	TRAPD(err, DoCommitAndCompactL());
	if (err != KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_COMMITTRANSACTIONL_2, "WARNING! could not CommitL/CompactL due to %d", err);
		User::Leave(err);
		}
#else
	DoCommitAndCompactL();
#endif

	iInTransaction = EFalse;
	RemoveDeletedEntries();
	}

/**
 *  It removes the deleted entries from entry arry.
 *	This function is called after commit.
 */
void CSmsPermanentFileStore::RemoveDeletedEntries()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_REMOVEDELETEDENTRIES_1, "CSmsPermanentFileStore::RemoveDeletedEntries()");

	TInt count=iEntryArray.Count();
	while (count--)
		{
		TSmsReassemblyEntry& entry = iEntryArray[count];

		if (entry.IsDeleted())
			{
			iEntryArray.Delete(count);
			}
		else
			{
			entry.SetIsAdded(EFalse);
			}
		}
	} // CSmsPermanentFileStore::RemoveDeletedEntries

/**
 *  It reinstate the deleted/added entries from entry arry.
 *	This function is called after revert operation.
 */
void CSmsPermanentFileStore::ReinstateDeletedEntries()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPERMANENTFILESTORE_REINSTATEDELETEDENTRIES_1, "CSmsPermanentFileStore::ReinstateDeletedEntries()");

	TInt count=iEntryArray.Count();
	while (count--)
		{
		TSmsReassemblyEntry& entry = iEntryArray[count];

		if (entry.IsAdded())
			{
			iEntryArray.Delete(count);
			}
		else
			{
			entry.SetIsDeleted(EFalse);
			}
		}
	} // CSmsPermanentFileStore::ReinstateDeletedEntries

/**
 *  Constructor
 *  
 *  @capability None
 */
TSmsPreAllocatedFileStoreReassemblyEntry::TSmsPreAllocatedFileStoreReassemblyEntry():
	TSmsReassemblyEntry(),
	iPreAllocatedStorageId(0),
	iStatus(RMobileSmsStore::EStoredMessageUnknownStatus),
	iTimeOffset(0),
	iDecodedOnSim(EFalse),
	iForwardToClient(EFalse),
	iForwardedCount(0),
	iBitMap(NULL)
	{
	//Have to externalize so initialize to 0.
	for (TInt i=0; i<KBitMapLen; i++)
		{
		iBitMap.Append(0x00);
		}
	}

/*
It returns the reference of a particular bit-map index.

@param aIndex index number of the bit-map.
@param aBitMap bit-map value.

@internalComponent
*/
void TSmsPreAllocatedFileStoreReassemblyEntry::GetBitMap(TUint8 aIndex, TUint8& aBitMap)
	{
	aBitMap = iBitMap[aIndex];
	}

/*
It sets the value of a bit-map.

@param aIndex index number of the bit-map.
@param aBitMap bit-map value

@internalComponent
*/
void TSmsPreAllocatedFileStoreReassemblyEntry::SetBitMap(TUint8 aIndex, TUint8 aBitMap)
	{
	iBitMap[aIndex] = aBitMap;
	}

/**
Static factory constructor. Uses two phase 
construction and leaves nothing on the CleanupStack.

@param aFs  File Server handle.
@param aFileName  Permanent file store name.
@param aMaxClass0Msg max class 0 message that can be stored.
@param aMaxPDUSeg max number of pdus that can be stored.
@param aVersion version number of pre-allocated file.

@return A pointer to the newly created CPreallocatedFile object.

@pre A connected file server session must be passed as parameter.
@post CPreallocatedFile object is now fully initialised

@internalComponent
*/
CPreallocatedFile* CPreallocatedFile::NewL(RFs& aFs, const TDesC& aFileName, TInt aMaxClass0Msg, TInt aMaxPDUSeg, TPreAllocatedFileVersion aVersion)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_NEWL_1, "CPreallocatedFile::NewL()");
	CPreallocatedFile*  self = new (ELeave) CPreallocatedFile(aFs, aMaxClass0Msg, aMaxPDUSeg, aVersion);
	CleanupStack::PushL(self);
	self->ConstructL(aFileName);
	CleanupStack::Pop(self);

	return self;
	}

void CPreallocatedFile::ConstructL(const TDesC& aFileName)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_CONSTRUCTL_1, "CPreallocatedFile::ConstructL()");
	iFileName = aFileName.AllocL();
	}

/**
 *  Constructor.
*/
CPreallocatedFile::CPreallocatedFile(RFs& aFs, TInt aMaxClass0Msg, TInt aMaxPDUSeg, TPreAllocatedFileVersion aVersion)
	:iFs(aFs), iEntryArray(KFlatArrayGranularity), iReinstateEntryInfo(KFlatArrayGranularity), iMaxClass0Msg(aMaxClass0Msg), iMaxPDUSeg(aMaxPDUSeg), iVersion(aVersion)
	{
	/*
	Format of File:
	Version Number, Header Section & Data Section
	Version Number - Interger Value.
	Header Section: Number of entries, Array of Entries, Array of PDU identifier section, Checksum.
	Data Section: Array of Container. Each container contains Sms slot informatiuon, index number & PDU.
	*/

	// Calculate the size of each section.

	// Entry section will always contain one more entry than configured one by the user.
	// Because it will provide one extra slot to store the new message for time being
	// before forwarding the oldest message.
	iSizeOfEntrySection = (KSizeOfPreAllocatedStoreEntry * (iMaxClass0Msg + 1));

	iSizeOfStorageIdentifierSection = ((sizeof(TInt))*(iMaxPDUSeg+1));

	TInt sizeOfHeaderSection = KSizeOfNumberOfEntrySection + iSizeOfEntrySection + iSizeOfStorageIdentifierSection + KSizeOfChecksum;

	TInt sizeOfDataSection = KSizeOfAContainer  * (iMaxPDUSeg+1);

	// Calculate the size of File.
	iSizeOfFile = KSizeOfPreallocatedFileVersion + 2 * sizeOfHeaderSection + sizeOfDataSection;

	iBeginOfDuplicateHeaderSection = KSizeOfPreallocatedFileVersion + sizeOfHeaderSection;
	iBeginOfDataSection = KSizeOfPreallocatedFileVersion + 2 * sizeOfHeaderSection;
	}

/**
 *  Destructor. It destroys all the member variables.
*/
CPreallocatedFile::~CPreallocatedFile()
	{
	delete iFileName;
	}

/**
It checks & returns whether the file exist or not. If file is there whether
it is corrupted or not.

@internalComponent
*/
TBool CPreallocatedFile::IsFileOK()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_ISFILEOK_1, "CPreallocatedFile::IsFileOK()");

	TEntry entry;
	//  Check file exists
	TInt ret=iFs.Entry(iFileName->Des(), entry);
	// Check the size of file, if size does not match then assume that file
	// is corrupted, then return KErrNotFound.
	if ((ret == KErrNone) && (entry.iSize != iSizeOfFile))
		{
		ret = KErrNotFound;
		}

	if (ret == KErrNone)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

/**
It creates a pre-allocated file.

@internalComponent
*/
void CPreallocatedFile::CreateL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_CREATEL_1, "CPreallocatedFile::CreateL");

	User::LeaveIfError(iFile.Replace(iFs, iFileName->Des(), EFileWrite));
	User::LeaveIfError(iFile.SetSize(iSizeOfFile));
	iFile.Flush();

	// Externalize Version Number
	//TInt version = iVersion;
	TPtr8 memPtr((TUint8*) &iVersion, KSizeOfPreallocatedFileVersion, KSizeOfPreallocatedFileVersion);
	iFile.Write(0, memPtr);

	//Externalize Header Information
	ExternalizeEntryArray();

	// Initialize Storage Identifier Section.
	TInt storageIdentifier=0;
	memPtr.Set((TUint8*) &storageIdentifier, sizeof(storageIdentifier), sizeof(storageIdentifier));
	TInt pos = KSizeOfNumberOfEntrySection + iSizeOfEntrySection;
	TInt pos2 = 0;
	for (TInt count=0; count<iMaxPDUSeg+1; count++)
		{
		pos2 = pos + count * sizeof(storageIdentifier);
		iFile.Write(KBeginOfMasterHeaderSection + pos2, memPtr);
		//iFile.Write(iBeginOfDuplicateHeaderSection + pos2, memPtr);
		}

	// Initialize Checksum
	PutChecksumValueL();

	// Close it to make sure that file is created correctly (defensive approach).
	iFile.Close();
	//Again Open the file
	OpenL();
	}

/**
It opens the pre-allocated file. Then it internalizes all the entry information.

@internalComponent
*/
void CPreallocatedFile::OpenL()
	{
	User::LeaveIfError(iFile.Open(iFs, iFileName->Des(), EFileShareExclusive|EFileRead|EFileWrite));
	CleanupClosePushL(iFile);
	// Check the validity of the data.
	CheckDataL();
	// Internalize data
	InternalizeEntryArrayL();
	CleanupStack::Pop(&iFile);
	}

/**
It closes the pre-allocated file.

@internalComponent
*/
void CPreallocatedFile::Close()
	{
	iFile.Close();
	iEntryArray.Reset();
	iReinstateEntryInfo.Reset();
	}

/**
It internalizes the entry info from pre-allocated file.

@internalComponent
*/
void CPreallocatedFile::InternalizeEntryArrayL()
	{
	iEntryArray.Reset();
	TInt numberOfMessage;
	TPtr8 memPtr((TUint8*) &numberOfMessage, sizeof(numberOfMessage), sizeof(numberOfMessage));
	iFile.Read(KBeginOfMasterHeaderSection, memPtr);

	TSmsPreAllocatedFileStoreReassemblyEntry tmpClass0ReassemblyStore;
	memPtr.Set((TUint8*) &tmpClass0ReassemblyStore, KSizeOfPreAllocatedStoreEntry, KSizeOfPreAllocatedStoreEntry);

	TInt pos = 0;
	for (TInt count=0; count < numberOfMessage; count++)
		{
		pos = sizeof(TInt) + (count * KSizeOfPreAllocatedStoreEntry);
		iFile.Read(KBeginOfMasterHeaderSection + pos, memPtr);
		iEntryArray.AppendL(tmpClass0ReassemblyStore);
		}
	}

/**
It externalizes the entry info to pre-allocated file.

@internalComponent
*/
void CPreallocatedFile::ExternalizeEntryArray()
	{
	TInt count=iEntryArray.Count();
	TInt numberOfMessage=0;
	TInt i=0;

	for (; i<count; i++)
		{
		if (!iEntryArray[i].IsDeleted())
			{
			numberOfMessage++;
			}
		}

	//Externalize number of mesages.
	TPtr8 memPtr((TUint8*) &numberOfMessage, sizeof(numberOfMessage), sizeof(numberOfMessage));
	iFile.Write(KBeginOfMasterHeaderSection, memPtr);

	TInt entryNumber = 0;
	//Externalize all the entries.
	for (i=0; i<count; i++)
		{
		if (!iEntryArray[i].IsDeleted())
			{
			//At the time of externalizing don't externalize IsAdded(), IsDeleted() value
			TBool isAdded = iEntryArray[i].IsAdded();
			TBool isDeleted = iEntryArray[i].IsDeleted();
			iEntryArray[i].SetIsAdded(EFalse);
			iEntryArray[i].SetIsDeleted(EFalse);
			memPtr.Set((TUint8*) &iEntryArray[i], KSizeOfPreAllocatedStoreEntry, KSizeOfPreAllocatedStoreEntry);
			iFile.Write(KBeginOfMasterHeaderSection + KSizeOfNumberOfEntrySection + (entryNumber * KSizeOfPreAllocatedStoreEntry), memPtr);
			//Again update IsAdded(), IsDeleted() with previous value
			iEntryArray[i].SetIsAdded(isAdded);
			iEntryArray[i].SetIsDeleted(isDeleted);
			entryNumber++;
			}
		}

	count = numberOfMessage;
	/*
	Externalize extra entry information with default information.
	This initialization is required because of checksum.
	*/
	TSmsPreAllocatedFileStoreReassemblyEntry tmpClass0ReassemblyStore;
	memPtr.Set((TUint8*) &tmpClass0ReassemblyStore, KSizeOfPreAllocatedStoreEntry, KSizeOfPreAllocatedStoreEntry);
	TInt pos = 0;
	for (; count<iMaxClass0Msg + 1; count++)
		{
		pos = KSizeOfNumberOfEntrySection + (count * KSizeOfPreAllocatedStoreEntry);
		iFile.Write(KBeginOfMasterHeaderSection + pos, memPtr);
		}
	iFile.Flush();
	}

/*
It adds the new message in pre-allocated file.

@param aIndex (output) index number on which message is added.
@param aSmsMessage reference to sms message to be added.
@param aGsmSms reference to GsmSms object to be added.

@internalComponent
*/
void CPreallocatedFile::AddNewMessageL(TInt& aIndex, CSmsMessage& aSmsMessage,const TGsmSms& aGsmSms)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_ADDNEWMESSAGEL_1, "CPreallocatedFile::AddNewMessageL");
	//Gets the next free slot where the message will be stored.
	TInt nextFreeSlot = GetFreeContainer();
	TInt pduIndex=aSmsMessage.IsDecoded()? 0: aSmsMessage.SmsPDU().ConcatenatedMessagePDUIndex();
	if (aSmsMessage.Storage() == CSmsMessage::ESmsSIMStorage  ||
		aSmsMessage.Storage() == CSmsMessage::ESmsCombinedStorage)
		{
		const TGsmSmsSlotEntry&  newSlot = aSmsMessage.iSlotArray[0];
		ExternalizeEntry(nextFreeSlot, newSlot, pduIndex, aGsmSms);
		}
	else
		{
		ExternalizeEntry(nextFreeSlot, pduIndex, aGsmSms);
		}

	/*
	Gets the unique id which will identify the containers where the 
	PDU related to this messsage are stored.
	*/
	TInt freeStorageId = GetFreeStorageId();
	AddStorageIdL(nextFreeSlot, freeStorageId);

	TSmsPreAllocatedFileStoreReassemblyEntry tmpEntry;
	//Fill-up entry information...
	CReassemblyStoreUtility::PopulateEntry(tmpEntry, aSmsMessage, 1);
	tmpEntry.SetPreAllocatedStorageId(freeStorageId);
	//The properties below need to be set only once when a PDU of a new message arrives.
	tmpEntry.SetStatus((RMobileSmsStore::TMobileSmsStoreStatus)aSmsMessage.Status());
	tmpEntry.SetUTCOffset(aSmsMessage.UTCOffset());
	tmpEntry.SetDecodedOnSIM(aSmsMessage.DecodedOnSim());
	tmpEntry.SetForwardToClient(aSmsMessage.ForwardToClient());

	// Add & extrenalize entry array.
	aIndex = iEntryArray.Count();
	AddEntryL(tmpEntry);
	}

/*
It updates the existing message in permanent store file.

@param aIndex index number on which message is to be updated.
@param aSmsMessage reference to sms message to be updated.
@param aPduIndex index of the PDU to be updated.
@param aSms sms of the PDU to be updated.

@internalComponent
*/
void CPreallocatedFile::UpdateExistingMessageL(TInt aIndex, const CSmsMessage& aSmsMessage, TInt aPduIndex, const TGsmSms& aSms)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_UPDATEEXISTINGMESSAGEL_1, "CPreallocatedFile::UpdateExistingMessageL()");
	TInt preAllocatedStorageId = iEntryArray[aIndex].PreAllocatedStorageId();
	if (preAllocatedStorageId == KErrNotFound)
		{
		/*
		This condition arises when part of message is stored in permanent store file & 
		other parts arrive when system is out of disk.
		*/
		preAllocatedStorageId = GetFreeStorageId();
		iEntryArray[aIndex].SetPreAllocatedStorageId(preAllocatedStorageId);
		}
	// Externalize Entry in one of free containers.
	TInt freeSlot = GetFreeContainer();
	if (aSmsMessage.Storage() == CSmsMessage::ESmsSIMStorage  ||
		aSmsMessage.Storage() == CSmsMessage::ESmsCombinedStorage)
		{
		const TGsmSmsSlotEntry&  newSlot = aSmsMessage.iSlotArray[0];
		ExternalizeEntry(freeSlot, newSlot, aPduIndex, aSms);
		}
	else
		{
		ExternalizeEntry(freeSlot, aPduIndex, aSms);
		}

	AddStorageIdL(freeSlot, preAllocatedStorageId);
	TSmsPreAllocatedFileStoreReassemblyEntry entry;
	entry = iEntryArray[aIndex];
	/*
	This value must be set because this may the first PDU of an 
	existing message which is stored in pre-allocated store file.
	*/
	entry.SetPreAllocatedStorageId(preAllocatedStorageId);
	entry.SetCount(entry.Count()+1);
	ChangeEntryL(aIndex, entry);
	}

/*
It matches the entry with entry in pre-allocated file & returns
the index. If not found it returns KErrNotFound.

@param aEntry reference to entry information.
@param aIndex (output) returns the index number.

@internalComponent
*/
void CPreallocatedFile::MatchEntryToExistingMessage(const TReassemblyEntry& aEntry,
													TInt& aIndex)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_MATCHENTRYTOEXISTINGMESSAGE_1, "CPreallocatedFile::MatchEntryToExistingMessage()");

	aIndex = KErrNotFound;

	//
	// Search the reassembly store for a matching entry...
	//
	TInt reassemblyCount = iEntryArray.Count();

	for (TInt  index = 0;  index < reassemblyCount;  index++)
		{
		TSmsPreAllocatedFileStoreReassemblyEntry&  entry = iEntryArray[index];

		if (entry.Reference() == aEntry.Reference()  &&
		    entry.Total() == aEntry.Total()  &&
		    entry.PduType() == aEntry.PduType()  &&
			entry.Storage() == aEntry.Storage()  &&
			entry.Description2() == aEntry.Description2())
			{
			//
			// Found it!
			//
			aIndex = index;
			break;
			}
		}

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_MATCHENTRYTOEXISTINGMESSAGE_2, "CPreallocatedFile::MatchEntryToExistingMessage(): aIndex=%d", aIndex);
	}

/*
It updates the log server id of the message.

@param aIndex index number of the message to be updated.
@param aLogServerId log server id.

@internalComponent
*/
void CPreallocatedFile::UpdateLogServerIdL(TInt& aIndex, TLogId aLogServerId)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_UPDATELOGSERVERIDL_1, "CPreallocatedFile::UpdateLogServerId");

	TSmsPreAllocatedFileStoreReassemblyEntry entry;
	entry = iEntryArray[aIndex];

	if (entry.LogServerId() != aLogServerId)
		{
		entry.SetLogServerId(aLogServerId);
		ChangeEntryL(aIndex, entry);
		}
	}

/*
It sets the message passed to client or not.

@param aIndex index number of the message to be updated.
@param aBool boolean value indicating whether message is passes or not.

@internalComponent
*/
void CPreallocatedFile::SetPassedToClientL(TInt aIndex, TBool aBool)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_SETPASSEDTOCLIENTL_1, "CPreallocatedFile::SetPassedToClientL(): aIndex=%d", aIndex);

	TSmsPreAllocatedFileStoreReassemblyEntry entry;
	entry = iEntryArray[aIndex];

	if (entry.PassedToClient() != aBool)
		{
		entry.SetPassedToClient(aBool);
		ChangeEntryL(aIndex, entry);
		}
	}

/*
It adds the new entry in the existing entry array.

@param aEntry entry to be added.

@internalComponent
*/
void CPreallocatedFile::AddEntryL(TSmsPreAllocatedFileStoreReassemblyEntry& aEntry)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_ADDENTRYL_1, "CPreallocatedFile::AddEntryL");
	iEntryArray.AppendL(aEntry);
	iEntryArray[iEntryArray.Count()-1].SetIsAdded(ETrue);
	}

/*
It changes the existing entry with new entry.

@param aIndex index number of the entry which will be changed.
@param aNewEntry entry to be updated.

@internalComponent
*/
void CPreallocatedFile::ChangeEntryL(TInt aIndex, const TSmsPreAllocatedFileStoreReassemblyEntry& aNewEntry)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_CHANGEENTRYL_1, "CPreallocatedFile::ChangeEntryL(): aIndex=%d", aIndex);
	iEntryArray[aIndex].SetIsDeleted(ETrue);
	iEntryArray.InsertL(aIndex,aNewEntry);
	iEntryArray[aIndex].SetIsAdded(ETrue);
	}

/*
It deletes the entry.

@param aIndex index number of the entry which will be deleted.

@internalComponent
*/
void CPreallocatedFile::DeleteEntryL(TInt aIndex)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_DELETEENTRYL_1, "CPreallocatedFile::DeleteEntryL(): aIndex=%d", aIndex);
	if (iEntryArray[aIndex].PreAllocatedStorageId() != KErrNotFound)
		{
		ClearEntryL(iEntryArray[aIndex].PreAllocatedStorageId(), iEntryArray[aIndex].Count());
		}
	iEntryArray[aIndex].SetIsDeleted(ETrue);
	}

/*
It searches all the container & clears all the entries which
contain the PDUs of the message (uniquely identified by aStorageId).

NOTE:
	It keeps the record of removed entries (container id, pre-allocated storage id 
	& operation performed (add/delete)) in iReinstateEntryInfo variable.
	It will be required in case of revert operation. As the container which
	refers to the container which contains actual PDU is freed,
	so it will not be a problem to restore the removed PDUs.
	In case of revert operation, the removed information can be found in
	iReinstateEntryInfo variable which can be restored easily. 
	The container can be replaced with previous pre-allocated storage id.
	After commiting the transaction, this varaible will be re-initialized.

@param aStorageId unique id of message to be cleared.
@param aNumberOfPDUs number of PDUs to be cleared.

@internalComponent
*/
void CPreallocatedFile::ClearEntryL(TInt aStorageId, TInt aNumberOfPDUs)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_CLEARENTRYL_1, "CPreallocatedFile::ClearEntryL");

	//Read storage id.
	TInt storageId;
	TPtr8 memPtr((TUint8*) &storageId, sizeof(storageId), sizeof(storageId));
	TInt beginOfStorageIdSection = KBeginOfMasterHeaderSection + KSizeOfNumberOfEntrySection + iSizeOfEntrySection;
	TInt beginOfStorageId = 0;
	TInt count;
	TInt count2(0);

	TInt cleanStorageId=0;
	TPtr8 memPtr2((TUint8*) &cleanStorageId, sizeof(cleanStorageId), sizeof(cleanStorageId));

	for (count=0; count<iMaxPDUSeg+1; count++)
		{
		TReinstateEntryInfo entry;
		entry.iPreAllocatedStorageId=aStorageId;
		entry.iFlag=EEntryIsDeleted;

		beginOfStorageId = beginOfStorageIdSection + (count * sizeof(storageId));
		iFile.Read(beginOfStorageId, memPtr);
		if (storageId == aStorageId)
			{
			count2++;
			iFile.Write(beginOfStorageId, memPtr2);
			entry.iContainerId=count+1;
			iReinstateEntryInfo.AppendL(entry);
			}
		if (count2 >= aNumberOfPDUs)
			{
			break;
			}
		}
	iFile.Flush();
	}

/*
It updates the storage identifier section. It updates the corresponding container
in the storage identifier section specifying the PDU where it is stored.

NOTE:
	It keeps the record of added entries in iReinstateEntryInfo variable.
	It will be required in case of revert operation.
	After commiting the transaction, this varaible should be re-initialized.

@param aIndex index number of the container where PDU has been stored..
@param aStorageId unique id of added message.

@internalComponent
*/
void CPreallocatedFile::AddStorageIdL(TInt aIndex, TInt aStorageId)
	{
	//Put storage id in master header section
	TPtr8 memPtr((TUint8*) &aStorageId, sizeof(aStorageId), sizeof(aStorageId));
	TInt beginOfStorageIdentifierSection = KBeginOfMasterHeaderSection + KSizeOfNumberOfEntrySection + iSizeOfEntrySection;
	iFile.Write( beginOfStorageIdentifierSection + ((aIndex - 1) * sizeof(TInt)), memPtr);

	TReinstateEntryInfo entry;
	entry.iContainerId=aIndex;
	entry.iPreAllocatedStorageId=aStorageId;
	entry.iFlag=EEntryIsAdded;
	iReinstateEntryInfo.AppendL(entry);
	}

/*
It returns the next free available free container where next PDU can be stored.

It goes through storage identifier section, it reads the conents of all storage
identfier & return that container which is free.
All the free container is identifiable by value 0. If the value is not 0 then it
contains the storage id of a particular message.

@internalComponent
*/
TInt CPreallocatedFile::GetFreeContainer()
	{
	// Get the next free slot.
	TInt storageId;
	TPtr8 memPtr((TUint8*) &storageId, sizeof(storageId), sizeof(storageId));
	TInt beginOfStorageIdSection = KBeginOfMasterHeaderSection + KSizeOfNumberOfEntrySection + iSizeOfEntrySection;
	TInt beginOfStorageId = 0;
	TInt count;

	for (count=0; count<iMaxPDUSeg+1; count++)
		{
		beginOfStorageId = beginOfStorageIdSection + (count * sizeof(storageId));
		iFile.Read(beginOfStorageId, memPtr);
		if (storageId == 0)
			{
			break;
			}
		}

	// Count should never equal to iMaxPDUSeg+1
	__ASSERT_DEBUG(count < iMaxPDUSeg+1, SmspPanic(KSmspPanicPreallocatedFileNoFreeContainer));
	
	return (count + 1);
	}

/*
It returns the next free storage id.

Storage id is used to identify the messages stored in pre-allocated file.
A message might contain multiple PDUs and all those PDUs will be stored in 
different container. This storage id will be used to identify on which 
container the PDUs of a message are stored.

This logic is based on the max number of messages that can be stored in
pre-allocated file at one time. So free storage id is always used from
this pool of numbers (from 1 till (max calls 0 message+2)).

@internalComponent
*/
TInt CPreallocatedFile::GetFreeStorageId()
	{
	TInt count;

	for (count=1; count < iMaxClass0Msg + 2; count++)
		{
		TInt numberOfEntries = iEntryArray.Count();
		TInt count2;
		for (count2 = 0; count2 < numberOfEntries; count2++)
			{
			if (count == iEntryArray[count2].PreAllocatedStorageId())
				{
				break;
				}
			}
			if (count2 == numberOfEntries)
				{
				break;
				}
		}
		return count;
	}

/*
It externalizes(writes) the entry in permanent store file.

@param aContainerId container id where the sms pdu needs to externalized.
@param aSmsSlot reference to slot information.
@param aIndex index number of PDU.
@param aGsmSms refence to sms pdu.

@internalComponent
*/
void CPreallocatedFile::ExternalizeEntry(TInt aContainerId, const TGsmSmsSlotEntry& aSmsSlot, TInt aIndex, const TGsmSms& aGsmSms)
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_EXTERNALIZEENTRY_1, "CPreallocatedFile::ExternalizeEntry() 1: aContainerId=%d, aIndex=%d", aContainerId, aIndex);

	// Container id must not be greater than max pdu segment.
	TInt pos = iBeginOfDataSection + ((aContainerId - 1) * (KSizeOfGsmSmsSlotEntry + sizeof(TInt) + KSizeOfSmsGsmPDU));
	// Store slot info
	TPtr8 memPtr((TUint8*) &aSmsSlot, sizeof(aSmsSlot), sizeof(aSmsSlot));
	iFile.Write(pos, memPtr);
	// Store the PDU index value
	memPtr.Set((TUint8*) &aIndex, sizeof(aIndex), sizeof(aIndex));
	iFile.Write(pos + KSizeOfGsmSmsSlotEntry, memPtr);
	// Store the TGsmSms value
	RMobileSmsMessaging::TMobileSmsGsmTpdu pdu;
	pdu = aGsmSms.Pdu();
	memPtr.Set((TUint8*) &pdu, KSizeOfSmsGsmPDU, KSizeOfSmsGsmPDU);
	iFile.Write(pos + KSizeOfGsmSmsSlotEntry + sizeof(TInt), memPtr);

	__ASSERT_DEBUG(pos + KSizeOfGsmSmsSlotEntry + sizeof(TInt) + KSizeOfSmsGsmPDU <= iSizeOfFile,
			       SmspPanic(KSmspPanicPreallocatedFileCorrupt));
	}

/*
It externalizes(writes) the entry in permanent store file.

@param aContainerId container id where the sms pdu needs to externalized.
@param aIndex index number of PDU.
@param aGsmSms refence to sms pdu.

@internalComponent
*/
void CPreallocatedFile::ExternalizeEntry(TInt aContainerId, TInt aIndex, const TGsmSms& aGsmSms)
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_EXTERNALIZEENTRY1_1, "CPreallocatedFile::ExternalizeEntry() 2: aContainerId=%d, aIndex=%d", aContainerId, aIndex);

	// Container id must not be greater than max pdu segment.
	TInt pos = iBeginOfDataSection + ((aContainerId - 1) * (KSizeOfGsmSmsSlotEntry + sizeof(TInt) + KSizeOfSmsGsmPDU));
	// Store the PDU index value
	TPtr8 memPtr((TUint8*) &aIndex, sizeof(aIndex), sizeof(aIndex));
	iFile.Write(pos + KSizeOfGsmSmsSlotEntry, memPtr);
	// Store the TGsmSms value
	RMobileSmsMessaging::TMobileSmsGsmTpdu pdu;
	pdu = aGsmSms.Pdu();
	memPtr.Set((TUint8*) &pdu, KSizeOfSmsGsmPDU, KSizeOfSmsGsmPDU);
	iFile.Write(pos + KSizeOfGsmSmsSlotEntry + sizeof(TInt), memPtr);

	__ASSERT_DEBUG(pos + KSizeOfGsmSmsSlotEntry + sizeof(TInt) + KSizeOfSmsGsmPDU <= iSizeOfFile,
			       SmspPanic(KSmspPanicPreallocatedFileCorrupt));
	}

/*
It internalizes(reads) the entry from permanent store file.

@param aIndex index number of the message to be internalized.
@param aSmsMessage (output) reference to sms message.
@param aIndexArray (output) refence to array of index.
@param aSmsArray (output) refence to array of sms.

@internalComponent
*/
void CPreallocatedFile::InternalizeEntryL(const TInt aIndex, CSmsMessage& aSmsMessage, CArrayFix<TInt>& aIndexArray, CArrayFix<TGsmSms>& aSmsArray)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_INTERNALIZEENTRYL_1, "CPreallocatedFile::InternalizeEntryL");
	TSmsPreAllocatedFileStoreReassemblyEntry&  entry = iEntryArray[aIndex];
	//Set other properties of CSmsMessage
	aSmsMessage.SetStorage(entry.Storage());
	aSmsMessage.SetStatus((NMobileSmsStore::TMobileSmsStoreStatus)entry.Status());
	aSmsMessage.SetLogServerId(entry.LogServerId());
	aSmsMessage.SetTime(entry.Time());
	aSmsMessage.SetUTCOffset(entry.UTCOffset());
	aSmsMessage.SetDecodedOnSIM(entry.DecodedOnSIM());
	aSmsMessage.SetForwardToClient(entry.ForwardToClient());
	aSmsMessage.SetToFromAddressL(entry.Description2());

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_INTERNALIZEENTRYL_2, "CPreallocatedFile::InternalizeEntryL Start [sid=%d]", entry.PreAllocatedStorageId());
	if (entry.PreAllocatedStorageId()==KErrNotFound)
		{
		return;
		}

	TInt beginOfStorageIdSection = KBeginOfMasterHeaderSection + KSizeOfNumberOfEntrySection + iSizeOfEntrySection;
	//This will be used to read the storgae id which will indicate 
	//that corresponding container contains actual PDU
	TInt storageId=0;
	TPtr8 memPtr1((TUint8*) &storageId, sizeof(storageId), sizeof(storageId));

	TInt storageIdPos = 0;
	TInt dataPos = 0;
	//This will be used to read the slot information
	TGsmSmsSlotEntry smsSlotEntry;
	TPtr8 memPtr2((TUint8*) &smsSlotEntry, KSizeOfGsmSmsSlotEntry, KSizeOfGsmSmsSlotEntry);
	//This will be used to read the concatenated PDU index.
	TInt index=0;
	TPtr8 memPtr3((TUint8*) &index, sizeof(index), sizeof(index));
	//This will be used to read PDU.
	RMobileSmsMessaging::TMobileSmsGsmTpdu pdu;
	TPtr8 memPtr4((TUint8*) &pdu, KSizeOfSmsGsmPDU, KSizeOfSmsGsmPDU);
	TGsmSms sms;

	//Number of PDUs stored in this pre-allocated file
	TInt noOfPDUs = iEntryArray[aIndex].Count();
	//Will indicate how many number of PDUs already read from pre-allocated file.
	TInt noOfPDUsRead = 0;
	// Can also get the number of pdu stored in pre-allocated file from count's variable.
	for (TInt count=0; count<iMaxPDUSeg+1; count++)
		{
		storageIdPos = beginOfStorageIdSection + count * sizeof(TInt);
		iFile.Read(storageIdPos, memPtr1);
		if (storageId == entry.PreAllocatedStorageId())
			{//This means corresponding container contains the actual PDU
			noOfPDUsRead++;
			//Find the position of container where actaul PDU is stored.
			dataPos = iBeginOfDataSection + ((count) * (KSizeOfGsmSmsSlotEntry + sizeof(TInt) + KSizeOfSmsGsmPDU));
			//Read slot information
			if (entry.Storage() == CSmsMessage::ESmsSIMStorage  ||
				entry.Storage() == CSmsMessage::ESmsCombinedStorage)
				{
				iFile.Read(dataPos + KSizeOfGsmSmsSlotEntry, memPtr2);
				aSmsMessage.AddSlotL(smsSlotEntry);
				}
			//Read index information
			iFile.Read(dataPos + KSizeOfGsmSmsSlotEntry, memPtr3);
			aIndexArray.AppendL(index);
			//Read PDU
			iFile.Read(dataPos + KSizeOfGsmSmsSlotEntry + sizeof(index), memPtr4);
			sms.SetPdu(pdu);
			aSmsArray.AppendL(sms);
			if (noOfPDUsRead == noOfPDUs)
				{
				break;
				}
			}
		}

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_INTERNALIZEENTRYL_3, "CPreallocatedFile::InternalizeEntryL End [noOfPDUsRead=%d]", noOfPDUsRead);
	}

/*
It removes the PDUs from pre-allocated store file.
This function is needed because after forwarding the incomplete message
to client, the corresponding PDUs needs to be be removed from permanent
store file.
This functionality is specific to class 0 re-assembly store.

@param aIndex index number of the message to be removed.
@param aStartPos starting pos of pdu to be removed.
@param aEndPos end pos of pdu to be removed.

@internalComponent
*/
void CPreallocatedFile::RemovePDUsL(TInt aIndex, TInt aStartPos, TInt aEndPos)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_REMOVEPDUSL_1, "CPreallocatedFile::RemovePDUsL");

	if ((aStartPos < 1) || (aEndPos > 256) || (aStartPos > aEndPos))
		{
		User::Leave(KErrArgument);
		}

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_REMOVEPDUSL_2, "CPreallocatedFile::RemovePDUsL Start [sid=%d]", iEntryArray[aIndex].PreAllocatedStorageId());
	if (iEntryArray[aIndex].PreAllocatedStorageId()==KErrNotFound)
		{
		return;
		}

	TInt beginOfStorageIdSection = KBeginOfMasterHeaderSection + KSizeOfNumberOfEntrySection + iSizeOfEntrySection;
	//This will be used to read the storgae id which will indicate 
	//that corresponding container contains actual PDU
	TInt storageId=0;
	TPtr8 memPtr1((TUint8*) &storageId, sizeof(storageId), sizeof(storageId));

	//This will be used to cleanup the storage id.
	TInt cleanStorageId=0;
	TPtr8 memPtr2((TUint8*) &cleanStorageId, sizeof(cleanStorageId), sizeof(cleanStorageId));

	//This will be used to read the concatenated PDU index.
	TInt index=0;
	TPtr8 memPtr3((TUint8*) &index, sizeof(index), sizeof(index));

	//Number of PDUs stored in this pre-allocated file
	TInt noOfPDUs = iEntryArray[aIndex].Count();
	//Will indicate how many number of PDUs already read from pre-allocated file.
	TInt noOfPDUsRead = 0;
	// Can also get the number of pdu stored in pre-allocated file from count's variable.
	TInt noOfEntriesRemoved=0;
	for (TInt count=0; count<iMaxPDUSeg+1; count++)
		{
		TInt storageIdPos = beginOfStorageIdSection + count * sizeof(TInt);
		iFile.Read(storageIdPos, memPtr1);
		if (storageId == iEntryArray[aIndex].PreAllocatedStorageId())
			{//This means corresponding container contains the actual PDU
			noOfPDUsRead++;
			//Find the position of container where actaul PDU is stored.
			TInt dataPos = iBeginOfDataSection + ((count) * (KSizeOfGsmSmsSlotEntry + sizeof(TInt) + KSizeOfSmsGsmPDU));
			//Read index information
			iFile.Read(dataPos + KSizeOfGsmSmsSlotEntry, memPtr3);

			if ((index >= aStartPos) && (index <= aEndPos))
				{
				noOfEntriesRemoved++;
				//Remove the PDU entry.
				iFile.Write(storageIdPos, memPtr2);
				}

			if (noOfPDUsRead == noOfPDUs)
				{
				break;
				}
			}
		}

	/*
	There are 3 scenarios in this case:
	1. If all the entries need to be removed,
	then there is no need to store the entry in this permanent file.
	2. If few entries need to be removed,
	then externalize the remaining entries. Update count field also.
	3. If no entries need to be removed, then do nothing.
	*/
	TSmsPreAllocatedFileStoreReassemblyEntry entry;
	entry = iEntryArray[aIndex];

	if (entry.Count() == noOfEntriesRemoved)
		{
		entry.SetPreAllocatedStorageId(KErrNotFound);
		entry.SetCount(0);
		ChangeEntryL(aIndex, entry);
		}
	else if (noOfEntriesRemoved > 0)
		{
		entry.SetCount(entry.Count() - noOfEntriesRemoved);
		ChangeEntryL(aIndex, entry);
		}
	}

/*
It stores the forwarded message info in forwarded pdu bit-map.

@param aIndex index number of the forwarded message.
@param aStartPos starting pos of forwarded pdu.
@param aEndPos end pos of forwarded pdu.

@internalComponent
*/
void CPreallocatedFile::StoreForwardedPDUsInfoL(TInt aIndex, TInt aStartPos, TInt aEndPos)
	{
	TSmsPreAllocatedFileStoreReassemblyEntry entry;
	entry = iEntryArray[aIndex];

	TUint8 startBitMapIndex = (aStartPos-1)/8;
	TUint8 endBitMapIndex = (aEndPos-1)/8;
	TUint8 startBitPos = (aStartPos-1)%8;
	TUint8 endBitPos = (aEndPos-1)%8;

	if (startBitMapIndex == endBitMapIndex)
		{
		TUint8 bitMap;
		entry.GetBitMap(startBitMapIndex, bitMap);
		TUint8 tmpBitMap = (0 | 0xFF);
		tmpBitMap <<= ((8 - endBitPos) - 1);
		tmpBitMap >>= (8 - ((endBitPos - startBitPos)+1));
		tmpBitMap <<= startBitPos;
		bitMap |= tmpBitMap;
		entry.SetBitMap(startBitMapIndex, bitMap);
		}
	else
		{
		TUint8 bitMap;
		entry.GetBitMap(startBitMapIndex, bitMap);
		TUint8 tmpBitMap = (0 | 0xFF);
		tmpBitMap >>= (8 - ((7 - startBitPos)+1));
		tmpBitMap <<= startBitPos;
		bitMap |= tmpBitMap;
		entry.SetBitMap(startBitMapIndex, bitMap);

		entry.GetBitMap(endBitMapIndex, bitMap);
		tmpBitMap = (0 | 0xFF);
		tmpBitMap <<= ((8 - endBitPos) - 1);
		tmpBitMap >>= (8 - ((endBitPos - 0)+1));
		tmpBitMap <<= 0;
		bitMap |= tmpBitMap;
		entry.SetBitMap(endBitMapIndex, bitMap);
		}

	TInt noOfForwardedPDUs = (aEndPos - aStartPos) + 1;
	entry.SetNumberOfPDUsForwardToClient(entry.NumberOfPDUsForwardToClient()+noOfForwardedPDUs);

	ChangeEntryL(aIndex, entry);
	}

/*
It checks the validity of master header section by comparing checksum value.
If the checksum value does not match, then it copies data from duplicate header section to
master header section.
*/
void CPreallocatedFile::CheckDataL()
	{
	TUint32 checksum;
	TInt checksumPos = KBeginOfMasterHeaderSection + KSizeOfNumberOfEntrySection + iSizeOfEntrySection + iSizeOfStorageIdentifierSection;

	TPtr8 memPtr((TUint8*) &checksum, sizeof(checksum), sizeof(checksum));
	iFile.Read(checksumPos, memPtr);

	if (ChecksumValue() != checksum)
		{
		CopyDuplicateToMasterL();
		}
	}

/*
It writes the checksum value & then copies header information from master section to duplicate section.
*/
void CPreallocatedFile::PutChecksumValueL()
	{
	TUint32 checksum = ChecksumValue();
	TInt checksumPos = KBeginOfMasterHeaderSection + KSizeOfNumberOfEntrySection + iSizeOfEntrySection + iSizeOfStorageIdentifierSection;

	TPtr8 memPtr((TUint8*) &checksum, sizeof(checksum), sizeof(checksum));
	iFile.Write(checksumPos, memPtr);
	iFile.Flush();
	CopyMasterToDuplicateL();
	}

/*
It calculates & returns the checksum value of master header section.
*/
TUint32 CPreallocatedFile::ChecksumValue()
	{
	TInt sizeOfDataToBeChecksum = KSizeOfNumberOfEntrySection + iSizeOfEntrySection + iSizeOfStorageIdentifierSection;

	TUint32 checksum(0);

	TUint32 tmpValue;
	TPtr8 memPtr((TUint8*) &tmpValue, sizeof(tmpValue), sizeof(tmpValue));

	for (TInt i = 0; i < sizeOfDataToBeChecksum/4 ; i++)
		{
		iFile.Read(KBeginOfMasterHeaderSection + (i*4), memPtr);
		checksum += tmpValue;
		}
	return 	checksum;
	}

/*
It copies header information from master copy to duplicate copy.
*/
void CPreallocatedFile::CopyMasterToDuplicateL()
	{
	TInt sizeOfDataToBeCopied = KSizeOfNumberOfEntrySection + iSizeOfEntrySection + iSizeOfStorageIdentifierSection + 4;

	HBufC8* heapBuffer = HBufC8::NewL(sizeOfDataToBeCopied);
	CleanupStack::PushL(heapBuffer);
	TPtr8 memPtr(heapBuffer->Des());
	iFile.Read(KBeginOfMasterHeaderSection, memPtr);
	iFile.Write(iBeginOfDuplicateHeaderSection, memPtr);
	iFile.Flush();
	CleanupStack::PopAndDestroy(heapBuffer);
	}

/*
It copies header information from duplicate copy to master copy.
*/
void CPreallocatedFile::CopyDuplicateToMasterL()
	{
	TInt sizeOfDataToBeCopied = KSizeOfNumberOfEntrySection + iSizeOfEntrySection + iSizeOfStorageIdentifierSection + 4;

	HBufC8* heapBuffer = HBufC8::NewL(sizeOfDataToBeCopied);
	CleanupStack::PushL(heapBuffer);
	TPtr8 memPtr(heapBuffer->Des());
	iFile.Read(iBeginOfDuplicateHeaderSection, memPtr);
	iFile.Write(KBeginOfMasterHeaderSection, memPtr);
	iFile.Flush();
	CleanupStack::PopAndDestroy(heapBuffer);
	}

/*
It returns the total number of PDUs stored in pre-allocated file.
*/
TInt CPreallocatedFile::NumberOfPDUStored()
	{
	TInt noOfPDUs = 0;
	for (TInt i = 0; i < iEntryArray.Count(); i++)
		{
		noOfPDUs += iEntryArray[i].Count();
		}
	return noOfPDUs;
	}

/**
 *  Sets the pre-allocated file store as in-transaction.
 *  
 *  The function checks the validity of the call and leaves KErrAccessDenied if
 *  invalid.
 *  @capability None
 */
void CPreallocatedFile::BeginTransactionL()
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_BEGINTRANSACTIONL_1, "CPreallocatedFile::BeginTransactionL [this=0x%08X iInTransaction=%d]", (TUint)this, iInTransaction);

	if (iInTransaction)
		{
	    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_BEGINTRANSACTIONL_2, "WARNING CPreallocatedFile::BeginTransactionL leaving with KErrAccessDenied");
		User::Leave(KErrAccessDenied);
		}

	iInTransaction = ETrue;
	}

/**
 *  It commits the transaction.
 */
void CPreallocatedFile::CommitTransactionL()
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_COMMITTRANSACTIONL_1, "CPreallocatedFile::CommitTransactionL(): this=0x%08X iInTransaction=%d",(TUint)this, iInTransaction);

	ExternalizeEntryArray();
	//Commit
	PutChecksumValueL();
	iInTransaction = EFalse;
	iReinstateEntryInfo.Reset();
	RemoveDeletedEntries();
	}

/*
It reverts the transaction.
*/
void CPreallocatedFile::Revert()
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_REVERT_1, "CPreallocatedFile::Revert(): this=0x%08X, iInTransaction=%d",(TUint)this, iInTransaction);

	ReinstateEntries();
	ExternalizeEntryArray();
	iInTransaction = EFalse;
	ReinstateDeletedEntries();
	}

/**
 *  It removes the deleted entries from entry arry.
 *	This function is called after commit.
 */
void CPreallocatedFile::RemoveDeletedEntries()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_REMOVEDELETEDENTRIES_1, "CPreallocatedFile::RemoveDeletedEntries()");

	TInt count=iEntryArray.Count();
	while (count--)
		{
		TSmsPreAllocatedFileStoreReassemblyEntry& entry = iEntryArray[count];

		if (entry.IsDeleted())
			{
			iEntryArray.Delete(count);
			}
		else
			{
			entry.SetIsAdded(EFalse);
			}
		}
	} // CPreallocatedFile::RemoveDeletedEntries

/**
 *  It reinstate the deleted/added entries from entry arry.
 *	This function is called after revert operation.
 */
void CPreallocatedFile::ReinstateDeletedEntries()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_REINSTATEDELETEDENTRIES_1, "CPreallocatedFile::ReinstateDeletedEntries()");

	TInt count=iEntryArray.Count();
	while (count--)
		{
		TSmsPreAllocatedFileStoreReassemblyEntry& entry = iEntryArray[count];

		if (entry.IsAdded())
			{
			iEntryArray.Delete(count);
			}
		else
			{
			entry.SetIsDeleted(EFalse);
			}
		}
	} // CPreallocatedFile::ReinstateDeletedEntries

/**
 *  It reinstate the deleted/added entries from the container.
 *	This function is called at the time of revert operation.
 *	Unlike permanent store file which supports revert. pre-allocated
 *	file (raw data file) supports the revert mecahnism using this function.
 */
void CPreallocatedFile::ReinstateEntries()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_REINSTATEENTRIES_1, "CPreallocatedFile::ReinstateEntries()");

	TInt containerId;
	TInt storageId;
	TPtr8 memPtr((TUint8*) &storageId, sizeof(storageId), sizeof(storageId));

	TInt count = iReinstateEntryInfo.Count();

	for (TInt i=0; i<count; i++)
		{
		if (iReinstateEntryInfo[i].iFlag == EEntryIsDeleted)
			{
			//Add the Pre-allocated storage info in the container.
			storageId = iReinstateEntryInfo[i].iPreAllocatedStorageId;
			containerId = iReinstateEntryInfo[i].iContainerId;
			TInt beginOfStorageIdentifierSection = KBeginOfMasterHeaderSection + KSizeOfNumberOfEntrySection + iSizeOfEntrySection;
			iFile.Write( beginOfStorageIdentifierSection + ((containerId - 1) * sizeof(TInt)), memPtr);
			}
		else	//EEntryIsAdded
			{
			//Delete the Pre-allocated storage info from the container.
			storageId = 0;
			containerId = iReinstateEntryInfo[i].iContainerId;
			TInt beginOfStorageIdentifierSection = KBeginOfMasterHeaderSection + KSizeOfNumberOfEntrySection + iSizeOfEntrySection;
			iFile.Write( beginOfStorageIdentifierSection + ((containerId - 1) * sizeof(TInt)), memPtr);
			}
		}
	iFile.Flush();
	iReinstateEntryInfo.Reset();
	}

/*
It returns the index of oldest message in pre-allocated file.
*/
TInt CPreallocatedFile::GetOldestMessageEntryIndex()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPREALLOCATEDFILE_GETOLDESTMESSAGEENTRYINDEX_1, "CPreallocatedFile::GetOldestMessageEntryIndex()");

	TInt index = KErrNotFound;
	TTime time;
	time.UniversalTime();

	for (TInt index2=0; index2 < iEntryArray.Count(); index2++)
		{
		if ((iEntryArray[index2].Time() < time) && (iEntryArray[index2].Count()>0))
			{
			time = iEntryArray[index2].Time();
			index = index2;
			}
		}
	return index;
	}

/**
Static factory constructor. Uses two phase 
construction and leaves nothing on the CleanupStack.

@param aClass0ReassemblyStore reference to class 0 reasembly store.
@param aGuardTimeout guard time out in hours.

@return A pointer to the newly created CGuardTimer object.

@internalComponent
*/
CGuardTimer* CGuardTimer::NewL(CClass0SmsReassemblyStore& aClass0ReassemblyStore, TInt aGuardTimeout)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CGUARDTIMER_NEWL_1, "CGuardTimer::NewL()");

	CGuardTimer* timer = new(ELeave) CGuardTimer(aClass0ReassemblyStore, aGuardTimeout);
	CleanupStack::PushL(timer);
	timer->ConstructL();
	CleanupStack::Pop();
	return timer;
	} // CGuardTimer::NewL

/**
 *  Constructor
 */
CGuardTimer::CGuardTimer(CClass0SmsReassemblyStore& aClass0ReassemblyStore, TInt aGuardTimeout)
	:CTimer(KSmsSessionPriority),
	iClass0ReassemblyStore(aClass0ReassemblyStore),
	iTimeoutInSecs(aGuardTimeout*3600)
	{
	CActiveScheduler::Add(this);
	} //CGuardTimer::CGuardTimer

/**
 *  Destructor
 */
CGuardTimer::~CGuardTimer()
	{
	Cancel();
	} // CGuardTimer::~CGuardTimer

/**
 *  Enable the Gurad Timer
 */
void CGuardTimer::EnableGuardTimer()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CGUARDTIMER_ENABLEGUARDTIMER_1, "CGuardTimer::EnableGuardTimer()");
	if (!IsActive())
		{
		TTime nextTimeOut;
		iClass0ReassemblyStore.GetNextGuardTimeout(nextTimeOut);
		At(nextTimeOut);
		}
	} //CGuardTimer::EnableGuardTimer

/**
 *  Disable the Gurad Timer
 */
void CGuardTimer::DisableGuardTimer()
	{
	Cancel();
	}

/**
 *  Timer completed
 */
void CGuardTimer::RunL()
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CGUARDTIMER_RUNL_1, "CGuardTimer::RunL [iStatus=%d]", iStatus.Int());
	iClass0ReassemblyStore.ProcessTimeoutMessageL();
	EnableGuardTimer();
	} // CGuardTimer::RunL

/**
Static factory constructor. Uses two phase 
construction and leaves nothing on the CleanupStack.

@param aFs  File Server handle.
@param aSmsComm reference to MSmsComm event handler.

@return A pointer to the newly created CClass0SmsReassemblyStore object.

@pre A connected file server session must be passed as parameter.
@post CClass0SmsReassemblyStore object is now fully initialised

@internalComponent
*/
CClass0SmsReassemblyStore* CClass0SmsReassemblyStore::NewL(RFs& aFs, MSmsComm& aSmsComm)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_NEWL_1, "CClass0SmsReassemblyStore::NewL()");

	CClass0SmsReassemblyStore*  self = new (ELeave) CClass0SmsReassemblyStore(aFs, aSmsComm);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
	}

/**
 *  C'tor
 */
CClass0SmsReassemblyStore::CClass0SmsReassemblyStore(RFs& aFs, MSmsComm& aSmsComm)
	:CReassemblyStore(aFs), iSmsComm(aSmsComm), iPermanentFileStore(NULL), iPreallocatedFile(NULL),
	iGuardTimer(NULL), iInTransaction(EFalse), iDiskSpaceStatus(ESmsDiskSpaceAvailable)
	{
	}

/**
 *  D'tor
 */
CClass0SmsReassemblyStore::~CClass0SmsReassemblyStore()
	{
	delete iPermanentFileStore;
	delete iPreallocatedFile;
	delete iGuardTimer;
	}

/*
It constructs permanent and pre-allocated file to store the class 0 messages.
It creates a guard timer so that messages can be deleted if all the constituent
PDU of a message is not received within configured time-frame.
It also reserves 4KB disk space so that reserved space can be used to 
delete a stream from permanent file store in out-of-disk condition.

@internalComponent
*/
void CClass0SmsReassemblyStore::ConstructL()
	{
	//Reserve Drive Space
	User::LeaveIfError(iFs.ReserveDriveSpace(KStoreDrive, 4*1024));

	//Read Configuration file
	ReadConfigurableClass0SmsSettingsL(iMaxClass0Msg, iMaxPDUSeg, iGuardTimeOut);
	//Create Permanent store file
	TFileName permanentStoreFileName;
	CReassemblyStoreUtility::PrivatePath(iFs, permanentStoreFileName);
	permanentStoreFileName.Append(KClass0ReassemblyStoreName);
	iPermanentFileStore = CSmsPermanentFileStore::NewL(iFs, permanentStoreFileName, KClass0ReassemblyStoreUid);
	//Create Pre-allocated file
	TFileName preAllocatedFileName;
	CReassemblyStoreUtility::PrivatePath(iFs, preAllocatedFileName);
	preAllocatedFileName.Append(KPreAllocatedFileName);
	iPreallocatedFile = CPreallocatedFile::NewL(iFs, preAllocatedFileName, iMaxClass0Msg, iMaxPDUSeg);
	//Create Gurad Timer
	iGuardTimer = CGuardTimer::NewL(*this, iGuardTimeOut);
	}

/*
It reads class 0 re-assembly store configuration information from smswap.sms.esk file. 
It reads the following information: MaxClass0Messages, NumberOfPDUSegements, GuardTimeOut.
If any of the above element is missing it takes the default value.

@internalComponent
*/
void CClass0SmsReassemblyStore::ReadConfigurableClass0SmsSettingsL(TInt& aMaxClass0Msg, TInt& aMaxPDUSeg, TInt& aGuardTimeOut)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_READCONFIGURABLECLASS0SMSSETTINGSL_1, "CClass0SmsReassemblyStore::ReadConfigurableClass0SmsSettingsL()");

	aMaxClass0Msg = KMaxNumberOfClass0MessagesInReassemblyStore;
	aMaxPDUSeg    = KNumberOfPDUSegmentsStoredInOODCondition;
	aGuardTimeOut = KGuardTimeOut;

	CESockIniData*  ini = NULL;
	TRAPD(ret, ini=CESockIniData::NewL(_L("smswap.sms.esk")));
	if(ret!=KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_READCONFIGURABLECLASS0SMSSETTINGSL_2, "CESockIniData::NewL() returned=%d", ret);
		}
	else
		{
		CleanupStack::PushL(ini);

		TInt var(0);
		if(ini->FindVar(_L("ReassemblyStore"),_L("MaxClass0Messages"),var))
			{
			if (var > 0)
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_READCONFIGURABLECLASS0SMSSETTINGSL_3, "MaxClass0Messages [%d]", var);
				aMaxClass0Msg = var;
				}
			}

		if(ini->FindVar(_L("ReassemblyStore"),_L("NumberOfPDUSegements"),var))
			{
			if (var > 0)
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_READCONFIGURABLECLASS0SMSSETTINGSL_4, "MaxClass0Messages [%d]", var);
				aMaxPDUSeg = var;
				}
			}

		if(ini->FindVar(_L("ReassemblyStore"),_L("GuardTimeOut"),var))
			{
			if (var > 0)
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_READCONFIGURABLECLASS0SMSSETTINGSL_5, "MaxClass0Messages [%d]", var);
				aGuardTimeOut = var;
				}
			}

		CleanupStack::PopAndDestroy(ini);
		}

	OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_READCONFIGURABLECLASS0SMSSETTINGSL_6, "CClass0SmsReassemblyStore::ReadConfigurableClass0SmsSettingsL(): aMaxClass0Msg=%d, aMaxPDUSeg=%d, aGuardTimeOut=%d",aMaxClass0Msg, aMaxPDUSeg, aGuardTimeOut);
	}

/**
It opens the class 0 re-assembly store.
Then it populates the entry information (all the messages stored in re-assembly store).

@internalComponent
*/
void CClass0SmsReassemblyStore::OpenStoreL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_OPENSTOREL_1, "CClass0SmsReassemblyStore::OpenStoreL()");
	TFileName pathName;
	CReassemblyStoreUtility::PrivatePath(iFs, pathName);
	//Create the directory if it is not created.
	iFs.MkDirAll(pathName);
	// If any one file becomes corrupt or does not exist then we have to create both files.
	if (iPreallocatedFile->IsFileOK() && iPermanentFileStore->IsFileOK())
		{
		iPreallocatedFile->OpenL();
		iPermanentFileStore->OpenL();
		}
	else
		{
		iPreallocatedFile->CreateL();
		iPermanentFileStore->CreateL();
		}
	PopulateEntryArrayL(iEntryArray);
	iGuardTimer->EnableGuardTimer();
	}

/**
It closes the class 0 re-assembly store.

@internalComponent
*/
void CClass0SmsReassemblyStore::Close()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_CLOSE_1, "CClass0SmsReassemblyStore::CloseStore()");
	iGuardTimer->DisableGuardTimer();
	iEntryArray.Reset();
	iPreallocatedFile->Close();
	iPermanentFileStore->Close();
	}

/**
It populates the entry information stored in class 0 reassembly store.

@param aEntryArray reference to entry array.

@internalComponent
*/
void CClass0SmsReassemblyStore::PopulateEntryArrayL(CArrayFix<TReassemblyEntry>& aEntryArray)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_POPULATEENTRYARRAYL_1, "CClass0SmsReassemblyStore::PopulateEntryArrayL()");
	aEntryArray.Reset();
	//Populate Entries from Pre-allocated file.
	for (TInt count = 0; count < iPreallocatedFile->Entries().Count(); count++)
		{
		TReassemblyEntry entry;
		entry.SetReference(iPreallocatedFile->Entries()[count].Reference());
		entry.SetTotal(iPreallocatedFile->Entries()[count].Total());
		entry.SetCount(iPreallocatedFile->Entries()[count].Count()+iPreallocatedFile->Entries()[count].NumberOfPDUsForwardToClient());
		entry.SetLogServerId(iPreallocatedFile->Entries()[count].LogServerId());
 		
 		// Check that descriptor2 (sms address )is not corrupted
 		TPtrC ptr = iPreallocatedFile->Entries()[count].Description2();
		
		if (ptr.Length() <= CSmsAddress::KSmsAddressMaxAddressLength)
			{
 			entry.SetDescription2(iPreallocatedFile->Entries()[count].Description2());
 			}
	
		entry.SetPduType(iPreallocatedFile->Entries()[count].PduType());
		entry.SetStorage(iPreallocatedFile->Entries()[count].Storage());
		entry.SetTime(iPreallocatedFile->Entries()[count].Time());
		entry.SetPassedToClient(iPreallocatedFile->Entries()[count].PassedToClient());
		aEntryArray.AppendL(entry);
		}

	/*
	Then populate the entries from permanent store file. It is needed
	because permanent store file might contain few mesages.
	But before populating the entry information it is needed to cleanup 
	the entries in permanent store file. It is needed to ensure that permanent file
	clean itself with pre-allocated file. There migth be a scenario where 
	user has deleted a message but the corresponding message has not been 
	deleted from permanent store file due to out-of-disk condition. 
	And also at the time of forwarding an incomplete message a forwarded 
	message has not been deleted due to above reason. But the entry/PDU is 
	invalid because it is no more in the pre-allocated file which 
	contains master header info.
	*/
	TInt ret = CleanReassemblyEntries();

	if (ret == KErrNone)
		{
		/*
		In this case permanent store file contains correct information.
		So populate Entry information from Permanent store file.
		In this case only count information needs to be updated.
		*/
		TInt permanentFileStoreIndex;
		for (TInt i=0; i<aEntryArray.Count(); i++)
			{
			permanentFileStoreIndex=KErrNotFound;
			iPermanentFileStore->MatchEntryToExistingMessage(aEntryArray[i], permanentFileStoreIndex);
			if (permanentFileStoreIndex!=KErrNotFound)
				{
				aEntryArray[i].SetCount(aEntryArray[i].Count() + iPermanentFileStore->Entries()[permanentFileStoreIndex].Count());
				if (aEntryArray[i].Count() > aEntryArray[i].Total())
					{
					aEntryArray[i].SetCount(aEntryArray[i].Total());
					}
				}
			}
		}
	else if (ret == KErrDiskFull)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_POPULATEENTRYARRAYL_2, "CleanReassemblyEntries() returns KErrDiskFull");
		/*
		In this case permanent store file contains incorrect information.
		For example forwarded message might be still stored in this store.
		Because it has not been deleted due to out-of-disk condition.
		So be careful at the time of updating the count information
		about the message.
		*/
		TInt permanentFileStoreIndex;
		for (TInt i=0; i<aEntryArray.Count(); i++)
			{
			permanentFileStoreIndex=KErrNotFound;
			iPermanentFileStore->MatchEntryToExistingMessage(aEntryArray[i], permanentFileStoreIndex);
			if (permanentFileStoreIndex!=KErrNotFound)
				{
				TSmsPreAllocatedFileStoreReassemblyEntry entry = iPreallocatedFile->Entries()[i];
				if (entry.NumberOfPDUsForwardToClient() > 0)
					{
					//Internalize the entries.
					CSmsBuffer*  buffer = CSmsBuffer::NewL();
					CSmsMessage*  smsMessage = CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver, buffer);
					CleanupStack::PushL(smsMessage);

					CArrayFix<TInt>*  indexArray = new(ELeave) CArrayFixFlat<TInt>(KFlatArrayGranularity);
					CleanupStack::PushL(indexArray);
					CArrayFixFlat<TGsmSms>*  smsArray = new(ELeave) CArrayFixFlat<TGsmSms>(KFlatArrayGranularity);
					CleanupStack::PushL(smsArray);
					//Internalize to check whether removed PDUs still stored in Permanent store file.
					iPermanentFileStore->InternalizeEntryL(permanentFileStoreIndex, *smsMessage, *indexArray, *smsArray);
					TInt noOfForwardedEntries=0;
					for (TInt j=0; j<indexArray->Count() ;j++)
						{
						TUint8 bitMapIndex = (indexArray->At(j)-1)/8;
						TUint8 bitPos = (indexArray->At(j)-1)%8;
						TUint8 bitMap;
						entry.GetBitMap(bitMapIndex, bitMap);
						TUint8 tmpBitMap = 1;
						tmpBitMap <<= bitPos;
						if (tmpBitMap == (bitMap & tmpBitMap))
							{
							noOfForwardedEntries++;
							}
						}
					TInt count = iPermanentFileStore->Entries()[permanentFileStoreIndex].Count() - noOfForwardedEntries;
					aEntryArray[i].SetCount(aEntryArray[i].Count() + count);

					CleanupStack::PopAndDestroy(3, smsMessage); // smsMessage, indexArray, smsArray
					}
				else
					{
					aEntryArray[i].SetCount(aEntryArray[i].Count() + iPermanentFileStore->Entries()[permanentFileStoreIndex].Count());
					}
				}
			}
		}
	else
		{
		User::Leave(ret);
		}
	}

/**
It sets the disk space status.
If disk space is full, then class 0 re-assembly store stores the incoming message in
pre-allocated file. Otherwise it stores the message in permanent store file.
*/
void CClass0SmsReassemblyStore::SetDiskSpaceState(TSmsDiskSpaceMonitorStatus aDiskSpaceStatus)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_SETDISKSPACESTATE_1, "CClass0SmsReassemblyStore::SetDiskSpaceState()");
	iDiskSpaceStatus = aDiskSpaceStatus;
	}

/*
It adds the new message in class 0 reassembly store.
It first tries to add the message in permanent store file. If it is
successful, then it adds the entry information in pre-allocated file.
Because pre-allocated file contains master header entry information.
Otherwise (if disk space is full), it adds the message in pre-allocated file.

@param aSmsMessage reference to sms message to be added.
@param aGsmSms reference to GsmSms object to be added.

@internalComponent
*/
void CClass0SmsReassemblyStore::AddNewMessageL(CSmsMessage& aSmsMessage,const TGsmSms& aGsmSms)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_ADDNEWMESSAGEL_1, "CClass0SmsReassemblyStore::AddNewMessageL");

	// Add entry in permanent store file
	TInt index;
	TInt ret = KErrNone;

	if (iDiskSpaceStatus == ESmsDiskSpaceFull)
		{
		ret = KErrDiskFull;
		}
	else
		{
		TRAP(ret, iPermanentFileStore->AddNewMessageL(index, aSmsMessage, aGsmSms));
		}

	if (ret == KErrNone)
		{
		// Add a new entry in pre-allocated file
		TSmsPreAllocatedFileStoreReassemblyEntry tmpEntry;
		//Fill-up entry information...
		CReassemblyStoreUtility::PopulateEntry(tmpEntry, aSmsMessage, 1);

		//The properties below need to be set only once when a PDU of a new message arrives.
		tmpEntry.SetStatus((RMobileSmsStore::TMobileSmsStoreStatus)aSmsMessage.Status());
		tmpEntry.SetUTCOffset(aSmsMessage.UTCOffset());
		tmpEntry.SetDecodedOnSIM(aSmsMessage.DecodedOnSim());
		tmpEntry.SetForwardToClient(aSmsMessage.ForwardToClient());
		//Set KErrNotFound because message corresponding to 
		//this entry is not in this pr-allocated file.
		tmpEntry.SetPreAllocatedStorageId(KErrNotFound);
		//Has to set 0 because PDU is stored in Permanent store file instead of pre-allocated file.
		tmpEntry.SetCount(0);

		iPreallocatedFile->AddEntryL(tmpEntry);
		}
	else if (ret == KErrDiskFull)
		{
		// Add the new message in pre-allocated file
		iPreallocatedFile->AddNewMessageL(index, aSmsMessage, aGsmSms);
		}
	else
		{
		User::Leave(ret);
		}
	}

/*
It updates the existing message in class 0 re-assembly store.

@param aSmsMessage reference to sms message to be updated.
@param aGsmSms reference to GsmSms object to be added.
@param aDuplicateMsgRef boolean value (output) indicates whether the added PDU is a duplicate or not.
@param aDuplicateSlot boolean value (output) indicates whether the added PDU is a duplicate enumerated PDU.

@internalComponent
*/
void CClass0SmsReassemblyStore::UpdateExistingMessageL(CSmsMessage& aSmsMessage, const TGsmSms& aGsmSms, TBool& aDuplicateMsgRef, TBool& aDuplicateSlot)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_UPDATEEXISTINGMESSAGEL_1, "CClass0SmsReassemblyStore::UpdateExistingMessageL()");

	aDuplicateMsgRef = EFalse;
	aDuplicateSlot   = EFalse;

	CSmsBuffer*  buffer = CSmsBuffer::NewL();
	CSmsMessage*  smsMessage = CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver, buffer);
	CleanupStack::PushL(smsMessage);

	CArrayFix<TInt>*  indexArray = new(ELeave) CArrayFixFlat<TInt>(KFlatArrayGranularity);
	CleanupStack::PushL(indexArray);
	CArrayFixFlat<TGsmSms>*  smsArray = new(ELeave) CArrayFixFlat<TGsmSms>(KFlatArrayGranularity);
	CleanupStack::PushL(smsArray);

	//Internalize stored message in permanent store file
	TReassemblyEntry entry;
	CReassemblyStoreUtility::PopulateEntry(entry, aSmsMessage, 1);
	TInt permanentStoreIndex;
	iPermanentFileStore->MatchEntryToExistingMessage(entry, permanentStoreIndex);
	if (permanentStoreIndex!=KErrNotFound)
		{
		iPermanentFileStore->InternalizeEntryL(permanentStoreIndex, *smsMessage, *indexArray, *smsArray);
		}

	//Internalize stored message in pre-allocated file
	TInt preAllocatedFileIndex;
	iPreallocatedFile->MatchEntryToExistingMessage(entry, preAllocatedFileIndex);
	if (preAllocatedFileIndex!=KErrNotFound)
		{
		iPreallocatedFile->InternalizeEntryL(preAllocatedFileIndex, *smsMessage, *indexArray, *smsArray);
		}
	else
		{
		//This condition should never arise
		User::Leave(KErrNotFound);
		}

	//
	// Check if this is a duplicated enumerated PDU (e.g. on the SIM or phone memory)
	// or a duplicated PDU (e.g. in the Reassembly Store)...
	//
	TInt  concatPDUIndex = aSmsMessage.SmsPDU().ConcatenatedMessagePDUIndex();

	if (smsMessage->Storage() == CSmsMessage::ESmsSIMStorage  ||
		smsMessage->Storage() == CSmsMessage::ESmsCombinedStorage)
		{
		//
		// In most cases this PDU is being enumerated, but not always. It is
		// possible for the PDU to be stored on the SIM first before it is
		// received.
		//
		const TGsmSmsSlotEntry&  newSlot = aSmsMessage.iSlotArray[0];
		TInt  slotArrayCount = smsMessage->iSlotArray.Count();

		for (TInt slotNum = 0;  slotNum < slotArrayCount;  slotNum++ )
			{
			const TGsmSmsSlotEntry&  slot = smsMessage->iSlotArray[slotNum];

			if (slot.iIndex == newSlot.iIndex  && slot.iStore == newSlot.iStore)
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_UPDATEEXISTINGMESSAGEL_2, "CSmsReassemblyStore::UpdateExistingMessageL(): Duplicate enumerated PDU.");

				// It is a duplicate that was already stored on the SIM...
				aDuplicateSlot = ETrue;
				break;
				}
			}
		}

	TInt  indexArrayCount = indexArray->Count();

	for (TInt index = 0;  index < indexArrayCount;  index++ )
		{
		if (indexArray->At(index) == concatPDUIndex)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_UPDATEEXISTINGMESSAGEL_3, "CSmsReassemblyStore::UpdateExistingMessageL(): Duplicate concatenated PDU.");

			// The PDU is already stored in the reassembly store.
			aDuplicateMsgRef = ETrue;
			break;
			}
		}

	//Check whether this PDU is part of forwarded PDU.
	TSmsPreAllocatedFileStoreReassemblyEntry preAllocatedFileEntry = iPreallocatedFile->Entries()[preAllocatedFileIndex];
	if (preAllocatedFileEntry.NumberOfPDUsForwardToClient() > 0)
		{
		TUint8 bitMapIndex = (concatPDUIndex-1)/8;
		TUint8 bitPos = (concatPDUIndex-1)%8;
		TUint8 bitMap;
		preAllocatedFileEntry.GetBitMap(bitMapIndex, bitMap);
		TUint8 tmpBitMap = 1;
		tmpBitMap <<= bitPos;
		if (tmpBitMap == (bitMap & tmpBitMap))
			{
			aDuplicateMsgRef = ETrue;
			}		
		}

	if (aDuplicateMsgRef  ||  aDuplicateSlot)
		{
		CleanupStack::PopAndDestroy(3, smsMessage); // smsMessage, smsArray, indexArray
		return;
		}

	//
	// If the PDU is stored then add the slot information...
	//
	if (aSmsMessage.Storage() == CSmsMessage::ESmsSIMStorage  ||
		aSmsMessage.Storage() == CSmsMessage::ESmsCombinedStorage)
		{
		smsMessage->AddSlotL(aSmsMessage.iSlotArray[0]);
		}

	//
	// If the PDU is Unsent or Unread, then store that information...
	//
	NMobileSmsStore::TMobileSmsStoreStatus  status = aSmsMessage.Status();

	if (status == NMobileSmsStore::EStoredMessageUnsent  ||
		status == NMobileSmsStore::EStoredMessageUnread)
		{
		smsMessage->SetStatus(status);
		}

	/*
	Below logic is written in keeping view of different scenarios that might occur here:
	(1) Previous PDUs are in Permanent store file, this PDU arrives in out-of-disk condition (so will be stored in Pre-allocated file).
	(2) Previous PDUs are in Pre-allocated file (means PDU arrives in out-of-disk condition), this PDU arrives in normal condition.
	(3) Previous PDUs are in Permanent store file, this PDU also arrives in normal condition.
	(4) Previous PDUs are in Pre-allocated file (means PDU arrives in out-of-disk condition), this PDU also arrives in out-of-disk condition.
	*/

	//Always first try to store the message in permanent store file.
	//In case of out-of-disk condition store the message in pre-allocated file
	TInt ret = KErrNone;
	if (iDiskSpaceStatus == ESmsDiskSpaceFull)
		{
		ret = KErrDiskFull;
		}
	else
		{
		if (permanentStoreIndex==KErrNotFound)
			{
			// See condition (2)
			TInt tmpIndex;
			TRAP(ret, iPermanentFileStore->AddNewMessageL(tmpIndex, aSmsMessage, aGsmSms));
			}
		else
			{
			CSmsBuffer*  tmpBuffer = CSmsBuffer::NewL();
			CSmsMessage*  tmpSmsMessage = CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver, tmpBuffer);
			CleanupStack::PushL(tmpSmsMessage);

			indexArray->Reset();
			smsArray->Reset();
			iPermanentFileStore->InternalizeEntryL(permanentStoreIndex, *tmpSmsMessage, *indexArray, *smsArray);
			/*
			Check & remove those PDU from permanent store file which has already been 
			forwarded to client. Forwarded PDU might still remain in permanent store 
			file if it is deleted at the time of out-of-disk condition.
			*/
			if (preAllocatedFileEntry.NumberOfPDUsForwardToClient()>0)
				{
				for (TInt j=indexArray->Count(); j>0 ;j--)
					{
					TUint8 bitMapIndex = (indexArray->At(j-1)-1)/8;
					TUint8 bitPos = (indexArray->At(j-1)-1)%8;
					TUint8 bitMap;
					preAllocatedFileEntry.GetBitMap(bitMapIndex, bitMap);
					TUint8 tmpBitMap = 1;
					tmpBitMap <<= bitPos;
					if (tmpBitMap == (bitMap & tmpBitMap))
						{
						indexArray->Delete(j-1);
						smsArray->Delete(j-1);
						}
					}
				}

			indexArray->AppendL(concatPDUIndex);
			smsArray->AppendL(aGsmSms);

			TRAP(ret, iPermanentFileStore->UpdateExistingMessageL(permanentStoreIndex, *smsMessage, *indexArray, *smsArray));

			CleanupStack::PopAndDestroy(1); //tmpSmsMessage
			}
		}

	if (ret == KErrDiskFull)
		{
		iPreallocatedFile->UpdateExistingMessageL(preAllocatedFileIndex, aSmsMessage, concatPDUIndex, aGsmSms);
		}
	else if (ret!=KErrNone)
		{
		User::Leave(ret);
		}

	CleanupStack::PopAndDestroy(3, smsMessage); // smsMessage, indexArray, smsArray
	}

/*
It retrieves class 0 message from re-assembly store.
The message which match the passed entry information is returned from this function.

@param aEntry reference to entry information.
@param aSmsMessage reference to returned sms message.

@internalComponent
*/
void CClass0SmsReassemblyStore::RetrieveMessageL(const TReassemblyEntry& aEntry, CSmsMessage& aSmsMessage)
	{
	CArrayFix<TInt>*  indexArray = new(ELeave) CArrayFixFlat<TInt>(KFlatArrayGranularity);
	CleanupStack::PushL(indexArray);
	CArrayFixFlat<TGsmSms>*  smsArray = new(ELeave) CArrayFixFlat<TGsmSms>(KFlatArrayGranularity);
	CleanupStack::PushL(smsArray);

	GetSmsEntriesL(aEntry, aSmsMessage, *indexArray, *smsArray);

	//There is no need to decode a single segment message
	if (aEntry.Total()>1)
		{
		if (smsArray->Count() == aEntry.Total())
			{
			aSmsMessage.DecodeMessagePDUsL(*smsArray);
			}
		}
	CleanupStack::PopAndDestroy(2, indexArray); //smsArray, indexArray
	}

/*
It deletes the entry from re-assembly store.

@param aEntry reference to entry information.

@internalComponent
*/
void CClass0SmsReassemblyStore::DeleteEntryL(const TReassemblyEntry& aEntry)
	{
	TInt index;
	iPreallocatedFile->MatchEntryToExistingMessage(aEntry, index);
	if (index == KErrNotFound)
		{
		//This condition should never arise
		User::Leave(KErrNotFound);
		}
	iPreallocatedFile->DeleteEntryL(index);
	iPermanentFileStore->MatchEntryToExistingMessage(aEntry, index);
	if (index != KErrNotFound)
		{
		TRAP_IGNORE(iPermanentFileStore->DeleteEntryL(index));
		}
	}

/*
It updates the log server id of the passed entry (message).

@param aEntry reference to entry information.
@param aLogId log id of the message to be updated.

@internalComponent
*/
void CClass0SmsReassemblyStore::UpdateLogServerIdL(const TReassemblyEntry& aEntry, TLogId aLogServerId)
	{
	TInt index;
	iPreallocatedFile->MatchEntryToExistingMessage(aEntry, index);
	if (index == KErrNotFound)
		{
		//This condition should never arise
		User::Leave(KErrNotFound);
		}
	iPreallocatedFile->UpdateLogServerIdL(index, aLogServerId);
	}

/*
It sets the message passed to client or not. The message, which match the 
passed entry information, its passed to client value is set.

@param aEntry reference to entry information.
@param aBool boolean value indicating whether message is passed or not.

@internalComponent
*/
void CClass0SmsReassemblyStore::SetPassedToClientL(const TReassemblyEntry& aEntry, TBool aBool)
	{
	TInt index;
	iPreallocatedFile->MatchEntryToExistingMessage(aEntry, index);
	if (index == KErrNotFound)
		{
		//This condition should never arise
		User::Leave(KErrNotFound);
		}
	iPreallocatedFile->SetPassedToClientL(index, aBool);
	}

/**
It forwards the complete class 0 messages to client.
It checks whether the received object is complete or not.
If it is complete, then it forwards the complete message to client.
Otherwise it forms the message & forward the message to client.
In this it might be possible to forward multiple incomplete message.

@param aSmsComm  a reference to aSmsComm object which implemented the events.

@param aSmsMessage	a reference to sms message object. This sms message must be class 0 messages.

@param aOriginalSmsAddr pointer to the address of the sender of a previously sent

@param aOriginalSmsMessage pointer to a message previously sent matched to the received 
							one (e.g. status report).	Null if not matched.

@param aDes user data for the deliver report acknowledging this message to the SC.
			Filled in by the observer.

@internalComponent
*/
void CClass0SmsReassemblyStore::ForwardCompleteClass0SmsMessagesL(MSmsComm& aSmsComm, const CSmsMessage& aSmsMessage,const TSmsAddr* aOriginalSmsAddr,const CSmsMessage* aOriginalSmsMessage,TDes& aDes)
	{
	TBool passedToClient=ETrue;

	if (aSmsMessage.IsComplete())
		{
		//Message is complete so forward it.
		TInt ret = aSmsComm.ProcessMessageL(aSmsMessage, aOriginalSmsAddr, aOriginalSmsMessage, aDes);
		if (ret!=KErrNone)
			{
			passedToClient = EFalse;
			}
		}
	else
		{
		/*
		Message is not complete, but re-assembly store contains other constituent PDU
		of the message which makes the message complete.

		In case of class 0 messages, it is possible to forward the incomplete messages. So after forwarding
		the incomplete message, if we receive other constituent PDU of that message then in that case we 
		might receive all the constituent PDU of that message but aSmsMesssage will contain partial complete message.
		*/
		TReassemblyEntry entry;
		CReassemblyStoreUtility::PopulateEntry(entry, aSmsMessage, 1);

		CSmsBuffer* buffer = CSmsBuffer::NewL();
		CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver, buffer );
		CleanupStack::PushL( smsMessage );

		CArrayFix<TInt>*  indexArray = new(ELeave) CArrayFixFlat<TInt>(KFlatArrayGranularity);
		CleanupStack::PushL(indexArray);
		CArrayFixFlat<TGsmSms>*  smsArray = new(ELeave) CArrayFixFlat<TGsmSms>(KFlatArrayGranularity);
		CleanupStack::PushL(smsArray);

		GetSmsEntriesL(entry, *smsMessage, *indexArray, *smsArray);
		SortPDUsL(*indexArray, *smsArray);

		CArrayFixFlat<TGsmSms>*  tmpSmsArray = new(ELeave) CArrayFixFlat<TGsmSms>(KFlatArrayGranularity);
		CleanupStack::PushL(tmpSmsArray);

		//Form the sequence of incomplete message & forward it to client.
		TInt startSeqIndex(0), endSeqIndex(0);
		TInt i;
		do
			{
			i = startSeqIndex;
			for (; i < indexArray->Count() - 1; i++)
				{
				if (indexArray->At(i) + 1 != indexArray->At(i+1))
					{
					endSeqIndex = i;
					break;
					}
				}

			if (i == (indexArray->Count() - 1))
				{
				endSeqIndex = i;
				}

			tmpSmsArray->Reset();
			for (TInt j = startSeqIndex; j <= endSeqIndex; j++)
				{
				tmpSmsArray->AppendL(smsArray->At(j));
				}

			if (tmpSmsArray->Count() == entry.Total())
				{
				smsMessage->DecodeMessagePDUsL(*tmpSmsArray);
				}
			else
				{
				//Build the partial complete message.
				if (endSeqIndex == (indexArray->Count() - 1))
					{
					smsMessage->DecodePartialCompleteMessagePDUsL(*tmpSmsArray, ETrue);
					}
				else
					{
					smsMessage->DecodePartialCompleteMessagePDUsL(*tmpSmsArray, EFalse);
					}
				}

			//Forward the partial complete message to client.
			TInt ret=KErrNone;
			TRAP(ret, aSmsComm.ProcessMessageL(*smsMessage, aOriginalSmsAddr, aOriginalSmsMessage, aDes));
			if (ret != KErrNone)
				{
				passedToClient = EFalse;
				}
			startSeqIndex = endSeqIndex + 1;
			endSeqIndex = startSeqIndex;
			}
		while (startSeqIndex < indexArray->Count());

		CleanupStack::PopAndDestroy(4);	//tmpSmsArray, indexArray, smsArray, smsMessage
		}

	if(passedToClient)
		{
		SetMessagePassedToClientL(aSmsMessage);
		}

	//If the latest segement exceeds the limitation, then we can't wait till ack.
	//So call ProcessMessageIfExceedLimitationL() function & delete the oldest message.
	if (IsExceedLimitation())
		{
		ProcessMessageIfExceedLimitationL(aSmsComm);
		}
	}

/**
It frees the space by forwarding the class 0 message if class 0 re-assembly store 
exceeds limitation (max class 0 message, max reserved pdu segment).

@param aSmsComm  a reference to aSmsComm object which implemented the events.

@internalComponent
*/
void CClass0SmsReassemblyStore::ProcessMessageIfExceedLimitationL(MSmsComm& aSmsComm)
	{
	//Add Comment
	TInt class0MsgCount = iEntryArray.Count();
	TInt noOfMsgStoredInPreAllocatedFile = iPreallocatedFile->NumberOfPDUStored();
	if ((class0MsgCount > iMaxClass0Msg)
			|| (noOfMsgStoredInPreAllocatedFile >= iMaxPDUSeg))
		{
		CSmsBuffer* buffer = CSmsBuffer::NewL();
		CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver, buffer );
		CleanupStack::PushL( smsMessage );

		CArrayFix<TInt>*  indexArray = new(ELeave) CArrayFixFlat<TInt>(KFlatArrayGranularity);
		CleanupStack::PushL(indexArray);
		CArrayFixFlat<TGsmSms>*  smsArray = new(ELeave) CArrayFixFlat<TGsmSms>(KFlatArrayGranularity);
		CleanupStack::PushL(smsArray);

		//Add Comment
		TInt index;
		if (class0MsgCount > iMaxClass0Msg)
			{
			index = GetIndexOfOldestMessage();
			}
		else
			{
			index = GetIndexOfOldestMessageFromReservedFileL();
			}

		TReassemblyEntry entry = iEntryArray[index];

		GetSmsEntriesL(entry, *smsMessage, *indexArray, *smsArray);

		SortPDUsL(*indexArray, *smsArray);

		CArrayFixFlat<TGsmSms>*  tmpSmsArray = new(ELeave) CArrayFixFlat<TGsmSms>(KFlatArrayGranularity);
		CleanupStack::PushL(tmpSmsArray);

		//Form the message & forward it to client.
		TInt startSeqIndex(0), endSeqIndex(0);
		TInt i;
		do
			{
			i = startSeqIndex;
			for (; i < indexArray->Count() - 1; i++)
				{
				if (indexArray->At(i) + 1 != indexArray->At(i+1))
					{
					endSeqIndex = i;
					break;
					}
				}
			if (i == (indexArray->Count() - 1))
				{
				endSeqIndex = i;
				}
			tmpSmsArray->Reset();

			for (TInt j = startSeqIndex; j <= endSeqIndex; j++)
				{
				tmpSmsArray->AppendL(smsArray->At(j));
				}

			if (entry.IsComplete())
				{
				if (tmpSmsArray->Count() == entry.Total())
					{
					smsMessage->DecodeMessagePDUsL(*tmpSmsArray);
					}
				else
					{
					//Build the partial complete message.
					if (endSeqIndex == (indexArray->Count() - 1))
						{
						smsMessage->DecodePartialCompleteMessagePDUsL(*tmpSmsArray, ETrue);
						}
					else
						{
						smsMessage->DecodePartialCompleteMessagePDUsL(*tmpSmsArray, EFalse);
						}
					}
				}
			else
				{
				//Build the partial complete message.
				smsMessage->DecodePartialCompleteMessagePDUsL(*tmpSmsArray, EFalse);
				}
			//Forward the partial complete message to client.
			TBuf16<CSmsPDUProcessor::ESmsMaxDeliverReportBufferSize> buffer;
			TRAP_IGNORE(aSmsComm.ProcessMessageL(*smsMessage, NULL, NULL, buffer));

			/*
			Can't do much. So free the reserved space & also stored the information 
			related to forwarded message. If the message is complete then delete
			the complete message (see the condition after the while loop)rather 
			than freeing the memory. No need to store the complete messages forwaded 
			info.
			*/
			if ((noOfMsgStoredInPreAllocatedFile > iMaxPDUSeg) && (!entry.IsComplete()))
				{
				SetIncompleteMessageForwardedToClientL(*smsMessage);
				}
			startSeqIndex = endSeqIndex + 1;
			endSeqIndex = startSeqIndex;
			}
		while (startSeqIndex < indexArray->Count());
		//Can't do much. So removed the oldest message.
		if ((class0MsgCount > iMaxClass0Msg) ||
			((noOfMsgStoredInPreAllocatedFile > iMaxPDUSeg) && (entry.IsComplete())))
			{
			DeleteMessageL(*smsMessage, EFalse);
			}
		CleanupStack::PopAndDestroy(4);	//tmpSmsArray, indexArray, smsArray, smsMessage
		}
	}

/**
Guard timer calls this function when timer expires.
It goes through all the messages stored in re-assembly store & forwards those message
whose time has expired.
It is also called when a new observer is added or a PDU has been received and successfully 
processed. It is also called from ProcessCompleteClass0SmsMessagesL.

@internalComponent
*/
void CClass0SmsReassemblyStore::ProcessTimeoutMessageL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_PROCESSTIMEOUTMESSAGEL_1, "CClass0SmsReassemblyStore::ProcessTimeoutMessageL()");
	TBool passedToClient=ETrue;
	TInt count=iEntryArray.Count();

	CSmsBuffer* buffer = CSmsBuffer::NewL();
	CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver, buffer );
	CleanupStack::PushL( smsMessage );

	CArrayFix<TInt>*  indexArray = new(ELeave) CArrayFixFlat<TInt>(KFlatArrayGranularity);
	CleanupStack::PushL(indexArray);
	CArrayFixFlat<TGsmSms>*  smsArray = new(ELeave) CArrayFixFlat<TGsmSms>(KFlatArrayGranularity);
	CleanupStack::PushL(smsArray);

	CArrayFixFlat<TGsmSms>*  tmpSmsArray = new(ELeave) CArrayFixFlat<TGsmSms>(KFlatArrayGranularity);
	CleanupStack::PushL(tmpSmsArray);

	TTime time;
	time.UniversalTime();

	TTimeIntervalSeconds timeOutInSecs = iGuardTimeOut*3600;

	for (TInt k=count-1; k>=0; k--)
		{
		if (!iEntryArray[k].PassedToClient())
			{
			TTimeIntervalSeconds diffInSecs;
			time.SecondsFrom(iEntryArray[k].Time(), diffInSecs);
			indexArray->Reset();
			smsArray->Reset();
			if (diffInSecs >= timeOutInSecs)
				{
				GetSmsEntriesL(iEntryArray[k], *smsMessage, *indexArray, *smsArray);
				SortPDUsL(*indexArray, *smsArray);

				//Form the message & forward it to client.
				TInt startSeqIndex(0), endSeqIndex(0);
				TInt i;
				do
					{
					i = startSeqIndex;
					for (; i < indexArray->Count() - 1; i++)
						{
						if (indexArray->At(i) + 1 != indexArray->At(i+1))
							{
							endSeqIndex = i;
							break;
							}
						}
					if (i == (indexArray->Count() - 1))
						{
						endSeqIndex = i;
						}
					tmpSmsArray->Reset();
					for (TInt j = startSeqIndex; j <= endSeqIndex; j++)
						{
						tmpSmsArray->AppendL(smsArray->At(j));
						}

					if (iEntryArray[k].IsComplete())
						{
						if (tmpSmsArray->Count() == iEntryArray[k].Total())
							{
							smsMessage->DecodeMessagePDUsL(*tmpSmsArray);
							}
						else
							{
							//Build the partial complete message.
							if (endSeqIndex == (indexArray->Count() - 1))
								{
								smsMessage->DecodePartialCompleteMessagePDUsL(*tmpSmsArray, ETrue);
								}
							else
								{
								smsMessage->DecodePartialCompleteMessagePDUsL(*tmpSmsArray, EFalse);
								}
							}				
						}
					else
						{
						//Build the partial complete message.
						if (endSeqIndex == (indexArray->Count() - 1))
							{
							smsMessage->DecodePartialCompleteMessagePDUsL(*tmpSmsArray, ETrue);
							}
						else
							{
							smsMessage->DecodePartialCompleteMessagePDUsL(*tmpSmsArray, EFalse);
							}
						}

					//Forward the partial complete message to client.
					TInt ret=KErrNone;
					TBuf16<CSmsPDUProcessor::ESmsMaxDeliverReportBufferSize> buffer;
					ret = iSmsComm.ProcessMessageL(*smsMessage, NULL, NULL, buffer);
					if (ret != KErrNone)
						{
						passedToClient = EFalse;
						}

					startSeqIndex = endSeqIndex + 1;
					endSeqIndex = startSeqIndex;
					}
				while (startSeqIndex < indexArray->Count());

				if(passedToClient)
					{
					SetMessagePassedToClientL(*smsMessage);
					}
				}	//diffInSecs >= timeOutInSecs
			}	//!iEntryArray[k].PassedToClient()
		}	//For k loop
	CleanupStack::PopAndDestroy(4);	//tmpSmsArray, indexArray, smsArray, smsMessage
	}

/**
It sets the bit-map of forwarded incomplete message to client.
It frees up the memory by removing the forwarded PDUs.

@param aSmsMessage	a reference to forwaded incomplete class 0 sms message object.

@internalComponent
*/
void CClass0SmsReassemblyStore::SetIncompleteMessageForwardedToClientL(const CSmsMessage& aSmsMessage)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_SETINCOMPLETEMESSAGEFORWARDEDTOCLIENTL_1, "CClass0SmsReassemblyStore::SetIncompleteMessageForwardedToClientL()");
	TInt index = KErrNotFound;

	if (aSmsMessage.IsComplete())
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_SETINCOMPLETEMESSAGEFORWARDEDTOCLIENTL_2, "This function must be called when message is incomplete");
		User::Leave(KErrArgument);
		}

	if (FindMessageL(aSmsMessage, EFalse, index))
		{
		CIncompleteClass0MessageInfo& incompleteClass0MsgInfo = (CIncompleteClass0MessageInfo &) aSmsMessage.GetOperationsForNonIEL(ESmsIncompleteClass0MessageParameter);
		// Retrieve incomplete class 0 message information & process
		TInt startPos, endPos;
		TBool isLastMessage;
		incompleteClass0MsgInfo.GetIncompleteMessageInfoL(startPos, endPos, isLastMessage);

		const TReassemblyEntry&  entry = iEntryArray[index];

		//Remove the forwarded PDU entries from pre-allocated file
		TInt preAllocatedFileIndex=KErrNotFound;
		iPreallocatedFile->MatchEntryToExistingMessage(entry, preAllocatedFileIndex);
		if (preAllocatedFileIndex!=KErrNotFound)
			{
			BeginTransactionLC();
			iPreallocatedFile->RemovePDUsL(preAllocatedFileIndex, startPos, endPos);
			iPreallocatedFile->StoreForwardedPDUsInfoL(preAllocatedFileIndex, startPos, endPos);
			CommitTransactionL();
			}
		else
			{
			//This situation should never arise
			User::Leave(KErrNotFound);
			}

		//Remove the forwarded PDU entries from permanent store file
		TInt permanentStoreIndex=KErrNotFound;
		iPermanentFileStore->MatchEntryToExistingMessage(entry, permanentStoreIndex);
		if (permanentStoreIndex!=KErrNotFound)
			{
			TRAP_IGNORE(BeginTransactionLC();
						iPermanentFileStore->RemovePDUsL(permanentStoreIndex, startPos, endPos);
						CommitTransactionL(););
			}
		}
	}

/**
This function is needed to ensure that permanent file
clean itself with pre-allocated file.
There migth be a scenario where user has deleted a message
but the corresponding message has not been deleted from permanent store file
due to out-of-disk condition. But the entry is invalid because
it is no more in the pre-allocated file which contains master header info.

@return the error code.

@internalComponent
*/
TInt CClass0SmsReassemblyStore::CleanReassemblyEntries()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_CLEANREASSEMBLYENTRIES_1, "CleanReassemblyEntries");
	const CArrayFix<TSmsPreAllocatedFileStoreReassemblyEntry>& preAllocatedFileEntryArray = iPreallocatedFile->Entries();
	TInt ret=KErrNone;
	TRAP(ret,	BeginTransactionLC();
				iPermanentFileStore->CleanupEntriesL(preAllocatedFileEntryArray);
				CommitTransactionL(););

	if (ret == KErrDiskFull)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_CLEANREASSEMBLYENTRIES_2, "CleanupEntriesL returns KErrDiskFull");
		/*
		Get access to reserve memory, call again to clean the entries with compact.
		Compact needs to be called at this instance because permanent store
		will use reserve disk space to delete, but after deletion reserve
		disk space will be freed. So compact will make sure that reserve disk
		space is not used after compaction.
		*/
		iFs.GetReserveAccess(KStoreDrive);
		TRAP(ret,	BeginTransactionLC();
					iPermanentFileStore->CleanupEntriesWithCompactL(preAllocatedFileEntryArray);
					CommitTransactionL(););
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_CLEANREASSEMBLYENTRIES_3, "CleanupEntriesWithCompactL returns %d", ret);
		iFs.ReleaseReserveAccess(KStoreDrive);
		}
	return ret;
	}

/**
It returns the SMS entries (sms messages, sms pdu arrays & the corresponding index array)
from its permanent store & pre-allocated file.

@param aEntry reference to entry information. The message which match this entry information
				is returned to caller of the function.
@param aSmsMessage	a reference to class 0 sms message object.
@param aIndexArray	a reference to the array of pdu index.
@param aSmsArray	a reference to the array of pdu.

@internalComponent
*/
void CClass0SmsReassemblyStore::GetSmsEntriesL(const TReassemblyEntry& aEntry, CSmsMessage& aSmsMessage, CArrayFix<TInt>& aIndexArray, CArrayFix<TGsmSms>& aSmsArray)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_GETSMSENTRIESL_1, "CClass0SmsReassemblyStore::GetSmsEntriesL()");
	TInt permanentStoreIndex=KErrNotFound;
	iPermanentFileStore->MatchEntryToExistingMessage(aEntry, permanentStoreIndex);
	if (permanentStoreIndex!=KErrNotFound)
		{
		iPermanentFileStore->InternalizeEntryL(permanentStoreIndex, aSmsMessage, aIndexArray, aSmsArray);
		}
	else
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_GETSMSENTRIESL_2, "No PDUs in Permanent store file");
		}

	TInt preAllocatedFileIndex=KErrNotFound;
	iPreallocatedFile->MatchEntryToExistingMessage(aEntry, preAllocatedFileIndex);
	if (preAllocatedFileIndex!=KErrNotFound)
		{
		iPreallocatedFile->InternalizeEntryL(preAllocatedFileIndex, aSmsMessage, aIndexArray, aSmsArray);

		//Filter forwarde PDU here..this condition might arise if the PDU
		//could not be deleted at the time of OOD condition from permanent store file.
		TSmsPreAllocatedFileStoreReassemblyEntry preAllocatedFileEntry = iPreallocatedFile->Entries()[preAllocatedFileIndex];
		if (preAllocatedFileEntry.NumberOfPDUsForwardToClient() > 0)
			{
			for (TInt j=aIndexArray.Count(); j>0 ;j--)
				{
				TUint8 bitMapIndex = (aIndexArray.At(j-1)-1)/8;
				TUint8 bitPos = (aIndexArray.At(j-1)-1)%8;
				TUint8 bitMap;
				preAllocatedFileEntry.GetBitMap(bitMapIndex, bitMap);
				TUint8 tmpBitMap = 1;
				tmpBitMap <<= bitPos;
				if (tmpBitMap == (bitMap & tmpBitMap))
					{
					aIndexArray.Delete(j-1);
					aSmsArray.Delete(j-1);
					}
				}
			}

		if (permanentStoreIndex==KErrNotFound)
			{
			//In this scenario a CSmsMessage object has to be created from the existing PDU in
			//pre-allocated file & then serialized into aSmsMessage.
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_GETSMSENTRIESL_3, "Number of PDUs in Pre-allocated file %d", aIndexArray.Count());
			if (aIndexArray.Count() > 0)
				{
				CSmsBuffer* smsBuffer = CSmsBuffer::NewL();
				CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, aSmsArray.At(0), smsBuffer, EFalse, ETrue);
				CleanupStack::PushL(smsMessage);

				//Update sms messages's properties.
				smsMessage->SetStorage(aSmsMessage.Storage());
				smsMessage->SetStatus((NMobileSmsStore::TMobileSmsStoreStatus)aSmsMessage.Status());
				smsMessage->SetLogServerId(aSmsMessage.LogServerId());
				smsMessage->SetTime(aSmsMessage.Time());
				smsMessage->SetUTCOffset(aSmsMessage.UTCOffset());
				smsMessage->SetDecodedOnSIM(aSmsMessage.DecodedOnSim());
				smsMessage->SetForwardToClient(aSmsMessage.ForwardToClient());
				smsMessage->SetToFromAddressL(aSmsMessage.ToFromAddress());

				CBufSeg* recvbufseg = CBufSeg::NewL(KSmsMaxSegmentLength);
				CleanupStack::PushL(recvbufseg);
				// Attempt to serialize this message into the buffer
				RBufWriteStream writestream(*recvbufseg);
				writestream.Open(*recvbufseg);
				CleanupClosePushL(writestream);
				writestream << *smsMessage;

				// Initialize the read stream with the buffer
				RBufReadStream readstream(*recvbufseg);
				readstream.Open(*recvbufseg,0);
				CleanupClosePushL(readstream);
				// De-serialize the message from using the read stream
				readstream >> aSmsMessage;
				CleanupStack::PopAndDestroy(4);		//readstream, writestream, recvbufseg, smsMessage
				}
			}
		}
	}

/*
It returns when the next timeout should occur.
*/
void CClass0SmsReassemblyStore::GetNextGuardTimeout(TTime& aNextTimeout)
	{
	TTime curTime;
	curTime.UniversalTime();
	TTimeIntervalSeconds timeOutInSecs = iGuardTimeOut*3600;
	aNextTimeout = curTime + timeOutInSecs;

	TInt class0ReassemblyStoreCount = iEntryArray.Count();

	if (class0ReassemblyStoreCount > 0)
		{
		for (TInt i=0; i<class0ReassemblyStoreCount;i++)
			{
			TTime guradTimeout = iEntryArray[i].Time() + timeOutInSecs;
			if ((guradTimeout > curTime) &&
				(guradTimeout < aNextTimeout))
				{
				aNextTimeout = guradTimeout;
				}
			}
		}
	}

/*
It returns the index of oldest message in class 0 re-assembly store.
*/
TInt CClass0SmsReassemblyStore::GetIndexOfOldestMessage()
	{
	TInt index = 0;
	TTime time = iEntryArray[index].Time();
	for (TInt index2=1; index2 < iEntryArray.Count(); index2++)
		{
		if (iEntryArray[index2].Time() < time)
			{
			index = index2;
			}
		}
	return index;
	}

/*
It returns the index of oldest message in class 0 re-assembly store.
*/
TInt CClass0SmsReassemblyStore::GetIndexOfOldestMessageFromReservedFileL()
	{
	TInt index = KErrNotFound;
	//Find the index of oldest message in pre-allocated file.
	index = iPreallocatedFile->GetOldestMessageEntryIndex();
	if (index == KErrNotFound)
		{
		//This condition should never arise
		User::Leave(KErrNotFound);
		}

	//Compare the oldest pre-allocated message entry against class 0 reassembly store entry
	//and return the index of class 0 reassembly store entry.
	const TSmsPreAllocatedFileStoreReassemblyEntry& preAllocatedFileEntry = iPreallocatedFile->Entries()[index];

	TInt reassemblyCount = iEntryArray.Count();
	TInt count;
	for (count = 0;  count < reassemblyCount;  count++)
		{
		TReassemblyEntry&  entry = iEntryArray[count];

		if (entry.Reference() == preAllocatedFileEntry.Reference()  &&
			entry.Total() == preAllocatedFileEntry.Total()  &&
			entry.PduType() == preAllocatedFileEntry.PduType()  &&
			entry.Storage() == preAllocatedFileEntry.Storage()  &&
			entry.Description2() == preAllocatedFileEntry.Description2())
			{
			//
			// Found it!
			//
			index = count;
			break;
			}
		}
	if (count == reassemblyCount)
		{
		//This condition should never arise
		User::Leave(KErrNotFound);
		}
	return index;
	}

/*
It returns a boolean value indicating whether the limitation (max class 0 message,
max pdu stored in pre-allocated file) imposed on class 0 reassembly store exceeds or not.
*/
TBool CClass0SmsReassemblyStore::IsExceedLimitation()
	{
	TInt class0MsgCount = iEntryArray.Count();
	TInt noOfMsgStoredInPreAllocatedFile = iPreallocatedFile->NumberOfPDUStored();
	if ((class0MsgCount > iMaxClass0Msg)
			|| (noOfMsgStoredInPreAllocatedFile > iMaxPDUSeg))
		{
		return ETrue;
		}

	return EFalse;
	}

/*
This sorting algorithm is based on bubble sort
*/
void CClass0SmsReassemblyStore::SortPDUsL(CArrayFix<TInt>& aIndexArray, CArrayFix<TGsmSms>& aSmsArray)
	{
	//Test---Index array count must be equal to sms array count
	TBool swapped;

	do
		{
		swapped = EFalse;
		for (TInt i=0; i<aIndexArray.Count()-1 ;i++)
			{
			if (aIndexArray[i] > aIndexArray[i+1])
				{
				aSmsArray.InsertL(i, aSmsArray[i+1]);
				aSmsArray.Delete(i+2);
				aIndexArray.InsertL(i, aIndexArray[i+1]);
				aIndexArray.Delete(i+2);
				swapped = ETrue;
				}
			}
		}
	while (swapped);
	}

/**
 *  Utility func for cleanup stack
 */
void CClass0StoreCloseObject(TAny* aObj)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0STORECLOSEOBJECT_1, "WARNING! Hey, CClass0StoreCloseObject called by Untrapper! [0x%08x]", (TUint)aObj);
	((CClass0SmsReassemblyStore*)aObj)->Revert();
	}

/**
 *  Sets the class 0 re-assembly store as in-transaction.
 *  
 *  The function checks the validity of the call and leaves KErrAccessDenied if
 *  invalid.
 *  @capability None
 */
void CClass0SmsReassemblyStore::BeginTransactionLC()
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_BEGINTRANSACTIONLC_1, "CClass0SmsReassemblyStore::BeginTransactionLC [this=0x%08X iInTransaction=%d]", (TUint)this, iInTransaction);

	if (iInTransaction)
		{
	    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_BEGINTRANSACTIONLC_2, "WARNING CClass0SmsReassemblyStore::BeginTransactionLC leaving with KErrAccessDenied");
		User::Leave(KErrAccessDenied);
		}

	TCleanupItem class0StoreClose(CClass0StoreCloseObject, this);
	CleanupStack::PushL(class0StoreClose);
	iPreallocatedFile->BeginTransactionL();
	iPermanentFileStore->BeginTransactionL();
	iInTransaction = ETrue;
	}

/**
 *  It commits the transaction.
 */
void CClass0SmsReassemblyStore::CommitTransactionL()
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_COMMITTRANSACTIONL_1, "CClass0SmsReassemblyStore::CommitTransactionL(): this=0x%08X iInTransaction=%d",(TUint)this, iInTransaction);

	//Commit permanent store file
	iPermanentFileStore->CommitTransactionL();
	//Commit pre-allocated file
	iPreallocatedFile->CommitTransactionL();
	CleanupStack::Pop(this);	//CClass0StoreCloseObject
	iInTransaction = EFalse;
	}

/**
 *  It reverts the transaction.
 */
void CClass0SmsReassemblyStore::Revert()
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CCLASS0SMSREASSEMBLYSTORE_REVERT_1, "CClass0SmsReassemblyStore::Revert(): this=0x%08X, iInTransaction=%d",(TUint)this, iInTransaction);

	iPreallocatedFile->Revert();
	iPermanentFileStore->Revert();
	iInTransaction = EFalse;
	}
