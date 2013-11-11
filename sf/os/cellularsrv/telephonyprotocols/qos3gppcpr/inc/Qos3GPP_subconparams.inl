// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Inline Functions file for the 3GPP SubConnection Extension Parameters
// 
//

/**
 @file
 @publishedAll
 @released
*/



#ifndef QOS3GPP_SUBCONPARAMS_INL
#define QOS3GPP_SUBCONPARAMS_INL

/**
@param      aFamily the sub-connection parameter family to which the newly created object is to be added
@param      aType The type of the set from TParameterSetType (ERequested, EAcceptable or EGranted)
@return     pointer to the created object
@publishedAll
@released Since 9.3
*/
CSubConQosR99ParamSet* CSubConQosR99ParamSet::NewL(CSubConParameterFamily& aFamily, CSubConParameterFamily::TParameterSetType aType)
    {
    CSubConQosR99ParamSet* obj = NewL();
    CleanupStack::PushL(obj);
    aFamily.AddExtensionSetL(*obj, aType);
    CleanupStack::Pop(obj);
    return obj;
    }

/**
@param      aFamily the parameter family to which the newly created object is to be added
@param      aType The type of the set from TParameterSetType (ERequested, EAcceptable or EGranted)
@return     pointer to the created object
@publishedAll
@released Since 9.3
*/
CSubConQosR99ParamSet* CSubConQosR99ParamSet::NewL(RParameterFamily& aFamily, RParameterFamily::TParameterSetType aType)
    {
    CSubConQosR99ParamSet* obj = NewL();
    CleanupStack::PushL(obj);
    aFamily.AddParameterSetL(obj, aType);
    CleanupStack::Pop(obj);
    return obj;
    }

/**
@return     pointer to the created object
@publishedAll
@released Since 9.3
*/
CSubConQosR99ParamSet* CSubConQosR99ParamSet::NewL()
    {
    STypeId typeId = STypeId::CreateSTypeId(KSubCon3GPPExtParamsFactoryUid, KSubConQosR99ParamsType);
    return static_cast<CSubConQosR99ParamSet*>(CSubConParameterSet::NewL(typeId));
    }

/**
@publishedAll
@released Since 9.3
*/
CSubConQosR99ParamSet::CSubConQosR99ParamSet()
    : CSubConExtensionParameterSet(),
    iTrafficClass(RPacketQoS::ETrafficClassUnspecified),
    iDeliveryOrder(RPacketQoS::EDeliveryOrderUnspecified),
    iDeliveryOfErroneusSdu(RPacketQoS::EErroneousSDUDeliveryUnspecified),
    iResidualBer(RPacketQoS::EBERUnspecified),
    iErrorRatio(RPacketQoS::ESDUErrorRatioUnspecified),
    iPriority(RPacketQoS::ETrafficPriorityUnspecified),
    iTransferDelay(0),
    iMaxSduSize(0),
    iMaxBitrateUplink(0),
    iMaxBitrateDownlink(0),
    iGuaBitrateUplink(0),
    iGuaBitrateDownlink(0)
    {
    }

/** Identifies the current traffic class. Traffic class is; Converstional, Streaming, Interactive or Background.
See 3GPP TS 23.107.
@return the traffic class
@publishedAll
@released Since 9.3
*/
RPacketQoS::TTrafficClass CSubConQosR99ParamSet::GetTrafficClass() const
    {
    return iTrafficClass;
    }

/** Current delivery order. Indicates whether the bearer shall provide in-sequence SDU delivery or not.
@return Current delivery order. 
@publishedAll
@released Since 9.3
*/
RPacketQoS::TDeliveryOrder CSubConQosR99ParamSet::GetDeliveryOrder() const
    {
    return iDeliveryOrder;
    }

/** Current delivery of erroneous SDUs. Indicates whether SDUs detected as erroneous shall be delivered or discarded.
@return Current delivery of erroneous SDUs. 
@publishedAll
@released Since 9.3
*/
RPacketQoS::TErroneousSDUDelivery CSubConQosR99ParamSet::GetErroneousSDUDelivery() const
    {
    return iDeliveryOfErroneusSdu;
    }

