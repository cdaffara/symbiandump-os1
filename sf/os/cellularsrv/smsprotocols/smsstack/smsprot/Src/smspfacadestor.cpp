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
#include "smspfacadestorTraces.h"
#endif

#include "gsmubuf.h"
#include "smspfacadestor.h"

/**
Static factory constructor. Uses two phase 
construction and leaves nothing on the CleanupStack.

@internalComponent

@return A pointer to the newly created CFacadeSmsReassemblyStore object.
@param aFs  File Server handle.
@param aSmsComm  Notification Event Reference.
@leave KErrNoMemory

@pre A connected file server session must be passed as parameter.
@post CFacadeSmsReassemblyStore object is now fully initialised
*/
CFacadeSmsReassemblyStore* CFacadeSmsReassemblyStore::NewL(RFs& aFs, MSmsComm& aSmsComm)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_NEWL_1, "CFacadeSmsReassemblyStore::NewL()");

	CFacadeSmsReassemblyStore*  self = new (ELeave) CFacadeSmsReassemblyStore(aFs, aSmsComm);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
	}

/**
 *  Constructor.
*/
CFacadeSmsReassemblyStore::CFacadeSmsReassemblyStore(RFs& aFs, MSmsComm& aSmsComm)
	:iFs(aFs), iSmsComm(aSmsComm), iReassemblyStore(NULL), iClass0ReassemblyStore(NULL)
	{
	// NOP
	}

/**
 *  Destructor. It destroys all the member variables.
*/
CFacadeSmsReassemblyStore::~CFacadeSmsReassemblyStore()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_DTOR_1, "~CFacadeSmsReassemblyStore()");
	iReassemblyStore->Close();
	delete iReassemblyStore;

	if (iClass0ReassemblyStore)
		{
		iClass0ReassemblyStore->Close();
		delete iClass0ReassemblyStore;
		}
	}

/**
Second Phase construction. It creates re-assembly stores. If SMS stack is configured
for handling class 0 messages in out-of-disk condition. It creates class 0 
& non-class 0 re-assembly store. Otherwise it creates only one re-assembly store
which will be used to store all type of SMS messages.

@internalComponent

@leave KErrNoMemory

@pre A connected file server session must be passed as parameter.
@post CFacadeSmsReassemblyStore object is now fully constructed.
*/
void CFacadeSmsReassemblyStore::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_CONSTRUCTL_1, "CFacadeSmsReassemblyStore::ConstructL()");

	iReassemblyStore = CSmsReassemblyStore::NewL(iFs);
	/*
	Read [ReassemblyStore] section from smswap.sms.esk file. If section is absent then there is
	no need to create class 0 re-assembly store. This sms stack is configured to work
	as before (class 0 message will be -vely ack in out-of-disk condition).
	If ReassemblyStore section is present then read the value of Class0.
	If the value of Class0 is 1. Then create class 0 re-assembly store object.
	For example:
	[ReassemblyStore]
	Class0= 1
	MaxClass0Messages=10
	NumberOfPDUSegements=20
	GuardTimeOut=12
	*/
	CESockIniData*  ini = NULL;
	ini=CESockIniData::NewL(_L("smswap.sms.esk"));
	CleanupStack::PushL(ini);
	TBool status(EFalse);
	if (ini->FindVar(_L("ReassemblyStore"), _L("Class0"), status))
		{
		if (status)
			{
			iClass0ReassemblyStore = CClass0SmsReassemblyStore::NewL(iFs, iSmsComm);
			}
		}

	// Set the default value for iMaxmumNumberOfCompleteMessagesInReassemblyStore.
	iMaxmumNumberOfCompleteMessagesInReassemblyStore = KDefaultMaxmumNumberOfCompleteMessagesInReassemblyStore;
	// Load up the user configurable setting for the maximum number of complete messages in 
	// the reassembly store.
	TPtrC value;
	if((ini->FindVar(_L("ReasmemblyStoreOptions"),_L("MaxNumOfComMessInReStore"),value)))
		{
		TLex16 valueconv(value);
		valueconv.Val(iMaxmumNumberOfCompleteMessagesInReassemblyStore); 
		}

	CleanupStack::PopAndDestroy(ini);
	}

/**
It open the re-assembly stores.
This function needs to be called before doing any operations (add/delete/update) in the 
re-assembly stores.

@internalComponent
*/
void CFacadeSmsReassemblyStore::OpenStoreL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_OPENSTOREL_1, "CFacadeSmsReassemblyStore::OpenStoreL()");
	iReassemblyStore->OpenStoreL();
	if (iClass0ReassemblyStore)
		{
		iClass0ReassemblyStore->OpenStoreL();
		}
	}

/**
It closes the re-assembly stores.

@internalComponent
*/
void CFacadeSmsReassemblyStore::Close()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_CLOSE_1, "CFacadeSmsReassemblyStore::Close()");
	// Close general Re-assembly store.
	iReassemblyStore->Close();
	// Close Class0 re-assembly store.
	if (iClass0ReassemblyStore)
		{
		iClass0ReassemblyStore->Close();
		}
	}

