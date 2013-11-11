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
// Name        : CSatNotifyLocalInfo.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Local info notification functionality of SAT Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifyLocalInfoTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatTsy.h"                // Tsy class header
#include "CSatNotifyLocalInfo.h"    // Class header
#include "CBerTlv.h"                // Ber Tlv data handling
#include "TTlv.h"					// TTlv class
#include "CSatDataPackage.h"        // Parameter packing 
#include "TSatUtility.h"            // Utilities
#include "CSatTsyReqHandleStore.h"  // Request handle class
#include "cmmmessagemanagerbase.h"  // Message manager class for forwarding req.
#include "MSatTsy_IPCDefs.h"        // Sat Tsy internal request types

// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CSatNotifyLocalInfo* CSatNotifyLocalInfo::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_NEWL_1, "CSAT: CSatNotifyLocalInfo::NewL");
   	CSatNotifyLocalInfo* const satNotifyLocalInfo = 
        new ( ELeave ) CSatNotifyLocalInfo( aNotificationsTsy );
    CleanupStack::PushL( satNotifyLocalInfo );
    satNotifyLocalInfo->ConstructL();
    CleanupStack::Pop( satNotifyLocalInfo );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_NEWL_2, "CSAT: CSatNotifyLocalInfo::NewL, end of method");
    return satNotifyLocalInfo;
    }

// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::~CSatNotifyLocalInfo
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifyLocalInfo::~CSatNotifyLocalInfo
        ( 
		// None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_DTOR_1, "CSAT: CSatNotifyLocalInfo::~CSatNotifyLocalInfo");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::CSatNotifyLocalInfo
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifyLocalInfo::CSatNotifyLocalInfo
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iTimingAdvance( 0xFF ), iTimeZone( 0xFF ), 
        iMEStatus( KMEIsInTheIdleState ), iCurrentAccTech( 0xFF ), 
        iNotificationsTsy( aNotificationsTsy )
    {
	// Set time zone as not available ( 0xFF )
	// Set timing advance as not available ( 0xFF )
	// Set ME status as in the idle state ( KMEIsInTheIdleState )
	// Set current access technology as unknown ( 0xFF )  
    }

// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifyLocalInfo::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_CONSTRUCTL_1, "CSAT: CSatNotifyLocalInfo::ConstructL");
    // IMEI of the ME
    iIMEI.Zero();
    // Clear NMR (Network Measurement result)
    iNMR.Zero();
    // Clear BCCH channel list
    iBCCHChannelList.Zero();
    // Initialize location info data
    iLocalInformation.iOperatorCode.FillZ( KOperatorCodeLength );				
    iLocalInformation.iLocationAreaCode = 0;					
    iLocalInformation.iCellId = 0;
    }

// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::Notify
// This request allows a client to be notified of a LOCAL INFO proactive 
// command
// -----------------------------------------------------------------------------
//
TInt CSatNotifyLocalInfo::Notify
        (
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage    
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_NOTIFY_1, "CSAT: CSatNotifyLocalInfo::Notify");
    // Save data pointer to client side for completion
    iLocalInfoV3Pckg = reinterpret_cast<RSat::TLocalInfoV3Pckg*>( 
        aPackage.Des1n() );
    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifyLocalInfoPCmdReqType );
    // Check if requested notification is already pending
    iNotificationsTsy->NotifySatReadyForNotification( KProvideLocalInformation );   

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::CancelNotification
// This method cancels an outstanding asynchronous NotifyLocalInfo request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyLocalInfo::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_CANCELNOTIFICATION_1, "CSAT: CSatNotifyLocalInfo::CancelNotification");
    // Reset the request handle
    iNotificationsTsy->iSatReqHandleStore->ResetTsyReqHandle( 
        CSatTsy::ESatNotifyLocalInfoPCmdReqType );
    // Reset the data pointer
    iLocalInfoV3Pckg = NULL;
    // Complete the request with KErrCancel
    iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::CompleteNotifyL
