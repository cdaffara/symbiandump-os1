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
#include "smspenumTraces.h"
#endif

#include "smspenum.h"
#include "smspproc.h"
#include "smsuset.h"
#include "smspmain.h"
#include "smspstor.h"
#include "Gsmumsg.h"
#include "SmsuTimer.h"
#include "exterror.h"

// constant used to handle inconsistency between slots used and total slots
const TInt KSmsDefaultTotalMessageStoreSlots = 50;


/**
 *  Creates new CSmsStoreRead instance
 *  
 *  @param aSmsComm Used to set CSmsPDUProcess object
 *  @param aSmsSettings Used to start CSmsTimeout object
 *  @param aSmsMessaging Used to get message store info and open store
 *  @param aReassemblyStore Used to set CSmsPDUProcess object
 *  @param aSegmentationStore Used to set CSmsPDUProcess object
 *  @param aPriority Used to set CSmsPDUProcess object
 *  @param aSmsMonitorDiskSpace Used to set CSmsPDUProcess object
 *  
 *  @leave Leaves if ContructL() leaves, or not enough memory is available.
 *  @return a new CSmsStoreRead object.
 */
CSmsStoreRead* CSmsStoreRead::NewL(MSmsComm& aSmsComm,
								   const TSmsSettings& aSmsSettings,
								   RMobileSmsMessaging& aSmsMessaging,
								   CFacadeSmsReassemblyStore& aReassemblyStore,
								   CSmsSegmentationStore& aSegmentationStore,
								   TInt aPriority,
								   CSmsMonitorDiskSpace& aSmsMonitorDiskSpace)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSTOREREAD_NEWL_1, "CSmsStoreRead::NewL()");

	CSmsStoreRead* smsStoreRead = new ( ELeave ) CSmsStoreRead(aSmsComm,
															   aSmsSettings,
															   aSmsMessaging,
															   aReassemblyStore,
															   aSegmentationStore,
															   aPriority,
															   aSmsMonitorDiskSpace);
	CleanupStack::PushL(smsStoreRead);
	smsStoreRead->ConstructL();
	CleanupStack::Pop(smsStoreRead);
	return smsStoreRead;
	} // CSmsStoreRead::NewL


/**
 *  Constructor
 *  
 *  @param aSmsComm Used to set CSmsPDUProcess object
 *  @param aSmsSettings Used to start CSmsTimeout object
 *  @param aSmsMessaging Used to get message store info and open store
 *  @param aReassemblyStore Used to  set CSmsPDUProcess object
 *  @param aSegmentationStore Used to  set CSmsPDUProcess object
 *  @param aPriority Used to  set CSmsPDUProcess object
 *  @param aSmsMonitorDiskSpace Used to set CSmsPDUProcess object
 */
CSmsStoreRead::CSmsStoreRead(MSmsComm& aSmsComm,
							 const TSmsSettings& aSmsSettings,
							 RMobileSmsMessaging& aSmsMessaging,
							 CFacadeSmsReassemblyStore& aReassemblyStore,
							 CSmsSegmentationStore& aSegmentationStore,
							 TInt aPriority,
							 CSmsMonitorDiskSpace& aSmsMonitorDiskSpace)
  : CSmsuActiveBase(aPriority),
	iState(ESmsStoreReadIdle),
	iSmsComm(aSmsComm),
	iSmsSettings(aSmsSettings),
	iSmsMessaging(aSmsMessaging),
	iReassemblyStore(aReassemblyStore),
	iSegmentationStore(aSegmentationStore),
	iSmsMonitorDiskSpace(aSmsMonitorDiskSpace),
	iStoreInfoPckg(iStoreInfo),
	iEntryV1Pckg(iSmsEntry)
	{
	} // CSmsStoreRead::CSmsStoreRead


/**
 *  Calls ConstructTimeoutL from the class CSmsuActiveBase
 */
void CSmsStoreRead::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSTOREREAD_CONSTRUCTL_1, "CSmsStoreRead::ConstructL()");

	ConstructTimeoutL();
	ReadConfigurableSmsSettingsL();
	} // CSmsStoreRead::ConstructL


/**
 *  
 *  Synopsis:
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
 *  more than one Release 6 configuration option is specified. For example:
 *  [Release6Configurability]
 *  AutomaticDeletionOfClass2=  1
 *  DiscardType0_Class2Message= 1
 *  DiscardType0_Class0Message= 1
 *  
 */
