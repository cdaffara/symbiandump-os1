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



// INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmpacketcontextgsmwcdmaextTraces.h"
#endif

#include <in_sock.h>
#include "cmmpacketcontextgsmwcdmaext.h"
#include "Cmmpacketcontexttsy.h"
#include "Cmmpacketcontextlist.h"
#include "cmmpacketservicegsmwcdmaext.h"
#include "CMmCommonStaticUtility.h"
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <ctsy/pluginapi/cmmdatapackage.h>


// ======== MEMBER FUNCTIONS ========

CMmPacketContextGsmWcdmaExt* CMmPacketContextGsmWcdmaExt::NewL(
        CMmPacketContextTsy* const aMmPacketContextTsy, 
        CMmPacketServiceTsy* const aMmPacketServiceTsy  ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_NEWL_1,  "TSY: CMmPacketContextGsmWcdmaExt::NewL." );

    CMmPacketContextGsmWcdmaExt* packetContextGsmWcdmaExt = 
        new ( ELeave ) CMmPacketContextGsmWcdmaExt();
    
    CleanupStack::PushL( packetContextGsmWcdmaExt );

    packetContextGsmWcdmaExt->iMmPacketContextTsy = aMmPacketContextTsy;
    packetContextGsmWcdmaExt->iMmPacketServiceTsy = aMmPacketServiceTsy;
    packetContextGsmWcdmaExt->ConstructL();

    CleanupStack::Pop();

    return packetContextGsmWcdmaExt;
    }

CMmPacketContextGsmWcdmaExt::CMmPacketContextGsmWcdmaExt()
    {
    }

void CMmPacketContextGsmWcdmaExt::ConstructL()
    {
    InitInternalAttributesL();
    }
 
CMmPacketContextGsmWcdmaExt::~CMmPacketContextGsmWcdmaExt()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_DTOR_1,  "TSY: CMmPacketContextGsmWcdmaExt::~CMmPacketContextGsmWcdmaExt." );

    TInfoName contextName;
    if( iMmPacketContextTsy )
        {
        contextName = iMmPacketContextTsy->ContextName();
        }
	if ( contextName.Compare( KNullDesC ) != 0 )
        {
        CMmDataPackage data;
        data.PackData( &contextName );
        
        if( iMmPacketServiceTsy )
        	{
         	TRAP_IGNORE( iMmPacketServiceTsy->MessageManager()->
                HandleRequestL( EPacketContextDelete, &data ) );       		
        	}
    
        }

    // Notify status change
    // This is needed to inform Service that context is no longer active
    if( iMmPacketContextTsy )
    	{
	    iMmPacketContextTsy->CompleteNotifyStatusChange(
	        RPacketContext::EStatusDeleted );
    	}
    	
    // Release memory used for dynamic data members
    delete iTempConfig;
    delete iConfig;
    delete iFilterArray;
    delete iFilterTemporaryArray;
    delete iTempMediaAuthorizationArray;
    }


// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::InitInternalAttributesL
// Initialises miscellaneous internal attributes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextGsmWcdmaExt::InitInternalAttributesL()
    {
    // Network initiated context indication.
    //iContextType = EFalse;
    
    iIsActivateAllowed = EFalse;
    iIsInitialiseAllowed = ETrue;
    iConnectionSpeed = 0;

    iTempConfig = NULL;

    iNotifyConfig = RPacketContext::TContextConfigGPRS();
    
    iFilterArray = new ( ELeave ) CArrayFixFlat
        <RPacketContext::TPacketFilterV2>( 1 );

    iFilterTemporaryArray = NULL;
    
    iTempMediaAuthorizationArray = NULL;
    
    for ( TInt i = 0 ; KMaxNumberOfTftFilters > i; i++ )
        {
        // cell is set 0xFF because of internal check(s)
        iFilterIdTemporaryArray[i] = 0xFF; 
        }

    TInfoName hostCidName = iMmPacketContextTsy->HostCID();

    iConfig = new ( ELeave ) RPacketContext::TContextConfigGPRS();

    if( 0 < hostCidName.Length() )
        {
        CMmPacketContextTsy* iPrimaryContextObject = iMmPacketServiceTsy->
            PacketContextList()->PacketContextByName( &hostCidName );

        if ( NULL != iPrimaryContextObject )
            {
            *iConfig = *( iPrimaryContextObject->
                PacketContextGsmWcdmaExt()->ContextConfig() );
            }
        }

    iPrevFilterStatus = EAddPacketFilterStatusUnknown;
    iFilterStatus = EAddPacketFilterStatusUnknown;
    isAddMediaAuthorizationCalledFirstTime = EFalse;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::InitialiseContextL
// This function starts context initialisation.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextGsmWcdmaExt::InitialiseContextL(RPacketContext::TDataChannelV2* aDataChannel)
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_INITIALISECONTEXTL_1,  "TSY: CMmPacketContextGsmWcdmaExt::InitialiseContextL." );

    TInt ret( KErrArgument );

    if ( iIsInitialiseAllowed )
        {
        TInfoName contextName = iMmPacketContextTsy->ContextName();
        TInfoName hostCidName = iMmPacketContextTsy->HostCID();
        CMmDataPackage dataPackage;
        dataPackage.PackData( &contextName, &hostCidName );

        iDataChannel = aDataChannel;
        
        ret = iMmPacketServiceTsy->MessageManager()->HandleRequestL( 
           EPacketContextInitialiseContext, &dataPackage );
        
        if(ret == KErrNone)
        	{
            iIsInitialiseAllowed = EFalse;
        	}
        }
    return ret;
    }

#ifndef USING_CTSY_DISPATCHER

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::CompleteInitialiseContextL
// This function starts context initialisation.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextGsmWcdmaExt::CompleteInitialiseContext(const TInt aResult) // Result
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_COMPLETEINITIALISECONTEXT_1,  "TSY: CMmPacketContextGsmWcdmaExt::CompleteInitialiseContext." );

    if ( KErrNone == aResult )
        {
        iMmPacketContextTsy->CompleteNotifyStatusChange(RPacketContext::EStatusInactive );

        if(iDataChannel != NULL)
        	{      	
        	iDataChannel->iChannelId.Copy(iMmPacketContextTsy->ContextName());
        	iDataChannel->iPort.Copy(iMmPacketContextTsy->ContextName());
        	iDataChannel = NULL;
        	}

        iIsActivateAllowed = ETrue;
        // Set Dynamic Caps
        iMmPacketServiceTsy->ActivePacketServiceExtension()->
            SetDynamicCapsFlag( RPacketService::KCapsActivate, ETrue );
        }
    else
        {
        iIsInitialiseAllowed = ETrue;
        }
      iMmPacketContextTsy->CompleteInitialiseContext( aResult );

    }
