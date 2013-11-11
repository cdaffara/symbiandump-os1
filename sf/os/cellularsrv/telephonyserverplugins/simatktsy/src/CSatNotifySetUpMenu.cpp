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
// Name        : CSatNotifySetUpMenu.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// SetUpMenu notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifySetUpMenuTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatTsy.h"                // Tsy class header
#include "CSatNotifySetUpMenu.h"    // Tsy class header
#include "CSatNotificationsTsy.h"   // Class header
#include "CBerTlv.h"                // Ber Tlv data handling
#include "TTlv.h"					// TTlv class
#include "CSatDataPackage.h"        // Parameter packing 
#include "TSatUtility.h"            // Utilities
#include "CSatTsyReqHandleStore.h"  // Request handle class
#include "cmmmessagemanagerbase.h"  // Message manager class for forwarding req.

// -----------------------------------------------------------------------------
// CSatNotifySetUpMenu::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CSatNotifySetUpMenu* CSatNotifySetUpMenu::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_NEWL_1, "CSAT: CSatNotifySetUpMenu::NewL");
   	CSatNotifySetUpMenu* const satNotifySetUpMenu = 
        new ( ELeave ) CSatNotifySetUpMenu( aNotificationsTsy );
    CleanupStack::PushL( satNotifySetUpMenu );
    satNotifySetUpMenu->ConstructL();
    CleanupStack::Pop( satNotifySetUpMenu );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_NEWL_2, "CSAT: CSatNotifySetUpMenu::NewL");
    return satNotifySetUpMenu;
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpMenu::~CSatNotifySetUpMenu
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifySetUpMenu::~CSatNotifySetUpMenu
        ( 
		// None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_DTOR_1, "CSAT: CSatNotifySetUpMenu::~CSatNotifySetUpMenu");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifySetUpMenu::CSatNotifySetUpMenu
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifySetUpMenu::CSatNotifySetUpMenu
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpMenu::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifySetUpMenu::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_CONSTRUCTL_1, "CSAT: CSatNotifySetUpMenu::ConstructL");
    iItemsNextIndicatorRemoved = EFalse;
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpMenu::Notify
// This request allows a client to be notified of a SET UP MENU proactive 
// command
// -----------------------------------------------------------------------------
//
TInt CSatNotifySetUpMenu::Notify
        (
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage    
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_NOTIFY_1, "CSAT: CSatNotifySetUpMenu::Notify");
    // Save data pointer to client side for completion
    iSetUpMenuV2Pckg = reinterpret_cast<RSat::TSetUpMenuV2Pckg*>( 
        aPackage.Des1n() );
    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifySetUpMenuPCmdReqType );
    // Check if requested notification is already pending
    iNotificationsTsy->NotifySatReadyForNotification( KSetUpMenu );   
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CSatNotifySetUpMenu::CancelNotification
// This method cancels an outstanding asynchronous 
// NotifySetUpMenu request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifySetUpMenu::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle		
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_CANCELNOTIFICATION_1, "CSAT: CSatNotifySetUpMenu::CancelNotification");
    // Reset the request handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
        ResetTsyReqHandle( CSatTsy::ESatNotifySetUpMenuPCmdReqType );
	// Reset the data pointer
	iSetUpMenuV2Pckg = NULL;
	// Complete the request with KErrCancel
	iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CSatNotifySetUpMenu::CompleteNotifyL
