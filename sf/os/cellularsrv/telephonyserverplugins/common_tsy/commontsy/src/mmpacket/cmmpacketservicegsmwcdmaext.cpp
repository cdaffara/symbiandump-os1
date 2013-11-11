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
#include "cmmpacketservicegsmwcdmaextTraces.h"
#endif

#include "cmmpacketservicegsmwcdmaext.h"
#include "Cmmpacketcontexttsy.h"
#include "Cmmpacketcontextlist.h"
#include "CMmCommonStaticUtility.h"
#include "CMmCustomTsy.h"
#include "CMmPacketTsy.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "cmmmbmscontextlist.h"
#include <ctsy/mltsyfactory.h>

// ======== MEMBER FUNCTIONS ========

CMmPacketServiceGsmWcdmaExt::CMmPacketServiceGsmWcdmaExt()
    {
    }

CMmPacketServiceGsmWcdmaExt* CMmPacketServiceGsmWcdmaExt::NewL(
        CMmPacketServiceTsy* const aMmPacketServiceTsy,
        MLtsyFactoryBase* aFactory ) 
    {

    CMmPacketServiceGsmWcdmaExt* const packetServiceGsmWcdmaExt = 
        new ( ELeave ) CMmPacketServiceGsmWcdmaExt();
    
    CleanupStack::PushL( packetServiceGsmWcdmaExt );
    packetServiceGsmWcdmaExt->iMmPacketServiceTsy = aMmPacketServiceTsy;
    packetServiceGsmWcdmaExt->ConstructL( aFactory );
    CleanupStack::Pop();

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_NEWL_1, "TSY: CMmPacketServiceGsmWcdmaExt::NewL.\n" );
    return packetServiceGsmWcdmaExt;

    }

void CMmPacketServiceGsmWcdmaExt::ConstructL( MLtsyFactoryBase* aFactory )
    {
	TBool initPhase = ETrue;
	CMmDataPackage data;

    InitInternalAttributesL( aFactory );

    // get current status
	iMmPacketServiceTsy->MessageManager()->HandleRequestL(  
												EPacketNotifyStatusChange );

    // Call to get current preferred bearer
	data.PackData( &initPhase );
	iMmPacketServiceTsy->MessageManager()->HandleRequestL(  
												 EPacketGetAttachMode, &data );	
    // get max number of monitored service list
    iMmPacketServiceTsy->MessageManager()->HandleRequestL(  
    											 EPacketEnumerateMbmsMonitorServiceList );
    											     	
    iMmPacketServiceTsy->MessageManager()->HandleRequestL( 
                                                 EPacketEnumerateMbmsActiveServiceList );      											 
    											 
    }
    
CMmPacketServiceGsmWcdmaExt::~CMmPacketServiceGsmWcdmaExt()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_DTOR_1, "TSY: CMmPacketServiceGsmWcdmaExt::~CMmPacketServiceGsmWcdmaExt.\n" );
    }
    
//----------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::InitInternalAttributesL
// Initialises internal attributes.
//----------------------------------------------------------------------------
//
void CMmPacketServiceGsmWcdmaExt::InitInternalAttributesL( MLtsyFactoryBase* aFactory )
    {
	iStaticCaps = KMmPacketServiceStaticCaps;
	
	// Version number is used to specify LTSY interface
	MLtsyFactoryBase::TCtsyInterfaceVersion vers = 
		aFactory->Version();
	if( MLtsyFactoryBase::EIfVersion3 <= vers )
		{
		MLtsyFactoryV3* ptr_v3 = 
			static_cast<MLtsyFactoryV3*>( aFactory );
		//Get static caps using factory method
		iStaticCaps = 
			ptr_v3->GetLTSYStaticCapabilities( KStaticCapsPacketService );
		}
	else
		{
		//Get static caps using plugin api method
    	iMmPacketServiceTsy->MessageManager()->HandleRequestL( EPacketGetStaticCaps );	
		}
		
    iDynamicCaps = 0;

    iLastTransferStatus = RPacketService::EStatusUnattached;
	iLastEdgeGprsSupport = EFalse;
	
	iGprsIsSupportedOnCell =EFalse;
	iCsServicesIsSupportedOnCell = EFalse;

    iAccessPointName.Zero();
    iPdpType = RPacketContext::EPdpTypeIPv4;
    iPdpCompression = 0;
    iIsSetDefaultContextParamsCalled = EFalse;

    }


