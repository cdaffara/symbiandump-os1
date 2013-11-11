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
#include "cmmsmsgsmextTraces.h"
#endif

#include <e32std.h>        // error values
#include <mmlist.h>
#include "cmmsmsgsmext.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "cmmphonetsy.h"
#include <ctsy/serviceapi/cmmsmsutility.h>

// ======== MEMBER FUNCTIONS ========

CMmSmsGsmExt::CMmSmsGsmExt()
    {
    }

CMmSmsGsmExt* CMmSmsGsmExt::NewL()
    {
    CMmSmsGsmExt* aSmsGsmExt = new ( ELeave ) CMmSmsGsmExt();

    return aSmsGsmExt;
    }

CMmSmsGsmExt::~CMmSmsGsmExt()
    {
    }

// --------------------------------------------------------------------------- 
// CMmSmsGsmExt::CompleteReceiveMessage 
// This method takes reception of incoming unstored SMS
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsGsmExt::CompleteReceiveMessage(
    TSmsMsg* aSmsMsg, 
    TAny* aReceiveMessageParamsPtr, 
    TDes8* aReceiveMessagePduPtr )
    {   
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg* msgAttPckg = 
        reinterpret_cast< RMobileSmsMessaging::
        TMobileSmsReceiveAttributesV1Pckg* > ( aReceiveMessageParamsPtr );
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1& msgAttributes = 
        ( *msgAttPckg )();

    // This information is available
    msgAttributes.iFlags = ( RMobileSmsMessaging::KSmsDataFormat | 
        RMobileSmsMessaging::KGsmServiceCentre | 
        RMobileSmsMessaging::KIncomingStatus );

    // Message type
    msgAttributes.iDataFormat = RMobileSmsMessaging::EFormatGsmTpdu;

    // Lets handle Service Centre address
    msgAttributes.iGsmServiceCentre.iTypeOfNumber = aSmsMsg->iMobileScTON;
    msgAttributes.iGsmServiceCentre.iNumberPlan = aSmsMsg->iMobileScNPI;
    msgAttributes.iGsmServiceCentre.iTelNumber.Copy( 
        aSmsMsg->iServiceCentre );

     // Copy SMS TPDU
    aReceiveMessagePduPtr->Copy( aSmsMsg->iSmsMsg );

    if ( KSimSmsNotPresent == aSmsMsg->iMessageStatus )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSGSMEXT_COMPLETERECEIVEMESSAGE_1, "TSY: CMmSmsGsmExt::CompleteReceiveMessage. Class 0, 1 or 3 message");
        
        // SMS message that is routed directly to the TSY. This message is not
        // acknowledge to the network.
        msgAttributes.iStatus = 
            RMobileSmsMessaging::EMtMessageUnstoredClientAck;
        // Reset store information
        msgAttributes.iStore.SetLength( 0 );
        msgAttributes.iStoreIndex = 0;
        }
   else if ( aSmsMsg->iSmsClass2 && ( 0 != aSmsMsg->iLocation ) ) 
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSGSMEXT_COMPLETERECEIVEMESSAGE_2, "TSY: CMmSmsGsmExt::CompleteReceiveMessage. Class 2 message stored in SIM");
        // Class 2 SMS message that is stored in SIM. This message is 
        // acknowledged to the network by SMS server.
		msgAttributes.iStatus = RMobileSmsMessaging::EMtMessageUnknownStatus;
        // Add storage information
        msgAttributes.iFlags |= RMobileSmsMessaging::KStorageLocation;
        msgAttributes.iStore = KETelIccSmsStore;
        msgAttributes.iStoreIndex = aSmsMsg->iLocation;
        }
    else
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSGSMEXT_COMPLETERECEIVEMESSAGE_3, "TSY: CMmSmsGsmExt::CompleteReceiveMessage. Class 2 message not stored in SIM");
        // Class 2 SMS message that is not stored in SIM. This message is 
        // acknowledged to the network by SMS server.
        msgAttributes.iStatus = 
            RMobileSmsMessaging::EMtMessageUnstoredPhoneAck;

        // Reset store information
        msgAttributes.iStore.SetLength( 0 );
        msgAttributes.iStoreIndex = 0;
        }

    return KErrNone;
    }

// --------------------------------------------------------------------------- 
// CMmSmsGsmExt::MessageStoreInfo 
// This method set attributes values to the store info
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsGsmExt::MessageStoreInfo(
    TAny* aGetMessageStoreInfoPtr, 
    TInt aTotalEntries, 
    TInt aUsedEntries )
    {   
    RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg* storeInfoPckg = 
        reinterpret_cast< RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg* >
        ( aGetMessageStoreInfoPtr );
    RMobileSmsStore::TMobilePhoneStoreInfoV1& storeInfo = 
        ( *storeInfoPckg )();

    storeInfo.iName = KETelIccSmsStore;
    storeInfo.iType = RMobilePhoneStore::EShortMessageStore;
    storeInfo.iTotalEntries = aTotalEntries;
    storeInfo.iUsedEntries = aUsedEntries;
    storeInfo.iCaps = KSmsMessageStoreInfoCaps;

    return KErrNone;
    }

