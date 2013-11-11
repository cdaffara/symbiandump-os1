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
// Name        : CSatCcTsy.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Call control-related functionality of Sat Tsy
// Version     : 1.0
//




//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "csatcctsyTraces.h"
#endif

#include "etelsat.h"                // SAT specific Etel definitions
#include "CSatDataPackage.h"        // Parameter packing 
#include "CSatCCTsy.h"              // Class header
#include "CSatTsy.h"                // SAT TSY general class
#include "CSatNotificationsTsy.h"   // SAT TSY Notifications class
#include "cmmmessagemanagerbase.h"  // Message manager class for forwarding req.
#include "msattsy_ipcdefs.h"		// Sat Tsy specific request types
#include "TSatUtility.h"			// Utility class
#include "TTlv.h"					// TTlv class


// -----------------------------------------------------------------------------
// CSatCCTsy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSatCCTsy* CSatCCTsy::NewL
        (    
        CSatTsy* aSatTsy,
        CSatNotificationsTsy* aNotificationsTsy   
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_NEWL_1, "CSAT: CSatCCTsy::NewL");
    CSatCCTsy* const satCcTsy = new ( ELeave ) CSatCCTsy( aNotificationsTsy );
    CleanupStack::PushL( satCcTsy );
    satCcTsy->iSatTsy = aSatTsy;
    satCcTsy->ConstructL();
    CleanupStack::Pop( satCcTsy );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_NEWL_2, "CSAT: CSatCCTsy::NewL, end of method");
    return satCcTsy;
    }

// -----------------------------------------------------------------------------
// CSatCCTsy::~CSatCCTsy
// Destructor
// -----------------------------------------------------------------------------
//    
CSatCCTsy::~CSatCCTsy
        (    
        //None   
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_DTOR_1, "CSAT: CSatCCTsy::~CSatCCTsy");
    // Unregister.
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatCCTsyObjType, NULL );
    }
    
// -----------------------------------------------------------------------------
// CSatCCTsy::ConstructL
// Symbian 2nd phase constructor. Initialises internal attributes.
// -----------------------------------------------------------------------------
//
void CSatCCTsy::ConstructL
        (    
        //None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_CONSTRUCTL_1, "CSAT: CSatCCTsy::ConstructL\n" );
    // Register.
    iSatTsy->MessageManager()->RegisterTsyObject(
		CMmMessageManagerBase::ESatCCTsyObjType, this );
	iBearerData.Zero();
	// Initialize to false
	iTonNpiPresent = EFalse;
	// Clean the address buffer
	iProactiveCommandAddress.Zero();
	// Initialize USSD TLV support to not supported
	iUssdTlvSupported = EFalse;
    }

// -----------------------------------------------------------------------------
// CSatCCTsy::CSatCCTsy
// C++ constructor
// -----------------------------------------------------------------------------
//
CSatCCTsy::CSatCCTsy
        (    
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatCCTsy::CompleteBearerCapability
// Saves default bearer data for voice call.
// -----------------------------------------------------------------------------
//
void CSatCCTsy::CompleteBearerCapability
         (
		 CSatDataPackage* aDataPackage, 
		 TInt /*aResult*/
		 )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_COMPLETEBEARERCAPABILITY_1, "CSAT: CSatCCTsy::CompleteBearerCapability");
    TPtrC8* bearerData = NULL;

    // Unpack parameters 
    aDataPackage->UnPackData( &bearerData );
    iBearerData.Copy( *bearerData );
	}
	
