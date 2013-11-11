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
//
/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "smspreadTraces.h"
#endif

#include "smspread.h"
#include "smspmain.h"

#include "smspproc.h"
#include "smspfacadestor.h"
#include "smsuset.h"
#include "smsulog.h"

#include "gsmubuf.h"
#include "Gsmumsg.h"
#include "smspcdb.h"
#include "SmsuTimer.h"
#include "exterror.h"

#include "es_ini.h"
#include <logwraplimits.h>

// Delay between sending NAK to TSY & posting Receive() SMS on ETel again
const TInt KWaitReadPduTime = 1000; // 1 millisecond


/**
 *  Creates new CSmsPDURead instance
 *  
 *  @param aSmsComm is used to complete read pdu reguest
 *  @param aSmsMessaging	used to set receive mode and start receive message
 */
CSmsPDURead* CSmsPDURead::NewL(MSmsComm& aSmsComm,
								const TSmsSettings& aSmsSettings,
								RMobileSmsMessaging& aSmsMessaging,
								CFacadeSmsReassemblyStore& aReassemblyStore,
								CSmsSegmentationStore& aSegmentationStore,
								const RMobileSmsMessaging::TMobileSmsCapsV1& aMobileSmsCaps,
								TInt aPriority,
								CSmspReceiveMode& aSmspReceiveMode,
								CSmsMonitorDiskSpace& aSmsMonitorDiskSpace)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_NEWL_1, "CSmsPDURead::NewL()");

	CSmsPDURead* smsPduRead = new (ELeave) CSmsPDURead(aSmsComm,
													   aSmsSettings,
													   aSmsMessaging,
													   aReassemblyStore,
													   aSegmentationStore,
													   aMobileSmsCaps,
													   aPriority,
													   aSmspReceiveMode,
													   aSmsMonitorDiskSpace);

	CleanupStack::PushL(smsPduRead);
	smsPduRead->ConstructL();
	CleanupStack::Pop(smsPduRead);

	return smsPduRead;
    } // CSmsPDURead::NewL


void CSmsPDURead::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_CONSTRUCTL_1, "CSmsPDURead::ConstructL()");

	ConstructTimeoutL();
	User::LeaveIfError(iOOMTimer.CreateLocal());

	iLogger = CSmsPDUReadLogger::NewL(iReassemblyStore.FileSession(), Priority());
	ReadConfigurableSmsSettingsL();
	} // CSmsPDURead::ConstructL


/**
 *  Constructor
 *  Adds CSmsPDURead object to the Active Scheduler
 *  
 *  @param aSmsComm is used to complete read pdu reguest
 *  @param aSmsMessaging	used to set receive mode and start receive message
 *  @param aReassemblyStore used to set CSmsPDUProcess
 *  @param aSegmentationStore used to set CSmsPDUProcess
 *  @param aMobileSmsCaps used to set iMobileSmsCaps
 *  @param aPriority used to set CSmsPDUProcess
 *  @param aSmspReceiveMode used to set iReceiveMode
 *  @param aSmsMonitorDiskSpace used to set iSmsMonitorDiskSpace
 */
CSmsPDURead::CSmsPDURead(MSmsComm& aSmsComm,
						 const TSmsSettings& aSmsSettings,
						 RMobileSmsMessaging& aSmsMessaging,
						 CFacadeSmsReassemblyStore& aReassemblyStore,
						 CSmsSegmentationStore& aSegmentationStore,
						 const RMobileSmsMessaging::TMobileSmsCapsV1& aMobileSmsCaps,
						 TInt aPriority,
						 CSmspReceiveMode& aSmspReceiveMode,
						 CSmsMonitorDiskSpace& aSmsMonitorDiskSpace)
  : CSmsuActiveBase(aPriority),
	iState(ESmsPDUReadIdle),
	iSmsMessaging(aSmsMessaging),
	iMsgReceiveAttributesPckg(iMsgReceiveAttributes),
	iSmsComm(aSmsComm),
	iSmsSettings(aSmsSettings),
	iReassemblyStore(aReassemblyStore),
	iSegmentationStore(aSegmentationStore),
	iMobileSmsCaps(aMobileSmsCaps),
	iReceiveMode(aSmspReceiveMode),
	iSmsMonitorDiskSpace(aSmsMonitorDiskSpace)
	{
	} // CSmsPDURead::CSmsPDURead


/**
 *  Called by CSmsProtocol to start CSmsStoreRead active object
 *  Issues a Receive to the TSY
 */
void CSmsPDURead::Start()
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_START_1, "CSmsPDURead::Start(): iStatus=%d, iState=%d", iStatus.Int(), iState );

	if (IsSupported()  &&  IsActive() == EFalse  &&  iReassemblyStore.IsFull() == EFalse)
		{
		iState = ESmsPDUReadSetReceiveMode;

		iReceiveMode.Start(iStatus);
		SetActive();
		}
    } // CSmsPDURead::Start