/** Indicates the undetected bit error ratio in the delivered SDUs. If no error detection is
requested, Residual bit error ratio indicates the bit error ratio in
the delivered SDUs.
@return the residual bit error ratio. 
@publishedAll
@released Since 9.3
*/
RPacketQoS::TBitErrorRatio CSubConQosR99ParamSet::GetResidualBitErrorRatio() const
    {
    return iResidualBer;
    }

/** Indicates the fraction of SDUs lost or detected as erroneous.
@return the SDU error ratio.  
@publishedAll
@released Since 9.3
*/
RPacketQoS::TSDUErrorRatio CSubConQosR99ParamSet::GetSDUErrorRatio() const
    {
    return iErrorRatio;
    }

/** Current traffic handling priority. Specifies the relative importance for handling of all SDUs
belonging to the UMTS bearer compared to the SDUs of other bearers. This is defined only for
Interactive traffic class. See 3GPP TS 23.107
@return the traffic handling priority. 
@publishedAll
@released Since 9.3
*/
RPacketQoS::TTrafficHandlingPriority CSubConQosR99ParamSet::GetTrafficHandlingPriority() const
    {
    return iPriority;
    }

/** @return the transfer delay. Indicates maximum delay for 95th percentile of the distribution
of delay for all delivered SDUs during the lifetime of a bearer service, where delay for an SDU
is defined as the time from a request to transfer an SDU at one SAP to its delivery at the other
SAP. 
@publishedAll
@released Since 9.3
*/
TInt CSubConQosR99ParamSet::GetTransferDelay() const
    {
    return iTransferDelay;
    }

/** @return the maximum SDU size. 
@publishedAll
@released Since 9.3
*/
TInt CSubConQosR99ParamSet::GetMaxSduSize() const
    {
    return iMaxSduSize;
    }

/** @return Current maximum bitrate for uplink direction. The traffic is conformant with Maximum
bitrate as long as it follows a token bucket algorithm where token rate equals Maximum bitrate
and bucket size equals Maximum SDU size.
@publishedAll
@released Since 9.3
*/
TInt CSubConQosR99ParamSet::GetMaxBitrateUplink() const
    {
    return iMaxBitrateUplink;
    }

/** Getter function for the maximum downlink bit rate.
@return the maximum downlink bit rate. 
@publishedAll
@released Since 9.3
*/
TInt CSubConQosR99ParamSet::GetMaxBitrateDownlink() const
    {
    return iMaxBitrateDownlink;
    }

/** Getter function for the guaranteed uplink bit rate.
@return the guaranteed uplink bit rate. 
@publishedAll
@released Since 9.3
*/
TInt CSubConQosR99ParamSet::GetGuaBitrateUplink() const
    {
    return iGuaBitrateUplink;
    }

/** Getter function for the guaranteed downlink bit rate.
@return the guaranteed downlink bit rate. 
@publishedAll
@released Since 9.3
*/
TInt CSubConQosR99ParamSet::GetGuaBitrateDownlink() const
    {
    return iGuaBitrateDownlink;
    }

/** Sets the traffic class.
@param aTrafficClass Value to which to set the traffic class. 
@publishedAll
@released Since 9.3
*/
void CSubConQosR99ParamSet::SetTrafficClass(RPacketQoS::TTrafficClass aTrafficClass)
    {
    iTrafficClass = aTrafficClass;
    }

/** Sets the delivery order. Indicates whether in-sequence SDU delivery shall be provided or not.
@param aDeliveryOrder Value to which to set the delivery order. 
@publishedAll
@released Since 9.3
*/
void CSubConQosR99ParamSet::SetDeliveryOrder(RPacketQoS::TDeliveryOrder aDeliveryOrder)
    {
    iDeliveryOrder = aDeliveryOrder;
    }

