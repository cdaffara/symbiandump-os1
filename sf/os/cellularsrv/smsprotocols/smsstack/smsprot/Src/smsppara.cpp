// Copyright (c) 2001-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "smspparaTraces.h"
#endif

#include "smsppara.h"
#include "smsuset.h"
#include "smsulog.h"
#include "smspmain.h"

#include "gsmubuf.h"
#include "Gsmumsg.h"
#include <mmretrieve.h>
#include "SmsuTimer.h"


/**
 *  Constructor
 *  
 *  @param aSmsComm a reference to the protocol.
 *  @param aSmsSettings a reference to the global SMS settings.
 *  @param aSmsMessaging a reference to the ETEL SMS messaging subsession.
 *  
 */
CSmsParamsBase::CSmsParamsBase(MSmsComm& aSmsComm,const TSmsSettings& aSmsSettings,RMobileSmsMessaging& aSmsMessaging)
  : CSmsuActiveBase(KSmsSessionPriority),
	iSmsComm(aSmsComm),
	iSmsSettings(aSmsSettings),
	iSmsMessaging(aSmsMessaging)
	{
	} // CSmsParamsBase::CSmsParamsBase


/**
 *  2nd Phase of construction.
 *  
 *  Creates CSmsTimeout object.
 *  
 */
void CSmsParamsBase::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPARAMSBASE_CONSTRUCTL_1, "CSmsParamsBase::ConstructL()");

	ConstructTimeoutL();
	} // CSmsParamsBase::ConstructL


/**
 *  2 Phase constructor.
 *  
 *  @param aSmsComm a reference to the protocol.
 *  @param aSmsSettings a reference to the global SMS settings.
 *  @param aSmsMessaging a reference to the ETEL SMS messaging subsession.
 *  @leave Leaves if ContructL() leaves, or not enough memory is available.
 *  @return a new CSmsReadParams object.
 *  
 */
CSmsReadParams* CSmsReadParams::NewL(MSmsComm& aSmsComm,const TSmsSettings& aSmsSettings,RMobileSmsMessaging& aSmsMessaging)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREADPARAMS_NEWL_1, "CSmsReadParams::NewL()");

	CSmsReadParams* smsReadParams=new(ELeave) CSmsReadParams(aSmsComm,aSmsSettings,aSmsMessaging);
	CleanupStack::PushL(smsReadParams);
	smsReadParams->ConstructL();
	CleanupStack::Pop(smsReadParams);
	return smsReadParams;
	} // CSmsReadParams::NewL


/**
 *  Destructor
 *  
 *  Cancel reading process and delete all created objects.
 *  
 */
CSmsReadParams::~CSmsReadParams()
	{
	Cancel();
	delete iRetrieveMobilePhoneSmspList;
	delete iMobilePhoneSmspList;
	} // CSmsReadParams::~CSmsReadParams


/**
 *  Start retrieving of SMS parameters. Called by CSmsProtocol.
 *  
 *  The behavour has been changed so that these parameters
 *  will always be retrieved from the TSY. [Previously the
 *  algorithm only retrieved the parameters from the TSY once
 *  and cached them so the TSY would not need to be polled if
 *  a client asked for the again, the assumption being that the
 *  SC number would never change. Subsequent client requests to
 *  read the SMS parameters would result in the cached parameters
 *  being returned rather than retrieving the parameters from the
 *  SIM. However this can cause problems (DEF40029) if the SIM is
 *  replaced with a new SIM that has an incorrect service center
 *  number: the stack can be requested to load the SMS parameters
 *  twice, once when the phone boots with the new SIM and again when
 *  the SIM is updated with the new Service Centre Number. If 2nd
 *  request only results in the cached parameters being returned then
 *  the incorrect SC Address will be returned.]
 *  
 *  @param aObserver a reference to the observer object.
 */