// This method completes an outstanding asynchronous NotifyLocalInfo request. 
// -----------------------------------------------------------------------------
//
TInt CSatNotifyLocalInfo::CompleteNotifyL
        (
        CSatDataPackage* aDataPackage,   
        TInt aErrorCode                 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_COMPLETENOTIFYL_1, "CSAT: CSatNotifyLocalInfo::CompleteNotifyL");

    TInt ret( KErrNone );
    TPtrC8* data;
	
    // Unpack parameters
    CSatDataPackage dataPackage;
    aDataPackage->UnPackData( &data );
    // Clear additional info
    iLocalInfoRspV3.iAdditionalInfo.Zero();
    // Get ber tlv 
    CBerTlv berTlv;
    berTlv.SetData( *data );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    // Store command details tlv
    iNotificationsTsy->iTerminalRespData.iCommandDetails.Copy( 
        commandDetails.Data() );
     
    TUint8 pCmdNumber( commandDetails.GetShortInfo( ETLV_CommandNumber ) ); 
    // Store pCmdNumber
    iNotificationsTsy->iTerminalRespData.iPCmdNumber = pCmdNumber;
        
    switch ( iNotificationsTsy->iTerminalRespData.
    	iCommandDetails[KCommandQualifier] )
        {
        case RSat::KProvideLocalInfo:
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_COMPLETENOTIFYL_2, "CSAT: CSatNotifyLocalInfo::CompleteNotifyL request: LOCAL INFO");
            iLocalInfoIsOngoing = ETrue;
            // Get Local Info
            dataPackage.PackData( &iLocalInfoIsOngoing );
            iNotificationsTsy->iSatTsy->MessageManager()->
                HandleRequestL( ESatTsyProvideLocalInfo, &dataPackage );
            break;
            }
        case RSat::KProvideLocalInfoImei:
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_COMPLETENOTIFYL_3, "CSAT: CSatNotifyLocalInfo:: CompleteNotifyL, request: IMEI");
            // Check if IMEI is received
            if ( iIMEI.Length() )
                {
                iLocalInfoRspV3.iGeneralResult = RSat::KSuccess;
                }
            else
                {
                iLocalInfoRspV3.iGeneralResult = 
                    RSat::KMeUnableToProcessCmd;
                iLocalInfoRspV3.iAdditionalInfo.Append( 
                    KNoSpecificCauseCanBeGiven );
                }
            break;
            }
        case RSat::KProvideLocalInformationNmr:
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_COMPLETENOTIFYL_4, "CSAT: CSatNotifyLocalInfo:: request: NMR");
            iLocalInfoIsOngoing = ETrue;
            // Request Network Measurement Results
            dataPackage.PackData( &iLocalInfoIsOngoing );
            iNotificationsTsy->iSatTsy->MessageManager()->
                HandleRequestL( ESatTsyLocalInformationNmr, &dataPackage );
            break;
            }
        case RSat::KProvideLocalInfoDateTimeTimeZone:
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_COMPLETENOTIFYL_5, "CSAT: CSatNotifyLocalInfo:: request: DATE/TIME/TIMEZONE");
            SetDateTimeTimeZone();	
            break;
            }
        case RSat::KProvideLocalInfoLanguage:
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_COMPLETENOTIFYL_6, "CSAT: CSatNotifyLocalInfo::CompleteNotifyL request, Language");
		    // Reset req handle. Returns the deleted req handle
		    TTsyReqHandle reqHandle = 
		        iNotificationsTsy->iSatReqHandleStore->ResetTsyReqHandle( 
		        CSatTsy::ESatNotifyLocalInfoPCmdReqType );   
		                 
            if ( CSatTsy::ESatReqHandleUnknown != reqHandle )
                {
                if ( KErrNone == aErrorCode )
                    {
                    // Fill the send sm structure             
                    RSat::TLocalInfoV3& localInfoV3 = ( *iLocalInfoV3Pckg )();

                    // Get command details tlv
                    CTlv deviceIdentities;
                    berTlv.TlvByTagValue( &deviceIdentities, 
                        KTlvDeviceIdentityTag );

                    localInfoV3.iDevideId = ( RSat::TDeviceId ) 
                            deviceIdentities.GetShortInfo( 
                            ETLV_SourceDeviceIdentity );

                    localInfoV3.iInfoType = 
                        ( RSat::TLocaInfomationType) iNotificationsTsy->
                        iTerminalRespData.iCommandDetails[KCommandQualifier];

                    // Command number
                    localInfoV3.SetPCmdNumber( pCmdNumber );
                    }
                iNotificationsTsy->iSatTsy->ReqCompleted( 
                    reqHandle, aErrorCode );
                }
            else
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_COMPLETENOTIFYL_7, "CSAT: CSatNotifyLocalInfo::CompleteNotifyL No reqHandle");
                iLocalInfoRspV3.iGeneralResult = 
                    RSat::KMeUnableToProcessCmd;
                iLocalInfoRspV3.iAdditionalInfo.Append( 
                    RSat::KNoSpecificMeProblem );
                TerminalResponseL( NULL );
                }      
            break;
            }
        case RSat::KProvideLocalInfoTimingAdv:
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_COMPLETENOTIFYL_8, "CSAT: CSatNotifyLocalInfo::CompleteNotifyL request: TimingAdvance");
            iLocalInfoIsOngoing = ETrue;
            dataPackage.PackData( &iLocalInfoIsOngoing );
            // Request service
            iNotificationsTsy->iSatTsy->MessageManager()->
                HandleRequestL( ESatTsyTimingAdvance, &dataPackage );
            break;
            }
        case RSat::KProvideLocalInfoAccTech:
        	{
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_COMPLETENOTIFYL_9, "CSAT: CSatNotifyLocalInfo::CompleteNotifyL request: Access Technology");
            iLocalInfoIsOngoing = ETrue;
			dataPackage.PackData( &iLocalInfoIsOngoing );
            // Request Access technology
            iNotificationsTsy->iSatTsy->MessageManager()->
                HandleRequestL( ESatTsyAccTech, &dataPackage );                             
            break;
            }
        // Currently not supported:        
        case RSat::KProvideLocalInfoESN:
            {
            iLocalInfoRspV3.iGeneralResult = 
                RSat::KCmdBeyondMeCapabilities;
            break;
            }
        default:
            {
            iLocalInfoRspV3.iGeneralResult = RSat::KCmdTypeNotUnderstood;
            }
        }
        
    if ( ( RSat::KProvideLocalInformationNmr != iNotificationsTsy->
            iTerminalRespData.iCommandDetails[KCommandQualifier] )
            && ( RSat::KProvideLocalInfo != iNotificationsTsy->
            iTerminalRespData.iCommandDetails[KCommandQualifier] )
            && ( RSat::KProvideLocalInfoTimingAdv != iNotificationsTsy->
            iTerminalRespData.iCommandDetails[KCommandQualifier] )
            && ( RSat::KProvideLocalInfoLanguage != iNotificationsTsy->
            iTerminalRespData.iCommandDetails[KCommandQualifier] )
        	&& ( RSat::KProvideLocalInfoAccTech != iNotificationsTsy->
            iTerminalRespData.iCommandDetails[KCommandQualifier] ) ) 
        {
        // Send terminal response to the NAA   
        TerminalResponseL( NULL );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::TerminalResponseL
// Called by ETel server, passes terminal response to DOS
// -----------------------------------------------------------------------------
//
TInt CSatNotifyLocalInfo::TerminalResponseL
        ( 
        TDes8* aRsp
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_TERMINALRESPONSEL_1, "CSAT: CSatNotifyLocalInfo::TerminalResponseL");

    TInt ret( KErrNone );
    TBuf<RSat::KAdditionalInfoMaxSize> additionalInfo;    
    if ( aRsp )
        {
        RSat::TLocalInfoRspV3Pckg* aRspPckg = 
                reinterpret_cast<RSat::TLocalInfoRspV3Pckg*>( aRsp );
        RSat::TLocalInfoRspV3& rspV1 = ( *aRspPckg ) ();
        // Get Proactive command number
        iNotificationsTsy->iTerminalRespData.iPCmdNumber = rspV1.PCmdNumber();
        // Check that general result value is valid.
        if ( ( RSat::KSuccess != rspV1.iGeneralResult ) 
            && ( RSat::KSuccessLimitedService != rspV1.iGeneralResult )
            && ( RSat::KMeUnableToProcessCmd != rspV1.iGeneralResult )
            && ( RSat::KCmdBeyondMeCapabilities != rspV1.iGeneralResult ) )
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_TERMINALRESPONSEL_2, "CSAT: CSatNotifyLocalInfo::TerminalResponseL, Invalid General Result");
            // Invalid general result
            ret = KErrCorrupt;
            }
		else
			{
			// Do nothing
			}
        iLocalInfoRspV3.iGeneralResult = rspV1.iGeneralResult;

        // If there is ME (Mobile Entity) error, additional info is needed
        if ( ( RSat::KMeProblem == rspV1.iInfoType ) &&
             ( NULL == rspV1.iAdditionalInfo.Length() ) )
            {   
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_TERMINALRESPONSEL_3, "CSAT: CSatNotifyLocalInfo::TerminalResponseL, Invalid Additional Info");
            // Invalid additional info field
            ret = KErrCorrupt;
            }
		else
			{
			// Do nothing
			}
			
        iLocalInfoRspV3.iAdditionalInfo = rspV1.iAdditionalInfo;
        }
	else
		{
		// Do nothing
		}   
		
    additionalInfo.Copy( iLocalInfoRspV3.iAdditionalInfo ); 
    CreateTerminalRespL( iNotificationsTsy->iTerminalRespData.iPCmdNumber, 
                        static_cast<TUint8>( iLocalInfoRspV3.iGeneralResult ),
                        additionalInfo );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::CreateTerminalRespL