// --------------------------------------------------------------------------- 
// CMmSmsGsmExt::StoreInfo 
// This method set attributes values to the sms store info
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsGsmExt::StoreInfo( 
    TAny* aGetInfoInfoPtr, 
    TInt aTotalEntries, 
    TInt aUsedEntries )
    {
    RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg* storeInfoPckg = 
        reinterpret_cast< RMobileSmsStore::TMobilePhoneStoreInfoV1Pckg* >
            ( aGetInfoInfoPtr );
    RMobileSmsStore::TMobilePhoneStoreInfoV1& storeInfo = 
        ( *storeInfoPckg )();

    storeInfo.iName = KETelIccSmsStore;
    storeInfo.iType = RMobilePhoneStore::EShortMessageStore;
    storeInfo.iTotalEntries = aTotalEntries;
    storeInfo.iUsedEntries = aUsedEntries;
    storeInfo.iCaps = KSmsStoreInfoCaps;
        
    return KErrNone;
    }

// --------------------------------------------------------------------------- 
// CMmSmsGsmExt::CompleteReadAllSmsPhase1L 
// This method read the list, store its content and then return 
// size of this buffer to client. This methos runs until TSY's 
// internal SMS storage is empty
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsGsmExt::CompleteReadAllSmsPhase1L(
    CArrayPtrFlat<TSmsMsg>* aSmsListArray, 
    RMobilePhone::TClientId aReadAllId, 
    CArrayPtrFlat<CListReadAllAttempt>* aSmsReadAllArray, 
    TInt* aReadAllBufSizePtr )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMSMSGSMEXT_COMPLETEREADALLSMSPHASE1L_1, "TSY: CMmSmsGsmExt::CompleteReadAllSmsPhase1L");
    // read the list, store its content and then return size of this 
    // buffer to client
    CMobilePhoneGsmSmsList* list = CMobilePhoneGsmSmsList::NewL();
    CleanupStack::PushL( list );

    RMobileSmsStore::TMobileGsmSmsEntryV1 entry;

    for ( TInt i = 0; i < aSmsListArray->Count(); i++ )
        {
        TSmsMsg* smsMsg = aSmsListArray->At( i );

        // Lets handle Service Centre address
        entry.iServiceCentre.iTypeOfNumber = smsMsg->iMobileScTON;
        entry.iServiceCentre.iNumberPlan = smsMsg->iMobileScNPI;
        entry.iServiceCentre.iTelNumber.Copy( smsMsg->iServiceCentre );

        // Lets copy location index
        entry.iIndex = smsMsg->iLocation;

        // TMobileSmsFixedEntryV1 information
        entry.iMsgData.Copy( smsMsg->iSmsMsg );

        switch ( smsMsg->iMessageStatus )
            {
            case KSimSmsMoNotSent:       //0x07
                entry.iMsgStatus = RMobileSmsStore::EStoredMessageUnsent;
                break;
            case KSimSmsStatusReportReqNotReceived:     //0x0D
            case KSimSmsMoSent:          //0x05
                entry.iMsgStatus = RMobileSmsStore::EStoredMessageSent;
                break;
            case KSimSmsMtRead:          //0x01
                entry.iMsgStatus = RMobileSmsStore::EStoredMessageRead;
                break;
            case KSimSmsMtNotRead:       //0x03
                entry.iMsgStatus = RMobileSmsStore::EStoredMessageUnread;
                break;
			case KSimSmsStatusReportReqReceivedNotStored: //0x15
			case KSimSmsStatusReportReqReceivedStored:   //0x1D
				entry.iMsgStatus = RMobileSmsStore::EStoredMessageDelivered;
                break;
            default:
                entry.iMsgStatus = 
                    RMobileSmsStore::EStoredMessageUnknownStatus;
                break;
            }

        list->AddEntryL( entry );

        // Delete handled SMS message from TSY's internal store
        delete smsMsg; // delete object
        }

    // Store the streamed list and the client ID
    CListReadAllAttempt* read = CListReadAllAttempt::NewL( &aReadAllId );
    CleanupStack::PushL( read );
    
    read->iListBuf = list->StoreLC();
    CleanupStack::Pop(); // pop the CBufFlat allocated by StoreLC
    
    aSmsReadAllArray->AppendL( read );
    CleanupStack::Pop(); // pop the CListReadAllAttempt
    
    // return the CBufFlat’s size to client
    *aReadAllBufSizePtr=( read->iListBuf )->Size();

    CleanupStack::PopAndDestroy(list);
    
    return KErrNone;
    }

// --------------------------------------------------------------------------- 
// CMmSmsGsmExt::GetSupportedEntry 
// Return supported entry
// (other items were commented in a header).
// --------------------------------------------------------------------------- 
//
TInt CMmSmsGsmExt::GetSupportedEntry()
    {
    return RMobileSmsStore::KETelMobileGsmSmsEntryV1;
    }

//  End of File 