// Complete Set Up Menu notification to the client
// -----------------------------------------------------------------------------
// 
TInt CSatNotifySetUpMenu::CompleteNotifyL
		(
        CSatDataPackage* aDataPackage,   
        TInt aErrorCode                  
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_COMPLETENOTIFYL_1, "CSAT: CSatNotifySetUpMenu::CompleteNotifyL");
    TInt ret( KErrNone );
	TInt returnValue( KErrNone );
	// Unpack parameters
    TPtrC8* data;
    aDataPackage->UnPackData( &data );

	// Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = 
        iNotificationsTsy->iSatReqHandleStore->ResetTsyReqHandle(
		CSatTsy::ESatNotifySetUpMenuPCmdReqType );
		
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_COMPLETENOTIFYL_2, "CSAT: CSatNotifySetUpMenu::CompleteNotifyL reqHandle is :%d", reqHandle );
					
	// Get ber tlv 
    CBerTlv berTlv;
    berTlv.SetData( *data );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    // Store command details tlv
    iNotificationsTsy->iTerminalRespData.iCommandDetails.Copy( 
        commandDetails.Data() );
	// Get command qualifier
	TUint8 cmdQualifier( commandDetails.GetShortInfo(
		ETLV_CommandQualifier ) );
		
	TUint8 pCmdNumber( commandDetails.GetShortInfo( ETLV_CommandNumber ) );	
		
	// In case the request was ongoing, continue..
    if ( CSatTsy::ESatReqHandleUnknown != reqHandle )
        {
        // Complete right away if error has occured, otherwise continue..
        if ( KErrNone == aErrorCode )
            {			
			RSat::TSetUpMenuV2 menu;

			// Store transaction id
			menu.SetPCmdNumber( pCmdNumber );
    
			// Selection preference
			menu.iPreference = RSat::ESelectionPreferenceNotSet;
			if ( cmdQualifier & KSelectionUsingSoftKey )
				{
				menu.iPreference = RSat::ESoftKeyPreferred;
				}
			else
				{
				menu.iPreference = RSat::ENoSelectionPreference;
				}

			// Alpha Id string (optional)
			menu.iAlphaId.iAlphaId.Zero();
			// Get alpha identifier tlv
			CTlv alphaIdentifier;
			returnValue = berTlv.TlvByTagValue( &alphaIdentifier, 
			    KTlvAlphaIdentifierTag );

			if ( KErrNone != returnValue )
				{
				CreateTerminalRespL( pCmdNumber,
					RSat::KErrorRequiredValuesMissing, KNullDesC16 );

				ret = KErrCorrupt;
				}
			else
				{
    
				TUint16 alphaIdLength = alphaIdentifier.GetLength();

				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_COMPLETENOTIFYL_3, "CSAT: Alpha ID length:%d", alphaIdLength );

				if ( RSat::KAlphaIdMaxSize < alphaIdLength )
					{
					// String too long
					ret = KErrCorrupt;
					}
				else if ( alphaIdLength )
					{
					TPtrC8 temp;
					// Get the alpha id
					temp.Set( alphaIdentifier.GetData( ETLV_AlphaIdentifier ) );
					// Convert and set the alpha id
					TSatUtility::SetAlphaId( temp , menu.iAlphaId.iAlphaId ); 

					// Set SAT toolikit name, send SAT SMS logging purpose
					iToolKitName.Copy( menu.iAlphaId.iAlphaId );

					OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_COMPLETENOTIFYL_4, "CSAT: CSatNotifySetUpMenu::CompleteNotifyL, iToolKitName:%S", iToolKitName );
					}
				else
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_COMPLETENOTIFYL_5, "CSAT: CSatNotifySetUpMenu::CompleteNotifyL, Wrong length of alpha id.");
					}

				// Alpha Id status
				if ( menu.iAlphaId.iAlphaId.Length() )
					{
					menu.iAlphaId.iStatus = RSat::EAlphaIdProvided;
					}
				else
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_COMPLETENOTIFYL_6, "CSAT: CSatNotifySetUpMenu::CompleteNotifyL, Alpha ID is NULL");
					menu.iAlphaId.iStatus = RSat::EAlphaIdNull;
					}

				// Help information.
				if ( commandDetails.GetShortInfo( ETLV_CommandQualifier ) 
					& KHelpAvailabilityMask )
					{
					// Help information available
					menu.iHelp = RSat::EHelpAvailable;
					}
				else
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_COMPLETENOTIFYL_7, "CSAT: CSatNotifySetUpMenu::CompleteNotifyL, No Help available");
					// No help
					menu.iHelp = RSat::ENoHelpAvailable;
					}

				// Icon identifier
				TSatUtility::FillIconStructure( berTlv, menu.iIconId );      
				}

            // Item Icon identifier list
            CTlv itemsIconIdentifierList;
            TInt retValue = berTlv.TlvByTagValue( &itemsIconIdentifierList, 
                KTlvItemIconIdentifierListTag );

            TPtrC8 iconIdList;

            menu.iIconListQualifier = RSat::EIconQualifierNotSet;

            if ( KErrNone == retValue )
                {
                TUint8 iIconListQualifier( itemsIconIdentifierList.GetShortInfo( 
                    ETLV_IconListQualifier ) );
                iconIdList.Set( itemsIconIdentifierList.GetData( 
                    ETLV_IconIdentifierList ));

                if( iIconListQualifier )
                    {
                    menu.iIconListQualifier = RSat::ENotSelfExplanatory;
                    }
                else
                    {
                    menu.iIconListQualifier = RSat::ESelfExplanatory;
                    }

                }
            else if ( KErrNotFound == retValue )
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_COMPLETENOTIFYL_8, "CSAT: CSatNotifySetUpMenu::CompleteNotifyL, No Icon ID");
                menu.iIconListQualifier = RSat::ENoIconId;
                }
            else
            	{
	            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_COMPLETENOTIFYL_9, "CSAT: CSatNotifySetUpMenu::CompleteNotifyL, Wrong return value of icon identifier list.");
           		}	
           	
	        //Items Data
	        TInt itemNbr( 0 );
	        CTlv itemsData;
	        returnValue = berTlv.TlvByTagValue( 
	        	&itemsData, KTlvItemTag, itemNbr );
	        TUint8 numberOfItemData( 0 );                          
	        TUint16 stringLength( 0 );
	        
	        if( !returnValue )
	            {
	            CTlv itemsDataTemp;
	        	stringLength = ( itemsData.GetLength());
	            TInt ret( KErrNone );
	            // Set numberOfItemData                        
		        while ( KErrNone == ret )
		        	{
		        	numberOfItemData++; 
		        	ret = berTlv.TlvByTagValue( &itemsDataTemp, KTlvItemTag,
		        		numberOfItemData ); 
		        	}
	            }

            // Item Next Action Indicator
            CTlv itemNextActionIndicator;
            retValue = berTlv.TlvByTagValue( &itemNextActionIndicator, 
                                             KTlvItemsNextActionIndicatorTag );
            TPtrC8 itemNextIndicator;

            if ( KErrNone == retValue )
                {
	            TPtrC8 itemNextIndicatorTemp;
	            itemNextIndicatorTemp.Set( itemNextActionIndicator.GetData( 
	            	ETLV_ItemsNextActionIndicator ) );
	            
	            // In case the number of items in this list does not match the 
	            // number of items in the menu the Items Next Action Indicator 
	            // list is ignored by ME
	            if( itemNextIndicatorTemp.Length() != numberOfItemData )
	            	{
	            	iItemsNextIndicatorRemoved = ETrue;
	            	}
	            else
	           		{
	            	itemNextIndicator.Set( itemNextIndicatorTemp );	
	           		}
	            }

            // If first item is NULL -> remove existing menu
            if ( stringLength != 0 )
                {
                for ( TUint8 i = 0; ( KErrNone == ret ) 
                    && ( KErrNone == returnValue ) ; i++ )
                    {
                    // Filling up the menu items
                    RSat::TItem newItem;
                                     
                    // Suffle through all the menu items
                    stringLength = 0;

                    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_COMPLETENOTIFYL_10, "CSAT: CSatNotifySetUpMenu::CompleteNotifyL, item number:%d", i );
                    
                    // Fill the new item
                    newItem.iItemId = itemsData.GetShortInfo( 
                        ETLV_IdentifierOfItem );
                    stringLength = itemsData.GetLength();
                    stringLength--;

                    TPtrC8 itemData = itemsData.GetData( 
                        ETLV_TextStringOfItem );

                    // Menu item string, coded as EFadn (3gpp 11.11)
                    newItem.iItemString.Zero();

                    if ( ( KUCS2ArabicCoding == itemData[0] )
                        || ( KUCS2GreekCoding == itemData[0] )
                        || ( KUCS2TurkishCoding == itemData[0] ) )
                        {
                        TSatUtility::ConvertAlphaFieldsToUnicode( itemData, 
                            newItem.iItemString );
                        }
                    else
                        {
                        TSatUtility::Convert7BitToUnicode16( itemData,
                            newItem.iItemString );
                        }
            
                    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_COMPLETENOTIFYL_11, "CSAT: CSatNotifySetUpMenu::CompleteNotifyL newItem.iItemString:%S", newItem.iItemString );

	                // Adding the new menuitem
	                if( NULL != iconIdList.Size()  && ( i < iconIdList.Length() ) )
	                    {
	                    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_COMPLETENOTIFYL_12, "CSAT: CSatNotifySetUpMenu::CompleteNotifyL Icons on the list" );
	                    if( ( NULL != itemNextIndicator.Size() ) 
	                        && ( i < itemNextIndicator.Length() ) )
	                        {
	                        // Menu item with item next idicator and icon identifier
	                        if ( KErrNoMemory == menu.AddItem( newItem, 
	                             itemNextIndicator[i], iconIdList[i] ) )
	                            {
	                            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_COMPLETENOTIFYL_13, "CSAT: CSatNotifySetUpMenu::CompleteNotifyL, Menu item length exceeded");
	                            // Too many or long menu items
	                            ret = KErrCorrupt;
	                            }
	                        }
	                    // Menu item with icon identifier
	                    else if ( KErrNoMemory == menu.AddItemIcon( newItem, 
	                        iconIdList[i] ) )
	                        {
	                        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_COMPLETENOTIFYL_14, "CSAT: CSatNotifySetUpMenu::CompleteNotifyL, Menu item length exceeded");
	                        // Too many or long menu items
	                        ret = KErrCorrupt;
	                        }
	                    }
	                // No icon on the list    
	                else 
	                    {
	                    if( ( NULL != itemNextIndicator.Size() ) 
	                        && ( i < itemNextIndicator.Length() ) )
	                        {
	                        // Menu item with item next indicator
	                        if ( KErrNoMemory == menu.AddItem( newItem, 
	                            itemNextIndicator[i] ) )
	                            {
	                            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_COMPLETENOTIFYL_15, "CSAT: CSatNotifySetUpMenu::CompleteNotifyL, Menu item length exceeded");
	                            // Too many or long menu items
	                            ret = KErrCorrupt;
	                            }
	                        }
	                    // Menu item
	                    else 
	                        {
	                        TInt retAdd = menu.AddItem( newItem );
	                        if ( KErrNoMemory == retAdd )
	                            {
	                            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_COMPLETENOTIFYL_16, "CSAT: CSatNotifySetUpMenu::CompleteNotifyL, Menu item length exceeded");
	                            // Too many or long menu items
	                            // If there is not enough space left in the buffer used 
	                            // by the menu KErrNoMemory is returned.
	                            ret = KErrCorrupt;
	                            // Send terminal response
	                            CreateTerminalRespL( pCmdNumber,
	                                    RSat::KCmdDataNotUnderstood, 
	                                    KNullDesC16 );
	                            }
	                        }
	                    }

                    itemNbr++;
                    returnValue = berTlv.TlvByTagValue( &itemsData, 
                        KTlvItemTag, itemNbr );
                    } // End of for
                }

            // Set the data for the client
            RSat::TSetUpMenuV2& setUpMenuV2 = ( *iSetUpMenuV2Pckg )();
            setUpMenuV2 = menu;
			} // End of if (KErrNone == aErrorCode)
        else
        	{
        	ret = aErrorCode;
        	}
        
		// Complete to the client side
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_COMPLETENOTIFYL_17, "CSAT: Completing CSatNotifySetUpMenu: error %d",aErrorCode );
		iNotificationsTsy->iSatTsy->ReqCompleted( reqHandle, ret );
		} 
    else 
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_COMPLETENOTIFYL_18, "CSAT: CSatNotifySetUpMenu::CompleteNotifyL, Request not ongoing");
		TBuf16<1>additionalInfo;
        additionalInfo.Append ( RSat::KNoSpecificMeProblem );
		CreateTerminalRespL( pCmdNumber,                         
			RSat::KMeUnableToProcessCmd, additionalInfo );		
        }
	
    return ret;
	}
