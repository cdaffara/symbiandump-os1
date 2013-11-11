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
// Implements CSmsPhoneInitialization and CSmsModemNotification
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "smspmodmTraces.h"
#endif

#include "smspmodm.h"
#include "smsuset.h"
#include "smspmain.h"
#include "SmsuTimer.h"
#include "smspread.h"


//
// implementation of CSmsPhoneInitialization
//


/**
 *  D'tor
 */
CSmsPhoneInitialization::~CSmsPhoneInitialization()
	{
	Cancel();
	} // CSmsPhoneInitialization::~CSmsPhoneInitialization


/**
 *  start initializing the phone
 */
void CSmsPhoneInitialization::Start()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPHONEINITIALIZATION_START_1, "CSmsPhoneInitialization::Start  Start initializing the phone");

	Cancel();

	iInitialized = KErrNotReady;
	iNetworkInfoAvailable =EFalse;

	iState=ESmsPhoneInitializationPhoneInit;
	iGsmPhone.Initialise(iStatus);
	SetActive();
	} // CSmsPhoneInitialization::Start


void CSmsPhoneInitialization::DoRunL()
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPHONEINITIALIZATION_DORUNL_1, "CSmsPhoneInitialization::DoRunL [iStatus=%d, iState=%d]", iStatus.Int(), iState);

	switch (iState)
		{
		case ESmsPhoneInitializationPhoneInit:
			{
			AfterPhoneInitialize(iStatus.Int());
			}
			break;

		case ESmsPhoneInitializationSettingMoBearer:
			{
			if (iStatus.Int() != KErrNone)
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPHONEINITIALIZATION_DORUNL_2, "ERROR - TSY failed to set bearer but initialisation will continue; bearer setting may be successful during send");
				}

			iState=ESmsPhoneInitializationGettingRegistrationStatus;
			iGsmPhone.GetNetworkRegistrationStatus(iStatus, iRegistrationStatus);
			SetActive();
			}
			break;

		case ESmsPhoneInitializationGettingRegistrationStatus:
			{
			if(iStatus == KErrNone)
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPHONEINITIALIZATION_DORUNL_3, "CSmsPhoneInitialization::DoRunL registration status  : >%d<",iRegistrationStatus);
				if (iRegistrationStatus==RMobilePhone::ERegisteredOnHomeNetwork  ||
					iRegistrationStatus == RMobilePhone::ERegisteredRoaming)
					{
					iState=ESmsPhoneInitializationGettingCurrentNetworkInfo;
					iGsmPhone.GetCurrentNetwork(iStatus, iNetworkInfoPckg);
					SetActive();
					}
				else
					{
					iState=ESmsPhoneInitializationWaitingForCurrentNetwork;
					iGsmPhone.NotifyCurrentNetworkChange(iStatus,iNetworkInfoPckg);
					SetActive();
					}
				}
			else
				{
				iState=ESmsPhoneInitializationCompleted;
				iNetworkInfoAvailable =EFalse;
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPHONEINITIALIZATION_DORUNL_4, "CSmsPhoneInitialization::DoRunL NETWORK INFO NOT available due %d",iStatus.Int());
				iSmsPDURead.Start();
				}
			}
			break;

		case ESmsPhoneInitializationWaitingForCurrentNetwork:
			{
			if(iStatus == KErrNone)
				{
				iState=ESmsPhoneInitializationGettingCurrentNetworkInfo;
				iGsmPhone.GetCurrentNetwork(iStatus, iNetworkInfoPckg);
				SetActive();
				}
			else
				{
				iState=ESmsPhoneInitializationCompleted;
				iNetworkInfoAvailable =EFalse;
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPHONEINITIALIZATION_DORUNL_5, "CSmsPhoneInitialization::DoRunL NETWORK INFO NOT available due %d",iStatus.Int());
				iSmsPDURead.Start();
				}
			}
			break;

		case ESmsPhoneInitializationGettingCurrentNetworkInfo:
			{
			iState=ESmsPhoneInitializationCompleted;
			if(iStatus == KErrNone)
				{
				iNetworkInfoAvailable =ETrue;
				OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPHONEINITIALIZATION_DORUNL_6, "CSmsPhoneInitialization::DoRunL network longname : >%S<",iNetworkInfo.iLongName);
				OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPHONEINITIALIZATION_DORUNL_7, "CSmsPhoneInitialization::DoRunL network shortname : >%S<",iNetworkInfo.iShortName);
				OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPHONEINITIALIZATION_DORUNL_8, "CSmsPhoneInitialization::DoRunL network shortname : >%S<",iNetworkInfo.iDisplayTag);
				}
			else
				{
				iNetworkInfoAvailable =EFalse;
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPHONEINITIALIZATION_DORUNL_9, "CSmsPhoneInitialization::DoRunL NETWORK INFO NOT available due %d",iStatus.Int());
				}
			iSmsPDURead.Start();
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
	} // CSmsPhoneInitialization::DoRunL


