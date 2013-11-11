// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : CSatNotifyLaunchBrowser.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// LaunchBrowser notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifyLaunchBrowserTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatTsy.h"                // Tsy class header
#include "CSatNotifyLaunchBrowser.h"// Tsy class header
#include "CSatNotificationsTsy.h"   // Class header
#include "CBerTlv.h"                // Ber Tlv data handling
#include "TTlv.h"					// TTlv class
#include "CSatDataPackage.h"        // Parameter packing 
#include "TSatUtility.h"            // Utilities
#include "CSatTsyReqHandleStore.h"  // Request handle class
#include "cmmmessagemanagerbase.h"  // Message manager class for forwarding req.

// -----------------------------------------------------------------------------
// CSatNotifyLaunchBrowser::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CSatNotifyLaunchBrowser* CSatNotifyLaunchBrowser::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_NEWL_1, "CSAT: CSatNotifyLaunchBrowser::NewL");
   	CSatNotifyLaunchBrowser* const satNotifyLaunchBrowser = 
        new ( ELeave ) CSatNotifyLaunchBrowser( aNotificationsTsy );
    CleanupStack::PushL( satNotifyLaunchBrowser );
    satNotifyLaunchBrowser->ConstructL();
    CleanupStack::Pop( satNotifyLaunchBrowser );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_NEWL_2, "CSAT: CSatNotifyLaunchBrowser::NewL, end of method");
    return satNotifyLaunchBrowser;
    }

// -----------------------------------------------------------------------------
// CSatNotifyLaunchBrowser::~CSatNotifyLaunchBrowser
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifyLaunchBrowser::~CSatNotifyLaunchBrowser
        ( 
		// None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_DTOR_1, "CSAT: CSatNotifyLaunchBrowser::~CSatNotifyLaunchBrowser");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyLaunchBrowser::CSatNotifyLaunchBrowser
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifyLaunchBrowser::CSatNotifyLaunchBrowser
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyLaunchBrowser::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifyLaunchBrowser::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_CONSTRUCTL_1, "CSAT: CSatNotifyLaunchBrowser::ConstructL");
    iSsStatus = CSatNotificationsTsy::ENotBusy;
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyLaunchBrowser::Notify
// This request allows a client to be notified of a Launch Browser proactive 
// command
// -----------------------------------------------------------------------------
//
TInt CSatNotifyLaunchBrowser::Notify
        (
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage   
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_NOTIFY_1, "CSAT: CSatNotifyLaunchBrowser::Notify");
    // Save data pointer to client side for completion
    iLaunchBrowserV2Pckg = reinterpret_cast<RSat::TLaunchBrowserV2Pckg*>( 
        aPackage.Des1n() );
    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifyLaunchBrowserPCmdReqType );
    // Check if requested notification is already pending
    iNotificationsTsy->NotifySatReadyForNotification( KLaunchBrowser );   

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatNotifyLaunchBrowser::CancelNotification
// This method cancels an outstanding asynchronous 
// NotifyLaunchBrowser request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyLaunchBrowser::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_CANCELNOTIFICATION_1, "CSAT: CSatNotifyLaunchBrowser::CancelNotification");
    
    // Reset the request handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
        ResetTsyReqHandle( CSatTsy::ESatNotifyLaunchBrowserPCmdReqType );
	// Reset the data pointer
	iLaunchBrowserV2Pckg = NULL;
	// Complete the request with KErrCancel
	iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;    
    }



