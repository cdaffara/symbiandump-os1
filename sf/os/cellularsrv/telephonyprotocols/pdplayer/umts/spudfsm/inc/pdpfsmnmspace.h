// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Common header for pdp FSM
// 
//

/**
 @file 
 @internalComponent
*/

#ifndef PDPFSMNMSPACE_H
#define PDPFSMNMSPACE_H

#include <etelqos.h>
#include <networking/mbmsparams.h>//for MBMS Extension Set Operation

namespace PdpFsm
{
// Needs to be big enough to hold the largest of the context config structures
const TInt KContextConfigBufferSize = 1024;
/** completion and notification signals */
enum TEtelSignal
	{
	EPhoneOpened,
	EPhoneOpenedFailed,
	
	EQoSSet,
	EQoSSetFailed,
	ETftSet,
	ETftSetFailed,
	ETftChanged,
	ETftChangedFailed,
	
	E1ryPdpContextCreated,
	E1ryPdpContextCreatedFailed,
	E2ryPdpContextCreated,
	E2ryPdpContextCreatedFailed,
	
	EMbmsPdpContextCreated,
	EMbmsPdpContextCreatedFailed,
	EMbmsActivated,
	EMbmsActivatedFailed,
	EPdpNegQoSRetrieved,
	EPdpNegQoSRetrievedFailed,
	EPdpContextModified,
	EPdpContextModifiedFailed,
	EPdpActivated,
	EPdpActivatedFailed,
	EContextDeleted,
	EContextDeletedFailed,
	// add new signals here
	
	// network signals
	EQoSProfileChangeNetwork,
	EConfigGPRSChangeNetwork,
	EContextStatusChangeNetwork,
	EServiceStatusChangeNetwork,
	EPacketStatusChangeNetwork,
	// add new signals here
	
	ESentinelSignal
	};
} // namespace PdpFsm




/**
A buffer to potentially hold one of the following:
Unfortuntely can't use a union for these since they have default ctors.

RPacketQoS::TQoSR99_R4Negotiated
RPacketQoS::TQoSR5Negotiated

@internalComponent
*/
struct TNegotiatedProfileBuffer
    {
private:    
#ifdef SYMBIAN_NETWORKING_UMTSR5
    RPacketQoS::TQoSR5Negotiated iNegotiatedBuffer;
#else
    RPacketQoS::TQoSR99_R4Negotiated iNegotiatedBuffer;
#endif    
    
public:
    inline TInt ExtensionId();

    inline const RPacketQoS::TQoSR99_R4Negotiated& NegotiatedQoSR99_R4();
    inline TNegotiatedProfileBuffer& operator=(const RPacketQoS::TQoSR99_R4Negotiated& aParam);
    
#ifdef SYMBIAN_NETWORKING_UMTSR5    
    inline const RPacketQoS::TQoSR5Negotiated& NegotiatedQoSR5();
    inline TNegotiatedProfileBuffer& operator=(const RPacketQoS::TQoSR5Negotiated& aParam);
#endif    
    };
    

struct TRequestedProfileBuffer
    {
private:
    // Use R5 since its the largest
#ifdef SYMBIAN_NETWORKING_UMTSR5
    RPacketQoS::TQoSR5Requested iRequestedBuffer;
#else
    RPacketQoS::TQoSR99_R4Requested iRequestedBuffer;
#endif  

public:
    inline TInt ExtensionId();

    inline const RPacketQoS::TQoSR99_R4Requested& RequestedQoSR99_R4();
    inline TRequestedProfileBuffer& operator=(const RPacketQoS::TQoSR99_R4Requested& aParam);

#ifdef SYMBIAN_NETWORKING_UMTSR5
    inline const RPacketQoS::TQoSR5Requested& RequestedQoSR5();
    inline TRequestedProfileBuffer& operator=(const RPacketQoS::TQoSR5Requested& aParam);
#endif    
    };


inline TInt TNegotiatedProfileBuffer::ExtensionId()
    {
    return (reinterpret_cast<TPacketDataConfigBase&>(iNegotiatedBuffer)).ExtensionId();
    }


inline const RPacketQoS::TQoSR99_R4Negotiated& TNegotiatedProfileBuffer::NegotiatedQoSR99_R4()
    {
    return reinterpret_cast<RPacketQoS::TQoSR99_R4Negotiated&>(iNegotiatedBuffer);
    }


