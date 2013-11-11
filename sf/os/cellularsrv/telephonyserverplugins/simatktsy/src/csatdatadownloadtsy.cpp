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
// Name        : CSatDataDownloadTsy.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// DataDownload-related functionality of Sat Tsy
// Version     : 1.0
//




#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "csatdatadownloadtsyTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include <etelmm.h> 				// Etel MM Definitions
#include "CSatDataPackage.h"        // Parameter packing 
#include "CSatDataDownloadTsy.h"    // Class header
#include "CSatTsy.h"                // Sat Tsy class
#include "TSatUtility.h"            // Utilities
#include "cmmmessagemanagerbase.h"  // Message manager class for forwarding req.
#include "CBerTlv.h"                // Ber Tlv
#include "TTlv.h"					// TTlv class
#include "MSatTsy_IPCDefs.h"        // Sat Tsy internal request types

// -----------------------------------------------------------------------------
// CSatDataDownloadTsy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSatDataDownloadTsy* CSatDataDownloadTsy::NewL
        (    
        CSatTsy* aSatTsy  
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATDATADOWNLOADTSY_NEWL_1,  "CSAT: CSatDataDownloadTsy::NewL" );
    CSatDataDownloadTsy* const satDataDownloadTsy = 
        new ( ELeave ) CSatDataDownloadTsy();
    CleanupStack::PushL( satDataDownloadTsy );
    satDataDownloadTsy->iSatTsy = aSatTsy;
    satDataDownloadTsy->ConstructL();
    CleanupStack::Pop( satDataDownloadTsy );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATDATADOWNLOADTSY_NEWL_2,  "CSAT: CSatDataDownloadTsy::NewL, end of method" );
    return satDataDownloadTsy; 
    }

// -----------------------------------------------------------------------------
// CSatDataDownloadTsy::~CSatDataDownloadTsy
// Destructor
// -----------------------------------------------------------------------------
//    
CSatDataDownloadTsy::~CSatDataDownloadTsy
        (    
        void   
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATDATADOWNLOADTSY_DTOR_1,  "CSAT: CSatDataDownloadTsy::~CSatDataDownloadTsy" );
    // Unregister
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatDataDownloadTsyObjType, NULL );
    }

// -----------------------------------------------------------------------------
// CSatDataDownloadTsy::CSatDataDownloadTsy
// C++ constructor
// -----------------------------------------------------------------------------
//
CSatDataDownloadTsy::CSatDataDownloadTsy
        (
        void
        )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatDataDownloadTsy::ConstructL
// Symbian 2nd phase constructor. Initialises internal attributes.
// -----------------------------------------------------------------------------
//
void CSatDataDownloadTsy::ConstructL
        (    
        void
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATDATADOWNLOADTSY_CONSTRUCTL_1,  "CSAT: CSatDataDownloadTsy::ConstructL" );
    // Register
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatDataDownloadTsyObjType, this );
	// Initialize as not supported
	iIsSmsPpDdlSupported = EFalse;
    }

// -----------------------------------------------------------------------------
// CSatDataDownloadTsy::CompleteCellBroadcastDdlL
// Prepare the envelope to be sent to DOS after a Cell
// Broadcast message has been received, without request, by the mobile equipment.
// -----------------------------------------------------------------------------
//
void CSatDataDownloadTsy::CompleteCellBroadcastDdlL
         (
         const CSatDataPackage* aDataPackage
         )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATDATADOWNLOADTSY_COMPLETECELLBROADCASTDDLL_1,  "CSAT:CSatDataDownloadTsy::CompleteCellBroadcastDdlL" );

    TBuf8<KCbsMsgMaxLength> aPdu;
    aDataPackage->UnPackData( aPdu );     
    TTlv envelope;
    // Tag
    envelope.Begin( KBerTlvCellBroadcastTag );
    // Device identities
    envelope.AddTag( KTlvDeviceIdentityTag );
    envelope.AddByte( KNetwork );
    envelope.AddByte( KSim );    
    // Cell broadcast page
    envelope.AddTag( KTlvCellBroadcastPageTag );
    envelope.AddData( aPdu );  
    // Prepare data
    TPtrC8 data = envelope.End();
    // Pack data
    CSatDataPackage dataPackage;
	dataPackage.PackData( &data );
    // Forward request to the DOS
    iSatTsy->MessageManager()->HandleRequestL( 
		    ESatTsyCellBroadcast, &dataPackage ) ;        
    }