void CSmsStoreRead::ReadConfigurableSmsSettingsL()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSTOREREAD_READCONFIGURABLESMSSETTINGSL_1, "CSmsStoreRead::ReadConfigurableSmsSettingsL()");

    CESockIniData*  ini = NULL;
    TRAPD(ret, ini=CESockIniData::NewL(_L("smswap.sms.esk")));
    if(ret!=KErrNone)
        {
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSTOREREAD_READCONFIGURABLESMSSETTINGSL_2, "CSmsStoreRead::ReadConfigurableSmsSettingsL(): ret=%d", ret);
        User::Leave(ret);
        }

    CleanupStack::PushL(ini);
    
    //
    // Save Release 6 options...
    //
    TBool status(EFalse);

    if (ini->FindVar(_L("Release6Configurability"),
    				 _L("AutomaticDeletionOfClass2"), status))
    	{
        iConfigAutomaticDeletionForClass2 = status;
        }

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSTOREREAD_READCONFIGURABLESMSSETTINGSL_3, "CSmsStoreRead::ReadConfigurableSmsSettingsL(): iConfigAutomaticDeletionForClass2=%d",iConfigAutomaticDeletionForClass2);

    if (ini->FindVar(_L("Release6Configurability"),
    				 _L("DiscardType0_Class2Message"), status))
    	{
        iConfigDiscardingType0Class2 = status;
        }

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSTOREREAD_READCONFIGURABLESMSSETTINGSL_4, "CSmsStoreRead::ReadConfigurableSmsSettingsL(): iConfigDiscardingType0Class2=%d",iConfigDiscardingType0Class2);

    if (ini->FindVar(_L("Release6Configurability"),
    				 _L("DiscardType0_Class0Message"), status))
    	{
        iConfigDiscardingType0Class0 = status;
        }

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSTOREREAD_READCONFIGURABLESMSSETTINGSL_5, "CSmsStoreRead::ReadConfigurableSmsSettingsL(): iConfigDiscardingType0Class0=%d",iConfigDiscardingType0Class0);

    CleanupStack::PopAndDestroy(ini);
    } // CSmsStoreRead::ReadConfigurableSmsSettingsL


/**
 *  Starts CSmsStoreRead active object
 *  Calls GetMessageStoreInfo method from the MM API
 *  
 *  @param aStatus Reguest status
 *  @param aStoreIndex Index to the store to be read
 */
void CSmsStoreRead::Start( TRequestStatus& aStatus, TInt aStoreIndex )
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSTOREREAD_START_1, "CSmsStoreRead::Start()");

	iState = ESmsStoreReadGettingStoreInfo;
	// Initialises current slot index
	iCurrentSlotIndex = 0;

	// Gets current store info
	iSmsMessaging.GetMessageStoreInfo( iStatus, aStoreIndex, iStoreInfoPckg );

	Queue(aStatus);
	TimedSetActive(iSmsSettings.Timeout());
	} // CSmsStoreRead::Start


/**
 *  Called by CSmsuActive object to run this object
 *  Cancels timeout timer
 *  State ESmsStoreReadGettingStoreInfo:
 *  Change state to ESmsStoreReadReadingList or ESmsStoreReadReadingSlot
 *  Sets value of the total entries if inconsistency between slots used and total slots
 *  If store can be read opens store, otherwise leaves with KErrNotSupported
 *  If store can be read att once, starts two-phased reading of the message list,
 *  Otherwise start reading store slot by slot
 *  State ESmsStoreReadReadingList:
 *  Gets readed store list
 *  Starts process Pdus
 *  State ESmsStoreReadReadingSlot:
 *  Adds readed store entry to the entry list
 *  Starts reading of the next slot
 *  Starts process Pdus when all entries are read and
 *  State ESmsStoreReadIdle is unexpected state
 *  
 *  @leave Leaves if there was an error
 *  @leave Leaves if open store leaves
 *  @leave Leaves with KErrNotSupported if store has no read access
 *  @leave Panic if complete is called while state is ESmsStoreReadIdle
 */