void CSmsPDURead::ProcessPDUL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_PROCESSPDUL_1, "CSmsPDURead::ProcessPDUL()");

	//
	// Initilse the Slot location data...
	//
	iSlot.iIndex         = iMsgReceiveAttributes.iStoreIndex;
	iSlot.iStore         = iMsgReceiveAttributes.iStore;
	iSlot.iMsgData       = iMsgData;
	iSlot.iMsgStatus     = RMobileSmsStore::EStoredMessageUnread;
	iSlot.iServiceCentre = iMsgReceiveAttributes.iGsmServiceCentre;

	//
	// Initialise the PDU processor class...
	//
	delete iPduProcessor;
	iPduProcessor = NULL;

	iPduProcessor = CSmsPDUProcessor::NewL(iSmsComm, iSmsSettings,
										   iReassemblyStore, iSegmentationStore,
										   iSmsMonitorDiskSpace);

	iPduProcessor->SetApplyAutoDeletionToClass2(iConfigAutomaticDeletionForClass2);
	iPduProcessor->SetDiscardType0Class0(iConfigDiscardingType0Class0);
	iPduProcessor->SetDiscardType0Class2(iConfigDiscardingType0Class2);

	TRAPD(decodeError, iPduProcessor->DecodeAndProcessPDUL(iSlot, EFalse));
	iStatus = decodeError;
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_PROCESSPDUL_2, "CSmsPDURead::ProcessPDUL(): DecodeAndProcessPDUL() returned %d", iStatus.Int());

	//
	// If the PDU cannot be decoded, and this was not due to memory or disk
	// space problems, then get rid of it...
	//
	if (iStatus != KErrNone  &&  iStatus != KErrNoMemory  &&  iStatus != KErrDiskFull)
		{
		iPduProcessor->DeletePDUL();
		}

	//
	// Send ACK or NACK delivery report (depends on iState and iError)...
	//
	SendDeliverReport();
	} // CSmsPDURead::ProcessPDUL


/**
 *  Called by Active Scheduler to run this object
 *  State ESmsPDUSetReceiveMode:
 *  Calls private method Receive() to change state to ESmsPDUReadWaitingForPDU
 *  and start receive message
 *  State ESmsPDUReadWaitingForPDU:
 *  Calls method Complete to change state to ESmsPDUReadIdle and complete message receive
 *  
 *  @leave Leaves if there was an error
 *  @leave Panic if complete is called while state is ESmsStoreReadIdle
 */
void CSmsPDURead::DoRunL()
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_DORUNL_1, "CSmsPDURead::DoRunL [iStatus=%d, iState=%d]", iStatus.Int(), iState);

	//
	// Handle state changes only for successful operations. Any errors will be
	// handled when Complete() is called following this DoRunL() call.
	// Complete() is not called if the object becomes active.
	//
    switch (iState)
        {
        case ESmsPDUReadSetReceiveMode:
            {
			if (iStatus.Int() == KErrNone)
				{
	            ResumeSmsReception();
	            }
            }
            break;

        case ESmsPDUReadResumeReception:
            {
			if (iStatus.Int() == KErrNone)
				{
    	        Receive();
    	        }
	        }
            break;

        case ESmsPDUReadWaitingForPDU:
            {
			if (iStatus.Int() == KErrNone)
				{
#if (OST_TRACE_CATEGORY & OST_TRACE_CATEGORY_DEBUG)
                LogSmsIfPDUL(_L8("ETEL RX PDU: "), iMsgData, EFalse); 
                TBuf<40> timestamp;
                SmsTimeStampL(timestamp);
                OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS,CSMSPDUREAD_DORUNL_2, "%S",timestamp);
#endif
	            ProcessPDUL();
	            }
            }
            break;

        case ESmsPDUReadAcceptValidPduSendPositiveAck:
            {
			if (iStatus.Int() == KErrNone)
				{
	            AddLogEvent();
	            }
            }
            break;

        case ESmsPDUReadLogging:
            {
			if (iStatus.Int() == KErrNone)
				{
	            iPduProcessor->UpdateLogServerIdL();
				}

 			if ((iStatus.Int() == KErrNone) ||
 				(iStatus.Int() == -KErrNotSupported))
 				{				
    	        iPduProcessor->ProcessMessageIfCompleteL();
    	        }
            }
            break;

        case ESmsPDUReadSystemOutOfMemorySendNegativeAck:
            {
			if (iStatus.Int() == KErrNone)
				{
	            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_DORUNL_3, "CSmsPDURead::DoRunL in ESmsPDUReadSystemOutOfMemorySendNack2");
    	        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_DORUNL_4, "CSmsPDURead::DoRunL activate post OOM timer");
        	    iState = ESmsPDUWaitTimerAfterOOM;
            	iOOMTimer.After(iStatus,KWaitReadPduTime);
            	SetActive();
            	}
            }
            break;

        case ESmsPDUWaitTimerAfterOOM:
            {
			if (iStatus.Int() == KErrNone)
				{
	            Receive();
	            }
            }
            break;

        case ESmsPDUReadInvalidPDUSendNegativeAck:
        case ESmsPDUReadAbsorbInvalidPduSendPositiveAck:
   		case ESmsPDUReadAbsorbValidPduSendPositiveAck:
            {
            // NOP
            }
            break;

        default:
        	{
            SmspPanic(KSmspPanicUnexpectedState);
            }
            break;
        }

	//
	// DoRunL() will now return to CSmsuActiveBase which if the object
	// is not active, will call Complete().
	//
    } // CSmsPDURead::DoRunL


/**
 *  public method for starting receive message
 */
void CSmsPDURead::ResumeSmsReception()
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_RESUMESMSRECEPTION_1, "CSmsPDURead::ResumeSmsReception [iStatus=%d, iState=%d]", iStatus.Int(), iState );

    if (!IsActive())
        {
		iDelayedResumeSmsReceptionRequired = EFalse;
		iSmsReceptionSupended = EFalse;
        iState = ESmsPDUReadResumeReception;
        iSmsMessaging.ResumeSmsReception( iStatus );
        SetActive();
        }
	else if (iSmsReceptionSupended)
		{
		//
		// We will need to save this for later...
		//
		iDelayedResumeSmsReceptionRequired = ETrue;
		}
    } // CSmsPDURead::ResumeSmsReception