/**
 *  cancel the initialization
 */
void CSmsPhoneInitialization::DoCancel()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPHONEINITIALIZATION_DOCANCEL_1, "CSmsPhoneInitialization::DoCancel()");

	switch (iState)
		{
		case ESmsPhoneInitializationPhoneInit:
			{
			iGsmPhone.InitialiseCancel();
			}
			break;

		case ESmsPhoneInitializationSettingMoBearer:
			{
			iInitialisationSetBearer->Cancel();
			}
			break;

		case ESmsPhoneInitializationGettingRegistrationStatus:
			{
			iGsmPhone.CancelAsyncRequest(EMobilePhoneGetNetworkRegistrationStatus);
			}
			break;

		case ESmsPhoneInitializationWaitingForCurrentNetwork:
			{
			iGsmPhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkNoLocationChange);
			}
			break;

		case ESmsPhoneInitializationGettingCurrentNetworkInfo:
			{
			iGsmPhone.CancelAsyncRequest(EMobilePhoneGetCurrentNetwork);
			}
			break;

		case ESmsPhoneInitializationCompleted:
		case ESmsPhoneInitializationIdle:
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

	iInitialized = KErrNotReady;
	} // CSmsPhoneInitialization::DoCancel


/**
 *  Completes the phone initialization request
 *  
 *  @param aStatus a status value
 */
void CSmsPhoneInitialization::Complete(TInt aStatus)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPHONEINITIALIZATION_COMPLETE_1, "CSmsPhoneInitialization::Complete()");

	//
	// Call the base function to perform the actual complete...
	//
	CSmsuActiveBase::Complete(aStatus);
	} // CSmsPhoneInitialization::Complete


/**
 *  Read the TSY caps and start CSmsPDURead if aStatus == KErrNone
 *  Otherwise, issue another asynchronous Initialize request.
 *  
 *  @param aStatus An error code indicating the status returned on the
 *  previous call to RPhone::Initialise().
 */
void CSmsPhoneInitialization::AfterPhoneInitialize(TInt aStatus)
	{
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPHONEINITIALIZATION_AFTERPHONEINITIALIZE_1, "CSmsPhoneInitialization::AfterPhoneIntialize [status=%d]", aStatus);

	if (aStatus == KErrNone)
		{
		RMobileSmsMessaging::TMobileSmsCapsV1Pckg smsCapsPckg(iMobileSmsCaps);
		aStatus = iSmsMessaging.GetCaps(smsCapsPckg);
		}

	if (aStatus == KErrNone)
		{
		if (iMobileSmsCaps.iSmsMode != RMobileSmsMessaging::KCapsGsmSms)
			{
		    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPHONEINITIALIZATION_AFTERPHONEINITIALIZE_2, "TSY does not support GsmSms");
			aStatus = KErrNotSupported;
			}
		else
			{
			// Initialisation of phone is complete and phone supports GSM SMS.
			// Therefore, set the MO SMS bearer.
			iState=ESmsPhoneInitializationSettingMoBearer;
			iInitialisationSetBearer->NotifyBearerSet(iStatus);
			SetActive();
			}
		}
	else
		{
	    if (++iNumberOfAttempts < iMaxNumberOfAttempts)
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPHONEINITIALIZATION_AFTERPHONEINITIALIZE_3, "CSmsPhoneInitialization Restarting [aStatus=%d]", aStatus);
			iGsmPhone.Initialise(iStatus);
			SetActive();
			}
		else
			{
			OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPHONEINITIALIZATION_AFTERPHONEINITIALIZE_4, "CSmsPhoneInitialization Failed after %d attempts [aStatus=%d]", iNumberOfAttempts, aStatus);
			}
		}

	iInitialized = aStatus;
	} // CSmsPhoneInitialization::AfterPhoneInitialize


/**
 *  Default constructor.
 */
CSmsPhoneInitialization::CSmsPhoneInitialization(RMobileSmsMessaging& aSmsMessaging, RMobilePhone& aGsmPhone, CSmsPDURead& aSmsPDURead,
                                                 RMobileSmsMessaging::TMobileSmsCapsV1& aMobileSmsCaps, TInt aPriority, CSmspSetBearer* aInitialisationSetBearer)
	:CSmsuActiveBase(aPriority)
	,iState(ESmsPhoneInitializationPhoneInit)
	,iSmsMessaging(aSmsMessaging)
	,iGsmPhone(aGsmPhone)
	,iSmsPDURead(aSmsPDURead)
	,iMobileSmsCaps(aMobileSmsCaps)
	,iMaxNumberOfAttempts(KMaxNumberOfInitiaisationAttempts)
	,iNetworkInfoPckg(iNetworkInfo)
	,iInitialisationSetBearer(aInitialisationSetBearer)
	{
	} // RMobileSmsMessaging::TMobileSmsCapsV1

