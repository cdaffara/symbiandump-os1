// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation for the CSmsuActiveBase based class CSmsMessageSend.
// 
//

/**
 @file
 @internalAll
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "smspsendTraces.h"
#endif

#include "smspsend.h"
#include "smspstor.h"
#include "smspmain.h"
#include "smsuset.h"
#include "smsulog.h"
#include "exterror.h"
#include "gsmubuf.h"
#include "SmsuTimer.h"
#include <logwraplimits.h>


/** 
 *  Static object constructor for CSmsMessageSend.
 *
 *  @param aSegmentationStore  Segmentation Store.
 *  @param aSmsMessaging       ETel SMS messaging subsession.
 *  @param aSmsSettings        The global SMS settings.
 *  @param aMobileSmsCaps      SMS messaging capabilities of TSY/phone.
 *  @param aPriority           Priority to use for the Active Object.
 *  @param aSmspSetBearer      Set bearer object.
 */
CSmsMessageSend* CSmsMessageSend::NewL(CSmsSegmentationStore& aSegmentationStore,
									   const TSmsSettings& aSmsSettings,
									   const RMobileSmsMessaging::TMobileSmsCapsV1& aMobileSmsCaps,
									   RMobileSmsMessaging& aSmsMessaging,
									   TInt aPriority,
									   CSmspSetBearer& aSmspSetBearer)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGESEND_NEWL_1, "CSmsMessageSend::NewL()");

	CSmsMessageSend*  self = new (ELeave) CSmsMessageSend(aSegmentationStore,
														  aSmsSettings,
														  aMobileSmsCaps,
														  aSmsMessaging,
														  aPriority,
														  aSmspSetBearer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
	} // CSmsMessageSend::NewL


/**
 *  2nd Phase of construction.
 *  
 *  Creates CSmsTimeout object and the SMS event logger.
 */
void CSmsMessageSend::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGESEND_CONSTRUCTL_1, "CSmsMessageSend::ConstructL()");

	iSmsEventLogger = CSmsEventLogger::NewL(iSegmentationStore.FileSession(),
											Priority());

	ConstructTimeoutL();
	} // CSmsMessageSend::ConstructL


/** 
 *  Default constructor.
 *
 *  @param aSegmentationStore  Segmentation Store.
 *  @param aSmsSettings        The global SMS settings.
 *  @param aMobileSmsCaps      SMS messaging capabilities of TSY/phone.
 *  @param aSmsMessaging       ETel SMS messaging subsession.
 *  @param aPriority           Priority to use for the Active Object.
 *  @param aSmspSetBearer      Set bearer object.
 */
CSmsMessageSend::CSmsMessageSend(CSmsSegmentationStore& aSegmentationStore,
								 const TSmsSettings& aSmsSettings,
								 const RMobileSmsMessaging::TMobileSmsCapsV1& aMobileSmsCaps,
								 RMobileSmsMessaging& aSmsMessaging,
								 TInt aPriority,
								 CSmspSetBearer& aSmspSetBearer)
  : CSmsuActiveBase(aPriority),
	iState(ESmsMessageSendIdle),
	iSegmentationStore(aSegmentationStore),
	iSmspSetBearer(aSmspSetBearer),
	iSmsSettings(aSmsSettings),
	iMobileSmsCaps(aMobileSmsCaps),
	iSmsMessaging(aSmsMessaging),
	iMobileSmsSendAttributesV1Pckg(iMobileSmsSendAttributesV1),
	iSmsArray(8),
	iSegmentSequenceNumber(-1)
	{
	// NOP
	} // CSmsMessageSend::CSmsMessageSend


/** 
 *  Standard destructor.
 */
CSmsMessageSend::~CSmsMessageSend()
	{
	Cancel();
	delete iSmsEventLogger;
	} // CSmsMessageSend::~CSmsMessageSend


