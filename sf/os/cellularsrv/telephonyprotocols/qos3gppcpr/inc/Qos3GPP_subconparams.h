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
// Header file for the 3GPP SubConnection Extension Parameters
// 
//

/**
 @file
 @publishedAll
 @released
*/


#ifndef QOS3GPP_SUBCONPARAMS_H
#define QOS3GPP_SUBCONPARAMS_H

#include <es_sock.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <etelqos.h>
#else
#include <eteldefaultqos.h>
#endif
#include <comms-infras/es_parameterfamily.h>

const TInt KSubCon3GPPExtParamsFactoryUid = 0x1020D460;
const TInt KSubConQosR99ParamsType = 1;
const TInt KSubConnSBLPR5ExtensionParamsType = 2;


#ifdef SYMBIAN_NETWORKING_UMTSR5  
const TInt KSubConQosR5ParamsType =3;
const TInt KSubConImsExtParamsType = 4;
#endif 
// SYMBIAN_NETWORKING_UMTSR5 

/** Constant definitions particular to the Generic Parameters. */
const TInt KMAuthTokenLength = 255;
/** Typedef for the AuthToken Holder. */
typedef TBuf8<KMAuthTokenLength> TAuthToken;

/** Provides Implementation of IP QoS Parameters

@publishedAll
@released Since 9.3
*/
//SMetaDataECom must be the first in the superclasses' list
class CSubConQosR99ParamSet : public CSubConExtensionParameterSet
{
public:
	enum 
		{
		EUid = KSubCon3GPPExtParamsFactoryUid,
		EId = KSubConQosR99ParamsType,
		};
public:
	inline static CSubConQosR99ParamSet* NewL(CSubConParameterFamily& aFamily, CSubConParameterFamily::TParameterSetType aType);
	inline static CSubConQosR99ParamSet* NewL(RParameterFamily& aFamily, RParameterFamily::TParameterSetType aType);
	inline static CSubConQosR99ParamSet* NewL();

	inline CSubConQosR99ParamSet();

	inline RPacketQoS::TTrafficClass GetTrafficClass() const;
	inline RPacketQoS::TDeliveryOrder GetDeliveryOrder() const;
	inline RPacketQoS::TErroneousSDUDelivery GetErroneousSDUDelivery() const;
	inline RPacketQoS::TBitErrorRatio GetResidualBitErrorRatio() const;
	inline RPacketQoS::TSDUErrorRatio GetSDUErrorRatio() const;
	inline RPacketQoS::TTrafficHandlingPriority GetTrafficHandlingPriority() const;
	inline TInt GetTransferDelay() const;
	inline TInt GetMaxSduSize() const;
	inline TInt GetMaxBitrateUplink() const;
	inline TInt GetMaxBitrateDownlink() const;
	inline TInt GetGuaBitrateUplink() const;
	inline TInt GetGuaBitrateDownlink() const;
	inline void SetTrafficClass(RPacketQoS::TTrafficClass aTrafficClass);
	inline void SetDeliveryOrder(RPacketQoS::TDeliveryOrder aDeliveryOrder);
	inline void SetErroneousSDUDelivery(RPacketQoS::TErroneousSDUDelivery aDeliveryOfErroneusSdu);
	inline void SetResidualBitErrorRatio(RPacketQoS::TBitErrorRatio aResidualBer);
	inline void SetSDUErrorRatio(RPacketQoS::TSDUErrorRatio aErrorRatio);
	inline void SetTrafficHandlingPriority(RPacketQoS::TTrafficHandlingPriority aPriority);
	inline void SetTransferDelay(TInt aTransferDelay);
	inline void SetMaxSduSize(TInt aMaxSduSize);
	inline void SetMaxBitrateUplink(TInt aMaxBitrateUplink);
	inline void SetMaxBitrateDownlink(TInt aMaxBitrateDownlink);
	inline void SetGuaBitrateUplink(TInt aGuaBitrateUplink);
	inline void SetGuaBitrateDownlink(TInt aGuaBitrateDownlink);

protected:

	DATA_VTABLE

protected:
	RPacketQoS::TTrafficClass				iTrafficClass;			// Traffic class
	RPacketQoS::TDeliveryOrder				iDeliveryOrder;			// Delivery order
	RPacketQoS::TErroneousSDUDelivery		iDeliveryOfErroneusSdu;	// Delivery of erroneous SDUs
	RPacketQoS::TBitErrorRatio				iResidualBer;			// Residual BER
	RPacketQoS::TSDUErrorRatio				iErrorRatio;			// SDU error ratio
	RPacketQoS::TTrafficHandlingPriority	iPriority;				// Traffic handling priority
	TInt									iTransferDelay;			// Transfer delay
	TInt									iMaxSduSize;			// Maximum SDU size
	TInt									iMaxBitrateUplink;		// Maximum bit rate for uplink
	TInt									iMaxBitrateDownlink;	// Maximum bit rate for downlink
	TInt									iGuaBitrateUplink;		// Guaranteed bit rate for uplink
	TInt									iGuaBitrateDownlink;	// Guaranteed bit rate for downlink
	};




