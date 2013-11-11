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



//  INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmpacketqosgsmwcdmaextTraces.h"
#endif

#include "Cmmpacketservicetsy.h"
#include "Cmmpacketcontexttsy.h"
#include "cmmpacketqosgsmwcdmaext.h"
#include "Cmmpacketqostsy.h"
#include "Cmmpacketdef.h"
#include "etelqos.h"
#include "etelpckt.h"

// ======== MEMBER FUNCTIONS ========

CMmPacketQoSGsmWcdmaExt::CMmPacketQoSGsmWcdmaExt()
    {
    }

CMmPacketQoSGsmWcdmaExt* CMmPacketQoSGsmWcdmaExt::NewL(
        CMmPacketQoSTsy* aMmPacketQoSTsy,     
        CMmPacketServiceTsy* aMmPacketService ) 
    {
    CMmPacketQoSGsmWcdmaExt* packetQoSGsmWcdmaExt =
        new ( ELeave ) CMmPacketQoSGsmWcdmaExt();
    
    CleanupStack::PushL( packetQoSGsmWcdmaExt );
    packetQoSGsmWcdmaExt->iMmPacketQoSTsy = aMmPacketQoSTsy;
    packetQoSGsmWcdmaExt->iMmPacketService = aMmPacketService;
    packetQoSGsmWcdmaExt->ConstructL();
    CleanupStack::Pop( packetQoSGsmWcdmaExt );

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETQOSGSMWCDMAEXT_NEWL_1, "TSY: CMmPacketQoSGsmWcdmaExt::NewL. ");
    return packetQoSGsmWcdmaExt;
    }

void CMmPacketQoSGsmWcdmaExt::ConstructL()
    {
	iQoSGPRSNotifyPckg = NULL;
    iQosAsked = EFalse;
    }
    
CMmPacketQoSGsmWcdmaExt::~CMmPacketQoSGsmWcdmaExt()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETQOSGSMWCDMAEXT_DTOR_1, "TSY: CMmPacketQoSGsmWcdmaExt::~CMmPacketQoSGsmWcdmaExt." );
    }



//----------------------------------------------------------------------------
// CMmPacketQoSGsmWcdmaExt::GetProfileCapabilities
// Gets profile capabilities 
//----------------------------------------------------------------------------

TInt CMmPacketQoSGsmWcdmaExt::GetProfileCapabilities(
        TDes8* aProfileCaps )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETQOSGSMWCDMAEXT_GETPROFILECAPABILITIES_1, "TSY: CMmPacketQoSGsmWcdmaExt::GetProfileCapabilities." );

    // Unpack the packet
    TPckg<TPacketDataConfigBase>* profileCapsPckg = 
        ( TPckg<TPacketDataConfigBase>* ) aProfileCaps;
    TPacketDataConfigBase& profileCaps = ( *profileCapsPckg )();

    // If extension is KConfigGPRS
    if ( TPacketDataConfigBase::KConfigGPRS == profileCaps.ExtensionId() )
        {
        TPckg<RPacketQoS::TQoSCapsGPRS>* gprsProfileCapsPckg = 
            ( TPckg<RPacketQoS::TQoSCapsGPRS>* ) aProfileCaps;
        RPacketQoS::TQoSCapsGPRS& gprsProfileCaps = ( *gprsProfileCapsPckg )();

        gprsProfileCaps.iPrecedence = KMmPrecedence;
        gprsProfileCaps.iDelay = KMmDelay;
        gprsProfileCaps.iReliability = KMmReliability;
        gprsProfileCaps.iPeak = KMmPeak;
        gprsProfileCaps.iMean = KMmMean;
        }

    // Else if extension is KConfigRel99Rel4 or KConfigRel5
    else if ( TPacketDataConfigBase::KConfigRel99Rel4 == 
        profileCaps.ExtensionId() ||
        TPacketDataConfigBase::KConfigRel5 == profileCaps.ExtensionId() )
        {
        TPckg<RPacketQoS::TQoSCapsR99_R4>* gprs99ProfileCapsPckg = 
            ( TPckg<RPacketQoS::TQoSCapsR99_R4>* ) aProfileCaps;
        RPacketQoS::TQoSCapsR99_R4& gprsProfileCaps = 
            ( *gprs99ProfileCapsPckg )();

        gprsProfileCaps.iTrafficClass = KMmTrafficClass;
        gprsProfileCaps.iDeliveryOrderReqd = KMmDeliveryOrderReqd;
        gprsProfileCaps.iDeliverErroneousSDU = KMmErroneousSDUDelivery;
        gprsProfileCaps.iBER = KMmBitErrorRatio;
        gprsProfileCaps.iSDUErrorRatio = KMmSDUErrorRatio;
        gprsProfileCaps.iTrafficHandlingPriority = KMmTrafficHandlingPriority;
        }

    else
        {
        return KErrNotSupported;
        }

    return KErrNone;
    }