void CSmsMessageSend::Start(CSmsMessage& aSmsMessage, TInt aOptions,
							const TSmsAddr& aSmsAddr, TRequestStatus& aStatus)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGESEND_START_1, "CSmsMessageSend::Start()");

	__ASSERT_DEBUG(iState == ESmsMessageSendIdle,SmspPanic(KSmspPanicUnexpectedState));

	//
	// Store the request status that we will complete when finished...
	//
	Queue(aStatus);

	//
	// Initialise member data...
	//
	iSmsMessage=&aSmsMessage;
	iSmsMessage->ParsedToFromAddress(iToFromTelNumber);
	iOptions = aOptions;
	iSmsAddr = aSmsAddr;

	iSmsPDUData.iType      = aSmsMessage.Type();
	iSmsPDUData.iTotal     = 0;
	iSmsPDUData.iSent      = 0;
	iSmsPDUData.iDelivered = 0;

	iSendError = KErrNone;
	iSmsEventLogger->Event().SetId(KLogNullId);

	//
	// Begin by setting the bearer...
	//
	iState = ESmsMessageSendSetBearer;
	iSmspSetBearer.NotifyBearerSet(iStatus);
	SetActive();
	} // CSmsMessageSend::Start


void CSmsMessageSend::DoRunL()
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGESEND_DORUNL_1, "CSmsMessageSend::RunL(): iStatus=%d, iState=%d", iStatus.Int(), iState);

	switch (iState)
		{
		case ESmsMessageSendSetBearer:
			{
			//
			// If setting of bearer was successful then send the message.
			//
			// The TSY will complete with KErrNotSupported if it doesn't
			// support setting the send bearer.
			//
			if (iStatus.Int() == KErrNone  ||
				iStatus.Int() == KErrNotSupported)
				{
				if (iSmsEventLogger->ClientAvailable())
					{
					CreateLogServerEvent();
					}
				else
					{
					SegmentMessage();
					}
				}
			}
			break;

		case ESmsMessageSendCreatingLogServerEvent:
			{
			//
			// If a log event was created ok, then store the id...
			//
			if (iStatus.Int() == KErrNone)
				{
				iSmsMessage->SetLogServerId(iSmsEventLogger->Event().Id());
				}
			else
				{
				iSmsEventLogger->Event().SetId(KLogNullId);
				}

			//
			// Segment the message. This will send the first PDU as well...
			//
			SegmentMessage();
			}
			break;

		case ESmsMessageSendSegmentingMessage:
			{
			//
			// If the previous PDU was sent ok, then send the next one...
			//
			if (iStatus.Int() == KErrNone)
				{
				SendNextPDU();
				}
			else
				{
				//
				// An error occured. Save the error for later and either
				// update the log event, or drop to the end of RunL() so
				// that Complete() is called...
				//
				iSendError = iStatus.Int();

				if (iSmsEventLogger->Event().Id() != KLogNullId)
					{
					UpdateLogServerEvent();
					}
				}
			}
			break;

		case ESmsMessageSendPDU:
			{
			//
			// If the PDU is sent with an acknowledgement, then decode the
			// submit report...
			//
			if (iMobileSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsSendWithAck  &&
				iMobileSmsSendAttributesV1.iFlags & RMobileSmsMessaging::KGsmSubmitReport)
				{
				DecodeSubmitReportL();
				}

			if (iStatus.Int() == KErrNone  &&  !IsRPError())
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGESEND_DORUNL_2, "CSmsMessageSend::RunL(): Submit ACK'd MsgRef=%d",iMobileSmsSendAttributesV1.iMsgRef);
				}
			else if (IsRPError())
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGESEND_DORUNL_3, "CSmsMessageSend::RunL(): Submit N'ACK'D MsgRef=%d",iMobileSmsSendAttributesV1.iMsgRef);
				}
			else
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGESEND_DORUNL_4, "CSmsMessageSend::RunL(): MsgRef=%d",iMobileSmsSendAttributesV1.iMsgRef);
				}

			//
			// If the PDU was sent okay, then update the segmentation store...
			//
			if (iStatus.Int() ==KErrNone)
				{
				iSegmentSequenceNumber++; 
				UpdateSegmentationStore();
				}
			else
				{
				//
				// An error occured. Save the error for later and either
				// update the log event, or drop to the end of RunL() so
				// that Complete() is called...
				//
				iSendError = iStatus.Int();

				if (iSmsEventLogger->Event().Id() != KLogNullId)
				 	{
				 	UpdateLogServerEvent();
				 	}
				}
			}
			break;

		case ESmsMessageSendUpdatingSegmentationStore:
			{
			if (iStatus.Int() == KErrNone)
				{
				//
				// The update has been successful, so update the log event or
				// send the next PDU. If the log event is updated, then it
				// will then send the next PDU...
				// 
				if (iSmsEventLogger->Event().Id() != KLogNullId)
					{
					UpdateLogServerEvent();
					}
				else
					{
					SendNextPDU();
					}
				}
			else
				{
				//
				// An error occured. Save the error for later and either
				// update the log event, or drop to the end of RunL() so
				// that Complete() is called...
				//
				iSendError = iStatus.Int();

				if (iSmsEventLogger->Event().Id() != KLogNullId)
					{
					UpdateLogServerEvent();
					}
				}
			}
			break;

		case ESmsMessageSendUpdatingLogEvent:
			{
			//
			// Ignore log server error and continue with the next PDU if
			// the last send was okay, otherwise drop to the bottom of the
			// RunL() and Complete().
			//
			if (iSendError == KErrNone)
				{
				SendNextPDU();
				}
			}
			break;

		default:
			{
			SmspPanic(KSmspPanicUnexpectedState);
			}
			break;
		};

	//
	// DoRunL() will now return to CSmsuActiveBase which if the object
	// is not active, will call Complete().
	//
	} // CSmsMessageSend::DoRunL