/**
It returns the file session.

@internalComponent

@return returns the file session.
*/
RFs& CFacadeSmsReassemblyStore::FileSession() const
	{
	return iFs;
	}

/**
It initializes the re-assembly store.
It goes through all the entries in re-assembly store. It updates its header information.
If any sms message is either SIM based or combined storage based then it is deleted from re-assembly store.
For other type of SMS messages, its header info is updated to indicate that it is not passed to client.

This initialization process is required because SMS stack might have been re-started.

@internalComponent
*/
void CFacadeSmsReassemblyStore::InitL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_INITL_1, "CFacadeSmsReassemblyStore::InitL()");
	// Initialize Non-class 0 Re-assembly store.
	InitializeNonClass0StoreL();
	// Initialize Class0 re-assembly store.
	if (iClass0ReassemblyStore)
		{
		iClass0ReassemblyStore->InitializeL();
		}
	}

/**
Purges the reassembly file stores.

@param aTimeIntervalMinutes Purge time
@param aPurgeIncompleteOnly Purge complete messages flag

@internalComponent
*/
void CFacadeSmsReassemblyStore::PurgeL(const TTimeIntervalMinutes& aTimeIntervalMinutes,TBool aPurgeIncompleteOnly)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_PURGEL_1, "CFacadeSmsReassemblyStore::PurgeL()");
	iReassemblyStore->PurgeL(aTimeIntervalMinutes, aPurgeIncompleteOnly);
	if (iClass0ReassemblyStore)
		{
		iClass0ReassemblyStore->PurgeL(aTimeIntervalMinutes, aPurgeIncompleteOnly);
		}
	}

/**
It returns a boolean value indicating whether re-assembly store is full or not.
If number of complete sms messages exceed the configured value (KMaxmumNumberOfCompleteMessagesInReassemblyStore),
it return TRUE. Otherwise it returns FALSE.

@internalComponent
*/
TBool CFacadeSmsReassemblyStore::IsFull()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_ISFULL_1, "CFacadeSmsReassemblyStore::IsFull()");

	//local variable for complete entries
	TInt count( 0 );
	count = NumberOfCompleteNonClass0Messages();
	if (iClass0ReassemblyStore)
		{
		count += iClass0ReassemblyStore->NumberOfCompleteMessages();
		}
	if (count > iMaxmumNumberOfCompleteMessagesInReassemblyStore)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

/**
It deletes all the enumerated SIM messages stored in re-assembly store.
This function will be called if user choses to cancel the enumeration.

@internalComponent
*/
void CFacadeSmsReassemblyStore::DeleteEnumeratedSIMEntries()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_DELETEENUMERATEDSIMENTRIES_1, "CFacadeSmsReassemblyStore::DeleteEnumeratedSIMEntries()");
	DeleteNonClass0EnumeratedSIMEntries();
	if (iClass0ReassemblyStore)
		{
		iClass0ReassemblyStore->DeleteEnumeratedSIMEntries();
		}
	}

/**
It externalizes all the enumerated messages.
It goes through the re-assembly store and sends all those SMS messages 
(which is SIM/Combined storage based) to client (aSmsProvider).

@param aSmsProvider  a reference to a service access point.
@param aCount	number of sms messages enumerated.
@return number of new segments.

@internalComponent
*/
TInt CFacadeSmsReassemblyStore::ExternalizeEnumeratedMessagesL(CSmsProvider& aProvider,TInt& aCount)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_EXTERNALIZEENUMERATEDMESSAGESL_1, "CFacadeSmsReassemblyStore::ExternalizeEnumeratedMessagesL()");
	TInt numNewSegments(0);
	numNewSegments = ExternalizeEnumeratedNonClass0SmsMessagesL(aProvider, aCount);
	if (iClass0ReassemblyStore)
		{
		numNewSegments += ExternalizeEnumeratedClass0SmsMessagesL(aProvider, aCount);
		}
	return numNewSegments;
	}

/**
It searches the reassembly store for complete messages and then it sends that 
message for further processing. It is called when a new observer is added or 
a PDU has been received and successfully processed.

@param aSmsComm  a reference to the protocol.
@param aCurrentSmsMessage	a pointer to current SMS message.

@internalComponent
*/
void CFacadeSmsReassemblyStore::ProcessCompleteSmsMessagesL(MSmsComm& aSmsComm, const CSmsMessage* aCurrentSmsMessage)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_PROCESSCOMPLETESMSMESSAGESL_1, "CFacadeSmsReassemblyStore::ProcessCompleteSmsMessagesL");

	ProcessCompleteNonClass0SmsMessagesL(aSmsComm, aCurrentSmsMessage);
	if (iClass0ReassemblyStore)
		{
		ProcessCompleteClass0SmsMessagesL(aSmsComm, aCurrentSmsMessage);
		}
	}

