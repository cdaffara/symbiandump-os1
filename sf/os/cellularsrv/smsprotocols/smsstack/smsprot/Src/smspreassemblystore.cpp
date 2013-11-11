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
//


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "smspreassemblystoreTraces.h"
#endif

#include "smsstacklog.h"
#include "gsmubuf.h"
#include "smspreassemblystore.h"

/**
 *  Populate entry information from sms message.
 *  @param aEntry fills up re-assembly entry information from aSmsMessage.
 *  @param aSmsMessage refernce to sms message.
 *	@param aNumSmss number of sms.
 */
void CReassemblyStoreUtility::PopulateEntry(TSmsReassemblyEntry& aEntry,const CSmsMessage& aSmsMessage,TInt aNumSmss)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CREASSEMBLYSTOREUTILITY_POPULATEENTRY_1, "CReassemblyStoreUtility::PopulateEntry");
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
	}

/**
 *  Returns the private path of the component.
 *  @param aFs File Server handle.
 *  @param aPath (retrurns) private path of the component.
 */
void CReassemblyStoreUtility::PrivatePath(RFs& aFs, TDes& aPath)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CREASSEMBLYSTOREUTILITY_PRIVATEPATH_1, "CReassemblyStoreUtility::PrivatePath()");

	TDriveUnit driveUnit(KStoreDrive);
	TDriveName drive=driveUnit.Name();
	aPath.Insert(0, drive);
	//append private path
	TPath privatePath;
	aFs.PrivatePath(privatePath);
	aPath.Append(privatePath);
	aPath.Append(KStoreSubDir);
	} // CReassemblyStoreUtility::PrivatePath

/**
 *  Constructor.
*/
CReassemblyStore::CReassemblyStore(RFs& aFs) : iFs(aFs), iEntryArray(KFlatArrayGranularity)
	{
	iLastReceivedTime.UniversalTime();
	iLastRealTime = iLastReceivedTime;
	}

/**
 *  Destructor.
*/
CReassemblyStore::~CReassemblyStore()
	{
	iEntryArray.Reset();
	}

/**
It cleans up the re-assembly store.
This function will be called to allow re-assembly store to initialize/clean-up

@internalComponent
*/
void CReassemblyStore::InitializeL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CREASSEMBLYSTORE_INITIALIZEL_1, "CClass0SmsReassemblyStore::InitializeL()");
	// Initialize Re-assembly store.
	OpenStoreL();
	BeginTransactionLC();
	TInt count = iEntryArray.Count();
	while (count--)
		{
		TReassemblyEntry entry= iEntryArray[count];
		if ((entry.Storage() == CSmsMessage::ESmsSIMStorage) || (entry.Storage() == CSmsMessage::ESmsCombinedStorage))
			{
			DeleteEntryL(entry);
			}
		else
			{
			SetPassedToClientL(entry, EFalse);
			}
		}
	CommitTransactionL();
	Close();
	}

/**
 *  Purges the reassembly file store.
 *  
 *  After a multipart message, it delete all the old entries.
 *  
 *  Entries will be purged when: 1) The complete message is received; 2) After
 *  aTimerintervalMinutes, if aPurgeIncompletely is false.
 *  
 *  PurgeL() will be called after the booting of the device or when a message
 *  has been received.
 *  
 *  This function opens and closes the file automatically.
 *  
 *  
 *  @param aTimeIntervalMinutes Purge time
 *  @param aPurgeIncompleteOnly Purge complete messages flag
 */
void CReassemblyStore::PurgeL(const TTimeIntervalMinutes& aTimeIntervalMinutes,TBool aPurgeIncompleteOnly)
	{
	//Call purging function
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CREASSEMBLYSTORE_PURGEL_1, "CReassemblyStore::PurgeL(): aTimeIntervalMinutes=%d, aPurgeIncompleteOnly=%d",aTimeIntervalMinutes.Int(), aPurgeIncompleteOnly);

	// TODO - flag
	// we could also save the call of the method from the consruction of the smsprot
	if( aPurgeIncompleteOnly )
		return;

	TInt count=iEntryArray.Count();
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CREASSEMBLYSTORE_PURGEL_2, "CClass0SmsReassemblyStore::PurgeL(): count=%d", count);

	TTime time;
	time.UniversalTime();

	// we open the file outside the loop
	// to save some CPU
	BeginTransactionLC();
	for (TInt i=count-1; i>=0; i--)
		{
		//TReassemblyEntry entry=iEntryArray[i];
		if (time > (iEntryArray[i].Time()+aTimeIntervalMinutes))
			// TODO - flag
			// check the logic o the aPurgeIncompleteOnly flg
			// don't purge the store if the entry is complete
			// entry.IsComplete()  )
			{
			DeleteEntryL(iEntryArray[i]);
			}
		}
	CommitTransactionL();

	PopulateEntryArrayL(iEntryArray);
	}