//----------------------------------------------------------------------------
// CMmPacketQoSGsmWcdmaExt::GetProfileParameters
// Gets negotiated Qos parameters 
//----------------------------------------------------------------------------

TInt CMmPacketQoSGsmWcdmaExt::GetProfileParameters(
        TDes8* aProfile ) 
    {

    // Unpack the packet
    TPckg<TPacketDataConfigBase>* profilePckg = 
        ( TPckg<TPacketDataConfigBase>* ) aProfile;
    TPacketDataConfigBase& profile = ( *profilePckg )();
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETQOSGSMWCDMAEXT_GETPROFILEPARAMETERS_1, "TSY: CMmPacketQoSGsmWcdmaExt::GetProfileParameters.");

// If extension is KConfigGPRS
    if ( TPacketDataConfigBase::KConfigGPRS == profile.ExtensionId() )
        {
        TPckg<RPacketQoS::TQoSGPRSNegotiated >* gprsProfilePckg = 
                ( TPckg<RPacketQoS::TQoSGPRSNegotiated >* ) aProfile;
        RPacketQoS::TQoSGPRSNegotiated& gprsProfile = ( *gprsProfilePckg )();

        gprsProfile.iPrecedence = iQoSGPRSNegotiated.iPrecedence;
        gprsProfile.iDelay = iQoSGPRSNegotiated.iDelay;
        gprsProfile.iReliability = iQoSGPRSNegotiated.iReliability;
        gprsProfile.iPeakThroughput = iQoSGPRSNegotiated.iPeakThroughput;
        gprsProfile.iMeanThroughput = iQoSGPRSNegotiated.iMeanThroughput;
        }