/**
It adds the message segment to the reassembly store.
This function first checks on which re-assembly store the message should be stored and then add
the message to appropriate re-assembly store.

@note Only SUBMIT or DELIVER PDUs can be added to the reassembly store.

@param aSmsMessage  a reference to the SMS message.
	It acts both as input & output. At the time of function call it contains the message.
	When the function returns it contains full decoded message if it is complete.
	But in case of class 0 messages it might not contain complete class 0 messages.

@param aGsmSms	a reference to GsmSms object which contain actual PDU.
	It acts as pure input.

@param aIsComplete  boolean value indicating whether the message is complete or not.
	It acts both as input & output.
	In case of multi-segment message, the value of aIsComplete will be true when function returns 
	if the added segment makes the messsage complete.

@param aIsEnumeration	boolean value indicating whether the function is called at the time of enumeration.
	It acts as only input.

@param aCount  value indicating the number of current PDUs in the re-assembly store for the given SMS message.
	It acts as only output.

@param aTotal	value indicating the total number of PDUs for the given sms message.
	It acts as only output.

@internalComponent

NOTE:
	When function returns, if the value of aIsComplete is True, then aSmsMesssage will contain the
	complete decoded SMS message.
	But the above statement might not be always true:
	In case of class 0 messages, it is possible to forward the incomplete messages. So after forwarding
	the incomplete message, if we receive other constituent PDU of that message then in that case we 
	might receive all the constituent PDU of that message but aSmsMesssage will contain partial complete message.
	To find out complete/incompletness of the message, CSmsMessage::IsComplete() message function has to be called.
*/
void CFacadeSmsReassemblyStore::AddSegmentToReassemblyStoreL(CSmsMessage& aSmsMessage,const TGsmSms& aGsmSms, TInt& aIndex, TBool& aIsComplete, TBool aIsEnumeration, TInt& aCount, TInt& aTotal)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_ADDSEGMENTTOREASSEMBLYSTOREL_1, "CFacadeSmsReassemblyStore::AddSegmentToReassemblyStoreL(): isComplete Message=%d",aSmsMessage.IsComplete());

	TBool toBeStoredInClass0ReassemblyStore = IsForClass0ReassemblyStore(aSmsMessage);

	if (toBeStoredInClass0ReassemblyStore)
		{
		iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(aSmsMessage, aGsmSms, aIndex, aIsComplete, aIsEnumeration, aCount, aTotal);
		}
	else
		{
		AddSegmentToNonClass0ReassemblyStoreL(aSmsMessage, aGsmSms, aIndex, aIsComplete, aIsEnumeration, aCount, aTotal);
		}
	}

/**
It forwards the complete class 0 messages to client.
NOTE:
	This function needs to be called only in case of class 0 messages.

@param aSmsComm  a reference to aSmsComm object which implemented the events.

@param aSmsMessage	a reference to sms message object. This sms message must be class 0 messages.

@param aOriginalSmsAddr pointer to the address of the sender of a previously sent

@param aOriginalSmsMessage pointer to a message previously sent matched to the received 
							one (e.g. status report).	Null if not matched.

@param aDes user data for the deliver report acknowledging this message to the SC.
			Filled in by the observer.

@internalComponent
*/
void CFacadeSmsReassemblyStore::ForwardCompleteClass0SmsMessagesL(MSmsComm& aSmsComm, const CSmsMessage& aSmsMessage,const TSmsAddr* aOriginalSmsAddr,const CSmsMessage* aOriginalSmsMessage,TDes& aDes)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_FORWARDCOMPLETECLASS0SMSMESSAGESL_1, "CFacadeSmsReassemblyStore::ForwardCompleteClass0SmsMessagesL");
	if (iClass0ReassemblyStore)
		{
		iClass0ReassemblyStore->ForwardCompleteClass0SmsMessagesL(aSmsComm, aSmsMessage, aOriginalSmsAddr, aOriginalSmsMessage, aDes);
		}
	}

/**
It frees the space by forwarding the class 0 message if class 0 re-assembly store 
exceeds limitation (max class 0 message, max reserved pdu segment).
NOTE:
	This function needs to be called only in case of class 0 messages.

@param aSmsComm  a reference to aSmsComm object which implemented the events.

@internalComponent
*/
void CFacadeSmsReassemblyStore::ProcessMessageIfExceedLimitationL(MSmsComm& aSmsComm)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_PROCESSMESSAGEIFEXCEEDLIMITATIONL_1, "CFacadeSmsReassemblyStore::ProcessMessageIfExceedLimitationL");
	if (iClass0ReassemblyStore)
		{
		iClass0ReassemblyStore->ProcessMessageIfExceedLimitationL(aSmsComm);
		}
	}

/**
It sets the incomplete messsage forwarded to client.
Internally it frees up the memory by removing the forwarded PDUs and also it stores forwarded PDU index.

NOTE:
	This function needs to be called only in case of class 0 messages.

@param aSmsMessage	a reference to sms message object. This sms message must be class 0 messages.

@internalComponent
*/
void CFacadeSmsReassemblyStore::SetIncompleteMessageForwardedToClientL(const CSmsMessage& aSmsMessage)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_SETINCOMPLETEMESSAGEFORWARDEDTOCLIENTL_1, "CFacadeSmsReassemblyStore::SetIncompleteMessageForwardedToClientL()");
	if (iClass0ReassemblyStore)
		{
		iClass0ReassemblyStore->SetIncompleteMessageForwardedToClientL(aSmsMessage);
		}
	}