// -----------------------------------------------------------------------------
// CSatDataDownloadTsy::CompleteSmsPpDdlL
// Completes the SMS-PP by sending the envelope if SMS-PP is supported. 
// Otherwise requests saving of the SMS to EFsms (only class 2 sms)
// 3GPP TS 11.14, 7 Data download to SIM: If the service "data download via SMS 
// Point-to-point" is allocated and activated in the SIM Service Table, the ME 
// shall pass the message transparently to the SIM using the ENVELOPE. If the 
// service "data download via SMS-PP" is not allocated and activated in the SIM
// Service Table, and the ME receives a Short Message with the protocol 
// identifier = SIM data download and data coding scheme = class 2 message,
// then the ME shall store the message in EFSMS in accordance with TS 11.11.
// -----------------------------------------------------------------------------
//
void CSatDataDownloadTsy::CompleteSmsPpDdlL
        (
        const CSatDataPackage* aDataPackage
        )
    {  
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATDATADOWNLOADTSY_COMPLETESMSPPDDLL_1,  "CSAT:CSatDataDownloadTsy::CompleteSmsPpDdlL" );
    TBuf8<KAddrMaxLength> smsScAddress;
    TBuf8<RMobileSmsMessaging::KGsmTpduSize> smsTpdu;
    
    // Unpack data
    aDataPackage->UnPackData( smsScAddress, smsTpdu );
    
    // Check if SMS PP Data Download is supported in NAA, then send envelope
    if ( iIsSmsPpDdlSupported )
        {
        CreateSmsPpDdlEnvelopeL( smsScAddress, smsTpdu );
        }
    // Otherwise parse TPDU data and check if SMS must be saved
    else 
        {       
        // Check if the SMS should be saved to EFsms (see method descr.)
        if ( KErrNone == ParseSmsTpdu( smsTpdu ) &&
             KSmsTpduProtcolIdUSimDdl == iSmsTpdu.iProtocolId && 
            ( !( iSmsTpdu.iDcs & 0x01 ) ) && ( iSmsTpdu.iDcs & 0x02 ) )
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATDATADOWNLOADTSY_COMPLETESMSPPDDLL_2,  "CSAT:CSatDataDownloadTsy::CompleteSmsPpDdlL, SMS PP DDL is not supported, Store SMS to EFsms" );
            CreateEntryForSavingSmsL( smsScAddress, smsTpdu );
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CSatDataDownloadTsy::CreateEntryForSavingSmsL
// Creates the TMobileGsmSmsEntryV1Pckg and makes a request to DOS to save the 
// SMS to EFsms
// -----------------------------------------------------------------------------
//
void CSatDataDownloadTsy::CreateEntryForSavingSmsL
        (
        const TDesC8& aSmsScAddress,
        const TDesC8& aSmsTpdu
        )
    {  
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATDATADOWNLOADTSY_CREATEENTRYFORSAVINGSMSL_1,  "CSAT:CSatDataDownloadTsy::CreateEntryForSavingSms SC Addr. length: %d", aSmsScAddress.Length() );
    TInt offset = 0;
    
    // Check that the SC address length contains the length in
    // addition to the address.
    if ( ( aSmsScAddress[offset] + 1 ) == aSmsScAddress.Length() )
        {
        RMobileSmsStore::TMobileGsmSmsEntryV1 smsEntry;
        RMobileSmsStore::TMobileGsmSmsEntryV1Pckg smsEntryPckg( smsEntry ); 
            
        // Set the Type of number: bits 6,5,4
        smsEntry.iServiceCentre.iTypeOfNumber = 
            static_cast<RMobilePhone::TMobileTON>( 
            ( aSmsScAddress[++offset] >> 4 ) & 0x07 );
        // Set the Numbering plan identification: bits 3,2,1,0
        smsEntry.iServiceCentre.iNumberPlan =
            static_cast<RMobilePhone::TMobileNPI>(
            aSmsScAddress[offset] & 0x0F );
        
        // Set data
        TBuf8<RSat::KMaxMobileTelNumberSize> scAddress;
        TBuf16<RSat::KMaxMobileTelNumberSize> scAddress16;
        
        // First convert the number to ASCII
        TSatUtility::BCDToAscii( aSmsScAddress.Mid( ++offset ), scAddress );
                
        // Then set the 8 bit data to 16 bit data buffer                  
        for ( TInt i = 0; i < scAddress.Length(); i++ )
            {
            scAddress16.Append( static_cast<TUint16>( 
                scAddress[i] ) );
            }

        // Copy the Service Centre address and TPDU data
        smsEntry.iServiceCentre.iTelNumber.Copy( scAddress16 );
                
        OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATDATADOWNLOADTSY_CREATEENTRYFORSAVINGSMSL_2, "CSAT:CSatDataDownloadTsy::CreateEntryForSavingSms, iTelNumber: %S", smsEntry.iServiceCentre.iTelNumber );
            
        smsEntry.iMsgData.Copy( aSmsTpdu ); 
        
        // We set the index to point -1, since it means we use the 1st free 
        // slot. See Etel MM API Specification, 8.1.4 Write Entry. 
        smsEntry.iIndex = -1;

        // Pack data and forward request to DOS
        TDesC8* data = reinterpret_cast<TDesC8*>( &smsEntryPckg );
        CMmDataPackage package;
        package.PackData( &data, &smsEntry.iIndex );
        iSatTsy->MessageManager()->HandleRequestL( EMobilePhoneStoreWrite, 
            &package ); 
        }
    }
    