// If extension is KConfigRel99Rel4
    else if ( TPacketDataConfigBase::KConfigRel99Rel4 == profile.ExtensionId() )
        {
        TPckg<RPacketQoS::TQoSR99_R4Negotiated >* gprsProfilePckg = 
                ( TPckg<RPacketQoS::TQoSR99_R4Negotiated >* ) aProfile;
        RPacketQoS::TQoSR99_R4Negotiated& gprsProfile = ( *gprsProfilePckg )();

        gprsProfile.iTrafficClass = iQoSR99_R4Negotiated.iTrafficClass;
        gprsProfile.iDeliveryOrderReqd =
            iQoSR99_R4Negotiated.iDeliveryOrderReqd;
        gprsProfile.iDeliverErroneousSDU =
            iQoSR99_R4Negotiated.iDeliverErroneousSDU;
        gprsProfile.iMaxSDUSize = iQoSR99_R4Negotiated.iMaxSDUSize;
        gprsProfile.iMaxRate = iQoSR99_R4Negotiated.iMaxRate;
        gprsProfile.iBER = iQoSR99_R4Negotiated.iBER;
        gprsProfile.iSDUErrorRatio = iQoSR99_R4Negotiated.iSDUErrorRatio;
        gprsProfile.iTrafficHandlingPriority 
            = iQoSR99_R4Negotiated.iTrafficHandlingPriority;
        gprsProfile.iTransferDelay = iQoSR99_R4Negotiated.iTransferDelay;
        gprsProfile.iGuaranteedRate = iQoSR99_R4Negotiated.iGuaranteedRate;
        }
   // If extension is KConfigRel5
   else if ( TPacketDataConfigBase::KConfigRel5 == profile.ExtensionId() )
        { 
        
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETQOSGSMWCDMAEXT_GETPROFILEPARAMETERS_2, "TSY: CMmPacketQoSGsmWcdmaExt::GetProfileParameters. Rel 5");
        TPckg<RPacketQoS::TQoSR5Negotiated >* gprsProfilePckg = 
                ( TPckg<RPacketQoS::TQoSR5Negotiated >* ) aProfile;
        RPacketQoS::TQoSR5Negotiated& gprsProfile = ( *gprsProfilePckg )();

        gprsProfile.iTrafficClass = iQoSR5Negotiated.iTrafficClass;
        gprsProfile.iDeliveryOrderReqd =
            iQoSR5Negotiated.iDeliveryOrderReqd;
        gprsProfile.iDeliverErroneousSDU =
            iQoSR5Negotiated.iDeliverErroneousSDU;
        gprsProfile.iMaxSDUSize = iQoSR5Negotiated.iMaxSDUSize;
        gprsProfile.iMaxRate = iQoSR5Negotiated.iMaxRate;
        gprsProfile.iBER = iQoSR5Negotiated.iBER;
        gprsProfile.iSDUErrorRatio = iQoSR5Negotiated.iSDUErrorRatio;
        gprsProfile.iTrafficHandlingPriority =
            iQoSR5Negotiated.iTrafficHandlingPriority;
        gprsProfile.iTransferDelay = iQoSR5Negotiated.iTransferDelay;
        gprsProfile.iGuaranteedRate = iQoSR5Negotiated.iGuaranteedRate;	   
	    gprsProfile.iSignallingIndication = 
            iQoSR5Negotiated.iSignallingIndication;	
	    gprsProfile.iSourceStatisticsDescriptor =
            iQoSR5Negotiated.iSourceStatisticsDescriptor;	
        }        
        
    else 
        {
        return KErrNotSupported;
        }

    return KErrNone;
    }

//----------------------------------------------------------------------------
// CMmPacketQoSGsmWcdmaExt::NotifyProfileChanged
// Saves new profile internally
//----------------------------------------------------------------------------

TInt CMmPacketQoSGsmWcdmaExt::NotifyProfileChanged(
        TDes8* aProfile )    
	{

  	TPckg<TPacketDataConfigBase>* profilePckg = 
            ( TPckg<TPacketDataConfigBase>* ) aProfile;
        TPacketDataConfigBase& profile = ( *profilePckg )();

    if ( TPacketDataConfigBase::KConfigGPRS == profile.ExtensionId() ||
        TPacketDataConfigBase::KConfigRel99Rel4 == profile.ExtensionId()||
        TPacketDataConfigBase::KConfigRel5 == profile.ExtensionId() )
        {
        iQoSGPRSNotifyPckg = aProfile; 
        return KErrNone;
        }

	return KErrNotSupported;

    }

//----------------------------------------------------------------------------
// CMmPacketQoSGsmWcdmaExt::NotifyProfileChangedCancel
// Clears internally saved profile
//----------------------------------------------------------------------------

void CMmPacketQoSGsmWcdmaExt::NotifyProfileChangedCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETQOSGSMWCDMAEXT_NOTIFYPROFILECHANGEDCANCEL_1,  "TSY: CMmPacketQoSGsmWcdmaExt::NotifyProfileChangedCancel." );

	iQoSGPRSNotifyPckg = NULL;

    }

//----------------------------------------------------------------------------
// CMmPacketQoSGsmWcdmaExt::SetProfileParametersL
// Sends request for profile parameters to network
//----------------------------------------------------------------------------