void CSmsStoreRead::DoRunL()
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSTOREREAD_DORUNL_1, "CSmsStoreRead::DoRunL [iStatus=%d]", iStatus.Int() );

	TInt status = iStatus.Int();

	//status is KErrNotFound or KErrGsmSMSInvalidMemoryIndex if last read entry was empty
	//status is KErrCorrupt if the TSY declares a PDU to be corrupt
 	if (status != KErrNone  &&  status != KErrNotFound  &&
 	    status != KErrCorrupt  &&  status != KErrGsmSMSInvalidMemoryIndex)
 	    {
		User::Leave(status); //< Trapped in CSmsuActiveBase::RunError() which calls Complete().
		}

    switch ( iState )
		{
		case ESmsStoreReadGettingStoreInfo:
			{
			if ( ( iStoreInfo.iTotalEntries == 0 ) && ( iStoreInfo.iUsedEntries > 0 ) )
				{
				// Handle inconsistency between slots used & total slots (7110!)
				iStoreInfo.iTotalEntries = KSmsDefaultTotalMessageStoreSlots;
				}
			// Checks reading capabilities of the current store
			if ( iStoreInfo.iCaps & RMobilePhoneStore::KCapsReadAccess )
				{
				// Opens current store
				User::LeaveIfError( iSmsStorage.Open( iSmsMessaging, iStoreInfo.iName ) );
				}
			else
				{
				User::Leave( KErrNotSupported );
				}
			if ( iStoreInfo.iCaps & static_cast<TUint32>(RMobilePhoneStore::KCapsWholeStore) )
				{
				iState = ESmsStoreReadReadingList;
				delete iRetrieveMobilePhoneSmsList;
				iRetrieveMobilePhoneSmsList = NULL;
				// Create CRetrieveMobilePhoneSmsList object
				iRetrieveMobilePhoneSmsList = CRetrieveMobilePhoneSmsList::NewL(
					iSmsStorage, RMobileSmsStore::KETelMobileGsmSmsEntryV1 );
				// Starts two-phased reading of the message list
				iRetrieveMobilePhoneSmsList->Start(iStatus);
				TimedSetActive(KDefaultEnumerateTimeoutSeconds);
				}
			else if ( iStoreInfo.iCaps & RMobilePhoneStore::KCapsIndividualEntry )
				{
				iState = ESmsStoreReadReadingSlot;
				// Reads next single message from the store
				delete iMobilePhoneGsmSmsList;
				iMobilePhoneGsmSmsList = NULL;
				iMobilePhoneGsmSmsList = CMobilePhoneGsmSmsList::NewL();
				ReadNextSlotL();
				}
			else
				{
				User::Leave( KErrNotSupported );
				}
			}
			break;

		case ESmsStoreReadReadingSlot:
			{
			if ( iStatus == KErrNone )
				{
				// Add current message to the list
				iMobilePhoneGsmSmsList->AddEntryL( iSmsEntry );
#if (OST_TRACE_CATEGORY & OST_TRACE_CATEGORY_DEBUG)
                LogSmsIfSmsEntry(_L8("SMSENTRY: "), iSmsEntry);
#endif
				iStoreInfo.iUsedEntries--;
				}
			// Reads next single message from the store
			ReadNextSlotL();
			}
			break;

		case ESmsStoreReadReadingList:
			{
			// Gets whole message list
			if (iStatus != KErrNotFound  &&
			    iStatus != KErrGsmSMSInvalidMemoryIndex  &&
			    iStatus != KErrCorrupt)	//ReadList has completed if iStatus is KErrNotFound => empty list
				{
				CMobilePhoneGsmSmsList* tempList = iRetrieveMobilePhoneSmsList->RetrieveGsmListL();
				delete iMobilePhoneGsmSmsList;
				iMobilePhoneGsmSmsList = tempList;
				ProcessPduL();
				}
			else
				{
				iStatus = KErrNone;	//iStatus is used as completion object for this AO call, so set to status to KErrNone as we do not want to propogate KErrNotFound back to phone enumeration AO as it is not actually an error.
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
	} // CSmsStoreRead::DoRunL


/**
 *  private method for reading one slot from the store
 */
void CSmsStoreRead::ReadNextSlotL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSTOREREAD_READNEXTSLOTL_1, "CSmsStoreRead::ReadNextSlotL()");

	//Increments current slot index
	iCurrentSlotIndex++;
	// Sets message entry index
	iSmsEntry.iIndex = iCurrentSlotIndex;

	if (iStoreInfo.iUsedEntries > 0  &&
		iCurrentSlotIndex <= iStoreInfo.iTotalEntries)
		{
		// Reads message from the store
		iSmsStorage.Read( iStatus, iEntryV1Pckg );
		SetActive();
		}
	else
		{
		ProcessPduL();
		}
	} // CSmsStoreRead::ReadNextSlotL


/**
 *  private method for process one slot from the entry list
 */