// -----------------------------------------------------------------------------
// CSatDataDownloadTsy::CreateSmsDeliverReportL
// Completes the writing of the SMS to EFsms by creating the resulting report
// (see 3GPP TS 23.040, 9.2.2.1a). See also 3GPP TS 23.038 "When a mobile 
// terminated message is Class 2 ((U)SIM-specific), an MS shall ensure that 
// the message has been transferred to the SMS data field in the (U)SIM before 
// sending an acknowledgement to the SC"
// -----------------------------------------------------------------------------
//
void CSatDataDownloadTsy::CreateSmsDeliverReportL
        (
        TInt aResult
        )  
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATDATADOWNLOADTSY_CREATESMSDELIVERREPORTL_1, "CSAT: CSatDataDownloadTsy::CreateSmsDeliverReportL");
    TBuf8<KTpduMaxSize> reportData;
       
    // Get User Data length
    TUint8 userDataLength = iSmsTpdu.iUserData.Length();
    
    // Check that SMS TPDU data exists by checking the last mandatory item
    if ( KSmsTpduByteUnknownOrReserved != iSmsTpdu.iDcs )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATDATADOWNLOADTSY_CREATESMSDELIVERREPORTL_2, "CSAT: CSatDataDownloadTsy::CreateSmsDeliverReportL, SMS TPDU OK");
        reportData.Zero();
        reportData.Append( iSmsTpdu.iParameters );   // 1st byte -> parameters
        reportData.Append( KAllOptParamsPresent );   // TP-Parameter-Indicator
        reportData.Append( iSmsTpdu.iProtocolId );   // TP-Protocol-Identifier
        reportData.Append( iSmsTpdu.iDcs );          // TP-Data-Coding-Scheme
        reportData.Append( userDataLength );         // TP-User-Data-Length
        
        // If user data exists, add it to the end of buffer
        if ( userDataLength )
            {
            reportData.Append( iSmsTpdu.iUserData );
            }
        
        // If saving went OK it means that RP-ACK must to be sent and data is
        // ready, otherwise RP-ERROR must to be sent, which means that TP Failure 
        // Cause must be inserted after the parameters
        if ( KErrNone != aResult )
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATDATADOWNLOADTSY_CREATESMSDELIVERREPORTL_3, "CSAT: CSatDataDownloadTsy::CreateSmsDeliverReportL, Add TP Failure Cause: KDataDownloadError");
            TBuf8<1> failureCause;
            failureCause.Zero();
            failureCause.Append( KDataDownloadError );
            reportData.Insert( 1, failureCause );
            }
        
        // Pack data and forward request to DOS
        CMmDataPackage package;
        package.PackData( &reportData );
        iSatTsy->MessageManager()->HandleRequestL( ESatTsySmsDeliverReport, 
            &package );
        }
    
    }

    