// -----------------------------------------------------------------------------
// CSatCCTsy::CreateEnvelope
// Identifies the type of envelope and calls appropriate method.
// -----------------------------------------------------------------------------
void CSatCCTsy::CreateEnvelopeL
        (
        CSatDataPackage* aDataPackage
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_CREATEENVELOPEL_1, "CSAT: CSatCCTsy::CreateEnvelope");
    TCCInfo* ccInfo;
    
    // Unpack parameters
    aDataPackage->UnPackData( &ccInfo );
    
    switch ( ccInfo->iTag )
        {
        case KTlvAddressTag:
            {
            CreateCallEnvelopeL( ccInfo );
            break;
            }
        case KTlvSsStringTag:
            {
            CreateSSEnvelopeL( ccInfo );
            break;
            }
        case KTlvUssdStringTag:
            {
            CreateUSSDEnvelopeL( ccInfo );
            break;
            }
        default:
            {
            OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_CREATEENVELOPEL_2, "CSAT: CSatCCTsy::CreateEnvelope, Unidentified tag: %d", ccInfo->iTag );
            }
        }
    }

// -----------------------------------------------------------------------------
// CSatCCTsy::CheckIfAlphaIdPresent
// Notifies client if alpha ID was present in incoming data notification.
// -----------------------------------------------------------------------------
//
void CSatCCTsy::CheckIfAlphaIdPresent
        (
        CSatDataPackage* aDataPackage
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_CHECKIFALPHAIDPRESENT_1, "CSAT: CSatCCTsy::CheckIfAlphaIdPresent");
    TDesC8* atkData;            
    RSat::TControlResult* result;
    
    // Unpack parameters
    aDataPackage->UnPackData( &atkData, &result );
        
    TUint16 envLength( 0 );
    TInt lengthOfLength( 1 );
    TInt index( 1 );
    
    // Check first that the data exists
    if ( 1 < atkData->Length() )
        {
        // Check if length is taking two bytes or not.
        if ( KTwoByteLengthCoding != ( *atkData )[ index ] ) 
            {
            // It takes one
            envLength = ( ( *atkData )[ index ] );
            }
        else
            {
            // It takes two. ( Actually it takes only one, but first byte is 
            // meaningless. See ETSI 11.14 Annex D )
            envLength = ( ( *atkData )[ index + 1 ] );
            lengthOfLength = 2;
            }

        index += lengthOfLength;

        if ( envLength != 0 )
            {
            while ( index < envLength )
                {
    	        // Check next tag
    	        TUint8 nextTag( ( *atkData )[ index ] );

                switch ( nextTag & KTagValueMask )
                    {
                    case KTlvAddressTag:
                    case KTlvCapabilityConfigurationParametersTag:
                    case KTlvSubaddressTag:
                    case KTlvSsStringTag:
                    case KTlvUssdStringTag:
                    case KTlvBcRepeatIndicatorTag:
                        {
                        TUint16 length( 0 );
                        TUint8 localLengthOfLength( 1 );
                        index++;
                        // Check if length is taking two bytes or not.
                        if ( KTwoByteLengthCoding  != ( *atkData )[ index ] )
                            {
                            // It takes one
                            length = ( ( *atkData )[ index ] );
                            }
                        else
                            {
                            // It takes two
                            length = ( ( *atkData )[ index + 1 ] );
                            localLengthOfLength = 2;
                            }
                        index += localLengthOfLength + length;
                        break;
                        }
                    case KTlvAlphaIdentifierTag:
                        {
                        TUint16 alphaIdLength( 0 );
                        lengthOfLength = 1;
                        index++;
                        // Check if length is taking two bytes or not
                        if ( KTwoByteLengthCoding != ( *atkData )[ index ] )
                            {
                            alphaIdLength = ( *atkData )[ index ];
                            }
                        else
                            {
                            alphaIdLength = ( *atkData )[ index + 1 ];
                            lengthOfLength = 2;
                            }

                        // If the alpha identifier is provided by the NAA and is 
                        // not a null data object, the ME shall use it to inform  
                        // the user. The ME shall then not display the destination 
                        // address or SS string given by the NAA. This is also an 
                        // indication that the ME should not give any other 
                        // information to the user on the changes made by the NAA 
                        // to the initial user request. Design decision: If alpha 
                        // identifier is not given or it is NULL, client will not 
                        // be notified. Specs leaves this open so this decision 
                        // can be made.
                        if ( 0 != alphaIdLength )
                            {
                            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_CHECKIFALPHAIDPRESENT_2, "CSAT: CSatCCTsy::CheckIfAlphaIdPresent. inform user.");

                            TPtrC8 sourceString;
                            sourceString.Set( ( *atkData ).Mid( index + 
                                lengthOfLength, alphaIdLength ) );

                            RSat::TAlphaIdBuf alphaId;                       
                            // Convert and set the alpha id
    						TSatUtility::SetAlphaId( sourceString, alphaId ); 
                                    
                            iNotificationsTsy->NotifyClientForAlphaId( alphaId, 
                                *result );
                            }
                        else
                            {
                            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_CHECKIFALPHAIDPRESENT_3, "CSAT: CSatCCTsy::CheckIfAlphaIdPresent. AlphaID length: 0.");
                            }

                        // Update index
                        index += alphaIdLength + lengthOfLength;
                        break;
                        }
                    default:
                        {
                        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_CHECKIFALPHAIDPRESENT_4, "CSAT: CSatCCTsy::CheckIfAlphaIdPresent Unknown tag.");
                        index = envLength;
                        break;
                        }
                    }
                }
            }
        }
    else
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_CHECKIFALPHAIDPRESENT_5, "CSAT: CSatCCTsy::CheckIfAlphaIdPresent. Input data missing!");
        }
    }
    
