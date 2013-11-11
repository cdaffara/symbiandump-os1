// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements CSmsReassemblyStore and CSmsSegmentationStore.
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "smspstorTraces.h"
#endif

#include <e32svr.h>
#include <es_ini.h>
#include "smspstor.h"
#include "smspmain.h"
#include "smsuaddr.H"
#include "Gsmumsg.h"
#include "gsmubuf.h"
#include <logwraplimits.h>
#include "Gsmuelem.h"
#include "gsmuieoperations.h"
#include "gsmunonieoperations.h"

LOCAL_C TPtrC TrimLeadingZeros(const TDesC& aString)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TRIMLEADINGZEROS_1, "CSARStore::ExternalizeEntryArrayL()");

	const TInt len = aString.Length();

	if (len == 0)
		return aString;

	const TUint16* startChar = &aString[0];
	const TUint16* endChar = &aString[len-1];

	while (startChar <= endChar && *startChar == '0')
		{
		++startChar;
		}
	return TPtrC(startChar, endChar - startChar + 1);
	} // TrimLeadingZeros


/**
 *  Creates new CSmsReassemblyStore instance
 *  
 *  @param aFs  File Server handle.
 */
CSmsReassemblyStore* CSmsReassemblyStore::NewL(RFs& aFs)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREASSEMBLYSTORE_NEWL_1, "CSmsReassemblyStore::NewL()");

	CSmsReassemblyStore*  self = new (ELeave) CSmsReassemblyStore(aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
	} // CSmsReassemblyStore::NewL


/**
 *  Creates and starts timer
 */
void CSmsReassemblyStore::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREASSEMBLYSTORE_CONSTRUCTL_1, "CSmsReassemblyStore::ConstructL()");

	//
	// Generate the full path to the reassembly store.
	//
	PrivatePath(iFullPathBuf);
	iFullPathBuf.Append(KReassemblyStoreName);

 	iLastReceivedTime.UniversalTime();
 	iLastRealTime = iLastReceivedTime;
	} // CSmsReassemblyStore::ConstructL


/**
 *  Destructor
 */
CSmsReassemblyStore::~CSmsReassemblyStore()
	{
	// NOP
	} // CSmsReassemblyStore::~CSmsReassemblyStore


void CSmsReassemblyStore::UpdateLogServerIdL(TInt aIndex, TLogId aLogServerId)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREASSEMBLYSTORE_UPDATELOGSERVERIDL_1, "CSmsReassemblyStore::UpdateLogServerIdL()");

	TSmsReassemblyEntry entry(reinterpret_cast<const TSmsReassemblyEntry&>(Entries()[aIndex]));

	if (entry.LogServerId() != aLogServerId)
		{
		entry.SetLogServerId(aLogServerId);
		BeginTransactionLC();
		ChangeEntryL(aIndex, entry);
		CommitTransactionL();
		}
	} // CSmsReassemblyStore::UpdateLogServerIdL


/**
 *  Searches the reassembly store for a CSmsMessage and returns its index.
 *
 *  @param aSmsMessage  Message to search for.
 *  @param aPassed      Determines if we are searching for a message already
 *                      passed to the client.
 *  @param aIndex       Return index value.
 *
 *  @return True and an index if aSmsMessage is found in this reassembly store
 */
TBool CSmsReassemblyStore::FindMessageL(const CSmsMessage& aSmsMessage,
										TBool aPassed,
										TInt& aIndex)
 	{
 	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREASSEMBLYSTORE_FINDMESSAGEL_1, "CSmsReassemblyStore::FindMessageL()");

	//
	// Parse the GSM data from the SMS message...
	//
	TGsmSmsTelNumber  parsedAddress;

	aSmsMessage.ParsedToFromAddress(parsedAddress);

	//
	// Search the store for a matching message...
	//
 	for (TInt index = Entries().Count() - 1;  index >= 0;  index--)
 		{
		const TSmsReassemblyEntry&  entry = reinterpret_cast<const TSmsReassemblyEntry&>(Entries()[index]);

		// Always search the basic types first and strings last!
		if (entry.PduType() == aSmsMessage.Type()  &&
			entry.PassedToClient() == aPassed  &&
			entry.Storage() == aSmsMessage.Storage()  &&
			entry.Time() == aSmsMessage.Time()  &&
			entry.Description2().Right(8) == parsedAddress.iTelNumber.Right(8))
 			{
			//
			// Found!
			//
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREASSEMBLYSTORE_FINDMESSAGEL_2, "CSmsReassemblyStore::FindMessage(): Found! index=%d", index);

			aIndex = index;

			return ETrue;
			}
 		}

	//
	// Not found...
	//
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREASSEMBLYSTORE_FINDMESSAGEL_3, "CSmsReassemblyStore::FindMessage(): Not found!");

 	return EFalse;
 	} // CSmsReassemblyStore::FindMessageL


/**
 *  Adds Pdu to reassembly store
 *  
 *  @param aSmsMessage  PDU to 
 *  @param aGsmSms Used to
 *  @param aIndex Used to
 *  @param aComplete Used to
 *  @param aServiceCenterAddressPresent Used to
 */
void CSmsReassemblyStore::MatchPDUToExistingMessage(const CSmsMessage& aSmsMessage,
													TInt& aIndex)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREASSEMBLYSTORE_MATCHPDUTOEXISTINGMESSAGE_1, "CSmsReassemblyStore::MatchPDUToExistingMessage()");

	__ASSERT_ALWAYS(!aSmsMessage.IsDecoded(), SmspPanic(KSmspPanicMessageConcatenated));

	aIndex = KErrNotFound;

	TGsmSmsTelNumber  parsedAddress;
	aSmsMessage.ParsedToFromAddress(parsedAddress);

	//
	// Search the reassembly store for a matching entry (start from the
	// end as the most recent PDUs appear at the end)...
	//
	TInt reassemblyCount = Entries().Count();

	for (TInt index = reassemblyCount - 1;  index >= 0;  index--)
		{
		TSmsReassemblyEntry&  entry = (TSmsReassemblyEntry&) Entries()[index];

		// Always check the fields in order of the quickest to check...
		if (entry.IsComplete() == EFalse  &&
		    entry.PduType() == aSmsMessage.Type()  &&
			entry.Storage() == aSmsMessage.Storage())
			{
			TInt  telLen = Min(entry.Description2().Length(),
					           parsedAddress.iTelNumber.Length());
			
			if (entry.Description2().Right(telLen) == parsedAddress.iTelNumber.Right(telLen)  &&
		        entry.Total() == aSmsMessage.SmsPDU().NumConcatenatedMessagePDUs()  &&
			    entry.Reference() == aSmsMessage.SmsPDU().ConcatenatedMessageReference())
				{
				//
				// Found it!
				//
				aIndex = index;
				break;
				}
			}
		}

	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREASSEMBLYSTORE_MATCHPDUTOEXISTINGMESSAGE_2, "CSmsReassemblyStore::MatchPDUToExistingMessage(): reassemblyCount=%d, aIndex=%d", reassemblyCount, aIndex);
	} // CSmsReassemblyStore::MatchPDUToExistingMessage


