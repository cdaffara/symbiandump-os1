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
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "smspprocTraces.h"
#endif

#include "smspproc.h"
#include "smspmondsk.h"
#include "smspfacadestor.h"
#include "smsuset.h"
#include "smsulog.h"
#include "smspmain.h"

#include <gsmusar.h>
#include <exterror.h>
#include "gsmubuf.h"
#include "Gsmumsg.h"
#include "smspcdb.h"
#include "smspread.h"


/**
 *  Creates new CSmsPDUProcess instance
 *  
 *  @return a new CSmsPDUProcess object.
 */
CSmsPDUProcessor* CSmsPDUProcessor::NewL(MSmsComm& aSmsComm, const TSmsSettings& aSmsSettings,
										 CFacadeSmsReassemblyStore& aReassemblyStore,
										 CSmsSegmentationStore& aSegmentationStore,
										 CSmsMonitorDiskSpace& aSmsMonitorDiskSpace)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_NEWL_1, "CSmsPDUProcessor::NewL()");

	CSmsPDUProcessor*  smsPDUProcessor = new (ELeave) CSmsPDUProcessor(aSmsComm,
																	   aSmsSettings,
																	   aReassemblyStore,
																	   aSegmentationStore,
																	   aSmsMonitorDiskSpace);

	CleanupStack::PushL(smsPDUProcessor);
	smsPDUProcessor->ConstructL();
	CleanupStack::Pop(smsPDUProcessor);

	return smsPDUProcessor;
	} // CSmsPDUProcessor::NewL


CSmsPDUProcessor::~CSmsPDUProcessor()
	{
	delete iSmsMessage;
	delete iOriginalSmsMessage;
	iStatusArray.Close();
	} // CSmsPDUProcessor::~CSmsPDUProcessor


CSmsPDUProcessor::CSmsPDUProcessor(MSmsComm& aSmsComm,
								   const TSmsSettings& aSmsSettings,
								   CFacadeSmsReassemblyStore& aReassemblyStore,
								   CSmsSegmentationStore& aSegmentationStore,
								   CSmsMonitorDiskSpace& aSmsMonitorDiskSpace)
	:iSmsComm(aSmsComm),
	iSmsSettings(aSmsSettings),
	iReassemblyStore(aReassemblyStore),
	iSegmentationStore(aSegmentationStore),
	iSmsMonitorDiskSpace(aSmsMonitorDiskSpace)
	{
	// NOP
	} // CSmsPDUProcessor::CSmsPDUProcess


/**
 *  Second phase constructor.
 */
void CSmsPDUProcessor::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_CONSTRUCTL_1, "CSmsPDUProcessor::ConstructL()");
	} // CSmsPDUProcessor::ConstructL