void CSmsReadParams::Start(MSmsMessageObserver& aObserver)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREADPARAMS_START_1, "CSmsReadParams::Start()");

	__ASSERT_DEBUG(iSmsMessageObserver == NULL  &&  iReport == NULL,
				   SmspPanic(KSmspPanicUnexpectedState));

	//
	// Store the observer interface object for and start the retrieve
	// operation.
	//
	iSmsMessageObserver = &aObserver;

	iRetrieveMobilePhoneSmspList->Start(iStatus);

	TimedSetActive(iSmsSettings.Timeout());
	} // CSmsReadParams::Start


/**
 *  Start retrieving of SMS parameters. Called by CSmsWriteParams.
 *  
 *  The behavour has been changed so that these parameters
 *  will always be retrieved from the TSY. [Previously the
 *  algorithm only retrieved the parameters from the TSY once
 *  and cached them so the TSY would not need to be polled if
 *  a client asked for the again, the assumption being that the
 *  SC number would never change. Subsequent client requests to
 *  read the SMS parameters would result in the cached parameters
 *  being returned rather than retrieving the parameters from the
 *  SIM. However this can cause problems (DEF40029) if the SIM is
 *  replaced with a new SIM that has an incorrect service center
 *  number: the stack can be requested to load the SMS parameters
 *  twice, once when the phone boots with the new SIM and again when
 *  the SIM is updated with the new Service Centre Number. If 2nd
 *  request only results in the cached parameters being returned then
 *  the incorrect SC Address will be returned.]
 *  
 *  @param aStatus a reference to the outstanding request.
 */
void CSmsReadParams::Start(TRequestStatus& aStatus)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREADPARAMS_START1_1, "CSmsReadParams::Start()");

	__ASSERT_DEBUG(iSmsMessageObserver == NULL  &&  iReport == NULL,
				   SmspPanic(KSmspPanicUnexpectedState));

	//
	// Store the TRequestStatus in the CSmsuActiveBase object for later
	// completion and start the retrieve operation.
	//
	Queue(aStatus);

	iRetrieveMobilePhoneSmspList->Start(iStatus);

	TimedSetActive(iSmsSettings.Timeout());
	} // CSmsReadParams::Start


/**
 *  Retrieves parameter list from CRetrieveMobilePhoneSmspList object.
 *  Cancel timeout timer. Delete old SMSP list and retrieve new list from
 *  CMobilePhoneSmspList object if retrieving was successful.
 *  
 *  @leave Leave if RetrieveListL() leaves.
 */
void CSmsReadParams::DoRunL()
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREADPARAMS_DORUNL_1, "CSmsReadParams::RunL [iStatus=%d]", iStatus.Int() );

	//
	// If the start retrieve was successful, then continue retrieving the list.
	// A temporary variable is used to keep the old list until the new one is
	// obtained successfully.
	//
	if (iStatus == KErrNone)
		{
		CMobilePhoneSmspList* tempList = iRetrieveMobilePhoneSmspList->RetrieveListL();

		delete iMobilePhoneSmspList;
		iMobilePhoneSmspList = tempList;
		}

	//
	// DoRunL() will now return to CSmsuActiveBase which if the object
	// is not active, will call Complete().
	//
	} // CSmsReadParams::DoRunL


/**
 *  Handles cancel read parameters request.
 *  
 *  Cancel timeout timer.
 *  Cancel also retrieving of parameters process.
 *  Call complete.
 */
void CSmsReadParams::DoCancel()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREADPARAMS_DOCANCEL_1, "CSmsReadParams::DoCancel()");

	TimedSetActiveCancel();

	//
	// Cancel the request...
	//
	iRetrieveMobilePhoneSmspList->Cancel();

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
	} // CSmsReadParams::DoCancel


/**
 *  Constructor
 *  
 *  @param aSmsComm a reference to the protocol.
 *  @param aSmsSettings a reference to the global SMS settings.
 *  @param aSmsMessaging a reference to the ETEL SMS messaging subsession.
 *  
 */