/**
It sets the disk space status.
If disk space is full, then class 0 re-assembly store stores the incoming message in
pre-allocated file. Otherwise it stores the message in permanent store file.
*/
void CFacadeSmsReassemblyStore::SetDiskSpaceState(TSmsDiskSpaceMonitorStatus aDiskSpaceStatus)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_SETDISKSPACESTATE_1, "CFacadeSmsReassemblyStore::SetDiskSpaceState()");
	if (iClass0ReassemblyStore)
		{
		iClass0ReassemblyStore->SetDiskSpaceState(aDiskSpaceStatus);
		}
	}

/**
It deletes the given SMS message from re-assembly store.

@param aSmsMessage  Message to delete.
@param aPassed      Determines if we are searching for a message already
					passed to the client.

@internalComponent
*/
void CFacadeSmsReassemblyStore::DeleteMessageL(const CSmsMessage& aSmsMessage, TBool aPassed)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_DELETEMESSAGEL_1, "CFacadeSmsReassemblyStore::DeleteEntryL()");

	TBool toBeStoredInClass0ReassemblyStore = IsForClass0ReassemblyStore(aSmsMessage);

	if (toBeStoredInClass0ReassemblyStore)
		{
		iClass0ReassemblyStore->DeleteMessageL(aSmsMessage, aPassed);
		}
	else
		{
		DeleteNonClass0MessageL(aSmsMessage, aPassed);
		}
	}

/**
It updates log server id of the passed message in re-assembly store.

@param aSmsMessage  a reference to a message.
@param aIndex	index number of sms message to be updated.

@internalComponent
*/
void CFacadeSmsReassemblyStore::UpdateLogServerIdL(const CSmsMessage& aSmsMessage, TInt aIndex)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_UPDATELOGSERVERIDL_1, "CFacadeSmsReassemblyStore::UpdateLogServerIdL()");

	TBool toBeStoredInClass0ReassemblyStore = IsForClass0ReassemblyStore(aSmsMessage);

	if (toBeStoredInClass0ReassemblyStore)
		{
		iClass0ReassemblyStore->UpdateLogServerIdOfMessageL(aSmsMessage, aIndex);
		}
	else
		{
		UpdateLogServerIdOfNonClass0MessageL(aSmsMessage, aIndex);
		}
	}

/**
It updates that the given SMS message in re-assembly store is passed to client.

@param aSmsMessage  Message which is passed to client.

@internalComponent
*/
void CFacadeSmsReassemblyStore::SetMessagePassedToClientL(const CSmsMessage& aSmsMessage, TBool aPassed)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_SETMESSAGEPASSEDTOCLIENTL_1, "CFacadeSmsReassemblyStore::SetMessagePassedToClientL()");

	TBool toBeStoredInClass0ReassemblyStore = IsForClass0ReassemblyStore(aSmsMessage);

	if (toBeStoredInClass0ReassemblyStore)
		{
		iClass0ReassemblyStore->SetMessagePassedToClientL(aSmsMessage, aPassed);
		}
	else
		{
		SetNonClass0MessagePassedToClientL(aSmsMessage, aPassed);
		}
	}

/**
Returns a boolean value indicating whether this class contains separate
re-assembly store for class 0 message or not.

@internalComponent
*/
TBool CFacadeSmsReassemblyStore::IsSeparateClass0StoreSupported()
	{
	if (iClass0ReassemblyStore)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

/**
Returns a boolean value indicating where this SMS message will be stored.
If it rerurns EFalse, message will be stored in normal re-assembly store,
otherwise it will be stored in class 0 re-assembly store.

@internalComponent
*/
TBool CFacadeSmsReassemblyStore::IsForClass0ReassemblyStore(const CSmsMessage& aSmsMessage)
	{
	if (iClass0ReassemblyStore == NULL)
		{
		return EFalse;
		}

	TSmsDataCodingScheme::TSmsClass  msgClass;

	if (aSmsMessage.SmsPDU().DataCodingSchemePresent()	&&	aSmsMessage.SmsPDU().Class(msgClass))
		{
		if (msgClass == TSmsDataCodingScheme::ESmsClass0)
			{
			//Check also whether it is a WAP Datagram
			// In that case return EFalse otherwise ETrue (REQ7012)
			if (!IsWapSMS(aSmsMessage))
				{
				return ETrue;
				}
			}
		}
	return EFalse;
	}

/**
It initializes the re-assembly store.
It goes through all the entries in re-assembly store. It updates its header information.
If any sms message is either SIM based or combined storage based then it is deleted from re-assembly store.
For other type of SMS messages, its header info is updated to indicate that it is not passed to client.

This initialization process is required because SMS stack might have been re-started.

@internalComponent
*/
void CFacadeSmsReassemblyStore::InitializeNonClass0StoreL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_INITIALIZENONCLASS0STOREL_1, "CFacadeSmsReassemblyStore::InitializeNonClass0StoreL()");
	// Initialize Re-assembly store.
	iReassemblyStore->OpenStoreL();
	iReassemblyStore->BeginTransactionLC();
	TInt count = iReassemblyStore->Entries().Count();
	while (count--)
		{
		TSmsReassemblyEntry entry= (TSmsReassemblyEntry&) iReassemblyStore->Entries()[count];
		CSmsPDU::TSmsPDUType pdu = entry.PduType();
		CSmsBuffer* buffer = CSmsBuffer::NewL();
		CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, pdu, buffer );
		CleanupStack::PushL(smsMessage);
		iReassemblyStore->GetMessageL(count,*smsMessage);
		if ((smsMessage->Storage() == CSmsMessage::ESmsSIMStorage) || (smsMessage->Storage() == CSmsMessage::ESmsCombinedStorage))
			{
			iReassemblyStore->DeleteEntryL(count);
			}
		else
			{
			iReassemblyStore->SetPassedToClientL(count, EFalse);
			}
		CleanupStack::PopAndDestroy(smsMessage);
		}
	iReassemblyStore->CommitTransactionL();
	iReassemblyStore->Close();
	}

