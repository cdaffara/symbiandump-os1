// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// PDP QoS Parameter Mapping functionality here.
// 
//

/**
 @file
 @internalComponent
*/

#include "PDPParamMapper.h"

using namespace ESock;
const TInt KQoSMappingBorderDelay = 250;

#define IN_RANGE_INCLUSIVE(value, left, right) (value >= left && value <= right )
#define IN_RANGE_NOT_INCLUSIVE(value, left, right) (value > left && value < right )
#define MIN(left, right) (left < right ? left : right)

TBool MPDPParamMapper::QoSRequested(const RCFParameterFamilyBundleC& aBundle)
	{
	RParameterFamily family=aBundle.FindFamily(KSubConQoSFamily);
    if ( ! family.IsNull())
    	{
    	if (family.FindParameterSet(STypeId::CreateSTypeId(CSubConQosGenericParamSet::EUid,CSubConQosGenericParamSet::EType), RParameterFamily::ERequested) ||
   			family.FindParameterSet(STypeId::CreateSTypeId(KSubCon3GPPExtParamsFactoryUid, KSubConQosR5ParamsType), RParameterFamily::ERequested) ||
    		family.FindParameterSet(STypeId::CreateSTypeId(KSubCon3GPPExtParamsFactoryUid, KSubConQosR99ParamsType), RParameterFamily::ERequested))
			{
			return ETrue;
			}    		
    	}
    	
    return EFalse;
	}

MPDPParamMapper::TQosParameterRelease 
MPDPParamMapper::MapQosParamBundleToEtelL(const ESock::RCFParameterFamilyBundleC& aIn, 
                                                     RPacketQoS::TQoSR5Requested& aOut)
   {
    //Note: There should probably be const and non-const versions of FindFamily. 
    //There is only non-const, hence casting below.
    RParameterFamily family = aIn.FindFamily(KSubConQoSFamily);
	if ( family.IsNull() )
        {
        User::Leave(KErrArgument);
        }


	// check the UMTS R5 extension first
	CSubConQosR5ParamSet* extRequestedR5 = NULL;
	CSubConQosR5ParamSet* extAcceptableR5 = NULL;
	
	extRequestedR5 = static_cast<CSubConQosR5ParamSet*>(family.FindParameterSet(
        STypeId::CreateSTypeId(KSubCon3GPPExtParamsFactoryUid,KSubConQosR5ParamsType), RParameterFamily::ERequested));

	extAcceptableR5 = static_cast<CSubConQosR5ParamSet*>(family.FindParameterSet(
        STypeId::CreateSTypeId(KSubCon3GPPExtParamsFactoryUid,KSubConQosR5ParamsType), RParameterFamily::EAcceptable));

	if (extRequestedR5)
        {
        MapQosR5ExtensionSetToEtel (*extRequestedR5, extAcceptableR5, aOut);
        return KParameterRel5;
        }

	// No R5 so check the UMTS R99/R4 extension next
	CSubConQosR99ParamSet* extRequestedR99 = NULL;
	CSubConQosR99ParamSet* extAcceptableR99 = NULL;
	extRequestedR99 = static_cast<CSubConQosR99ParamSet*>(family.FindParameterSet(
        STypeId::CreateSTypeId(KSubCon3GPPExtParamsFactoryUid,KSubConQosR99ParamsType), RParameterFamily::ERequested));

	extAcceptableR99 = static_cast<CSubConQosR99ParamSet*>(family.FindParameterSet(
        STypeId::CreateSTypeId(KSubCon3GPPExtParamsFactoryUid,KSubConQosR99ParamsType), RParameterFamily::EAcceptable));

	if (extRequestedR99)
        {
        MapQosR99ExtensionSetToEtel (*extRequestedR99, extAcceptableR99, aOut);
        return KParameterRel4Rel99;
        }
      
      
	// Still nothing specific, try to use the generic params
	CSubConQosGenericParamSet* genericRequested = 
		static_cast<CSubConQosGenericParamSet*>(family.FindParameterSet(
	        STypeId::CreateSTypeId(CSubConQosGenericParamSet::EUid,CSubConQosGenericParamSet::EType),
				RParameterFamily::ERequested));
	CSubConQosGenericParamSet* genericAcceptable = 
		static_cast<CSubConQosGenericParamSet*>(family.FindParameterSet(
	        STypeId::CreateSTypeId(CSubConQosGenericParamSet::EUid,CSubConQosGenericParamSet::EType),
				RParameterFamily::EAcceptable));

    if (genericRequested)
        {
        MapQosGenericSetToEtel (*genericRequested, genericAcceptable, aOut);
        return KParameterRelGeneric;
        }
    	
    // Nothing left that we support
    User::Leave(KErrNotSupported);
    return KParameterRelInvalid;
    }

    
