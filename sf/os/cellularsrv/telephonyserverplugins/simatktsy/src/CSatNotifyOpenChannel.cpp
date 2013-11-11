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
// Name        : CSatNotifyOpenChannel.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Open channel notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifyOpenChannelTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatTsy.h"                // Tsy class header
#include "CSatNotifyOpenChannel.h"  // Class header
#include "CSatNotificationsTsy.h"   // Tsy class header
#include "CBerTlv.h"                // Ber Tlv data handling
#include "TTlv.h"					// TTlv class
#include "CSatDataPackage.h"        // Parameter packing 
#include "TSatUtility.h"            // Utilities
#include "CSatTsyReqHandleStore.h"  // Request handle class
#include "cmmmessagemanagerbase.h"  // Message manager class for forwarding req.

// -----------------------------------------------------------------------------
// CSatNotifyOpenChannel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CSatNotifyOpenChannel* CSatNotifyOpenChannel::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_NEWL_1, "CSAT: CSatNotifyOpenChannel::NewL");
   	CSatNotifyOpenChannel* const satNotifyOpenChannel = 
        new ( ELeave ) CSatNotifyOpenChannel( aNotificationsTsy );
    CleanupStack::PushL( satNotifyOpenChannel );
    satNotifyOpenChannel->ConstructL();
    CleanupStack::Pop( satNotifyOpenChannel );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_NEWL_2, "CSAT: CSatNotifyOpenChannel::NewL, end of method");
    return satNotifyOpenChannel;
    }

// -----------------------------------------------------------------------------
// CSatNotifyOpenChannel::~CSatNotifyOpenChannel
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifyOpenChannel::~CSatNotifyOpenChannel
        ( 
		// None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_DTOR_1, "CSAT: CSatNotifyOpenChannel::~CSatNotifyOpenChannel");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyOpenChannel::CSatNotifyOpenChannel
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifyOpenChannel::CSatNotifyOpenChannel
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy ), iBearerType( 0 ),
        	iBearer( 0 ), iBufferSize( 0 )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyOpenChannel::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifyOpenChannel::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_CONSTRUCTL_1, "CSAT: CSatNotifyOpenChannel::ConstructL");
    }

// -----------------------------------------------------------------------------
// CSatNotifyOpenChannel::Notify
// This request allows a client to be notified of a OPEN CHANNEL proactive 
// command
// -----------------------------------------------------------------------------
//
TInt CSatNotifyOpenChannel::Notify
        (
        const TTsyReqHandle aTsyReqHandle,
        const TDataPackage& aPackage
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_NOTIFY_1, "CSAT: CSatNotifyOpenChannel::Notify");
    // Save data pointers to client side for completion
    iOpenBaseChannelV2Pckg = reinterpret_cast<RSat::TOpenChannelBaseV2Pckg*>( 
        aPackage.Des1n() );  
    iOpenCsChannelV2Pckg = reinterpret_cast<RSat::TOpenCsChannelV2Pckg*>( 
        aPackage.Des1n() );    
    iOpenGprsChannelV4Pckg = reinterpret_cast<RSat::TOpenGprsChannelV4Pckg*>( 
        aPackage.Des1n() );       
    iOpenLocalLinksChannelV2Pckg = reinterpret_cast<
        RSat::TOpenLocalLinksChannelV2Pckg*>( aPackage.Des1n() );     
    // Save the request handle
    iNotificationsTsy->iSatTsy->SaveReqHandle( aTsyReqHandle, 
		CSatTsy::ESatNotifyOpenChannelPCmdReqType );
    // Check if requested notification is already pending
    iNotificationsTsy->NotifySatReadyForNotification( KOpenChannel );   

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSatNotifyOpenChannel::CancelNotification
// This method cancels an outstanding asynchronous 
// NotifyOpenChannel request.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyOpenChannel::CancelNotification
        (
        const TTsyReqHandle aTsyReqHandle 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_CANCELNOTIFICATION_1, "CSAT: CSatNotifyOpenChannel::CancelNotification");
    
    // Reset the request handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
        ResetTsyReqHandle( CSatTsy::ESatNotifyOpenChannelPCmdReqType );
	// Reset the data pointers
	iOpenCsChannelV2Pckg = NULL;
	iOpenGprsChannelV4Pckg = NULL;
	iOpenLocalLinksChannelV2Pckg = NULL;	
    // Complete the request with KErrCancel
    iNotificationsTsy->iSatTsy->ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CSatNotifyOpenChannel::CompleteNotifyL