/**
It deletes all the enumerated SIM messages stored in re-assembly store.
This function will be called if user choses to cancel the enumeration.

@internalComponent
*/
void CFacadeSmsReassemblyStore::DeleteNonClass0EnumeratedSIMEntries()
	{
	const TInt count = iReassemblyStore->Entries().Count();

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_DELETENONCLASS0ENUMERATEDSIMENTRIES_1, "CFacadeSmsReassemblyStore::DeleteNonClass0EnumeratedSIMEntries(): %d messages in RAS", count);

	TInt index;

	for (index = count-1;  index >= 0;  --index)
		{
		TSmsReassemblyEntry  entry = (TSmsReassemblyEntry&) iReassemblyStore->Entries()[index];

		if (entry.Storage()==CSmsMessage::ESmsSIMStorage)
			{
			TRAP_IGNORE(iReassemblyStore->BeginTransactionLC();
						iReassemblyStore->DeleteEntryL(index);
						iReassemblyStore->CommitTransactionL());
			}
		}
	}

/**
It deletes the given SMS message from re-assembly store.

@param aSmsMessage  Message to delete.
@param aPassed      Determines if we are searching for a message already
					passed to the client.

@internalComponent
*/
void CFacadeSmsReassemblyStore::DeleteNonClass0MessageL(const CSmsMessage& aSmsMessage, TBool aPassed)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_DELETENONCLASS0MESSAGEL_1, "CFacadeSmsReassemblyStore::DeleteNonClass0MessageL()");
	TInt index(0);

	if(!iReassemblyStore->InTransaction())
		{
		iReassemblyStore->BeginTransactionLC();
 		if (iReassemblyStore->FindMessageL(aSmsMessage, aPassed, index))
 			{
			iReassemblyStore->DeleteEntryL(index);
 			}
		iReassemblyStore->CommitTransactionL();
		}
	else if (iReassemblyStore->FindMessageL(aSmsMessage, aPassed,  index))
		{
		iReassemblyStore->DeleteEntryL(index);
		}
	}

/**
It updates that the given SMS message in re-assembly store is passed to client.

@param aSmsMessage  Message which is passed to client.

@internalComponent
*/
void CFacadeSmsReassemblyStore::SetNonClass0MessagePassedToClientL(const CSmsMessage& aSmsMessage, TBool aPassed)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_SETNONCLASS0MESSAGEPASSEDTOCLIENTL_1, "CFacadeSmsReassemblyStore::SetNonClass0MessagePassedToClientL()");
	TInt index(0);

	iReassemblyStore->BeginTransactionLC();
	if (iReassemblyStore->FindMessageL(aSmsMessage , !aPassed, index))
		{
 		iReassemblyStore->SetPassedToClientL(index, aPassed);
 		}
	iReassemblyStore->CommitTransactionL();
	}

/**
It returns the number of complete messages stored in general re-assembly store.
*/
TInt CFacadeSmsReassemblyStore::NumberOfCompleteNonClass0Messages()
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_NUMBEROFCOMPLETENONCLASS0MESSAGES_1, "CFacadeSmsReassemblyStore::NumberOfCompleteMessages(): Entries().Count()=%d",iReassemblyStore->Entries().Count());

	//local variable for complete entries
	TInt count( 0 );
	// checks all entrys in the reassembly store
	for ( TInt i = iReassemblyStore->Entries().Count()-1; i >= 0; i-- )
		{
		// checks if entry is completed
		if ( iReassemblyStore->Entries()[i].IsComplete() )
			{
			++count;
			}
		}
	return count;
	}