/**
 *  private method for starting receive message
 */
void CSmsPDURead::Receive()
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_RECEIVE_1, "CSmsPDURead::Receive [iStatus=%d, iState=%d]", iStatus.Int(), iState );

	//
	// Check if we need to resume reception first. This may be required if a
	// previous attempt to resume could not be actioned.
	//
	if (iDelayedResumeSmsReceptionRequired)
		{
		ResumeSmsReception();
		}
	else
		{
		//
		// Clears message data buffer before passing reference to the phone side
		//
		RMobileSmsMessaging::TMobileSmsReceiveAttributesV1  empty;

		iMsgData.Zero();
		iMsgReceiveAttributes = empty;

		//
		// Receive a message...
		//
    	iState = ESmsPDUReadWaitingForPDU;

		iSmsMessaging.ReceiveMessage( iStatus, iMsgData, iMsgReceiveAttributesPckg );
		SetActive();
	   	}
    } // CSmsPDURead::Receive


/**
 *  Cancels CSmsPDURead object
 *  Cancels reveive message
 *  Completes CSmsPDURead object
 */
void CSmsPDURead::DoCancel()
    {
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_DOCANCEL_1, "CSmsPDURead::DoCancel [iStatus=%d iState=%d]", iStatus.Int(), iState );

	TimedSetActiveCancel();

	switch (iState)
		{
		case ESmsPDUReadResumeReception:
			{
 			iSmsMessaging.CancelAsyncRequest(EMobileSmsMessagingResumeSmsReception);
			}
			break;

		case ESmsPDUReadSetReceiveMode:
			{
			iReceiveMode.Cancel();
			}
			break;

		case ESmsPDUReadWaitingForPDU:
			{
			iSmsMessaging.CancelAsyncRequest(EMobileSmsMessagingReceiveMessage);
			}
			break;

		case ESmsPDUReadLogging:
			{
			iLogger->Cancel();
			}
			break;

		case ESmsPDUReadAcceptValidPduSendPositiveAck:
		case ESmsPDUReadAbsorbValidPduSendPositiveAck:
		case ESmsPDUReadAbsorbInvalidPduSendPositiveAck:
			{
 			iSmsMessaging.CancelAsyncRequest(EMobileSmsMessagingAckSmsStored);
			}
			break;

		case ESmsPDUReadInvalidPDUSendNegativeAck:
		case ESmsPDUReadSystemOutOfMemorySendNegativeAck:
			{
 			iSmsMessaging.CancelAsyncRequest(EMobileSmsMessagingNackSmsStored);
			}
			break;

		case ESmsPDUWaitTimerAfterOOM:
			{
			iOOMTimer.Cancel();
			}
			break;

		case ESmsPDUWaitingForDiskMonitor:
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
    } // CSmsPDURead::DoCancel


void CSmsPDURead::AddLogEvent()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_ADDLOGEVENT_1, "CSmsPDURead::AddLogEvent()");

	iState = ESmsPDUReadLogging;
	
	if (iPduProcessor->OriginalSmsMessage() == NULL)
		{
		iLogger->ChangeOrAddLogEvent(*iPduProcessor->SmsMessage(),
									 *iPduProcessor->SmsPDUData(), NULL, iStatus);
		}
	else
		{
		iPduProcessor->OriginalSmsMessage()->SetLogServerId(iPduProcessor->SmsMessage()->LogServerId());
        //If the original SMS TX log entry is to be updated because a
        // Status report has come in, pass as arguments both the orginal
        // TX SMS (*smsmessage) and the Status Report (contained in iSmsMessage).
        // This allows the discharge time in the Status Report to be logged.        
		
		CSmsMessage* pSmsMsg = iPduProcessor->SmsMessage();
	    CSmsStatusReport& statusReport = const_cast<CSmsStatusReport&>(static_cast<const CSmsStatusReport&>(pSmsMsg->SmsPDU()));
        TTime dischargeTime;
        TInt numQuarterHours = 0;
        statusReport.DischargeTime(dischargeTime, numQuarterHours);   
        iLogger->ChangeOrAddLogEvent(*(iPduProcessor->OriginalSmsMessage()), *iPduProcessor->SmsPDUData(), &dischargeTime,  iStatus);                	 		
		}

	SetActive();
	} // CSmsPDURead::AddLogEvent