//----------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::Attach
// Attaches phone to the network.
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceGsmWcdmaExt::AttachL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_ATTACHL_1, "TSY: CMmPacketServiceGsmWcdmaExt::AttachL." );

	// Call and return
	return iMmPacketServiceTsy->MessageManager()->HandleRequestL( 
														EPacketAttach );
    }

//----------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::CompleteAttach
// Completes Attach.
// Complete to service tsy with KErrNone or epoc error code
//----------------------------------------------------------------------------
//
void CMmPacketServiceGsmWcdmaExt::CompleteAttach( 
		TInt aErrorValue ) const
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_COMPLETEATTACH_1, "TSY: CMmPacketServiceGsmWcdmaExt::CompleteAttach.\n  \t ErrorValue:%d\n", aErrorValue );

	if ( KErrNone == aErrorValue )
		{
		 // Initiate EStatusAttached to packet status
		RPacketService::TStatus packetStatus(
            RPacketService::EStatusAttached );
		
		TBool resumed = EFalse;
		CMmDataPackage data;
		data.PackData( &packetStatus, &resumed );

		iMmPacketServiceTsy->CompletePacketStatusChanged( &data, KErrNone );

		}

    // Complete to service tsy with KErrNone or EPOC error code
    iMmPacketServiceTsy->CompleteAttach( aErrorValue );

    }

//----------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::Detach
// Detaches phone from the network
// Sends Detach request to Message manager
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceGsmWcdmaExt::DetachL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_DETACHL_1, "TSY: CMmPacketServiceGsmWcdmaExt::DetachL.\n  ");

	return iMmPacketServiceTsy->MessageManager()->
                                           HandleRequestL( EPacketDetach );

    }

//----------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::CompleteDetachL
// Completes Detach.
// Complete to service tsy with KErrNone or epoc error code
//----------------------------------------------------------------------------
//
void CMmPacketServiceGsmWcdmaExt::CompleteDetachL( 
		TInt aErrorValue ) const
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_COMPLETEDETACHL_1, "TSY: CMmPacketServiceGsmWcdmaExt::CompleteDetachL\n  \t ErrorValue:%d\n", aErrorValue );

	if ( KErrNone == aErrorValue )
		{
		
		RPacketService::TStatus packetStatus( 
								RPacketService::EStatusUnattached );
		
		TBool resumed = EFalse;
		CMmDataPackage data;
		data.PackData( &packetStatus, &resumed  );

		iMmPacketServiceTsy->CompletePacketStatusChanged( &data, KErrNone );

		}

    iMmPacketServiceTsy->CompleteDetachL( aErrorValue );

    }

//---------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::GetNtwkRegStatus
// Network registration status request to the network.
//---------------------------------------------------------------------------
//
TInt CMmPacketServiceGsmWcdmaExt::GetNtwkRegStatusL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_GETNTWKREGSTATUSL_1, "TSY: CMmPacketServiceGsmWcdmaExt::GetNtwkRegStatusL.\n" );

	return iMmPacketServiceTsy->MessageManager()->HandleRequestL( 
		EPacketGetNtwkRegStatus );

    }

//----------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::CompleteGetNtwkRegStatus
// Completes get network registration status request.
// map network registration status
// call CompleteGetNtwkRegStatus() from service tsy
//----------------------------------------------------------------------------
//
void CMmPacketServiceGsmWcdmaExt::CompleteGetNtwkRegStatus(
		CMmDataPackage* aDataPackage, TInt aResult )
    {
	RPacketService::TRegistrationStatus regStatusClient;
    //Unpack data
	aDataPackage->UnPackData( regStatusClient );

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_COMPLETEGETNTWKREGSTATUS_1, "TSY: CMmPacketServiceGsmWcdmaExt::CompleteGetNtwkRegStatus.\n RegStatusClient = %d \t", regStatusClient);

    // Call CompleteGetNtwkRegStatus() from service tsy
	iMmPacketServiceTsy->CompleteGetNtwkRegStatus( regStatusClient, 
			aResult );

    }

//----------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::SetAttachMode
// Sets attach mode to the lower level 
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceGsmWcdmaExt::SetAttachModeL(
		RPacketService::TAttachMode aMode )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_SETATTACHMODEL_1, "TSY: CMmPacketServiceGsmWcdmaExt::SetAttachModeL." );

	TInt ret( KErrNone );
	CMmDataPackage data;
    data.PackData( &aMode );

	ret = iMmPacketServiceTsy->MessageManager()->HandleRequestL( 
		EPacketSetAttachMode, &data );

    return ret;

    }