/**
It deletes all the enumerated SIM messages stored in re-assembly store.
This function will be called if user choses to cancel the enumeration.

@internalComponent
*/
void CReassemblyStore::DeleteEnumeratedSIMEntries()
	{
	const TInt count = iEntryArray.Count();

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CREASSEMBLYSTORE_DELETEENUMERATEDSIMENTRIES_1, "CReassemblyStore::DeleteEnumeratedSIMEntries(): %d messages in RAS", count);

	TInt index;

	for (index = count-1;  index >= 0;  --index)
		{
		TReassemblyEntry  entry = iEntryArray[index];

		if (entry.Storage()==CSmsMessage::ESmsSIMStorage)
			{
			TRAP_IGNORE(BeginTransactionLC();
						DeleteEntryL(entry);
						CommitTransactionL();
						iEntryArray.Delete(index));
			}
		}
	}

/**
It returns the number of complete messages in reassembly store.

@internalComponent
*/
TInt CReassemblyStore::NumberOfCompleteMessages()
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CREASSEMBLYSTORE_NUMBEROFCOMPLETEMESSAGES_1, "CReassemblyStore::NumberOfCompleteMessages(): iEntryArray.Count()=%d",iEntryArray.Count());

	//local variable for complete entries
	TInt count( 0 );
	// checks all entrys in the reassembly store
	for ( TInt i = iEntryArray.Count()-1; i >= 0; i-- )
		{
		// checks if entry is completed
		if ( iEntryArray[i].IsComplete() )
			{
			++count;
			}
		}
	return count;
	}