void CSmsPDURead::DoEncodeDeliverReportL()
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_DOENCODEDELIVERREPORTL_1, "CSmsPDUReadProcess::DoEncodeDeliverReport [iStatus=%d]", iStatus.Int());

    iDeliverReport.SetPdu(KNullDesC8);

    const TPtrC des(iPduProcessor->DeliverReportBuffer());
    CSmsBuffer* buffer = CSmsBuffer::NewL();
    CSmsMessage* smsMessage = CSmsMessage::NewL(iReassemblyStore.FileSession(), CSmsPDU::ESmsDeliverReport, buffer);
    CleanupStack::PushL(smsMessage);

    CSmsDeliverReport& deliverReport = ( CSmsDeliverReport& ) smsMessage->SmsPDU();

    switch(iState)
    {
        case ESmsPDUReadInvalidPDUSendNegativeAck:
        case ESmsPDUReadSystemOutOfMemorySendNegativeAck:
            {
            OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_DOENCODEDELIVERREPORTL_2, "CSmsPDURead::DoEncodeDeliverReport SendNegativeAck [iStatus=%d, iState=%d, iRpErrorCodesSupported= %d]",iStatus.Int(),    iState,    iRpErrorCodesSupported);

            deliverReport.SetIsRPError(ETrue);
            const TInt failureCause = ErrorToTPError(iStatus.Int());
            deliverReport.SetFailureCause((TSmsFailureCause::TSmsFailureCauseError) failureCause);
            break;
            }
        case ESmsPDUReadAcceptValidPduSendPositiveAck:
        case ESmsPDUReadAbsorbInvalidPduSendPositiveAck:
       	case ESmsPDUReadAbsorbValidPduSendPositiveAck:
            {
            OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_DOENCODEDELIVERREPORTL_3, "CSmsPDURead::DoEncodeDeliverReport SendPositiveAck [iStatus=%d, iState=%d, iRpErrorCodesSupported= %d]",iStatus.Int(),    iState,    iRpErrorCodesSupported);
            deliverReport.SetIsRPError(EFalse);
            break;
            }
        default:
            {
            OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_DOENCODEDELIVERREPORTL_4, "CSmsPDURead::DoEncodeDeliverReport [iStatus=%d, iState=%d, iRpErrorCodesSupported= %d]",iStatus.Int(), iState, iRpErrorCodesSupported);
            SmspPanic( KSmspPanicUnexpectedStateEncountered );
            break;
            }
    }

    const TBool textPresent = (des.Length() > 0);

    deliverReport.SetUserDataPresent(textPresent);
    deliverReport.SetDataCodingSchemePresent(textPresent);
    deliverReport.SetProtocolIdentifierPresent(textPresent);

    if (textPresent)
        buffer->InsertL( 0, des );

    CArrayFixFlat<TGsmSms>* encodedPdus = new (ELeave) CArrayFixFlat<TGsmSms>(1);
    CleanupStack::PushL(encodedPdus);

    if(!smsMessage->EncodeIntoSinglePDUL(*encodedPdus))
        smsMessage->EncodeMessagePDUsL(*encodedPdus);

    if (encodedPdus->Count() > 0)
        iDeliverReport = (*encodedPdus)[0];

    CleanupStack::PopAndDestroy(2);  //  smsmessage, encodedPdus
    } // CSmsPDURead::DoEncodeDeliverReportL


/**
 *  The received SMS deliver PDU will be acknowledged or not acknowledged here.
 *  
 *  Because all correction and resending of corrupt PDUs happens within the phone stack we
 *  are assured that the PDU we have is a faithful copy of the one at the SC.
 *  The 'Out of Memory' errors KErrNoMemory & KErrDiskFull are always negatively acknowledged,
 *  All other errors are by default positively acknowledged but quietly discarded
 *  without forwarding to the client. This is because it has been observed that some network
 *  operators do not recognise the possible error codes as permanent and repeatly send the errored
 *  PDU back to the mobile. However, the stack can be configured to support each of the error
 *  codes specified 04.11 5.30. Table 8.4 (Part 2) by configuring the sms esk file - this
 *  procedure is described in the comments relating to method ReadConfigurableSmsSettingsL().
 *  
 *  @note Basic interface for sending sms deliver report to the SC.
 */
void CSmsPDURead::SendDeliverReport()
	{
	// Method behaviour based on DEF047323
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_SENDDELIVERREPORT_1, "CSmsPDUReadProcess::SendDeliverReport(): iStatus=%d ",iStatus.Int());

    MakeStateTransitionBasedOnErrorCode();

    if ((iMsgReceiveAttributes.iStatus == RMobileSmsMessaging::EMtMessageUnstoredClientAck) ||
        (iMsgReceiveAttributes.iStatus == RMobileSmsMessaging::EMtMessageUnknownStatus))
        {
        iDeliverReport = TGsmSms();

        TRAPD(err, DoEncodeDeliverReportL());
        if(err != KErrNone)
            {
            OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_SENDDELIVERREPORT_2, "CSmsPDURead::DoEncodeDeliverReport [err=%d]", err);
            }
        }
    else
    	{
        iDeliverReport.SetPdu(iMsgData);	//create quasi deliver report
        }

    switch(iState)
        {
        case ESmsPDUReadInvalidPDUSendNegativeAck:
        case ESmsPDUReadSystemOutOfMemorySendNegativeAck:
             {
             TInt failureCause = 0;
             failureCause = ErrorToRPError(iStatus.Int());
             iSmsMessaging.NackSmsStored(iStatus, iDeliverReport.Pdu(), failureCause);
             }
             break;

        case ESmsPDUReadAcceptValidPduSendPositiveAck:
        case ESmsPDUReadAbsorbValidPduSendPositiveAck:
        case ESmsPDUReadAbsorbInvalidPduSendPositiveAck:
             {
             const TBool full = iReassemblyStore.IsFull();
             iSmsMessaging.AckSmsStored(iStatus, iDeliverReport.Pdu(), full);
             iSmsReceptionSupended = (iSmsReceptionSupended || full);
             }
             break;

        default:
             {
             OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_SENDDELIVERREPORT_3, "CSmsPDURead::DoEncodeDeliverReport [iStatus=%d, iState=%d, iRpErrorCodesSupported= %d]",iStatus.Int(), iState, iRpErrorCodesSupported);
             SmspPanic( KSmspPanicUnexpectedStateEncountered );
             }
             break;
         }

	TimedSetActive(iSmsSettings.Timeout());
	} // CSmsPDURead::SendDeliverReport