void CSmsReassemblyStore::UpdateExistingMessageL(const CSmsMessage& aSmsMessage,
												 const TGsmSms& aGsmSms, TInt aIndex,
												 TBool& aComplete,
												 TBool& aDuplicateMsgRef,
												 TBool& aDuplicateSlot)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREASSEMBLYSTORE_UPDATEEXISTINGMESSAGEL_1, "CSmsReassemblyStore::UpdateExistingMessageL()");

	aComplete        = EFalse;
	aDuplicateMsgRef = EFalse;
	aDuplicateSlot   = EFalse;

	//
	// Read the segment from the store into a CSmsMessage buffer...
	//
	TSmsReassemblyEntry  entry = (TSmsReassemblyEntry&) Entries()[aIndex];

	CArrayFix<TInt>*  indexArray = new(ELeave) CArrayFixFlat<TInt>(8);
	CleanupStack::PushL(indexArray);
	CArrayFixFlat<TGsmSms>*  smsArray = new(ELeave) CArrayFixFlat<TGsmSms>(8);
	CleanupStack::PushL(smsArray);
	CSmsBuffer*  buffer = CSmsBuffer::NewL();
	CSmsMessage*  smsMessage = CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver, buffer);
	CleanupStack::PushL(smsMessage);

	InternalizeEntryL(entry.DataStreamId(), *smsMessage, *indexArray, *smsArray);
	
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
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREASSEMBLYSTORE_UPDATEEXISTINGMESSAGEL_2, "CSmsReassemblyStore::UpdateExistingMessageL(): Duplicate enumerated PDU.");

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
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREASSEMBLYSTORE_UPDATEEXISTINGMESSAGEL_3, "CSmsReassemblyStore::UpdateExistingMessageL(): Duplicate concatenated PDU.");

			// The PDU is already stored in the reassembly store.
			aDuplicateMsgRef = ETrue;
			break;
			}
		}

	if (aDuplicateMsgRef  ||  aDuplicateSlot)
		{
		CleanupStack::PopAndDestroy(3, indexArray); // smsMessage, smsArray, indexArray
		return;
		}

	//
	// If the PDU is stored then add the slot information...
	//
	if (smsMessage->Storage() == CSmsMessage::ESmsSIMStorage  ||
		smsMessage->Storage() == CSmsMessage::ESmsCombinedStorage)
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

	//
	// Does this PDU mean the message is complete? If so then decode the message,
	// reset the index and sms arrays (to save space for completed SMSs).
	//
	indexArray->AppendL(concatPDUIndex);
	smsArray->AppendL(aGsmSms);

	if (smsArray->Count() == smsMessage->SmsPDU().NumConcatenatedMessagePDUs())
		{
		smsMessage->DecodeMessagePDUsL(*smsArray);

		indexArray->Reset();
		smsArray->Reset();

		aComplete = ETrue;
		}

	//
	// Write the entry back into the store...
	//
	TStreamId  streamid = entry.DataStreamId();
	smsMessage->SetLogServerId(entry.LogServerId());

	BeginTransactionLC();
	ExternalizeEntryL(streamid, *smsMessage, *indexArray, *smsArray);
	PopulateEntry(entry, *smsMessage, smsArray->Count());
	ChangeEntryL(aIndex, entry);
	CommitTransactionL();

	CleanupStack::PopAndDestroy(3, indexArray); // smsMessage, smsArray, indexArray
	} // CSmsReassemblyStore::UpdateExistingMessageL


void CSmsReassemblyStore::NewMessagePDUL(TInt& aIndex,CSmsMessage& aSmsMessage,const TGsmSms& aGsmSms)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREASSEMBLYSTORE_NEWMESSAGEPDUL_1, "CSmsReassemblyStore::NewMessagePDUL");

	CArrayFix<TInt>* indexarray=new(ELeave) CArrayFixFlat<TInt>(8);
	CleanupStack::PushL(indexarray);
	CArrayFixFlat<TGsmSms>* smsarray=new(ELeave) CArrayFixFlat<TGsmSms>(8);
	CleanupStack::PushL(smsarray);
	TInt index=aSmsMessage.IsDecoded()? 0: aSmsMessage.SmsPDU().ConcatenatedMessagePDUIndex();
	indexarray->AppendL(index);
	smsarray->AppendL(aGsmSms);
	aIndex=Entries().Count();
	CreateEntryL(aSmsMessage,*indexarray,*smsarray);
	CleanupStack::PopAndDestroy(2);
	} // CSmsReassemblyStore::NewMessagePDUL


void CSmsReassemblyStore::GetMessageL(TInt aIndex,CSmsMessage& aSmsMessage)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREASSEMBLYSTORE_GETMESSAGEL_1, "CSmsReassemblyStore::GetMessageL [aIndex=%d]", aIndex);

	CArrayFix<TInt>* indexarray=new(ELeave) CArrayFixFlat<TInt>(8);
	CleanupStack::PushL(indexarray);
	CArrayFixFlat<TGsmSms>* smsarray=new(ELeave) CArrayFixFlat<TGsmSms>(8);
	CleanupStack::PushL(smsarray);
	InternalizeEntryL(Entries()[aIndex].DataStreamId(),aSmsMessage,*indexarray,*smsarray);
	TInt logid=Entries()[aIndex].LogServerId();
	if(aSmsMessage.LogServerId() == KLogNullId &&  logid != KLogNullId)
		aSmsMessage.SetLogServerId(logid);
	CleanupStack::PopAndDestroy(2); // smsarray, indexarray
	} // CSmsReassemblyStore::GetMessageL


/**
 *  internalize all the entries from the permanent file store to internal memory
 *  
 *  NOTE! You have to call CSARStore::OpenFileLC() before calling this function
 */
void CSmsReassemblyStore::InternalizeEntryL(const TStreamId& aStreamId,CSmsMessage& aSmsMessage,CArrayFix<TInt>& aIndexArray,CArrayFix<TGsmSms>& aSmsArray)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREASSEMBLYSTORE_INTERNALIZEENTRYL_1, "CSmsReassemblyStore::InternalizeEntryL Start [sid=%d]", aStreamId.Value());
	RStoreReadStream readstream;
	readstream.OpenLC(FileStore(),aStreamId);
	readstream >> aSmsMessage;
	TInt count=readstream.ReadInt32L();
	aIndexArray.Reset();
	TInt i;
	for (i=0; i<count; i++)
		{
		TInt index=readstream.ReadInt32L();
		aIndexArray.AppendL(index);
		}
	count=readstream.ReadInt32L();
	if(count!=aIndexArray.Count())
		User::Leave(KErrCorrupt);
	aSmsArray.Reset();
	for (i=0; i<count; i++)
		{
		RMobileSmsMessaging::TMobileSmsGsmTpdu pdu;
		readstream >> pdu;
		TGsmSms sms;
		sms.SetPdu(pdu);
		aSmsArray.AppendL(sms);
		}
	CleanupStack::PopAndDestroy();
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREASSEMBLYSTORE_INTERNALIZEENTRYL_2, "CSmsReassemblyStore::InternalizeEntryL End [count=%d]", count);
	} // CSARStore::OpenFileLC


/**
 *  externalizes all the entries from the internal memory to the permanent file store
 *  
 *  NOTE! You have to call CSARStore::OpenFileLC() before calling this function
 */
void CSmsReassemblyStore::ExternalizeEntryL(TStreamId& aStreamId,const CSmsMessage& aSmsMessage,const CArrayFix<TInt>& aIndexArray,const CArrayFix<TGsmSms>& aSmsArray)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREASSEMBLYSTORE_EXTERNALIZEENTRYL_1, "CSmsReassemblyStore::ExternalizeEntryL Start [sid=%d]", aStreamId.Value());

	RStoreWriteStream writestream;
	if (aStreamId==KNullStreamId)
		aStreamId=writestream.CreateLC(FileStore());
	else
		writestream.ReplaceLC(FileStore(),aStreamId);
	writestream << aSmsMessage;
	TInt count=aIndexArray.Count();
	__ASSERT_ALWAYS(count==aIndexArray.Count(),SmspPanic(KSmspPanicBadIndexArray));
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

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREASSEMBLYSTORE_EXTERNALIZEENTRYL_2, "CSmsReassemblyStore::ExternalizeEntryL End [count=%d]", count);
	} // CSARStore::OpenFileLC