/** 
Maps given requested and acceptable sets of R99/R4 parameters as defined CSubConQosR99ParamSet
into the Etel format (RPacketQoS::TQoSR99_R4Requested).

@param aInRequested requested CSubConQosR99ParamSet to be converted
@param aInAcceptable optional acceptable CSubConQosR99ParamSet to be converted
@param aOut RPacketQoS::TQoSR99_R4Requested derived class to be mapped into.
*/
void MPDPParamMapper::MapQosR99ExtensionSetToEtel(
                                const CSubConQosR99ParamSet& aInRequested,
                                const CSubConQosR99ParamSet* aInAcceptable, 
                                RPacketQoS::TQoSR99_R4Requested& aOut)
   {
	aOut.iReqTrafficClass =  aInRequested.GetTrafficClass();
	aOut.iReqDeliveryOrderReqd = aInRequested.GetDeliveryOrder();
	aOut.iReqDeliverErroneousSDU = aInRequested.GetErroneousSDUDelivery();
	aOut.iReqBER = aInRequested.GetResidualBitErrorRatio();
	aOut.iReqSDUErrorRatio = aInRequested.GetSDUErrorRatio();
	aOut.iReqTrafficHandlingPriority = aInRequested.GetTrafficHandlingPriority();
	aOut.iReqTransferDelay = aInRequested.GetTransferDelay();
	aOut.iReqMaxSDUSize = aInRequested.GetMaxSduSize();
	aOut.iReqMaxRate.iUplinkRate = aInRequested.GetMaxBitrateUplink();
	aOut.iReqMaxRate.iDownlinkRate = aInRequested.GetMaxBitrateDownlink();
	aOut.iReqGuaranteedRate.iUplinkRate = aInRequested.GetGuaBitrateUplink();
	aOut.iReqGuaranteedRate.iDownlinkRate = aInRequested.GetGuaBitrateDownlink();
	
	if (!aInAcceptable)
	   {
	   // when aInAcceptable is ommitted the minimum is the requested
	   aInAcceptable = &aInRequested;
	   }
	   
	aOut.iMinTrafficClass = aInAcceptable->GetTrafficClass();
	aOut.iMinDeliveryOrderReqd = aInAcceptable->GetDeliveryOrder();
	aOut.iMinDeliverErroneousSDU = aInAcceptable->GetErroneousSDUDelivery();
	aOut.iMaxBER = aInAcceptable->GetResidualBitErrorRatio();
	aOut.iMaxSDUErrorRatio = aInAcceptable->GetSDUErrorRatio();
	aOut.iMinTrafficHandlingPriority = aInAcceptable->GetTrafficHandlingPriority();
	aOut.iMaxTransferDelay = aInAcceptable->GetTransferDelay();
	aOut.iMinAcceptableMaxSDUSize = aInAcceptable->GetMaxSduSize();
	aOut.iMinAcceptableMaxRate.iUplinkRate = aInAcceptable->GetMaxBitrateUplink();
	aOut.iMinAcceptableMaxRate.iDownlinkRate = aInAcceptable->GetMaxBitrateDownlink();
	aOut.iMinGuaranteedRate.iUplinkRate = aInAcceptable->GetGuaBitrateUplink();
	aOut.iMinGuaranteedRate.iDownlinkRate = aInAcceptable->GetGuaBitrateDownlink();
   }
    
  