void CSmsMessageSend::DoCancel()
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGESEND_DOCANCEL_1, "CSmsMessageSend::DoCancel(): iState=%d", iState);

	TimedSetActiveCancel();

	switch (iState)
		{
		case ESmsMessageSendSetBearer:
			{
			iSmspSetBearer.Cancel();
			}
			break;

		case ESmsMessageSendSegmentingMessage:
			{
			// NOP
			}
			break;

		case ESmsMessageSendCreatingLogServerEvent:
		case ESmsMessageSendUpdatingLogEvent:
			{
			iSmsEventLogger->Cancel();
			}
			break;

		case ESmsMessageSendPDU:
			{
			iSmsMessaging.CancelAsyncRequest(EMobileSmsMessagingSendMessage);
			}
			break;

		case ESmsMessageSendUpdatingSegmentationStore:
			{
			// NOP
			}
			break;

		default:
			{
			SmspPanic(KSmspPanicUnexpectedState);
			}
			break;
		};

	iSendError = KErrCancel;

	//
	// Handle completion of this Active Object. Note that the object
	// may well still be active at this point...
	//
	if (TimedOut())
		{
		Complete(KErrTimedOut);
		}
	else
		{
		Complete(KErrCancel);
		}
	} // CSmsMessageSend::DoCancel


/**
 *  Decodes SMS-SUBMIT-REPORT PDU and checks if it is SMS-SUBMIT-REPORT for
 *  RP-ERROR.
 *  
 *  @leave Leaves if CSmsBuffer::NewL or CSmsMessage::NewL leaves.
 *  
 */
void CSmsMessageSend::DecodeSubmitReportL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGESEND_DECODESUBMITREPORTL_1, "CSmsMessageSend::DecodeSubmitReportL()");

	//
	// Only try to decode the submit report if error is a RP-error, in which case there should be a valid PDU
	//
	if (IsRPError())
		{
		TGsmSms sms;
		sms.SetPdu(iMobileSmsSendAttributesV1.iSubmitReport);
		CSmsBuffer* buffer=CSmsBuffer::NewL();

		// try to create a sms message from the submit report PDU (smsMessage takes ownership of the buffer)
		CSmsMessage*  smsMessage = CSmsMessage::NewL(iSegmentationStore.FileSession(),
													 sms,buffer,IsRPError(),ETrue);
		CleanupStack::PushL(smsMessage);

		if (smsMessage->SmsPDU().Type() == CSmsPDU::ESmsSubmitReport)
			{
			CSmsSubmitReport&  submitReport =(CSmsSubmitReport&) smsMessage->SmsPDU();
			TInt  failureCause = submitReport.FailureCause();

			if (failureCause != TSmsFailureCause::ESmsErrorFree)
				{
				iStatus = FailureCauseToError(failureCause);
				}
			}
		else
			{
			OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGESEND_DECODESUBMITREPORTL_2, "Invalid PDU Type = %d with iStatus = %d",  smsMessage->SmsPDU().Type(),  iStatus.Int()  );
			}

		CleanupStack::PopAndDestroy(smsMessage);
		}
	} // CSmsMessageSend::DecodeSubmitReportL


/**
 *  Checks if TSY completed send message request with RP-error.
 *  
 *  @return boolean.
 */