CSmsReadParams::CSmsReadParams(MSmsComm& aSmsComm,const TSmsSettings& aSmsSettings,RMobileSmsMessaging& aSmsMessaging):
	CSmsParamsBase(aSmsComm,aSmsSettings,aSmsMessaging)
	{
	} // CSmsReadParams::CSmsReadParams


/**
 *  2nd Phase of construction.
 *  
 *  Creates CRetrieveMobilePhoneSmspList object.
 *  
 */
void CSmsReadParams::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREADPARAMS_CONSTRUCTL_1, "CSmsReadParams::ConstructL()");

	CSmsParamsBase::ConstructL();

	iRetrieveMobilePhoneSmspList = CRetrieveMobilePhoneSmspList::NewL(iSmsMessaging);
	} // CSmsReadParams::ConstructL


/**
 *  Check validity of SMSP list. List shouldn't contain more entries that can be stored
 *  to phone-side storage.
 *  Set maximum number of enties value to the client's list.
 *  
 *  @param aMobilePhoneSmspList a reference to the CMobilePhoneSmspList object.
 *  @leave Leave if aMobilePhoneSmspList object contains too many SMSP sets.
 *  
 */
void CSmsReadParams::CheckListValidityL(CMobilePhoneSmspList& aMobilePhoneSmspList)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREADPARAMS_CHECKLISTVALIDITYL_1, "CSmsReadParams::CheckListValidityL()");

	//
	// MaxNumberEntries method returns the maximum number of entries that can
	// be stored in this list. This attribute is required because most phone-side
	// storage will have an upper storage limit. If the TSY has not set this
	// attribute during a list retrieval then the value returned will be -1.
	//
	if (iMobilePhoneSmspList->MaxNumberEntries() != -1)
		{
		if (aMobilePhoneSmspList.Enumerate() > iMobilePhoneSmspList->MaxNumberEntries())
			{
			User::Leave(KErrArgument); 
			}

		aMobilePhoneSmspList.SetMaxNumberEntries(iMobilePhoneSmspList->MaxNumberEntries());
		}
	} // CSmsReadParams::CheckListValidityL


/**
 *  Store successfully saved parameters also to SMSPROT's internal memory. Actually
 *  StoreParameters takes ownership of aMobilePhoneSmspList.
 *  
 *  Delete old list and set iMobilePhoneSmspList to point to the new SMSP list.
 *  
 *  @param aMobilePhoneSmspList a reference to the CMobilePhoneSmspList object.
 *  
 */
void CSmsReadParams::StoreParameters(CMobilePhoneSmspList* aMobilePhoneSmspList)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREADPARAMS_STOREPARAMETERS_1, "CSmsReadParams::StoreParameters()");

	__ASSERT_DEBUG(iMobilePhoneSmspList != NULL && aMobilePhoneSmspList != NULL && iMobilePhoneSmspList->MaxNumberEntries() == aMobilePhoneSmspList->MaxNumberEntries(),SmspPanic(KSmspPanicCorruptedSmspList));

	delete iMobilePhoneSmspList;
	iMobilePhoneSmspList = aMobilePhoneSmspList;
	} // CSmsReadParams::StoreParameters


/**
 *  Completes read request or cancel read request.
 *  
 *  Cancel timeout timer.
 *  Complete request to the CSmsWriteParams object or observer.
 *  
 *  @param aStatus a status value.
 *  
 */
void CSmsReadParams::Complete(TInt aStatus)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSREADPARAMS_COMPLETE_1, "CSmsReadParams::Complete()");

	//
	// Call the base function to perform the actual complete...
	//
	CSmsuActiveBase::Complete(aStatus);

	if (iSmsMessageObserver != NULL  &&
		iSmsComm.ObserverIsPresent(*iSmsMessageObserver))
		{
		iSmsMessageObserver->ReadSmsParamsCompleted(aStatus, iMobilePhoneSmspList);
		iSmsMessageObserver = NULL;
		}
	} // CSmsReadParams::Complete