/** 
Maps given requested and acceptable sets of R5 parameters as defined CSubConQosR5ParamSet
into the Etel format (RPacketQoS::TQoSR5Requested).


@param aInRequested requested CSubConQosR5ParamSet to be converted
@param aInAcceptable optional acceptable CSubConQosR5ParamSet to be converted
@param aOut RPacketQoS::TQoSR5Requested derived class to be mapped into.
*/
void MPDPParamMapper::MapQosR5ExtensionSetToEtel(
                                const CSubConQosR5ParamSet& aInRequested,
                                const CSubConQosR5ParamSet* aInAcceptable, 
                                RPacketQoS::TQoSR5Requested& aOut)
    {
    MapQosR99ExtensionSetToEtel(aInRequested,aInAcceptable,aOut);
    aOut.iSourceStatisticsDescriptor =  aInRequested.GetSourceStatisticsDescriptor();
    aOut.iSignallingIndication = aInRequested.GetSignallingIndicator();
    }
    

    
/** 
Given a set of generic parameters, the method derives the R99 value for the traffic class.


@param aIn requested CSubConQosGenericParamSet to be converted
@return the value of RPacketQoS::TTrafficClass
*/	
RPacketQoS::TTrafficClass MPDPParamMapper::DeriveTrafficClass(const CSubConQosGenericParamSet& aIn )
    {/*
	if ((aIn.GetUpLinkDelay() < 250 && aIn.GetUpLinkDelay() > 0) || 
		(aIn.GetDownLinkDelay() < 250 && aIn.GetDownLinkDelay() > 0))
		return RPacketQoS::ETrafficClassConversational;
	else if (aIn.GetUpLinkDelay() >= 250 || aIn.GetDownLinkDelay() >= 250)
		return RPacketQoS::ETrafficClassStreaming;
	else if ((aIn.GetUpLinkPriority() >= 0 && aIn.GetUpLinkPriority() <= 2) || 
		(aIn.GetDownLinkPriority() >= 0 && aIn.GetDownLinkPriority() <= 2))
		return RPacketQoS::ETrafficClassInteractive;
	else
		return RPacketQoS::ETrafficClassBackground;
*/
    //-----------------------------------
    // Traffic class
    //-----------------------------------
    if (IN_RANGE_NOT_INCLUSIVE(aIn.GetUpLinkDelay(), 0, KQoSMappingBorderDelay) ||
        IN_RANGE_NOT_INCLUSIVE(aIn.GetDownLinkDelay(), 0, KQoSMappingBorderDelay) )
	    {
	    return RPacketQoS::ETrafficClassConversational;
	    }
    else if (aIn.GetUpLinkDelay() >= KQoSMappingBorderDelay || 
    		 aIn.GetDownLinkDelay() >= KQoSMappingBorderDelay )
	    {
	    return RPacketQoS::ETrafficClassStreaming;	
	    }  
    else if (IN_RANGE_INCLUSIVE(aIn.GetUpLinkPriority(), 0, 2) ||
    		 IN_RANGE_INCLUSIVE(aIn.GetDownLinkPriority(), 0, 2))
	    {
	    return RPacketQoS::ETrafficClassInteractive;	
	    }

	return RPacketQoS::ETrafficClassBackground;	
	}
	
	
/** 
Given a set of generic parameters, the method derives the R99 value for the traffic priority.


@param aIn requested CSubConQosGenericParamSet to be converted
@return the value of RPacketQoS::TTrafficHandlingPriority
*/	
RPacketQoS::TTrafficHandlingPriority MPDPParamMapper::DeriveTrafficPriority(const CSubConQosGenericParamSet& aIn, TUint aTrafficClass)
    {
    //
    // Traffic handling priority
    //
	if (aTrafficClass == RPacketQoS::ETrafficClassInteractive)
		{
	    if (aIn.GetUpLinkPriority() == 1 || aIn.GetDownLinkPriority() == 1)
		    {
		    return RPacketQoS::ETrafficPriority1;	
		    }
	    else if (aIn.GetUpLinkPriority() == 2 || aIn.GetDownLinkPriority() == 2)
		    {
		    return RPacketQoS::ETrafficPriority2;
		    }

		return RPacketQoS::ETrafficPriority3;
		}
		
	return RPacketQoS::ETrafficPriorityUnspecified;
    }
	    
    