//----------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::CompleteSetAttachMod
// Completes Set Attach Mode.
//----------------------------------------------------------------------------
//
void CMmPacketServiceGsmWcdmaExt::CompleteSetAttachMode( 
		const TInt aErrorValue ) const
    {

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_COMPLETESETATTACHMODE_1, "TSY: CMmPacketServiceGsmWcdmaExt::CompleteSetAttachMode.\n  \t  Error Value:%d\n", aErrorValue );

    iMmPacketServiceTsy->CompleteSetAttachMode( aErrorValue );

    }

//----------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::GetAttachMode
// Gets attach mode 
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceGsmWcdmaExt::GetAttachModeL() 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_GETATTACHMODEL_1, "TSY: CMmPacketServiceGsmWcdmaExt::GetAttachModeL.\n\t " );

	TBool calledOnInitPhase = EFalse;
	CMmDataPackage data;
	data.PackData( &calledOnInitPhase );

	TInt ret = iMmPacketServiceTsy->MessageManager()->HandleRequestL(
		EPacketGetAttachMode, &data );

	return ret;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::CompleteGetAttachMode
// Completes Get Attach Mode request.
//----------------------------------------------------------------------------
//
void CMmPacketServiceGsmWcdmaExt::CompleteGetAttachMode(CMmDataPackage* aDataPackage, 
		                                                TInt aResult) const  
    {
	RPacketService::TAttachMode attachMode;

	aDataPackage->UnPackData( attachMode );

    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_COMPLETEGETATTACHMODE_1, "TSY: CMmPacketServiceGsmWcdmaExt::CompleteGetAttachMode.\n  \t Status:%d\n\t ", attachMode );

    iMmPacketServiceTsy->CompleteGetAttachMode( attachMode, aResult);

    }

//----------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::SetPreferredBearer
// Sets preferrer bearer 
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceGsmWcdmaExt::SetPreferredBearerL(
		const TDataPackage& aPackage )
    {
	TInt ret( KErrNone );
	TDataPackage package = aPackage;

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_SETPREFERREDBEARERL_1, "TSY: CMmPacketServiceGsmWcdmaExt::SetPreferredBearerL.");

	ret = iMmPacketServiceTsy->MessageManager()->HandleRequestL( 
		 EPacketSetPrefBearer, &package );

	return ret;
    }

//----------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::CompleteSetPreferredBearer
// Completes Set Preferred Bearer request.
//----------------------------------------------------------------------------
//
void CMmPacketServiceGsmWcdmaExt::CompleteSetPreferredBearer(
		TInt aErrorValue ) const  
    {

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_COMPLETESETPREFERREDBEARER_1, "TSY: CMmPacketServiceGsmWcdmaExt::CompleteSetPreferredBearer.\n\t Error Value:%d\n", aErrorValue );

    iMmPacketServiceTsy->CompleteSetPreferredBearer( aErrorValue );

    }

//----------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::GetDynamicCaps
// This function returns the Packet Data's dynamic capabilities.
// set dynamic capabilities to Etel side memory
//----------------------------------------------------------------------------
//
void CMmPacketServiceGsmWcdmaExt::GetDynamicCaps(
        RPacketService::TDynamicCapsFlags* const aCaps ) const  
    {

    // Set caps
    *aCaps = iDynamicCaps;

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_GETDYNAMICCAPS_1, "TSY: CMmPacketServiceGsmWcdmaExt::GetDynamicCaps.\n\t Dynamic Caps:0x%08x\n", (TUint)*aCaps );

    }