#ifdef SYMBIAN_NETWORKING_UMTSR5  

/** Extension QoS Parameter Set for UMTS Release 5 */

class CSubConImsExtParamSet : public CSubConExtensionParameterSet
	{
public:
	enum 
		{
		EUid = KSubCon3GPPExtParamsFactoryUid,
		EId = KSubConImsExtParamsType,
		};
public:
	inline static CSubConImsExtParamSet* NewL(CSubConParameterFamily& aFamily,CSubConParameterFamily::TParameterSetType aType);
	inline static CSubConImsExtParamSet* NewL(RParameterFamily& aFamily,RParameterFamily::TParameterSetType aType);
	inline static CSubConImsExtParamSet* NewL();
	inline CSubConImsExtParamSet();
	
	// New IM CN Signalling Indicator   
	inline TBool GetImsSignallingIndicator() const;
	inline void SetImsSignallingIndicator(TBool aIMSSignallingIndicator);
protected:
	DATA_VTABLE
	
private:	
	// IMSSignallingIndicator for raised priority of traffic 
	TBool iImsSignallingIndicator;
	};

class CSubConQosR5ParamSet : public CSubConQosR99ParamSet
	{
public:
	enum 
		{
		EUid = KSubCon3GPPExtParamsFactoryUid,
		EId = KSubConQosR5ParamsType,
		};
	
public:
	inline static CSubConQosR5ParamSet* NewL(CSubConParameterFamily& aFamily,CSubConParameterFamily::TParameterSetType aType);
	inline static CSubConQosR5ParamSet* NewL(RParameterFamily& aFamily,RParameterFamily::TParameterSetType aType);
	inline static CSubConQosR5ParamSet* NewL();
	inline CSubConQosR5ParamSet();

	inline TBool GetSignallingIndicator() const;
	inline void SetSignallingIndicator(TBool aSignallingIndicator);
	inline RPacketQoS::TSourceStatisticsDescriptor GetSourceStatisticsDescriptor() const;
	inline void SetSourceStatisticsDescriptor(RPacketQoS::TSourceStatisticsDescriptor aSrcStatsDescType);
	
protected:
	DATA_VTABLE
	
private:
	// Source Statistics Descriptor 
	RPacketQoS::TSourceStatisticsDescriptor iSrcStatsDesc;
	TBool iSignallingIndicator;
		
	};

#endif 
// SYMBIAN_NETWORKING_UMTSR5 



/** Flow Identifires
ECOM Implementation Id for SBLP Extension parameters.

@publishedAll
@released Since 9.3
*/
struct TFlowId
	{
	/** Getter Function for the Components of Flow ids. */
	inline TUint16 GetMediaComponentNumber() const;
	inline TUint16 GetIPFlowNumber() const;
	/**	Setter Function For Flow Id Components.	*/
	inline void SetMediaComponentNumber(TUint16 aMediaComponentNumber);
	inline void SetIPFlowNumber(TUint16 aIPFlowNumber);

private:	
	TUint16 iMediaComponentNumber;
	TUint16 iIPFlowNumber;
	};	

typedef RArray<TFlowId> RFlowIdentifiers;

/** Extension Parameter Sets, Consise of MAT and FI(s).
Provides Implementation of Extension parameters of the SBLP Family.

@publishedAll
@released Since 9.3
*/
class CSubConSBLPR5ExtensionParamSet : public CSubConExtensionParameterSet
	{
public:
	enum 
		{
		EUid = KSubCon3GPPExtParamsFactoryUid,
		EId = KSubConnSBLPR5ExtensionParamsType,
		};
	

public:
	inline static CSubConSBLPR5ExtensionParamSet* NewL(CSubConParameterFamily& aFamily, CSubConParameterFamily::TParameterSetType aType);
	inline static CSubConSBLPR5ExtensionParamSet* NewL(RParameterFamily& aFamily, RParameterFamily::TParameterSetType aType);
	inline static CSubConSBLPR5ExtensionParamSet* NewL();

	/**	Media Authorization Token setter and getter functions. */
	inline const TAuthToken& GetMAT() const;
	inline void SetMAT(const TAuthToken& aAuthToken);
	
	/** Flow identifires setter and getter functions. */
	inline TInt GetNumberOfFlowIds() const;
	inline const TFlowId& GetFlowIdAt(TInt aIndex) const;
	
	/** Adding of Flow Identifires into an array. */
	inline void AddFlowIdL(const TFlowId& aFlowId);
	
	/** public constructors so that it can be accessed by factory. */
	inline CSubConSBLPR5ExtensionParamSet();	
	inline ~CSubConSBLPR5ExtensionParamSet();
	
protected:
	DATA_VTABLE

	/** Single Media Authorization Token (MAT). */
	TAuthToken iAuthToken;
	
	/** Multiple Flow Identifiers. */
	RFlowIdentifiers	iFlowIds;
	};

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <networking/qos3gpp_subconparams_factory.h>
#endif

#include <networking/qos3gpp_subconparams.inl>

#endif
// QOS3GPP_SUBCONPARAMS_H
