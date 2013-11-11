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
// Implements the CSmsEventLogerClass
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "smsulogTraces.h"
#endif

#include "smsulog.h"
#include "smsumain.h"
#include "smsstacklog.h"
#include "gsmubuf.h"
#include "Gsmumsg.h"
#include "Gsmuelem.h"
#include <logwraplimits.h>


/**
 *  Allocates and creates a logger.
 *  
 *  @param aFs File server handle
 *  @param aPriority Active object priority
 *  @return New CSmsEventLogger object
 *  @capability None
 */
EXPORT_C CSmsEventLogger* CSmsEventLogger::NewL(RFs& aFs,TInt aPriority)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSEVENTLOGGER_NEWL_1, "CSmsEventLogger::NewL()");

	CSmsEventLogger* smseventlogger = new(ELeave) CSmsEventLogger(aPriority);
	CleanupStack::PushL(smseventlogger);
	smseventlogger->ConstructL(aFs);
	CleanupStack::Pop();
	return smseventlogger;
	} // CSmsEventLogger::NewL


/**
 *  Destructor.
 *  @capability None
 */
EXPORT_C CSmsEventLogger::~CSmsEventLogger()
	{
	Cancel();
	delete iLogEvent;
	delete iLogWrapper;
	} // CSmsEventLogger::~CSmsEventLogger


/**
 *  Adds an event to the log database.
 *  
 *  @param aStatus Asynchronous status word to signal when the operation is complete
 *  @param aSmsMessage The SMS message that the event concerns
 *  @param aData SMS PDU information for the event
 *  @param aStatusId Optional string to include in the log message, specified by
 *  resource ID
 *  @capability WriteUserData
 */
EXPORT_C void CSmsEventLogger::AddEvent(TRequestStatus& aStatus,const CSmsMessage& aSmsMessage,const TLogSmsPduData& aData,TInt* aStatusId)
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSEVENTLOGGER_ADDEVENT_1, "CSmsEventLogger::AddEvent");

	__ASSERT_DEBUG(iState==ESmsEventLoggerIdle,SmsuPanic(KSmsuPanicUnexpectedState));
	
	iState=ESmsEventLoggerAddingEvent;

	Queue(aStatus);

	iSmsPDUData=aData;
	iSmsPDUData.iType=aSmsMessage.Type();

	TRAPD(ret,DoAddEventL(aSmsMessage,aStatusId));
	if (ret!=KErrNone)
		{
		CompleteMyself(ret);
		}
	else
		{
		iLogWrapper->Log().AddEvent(*iLogEvent,iStatus);
		SetActive();
		}
	} // CSmsEventLogger::AddEvent


/**
 *  Gets the specified event from the log.
 *  
 *  The event itself can be obtained using a subsequent call to Event().
 *  
 *  @param aStatus Asynchronous status word to signal when the operation is complete
 *  @param aId Log event ID for the event
 *  @capability ReadUserData
 */
EXPORT_C void CSmsEventLogger::GetEvent(TRequestStatus& aStatus,TLogId aId)
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSEVENTLOGGER_GETEVENT_1, "CSmsEventLogger::GetEvent");

	__ASSERT_DEBUG(iState==ESmsEventLoggerIdle,SmsuPanic(KSmsuPanicUnexpectedState));
	iState=ESmsEventLoggerGettingEvent;
	Queue(aStatus);
	iLogEvent->SetId(aId);
	iLogWrapper->Log().GetEvent(*iLogEvent,iStatus);
	SetActive();
	} // CSmsEventLogger::GetEvent