// -----------------------------------------------------------------------------
// CSatNotifySetUpMenu::TerminalResponseL
// Called by ETel server, passes terminal response to DOS
// -----------------------------------------------------------------------------
//
TInt CSatNotifySetUpMenu::TerminalResponseL
        ( 
        TDes8* aRsp
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_TERMINALRESPONSEL_1, "CSAT: CSatNotifySetUpMenu::TerminalResponseL");

    TInt ret( KErrNone );
    TBuf16<1> additionalInfo;
    RSat::TSetUpMenuRspV1Pckg* aRspPckg = 
            reinterpret_cast<RSat::TSetUpMenuRspV1Pckg*> ( aRsp );
    RSat::TSetUpMenuRspV1& rspV1 = ( *aRspPckg ) ();
    TUint8 pCmdNumber( rspV1.PCmdNumber() );

    // Check that general result value is valid
    if ( ( RSat::KSuccess != rspV1.iGeneralResult ) 
        && ( RSat::KMeUnableToProcessCmd != rspV1.iGeneralResult )
        && ( RSat::KCmdBeyondMeCapabilities != rspV1.iGeneralResult ) 
        && ( RSat::KCmdDataNotUnderstood != rspV1.iGeneralResult )
        && ( RSat::KErrorRequiredValuesMissing != rspV1.iGeneralResult)
        && ( RSat::KSuccessRequestedIconNotDisplayed != 
        	rspV1.iGeneralResult ) )
        {
        // Invalid general result
        ret = KErrCorrupt;
        }

    // If there is Me (Mobile Entity) error, additional info is needed
    if ( RSat::KMeProblem == rspV1.iInfoType )
        {
        // Check the length of additional info
        if ( rspV1.iAdditionalInfo.Length() != 0 )
            {
            additionalInfo.Append( rspV1.iAdditionalInfo[0] );
			}
        else
            {
            // Invalid additional info field
            ret = KErrCorrupt;
            }
        }
        
    if( RSat::KSuccess == rspV1.iGeneralResult && iItemsNextIndicatorRemoved )
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_TERMINALRESPONSEL_2, "CSatNotifySetUpMenu::TerminalResponseL, iItemsNextIndicatorRemoved");
		rspV1.iGeneralResult = RSat::KPartialComprehension;
		}
		
    iItemsNextIndicatorRemoved = EFalse;

	CreateTerminalRespL( pCmdNumber, static_cast<TUint8>( 
		rspV1.iGeneralResult ), additionalInfo );                            
                         
    return ret;
    }

// ------------------------------------------------------------------
// CSatNotifySetUpMenu::CreateTerminalRespL
// Constructs SetUpMenu specific part of terminal response and calls 
// DOS to send the actual message.
// ------------------------------------------------------------------
//
TInt CSatNotifySetUpMenu::CreateTerminalRespL
        ( 
        TUint8 aPCmdNumber,         
        TUint8 aGeneralResult,     
        const TDesC16& aAdditionalInfo		
		)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSETUPMENU_CREATETERMINALRESPL_1, "CSAT: CSatNotifySetUpMenu::CreateTerminalRespL");
    // Create and append response data
    TTlv tlvSpecificData;
    // Create General Result TLV here
    tlvSpecificData.AddTag( KTlvResultTag );
    // General result
    tlvSpecificData.AddByte( aGeneralResult ); 
	
	if ( RSat::KMeUnableToProcessCmd == aGeneralResult )
        {
        if ( aAdditionalInfo.Length() > 0 )
        	{
        	tlvSpecificData.AddByte( static_cast<TUint8>( aAdditionalInfo[0] ) );
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
    
// End of file