// -----------------------------------------------------------------------------
// CSatCCTsy::CompleteCCEventL
// Completes the received call control event data notification
// -----------------------------------------------------------------------------
//
void CSatCCTsy::CompleteCCEventL
        (
        TInt aIpc,
        CSatDataPackage* aDataPackage 
        )
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_COMPLETECCEVENTL_1, "CSAT: CSatCCTsy::CompleteCCEventL, aIpc: %d", aIpc);
    TCCInfo* ccInfo;
    
    // Unpack parameters
    aDataPackage->UnPackData( &ccInfo );
    
    switch ( aIpc )
        {
        case ESatTsyCallEvent:
            {
            // 3GPP TS 11.14 V8.17.0 (2004-09):
            // It is possible that NAA requests ME to set up an emergency call by 
            // supplying "112" as called party number. If NAA supplies a number 
            // stored in EFECC, this shall not result in an emergency call.
            //
            // The number included in the SET UP CALL proactive command is never
            // checked against FDN list. So no need to done it here.
            if ( HasProactiveOrigin( ccInfo->iAddress ) )
                {
                // Match! Address can be reset and envelope sent
                iProactiveCommandAddress.Zero();            		
                CreateCallEnvelopeL( ccInfo );
                }
            else
                {
                // Check if this is a emergency call creation attempt. Although 
                // emergency calls made through TSY always use emergency flag 
                // and emergency mode and are therefore passed straight through
                // guardians, emergency calls made by AT commands and bypassing 
                // TSY do not use emergency flag and mode. A possility that NAA 
                // would alter the emergency call to something else cannot be 
                // tolerated. Forward request to the DOS
                iSatTsy->MessageManager()->HandleRequestL( 
            		ESatTsyEmergencyNbrCheck );
                }  
            break;
            }
        case ESatTsySSEvent:
            {
            // 3GPP TS 11.14 V8.17.0 (2004-09):
            // The SS control string included in the SEND SS proactive cmd is 
            // never checked against FDN list. So make sure this wont happen.
            if ( HasProactiveOrigin( ccInfo->iAddress ) )
                {
                // Match! Address can be reset and envelope sent without FDN 
                // check.
                iProactiveCommandAddress.Zero();
                CreateSSEnvelopeL( ccInfo );
                }
            // Not proactive originated, check if FDN is activated.
            else
                {
                // Forward request to the DOS
                iSatTsy->MessageManager()->HandleRequestL( 
            		ESatTsyFdnStatusCheck );
                }  
            break;
            }
        case ESatTsyUSSDEvent:
            {
            // Send envelope right away, there's no difference whether the CC 
            // event was originated by proactive command or not.
            CreateUSSDEnvelopeL( ccInfo );
            break;
            }
        default:
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_COMPLETECCEVENTL_2, "CSAT: CSatCCTsy::CompleteCCEventL, Unknown event!");
            }
        }
    }   
    	