// This method completes an outstanding asynchronous 
// NotifyOpenChannel request. 
// -----------------------------------------------------------------------------
//
TInt CSatNotifyOpenChannel::CompleteNotifyL
        (
        CSatDataPackage* aDataPackage,   
        TInt aErrorCode                  
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_COMPLETENOTIFYL_1, "CSAT: CSatNotifyOpenChannel::CompleteNotifyL");

    TInt ret( KErrNone );	
    // Unpack parameters
    TPtrC8* data;
    aDataPackage->UnPackData( &data );
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iNotificationsTsy->iSatReqHandleStore->
        ResetTsyReqHandle( CSatTsy::ESatNotifyOpenChannelPCmdReqType );
        
    // Get ber tlv 
    CBerTlv berTlv;
    berTlv.SetData( *data );
    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    // Store command details tlv
    iNotificationsTsy->iTerminalRespData.iCommandDetails.Copy( 
        commandDetails.Data() );
    // Get proactive command number
    TUint8 pCmdNumber( commandDetails.GetShortInfo( ETLV_CommandNumber ) );
                            
    // In case the request was ongoing, continue..
    if ( CSatTsy::ESatReqHandleUnknown != reqHandle )
        {
        // Complete right away if error has occured, otherwise continue..
        if ( KErrNone == aErrorCode )
            {
        	RSat::TOpenChannelBaseV2* openChannelV2 = NULL; 
            
            // Mandatory: Bearer Description & Buffer Size
        	CTlv bearerDescription;
        	CTlv bufferSize;
       	    if ( KErrNone == berTlv.TlvByTagValue( 
        	     &bearerDescription, KTlvBearerDescriptionTag ) && 
            	 KErrNone == berTlv.TlvByTagValue( &bufferSize, 
            	    KTlvBufferSizeTag ) )
            	{
            	// Switch according to bearer
            	switch ( bearerDescription.GetShortInfo( ETLV_BearerType ) )
                	{
                	case KBipCsdBearer:
                    	{
                    	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_COMPLETENOTIFYL_2, "CSAT: CSatNotifyOpenChannel::CompleteNotifyL, Command not supported: Csd Bearer");
                    	// Buffer size
            	        iBufferSize = bufferSize.GetLongInfo( 
            	            ETLV_BufferSize );
                    	// Bearer description
                    	iBearerType = RSat::ECSDBearer;
                    	iBearer = bearerDescription.GetData( 
                    	    ETLV_BearerParameters ); 
                        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_COMPLETENOTIFYL_3, "CSAT: CSatNotifyOpenChannel::CompleteNotifyL, BipCsdBearer ");
                        CreateTerminalRespL( pCmdNumber, 
							RSat::KCmdBeyondMeCapabilities, KNullDesC16 );
                        ret = KErrCorrupt; 
        				break;
                    	}
               		case KBipGprsBearer:
                    	{
                    	// Fill in TOpenGprsChannelV4
                    	RSat::TOpenGprsChannelV4& openGprsChannelV4 
                        	= ( *iOpenGprsChannelV4Pckg )();
                    	openChannelV2 = &openGprsChannelV4;
                    	    
                    	openChannelV2->iPCmdType = RSat::EGprsBearer;
                    	// Network access name (Optional)
                    	CTlv nanTlv;
                    	if ( KErrNone == berTlv.TlvByTagValue( &nanTlv, 
                        	KTlvNetworkAccessNameTag ) )
                        	{
                        	openGprsChannelV4.iAccessName =  
                            	nanTlv.GetData( ETLV_NetworkAccessName );
                        	}
                    
                        // Text string (User Logging)
                        CTlv textStringUL;
                        TInt returnValue = berTlv.TlvByTagValue( 
                            &textStringUL, KTlvTextStringTag );
                        if ( KErrNone == returnValue )
                            {
                            // Convert and set text
                            TSatUtility::SetText( textStringUL, 
                                openGprsChannelV4.iUserLogin );
                            }

                        // Text string (User password)
                        CTlv textStringUP;
                        returnValue = berTlv.TlvByTagValue( &textStringUP, 
                            KTlvTextStringTag, 1 );
                        if ( KErrNone == returnValue )
                            {
                            // Convert and set text
                            TSatUtility::SetText( textStringUP, 
                                openGprsChannelV4.iUserPassword );
                            }
                        break;
                        }
        			case KBipDefaultBearer:
                    	{
                    	// Fill in TOpenChannelBaseV2
                    	openChannelV2 = &( *iOpenBaseChannelV2Pckg )();
                    	openChannelV2->iPCmdType = RSat::EAnyBearer;
                    	break;
                    	}
                	case KBipLocalLinkBearer:
                	default:
                    	{
                    	// Bearer not supported (RFU)
                    	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_COMPLETENOTIFYL_4, "CSAT: CSatNotifyOpenChannel::CompleteNotifyL, Bearer not supported");
                    	// Required values missing
                    	iBearer.FillZ( 1 );
                    	iBufferSize = 0;
                    	iBearerType = static_cast<TUint8>( 
                    	    RSat::EBearerTypeNotSet );
                    	CreateTerminalRespL( pCmdNumber,  
							RSat::KCmdDataNotUnderstood, KNullDesC16 );
                    	ret = KErrCorrupt;
                    	break;
                    	}
                	}    
            	}
        	else
            	{
            	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_COMPLETENOTIFYL_5, "CSAT: CSatNotifyOpenChannel::CompleteNotifyL, required values missing");
            	// Required values missing
            	iBearer.FillZ( 1 );
            	iBufferSize = 0;
            	iBearerType = static_cast<TUint8>( RSat::EBearerTypeNotSet );
            	CreateTerminalRespL( pCmdNumber, 
					RSat::KErrorRequiredValuesMissing, KNullDesC16 ); 
            	ret = KErrCorrupt;
            	}
        
        	if ( KErrNone == ret )
            	{
            	// Buffer size
            	openChannelV2->iBufferSize = bufferSize.GetLongInfo( 
            	    ETLV_BufferSize );
            	// Bearer description
            	openChannelV2->iBearer.iType = ( RSat::TBearerType ) 
            	    bearerDescription.GetShortInfo( ETLV_BearerType );
            	openChannelV2->iBearer.iParams = bearerDescription.GetData( 
            	    ETLV_BearerParameters ); 
                // Proactive command number
            	openChannelV2->SetPCmdNumber( pCmdNumber );

            	// Link Establishment, obtained from Command details Bit 1
            	// Get command qualifier
           		TUint8 cmdQualifier( commandDetails.GetShortInfo( 
                	ETLV_CommandQualifier ) );
            	if ( 0x01 & cmdQualifier )
                	{
                	openChannelV2->iLinkEst = RSat::EImmediate;
                	}
           		else
                	{
                	openChannelV2->iLinkEst = RSat::EOnDemand;
                	}
            	// Reconnection mode, obtained from Command details Bit 2 
            	if ( 0x02 & cmdQualifier )
                	{
                	openChannelV2->iReconnectionMode = 
                    	RSat::EAutomaticReconnection;
                	}
            	else
                	{
                	openChannelV2->iReconnectionMode = 
                    	RSat::ENoAutomaticReconnection;
                	}

            	// Alpha Id (Optional)
            	TPtrC8 sourceString; // Used with conversions
            	CTlv alphaIdentifier;
            	// Get alpha id tlv from berTlv
            	TInt returnValue( berTlv.TlvByTagValue( &alphaIdentifier, 
            	    KTlvAlphaIdentifierTag ) );
            	// If alpha id string exist
            	if ( KErrNone == returnValue ) 
                	{
                	if ( alphaIdentifier.GetLength() )
                    	{
                   		// 8-bit string to 16-bit string
                    	sourceString.Set( alphaIdentifier.GetData( 
                    	    ETLV_AlphaIdentifier ) );
                    	// Convert and set the alpha id
                    	TSatUtility::SetAlphaId( sourceString, 
                    	    openChannelV2->iAlphaId.iAlphaId ); 
                    	// Set alpha id status
                    	openChannelV2->iAlphaId.iStatus = RSat::EAlphaIdProvided;
                    	}
                	else
                    	{
                    	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_COMPLETENOTIFYL_6, "CSAT: CSatNotifyOpenChannel::CompleteNotifyL, Alpha ID is NULL");
                    	openChannelV2->iAlphaId.iStatus = RSat::EAlphaIdNull;
                    	}
               		}
            	// Alpha id not present
            	else
                	{
                	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_COMPLETENOTIFYL_7, "CSAT: CSatNotifyOpenChannel::CompleteNotifyL, Alpha ID not present");
                	openChannelV2->iAlphaId.iStatus = RSat::EAlphaIdNotPresent;
                	}


            	// Iconid (optional)
            	TSatUtility::FillIconStructure( berTlv, 
                	openChannelV2->iIconId );

            	// SIM/ME interface transport level
            	CTlv simMeInterfaceTlv;
            	returnValue = berTlv.TlvByTagValue( &simMeInterfaceTlv, 
                	KTlvSimMeTransportLevelTag );
                	
           		if ( KErrNone == returnValue )
                	{
                	openChannelV2->iSimMeInterface.iPrtNumber = 
                    	simMeInterfaceTlv.GetLongInfo( ETLV_PortNumber );
                	TUint8 protocol = simMeInterfaceTlv.GetShortInfo(
                    	ETLV_TransportProtocolType );
                    	
                	switch ( protocol )
                	    {
                	    case 0x01:
                        	{
                        	openChannelV2->iSimMeInterface.iTransportProto =
                            	RSat::EUdp;
                            break;
                        	}
                	    case 0x02:
                        	{
                        	openChannelV2->iSimMeInterface.iTransportProto =
                            	RSat::ETcp;
                            break;
                        	}
                	    default:
                        	{
                        	openChannelV2->iSimMeInterface.iTransportProto =
                            	RSat::EProtocolNotSet;
                        	}
                	    }
                	}
            	else
                	{
                	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_COMPLETENOTIFYL_8, "CSAT: CSatNotifyOpenChannel::CompleteNotifyL, Protocol not present");
                	openChannelV2->iSimMeInterface.iTransportProto =
                    	RSat::EProtocolNotPresent;
                	}

                // Data destination address
            	// The Data Destination Address is the end point destination 
            	// address of sent data. This data destination address is 
            	// requested when a SIM/ME interface transport is present, 
            	// otherwise it is ignored

            	// Determine the number of Other Address tags, if there is one
            	// and no  SIM/ME interface then this is a Local Address
            	// But if there is one and SIM/ME interface, this is a
            	// Destination address,
            	// And if there are 2, then the last one is the destination 
            	// address.

            	CTlv* dataDestAddressTlv=NULL;
            	CTlv* localAddressTlv=NULL;
            	CTlv localAddress1;
            	CTlv localAddress2;
            	TInt ret_laddr1 = berTlv.TlvByTagValue( &localAddress1, 
            	    KTlvOtherAddressTag );
            	TInt ret_laddr2 = berTlv.TlvByTagValue( &localAddress2, 
                	KTlvOtherAddressTag, 1 );                    
                    
            	if ( ( RSat::EProtocolNotPresent != 
                   	openChannelV2->iSimMeInterface.iTransportProto )
                 	&& ( KErrNone     == ret_laddr1 )
                 	&& ( KErrNotFound == ret_laddr2 ) )
                	{
                	// Destination Address is localAddress1, no local address
                	dataDestAddressTlv = &localAddress1;
                	}
            	else if ( ( RSat::EProtocolNotPresent != 
                   openChannelV2->iSimMeInterface.iTransportProto )
                 	&& ( KErrNone == ret_laddr1 )
                 	&& ( KErrNone == ret_laddr2 ) )
                	{
                	// Local address is localAddress1
                	localAddressTlv = &localAddress1;
                	// Destination Address is localAddress2
                	dataDestAddressTlv = &localAddress2;
                	}
            	else if ( ( RSat::EProtocolNotPresent == 
                   	openChannelV2->iSimMeInterface.iTransportProto )
                 	&& ( KErrNone == ret_laddr1 ) )
                	{
                	// There is only a local address, which is localAddress1
                	localAddressTlv = &localAddress1;
                	}
                                        
            	// Set Destination address    
            	if( dataDestAddressTlv )
                	{
                	TUint8 typeOfAddress = dataDestAddressTlv->GetShortInfo( 
                	    ETLV_TypeOfAddress );
                	    
                	switch( typeOfAddress )
                    	{
                    	case KIPV4: // IPv4
                        	{
                        	openChannelV2->iDestinationAddress.iType = 
                        	    RSat::EIPv4Address;
                        	break;
                        	}
                    	case KIPV6:  // IPv6
                        	{
                        	openChannelV2->iDestinationAddress.iType = 
                        	    RSat::EIPv6Address;
                        	break;
                        	}
                    	default:
                        	{
                        	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_COMPLETENOTIFYL_9, "CSAT: CSatNotifyOpenChannel::CompleteNotifyL, Address not set");
                        	openChannelV2->iDestinationAddress.iType = 
                        	    RSat::EAddressNotSet;
                        	break;
                        	}
                    	}
                	//Is there an address
                	if( dataDestAddressTlv->GetLength() - 1 )
                    	{
                    	openChannelV2->iDestinationAddress.iAddress = 
                        	dataDestAddressTlv->GetData( ETLV_Address );
                    	}
            		}
            	else
            		{
            		openChannelV2->iDestinationAddress.iType = 
            		    RSat::EAddressNotPresent;
            		}
                    
            	// Set Local address (Other Address)
            	if ( localAddressTlv )
            		{
            		CTlv bearerDescription;
                	if ( KErrNone == berTlv.TlvByTagValue( &bearerDescription,
                	    KTlvBearerDescriptionTag ) )
                	    {
                		switch ( bearerDescription.GetShortInfo( 
                		    ETLV_BearerType ) )
                    		{
                    		case KBipCsdBearer:
                        		{
                        		RSat::TOpenCsChannelV2& openCsChannelV2 
                        		= ( *iOpenCsChannelV2Pckg )();
                        		openCsChannelV2.iLocalAddress.iAddress = 
                        		    localAddressTlv->GetData( ETLV_Address );
                        		TUint8 typeOfAddress = localAddressTlv->
                        		    GetShortInfo( ETLV_TypeOfAddress );
                        		    
                        		switch ( typeOfAddress )
                        		    {
                        		    case KIPV4:
                                		{
                                		openCsChannelV2.iLocalAddress.iType = 
                                		    RSat::EIPv4Address;
                                    	break;
                                		}
                        		    case KIPV6:
                                		{
                                		openCsChannelV2.iLocalAddress.iType = 
                                    		RSat::EIPv6Address;
                                    	break;
                                		}
                                	default:
                                    	{
                                    	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_COMPLETENOTIFYL_10, "CSAT: CSatNotifyOpenChannel::CompleteNotifyL, Address not set");
                                		openCsChannelV2.iLocalAddress.iType = 
                                    		RSat::EAddressNotSet;
                                		}
                        		    }
                        		    
                        		// Is there an address
                        		if( localAddressTlv->GetLength() - 1 )
                            		{
                            		openCsChannelV2.iLocalAddress.iAddress = 
                                		localAddressTlv->GetData( ETLV_Address );
                            		}
                       			else
                            		{
                            		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_COMPLETENOTIFYL_11, "CSAT: CSatNotifyOpenChannel::CompleteNotifyL, Address not present");
                            		openCsChannelV2.iLocalAddress.iType = 
                                		RSat::EAddressNotPresent;
                            		}    
                            		
                        		break;
                        		}
                        	case KBipGprsBearer:
                            	{
                            	RSat::TOpenGprsChannelV4& openGprsChannelV4 
                            	= ( *iOpenGprsChannelV4Pckg )();
                            	openGprsChannelV4.iLocalAddress.iAddress = 
                            	localAddressTlv->GetData( ETLV_Address );
                            	TUint8 typeOfAddress = localAddressTlv->
                            	    GetShortInfo( ETLV_TypeOfAddress );
                            	
                            	switch ( typeOfAddress )
                            	    {
                            	    case KIPV4:
                                    	{
                                    	openGprsChannelV4.iLocalAddress.iType =
                                        	RSat::EIPv4Address;
                                        break;
                                    	}
                            	    case KIPV6:
                                    	{
                                    	openGprsChannelV4.iLocalAddress.iType = 
                                        	RSat::EIPv6Address;
                                        break;
                                    	}
                                    default:
                                    	{
                                    	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_COMPLETENOTIFYL_12, "CSAT: CSatNotifyOpenChannel::CompleteNotifyL, Address not set");
                                    	openGprsChannelV4.iLocalAddress.iType = 
                                        	RSat::EAddressNotSet;
                                    	}
                            	    }
                            	    
                            	// Is there an address
                            	if( localAddressTlv->GetLength() - 1 )
                                	{
                                	openGprsChannelV4.iLocalAddress.iAddress = 
                                    	localAddressTlv->GetData( 
                                    	    ETLV_Address );
                                	}
                            	else
                                	{
                                	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_COMPLETENOTIFYL_13, "CSAT: CSatNotifyOpenChannel::CompleteNotifyL, Address not present");
                                	openGprsChannelV4.iLocalAddress.iType = 
                                    	RSat::EAddressNotPresent;
                                	}    
                            
                            	break;
                            	}
                        	default:
                            	{
                            	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_COMPLETENOTIFYL_14, "CSAT: CSatNotifyOpenChannel::CompleteNotifyL, Unknown Bearer type");
                            	break;
                            	}
                            	
                    		} // Switch-case
                	    } // If bearer description tag exists
            		} // if ( localAddressTlv )
            	} // if ( KErrNone == ret )
            else
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_COMPLETENOTIFYL_15, "CSAT: CSatNotifyOpenChannel::CompleteNotifyL, Error when parsing Bearer data");
                }
            } // if ( KErrNone == aErrorCode )
		else
            {
            ret = aErrorCode;
            }
                	
        iNotificationsTsy->iSatTsy->ReqCompleted( reqHandle, ret );
        } // if ( CSatTsy::ESatReqHandleUnknown != reqHandle )
    else
        {
        // Request not on, returning response immediately
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_COMPLETENOTIFYL_16, "CSAT:  CSatNotifyOpenChannel::CompleteNotifyL, Request not ongoing");
        // additional info must be provided with KMeUnableToProcessCmd 
		TBuf16<1> additionalInfo;
		additionalInfo.Append( RSat::KNoSpecificMeProblem );
        iBearer.FillZ( 1 );
        iBufferSize = 0;
        iBearerType = static_cast<TUint8>( RSat::EBearerTypeNotSet );
        CreateTerminalRespL( pCmdNumber, RSat::KMeUnableToProcessCmd,
             additionalInfo );                                       
        ret = KErrCorrupt;
        }
    return ret;    
    }