/**
 *  2 Phase constructor.
 *  
 *  @param aSmsComm a reference to the protocol.
 *  @param aSmsSettings a reference to the global SMS settings.
 *  @param aSmsMessaging a reference to the ETEL SMS messaging subsession.
 *  @param aSmsReadParams a reference to the CSmsReadParams object.
 *  @leave Leaves if ContructL() leaves, or not enough memory is available.
 *  @return a new CSmsReadParams object.
 *  
 */
CSmsWriteParams* CSmsWriteParams::NewL(MSmsComm& aSmsComm,const TSmsSettings& aSmsSettings,RMobileSmsMessaging& aSmsMessaging,CSmsReadParams& aSmsReadParams)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSWRITEPARAMS_NEWL_1, "CSmsWriteParams::NewL()");

	CSmsWriteParams* smsWriteParams=new(ELeave) CSmsWriteParams(aSmsComm,aSmsSettings,aSmsMessaging,aSmsReadParams);
	CleanupStack::PushL(smsWriteParams);
	smsWriteParams->ConstructL();
	CleanupStack::Pop(smsWriteParams);
	return smsWriteParams;
	} // CSmsWriteParams::NewL


/**
 *  Destructor
 *  
 *  Cancel writing process and delete all created objects.
 *  
 */
CSmsWriteParams::~CSmsWriteParams()
	{
	Cancel();
	delete iMobilePhoneSmspList;
	} // CSmsWriteParams::~CSmsWriteParams


/**
 *  Start writing of SMS parameters process.
 *  Set state to ESmsParamsStoringList.
 *  Read parameters to SMSPROT's internal memory if client tries to write
 *  parameters before SMSPROT has read them. This is because SMSPROT has to
 *  deliver whole SMSP list while making StoreSmspList request to the MM ETel.
 *  If client deliver only some SMSP sets while making write request, we have
 *  to add missing SMSP sets to the clients list.
 *  Set timeout timer on.
 *  
 *  @param aObserver a reference to the observer object.
 *  @param aMobilePhoneSmspList a pointer to SMSP list to be saved.
 *  
 */
void CSmsWriteParams::Start(MSmsMessageObserver& aObserver,CMobilePhoneSmspList* aMobilePhoneSmspList)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSWRITEPARAMS_START_1, "CSmsWriteParams::Start()");

	__ASSERT_DEBUG(iState==ESmsParamsIdle,SmspPanic(KSmspPanicUnexpectedState));
	
	iState = ESmsParamsStoringList;

	iSmsMessageObserver= &aObserver;
	iMobilePhoneSmspList=aMobilePhoneSmspList;

	//
	//Read parameters to SMSPROT's internal memory before starting writing process...
	//
	iSmsReadParams.Start(iStatus);

	TimedSetActive(iSmsSettings.Timeout());
	} // CSmsWriteParams::Start


/**
 *  Store SMSP list to phone. If storing was successful, store list also
 *  to CSmsReadParams object.
 *  
 *  Cancel timeout timer.
 *  State ESmsParamsStoringList:
 *  Change state to ESmsParamsStoringListToInternal.
 *  Check validity of SMSP list using CSmsReadParams objects services.
 *  After that makes StoreSmspList request to the MM ETel.
 *  Set timeout timer on.
 *  State ESmsParamsStoringListToInternal:
 *  Store successfully stored parameters to CSmsReadParams object.
 *  
 *  @leave Leaves if there was an error.
 *  @leave Leave if RetrieveListL() leaves.
 *  @leave Leave if StoreSmspListL() leaves.
 *  @leave Panic if RunL is called while object is in idle state.
 *  
 */