/** Sets the handling of the delivery of erroneous SDUs
@param aDeliveryOfErroneusSdu Value to which to set the dilvery of erroneous SDUs.
Indicates whether SUDs detected as erroneous shall be delivered or discarded. 
@publishedAll
@released Since 9.3
*/
void CSubConQosR99ParamSet::SetErroneousSDUDelivery(RPacketQoS::TErroneousSDUDelivery aDeliveryOfErroneusSdu)
    {
    iDeliveryOfErroneusSdu = aDeliveryOfErroneusSdu;
    }

/** Sets the residual bit error ratio. Indicates the undetected bit error ratio in the delivered SDUs.
If no error detection is requested, Residual bit error ratio indicates the bit error ratio in the delivered SDUs.
@param aResidualBer Value to which to set the residual bit error ratio. 
@publishedAll
@released Since 9.3
*/
void CSubConQosR99ParamSet::SetResidualBitErrorRatio(RPacketQoS::TBitErrorRatio aResidualBer)
    {
    iResidualBer = aResidualBer;
    }

/** Sets the error ratio. Indicates the fraction of SDUs lost or detected as erroneous.
SDU error ratio is defined only for conforming traffic.
@param aErrorRatio Sets the error ratio. Indicates the fraction of SDUs lost or detected as erroneous.
SDU error ratio is defined only for conforming traffic. 
@publishedAll
@released Since 9.3
*/
void CSubConQosR99ParamSet::SetSDUErrorRatio(RPacketQoS::TSDUErrorRatio aErrorRatio)
    {
    iErrorRatio = aErrorRatio;
    }

/** Sets the traffic handling priority.
@param aPriority Value to which to set the traffic handling priority. 
@publishedAll
@released Since 9.3
*/
void CSubConQosR99ParamSet::SetTrafficHandlingPriority(RPacketQoS::TTrafficHandlingPriority aPriority)
    {
    iPriority = aPriority;
    }

/** Sets the transfer delay. Indicates maximum delay for 95th percentile of the distribution of delay for
all delivered SDUs during the lifetime of a bearer service, where delay for an SDU is defined as the time
from a request to transfer an SDU at one SAP to its delivery at the other SAP.
@param aTransferDelay Value to which to set the transfer delay. 
@publishedAll
@released Since 9.3
*/
void CSubConQosR99ParamSet::SetTransferDelay(TInt aTransferDelay)
    {
    iTransferDelay = aTransferDelay;
    }

/** Sets the maximum SDU size. Defines the maximum allowed SDU size.
@param aMaxSduSize Value to which to set the maximum SDU size. 
@publishedAll
@released Since 9.3
*/
void CSubConQosR99ParamSet::SetMaxSduSize(TInt aMaxSduSize)
    {
    iMaxSduSize = aMaxSduSize;
    }

/** Sets the maximum bitrate for uplink direction. The traffic is conformant with Maximum bitrate as
long as it follows a token bucket algorithm where token rate equals Maximum bitrate and bucket size
equals Maximum SDU size.
@param aMaxBitrate Value to which to set the maximum bitrate for uplink direction.
@publishedAll
@released Since 9.3
*/
void CSubConQosR99ParamSet::SetMaxBitrateUplink(TInt aMaxBitrateUplink)
    {
    iMaxBitrateUplink = aMaxBitrateUplink;
    }

/** Sets the maximum bitrate for downlink direction. The traffic is conformant with Maximum bitrate as
long as it follows a token bucket algorithm where token rate equals Maximum bitrate and bucket size
equals Maximum SDU size.
@param aMaxBitrate Value to which to set the maximum bitrate for downlink direction.
@publishedAll
@released Since 9.3
*/
void CSubConQosR99ParamSet::SetMaxBitrateDownlink(TInt aMaxBitrateDownlink)
    {
    iMaxBitrateDownlink = aMaxBitrateDownlink;
    }