/**
It searches the non class 0 reassembly store for complete messages and then 
it sends that message for further processing. It is called when a new 
observer is added or a PDU has been received and successfully processed.

@param aSmsComm  a reference to the protocol.
@param aCurrentSmsMessage	a pointer to current SMS message.

@internalComponent
*/
void CFacadeSmsReassemblyStore::ProcessCompleteNonClass0SmsMessagesL(MSmsComm& aSmsComm, const CSmsMessage* aCurrentSmsMessage)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_PROCESSCOMPLETENONCLASS0SMSMESSAGESL_1, "CFacadeSmsReassemblyStore::ProcessCompleteNonClass0SmsMessagesL [from %d to 0]", iReassemblyStore->Entries().Count()-1);

	iReassemblyStore->BeginTransactionLC();
	TInt count = iReassemblyStore->Entries().Count();

	CSmsBuffer* buffer = CSmsBuffer::NewL();
	CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver, buffer );
	CleanupStack::PushL( smsMessage );

	while (count--)
		{
		TSAREntry entry(iReassemblyStore->Entries()[count]);
		if ( entry.IsComplete() )
			{
			TRAPD( ret, iReassemblyStore->GetMessageL( count , *smsMessage ) );
			if ( ret == KErrNone )
				{
				if( !iReassemblyStore->PassedToClient( count ) )
					{
						if(!(aCurrentSmsMessage && aCurrentSmsMessage->Time()==smsMessage->Time()))
						{
						TBuf16<CSmsPDUProcessor::ESmsMaxDeliverReportBufferSize> buffer;
						ret = aSmsComm.ProcessMessageL( *smsMessage, NULL, NULL, buffer );
						if ( ret == KErrNone )
							iReassemblyStore->SetPassedToClientL( count , ETrue);
						}
					}
				}
			}
		}
	CleanupStack::PopAndDestroy( smsMessage );
	iReassemblyStore->CommitTransactionL();
	}

/**
It searches the class 0 reassembly store for complete messages and then 
it sends that message for further processing. It is called when a new 
observer is added or a PDU has been received and successfully processed.

@param aSmsComm  a reference to the protocol.
@param aCurrentSmsMessage	a pointer to current SMS message.

@internalComponent
*/
void CFacadeSmsReassemblyStore::ProcessCompleteClass0SmsMessagesL(MSmsComm& aSmsComm, const CSmsMessage* aCurrentSmsMessage)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_PROCESSCOMPLETECLASS0SMSMESSAGESL_1, "CFacadeSmsReassemblyStore::ProcessCompleteClass0SmsMessagesL [from %d to 0]", iClass0ReassemblyStore->Entries().Count()-1);

	iClass0ReassemblyStore->BeginTransactionLC();
	TInt count = iClass0ReassemblyStore->Entries().Count();

	CSmsBuffer* buffer = CSmsBuffer::NewL();
	CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver, buffer );
	CleanupStack::PushL( smsMessage );

	while (count--)
		{
		TReassemblyEntry entry(iClass0ReassemblyStore->Entries()[count]);
		if ( entry.IsComplete() )
			{
			TRAPD( ret, iClass0ReassemblyStore->GetMessageL( count , *smsMessage ) );
			if ( ret == KErrNone )
				{
				if( !entry.PassedToClient() )
					{
						if(!(aCurrentSmsMessage && aCurrentSmsMessage->Time()==smsMessage->Time()))
						{
						TBuf16<CSmsPDUProcessor::ESmsMaxDeliverReportBufferSize> buffer;
						ret = aSmsComm.ProcessMessageL( *smsMessage, NULL, NULL, buffer );
						if ( ret == KErrNone )
							iClass0ReassemblyStore->SetPassedToClientL(entry, ETrue);
						}
					}
				}
			}
		}
	CleanupStack::PopAndDestroy( smsMessage );
	iClass0ReassemblyStore->CommitTransactionL();
	//Call this function to process those messages whose time has expired.
	iClass0ReassemblyStore->ProcessTimeoutMessageL();
	iClass0ReassemblyStore->CleanReassemblyEntries();
	}