TInt CMmPacketQoSGsmWcdmaExt::SetProfileParametersL(
        TDes8* aProfile ) 
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETQOSGSMWCDMAEXT_SETPROFILEPARAMETERSL_1, "TSY: CMmPacketQoSGsmWcdmaExt::SetProfileParametersL. \t aProfile:%d", aProfile );

	//Datapackage
	CMmDataPackage data;

    // Initialize parameters 
    TInfoName contextName;
    RPacketContext::TProtocolType pdpType;
    TBool contextType( EFalse );
    TInfoName primaryContextName;

    //Get context configuration information 
    TInt ret = iMmPacketQoSTsy->PacketContextTsy()->
        PacketContextGsmWcdmaExt()->GetConfigurationData(
        contextName, pdpType, contextType, primaryContextName );

	 //Get context status
    RPacketContext::TContextStatus contextStatus =
        iMmPacketQoSTsy->PacketContextTsy()->ContextStatus();

	//initialize contextParams struct  
	TContextParams	contextParams; 

	contextParams.iContextName = contextName;
	contextParams.iContextType = contextType;
	contextParams.iPdpType = pdpType;
	contextParams.iPrimaryContextName = primaryContextName;
	contextParams.iContextStatus = contextStatus;
	
	TPckg<TPacketDataConfigBase>* profilePckg = 
						( TPckg<TPacketDataConfigBase>* ) aProfile;
    TPacketDataConfigBase& profile = ( *profilePckg )();

	//Save what client has requested
	if ( TPacketDataConfigBase::KConfigGPRS == profile.ExtensionId() )
		{
        TPckg<RPacketQoS::TQoSGPRSRequested >* gprsProfilePckg = 
            ( TPckg<RPacketQoS::TQoSGPRSRequested >* ) aProfile;
        RPacketQoS::TQoSGPRSRequested& gprsProfile =( *gprsProfilePckg )();
        iRequested = gprsProfile;
        iConfigurationType = TPacketDataConfigBase::KConfigGPRS;        

        iQoSGPRSRequested = ETrue;
        }
    else if ( TPacketDataConfigBase::KConfigRel99Rel4 == profile.ExtensionId() )
        {
        TPckg<RPacketQoS::TQoSR99_R4Requested >* gprsProfilePckg = 
             ( TPckg<RPacketQoS::TQoSR99_R4Requested >* ) aProfile;
        RPacketQoS::TQoSR99_R4Requested& gprsProfile = ( *gprsProfilePckg )();
        iRequested99 = gprsProfile;
        iConfigurationType = TPacketDataConfigBase::KConfigRel99Rel4;

        iQoSR99_R4Requested = ETrue;
        }
    else if ( TPacketDataConfigBase::KConfigRel5 == profile.ExtensionId() )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETQOSGSMWCDMAEXT_SETPROFILEPARAMETERSL_2, "TSY: CMmPacketQoSGsmWcdmaExt::SetProfileParameters. Rel 5");
        TPckg<RPacketQoS::TQoSR5Requested >* gprsProfilePckg = 
             ( TPckg<RPacketQoS::TQoSR5Requested >* ) aProfile;
        RPacketQoS::TQoSR5Requested& gprsProfile = ( *gprsProfilePckg )();
        iRequestedR5 = gprsProfile;
        iConfigurationType = TPacketDataConfigBase::KConfigRel5;

        iQoSR5Requested = ETrue;
        }
    else
        {
        ret = KErrNotSupported;		
        }


    if ( KErrNone == ret )
        {
      
        data.PackData( &contextParams, &aProfile );
        TInt err = iMmPacketService->MessageManager()->HandleRequestL( 
            EPacketQoSSetProfileParams, &data ); 

		if ( KErrNone != err )
			{
			ret = KErrNotSupported;
			}
			
        iQosAsked = ETrue;
        }

    else
        {
        ret = KErrNotSupported;
        }

    return ret;
    }

//----------------------------------------------------------------------------
// CMmPacketQoSGsmWcdmaExt::CompleteSetProfileParameters
// Completes SetProfileParameters request to client
//----------------------------------------------------------------------------

void CMmPacketQoSGsmWcdmaExt::CompleteSetProfileParameters(
		const TInt aErrorValue )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETQOSGSMWCDMAEXT_COMPLETESETPROFILEPARAMETERS_1, "TSY: CMmPacketQoSGsmWcdmaExt::CompleteSetProfileParameters.  \t aErrorValue:%d", aErrorValue );

    // Call CompleteSetProfileParameters from QoS Tsy
    iMmPacketQoSTsy->CompleteSetProfileParameters( aErrorValue );

    }

//----------------------------------------------------------------------------
// CMmPacketQoSGsmWcdmaExt::CompleteSetProfileParameters
// Completes the notification to client.
// save negotiated  qos values (values returned by network) 
//----------------------------------------------------------------------------

