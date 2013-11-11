// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmsmsstoragetsyTraces.h"
#endif

#include <etelmm.h>
#include "cmmsmsstoragetsy.h"
#include "cmmphonetsy.h"
#include "cmmsmstsy.h"
#include "cmmsmsextinterface.h"
#include "MmTsy_numberOfSlots.h"
#include <ctsy/pluginapi/cmmdatapackage.h>

// ======== MEMBER FUNCTIONS ========

CMmSmsStorageTsy::CMmSmsStorageTsy():
    iReqHandleType(CMmSmsTsy::EMultimodeSmsReqHandleUnknown)
    {
    }

void CMmSmsStorageTsy::ConstructL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_CONSTRUCTL_1, "TSY: CMmSmsStorageTsy::ConstructL\n");
    ResetVariables();

    iSmsListArray = new ( ELeave ) CArrayPtrFlat<TSmsMsg>( 1 );
    iSmsReadAllArray = new ( ELeave ) CArrayPtrFlat<CListReadAllAttempt>( 1 );

    // register sms storage tsy in the message manager
    iMmPhone->MessageManager()->RegisterTsyObject( 
        CMmMessageManagerBase::EPhoneStoreTsy, this );
    }

CMmSmsStorageTsy* CMmSmsStorageTsy::NewL( 
    CMmSmsTsy* aMmSmsTsy, 
    CMmPhoneTsy* aMmPhone, 
    CMmSmsExtInterface* aMmSmsExtInterface, 
    CMmTsyReqHandleStore* aMmTsyReqHandleStore )
    {
    CMmSmsStorageTsy* subsession = new ( ELeave ) CMmSmsStorageTsy();
    CleanupClosePushL( *subsession );
    subsession->iMmSmsTsy = aMmSmsTsy;
    subsession->iMmPhone = aMmPhone;
    subsession->iMmSmsExtInterface = aMmSmsExtInterface;
    subsession->iMmTsyReqHandleStore = aMmTsyReqHandleStore;
    subsession->ConstructL();
    CleanupStack::Pop(subsession);

    return subsession;  
    }

CMmSmsStorageTsy::~CMmSmsStorageTsy()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_DTOR_1, "TSY: CMmSmsStorageTsy::~CMmSmsStorageTsy");
    if ( iMmPhone )
        {
        // deregister tsy object from message manager
        iMmPhone->MessageManager()->DeregisterTsyObject(this);        
        }

    if ( iSmsListArray )
        {
        delete iSmsListArray;        
        }
    iSmsListArray = NULL;

    if ( iSmsReadAllArray )
        {
        iSmsReadAllArray->ResetAndDestroy();
        delete iSmsReadAllArray;
        }
	iSmsReadAllArray = NULL;

	// Set pointers to NULL
	iMmTsyReqHandleStore = NULL;
	iMmPhone = NULL;
	iMmSmsExtInterface = NULL;
	iGetInfoInfoPtr = NULL;
	iNotifySmsStoreEventIndexPtr = NULL;
	iReadSmsEntryPtr = NULL;
	iReadAllBufSizePtr = NULL;
	iWriteSmsEntryPtr = NULL;
	iNotifySmsStoreEventPtr = NULL;


    // Set CMmSmsTsy's SmsStoragePointer to NULL
    iMmSmsTsy->SetSmsStorageTsy( NULL );
	iMmSmsTsy = NULL;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::Init 