/**
 *  Sets state to ESmsPDUReadIdle
 *  Completes message receive request to the CSmsProtocol
 */
void CSmsPDURead::Complete(TInt aError)
	{
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_COMPLETE_1, "CSmsPDURead::Complete [aError=%d, iState=%d]", aError, iState );

	switch (iState)
		{
	    case ESmsPDUReadAcceptValidPduSendPositiveAck:
	    	{
	        if (aError != KErrCancel  &&  aError != KErrTimedOut)
	        	{
		        AddLogEvent();
		        break;					
		        //  pass through otherwise
		        }
		     }
	    case ESmsPDUReadResumeReception:
	    case ESmsPDUReadWaitingForPDU:		
	    case ESmsPDUReadLogging:	
	    case ESmsPDUReadSystemOutOfMemorySendNegativeAck:
		case ESmsPDUReadAbsorbValidPduSendPositiveAck:
	    case ESmsPDUReadAbsorbInvalidPduSendPositiveAck:
	    case ESmsPDUReadInvalidPDUSendNegativeAck:
	    case ESmsPDUWaitTimerAfterOOM:
	        {
	        delete iPduProcessor;
	        iPduProcessor = NULL;
	        
	        if (aError == KErrServerTerminated)
	        	{
	            SmspPanic(ESmspCantError);	// could add a new panic code for this?
	            }
	            
	        if (aError == KErrTimedOut  &&  IsActive())
	        	{
	        	//
	        	// This request is being called during a timeout Cancel(). We'd like
	        	// to kick of a new request, but we cannot as we are still active.
	        	// So, request a completion after the Cancel() finishes...
	        	//
	        	CompleteMyselfAfterTimeout(KErrNone);
	        	break;
	        	}

			//
			// Start receiving more messages if possible...
			//
	        if (aError != KErrCancel  &&  iReassemblyStore.IsFull() == EFalse  &&
	        	(iState != ESmsPDUReadWaitingForPDU  ||  iStatus != KErrNotSupported))
	            {
				Receive();																
				}
			else
				{
		        iState = ESmsPDUReadIdle;
		        }
	        }
	        break;

	    case ESmsPDUReadSetReceiveMode:
	    	{
	        iState = ESmsPDUReadIdle;
	        }
	        break;

	    case ESmsPDUWaitingForDiskMonitor:
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
	} // CSmsPDURead::Complete


/**
 *  Destructor
 */
CSmsPDURead::~CSmsPDURead()
    {
    Cancel();
	delete iPduProcessor;
	iOOMTimer.Close();
	delete iLogger;
    } // CSmsPDURead::~CSmsPDURead


/**
 *  Sets a specified error code to either supported ( i.e. reported
 *  as the Rp Error condition in a delivery report if the error condition
 *  is detected ) or not supported ( i.e not reported as an Rp Error Condition
 *  in a Deliver Report when the condition is detected).
 *  
 *  @param aIndex identifies the bit that represents the error condition in the bit mask
 *  @param aValue specifies whether the error condition is supported.
 */
void CSmsPDURead::SetErrorCodeStatus(enum TRpErrorCodeIndex aIndex, TBool aValue)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_SETERRORCODESTATUS_1, "CSmsPDURead::SetErrorCodeStatus()");

    if (aValue)
    	{
        iRpErrorCodesSupported |=  (((TUint8) 0x01) << aIndex);
        }
    else
    	{
        iRpErrorCodesSupported &= ~(((TUint8) 0x01) << aIndex);
        }
    } // CSmsPDURead::SetErrorCodeStatus


/**
 *  
 *  Retrieves the Error Code Status (i.e. whether the error is to be
 *  reported in a delivery report when the error condition is detected).
 *  
 *  @param   aIndex identifies the bit that represents the error condition in the bit mask.
 *  @return  whether the error condition is to be reported in a delivery report, if the
 *  condition is detected.
 */
TBool CSmsPDURead::GetErrorCodeStatus(enum TRpErrorCodeIndex aIndex)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_GETERRORCODESTATUS_1, "CSmsPDURead::GetErrorCodeStatus()");

    TBool rc;
    iRpErrorCodesSupported & (((TUint8) 0x01) << aIndex) ? rc = ETrue : rc = EFalse;
    return rc;
    } // CSmsPDURead::GetErrorCodeStatus


/**
 *  
 *  Maps an RpErrorCode to an index that represents the error in a bit mask.
 *  
 *  @param aRpErrorCode represents the error code to be mapped.
 *  @param aIndex represents the index to the error status in a bit mask.
 *  @return True if the error can be mapped to a bit mask index, otherwise False
 */
