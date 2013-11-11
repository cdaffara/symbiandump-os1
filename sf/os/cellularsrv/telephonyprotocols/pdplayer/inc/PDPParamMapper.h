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
// PDP SubConnection Provider class definition.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_PDPPARAMMAPPER_H
#define SYMBIAN_PDPPARAMMAPPER_H

#include <etelqos.h>
#include <comms-infras/ss_parameterfamilybundle.h>

#include <networking/qos3gpp_subconparams.h>
#include <cs_subconparams.h>
#include <cs_subconevents.h>

class MPDPParamMapper
/** Aggegation of the QoS parameter mapping methods.

@internalTechnology
@released Since 9.4 */
    {
public:

    enum TQosParameterRelease
        {
        KParameterRelInvalid = 0,
        KParameterRelGeneric = 1,
        KParameterRel4Rel99 = 4,
        KParameterRel5 = 5,
        };    
	/**
	Scans given parameter bundle for supported parameter sets
	The first one found gets converted to RPacketQoS::TQoSR5Requested. 
    The supported parameter sets are (in order of priority):
    1) KSubConQosR5ParamsType
    2) KSubConQosR99ParamsType
    3) Generic QoS 
	
	@param aIn RCFParameterFamilyBundle to be converted.
	@param aOut RPacketQoS::TQoSR5Requested to be populated from aIn
	@exception KErrNotSupported - if CUmtsGprsSubConnProvd does not support conversion from what's
	in the supplied parameter bundle.
	*/
	static TQosParameterRelease MapQosParamBundleToEtelL(const ESock::RCFParameterFamilyBundleC& aIn,
	                                                     RPacketQoS::TQoSR5Requested& aOut);

    /**
    Maps QoS parameters received from the network via Etel into a CSubConGenEventParamsGranted class
    for client event notification.

    @param aNetworkQoS  - A pointer to an Etel TPacketDataConfigBase class containing the
    QoS to be mapped into the event.
    @param aGranted - A reference to the CSubConGenEventParamsGranted object that should receive the mapped
    QoS parameters.
    */
    static void MapQosEtelToGrantedParamsL(TPacketDataConfigBase* aNetworkQoS,
                                           CSubConGenEventParamsGranted& aGranted,
	                                       TQosParameterRelease aRequestedRelease);

	static TBool QoSRequested(const ESock::RCFParameterFamilyBundleC& aIn);

protected:
    //-===================================================
    //
    // To Etel mappers
    //
    //-===================================================
    static void MapQosR99ExtensionSetToEtel(const CSubConQosR99ParamSet& aInRequested,
                                            const CSubConQosR99ParamSet* aInAcceptable,
                                            RPacketQoS::TQoSR99_R4Requested& aOut);

    static void MapQosR5ExtensionSetToEtel( const CSubConQosR5ParamSet& aInRequested,
                                            const CSubConQosR5ParamSet* aInAcceptable,
                                            RPacketQoS::TQoSR5Requested& aOut);

    static void MapQosGenericSetToEtel(     const CSubConQosGenericParamSet& aInRequested, 
                                            const CSubConQosGenericParamSet* aInAcceptable,
                                            RPacketQoS::TQoSR99_R4Requested& aOut);
	/** 
	Given a set of generic parameters, the method derives the R99 value for the traffic class.

	@param aIn requested CSubConQosGenericParamSet to be converted
	@return the value of RPacketQoS::TTrafficClass
	*/	
	static RPacketQoS::TTrafficClass DeriveTrafficClass(const CSubConQosGenericParamSet& aIn );
	
	/** 
	Given a set of generic parameters, the method derives the R99 value for the traffic priority.

	@param aIn requested CSubConQosGenericParamSet to be converted
	@return the value of RPacketQoS::TTrafficHandlingPriority
	*/		
	static RPacketQoS::TTrafficHandlingPriority DeriveTrafficPriority(const CSubConQosGenericParamSet& aIn, TUint aTrafficClass );
	
    //-===================================================
    //
    // From Etel mappers
    //
    //-===================================================
    static void MapQosEtelToGenericSetL(RPacketQoS::TQoSR99_R4Negotiated* aNetworkQoS,
                                        CSubConGenEventParamsGranted& aGranted);
    static void MapQosEtelToR99SetL(RPacketQoS::TQoSR99_R4Negotiated* aNetworkQoS,
                                    CSubConGenEventParamsGranted& aGranted);
    static void MapQosEtelToR5SetL(RPacketQoS::TQoSR5Negotiated* aNetworkQoS,
                                   CSubConGenEventParamsGranted& aGranted);
                             
    };

#endif //SYMBIAN_PDPPARAMMAPPER_H