void CSmsReassemblyStore::PopulateEntry(TSmsReassemblyEntry& aEntry,const CSmsMessage& aSmsMessage,TInt aNumSmss)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREASSEMBLYSTORE_POPULATEENTRY_1, "CSmsReassemblyStore::PopulateEntry");
	aEntry.SetReference(0);
	aEntry.SetTotal(1);
	aEntry.SetCount(1);

	if (aSmsMessage.TextPresent())
		{
		if (aSmsMessage.SmsPDU().TextConcatenated())
			{
			aEntry.SetReference(aSmsMessage.SmsPDU().ConcatenatedMessageReference());
			aEntry.SetTotal(aSmsMessage.SmsPDU().NumConcatenatedMessagePDUs());
			aEntry.SetCount(aSmsMessage.IsComplete()? aEntry.Total(): aNumSmss);
			}
		TInt bits7to4=aSmsMessage.SmsPDU().Bits7To4();
		TInt count=aSmsMessage.SmsPDU().UserData().NumInformationElements();
		TInt identifier1=0xFF;
		TInt identifier2=0x00;
		for (TInt i=0; i<count; i++)
			{
			TInt identifier=aSmsMessage.SmsPDU().UserData().InformationElement(i).Identifier();
			if ((identifier!=CSmsInformationElement::ESmsIEIConcatenatedShortMessages8BitReference) && (identifier!=CSmsInformationElement::ESmsIEIConcatenatedShortMessages16BitReference))
				{
				if (identifier<identifier1)
					identifier1=identifier;
				if (identifier>identifier2)
					identifier2=identifier;
				}
			}

		if ((bits7to4>=TSmsDataCodingScheme::ESmsDCSMessageWaitingIndicationDiscardMessage) && (bits7to4<=TSmsDataCodingScheme::ESmsDCSMessageWaitingIndicationUCS2))
			aEntry.SetBits7to4andIdentifiers(bits7to4, identifier1, identifier2);
		else
			aEntry.SetBits7to4andIdentifiers(0, identifier1, identifier2);
		}

	//Set the logServerId to aSmsMessage.LogServerId()

	aEntry.SetLogServerId(aSmsMessage.LogServerId());

	const CSmsPDU::TSmsPDUType type(aSmsMessage.Type());
	aEntry.SetPduType(type);
	aEntry.SetPassedToClient(EFalse);

	aEntry.SetStorage(aSmsMessage.Storage());
	if ((type!=CSmsPDU::ESmsSubmitReport) && (type!=CSmsPDU::ESmsDeliverReport))
		{
		//  Strip out spaces etc from address
		TGsmSmsTelNumber parsedaddress;
		aSmsMessage.ParsedToFromAddress(parsedaddress);
		aEntry.SetDescription2(parsedaddress.iTelNumber);
		}

	aEntry.SetTime(aSmsMessage.Time());
	} // CSmsReassemblyStore::PopulateEntry


void CSmsReassemblyStore::CreateEntryL(CSmsMessage& aSmsMessage,const CArrayFix<TInt>& aIndexArray,const CArrayFix<TGsmSms>& aSmsArray)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREASSEMBLYSTORE_CREATEENTRYL_1, "CSmsReassemblyStore::CreateEntryL");
 	TStreamId streamid=KNullStreamId;
 	if (aSmsMessage.Time() >= iLastRealTime)
 		{
 		iLastRealTime=aSmsMessage.Time();
 		if(iLastReceivedTime >= aSmsMessage.Time())
 			{
 			aSmsMessage.SetTime(iLastReceivedTime+(TTimeIntervalMicroSeconds32)1);
 			}
 		iLastReceivedTime=aSmsMessage.Time(); //provide uniqueness of time
 		}
 	else  // clock turned back
 		{
 		iLastReceivedTime=aSmsMessage.Time();
 		}
	BeginTransactionLC();
	ExternalizeEntryL(streamid,aSmsMessage,aIndexArray,aSmsArray);
	TSmsReassemblyEntry entry;
	entry.SetDataStreamId(streamid);

	PopulateEntry(entry,aSmsMessage,aSmsArray.Count());
	AddEntryL(entry);
	CommitTransactionL();
	} // CSmsReassemblyStore::CreateEntryL


TBool CSmsReassemblyStore::PassedToClient( TInt aIndex ) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREASSEMBLYSTORE_PASSEDTOCLIENT_1, "CSmsReassemblyStore::PassedToClient()");

	const TSmsReassemblyEntry& entry = reinterpret_cast<const TSmsReassemblyEntry&>(Entries()[ aIndex ]);
	return entry.PassedToClient();
	} // CSmsReassemblyStore::PassedToClient


void CSmsReassemblyStore::SetPassedToClientL(TInt aIndex, TBool aPassed)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREASSEMBLYSTORE_SETPASSEDTOCLIENTL_1, "CSmsReassemblyStore::SetPassedToClientL()");

	TSmsReassemblyEntry entry(reinterpret_cast<const TSmsReassemblyEntry&>(Entries()[aIndex]));

	const TBool alreadyPassed = entry.PassedToClient();

	if ((!aPassed && alreadyPassed) || (aPassed && !alreadyPassed))
		{
		entry.SetPassedToClient(aPassed);
		ChangeEntryL(aIndex, entry);
		}
	} // CSmsReassemblyStore::SetPassedToClientL


/**
 *  Open the sms reassembly store.
 */
void CSmsReassemblyStore::OpenStoreL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREASSEMBLYSTORE_OPENSTOREL_1, "CSmsReassemblyStore::OpenStoreL()");

	this->OpenL(iFullPathBuf,KReassemblyStoreUid);
	} // CSmsReassemblyStore::OpenStoreL


/**
 *  Constructor
 *  
 *  @param aFs Used to set CSARStore object
 */
CSmsReassemblyStore::CSmsReassemblyStore(RFs& aFs)
	:CSARStore(aFs)
	{
	// NOP
	} // CSmsReassemblyStore::CSmsReassemblyStore


CSmsSegmentationStore* CSmsSegmentationStore::NewL(RFs& aFs)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_NEWL_1, "CSmsSegmentationStore::NewL()");

	CSmsSegmentationStore* segmentationStore = new(ELeave) CSmsSegmentationStore(aFs);
	CleanupStack::PushL( segmentationStore );
	segmentationStore->ConstructL();
	CleanupStack::Pop( segmentationStore );
	return segmentationStore;
	} // CSmsSegmentationStore::NewL


void CSmsSegmentationStore::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_CONSTRUCTL_1, "CSmsSegmentationStore::ConstructL()");

	//generate fullpath of segmentation store.
	PrivatePath(iFullPathBuf);
	//append store name
	iFullPathBuf.Append(KSegmentationStoreName);
	
	// Set the default value for the maxmum number messages in the segmentation store.
	iMaxmumNumberOfMessagesInSegmentationStore = KDefaultMaxmumNumberOfMessagesInSegmentationStore;

	CESockIniData*  ini = NULL;
	_LIT(KSmseskfile, "smswap.sms.esk");
	TRAPD(ret, ini=CESockIniData::NewL(KSmseskfile));
	if(ret == KErrNone)
	    {
	    // Get the maximum number of messages allowed in the segmentation store from .ESK file
        CleanupStack::PushL(ini);
        
        TPtrC value;
        if((ini->FindVar(_L("SegmentationStoreOptions"),_L("MaxNumOfMessInSegStore"),value)))
            {
            TLex16 valueconv(value);
            valueconv.Val(iMaxmumNumberOfMessagesInSegmentationStore); 
            }
        CleanupStack::PopAndDestroy(ini);    
	    }
	else if (ret != KErrNotFound && ret != KErrPathNotFound)
	    {
	    User::Leave(ret);
	    }
	} // CSmsSegmentationStore::ConstructL


CSmsSegmentationStore::~CSmsSegmentationStore()
    {
    // NOP
    } // CSmsSegmentationStore::~CSmsSegmentationStore


