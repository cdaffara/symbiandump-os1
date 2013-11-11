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
//

/**
 @file
 @publishedAll
 @released
*/

#ifndef __CS_SUBCONPARAMS_INL__
#define __CS_SUBCONPARAMS_INL__

CSubConQosGenericParamSet* CSubConQosGenericParamSet::NewL(CSubConParameterFamily& aFamily, CSubConParameterFamily::TParameterSetType aType)
/** Creates a generic Qos family parameterset.

Note:
The aFamily parameter that is passed into this method will take ownership of the newly 
created CSubConQosGenericParamSet object.  When the family is destroyed, this parameter 
set object will also be destroyed (along with any other parameter sets owned by the family).

@param aFamily Parameter sets.
@param aType Parameter type (Requested/Acceptable/Granted)
@return a CSubConQosGenericParamSet object pointer if successful,
 otherwise leaves with a system error code.
*/
	{
	CSubConQosGenericParamSet* obj = NewL();
	CleanupStack::PushL(obj);
	aFamily.SetGenericSetL(*obj, aType);
	CleanupStack::Pop(obj);
	return obj;
	}

CSubConQosGenericParamSet* CSubConQosGenericParamSet::NewL(RParameterFamily& aFamily, RParameterFamily::TParameterSetType aType)
/** Creates a generic Qos family parameterset.

Note:
The aFamily parameter that is passed into this method will take ownership of the newly 
created CSubConQosGenericParamSet object.  When the family is destroyed, this parameter 
set object will also be destroyed (along with any other parameter sets owned by the family).

@param aFamily Parameter sets.
@param aType Parameter type (Requested/Acceptable/Granted)
@return a CSubConQosGenericParamSet object pointer if successful,
 otherwise leaves with a system error code.
*/
	{
	CSubConQosGenericParamSet* obj = NewL();
	CleanupStack::PushL(obj);
	aFamily.AddParameterSetL(obj, aType);
	CleanupStack::Pop(obj);
	return obj;
	}

CSubConQosGenericParamSet* CSubConQosGenericParamSet::NewL()
/** Creates a generic Qos family parameterset.

@return a CSubConQosGenericParamSet object pointer if successful,
 otherwise leaves with a system error code.
*/
	{
	STypeId typeId = STypeId::CreateSTypeId(CSubConQosGenericParamSet::EUid, CSubConQosGenericParamSet::EType);
	return static_cast<CSubConQosGenericParamSet*>(CSubConParameterSet::NewL(typeId));
	}

CSubConQosGenericParamSet::CSubConQosGenericParamSet()
	: CSubConGenericParameterSet(),
	iDownlinkBandwidth(0),
	iUplinkBandwidth(0),
	iDownLinkMaximumBurstSize(0),
	iUpLinkMaximumBurstSize(0),
	iDownLinkAveragePacketSize(0),
	iUpLinkAveragePacketSize(0),
	iDownLinkMaximumPacketSize(0),
	iUpLinkMaximumPacketSize(0),
	iDownLinkDelay(0),
	iUpLinkDelay(0),
	iDownLinkDelayVariation(0),
	iUpLinkDelayVariation(0),
	iDownLinkPriority(0),
	iUpLinkPriority(0),
	iHeaderMode(EFalse)
/** Empty CSubConQosGenericParamSet constructor
*/
	{
	}

TInt CSubConQosGenericParamSet::GetDownlinkBandwidth() const
/** Gets downlink bandwidth value.

@return downlink bandwidth value. */
	{
	return iDownlinkBandwidth;
	}

TInt CSubConQosGenericParamSet::GetUplinkBandwidth() const
/** Gets uplink bandwidth value.

@return uplink bandwidth value. */
	{
	return iUplinkBandwidth;
	}

TInt CSubConQosGenericParamSet::GetDownLinkMaximumBurstSize() const
/** Gets downlink max burst size client can handle.

@return downlink max burst size . */
	{
	return iDownLinkMaximumBurstSize;
	}

TInt CSubConQosGenericParamSet::GetUpLinkMaximumBurstSize() const
/** Gets uplink max burst size client can handle.

@return uplink max burst size . */
	{
	return iUpLinkMaximumBurstSize;
	}

TInt CSubConQosGenericParamSet::GetDownLinkAveragePacketSize() const
/** Gets downlink average packet size required.

@return downlink average packet size . */
	{
	return iDownLinkAveragePacketSize;
	}

TInt CSubConQosGenericParamSet::GetUpLinkAveragePacketSize() const
/** Gets uplink average packet size required.

@return uplink average packet size . */
	{
	return iUpLinkAveragePacketSize;
	}

TInt CSubConQosGenericParamSet::GetDownLinkMaximumPacketSize() const
/** Gets downlink max packet size client can handle.

@return downlink max packet size . */
	{
	return iDownLinkMaximumPacketSize;
	}