// -----------------------------------------------------------------------------
// CSatNotifyLaunchBrowser::CompleteNotifyL
// This method completes an outstanding asynchronous 
// NotifyLaunchBrowser request. 
// -----------------------------------------------------------------------------
//
TInt CSatNotifyLaunchBrowser::CompleteNotifyL
        (
        CSatDataPackage* aDataPackage,  
        TInt aErrorCode
        )
    {  
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_COMPLETENOTIFYL_1, "CSAT: CSatNotifyLaunchBrowser::CompleteNotifyL");
	TInt ret( KErrNone );
	TInt returnValue( KErrNone );		
    // Unpack parameters
    TPtrC8* data;
    aDataPackage->UnPackData( &data );

    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->ResetTsyReqHandle( 
        CSatTsy::ESatNotifyLaunchBrowserPCmdReqType );
    // Get ber tlv 
	CBerTlv berTlv;
    berTlv.SetData( *data );
	// Get command details tlv
	CTlv commandDetails;
	berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );	
    // Store command details tlv
	iNotificationsTsy->iTerminalRespData.iCommandDetails.Copy( commandDetails.Data() );	
	// Get command qualifier
	TUint8 cmdQualifier( commandDetails.GetShortInfo( ETLV_CommandQualifier ) );			
	TUint8 pCmdNumber( commandDetails.GetShortInfo( ETLV_CommandNumber ) );
	
	// In case the request was ongoing, continue..
    if ( CSatTsy::ESatReqHandleUnknown != reqHandle )
        {		
		// Check if busy 
    	if ( CSatNotificationsTsy::ENotBusy != iSsStatus )
    		{
    		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_COMPLETENOTIFYL_2, "CSAT: CSatNotifyLaunchBrowser::CompleteNotifyL, iSsStatus is busy");

        	if( CSatNotificationsTsy::EUssdBusy == iSsStatus)
    			{
    			// Ussd transaction ongoing
    			CreateTerminalRespL( pCmdNumber, RSat::KMeUnableToProcessCmd, 
			        RSat::KMeBusyOnUssd );	
    			}
    		else
    			{
    			// Ss transaction ongoing
    			CreateTerminalRespL( pCmdNumber, RSat::KMeUnableToProcessCmd, 
			        RSat::KMeBusyOnSs );		
    			}		
    		}
    	else
    		{	    
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_COMPLETENOTIFYL_3, "CSAT: CSatNotifyLaunchBrowser::CompleteNotifyL No SS Transaction ongoing");
			// Complete right away if error has occured, otherwise continue..
	        if ( KErrNone == aErrorCode )
	            {			
				// Launch browser structure
				RSat::TLaunchBrowserV2& launchBrowserV2 = 
					( *iLaunchBrowserV2Pckg )();
				
				// Store command number
                launchBrowserV2.SetPCmdNumber( pCmdNumber );

            	switch ( cmdQualifier )
            		{        
            		case KLaunchBrowserIfNotYetLaunched:
            			{
            			launchBrowserV2.iBrowserSel = 
            				RSat::ELaunchBrowserIfNotAlreadyLaunched;
            			}
            			break;
            		case KUseExistingBrowser:
            			{
            			launchBrowserV2.iBrowserSel = 
            			    RSat::EUseExistingBrowser;
            			break;
            			}
            		case KCloseAndLaunchNewBrowser:
            			{
            			launchBrowserV2.iBrowserSel = 
            				RSat::ECloseExistingLaunchNewBrowserSession;
            			break;
            			}
            		case KLaunchBrowserCmdQualifierNotUsed:
            		case KLaunchBrowserCmdQualifierReserved:
            			{
            			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_COMPLETENOTIFYL_4, "CSAT: CSatNotifyLaunchBrowser::CompleteNotifyL, Command Type not understood");
            			CreateTerminalRespL( pCmdNumber, 
            			    RSat::KCmdTypeNotUnderstood, KNoCause );
                        ret = KErrCorrupt;
            			break;
            			}
            		default: 
            			{
            			launchBrowserV2.iBrowserSel = 
            				RSat::EBrowserSelectionNotSet;
            			break;
            			}
            		}
	    
				if ( KErrNone == ret )
					{
					ret = ParseBrowserIdL( pCmdNumber, berTlv, 
					    launchBrowserV2 );
					}

				if ( KErrNone == ret )
					{
					ret = ParseUrlL( pCmdNumber, berTlv, launchBrowserV2 );
					}

				if ( KErrNone == ret )
					{
					ret = ParseProvisioningFileListL( pCmdNumber, berTlv, 
					    launchBrowserV2 );
					}
					
				if ( KErrNone == ret )
					{
					ret = ParseBearerListL( pCmdNumber, berTlv, 
					    launchBrowserV2 );
					}

				if ( KErrNone == ret )
					{
					// Text string (Gateway/Proxy) 
					CTlv textString;
					returnValue = berTlv.TlvByTagValue( &textString, 
					    KTlvTextStringTag );
					if ( KErrNone == returnValue )
						{
						TSatUtility::SetText( textString, 
						    launchBrowserV2.iText );
						}
					}
	        
				if ( KErrNone == ret )
					{
					// Alpha ID string (optional) for user confirmation phase
					CTlv alphaIdentifier;
					returnValue = berTlv.TlvByTagValue( &alphaIdentifier, 
						KTlvAlphaIdentifierTag );
					launchBrowserV2.iAlphaId.iStatus = 
					    RSat::EAlphaIdNotPresent;
					
					if ( KErrNotFound != returnValue )
						{
						OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_COMPLETENOTIFYL_5, "CSAT: CSatNotifyLaunchBrowser::CompleteNotifyL, Alpha ID present");
						TUint8 alphaIdLength( alphaIdentifier.GetLength() );
						if ( alphaIdLength )
							{
							TPtrC8 sourceString;

							// Get the alpha id
							sourceString.Set( alphaIdentifier.GetData( 
							    ETLV_AlphaIdentifier ) );
	        
							// Convert and set the alpha id
							TSatUtility::SetAlphaId( sourceString , 
								launchBrowserV2.iAlphaId.iAlphaId ); 

							launchBrowserV2.iAlphaId.iStatus = 
							    RSat::EAlphaIdProvided;
							}
						else
							{
							OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_COMPLETENOTIFYL_6, "CSAT: CSatNotifyLaunchBrowser::CompleteNotifyL, Alpha ID is NULL");
							launchBrowserV2.iAlphaId.iStatus = 
							    RSat::EAlphaIdNull;
							}
						}
	        
            		// Iconid 
            		TSatUtility::FillIconStructure( berTlv, 
            			launchBrowserV2.iIconId );
            		}
			
			    } // End of if (KErrNone == aErrorCode)
	        else
	        	{
	        	ret = aErrorCode;
	        	}

	        iNotificationsTsy->iSatTsy->ReqCompleted( reqHandle, ret );
	        }
        }// End of if ( reqHandle != CSatTsy::ESatReqHandleUnknown ) 
	else 
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_COMPLETENOTIFYL_7, "CSAT: CSatNotifyLaunchBrowser::CompleteNotifyL, Request not ongoing");
        // Request not on, returning response immediately
		CreateTerminalRespL( pCmdNumber, RSat::KMeUnableToProcessCmd, 
			KNoCause );
		}    

	return ret;
	}