TBool CSmsPDURead::MapRpErrorToIndex(TInt aRpErrorCode, TRpErrorCodeIndex& aIndex)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_MAPRPERRORTOINDEX_1, "CSmsPDURead::MapRpErrorToIndex()");

    switch( aRpErrorCode )
        {
        case KErrGsmSMSInvalidShortMessageTransferReferenceValue:
            {
		    aIndex = EInvalidShortMsgTransferReferenceValue;
            }
		    break;

        case KErrGsmSMSUnspecifiedInvalidMessage:
            {
		    aIndex = ESemanticallyIncorrectMessage;
            }
		    break;

        case KErrGsmSMSInvalidMandatoryInformation:
            {
		    aIndex = EInvalidMandatoryInformaton;
            }
		    break;

        case KErrGsmSMSNonExistentMessageType:
            {
		    aIndex = EMessageTypeNonExistent;
            }
		    break;

        case KErrGsmSMSIncompatibleMessageWithSmsProtocolState:
            {
		    aIndex = EMessageNotCompatibleWithShortMessageProtocolState;
            }
		    break;

        case KErrGsmSMSInformationElementNotImplemented:
            {
		    aIndex = EInformationElementNonExistentOrNotImplemented;
            }
		    break;

        case KErrGsmSMSUnspecifiedProtocolError:
            {
		    aIndex = EProtocolErrorUnspecified;
            }
		    break;

        default:
            {
            aIndex = EProtocolErrorUnspecified;
            }
            break;
        }

	//
    // Currently always returns true, as the default error code
    // "Protocol Error Unspecified" can be used to map any
    // error condition that is not explicitly mapped.
	//
    return ETrue;
    } // CSmsPDURead::MapRpErrorToIndex


/**
 *  Synopsis: Reads the smswap.sms.ESK file. This can be used
 *  to specify whether the SMS Stack should negatively
 *  acknowledge any of the Rp Error conditions specified
 *  in 04.11 v5.3.0 Table 8.4 (part 2).
 *  
 *  When the behaviour is not specified in the smswap.sms.ESK,
 *  the default behaviour is for the stack to positively
 *  acknowledge any PDU even if decoding it has resulted in one
 *  of the specified error conditions. In addition, the 'Out
 *  of Memory' Condition can not be specified as unsupported -
 *  it always results in the Rp Error 'MemoryCapacityExceeded'
 *  being included a Deliver Report. It cannot be configured as
 *  disabled.
 *  
 *  Example: Segment of a smswap.sms.ESK file which specifies all
 *  the 04.11 error conditions are to be supported.
 *  
 *  [CustomerSmsRpErrorCodes]
 *  InvalidShortMsgTransferRefValue= 1
 *  SemanticallyIncorrectMsg= 1
 *  InvalidMandatoryInformaton= 1
 *  MsgTypeNonExistent= 1
 *  MsgNonCompatibleWithPrtclState= 1
 *  InfoElemNotExist_NotImplemented= 1
 *  ProtocolErrorUnspecified= 1
 *  
 *  Errors will not be reported if the corresponding identifier is set to 0
 *  or is omitted from the file.
 *  
 *  Certain pieces of Release 6 functionality cause breaks to existing functionality.
 *  These are disabled by default, but can be configured as follows.
 *  
 *  (1) Class 2 messages on the SIM will be deleted when the DCS
 *  byte specifies that "the message is specified for automatic deletion"
 *  See 23.038 Section 4, "SMS Data Coding Scheme" and when the following
 *  text segment is included in the smswap.sms.ESK file:
 *  
 *  [Release6Configurability]
 *  AutomaticDeletionOfClass2= 1
 *  
 *  (2) Class 2 messages on the SIM will be deleted and will not be
 *  forwarded to the client when the PID  byte specifies that the
 *  message is type 0 as per 23.40 v6.5.0  9.2.3.9 and when the
 *  following text segment is included in the  smswap.sms.ESK file:
 *  
 *  [Release6Configurability]
 *  DiscardType0_Class2Message= 1
 *  
 *  
 *  (3) Class 0 messages will not be forwarded to the SMS Stack client
 *  when the PID byte specifies that the message is type 0 as per
 *  23.40 v6.5.0 9.2.3.9 and when the following text segment is included
 *  in the smswap.sms.esk file:
 *  [Release6Configurability]
 *  DiscardType0_Class0Message= 1
 *  
 *  Note: The Release 6 Configuration Option tag [Release6Configurability]
 *  must only be specified once in the configuration file, even if
 *  more than one Release 6 configuration option is specified. For example
 *  [Release6Configurability]
 *  AutomaticDeletionOfClass2=  1
 *  DiscardType0_Class2Message= 1
 *  DiscardType0_Class0Message= 1
 */
void CSmsPDURead::ReadConfigurableSmsSettingsL()
    {
    CESockIniData* ini = NULL;
    TRAPD(ret, ini=CESockIniData::NewL(_L("smswap.sms.esk")));
    if(ret!=KErrNone)
        {
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_READCONFIGURABLESMSSETTINGSL_1, "esk read failed, error code = [%d]", ret);
        User::Leave(ret);
        }

    CleanupStack::PushL(ini);

    iRpErrorCodesSupported = 0;

    TBool status(EFalse);
    if (ini->FindVar(_L("CustomerSmsRpErrorCodes"),
    				 _L("InvalidShortMsgTransferRefValue"), status))
    	{
        SetErrorCodeStatus(EInvalidShortMsgTransferReferenceValue, status);
        }

    if (ini->FindVar(_L("CustomerSmsRpErrorCodes"),
    				 _L("SemanticallyIncorrectMsg"),status))
    	{
        SetErrorCodeStatus(ESemanticallyIncorrectMessage, status);
    	}

    if (ini->FindVar(_L("CustomerSmsRpErrorCodes"),
    				 _L("InvalidMandatoryInformaton"),status))
    	{
        SetErrorCodeStatus(EInvalidMandatoryInformaton, status);
    	}

    if (ini->FindVar(_L("CustomerSmsRpErrorCodes"),
    				 _L("MsgTypeNonExistent"),status))
    	{
        SetErrorCodeStatus(EMessageTypeNonExistent, status);
    	}

    if (ini->FindVar(_L("CustomerSmsRpErrorCodes"),
    				 _L("MsgNonCompatibleWithPrtclState"),status))
    	{
        SetErrorCodeStatus(EMessageNotCompatibleWithShortMessageProtocolState, status);
    	}

    if (ini->FindVar(_L("CustomerSmsRpErrorCodes"),
    				 _L("InfoElemNotExist_NotImplemented"),status))
    	{
        SetErrorCodeStatus(EInformationElementNonExistentOrNotImplemented, status);
    	}

    if (ini->FindVar(_L("CustomerSmsRpErrorCodes"),
    				 _L("ProtocolErrorUnspecified"), status))
    	{
        SetErrorCodeStatus(EProtocolErrorUnspecified, status);
    	}

    if (ini->FindVar(_L("Release6Configurability"),
    				 _L("AutomaticDeletionOfClass2"), status))
    	{
        iConfigAutomaticDeletionForClass2 = status;
    	}

    if (ini->FindVar(_L("Release6Configurability"),
    				 _L("DiscardType0_Class2Message"), status))
    	{
        iConfigDiscardingType0Class2 = status;
    	}

    if (ini->FindVar(_L("Release6Configurability"),
    				 _L("DiscardType0_Class0Message"), status))
    	{
        iConfigDiscardingType0Class0 = status;
    	}

    CleanupStack::PopAndDestroy(ini);
    } // CSmsPDURead::ReadConfigurableSmsSettingsL