void CSmsWriteParams::DoRunL()
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSWRITEPARAMS_DORUNL_1, "CSmsWriteParams::DoRunL [iStatus=%d]", iStatus.Int() );

	switch (iState)
		{
		case ESmsParamsStoringList:
			{
			//
			// Request the list to be stored...
			//
			if (iStatus.Int() == KErrNone)
				{
				iState = ESmsParamsStoringListToInternal;

				iSmsReadParams.CheckListValidityL(*iMobilePhoneSmspList);
				iSmsMessaging.StoreSmspListL(iStatus, iMobilePhoneSmspList);

				TimedSetActive(iSmsSettings.Timeout());
				}
			}
			break;

		case ESmsParamsStoringListToInternal:
			{
			//
			// If the list was stored successfully, then store it internally.
			//
			if (iStatus.Int() == KErrNone)
				{
				iSmsReadParams.StoreParameters(iMobilePhoneSmspList);

				//
				// Set iMobilePhoneSmspList to NULL because it points now to SMSPROT's
				// internal parameter storage.
				//
				iMobilePhoneSmspList = NULL;
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
	} // CSmsWriteParams::DoRunL


/**
 *  Handles cancel write parameters request.
 *  
 *  Cancel timeout timer. Cancel reading and writing processes
 *  if they are running.
 *  
 */
void CSmsWriteParams::DoCancel()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSWRITEPARAMS_DOCANCEL_1, "CSmsWriteParams::DoCancel()");

	TimedSetActiveCancel();

	switch (iState)
		{
		case ESmsParamsStoringList:
			{
			iSmsReadParams.Cancel();
			}
			break;

		case ESmsParamsStoringListToInternal:
			{
			iSmsMessaging.CancelAsyncRequest(EMobileSmsMessagingStoreSmspList);
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
	} // CSmsWriteParams::DoCancel


/**
 *  Constructor
 *  
 *  @param aSmsComm a reference to the protocol.
 *  @param aSmsSettings a reference to the global SMS settings.
 *  @param aSmsMessaging a reference to the ETEL SMS messaging subsession.
 *  @param aSmsReadParams a reference to the CSmsReadParams object.
 *  
 */
CSmsWriteParams::CSmsWriteParams(MSmsComm& aSmsComm,const TSmsSettings& aSmsSettings,RMobileSmsMessaging& aSmsMessaging,CSmsReadParams& aSmsReadParams):
	CSmsParamsBase(aSmsComm,aSmsSettings,aSmsMessaging),
	iState(ESmsParamsIdle),
	iSmsReadParams(aSmsReadParams)
	{
	} // CSmsWriteParams::CSmsWriteParams


/**
 *  2nd Phase of construction.
 *  
 *  Creates CSmsuTimeout object.
 *  
 */
void CSmsWriteParams::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSWRITEPARAMS_CONSTRUCTL_1, "CSmsWriteParams::ConstructL()");

	CSmsParamsBase::ConstructL();
	} // CSmsWriteParams::ConstructL


/**
 *  Completes write request or cancel write request.
 *  
 *  Cancel timeout timer. Set state to ESmsParamsIdle. Complete request to the
 *  observer.
 *  
 *  @param aStatus a status value.
 *  @leave Panic if Complete is called while object is in ESmsParamsIdle state.
 *  
 */
void CSmsWriteParams::Complete(TInt aStatus)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSWRITEPARAMS_COMPLETE_1, "CSmsWriteParams::Complete()");

	//
	// Call the base function to perform the actual complete...
	//
	CSmsuActiveBase::Complete(aStatus);

	delete iMobilePhoneSmspList;
	iMobilePhoneSmspList = NULL;

	iState = ESmsParamsIdle;

	if (iSmsComm.ObserverIsPresent(*iSmsMessageObserver))
		{
		iSmsMessageObserver->WriteSmsParamsCompleted(aStatus);
		iSmsMessageObserver = NULL;
		}
	} // CSmsWriteParams::Complete