#endif //USING_CTSY_DISPATCHER

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::ActivateL
// This function starts the context activation.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextGsmWcdmaExt::ActivateL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_ACTIVATEL_1,  "TSY: CMmPacketContextGsmWcdmaExt::ActivateL " );

    TInt ret( KErrArgument );

    if ( iIsActivateAllowed )
        {

        TInfoName hostCidRef = iMmPacketContextTsy->HostCID();
        TInfoName contextName = iMmPacketContextTsy->ContextName();
        // Primary context activation
        CMmDataPackage data;
        data.PackData( &contextName );
        //This is primary context activation and host context's length is 
        //hence zero
        if ( 0 == hostCidRef.Length() )
            {  
            ret = iMmPacketServiceTsy->MessageManager()->HandleRequestL(
                EPacketContextActivate, &data );
            }        
        // Secondary context activation 
        else
            {
            TBool qosExtensionId( EFalse );
            CMmPacketQoSTsy* qosProfile = iMmPacketContextTsy->PacketQoSTsy();
        
            if ( NULL != qosProfile )
                {
                qosExtensionId = qosProfile->PacketQoSGsmWcdmaExt()->
                      ProfileAsked();
                }

            // Check that secondary context have qos and at least one filter
            if ( qosExtensionId && 0 != iFilterArray->Count() )
                {
                ret = iMmPacketServiceTsy->MessageManager()->HandleRequestL(
                EPacketContextActivate, &data );
                }
            }

        if ( KErrNone == ret )
            {
            iIsActivateAllowed = EFalse;
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::CompleteActivate
// Completes the activate request. Indicates DOS TSY has started activation.
//  Activation to network may take minutes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextGsmWcdmaExt::CompleteActivate(
        CMmDataPackage* aDataPackage,
        TInt aResult ) 
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_COMPLETEACTIVATE_1,  "TSY: CMmPacketContextGsmWcdmaExt::CompleteActivate. Status: %{TSymbianErrorCodes}",aResult );

    if ( KErrNone == aResult )
        {
        //Unpack to TPacketDataConfigBase first, determine the Extension ID and
        //then cast to the appropriate structure before extracting all the necessary
        //data
        TPacketDataConfigBase* config = NULL;
        //DOS layer brings also name in packet due routing in manager.
        TInfoName* null; 
        aDataPackage->UnPackData( &null, &config );
        null = NULL;
        
        RPacketContext::TGSNAddress accessPointName = iConfig->iAccessPointName;
        
        if ( TPacketDataConfigBase::KConfigGPRS == config->ExtensionId() )
        	{
        	iConfig->iPdpType = static_cast<RPacketContext::TContextConfigGPRS*>(config)->
        		iPdpType;
        	accessPointName = static_cast<RPacketContext::TContextConfigGPRS*>(config)->
        		iAccessPointName;
        	}
        
        if ( TPacketDataConfigBase::KConfigRel99Rel4 == config->ExtensionId() ||
        		TPacketDataConfigBase::KConfigRel5 == config->ExtensionId())
        	{
        	iConfig->iPdpType = static_cast<RPacketContext::TContextConfigR99_R4*>(config)->
        		iPdpType;
        	accessPointName = static_cast<RPacketContext::TContextConfigR99_R4*>(config)->
        		iAccessPointName;
        	}       
        
        TInt temp = accessPointName.Length();
        if ( !temp )
            {
            iConfig->iAccessPointName = accessPointName;
            // Notify config changed
            }
        CompleteNotifyConfigChanged();

        // Notify status change
        iMmPacketContextTsy->CompleteNotifyStatusChange( 
            RPacketContext::EStatusActivating );
        }
    else
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_COMPLETEACTIVATE_2,  "TSY: CMmPacketContextGsmWcdmaExt::CompleteActivate. ErrorCause: %{TSymbianErrorCodes}",aResult );
        // Re-activation is allowed because activation has not succeeded
        iIsActivateAllowed = ETrue;
        }

    // Set Dynamic Caps
    iMmPacketServiceTsy->ActivePacketServiceExtension()->SetDynamicCapsFlag(
        RPacketService::KCapsActivate, iIsActivateAllowed );

    // Complete Activate
    iMmPacketContextTsy->CompleteActivate( aResult );

    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::CompleteActivatePhase2L
// Final completion for the context activation. Context is activated to 
// network.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextGsmWcdmaExt::CompleteActivatePhase2(  
        CMmDataPackage* aDataPackage, const TInt aResult )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_COMPLETEACTIVATEPHASE2_1,  "TSY: CMmPacketContextGsmWcdmaExt::CompleteActivatePhase2. Status: %d", aResult );

    if ( KErrNone == aResult )
        {
        //Unpack to TPacketDataConfigBase first, determine the Extension ID and
        //then cast to the appropriate structure before extracting all the necessary
        //data
        TPacketDataConfigBase* aConfig = NULL;
        TInfoName* name = NULL;
        aDataPackage->UnPackData( &name, &aConfig );

        if( NULL != aConfig )
            {
            if ( TPacketDataConfigBase::KConfigGPRS == aConfig->ExtensionId() )
                {
                RPacketContext::TContextConfigGPRS* configGPRS =
                	static_cast<RPacketContext::TContextConfigGPRS*>(aConfig); 
          
                iConfig->iPdpAddress = configGPRS->iPdpAddress;
               	iConfig->iAccessPointName = configGPRS->iAccessPointName;
               	iConfig->iProtocolConfigOption.iDnsAddresses.iPrimaryDns = configGPRS->
               		iProtocolConfigOption.iDnsAddresses.iPrimaryDns;
                iConfig->iProtocolConfigOption.iDnsAddresses.iSecondaryDns = configGPRS->
               		iProtocolConfigOption.iDnsAddresses.iSecondaryDns;
               	iConfig->iPdpCompression = configGPRS->iPdpCompression;
               	iConfig->iProtocolConfigOption.iMiscBuffer = 
               		configGPRS->iProtocolConfigOption.iMiscBuffer;
                }
            
            if ( TPacketDataConfigBase::KConfigRel99Rel4 == aConfig->ExtensionId() ||
            		TPacketDataConfigBase::KConfigRel5 == aConfig->ExtensionId())
                {
                RPacketContext::TContextConfigR99_R4* configR99_R4 =
                	static_cast<RPacketContext::TContextConfigR99_R4*>(aConfig); 
          
               	iConfig->iPdpAddress = configR99_R4->iPdpAddress;
               	iConfig->iAccessPointName = configR99_R4->iAccessPointName;
               	iConfig->iProtocolConfigOption.iDnsAddresses.iPrimaryDns = configR99_R4->
               		iProtocolConfigOption.iDnsAddresses.iPrimaryDns;
                iConfig->iProtocolConfigOption.iDnsAddresses.iSecondaryDns = configR99_R4->
               		iProtocolConfigOption.iDnsAddresses.iSecondaryDns;
               	iConfig->iPdpCompression = 0;
               	iConfig->iProtocolConfigOption.iMiscBuffer = 
               		configR99_R4->iProtocolConfigOption.iMiscBuffer;
                }
            }

        // Complete notifies
        CompleteNotifyConfigChanged();
        iMmPacketContextTsy->CompleteNotifyStatusChange( 
            RPacketContext::EStatusActive );
        }

    else 
        {
        // Reactivation is allowed because activation has not succeeded
        iIsActivateAllowed = ETrue;

        // Set Dynamics Caps
        iMmPacketServiceTsy->ActivePacketServiceExtension()->
            SetDynamicCapsFlag( RPacketService::KCapsActivate, ETrue );
        }
    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::Deactivate
