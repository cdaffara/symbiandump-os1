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
// Name        : CSatNotifySelectItem.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// SelectItem notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifySelectItemTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatTsy.h"                // Tsy class header
#include "CSatNotifySelectItem.h"   // Tsy class header
#include "CSatNotificationsTsy.h"   // Class header
#include "CBerTlv.h"                // Ber Tlv data handling
#include "TTlv.h"					// TTlv class
#include "CSatDataPackage.h"        // Parameter packing 
#include "TSatUtility.h"            // Utilities
#include "CSatTsyReqHandleStore.h"  // Request handle class
#include "cmmmessagemanagerbase.h"  // Message manager class for forwarding req.

// -----------------------------------------------------------------------------
// CSatNotifySelectItem::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CSatNotifySelectItem* CSatNotifySelectItem::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_NEWL_1, "CSAT: CSatNotifySelectItem::NewL");
   	CSatNotifySelectItem* const satNotifySelectItem = 
        new ( ELeave ) CSatNotifySelectItem( aNotificationsTsy );
    CleanupStack::PushL( satNotifySelectItem );
    satNotifySelectItem->ConstructL();
    CleanupStack::Pop( satNotifySelectItem );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_NEWL_2, "CSAT: CSatNotifySelectItem::NewL, end of method");
    return satNotifySelectItem;
    }

// -----------------------------------------------------------------------------
// CSatNotifySelectItem::~CSatNotifySelectItem
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifySelectItem::~CSatNotifySelectItem
        ( 
		// None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_DTOR_1, "CSAT: CSatNotifySelectItem::~CSatNotifySelectItem");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifySelectItem::CSatNotifySelectItem
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifySelectItem::CSatNotifySelectItem
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy ), iItemsIconIdListRemoved ( EFalse )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifySelectItem::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifySelectItem::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_CONSTRUCTL_1, "CSAT: CSatNotifySelectItem::ConstructL");
    // Initiliaze Item Next Indicator flag
    iItemNextIndicatorRemoved = EFalse;
    }

// -----------------------------------------------------------------------------
// CSatNotifySelectItem::Notify
// This request allows a client to be notified of a SELECT ITEM proactive 
// command
// -----------------------------------------------------------------------------
//
TInt CSatNotifySelectItem::Notify
        (
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage    
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_NOTIFY_1, "CSAT: CSatNotifySelectItem::Notify");
    // Save data pointer to client side for completion
    iSelectItemV2Pckg = reinterpret_cast<RSat::TSelectItemV2Pckg*>( 
        aPackage.Des1n() );
    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifySelectItemPCmdReqType );
    // Check if requested notification is already pending
    iNotificationsTsy->NotifySatReadyForNotification( KSelectItem );   
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatNotifySelectItem::CancelNotification
// This method cancels an outstanding asynchronous 
// NotifyDisplayText request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifySelectItem::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_CANCELNOTIFICATION_1, "CSAT: CSatNotifySelectItem::CancelNotification");
    // Reset the request handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
        ResetTsyReqHandle( CSatTsy::ESatNotifySelectItemPCmdReqType );
	// Reset the data pointer
	iSelectItemV2Pckg = NULL;
	// Complete the request with KErrCancel
	iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;    
    }