/** Sets the guaranteed bitrate for uplink direction.
@param aGuaBitrateUplink Value to which to set the guaranteed SDU size.
@publishedAll
@released Since 9.3
*/
void CSubConQosR99ParamSet::SetGuaBitrateUplink(TInt aGuaBitrateUplink)
    {
    iGuaBitrateUplink = aGuaBitrateUplink;
    }

/** Sets the guaranteed bitrate for downlink direction.
@param aGuaBitrateDownlink Value to which to set the guaranteed SDU size.
@publishedAll
@released Since 9.3
*/
void CSubConQosR99ParamSet::SetGuaBitrateDownlink(TInt aGuaBitrateDownlink)
    {
    iGuaBitrateDownlink = aGuaBitrateDownlink;
    }


#ifdef SYMBIAN_NETWORKING_UMTSR5
/**
@param aFamily the sub-connection parameter family to which the newly created object is to be added
@param aType The type of the set from TParameterSetType (ERequested, EAcceptable or EGranted)
@return pointer to the created object
*/
CSubConImsExtParamSet* CSubConImsExtParamSet::NewL(CSubConParameterFamily& aFamily, CSubConParameterFamily::TParameterSetType aType)
    {
    CSubConImsExtParamSet* obj = NewL();
    CleanupStack::PushL(obj);
    aFamily.AddExtensionSetL(*obj, aType);
    CleanupStack::Pop(obj);
    return obj;
    }

/**
@param      aFamily the parameter family to which the newly created object is to be added
@param      aType The type of the set from TParameterSetType (ERequested, EAcceptable or EGranted)
@return     pointer to the created object
*/
CSubConImsExtParamSet* CSubConImsExtParamSet::NewL(RParameterFamily& aFamily, RParameterFamily::TParameterSetType aType)
    {
    CSubConImsExtParamSet* obj = NewL();
    CleanupStack::PushL(obj);
    aFamily.AddParameterSetL(obj, aType);
    CleanupStack::Pop(obj);
    return obj;
    }

/**
@return pointer to the created object
*/
CSubConImsExtParamSet* CSubConImsExtParamSet::NewL()
    {
    STypeId typeId = STypeId::CreateSTypeId(KSubCon3GPPExtParamsFactoryUid, KSubConImsExtParamsType);
    return static_cast<CSubConImsExtParamSet*>(CSubConParameterSet::NewL(typeId));
    }

CSubConImsExtParamSet::CSubConImsExtParamSet()
    : iImsSignallingIndicator(EFalse)
    {
    }

/** @return the IMS Signalling Indicator flag for 3GPP R5 dedicated signalling PDP contexts.
*/
TBool CSubConImsExtParamSet::GetImsSignallingIndicator() const
    {
    return iImsSignallingIndicator;
    }

/** Sets the IMS Signalling Indicator flag for 3GPP R5 dedicated signalling PDP contexts.
@param aImsSignallingIndicator Value to which to set the IMS signalling indicator. */
void CSubConImsExtParamSet::SetImsSignallingIndicator(TBool aImsSignallingIndicator)
    {
    iImsSignallingIndicator = aImsSignallingIndicator;
    }

/**
@param      aFamily the sub-connection parameter family to which the newly created object is to be added
@param      aType The type of the set from TParameterSetType (ERequested, EAcceptable or EGranted)
@return     pointer to the created object
*/
CSubConQosR5ParamSet* CSubConQosR5ParamSet::NewL(CSubConParameterFamily& aFamily,CSubConParameterFamily::TParameterSetType aType)
    {
    CSubConQosR5ParamSet* obj = NewL();
    CleanupStack::PushL(obj);
    aFamily.AddExtensionSetL(*obj, aType);
    CleanupStack::Pop(obj);
    return obj;
    }

/**
@param      aFamily the parameter family to which the newly created object is to be added
@param      aType The type of the set from TParameterSetType (ERequested, EAcceptable or EGranted)
@return     pointer to the created object
*/
CSubConQosR5ParamSet* CSubConQosR5ParamSet::NewL(RParameterFamily& aFamily, RParameterFamily::TParameterSetType aType)
    {
    CSubConQosR5ParamSet* obj = NewL();
    CleanupStack::PushL(obj);
    aFamily.AddParameterSetL(obj, aType);
    CleanupStack::Pop(obj);
    return obj;
    }