void CMmPacketQoSGsmWcdmaExt::CompleteNotifyProfileChanged(
        CMmDataPackage* aDataPackage )
    {

	TInfoName mmPacketContextName;
    
    const CMmPacketDataPackage* data = 
        reinterpret_cast<const CMmPacketDataPackage*>(aDataPackage);
    //Get Qos parameters
    data->GetProfileChanged( iQoSGPRSNegotiated, iQoSR99_R4Negotiated, 
        iQoSR5Negotiated );
    //Unpack data from datapackage
    data->UnPackData( mmPacketContextName );



   
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETQOSGSMWCDMAEXT_COMPLETENOTIFYPROFILECHANGED_1, "TSY: CMmPacketQoSGsmWcdmaExt::CompleteNotifyProfileChanged . COPY new R99/R4 default values");
		//Copy negotiated values to minimun values
		iRequested99.iMinTrafficClass = iQoSR99_R4Negotiated.iTrafficClass;
	    iRequested99.iMinDeliveryOrderReqd = 


									iQoSR99_R4Negotiated.iDeliveryOrderReqd;
	    iRequested99.iMinDeliverErroneousSDU = 


									iQoSR99_R4Negotiated.iDeliverErroneousSDU;
	    iRequested99.iMinAcceptableMaxSDUSize = 


									iQoSR99_R4Negotiated.iMaxSDUSize;
	    iRequested99.iMinAcceptableMaxRate = iQoSR99_R4Negotiated.iMaxRate;
	    iRequested99.iMaxBER = iQoSR99_R4Negotiated.iBER;
	    iRequested99.iMaxSDUErrorRatio = iQoSR99_R4Negotiated.iSDUErrorRatio;
	    iRequested99.iMinTrafficHandlingPriority = 


						iQoSR99_R4Negotiated.iTrafficHandlingPriority;
	    iRequested99.iMaxTransferDelay = iQoSR99_R4Negotiated.iTransferDelay;
		iRequested99.iMinGuaranteedRate = 


										iQoSR99_R4Negotiated.iGuaranteedRate;

		//Copy negotiated values to requested values for default usage of 2nd 
        //context TFT modify
	    iRequested99.iReqTrafficClass = iQoSR99_R4Negotiated.iTrafficClass;
	    iRequested99.iReqDeliveryOrderReqd = 


	                                iQoSR99_R4Negotiated.iDeliveryOrderReqd;
        iRequested99.iReqDeliverErroneousSDU = 


                                    iQoSR99_R4Negotiated.iDeliverErroneousSDU;
        iRequested99.iReqMaxSDUSize = iQoSR99_R4Negotiated.iMaxSDUSize;
        iRequested99.iReqMaxRate = iQoSR99_R4Negotiated.iMaxRate;
        iRequested99.iReqBER = iQoSR99_R4Negotiated.iBER;
        iRequested99.iReqSDUErrorRatio = iQoSR99_R4Negotiated.iSDUErrorRatio;
        iRequested99.iReqTrafficHandlingPriority = 


                                iQoSR99_R4Negotiated.iTrafficHandlingPriority;
        iRequested99.iReqTransferDelay = iQoSR99_R4Negotiated.iTransferDelay;
        iRequested99.iReqGuaranteedRate = iQoSR99_R4Negotiated.iGuaranteedRate;


OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETQOSGSMWCDMAEXT_COMPLETENOTIFYPROFILECHANGED_2, "TSY: CMmPacketQoSGsmWcdmaExt::CompleteNotifyProfileChanged . COPY new R97 default values");
        iRequested.iReqPrecedence = iQoSGPRSNegotiated.iPrecedence;
        iRequested.iMinPrecedence = iQoSGPRSNegotiated.iPrecedence;

        iRequested.iReqDelay = iQoSGPRSNegotiated.iDelay;
        iRequested.iMinDelay = iQoSGPRSNegotiated.iDelay;

        iRequested.iReqReliability = iQoSGPRSNegotiated.iReliability;
        iRequested.iMinReliability = iQoSGPRSNegotiated.iReliability;

        iRequested.iReqPeakThroughput = iQoSGPRSNegotiated.iPeakThroughput;
        iRequested.iMinPeakThroughput = iQoSGPRSNegotiated.iPeakThroughput;

        iRequested.iReqMeanThroughput = iQoSGPRSNegotiated.iMeanThroughput;
        iRequested.iMinMeanThroughput = iQoSGPRSNegotiated.iMeanThroughput;
        


OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETQOSGSMWCDMAEXT_COMPLETENOTIFYPROFILECHANGED_3, "TSY: CMmPacketQoSGsmWcdmaExt::CompleteNotifyProfileChanged . COPY new R5 default values");

        //Copy negotiated values to minimun values
        iRequestedR5.iMinTrafficClass = iQoSR5Negotiated.iTrafficClass;
        iRequestedR5.iMinDeliveryOrderReqd = 
            iQoSR5Negotiated.iDeliveryOrderReqd;
        iRequestedR5.iMinDeliverErroneousSDU = 
            iQoSR5Negotiated.iDeliverErroneousSDU;
        iRequestedR5.iMinAcceptableMaxSDUSize = 
            iQoSR5Negotiated.iMaxSDUSize;
        iRequestedR5.iMinAcceptableMaxRate = iQoSR5Negotiated.iMaxRate;
        iRequestedR5.iMaxBER = iQoSR5Negotiated.iBER;
        iRequestedR5.iMaxSDUErrorRatio = iQoSR5Negotiated.iSDUErrorRatio;
        iRequestedR5.iMinTrafficHandlingPriority = 
            iQoSR5Negotiated.iTrafficHandlingPriority;
        iRequestedR5.iMaxTransferDelay = iQoSR5Negotiated.iTransferDelay;
        iRequestedR5.iMinGuaranteedRate = iQoSR5Negotiated.iGuaranteedRate;

        //Copy negotiated values to requested values for default usage of 2nd 
        //context TFT modify
        iRequestedR5.iReqTrafficClass = iQoSR5Negotiated.iTrafficClass;
        iRequestedR5.iReqDeliveryOrderReqd = 
            iQoSR5Negotiated.iDeliveryOrderReqd;
        iRequestedR5.iReqDeliverErroneousSDU =
            iQoSR5Negotiated.iDeliverErroneousSDU;
        iRequestedR5.iReqMaxSDUSize = iQoSR5Negotiated.iMaxSDUSize;
        iRequestedR5.iReqMaxRate = iQoSR5Negotiated.iMaxRate;
        iRequestedR5.iReqBER = iQoSR5Negotiated.iBER;
        iRequestedR5.iReqSDUErrorRatio = iQoSR5Negotiated.iSDUErrorRatio;
        iRequestedR5.iReqTrafficHandlingPriority = 
            iQoSR5Negotiated.iTrafficHandlingPriority;
        iRequestedR5.iReqTransferDelay = iQoSR5Negotiated.iTransferDelay;
        iRequestedR5.iReqGuaranteedRate = iQoSR5Negotiated.iGuaranteedRate;
        iRequestedR5.iSignallingIndication = 
            iQoSR5Negotiated.iSignallingIndication;


        iRequestedR5.iSourceStatisticsDescriptor = 
            iQoSR5Negotiated.iSourceStatisticsDescriptor;
        
    // If pointer iQoSGPRSNotifyPckg exist
    if ( NULL != iQoSGPRSNotifyPckg )
        {
        TBool completeToClient( EFalse );

        TPckg<TPacketDataConfigBase>* profilePckg = 
                ( TPckg<TPacketDataConfigBase>* ) iQoSGPRSNotifyPckg;
        TPacketDataConfigBase& profile = ( *profilePckg )();

        // if client uses R97 QoS parameters and aQoSNegotiated 
        // have been set
        if ( TPacketDataConfigBase::KConfigGPRS == profile.ExtensionId() &&
            iQoSGPRSRequested )
            {
            completeToClient = ETrue;

            TPckg<RPacketQoS::TQoSGPRSNegotiated >* gprsProfilePckg = 
                ( TPckg<RPacketQoS::TQoSGPRSNegotiated >* ) iQoSGPRSNotifyPckg;
            RPacketQoS::TQoSGPRSNegotiated& gprsProfile = 
                ( *gprsProfilePckg )();

            // Update the client side data through pointers
            gprsProfile = iQoSGPRSNegotiated;

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETQOSGSMWCDMAEXT_COMPLETENOTIFYPROFILECHANGED_4, "TSY: CMmPacketQoSGsmWcdmaExt::CompleteNotifyProfileChanged.  R97/98 QoS Parameters in use");
            }

        // if client uses R99 QoS parameters and aQoS99Negotiated 
        // have been set
        else if ( TPacketDataConfigBase::KConfigRel99Rel4 ==
                 profile.ExtensionId() && iQoSR99_R4Requested )
            {
            completeToClient = ETrue;
                        
                TPckg<RPacketQoS::TQoSR99_R4Negotiated >* gprsProfilePckg = 
                    ( TPckg<RPacketQoS::TQoSR99_R4Negotiated >* ) iQoSGPRSNotifyPckg;
                RPacketQoS::TQoSR99_R4Negotiated& gprsProfile = 
                    ( *gprsProfilePckg )();
                                       
                // Update the client side data through pointers
                gprsProfile = iQoSR99_R4Negotiated;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETQOSGSMWCDMAEXT_COMPLETENOTIFYPROFILECHANGED_5, "TSY: CMmPacketQoSGsmWcdmaExt::CompleteNotifyProfileChanged.  R99/R4 QoS Parameters in use");
            
            }

        else if ( TPacketDataConfigBase::KConfigRel5 ==
                profile.ExtensionId() && iQoSR5Requested )
            {
            completeToClient = ETrue;

            TPckg<RPacketQoS::TQoSR5Negotiated >* gprsProfilePckg = 
                ( TPckg<RPacketQoS::TQoSR5Negotiated >* ) iQoSGPRSNotifyPckg;
            RPacketQoS::TQoSR5Negotiated& gprsProfile = 
                ( *gprsProfilePckg )();

            // Update the client side data through pointers
            gprsProfile = iQoSR5Negotiated;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPACKETQOSGSMWCDMAEXT_COMPLETENOTIFYPROFILECHANGED_6, "TSY: CMmPacketQoSGsmWcdmaExt::CompleteNotifyProfileChanged.  R5 QoS Parameters in use");
            }            

        if ( completeToClient )
            {

            // Null pointers as notification is completed
            iQoSGPRSNotifyPckg = NULL;

            // Call CompleteNotifyProfileChanged from TSY
            iMmPacketQoSTsy->CompleteNotifyProfileChanged();
                                
            }
        }
    }