TInt CSubConQosGenericParamSet::GetUpLinkMaximumPacketSize() const
/** Gets uplink max packet size client can handle.

@return uplink max packet size . */
	{
	return iUpLinkMaximumPacketSize;
	}

TInt CSubConQosGenericParamSet::GetDownLinkDelay() const
/** Gets acceptable downlink delay value.

@return downlink delay value. */
	{
	return iDownLinkDelay;
	}

TInt CSubConQosGenericParamSet::GetUpLinkDelay() const
/** Gets acceptable uplink delay value.

@return uplink delay value. */
	{
	return iUpLinkDelay;
	}

TInt CSubConQosGenericParamSet::GetDownLinkDelayVariation() const
/** Gets acceptable downlink delay variation value.

@return downlink delay variation value. */
	{
	return iDownLinkDelayVariation;
	}

TInt CSubConQosGenericParamSet::GetUpLinkDelayVariation() const
/** Gets acceptable uplink delay variation value.

@return uplink delay variation value. */
	{
	return iUpLinkDelayVariation;
	}

TInt CSubConQosGenericParamSet::GetDownLinkPriority() const
/** Gets downlink priority value (relative to other channel's priority).

@return downlink priority value. */
	{
	return iDownLinkPriority;
	}

TInt CSubConQosGenericParamSet::GetUpLinkPriority() const
/** Gets uplink priority value (relative to other channel's priority).

@return uplink priority value. */
	{
	return iUpLinkPriority;
	}

TBool CSubConQosGenericParamSet::GetHeaderMode() const
/** Gets header mode.

@return boolean value indicating whether the header size
should be calculated by the QoS module or specified by the client. */
	{
	return iHeaderMode;
	}

const TName& CSubConQosGenericParamSet::GetName() const
/** Gets Qos Parameter set name.

@return Qos Parameter set name. */
	{
	return iName;
	}

void CSubConQosGenericParamSet::SetDownlinkBandwidth(TInt aDownlinkBandwidth)
/** Sets downlink bandwidth value.

@param aDownlinkBandwidth downlink bandwidth value. */
	{
	iDownlinkBandwidth = aDownlinkBandwidth;
	}

void CSubConQosGenericParamSet::SetUplinkBandwidth(TInt aUplinkBandwidth)
/** Sets uplink bandwidth value.

@param aUplinkBandwidth uplink bandwidth value. */
	{
	iUplinkBandwidth = aUplinkBandwidth;
	}

void CSubConQosGenericParamSet::SetDownLinkMaximumBurstSize(TInt aDownLinkMaximumBurstSize)
/** Sets downlink max burst size client can handle.

@param aDownLinkMaximumBurstSize downlink max burst size . */
	{
	iDownLinkMaximumBurstSize = aDownLinkMaximumBurstSize;
	}

void CSubConQosGenericParamSet::SetUpLinkMaximumBurstSize(TInt aUpLinkMaximumBurstSize)
/** Sets uplink max burst size client can handle.

@param aUpLinkMaximumBurstSize uplink max burst size . */
	{
	iUpLinkMaximumBurstSize = aUpLinkMaximumBurstSize;
	}

void CSubConQosGenericParamSet::SetDownLinkAveragePacketSize(TInt aDownLinkAveragePacketSize)
/** Sets downlink average packet size required.

@param aDownLinkAveragePacketSize downlink average packet size . */
	{
	iDownLinkAveragePacketSize = aDownLinkAveragePacketSize;
	}

void CSubConQosGenericParamSet::SetUpLinkAveragePacketSize(TInt aUpLinkAveragePacketSize)
/** Sets uplink average packet size required.

@param aUpLinkAveragePacketSize uplink average packet size . */
	{
	iUpLinkAveragePacketSize = aUpLinkAveragePacketSize;
	}

void CSubConQosGenericParamSet::SetDownLinkMaximumPacketSize(TInt aDownLinkMaximumPacketSize)
/** Sets downlink max packet size client can handle.

@param aDownLinkMaximumPacketSize downlink max packet size . */
	{
	iDownLinkMaximumPacketSize = aDownLinkMaximumPacketSize;
	}

void CSubConQosGenericParamSet::SetUpLinkMaximumPacketSize(TInt aUpLinkMaximumPacketSize)
/** Sets uplink max packet size client can handle.

@param aUpLinkMaximumPacketSize uplink max packet size . */
	{
	iUpLinkMaximumPacketSize = aUpLinkMaximumPacketSize;
	}

void CSubConQosGenericParamSet::SetDownLinkDelay(TInt aDownLinkDelay)
/** Sets acceptable downlink delay value.

@param aDownLinkDelay downlink delay value. */
	{
	iDownLinkDelay = aDownLinkDelay;
	}

void CSubConQosGenericParamSet::SetUpLinkDelay(TInt aUpLinkDelay)
/** Sets acceptable uplink delay value.

@param aUpLinkDelay uplink delay value. */
	{
	iUpLinkDelay = aUpLinkDelay;
	}