/**
 *  Checks that receiving message is supported by Tsy.
 */
TBool CSmsPDURead::IsSupported() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_ISSUPPORTED_1, "CSmsPDURead::IsSupported()");

	if (( iMobileSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsReceiveUnstoredClientAck ) ||
		( iMobileSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsReceiveUnstoredPhoneAck ) ||
		( iMobileSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsReceiveStored ) ||
		( iMobileSmsCaps.iSmsControl & RMobileSmsMessaging::KCapsReceiveEither  ))
		{
		return ETrue;
		}

	return EFalse;
	} // CSmsPDURead::IsSupported


/**
 *  Make a state transition from WaitingForPDU based
 *  on:
 *  (i) any error condition detected
 *  (ii) whether the error condition needs to be
 *  positively or negatively acknowledged in the
 *  Delivery Report.
 *  
 *  Note that this function also handles the absorption of
 *  valid PDUs when needed.
 */
void CSmsPDURead::MakeStateTransitionBasedOnErrorCode()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_MAKESTATETRANSITIONBASEDONERRORCODE_1, "CSmsPDURead::MakeStateTransitionBasedOnErrorCode()");

    if (iState != ESmsPDUReadWaitingForPDU)
        SmspPanic( KSmspPanicUnexpectedStateEncountered );

    if (iStatus == KErrNoMemory  ||  iStatus == KErrDiskFull)
        {
        iState = ESmsPDUReadSystemOutOfMemorySendNegativeAck;
        }
    else if (iStatus != KErrNone)
        {
        TRpErrorCodeIndex index;
        // check:
        //   1) Is this is a configurable error code ?
        //   2) If it is, should an error code be sent in a NegativeAck ?
        if (MapRpErrorToIndex(iStatus.Int(), index)  &&
            GetErrorCodeStatus(index))
            {
            iState = ESmsPDUReadInvalidPDUSendNegativeAck;
            }
        else
            {
            iState =  ESmsPDUReadAbsorbInvalidPduSendPositiveAck;
            }
		}
    else
    	{
      // PDU is valid
    if (iPduProcessor->IsPIDType0()  &&
	    ( (iPduProcessor->IsClass0Message()  &&  iPduProcessor->IsDiscardType0Class0() ) ||
		  (iPduProcessor->IsClass2Message()  &&  iPduProcessor->IsDiscardType0Class2() ) ||
		  (iPduProcessor->IsClass0Message() == EFalse  &&  iPduProcessor->IsClass2Message() == EFalse)) )
		    {
		    	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREAD_MAKESTATETRANSITIONBASEDONERRORCODE_2, "CSmsPDURead::MakeStateTransitionBasedOnErrorCode Absorb Valid PDU");
	            iState = ESmsPDUReadAbsorbValidPduSendPositiveAck;
		    }
	        else
		    {
	            iState = ESmsPDUReadAcceptValidPduSendPositiveAck;
	        }
    	}

    } // CSmsPDURead::MakeStateTransitionBasedOnErrorCode


/**
 *  Two-phase factory for CSmsPDUReadLogger
 *  
 *  @param aFs File Server session
 *  @param aPriority ActiveScheduler priority of this active object
 *  @return Newly constructed CSmsPDUReadLogger
 */
CSmsPDUReadLogger* CSmsPDUReadLogger::NewL(RFs& aFs, TInt aPriority)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREADLOGGER_NEWL_1, "CSmsPDUReadLogger::NewL()");

	CSmsPDUReadLogger* self = new (ELeave) CSmsPDUReadLogger(aPriority);
	CleanupStack::PushL(self);
	self->ConstructL(aFs);
	CleanupStack::Pop(self);

	return self;
	} // CSmsPDUReadLogger::NewL


/**
 *  CSmsPDUReadLogger constructor
 *  
 *  @param aPriority ActiveScheduler priority of this active object
 */
CSmsPDUReadLogger::CSmsPDUReadLogger(TInt aPriority)
  : CSmsuActiveBase(aPriority)
	{
	} // CSmsPDUReadLogger::CSmsPDUReadLogger