//----------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::SetDynamicCapsFlag
// Sets one of the Packet Data's dynamic capabilities flag.
//----------------------------------------------------------------------------
//
void CMmPacketServiceGsmWcdmaExt::SetDynamicCapsFlag(
        const RPacketService::TDynamicCapsFlags aFlag,  
        const TBool aIsActionPossible ) 
    {

    RPacketService::TDynamicCapsFlags dynamicCaps( iDynamicCaps );

    if ( aIsActionPossible )
        {
		// Activate aFlag in dynamicCaps 
        dynamicCaps |= aFlag;
        }

    else
        {

        TBool contextActivationPossible( EFalse );

        // If flag to be reset is KCapsActivate
        if ( RPacketService::KCapsActivate == aFlag )
            {

            // Get number of contexts
            const TInt numOfCreatedContext(
                iMmPacketServiceTsy->PacketContextList()->NumberOfContexts() );

            for ( TInt i = 0; i < numOfCreatedContext; i++ )
                {

                // Get context by index
                CMmPacketContextTsy* const context = iMmPacketServiceTsy->
                    PacketContextList()->PacketContextByIndex( i );

                if ( NULL != context )
                    {
                    
                    if ( context->PacketContextGsmWcdmaExt()->
                        IsContextActivationAllowed() )
                        {

                        // At least one context is ready to be activated
                        contextActivationPossible = ETrue;
                        i = numOfCreatedContext;

                        }
                    }
                }
            }
        if ( RPacketService::KCapsMBMS == aFlag )
            {
            
            // See if there are context in receiving state
            contextActivationPossible = iMmPacketServiceTsy->MBMSContextList()->IsMbmsContextReceiving();
            }


        // If no context ready to be activated
        if ( !contextActivationPossible )
            {
            
            // Deactivate aFlag in dynamicCaps 
            dynamicCaps &= ~aFlag;
            
            }
        }

    if ( dynamicCaps != iDynamicCaps )
        {

        // Set dynamic caps to iDynamicCaps
        iDynamicCaps = dynamicCaps;
        // Call complete notify dynamic caps change from service tsy
        iMmPacketServiceTsy->CompleteNotifyDynamicCapsChange( iDynamicCaps );

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_SETDYNAMICCAPSFLAG_1, "TSY: CMmPacketServiceGsmWcdmaExt::SetDynamicCapsFlag.\n\t Dynamic Caps:%u\n", (TUint)iDynamicCaps );

        }
    }

//----------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::CompleteDynamicCapsChange
// Completes dynamic capabilities change to the Service Tsy.
//----------------------------------------------------------------------------
//
void CMmPacketServiceGsmWcdmaExt::CompleteDynamicCapsChange(
        CMmDataPackage* aPackage )	
    {

	aPackage->UnPackData( iGprsIsSupportedOnCell, 
									iCsServicesIsSupportedOnCell );

    RPacketService::TDynamicCapsFlags dynamicCaps( iDynamicCaps );

    // If Gprs is supported in current cell
    if ( iGprsIsSupportedOnCell )
        {
        
        // check current packet service status
        switch ( iMmPacketServiceTsy->ServiceStatus() )
            {
            case RPacketService::EStatusAttached:
            case RPacketService::EStatusActive:
                // Deativate KCapsManualAttach in dynamicCaps 
                dynamicCaps &= ~RPacketService::KCapsManualAttach;

                // Activate KCapsSMSTransfer, KCapsManualDetach and
                // KCapsRxContextActivationReq in dynamicCaps 
                dynamicCaps |= RPacketService::KCapsSMSTransfer |
                               RPacketService::KCapsManualDetach |
                               RPacketService::KCapsRxContextActivationReq;
                break;
            case RPacketService::EStatusUnattached:
                // Activate KCapsManualAttach in dynamicCaps 
                dynamicCaps |= RPacketService::KCapsManualAttach;

                // Deativate KCapsSMSTransfer, KCapsManualDetach and
                // KCapsRxContextActivationReq in dynamicCaps 
                dynamicCaps &= ~( RPacketService::KCapsSMSTransfer |
                               RPacketService::KCapsManualDetach |
                               RPacketService::KCapsRxContextActivationReq );

                break;
            case RPacketService::EStatusSuspended:
                // Activate KCapsManualDetach in dynamicCaps 
                dynamicCaps |= RPacketService::KCapsManualDetach;

                // Deativate KCapsSMSTransfer, KCapsManualAttach and
                // KCapsRxContextActivationReq in dynamicCaps 
                dynamicCaps &= ~( RPacketService::KCapsSMSTransfer |
                               RPacketService::KCapsManualAttach |
                               RPacketService::KCapsRxContextActivationReq );
                break;
            default:
                break;
            }
        }
    else
        {
        
        // Gprs is not supported in current cell
        dynamicCaps = 0;
        
        }

    // If cs services are supported
    if ( iCsServicesIsSupportedOnCell )
        {

        // Activate KCapsRxCSCall in dynamicCaps 
        dynamicCaps |= RPacketService::KCapsRxCSCall;

        }
    else
        {
        
        // Deativate KCapsRxCSCall in dynamicCaps 
        dynamicCaps &= ~RPacketService::KCapsRxCSCall;

        }

    if ( dynamicCaps != iDynamicCaps )
        {

        // Set dynamic caps to iDynamicCaps
        iDynamicCaps = dynamicCaps;
        // Call complete notify dynamic caps change from service tsy
        iMmPacketServiceTsy->CompleteNotifyDynamicCapsChange( iDynamicCaps );

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_COMPLETEDYNAMICCAPSCHANGE_1, "TSY: CMmPacketServiceGsmWcdmaExt::CompleteDynamicCapsChange.\n\t Dynamic Caps:%x\n", (TUint)iDynamicCaps );
        }
    }