/**
It adds the message segment to the reassembly store. There are 5 possiblities:

1) This is the single segment message.
We therefore have all the segments.
2) This is a duplicate message segment.
We will ignore it.
3) This is the last segment in the message required to complete it.
The other segments are already stored.
4) This is another PDU to an existing message in the store, but it is
not yet complete.
5) This is the first PDU in the message, and therefore the message is
not yet complete and no segments are stored.

@note Only SUBMIT or DELIVER PDUs can be added to the reassembly store.

@param aSmsMessage  a reference to the SMS message.
	It acts both as input & output. If the message is complete, it contains the decoded message.
	Otherwise it contains the received message with few properties set (LogServerId, Time).

@param aGsmSms	a reference to GsmSms object which contain actual PDU.
	It acts as input.

@param aIsComplete  Boolean value indicating whether the message is complete or not.
	It acts both as input & output.

@param aIsEnumeration	Boolean value indicating whether the function is called at the time of enumeration.
	It acts as only input.

@param aCount  value indicating the number of current PDUs in the re-assembly store for the given SMS message.
	It acts as only output.

@param aTotal	value indicating the total number of PDUs in the re-assembly store for the given SMS message.
	It acts as only output.

@internalComponent
*/
void CReassemblyStore::AddSegmentToReassemblyStoreL(CSmsMessage& aSmsMessage,const TGsmSms& aGsmSms, TInt& aIndex, TBool& aIsComplete, TBool aIsEnumeration, TInt& aCount, TInt& aTotal)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CREASSEMBLYSTORE_ADDSEGMENTTOREASSEMBLYSTOREL_1, "CReassemblyStore::AddSegmentToReassemblyStoreL(): isComplete Message=%d",aSmsMessage.IsComplete());

	/*
	(1) If it is a single segment message create a new message
	(2) If it is part of concatenated message find whether this is the first PDU
	or it is the part of existing message.
	If it is a new message then create new one.
	If it is part of existing message, then check duplication (CheckDuplication()).
	If it is a duplicate, return. Otherwise update the reassembly store.
	*/

	if (aIsComplete ||  aSmsMessage.Type() == CSmsPDU::ESmsStatusReport)
		{
		//
		// 1) This is the complete message (e.g. a single-segment message).
		//    We therefore have all the segments.
		//
		// Create the new message in the reassembly store. This is incase the
		// power fails before the client gets it (note that it will be ack'd
		// before passed to the client) so keeping it in memory is not
		// acceptable...
		//
		NewMessagePDUL(aIndex, aSmsMessage, aGsmSms);
		}
	else
		{
		//
		// If not yet complete, then we must be part of a multiple PDU message.
		// Search the reassembly store for existing parts of the message.
		//
		TInt  segStoreIndex(KErrNotFound);

		MatchPDUToExistingMessage(aSmsMessage, segStoreIndex);
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CREASSEMBLYSTORE_ADDSEGMENTTOREASSEMBLYSTOREL_2, "CSmsReassemblyStore::AddSegmentToReassemblyStoreL(): ""segStoreIndex=%d", segStoreIndex);

		//
		// If not yet complete, then we must be part of a multiple PDU message.
		// Search the reassembly store for existing parts of the message. This
		// may set iIsComplete to true if all segments are then found.
		//
		if (segStoreIndex != KErrNotFound)
			{
			TBool  isDuplicateSlot(EFalse);
			TBool  isDuplicateMsgRef(EFalse);
			//
			// So we found a related part of the message, add this message to the
			// store...
			//
			aIndex = segStoreIndex;
			UpdateExistingMessageL(aSmsMessage, aGsmSms, aIndex,
									aIsComplete, isDuplicateMsgRef,
									isDuplicateSlot);
			OstTraceDefExt4(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CREASSEMBLYSTORE_ADDSEGMENTTOREASSEMBLYSTOREL_3, "CSmsReassemblyStore::AddSegmentToReassemblyStoreL(): ""aIndex=%d, isComplete=%d, isDuplicateMsgRef=%d, isDuplicateSlot=%d",aIndex, aIsComplete, isDuplicateMsgRef, isDuplicateSlot);

			if (isDuplicateMsgRef)
				{
				//
				// In most cases discard it, unless we are doing an enumeration???
				//
				if (aIsEnumeration)
					{
					NewMessagePDUL(aIndex, aSmsMessage, aGsmSms);
					}
				}
			else if (aIsComplete)
				{
				//
				// 3) This is the last segment in the message required to complete it.
				//    The other segments are already stored.
				//
				// Load the complete message into memory for futher processing.
				//
				GetMessageL(aIndex, aSmsMessage);
				}
			else
				{
				//
				// 4) This is another PDU to an existing message in the store, but it is
				//    not yet complete.
				//
				// Update the this segment with the timestamp of the original message.
				//
				CSmsBuffer*  buffer = CSmsBuffer::NewL();
				CSmsMessage*  firstMessagePdu = CSmsMessage::NewL(iFs,
																  CSmsPDU::ESmsDeliver, buffer);
				CleanupStack::PushL(firstMessagePdu);
				GetMessageL(aIndex, *firstMessagePdu);
				aSmsMessage.SetUTCOffset(firstMessagePdu->UTCOffset());
				CleanupStack::PopAndDestroy(firstMessagePdu);
				}
			}
		else
			{
			//
			// 5) This is the first PDU in the message, and therefore the message is
			//    not yet complete and no segments are stored.
			//
			// The entry needs to be added to the reassembly store as a new entry.
			//
			NewMessagePDUL(aIndex, aSmsMessage, aGsmSms);
			}
		}

	const TReassemblyEntry&  entry = iEntryArray[aIndex];
	aCount = entry.Count();
	aTotal = entry.Total();
	}