// Initialisation method
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsStorageTsy::Init()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_INIT_1, "TSY: CMmSmsStorageTsy::Init.");
    // Send request to the Domestic OS layer.
    TRAP_IGNORE( iMmPhone->MessageManager()->HandleRequestL( 
        EMobilePhoneStoreGetInfo ); );
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::ExtFunc 
// Handle sms storage related requests that doesn't need
// trapping or call DoExtFuncL-method
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsStorageTsy::ExtFunc( 
    const TTsyReqHandle aTsyReqHandle, 
    const TInt aIpc, 
    const TDataPackage& aPackage )
    {
    TInt ret( KErrNone );
    TAny* dataPtr = aPackage.Ptr1();

    switch ( aIpc )
        {
        // SMS storage requests that doesn't need trapping
        case EMobilePhoneStoreReadAllPhase2:
            ret = ReadAllSmsPhase2( aTsyReqHandle, 
                REINTERPRET_CAST( RMobilePhone::TClientId*, dataPtr ),
                aPackage.Des2n() );
            break;
        // SMS storage requests that need trapping
        default:
            // Ensure the ReqHandleType is unset.
            // This will detect cases where this method indirectly calls itself
            // (e.g. servicing a client call that causes a self-reposting notification to complete and thus repost).
            // Such cases are not supported because iReqHandleType is in the context of this class instance,
            // not this request, and we don't want the values set by the inner request and the outer request
            // interfering with each other.
            __ASSERT_DEBUG(iReqHandleType==CMmSmsTsy::EMultimodeSmsReqHandleUnknown, User::Invariant());

            TInt leaveCode( KErrNone );
            TRAP( leaveCode, ret = DoExtFuncL( aTsyReqHandle, aIpc, 
                aPackage ); );

            if ( KErrNone != leaveCode )
                {
				//reset request handle to indicate the request is no longer ongoing
                iMmTsyReqHandleStore->FindAndResetTsyReqHandle( aTsyReqHandle );
				ReqCompleted( aTsyReqHandle, leaveCode );
                }
            else if ( CMmSmsTsy::EMultimodeSmsReqHandleUnknown != iReqHandleType )
                {
                // save request handle
#ifdef REQHANDLE_TIMER
                iMmSmsTsy->SetTypeOfResponse( iReqHandleType, aTsyReqHandle );
#else
                // Never comes here. See SetTypeOfResponse.
                iMmTsyReqHandleStore->SetTsyReqHandle( iReqHandleType, 
                    aTsyReqHandle );
#endif // REQHANDLE_TIMER   
                }
            // We've finished with this value now. Clear it so it doesn't leak
            //  up to any other instances of this method down the call stack
            iReqHandleType = CMmSmsTsy::EMultimodeSmsReqHandleUnknown;
            break;
        }
    
    return ret;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::DoExtFunc 
// Handling of sms storage related requests that needs trapping
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsStorageTsy::DoExtFuncL( 
    const TTsyReqHandle aTsyReqHandle, 
    const TInt aIpc, 
    const TDataPackage& aPackage )      
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_DOEXTFUNCL_1, "TSY: CMmSmsStorageTsy::DoExtFuncL. IPC:%{TIPCNamesList} Handle:%d Object:0x%08x", (TUint)aIpc, aTsyReqHandle, (TUint)this);
    TAny* dataPtr = aPackage.Ptr1();
    TAny* dataPtr2 = aPackage.Ptr2();

    TInt ret( KErrNone );

    switch ( aIpc )
        {
        // SMS message storage requests
        case EMobilePhoneStoreGetInfo:
            ret = GetInfoL( aTsyReqHandle, aPackage.Des1n() );
            break;
        case EMobilePhoneStoreRead:
            ret = ReadSmsL( aTsyReqHandle, aPackage.Des1n() );
            break;
        case EMobilePhoneStoreWrite:
            ret = WriteSmsL( aTsyReqHandle, aPackage.Des1n() );
            break;
        case EMobilePhoneStoreDelete:
            ret = DeleteSmsL( aTsyReqHandle, 
                REINTERPRET_CAST( TInt*, dataPtr ) );
            break;
        case EMobilePhoneStoreDeleteAll:
            ret = DeleteAllSmsL( aTsyReqHandle );
            break;
        case EMobilePhoneStoreNotifyStoreEvent:
            ret = NotifySmsStoreEvent( REINTERPRET_CAST( 
                RMobilePhoneStore::TMobileStoreEvent*, dataPtr ),
                REINTERPRET_CAST( TInt*, dataPtr2 ) );
            break;
        case EMobilePhoneStoreReadAllPhase1:
            ret = ReadAllSmsPhase1L( aTsyReqHandle, REINTERPRET_CAST( 
                CRetrieveMobilePhoneSmsList::TBatchRequestData*, dataPtr ), 
                REINTERPRET_CAST( TInt*, dataPtr2 ) );
            break;
        default:
            ret = KErrNotSupported;
            break;
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_DOEXTFUNCL_2, "TSY: CMmSmsStorageTsy::DoExtFuncL, error=%{TSymbianErrorCodes}", ret);
    return ret;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::CancelService 