// -----------------------------------------------------------------------------
// CSatCCTsy::SetTonNpiForSS
// Set CSatCC internal Ton and Npi
// Called by CSatNotifySendSs::CompleteNotifyL when SEND SS proactive command 
// arrives
// -----------------------------------------------------------------------------
//
void CSatCCTsy::SetTonNpiForSS
        ( 
        const TUint8 aTonNpi 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_SETTONNPIFORSS_1, "CSAT: CSatCCTsy::SetTonNpiForSS");
    iTonNpiForSS = aTonNpi;
    iTonNpiPresent = ETrue;
    }

// -----------------------------------------------------------------------------
// CSatCCTsy::SetUssdStatus
// Set internal flag according to EF-SST.
// -----------------------------------------------------------------------------
//
void CSatCCTsy::SetUssdStatus
		( 
    	CSatDataPackage* aDataPackage
    	)
    { 
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_SETUSSDSTATUS_1, "CSAT: CSatCCTsy::SetUssdStatus." );
    // Unpack parameters 
    aDataPackage->UnPackData( iUssdTlvSupported ); 
    }  

// -----------------------------------------------------------------------------
// CSatCCTsy::StoreAddressForCC
// Stores latest address of Send SS/Setup call proactive command to CSatCC
// (other items were commented in a header). Method is also called when 
// proactive session is over with parameter NULL.
// -----------------------------------------------------------------------------
//    
void CSatCCTsy::StoreAddressForCC
        ( 
        const TDesC8& aAddress 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_STOREADDRESSFORCC_1, "CSAT: CSatCCTsy::StoreAddressForCC");
    
    if ( NULL == &aAddress )
        {
        iProactiveCommandAddress.Zero();
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_STOREADDRESSFORCC_2, "CSAT: CSatCCTsy::StoreAddressForCC, address cleared");
        }
    else
        {
        iProactiveCommandAddress.Copy( aAddress );
        
        // Remove the '+' sign from the the beginning of the address
        // if it exists. Otherwise the comparison does not work.     
        if ( 0 < iProactiveCommandAddress.Length() )
            {
            if ( '+' == iProactiveCommandAddress[0] )
                {
                iProactiveCommandAddress.Delete( 0, 1 );
                }
            }
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_STOREADDRESSFORCC_3, "CSAT: CSatCCTsy::StoreAddressForCC, Address stored");
        }
    }     
       