/**
It deletes the given SMS message from re-assembly store.

@param aSmsMessage  Message to delete.
@param aPassed      Determines if we are searching for a message already
					passed to the client.

@internalComponent
*/
void CReassemblyStore::DeleteMessageL(const CSmsMessage& aSmsMessage, TBool aPassed)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CREASSEMBLYSTORE_DELETEMESSAGEL_1, "CReassemblyStore::DeleteMessageL()");
	TInt index(0);
	BeginTransactionLC();
	if (FindMessageL(aSmsMessage, aPassed, index))
		{
		const TReassemblyEntry&  entry = iEntryArray[index];
		DeleteEntryL(entry);
		iEntryArray.Delete(index);
		}
	CommitTransactionL();
	}

/**
It updates log server id of the passed message in re-assembly store.

@param aSmsMessage  a reference to a message.
@param aIndex	index number of sms message to be updated.

@internalComponent
*/
void CReassemblyStore::UpdateLogServerIdOfMessageL(const CSmsMessage& aSmsMessage, TInt aIndex)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CREASSEMBLYSTORE_UPDATELOGSERVERIDOFMESSAGEL_1, "CReassemblyStore::UpdateLogServerIdOfMessageL()");
    TInt  foundIndex(KErrNotFound);
	TBool  found(EFalse);

	BeginTransactionLC();

	found = FindMessageL(aSmsMessage , EFalse, foundIndex);
	if (found  &&  (aIndex == foundIndex))
		{
		const TReassemblyEntry&  entry = iEntryArray[foundIndex];
		UpdateLogServerIdL(entry, aSmsMessage.LogServerId());
		iEntryArray[foundIndex].SetLogServerId(aSmsMessage.LogServerId());
		}
	CommitTransactionL();
	}

/**
It updates that the given SMS message in re-assembly store is passed to client.

@param aSmsMessage  Message which is passed to client.

@internalComponent
*/
void CReassemblyStore::SetMessagePassedToClientL(const CSmsMessage& aSmsMessage, TBool aPassed)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CREASSEMBLYSTORE_SETMESSAGEPASSEDTOCLIENTL_1, "CReassemblyStore::SetMessagePassedToClientL()");
	TInt index(0);

	BeginTransactionLC();

	if (FindMessageL(aSmsMessage , !aPassed, index))
		{
		const TReassemblyEntry&  entry = iEntryArray[index];
		SetPassedToClientL(entry, aPassed);
		iEntryArray[index].SetPassedToClient(aPassed);
		}
	CommitTransactionL();
	}

/**
It adds a new message segment to the reassembly store and it returns an index to the message.

@param aIndex value indicating the index of the message added to re-assembly store.
	It acts as output.

@param aSmsMessage  a reference to the SMS message.
	It acts as input.

@param aGsmSms	a reference to GsmSms object which contain actual PDU.
	It acts as input.

@internalComponent
*/
void CReassemblyStore::NewMessagePDUL(TInt& aIndex,CSmsMessage& aSmsMessage,const TGsmSms& aGsmSms)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CREASSEMBLYSTORE_NEWMESSAGEPDUL_1, "CReassemblyStore::NewMessagePDUL");

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

	TReassemblyEntry entry;
	CReassemblyStoreUtility::PopulateEntry(entry,aSmsMessage,1);
	BeginTransactionLC();
	AddNewMessageL(aSmsMessage, aGsmSms);
	CommitTransactionL();
	//Successfully added so add the entry in entry array.
	aIndex = iEntryArray.Count();
	iEntryArray.AppendL(entry);
	}

/**
It adds a new message segment to the existing message in reassembly store & returns
whether this segment makes this message complete or not. It also returns whether this
segment is the duplicate one or not.

@param aSmsMessage  a reference to the SMS message.
	It acts as input.

@param aGsmSms	a reference to GsmSms object which contain actual PDU.
	It acts as input.

@param aIndex value indicating the index of the message added to re-assembly store.
	It acts as output.

@param aIsComplete  Boolean value indicating whether the message is complete or not.
	It acts as output.

@param aDuplicateMsgRef	Boolean value indicating whether the added segment is a duplicate one or not.
	It acts as output.

@param aDuplicateSlot Boolean value indicating whether the added segment is from duplicate slot or not.
	It acts as output.

@internalComponent
*/
void CReassemblyStore::UpdateExistingMessageL(CSmsMessage& aSmsMessage,const TGsmSms& aGsmSms,
												TInt aIndex, TBool& aIsComplete,
												TBool& aDuplicateMsgRef, TBool& aDuplicateSlot)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CREASSEMBLYSTORE_UPDATEEXISTINGMESSAGEL_1, "CReassemblyStore::UpdateExistingMessageL");
	aIsComplete = EFalse;
	BeginTransactionLC();
	UpdateExistingMessageL(aSmsMessage, aGsmSms, aDuplicateMsgRef, aDuplicateSlot);
	CommitTransactionL();
	if ((aDuplicateMsgRef == EFalse) && (aDuplicateSlot==EFalse))
		{
		iEntryArray[aIndex].SetCount(iEntryArray[aIndex].Count() + 1);
		if (iEntryArray[aIndex].IsComplete())
			{
			aIsComplete = ETrue;
			}
		}
	}