/**
 *  2nd phase of construction for CSmsPDUReadLogger.
 *  
 *  @param aFs File Server session
 *  @leave Leaves (usually with KErrNoMemory) if unable to construct the CSmsEventLogger
 */
void CSmsPDUReadLogger::ConstructL(RFs& aFs)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREADLOGGER_CONSTRUCTL_1, "CSmsPDUReadLogger::ConstructL()");

	iLogger = CSmsEventLogger::NewL(aFs, Priority());
	} // CSmsPDUReadLogger::ConstructL


/**
 *  CSmsPDUReadLogger destructor
 */
CSmsPDUReadLogger::~CSmsPDUReadLogger()
	{
	Cancel();
	delete iLogger;
	} // CSmsPDUReadLogger::~CSmsPDUReadLogger

/**
 *  Asynchonously changes or creates a new log event for aSmsMessage
 *  
 *  @pre aSmsMessage contains the correct LogServerId
 *  @param aSmsMessage The CSmsMessage to add or change on the system log
 *  @param aSmsPDUData Log data relating to aSmsMessage
 *  @param aTime Sms delivery time
 *  @leave Completes aStatus with KErrNone if there is no logging client available
 */
void CSmsPDUReadLogger::ChangeOrAddLogEvent(CSmsMessage& aSmsMessage, const TLogSmsPduData& aSmsPDUData, const TTime* aTime, TRequestStatus& aStatus)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREADLOGGER_CHANGEORADDLOGEVENT_1, "CSmsPDUReadLogger::ChangeOrAddLogEvent()");
        
    __ASSERT_DEBUG(iState == ESmsPDUReadLoggerIdle, SmspPanic(KSmspPanicUnexpectedState));

    Queue(aStatus);

    if (iLogger->ClientAvailable())
        {
        iSmsMessage    = &aSmsMessage;
        iSmsPDUData    = &aSmsPDUData;
        if (aTime != NULL)
           {
           iTime = *aTime;
           iTimePtr = &iTime;
           }
        else
           {
           iTimePtr = NULL;
           }
                
        if (aSmsMessage.LogServerId() == KLogNullId)
            AddLogEvent();
        else
            GetLogEvent();
        }
    else
        {
        Complete(KErrNone);
        }       
    }

/**
 *  Adds iSmsMessage to the log
 */
void CSmsPDUReadLogger::AddLogEvent()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREADLOGGER_ADDLOGEVENT_1, "CSmsPDUReadLogger::AddLogEvent()");

	iState = ESmsPDUReadLoggerAdd;

	iLogger->AddEvent(iStatus, *iSmsMessage, *iSmsPDUData);
	SetActive();
	} // CSmsPDUReadLogger::AddLogEvent


/**
 *  Gets the log event with Id iSmsMessage->LogServerId()
 */
void CSmsPDUReadLogger::GetLogEvent()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREADLOGGER_GETLOGEVENT_1, "CSmsPDUReadLogger::GetLogEvent()");

	__ASSERT_DEBUG(iSmsMessage->LogServerId() != KLogNullId, SmspPanic(KSmspPanicLogIdNull));

	iState = ESmsPDUReadLoggerGet;

	iLogger->GetEvent(iStatus, iSmsMessage->LogServerId());
	SetActive();
	} // CSmsPDUReadLogger::GetLogEvent


/**
 *  Changes the log event with iSmsMessage and iSmsPDUData
 */
void CSmsPDUReadLogger::ChangeLogEvent()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREADLOGGER_CHANGELOGEVENT_1, "CSmsPDUReadLogger::ChangeLogEvent()");
	
	iState = ESmsPDUReadLoggerChange;	
	iLogger->ChangeEvent(iStatus, *iSmsMessage, iTimePtr, *iSmsPDUData);
	SetActive();
	} // CSmsPDUReadLogger::ChangeLogEvent


void CSmsPDUReadLogger::DoRunL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREADLOGGER_DORUNL_1, "CSmsPDUReadLogger::DoRunL()");

	switch (iState)
		{
		case ESmsPDUReadLoggerGet:
			{
			if (iStatus.Int() == KErrNone)
				{
				ChangeLogEvent();
				}
			else
				{
				AddLogEvent();
				}
			}
			break;

		case ESmsPDUReadLoggerAdd:
			{
			if (iStatus.Int() == KErrNone)
				{
				iSmsMessage->SetLogServerId(iLogger->Event().Id());
				}
			}
			break;

		case ESmsPDUReadLoggerChange:
			{
			// NOP
			}
			break;

		default:
			{
			SmspPanic(KSmspPanicUnexpectedState);
			}
			break;
		}
	// DoRunL() will now return to CSmsuActiveBase which if the object
	// is not active, will call Complete().
	}

void CSmsPDUReadLogger::DoCancel()
	{
	// Ignore in code coverage - cancel method called from CSmsPDURead
	// only if CSmsPDURead is itself cancelled during logging state. 
	// CSmsPDURead is only cancelled when SMS stack shuts down. These
	// conditions are very difficult to create in a deterministic way.
	BULLSEYE_OFF
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREADLOGGER_DOCANCEL_1, "CSmsPDUReadLogger::DoCancel()");

	iLogger->Cancel();

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
	BULLSEYE_RESTORE
	}

void CSmsPDUReadLogger::Complete(TInt aStatus)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUREADLOGGER_COMPLETE_1, "CSmsPDUReadLogger::Complete()");
    
    // Call the base function to perform the actual complete...
    CSmsuActiveBase::Complete(aStatus);
    
    iState = ESmsPDUReadLoggerIdle;
    } // CSmsPDUReadLogger::Complete