// -----------------------------------------------------------------------------
// CSatCCTsy::CreateCallEnvelopeL
// Creates call Envelope.
// -----------------------------------------------------------------------------
//
void CSatCCTsy::CreateCallEnvelopeL
		( 
		const TCCInfo* aCCInfo
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_CREATECALLENVELOPEL_1, "CSAT: CSatCCTsy::CreateCallEnvelopeL" );
    // Create envelope
    TTlv envelope;
    envelope.Begin( KBerTlvCallControlTag );

    // Device identities
    envelope.AddTag( KTlvDeviceIdentityTag );
    envelope.AddByte( KMe );
    envelope.AddByte( KSim );

    // Address
    envelope.AddTag( KTlvAddressTag );
    envelope.AddByte( aCCInfo->iTonNpi ); 

    // Must be converted to BCD number
    TBuf8<KAddrMaxLength> address;
    TSatUtility::AsciiToBCD( aCCInfo->iAddress, address );
    envelope.AddData( address );
       
    // Optional
    envelope.AddTag( KTlvCapabilityConfigurationParametersTag );
    
    if ( 0 != aCCInfo->iBcc1.Length() )
        {
        if ( 2 > aCCInfo->iBcc1.Length() )
            {
            TBuf8<KMaxBccLength> bcc;
            bcc.Append( 0x01 );
            bcc.Append( aCCInfo->iBcc1 );
            envelope.AddData( bcc );
            }
        else
            {
            envelope.AddData( aCCInfo->iBcc1 );
            }
        }
    // Although this field is not mandatory, it could help testing.  
    // Therefore field is included to enable wider testability.
    else
        {  
        if ( iBearerData.Length() < 2 )
            {
            iBearerData.Append( 0x01 );
            // Add the length
            envelope.AddByte( static_cast<TUint8>( iBearerData.Length() ) );
            envelope.AddData( iBearerData );
            }
        // Design decision: if bearer data is bigger than capability field, 
        // then this field is not added.
        else if ( KMaxBccLength >= iBearerData.Length() )
            {
            // Add the length
            envelope.AddByte( static_cast<TUint8>( iBearerData.Length() ) );
            envelope.AddData( iBearerData ); 
            }
        else
        	{
        	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_CREATECALLENVELOPEL_2, "CSAT: CSatCCTsy::CreateCallEnvelopeL, Bearer data length exceeded, data not added" );
        	}
        }

	// Add location information data
	iNotificationsTsy->AddLocationInformationToTlv( envelope );

    if ( 0 != aCCInfo->iBcc2.Length() )
        {
        envelope.AddTag( KTlvCapabilityConfigurationParametersTag );
        envelope.AddData( aCCInfo->iBcc2 ); 
        }

    // Prepare data
    TPtrC8 data = envelope.End();
	// Pack data
    CSatDataPackage dataPackage;
	dataPackage.PackData( &data );

    // Send Call envelope
    iSatTsy->MessageManager()->HandleRequestL( ESatTsySendEnvelope, 
        &dataPackage );
    }