// -----------------------------------------------------------------------------
// CSatNotifyLaunchBrowser::TerminalResponseL
// Called by ETel server, passes terminal response to DOS
// -----------------------------------------------------------------------------
//
TInt CSatNotifyLaunchBrowser::TerminalResponseL
        ( 
        TDes8* aRsp
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_TERMINALRESPONSEL_1, "CSAT: CSatNotifyLaunchBrowser::TerminalResponseL");
	TInt   ret( KErrNone );
    TUint8 additionalInfo( KNoCause );
    RSat::TLaunchBrowserRspV2Pckg* aRspPckg = 
            reinterpret_cast<RSat::TLaunchBrowserRspV2Pckg*>( aRsp );
    RSat::TLaunchBrowserRspV2& rspV2 = ( *aRspPckg ) ();
    // Get Proactive command number
    TUint8 pCmdNumber( rspV2.PCmdNumber() );
    
    // Check that general result value is valid
    if ( ( RSat::KSuccess != rspV2.iGeneralResult ) && 
         ( RSat::KPSessionTerminatedByUser  != rspV2.iGeneralResult ) && 
         ( RSat::KBackwardModeRequestedByUser != rspV2.iGeneralResult ) && 
         ( RSat::KNoResponseFromUser != rspV2.iGeneralResult ) && 
         ( RSat::KMeUnableToProcessCmd != rspV2.iGeneralResult ) && 
         ( RSat::KCmdBeyondMeCapabilities != rspV2.iGeneralResult ) && 
         ( RSat::KCmdDataNotUnderstood != rspV2.iGeneralResult ) && 
         ( RSat::KLaunchBrowserError != rspV2.iGeneralResult ) && 
         ( RSat::KPCmdNotAcceptedByUser != rspV2.iGeneralResult ) &&
         ( RSat::KCmdTypeNotUnderstood != rspV2.iGeneralResult ) &&
         ( RSat::KPartialComprehension != rspV2.iGeneralResult ) &&
         ( RSat::KMissingInformation != rspV2.iGeneralResult ) &&
         ( RSat::KNetworkUnableToProcessCmd != rspV2.iGeneralResult ) &&
         ( RSat::KCmdNumberNotKnown != rspV2.iGeneralResult ) )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_TERMINALRESPONSEL_2, "CSAT: CSatNotifyLaunchBrowser::TerminalResponseL, Invalid General Result");
        // Invalid general result
        ret = KErrCorrupt;
        }

    // Check that infotype is valid: from enum TBrowserError in etelsat.h
    if ( ( RSat::KMeProblem != rspV2.iInfoType ) && 
         ( RSat::KNoAdditionalInfo != rspV2.iInfoType ) && 
         ( RSat::KBearerUnvailable != rspV2.iInfoType ) && 
         ( RSat::KBrowserUnavailable != rspV2.iInfoType ) && 
         ( RSat::KMeUnableToReadProvisioningData != rspV2.iInfoType ) && 
         ( RSat::KNoSpecificBrowserError != rspV2.iInfoType ) )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_TERMINALRESPONSEL_3, "CSAT: CSatNotifyLaunchBrowser::TerminalResponseL, Invalid Info Type");
        ret = KErrCorrupt;
        }

    // If there is Me (Mobile Equipment) error, additional info is needed
    if ( ( RSat::KMeProblem == rspV2.iInfoType ) )
        {
        // Check the length of additional info
        if ( 0 != rspV2.iAdditionalInfo.Length() )
            {
            additionalInfo = rspV2.iAdditionalInfo[0];
			}
        else
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_TERMINALRESPONSEL_4, "CSAT: CSatNotifyLaunchBrowser::TerminalResponseL, Invalid Additional Info");
            // Invalid additional info field
            ret = KErrCorrupt;
            }
        }

    CreateTerminalRespL( pCmdNumber, static_cast<TUint8>( 
        rspV2.iGeneralResult ), additionalInfo );                            
	
	return ret;
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyLaunchBrowser::SetSatTsySsStatus
// Stores Ss status information.
// -----------------------------------------------------------------------------
//
void CSatNotifyLaunchBrowser::SetSsStatus
		(
		CSatDataPackage* aDataPackage
		)
	{
	// Unpack parameters
	aDataPackage->UnPackData( iSsStatus );
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_SETSSSTATUS_1, "CSAT: CSatNotifyLaunchBrowser::SetSatTsySsStatus, status: %d", iSsStatus );
	}
    
