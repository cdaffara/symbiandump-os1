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
// Implements the CSmspReceiveMode and CSmspReceiveModeNotifier classes
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "smsprecvmodeTraces.h"
#endif

#include <commsdattypesv1_1.h>
#include <cdbcols.h>

#include "smspcdb.h"
#include "smspmain.h"
#include <commdb.h>
#include "smsuset.h"
#include "SmsuTimer.h"

using namespace CommsDat;


/**
 *  CSmspReceiveMode constructor
 */
CSmspReceiveMode::CSmspReceiveMode(const TSmsSettings& aSmsSettings,
								   RMobileSmsMessaging& aSmsMessaging,
								   const RMobileSmsMessaging::TMobileSmsCapsV1& aSmsCaps,
								   TInt aPriority)
  : CSmspCommDbEvent(aSmsSettings, aSmsMessaging, aPriority),
	iSmsReceiveModeLastSet(RMobileSmsMessaging::EReceiveModeUnspecified),
	iSmsCaps(aSmsCaps)
	{
	// NOP
	} // CSmspReceiveMode::CSmspReceiveMode


/**
 *  CSmspReceiveMode destructor
 */
CSmspReceiveMode::~CSmspReceiveMode()
	{
	Cancel();
	iCommDbRetryTimer.Close();
	} // CSmspReceiveMode::~CSmspReceiveMode


/**
 *  Two-phase construction of CSmspReceiveMode
 */
CSmspReceiveMode* CSmspReceiveMode::NewL(const TSmsSettings& aSmsSettings, RMobileSmsMessaging& aSmsMessaging, const RMobileSmsMessaging::TMobileSmsCapsV1& aSmsCaps, TInt aPriority)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPRECEIVEMODE_NEWL_1, "CSmspReceiveMode::NewL()");

	CSmspReceiveMode* self = new (ELeave) CSmspReceiveMode(aSmsSettings, aSmsMessaging, aSmsCaps, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	} // CSmspReceiveMode::NewL


void CSmspReceiveMode::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPRECEIVEMODE_CONSTRUCTL_1, "CSmspReceiveMode::ConstructL()");

	CSmspCommDbEvent::ConstructL();
	User::LeaveIfError(iCommDbRetryTimer.CreateLocal());
	User::LeaveIfError(iProperty.Attach(KUidSystemCategory, KUidCommDbSMSReceiveModeChange.iUid));
	} // CSmspReceiveMode::ConstructL


/**
 *  Intially sets the SMS Receive Mode on the TSY or waits for notification of the CommDB global setting SMS_RECEIVE_MODE to change
 */
void CSmspReceiveMode::Start(TRequestStatus& aStatus)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPRECEIVEMODE_START_1, "CSmspReceiveMode::Start()");

	Cancel();
	Queue(aStatus);

	RMobileSmsMessaging::TMobileSmsReceiveMode recvMode(SelectReceiveMode());
	
	//
	// Until we manage to read the CommDB the default choice is the best known
	//
	if (iSmsReceiveModeLastSet == RMobileSmsMessaging::EReceiveModeUnspecified)
		{
		iSmsReceiveModeLastSet = recvMode;
		}

	TRAPD(err, GetCommDbReceiveModeL(recvMode));
	if(err == KErrNone)
		{
		SetReceiveMode(recvMode);
		}
	else
		{
		iState = ESmspReceiveModeRetryCommDb;
		iCommDbRetryCount = KSmspReceiveModeCommDbRetryLimit;
		iCommDbRetryTimer.After(iStatus, KSmspReceiveModeCommDbRetryDelay);
		SetActive();
		}
	} // CSmspReceiveMode::Start


/**
 *  Wait for notification from property for when the CommDB global setting SMS_RECEIVE_MODE changes
 */
void CSmspReceiveMode::NotifyOnEvent()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPRECEIVEMODE_NOTIFYONEVENT_1, "CSmspReceiveMode::NotifyOnEvent()");

	iState = ESmspReceiveModeNotifyOnEvent;
	CSmspCommDbEvent::NotifyOnEvent();
	} // CSmspReceiveMode::NotifyOnEvent