void CSmsPDUProcessor::DecodeAndProcessPDUL(TGsmSmsSlot& aSlot, TBool aIsEnumeration)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_DECODEANDPROCESSPDUL_1, "CSmsPDUProcessor::DecodeAndProcessPDUL(): aIsEnumeration=%d",aIsEnumeration);

	//
	// Store the slot...
	//
	iSlot = aSlot;
	
	//
	// We need to know if this message is MT or MO before decoding...
	//
	if (iSlot.iMsgStatus == RMobileSmsStore::EStoredMessageUnread  ||
		iSlot.iMsgStatus == RMobileSmsStore::EStoredMessageRead)
		{
		iIsMobileTerminated = ETrue;
		}

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_DECODEANDPROCESSPDUL_2, "CSmsStoreRead::DecodeAndProcessPDUL(): iIsMobileTerminated=%d",iIsMobileTerminated);

	//
	// Put the PDU and Service Center Address in the TGsmSms structure...
	//
	TGsmSmsTelNumber  sca;

	sca.iTelNumber = iSlot.iServiceCentre.iTelNumber;
	sca.iTypeOfAddress.SetFromETelMM(static_cast<NMobilePhone::TMobileTON>(iSlot.iServiceCentre.iTypeOfNumber),
				 					 static_cast<NMobilePhone::TMobileNPI>(iSlot.iServiceCentre.iNumberPlan));
	iGsmSms.SetPdu(iSlot.iMsgData);
	iGsmSms.SetSca(sca);

	//
	// Allocate a CSmsBuffer for use during the decoding of the PDU and
	// create the CSmsMessage object - this will actaully decode the PDU. Note
	// that the CSmsBuffer ownership is transfered during NewL()!!!
	//
	CSmsBuffer*  smsBuffer = CSmsBuffer::NewL();
	iSmsMessage = CSmsMessage::NewL(iReassemblyStore.FileSession(), iGsmSms,
									smsBuffer, EFalse, iIsMobileTerminated);
	
	//
	// Store the enumeration type and calculate other basic info...
	//
	iIsEnumeration = aIsEnumeration;
	
	AnalysePDUCharacteristics();

	//
	// If the store information is supplied, then check if the PDU will
	// be deleted from the store.
	//
    // When enumerating, only messages which are on the SIM or in the
    // Combined Store are forwarded to the client. If the message on the
    // SIM is marked for automatic deletion or a type 0, then it may
    // have its storage set to CSmsMessage::ESmsNoStorage in this routine.
    // However the method  CSmsProtocol::ExternalizeEnumeratedMessagesL()
    // still needs to know that the message was originally on the SIM or
    // the combined store.
	//
	if (iSlot.iStore.Length() > 0)
		{
		iSmsMessage->SetStatus(static_cast<NMobileSmsStore::TMobileSmsStoreStatus>(iSlot.iMsgStatus));
		if (iSlot.iStore == KETelCombinedSmsStore)
			{
			if (iSmsSettings.DeletePDUsFromCombinedStores() == EFalse)
				{
				iSmsMessage->SetStorage(CSmsMessage::ESmsCombinedStorage);
				}

		    iSmsMessage->SetDecodedOnSIM(ETrue);
			}
 		else if (iSlot.iStore == KETelIccSmsStore)
			{
			if (iSmsSettings.DeletePDUsFromSIM() == EFalse)
				{
				iSmsMessage->SetStorage(CSmsMessage::ESmsSIMStorage);
				}

		    iSmsMessage->SetDecodedOnSIM(ETrue);
			}
		else if (iSmsSettings.DeletePDUsFromPhoneStores() == EFalse)
			{
			iSmsMessage->SetStorage(CSmsMessage::ESmsPhoneStorage);
			}
		}

	//
	// If the PDU will be deleted, then assume it has no storage...
	//
	if (iIsPDUToBeDeleted)
	    {
	    iSmsMessage->SetStorage(CSmsMessage::ESmsNoStorage);
	    }

	//
	// Is this a MOSES message?
	//
	if (iSmsMessage->ToFromAddress().CompareF(KNETWORK) == 0)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_DECODEANDPROCESSPDUL_3, "CSmsPDUProcessor::DecodeAndProcessPDUL(): MOSES OTE message!");
		if (iSmsComm.NetworkInfoAvailable())
			{
			iSmsMessage->SetToFromAddressL(iSmsComm.NetworkInfo().iDisplayTag);
			}
		}

	//
	// Call the processing function for this time of message. Enumerations of
	// a PDU can be SUBMIT, DELIVER or STATUS-REPORT. Receiving of a PDU can
	// be SUBMIT or STATUS-REPORT...
	//
	if ((iIsEnumeration  &&  iSmsMessage->Type() == CSmsPDU::ESmsSubmit)  ||
		iSmsMessage->Type() == CSmsPDU::ESmsDeliver)
		{
		if (iIsForwardMessageToClient)
			{
			//
			// Record that this SMS should be forwarded to the client...
			//
			iSmsMessage->SetForwardToClient(ETrue);

			//
			// Check that there is disk space to process this PDU. This may signal to
			// the network to stop SMS reception until further notice, and it may go
			// active to monitor the disk space. 
			//
			/*
			The below logic is quite complicated. This logic is written to support backward compatibility.
			Previously SMS stack was sending -ve ack for class 0 messages before implementation of REQ6523.
			Even after implementation of REQ6523, there will be provision so that the customer who does
			not want this feature, then can go back to previous implementation.
			With REQ6523, there is another requiremnt REQ7012 which states that class 0 WAP messages
			will be handled like other non-class 0 messages. That means in out-of-disk condition it will
			send -ve ack to network. WAP class 0 messages will be stored in non-class 0 re-assembly store.

			If SMS stack is configured for handling class 0 messages in out-of-disk condition,
			Class 0 messages will be stored in reserved space (Pre-allocated file) internally.
			But if this function is called at the time of enumeration and disk is full then -ve ack 
			will be sent to client for disk full. The behaviour is different for class 0 messages
			at the time of enumeration & normal read from network.
			*/
			TInt ret=KErrNone;
			iIsWapSms = iReassemblyStore.IsWapSMS(*iSmsMessage);
			if (iIsClass0Message && iIsWapSms)
				{
				//Wap class 0 sms message will be stored in non-class 0 reassembly store.
				TRAP(ret, iSmsMonitorDiskSpace.CheckDiskSpaceForPDUL(EFalse));
				}
			else
				{
				TRAP(ret, iSmsMonitorDiskSpace.CheckDiskSpaceForPDUL(iIsClass0Message));
				}

			if ((iIsClass0Message && !iIsWapSms) && iReassemblyStore.IsSeparateClass0StoreSupported() && (!aIsEnumeration))
				{
				if (ret == KErrDiskFull)
					{
					iReassemblyStore.SetDiskSpaceState(ESmsDiskSpaceFull);
					ret = KErrNone;
					}
				else if (ret == KErrNone)
					{
					iReassemblyStore.SetDiskSpaceState(ESmsDiskSpaceAvailable);
					}
				}

			if (ret!=KErrNone)
				{
				User::Leave(ret);
				}

			AddSlotToSmsMessageIfRequiredL();
			AddSegmentOfMessageToReassemblyStoreIfRequiredL();
			}
		}
	else if (iSmsMessage->Type() == CSmsPDU::ESmsStatusReport)
		{
		FindOriginalMessageAndProcessStatusReportL();

		if (iIsForwardMessageToClient  &&  iIsMessageGoingToBeStored)
		    {
			//
			// Record that this SMS should be forwarded to the client...
			//
	        iSmsMessage->SetForwardToClient(ETrue);

			AddSlotToSmsMessageIfRequiredL();
  	    	AddSegmentOfMessageToReassemblyStoreIfRequiredL();
			}

		//
		// If the message to which this report refers is complete (and
		// logically most will be as unless it refers to a SMS-SUBMIT
		// message we no longer have), then update the received time and
		// the 
		//
		if (iIsComplete)
			{
			UpdateStatusReportL();
			}
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	
	//
	// Should we delete the PDU?
	//
    if ((iIsEnumeration  &&  iSlot.iStore == KETelMeSmsStore)  ||
        (iIsEnumeration == EFalse  &&  iSmsMessage->Storage() != CSmsMessage::ESmsSIMStorage)  ||
    	iIsPDUToBeDeleted)
        {
 		DeletePDUL();
	    }
	} // CSmsPDUProcessor::DecodeAndProcessPDUL