TBool CSmsMessageSend::IsRPError() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGESEND_ISRPERROR_1, "CSmsMessageSend::IsRPError()");
	
	TBool  isRPError = EFalse;

	switch (iStatus.Int())
		{
		case KErrGsmSMSUnassignedNumber:
		case KErrGsmSMSOperatorDeterminedBarring:
		case KErrGsmSMSCallBarred:
		case KErrGsmSMSReserved:
		case KErrGsmSMSNetworkFailure:
		case KErrGsmSMSShortMessageTransferRejected:
		case KErrGsmSMSMemoryCapacityExceeded:
		case KErrGsmSMSDestinationOutOfOrder:
		case KErrGsmSMSUnidentifiedSubscriber:
		case KErrGsmSMSFacilityRejected:
		case KErrGsmSMSUnknownSubscriber:
		case KErrGsmSMSNetworkOutOfOrder:
		case KErrGsmSMSTemporaryFailure:
		case KErrGsmSMSCongestion:
		case KErrGsmSMSResourcesUnavailable:
		case KErrGsmSMSRequestedFacilityNotSubscribed:
		case KErrGsmSMSRequestedFacilityNotImplemented:
		case KErrGsmSMSInvalidShortMessageTransferReferenceValue:
		case KErrGsmSMSUnspecifiedInvalidMessage:
		case KErrGsmSMSInvalidMandatoryInformation:
		case KErrGsmSMSNonExistentMessageType:
		case KErrGsmSMSIncompatibleMessageWithSmsProtocolState:
		case KErrGsmSMSInformationElementNotImplemented:
		case KErrGsmSMSUnspecifiedProtocolError:
		case KErrGsmSMSUnspecifiedInterworkingError:
			{
			isRPError = ETrue;
			}
			break;

		default:
			{
			// NOP
			}
			break;
		}

	return isRPError;
	} // CSmsMessageSend::IsRPError


void CSmsMessageSend::SegmentMessage()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGESEND_SEGMENTMESSAGE_1, "CSmsMessageSend::SegmentMessage()");

	iState = ESmsMessageSendSegmentingMessage;

	TRAPD(ret,DoSegmentMessageL());
	
	CompleteMyself(ret);
	} // CSmsMessageSend::SegmentMessage


void CSmsMessageSend::DoSegmentMessageL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGESEND_DOSEGMENTMESSAGEL_1, "CSmsMessageSend::DoSegmentMessageL()");

	TInt reference=0;
	if (iSmsMessage->Type()==CSmsPDU::ESmsCommand)
		{
		CSmsCommand& command=(CSmsCommand&) iSmsMessage->SmsPDU();

		CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray referencearray;
		CleanupClosePushL(referencearray);

		if (!iSegmentationStore.AddCommandL(iSmsAddr,*iSmsMessage, referencearray))
			{
			User::Leave(KErrNotFound);
			}

		const TInt count=referencearray.Count();
		CArrayFix<TGsmSms>* smsarray=new(ELeave) CArrayFixFlat<TGsmSms>(8);
		CleanupStack::PushL(smsarray);

		for (TInt i=0; i<count; i++)
			{
			command.SetMessageNumber(referencearray[i].Reference());
			//TODO should reference be set to referencearray[i].iReference here?
			iSmsMessage->EncodeMessagePDUsL(*smsarray, reference);
			iSmsArray.AppendL(smsarray->At(0));
			smsarray->Reset();
			}
		CleanupStack::PopAndDestroy(2);  //  smsarray, referencearray
		}
	else
		{
		TBool iSentBefore=EFalse;
		TBool is16bit = EFalse;
		TBool concatenationIEPresent= iSmsMessage->SmsPDU().TextConcatenated( &is16bit );
		iSmsMessage->SmsPDU().SetTextConcatenatedL(EFalse,EFalse);

		// Fix for INC039985;
		// The cause was that resending a SMS message(single PDU, same LogId),
		// caused an assertion fault; This did not happen with an SMS with multiple PDUs.
		// The reason for that is that for the multiple PDU case the stack employes a caching mechanism
		// built in - it checks which PDUs from an SMS message have already been sent and
		// then sends only the ones which have not been sent. This is performed by using the LodId,
		// which is used to uniquely identify an SMS message and the PDUs it consists of.
		// If an SMS message consisting of multiple PDUs is resend by an application and it has been already
		// sent sucessfully, the stack deletes the previous entry corresponding to the old LogId
		// segmentation store and replaces it with the new one with a new logid.
		// This mechanism has been extended for SMS messages consisting of single PDU.
		if(!iSmsMessage->EncodeIntoSinglePDUL(iSmsArray))
			{
			if (concatenationIEPresent && is16bit)
				{
				iSmsMessage->Set16BitConcatenation(ETrue);
				}

			if(iSegmentationStore.HasEntryWithLogIdL(iSmsMessage->LogServerId(),reference,iSmsPDUData.iSent))
				{
				iSentBefore=ETrue;
				}
			else
				{
				reference=is16bit? iSegmentationStore.Next16BitReferenceL(): iSegmentationStore.Next8BitReferenceL();
				}
			iSmsMessage->EncodeMessagePDUsL(iSmsArray, reference);
			}
		else
			{
			 if( iSegmentationStore.HasEntryWithLogIdL(iSmsMessage->LogServerId(),reference,iSmsPDUData.iSent))
			 	{
			 	iSentBefore=ETrue;
			 	}
			}

		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGESEND_DOSEGMENTMESSAGEL_2, "CSmsMesageSend::DoSegmentMessageL [LogServerId=%d][iSentBefore=%d]",iSmsMessage->LogServerId(),iSentBefore);
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGESEND_DOSEGMENTMESSAGEL_3, "CSmsMesageSend::DoSegmentMessageL [referenceNo=%d] [iCountOfSentBefore=%d ]",reference,iSmsPDUData.iSent);
		if(!iSentBefore)
			{
			iSegmentationStore.AddSubmitL(iSmsAddr,*iSmsMessage);
			}
		}
	} // CSmsMessageSend::DoSegmentMessageL