//----------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::GetStaticCaps
// This function returns the service's static capabilities.
//----------------------------------------------------------------------------
//
void CMmPacketServiceGsmWcdmaExt::GetStaticCaps(
        TUint* const aCaps )const 
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_GETSTATICCAPS_1, "TSY: CMmPacketServiceGsmWcdmaExt::GetStaticCaps.\n\t Static Caps:%u\n", iStaticCaps );

    *aCaps = iStaticCaps;

    }

//----------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::SetDefaultContextParams
// Set default context parameters
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceGsmWcdmaExt::SetDefaultContextParamsL(
		const TDataPackage& aPackage )
    {

    TInt ret( KErrNotSupported );
	TDataPackage package = aPackage;
	
	TAny* const dataPtr = aPackage.Ptr1(); 
	TPacketDataConfigBase* aConfig =  REINTERPRET_CAST( 
		TPacketDataConfigBase*, dataPtr );

	if ( TPacketDataConfigBase::KConfigGPRS == aConfig->ExtensionId() )
        {
        RPacketContext::TContextConfigGPRS& configGPRS =
            *( RPacketContext::TContextConfigGPRS* )aConfig;

        iTempAccessPointName = configGPRS.iAccessPointName;

        iTempPdpType = configGPRS.iPdpType;

        iTempPdpCompression = configGPRS.iPdpCompression;

        }

	else if ( TPacketDataConfigBase::KConfigRel99Rel4 == 
        aConfig->ExtensionId() )
        {

        RPacketContext::TContextConfigR99_R4& configR99_R4 =
            *( RPacketContext::TContextConfigR99_R4* )aConfig;

        iTempAccessPointName = configR99_R4.iAccessPointName;
        iTempPdpType = configR99_R4.iPdpType;

        }
    else
        {
        return KErrArgument;
        }

	ret = iMmPacketServiceTsy->MessageManager()->HandleRequestL( 
			EPacketSetDefaultContextParams, &package );

    return ret;
    
    }

//----------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::CompleteSetDefaultContextParams
// Completes call SetDefaultContextParams to client
//----------------------------------------------------------------------------
//
void CMmPacketServiceGsmWcdmaExt::CompleteSetDefaultContextParams(
		TInt aErrorCause )
    {

	if ( KErrNone == aErrorCause )
        {
        iIsSetDefaultContextParamsCalled = ETrue;
    
        iAccessPointName = iTempAccessPointName;
        iPdpType = iTempPdpType;
        iPdpCompression = iTempPdpCompression;

		}

	iMmPacketServiceTsy->CompleteSetDefaultContextParams( aErrorCause );

    }

//----------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::GetDefaultContextParams
// Gets default context parameters
//----------------------------------------------------------------------------
//
TInt CMmPacketServiceGsmWcdmaExt::GetDefaultContextParams(
        TPacketDataConfigBase* aContextConfig ) 
    {

    TInt ret( KErrNone );

    if ( iIsSetDefaultContextParamsCalled )
        {
        // Check that the mode given in aConfig is GPRS
        if ( TPacketDataConfigBase::KConfigGPRS == 
            aContextConfig->ExtensionId() )
            {
            RPacketContext::TContextConfigGPRS configGPRS;

            configGPRS.iPdpType = iPdpType;
            configGPRS.iAccessPointName = iAccessPointName;
            configGPRS.iPdpCompression = iPdpCompression;

            // Set configuration data to ETel side memory
            *( RPacketContext::TContextConfigGPRS* )aContextConfig = 
               configGPRS;
            }

        else if ( TPacketDataConfigBase::KConfigRel99Rel4 == 
            aContextConfig->ExtensionId() )
            {
            RPacketContext::TContextConfigR99_R4 configR99_R4;

            configR99_R4.iPdpType = iPdpType;
            configR99_R4.iAccessPointName = iAccessPointName;
   
            // Set configuration data to ETel side memory
            *( RPacketContext::TContextConfigR99_R4* )aContextConfig = 
                configR99_R4; 
            
            }

        else
            {
            // Given mode is not supported
            ret = KErrNotSupported;
            }
        }
    else
        {
        ret = KErrNotReady;
        }

    return ret;

    }