/**
It matches the passed message in re-assembly store & returns the index.

@param aSmsMessage  a reference to the SMS message.
	It acts as input.

@param aIndex index number of message in re-assembly store.
	It acts as input.

@internalComponent
*/
void CReassemblyStore::MatchPDUToExistingMessage(const CSmsMessage& aSmsMessage,
													TInt& aIndex)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CREASSEMBLYSTORE_MATCHPDUTOEXISTINGMESSAGE_1, "CReassemblyStore::MatchPDUToExistingMessage()");

	aIndex = KErrNotFound;

	TGsmSmsTelNumber  parsedAddress;
	aSmsMessage.ParsedToFromAddress(parsedAddress);

	//
	// Search the reassembly store for a matching entry (start from the
	// end as the most recent PDUs appear at the end)...
	//
	TInt reassemblyCount = iEntryArray.Count();

	for (TInt  index = 0;  index < reassemblyCount;  index++)
		{
		TReassemblyEntry&  entry = iEntryArray[index];
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

	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CREASSEMBLYSTORE_MATCHPDUTOEXISTINGMESSAGE_2, "CReassemblyStore::MatchPDUToExistingMessage(): reassemblyCount=%d, aIndex=%d", reassemblyCount, aIndex);
	} // CReassemblyStore::MatchPDUToExistingMessage

/**
It retrieves the message from re-assembly store.

@param aIndex index number of message in re-assembly store.
	It acts as input.

@param aSmsMessage  a reference to the SMS message.
	It acts as output.

@internalComponent
*/
void CReassemblyStore::GetMessageL(TInt aIndex, CSmsMessage& aSmsMessage)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CREASSEMBLYSTORE_GETMESSAGEL_1, "CReassemblyStore::GetMessageL()");
	const TReassemblyEntry&  entry = iEntryArray[aIndex];
	RetrieveMessageL(entry, aSmsMessage);
	}

/**
 *  Searches the reassembly store for a CSmsMessage with aPassed value (indicates if we 
 *	are searching for a message already passed to client) and returns its index.
 *
 *  @param aSmsMessage  Message to search for.
 *  @param aPassed      Determines if we are searching for a message already
 *                      passed to the client.
 *  @param aIndex       Return index value.
 *
 *  @return True and an index if aSmsMessage is found in this reassembly store
 */
TBool CReassemblyStore::FindMessageL(const CSmsMessage& aSmsMessage,
										TBool aPassed,
										TInt& aIndex)
 	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CREASSEMBLYSTORE_FINDMESSAGEL_1, "CReassemblyStore::FindMessageL()");

	//
	// Parse the GSM data from the SMS message...
	//
	TGsmSmsTelNumber  parsedAddress;

	aSmsMessage.ParsedToFromAddress(parsedAddress);

	//
	// Search the store for a matching message...
	//
 	for (TInt index = iEntryArray.Count() - 1;  index >= 0;  index--)
 		{
		const TReassemblyEntry&  entry = iEntryArray[index];

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
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CREASSEMBLYSTORE_FINDMESSAGEL_2, "CReassemblyStore::FindMessage(): Found! index=%d", index);

			aIndex = index;
			
			return ETrue;
			}
 		}

	//
	// Not found...
	//
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CREASSEMBLYSTORE_FINDMESSAGEL_3, "CReassemblyStore::FindMessage(): Not found!");

	return EFalse;
	} // CReassemblyStore::FindMessageL