// Constructs LocalInfo specific part of terminal response and calls 
// DOS to send the actual message.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyLocalInfo::CreateTerminalRespL
        ( 
        TUint8 aPCmdNumber,         
        TUint8 aGeneralResult,     
        TDesC16& aAdditionalInfo		
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_CREATETERMINALRESPL_1, "CSAT: CSatNotifyLocalInfo::CreateTerminalRespL");
    // Create and append response data
    TTlv tlvSpecificData;
    // Create General Result TLV 
    tlvSpecificData.AddTag( KTlvResultTag ); 
    // General result
    tlvSpecificData.AddByte( aGeneralResult );
    
    if ( ( RSat::KSuccess != aGeneralResult ) && 
         ( RSat::KSuccessLimitedService != aGeneralResult ) )    
        {
        if ( aAdditionalInfo.Length() )
            {
            tlvSpecificData.AddByte( static_cast<TUint8>( 
                ( aAdditionalInfo[0] >> 0 ) & 0xFF ) );
            }
        }
    else
        {        
        switch ( iNotificationsTsy->iTerminalRespData.
        			iCommandDetails[KCommandQualifier] )
            {      
            case RSat::KProvideLocalInfo:
                {  
                // Create Location Information TLV
                tlvSpecificData.AddTag( KTlvLocationInformationTag );
                // Append Mobile Country  & Network Codes, 3 bytes
                tlvSpecificData.AddData( iLocalInformation.iOperatorCode );
                // Append location area code
                tlvSpecificData.AddByte( static_cast<TUint8>(
                	iLocalInformation.iLocationAreaCode>>8 ) );
                tlvSpecificData.AddByte( static_cast<TUint8>(
                	iLocalInformation.iLocationAreaCode>>0 ) );
                // Append Cell Id
                tlvSpecificData.AddByte( static_cast<TUint8>(
                	iLocalInformation.iCellId >> 8 ) );
                tlvSpecificData.AddByte( static_cast<TUint8>(
                	iLocalInformation.iCellId >> 0 ) );
                break;
                }
            case RSat::KProvideLocalInfoImei:
                {                 
                // Add tlv tag
                tlvSpecificData.AddTag( KTlvImeiTag );
                TBuf8<8> bcdImei;
                // Convert IMEI to BCD format
                _LIT8( KTen, "*" );
                _LIT8( KZero, "0" );

                // Check if required changes is allready done for the IMEI
                if ( '*' != iIMEI[0] )
                    {
                    // Processing BCD string is much harder than ASCII so 
                    // required changes is done before BCD conversion.
                    // (lot's of shifting etc..)
                    // Insert '*'(0x2A) as a first character which is converted
                    // to 0xA in AsciiToBCD method.
                    // Type of identity for IMEI code is 0xA "1010b"
                    // See 3GPP TS 04.08 
                    iIMEI.Insert( 0, KTen );

                    // Spare digit: this digit shall be zero, when transmitted 
                    // by the Mobile Station.
                    // See GSM 03.03
                    iIMEI.Replace( iIMEI.Length() - 1, 1 , KZero );
                    }

                TSatUtility::AsciiToBCD( iIMEI, bcdImei );
                // Add IMEI code to the terminal response 
                tlvSpecificData.AddData( bcdImei );
                break;
                }
            case RSat::KProvideLocalInformationNmr:
                {
                SetInformationNmr( &tlvSpecificData );				                
                break;
                }
            case RSat::KProvideLocalInfoDateTimeTimeZone:
                {
                tlvSpecificData.AddTag( KTlvDateTimeAndTimeZoneTag );
                // Append current date/time/timezone to the terminal response
                TInt i( 0 );
                for ( i = 0; i < aAdditionalInfo.Length(); i++ )
                    {
                    tlvSpecificData.AddByte( static_cast<TUint8>( aAdditionalInfo[i] 
                                                        & 0x00FF ) );
                    }
                break; 
                }
            case RSat::KProvideLocalInfoTimingAdv:
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_CREATETERMINALRESPL_2, "CSAT: CSatNotifyLocalInfo::CreateTerminalRespL, TimingAdvance");
                // Timing advance result
                tlvSpecificData.AddTag( KTlvTimingAdvanceTag );
                tlvSpecificData.AddByte( iMEStatus );
                tlvSpecificData.AddByte( iTimingAdvance );
                break;
                }
            case RSat::KProvideLocalInfoLanguage:
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_CREATETERMINALRESPL_3, "CSAT: CSatNotifyLocalInfo::CreateTerminalRespL, LocalInfoLanguage");
                
                if ( aAdditionalInfo.Length() )
                    {
                    // create Language TLV
                    tlvSpecificData.AddTag( KTlvLanguageTag );
                    TBuf8<2> character;
                    TSatUtility::Copy16to8LE( aAdditionalInfo, character);
                    tlvSpecificData.AddData( character );
                    }
                else
                	{
                	// Do nothing
                	}
                break; 
                }
            case RSat::KProvideLocalInfoAccTech:
            	{
            	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_CREATETERMINALRESPL_4, "CSAT: CSatNotifyLocalInfo::CreateTerminalRespL, Access Technology");
            	// Access technology result
                tlvSpecificData.AddTag( KTlvAccessTechnologyTag );
                tlvSpecificData.AddByte( iCurrentAccTech );
            	break;
            	}                
            default:
                {
                break;
                }
            }
        }
    // Prepare data
    iNotificationsTsy->iTerminalRespData.iPCmdNumber = aPCmdNumber;
    TPtrC8 data = tlvSpecificData.GetDataWithoutTopLevelTag();

    // Pack data
    CSatDataPackage dataPackage;
	dataPackage.PackData( &iNotificationsTsy->iTerminalRespData, &data );

    // Forward request to the DOS
    return iNotificationsTsy->iSatTsy->MessageManager()->HandleRequestL( 
		ESatTerminalRsp, &dataPackage );
    }

// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::CompleteTerminalRespDataL
// Unpacks terminal response specific data
// calls CreateTerminalRespL.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyLocalInfo::CompleteTerminalRespDataL
        (
        CSatDataPackage* aDataPackage,
        TInt /*aErrorCode*/
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_COMPLETETERMINALRESPDATAL_1, "CSAT: CSatNotifyLocalInfo::CompleteTerminalRespDataL");
    TInt ret( KErrNone );
    // Unpack parameters
    CSatNotificationsTsy::TTerminalRespData* terminalRespData;
    aDataPackage->UnPackData( &terminalRespData );    
    CreateTerminalRespL( iNotificationsTsy->iTerminalRespData.iPCmdNumber,
                         terminalRespData->iGeneralResult,
                         terminalRespData->iAdditionalInfo );
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::CompleteUpdateLocalInfo
// Called when local info is changed
// -----------------------------------------------------------------------------
//
void CSatNotifyLocalInfo::CompleteUpdateLocalInfo
		( 
		CSatDataPackage* aDataPackage, 
		TInt /*aErrorCode*/ 
		)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_COMPLETEUPDATELOCALINFO_1, "CSAT: CSatNotifyLocalInfo::CompleteUpdateLocalInfo");
	aDataPackage->UnPackData( iLocalInformation );	
	}
	
// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::CompleteUpdateNetInfo
// Called when NMR or BCCHChannel list is changed
// -----------------------------------------------------------------------------
//	
void CSatNotifyLocalInfo::CompleteUpdateNetInfo
		( 
		CSatDataPackage* aDataPackage, 
        TInt /*aErrorCode*/ 
        )
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_COMPLETEUPDATENETINFO_1, "CSAT: CSatNotifyLocalInfo::CompleteUpdateNetInfo");
	aDataPackage->UnPackData( iNMR, iBCCHChannelList );	
	}
	
// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::CompleteUpdateTimingAdvAndMEStatus
// Called when Timing addvance or ME Status is changed
// -----------------------------------------------------------------------------
//	               		
void CSatNotifyLocalInfo::CompleteUpdateTimingAdvAndMEStatus
		(
		CSatDataPackage* aDataPackage, 
        TInt /*aErrorCode*/ 
        )
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_COMPLETEUPDATETIMINGADVANDMESTATUS_1, "CSAT: CSatNotifyLocalInfo::CompleteUpdateTimingAdvAndMEStatus");
	aDataPackage->UnPackData( iTimingAdvance, iMEStatus );		
	}
	
// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::CompleteUpdateAccTech
// Called when Access technology is changed
// -----------------------------------------------------------------------------
//	               		
void CSatNotifyLocalInfo::CompleteUpdateAccTech
		( 
		CSatDataPackage* aDataPackage, 
		TInt /*aErrorCode*/ 
		)      
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_COMPLETEUPDATEACCTECH_1, "CSAT: CSatNotifyLocalInfo::CompleteUpdateAccTech");
	aDataPackage->UnPackData( iCurrentAccTech );	
	}
// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::CompleteUpdateIMEI
// Called when IMEI is changed ( in boot up )
// -----------------------------------------------------------------------------
//	               		
void CSatNotifyLocalInfo::CompleteUpdateIMEI
		( 
		CSatDataPackage* aDataPackage, 
		TInt /*aErrorCode*/ 
		)      
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_COMPLETEUPDATEIMEI_1, "CSAT: CSatNotifyLocalInfo::CompleteUpdateIMEI");
	aDataPackage->UnPackData( iIMEI );	
	}

// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::CompleteUpdateTimeZone
// Called when Time zone is changed
// -----------------------------------------------------------------------------
//	               		
void CSatNotifyLocalInfo::CompleteUpdateTimeZone
		( 
		CSatDataPackage* aDataPackage, 
		TInt /*aErrorCode*/ 
		)      
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_COMPLETEUPDATETIMEZONE_1, "CSAT: CSatNotifyLocalInfo::CompleteUpdateTimeZone");
	aDataPackage->UnPackData( iTimeZone );	
	}
	
// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::LocalInformation
// Returns const reference to the up-to-date local information data
// -----------------------------------------------------------------------------
//
const CSatNotificationsTsy::TLocalInformation& 
    CSatNotifyLocalInfo::LocalInformation
		( 
		// None
		)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFICATIONSTSY_TLOCALINFORMATION_1, "CSAT: CSatNotifyLocalInfo::LocalInformation");
	return iLocalInformation;
	}
	
// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::SetDateTimeTimeZone
// Sets Date, Time and TimeZone into local info structure
// -----------------------------------------------------------------------------
//			
void CSatNotifyLocalInfo::SetDateTimeTimeZone
		(
		// None
		)
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_SETDATETIMETIMEZONE_1, "CSAT: CSatNotifyLocalInfo::SetDateTimeTimeZone");
    TTime time;
    TLocale locale;
    TDateTime dateTime;
    // Get Universal time
    time.UniversalTime();
    // Get Universal time offset ( Time zone in seconds )
    TTimeIntervalSeconds universalTimeOffset( 
        locale.UniversalTimeOffset() );
    // Add locale's universal time offset to universal time
    // to get the local time
    time += universalTimeOffset;
    // Converts a TTime object into a TDateTime object
    dateTime = time.DateTime();
    // Get and conver year
    TUint8 temp( TSatUtility::ConvertToSemiOctet(
        dateTime.Year() ) );
    // Append year to additional info
    iLocalInfoRspV3.iAdditionalInfo.Append( temp );                        
    // Convert month
    // Offset from zero. So adding one.
    temp = TSatUtility::ConvertToSemiOctet( 
        dateTime.Month() + 1 );
    // Append month value to the additional info
    iLocalInfoRspV3.iAdditionalInfo.Append( temp );
    // Convert day to the semi-octec presentation
    // The day.Offset from zero, so add one before displaying
    // the day number.
    temp = TSatUtility::ConvertToSemiOctet( 
        dateTime.Day() + 1 );
    // Append the day value to the additional info
    iLocalInfoRspV3.iAdditionalInfo.Append( temp );
    // Conver current time to the semi-octec presentation.
    // First hour
    temp = TSatUtility::ConvertToSemiOctet(
        dateTime.Hour() );
    // Append the hour value to the additional info
    iLocalInfoRspV3.iAdditionalInfo.Append( temp );
    // Minute value
    temp = TSatUtility::ConvertToSemiOctet(
        dateTime.Minute() );
    // Append the minute value to the additional info
    iLocalInfoRspV3.iAdditionalInfo.Append( temp );
    // Seconds
    temp = TSatUtility::ConvertToSemiOctet(
        dateTime.Second() );
    // Append the seconds value to the additional info
    iLocalInfoRspV3.iAdditionalInfo.Append( temp );
    // Append time zone
    iLocalInfoRspV3.iAdditionalInfo.Append( iTimeZone );             
    // Set generall result as Success
    iLocalInfoRspV3.iGeneralResult = RSat::KSuccess;	
	}

// -----------------------------------------------------------------------------
// CSatNotifyLocalInfo::SetInformationNmr
// Sets NMR and BCCHChannelList to tlv data
// -----------------------------------------------------------------------------
//
void CSatNotifyLocalInfo::SetInformationNmr
		(
		TTlv* aTlvSpecificData
		)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLOCALINFO_SETINFORMATIONNMR_1, "CSAT: CSatNotifyLocalInfo::SetInformationNmr");
    if ( iNMR.Length() )
        {
        // Network measurement result
        aTlvSpecificData->AddTag( KTlvNetworkMeasurementResultsTag );
        aTlvSpecificData->AddData( iNMR );
        }

    if ( iBCCHChannelList.Length() )
        {
        // Append BCCH channel list tag
        aTlvSpecificData->AddTag( KTlvBccdChannelListTag );

        TInt i( 0 );
        TInt mask( 3 );
        TInt shiftMove( 2 );
        TUint8 nextChar( 0 );
      
        // Convert BCCH channel list from 16-bit to 10-bit format
        for ( i = 0; i < iBCCHChannelList.Length(); i++ )
            {
            TUint char2 =  ( iBCCHChannelList[i] & 0x03FF );
            TUint8 char1 = static_cast<TUint8>( char2 >> shiftMove );

            // Append byte
            aTlvSpecificData->AddByte( 
                static_cast<TUint8>( char1 | nextChar ) );
            
            nextChar = static_cast<TUint8>( ( char2 & mask )  
                << ( 8 - shiftMove ) );

            if ( 8 == shiftMove )
                {
                shiftMove = 2;
                nextChar = 0;
                aTlvSpecificData->AddByte( 
                    static_cast<TUint8> ( char2 & 0x00FF ) );
                mask = 3;
                }
            else
                {
                shiftMove += 2;
                mask = ( 1L << shiftMove ) - 1;
                }
            }
        // Add last byte
        aTlvSpecificData->AddByte( nextChar );
        }
    // Clear nmr
    iNMR.Zero();
    // Clear BCCH channel list
    iBCCHChannelList.Zero();	
	}
			
			
// End of file