// -----------------------------------------------------------------------------
// CSatNotifySelectItem::CompleteNotifyL
// Complete Sim Session End notification to the client
// -----------------------------------------------------------------------------
// 
TInt CSatNotifySelectItem::CompleteNotifyL
        (
		CSatDataPackage* aDataPackage, 
		TInt aErrorCode
        )
    {
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_COMPLETENOTIFYL_1, "CSAT: CSatNotifySelectItem::CompleteNotifyL");
	TInt ret( KErrNone );
    // Unpack parameters
    TPtrC8* data;
    aDataPackage->UnPackData( &data );
	TBuf<1> additionalInfo;
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
    	ResetTsyReqHandle( CSatTsy::ESatNotifySelectItemPCmdReqType );
    
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
			// Fill the select item structure
			RSat::TSelectItemV2& selectItemV2 = ( *iSelectItemV2Pckg )();
			selectItemV2.Reset();

			// Store Transaction id place of command number
			// This is done because we keep interface same as in Calimero.
			// Client returns back same value in response package.
			selectItemV2.SetPCmdNumber( pCmdNumber );

			TPtrC8 sourceString; // Used in unicode conversions

			// Alpha id string (optional)
			selectItemV2.iAlphaId.iAlphaId.Zero();
			CTlv alphaIdentifier;
			TInt returnValue( berTlv.TlvByTagValue( &alphaIdentifier, 
				KTlvAlphaIdentifierTag ) );
			// If alpha id string exist
			if ( KErrNone == returnValue ) 
				{
				TUint16 alphaIdLength( alphaIdentifier.GetLength() );
        
			   if ( alphaIdLength )
					{
					// Get the alpha id
					sourceString.Set( alphaIdentifier.GetData( 
						ETLV_AlphaIdentifier ) );
                
					// Convert and set the alpha id
					TSatUtility::SetAlphaId( sourceString,
					    selectItemV2.iAlphaId.iAlphaId );
					}

				// Check alpha tag (id) status
				if ( alphaIdLength )
					{
					// Alpha tag present
					if ( selectItemV2.iAlphaId.iAlphaId.Length() )
						{
						selectItemV2.iAlphaId.iStatus = RSat::EAlphaIdProvided;
						}
					else
						{
						OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_COMPLETENOTIFYL_2, "CSAT: CSatNotifySelectItem::CompleteNotifyL, Alpha ID is NULL");
						selectItemV2.iAlphaId.iStatus = RSat::EAlphaIdNull;
						}
					}
				}
			// Alpha id not present
			else 
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_COMPLETENOTIFYL_3, "CSAT: CSatNotifySelectItem::CompleteNotifyL Alpha ID not present");
				selectItemV2.iAlphaId.iStatus = RSat::EAlphaIdNotPresent;
				}
			// Help information
			if ( KHelpAvailabilityMask & cmdQualifier )
				{
				// Help information available
				selectItemV2.iHelp = RSat::EHelpAvailable;
				}
			else
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_COMPLETENOTIFYL_4, "CSAT: CSatNotifySelectItem::CompleteNotifyL, No Help available");
				// No help
				selectItemV2.iHelp = RSat::ENoHelpAvailable;
				}
			// Presentation as a choice of navigation options 
			if ( KSelectItemChoiceOfNavigation == ( KSelectItemPresentationTypeMask & cmdQualifier ) )
				{
				selectItemV2.iPresentationType = RSat::ENavigationOptions;
				}
			// Presentation as a choice of data values 
			else if ( KSelectItemChoiceOfData == ( KSelectItemPresentationTypeMask & cmdQualifier ) )
				{
				selectItemV2.iPresentationType = RSat::EDataValues;
				}
			else
				{
				selectItemV2.iPresentationType = RSat::ENotSpecified;
				}
			// Selection preference
			if ( KSelectItemSelectUsingSoftKeyMask & cmdQualifier ) 
				{
				// Preference = soft key
				selectItemV2.iPreference = RSat::ESoftKeyPreferred;
				}
			else
				{
				// Preference not set
				selectItemV2.iPreference = RSat::ESelectionPreferenceNotSet;
				}
			// Default item id ( optional )
			CTlv defaultItem;
			returnValue = berTlv.TlvByTagValue( &defaultItem, 
			    KTlvItemIdentifierTag );

			if ( KErrNone == returnValue ) 
				{
				selectItemV2.iDefaultItemId = 
					defaultItem.GetShortInfo( ETLV_IdentifierOfItemChosen );
				}
        
			// Icon qualifier list ( optional )
			// Each item of a list of items has an icon identifier coded on one 
			// byte. The length of the Items icon identifier list shall be the 
			// number of items of the list of items (X-1 shall be the number of 
			// items in the list). The order of each item icon identifier, 
			// shall reflect the order of the items in the list of items.
			// Each icon identifier addresses a record 
			// in EFIMG as defined in TS 11.11 

			CTlv itemsIconIdentifierList;
			TInt retValue( berTlv.TlvByTagValue( &itemsIconIdentifierList, 
												 KTlvItemIconIdentifierListTag ) );

			selectItemV2.iIconListQualifier = RSat::EIconQualifierNotSet;
			TPtrC8 iconIdList;
			if ( KErrNone == retValue )
				{
				TUint8 iconListQualifier( itemsIconIdentifierList.GetShortInfo( 
					ETLV_IconListQualifier ) );
				// Set iconIdList pointer
				iconIdList.Set( 
					itemsIconIdentifierList.GetData( ETLV_IconIdentifierList ));
				// The icon list qualifier indicates to the ME how 
				// the icons are to be used
				if( iconListQualifier )
					{
					// icon is not self-explanatory, i.e. if displayed, 
					// it shall be displayed together with the item text
					selectItemV2.iIconListQualifier = RSat::ENotSelfExplanatory;
					}
				else
					{
					// icon is self-explanatory, i.e. if displayed, 
					// it replaces the item text
					selectItemV2.iIconListQualifier = RSat::ESelfExplanatory;
					}
				}
			else if ( KErrNotFound == retValue )
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_COMPLETENOTIFYL_5, "CSAT: CSatNotifySelectItem::CompleteNotifyL, No Icon ID");
				selectItemV2.iIconListQualifier = RSat::ENoIconId;
				}
			else
				{
        		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_COMPLETENOTIFYL_6, "CSAT: CSatNotifySelectItem::CompleteNotifyL, Return value not valid.");
				}
             
            ret = HandleItemsL( pCmdNumber, &berTlv, ret, iconIdList, 
            	&selectItemV2 );
                        
			// Iconid 
			TSatUtility::FillIconStructure( berTlv, selectItemV2.iIconId );

            } // End of if ( KErrNone == aErrorCode )
        else
        	{
        	ret = aErrorCode;
        	}
        iNotificationsTsy->iSatTsy->ReqCompleted( reqHandle, ret );

        } // End of if ( CSatTsy::ESatReqHandleUnknown != reqHandle )
	else 
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_COMPLETENOTIFYL_7, "CSAT: CSatNotifySelectItem::CompleteNotifyL, Request not ongoing");
        // Request not on, returning response immediately       
		additionalInfo.Zero();
        additionalInfo.Append( KNoCause );
		CreateTerminalRespL( pCmdNumber, RSat::KMeUnableToProcessCmd,        
			additionalInfo );					
    	}
	
	return ret;
	}