// CancelService is called by the server when it is 
// "cleaning-up" any still outstanding asynchronous requests before closing 
// a client's sub-session. This will happen if a client closes its R-class 
// handle without cancelling outstanding asynchronous requests on
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsStorageTsy::CancelService( 
    const TInt aIpc, 
    const TTsyReqHandle aTsyReqHandle )
    {
    TInt ret( KErrNone );

    switch ( aIpc )
        {
        case EMobilePhoneStoreGetInfo:
            ret = GetInfoCancel( aTsyReqHandle );
            break;
        case EMobilePhoneStoreRead:
            ret = ReadSmsCancel( aTsyReqHandle );
            break;
        case EMobilePhoneStoreWrite:
        case EMobilePhoneStoreDelete:
        case EMobilePhoneStoreDeleteAll:
            ret = KErrNone;
            break;
        case EMobilePhoneStoreNotifyStoreEvent:
            ret = NotifySmsStoreEventCancel( aTsyReqHandle );
            break;
        case EMobilePhoneStoreReadAllPhase1:
        case EMobilePhoneStoreReadAllPhase2:
            ret = ReadAllSmsCancel( aTsyReqHandle );
            break;
        default:
            ret = KErrGeneral; 
            break;
        } 

    return ret;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::ReqModeL 
// When the ETel server receives an "extension" client request, 
// it will pass the IPC request number down to the TSY in order to find out 
// what type of request it is
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
CTelObject::TReqMode CMmSmsStorageTsy::ReqModeL( 
    const TInt aIpc )
    {
    CTelObject::TReqMode ret( 0 );
    switch ( aIpc )
        {        
        // Non-flow Controlled Services
        case EMobilePhoneStoreGetInfo:
        case EMobilePhoneStoreRead:
        case EMobilePhoneStoreWrite:
        case EMobilePhoneStoreDelete:
        case EMobilePhoneStoreDeleteAll:
        case EMobilePhoneStoreReadAllPhase1:
        case EMobilePhoneStoreReadAllPhase2:
            break;
        // Multiple Completion Services with Immediate Server Repost
        case EMobilePhoneStoreNotifyStoreEvent:
            ret = KReqModeMultipleCompletionEnabled | 
                  KReqModeRePostImmediately;
            break;
        default:
            User::Leave( KErrNotSupported );
            break;
        }

    return ret;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::NumberOfSlotsL 
// When the ETel server discovers that a request is "repost 
// immediately" it will ask the TSY how big a buffer it wants. 
// NumberOfSlotsL Returns number of slots to be used for given requests
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsStorageTsy::NumberOfSlotsL( 
    const TInt aIpc )
    {
    TInt numberOfSlots( 1 );

    switch ( aIpc )
        {
        case EMobilePhoneStoreNotifyStoreEvent:
            numberOfSlots = KMmPhoneStoreNotifyStoreEventSlots;
            break;
        default:
            User::Leave( KErrNotSupported );
            break;
        }

    return numberOfSlots;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::OpenNewObjectL 
// This method opens extendable sub-sessions
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
CTelObject* CMmSmsStorageTsy::OpenNewObjectL(
    TDes& /*aName*/ )     
    {
    User::Leave( KErrNotSupported );
	//lint -e{527} "unreachable code"

    return NULL;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::OpenNewObjectByNameL 
// This method opens extendable sub-sessions
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
CTelObject* CMmSmsStorageTsy::OpenNewObjectByNameL( 
    const TDesC& /*aName*/ )  
    {
    User::Leave( KErrNotSupported );
	//lint -e{527} "unreachable code"

    return NULL;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::RegisterNotification 
// Called when the server recognises that this notification
// is being posted for the first time on this sub-session object.
// It enables the TSY to "turn on" any regular notification messages that it 
// may receive from DOS
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsStorageTsy::RegisterNotification( 
    const TInt aIpc )     
    {
    TInt ret( KErrNone );

    switch ( aIpc )
        {
        case EMobilePhoneStoreNotifyStoreEvent:
            ret = KErrNone;
            break;
        default:
            ret = KErrNotSupported;
            break;
        }

    return ret;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::DeregisterNotification 
// Called when the server recognises that this notification
// will not be posted again because the last client to have 
// a handle on this sub-session object has just closed the handle.
// It enables the TSY to "turn off" any regular notification messages that 
// it may receive from the DOS
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsStorageTsy::DeregisterNotification( 
    const TInt aIpc )
    {
    TInt ret( KErrNotSupported );

    switch ( aIpc )
        {
        case EMobilePhoneStoreNotifyStoreEvent:
            ret = KErrNone;
            break;
        default:
            ret = KErrNotSupported;
            break;
        }

    return ret;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::GetInfoL 
// This method start getting info prosess from selected store (SIM)
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsStorageTsy::GetInfoL( 
    const TTsyReqHandle aTsyReqHandle, 
    TDes8* aSmsStoreInfoPckg )
    {
    
    TInt ret = KErrArgument;

    if (aSmsStoreInfoPckg)
    	{
	    	
    	RMobilePhoneStore::TMobilePhoneStoreInfoV1 temp;
		TPckg<RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg> tempPckg(temp);
		
		TInt expectedLength = tempPckg.MaxLength();
		TInt paramLength = aSmsStoreInfoPckg->MaxLength();
		
		if (paramLength >= expectedLength)
			{    
		    TTsyReqHandle getInfoHandle = iMmTsyReqHandleStore->GetTsyReqHandle( 
		        CMmSmsTsy::EMultimodeSmsGetInfo );

		    if ( CMmSmsTsy::EMultimodeSmsReqHandleUnknown < getInfoHandle )
		        {
		        // The request is already in processing because of previous request.
		        // Complete request with status value informing the client about 
		        // the situation.
		        ReqCompleted( aTsyReqHandle, KErrServerBusy );
		        }
		    else
		        {
		        iGetInfoInfoPtr = aSmsStoreInfoPckg;

		        // allow for EMobilePhoneStoreGetInfo immediate completion
#ifdef REQHANDLE_TIMER
		        iMmSmsTsy->SetTypeOfResponse( CMmSmsTsy::EMultimodeSmsGetInfo, 
		            aTsyReqHandle );
#else
		            // Never comes here. See SetTypeOfResponse.
		            iMmTsyReqHandleStore->SetTsyReqHandle( 
		                CMmSmsTsy::EMultimodeSmsGetInfo, aTsyReqHandle );
#endif // REQHANDLE_TIMER

		        // Send request to the Domestic OS layer.
		        ret = iMmPhone->MessageManager()->HandleRequestL( 
		            EMobilePhoneStoreGetInfo );

		        // dos layer returned with error and didn't complete request
		        if ( ret != KErrNone && iMmTsyReqHandleStore->ResetTsyReqHandle(
		            CMmSmsTsy::EMultimodeSmsGetInfo) )
		            {
		            ReqCompleted( aTsyReqHandle, ret );
		            }
		        }
			
			ret = KErrNone;
			}
    	}
    
    return ret;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::GetInfoCancel 
// This method cancel GetInfo request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsStorageTsy::GetInfoCancel( 
    const TTsyReqHandle aTsyReqHandle )
    {
    iMmTsyReqHandleStore->ResetTsyReqHandle( 
        CMmSmsTsy::EMultimodeSmsGetInfo );
    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::CompleteGetInfo 
// Complete Clients GetInfo request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsStorageTsy::CompleteGetInfo( 
    TInt aResult, 
    CMmDataPackage* aDataPackage )
    {
    TUint8 totalEntries( 0 );
    TInt usedEntries( 0 );

    TTsyReqHandle reqHandle = iMmTsyReqHandleStore->ResetTsyReqHandle( 
        CMmSmsTsy::EMultimodeSmsGetInfo );

    if ( ( KErrNone == aResult ) || ( KErrNotFound == aResult ) )
        {
        aDataPackage->UnPackData( totalEntries, usedEntries );
        }
	iTotalEntries = totalEntries;
	iUsedEntries = usedEntries;
    
	// set the member variable to aTotalEntries. It Will be used
	// when it writes SMS on SIM card
	iMmSmsTsy->SetSmsNumOfLoc ( totalEntries );

    if ( reqHandle )
        {
        // Request was compeleted successfully
        if ( ( KErrNone == aResult ) || ( KErrNotFound == aResult ) )
            {
            aResult = iMmSmsExtInterface->StoreInfo( 
                iGetInfoInfoPtr, totalEntries, usedEntries );
            }
        ReqCompleted( reqHandle, aResult );   
        }
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::ReadSmsL
// This method start SMS message reading process from selected location
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsStorageTsy::ReadSmsL( 
    const TTsyReqHandle aTsyReqHandle, 
    TDes8* aSmsPckg )                       
    {
    TInt ret( KErrArgument );
    
    if ( aSmsPckg )
        {        
        TInt paramLength = aSmsPckg->MaxLength();
        
        RMobileSmsStore::TMobileGsmSmsEntryV1 temp;
        RMobileSmsStore::TMobileGsmSmsEntryV1Pckg tempPckg(temp);
                	
        TInt expectedLength = tempPckg.MaxLength();
        
        if (paramLength == expectedLength)
            {    
    
            TTsyReqHandle readSmsHandle = iMmTsyReqHandleStore->GetTsyReqHandle( 
                CMmSmsTsy::EMultimodeSimStSmsReadSms );

            if ( CMmSmsTsy::EMultimodeSmsReqHandleUnknown < readSmsHandle ) 
                {
                // The request is already in processing because of previous request.
                // Complete request with status value informing the client about 
                // the situation.
                ReqCompleted( aTsyReqHandle, KErrServerBusy );
                }
            else
                {
                RMobileSmsStore::TMobileGsmSmsEntryV1Pckg* smsPckg = 
                    reinterpret_cast< RMobileSmsStore::TMobileGsmSmsEntryV1Pckg* > 
        			( aSmsPckg );
                RMobileSmsStore::TMobileGsmSmsEntryV1& smsEntry = ( *smsPckg )();

                iReadSmsEntryPtr = aSmsPckg;

                CMmDataPackage package;
                package.PackData( &smsEntry.iIndex );


                //  allow for EMobilePhoneStoreRead immediate completion
#ifdef REQHANDLE_TIMER
                iMmSmsTsy->SetTypeOfResponse( CMmSmsTsy::EMultimodeSimStSmsReadSms,
                    aTsyReqHandle );
#else
                    // Never comes here. See SetTypeOfResponse.
                    iMmTsyReqHandleStore->SetTsyReqHandle( 
                        CMmSmsTsy::EMultimodeSimStSmsReadSms, aTsyReqHandle );
#endif // REQHANDLE_TIMER

                // send request to DOS layer
                ret = iMmPhone->MessageManager()->HandleRequestL( 
                    EMobilePhoneStoreRead, &package );

                 // DOS layer returned error (without completing request)
        		if ( KErrNone != ret && iMmTsyReqHandleStore->ResetTsyReqHandle( 
                    CMmSmsTsy::EMultimodeSimStSmsReadSms ))
                    {
                    ReqCompleted( aTsyReqHandle, ret );
                    }
                }
            ret = KErrNone;
            }
        }
            
    return ret;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::ReadSmsCancel 
// This method cancels Read request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsStorageTsy::ReadSmsCancel( 
    const TTsyReqHandle aTsyReqHandle )
    {
    iMmTsyReqHandleStore->ResetTsyReqHandle( 
        CMmSmsTsy::EMultimodeSimStSmsReadSms );
    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::CompleteReadSms 
// Complete Clients Read Sms request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsStorageTsy::CompleteReadSms( 
    TInt aResult, 
    CMmDataPackage* aDataPackage )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_COMPLETEREADSMS_1, "TSY: CMmSmsStorageTsy::CompleteReadSms. Error: %{TSymbianErrorCodes}",aResult);
    TTsyReqHandle reqHandle = iMmTsyReqHandleStore->ResetTsyReqHandle( 
        CMmSmsTsy::EMultimodeSimStSmsReadSms );

    if ( reqHandle )
        {
        if ( KErrNone == aResult )
            {   
            TSmsMsg* smsMsg;
            aDataPackage->UnPackData( &smsMsg );
            aResult = UnpackSmsReadEntryPackage( iReadSmsEntryPtr, smsMsg );
			}
        ReqCompleted( reqHandle, aResult );    
        }
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::WriteSmsL 
// This method start SMS message writing prosess to the 
// selected location
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsStorageTsy::WriteSmsL( 
    const TTsyReqHandle aTsyReqHandle, 
    TDesC8* aSmsPckg )       
    {
    TTsyReqHandle writeSmsHandle = iMmTsyReqHandleStore->GetTsyReqHandle( 
        CMmSmsTsy::EMultimodeSimStSmsWriteSms );

    if ( sizeof( RMobilePhoneStore::TMobilePhoneStoreEntryV1 ) > aSmsPckg->Length() )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_WRITESMSL_1, "TSY: CMmSmsStorageTsy::WriteSms bad size argument");
        // Complete the request with appropiate error        
        return KErrArgument;
        }

    if ( CMmSmsTsy::EMultimodeSmsReqHandleUnknown < writeSmsHandle )
        {
        // The request is already in processing because of previous request.
        // Complete request with status value informing the client about 
        // the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
		TInt ret (0 );

		 // typecast for aEntry
		RMobileSmsStore::TMobileGsmSmsEntryV1Pckg* entryPckg = 
			reinterpret_cast< RMobileSmsStore::TMobileGsmSmsEntryV1Pckg* > 
			( aSmsPckg );
		RMobileSmsStore::TMobileGsmSmsEntryV1 entry = ( *entryPckg )();

		iWriteSmsEntryPtr = aSmsPckg;

        // pack data
        CMmDataPackage package;
        package.PackData( &aSmsPckg, &entry.iIndex );

        ret = iMmPhone->MessageManager()->HandleRequestL( 
            EMobilePhoneStoreWrite, &package );

		if ( KErrNone == ret )
			{
			iReqHandleType = CMmSmsTsy::EMultimodeSimStSmsWriteSms;
			}
		else
			{
			// Message construction failed or phonet sender returned error
			ReqCompleted( aTsyReqHandle, ret );
			}
       }

    return KErrNone;
    }   

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::CompleteWriteSms 
// Complete Clients WriteSms request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsStorageTsy::CompleteWriteSms( 
    TInt aResult, 
    CMmDataPackage* aDataPackage )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_COMPLETEWRITESMS_1, "TSY: CMmSmsStorageTsy::CompleteWriteSms. Error: %{TSymbianErrorCodes}",aResult);
    TTsyReqHandle reqHandle = iMmTsyReqHandleStore->ResetTsyReqHandle( 
        CMmSmsTsy::EMultimodeSimStSmsWriteSms );

    if ( reqHandle )
        {
        TUint8 location( 0 );
		TBool receivedClass2ToBeResent ( EFalse );
		if( aDataPackage )
			{
			aDataPackage->UnPackData( location, receivedClass2ToBeResent );	
			}
        if ( KErrNone == aResult )
            {
            TInt ret = UnpackSmsWriteEntryPackage( iWriteSmsEntryPtr, 
                location );
            
            CompleteNotifyStoreEvent( location, 
                RMobilePhoneStore::KStoreEntryAdded );   

            ReqCompleted( reqHandle, ret );
            }
        else
            {
            // SIM full
            if ( KErrDiskFull == aResult )
                {
                CompleteNotifyStoreEvent( -1, RMobilePhoneStore::KStoreFull );              
                }
            ReqCompleted( reqHandle, aResult );
            }
		
		if ( receivedClass2ToBeResent )
			{
            // silently IGNORE if call to DOS fails
            TInt trapError = 0;
            TRAP( trapError, iMmPhone->MessageManager()->HandleRequestL( 
                EMobileSmsMessagingResumeSmsReception ); );
            }
        }
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::DeleteSmsL 
// This method start SMS message deleting prosess on the 
// selected location
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsStorageTsy::DeleteSmsL( 
    const TTsyReqHandle aTsyReqHandle, 
    TInt* aIndex )       
    {
    TTsyReqHandle deleteSmsHandle = iMmTsyReqHandleStore->GetTsyReqHandle( 
        CMmSmsTsy::EMultimodeSimStSmsEraseSms );

    if ( 0 < deleteSmsHandle )
        {
        // The request is already in processing because of previous request.
        // Complete request with status value informing the client about 
        // the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
		iDeleteSmsIndex = *aIndex;
       
        CMmDataPackage package;
        package.PackData( aIndex );

        TInt ret = iMmPhone->MessageManager()->HandleRequestL( 
            EMobilePhoneStoreDelete, &package );
  
		if ( KErrNone == ret )
			{
			iReqHandleType = CMmSmsTsy::EMultimodeSimStSmsEraseSms;
			}
		else
			{
            // Message construction failed or phonet sender returned error
            ReqCompleted( aTsyReqHandle, ret );
            }
        }
        
    return KErrNone;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::CompleteDeleteSms 
// Complete Clients DeleteSms request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsStorageTsy::CompleteDeleteSms( 
    TInt aResult )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_COMPLETEDELETESMS_1, "TSY: CMmSmsStorageTsy::CompleteDeleteSms. Error: %{TSymbianErrorCodes}", aResult);
    TTsyReqHandle reqHandle = iMmTsyReqHandleStore->ResetTsyReqHandle( 
        CMmSmsTsy::EMultimodeSimStSmsEraseSms );

    if ( reqHandle )
        {
        if ( KErrNone == aResult )
            {
            // Notify that one entry was deleted from the store.
            CompleteNotifyStoreEvent( iDeleteSmsIndex, 
                RMobilePhoneStore::KStoreEntryDeleted );
            }
        ReqCompleted( reqHandle, aResult );
        }
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::DeleteAllSmsL
// This method start Delete all messages prosess on the selected
// Store. Method make SimStSms Erase Request where location of 
// slot (KDeleteAllSmsMessagesIndex) is zero. It means that all
// memory locations are erased
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsStorageTsy::DeleteAllSmsL( 
    const TTsyReqHandle aTsyReqHandle )
    {
    TTsyReqHandle deleteAllSmsHandle = iMmTsyReqHandleStore->GetTsyReqHandle( 
        CMmSmsTsy::EMultimodeSimStSmsEraseAllSms );

    if ( 0 < deleteAllSmsHandle )
        {
        // The request is already in processing because of previous request.
        // Complete request with status value informing the client about 
        // the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        // Send request to the Domestic OS layer.
        TInt ret = iMmPhone->MessageManager()->HandleRequestL( 
            EMobilePhoneStoreDeleteAll );
    
        if ( KErrNone == ret )
            {
            iReqHandleType = CMmSmsTsy::EMultimodeSimStSmsEraseAllSms;
            }
        else
            {
            // Message construction failed or phonet sender returned error
            ReqCompleted( aTsyReqHandle, ret );
            }
        }
        
    return KErrNone;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::CompleteDeleteAllSms 
// Complete Clients DeleteAllSms request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsStorageTsy::CompleteDeleteAllSms( 
    TInt aResult )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_COMPLETEDELETEALLSMS_1,  "TSY: CMmSmsStorageTsy::CompleteDeleteAllSms. Error: %{TSymbianErrorCodes}", aResult );
    TTsyReqHandle reqHandle = iMmTsyReqHandleStore->ResetTsyReqHandle( 
        CMmSmsTsy::EMultimodeSimStSmsEraseAllSms );

    if ( reqHandle )
        {
        if ( KErrNone == aResult )
            {
            // Notify that all messages were deleted from the store.
            CompleteNotifyStoreEvent( -1, RMobilePhoneStore::KStoreEmpty );
            }
        ReqCompleted( reqHandle, aResult );
        }
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::NotifySmsStoreEvent 
// This method activates notifying of SMS store events
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsStorageTsy::NotifySmsStoreEvent( 
    RMobilePhoneStore::TMobileStoreEvent* aEvent, 
    TInt* aIndex )                                   
    {
    iReqHandleType = CMmSmsTsy::EMultimodeSimSmsNotifyStoreEvent;
    iNotifySmsStoreEventPtr = aEvent;
    iNotifySmsStoreEventIndexPtr = aIndex;

    return KErrNone;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::NotifyStoreEventCancel 
// This method cancels NotifyStoreEventCancel request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsStorageTsy::NotifySmsStoreEventCancel( 
    const TTsyReqHandle aTsyReqHandle )
    {
    iMmTsyReqHandleStore->ResetTsyReqHandle( 
        CMmSmsTsy::EMultimodeSimSmsNotifyStoreEvent );
    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::CompleteNotifyStoreEvent 
// This method is called when some of SMS store events happen
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsStorageTsy::CompleteNotifyStoreEvent( 
    TInt aLocation, 
    TUint32 aEvent )
    {     
    TTsyReqHandle reqHandle = iMmTsyReqHandleStore->ResetTsyReqHandle( 
        CMmSmsTsy::EMultimodeSimSmsNotifyStoreEvent );

    // if new entry has been added to Store, get correct notification    
	if ( RMobilePhoneStore::KStoreEntryAdded == aEvent )  
        {
		// if store has space, get "StoreAdded" notifications
		iUsedEntries++;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_COMPLETENOTIFYSTOREEVENT_1, "TSY: CMmSmsStorageTsy::StoreEntryAdded notifications");
		if ( iUsedEntries == iTotalEntries )
			{ 
		// if store gets full after class2 message, get "StoreAdded and 
		// StoreFull" notifications
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_COMPLETENOTIFYSTOREEVENT_2, "TSY: CMmSmsStorageTsy::StoreEntryAdded and StoreFull notifications");
			aEvent = ( RMobilePhoneStore::KStoreFull | 
			    RMobilePhoneStore::KStoreEntryAdded );
			}
		}
	// if one or more entry has been deleted from store, get correct 
    // notifications
	if ( RMobilePhoneStore::KStoreEntryDeleted == aEvent )  
        {
		// if entry has been deleted from store, get "EntryDeleted" 
        // notification
		iUsedEntries--;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_COMPLETENOTIFYSTOREEVENT_3, "TSY: CMmSmsStorageTsy::StoreEntryDeleted notifications");
		if ( (iUsedEntries+1) == iTotalEntries )
			{
		    // if store has been full, after deleting get notifications 
		    // "EntryDeleted" and "StoreHasSpace"
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_COMPLETENOTIFYSTOREEVENT_4, "TSY: CMmSmsStorageTsy::StoreEntryDeleted and StoreHasSpace notifications");
			aEvent = ( RMobilePhoneStore::KStoreHasSpace | 
			    RMobilePhoneStore::KStoreEntryDeleted );
			}
		if ( 0 == iUsedEntries )
			{
			aEvent = ( 
				RMobilePhoneStore::KStoreEmpty
				| RMobilePhoneStore::KStoreEntryDeleted );
			}
			
		}

    if ( reqHandle )
        {

        TUint32* smsStoreEvent = REINTERPRET_CAST( TUint32*, 
            iNotifySmsStoreEventPtr );

        *smsStoreEvent = aEvent;
        *iNotifySmsStoreEventIndexPtr = aLocation;

OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_COMPLETENOTIFYSTOREEVENT_5, "TSY: CMmSmsStorageTsy::CompleteNotifyStoreEvent:notified client about event=0x%08x, location=%d", (TUint)aEvent, aLocation );
        ReqCompleted( reqHandle, KErrNone );
        }
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::ReadAllSmsPhase1L 
// This method starts first phase of SMS messages retrieval. On 
// the first phase method store locations temporarely to the TSY.
// On the second phase method copy the streamed list to the 
// client
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsStorageTsy::ReadAllSmsPhase1L( 
    const TTsyReqHandle aTsyReqHandle, 
    CRetrieveMobilePhoneSmsList::TBatchRequestData const* aRequest, 
    TInt* aBufSize )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_READALLSMSPHASE1L_1, "TSY: CMmSmsStorageTsy::ReadAllSmsPhase1L");
    TInt ret( KErrNone );

    TTsyReqHandle readAllSmsHandle = iMmTsyReqHandleStore->GetTsyReqHandle( 
        CMmSmsTsy::EMultimodeSimStSmsReadAllSms );

    if ( CMmSmsTsy::EMultimodeSmsReqHandleUnknown < readAllSmsHandle )
        {
        // The request is already in processing because of previous request.
        // Complete request with status value informing the client about 
        // the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else if ( aRequest->iBatchRequest || 
        iMmSmsExtInterface->GetSupportedEntry() != aRequest->iEntryType )
        {
        // Client is asking for batch of SMS store entries or CDMA SMS store 
        // entries. This feature is not supported.
        ReqCompleted( aTsyReqHandle, KErrNotSupported );
        }
    else   
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_READALLSMSPHASE1L_2, "TSY: CMmSmsStorageTsy::ReadAllSmsPhase1. SMS reading starts.");
        // Client is asking for all SMS store entries
        iReadAllBufSizePtr = aBufSize;
        iReadAllId = aRequest->iClient;

        // Lets reset the array
		iSmsListArray->Reset();
		
		// SMS storage contains messages
        if ( 0 < iMmSmsTsy->GetSmsNumOfLoc () ) 
            {
            // allow for EMobilePhoneStoreReadAllPhase1 immediate completion
#ifdef REQHANDLE_TIMER
            iMmSmsTsy->SetTypeOfResponse( 
                CMmSmsTsy::EMultimodeSimStSmsReadAllSms, aTsyReqHandle );
#else
            iMmTsyReqHandleStore->SetTsyReqHandle( 
                CMmSmsTsy::EMultimodeSimStSmsReadAllSms, aTsyReqHandle );
#endif // REQHANDLE_TIMER

            // Lets read first message from store
		    // First index is 1
            ret = iMmPhone->MessageManager()->HandleRequestL( 
                EMobilePhoneStoreReadAllPhase1 );
            
            if ( KErrNone != ret )
            	{
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_READALLSMSPHASE1L_3, "TSY: CMmSmsStorageTsy::ReadAllSmsPhase1L - and the returned value for EMobilePhoneStoreReadAllPhase1:%{TSymbianErrorCodes}", ret);
				(void)iMmTsyReqHandleStore->ResetTsyReqHandle( CMmSmsTsy::EMultimodeSimStSmsReadAllSms );
				// the returned value from ResetTsyReqHandle is not needed. 
            	}
            }
        // SMS Storage contains 0 messages
		else 
			{
			ret =  KErrNotFound;
			}

		// DOS layer returned with error
	    if ( ret != KErrNone )
	        {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_READALLSMSPHASE1L_4, "TSY: CMmSmsStorageTsy::ReadAllSmsPhase1L aTsyReqHandle: %u and ret:%{TSymbianErrorCodes}", (TUint)aTsyReqHandle, ret);
	        ReqCompleted( aTsyReqHandle, ret );
	        }
			
		}

    return KErrNone;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::ReadAllSmsPhase2 