inline TNegotiatedProfileBuffer& TNegotiatedProfileBuffer::operator=(const RPacketQoS::TQoSR99_R4Negotiated& aParam)
    {
    // Use this tmp value, since its ExtensionId is set when it is constructed.
    // Can't trust the ExtensionId of struct passed in.

#ifdef SYMBIAN_NETWORKING_UMTSR5
    // If we support R5 then store as R5
    RPacketQoS::TQoSR5Negotiated tmp; 
    tmp.iSignallingIndication = EFalse;
    tmp.iSourceStatisticsDescriptor = RPacketQoS::ESourceStatisticsDescriptorUnknown;

#else
    RPacketQoS::TQoSR99_R4Negotiated tmp; 
#endif

    tmp.iBER = aParam.iBER;
    tmp.iDeliverErroneousSDU = aParam.iDeliverErroneousSDU;
    tmp.iDeliveryOrderReqd = aParam.iDeliveryOrderReqd;
    tmp.iGuaranteedRate = aParam.iGuaranteedRate;
    tmp.iMaxRate = aParam.iMaxRate;
    tmp.iMaxSDUSize = aParam.iMaxSDUSize;
    tmp.iSDUErrorRatio = aParam.iSDUErrorRatio;
    tmp.iTrafficClass = aParam.iTrafficClass;
    tmp.iTrafficHandlingPriority = aParam.iTrafficHandlingPriority;
    tmp.iTransferDelay = aParam.iTransferDelay;
    
    iNegotiatedBuffer = tmp;

    return *this;
    }


#ifdef SYMBIAN_NETWORKING_UMTSR5
inline const RPacketQoS::TQoSR5Negotiated& TNegotiatedProfileBuffer::NegotiatedQoSR5()
    {
    return iNegotiatedBuffer;
    }


inline TNegotiatedProfileBuffer& TNegotiatedProfileBuffer::operator=(const RPacketQoS::TQoSR5Negotiated& aParam)
    {
    // Use this tmp value, since its ExtensionId is set when it is constructed.
    // Can't trust the ExtensionId of struct passed in.
    RPacketQoS::TQoSR5Negotiated tmp;

    tmp.iBER = aParam.iBER;
    tmp.iDeliverErroneousSDU = aParam.iDeliverErroneousSDU;
    tmp.iDeliveryOrderReqd = aParam.iDeliveryOrderReqd;
    tmp.iGuaranteedRate = aParam.iGuaranteedRate;
    tmp.iMaxRate = aParam.iMaxRate;
    tmp.iMaxSDUSize = aParam.iMaxSDUSize;
    tmp.iSDUErrorRatio = aParam.iSDUErrorRatio;
    tmp.iTrafficClass = aParam.iTrafficClass;
    tmp.iTrafficHandlingPriority = aParam.iTrafficHandlingPriority;
    tmp.iTransferDelay = aParam.iTransferDelay;
    tmp.iSignallingIndication = aParam.iSignallingIndication;
    tmp.iSourceStatisticsDescriptor = aParam.iSourceStatisticsDescriptor;
 
    iNegotiatedBuffer = tmp;

    return *this;
    }
#endif

    


inline TInt TRequestedProfileBuffer::ExtensionId()
    {
    return (reinterpret_cast<TPacketDataConfigBase&>(iRequestedBuffer)).ExtensionId();
    }

    
inline const RPacketQoS::TQoSR99_R4Requested& TRequestedProfileBuffer::RequestedQoSR99_R4()
    {
    return reinterpret_cast<RPacketQoS::TQoSR99_R4Requested&>(iRequestedBuffer);
    }

    
inline TRequestedProfileBuffer& TRequestedProfileBuffer::operator=(const RPacketQoS::TQoSR99_R4Requested& aParam)
    {
    // Use this tmp value, since its ExtensionId is set when it is constructed.
    // Can't trust the ExtensionId of struct passed in.
#ifdef SYMBIAN_NETWORKING_UMTSR5
    // If we support R5 then store as R5
    RPacketQoS::TQoSR5Requested tmp;
    tmp.iSignallingIndication = EFalse;
    tmp.iSourceStatisticsDescriptor = RPacketQoS::ESourceStatisticsDescriptorUnknown;

#else
    // If we don't support R5 then store as R4/R99
    RPacketQoS::TQoSR99_R4Requested tmp;
#endif

    tmp.iReqTrafficClass = aParam.iReqTrafficClass;
    tmp.iMinTrafficClass = aParam.iMinTrafficClass;
    tmp.iReqDeliveryOrderReqd = aParam.iReqDeliveryOrderReqd;
    tmp.iMinDeliveryOrderReqd = aParam.iMinDeliveryOrderReqd;
    tmp.iReqDeliverErroneousSDU = aParam.iReqDeliverErroneousSDU;
    tmp.iMinDeliverErroneousSDU = aParam.iMinDeliverErroneousSDU;
    tmp.iReqMaxSDUSize = aParam.iReqMaxSDUSize;
    tmp.iMinAcceptableMaxSDUSize = aParam.iMinAcceptableMaxSDUSize;
    tmp.iReqMaxRate = aParam.iReqMaxRate;
    tmp.iMinAcceptableMaxRate = aParam.iMinAcceptableMaxRate;
    tmp.iReqBER = aParam.iReqBER;
    tmp.iMaxBER = aParam.iMaxBER;
    tmp.iReqSDUErrorRatio = aParam.iReqSDUErrorRatio;
    tmp.iMaxSDUErrorRatio = aParam.iMaxSDUErrorRatio;
    tmp.iReqTrafficHandlingPriority = aParam.iReqTrafficHandlingPriority;
    tmp.iMinTrafficHandlingPriority = aParam.iMinTrafficHandlingPriority;
    tmp.iReqTransferDelay = aParam.iReqTransferDelay;
    tmp.iMaxTransferDelay = aParam.iMaxTransferDelay;
    tmp.iReqGuaranteedRate = aParam.iReqGuaranteedRate;
    tmp.iMinGuaranteedRate = aParam.iMinGuaranteedRate;
 
    iRequestedBuffer = tmp;

    return *this;
    }