TInt CSmsSegmentationStore::Next8BitReferenceL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_NEXT8BITREFERENCEL_1, "CSmsSegmentationStore::Next8BitReferenceL");

	TInt reference8bit=0;
	TInt reference16bit=0;
	TStreamId streamid=ExtraStreamId();

	//
	// access file store
	//
	BeginTransactionLC();
	if (streamid!=KNullStreamId)
		{
		TRAPD(ret,InternalizeConcatenationReferencesL(streamid,reference8bit,reference16bit));
		if(ret != KErrNone)
			{
			// We have to leave on any error; otherwise a duplicate reference number will be generated
			// The transaction will revert
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_NEXT8BITREFERENCEL_2, "WARNING! CSmsSegmentationStore::InternalizeConcatenationReferencesL left with %d", ret);
			User::Leave(ret);  //  stream not corrupted
			}
		reference8bit=(reference8bit+1)%0x100;
		}
	TRAPD(ret, ExternalizeConcatenationReferencesL(streamid,reference8bit,reference16bit));
	if(ret != KErrNone)
		{
		// We have to leave on any error; otherwise a duplicate reference number will be generated
		// The transaction will revert
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_NEXT8BITREFERENCEL_3, "WARNING! CSmsSegmentationStore::ExternalizeConcatenationReferencesL left with %d", ret);
		User::Leave(ret);  //  stream not corrupted
		}
	SetExtraStreamIdL(streamid);
	CommitTransactionL();
	return reference8bit;
	} // CSmsSegmentationStore::Next8BitReferenceL


TInt CSmsSegmentationStore::Next16BitReferenceL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_NEXT16BITREFERENCEL_1, "CSmsSegmentationStore::Next16BitReferenceL");
	TInt reference8bit=0;
	TInt reference16bit=0x100;
	TStreamId streamid=ExtraStreamId();
	//
	// access file store
	//
	BeginTransactionLC();
	if (streamid!=KNullStreamId)
		{
		TRAPD(ret,InternalizeConcatenationReferencesL(streamid,reference8bit,reference16bit));
		if(ret != KErrNone)
			{
			// We have to leave on any error; otherwise a duplicate reference number will be generated
			// The transaction will revert
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_NEXT16BITREFERENCEL_2, "WARNING! CSmsSegmentationStore::InternalizeConcatenationReferencesL left with %d", ret);
			User::Leave(ret);  //  stream not corrupted
			}
		reference16bit=((reference16bit+1)%0xFF00)+0x100;
		}
	TRAPD(ret, ExternalizeConcatenationReferencesL(streamid,reference8bit,reference16bit));
	if(ret != KErrNone)
		{
		// We have to leave on any error; otherwise a duplicate reference number will be generated
		// The transaction will revert
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_NEXT16BITREFERENCEL_3, "WARNING! CSmsSegmentationStore::ExternalizeConcatenationReferencesL left with %d", ret);
		User::Leave(ret);  //  stream not corrupted
		}
	SetExtraStreamIdL(streamid);
	CommitTransactionL();
	return reference16bit;
	} // CSmsSegmentationStore::Next16BitReferenceL


/**
 *  Adds a CSmsMessage to the segmentation store
 *  
 *  @note aSumbit.Buffer() may be shortened to TSAREntry::ESmsSAREntryDescriptionLength
 *  
 *  @param aSubmit Message to add to the segmentation store
 *  @pre aSubmit.Type() is ESmsSubmit
 *  @pre aSubmit.EncodeMessagePdusL() has been called. This is so PopulateEntry sets the correct total on the TSAREntry
 */
void CSmsSegmentationStore::AddSubmitL(const TSmsAddr& aSmsAddr,CSmsMessage& aSubmit)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_ADDSUBMITL_1, "CSmsSegmentationStore::AddSubmitL");

	__ASSERT_ALWAYS(aSubmit.Type()==CSmsPDU::ESmsSubmit,SmspPanic(KSmspPanicNotSubmit));

	BeginTransactionLC();


	RSmsSegmentationStoreRefStatusArray refStatus;
	CleanupClosePushL(refStatus);

	TStreamId streamid=KNullStreamId;
	CSmsBufferBase& buffer=aSubmit.Buffer();
	TInt length=buffer.Length();
	if (length>TSAREntry::ESmsSAREntryDescriptionLength)
		buffer.DeleteL(TSAREntry::ESmsSAREntryDescriptionLength,length-TSAREntry::ESmsSAREntryDescriptionLength);

	ExternalizeEntryL(streamid,aSmsAddr,aSubmit, refStatus);

	TSmsSegmentationEntry entry;
	entry.SetDataStreamId(streamid);
	PopulateEntry(entry, aSubmit, refStatus);

	CleanupStack::PopAndDestroy(&refStatus);

	AddEntryL(entry);
	CommitTransactionL();
	} // CSmsSegmentationStore::AddSubmitL


TBool CSmsSegmentationStore::AddCommandL(const TSmsAddr& aSmsAddr,const CSmsMessage& aCommand, CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray& aRefStatus)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_ADDCOMMANDL_1, "CSmsSegmentationStore::AddCommandL");
	__ASSERT_ALWAYS(aCommand.Type()==CSmsPDU::ESmsCommand,SmspPanic(KSmspPanicNotCommand));
	const TInt count=Entries().Count();
	const TLogId logid=(TLogId) aCommand.LogServerId();

	BeginTransactionLC();

	for (TInt i=count-1; i>=0; --i)
		{
		if ((logid!=KLogNullId) && (logid==Entries()[i].LogServerId()))
			{
			DeleteEntryL(i);
			break;
			}
		}
	TBool found=EFalse;

	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsmessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,buffer);
	CleanupStack::PushL(smsmessage);
	TGsmSmsTelNumber parsedaddress;
	aCommand.ParsedToFromAddress(parsedaddress);
	TInt telLen;

	for (TInt j=0; j<count; j++)
		{
		TSmsSegmentationEntry entry = (TSmsSegmentationEntry&)Entries()[j];
		const CSmsPDU::TSmsPDUType type = entry.PduType();
		telLen=Min(entry.Description2().Length(),parsedaddress.iTelNumber.Length());
		if ((type==CSmsPDU::ESmsSubmit) &&
			 entry.IsComplete() &&
		    (entry.Description2().Right(telLen)==parsedaddress.iTelNumber.Right(telLen)) &&
		    (entry.Time()==aCommand.Time()))
			{
			found=ETrue;
			TSmsAddr smsaddr;

			InternalizeEntryL(entry.DataStreamId(),smsaddr,*smsmessage, aRefStatus);
			TStreamId streamid;
			CSmsSubmit& submit=(CSmsSubmit&) smsmessage->SmsPDU();
			if ((((CSmsCommand&) aCommand.SmsPDU()).CommandType()==TSmsCommandType::ESmsCommandTypeEnableStatusReportRequest) &&
			     (!submit.StatusReportRequest()))
				{
				submit.SetStatusReportRequest(ETrue);
				streamid=entry.DataStreamId();
				ExternalizeEntryL(streamid,smsaddr,*smsmessage, aRefStatus);
				PopulateEntry(entry,*smsmessage, aRefStatus);
				ChangeEntryL(j,entry);
				}

			RSmsSegmentationStoreRefStatusArray refStatusTemp;
			CleanupClosePushL(refStatusTemp);

			streamid=KNullStreamId;

			ExternalizeEntryL(streamid,aSmsAddr,aCommand, refStatusTemp);
			entry.SetDataStreamId(streamid);
			PopulateEntry(entry,aCommand, refStatusTemp);

			CleanupStack::PopAndDestroy(&refStatusTemp);

			AddEntryL(entry);

			break;
			}
		}
	CleanupStack::PopAndDestroy(smsmessage);  // smsmessage
	CommitTransactionL();

	return found;
	} // CSmsSegmentationStore::AddCommandL