// This method handle and complete second phase of SMS messages retrieval
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsStorageTsy::ReadAllSmsPhase2( 
    const TTsyReqHandle aTsyReqHandle, 
    RMobilePhone::TClientId const* aId, 
    TDes8* aBuffer )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_READALLSMSPHASE2_1, "TSY: CMmSmsStorageTsy::ReadAllSmsPhase2");
    CListReadAllAttempt* read = NULL;

    // Find the read attempt from this client
    for ( TInt i = 0 ; i < iSmsReadAllArray->Count() ; ++i )
        {
        read = iSmsReadAllArray->At( i );
        if ( ( read->iClient.iSessionHandle == aId->iSessionHandle ) &&
            ( read->iClient.iSubSessionHandle == aId->iSubSessionHandle ) )
            {
            TPtr8 bufPtr( ( read->iListBuf )->Ptr( 0 ) );
            // Copy the streamed list to the client
            aBuffer->Copy( bufPtr );
            delete read;
            iSmsReadAllArray->Delete( i );
            ReqCompleted( aTsyReqHandle, KErrNone );
            
            return KErrNone;
            }
        }

    // Doesn't found the matching client from read all phase 1
    iSmsReadAllArray->ResetAndDestroy();
    ReqCompleted( aTsyReqHandle, KErrCorrupt );
    
    return KErrNone;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::ReadAllSmsCancel 