// -----------------------------------------------------------------------------
// CSatNotifyLaunchBrowser::ParseBrowserIdL
// Help method for CompleteNotify, sets the Browser ID
// -----------------------------------------------------------------------------
//
TInt CSatNotifyLaunchBrowser::ParseBrowserIdL
        ( 
        TUint8 aPCmdNumber,
        CBerTlv& aBerTlv,
        RSat::TLaunchBrowserV2& aLaunchBrowserV2
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_PARSEBROWSERIDL_1, "CSAT: CSatNotifyLaunchBrowser::ParseBrowserId");
    TInt ret( KErrNone );
    CTlv browserId; // optional
	TInt returnValue = aBerTlv.TlvByTagValue( &browserId, 
		KTlvBrowserIdentityTag );
		
	if ( KErrNotFound != returnValue )
		{
		// Browser id 0x00 allowed, other values are RFU
		if ( 0x00 != browserId.GetShortInfo( ETLV_BrowserIdentity ) )
			{				
			CreateTerminalRespL( aPCmdNumber, RSat::KCmdDataNotUnderstood, 
			    KNoCause );
			ret = KErrCorrupt;
			}
		else
			{
			aLaunchBrowserV2.iBrowserId = ( RSat::TBrowserIdentity )
				browserId.GetShortInfo( ETLV_BrowserIdentity );
			}
		}
	else
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_PARSEBROWSERIDL_2, "CSAT: CSatNotifyLaunchBrowser::ParseBrowserId, Browser ID not set");
		aLaunchBrowserV2.iBrowserId = RSat::EBrowserIdNotSet;
		}
	return ret;
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyLaunchBrowser::ParseUrlL
// Help method for CompleteNotify, sets the URL
// -----------------------------------------------------------------------------
//
TInt CSatNotifyLaunchBrowser::ParseUrlL
        ( 
        TUint8 aPCmdNumber,
        CBerTlv& aBerTlv,
        RSat::TLaunchBrowserV2& aLaunchBrowserV2
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_PARSEURLL_1, "CSAT: CSatNotifyLaunchBrowser::ParseUrl");
    TInt ret( KErrNone );
    CTlv url; // mandatory
	TInt returnValue = aBerTlv.TlvByTagValue( &url, KTlvUrlTag );
	
	if ( KErrNotFound != returnValue )
		{
		if ( RSat::KUrlMaxSize < url.GetLength() )
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_PARSEURLL_2, "CSAT: CSatNotifyLaunchBrowser::ParseUrl, False URL length");
			CreateTerminalRespL( aPCmdNumber, RSat::KMeUnableToProcessCmd, 
			    KNoCause );
			ret = KErrCorrupt;
			}
		else
			{
			aLaunchBrowserV2.iUrl.Copy( url.GetData( ETLV_Url ) );
			}
		}
	else
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_PARSEURLL_3, "CSAT: CSatNotifyLaunchBrowser::ParseUrl, Required values missing");
		CreateTerminalRespL( aPCmdNumber, RSat::KErrorRequiredValuesMissing, 
	        KNoCause );
		ret = KErrCorrupt;
		}
	return ret;
    }
 