TBool CSmsSegmentationStore::AddReferenceL(const CSmsMessage& aSmsMessage,TInt aReference)
	{
	const TInt count=Entries().Count();
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_ADDREFERENCEL_1, "CSmsSegmentationStore::AddReferenceL [count=%d, ref=%d]", count, aReference);
	TInt i=0;
	TInt logserverid=aSmsMessage.LogServerId();
	if (logserverid!=KLogNullId)
		{
		for (i=0; i<count; i++)
			{
            if (logserverid==((TSmsSegmentationEntry&)Entries()[i]).LogServerId())
				break;
			}
		}
	else
		{
		TGsmSmsTelNumber parsedaddress;
		aSmsMessage.ParsedToFromAddress(parsedaddress);
		TInt telLen;
		for (i=0; i<count; i++)
			{
            TSAREntry tsareenty = Entries()[i];      
            TSmsSegmentationEntry& entry = static_cast<TSmsSegmentationEntry&>(tsareenty);        
            telLen=Min(entry.Description2().Length(),parsedaddress.iTelNumber.Length());
            const CSmsPDU::TSmsPDUType type=entry.PduType();
            if ((type==aSmsMessage.Type()) && (!entry.IsComplete()) && (aSmsMessage.Time()==entry.Time()) && (entry.Description2().Right(telLen)==parsedaddress.iTelNumber.Right(telLen)))
                    break;
                }
            }
	if(i>=count)
		{
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_ADDREFERENCEL_2, "WARNING! KSmspPanicEntryWithLogServerIdNotFound [i=%d, count=%d]", i, count);
		return EFalse;
		}

	RSmsSegmentationStoreRefStatusArray refStatusArray;
	CleanupClosePushL(refStatusArray);

	TSAREntry tsareenty = Entries()[i];      
	TSmsSegmentationEntry& entry = static_cast<TSmsSegmentationEntry&>(tsareenty);        
	TStreamId streamid=entry.DataStreamId();
	TSmsAddr smsaddr;
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsmessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver,buffer);
	CleanupStack::PushL(smsmessage);

	//
	// access the file store
	//
	InternalizeEntryL(streamid,smsaddr,*smsmessage, refStatusArray);
	refStatusArray.InsertL(aReference);

	BeginTransactionLC();
	ExternalizeEntryL(streamid,smsaddr,*smsmessage, refStatusArray);

	PopulateEntry(entry,/*smsaddr,*/*smsmessage, refStatusArray);
	ChangeEntryL(i,entry);
	CommitTransactionL();

	//
	// AEH: moved here because if is done before calling ChangeEntryL
	//      it will pop and destroy the filestore which is also on
	//      the cleanup stack
	//
	CleanupStack::PopAndDestroy(2);  //  smsmessage, refStatus

	return entry.Total()==entry.Count();
	}

	
/**
 *  Does exactly the same thing as AddReferenceL() i.e. adds the the segment refernce to a list. But 
 *  to support the new status report schemes a slight change has been made. Instead of inserting
 *  just a reference now its status is inserted as well. This is provided one of the two new schemes
 *  is being used. If the status is required then we do exactly the same as AddReferenceL(), but if
 *  it's not then we just call the InsertL() method with an extra parameter: EStatusComplete.
 *  
 *  @param aSmsMessage Reference to CSmsMessage.
 *  @param aReference  The PDU reference.
 */
TBool CSmsSegmentationStore::AddReferenceStatusPairL(const CSmsMessage& aSmsMessage,TInt aReference, TUint aSegmentSequenceNumber)
 	{
	const TInt count=Entries().Count();
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_ADDREFERENCESTATUSPAIRL_1, "CSmsSegmentationStore::AddReferenceStatusPairL [count=%d, ref=%d]", count, aReference);
	TInt i=0;
	TInt logserverid=aSmsMessage.LogServerId();
	if (logserverid!=KLogNullId)
		{
		for (i=0; i<count; i++)
			{
            if (logserverid==((TSmsSegmentationEntry&)Entries()[i]).LogServerId())
				break;
			}
		}
	else
		{
		TGsmSmsTelNumber parsedaddress;
		aSmsMessage.ParsedToFromAddress(parsedaddress);
		TInt telLen;
		for (i=0; i<count; i++)
			{
            TSAREntry tsareenty = Entries()[i];  
            TSmsSegmentationEntry& entry = static_cast<TSmsSegmentationEntry&>(tsareenty);
			telLen=Min(entry.Description2().Length(),parsedaddress.iTelNumber.Length());
			const CSmsPDU::TSmsPDUType type=entry.PduType();
			if ((type==aSmsMessage.Type()) && (!entry.IsComplete()) && (aSmsMessage.Time()==entry.Time()) && (entry.Description2().Right(telLen)==parsedaddress.iTelNumber.Right(telLen)))
				break;
			}
		}
	if(i>=count)
		{
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_ADDREFERENCESTATUSPAIRL_2, "WARNING! KSmspPanicEntryWithLogServerIdNotFound [i=%d, count=%d]", i, count);
		return EFalse;
		}

	RSmsSegmentationStoreRefStatusArray refStatusArray;
	CleanupClosePushL(refStatusArray);
    TSAREntry tsareenty = Entries()[i];  
    TSmsSegmentationEntry& entry = static_cast<TSmsSegmentationEntry&>(tsareenty);

	TStreamId streamid=entry.DataStreamId();
	TSmsAddr smsaddr;
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsmessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver,buffer);
	CleanupStack::PushL(smsmessage);

	//
	// access the file store
	//
	InternalizeEntryL(streamid,smsaddr,*smsmessage, refStatusArray);
	
	if (aSmsMessage.Scheme() == EControlParametersScheme)
		{
		TUint8 octet(0);
		TInt  ret;
		
		ret = ((CSmsSMSCCtrlParameterOperations&)aSmsMessage.GetOperationsForIEL(CSmsInformationElement::ESmsIEISMSCControlParameters)).GetStatusReport(aSegmentSequenceNumber, octet);
		if (ret == KErrNone)
			{
			if (octet & ESmsSMSCControlParametersMask)
				{
				refStatusArray.InsertL(aReference);	
				}
			else
				{
				refStatusArray.InsertL(TSmsSegmentationStoreRefStatus(aReference, EStatusComplete));	
				}
			}
		}
	else if(aSmsMessage.Scheme() == ETPSRRScheme)
		{
		TInt tpsrr;
		tpsrr =  ((CSmsTPSRROperations&)aSmsMessage.GetOperationsForNonIEL(ESmsTPSRRParameter)).GetStatusReport(aSegmentSequenceNumber);
		
		if(tpsrr == TSmsFirstOctet::ESmsStatusReportNotRequested)
			{
			refStatusArray.InsertL(TSmsSegmentationStoreRefStatus(aReference, EStatusComplete));
			}
		else if(tpsrr == TSmsFirstOctet::ESmsStatusReportRequested)
			{
			refStatusArray.InsertL(aReference);	
			}	
		}
	else
		{
		User::Leave(KErrArgument);
		}
	

	BeginTransactionLC();
	ExternalizeEntryL(streamid,smsaddr,*smsmessage, refStatusArray);

	PopulateEntry(entry,/*smsaddr,*/*smsmessage, refStatusArray);
	ChangeEntryL(i,entry);
	CommitTransactionL();

	//
	// AEH: moved here because if is done before calling ChangeEntryL
	//      it will pop and destroy the filestore which is also on
	//      the cleanup stack
	//
	CleanupStack::PopAndDestroy(2);  //  smsmessage, refStatus

	return entry.Total()==entry.Count();
	} // CSmsSegmentationStore::AddReferenceStatusPairL