/**
 *  Sets the SMS Receive Mode on the TSY
 *  
 *  @param aReceiveMode SMS Receive Mode to set on the TSY
 */
void CSmspReceiveMode::SetReceiveMode(RMobileSmsMessaging::TMobileSmsReceiveMode aReceiveMode)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPRECEIVEMODE_SETRECEIVEMODE_1, "CSmspReceiveMode::SetReceiveMode()");

	aReceiveMode = SelectReceiveMode(aReceiveMode);
	iSmsReceiveModeLastSet = aReceiveMode;

	iState = ESmspReceiveModeSet;
	iSmsMessaging.SetReceiveMode(iStatus, aReceiveMode);
	TimedSetActive(iSmsSettings.Timeout());
	} // CSmspReceiveMode::SetReceiveMode


void CSmspReceiveMode::DoCancel()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPRECEIVEMODE_DOCANCEL_1, "CSmspReceiveMode::DoCancel()");

	TimedSetActiveCancel();
	switch (iState)
		{
		case ESmspReceiveModeInit:
			{
			// NOP
			}
			break;

		case ESmspReceiveModeNotifyOnEvent:
			{
			iProperty.Cancel();
			}
			break;

		case ESmspReceiveModeSet:
			{
			iSmsMessaging.CancelAsyncRequest(EMobileSmsMessagingSetReceiveMode);
			}
			break;

		case ESmspReceiveModeRetryCommDb:
			{
			iCommDbRetryTimer.Cancel();
			}
			break;

		default:
			{
			SmspPanic(KSmspPanicUnexpectedState);
			}
			break;
		}

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
	} // CSmspReceiveMode::DoCancel


/**
 *  Read Comms Database global setting SMS_RECEIVE_MODE
 *  
 *  @param aReceiveMode Will be set to the CommsDat global setting SMS_RECEIVE_MODE
 *  @return Error code if we cannot create the session, or load from the db
 */
void CSmspReceiveMode::GetCommDbReceiveModeL(RMobileSmsMessaging::TMobileSmsReceiveMode& aReceiveMode)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPRECEIVEMODE_GETCOMMDBRECEIVEMODEL_1, "CSmspReceiveMode::GetCommDbReceiveModeL()");

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession* sess = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession* sess = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(sess);

	CMDBField<TUint32>* smsReceiveModeField = new(ELeave) CMDBField<TUint32>(KCDTIdSMSReceiveMode);
	CleanupStack::PushL(smsReceiveModeField);
	smsReceiveModeField->SetRecordId(1);

	smsReceiveModeField->LoadL(*sess);
	aReceiveMode = static_cast<RMobileSmsMessaging::TMobileSmsReceiveMode>(static_cast<TUint32>(*smsReceiveModeField));

	CleanupStack::PopAndDestroy(smsReceiveModeField);
	CleanupStack::PopAndDestroy(sess);
	} // CSmspReceiveMode::GetCommDbReceiveModeL


void CSmspReceiveMode::DoRunL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPRECEIVEMODE_DORUNL_1, "CSmspReceiveMode::DoRunL()");

	switch (iState)
		{
		case ESmspReceiveModeSet:
			{
			CSmsuActiveBase::Complete(iStatus.Int());

			if (iStatus.Int() == KErrNone)
				{
				NotifyOnEvent();
				}
			}
			break;

		case ESmspReceiveModeRetryCommDb:
			{
			if (iStatus.Int() == KErrNone)
				{
				RMobileSmsMessaging::TMobileSmsReceiveMode recvMode;
				TRAPD(err, GetCommDbReceiveModeL(recvMode));
				if(err == KErrNone)
					{
					SetReceiveMode(recvMode);
					}
				else if(--iCommDbRetryCount > 0)
					{
					iCommDbRetryTimer.After(iStatus, KSmspReceiveModeCommDbRetryDelay);
					SetActive();
					}
				else
					{
					// Failed to read CommDB even after retries. Set the receive mode to the last known setting
					OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPRECEIVEMODE_DORUNL_2, "CSmspReceiveMode::DoRunL in ESmspReceiveModeRetryCommDb state, failed to read CommDb, using mode #%d", iSmsReceiveModeLastSet );
					SetReceiveMode(iSmsReceiveModeLastSet);
					}
				}
			}
			break;

		case ESmspReceiveModeNotifyOnEvent:
			{
			if (iStatus.Int() == KErrNone)
				{
				TInt tempRecvMode;
				User::LeaveIfError(iProperty.Get(tempRecvMode));
				const RMobileSmsMessaging::TMobileSmsReceiveMode recvMode = static_cast<RMobileSmsMessaging::TMobileSmsReceiveMode>(tempRecvMode);

				SetReceiveMode(recvMode);
				}
			else
				{
				NotifyOnEvent();
				}
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
	} // CSmspReceiveMode::DoRunL