/**
@return     pointer to the created object
*/
CSubConQosR5ParamSet* CSubConQosR5ParamSet::NewL()
    {
    STypeId typeId = STypeId::CreateSTypeId(KSubCon3GPPExtParamsFactoryUid, KSubConQosR5ParamsType);
    return static_cast<CSubConQosR5ParamSet*>(CSubConParameterSet::NewL(typeId));
    }

/**
*/
CSubConQosR5ParamSet::CSubConQosR5ParamSet()
    : iSrcStatsDesc(RPacketQoS::ESourceStatisticsDescriptorUnknown), iSignallingIndicator(EFalse)
    {
    }

/** Gets the signalling nature of the SDU's. Signalling traffic can have different characteristics
to other interactive traffic, eg higher priority, lower delay and increased peakiness. This attribute
permits enhancing the Radio Access Network (RAN) operation accordingly. See 3GPP TS 23.107
@return the signalling nature of the SDU's.
*/
TBool CSubConQosR5ParamSet::GetSignallingIndicator() const
    {
    return iSignallingIndicator;
    }

/** Sets signalling nature of the SDU's. This attribute is additional to the other QoS
attributes and does not over-ride them. Signalling traffic can have different characteristics
to other interactive traffic, eg higher priority, lower delay and increased peakiness. This
attribute permits enhancing the Radio Access Network (RAN) operation accordingly. An example use of
the Signalling Indication is for IMS signalling traffic. See 3GPP TS 23.107
@param aSignallingIndicator Value to which to set the signalling indicator. */
void CSubConQosR5ParamSet::SetSignallingIndicator(TBool aSignallingIndicator)
    {
    iSignallingIndicator = aSignallingIndicator;
    }

/** @return the source of the SDU's. */
RPacketQoS::TSourceStatisticsDescriptor CSubConQosR5ParamSet::GetSourceStatisticsDescriptor() const
    {
    return iSrcStatsDesc;
    }

/** Sets the source characteristic of the SDU's. Conversational speech has a well-known statistical
behaviour (or the discontinuous transmission (DTX) factor). By being informed that the SDUs for a Radio
Access Bearer (RAB) are generated by a speech source, Radio Access Network (RAN) may, based on experience,
calculate a statistical multiplex gain for use in admission control on the radio and RAN Access interfaces.
@param aSrcStatsDescType Value to which to set the source statistics indicator. */
void CSubConQosR5ParamSet::SetSourceStatisticsDescriptor(RPacketQoS::TSourceStatisticsDescriptor aSrcStatsDescType)
    {
    iSrcStatsDesc = aSrcStatsDescType;
    }

#endif
// SYMBIAN_NETWORKING_UMTSR5

/** the Media Component number as specified in 3GPP TS 29.207
@return the media component number portion of the flow identifier
@publishedAll
@released Since 9.3
*/
TUint16 TFlowId::GetMediaComponentNumber() const
    {
    return iMediaComponentNumber;
    }

/** the IP flow number as specified in 3GPP TS 29.207
@return the IP flow portion of the flow identifier
@publishedAll
@released Since 9.3
*/
TUint16 TFlowId::GetIPFlowNumber() const
    {
    return iIPFlowNumber;
    }

/** Sets the media component number portion of the flow identifier. The Media Component number is specified in 3GPP TS 29.207
@param aMediaComponentNumber the media component portion of the flow identifier
@publishedAll
@released Since 9.3
*/
void TFlowId::SetMediaComponentNumber(TUint16 aMediaComponentNumber)
    {
    iMediaComponentNumber = aMediaComponentNumber;
    }