// This function deactivates the context
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextGsmWcdmaExt::DeactivateL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_DEACTIVATEL_1,  "TSY: CMmPacketContextGsmWcdmaExt::DeactivateL." );

    TInt ret( KErrArgument );
    //zero length
    TInfoName contextName = iMmPacketContextTsy->ContextName();
	if ( contextName.Compare( KNullDesC ) != 0 )
        {
        CMmDataPackage data;
        data.PackData( &contextName );
        ret = iMmPacketServiceTsy->MessageManager()->HandleRequestL(
            EPacketContextDeactivate, &data );

        if ( KErrNone == ret )
            {
            iContextStatusBeforeDeactivation = 
                iMmPacketContextTsy->ContextStatus();
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::CompleteDeactivateL
// Completes the deactivate request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextGsmWcdmaExt::CompleteDeactivate( 
        const TInt aResult )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_COMPLETEDEACTIVATE_1,  "TSY: CMmPacketContextGsmWcdmaExt::CompleteDeactivate." );

    if ( KErrNone == aResult )
        {
        
        // Notify status change
        iMmPacketContextTsy->CompleteNotifyStatusChange(
            RPacketContext::EStatusInactive );
        }
    else
        {
        // Notify status change
        iMmPacketContextTsy->CompleteNotifyStatusChange(
            iContextStatusBeforeDeactivation );
        }

    iMmPacketContextTsy->CompleteDeactivate( aResult );

    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::DeleteL
// This function starts deletion.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextGsmWcdmaExt::DeleteL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_DELETEL_1,  "TSY: CMmPacketContextGsmWcdmaExt::DeleteL." );

    TInt ret( KErrNotReady );

    if ( RPacketContext::EStatusUnknown != 
            iMmPacketContextTsy->ContextStatus() )
        {
        TInfoName contextName = iMmPacketContextTsy->ContextName();
        CMmDataPackage data;
        data.PackData( &contextName );

        ret = iMmPacketServiceTsy->MessageManager()->HandleRequestL(
            EPacketContextDelete, &data );

        }

    return ret;

    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::CompleteDelete
// Completes the delete request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextGsmWcdmaExt::CompleteDelete( 
		TInt const aResult )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_COMPLETEDELETE_1,  "TSY: CMmPacketContextGsmWcdmaExt::CompleteDelete." );

    if( KErrNone == aResult )
        {
        iContextStatusBeforeDeactivation = RPacketContext::EStatusDeleted;

        // Notify status change
        iMmPacketContextTsy->CompleteNotifyStatusChange(
            RPacketContext::EStatusDeleted );


        if ( !iMmPacketContextTsy->IsDialUpContext( NULL ) )
            {
            // Release memory used for context configuration info
            delete iConfig;
            iConfig = NULL;
            }
        }   

    iMmPacketContextTsy->CompleteDelete( aResult );        

    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::GetConfig
// Retrieves the current context configuration to the client.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextGsmWcdmaExt::GetConfig( 
		TPacketDataConfigBase* const aConfig )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_GETCONFIG_1,  "TSY: CMmPacketContextGsmWcdmaExt::GetConfig." );

    TInt ret( KErrNone );

    // Check that the mode given in aConfig is GPRS
    if ( TPacketDataConfigBase::KConfigGPRS == aConfig->ExtensionId() ||
        TPacketDataConfigBase::KConfigRel99Rel4 == aConfig->ExtensionId() )
        {
        if ( NULL != iConfig &&
            TPacketDataConfigBase::KConfigGPRS == aConfig->ExtensionId() )
            {
            // Set configuration data to ETel side memory
            *( RPacketContext::TContextConfigGPRS* )aConfig = 
                *iConfig;
            }
        else if ( NULL != iConfig &&
            TPacketDataConfigBase::KConfigRel99Rel4 == aConfig->ExtensionId() )
            {
            RPacketContext::TContextConfigR99_R4 configR99_R4;

            configR99_R4.iPdpType = iConfig->iPdpType;
            configR99_R4.iAccessPointName = iConfig->iAccessPointName;
            configR99_R4.iPdpAddress = iConfig->iPdpAddress;
            configR99_R4.iUseEdge = iConfig->iUseEdge;
            configR99_R4.iProtocolConfigOption = 
                iConfig->iProtocolConfigOption;
            // Set configuration data to ETel side memory
            *( RPacketContext::TContextConfigR99_R4* )aConfig = configR99_R4; 
                
            }
        else
            {
            ret = KErrNotFound;
            }
        }
    else
        {
        // Given mode is not supported
        ret = KErrNotSupported;
        }

    return ret;

    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::GetDataVolumeTransferred
// Gets details of the current amount of data transmitted and
// received over the airlink since the context was activated.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextGsmWcdmaExt::GetDataVolumeTransferredL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_GETDATAVOLUMETRANSFERREDL_1,  "TSY: CMmPacketContextGsmWcdmaExt::GetDataVolumeTransferredL." );

    TInt ret( KErrNone );
    TInfoName contextName = iMmPacketContextTsy->ContextName();
    
	if ( contextName.Compare( KNullDesC ) != 0 )
        {
        CMmDataPackage data;
        data.PackData( &contextName );
        ret = iMmPacketServiceTsy->MessageManager()->HandleRequestL(
            EPacketContextGetDataVolumeTransferred, &data );
        }

    return ret;

    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::CompleteGetDataVolumeTransferred
// Completes the get data volume transferred request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextGsmWcdmaExt::CompleteGetDataVolumeTransferred(  
        const CMmDataPackage* aDataPackage,
        const TInt aResult )
    { 
    TInt errorValue( KErrNone );

    if ( KErrNone == aResult )
        {
        TInfoName contextName;
        RPacketContext::TDataVolume dataVolume;
        aDataPackage->UnPackData( contextName, dataVolume ); 

        // Set data volume to the tsy
        iMmPacketContextTsy->SetDataVolume( KErrNone, dataVolume );
        }

    else
        {
        errorValue = KErrGeneral;
        }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_COMPLETEGETDATAVOLUMETRANSFERRED_1,  "TSY: CMmPacketContextGsmWcdmaExt::CompleteGetDataVolumeTransferred. Error Value:%{TSymbianErrorCodes}",errorValue );

    iMmPacketContextTsy->CompleteGetDataVolumeTransferred( errorValue );

    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::NotifyConfigChanged