void CSmsPDUProcessor::AnalysePDUCharacteristics()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_ANALYSEPDUCHARACTERISTICS_1, "CSmsPDUProcessor::AnalysePDUCharacteristics()");

	CSmsPDU&  pdu = iSmsMessage->SmsPDU();

	//
	// Check store the message class...
	//
	TSmsDataCodingScheme::TSmsClass  msgClass;

    if (pdu.DataCodingSchemePresent()  &&  pdu.Class(msgClass))
        {
        if (msgClass == TSmsDataCodingScheme::ESmsClass0)
        	{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_ANALYSEPDUCHARACTERISTICS_2, "CSmsPDUProcessor::AnalysePDUCharacteristics(): Class 0");
	    	iIsClass0Message = ETrue;
        	}
        else if (msgClass == TSmsDataCodingScheme::ESmsClass1)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_ANALYSEPDUCHARACTERISTICS_3, "CSmsPDUProcessor::AnalysePDUCharacteristics(): Class 1");
			iIsClass1Message = ETrue;
			}
        else if (msgClass == TSmsDataCodingScheme::ESmsClass2)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_ANALYSEPDUCHARACTERISTICS_4, "CSmsPDUProcessor::AnalysePDUCharacteristics(): Class 2");
			iIsClass2Message = ETrue;
			}
        else if (msgClass == TSmsDataCodingScheme::ESmsClass3)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_ANALYSEPDUCHARACTERISTICS_5, "CSmsPDUProcessor::AnalysePDUCharacteristics(): Class 3");
			iIsClass3Message = ETrue;
			}
		else
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_ANALYSEPDUCHARACTERISTICS_6, "CSmsPDUProcessor::AnalysePDUCharacteristics(): Class Unknown!");
			}
		}
	else
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_ANALYSEPDUCHARACTERISTICS_7, "CSmsPDUProcessor::AnalysePDUCharacteristics(): Class-less");
		}

	//
	// Is this PDU PID Type 0?
	//
	if (pdu.ProtocolIdentifierPresent()  &&
	    pdu.PIDType() == TSmsProtocolIdentifier::ESmsPIDShortMessageType  &&
	    pdu.ShortMessageType() == TSmsProtocolIdentifier::ESmsShortMessageType0)
	   {
	   iIsPIDType0 = ETrue;
	   }

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_ANALYSEPDUCHARACTERISTICS_8, "CSmsPDUProcessor::AnalysePDUCharacteristics(): iIsPIDType0=%d",iIsPIDType0);

	//
	// Should this message be forwarded to the client?
	//
    if (!(iIsPIDType0  &&
	      ((iIsClass0Message  &&  iOptionDiscardType0Class0)  ||
		   (iIsClass2Message  &&  iOptionDiscardType0Class2)  ||
		   (iIsClass0Message == EFalse  &&  iIsClass2Message == EFalse))))
        {
		iIsForwardMessageToClient = ETrue;
		}

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_ANALYSEPDUCHARACTERISTICS_9, "CSmsPDUProcessor::AnalysePDUCharacteristics(): iIsForwardMessageToClient=%d",iIsForwardMessageToClient);

	//
	// Is it flaged for auto-delete?
	//
    if (pdu.DataCodingSchemePresent())
        {
        TInt  autoDelete = pdu.Bits7To4() & TSmsDataCodingScheme::ESmsDCSAutomaticDeletionMask;

        if (autoDelete == TSmsDataCodingScheme::ESmsDCSAutomaticDeletion)
            {
            iIsMarkedForAutoDelete = ETrue;
            }
        }

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_ANALYSEPDUCHARACTERISTICS_10, "CSmsPDUProcessor::AnalysePDUCharacteristics(): iIsMarkedForAutoDelete=%d",iIsMarkedForAutoDelete);

	//
	// Should this PDU be deleted after processing?
	//
 	if (iIsMarkedForAutoDelete  &&
 	    (iIsClass2Message == EFalse  ||  iOptionApplyAutoDeletionToClass2))
 		{
 		iIsPDUToBeDeleted = ETrue;
 		}
 	else if (iIsPIDType0  &&
			 (iIsClass2Message == EFalse  ||  iOptionDiscardType0Class2))
 		{
 		iIsPDUToBeDeleted = ETrue;
 		}

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_ANALYSEPDUCHARACTERISTICS_11, "CSmsPDUProcessor::AnalysePDUCharacteristics(): iIsPDUToBeDeleted=%d",iIsPDUToBeDeleted);

	//
	// Does the message need to be stored???
	//
	// Don't store class 2 SMSs with index 0 on SIM. Not sure why, but
	// it appears to be delibrate and no one has ever complained!
	//
	// Don't store messages already stored somewhere other than the
	// phone store.
	//
	if (iIsPDUToBeDeleted == EFalse  &&
		((iIsEnumeration  &&  iSlot.iIndex != 0  &&  iSlot.iStore != KETelMeSmsStore)  ||
	     (iSlot.iStore != KETelMeSmsStore  &&  iSlot.iStore.Length() > 0)))
		{
		iIsMessageGoingToBeStored = ETrue;
		}

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_ANALYSEPDUCHARACTERISTICS_12, "CSmsPDUProcessor::AnalysePDUCharacteristics(): iIsMessageGoingToBeStored=%d",iIsMessageGoingToBeStored);

	//
	// Is the message complete? This value may change later, when the segmentation and
	// reassembley stores are examined.
	//
	iIsComplete = iSmsMessage->IsComplete();

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_ANALYSEPDUCHARACTERISTICS_13, "CSmsPDUProcessor::AnalysePDUCharacteristics(): iIsComplete=%d",iIsComplete);
	
	//
	// Store PDU Data. These values may be updated later.
	//
	iSmsPDUData.iType     = iSmsMessage->Type();
	iSmsPDUData.iTotal    = 1;
	iSmsPDUData.iReceived = 1;
	} // CSmsPDUProcessor::AnalysePDUCharacteristics