TBool CSmsSegmentationStore::AddStatusReportL(TInt& aIndex,TBool& aComplete,const CSmsMessage& aStatusReport)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_ADDSTATUSREPORTL_1, "CSmsSegmentationStore::AddStatusReportL");

	__ASSERT_DEBUG(aStatusReport.Type()==CSmsPDU::ESmsStatusReport,SmspPanic(KSmspPanicNotStatusReport));

	const CSmsStatusReport& statusreport=(CSmsStatusReport&) aStatusReport.SmsPDU();
	const TInt reference=statusreport.MessageReference();
	const TInt status=statusreport.Status();
	const TInt isPerm = IsPermanentStatus(status);
	const TSmsFirstOctet::TSmsStatusReportQualifier qualifier=statusreport.StatusReportQualifier();
	TBool found=EFalse;
	aComplete=EFalse;

	OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_ADDSTATUSREPORTL_2, "CSmsSegmentationStore::AddStatusReportL [ref=%d status=%d IsPerm=%d]", reference, status, isPerm);

	if(!isPerm)
		{
		return EFalse;
		}

	RSmsSegmentationStoreRefStatusArray refStatusArray;
	CleanupClosePushL(refStatusArray);

	const TInt count1=Entries().Count();

	TSmsAddr smsaddr;
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsmessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver,buffer);
	CleanupStack::PushL(smsmessage);
	TGsmSmsTelNumber parsedaddress;
	aStatusReport.ParsedToFromAddress(parsedaddress);

	BeginTransactionLC();

	aIndex = count1;

	TInt telLen;
	while (!found && aIndex--)
		{
        TSAREntry tsareenty = Entries()[aIndex]; 
        TSmsSegmentationEntry& entry = static_cast<TSmsSegmentationEntry&>(tsareenty);  

		// Remove leading zeros of national numbers
		TPtrC trimmedTelNumber(TrimLeadingZeros(entry.Description2()));
		TPtrC trimmedParsedTelNumber(TrimLeadingZeros(parsedaddress.iTelNumber));

		telLen=Min(trimmedTelNumber.Length(),trimmedParsedTelNumber.Length());

		const CSmsPDU::TSmsPDUType type = entry.PduType();
		const TInt startref = entry.Reference1();
		const TInt stopref = entry.Reference2();

		TBool sameTelNumbers = entry.Description2().Right(telLen) == parsedaddress.iTelNumber.Right(telLen);
		
		if (sameTelNumbers)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_ADDSTATUSREPORTL_3, "CSmsSegmentationStore::AddStatusReportL telNumber from submit report matches that from SMS message");
			}
		else
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_ADDSTATUSREPORTL_4, "CSmsSegmentationStore::AddStatusReportL telNumber from submit report does NOT match that from SMS message");
			}

		if (sameTelNumbers &&
		  (((qualifier==TSmsFirstOctet::ESmsStatusReportResultOfCommand) && (type==CSmsPDU::ESmsCommand)) ||
		   ((qualifier==TSmsFirstOctet::ESmsStatusReportResultOfSubmit) &&  (type==CSmsPDU::ESmsSubmit))) &&
		   (((stopref>=startref) &&(reference>=startref) && (reference<=stopref))||((stopref<startref) &&((reference>=startref) || (reference<=stopref))))
		   )
			{
			InternalizeEntryL(entry.DataStreamId(),smsaddr,*smsmessage, refStatusArray);
			TInt refStatusPos = refStatusArray.Find(reference); //assumes Find returns the first matching reference in the array
			TInt numMessagePDUs=entry.Total();

			if (refStatusPos != KErrNotFound)
				{
				const TInt refStatusArrayCount = refStatusArray.Count();

				//Find an element in refStatusArray where Reference() == reference and Status() is not permanent
				while (!found && refStatusPos < refStatusArrayCount && refStatusArray[refStatusPos].Reference() == reference)
					{
					//@note This loop assumes refStatusArray is sorted iReference
					if (!IsPermanentStatus(refStatusArray[refStatusPos].Status())&&(refStatusArrayCount <= numMessagePDUs))
						{
						found = ETrue;
						}
					else
						{
						OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_ADDSTATUSREPORTL_5, "CSmsSegmentationStore::AddStatusReportL WARNING: Status already perm [status=%d refStatusPos=%d count=%d]", refStatusArray[refStatusPos].Status(), refStatusPos, refStatusArrayCount);
						refStatusPos++;
						}
					}

				if (found)
					{
					OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_ADDSTATUSREPORTL_6, "CSmsSegmentationStore::AddStatusReportL Found [refStatusPos=%d]", refStatusPos);
					refStatusArray[refStatusPos].SetStatus(status);
					TStreamId streamid=entry.DataStreamId();
					ExternalizeEntryL(streamid,smsaddr,*smsmessage, refStatusArray);
					PopulateEntry(entry,*smsmessage, refStatusArray);
					ChangeEntryL(aIndex,entry);
					aComplete=StatusArrayComplete(refStatusArray, entry);
				    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_ADDSTATUSREPORTL_7, "CSmsSegmentationStore::AddStatusReportL StatusArrayComplete %d", aComplete);
					}
				}
			}
		}

	if (found && (smsmessage->Type()==CSmsPDU::ESmsCommand))  // look for original submit
		{
		TTime time=smsmessage->Time();
		found=EFalse;

		RSmsSegmentationStoreRefStatusArray refStatusArray2;
		CleanupClosePushL(refStatusArray2);
		refStatusArray2.CopyL(refStatusArray);
		refStatusArray2.ResetAllStatus();

		aComplete=EFalse;
		TInt telLen;
		for (aIndex=0; aIndex<count1; aIndex++)
			{
            TSAREntry tsareenty = Entries()[aIndex]; 
	        TSmsSegmentationEntry& entry = static_cast<TSmsSegmentationEntry&>(tsareenty);  
			telLen=Min(entry.Description2().Length(),parsedaddress.iTelNumber.Length());
			const CSmsPDU::TSmsPDUType type = entry.PduType();
			if ((type==CSmsPDU::ESmsSubmit) &&
				 entry.IsComplete() &&
			    (entry.Time()==time) &&
			    (entry.Description2().Right(telLen)==parsedaddress.iTelNumber.Right(telLen)))
				{

				/*
				TODO ahe - this should take out of the for loop
				only set/delete flags in the for loop and let the methods:
				InternalizeXXX
				ExternalizeXXX
				PopulateEntry
				ChangeEntry
				... all ... other big CSarStore methods
				have only to run once -> internal loop in this function through
				the array of flags
				*/

				found=ETrue;
				InternalizeEntryL(entry.DataStreamId(),smsaddr,*smsmessage, refStatusArray2);
				const TInt count2 = refStatusArray.Count();
				__ASSERT_DEBUG(count2 == refStatusArray2.Count(),SmspPanic(KSmspPanicBadReferenceArray));
				for (TInt i=0; i<count2; i++)
					{
					TSmsSegmentationStoreRefStatus& refStatus2 = refStatusArray2[i];
					if (!IsPermanentStatus(refStatus2.Status()))
						{
						refStatus2.SetStatus(refStatusArray[i].Status());
						}
					}

				TStreamId streamid=entry.DataStreamId();
				ExternalizeEntryL(streamid,smsaddr,*smsmessage, refStatusArray2);
				PopulateEntry(entry,/*smsaddr,*/*smsmessage, refStatusArray2);
				ChangeEntryL(aIndex,entry);
				aComplete=StatusArrayComplete(refStatusArray2, entry);
			    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_ADDSTATUSREPORTL_8, "CSmsSegmentationStore::StatusArrayComplete [aStatus=%d, ret=%d]", status, aComplete);
				break;
				}
			}

		CleanupStack::PopAndDestroy(&refStatusArray2);
		}

	CommitTransactionL();
	CleanupStack::PopAndDestroy(2); // smsmessage, refStatusArray

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_ADDSTATUSREPORTL_9, "CSmsSegmentationStore::AddStatusReportL Exit [found=%d]", found);
	return found;
	} // CSmsSegmentationStore::AddStatusReportL


void CSmsSegmentationStore::GetMessageL(TInt aIndex,TSmsAddr& aSmsAddr,CSmsMessage& aSmsMessage, RSmsSegmentationStoreRefStatusArray& aRefStatusArray)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_GETMESSAGEL_1, "CSmsSegmentationStore::GetMessageL [aIndex=%d]", aIndex);

	InternalizeEntryL(Entries()[aIndex].DataStreamId(),aSmsAddr,aSmsMessage, aRefStatusArray);
	} // CSmsSegmentationStore::GetMessageL