/**
@param the IP flow portion of the flow identifier.
@publishedAll
@released Since 9.3
*/
void TFlowId::SetIPFlowNumber(TUint16 aIPFlowNumber)
    {
    iIPFlowNumber = aIPFlowNumber;
    }


//===========================
// Implementation Extension class
/**
@publishedAll
@released Since 9.3
*/
CSubConSBLPR5ExtensionParamSet::CSubConSBLPR5ExtensionParamSet()
    : CSubConExtensionParameterSet()
    {
    }

/**
@publishedAll
@released Since 9.3
*/
CSubConSBLPR5ExtensionParamSet::~CSubConSBLPR5ExtensionParamSet()
    {
    iFlowIds.Close();
    }

/**
@param      aFamily the sub-connection parameter family to which the newly created object is to be added
@param      aType The type of the set from TParameterSetType (ERequested, EAcceptable or EGranted)
@return     pointer to the created object
@publishedAll
@released Since 9.3
*/
CSubConSBLPR5ExtensionParamSet* CSubConSBLPR5ExtensionParamSet::NewL(CSubConParameterFamily& aFamily, CSubConParameterFamily::TParameterSetType aType)
    {
    CSubConSBLPR5ExtensionParamSet* sblpExtn = NewL();
    CleanupStack::PushL(sblpExtn);
    aFamily.AddExtensionSetL(*sblpExtn, aType);
    CleanupStack::Pop(sblpExtn);
    return sblpExtn;
    }

/**
@param      aFamily the parameter family to which the newly created object is to be added
@param      aType The type of the set from TParameterSetType (ERequested, EAcceptable or EGranted)
@return     pointer to the created object
@publishedAll
@released Since 9.3
*/
CSubConSBLPR5ExtensionParamSet* CSubConSBLPR5ExtensionParamSet::NewL(RParameterFamily& aFamily, RParameterFamily::TParameterSetType aType)
    {
    CSubConSBLPR5ExtensionParamSet* obj = NewL();
    CleanupStack::PushL(obj);
    aFamily.AddParameterSetL(obj, aType);
    CleanupStack::Pop(obj);
    return obj;
    }

/**
@return     pointer to the created object
@publishedAll
@released Since 9.3
*/
CSubConSBLPR5ExtensionParamSet* CSubConSBLPR5ExtensionParamSet::NewL()
    {
    STypeId typeId = STypeId::CreateSTypeId(KSubCon3GPPExtParamsFactoryUid, KSubConnSBLPR5ExtensionParamsType);
    return static_cast<CSubConSBLPR5ExtensionParamSet*>(CSubConParameterSet::NewL(typeId));
    }

/**
@return     the media authorisation token
@publishedAll
@released Since 9.3
*/
const TAuthToken& CSubConSBLPR5ExtensionParamSet::GetMAT() const
    {
    return iAuthToken;
    }

/**
@param      aAuthToken sets the media authorisation token
@publishedAll
@released Since 9.3
*/
void CSubConSBLPR5ExtensionParamSet::SetMAT(const TAuthToken& aAuthToken)
    {
    iAuthToken = aAuthToken;
    }

/**
@return the number of flow indexes in this session
@publishedAll
@released Since 9.3
*/
TInt CSubConSBLPR5ExtensionParamSet::GetNumberOfFlowIds() const
    {
    return iFlowIds.Count();
    }

/**
@param      aIndex the index of the flow identifier that will be returned.
@return     the flow identifier at the index given by aIndex.
@publishedAll
@released Since 9.3
*/
const TFlowId& CSubConSBLPR5ExtensionParamSet::GetFlowIdAt(TInt aIndex) const
    {
    return iFlowIds[aIndex];
    }

/**         adds a flow identifier to this session.
@param      aFlowId the flow indentifier to be added.
@publishedAll
@released Since 9.3
*/
void CSubConSBLPR5ExtensionParamSet::AddFlowIdL(const TFlowId & aFlowId)
    {
    iFlowIds.AppendL(aFlowId);
    }

#endif
// QOS3GPP_SUBCONPARAMS_INL