void CSmsStoreRead::ProcessPduL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSTOREREAD_PROCESSPDUL_1, "CSmsStoreRead::ProcessPduL()");

	// Increments slot index
	for ( iCurrentSlotIndex = 0; iCurrentSlotIndex < iMobilePhoneGsmSmsList->Enumerate(); iCurrentSlotIndex++ )
		{
		CSmsPDUProcessor*  pduProcessor = CSmsPDUProcessor::NewL(iSmsComm, iSmsSettings, iReassemblyStore,
											 iSegmentationStore, iSmsMonitorDiskSpace);
		CleanupStack::PushL(pduProcessor);

		pduProcessor->SetApplyAutoDeletionToClass2(iConfigAutomaticDeletionForClass2);
		pduProcessor->SetDiscardType0Class0(iConfigDiscardingType0Class0);
		pduProcessor->SetDiscardType0Class2(iConfigDiscardingType0Class2);

		// Gets message from the list
		iSlot = iMobilePhoneGsmSmsList->GetEntryL( iCurrentSlotIndex );

		// Sets store name to the slot
		iSlot.iStore = iStoreInfo.iName;

		//User::LeaveIfError( Decode( iSlot ) );
        // ALR-56UJLB - Enumerating messages on SIM may fail
        // if there is a corrupt message on SIM
        // the enumeration of a submit pdu with a 9210
        // return KErrcorrupt, because the submit pdu
        // is stored as a deliver pdu in the SIM store
        // that is a defect in the modem software of the 9210
        // but in this case none of the messages have been passed
        // to the client, this has been fixed here
		TRAPD(ret, pduProcessor->DecodeAndProcessPDUL(iSlot, ETrue));
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSTOREREAD_PROCESSPDUL_2, "CSmsStoreRead::ProcessPduL(): DecodeAndProcessPDUL() returned %d", ret);

		if (ret == KErrNone)
			{
			if (pduProcessor->IsForwardMessageToClient())
				{
				//
				// Note, at this point we have not placed the message in
				// the reassembly store.
				//
				pduProcessor->ProcessMessageIfCompleteL();
				}
			}
		 else if (ret == KErrCorrupt)
			{
      		pduProcessor->DeletePDUL();
			}
		else if (ret ==  KErrDiskFull  ||  ret ==  KErrNoMemory)
			{
		 	//
		 	// Permanant error, can't continue message enumeration,
		 	// so just leave...
		 	//
		 	User::Leave(ret);
			}
			
		CleanupStack::PopAndDestroy(pduProcessor);
		}
	} // CSmsStoreRead::ProcessPduL


/**
 *  Called by CSmsuActive object to cancel this object
 *  Cancels timeout timer
 *  State ESmsStoreReadGettingStoreInfo:
 *  Cancels reading of the message store info
 *  State ESmsStoreReadReadingList:
 *  Cancels reading of the message store list
 *  State ESmsStoreReadReadingSlot:
 *  Cancels reading of the single slot
 *  State ESmsStoreReadIdle is unexpected state
 *  
 *  @leave Panic if complete is called while state is ESmsStoreReadIdle
 */
void CSmsStoreRead::DoCancel()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSTOREREAD_DOCANCEL_1, "CSmsStoreRead::DoCancel()");

	TimedSetActiveCancel();

	switch (iState)
		{
		case ESmsStoreReadGettingStoreInfo:
			{
			// Cancels message store info reading
			iSmsMessaging.CancelAsyncRequest(EMobileSmsMessagingGetMessageStoreInfo);
			}
			break;

		case ESmsStoreReadReadingList:
			{
			// Cancels message list reading
			iRetrieveMobilePhoneSmsList->Cancel();
			}
			break;

		case ESmsStoreReadReadingSlot:
			{
			// Cancels slot reading
 			iSmsStorage.CancelAsyncRequest( EMobilePhoneStoreRead );
 			iState = ESmsStoreReadReadingSlotCancel;
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
	} // CSmsStoreRead::DoCancel


/**
 *  Completes store read request
 *  
 *  Closes phone side store if necessary.
 *  
 *  @param aStatus a status value
 *  @leave Panic if complete is called while state is ESmsStoreReadIdle
 */
void CSmsStoreRead::Complete(TInt aStatus)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSSTOREREAD_COMPLETE_1, "CSmsStoreRead::Complete()");

	__ASSERT_DEBUG(iState != ESmsStoreReadIdle, SmspPanic(KSmspPanicUnexpectedState));

	//
	// Close the store if needed...
	//
	if (iState != ESmsStoreReadReadingSlotCancel)
		{
		CloseStore();
		}

	//
	// Call the base function to perform the actual complete...
	//
	CSmsuActiveBase::Complete(aStatus);

	iState = ESmsStoreReadIdle;
	} // CSmsStoreRead::Complete


/**
 *  Closes the store
 */
void CSmsStoreRead::CloseStore()
 	{
 	iSmsStorage.Close();
 	} // CSmsStoreRead::CloseStore


/**
 *  Destructor
 */
CSmsStoreRead::~CSmsStoreRead()
	{
	Cancel();

	// Deletes created instances
	delete iRetrieveMobilePhoneSmsList;
	delete iMobilePhoneGsmSmsList;
	} // CSmsStoreRead::~CSmsStoreRead