void CSmsPDUProcessor::FindOriginalMessageAndProcessStatusReportL()
	{
	__ASSERT_DEBUG(iSmsMessage->Type() == CSmsPDU::ESmsStatusReport, 
				   SmspPanic(KSmspPanicNotStatusReport));

	//
	// Find the original message in the segmentation store and update its status
	// to that contained in the status report.
	//
	TBool  found = iSegmentationStore.AddStatusReportL(iIndex, iIsComplete, *iSmsMessage);
	
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_FINDORIGINALMESSAGEANDPROCESSSTATUSREPORTL_1, "CSmsPDUReadProcess::FindOriginalMessageAndProcessStatusReportL(): found=%d",found);

	if (found)
		{
		//
		// The status report refers to a known message. Update the this status message
		// with the correct Log Server ID and it's related info.
		//
		const TSmsSegmentationEntry&  entry = (const TSmsSegmentationEntry&)
												iSegmentationStore.Entries()[iIndex];

		iSmsMessage->SetLogServerId(entry.LogServerId());

		iSmsPDUData.iType      = entry.PduType();
		iSmsPDUData.iTotal     = entry.Total();
		iSmsPDUData.iSent      = entry.Count();
		iSmsPDUData.iDelivered = entry.Delivered();
		iSmsPDUData.iFailed    = entry.Failed();

		//
		// Load the original SUBMIT message into memory for further processing later...
		// Note that smsBuffer's ownership transfers straight away!!!
		//
		CSmsBuffer*  smsBuffer = CSmsBuffer::NewL();

		iOriginalSmsMessage = CSmsMessage::NewL(iReassemblyStore.FileSession(),
												CSmsPDU::ESmsSubmit,
												smsBuffer);
		iSegmentationStore.GetMessageL(iIndex, iSmsAddr, *iOriginalSmsMessage,
									   iStatusArray);
		}
	} // CSmsPDUProcessor::FindOriginalMessageAndProcessStatusReportL