//----------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::EGprsSupportInCell
// returns EGprs support in cell
//----------------------------------------------------------------------------
//
TBool CMmPacketServiceGsmWcdmaExt::EGprsSupportInCell()
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_EGPRSSUPPORTINCELL_1, "TSY: CMmPacketServiceGsmWcdmaExt::EGprsSupportInCell.\n\t Last Edge Support:%u\n", (TUint)iLastEdgeGprsSupport );

    return iLastEdgeGprsSupport;  
    }

//----------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::SetEdgeGprsSupport
// Defines if edge is supported on cell
//----------------------------------------------------------------------------
//
void CMmPacketServiceGsmWcdmaExt::SetEdgeGprsSupport(
		TBool aLastEdgeGprsSupport )
	{
	iLastEdgeGprsSupport = aLastEdgeGprsSupport;

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_SETEDGEGPRSSUPPORT_1, "TSY: CMmPacketServiceGsmWcdmaExt::SetEdgeGprsSupport.\n\t Last Edge Support:%u\n", (TUint)iLastEdgeGprsSupport );
	}

//----------------------------------------------------------------------------
// CMmPacketServiceGsmWcdmaExt::CompleteTransferCapsChange
// Completes transfer capabilities change to the Service Tsy.
// (other items were commented in a header).
//----------------------------------------------------------------------------
//
void CMmPacketServiceGsmWcdmaExt::CompleteTransferCapsChange(
        CMmDataPackage* aPackage )	
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_COMPLETETRANSFERCAPSCHANGE_1, "TSY: CMmPacketServiceGsmWcdmaExt::CompleteTransferCapsChange" );

    // create temporary variable to hold the tranfer capabilities
    TDynamicTransferCapsFlags transferCaps;
    
    // unpack the data form the adaptation layer, contains the transfer caps
    aPackage->UnPackData( transferCaps );

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_COMPLETETRANSFERCAPSCHANGE_2, "TSY: CMmPacketServiceGsmWcdmaExt::CompleteTransferCapsChange.\n\t Dynamic Caps:%u\n", (TUint)transferCaps );
	
    // create temporary dynamic capability variable from the member variable
    RPacketService::TDynamicCapsFlags dynamicCaps( iDynamicCaps );

    // set on/off the HSUPA dynamic transfer caps FLAG
    ( ( transferCaps & KCapsHSUPA ) == KCapsHSUPA ) ? 
        dynamicCaps |= RPacketService::KCapsHSUPA :
        dynamicCaps &= ~RPacketService::KCapsHSUPA;

    // set on/off the HSDPA dynamic transfer caps FLAG
    ( ( transferCaps & KCapsHSDPA ) == KCapsHSDPA ) ? 
        dynamicCaps |= RPacketService::KCapsHSDPA :
        dynamicCaps &= ~RPacketService::KCapsHSDPA;
	
    // set on/off the EGPRS dynamic transfer caps FLAG
    ( ( transferCaps & KCapsEGPRS ) == KCapsEGPRS ) ? 
        dynamicCaps |= RPacketService::KCapsEGPRS :
        dynamicCaps &= ~RPacketService::KCapsEGPRS;
	    
    if ( dynamicCaps != iDynamicCaps )
        {
        // Set modified dynamic caps to iDynamicCaps
        iDynamicCaps = dynamicCaps;
        // Call complete notify dynamic caps change from service tsy
        iMmPacketServiceTsy->CompleteNotifyDynamicCapsChange( iDynamicCaps );
        }
    }

//-----------------------------------------------------------------------------
// TInt CMmPacketServiceTsy::CompleteGetStaticCaps
// Completes static capabilities
// (other items were commented in a header).
//----------------------------------------------------------------------------
//
void CMmPacketServiceGsmWcdmaExt::CompleteGetStaticCaps( CMmDataPackage* aDataPackage, TInt aError  )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETSERVICEGSMWCDMAEXT_COMPLETEGETSTATICCAPS_1, "TSY: CMmPacketServiceGsmWcdmaExt::CompleteGetStaticCaps.");

	//Unpack necessary data from datapackage
	if (KErrNone == aError)
        { 
        RPacketService::TStaticMiscCaps staticCaps;
        aDataPackage->UnPackData( staticCaps );
	
        // Save current caps values
        iStaticCaps = staticCaps;	
        }
    }


//  End of File