// Requests notification when configuration of context changes.
// Implements the GSM/WCDMA specific functionality for the notification.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextGsmWcdmaExt::NotifyConfigChanged(
        TPacketDataConfigBase& aConfig )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_NOTIFYCONFIGCHANGED_1,  "TSY: CMmPacketContextGsmWcdmaExt::NotifyConfigChanged." );

    TInt ret( KErrNone );

    // Check that used mode is GPRS
    if ( ( TPacketDataConfigBase::KConfigGPRS != aConfig.ExtensionId() ) &&
        ( TPacketDataConfigBase::KConfigRel99Rel4 != aConfig.ExtensionId() ) ) 
        {
        ret = KErrNotSupported;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::CompleteNotifyConfigChanged
// Completes notification when the context configuration changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextGsmWcdmaExt::CompleteNotifyConfigChanged()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_COMPLETENOTIFYCONFIGCHANGED_1,  "TSY: CMmPacketContextGsmWcdmaExt::CompleteNotifyConfigChanged." );

    if ( NULL != iConfig )
        {
        // Check that the configuration has really changed
        if ( iNotifyConfig.iPdpType != iConfig->iPdpType ||
             iNotifyConfig.iAccessPointName != 
                iConfig->iAccessPointName ||
             iNotifyConfig.iPdpAddress != iConfig->iPdpAddress ||
             iNotifyConfig.iPdpCompression != 
                iConfig->iPdpCompression ||
             iNotifyConfig.iAnonymousAccessReqd != 
                iConfig->iAnonymousAccessReqd ||
             iNotifyConfig.iUseEdge != iConfig->iUseEdge )
            {
            // Store the configuration
            iNotifyConfig = *iConfig;

            // Complete notification
            iMmPacketContextTsy->CompleteNotifyConfigChanged();
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::SetConfigL
// This method starts the Context configuration.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextGsmWcdmaExt::SetConfigL( 
		TPacketDataConfigBase* const aConfig )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_SETCONFIGL_1,  "TSY: CMmPacketContextGsmWcdmaExt::SetConfigL." );

    TInt ret( KErrNotSupported );

	TInfoName contextName = iMmPacketContextTsy->ContextName();
	if ( contextName.Compare( KNullDesC ) != 0 )
        {    
        if ( TPacketDataConfigBase::KConfigGPRS == aConfig->ExtensionId() )
            {
            if( !iIsInitialiseAllowed )
                {
                iContextConfigurationType = 
                    TPacketDataConfigBase::KConfigGPRS;

                RPacketContext::TContextConfigGPRS& configGPRS =
                    *( RPacketContext::TContextConfigGPRS* )aConfig;  

                if ( NULL == iTempConfig )
                    {
                    // Allocate memory for iTempConfig
                    iTempConfig = 
                        new ( ELeave ) RPacketContext::TContextConfigGPRS();
                    }

                 // Temporary store the configGPRS
                *iTempConfig = configGPRS;

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_SETCONFIGL_2,  "TSY: CMmPacketContextGsmWcdmaExt::SetConfigL. NWIContext: %d", configGPRS.iNWIContext );

                TInfoName contextName = iMmPacketContextTsy->ContextName();
                CMmDataPackage data;
                data.PackData( &( *aConfig ), &contextName );

                ret = iMmPacketServiceTsy->MessageManager()->HandleRequestL(
                    EPacketContextSetConfig, &data ); 
        
                if ( KErrNone != ret )
                    {
                    // Release the memory allocated for iTempConfig
                    delete iTempConfig;
                    iTempConfig = NULL;
                    }
                }
            else
                {
                ret = KErrNotReady;
                }
            
            }
        else if ( TPacketDataConfigBase::KConfigRel99Rel4 ==
			aConfig->ExtensionId() ||
			TPacketDataConfigBase::KConfigRel5 == aConfig->ExtensionId() )
            {
            if( !iIsInitialiseAllowed )
                {
                iContextConfigurationType = 
                    TPacketDataConfigBase::KConfigRel99Rel4;
                RPacketContext::TContextConfigR99_R4& configR99_R4 =
                    *( RPacketContext::TContextConfigR99_R4* )aConfig;

                if ( NULL == iTempConfig )
                    {
                    // Allocate memory for iTempConfig
                    iTempConfig = 
                        new ( ELeave ) RPacketContext::TContextConfigGPRS();
                    }

                    // Temporary store the configR99_R4 to iTempConfig
                iTempConfig->iPdpType = configR99_R4.iPdpType;
                iTempConfig->iAccessPointName = configR99_R4.iAccessPointName;
                iTempConfig->iPdpAddress = configR99_R4.iPdpAddress;
                iTempConfig->iUseEdge = configR99_R4.iUseEdge;
                iTempConfig->iProtocolConfigOption = 
                        configR99_R4.iProtocolConfigOption;
                        
                TInt len = 
                    iTempConfig->iProtocolConfigOption.iMiscBuffer.Length();
				

                TInfoName contextName = iMmPacketContextTsy->ContextName();
                CMmDataPackage data;
                data.PackData( &( *aConfig ), &contextName );
                ret = iMmPacketServiceTsy->MessageManager()->HandleRequestL(
                    EPacketContextSetConfig, &data );    

                if ( KErrNone != ret )
                    {
                    // Release the memory allocated for iTempConfig
                    delete iTempConfig;
                    iTempConfig = NULL;
                    }
                }    
            else
                {
                ret = KErrNotReady;
                }
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::CompleteSetConfig
// Completes the SetConfig request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextGsmWcdmaExt::CompleteSetConfig( 
		const TInt aResult )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_COMPLETESETCONFIG_1,  "TSY: CMmPacketContextGsmWcdmaExt::CompleteSetConfig. aStatus:%d",aResult );


    if ( NULL != iTempConfig )
        {        
            iConfig->iProtocolConfigOption.iMiscBuffer.Zero();
            iConfig->iProtocolConfigOption.iMiscBuffer.FillZ();
        
            *iConfig = *iTempConfig;  
            TInt len = iConfig->iProtocolConfigOption.iMiscBuffer.Length();
            // Release the memory allocated for iTempConfig
            delete iTempConfig;
            iTempConfig = NULL;
            CompleteNotifyConfigChanged();
        }
    if( isAddMediaAuthorizationCalledFirstTime )
        {
        iMmPacketContextTsy->CompleteSetConfig( aResult, 
            isAddMediaAuthorizationCalledFirstTime );
        //Null changed params					
        isAddMediaAuthorizationCalledFirstTime = EFalse;
        
        }
    else
        {
        iMmPacketContextTsy->CompleteSetConfig( aResult ) ;	
        }
    
    
    }


// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::IsContextActivationAllowed
// Tells if context activation is allowed.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmPacketContextGsmWcdmaExt::IsContextActivationAllowed() const
    {

    return iIsActivateAllowed;

    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::GetConfigurationData
// Return the configuration data.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextGsmWcdmaExt::GetConfigurationData(
        TInfoName& aContextName,
        RPacketContext::TProtocolType& aPdpType,    
        TBool& aContextType,                       
        TInfoName& aPrimaryContextName )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_GETCONFIGURATIONDATA_1,  "TSY: CMmPacketContextGsmWcdmaExt::GetConfigurationData." );

    TInt ret( KErrNotReady );

	TInfoName contextName = iMmPacketContextTsy->ContextName();
	if ( contextName.Compare( KNullDesC ) != 0 )
        {
        
        aPdpType = iConfig->iPdpType;
        aContextName = contextName;
        aContextType =  iConfig->iNWIContext; //iContextType;
        aPrimaryContextName = iMmPacketContextTsy->HostCID();
		ret = KErrNone;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::AddPacketFilterL
// Add or modify existing packet filter to specified context
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextGsmWcdmaExt::AddPacketFilterL(
            TDes8* aPacketFilter )
    {
    TInt ret( KErrGeneral );

    RPacketContext::TPacketFilterV2Pckg* contextFilterPckg =
        ( RPacketContext::TPacketFilterV2Pckg* ) aPacketFilter;

    RPacketContext::TPacketFilterV2& contextFilter =
        ( *contextFilterPckg )();

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_ADDPACKETFILTERL_1,  "TSY: CMmPacketContextGsmWcdmaExt::AddPacketFilterL contextFilterId: %d", contextFilter.iId );

    // Check out Tft operation
    iFilterStatus = EAddPacketFilterStatusAdd;
    for ( TInt i = 0 ; i < iFilterArray->Count() ; i++ )
        {
        if ( contextFilter.iId == iFilterArray->At( i ).iId ) 
            {
            iFilterStatus = EAddPacketFilterStatusModify;
            i = iFilterArray->Count();
            }
        }

    if ( iPrevFilterStatus == iFilterStatus ||
            EAddPacketFilterStatusUnknown == iPrevFilterStatus )
        {
        // context modification if context status is active or activating
        if( iMmPacketContextTsy->ContextStatus() == 
            RPacketContext::EStatusActive ||
            iMmPacketContextTsy->ContextStatus() == 
            RPacketContext::EStatusActivating )
            {

            if ( 0 < contextFilter.iId && 8 >= contextFilter.iId )
                {
                // if it's first time to use iFilterTemporaryArray
                if ( NULL == iFilterTemporaryArray )
                    {
                    // Eight is max number of TFT filters. If count is less 
                    // than eight, we can be sure that there is enough space
                    // for one more filter
                    if ( 8 > iFilterArray->Count() )
                        {
                        iFilterTemporaryArray = new ( ELeave ) CArrayFixFlat
                            <RPacketContext::TPacketFilterV2>( 1 );
                        iFilterTemporaryArray->AppendL( contextFilter );
                        iPrevFilterStatus = iFilterStatus;
                        ret = KErrNone;
                        }
                    }

                // if iFilterTemporaryArray is already used at least once
                else
                    {
                    // if previous filter operation was add
                    if ( EAddPacketFilterStatusAdd == iPrevFilterStatus )
                        {
                        // Eight is max number of TFT filters. If count is 
                        // less than eight, we can be sure that there is
                        // enough space for one more filter
                        if ( 8 > ( iFilterArray->Count() + 
                            iFilterTemporaryArray->Count() ) )
                            {
                            // Check that same filter id doesn't appear twice
                            TBool isNewFilter( ETrue );
                            for ( TInt i = 0; 
                                iFilterTemporaryArray->Count() > i; i++ )
                                {
                                if ( ( iFilterTemporaryArray->At( i ).iId ) ==
                                    ( contextFilter.iId ) )
                                    {
                                    isNewFilter = EFalse;
                                    i = iFilterTemporaryArray->Count();
                                    }
                                }

                            // if id doesn't exist already add filter to Array
                            if ( isNewFilter )
                                {
                                iFilterTemporaryArray->
                                    AppendL ( contextFilter );
                                iPrevFilterStatus = iFilterStatus;
                                ret = KErrNone;
                                }
                            }
                        }

                    // if previous filter operation was modify
                    if ( EAddPacketFilterStatusModify == iPrevFilterStatus )
                        {
                        // Check that same filter id doesn't appear twice
                        TBool isNewFilter( ETrue );
                        for ( TInt i = 0; iFilterTemporaryArray->Count() > i; 
                            i++ )
                            {
                            if ( ( iFilterTemporaryArray->At( i ).iId ) == 
                                ( contextFilter.iId ) )
                                {
                                isNewFilter = EFalse;
                                i = iFilterTemporaryArray->Count();
                                }
                            }

                        // if id doesn't exist already add filter to Array
                        if ( isNewFilter )
                            {
                            iFilterTemporaryArray->AppendL ( contextFilter );
                            iPrevFilterStatus = iFilterStatus;
                            ret = KErrNone;
                            }
                        }
                    }
                }
            }
        }
    // context configuration if context status is unknown or inactive
    if ( iMmPacketContextTsy->ContextStatus() == 
        RPacketContext::EStatusUnknown ||
        iMmPacketContextTsy->ContextStatus() == 
        RPacketContext::EStatusInactive )
        {
        // Eight is max number of TFT filters. If count is less than eight,
        // we can be sure that there is enough space for one more filter
        if ( iFilterArray->Count() < 8 && 
            EAddPacketFilterStatusAdd == iFilterStatus )
            {
            // if filter id is allowed value. Allowed value is between 1..8
            if ( 0 < contextFilter.iId && 8 >= contextFilter.iId )
                { 

                TBool filterIdAlreadyInArray( EFalse );

              
                    if ( NULL == iFilterTemporaryArray )
                        {
                        iFilterTemporaryArray = new ( ELeave ) CArrayFixFlat
                            <RPacketContext::TPacketFilterV2>( 1 );
                        }
                    
                    
                    // if pdp type is Ipv4 8 is max number of TFT 
                    // filters. If count is less than eight, we can be sure 
                    // that there is enough space for one more filter
                    // if pdp type is Ipv6 4 is max number of TFT 
                    // filters. If count is less than four, we can be sure 
                    // that there is enough space for one more filter
                    if ( ( 8 > iFilterArray->Count() &&
                       RPacketContext::EPdpTypeIPv4 == iConfig->iPdpType ) ||
                       ( 4 > iFilterArray->Count() &&
                        RPacketContext::EPdpTypeIPv6 == iConfig->iPdpType )
                        )
                        {
                        for ( TInt i = 0 ; iFilterArray->Count() > i ; i++ )
                            {
                            if ( contextFilter.iId != 
                                iFilterArray->At(i).iId )
                                {
                                iFilterTemporaryArray->AppendL( 
                                    iFilterArray->At(i) );
                                }

                            else 
                                {
                                filterIdAlreadyInArray = ETrue;
                                }
                            }
                        
                        iFilterTemporaryArray->AppendL( contextFilter );

                        TInfoName contextName = 
                            iMmPacketContextTsy->ContextName();
                        CMmPacketDataPackage data;
                        data.SetConfig( iConfig , 
                            iContextConfigurationType  );
                        data.PackData( &contextName, iFilterTemporaryArray );

                        ret = iMmPacketServiceTsy->MessageManager()->
                            HandleRequestL( EPacketContextAddPacketFilter, 
                            &data );

                        }

                    else
                        {
                        ret = KErrArgument;
                        }
               
                // add configured filter to iFilterArray
                if ( KErrNone == ret )
                    {
                    if ( !filterIdAlreadyInArray )
                        {
                        iFilterArray->AppendL( contextFilter );
                        }
                    else
                        {
                        for ( TInt i = 0 ; iFilterArray->Count() > i ; i++ )
                            {
                            if ( iFilterArray->At(i).iId == 
                                contextFilter.iId )
                                {
                                iFilterArray->At(i) = contextFilter;
                                }
                            }
                        }

                    delete iFilterTemporaryArray;
                    iFilterTemporaryArray = NULL;
                    }
                }
            }
        }

    // if error occures empty iFilterTemporaryArray
    if ( ( KErrNone != ret ) && ( NULL != iFilterTemporaryArray ) )
        {
        delete iFilterTemporaryArray;
        iFilterTemporaryArray = NULL;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::RemovePacketFilter
// This method stores Id(s) client wants to remove
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextGsmWcdmaExt::RemovePacketFilter( 
			const TInt aID )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_REMOVEPACKETFILTER_1,  "TSY: CMmPacketContextGsmWcdmaExt::RemovePacketFilter. FilterId:%d", aID );

    TInt ret( KErrArgument );

    if ( EAddPacketFilterStatusUnknown == iPrevFilterStatus ||
        EAddPacketFilterStatusRemove == iPrevFilterStatus )
        {

        // check that aID is in iFilterArray
        TBool isRemoveFilterId( EFalse );
        TInt index = 0;
        for ( TInt i = 0 ; iFilterArray->Count() > i ; i++ )
            {
            if ( iFilterArray->At( i ).iId == aID )
                {
                isRemoveFilterId = ETrue;
                index = i;
                i = iFilterArray->Count();
                }
            }

        if ( isRemoveFilterId )
            {
            // Find empty place for filter id and set filter id in it
            for ( TInt i = 0 ; KMaxNumberOfTftFilters > i ; i++ )
                {
                if ( aID == iFilterIdTemporaryArray[i] )
                    {
                    // breaks if id already in array
                    i = KMaxNumberOfTftFilters;
                    }

                // 0xFF value is set default to empty cell
                else if ( 0xFF == iFilterIdTemporaryArray[i] )
                    {
                    iFilterIdTemporaryArray[i] = aID;
                    iFilterArray->Delete(index);
                    
                    // breaks when empty cell found in array
                    i = KMaxNumberOfTftFilters;
                    iPrevFilterStatus = EAddPacketFilterStatusRemove;
                    ret = KErrNone;
                    }
                }
            }
        }
    
#ifdef USING_CTSY_DISPATCHER
	TInfoName contextName = iMmPacketContextTsy->ContextName();
    CMmPacketDataPackage data;    
    data.PackData( &contextName, const_cast< TInt* >(&aID));
    ret = iMmPacketServiceTsy->MessageManager()-> HandleRequestL( EPacketContextRemovePacketFilter, &data );
#endif

    if ( KErrNone != ret )
        {
        for ( TInt i = 0 ; KMaxNumberOfTftFilters > i ; i++ )
            {
            // cell is set 0xFF because of internal check(s)
            iFilterIdTemporaryArray[i] = 0xFF;
            iPrevFilterStatus = EAddPacketFilterStatusUnknown;
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::ModifyActiveContextL
// Method sends modification of an active context to the network.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextGsmWcdmaExt::ModifyActiveContextL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_MODIFYACTIVECONTEXTL_1,  "TSY: CMmPacketContextGsmWcdmaExt::ModifyActiveContextL." );

    TInt ret( KErrArgument );
    
    TPacketFilterFilterOperation tftOperation( EDeletePacketFilters );

    // Get pointer to QosProfile
    CMmPacketQoSTsy* qosProfile = iMmPacketContextTsy->PacketQoSTsy();
    RPacketQoS::TQoSGPRSRequested* qosProfile97 = NULL;
            RPacketQoS::TQoSR99_R4Requested* qosProfile99 = NULL;

	RPacketQoS::TQoSR5Requested* qosProfileR5 = NULL;

    if ( NULL != qosProfile )
        {
        CMmPacketQoSGsmWcdmaExt* pointerToExt = qosProfile->
            PacketQoSGsmWcdmaExt();

        if ( NULL != pointerToExt )
            {
            // Get pointer to qos
            TInt configurationType = pointerToExt->ConfigurationType();
            
            
            switch( configurationType )
                {
                case TPacketDataConfigBase::KConfigGPRS:
                    qosProfile97 = pointerToExt->Requested97();
                    break;
                case TPacketDataConfigBase::KConfigRel99Rel4:
                    qosProfile99 = pointerToExt->Requested99();
                    break;
				case TPacketDataConfigBase::KConfigRel5:
                    qosProfileR5 = pointerToExt->RequestedR5();
                    break;
                default:
                    break;                
                }

            // If qos includes something
            if ( NULL != qosProfile97 || NULL != qosProfile99 
				|| NULL != qosProfileR5 )
                {              
                // use 0xFF and index 0 to check if no filter(s) to remove
                if ( NULL == iFilterTemporaryArray && 
                    0xFF == iFilterIdTemporaryArray[0] )
                    {
                    //MODIFY QOS PARAMETERS
                    //Pack configuration and send to messagemanager
                    TInfoName contextName = iMmPacketContextTsy->ContextName();
                    iContextConfiguration.iContextName = contextName;
                    iContextConfiguration.iQosParams97 = qosProfile97;
                    iContextConfiguration.iQosParams99 = qosProfile99;
					iContextConfiguration.iQosParamsR5 = qosProfileR5;
                    iContextConfiguration.iConfiguration = iConfig->iPdpType;
                    iContextConfiguration.iTftFilterArray = iFilterArray;
                    iContextConfiguration.iTftOperation = tftOperation;
                    for( TInt i=0; i < KMaxNumberOfTftFilters; i++  )
                        {
                        iContextConfiguration.iFilterIdArray[i] = 
                            iFilterIdTemporaryArray[i];
                        }

                    CMmDataPackage data;
                    data.PackData( &iContextConfiguration,
                        &iTempMediaAuthorizationArray );
                    
                    ret = iMmPacketServiceTsy->MessageManager()->
                        HandleRequestL( EPacketContextModifyActiveContext,
                        &data );
                     
                    }
                }
            }
        }

    // Add or Modify Filters
    if( NULL != iFilterTemporaryArray )
        {
        if ( 0 < iFilterTemporaryArray->Count() )
            {
            // filteroperation is add
            if ( EAddPacketFilterStatusAdd == iPrevFilterStatus )
                {
                tftOperation = EAddPacketFilters;
                TInt beforeFilterCount = iFilterArray->Count();
                TInt filterArrayExpandedSize = ( beforeFilterCount +
                    ( iFilterTemporaryArray->Count() ) );
                
                iFilterArray->ResizeL( filterArrayExpandedSize );

                for ( ; filterArrayExpandedSize > beforeFilterCount ; 
                    beforeFilterCount++ )
                    {
                    // iId is set 0xFF because of internal check(s)
                    iFilterArray->At( beforeFilterCount ).iId = 0xFF;
                    }
                }

            // filteroperation is modify
            if ( EAddPacketFilterStatusModify == iPrevFilterStatus )
                {
                tftOperation = EReplacePacketFilters;
                }

            if ( EAddPacketFilterStatusUnknown != iPrevFilterStatus )
                {

                CMmPacketQoSGsmWcdmaExt* pointerToExt = NULL;
                if(qosProfile)
                	{
                	pointerToExt = qosProfile->PacketQoSGsmWcdmaExt();
                	}

                if ( NULL != pointerToExt )
                    {
                    TInt configurationType = pointerToExt->ConfigurationType();
                    
                    switch( configurationType )
                    	{
                    	case TPacketDataConfigBase::KConfigGPRS:
                        	qosProfile97 = pointerToExt->Requested97();
                        	break;
                    	case TPacketDataConfigBase::KConfigRel99Rel4:
                        	qosProfile99 = pointerToExt->Requested99();
                        	break;
						case TPacketDataConfigBase::KConfigRel5:
							qosProfileR5 = pointerToExt->RequestedR5();
							break;
                    	default:
                       	 	break;                
                    	}

                    //Pack configuration and send to messagemanager
                    TInfoName contextName = iMmPacketContextTsy->ContextName();
                    iContextConfiguration.iContextName = contextName;
                    iContextConfiguration.iQosParams97 = qosProfile97;
                    iContextConfiguration.iQosParams99 = qosProfile99;
					iContextConfiguration.iQosParamsR5 = qosProfileR5;
                    iContextConfiguration.iConfiguration = iConfig->iPdpType;
                    iContextConfiguration.iTftFilterArray = iFilterTemporaryArray;
                    iContextConfiguration.iTftOperation = tftOperation;

                    for( TInt i=0; i < KMaxNumberOfTftFilters; i++  )
                        {
                        iContextConfiguration.iFilterIdArray[i] = 
                            iFilterIdTemporaryArray[i];
                        }
                
                    CMmDataPackage data;
                    data.PackData( &iContextConfiguration, &iTempMediaAuthorizationArray );
                
                    ret = iMmPacketServiceTsy->MessageManager()->
                        HandleRequestL( EPacketContextModifyActiveContext,
                        &data );
                    }
                }          
            }
        }
        
    // Check if first cell includes id 
    if ( 0xFF != iFilterIdTemporaryArray[0] )
        {
        
        CMmPacketQoSGsmWcdmaExt* pointerToExt = NULL;
        if(qosProfile)
        	{
        	pointerToExt = qosProfile->PacketQoSGsmWcdmaExt();
        	}
        
        if( NULL != pointerToExt )
            {
            TInt configurationType = pointerToExt->ConfigurationType();
            switch( configurationType )
                {
                case TPacketDataConfigBase::KConfigGPRS:
                    qosProfile97 = pointerToExt->Requested97();
                    break;
                case TPacketDataConfigBase::KConfigRel99Rel4:
                    qosProfile99 = pointerToExt->Requested99();
                    break;
					case TPacketDataConfigBase::KConfigRel5:
                    qosProfileR5 = pointerToExt->RequestedR5();
                    break;
                default:
                    break;                
                }

            //Pack configuration and send to messagemanager
            TInfoName contextName = iMmPacketContextTsy->ContextName();
            iContextConfiguration.iContextName = contextName;
            iContextConfiguration.iQosParams97 = qosProfile97;
            iContextConfiguration.iQosParams99 = qosProfile99;
			iContextConfiguration.iQosParamsR5 = qosProfileR5;
            iContextConfiguration.iConfiguration = iConfig->iPdpType;
            iContextConfiguration.iTftFilterArray = iFilterTemporaryArray;
            iContextConfiguration.iTftOperation = tftOperation;

            for(TInt i=0; i < KMaxNumberOfTftFilters; i++  )
                {
                iContextConfiguration.iFilterIdArray[i] =
                    iFilterIdTemporaryArray[i];
                }
        
            CMmDataPackage data;
            data.PackData( &iContextConfiguration, 
                &iTempMediaAuthorizationArray );        
            ret = iMmPacketServiceTsy->MessageManager()->HandleRequestL(
                EPacketContextModifyActiveContext, &data );
            }
        
        }

    return ret;

    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::CompleteModifyActiveContext
//  When modification is done completes to client
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextGsmWcdmaExt::CompleteModifyActiveContext(
           CMmDataPackage* aDataPackage, const TInt aResult )
    {
    //Unpack data
    TBuf8<1> rejectioncode;
    rejectioncode.Zero();
    TInfoName contextName;
   	aDataPackage->UnPackData( contextName, rejectioncode );	
    
    
    //check result
    if ( KErrNone == aResult )
        {
        if ( NULL != iFilterTemporaryArray )
            {
            if ( EAddPacketFilterStatusAdd == iFilterStatus )
                {
                TInt temporaryFilterArrayCount ( 0 );
                for ( TInt i = 0 ; i < iFilterArray->Count() ; i++ )
                    {
                    if ( 0xFF == iFilterArray->At( i ).iId )
                        {
                        iFilterArray->At( i ) = iFilterTemporaryArray->
                            At( temporaryFilterArrayCount++ );
                        }
                    }

                delete iFilterTemporaryArray; 
                iFilterTemporaryArray = NULL; 

                }

            else if ( EAddPacketFilterStatusModify == iFilterStatus )
                {
                for ( TInt k = 0 ; k < iFilterTemporaryArray->Count() ; k++ )
                    {
                    for ( TInt g = 0 ; g < iFilterArray->Count() ; g++ )
                        {
                        if ( iFilterTemporaryArray->At( k ).iId == 
                            iFilterArray->At( g ).iId )
                            {
                            iFilterArray->At( g ) = 
                                iFilterTemporaryArray->At( k );
                            g = iFilterArray->Count();
                            }
                        }
                    }

                delete iFilterTemporaryArray;
                iFilterTemporaryArray = NULL;
                }
            }

        else if ( 0xFF != iFilterIdTemporaryArray[0] )
            {
            for ( TInt g = 0 ; g < KMaxNumberOfTftFilters ; g++ )
                {
                for ( TInt i = 0;i < iFilterArray->Count(); i++ )
                    {
                    if( iFilterArray->At( i ).iId == 
                        iFilterIdTemporaryArray[g] )
                        {
                        iFilterArray->Delete( i );
                        iFilterIdTemporaryArray[g] = 0xFF;
                        i = iFilterArray->Count();
                        }
                    }
                }
            }
        }
        
		//Check if rejection code is received    
		if( 0 != rejectioncode.Length() && KErrNone != aResult )
    		{
    		
    		//Append rejection code
		TInt length = iConfig->iProtocolConfigOption.iMiscBuffer.MaxLength()-4;

    	if( iConfig->iProtocolConfigOption.iMiscBuffer.Length() <= length )
    		{    	
    		//1st byte of container ID
    		iConfig->iProtocolConfigOption.iMiscBuffer.Append(0x00);
			//2nd byte of container ID
			iConfig->iProtocolConfigOption.iMiscBuffer.Append(0x04);
			//content length
			iConfig->iProtocolConfigOption.iMiscBuffer.Append(0x01);
			//content
			iConfig->iProtocolConfigOption.iMiscBuffer.Append( rejectioncode );
    		}
		
    	}
    	
    	
    iMmPacketContextTsy->CompleteModifyActiveContext( aResult );
    iPrevFilterStatus = EAddPacketFilterStatusUnknown;
    
    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::EnumeratePacketFilters
// Returns number of defined packet filters to client
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextGsmWcdmaExt::EnumeratePacketFilters( 
				TInt *aCount )
    {

    *aCount = 0;

    if ( NULL != iFilterArray )
        {
        *aCount = iFilterArray->Count();
        }

    return KErrNone;

    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::GetPacketFilterInfo
// Returns the packet filter information specified by aIndex
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextGsmWcdmaExt::GetPacketFilterInfo( 
		const TInt aIndex,
        TPacketBase* aPacketFilterInfo )
    {
    TInt ret( KErrArgument );

    if ( NULL != iFilterArray )
        {
        if( iFilterArray->Count() > aIndex )
            {
            RPacketContext::TPacketFilterV2& contextFilter =
                *( RPacketContext::TPacketFilterV2* )aPacketFilterInfo;

            contextFilter = iFilterArray->At( aIndex );
            ret = KErrNone;

            }
        }

    return ret;

    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::ContextConfig
// Return Context configuration
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
RPacketContext::TContextConfigGPRS* CMmPacketContextGsmWcdmaExt::ContextConfig
        (
        ) const
    {
    return iConfig;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::GetDnsInfoEtel
// This method gets the DNS info
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPacketContextGsmWcdmaExt::GetDnsInfoEtel(
            TDes8* const aDnsInfo )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETCONTEXTGSMWCDMAEXT_GETDNSINFOETEL_1,  "TSY: CMmPacketContextGsmWcdmaExt::GetDnsInfoEtel." );

    // Unpack the aDnsInfo
    RPacketContext::TTDnsInfoV2Pckg* dnsInfoPckg =
        ( RPacketContext::TTDnsInfoV2Pckg* ) aDnsInfo;
    RPacketContext::TDnsInfoV2& dnsInfo = ( *dnsInfoPckg )();

    // Get DNS info
    dnsInfo.iPrimaryDns = iConfig->
                iProtocolConfigOption.iDnsAddresses.iPrimaryDns;
    dnsInfo.iSecondaryDns = iConfig->
                iProtocolConfigOption.iDnsAddresses.iSecondaryDns;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::SetContextStatusBeforeDeactivation
// Sets context status before activation
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextGsmWcdmaExt::SetContextStatusBeforeDeactivation(
        const RPacketContext::TContextStatus aContextStatusBeforeDeactivation )
    {

    iContextStatusBeforeDeactivation = aContextStatusBeforeDeactivation;
    
    }

// ---------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::CompleteNotifyStatusChange
// Routes completion to tsy and sets last error cause
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPacketContextGsmWcdmaExt::CompleteNotifyStatusChange(
        const CMmDataPackage* aDataPackage,
        const TInt aResult )
    {

    TInfoName* null= NULL;
	RPacketContext::TContextStatus contextStatus;
    
    //initialise struct for completion params   
	TContextMisc misc;
    
    aDataPackage->UnPackData( null, misc );
	contextStatus = misc.iStatus;

    if( KErrNone != aResult )
        {

        iMmPacketContextTsy->SetLastErrorCause( aResult );

        }
    
	//Check if rejection cause is included  
	if( 0 != misc.iRejectionCause.Length() && KErrNone != aResult )
    	{

    	//Append rejection code
		TInt length = iConfig->iProtocolConfigOption.iMiscBuffer.MaxLength()-4;
    	if( iConfig->iProtocolConfigOption.iMiscBuffer.Length() <= length )
    	{    	
    	//1st byte of container ID
    	iConfig->iProtocolConfigOption.iMiscBuffer.Append(0x00);
		//2nd byte of container ID
		iConfig->iProtocolConfigOption.iMiscBuffer.Append(0x04);
		//content length
		iConfig->iProtocolConfigOption.iMiscBuffer.Append(0x01);
		//content
			iConfig->iProtocolConfigOption.iMiscBuffer.Append( 
				misc.iRejectionCause );
	    	}
    	}
 	 
    iMmPacketContextTsy->CompleteNotifyStatusChange( contextStatus );

    }
    
//----------------------------------------------------------------------------
// CMmPacketContextGsmWcdmaExt::AddMediaAuthorizationL makes request for 
// licensee specific tsy for adding authorization params 
//----------------------------------------------------------------------------

TInt CMmPacketContextGsmWcdmaExt::AddMediaAuthorizationL(
		RPacketContext::CTFTMediaAuthorizationV3* const aMediaAuthorization, TBool &aHasMediaAuthorizationBeenCached )
	{
	
	TInt ret ( KErrGeneral );
	CMmPacketDataPackage data;
    TInfoName contextName = iMmPacketContextTsy->ContextName();
 
    //take copy and store locally
    RPacketContext::CTFTMediaAuthorizationV3* mediaAuthorization =
        aMediaAuthorization;
  
    	
    if ( NULL == iTempMediaAuthorizationArray )
    	{
   		iTempMediaAuthorizationArray = new ( ELeave ) CArrayFixFlat
                            <RPacketContext::CTFTMediaAuthorizationV3>( 8 );
    	//Reset Array
    	iTempMediaAuthorizationArray->Reset();
    	}

    
    //Store request locally
    if ( 3 > iTempMediaAuthorizationArray->Count() )
  		{
  		
     	iTempMediaAuthorizationArray->AppendL( *mediaAuthorization );
     	aHasMediaAuthorizationBeenCached=ETrue;
        ret = KErrNone;
        }
    else
    	{
    	aHasMediaAuthorizationBeenCached=EFalse;
    	}
    
    // context configuration if context status is unknown or inactive request 
    // will be sended first time to Licensee Tsy 
    
    if ( iMmPacketContextTsy->ContextStatus() == 
         RPacketContext::EStatusUnknown ||iMmPacketContextTsy->ContextStatus() 
         == RPacketContext::EStatusInactive )
    	{
    		
   		data.SetConfig( iConfig , iContextConfigurationType  );
    	data.PackData( &contextName, aMediaAuthorization );
		
		
		//Send request to licensee Tsy 
    	ret = iMmPacketServiceTsy->MessageManager()->HandleRequestL(
            EPacketAddMediaAuthorization, &data );
    									
    	isAddMediaAuthorizationCalledFirstTime = ETrue;
    	}

	return ret;
	}
	
	
//----------------------------------------------------------------------------
// TInt CMmPacketContextGsmWcdmaExt::RemoveMediaAuthorization
// removes authoriazation params
//----------------------------------------------------------------------------

TInt CMmPacketContextGsmWcdmaExt::RemoveMediaAuthorization(
        RPacketContext::TAuthorizationToken* const aAuthorizationToken ) 
 	{
 	
 	TInt ret( KErrNotFound );
 	RPacketContext::CTFTMediaAuthorizationV3* mediaAuthorization = NULL;
 	
 	for( TInt index (0); index < iTempMediaAuthorizationArray->Count(); index++ )
 		{
 		//get pointer from array by index
 		mediaAuthorization = &( iTempMediaAuthorizationArray->At( index ) );
 		
 		if( NULL != mediaAuthorization )
 			{
 			//Check if authorizationtoken is the one to be removed
 			if ( *aAuthorizationToken == mediaAuthorization->iAuthorizationToken )
 				{
 				iTempMediaAuthorizationArray->Delete( index );
 				
 				//break if found
 				index = iTempMediaAuthorizationArray->Count();
 				ret = KErrNone;	
 				}
 			}
 		//make sure that pointer will be renewed	
 		mediaAuthorization = NULL;
 		
 		}
 		
    return ret;
        		
	}
	
// ================= OTHER EXPORTED FUNCTIONS ===============================
	
//  End of File