/** Changes the details of an existing event.
 
    Use GetEvent() to specify the event.
 
    @param aStatus       Asynchronous status word to signal when the operation is complete
    @param aSmsMessage   The SMS message that the event concerns
    @param aTime SMS delivery time
    @param aData SMS PDU information for the event
    @param aStatusId Optional string to include in the log message, specified by
    resource ID
    @capability WriteUserData
*/
EXPORT_C void CSmsEventLogger::ChangeEvent(TRequestStatus& aStatus,const CSmsMessage& aSmsMessage, const TTime* aTime, const TLogSmsPduData& aData, TInt* aStatusId)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSEVENTLOGGER_CHANGEEVENT_1, "CSmsEventLogger::ChangeEvent");
    
    __ASSERT_DEBUG(iState==ESmsEventLoggerIdle,SmsuPanic(KSmsuPanicUnexpectedState));
    __ASSERT_DEBUG(aSmsMessage.LogServerId()==iLogEvent->Id(),SmsuPanic(KSmsuPanicWrongLogServerId));
    __ASSERT_DEBUG(iSmsPDUData.iType==aSmsMessage.Type(),SmsuPanic(ESmsuUnexpectedSmsPDUType));
    iState=ESmsEventLoggerChangingEvent;
    Queue(aStatus);
    iSmsPDUData=aData;
    iSmsPDUData.iType=aSmsMessage.Type();        

    TRAPD(ret,SetDataL(aSmsMessage,aStatusId, aTime));
    if (ret!=KErrNone)
        {
        Complete(ret);
        }
    else
        {
        iLogWrapper->Log().ChangeEvent(*iLogEvent,iStatus);
        SetActive();
        }
    }

/**
 *  Changes the details of an existing event.
 *  
 *  Use GetEvent() to specify the event.
 *  
 *  @param aStatus Asynchronous status word to signal when the operation is complete
 *  @param aSmsMessage The SMS message that the event concerns
 *  @param aData SMS PDU information for the event
 *  @param aStatusId Optional string to include in the log message, specified by
 *  resource ID
 *  @capability WriteUserData
 */
EXPORT_C void CSmsEventLogger::ChangeEvent(TRequestStatus& aStatus, const CSmsMessage& aSmsMessage, const TLogSmsPduData& aData, TInt* aStatusId)
	{
	ChangeEvent(aStatus, aSmsMessage, NULL, aData, aStatusId);
	} // CSmsEventLogger::ChangeEvent

/**
 *  Deletes an event.
 *  
 *  Use GetEvent() to specify the event.
 *  
 *  @param aStatus Asynchronous status word to signal when the operation is complete
 *  @capability WriteUserData
 */
EXPORT_C void CSmsEventLogger::DeleteEvent(TRequestStatus& aStatus)
    {
    // Ignore in code coverage - not used within the SMS stack.
    BULLSEYE_OFF
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSEVENTLOGGER_DELETEEVENT_1, "CSmsEventLogger::DeleteEvent");
    
    __ASSERT_DEBUG(iState==ESmsEventLoggerIdle,SmsuPanic(KSmsuPanicUnexpectedState));
    
    iState=ESmsEventLoggerDeletingEvent;
    
    Queue(aStatus);
    
    iLogWrapper->Log().DeleteEvent(iLogEvent->Id(),iStatus);
    SetActive();
    BULLSEYE_RESTORE
    }

void CSmsEventLogger::DoCancel()
    {
    // Ignore in code coverage - in order to test would need to cancel
    // event logger request; as these called when receiving/sending SMS
    // message would need to cancel these activities during event the 
    // logger request - too difficult to test. Need to add unit test.
    BULLSEYE_OFF
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_DOCANCEL_1, "CSmsEventLogger::DoCancel [iStatus=%d, iState=%d]", iStatus.Int(), iState);
    
    switch( iState )
        {
        case ESmsEventLoggerGettingEvent:
        case ESmsEventLoggerAddingEvent:
        case ESmsEventLoggerChangingEvent:
        case ESmsEventLoggerDeletingEvent:
            {
            iLogWrapper->Log().Cancel();
            } break;
        default:
            {
            SmsuPanic(KSmsuPanicUnexpectedState);
            } break;
        }
    // Handle completion of this Active Object. Note that the object
    // may well still be active at this point...
    if( TimedOut() )
        {
        Complete(KErrTimedOut);
        }
    else
        {
        Complete(KErrCancel);
        }
    BULLSEYE_RESTORE
    }