/** 
Maps given requested and acceptable sets of generic QoS parameters as defined CSubConQosGenericParamSet
into the Etel format (RPacketQoS::TQoSR99_R4Requested).


@param aInRequested requested CSubConQosGenericParamSet to be converted
@param aInAcceptable optional acceptable CSubConQosGenericParamSet to be converted
@param aOut RPacketQoS::TQoSR99_R4Requested to be produced.
*/	
void MPDPParamMapper::MapQosGenericSetToEtel(
                                const CSubConQosGenericParamSet& aInRequested, 
                                const CSubConQosGenericParamSet* aInAcceptable, 
                                RPacketQoS::TQoSR99_R4Requested& aOut)
	{
	aOut.iReqDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
	aOut.iReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryNotRequired;;
	aOut.iReqBER = RPacketQoS::EBERUnspecified;
    aOut.iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;
    
	aOut.iReqTrafficClass = DeriveTrafficClass(aInRequested);
	aOut.iReqTrafficHandlingPriority = DeriveTrafficPriority(aInRequested, aOut.iReqTrafficClass);
	aOut.iReqTransferDelay = MIN(aInRequested.GetUpLinkDelay(), aInRequested.GetDownLinkDelay());
	aOut.iReqMaxSDUSize = aInRequested.GetUpLinkMaximumPacketSize();
	aOut.iReqMaxRate.iUplinkRate = aInRequested.GetUplinkBandwidth();
	aOut.iReqMaxRate.iDownlinkRate = aInRequested.GetDownlinkBandwidth();
	aOut.iReqGuaranteedRate.iUplinkRate = aInRequested.GetUplinkBandwidth();
	aOut.iReqGuaranteedRate.iDownlinkRate = aInRequested.GetDownlinkBandwidth();
	
	if (!aInAcceptable)
	   {
	   // when aInAcceptable is ommitted the minimum is the requested
	   aInAcceptable = &aInRequested;
	   }
	
	aOut.iMinDeliveryOrderReqd = RPacketQoS::EDeliveryOrderUnspecified;
	aOut.iMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryNotRequired;
	aOut.iMaxBER = RPacketQoS::EBERUnspecified;
	aOut.iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioUnspecified;

	aOut.iMinTrafficClass = DeriveTrafficClass(*aInAcceptable);
	aOut.iMinTrafficHandlingPriority = DeriveTrafficPriority(*aInAcceptable, aOut.iMinTrafficClass);
	aOut.iMaxTransferDelay = MIN(aInAcceptable->GetUpLinkDelay(), aInAcceptable->GetDownLinkDelay());
	aOut.iMinAcceptableMaxSDUSize = aInAcceptable->GetUpLinkMaximumPacketSize();
	aOut.iMinAcceptableMaxRate.iUplinkRate = aInAcceptable->GetUplinkBandwidth();
	aOut.iMinAcceptableMaxRate.iDownlinkRate = aInAcceptable->GetDownlinkBandwidth();
	aOut.iMinGuaranteedRate.iUplinkRate = aInAcceptable->GetUplinkBandwidth();
	aOut.iMinGuaranteedRate.iDownlinkRate = aInAcceptable->GetDownlinkBandwidth();
	}