/**
 *  Creates new CSmsPhoneEnumeration instance
 *  
 *  @param aSmsComm Used to complete enumeration
 *  @param aSmsSettings Used to create CSmsStoreRead object and start timer
 *  @param aReassemblyStore Used to create CSmsStoreRead object
 *  @param aSegmentationStore Used to create CSmsStoreRead object
 *  @param aGsmPhone Used to Initialise phone and open RMobileSmsMessaging for enumerate message
 *  @param aPriority Used to set CSmsuActiveBase object
 */
CSmsPhoneEnumeration* CSmsPhoneEnumeration::NewL( MSmsComm& aSmsComm,
												  const TSmsSettings& aSmsSettings,
												  CFacadeSmsReassemblyStore& aReassemblyStore,
												  CSmsSegmentationStore& aSegmentationStore,
												  RMobilePhone& aGsmPhone,
												  TInt aPriority,
												  CSmsMonitorDiskSpace& aSmsMonitorDiskSpace )
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPHONEENUMERATION_NEWL_1, "CSmsPhoneEnumeration::NewL()");

	CSmsPhoneEnumeration* smsPhoneEnumeration =
		new ( ELeave ) CSmsPhoneEnumeration( aSmsComm,
											 aSmsSettings,
											 aGsmPhone,
											 aPriority );
	CleanupStack::PushL( smsPhoneEnumeration );
	smsPhoneEnumeration->ConstructL( aReassemblyStore, aSegmentationStore, aSmsMonitorDiskSpace );
	CleanupStack::Pop( smsPhoneEnumeration );
	return smsPhoneEnumeration;
	} // CSmsPhoneEnumeration::NewL


/**
 *  Constructor
 *  
 *  @param aSmsComm Used to complete enumeration
 *  @param aSmsSettings Used to create CSmsStoreRead object and start timer
 *  @param aReassemblyStore Used to create CSmsStoreRead object
 *  @param aSegmentationStore Used to create CSmsStoreRead object
 *  @param aGsmPhone Used to Initialise phone and open RMobileSmsMessaging for enumerate message
 *  @param aPriority Used to set CSmsuActiveBase object
 */
CSmsPhoneEnumeration::CSmsPhoneEnumeration( MSmsComm& aSmsComm,
											const TSmsSettings& aSmsSettings,
											RMobilePhone& aGsmPhone,
											TInt aPriority )
	:CSmsuActiveBase( aPriority )
	,iState( ESmsPhoneEnumerationIdle )
	,iSmsComm( aSmsComm )
	,iSmsSettings( aSmsSettings )
	,iGsmPhone( aGsmPhone )
	{
	} // CSmsPhoneEnumeration::CSmsPhoneEnumeration


/**
 *  Destructor
 */
CSmsPhoneEnumeration::~CSmsPhoneEnumeration()
	{
	Cancel();
	//deletes created instances
	delete iSmsStoreRead;
	} // CSmsPhoneEnumeration::~CSmsPhoneEnumeration


/**
 *  Starts CSmsPhoneEnumeration active object
 *  Calls Initialise method from RMobilePhone object
 *  Sets timed active
 */
void CSmsPhoneEnumeration::Start()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPHONEENUMERATION_START_1, "CSmsPhoneEnumeration::Start()");

	iState = ESmsPhoneEnumerationInitializing;
	iCurrentStore = 0;
	iGsmPhone.Initialise( iStatus );
	TimedSetActive( iSmsSettings.Timeout() );
	} // CSmsPhoneEnumeration::Start


/**
 *  Called by CSmsuActive object to run this object
 *  Cancels timeout timer
 *  State ESmsPhoneEnumerationInitializing:
 *  Change State to ESmsPhoneEnumerationReadingStores
 *  Calls Open method from class RMobileSmsMessaging
 *  If opening returns KErrNone
 *  Gets number of enumerate message stores
 *  Marks initialization begin in phone image
 *  Starts CSmsStoreRead active object to read first store
 *  State ESmsPhoneEnumerationReadingStores:
 *  Reads all existing stores
 *  State Completes when all stores are readed
 *  State ESmsPhoneEnumerationIdle is unexpected state
 *  Completes if object is not active
 *  
 *  @leave Leaves if there was an error
 *  @leave Panic if called while state is ESmsPhoneEnumerationIdle
 */