// -----------------------------------------------------------------------------
// CSatNotifyLaunchBrowser::ParseProvisioningFileListL
// Help method for CompleteNotify, sets the Provisioning File List
// -----------------------------------------------------------------------------
//
TInt CSatNotifyLaunchBrowser::ParseProvisioningFileListL
        ( 
        TUint8 aPCmdNumber,
        CBerTlv& aBerTlv,
        RSat::TLaunchBrowserV2& aLaunchBrowserV2
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_PARSEPROVISIONINGFILELISTL_1, "CSAT: CSatNotifyLaunchBrowser::ParseProvisioningFileList");
    TInt ret( KErrNone );
    
    CTlv provisioningFileList; // optional
	TInt returnValue = aBerTlv.TlvByTagValueMulti( &provisioningFileList, 
		KTlvProvisioningReferenceFileTag );

	TPtrC8 fileRefData = provisioningFileList.Data();
	TUint8 numberOfFileRefData( 0 );
	TInt ind( 0 );
	TInt tlvLength( 0 );
	TInt currentTlv( 0 );

	if ( KErrNotFound != returnValue )
		{
		// Search for number of file references
		TInt i( 0 );
		for ( i = 0; i < fileRefData.Length(); i += tlvLength )
			{
			// First determine if the TLV length is coded with 1 or 2 bytes.
			if ( KTwoByteLengthCoding == fileRefData[ind+1] )
				{
				// Length is coded with 2 bytes -> 1 extra byte required to be 
				// added to total length. Also TLV header bytes (2) must be 
				// added to total length
				tlvLength = fileRefData[ind+2] + KTlvHeaderLength + 1;
				}        
			else
				{
				// TLV header bytes (2) must be added to total length
				tlvLength = fileRefData[ind+1]  + KTlvHeaderLength;
				}

			currentTlv = fileRefData[ind];
			if ( KTlvProvisioningReferenceFileTag == currentTlv )
				{
				numberOfFileRefData++;   
				}
    
			ind += tlvLength;
			}

		RSat::TProvisioningFileRef newFileRef;
		// Set string length and pos
		TUint8 stringLength( 0 );
		TUint  pos( 2 );
    
		// Add FileReferences to the structure launchBrowserV2
		for ( i = 0; ( i < numberOfFileRefData ) && ( KErrNone == ret ); i++ )
			{
			// Fill the newfileref
			stringLength = ( TUint8 ) fileRefData[pos - 1];
			// Test whether the text contains more than "MF"
			if ( ( RSat::KFileRefMaxSize < stringLength ) || 
			     ( 4 > stringLength ) )
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_PARSEPROVISIONINGFILELISTL_2, "CSAT: CSatNotifyLaunchBrowser::ParseProvisioningFileList, False string length");
			    CreateTerminalRespL( aPCmdNumber, RSat::KLaunchBrowserError, 
				    RSat::KMeUnableToReadProvisioningData );
				ret = KErrCorrupt;
				break;
				}
			
			newFileRef.Zero();
			TUint8 x( 0 );
			
			for ( x = 0; x < stringLength; x++ )
				{
				newFileRef.Append( fileRefData[pos+x] );
				}
				
			 // Adding the new fileref
			if ( KErrNoMemory == aLaunchBrowserV2.AddFileRef( newFileRef ) )
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_PARSEPROVISIONINGFILELISTL_3, "CSAT: CSatNotifyLaunchBrowser::ParseProvisioningFileList, Menu items corrupted");
				// Too many or long menuitems				
			    CreateTerminalRespL( aPCmdNumber, RSat::KLaunchBrowserError, 
				    RSat::KMeUnableToReadProvisioningData );
				ret = KErrCorrupt;
				}
				
			// Calculating the new position
			pos = TUint( pos + fileRefData[pos - 1] + 2 );
			}
		}
	return ret;
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyLaunchBrowser::ParseBearerListL
// Help method for CompleteNotify, sets the Bearer List
// -----------------------------------------------------------------------------
//
TInt CSatNotifyLaunchBrowser::ParseBearerListL
        ( 
        TUint8 aPCmdNumber,
        CBerTlv& aBerTlv,
        RSat::TLaunchBrowserV2& aLaunchBrowserV2
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_PARSEBEARERLISTL_1, "CSAT: CSatNotifyLaunchBrowser::ParseBearerList");
    TInt ret( KErrNone );
    // Bearers (optional)
	CTlv bearerList;
	TInt returnValue = aBerTlv.TlvByTagValue( &bearerList, KTlvBearerTag );
	    
	if ( KErrNotFound != returnValue )
		{
		if ( RSat::KBearerListMaxSize >= bearerList.GetLength() )
			{
			TBuf8<RSat::KBearerListMaxSize> brList = bearerList.GetData( 
			    ETLV_ListOfBearers );
			    
			// SMS or USSD are not available bearers.
			TInt i( 0 );
			for ( i = 0; i < brList.Length() ; i++)
				{
				if ( KCsdBearer  == brList[i] || KGprsBearer == brList[i] )
					{
					aLaunchBrowserV2.iBearerList.Append( brList[i] );
					}
				}
				
			if ( 0x00 == aLaunchBrowserV2.iBearerList.Length() )
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_PARSEBEARERLISTL_2, "CSAT: CSatNotifyLaunchBrowser::ParseBearerList, Bearer unvailable");
				CreateTerminalRespL( aPCmdNumber, RSat::KLaunchBrowserError, 
				    RSat::KBearerUnvailable );
				ret = KErrCorrupt;
				}       
			}
		else
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_PARSEBEARERLISTL_3, "CSAT: CSatNotifyLaunchBrowser::ParseBearerList, Bearer List length exceeded");
			CreateTerminalRespL( aPCmdNumber, RSat::KCmdDataNotUnderstood, 
			    RSat::KCmdDataNotUnderstood );
			ret = KErrCorrupt;
			}
		}
	return ret;
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyLaunchBrowser::CreateTerminalRespL
// Constructs LaunchBrowser specific part of terminal response and calls 
// DOS to send the actual message.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyLaunchBrowser::CreateTerminalRespL
        ( 
        TUint8 aPCmdNumber,         
        TUint8 aGeneralResult,      
        TUint8 aAdditionalInfo		
		)
    {
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYLAUNCHBROWSER_CREATETERMINALRESPL_1, "CSAT: CSatNotifyLaunchBrowser::CreateTerminalRespL");
    TTlv tlvSpecificData;
    tlvSpecificData.AddTag( KTlvResultTag );
    // Create General Result TLV here
    tlvSpecificData.AddByte( aGeneralResult );
    
    if ( !( iNotificationsTsy->CommandPerformedSuccessfully( aGeneralResult ) ) )
        {
        if ( RSat::KCmdTypeNotUnderstood != aGeneralResult && 
             RSat::KCmdDataNotUnderstood != aGeneralResult && 
             RSat::KErrorRequiredValuesMissing != aGeneralResult )
            {
            // additional info
            tlvSpecificData.AddByte( aAdditionalInfo );
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
   		

// End of File