/**
Maps QoS parameters received from the network via Etel into a CSubConGenEventParamsGranted class
so that it is usable by the RSubConnection API.

@param aNetworkQoS  - A pointer to an Etel TPacketDataConfigBase class containing the
QoS to be mapped into the QoS event.
@param aGranted - A reference to the CSubConGenEventParamsGranted object that should receive the mapped
QoS parameters.
*/
void MPDPParamMapper::MapQosEtelToGrantedParamsL(TPacketDataConfigBase* aNetworkQoS, 
                                                 CSubConGenEventParamsGranted& aGranted,
                                                 TQosParameterRelease aRequestedRelease)
    {
    ASSERT (aNetworkQoS);
    
    // Always set the generic set
    MapQosEtelToGenericSetL((static_cast<RPacketQoS::TQoSR99_R4Negotiated*>(aNetworkQoS)), aGranted);
    
    switch (aRequestedRelease)
        {
        case KParameterRel5:
            // Control client sent us an R5 qos request
            MapQosEtelToR5SetL((static_cast<RPacketQoS::TQoSR5Negotiated*>(aNetworkQoS)), aGranted);
            break;
        case KParameterRelInvalid:
            // This will be the case if the client hasn't actually requested a level of QoS yet
            // but the network has notified us of a QoS change. We don't know what the client might
            // support so return both sets (if the define is set, else just what the code supports)
            MapQosEtelToR5SetL((static_cast<RPacketQoS::TQoSR5Negotiated*>(aNetworkQoS)), aGranted);
            MapQosEtelToR99SetL((static_cast<RPacketQoS::TQoSR99_R4Negotiated*>(aNetworkQoS)), aGranted);
            break;
            
        case KParameterRel4Rel99:
            // Control client sent us an R4/R99 qos request
            MapQosEtelToR99SetL((static_cast<RPacketQoS::TQoSR99_R4Negotiated*>(aNetworkQoS)), aGranted);
            break;
        
        case KParameterRelGeneric:
            // Generic set always returned. Done above.
            break;
        }
    }
    
/**
Maps QoS parameters received from the network via Etel into a generic set and sets it in the given event.

@param aNetworkQoS  - A pointer to an Etel RPacketQoS::TQoSR99_R4Requested derived class containing the QoS
to be mapped.
@param aFamily - A reference to the CSubConGenEventParamsGranted object that should receive the
generic QoS parameters.
*/
void MPDPParamMapper::MapQosEtelToGenericSetL(RPacketQoS::TQoSR99_R4Negotiated* aNetworkQoS,
                                              CSubConGenEventParamsGranted& aGranted)
   {
   ASSERT (aNetworkQoS);
   
   CSubConQosGenericParamSet* genericQoS = CSubConQosGenericParamSet::NewL ();

   genericQoS->SetDownlinkBandwidth (aNetworkQoS->iGuaranteedRate.iDownlinkRate);
   genericQoS->SetUplinkBandwidth (aNetworkQoS->iGuaranteedRate.iUplinkRate);
   genericQoS->SetDownLinkDelay (aNetworkQoS->iTransferDelay);
   genericQoS->SetUpLinkDelay (aNetworkQoS->iTransferDelay);
   genericQoS->SetDownLinkMaximumPacketSize (aNetworkQoS->iMaxSDUSize);
   genericQoS->SetUpLinkMaximumPacketSize (aNetworkQoS->iMaxSDUSize);
   
   switch (aNetworkQoS->iTrafficHandlingPriority) 
      {
         case RPacketQoS::ETrafficPriority1:
            genericQoS->SetDownLinkPriority (RPacketQoS::ETrafficPriority1);
            genericQoS->SetUpLinkPriority (RPacketQoS::ETrafficPriority1);
            break;

         case RPacketQoS::ETrafficPriority2:
            genericQoS->SetDownLinkPriority (RPacketQoS::ETrafficPriority2);
            genericQoS->SetUpLinkPriority (RPacketQoS::ETrafficPriority2);
            break;

         case RPacketQoS::ETrafficPriority3:
            genericQoS->SetDownLinkPriority (RPacketQoS::ETrafficPriority3);
            genericQoS->SetUpLinkPriority (RPacketQoS::ETrafficPriority3);
            break;
            
         default:
            genericQoS->SetDownLinkPriority (RPacketQoS::ETrafficPriorityUnspecified);
            genericQoS->SetUpLinkPriority (RPacketQoS::ETrafficPriorityUnspecified);
            break;
      }
      
   aGranted.SetGenericSet (genericQoS);
   }
   