void CSmsMessageSend::CreateLogServerEvent()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGESEND_CREATELOGSERVEREVENT_1, "CSmsMessageSend::CreateLogServerEvent()");

	iState=ESmsMessageSendCreatingLogServerEvent;
	TLogId logid=(TLogId) iSmsMessage->LogServerId();

	if (logid == KLogNullId)
		{
		iSmsPDUData.iTotal=iSmsArray.Count();
		iSmsEventLogger->AddEvent(iStatus,*iSmsMessage,iSmsPDUData);
		}
	else
		{
		iSmsEventLogger->GetEvent(iStatus,logid);
		}
	SetActive();
	} // CSmsMessageSend::CreateLogServerEvent


void CSmsMessageSend::SendNextPDU()
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGESEND_SENDNEXTPDU_1, "CSmsMessageSend::SendNextPDU [sending pdu %d of count=%d]",iSmsPDUData.iSent, iSmsArray.Count() );

	if (iSmsPDUData.iSent<iSmsArray.Count())
		{
		NMobilePhone::TMobileTON  ton;
		NMobilePhone::TMobileNPI  npi;

		TGsmSmsTelNumber  parsedAddress(iSmsArray[iSmsPDUData.iSent].Sca());

		iMobileSmsSendAttributesV1.iFlags = RMobileSmsMessaging::KGsmServiceCentre |
											RMobileSmsMessaging::KRemotePartyInfo  |
											RMobileSmsMessaging::KSmsDataFormat    |
											RMobileSmsMessaging::KMoreToSend;

	    //
	    // Set service centre address...
	    //
		parsedAddress.iTypeOfAddress.ConvertToETelMM(ton, npi);
		iMobileSmsSendAttributesV1.iGsmServiceCentre.iTelNumber    = parsedAddress.iTelNumber;
		iMobileSmsSendAttributesV1.iGsmServiceCentre.iTypeOfNumber = static_cast<RMobilePhone::TMobileTON>(static_cast<TInt>(ton));
		iMobileSmsSendAttributesV1.iGsmServiceCentre.iNumberPlan   = static_cast<RMobilePhone::TMobileNPI>(static_cast<TInt>(npi));

	    //
	    // Set destination address...
	    //
		iToFromTelNumber.iTypeOfAddress.ConvertToETelMM(ton, npi);
		iMobileSmsSendAttributesV1.iDestination.iTelNumber    = iToFromTelNumber.iTelNumber;
		iMobileSmsSendAttributesV1.iDestination.iTypeOfNumber = static_cast<RMobilePhone::TMobileTON>(static_cast<TInt>(ton));
		iMobileSmsSendAttributesV1.iDestination.iNumberPlan   = static_cast<RMobilePhone::TMobileNPI>(static_cast<TInt>(npi));

		iMobileSmsSendAttributesV1.iDataFormat = RMobileSmsMessaging::EFormatGsmTpdu;

		if (iSmsPDUData.iSent < iSmsArray.Count() - 1)
			{
			iMobileSmsSendAttributesV1.iMore = ETrue;
			}
		else
			{
			iMobileSmsSendAttributesV1.iMore = EFalse;
			}
			
		//
		// Debug logging of the PDU we are sending...
		//
#if (OST_TRACE_CATEGORY & OST_TRACE_CATEGORY_DEBUG)
        LogSmsIfPDUL(_L8("ETEL TX PDU: "), iSmsArray[iSmsPDUData.iSent].Pdu(), ETrue);
        LogSmsIfSendAttributes(_L8("SENDATTRIBS: "), iMobileSmsSendAttributesV1);
        TBuf<40> timestamp;
        SmsTimeStampL(timestamp);
        OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS,CSMSMESSAGESEND_SENDNEXTPDU_2, "%S",timestamp);