void CSmsPDUProcessor::UpdateStatusReportL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_UPDATESTATUSREPORTL_1, "CSmsPDUProcessor::UpdateStatusReportL()");

	//
	// Update the receive time of the status report...
	//
	TTime  currTime;
	currTime.UniversalTime();

	iSmsMessage->SetTime(currTime);
	iSmsMessage->SetUTCOffset(User::UTCOffset());
	iSmsMessage->SetToFromAddressL(iOriginalSmsMessage->ToFromAddress());

	//
	// Set the status report to the last status received by the SME...
	//
	CSmsStatusReport&  statusReport = (CSmsStatusReport&) iSmsMessage->SmsPDU();
	TInt  count = iStatusArray.Count();

	for (TInt index = 0;  index < count;  index++)
		{
		TInt  status = iStatusArray[index].Status();

		statusReport.SetStatus((TSmsStatus::TSmsStatusValue) status);
		if (status != TSmsStatus::ESmsShortMessageReceivedBySME)
			{
			break;
			}
		}
	} // CSmsPDUProcessor::UpdateStatusReportL


void CSmsPDUProcessor::AddSlotToSmsMessageIfRequiredL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_ADDSLOTTOSMSMESSAGEIFREQUIREDL_1, "CSmsPDUProcessor::AddSlotToSmsMessageIfRequiredL()");

	//
	// Add the slot to the message (if it is not going to be deleted and
	// the client needs to know it's location e.g. to store it)...
	//
	if (iIsMessageGoingToBeStored)
		{
		TGsmSmsSlotEntry  smsSlotEntry;

		smsSlotEntry.iIndex = iSlot.iIndex;
		smsSlotEntry.iStore = iSlot.iStore;

		iSmsMessage->AddSlotL(smsSlotEntry);
		}
	} // CSmsPDUProcessor::AddSlotToSmsMessageIfRequiredL


/**
 *  Add the message segment to the reassembly store. There are 5 possiblities:
 *
 *   1) This is the complete message (e.g. a single-segment message).
 *      We therefore have all the segments.
 *   2) This is a duplicate message segment.
 *      We will ignore it.
 *   3) This is the last segment in the message required to complete it.
 *      The other segments are already stored.
 *   4) This is another PDU to an existing message in the store, but it is
 *      not yet complete.
 *   5) This is the first PDU in the message, and therefore the message is
 *      not yet complete and no segments are stored.
 *
 *  @note Only SUBMIT or DELIVER PDUs can be added to the reassembly store.
 */