// -----------------------------------------------------------------------------
// CSatCCTsy::CreateSSEnvelopeL
// Creates SS Envelope.
// -----------------------------------------------------------------------------
//
void CSatCCTsy::CreateSSEnvelopeL
		( 
		const TCCInfo* aCCInfo
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_CREATESSENVELOPEL_1, "CSAT: CSatCCTsy::CreateSSEnvelopeL" );
    // There is no TON and NPI in SS cc request. Information is part of the 
    // number string: Spec ETSI TS 122 030 v5.0.0 says that if number starts
    // with +, TON is international and if it starts without it, it's unknown.
    // ( ATK isi spec mentions also possibility of no TON and NPI,but so far 
    // nothing found about it from ETSI specs. SS ISI spec mentions even more )
    // Default NPI is used if all digits are in the range 0-9. NPI shall be 
    // unknown if other number information is included ( whatever that means ). 
    // In any case if the user has selected some particular NPI, that is used.
    TInt i( 0 );
    TUint8 tonNpi( KTonNpiUnknown );
    
    // If SS is entered by user, there's no SEND SS from where to resolve TonNpi
    // Then TonNpi is resolved from SS string
    // TonNpi   = Unknown if '+'-sign is not present
    //          = International if '+'-sign is found
    //          = No TonNpi if phone number is not present or SS-string contains
    //            only SS code

    if( !iTonNpiPresent )
        {
        // Seek '+' from ss string. 3GPP TS 22 030 V7.0.0 (2004-03) states:
        // "The procedure always starts with *, #, **, ## or *# and is finished
        // by #. Each part within the procedure is separated by *."
        
        // Skip start characters
        while ( (i < aCCInfo->iAddress.Length()) && 
              ( ('*' == aCCInfo->iAddress[i] ) || 
                ('#' == aCCInfo->iAddress[i]) ) )
            {
            i++;
            }

        TInt ssCode( 0 );
        // Seek for separation character and store service code
        while ( ( i < aCCInfo->iAddress.Length() ) && 
                ( '*' != aCCInfo->iAddress[i] ) && 
                ( '#' != aCCInfo->iAddress[i] ) )
            {
            ssCode *= 10;
            ssCode += ( aCCInfo->iAddress[i] - 0x30 );
            i++;
            }
        
        // With these SS codes dialling number is included. It's always located 
        // after first separation mark '*'. See 3GPP TS 22.030 7.0.0, 6.5.2 and 
        // Annex B. With checking the service code ton/npi resolving is made 
        // unambiguous.
        if( KSsAllForwardings == ssCode || 
            KSsAllCondForwardings == ssCode || 
            KSsForwUnconditional == ssCode|| 
            KSsForwBusy == ssCode || 
            KSsForwNoReply == ssCode || 
            KSsForwNotReachable == ssCode )
            {        
            // Set tonNpi international, if separation character (*) and 
            // (+) sign is found
            if( ( ( i + 1 ) < aCCInfo->iAddress.Length() ) && 
                ( ('*' == aCCInfo->iAddress[i] ) && 
                  ('+' == aCCInfo->iAddress[i + 1] ) ) )
                {
                tonNpi = KTonNpiInternational;
                }
            // Procedure is finished by #
            // If * is not found -> no number found
            else if( (i < aCCInfo->iAddress.Length() ) && 
                ('#' == aCCInfo->iAddress[i] ) )
                {
                tonNpi = KTonNpiNotSet;
                }
            else
                {
                // Number is found but it's tonnpi id unknown
                tonNpi = KTonNpiUnknown;
                }
            }
        else
            {
            // With all other SS codes TON/NPI is set to 0xff
            tonNpi = KTonNpiNotSet;
            }
        }
    else
        {
        // TonNpi is resolved from SEND SS proactive command
        tonNpi = iTonNpiForSS;
        }
        
    iTonNpiPresent = EFalse;

    // Create SS envelope
    TTlv envelope;
    envelope.Begin( KBerTlvCallControlTag );

    // Device identities
    envelope.AddTag( KTlvDeviceIdentityTag );
    envelope.AddByte( KMe );
    envelope.AddByte( KSim );

    // SS string
    envelope.AddTag( KTlvSsStringTag );
    envelope.AddByte( tonNpi );

    TBuf8<KMaxSSStringLength> ss;
    TSatUtility::AsciiToBCD( aCCInfo->iAddress, ss );
    envelope.AddData( ss );

	// Add location information data
	iNotificationsTsy->AddLocationInformationToTlv( envelope );

    // Prepare data
    TPtrC8 data = envelope.End();
	// Pack data
    CSatDataPackage dataPackage;
	dataPackage.PackData( &data );

    // Send SS envelope
    iSatTsy->MessageManager()->HandleRequestL( 
        ESatTsySendEnvelope, &dataPackage );
	}