/**
 *  Finds an SMS Receive Mode supported by the TSY, giving preference to EReceiveUnstoredClientAck and so forth.
 *  
 *  @return An SMS Receive Mode supported by the TSY.
 */
RMobileSmsMessaging::TMobileSmsReceiveMode CSmspReceiveMode::SelectReceiveMode() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPRECEIVEMODE_SELECTRECEIVEMODE_1, "CSmspReceiveMode::SelectReceiveMode()");

	const TUint32 smsControl = iSmsCaps.iSmsControl;
	RMobileSmsMessaging::TMobileSmsReceiveMode recvMode = RMobileSmsMessaging::EReceiveModeUnspecified;

	if (smsControl & RMobileSmsMessaging::KCapsReceiveUnstoredClientAck)
		{
		recvMode = RMobileSmsMessaging::EReceiveUnstoredClientAck;
		}
	else if (smsControl & RMobileSmsMessaging::KCapsReceiveUnstoredPhoneAck)
		{
		recvMode =  RMobileSmsMessaging::EReceiveUnstoredPhoneAck;
		}
	else if (smsControl & RMobileSmsMessaging::KCapsReceiveStored)
		{
		recvMode = RMobileSmsMessaging::EReceiveStored;
		}
	else if (smsControl & RMobileSmsMessaging::KCapsReceiveEither)
		{
		recvMode =  RMobileSmsMessaging::EReceiveEither;
		}

	return recvMode;
	} // CSmspReceiveMode::SelectReceiveMode


/**
 *  Finds an SMS Receive Mode supported by the TSY, giving preference to aPreferredMode.
 *  
 *  @param aPreferredMode SMS Receive Mode that a client would like to use.
 *  @return aPreferredMode if it is supported by the TSY, otherwise return a receive mode that is supported by the TSY.
 */
RMobileSmsMessaging::TMobileSmsReceiveMode CSmspReceiveMode::SelectReceiveMode(RMobileSmsMessaging::TMobileSmsReceiveMode aPreferredMode) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPRECEIVEMODE_SELECTRECEIVEMODE1_1, "CSmspReceiveMode::SelectReceiveMode()");

	const TUint32 smsControl = iSmsCaps.iSmsControl;
	TBool usePreferred = EFalse;

	switch (aPreferredMode)
		{
		case RMobileSmsMessaging::EReceiveUnstoredClientAck:

			usePreferred = smsControl & RMobileSmsMessaging::KCapsReceiveUnstoredClientAck;
			break;

		case RMobileSmsMessaging::EReceiveUnstoredPhoneAck:

			usePreferred = smsControl & RMobileSmsMessaging::KCapsReceiveUnstoredPhoneAck;
			break;

		case RMobileSmsMessaging::EReceiveStored:

			usePreferred = smsControl & RMobileSmsMessaging::KCapsReceiveStored;
			break;

		case RMobileSmsMessaging::EReceiveEither:

			usePreferred = smsControl & RMobileSmsMessaging::KCapsReceiveEither;
			break;

		default:

			//usePreferred = EFalse;
			break;
		}

	return usePreferred ? aPreferredMode : SelectReceiveMode();
	} // CSmspReceiveMode::SelectReceiveMode