// -----------------------------------------------------------------------------
// CSatNotifyOpenChannel::TerminalResponseL
// Called by ETel server, passes terminal response to DOS
// -----------------------------------------------------------------------------
//
TInt CSatNotifyOpenChannel::TerminalResponseL
        ( 
        TDes8* aRsp 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_TERMINALRESPONSEL_1, "CSAT: CSatNotifyOpenChannel::TerminalResponseL");

    TInt ret( KErrNone );

	TBuf16<RSat::KAdditionalInfoMaxSize> additionalInfo;

    RSat::TOpenChannelRspV2Pckg* aRspPckg = 
            REINTERPRET_CAST( RSat::TOpenChannelRspV2Pckg*, aRsp);
    RSat::TOpenChannelRspV2& rspV2 = ( *aRspPckg ) ();

    TUint8 pCmdNumber( rspV2.PCmdNumber() );
    
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_TERMINALRESPONSEL_2, "CSAT: CSatNotifyOpenChannel::TerminalResponseL, General result: %d", rspV2.iGeneralResult);

    // Check that general result value is valid
    if ( ( RSat::KSuccess != rspV2.iGeneralResult ) 
        && ( RSat::KPartialComprehension != rspV2.iGeneralResult )
        && ( RSat::KMissingInformation != rspV2.iGeneralResult )
        && ( RSat::KSuccessRequestedIconNotDisplayed != rspV2.iGeneralResult )
        && ( RSat::KPerformedWithModifications != rspV2.iGeneralResult )
        && ( RSat::KRefreshUSIMNotActive != rspV2.iGeneralResult )
        && ( RSat::KPSessionTerminatedByUser != rspV2.iGeneralResult )
        && ( RSat::KMeUnableToProcessCmd != rspV2.iGeneralResult )
        && ( RSat::KNetworkUnableToProcessCmd != rspV2.iGeneralResult )
        && ( RSat::KPCmdNotAcceptedByUser != rspV2.iGeneralResult )
        && ( RSat::KInteractionWithCCTemporaryError != rspV2.iGeneralResult )
        && ( RSat::KCmdBeyondMeCapabilities != rspV2.iGeneralResult )
        && ( RSat::KCmdTypeNotUnderstood != rspV2.iGeneralResult )
        && ( RSat::KCmdDataNotUnderstood != rspV2.iGeneralResult )
        && ( RSat::KCmdNumberNotKnown != rspV2.iGeneralResult )
        && ( RSat::KErrorRequiredValuesMissing != rspV2.iGeneralResult )
        && ( RSat::KBearerIndepProtocolError != rspV2.iGeneralResult )
        && ( RSat::KAccessTechUnableProcessCmd != rspV2.iGeneralResult )
        && ( RSat::KFramesError != rspV2.iGeneralResult ) )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_TERMINALRESPONSEL_3, "CSAT: CSatNotifyOpenChannel::TerminalResponseL, Invalid General Result");
        // Invalid general result
        ret = KErrCorrupt;
        }

    // Check for additional info type
    // - If there is ME (Mobile Equipment) error, additional info is needed
    // - Channel Status Info is sent through additional info
    if ( ( RSat::KMeProblem == rspV2.iInfoType ) || 
         ( RSat::KChannelStatusInfo == rspV2.iInfoType ) )
        {
        // Check the length of additional info
        if ( rspV2.iAdditionalInfo.Length() <= 0 )
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_TERMINALRESPONSEL_4, "CSAT: CSatNotifyOpenChannel::TerminalResponseL, Invalid Additional Info");
            // Invalid additional info field
            ret = KErrCorrupt;
            }
        else
        	{
            additionalInfo.Copy(rspV2.iAdditionalInfo);
        	}   
        }
    else if ( RSat::KSatNetworkErrorInfo == rspV2.iInfoType )
    	{
        // Copy the additional info, if present
        if ( rspV2.iAdditionalInfo.Length() > 0 )
            {
            additionalInfo.Copy(rspV2.iAdditionalInfo);
            }
    	}
    else if ( RSat::KNoAdditionalInfo == rspV2.iInfoType )
	    {
	    // Do nothing
	    }
	else
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_TERMINALRESPONSEL_5, "CSAT: CSatNotifyOpenChannel::TerminalResponseL, Invalid Additional Info type");
        ret = KErrCorrupt;
        }
        
	iBearerType = static_cast<TUint8>(rspV2.iBearer.iType);
	iBearer.Copy( rspV2.iBearer.iParams );
	iBufferSize = rspV2.iBufferSize;
    // Send Terminal Response
    TInt response = CreateTerminalRespL( pCmdNumber, 
    	static_cast<TUint8>( rspV2.iGeneralResult ), additionalInfo );

    if ( KErrNone == ret )
        ret = response;

    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyOpenChannel::CreateTerminalRespL