// -----------------------------------------------------------------------------
// CSatDataDownloadTsy::CreateSmsPpDdlEnvelopeL
// Prepare the envelope to be sent to DOS after a SMS-PP
// message has been received, without request, by the mobile equipment.
// -----------------------------------------------------------------------------
//
void CSatDataDownloadTsy::CreateSmsPpDdlEnvelopeL
        (
        const TDesC8& aSmsScAddress,
        const TDesC8& aSmsTpdu
        )
    {  
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATDATADOWNLOADTSY_CREATESMSPPDDLENVELOPEL_1,  "CSAT:CSatDataDownloadTsy::CreateSmsPpDdlEnvelopeL" );
    // Check that the data exists
    if ( aSmsScAddress.Length() && aSmsTpdu.Length() )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATDATADOWNLOADTSY_CREATESMSPPDDLENVELOPEL_2,  "CSAT:CSatDataDownloadTsy::CreateSmsPpDdlEnvelopeL, Data Ok" );
        TTlv envelope;
 
        envelope.Begin( KBerTlvSmsPpDownloadTag );
        envelope.AddTag( KTlvDeviceIdentityTag );
        envelope.AddByte( KNetwork );
        envelope.AddByte( KSim );
        envelope.AddTag( KTlvAddressTag );
        envelope.AddData( aSmsScAddress.Mid( 1, aSmsScAddress[0] ) );
        envelope.AddTag( KTlvSmsTpduTag );
        envelope.AddData( aSmsTpdu );
     
        // Prepare data
        TPtrC8 data = envelope.End();
        // Pack data
        CSatDataPackage dataPackage;
    	dataPackage.PackData( &data );
        // Forward request to the DOS
        iSatTsy->MessageManager()->HandleRequestL( ESatTsySmsPpDdl, 
            &dataPackage );
        }
    }

// -----------------------------------------------------------------------------
// CSatDataDownloadTsy::CompleteReadCbmidsL
// Request to read Cbmids from the elementary file EFcbmids.
// -----------------------------------------------------------------------------
//
void CSatDataDownloadTsy::CompleteReadCbmidsL
        (
        void 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATDATADOWNLOADTSY_COMPLETEREADCBMIDSL_1,  "CSAT: CSatDataDownloadTsy::CompleteReadCbmids" );
    // Convert constants and append to simFilePath
    TBuf8<KMaxFilePath> simFilePath;
    // Append data
    simFilePath.Append( TUint8( KMasterFileCbmids >> 8 ) );
    //lint -e{778} Constant expression evaluates to 0 in operation 
	simFilePath.Append( TUint8( KMasterFileCbmids & 0xFF ) );

	simFilePath.Append( TUint8( KDedicatedFileCbmids >> 8 ) );
	simFilePath.Append( TUint8( KDedicatedFileCbmids & 0xFF ) );

	simFilePath.Append( TUint8( KElementaryFileCbmids >> 8 ) );
	simFilePath.Append( TUint8( KElementaryFileCbmids & 0xFF ) );

    // Pack data
    CSatDataPackage dataPackage;
	dataPackage.PackData( &simFilePath );
    // Forward request to the DOS
    iSatTsy->MessageManager()->HandleRequestL( 
		    ESatTsyReadCbmids, &dataPackage );          
    }    
    