CSmsEventLogger::CSmsEventLogger(TInt aPriority):
	CSmsuActiveBase(aPriority),
	iState(ESmsEventLoggerIdle)
	{
	// NOP
	}


void CSmsEventLogger::ConstructL(RFs& aFs)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_CONSTRUCTL_1, "CSmsEventLogger::ConstructL()");

	iLogWrapper=CLogWrapper::NewL(aFs);
	iLogEvent=CLogEvent::NewL();
	} // CSmsEventLogger::ConstructL


void CSmsEventLogger::DoRunL()
	{
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_DORUNL_1, "CSmsEventLogger::DoRunL [iStatus=%d]", iStatus.Int() );

	switch (iState)
		{
		case ESmsEventLoggerGettingEvent:
			{
			if (iStatus.Int() == KErrNone)
				{
				TPckg<TLogSmsPduData> packeddata(iSmsPDUData);
				packeddata.Copy(iLogEvent->Data());
				}
			}
            break;

		case ESmsEventLoggerAddingEvent:
		case ESmsEventLoggerChangingEvent:
		case ESmsEventLoggerDeletingEvent:
			{
			// NOP
			}
			break;

		default:
			{
            User::Leave(KErrGeneral);
            //SmsuPanic(KSmsuPanicUnexpectedState);
            }
            break;
		}

	//
	// DoRunL() will now return to CSmsuActiveBase which if the object
	// is not active, will call Complete().
	//
	} // CSmsEventLogger::DoRunL


void CSmsEventLogger::DoAddEventL(const CSmsMessage& aSmsMessage,TInt* aStatusId)
	{
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_DOADDEVENTL_1, "CSmsEventLogger::DoAddEventL [statusID*=%d]", aStatusId);

	 //  Reset event
	CLogEvent* logevent=CLogEvent::NewL();
	delete iLogEvent;
	iLogEvent=logevent;
	TBuf<KLogMaxDirectionLength> direction;
	switch (aSmsMessage.Type())
		{
		//  Incoming SMS
		case CSmsPDU::ESmsDeliver:
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_DOADDEVENTL_2, "DoAddEventL ESmsDeliver");
			GetStringL(direction,R_LOG_DIR_IN);
			break;
			}
		case CSmsPDU::ESmsStatusReport:
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_DOADDEVENTL_3, "DoAddEventL ESmsStatusReport");
			GetStringL(direction,R_LOG_DIR_IN);
			break;
			}
		//  Outgoing SMS
		case CSmsPDU::ESmsSubmit:
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_DOADDEVENTL_4, "DoAddEventL ESmsSubmit");
			GetStringL(direction,R_LOG_DIR_OUT);
			break;
			}
		case CSmsPDU::ESmsCommand:
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_DOADDEVENTL_5, "DoAddEventL ESmsCommand");
			GetStringL(direction,R_LOG_DIR_OUT);
			break;
			}
		default:
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_DOADDEVENTL_6, "DoAddEventL ESmsuUnexpectedSmsPDUType");
			SmsuPanic(ESmsuUnexpectedSmsPDUType);
			}
		}

	Event().SetDirection(direction);
	Event().SetEventType(KLogShortMessageEventTypeUid);

	TGsmSmsTelNumber address;
	aSmsMessage.ParsedToFromAddress(address);

	if (address.IsInstanceOf(TGsmSmsTelNumber::EVoiceMessageWaitingIndicator))
    	{
	    Event().SetNumber(_L("CPHS"));
	    }
	    else
	    {
	    Event().SetNumber(aSmsMessage.ToFromAddress());
	    }

	SetDataL(aSmsMessage,aStatusId, NULL);
	} // CSmsEventLogger::DoAddEventL