// -----------------------------------------------------------------------------
// CSatCCTsy::CreateUSSDEnvelopeL
// Creates USSD Envelope.
// -----------------------------------------------------------------------------
//
void CSatCCTsy::CreateUSSDEnvelopeL
		( 
		const TCCInfo* aCCInfo
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_CREATEUSSDENVELOPEL_1, "CSAT: CSatCCTsy::CreateUSSDEnvelopeL" );
    // Create USSD envelope
    TTlv envelope;
        
    envelope.Begin( KBerTlvCallControlTag );
    // Device identities
    envelope.AddTag( KTlvDeviceIdentityTag );
    envelope.AddByte( KMe );
    envelope.AddByte( KSim );

    // USSD string tlv
    if ( iUssdTlvSupported )
        {
        // USSD string
        envelope.AddTag( KTlvUssdStringTag );
        envelope.AddByte( aCCInfo->iDcs );
        envelope.AddData( aCCInfo->iAddress );
        
        iNotificationsTsy->AddLocationInformationToTlv( envelope );

        // Prepare data
        TPtrC8 data = envelope.End();
    	// Pack data
        CSatDataPackage dataPackage;
    	dataPackage.PackData( &data );
     
        // Send USSD envelope
        iSatTsy->MessageManager()->HandleRequestL( ESatTsySendEnvelope, 
            &dataPackage );
        }
    else
        {
        // UssdTlv is not supported by the NAA
        // Checks whether the USSD string can be coded in the SS obj. SS obj.
        // can only contain "*", "#", and the numbers 0-9.
        if ( IsOnlyDigitsInUssd( aCCInfo->iAddress ) )
            {
            envelope.AddTag( KTlvSsStringTag );
            // If the Dialling Number/SSC String does not contain a dialling 
            // number e.g. a control string deactivating a service, the TON/NPI
            // byte shall be set to 'FF' by the ME (see note 3GPP TS 11.11).
            // in this case TON & NPI must be set to 0xFF    
            envelope.AddByte( 0xFF );
            TBuf8<KMaxSSStringLength> ss;     
            TSatUtility::AsciiToBCD( aCCInfo->iAddress, ss );
    		envelope.AddData( ss );
    		iNotificationsTsy->AddLocationInformationToTlv( envelope );
    		// Prepare data
            TPtrC8 data = envelope.End();
            // Create package
			CSatDataPackage package;
			// Pack the data
	    	package.PackData( &data );
	    	// Send SS Envelope
	    	iSatTsy->MessageManager()->HandleRequestL( ESatTsySendEnvelope, 
	    	    &package );   
            }
        else
            {
            // NAA does not support USSD and converting it to SS was impossible.
            // The response must be sent back to NAA, to notify for the 
            // occurred error during the creation of an envelope
	    	iSatTsy->MessageManager()->HandleRequestL( ESatTsyEnvelopeError );        
            }        
        }
	}

// -----------------------------------------------------------------------------
// CSatCCTsy::IsOnlyDigitsInUssd
// This methods returns ETrue when a Ussd string contains only "*", "#", and 
// the numbers 0-9. Note: Ussd string is always received in packed format, when 
// it is 7-bit GSM default alphabet.
// -----------------------------------------------------------------------------
//  
TBool CSatCCTsy::IsOnlyDigitsInUssd
        (
        const TDesC8& aUSSDString
        )
    {    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_ISONLYDIGITSINUSSD_1, "CSAT: CSatCCTsy::IsOnlyDigitsInUssd." );
    // Unpack it
    TBuf8<KMaxUssdStringLengthInBytes> ussdString;
    TSatUtility::Packed7to8Unpacked( aUSSDString, ussdString );    
    TBool ret( ETrue );
    
    for( TInt k=0; k < ussdString.Length(); k++ )
        {
        if ( ( ussdString[k] != '#' ) &&
             ( ussdString[k] != '*' ) &&
             ( ussdString[k] != '+' ) &&
             ( ussdString[k] < '0'  || ussdString[k] > '9' ) )
            {
            ret = EFalse;
            break;
            }
        }
    
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSatCCTsy::HasProactiveOrigin
// Checks whether the given address matches with the one saved (from PCmd).
// -----------------------------------------------------------------------------
//    
TBool CSatCCTsy::HasProactiveOrigin
        ( 
        const TDesC8& aAddress 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_HASPROACTIVEORIGIN_1, "CSAT: CSatCCTsy::HasProactiveOrigin");
    TBool match( EFalse );
    
    if ( aAddress.Length() )
        {
        // Check first if there is a + in the beginning of the given address
        if ( '+' == aAddress[0] )
            {
            // It means we need to drop the first character out from comparison
            if ( iProactiveCommandAddress == aAddress.Mid( 1 ) )
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_HASPROACTIVEORIGIN_2, "CSAT: CSatCCTsy::HasProactiveOrigin, Match!");
                match = ETrue;
                }
            }
        // Otherwise we can use the whole string for checking
        else if ( iProactiveCommandAddress == aAddress )
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_HASPROACTIVEORIGIN_3, "CSAT: CSatCCTsy::HasProactiveOrigin, Match!");
            match = ETrue;
            }
        }
    else
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATCCTSY_HASPROACTIVEORIGIN_4, "CSAT: CSatCCTsy::HasProactiveOrigin, Invalid input address");
        }
        
    return match;
    }       


//  End of File 