//
// implementation of CSmsModemNotification
//


/**
 *  2 phase constructor - safely create a CSmsModemNotification object
 */
CSmsModemNotification* CSmsModemNotification::NewL(MSmsComm& aSmsComm)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMODEMNOTIFICATION_NEWL_1, "CSmsModemNotification::NewL");

	CSmsModemNotification*	self=new(ELeave) CSmsModemNotification(aSmsComm);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
	} // CSmsModemNotification::NewL


void CSmsModemNotification::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMODEMNOTIFICATION_CONSTRUCTL_1, "CSmsModemNotification::ConstructL()");

	User::LeaveIfError(iPhonePowerProperty.Attach(KUidSystemCategory, KUidPhonePwr.iUid));
	
	ConstructTimeoutL();
	} // CSmsModemNotification::ConstructL


/**
 *  D'tor
 */
CSmsModemNotification::~CSmsModemNotification()
	{
	Cancel();
	iPhonePowerProperty.Close();
	} // CSmsModemNotification::~CSmsModemNotification


RPhone::TModemDetection CSmsModemNotification::ModemState()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMODEMNOTIFICATION_MODEMSTATE_1, "CSmsModemNotification::ModemState()");

	//
	// Get the phone power state.
	//
	TInt phonePowerState;

	if (iPhonePowerProperty.Get(phonePowerState) != KErrNone)
		{
		phonePowerState = ESAPhoneOff;
		}

	if (phonePowerState == ESAPhoneOn)
		{
		return RPhone::EDetectedPresent;
		}
	else
		{
		return RPhone::EDetectedNotPresent;
		}
	} // CSmsModemNotification::ModemState


/**
 *  start the modem notification
 */
void CSmsModemNotification::Start()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMODEMNOTIFICATION_START_1, "CSmsModemNotification::Start");

	__ASSERT_DEBUG(iState==ESmsModemNotificationIdle,SmspPanic(KSmspPanicUnexpectedState));

	//
	// Subscribe to the power property...
	//
	iState = ESmsModemNotificationWaitingForNotification;
	iPhonePowerProperty.Subscribe(iStatus);

	SetActive();
	} // CSmsModemNotification::Start


/**
 *  asyncronous completion - called by ActiveScheduler when modem notification completed
 */
void CSmsModemNotification::DoRunL()
	{
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMODEMNOTIFICATION_DORUNL_1, "CSmsModemNotification::RunL [iStatus=%d]", iStatus.Int() );

	__ASSERT_DEBUG(iState==ESmsModemNotificationWaitingForNotification,SmspPanic(KSmspPanicUnexpectedState));

	//
	// DoRunL() will now return to CSmsuActiveBase which if the object
	// is not active, will call Complete().
	//
	} // CSmsModemNotification::DoRunL


/**
 *  cancel the modem notification
 */
void CSmsModemNotification::DoCancel()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMODEMNOTIFICATION_DOCANCEL_1, "CSmsModemNotification::DoCancel()");

	__ASSERT_DEBUG(iState==ESmsModemNotificationWaitingForNotification,SmspPanic(KSmspPanicUnexpectedState));
	
	//
	// Cancel the request...
	//
	iPhonePowerProperty.Cancel();

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
	} // CSmsModemNotification::DoCancel


/**
 *  C'tor
 */
CSmsModemNotification::CSmsModemNotification(MSmsComm& aSmsComm)
  : CSmsuActiveBase(KSmsSessionPriority),
	iState(ESmsModemNotificationIdle),
	iSmsComm(aSmsComm)
	{
	} // CSmsModemNotification::CSmsModemNotification


/**
 *  complete the modem notification - inform the observer
 */
void CSmsModemNotification::Complete(TInt aStatus)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMODEMNOTIFICATION_COMPLETE_1, "CSmsModemNotification::Complete()");

	//
	// Call the base function to perform the actual complete...
	//
	CSmsuActiveBase::Complete(aStatus);

	iState = ESmsModemNotificationIdle;

	//
	// If the request was not cancelled, then restart ourselves ready
	// for the next event...
	//
	if (aStatus != KErrCancel  &&  aStatus != KErrTimedOut)
		{
		Start(); // starting here to ensure no states are missed
		}

	//
	// Notify our client that the event occured...
	//
	RPhone::TModemDetection  modemDetection = ModemState();

	iSmsComm.ModemNotificationCompleted(aStatus, modemDetection);
	} // CSmsModemNotification::Complete