/**
 *  internalize the concat refs from the permanent file store to internal memory
 *  
 *  @note You have to call CSARStore::OpenFileLC() before calling this function
 *  and CSARStore::CloseFile() after.
 */
void CSmsSegmentationStore::InternalizeConcatenationReferencesL(const TStreamId& aStreamId,TInt& aReference8bit,TInt& aReference16bit)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_INTERNALIZECONCATENATIONREFERENCESL_1, "CSmsSegmentationStore::InternalizeConcatenationReferencesL Start");
	RStoreReadStream readstream;
	readstream.OpenLC(FileStore(),aStreamId);
	aReference8bit=readstream.ReadInt32L();
	aReference16bit=readstream.ReadInt32L();
	CleanupStack::PopAndDestroy();
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_INTERNALIZECONCATENATIONREFERENCESL_2, "CSmsSegmentationStore::InternalizeConcatenationReferencesL End");
	} // CSmsSegmentationStore::InternalizeConcatenationReferencesL


/**
 *  externalize the concat refs from the permanent file store to internal memory
 *  
 *  @note You have to call CSARStore::OpenFileLC() before calling this function
 *  and CSARStore::CloseFile() after.
 */
void CSmsSegmentationStore::ExternalizeConcatenationReferencesL(TStreamId& aStreamId,TInt aReference8bit,TInt aReference16bit)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_EXTERNALIZECONCATENATIONREFERENCESL_1, "CSmsSegmentationStore::ExternalizeConcatenationReferencesL Start");
	RStoreWriteStream writestream;
	if (aStreamId==KNullStreamId)
		aStreamId=writestream.CreateLC(FileStore());
	else
		writestream.ReplaceLC(FileStore(),aStreamId);
	writestream.WriteInt32L(aReference8bit);
	writestream.WriteInt32L(aReference16bit);
	writestream.CommitL();
	CleanupStack::PopAndDestroy();
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_EXTERNALIZECONCATENATIONREFERENCESL_2, "CSmsSegmentationStore::ExternalizeConcatenationReferencesL End");
	} // CSmsSegmentationStore::ExternalizeConcatenationReferencesL


/**
 *  internalize all the entries from the permanent file store to internal memory
 *  
 *  @note You have to call CSARStore::OpenFileLC() before calling this function
 *  and CSARStore::CloseFile() after.
 */
void CSmsSegmentationStore::InternalizeEntryL(const TStreamId& aStreamId,TSmsAddr& aSmsAddr,CSmsMessage& aSmsMessage, RSmsSegmentationStoreRefStatusArray& aRefStatusArray)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_INTERNALIZEENTRYL_1, "CSmsSegmentationStore::InternalizeEntryL Start");

	aRefStatusArray.Reset();

	RStoreReadStream readstream;
	readstream.OpenLC(FileStore(),aStreamId);
	readstream >> aSmsAddr;
	readstream >> aSmsMessage;
	readstream >> aRefStatusArray;
	CleanupStack::PopAndDestroy(&readstream);

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_INTERNALIZEENTRYL_2, "CSmsSegmentationStore::InternalizeEntryL End [count=%d]", aRefStatusArray.Count());
	} // CSmsSegmentationStore::InternalizeEntryL


/**
 *  externalizes all the entries from the internal memory to the permanent file store
 */
void CSmsSegmentationStore::ExternalizeEntryL(TStreamId& aStreamId,const TSmsAddr& aSmsAddr,const CSmsMessage& aSmsMessage, const RSmsSegmentationStoreRefStatusArray& aRefStatusArray)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_EXTERNALIZEENTRYL_1, "CSmsSegmentationStore::ExternalizeEntryL Start");

	RStoreWriteStream writestream;

	if (aStreamId==KNullStreamId)
		aStreamId=writestream.CreateLC(FileStore());
	else
		writestream.ReplaceLC(FileStore(),aStreamId);

	writestream << aSmsAddr;
	writestream << aSmsMessage;
	writestream << aRefStatusArray;
	writestream.CommitL();
	CleanupStack::PopAndDestroy(&writestream);

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_EXTERNALIZEENTRYL_2, "CSmsSegmentationStore::ExternalizeEntryL End [count=%d]", aRefStatusArray.Count());
	} // CSmsSegmentationStore::ExternalizeEntryL


/**
 *  Populates an SMS message into SAR store entry
 *  
 *  @pre aSmsMessage.EncodeMessagePdusL() has been called
 *  
 *  @param aEntry          Entry to be populated to
 *  @param aSmsMessage     SMS message to be populated from
 *  @param aReferenceArray Array containing references
 *  @param aStatusArray    Array containing status
 */
void CSmsSegmentationStore::PopulateEntry(TSmsSegmentationEntry& aEntry,
					  const CSmsMessage& aSmsMessage,
					  const RSmsSegmentationStoreRefStatusArray& aRefStatusArray)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_POPULATEENTRY_1, "CSmsSegmentationStore::PopulateEntry");
	TBool statusreportrequest=EFalse;
	if (aSmsMessage.Type()==CSmsPDU::ESmsSubmit)
		{
		aEntry.SetReference(0);
		aEntry.SetTotal(1);
		CSmsSubmit& submit=(CSmsSubmit&) aSmsMessage.SmsPDU();
		aEntry.SetValidityPeriod(submit.ValidityPeriod().Int());
		
		if (aSmsMessage.Scheme() == EDefaultScheme)
		    {
	    	statusreportrequest=((CSmsSubmit&) aSmsMessage.SmsPDU()).StatusReportRequest();		    
		    }
		else
		    {
		    statusreportrequest = ETrue;    
		    }
		}
	else
		{
		statusreportrequest=((CSmsCommand&) aSmsMessage.SmsPDU()).StatusReportRequest();
		}

	if (aSmsMessage.TextPresent())
		{
		if (aSmsMessage.SmsPDU().TextConcatenated())
			{
			aEntry.SetReference(aSmsMessage.SmsPDU().ConcatenatedMessageReference());

			//
			// aSmsMessage.EncodeMessagePdusL() must have been called before this point,
			// otherwise aSmsMessage.SmsPDU().NumConcatenatedMessagePDUs() will return 1.
			//
			aEntry.SetTotal(aSmsMessage.SmsPDU().NumConcatenatedMessagePDUs());
			}
		}

	aEntry.SetLogServerId(aSmsMessage.LogServerId());
	//  Strip out spaces etc from address
	TGsmSmsTelNumber parsedaddress;
	aSmsMessage.ParsedToFromAddress(parsedaddress);
	aEntry.SetDescription2(parsedaddress.iTelNumber);
	aEntry.SetTime(aSmsMessage.Time());

	const TInt count= aRefStatusArray.Count();
	__ASSERT_DEBUG((count>=0) && (count<=aEntry.Total()),SmspPanic(KSmspPanicBadReferenceArray));
	aEntry.SetCount(count);
	TInt reference1=0xFF;
	TInt reference2=0x00;
	TInt startref=reference1;
	TInt stopref=reference2;

	if(count>0 && statusreportrequest)
	{
		startref=aRefStatusArray[0].Reference();
		stopref=aRefStatusArray[count-1].Reference();
	}

	TInt delivered=0;
	TInt failed=0;
	for (TInt i=0; i<count; i++)
		{
		const TSmsSegmentationStoreRefStatus& refStatus = aRefStatusArray[i];

		if (refStatus.Status() == TSmsStatus::ESmsShortMessageReceivedBySME)
			delivered++;
		else if (IsPermanentStatus(refStatus.Status()))
			failed++;
		}

	//
	// AEH: Defect fix for EDNPAHN-4WADW3 'Unreliable logging'
	//
	//      a little hack here to store information about whether
	//      we need Status Report or not, in the TSAREntry. This
	//      is because we want to retrieve it later in PurgeL.
	//      An extra bit is added iData4, LSB of byte 3. See gsmustor.h
	//      for more documentation.
	//
	aEntry.SetDeliveredAndFailed(delivered, failed);	
	TBool have_sr = EFalse;

	if (aSmsMessage.Scheme() == EDefaultScheme)
		    {
	    	have_sr=((CSmsSubmit&) aSmsMessage.SmsPDU()).StatusReportRequest();		    
		    }
	else
		    {
		    have_sr = ETrue;    
		    }

	aEntry.SetPduTypeAndRefs(have_sr, aSmsMessage.Type(), startref, stopref);
	} // CSmsSegmentationStore::PopulateEntry