void CSmsPhoneEnumeration::DoRunL()
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPHONEENUMERATION_DORUNL_1, "CSmsPhoneEnumeration::RunL [iStatus=%d], [iState=%d]", iStatus.Int(), iState );

    switch (iState)
		{
		case ESmsPhoneEnumerationInitializing:
			{
		    if (iStatus.Int() == KErrNone)
    			{
				iState = ESmsPhoneEnumerationReadingStores;
				User::LeaveIfError(iSmsMessaging.Open(iGsmPhone));
				User::LeaveIfError(iSmsMessaging.EnumerateMessageStores(iNumStores));

				iSmsStoreRead->Start(iStatus, iCurrentStore);
				SetActive();
				}
			}
			break;

		case ESmsPhoneEnumerationReadingStores:
			{
		    if (iStatus.Int() == KErrNone)
    			{
				iCurrentStore++;
				if (iCurrentStore < iNumStores)
					{
					iSmsStoreRead->Start( iStatus, iCurrentStore );
					SetActive();
					}
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
	} // CSmsPhoneEnumeration::DoRunL


void CSmsPhoneEnumeration::DoCancel()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPHONEENUMERATION_DOCANCEL_1, "CSmsPhoneEnumeration::DoCancel()");

	TimedSetActiveCancel();

	switch (iState)
		{
		case ESmsPhoneEnumerationInitializing:
			{
			iGsmPhone.InitialiseCancel();
			}
			break;

		case ESmsPhoneEnumerationReadingStores:
			{
			iSmsStoreRead->Cancel();
			iSmsStoreRead->CloseStore();
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
	} // CSmsPhoneEnumeration::DoCancel


/**
 *  Creates instances to the classes CSmsTimeout and CSmsStoreRead
 */
void CSmsPhoneEnumeration::ConstructL(CFacadeSmsReassemblyStore& aReassemblyStore,CSmsSegmentationStore& aSegmentationStore,CSmsMonitorDiskSpace& aSmsMonitorDiskSpace)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPHONEENUMERATION_CONSTRUCTL_1, "CSmsPhoneEnumeration::ConstructL()");

	iSmsStoreRead = CSmsStoreRead::NewL(iSmsComm, iSmsSettings, iSmsMessaging, aReassemblyStore, aSegmentationStore, Priority(), aSmsMonitorDiskSpace);
	ConstructTimeoutL();
	} // CSmsPhoneEnumeration::ConstructL


void CSmsPhoneEnumeration::Complete(TInt aStatus)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPHONEENUMERATION_COMPLETE_1, "CSmsPhoneEnumeration::Complete [aStatus=%d]", aStatus);

	//
	// Call the base function to perform the actual complete...
	//
	CSmsuActiveBase::Complete(aStatus);

	//
	// Finish up...
	//
	iState = ESmsPhoneEnumerationIdle;
	iSmsMessaging.Close();
	
	//
	// Send an event to say that the enumeration is complete...
	//
	iSmsComm.PhoneEnumerationCompleted(aStatus);
	} // CSmsPhoneEnumeration::Complete


/**
 *  Creates new CSmsMessageWrite instance
 *  
 *  @param aSmsSettings Used to start CSmsTimeout object
 *  @param aGsmPhone Used to Initialise phone
 *  @param aSegmentationStore Used to get next 8 or 16bit reference
 */
CSmsMessageWrite* CSmsMessageWrite::NewL( MSmsComm& aSmsComm, const TSmsSettings& aSmsSettings,
										  RMobilePhone& aGsmPhone,
										  CSmsSegmentationStore& aSegmentationStore
										  )
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGEWRITE_NEWL_1, "CSmsMessageWrite::NewL()");

	CSmsMessageWrite* smsMessageWrite = new (ELeave) CSmsMessageWrite(aSmsComm, aSmsSettings,
																	  aGsmPhone,
																	  aSegmentationStore);
	CleanupStack::PushL(smsMessageWrite);
	smsMessageWrite->ConstructL();
	CleanupStack::Pop(smsMessageWrite);
	return smsMessageWrite;
	} // CSmsMessageWrite::NewL


/**
 *  Destructor
 */
CSmsMessageWrite::~CSmsMessageWrite()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGEWRITE_DTOR_1, "CSmsMessageWrite::~CSmsMessageWrite()");

	Cancel();
	iSmsStorage.Close();
	iSmsMessaging.Close();
	} // CSmsMessageWrite::~CSmsMessageWrite