/**
It externalizes all the enumerated messages.
It goes through the re-assembly store and sends all those SMS messages 
(which is SIM/Combined storage based) to client (aSmsProvider).

@param aSmsProvider  a reference to a service access point.
@param aCount	number of sms messages enumerated.
@return number of new segments.

@internalComponent
*/
TInt CFacadeSmsReassemblyStore::ExternalizeEnumeratedNonClass0SmsMessagesL(CSmsProvider& aProvider,TInt& aCount)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_EXTERNALIZEENUMERATEDNONCLASS0SMSMESSAGESL_1, "CFacadeSmsReassemblyStore::ExternalizeEnumeratedNonClass0SmsMessagesL()");
	TInt count = iReassemblyStore->Entries().Count();
	TInt index,numNewSegments(0);
	for(index = count-1; index >=0; --index)
		{
		TSmsReassemblyEntry entry= (TSmsReassemblyEntry&) iReassemblyStore->Entries()[index];
		if( entry.PassedToClient() == EFalse)
			{
			CSmsPDU::TSmsPDUType pdu = entry.PduType();
			CSmsBuffer* buffer = CSmsBuffer::NewL();
			CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, pdu, buffer );
			CleanupStack::PushL(smsMessage);
			iReassemblyStore->GetMessageL(index,*smsMessage);
			if (((smsMessage->Storage() == CSmsMessage::ESmsSIMStorage) || (smsMessage->Storage() == CSmsMessage::ESmsCombinedStorage)) ||
                ( (smsMessage->DecodedOnSim()) && (smsMessage->ForwardToClient())  ) )
				{
				numNewSegments+=aProvider.ExternalizeMessageL(*smsMessage,EFalse);
				for(TInt i=0; i< smsMessage->iSlotArray.Count() ;i++)
					{
					OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_EXTERNALIZEENUMERATEDNONCLASS0SMSMESSAGESL_2, "CFacadeSmsReassemblyStore::ExternalizeEnumeratedNonClass0SmsMessagesL %d", smsMessage->iSlotArray[i].iIndex);
					}
				++aCount;
				iReassemblyStore->BeginTransactionLC();
				iReassemblyStore->SetPassedToClientL(index,ETrue);
				iReassemblyStore->CommitTransactionL();
				}
			CleanupStack::PopAndDestroy(smsMessage);
			}
		}
	return numNewSegments;
	}

/**
It externalizes all the class 0 enumerated messages.
It goes through the class 0 re-assembly store and sends all those SMS messages 
(which is SIM/Combined storage based) to client (aSmsProvider).

@param aSmsProvider  a reference to a service access point.
@param aCount	number of sms messages enumerated.
@return number of new segments.

@internalComponent
*/
TInt CFacadeSmsReassemblyStore::ExternalizeEnumeratedClass0SmsMessagesL(CSmsProvider& aProvider,TInt& aCount)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_EXTERNALIZEENUMERATEDCLASS0SMSMESSAGESL_1, "CFacadeSmsReassemblyStore::ExternalizeEnumeratedClass0SmsMessagesL()");
	TInt count = iClass0ReassemblyStore->Entries().Count();
	TInt index,numNewSegments(0);
	for(index = count-1; index >=0; --index)
		{
		TReassemblyEntry entry= (TReassemblyEntry&) iClass0ReassemblyStore->Entries()[index];
		if( entry.PassedToClient() == EFalse)
			{
			CSmsPDU::TSmsPDUType pdu = entry.PduType();
			CSmsBuffer* buffer = CSmsBuffer::NewL();
			CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, pdu, buffer );
			CleanupStack::PushL(smsMessage);
			iClass0ReassemblyStore->GetMessageL(index,*smsMessage);
			if (((smsMessage->Storage() == CSmsMessage::ESmsSIMStorage) || (smsMessage->Storage() == CSmsMessage::ESmsCombinedStorage)) ||
                ( (smsMessage->DecodedOnSim()) && (smsMessage->ForwardToClient())  ) )
				{
				numNewSegments+=aProvider.ExternalizeMessageL(*smsMessage,EFalse);
				for(TInt i=0; i< smsMessage->iSlotArray.Count() ;i++)
					{
					OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_EXTERNALIZEENUMERATEDCLASS0SMSMESSAGESL_2, "CFacadeSmsReassemblyStore::ExternalizeEnumeratedClass0SmsMessagesL() %d", smsMessage->iSlotArray[i].iIndex);
					}
				++aCount;
				iClass0ReassemblyStore->BeginTransactionLC();
				iClass0ReassemblyStore->SetPassedToClientL(entry, ETrue);
				iClass0ReassemblyStore->CommitTransactionL();
				}
			CleanupStack::PopAndDestroy(smsMessage);
			}
		}
	return numNewSegments;
	}