//----------------------------------------------------------------------------
// CMmPacketQoSGsmWcdmaExt::ProfileAsked
// Return value iQosAsked is true if client has request 
// certain Qos values
//----------------------------------------------------------------------------

TBool CMmPacketQoSGsmWcdmaExt::ProfileAsked()
    {    
    return iQosAsked;    
    }


//----------------------------------------------------------------------------
// CMmPacketQoSGsmWcdmaExt::RequestedR5
// Returns Rel5 Qos values reguested by client
//----------------------------------------------------------------------------
RPacketQoS::TQoSR5Requested* CMmPacketQoSGsmWcdmaExt::RequestedR5()

    {
    return &iRequestedR5;
    }

//----------------------------------------------------------------------------
// CMmPacketQoSGsmWcdmaExt::Requested99
// Returns 99 Qos values reguested by client
//----------------------------------------------------------------------------

RPacketQoS::TQoSR99_R4Requested* CMmPacketQoSGsmWcdmaExt::Requested99()

    {
    return &iRequested99;
    }


//----------------------------------------------------------------------------
// CMmPacketQoSGsmWcdmaExt::Requested97
// Returns 97/98 Qos values reguested by client
//----------------------------------------------------------------------------

RPacketQoS::TQoSGPRSRequested* CMmPacketQoSGsmWcdmaExt::Requested97()
    {
    return &iRequested;
    }

//----------------------------------------------------------------------------
// CMmPacketQoSGsmWcdmaExt::ConfigurationType
// Returns return configuration type
//----------------------------------------------------------------------------

TInt CMmPacketQoSGsmWcdmaExt::ConfigurationType() const
    {
    return iConfigurationType;
    }


//  End of File