/**
Maps QoS parameters received from the network via Etel into a 3GPP Release R4/R99 set and
sets it in the given event.

@param aNetworkQoS  - A pointer to an Etel RPacketQoS::TQoSR99_R4Requested derived class containing
the QoS to be mapped.
@param aGranted - A reference to the CSubConGenEventParamsGranted object that should receive the
extension QoS parameters.
*/
void MPDPParamMapper::MapQosEtelToR99SetL(RPacketQoS::TQoSR99_R4Negotiated* aNetworkQoS,
                                          CSubConGenEventParamsGranted& aGranted)
   {
   ASSERT (aNetworkQoS);
   
   CSubConQosR99ParamSet* r99Extension = CSubConQosR99ParamSet::NewL ();
   CleanupStack::PushL(r99Extension);
   r99Extension->SetTrafficClass(aNetworkQoS->iTrafficClass);
   r99Extension->SetDeliveryOrder(aNetworkQoS->iDeliveryOrderReqd);
   r99Extension->SetErroneousSDUDelivery(aNetworkQoS->iDeliverErroneousSDU);
   r99Extension->SetResidualBitErrorRatio(aNetworkQoS->iBER);
   r99Extension->SetSDUErrorRatio(aNetworkQoS->iSDUErrorRatio);
   r99Extension->SetTrafficHandlingPriority(aNetworkQoS->iTrafficHandlingPriority);
   r99Extension->SetTransferDelay(aNetworkQoS->iTransferDelay);
   r99Extension->SetMaxSduSize(aNetworkQoS->iMaxSDUSize);
   r99Extension->SetMaxBitrateUplink(aNetworkQoS->iMaxRate.iUplinkRate);
   r99Extension->SetMaxBitrateDownlink(aNetworkQoS->iMaxRate.iDownlinkRate);
   r99Extension->SetGuaBitrateUplink(aNetworkQoS->iGuaranteedRate.iUplinkRate);
   r99Extension->SetGuaBitrateDownlink(aNetworkQoS->iGuaranteedRate.iDownlinkRate);
   
   aGranted.AddExtensionSetL (r99Extension);
   CleanupStack::Pop(r99Extension);
   }

/**
Maps QoS parameters received from the network via Etel into a 3GPP Release R5 set and
sets it in the given event.

@param aNetworkQoS  - A pointer to an Etel RPacketQoS::TQoSR5Requested derived class containing
the QoS to be mapped.
@param aGranted - A reference to the CSubConGenEventParamsGranted object that should receive the
extension QoS parameters.
*/
void MPDPParamMapper::MapQosEtelToR5SetL(RPacketQoS::TQoSR5Negotiated* aNetworkQoS,
                                         CSubConGenEventParamsGranted& aGranted)
   {
   ASSERT (aNetworkQoS);
   
   CSubConQosR5ParamSet* r5Extension = CSubConQosR5ParamSet::NewL ();
   CleanupStack::PushL(r5Extension);
   r5Extension->SetTrafficClass(aNetworkQoS->iTrafficClass);
   r5Extension->SetDeliveryOrder(aNetworkQoS->iDeliveryOrderReqd);
   r5Extension->SetErroneousSDUDelivery(aNetworkQoS->iDeliverErroneousSDU);
   r5Extension->SetResidualBitErrorRatio(aNetworkQoS->iBER);
   r5Extension->SetSDUErrorRatio(aNetworkQoS->iSDUErrorRatio);
   r5Extension->SetTrafficHandlingPriority(aNetworkQoS->iTrafficHandlingPriority);
   r5Extension->SetTransferDelay(aNetworkQoS->iTransferDelay);
   r5Extension->SetMaxSduSize(aNetworkQoS->iMaxSDUSize);
   r5Extension->SetMaxBitrateUplink(aNetworkQoS->iMaxRate.iUplinkRate);
   r5Extension->SetMaxBitrateDownlink(aNetworkQoS->iMaxRate.iDownlinkRate);
   r5Extension->SetGuaBitrateUplink(aNetworkQoS->iGuaranteedRate.iUplinkRate);
   r5Extension->SetGuaBitrateDownlink(aNetworkQoS->iGuaranteedRate.iDownlinkRate);

   r5Extension->SetSourceStatisticsDescriptor(aNetworkQoS->iSourceStatisticsDescriptor);
   r5Extension->SetSignallingIndicator(aNetworkQoS->iSignallingIndication);		
   
   aGranted.AddExtensionSetL (r5Extension);
   CleanupStack::Pop(r5Extension);
   }
   