void CSmsEventLogger::SetDataL(const CSmsMessage& aSmsMessage,TInt* aStatusId, const TTime* aDischargeTime)
	{

	__ASSERT_DEBUG(iSmsPDUData.iType==aSmsMessage.Type(),SmsuPanic(ESmsuUnexpectedSmsPDUType));
	TBuf<KLogMaxStatusLength> status;
#ifdef _DEBUG
	if (aStatusId!=NULL)
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_SETDATAL_1, "CSmsEventLogger::SetDataL StatusID = %d", *aStatusId);
#endif
	if (aStatusId==NULL)
		{
		CSmsPDU::TSmsPDUType type=aSmsMessage.Type();
		switch (type)
			{
			//  Incoming SMS
			case CSmsPDU::ESmsDeliver:
				{
				if (iSmsPDUData.iTotal==iSmsPDUData.iReceived)
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_SETDATAL_2, "SetDataL ESmsDeliver R_LOG_DEL_DONE");
					GetStringL(status,R_LOG_DEL_DONE);
					}
				else if (iSmsPDUData.iTotal>iSmsPDUData.iReceived)
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_SETDATAL_3, "SetDataL ESmsDeliver R_LOG_DEL_PENDING");
					GetStringL(status,R_LOG_DEL_PENDING);
					}
				break;
				}
			case CSmsPDU::ESmsStatusReport:
				{
				if (iSmsPDUData.iTotal==iSmsPDUData.iReceived)
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_SETDATAL_4, "SetDataL ESmsStatusReport R_LOG_DEL_DONE");
					GetStringL(status,R_LOG_DEL_DONE);
					}
				else if (iSmsPDUData.iTotal>iSmsPDUData.iReceived)
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_SETDATAL_5, "SetDataL ESmsStatusReport R_LOG_DEL_PENDING");
					GetStringL(status,R_LOG_DEL_PENDING);
					}
				break;
				}
			//  Outgoing SMS
			case CSmsPDU::ESmsSubmit:
			case CSmsPDU::ESmsCommand:
				{
				TBool statusreportrequest=EFalse;
				if (type==CSmsPDU::ESmsSubmit)
					{
					CSmsSubmit& submit=(CSmsSubmit&) aSmsMessage.SmsPDU();
					statusreportrequest=submit.StatusReportRequest();
					}
				else
					{
					CSmsCommand& command=(CSmsCommand&) aSmsMessage.SmsPDU();
					statusreportrequest=command.StatusReportRequest();
					}
				if (statusreportrequest)
					{
					if (iSmsPDUData.iSent==0)
						{
						// None sent yet
						GetStringL(status,R_LOG_DEL_NOT_SENT);
						OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_SETDATAL_6, "SetDataL ESmsSubmit R_LOG_DEL_NOT_SENT SR");
						}
					else if (iSmsPDUData.iTotal==iSmsPDUData.iDelivered)
						{
						// All have been delivered
						OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_SETDATAL_7, "SetDataL ESmsSubmit R_LOG_DEL_DONE SR iSmsPDUData.iDelivered=%d", iSmsPDUData.iDelivered);
						GetStringL(status,R_LOG_DEL_DONE);
						}
					else if (iSmsPDUData.iSent<=iSmsPDUData.iTotal && iSmsPDUData.iFailed==0)
						{
						// One or more sent but not all, no failures
						OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_SETDATAL_8, "SetDataL ESmsSubmit R_LOG_DEL_PENDING SR iSmsPDUData.iSent==%d, iSmsPDUData.iTotal==%d, iFailed==0", iSmsPDUData.iSent, iSmsPDUData.iTotal);
						GetStringL(status,R_LOG_DEL_PENDING);
						}
					else
						{
						// One or more failures or corruption of iSmsPDUData values
						OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_SETDATAL_9, "SetDataL ESmsSubmit R_LOG_DEL_FAILED SR");
                        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_SETDATAL_10, "   Total:  %d, Sent: %d",iSmsPDUData.iTotal, iSmsPDUData.iSent );
                        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_SETDATAL_11, "   Failed: %d, Delivered: %d",iSmsPDUData.iFailed, iSmsPDUData.iDelivered );
                        GetStringL(status,R_LOG_DEL_FAILED);
                        }
                    if (aDischargeTime != NULL)
                        {
                        iLogEvent->SetTime(*aDischargeTime);
                        }                   
                    }
                else
                    {
                    if (iSmsPDUData.iSent<iSmsPDUData.iTotal)
                        {
                        // IF not all PDUs sent, state is "NOT SENT".
                        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_SETDATAL_12, "SetDataL ESmsSubmit R_LOG_DEL_NOT_SENT NOSR");
                        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_SETDATAL_13, "   Total:  %d, Sent: %d", iSmsPDUData.iTotal, iSmsPDUData.iSent );
                        GetStringL(status,R_LOG_DEL_NOT_SENT);
                        }
					else if (iSmsPDUData.iTotal==iSmsPDUData.iSent)
						{
						// All sent
						OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_SETDATAL_14, "SetDataL ESmsSubmit R_LOG_DEL_SENT NOSR");
						GetStringL(status,R_LOG_DEL_SENT);
						}
					else
						{
						// The iSmsPDUData values have become corrupt
						OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_SETDATAL_15, "SetDataL ESmsSubmit R_LOG_DEL_FAILED NOSR");
                        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_SETDATAL_16, "   Total:  %d, Sent: %d",iSmsPDUData.iTotal, iSmsPDUData.iSent );
                        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_SETDATAL_17, "   Failed: %d, Delivered: %d",iSmsPDUData.iFailed, iSmsPDUData.iDelivered );
                        GetStringL(status,R_LOG_DEL_FAILED);
                        }
                    }
                OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_SETDATAL_18, "SetDataL [status string=%S]",status);
                break;
                }
			default:
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_SETDATAL_19, "SetDataL ESmsuUnexpectedSmsPDUType");
				SmsuPanic(ESmsuUnexpectedSmsPDUType);
				}
			}
		}
	else
		GetStringL(status,*aStatusId);

 	TBuf<KLogMaxSubjectLength> subject;
	//
	// EDNAVSA-4VA9FQ Incoming SMSes with special chars are entered in the log as "data messages"
	// only 8 bit messages are displayed as data message in the log viewer
	//
 	if (aSmsMessage.SmsPDU().DataCodingSchemePresent() && aSmsMessage.SmsPDU().Alphabet() == TSmsDataCodingScheme::ESmsAlphabet8Bit)
 		{
 		GetStringL(subject, R_LOG_SUBJECT_DATA_MESSAGE);
 		}
 	else if (aSmsMessage.TextPresent())
   		{
   		const TInt length= Min(KLogMaxSubjectLength, aSmsMessage.Buffer().Length());
 		aSmsMessage.Buffer().Extract(subject, 0, length);
 		subject.Trim();
 		}
 	if (subject.Length() == 0)
 		{
 		GetStringL(subject, R_LOG_SUBJECT_NONE);
   		}
 	iLogEvent->SetSubject(subject);

	iLogEvent->SetStatus(status);
	TPckg<TLogSmsPduData> packeddata(iSmsPDUData);
	iLogEvent->SetDataL(packeddata);
	} // CSmsEventLogger::SetDataL


void CSmsEventLogger::DoComplete(TInt& aStatus)
	{
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEVENTLOGGER_DOCOMPLETE_1, "CSmsEventLogger::DoComplete(): aStatus=%d, iState=%d", aStatus, iState);

	//
	// Check the state is valid and finish up...
	//
	__ASSERT_DEBUG(iState == ESmsEventLoggerGettingEvent  ||
				   iState == ESmsEventLoggerAddingEvent  ||
				   iState == ESmsEventLoggerChangingEvent  ||
				   iState == ESmsEventLoggerDeletingEvent, SmsuPanic(KSmsuPanicUnexpectedState));

	aStatus = -aStatus;
	iState  = ESmsEventLoggerIdle;
	} // CSmsEventLogger::Complete