void CSmsPDUProcessor::AddSegmentOfMessageToReassemblyStoreIfRequiredL()
	{
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_ADDSEGMENTOFMESSAGETOREASSEMBLYSTOREIFREQUIREDL_1, "CSmsPDUReadProcess::AddSegmentOfMessageToReassemblyStoreIfRequiredL(): iIsComplete=%d",iIsComplete);

	iReassemblyStore.AddSegmentToReassemblyStoreL(*iSmsMessage, iGsmSms, iIndex, iIsComplete, iIsEnumeration, iSmsPDUData.iReceived, iSmsPDUData.iTotal);
	} // CSmsPDUProcessor::AddSegmentOfMessageToReassemblyStoreIfRequiredL


void CSmsPDUProcessor::UpdateLogServerIdL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_UPDATELOGSERVERIDL_1, "CSmsPDUProcessor::UpdateLogServerIdL()");

	//
	// If this is a SUBMIT or DELIVER PDU,
	// then search for it in the Reassembly store.
	//
	if (iSmsMessage != NULL  &&  iIndex != KErrNotFound  &&
		(iSmsMessage->Type() == CSmsPDU::ESmsSubmit  ||
		iSmsMessage->Type() == CSmsPDU::ESmsDeliver))
		{
		iReassemblyStore.UpdateLogServerIdL(*iSmsMessage, iIndex);
		}
	} // CSmsPDUProcessor::UpdateLogServerIdL

void CSmsPDUProcessor::ProcessMessageIfCompleteL()
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_PROCESSMESSAGEIFCOMPLETEL_1, "CSmsPDUProcessor::ProcessMessageIfCompleteL()");

	if ((iIsClass0Message && !iIsWapSms) && iReassemblyStore.IsSeparateClass0StoreSupported())
		{
		//Note: Process Class 0 Message which is not wap message & return.
		if (iIsComplete)
			{
			//
			// Process the message...
			//
			if (iOriginalSmsMessage == NULL)
				{
				iReassemblyStore.ForwardCompleteClass0SmsMessagesL(iSmsComm, *iSmsMessage, NULL, iOriginalSmsMessage, iDeliverReportBuffer);
				}
			else
				{
				iReassemblyStore.ForwardCompleteClass0SmsMessagesL(iSmsComm, *iSmsMessage, &iSmsAddr, iOriginalSmsMessage, iDeliverReportBuffer);
				}
			}
		else
			{
			iReassemblyStore.ProcessMessageIfExceedLimitationL(iSmsComm);
			}
		return;
		}

	if (iIsComplete)
		{
		//
		// Process the message...
		//
		TInt  ret(KErrUnknown);

		if (iOriginalSmsMessage == NULL)
			{
			ret = iSmsComm.ProcessMessageL(*iSmsMessage, NULL,
										   iOriginalSmsMessage,
										   iDeliverReportBuffer);
			}
		else
			{
			ret = iSmsComm.ProcessMessageL(*iSmsMessage, &iSmsAddr,
										   iOriginalSmsMessage,
										   iDeliverReportBuffer);
			}

		//
		// If successful, mark the entry as delivered to the msgc...
		//
 		if(ret == KErrNone)
 			{
 			iReassemblyStore.SetMessagePassedToClientL(*iSmsMessage);
 			}
		}
	} // CSmsPDUProcessor::ProcessMessageIfCompleteL

void CSmsPDUProcessor::DeletePDUL()
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUPROCESSOR_DELETEPDUL_1, "CSmsPDUProcessor::DeletePDUL()");

	//
	// If the slot number has an index and store assigned, then we can delete
	// it.
	//
	if (iSlot.iIndex >= 0  &&  iSlot.iStore.Length() > 0)
		{
		CArrayFixFlat<TGsmSmsSlotEntry>*  smsSlotEntryArray = new (ELeave) CArrayFixFlat<TGsmSmsSlotEntry>(1);
		CleanupStack::PushL(smsSlotEntryArray);

		TGsmSmsSlotEntry smsSlotEntry;

		smsSlotEntry.iIndex = iSlot.iIndex;
		smsSlotEntry.iStore = iSlot.iStore;
		smsSlotEntryArray->AppendL(smsSlotEntry);

		iSmsComm.DeletePDUs(*smsSlotEntryArray, NULL);

		CleanupStack::PopAndDestroy(smsSlotEntryArray);
		}
	} // CSmsPDUProcessor::DeletePDUL