#endif
		//
		// Send the message and enter the correct state...
		//
		iState = ESmsMessageSendPDU;
		iSmsMessaging.SendMessage(iStatus, iSmsArray[iSmsPDUData.iSent].Pdu(),
								  iMobileSmsSendAttributesV1Pckg);
		TimedSetActive(iSmsSettings.SendTryTimeout());
		}
	} // CSmsMessageSend::SendNextPDU


void CSmsMessageSend::UpdateSegmentationStore()
	{
	iState = ESmsMessageSendUpdatingSegmentationStore;

	TRAPD(ret,DoUpdateSegmentationStoreL());

	CompleteMyself(ret);
	} // CSmsMessageSend::UpdateSegmentationStore


void CSmsMessageSend::DoUpdateSegmentationStoreL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGESEND_DOUPDATESEGMENTATIONSTOREL_1, "CSmsMessageSend::DoUpdateSegmentationStoreL()");

	iSmsPDUData.iSent++;  //  This is the end of the sequence for sending an SMS PDU
	
	if (iSmsMessage->Scheme() == EControlParametersScheme  ||
		iSmsMessage->Scheme() == ETPSRRScheme)
		{
		iSegmentationStore.AddReferenceStatusPairL(*iSmsMessage,(TInt) iMobileSmsSendAttributesV1.iMsgRef, iSegmentSequenceNumber);	
		}
	else if (iSmsMessage->Scheme() == EDefaultScheme)
		{
		iSegmentationStore.AddReferenceL(*iSmsMessage,(TInt) iMobileSmsSendAttributesV1.iMsgRef);	
		}
	else
		{
		User::Leave(KErrNotFound);	
		}
	} // CSmsMessageSend::DoUpdateSegmentationStoreL


void CSmsMessageSend::UpdateLogServerEvent()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGESEND_UPDATELOGSERVEREVENT_1, "CSmsMessageSend::UpdateLogServerEvent()");

	//
	//	Fill in log event...
	//
	iState = ESmsMessageSendUpdatingLogEvent;
	iSmsPDUData.iTotal = iSmsArray.Count();
	iSmsEventLogger->ChangeEvent(iStatus, *iSmsMessage, iSmsPDUData);
	SetActive();
	} // CSmsMessageSend::UpdateLogServerEvent


void CSmsMessageSend::Complete(TInt aStatus)
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGESEND_COMPLETE_1, "CSmsMessageSend::Complete [iStatus=%d, iState=%d]", iStatus.Int(), iState );

	//
	// For completion during log events, use the saved error code instead...
	//
	if (iState == ESmsMessageSendCreatingLogServerEvent  ||
		iState == ESmsMessageSendUpdatingLogEvent)
		{
		aStatus = iSendError;
		}

	//
	// Clear all PDU data that was sent...
	//
	for (TInt i=0;  i < iSmsPDUData.iSent; i++)
		{
		iSmsArray.Delete(0);
		}

	iSmsArray.Reset();
	
	iState = ESmsMessageSendIdle;

	//
	// Call the base function to perform the actual complete...
	//
	CSmsuActiveBase::Complete(aStatus);
	} // CSmsMessageSend::Complete