/**
 *  Returns ETrue if the status array is complete
 *  
 *  @param aStatusArray    Array containing status
 *  @param aEntry          SAR Entry
 */
TBool CSmsSegmentationStore::StatusArrayComplete(const RSmsSegmentationStoreRefStatusArray& aRefStatusArray, TSAREntry& aEntry)
	{
	TInt permanent=0;
	const TInt count= aRefStatusArray.Count();
	for (TInt i=0; i<count; i++)
		{
		const TBool ret = IsPermanentStatus(aRefStatusArray[i].Status());
		OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_STATUSARRAYCOMPLETE_1, "CSmsSegmentationStore::IsPermanentStatus [Status: %d, RetVal: %d, count=%d]", aRefStatusArray[i].Status(), ret, count);
		if (ret)
			permanent++;
		}
	return (permanent==count) && (permanent==aEntry.Total() );
	} // CSmsSegmentationStore::StatusArrayComplete


  /**
   *  C'tor
   */
CSmsSegmentationStore::CSmsSegmentationStore(RFs& aFs)
    :CSARStore(aFs)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_CTOR_1, "CSmsSegmentationStore::CSmsSegmentationStore()");

    } // CSmsSegmentationStore::CSmsSegmentationStore


TInt CSmsSegmentationStore::TSmsSegmentationStoreRefStatus::Compare(const TSmsSegmentationStoreRefStatus& aLeft, const TSmsSegmentationStoreRefStatus& aRight)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_TSMSSEGMENTATIONSTOREREFSTATUS_COMPARE_1, "CSmsSegmentationStore::TSmsSegmentationStoreRefStatus::Compare()");

	return aLeft.iReference - aRight.iReference;
	} // CSmsSegmentationStore::TSmsSegmentationStoreRefStatus::Compare


void CSmsSegmentationStore::TSmsSegmentationStoreRefStatus::InternalizeL(RReadStream& aStream)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_TSMSSEGMENTATIONSTOREREFSTATUS_INTERNALIZEL_1, "CSmsSegmentationStore::TSmsSegmentationStoreRefStatus::InternalizeL()");

	iReference = aStream.ReadInt32L();
	iStatus = aStream.ReadInt32L();
	} // CSmsSegmentationStore::TSmsSegmentationStoreRefStatus::InternalizeL


void CSmsSegmentationStore::TSmsSegmentationStoreRefStatus::ExternalizeL(RWriteStream& aStream) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_TSMSSEGMENTATIONSTOREREFSTATUS_EXTERNALIZEL_1, "CSmsSegmentationStore::TSmsSegmentationStoreRefStatus::ExternalizeL()");

	aStream.WriteInt32L(iReference);
	aStream.WriteInt32L(iStatus);
	} // CSmsSegmentationStore::TSmsSegmentationStoreRefStatus::ExternalizeL


void CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray::InsertL(const TSmsSegmentationStoreRefStatus& aRefStatus)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_RSMSSEGMENTATIONSTOREREFSTATUSARRAY_INSERTL_1, "CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray::InsertL()");

	TLinearOrder<TSmsSegmentationStoreRefStatus> order(TSmsSegmentationStoreRefStatus::Compare);
	User::LeaveIfError(InsertInOrderAllowRepeats(aRefStatus, order));
	} // CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray::InsertL


TInt CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray::Find(const TSmsSegmentationStoreRefStatus& aRefStatus) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_RSMSSEGMENTATIONSTOREREFSTATUSARRAY_FIND_1, "CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray::Find()");

	TLinearOrder<TSmsSegmentationStoreRefStatus> order(TSmsSegmentationStoreRefStatus::Compare);
	TInt index = FindInOrder(aRefStatus, order);
	if (index != KErrNotFound)
		{
        //The  function is to return  the first occurence.  However FindInOrder()
        //uses a binary search algorithm and does not guarantee to return the 1st item if there are duplicate items.
        //Therefore we manually check for duplicates to the left of the found item.
        while (index > 0 && (operator[](index-1).Reference() == aRefStatus.Reference()))
			{
			--index;
			}
		}
	return index;
	} // CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray::Find


void CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray::CopyL(const RSmsSegmentationStoreRefStatusArray& aOther)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_RSMSSEGMENTATIONSTOREREFSTATUSARRAY_COPYL_1, "CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray::CopyL()");

	Reset();

	TInt count = aOther.Count();
	while (count--)
		{
		InsertL(aOther[count]);
		}
	} // CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray::CopyL


void CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray::ResetAllStatus(TInt aStatus)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_RSMSSEGMENTATIONSTOREREFSTATUSARRAY_RESETALLSTATUS_1, "CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray::ResetAllStatus()");

	TInt count = Count();
	while (count--)
		{
		(*this)[count].SetStatus(aStatus);
		}
	} // CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray::ResetAllStatus


void CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray::InternalizeL(RReadStream& aStream)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_RSMSSEGMENTATIONSTOREREFSTATUSARRAY_INTERNALIZEL_1, "CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray::InternalizeL()");

	TInt count = aStream.ReadInt32L();
	while (count--)
		{
		TSmsSegmentationStoreRefStatus refStatus;
		aStream >> refStatus;
		InsertL(refStatus); //maintain order
		}
	} // CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray::InternalizeL


void CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray::ExternalizeL(RWriteStream& aStream) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_RSMSSEGMENTATIONSTOREREFSTATUSARRAYEXTERNALIZEL_1, "CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray::ExternalizeL()");

	const TInt count = Count();
	aStream.WriteInt32L(count);

	for (TInt i = 0; i < count; i++)
		{
		aStream << (*this)[i];
		}
	} // CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray::ExternalizeL


TBool CSmsSegmentationStore::HasEntryWithLogIdL(TLogId aLogID,TInt& aRefNo,TInt& aSent)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_HASENTRYWITHLOGIDL_1, "CSmsSegmentationStore::HasEntryWithLogIdL()");

	TInt count=Entries().Count();
	TBool found=EFalse;
	if(aLogID != KLogNullId)
		{
		TInt total;
		TInt sent;
		BeginTransactionLC();
		for (TInt i=count-1; i>=0; --i)
			{
			if (aLogID==Entries()[i].LogServerId())
				{
				const TSAREntry& entry=Entries()[i];
				total=entry.Total();
				sent=entry.Count();
				if( sent < total)
					{
					aSent=sent;
					aRefNo=entry.Reference();
					found=ETrue;
					}
				else
					{
					DeleteEntryL(i);
					OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_HASENTRYWITHLOGIDL_2, "CSmsSegmentationStore::HasEntryWithLogIdL [Entry: %d LogId %d - deleted]", i, (TInt)aLogID );
					}
				break;
				}
			}
		CommitTransactionL();
		}
	return found;
} // CSmsSegmentationStore::HasEntryWithLogIdL


/**
 *  Open the sms segmentation store.
 */
void CSmsSegmentationStore::OpenStoreL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSEGMENTATIONSTORE_OPENSTOREL_1, "CSmsSegmentationStore::OpenStoreL()");

	this->OpenL(iFullPathBuf,KSegmentationStoreUid);
	} // CSmsSegmentationStore::OpenStoreL