void CSmsMessageWrite::Start( CSmsMessage* aSmsMessage, TRequestStatus& aStatus )
	{
	Queue(aStatus);

	iSmsStorage.Close();
	iSmsMessaging.Close();

	iState = ESmsMessageWriteInitializing;


	iSmsMessage = aSmsMessage;
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGEWRITE_START_1, "CSmsMessageWrite::Start  Storage=[%d]   Type=[%d]", iSmsMessage->Storage(), iSmsMessage->Type());
	iCurrentStore = 0;
	iStoreInfo.iName = KNullDesC;

	switch (iSmsMessage->Storage())
		{
		case CSmsMessage::ESmsSIMStorage:
			{
 			iSlot.iStore = KETelIccSmsStore;
			}
			break;

		case CSmsMessage::ESmsPhoneStorage:
			{
 			iSlot.iStore = KETelMeSmsStore;
			}
			break;

		case CSmsMessage::ESmsCombinedStorage:
			{
 			iSlot.iStore = KETelCombinedSmsStore;
			}
			break;

		default:
			{
			Complete(KErrArgument);
			return;
			}
		}

	switch (iSmsMessage->Type())
		{
		case CSmsPDU::ESmsSubmit:
		case CSmsPDU::ESmsCommand:
			{
			iSlot.iMsgStatus = RMobileSmsStore::EStoredMessageUnsent;
			}
			break;

		case CSmsPDU::ESmsDeliver:
		case CSmsPDU::ESmsStatusReport:
			{
			// Check read status in iSmsMessage
			if (iSmsMessage->Status() == NMobileSmsStore::EStoredMessageRead)
		 		{
	 			iSlot.iMsgStatus = RMobileSmsStore::EStoredMessageRead;
	 			}
	 		else
	 			{
				iSlot.iMsgStatus = RMobileSmsStore::EStoredMessageUnread;
	 			}
			break;
			}
		default:
			{
			SmspPanic(ESmspUnexpectedSmsPDUType);
			}
			break;
		}

	//
	// Begin initialising...
	//
	iGsmPhone.Initialise(iStatus);
	TimedSetActive(iSmsSettings.Timeout());
	} // CSmsMessageWrite::Start


void CSmsMessageWrite::DoRunL()
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGEWRITE_DORUNL_1, "CSmsMessageWrite::DoRunL [iStatus=%d iState=%d]", iStatus.Int() , iState);

	switch (iState)
		{
		case ESmsMessageWriteInitializing:
			{
			if (iStatus.Int() == KErrNone)
				{
				iState = ESmsMessageWriteGettingStoreInfo;
				iSmsMessaging.Close();
				User::LeaveIfError(iSmsMessaging.Open(iGsmPhone));
				User::LeaveIfError(iSmsMessaging.EnumerateMessageStores(iNumStores));
	 			iSmsMessaging.GetMessageStoreInfo(iStatus, iCurrentStore, iStoreInfoPckg);

				TimedSetActive(iSmsSettings.Timeout());
				}
			}
			break;

		case ESmsMessageWriteGettingStoreInfo:
			{
			if (iStatus.Int() == KErrNone)
				{
				if (iStoreInfo.iName == iSlot.iStore)
					{
					if ((iStoreInfo.iCaps & RMobilePhoneStore::KCapsWriteAccess) == 0)
						{
                        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGEWRITE_DORUNL_2, "CSmsMessageWrite::DoRunL left with KErrNotSuported [iState=%d]", iState);
						User::Leave(KErrNotSupported);
						}

					iState = ESmsMessageWriteWritingPDUs;
					SegmentMessageL();
					WriteNextSlot();
					}
				else
					{
					iCurrentStore++;
					if (iCurrentStore < iNumStores)
						{
	 					iSmsMessaging.GetMessageStoreInfo(iStatus, iCurrentStore, iStoreInfoPckg);
						TimedSetActive(iSmsSettings.Timeout());
						}
					else
						{
                        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGEWRITE_DORUNL_3, "CSmsMessageWrite::DoRunL left with KErrNotFound [iState=%d]", iState);
						User::Leave(KErrNotFound);
						}
					}
				}
			}
			break;

		case ESmsMessageWriteWritingPDUs:
			{
			if (iStatus.Int() == KErrNone)
				{
				iSmsArray.Delete( 0 );
				iSlot.iIndex = iEntryToWrite.iIndex;

				//
				// Initialise slot entry for transfer to array
				//
				TGsmSmsSlotEntry smsSlotEntry;

				smsSlotEntry.iIndex = iSlot.iIndex;
				smsSlotEntry.iStore = iSlot.iStore;
				iSmsMessage->AddSlotL(smsSlotEntry);
				WriteNextSlot();
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
	} // CSmsMessageWrite::DoRunL


void CSmsMessageWrite::DoCancel()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGEWRITE_DOCANCEL_1, "CSmsMessageWrite::DoCancel()");

	TimedSetActiveCancel();

	switch (iState)
		{
		case ESmsMessageWriteInitializing:
			{
			iGsmPhone.InitialiseCancel();
			}
			break;

		case ESmsMessageWriteGettingStoreInfo:
			{
			iSmsMessaging.CancelAsyncRequest(EMobileSmsMessagingGetMessageStoreInfo);
			}
			break;

		case ESmsMessageWriteWritingPDUs:
			{
			iSmsStorage.CancelAsyncRequest(EMobilePhoneStoreWrite);
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
	} // CSmsMessageWrite::DoCancel


CSmsMessageWrite::CSmsMessageWrite(MSmsComm& aSmsComm, const TSmsSettings& aSmsSettings,
								   RMobilePhone& aGsmPhone,
								   CSmsSegmentationStore& aSegmentationStore)
  : CSmsuActiveBase(KSmsSessionPriority),
	iSmsComm(aSmsComm),
	iState(ESmsMessageWriteIdle),
	iSmsSettings(aSmsSettings),
	iGsmPhone(aGsmPhone),
	iStoreInfoPckg (iStoreInfo),
	iWriteEntryV1Pckg(iEntryToWrite),
	iSegmentationStore(aSegmentationStore),
	iSmsArray(8)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGEWRITE_CTOR_1, "CSmsMessageWrite::CSmsMessageWrite()");

	} // CSmsMessageWrite::CSmsMessageWrite