// -----------------------------------------------------------------------------
// CSatNotifySelectItem::TerminalResponseL
// Called by ETel server, passes terminal response to DOS
// -----------------------------------------------------------------------------
//
TInt CSatNotifySelectItem::TerminalResponseL
        ( 
        TDes8* aRsp 
        ) 
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_TERMINALRESPONSEL_1, "CSAT:: CSatNotifySelectItem::TerminalResponseL");

    TInt   ret( KErrNone );
    TBuf<1> additionalInfo;
    RSat::TSelectItemRspV1Pckg* aRspPckg = 
            reinterpret_cast<RSat::TSelectItemRspV1Pckg*> ( aRsp );
    RSat::TSelectItemRspV1& rspV1 = ( *aRspPckg ) ();	
	// Get Proactive command number
	TUint8 pCmdNumber( rspV1.PCmdNumber() );
   
    // Check that general result values are valid
    if ( ( RSat::KSuccess != rspV1.iGeneralResult ) 
        && ( RSat::KSuccessRequestedIconNotDisplayed != rspV1.iGeneralResult )
        && ( RSat::KPSessionTerminatedByUser != rspV1.iGeneralResult )
        && ( RSat::KBackwardModeRequestedByUser != rspV1.iGeneralResult )
        && ( RSat::KNoResponseFromUser != rspV1.iGeneralResult )
        && ( RSat::KHelpRequestedByUser != rspV1.iGeneralResult )
        && ( RSat::KMeUnableToProcessCmd != rspV1.iGeneralResult )
        && ( RSat::KCmdBeyondMeCapabilities != rspV1.iGeneralResult ) )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_TERMINALRESPONSEL_2, "CSAT: CSatNotifySelectItem::CompleteNotifyL, Invalid General Result");
        ret = KErrCorrupt;
        }
    // If there is ME (Mobile Entity) error or there there should be 
    // item identifier or
    // general result is success, additional info is needed
    if ( ( RSat::KMeProblem == rspV1.iInfoType )
        || ( RSat::KItemIdentifier == rspV1.iInfoType ) 
        || ( RSat::KSuccess == rspV1.iGeneralResult ) )
        {
        // Check the length of additional info
        if ( rspV1.iAdditionalInfo.Length() != 0 )
            {
            OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_TERMINALRESPONSEL_3, "CSAT: CSatNotifySelectItem::TerminalResponseL, Additional info: %S", rspV1.iAdditionalInfo);
            additionalInfo.Zero();
            additionalInfo.Append( rspV1.iAdditionalInfo[0] );
            OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_TERMINALRESPONSEL_4, "CSAT: CSatNotifySelectItem::TerminalResponseL, Additional info: %S", additionalInfo);
			}
        else
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_TERMINALRESPONSEL_5, "CSAT: CSatNotifySelectItem::CompleteNotifyL, Invalid Additional Info");
            ret = KErrCorrupt;
            }
        }
        
    if ( RSat::KSuccess == rspV1.iGeneralResult ) 
    	{
    	if (iItemNextIndicatorRemoved)
    		{
    		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_TERMINALRESPONSEL_6, "CSatNotifySelectItem::TerminalResponseL, Partial Comprehension, iItemNextIndicatorRemoved");
        	rspV1.iGeneralResult = RSat::KPartialComprehension;
    		}
    	if (iItemsIconIdListRemoved)
    		{
        	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_TERMINALRESPONSEL_7, "CSatNotifySelectItem::TerminalResponseL, Partial Comprehension, iItemsIconIdListRemoved");
        	rspV1.iGeneralResult = RSat::KPartialComprehension;
    		}
    	}
    	
    iItemNextIndicatorRemoved = EFalse;
    iItemsIconIdListRemoved = EFalse;
    
    CreateTerminalRespL( pCmdNumber, static_cast<TUint8>( 
        rspV1.iGeneralResult ), additionalInfo );                            
	
	return ret;
    }