// This method cancels read all SMSes request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsStorageTsy::ReadAllSmsCancel( 
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_READALLSMSCANCEL_1, "TSY: CMmSmsStorageTsy::ReadAllSmsCancel");
    iMmTsyReqHandleStore->ResetTsyReqHandle( 
        CMmSmsTsy::EMultimodeSimStSmsReadAllSms );

    iSmsListArray->Reset();
    iSmsReadAllArray->ResetAndDestroy();

    ReqCompleted( aTsyReqHandle, KErrCancel );
    
    return KErrNone;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::RetrieveSmsListReadSmsL 
// Complete SimStSmsReadReq request and calls CompleteReadAllSmsPhase1-method
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsStorageTsy::RetrieveSmsListReadSmsL( 
	TInt aResult,
    CMmDataPackage* aDataPackage )
	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_RETRIEVESMSLISTREADSMSL_1, "TSY: CMmSmsStorageTsy::RetrieveSmsListReadSmsL");

	TTsyReqHandle reqHandle = iMmTsyReqHandleStore->GetTsyReqHandle( 
        CMmSmsTsy::EMultimodeSimStSmsReadAllSms );

	// Flag which wil indicates if Class2 SMS has been received while 
	// it was reading the SMS Slots.
	TBool receivedClass2ToBeReSent;
	
	CArrayPtrFlat<TSmsMsg>* tempSmsListArray = NULL;

    if ( reqHandle )
        {
		aDataPackage->UnPackData( tempSmsListArray, receivedClass2ToBeReSent );
        
        for ( TInt i = 0; i < tempSmsListArray->Count(); i++ )
            {
            iSmsListArray->AppendL( tempSmsListArray->At( i ) );
            }
        
        tempSmsListArray->Reset();
        tempSmsListArray = NULL;

		// if last slot was empty or used
		if  ( ( KErrNotFound == aResult ) || ( KErrNone == aResult ) )
			{
       		if ( 0 < iSmsListArray->Count() )  
				{
				// Store contained SMS messages
				TInt ret = iMmSmsExtInterface->CompleteReadAllSmsPhase1L( 
				    iSmsListArray, // SMS message list storage array
				    iReadAllId, // Read all SMS messages Id
				    iSmsReadAllArray, // Read all SMS messages array
				    iReadAllBufSizePtr // Read all SMS messages pointer
    				);
				CompleteReadAllSmsPhase1( ret, receivedClass2ToBeReSent );
				}		
			else    
				{
				// Store was empty
				CompleteReadAllSmsPhase1( KErrNotFound, 
				    receivedClass2ToBeReSent );
				}
			}
		else
			{
			CompleteReadAllSmsPhase1( aResult, receivedClass2ToBeReSent );
			}
		}	
	}

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::CompleteReadAllSmsPhase1 
// This method completes client's ReadAllSmsPhase1 request
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsStorageTsy::CompleteReadAllSmsPhase1(
    TInt aResult, 
	TBool aReceivedClass2ToBeReSent )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_COMPLETEREADALLSMSPHASE1_1, "TSY: CMmSmsStorageTsy::CompleteReadAllSmsPhase1. Complete read first phase error: %{TSymbianErrorCodes}", aResult);
    TTsyReqHandle reqHandle = iMmTsyReqHandleStore->ResetTsyReqHandle( 
        CMmSmsTsy::EMultimodeSimStSmsReadAllSms );
    if ( reqHandle )
        {
        // Complete first phase of list retrieval
        ReqCompleted( reqHandle, aResult );
        iSmsListArray->Reset();

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSSTORAGETSY_COMPLETEREADALLSMSPHASE1_2, "TSY: CMmSmsStorageTsy::CompleteReadAllSmsPhase1 aReceivedClass2ToBeReSent: %d",aReceivedClass2ToBeReSent);
        if ( aReceivedClass2ToBeReSent )
            {
            // silently IGNORE if call to DOS fails
            TInt trapError = 0;
            TRAP( trapError, iMmPhone->MessageManager()->HandleRequestL( 
                 EMobileSmsMessagingResumeSmsReception ); );
            }
        }
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::GetTsyReqHandleStore 
// Get pointer to Tsy request handle store
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
CMmTsyReqHandleStore* CMmSmsStorageTsy::GetTsyReqHandleStore()
	{
    // TAN to be removed before releasing?
	return iMmTsyReqHandleStore;
	}

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::ResetVariables 
// Reset all used variables
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
void CMmSmsStorageTsy::ResetVariables()
    {
    // get info variables
    iGetInfoInfoPtr = NULL;

    // read sms variables
    iReadSmsEntryPtr = NULL;    

    // write sms variables
    iWriteSmsEntryPtr = NULL;

    // notify SMS store event
    iNotifySmsStoreEventPtr = NULL;
    iNotifySmsStoreEventIndexPtr = NULL;

    // read all parameter sets variables
    iReadAllBufSizePtr = NULL;
    iUsedEntries = 0;
    iTotalEntries = 0;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::UnpackSmsReadEntryPackage 
// This method unpack a entry package and set contents of a message to the 
// entry.
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsStorageTsy::UnpackSmsReadEntryPackage(
    TAny* aReadSmsEntryPtr, 
    TSmsMsg*  aSmsMsg )
    {
    RMobileSmsStore::TMobileGsmSmsEntryV1Pckg* entryPckg = 
        REINTERPRET_CAST( RMobileSmsStore::TMobileGsmSmsEntryV1Pckg*,
        aReadSmsEntryPtr );
    RMobileSmsStore::TMobileGsmSmsEntryV1& entry = ( *entryPckg )();
    
    switch( aSmsMsg->iMessageStatus )
        {
        case KSimSmsMoNotSent: //0x07
            entry.iMsgStatus = RMobileSmsStore::EStoredMessageUnsent;
            break;
        case KSimSmsStatusReportReqNotReceived: //0x0D
        case KSimSmsMoSent: //0x05
            entry.iMsgStatus = RMobileSmsStore::EStoredMessageSent;
            break;
        case KSimSmsMtRead: //0x01
            entry.iMsgStatus = RMobileSmsStore::EStoredMessageRead;
            break;
        case KSimSmsMtNotRead: //0x03
            entry.iMsgStatus = RMobileSmsStore::EStoredMessageUnread;
            break;
	    case KSimSmsStatusReportReqReceivedNotStored: //0x15
		case KSimSmsStatusReportReqReceivedStored: //0x1D
			entry.iMsgStatus = RMobileSmsStore::EStoredMessageDelivered;
            break;
        default:
            entry.iMsgStatus = RMobileSmsStore::EStoredMessageUnknownStatus;
            break;
        }

    // TMobileSmsFixedEntryV1 information
    entry.iMsgData.Copy( aSmsMsg->iSmsMsg );

    // Lets handle Service Centre address
    entry.iServiceCentre.iTypeOfNumber = aSmsMsg->iMobileScTON;
    entry.iServiceCentre.iNumberPlan = aSmsMsg->iMobileScNPI;
    entry.iServiceCentre.iTelNumber.Copy( aSmsMsg->iServiceCentre );
    
    return KErrNone;
    }

// --------------------------------------------------------------------------- 
// CMmSmsStorageTsy::UnpackSmsWriteEntryPackage 
// This method unpack a entry package and set location of the slot to the 
// entry.
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsStorageTsy::UnpackSmsWriteEntryPackage(
    TAny* aWriteSmsEntryPtr, 
    TInt  aLocation )
    {
    RMobileSmsStore::TMobileGsmSmsEntryV1Pckg* 
        entryPckg = REINTERPRET_CAST
        ( RMobileSmsStore::TMobileGsmSmsEntryV1Pckg*, aWriteSmsEntryPtr );
    RMobileSmsStore::TMobileGsmSmsEntryV1& entry = ( *entryPckg )();

    // Let's copy location index
    entry.iIndex = aLocation;      
    
    return KErrNone;
    }

//  End of File 