void CSmsMessageWrite::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGEWRITE_CONSTRUCTL_1, "CSmsMessageWrite::ConstructL()");

	ConstructTimeoutL();
	} // CSmsMessageWrite::ConstructL


void CSmsMessageWrite::SegmentMessageL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGEWRITE_SEGMENTMESSAGEL_1, "CSmsMessageWrite::SegmentMessageL()");

	if (iStoreInfo.iTotalEntries == iStoreInfo.iUsedEntries)
		{
		User::Leave(KErrNoMemory);
		}

	TInt reference = 0;

	TBool is16bit = EFalse;
	TBool concatenationIEPresent = EFalse;

	//
	// Ensure text present before testing concatenation requirement
	//
	if (iSmsMessage->TextPresent())
		{
		concatenationIEPresent= iSmsMessage->SmsPDU().TextConcatenated( &is16bit );
		iSmsMessage->SmsPDU().SetTextConcatenatedL(EFalse,EFalse);
		}

	if (!iSmsMessage->EncodeIntoSinglePDUL(iSmsArray))
		{
		if (concatenationIEPresent && is16bit)
			{
			iSmsMessage->Set16BitConcatenation(ETrue);
			}

		reference=is16bit? iSegmentationStore.Next16BitReferenceL(): iSegmentationStore.Next8BitReferenceL();
		iSmsMessage->EncodeMessagePDUsL(iSmsArray, reference);
		}

	TInt spare=iStoreInfo.iTotalEntries-iStoreInfo.iUsedEntries;
	if (spare<iSmsArray.Count())
		{
		User::Leave(KErrNoMemory);
		}

	User::LeaveIfError(iSmsStorage.Open(iSmsMessaging,iSlot.iStore));
	} // CSmsMessageWrite::SegmentMessageL


void CSmsMessageWrite::WriteNextSlot()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGEWRITE_WRITENEXTSLOT_1, "CSmsMessageWrite::WriteNextSlot()");

	if (iSmsArray.Count()>0)
		{
		iState=ESmsMessageWriteWritingPDUs;
		//Set PDU to the slot
		iSlot.iMsgData=iSmsArray[0].Pdu();
		//Sets Service Centre Address to the slot

		iSlot.iServiceCentre.iTelNumber = iSmsArray[0].Sca().iTelNumber;
		NMobilePhone::TMobileTON ton = static_cast<NMobilePhone::TMobileTON>(static_cast<TInt>(iSlot.iServiceCentre.iTypeOfNumber));
		NMobilePhone::TMobileNPI npi = static_cast<NMobilePhone::TMobileNPI>(static_cast<TInt>(iSlot.iServiceCentre.iNumberPlan));
		iSmsArray[0].Sca().iTypeOfAddress.ConvertToETelMM( ton, npi );

		iEntryToWrite = iSlot;
		iEntryToWrite.iIndex = -1;
 		iSmsStorage.Write( iStatus, iWriteEntryV1Pckg );
		TimedSetActive( iSmsSettings.Timeout() );
		}
	} // CSmsMessageWrite::WriteNextSlot


void CSmsMessageWrite::Complete(TInt aStatus)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGEWRITE_COMPLETE_1, "CSmsMessageWrite::Complete [aStatus=%d]", aStatus);

	__ASSERT_DEBUG(iState != ESmsMessageWriteIdle, SmspPanic(KSmspPanicUnexpectedState));

	//
	// Finish up...
	//
	iSmsArray.Reset();

	if (aStatus != KErrNone   &&
		iSmsMessage != NULL   &&  iSmsMessage->iSlotArray.Count() >0)
		{
		iSmsComm.DeletePDUs(iSmsMessage->iSlotArray, NULL);
		}

	iSmsMessage = NULL;
	iState      = ESmsMessageWriteIdle;

	//
	// Call the base function to perform the actual complete...
	//
	CSmsuActiveBase::Complete(aStatus);
	} // CSmsMessageWrite::Complete