#ifdef SYMBIAN_NETWORKING_UMTSR5
inline const RPacketQoS::TQoSR5Requested& TRequestedProfileBuffer::RequestedQoSR5()
    {
    return iRequestedBuffer;
    }
    
    
inline TRequestedProfileBuffer& TRequestedProfileBuffer::operator=(const RPacketQoS::TQoSR5Requested& aParam)
    {
    // Use this tmp value, since its ExtensionId is set when it is constructed.
    // Can't trust the ExtensionId of struct passed in.
    RPacketQoS::TQoSR5Requested tmp; 

    tmp.iReqTrafficClass = aParam.iReqTrafficClass;
    tmp.iMinTrafficClass = aParam.iMinTrafficClass;
    tmp.iReqDeliveryOrderReqd = aParam.iReqDeliveryOrderReqd;
    tmp.iMinDeliveryOrderReqd = aParam.iMinDeliveryOrderReqd;
    tmp.iReqDeliverErroneousSDU = aParam.iReqDeliverErroneousSDU;
    tmp.iMinDeliverErroneousSDU = aParam.iMinDeliverErroneousSDU;
    tmp.iReqMaxSDUSize = aParam.iReqMaxSDUSize;
    tmp.iMinAcceptableMaxSDUSize = aParam.iMinAcceptableMaxSDUSize;
    tmp.iReqMaxRate = aParam.iReqMaxRate;
    tmp.iMinAcceptableMaxRate = aParam.iMinAcceptableMaxRate;
    tmp.iReqBER = aParam.iReqBER;
    tmp.iMaxBER = aParam.iMaxBER;
    tmp.iReqSDUErrorRatio = aParam.iReqSDUErrorRatio;
    tmp.iMaxSDUErrorRatio = aParam.iMaxSDUErrorRatio;
    tmp.iReqTrafficHandlingPriority = aParam.iReqTrafficHandlingPriority;
    tmp.iMinTrafficHandlingPriority = aParam.iMinTrafficHandlingPriority;
    tmp.iReqTransferDelay = aParam.iReqTransferDelay;
    tmp.iMaxTransferDelay = aParam.iMaxTransferDelay;
    tmp.iReqGuaranteedRate = aParam.iReqGuaranteedRate;
    tmp.iMinGuaranteedRate = aParam.iMinGuaranteedRate;
    tmp.iSignallingIndication = aParam.iSignallingIndication;
    tmp.iSourceStatisticsDescriptor = aParam.iSourceStatisticsDescriptor;

    iRequestedBuffer = tmp; 
    
    return *this;
    }
#endif    

struct TSessionOperatioInfo
	{
	ConnectionServ::CSubConMBMSExtensionParamSet::TOperationType iOperation;
	RArray<TUint> iSessionIds;	
	inline TSessionOperatioInfo& operator=(const TSessionOperatioInfo& aParam);

	};


typedef TPckg<TNegotiatedProfileBuffer> TNegotiatedProfileBufferPckg;
typedef TPckg<TRequestedProfileBuffer>  TRequestedProfileBufferPckg;

inline TSessionOperatioInfo& TSessionOperatioInfo::operator=(const TSessionOperatioInfo& aParam)
    {

    this->iOperation = aParam.iOperation;
    this->iSessionIds.Reset();
	for(TInt i=0;i<aParam.iSessionIds.Count();i++)
		{
		this->iSessionIds.Append(aParam.iSessionIds[i]);
		}
	return *this;
	
    }
	
#endif
// PDPFSMNMSPACE_H