// -----------------------------------------------------------------------------
// CSatDataDownloadTsy::SetSmsPpDdlStatus
// Sets the support status of SMS PP Data Download in NAA
// -----------------------------------------------------------------------------
//
void CSatDataDownloadTsy::SetSmsPpDdlStatus
        (
        const CSatDataPackage* aDataPackage 
        )
    {
    aDataPackage->UnPackData( iIsSmsPpDdlSupported );   
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATDATADOWNLOADTSY_SETSMSPPDDLSTATUS_1, "CSAT: CSatNotifyMoSmControlRequest::SetActivationStatus, %d",iIsSmsPpDdlSupported );
    }  
    
// -----------------------------------------------------------------------------
// CSatDataDownloadTsy::ParseSmsTpdu
// Interprets the given TPDU data and sets the results to internal data member
// -----------------------------------------------------------------------------
//
TInt CSatDataDownloadTsy::ParseSmsTpdu
        (
        const TDesC8& aSmsTpdu
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATDATADOWNLOADTSY_PARSESMSTPDU_1, "CSAT: CSatDataDownloadTsy::ParseSmsTpdu");
    TInt ret( KErrCorrupt );
    
    // Initialize values
    iSmsTpdu.iParameters = KSmsTpduByteUnknownOrReserved;
    iSmsTpdu.iProtocolId = KSmsTpduByteUnknownOrReserved;
    iSmsTpdu.iDcs = KSmsTpduByteUnknownOrReserved;
    iSmsTpdu.iUserData.Zero();
    
     // Check first that the TPdu data and SC address exist before parsing
    if ( KSmsTpduMinLength < aSmsTpdu.Length() )
        {
        // Set parameters
        iSmsTpdu.iParameters = aSmsTpdu[0];
        // Then we need to get Protocol ID and DCS..
        // ..the length of address in chars is in the 1st byte of Addr. string
        TUint8 chars = aSmsTpdu[KSmsTpduAddrOffset];
        TUint8 addrLengthInBytes( 0 ); 
            
        if ( chars % 2 )
            {
            addrLengthInBytes = chars / 2 + 1;
            }
        else
            {
            addrLengthInBytes = chars / 2;
            }
            
        // Then add two in order to point to the protocol ID (the order is:
        // address length, type of address, address data, protocol id...
        TUint8 offset = KSmsTpduAddrOffset + 2 + addrLengthInBytes;

        // Just to be on the safe side, check that the offset is not too big
        if ( aSmsTpdu.Length() > ( offset + 1 + KSmsTpduSctsLength ) )
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATDATADOWNLOADTSY_PARSESMSTPDU_2,  "CSAT:CSatDataDownloadTsy::ParseSmsTpdu, SmsTpdu length Ok" );
            // Set protocol id and data coding shceme
            iSmsTpdu.iProtocolId = aSmsTpdu[offset];
            iSmsTpdu.iDcs = aSmsTpdu[++offset] ;
            
            // Update offset to point the TP-UDL (skip TP-SCTS, not needed)
            offset = offset + KSmsTpduSctsLength + 1;
            TUint8 tpduUserDataLength = aSmsTpdu[offset];
                
            // If user data exists, add it to the end of buffer
            if ( tpduUserDataLength )
                {
                iSmsTpdu.iUserData.Copy( aSmsTpdu.Mid( ++offset ) );
                OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATDATADOWNLOADTSY_PARSESMSTPDU_3,  "CSAT:CSatDataDownloadTsy::ParseSmsTpdu, iUserData length: %d", iSmsTpdu.iUserData.Length() );
                }

            ret = KErrNone;
            } 
        } // TPDU reaches the minimum length  
        
    return ret;
    }     

// End of file