/**
It adds the message segment to the general reassembly store.

@note Only SUBMIT or DELIVER PDUs can be added to the reassembly store.

@param aSmsMessage  a reference to the SMS message.
	It acts both as input & output. At the time of function call it contains the message.
	When the function returns it contains full decoded message.

@param aGsmSms	a reference to GsmSms object which contain actual PDU.
	It acts as pure input.

@param aIsComplete  boolean value indicating whether the message is complete or not.
	It acts both as input & output.
	In case of multi-segment message, the value of aIsComplete will be true when function returns 
	if the added segment makes the messsage complete.

@param aIsEnumeration	boolean value indicating whether the function is called at the time of enumeration.
	It acts as only input.

@param aCount  value indicating the number of current PDUs in the re-assembly store for the given SMS message.
	It acts as only output.

@param aTotal	value indicating the total number of PDUs for the given sms message.
	It acts as only output.

@internalComponent

NOTE:
	When function returns, if the value of aIsComplete is True, then aSmsMesssage will contain the
	complete decoded SMS message.
*/
void CFacadeSmsReassemblyStore::AddSegmentToNonClass0ReassemblyStoreL(CSmsMessage& aSmsMessage,const TGsmSms& aGsmSms, TInt& aIndex, TBool& aIsComplete, TBool aIsEnumeration, TInt& aCount, TInt& aTotal)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_ADDSEGMENTTONONCLASS0REASSEMBLYSTOREL_1, "CFacadeSmsReassemblyStore::AddSegmentToNonClass0ReassemblyStoreL(): isComplete Message=%d",aSmsMessage.IsComplete());

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
		iReassemblyStore->NewMessagePDUL(aIndex, aSmsMessage, aGsmSms);
		}
	else
		{
		//
		// If not yet complete, then we must be part of a multiple PDU message.
		// Search the reassembly store for existing parts of the message.
		//
		TInt  segStoreIndex(KErrNotFound);

		iReassemblyStore->MatchPDUToExistingMessage(aSmsMessage, segStoreIndex);
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_ADDSEGMENTTONONCLASS0REASSEMBLYSTOREL_2, "CFacadeSmsReassemblyStore::AddSegmentToNonClass0ReassemblyStoreL(): ""segStoreIndex=%d", segStoreIndex);

		//
		// If not yet complete, then we must be part of a multiple PDU message.
		// Search the reassembly store for existing parts of the message. This
		// may set iIsComplete to true if all segments are then found.
		//
		if (segStoreIndex != KErrNotFound)
		 	{
			TBool  isDuplicateSlot;
			TBool  isDuplicateMsgRef;
		 	//
		 	// So we found a related part of the message, add this message to the
		 	// store...
		 	//
		 	aIndex = segStoreIndex;
			iReassemblyStore->UpdateExistingMessageL(aSmsMessage, aGsmSms, aIndex,
													aIsComplete, isDuplicateMsgRef,
													isDuplicateSlot);
			OstTraceDefExt4(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_ADDSEGMENTTONONCLASS0REASSEMBLYSTOREL_3, "CFacadeSmsReassemblyStore::AddSegmentToNonClass0ReassemblyStoreL(): ""aIndex=%d, isComplete=%d, isDuplicateMsgRef=%d, isDuplicateSlot=%d",aIndex, aIsComplete, isDuplicateMsgRef, isDuplicateSlot);

			if (isDuplicateMsgRef)
				{
				//
				// In most cases discard it, unless we are doing an enumeration???
				//
				if (aIsEnumeration)
					{
					iReassemblyStore->NewMessagePDUL(aIndex, aSmsMessage, aGsmSms);
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
				iReassemblyStore->GetMessageL(aIndex, aSmsMessage);
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
				CSmsMessage*  firstMessagePdu = CSmsMessage::NewL(FileSession(),
																  CSmsPDU::ESmsDeliver, buffer);
				CleanupStack::PushL(firstMessagePdu);
				iReassemblyStore->GetMessageL(aIndex, *firstMessagePdu);
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
			iReassemblyStore->NewMessagePDUL(aIndex, aSmsMessage, aGsmSms);
			}
		}

	//
	// Update the Log Server ID and time data from the other segments.
	//
	const TSAREntry&  entry = iReassemblyStore->Entries()[aIndex];

	aSmsMessage.SetLogServerId(entry.LogServerId());
	aSmsMessage.SetTime(entry.Time());

	aCount = entry.Count();
	aTotal = entry.Total();
	}

/**
It updates log server id of the passed message in re-assembly store.

@param aSmsMessage  a reference to a message.
@param aIndex	index number of sms message to be updated.

@internalComponent
*/
void CFacadeSmsReassemblyStore::UpdateLogServerIdOfNonClass0MessageL(const CSmsMessage& aSmsMessage, TInt aIndex)
	{
	//
	// Find the message in the reassembly store...
	//
    TInt  foundIndex(KErrNotFound);
	TBool  found(EFalse);
    
	if (iReassemblyStore->InTransaction())
		{
		found = iReassemblyStore->FindMessageL(aSmsMessage, EFalse, foundIndex);
		}
	else
		{
		iReassemblyStore->BeginTransactionLC();
		found = iReassemblyStore->FindMessageL(aSmsMessage, EFalse, foundIndex);
		iReassemblyStore->CommitTransactionL();
		}

	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_UPDATELOGSERVERIDOFNONCLASS0MESSAGEL_1, "CFacadeSmsReassemblyStore::UpdateLogServerIdOfNonClass0MessageL(): found=%d, foundIndex=%d",found, foundIndex);

	//
	// If found and the index is valid, then update the Log Server ID...
	//
	if (found  &&  aIndex == foundIndex)
		{
		iReassemblyStore->UpdateLogServerIdL(aIndex,
											aSmsMessage.LogServerId());
		}
	}

/**
 *  Checks whether a message is a WAP message.
 *  
 */
TBool CFacadeSmsReassemblyStore::IsWapSMS(const CSmsMessage& aSmsMessage)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CFACADESMSREASSEMBLYSTORE_ISWAPSMS_1, "CFacadeSmsReassemblyStore::IsWapSMS()");

	return CSmsProtocol::IsAppPortSMS(aSmsMessage);
	}