// -----------------------------------------------------------------------------
// CSatNotifySelectItem::CreateTerminalRespL
// Constructs SelectItem specific part of terminal response and calls 
// DOS to send the actual message.
// -----------------------------------------------------------------------------
//
TInt CSatNotifySelectItem::CreateTerminalRespL
	    ( 
    	TUint8 aPCmdNumber,         
	    TUint8 aGeneralResult,      
	    TDesC16& aAdditionalInfo			
	    )	
	{	    
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_CREATETERMINALRESPL_1, "CSAT: CSatNotifySelectItem::CreateTerminalRespL, Additional info: %S", aAdditionalInfo);

	TTlv tlvSpecificData;    
    // Create General Result TLV here
    tlvSpecificData.AddTag( KTlvResultTag );    
    // General result
    tlvSpecificData.AddByte( aGeneralResult );
        
    if ( !( iNotificationsTsy->CommandPerformedSuccessfully( aGeneralResult ) ) )
        {
        switch ( aGeneralResult )
            {
            case RSat::KHelpRequestedByUser:   
                {
                // Add selected Item Id tag
                tlvSpecificData.AddTag( KTlvItemIdentifierTag );
                tlvSpecificData.AddByte( ( TUint8 ) aAdditionalInfo[0] ); 
				break;
                }
            case RSat::KPSessionTerminatedByUser:
            case RSat::KBackwardModeRequestedByUser:
            case RSat::KNoResponseFromUser:
                {
                break;
                }
            default:
                {
                if (aAdditionalInfo.Length() > 0)
                	{
                	tlvSpecificData.AddByte( ( TUint8 ) aAdditionalInfo[0] );
                	}
                break;
                }
            }
        }
    else
        {
        // Add selected Item Id tag
        tlvSpecificData.AddTag( KTlvItemIdentifierTag );
        tlvSpecificData.AddByte( ( TUint8 ) aAdditionalInfo[0] );
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
// CSatNotifySelectItem::HandleItemsL
// Handle menu items
// -----------------------------------------------------------------------------
//
TInt CSatNotifySelectItem::HandleItemsL
		(
		TUint8 aPCmdNumber,
		CBerTlv* aBerTlv,
		TInt aRet,
		TPtrC8 aIconIdList,
		RSat::TSelectItemV2* aSelectItemV2
		)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_HANDLEITEMSL_1, "CSAT: CSatNotifySelectItem::HandleItemsL");
	TInt ret( aRet );

	CTlv item;
	// Get all items
	TInt returnValue = aBerTlv->TlvByTagValueMulti( &item, KTlvItemTag );

	TPtrC8 itemData = item.Data();
	TUint8 numberOfItemData( 0 );
	TInt ind( 0 );
	TUint16 tlvLength( 0 );
	TInt currentTlv( 0 );
	TInt i( 0 );
	
	// Set numberOfItemData
	for ( i = 0; i < itemData.Length(); i += ( TInt ) tlvLength )
		{
		// First determine if the length of the TLV is 
		// coded with 1 or 2 bytes.
		if ( KTwoByteLengthCoding == itemData[ind+1] )
			{
			// Length is coded with 2 bytes -> real length is in second 
			// byte first byte is 81 and it is "tag" for 2 byte length 
			// coding.
			tlvLength = ( TUint16 ) 
				( itemData[ind + 2] + KTlvHeaderLength + 1 );
			}        
		else
			{
			// TLV header bytes (2) must be added to total length
			tlvLength = ( TUint16 ) 
				( itemData[ind + 1]  + KTlvHeaderLength );
			}

		currentTlv = itemData[ind]&KTagValueMask;

		if ( KTlvItemTag == currentTlv )
			{
			numberOfItemData++;   
			}

		ind += tlvLength;
		}
		
    // See if itemnextindicator is included ( optional )
    CTlv nextIndicator;
                 
    returnValue = aBerTlv->TlvByTagValue( &nextIndicator, 
        KTlvItemsNextActionIndicatorTag );
        
    TPtrC8 itemNextIndicator;
    
    if ( KErrNone == returnValue ) 
        {
        TPtrC8 itemNextIndicatorTemp;
        itemNextIndicatorTemp.Set( nextIndicator.GetData( 
            ETLV_ItemsNextActionIndicator ) );
            
        // In case the number of items in this list doesn't match the nr of 
        // items in the menu, Items Next Action Indicator list is ignored by ME
        if( itemNextIndicatorTemp.Length() != numberOfItemData )
        	{
        	iItemNextIndicatorRemoved = ETrue;
        	}
        else
        	{
        	itemNextIndicator.Set( itemNextIndicatorTemp );
        	}
        }

    if ( ( aIconIdList.Length() > 0 ) && 
    	 ( aIconIdList.Length() != numberOfItemData ) )
    	{
        // In case the number of items in this list doesn't match the nr of 
        // items in the menu, Items Icon Identifier list is ignored by ME
    	iItemsIconIdListRemoved = ETrue;
    	aIconIdList.Set( KNullDesC8 ); 
    	}
    
	// Filling up the menu items
	RSat::TItem newItem;
	// Set string length and pos
	TUint8 stringLength( 0 );
	TUint8  pos( 2 );

	// LOOP Through ALL MENU ITEMS AND TAKE THE DATA
	for ( i = 0; ( i < numberOfItemData ) && ( KErrNone == ret ); i++ )
		{
		// Check if two byte length coding is used
		if ( KTwoByteLengthCoding == itemData[pos - 1] )
			{
			// Set new offset
			pos++;
			}
		// Fill the newitem
		newItem.iItemId = itemData[pos];
		stringLength = itemData[pos - 1];
		// Copying the string from 8-bit to 16-bit
		
		if ( RSat::KMenuItemMaxSize < stringLength )
			{
			// String too long
			ret = KErrCorrupt;
			}
			
		newItem.iItemString.Zero();
		// ArabicCoding, GreekCoding and TurkishCoding have different 
		// coding method. There is a tag for each type of alphabet 
		// (resp. 80, 81 or 82) before the text, and there are base 
		// pointers used for expanding 1 byte to 2 bytes as required
		// in UCS2  
		// Ref: 3gpp 11.11, Annex B
		
		// Check if string present
        if ( 0 < ( stringLength - 1 ) )
        	{

			TBuf8<RSat::KTextStringMaxSize> itemText;
			itemText = itemData.Mid( pos + 1, stringLength - 1 );
	    
			if ( ( KUCS2ArabicCoding == itemText[0] )
				|| ( KUCS2GreekCoding == itemText[0] )
				|| ( KUCS2TurkishCoding == itemText[0] ) )
				{
				TSatUtility::ConvertAlphaFieldsToUnicode( itemText, 
					newItem.iItemString );
				}
			else
				{
				// 8-bit format
				TSatUtility::Convert7BitToUnicode16( itemText, 
				    newItem.iItemString );
				}       
        	}
        	
		if ( ( NULL != aIconIdList.Size() )
			&& ( i < aIconIdList.Length() ) )
			{
			if ( ( NULL != itemNextIndicator.Size() ) 
				&& ( i < itemNextIndicator.Length() ) )
				{
				// ADD ITEM WITH ITEM NEXT INDICATOR AND ICON IDENTIFIER
				if ( KErrNoMemory == aSelectItemV2->AddItem( 
					newItem, itemNextIndicator[i], aIconIdList[i] ) )
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_HANDLEITEMSL_2, "CSAT: CSatNotifySelectItem::HandleItemsL, Icon data length exceeded, AddItem1");
					// Too many or long menu items
					ret = KErrCorrupt;
					}
				}
			// ADD ITEM WITH ICON
			else if ( KErrNoMemory == aSelectItemV2->AddItemIcon( 
				newItem, aIconIdList[i] ) )
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_HANDLEITEMSL_3, "CSAT: CSatNotifySelectItem::HandleItemsL, Icon data length exceeded, AddItemIcon");
				// Too many or long menu items
				ret = KErrCorrupt;
				}
			}
		else 
			{
			if ( ( NULL != itemNextIndicator.Size() ) 
				&& ( i < itemNextIndicator.Length() ) )
				{
				// ADD ITEM WITH ITEM NEXT INDICATOR
				if ( KErrNoMemory == aSelectItemV2->AddItem( 
					newItem, itemNextIndicator[i] ) )
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_HANDLEITEMSL_4, "CSAT: CSatNotifySelectItem::HandleItemsL, Icon data length exceeded, AddItem2");
					// Too many or long menu items
					ret = KErrCorrupt;
					}
				}
			//ADD ITEM
			else 
				{
				TInt retAdd = aSelectItemV2->AddItem( newItem );
				if ( KErrNoMemory == retAdd ) 
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYSELECTITEM_HANDLEITEMSL_5, "CSAT: CSatNotifySelectItem::HandleItemsL, AddItem failed -> KErrNoMemory");
					// Too many or long menu items
					// If there is not enough space left in the buffer used
					// by the menu KErrNoMemory is returned.
					TBuf<1> additionalInfo;
					additionalInfo.Zero();
                    additionalInfo.Append( KNoCause );
					// Send terminal response
					CreateTerminalRespL( aPCmdNumber, 
					    RSat::KCmdDataNotUnderstood, additionalInfo );
					ret = KErrCorrupt;
					}
				}
			}

		// Calculating the new position
		pos = TUint8( pos + itemData[pos - 1] + 2 );
		}
		
	return ret;
	}
			
// End of File