// Constructs Open channel specific part of terminal response and calls 
// DOS to send the actual message.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyOpenChannel::CreateTerminalRespL
        ( 
        TUint8 aPCmdNumber,         
        TUint8 aGeneralResult,      
        const TDesC16& aAdditionalInfo
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_CREATETERMINALRESPL_1, "CSAT: CSatMessHandler::CreateTerminalRespL");
    // Create and append response data
    TTlv tlvSpecificData;
    // Create General Result TLV here
    tlvSpecificData.AddTag( KTlvResultTag );
    // General result
    tlvSpecificData.AddByte( aGeneralResult );

    if ( ( RSat::KSuccess == aGeneralResult )
        || ( RSat::KPartialComprehension == aGeneralResult )
        || ( RSat::KMissingInformation == aGeneralResult ) 
	    || ( RSat::KPerformedWithModifications == aGeneralResult )
	    || ( RSat::KSuccessRequestedIconNotDisplayed == aGeneralResult ) )
    	{
        // Successful result: append Channel Status using additional info.
		for ( TInt i = 1; i < aAdditionalInfo.Length(); i += 2 )
            {
	        tlvSpecificData.AddTag( KTlvChannelStatusTag );
			tlvSpecificData.AddByte( static_cast<TUint8>( aAdditionalInfo[i-1] ) );
			tlvSpecificData.AddByte( static_cast<TUint8>( aAdditionalInfo[i] ) );
            }
        }
    else
        {
        // Successful result: could/must provide an additional info byte
		if ( aAdditionalInfo.Length() > 0 )
        	{
        	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_CREATETERMINALRESPL_2, "CSAT: CSatMessHandler::CreateTerminalRespL, Unsuccessful result: aAddtionalInfo: %d", aAdditionalInfo[0]);
			tlvSpecificData.AddByte( static_cast<TUint8>( aAdditionalInfo[0] ) );
            }
        }
        
    // For any result: Append Bearer Description data
    if ( iBearer.Length() )
        {
        tlvSpecificData.AddTag( KTlvBearerDescriptionTag );
        tlvSpecificData.AddByte( iBearerType );
        tlvSpecificData.AddData( iBearer );
        }
    else
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYOPENCHANNEL_CREATETERMINALRESPL_3, "CSAT: CSatMessHandler::CreateTerminalRespL: No Bearer Description data available");
        }
        
    // Append Buffer Size
    tlvSpecificData.AddTag( KTlvBufferSizeTag );
    tlvSpecificData.AddByte( TUint8( ( iBufferSize & 0xFF00 ) >> 8 ) );
    tlvSpecificData.AddByte( TUint8( iBufferSize & 0x00FF ) );
    
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