void CSubConQosGenericParamSet::SetDownLinkDelayVariation(TInt aDownLinkDelayVariation)
/** Sets acceptable downlink delay variation value.

@param aDownLinkDelayVariation downlink delay variation value. */
	{
	iDownLinkDelayVariation = aDownLinkDelayVariation;
	}

void CSubConQosGenericParamSet::SetUpLinkDelayVariation(TInt aUpLinkDelayVariation)
/** Sets acceptable uplink delay variation value.

@param aUpLinkDelayVariation uplink delay variation value. */
	{
	iUpLinkDelayVariation = aUpLinkDelayVariation;
	}

void CSubConQosGenericParamSet::SetDownLinkPriority(TInt aDownLinkPriority)
/** Sets downlink priority value (relative to other channel's priority).

@param aDownLinkPriority downlink priority value. */
	{
	iDownLinkPriority = aDownLinkPriority;
	}

void CSubConQosGenericParamSet::SetUpLinkPriority(TInt aUpLinkPriority)
/** Sets uplink priority value (relative to other channel's priority).

@param aUpLinkPriority uplink priority value. */
	{
	iUpLinkPriority = aUpLinkPriority;
	}

void CSubConQosGenericParamSet::SetHeaderMode(TBool aHeaderMode)
/** Sets header mode.

@param aHeaderMode boolean value indicating whether the header size
should be calculated by the QoS module or specified by the client. */
	{
	iHeaderMode = aHeaderMode;
	}

void CSubConQosGenericParamSet::SetName(const TName& aName)
/** Sets Qos Parameter set name.

@param aName Qos Parameter set name. */
	{
	iName = aName;
	}

//=============
CSubConAuthorisationGenericParamSet* CSubConAuthorisationGenericParamSet::NewL(CSubConParameterFamily& aFamily, CSubConParameterFamily::TParameterSetType aType)
/** Creates a generic Authorisation family parameterset.

Note:
The aFamily parameter that is passed into this method will take ownership of the newly 
created CSubConAuthorisationGenericParamSet object.  When the family is destroyed, this 
parameter set object will also be destroyed (along with any other parameter sets owned 
by the family).

@param aFamily parameter sets.
@param aType parameter type (Requested/Acceptable/Granted)
@return a CSubConAuthorisationGenericParamSet object pointer if successful,
 otherwise leaves with a system error code.
*/
	{
	CSubConAuthorisationGenericParamSet* obj = NewL();
	CleanupStack::PushL(obj);
	aFamily.SetGenericSetL(*obj, aType);
	CleanupStack::Pop(obj);
	return obj;
	}

CSubConAuthorisationGenericParamSet* CSubConAuthorisationGenericParamSet::NewL(RParameterFamily& aFamily, RParameterFamily::TParameterSetType aType)
/** Creates a generic Authorisation family parameterset.

Note:
The aFamily parameter that is passed into this method will take ownership of the newly 
created CSubConAuthorisationGenericParamSet object.  When the family is destroyed, this 
parameter set object will also be destroyed (along with any other parameter sets owned 
by the family).

@param aFamily parameter sets.
@param aType parameter type (Requested/Acceptable/Granted)
@return a CSubConAuthorisationGenericParamSet object pointer if successful,
 otherwise leaves with a system error code.
*/
	{
	CSubConAuthorisationGenericParamSet* obj = NewL();
	CleanupStack::PushL(obj);
	aFamily.AddParameterSetL(obj, aType);
	CleanupStack::Pop(obj);
	return obj;
	}

CSubConAuthorisationGenericParamSet* CSubConAuthorisationGenericParamSet::NewL()
/** Creates a generic Authorisation family parameterset.

@return a CSubConAuthorisationGenericParamSet object pointer if successful,
 otherwise leaves with a system error code.
*/
	{
	STypeId typeId = STypeId::CreateSTypeId(CSubConAuthorisationGenericParamSet::EUid, CSubConAuthorisationGenericParamSet::EType);
	return static_cast<CSubConAuthorisationGenericParamSet*>(CSubConParameterSet::NewL(typeId));
	}

CSubConAuthorisationGenericParamSet::CSubConAuthorisationGenericParamSet()
	: CSubConGenericParameterSet(),
	iId(0)
/** Empty CSubConAuthorisationGenericParamSet constructor
*/
	{
	}

TInt CSubConAuthorisationGenericParamSet::GetId() const
/** Gets Id of Authorisation Parameter set.

@return Id of Authorisation Parameter set. */
	{
	return iId;
	}

void CSubConAuthorisationGenericParamSet::SetId(TInt aId)
/** Sets Id of Authorisation Parameter set.

@param aId Id of Authorisation Parameter set. */
	{
	iId = aId;
	}

#endif	// __CS_SUBCONPARAMS_INL__

