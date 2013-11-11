// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file for the Automated Test Tool
// 
//

/**
 @file 
 @publishedAll
 @released
*/

#ifndef __CS_SUBCONPARAMS_H__
#define __CS_SUBCONPARAMS_H__

#include <e32base.h>
#include <e32std.h>
#include <comms-infras/metadata.h>
#include <comms-infras/metatype.h>
#include <comms-infras/es_parameterfamily.h>

#include <es_sock.h>

//Parameter Types are defined here and enums defined locally in the class
const TInt32 KSubConnQosGenericParamsType = 1;
const TInt32 KSubConnAuthorisationGenericParamsType = 2;
const TInt32 KFlowRequestParametersType = 3;
const TInt32 KSubConnProtocolGenericParamsType = 4;

using Meta::SMetaDataECom;

class CSubConQosGenericParamSet : public CSubConGenericParameterSet
/** Qos generic parameter set.

This class contains a collection of generic QoS family parameters i.e, independent of any technology
and provides the interface to access them.

The following operations are also provided:

GetDownlinkBandwidth()/SetDownlinkBandwidth() - Accessor function for downlink bandwidth value.
GetUplinkBandwidth()/SetUplinkBandwidth() - Accessor function for uplink bandwidth value.
GetDownLinkMaximumBurstSize()/SetDownLinkMaximumBurstSize() - Accessor function for maximum size of downlink burst of data the client can handle.
GetUpLinkMaximumBurstSize()/SetUpLinkMaximumBurstSize() - Accessor function for maximum size of uplink burst of data the client can handle.
GetDownLinkAveragePacketSize()/SetDownLinkAveragePacketSize() - Accessor function for downlink average packet size data.
GetUpLinkAveragePacketSize()/SetUpLinkAveragePacketSize() - Accessor function for uplink average packet size data.
GetDownLinkMaximumPacketSize()/SetDownLinkMaximumPacketSize() - Accessor function for downlink maximum packet size data.
GetUpLinkMaximumPacketSize()/SetUpLinkMaximumPacketSize() - Accessor function for uplink maximum packet size data.
GetDownLinkDelay()/SetDownLinkDelay() - Accessor function for acceptable downlink delay/latency value.
GetUpLinkDelay()/SetUpLinkDelay() - Accessor function for acceptable uplink delay/latency value.
GetDownLinkDelayVariation()/SetDownLinkDelayVariation() - Accessor function for acceptable downlink variation in delay.
GetUpLinkDelayVariation()/SetUpLinkDelayVariation() - Accessor function for acceptable uplink variation in delay.
GetDownLinkPriority()/SetDownLinkPriority() - Accessor function for downlink priority
GetUpLinkPriority()/SetUpLinkPriority() - Accessor function for uplink priority
GetHeaderMode()/SetHeaderMode() - Accessor function for header mode (Specify whether the header size should be calculated by the QoS module or specified by the client)
GetName()/SetName() - Accessor function for QoS Parameters name.

@note SMetaDataECom must be the first in the superclasses' list
@publishedAll
@released since v9.1 */
{
public:
	enum
		{
		EUid=KSubConnGenericParamsImplUid,
		EType=KSubConnQosGenericParamsType
		};

	inline static CSubConQosGenericParamSet* NewL(CSubConParameterFamily& aFamily, CSubConParameterFamily::TParameterSetType aType);
	inline static CSubConQosGenericParamSet* NewL(RParameterFamily& aFamily, RParameterFamily::TParameterSetType aType);
	inline static CSubConQosGenericParamSet* NewL();

	inline CSubConQosGenericParamSet();

	inline TInt GetDownlinkBandwidth() const;
	inline TInt GetUplinkBandwidth() const;
	inline TInt GetDownLinkMaximumBurstSize() const;
	inline TInt GetUpLinkMaximumBurstSize() const;
	inline TInt GetDownLinkAveragePacketSize() const;
	inline TInt GetUpLinkAveragePacketSize() const;
	inline TInt GetDownLinkMaximumPacketSize() const;
	inline TInt GetUpLinkMaximumPacketSize() const;
	inline TInt GetDownLinkDelay() const;
	inline TInt GetUpLinkDelay() const;
	inline TInt GetDownLinkDelayVariation() const;
	inline TInt GetUpLinkDelayVariation() const;
	inline TInt GetDownLinkPriority() const;
	inline TInt GetUpLinkPriority() const;
	inline TBool GetHeaderMode() const;
	inline const TName& GetName() const;

	inline void SetDownlinkBandwidth(TInt);
	inline void SetUplinkBandwidth(TInt);
	inline void SetDownLinkMaximumBurstSize(TInt);
	inline void SetUpLinkMaximumBurstSize(TInt);
	inline void SetDownLinkAveragePacketSize(TInt);
	inline void SetUpLinkAveragePacketSize(TInt);
	inline void SetDownLinkMaximumPacketSize(TInt);
	inline void SetUpLinkMaximumPacketSize(TInt);
	inline void SetDownLinkDelay(TInt);
	inline void SetUpLinkDelay(TInt);
	inline void SetDownLinkDelayVariation(TInt);
	inline void SetUpLinkDelayVariation(TInt);
	inline void SetDownLinkPriority(TInt);
	inline void SetUpLinkPriority(TInt);
	inline void SetHeaderMode(TBool);
	inline void SetName(const TName&);

protected:

	DATA_VTABLE

	TInt iDownlinkBandwidth;
	TInt iUplinkBandwidth;
	TInt iDownLinkMaximumBurstSize;
	TInt iUpLinkMaximumBurstSize;
	TInt iDownLinkAveragePacketSize;
	TInt iUpLinkAveragePacketSize;
	TInt iDownLinkMaximumPacketSize;
	TInt iUpLinkMaximumPacketSize;
	TInt iDownLinkDelay;
	TInt iUpLinkDelay;
	TInt iDownLinkDelayVariation;
	TInt iUpLinkDelayVariation;
	TInt iDownLinkPriority;
	TInt iUpLinkPriority;
	TBool iHeaderMode;
	TName iName;
	};

class CSubConAuthorisationGenericParamSet : public CSubConGenericParameterSet
/** Authorisation generic parameter set.

This class contains authorization generic parameters.

@note SMetaDataECom must be the first in the superclasses' list
@publishedAll
@released since v9.1 */
{
public:
	enum
		{
		EUid=KSubConnGenericParamsImplUid,
		EType=KSubConnAuthorisationGenericParamsType
		};

	inline static CSubConAuthorisationGenericParamSet* NewL(CSubConParameterFamily& aFamily, CSubConParameterFamily::TParameterSetType aType);
	inline static CSubConAuthorisationGenericParamSet* NewL(RParameterFamily& aFamily, RParameterFamily::TParameterSetType aType);
	inline static CSubConAuthorisationGenericParamSet* NewL();

	inline CSubConAuthorisationGenericParamSet();

	inline TInt GetId() const;

	inline void SetId(TInt);

protected:

	DATA_VTABLE

	TInt iId;
	};


#include <cs_subconparams.inl>

#endif	// __CS_SUBCONPARAMS_H__

