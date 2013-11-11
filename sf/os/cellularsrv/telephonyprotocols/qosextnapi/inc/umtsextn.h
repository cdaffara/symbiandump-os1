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
// @file umtsextn.h
// Header file for UMTS (3GPP Release 4) API
// @internalTechnology
// @prototype
//

#ifndef __UMTSEXTN_H__
#define __UMTSEXTN_H__

#include <e32std.h>
#include <in_sock.h>
#include <es_mbuf.h>

#include <networking/qoslib.h>

#if MM_ETEL_API
#include "uscl_qos.h"
#else
#include <etelqos.h>
#endif

const TInt KPdpDataCompression		= 0x01;
const TInt KPdpHeaderCompression	= 0x02;

// Traffic class
enum TUmtsTrafficClass
{
	ETrafficClassUnspecified		= RPacketQoS::ETrafficClassUnspecified,
	ETrafficClassConversational		= RPacketQoS::ETrafficClassConversational,
	ETrafficClassStreaming			= RPacketQoS::ETrafficClassStreaming,
	ETrafficClassInteractive		= RPacketQoS::ETrafficClassInteractive,
	ETrafficClassBackground			= RPacketQoS::ETrafficClassBackground
};

// Delivery order
enum TUmtsDeliveryOrder
{
	EDeliveryOrderUnspecified		= RPacketQoS::EDeliveryOrderUnspecified,
	EDeliveryOrderRequired			= RPacketQoS::EDeliveryOrderRequired,
	EDeliveryOrderNotRequired		= RPacketQoS::EDeliveryOrderNotRequired
};

// Delivery of erroneous SDUs
enum TUmtsErroneousSDUDelivery
{
	EErroneousSDUDeliveryUnspecified	= RPacketQoS::EErroneousSDUDeliveryUnspecified,
	EErroneousSDUDeliveryRequired		= RPacketQoS::EErroneousSDUDeliveryRequired,
	EErroneousSDUDeliveryNotRequired	= RPacketQoS::EErroneousSDUDeliveryNotRequired
};

// Residual BER
enum TUmtsBitErrorRatio
{
	EBERUnspecified				= RPacketQoS::EBERUnspecified,
	EBERFivePerHundred			= RPacketQoS::EBERFivePerHundred,
	EBEROnePerHundred			= RPacketQoS::EBEROnePerHundred,
	EBERFivePerThousand			= RPacketQoS::EBERFivePerThousand,
	EBERFourPerThousand			= RPacketQoS::EBERFourPerThousand,
	EBEROnePerThousand			= RPacketQoS::EBEROnePerThousand,
	EBEROnePerTenThousand		= RPacketQoS::EBEROnePerTenThousand,
	EBEROnePerHundredThousand	= RPacketQoS::EBEROnePerHundredThousand,
	EBEROnePerMillion			= RPacketQoS::EBEROnePerMillion,
	EBERSixPerHundredMillion	= RPacketQoS::EBERSixPerHundredMillion
};

// Error ratio
enum TUmtsSDUErrorRatio
{
	ESDUErrorRatioUnspecified			= RPacketQoS::ESDUErrorRatioUnspecified,
	ESDUErrorRatioOnePerTen				= RPacketQoS::ESDUErrorRatioOnePerTen,
	ESDUErrorRatioOnePerHundred			= RPacketQoS::ESDUErrorRatioOnePerHundred,
	ESDUErrorRatioSevenPerThousand		= RPacketQoS::ESDUErrorRatioSevenPerThousand,
	ESDUErrorRatioOnePerThousand		= RPacketQoS::ESDUErrorRatioOnePerThousand,
	ESDUErrorRatioOnePerTenThousand		= RPacketQoS::ESDUErrorRatioOnePerTenThousand,
	ESDUErrorRatioOnePerHundredThousand	= RPacketQoS::ESDUErrorRatioOnePerHundredThousand,
	ESDUErrorRatioOnePerMillion			= RPacketQoS::ESDUErrorRatioOnePerMillion
};

// Priority
enum TUmtsTrafficHandlingPriority
{
	ETrafficPriorityUnspecified	= RPacketQoS::ETrafficPriorityUnspecified,
	ETrafficPriority1			= RPacketQoS::ETrafficPriority1,
	ETrafficPriority2			= RPacketQoS::ETrafficPriority2,
	ETrafficPriority3			= RPacketQoS::ETrafficPriority3
};

enum TUmtsSourceStatisticsDescriptor	
{
	/** Unknown source statistics descriptor. */
	ESourceStatisticsDescriptorUnknown	= RPacketQoS::ESourceStatisticsDescriptorUnknown,		
	/** Speech source statistics descriptor. */
	ESourceStatisticsDescriptorSpeech	= RPacketQoS::ESourceStatisticsDescriptorSpeech		 
};

// Delay
const TUint KTransferDelayUnspecified	= 0;
const TUint KTransferDelayMax			= 4100;	// 4100 ms

// Max SDU size
const TUint KMaxSDUUnspecified			= 0;
const TUint KMaxSDUMaximum				= 1520;

// Max bitrate
const TUint KMaxBitRateUnspecified		= 0;
const TUint KMaxBitRateMaximum			= 2048;	// 2048 kbps

// Guaranteed bitrate
const TUint KGrtdBitRateUnspecified		= 0;
const TUint KGrtdBitRateMaximum			= 2048;	// 2048 kbps

/**
 * This class contains the QoS parameters defined in 3GPP Release 4 specifications.
 * See 3GPP TS 23.107 for more information about the QoS parameters.
 *
 * @internalTechnology
 * @prototype
 */
class TUmtsQoSParameters
{
	friend class CUmtsQoSPolicy;
public:

	/**
	* Constructor
	*/
	IMPORT_C TUmtsQoSParameters();

	/**
	* @return Current traffic class. Defines the type of application for which the UMTS bearer service 
	* is optimised. 
	*/
	IMPORT_C TUmtsTrafficClass TrafficClass() const;

	/**
	* @return Current delivery order. Indicates whether the UMTS bearer shall provide in-sequence 
	* SDU delivery or not.
	*/
	IMPORT_C TUmtsDeliveryOrder DeliveryOrder() const;

	/**
	* @return Current delivery of erroneous SDUs. Indicates whether SDUs detected as erroneous 
	* shall be delivered or discarded.
	*/
	IMPORT_C TUmtsErroneousSDUDelivery DeliveryOfErroneusSdu() const;

	/**
	* @return Current residual bit error ratio. Indicates the undetected bit error ratio in the 
	* delivered SDUs. If no error detection is requested, Residual bit error ratio indicates the 
	* bit error ratio in the delivered SDUs.
	*/
	IMPORT_C TUmtsBitErrorRatio ResidualBer() const;

	/**
	* @return Current SDU error ratio. Indicates the fraction of SDUs lost or detected as erroneous. 
	* SDU error ratio is defined only for conforming traffic.
	*/
	IMPORT_C TUmtsSDUErrorRatio ErrorRatio() const;

	/**
	* @return Current traffic handling priority. Specifies the relative importance for handling 
	* of all SDUs belonging to the UMTS bearer compared to the SDUs of other bearers. 
	* This is defined only for Interactive traffic class.
	*/
	IMPORT_C TUmtsTrafficHandlingPriority Priority() const;

	/**
	* @return Current transfer delay. Indicates maximum delay for 95th percentile of the distribution 
	* of delay for all delivered SDUs during the lifetime of a bearer service, where delay for an SDU 
	* is defined as the time from a request to transfer an SDU at one SAP to its delivery at the other SAP.
	*/
	IMPORT_C TInt TransferDelay() const;

	/**
	* @return Current maximum SDU size. Defines the maximum allowed SDU size.
	*/
	IMPORT_C TInt MaxSduSize() const;

	/**
	* @return Current maximum bitrate for uplink direction. Maximum number of bits delivered by UMTS and to 
	* UMTS at a SAP within a period of time, divided by the duration of the period. The traffic is conformant 
	* with Maximum bitrate as long as it follows a token bucket algorithm where token rate equals Maximum 
	* bitrate and bucket size equals Maximum SDU size.
	*/
	IMPORT_C TInt MaxBitrateUplink() const;

	/**
	* @return Current maximum bitrate for downlink direction. Maximum number of bits delivered by UMTS and to 
	* UMTS at a SAP within a period of time, divided by the duration of the period. The traffic is conformant 
	* with Maximum bitrate as long as it follows a token bucket algorithm where token rate equals Maximum 
	* bitrate and bucket size equals Maximum SDU size.
	*/
	IMPORT_C TInt MaxBitrateDownlink() const;

	/**
	* @return Current guaranteed bitrate for uplink direction. Guaranteed number of bits delivered by UMTS at 
	* a SAP within a period of time (provided that there is data to deliver), divided by the duration of the 
	* period. The traffic is conformant with the guaranteed bitrate as long as it follows a token bucket 
	* algorithm where token rate equals Guaranteed bitrate and bucket size equals k*Maximum SDU size. 
	* For release 1999, k=1. 
	*/
	IMPORT_C TInt GuaranteedBitrateUplink() const;

	/**
	* @return Current guaranteed bitrate for downlink direction. Guaranteed number of bits delivered by UMTS at 
	* a SAP within a period of time (provided that there is data to deliver), divided by the duration of the 
	* period. The traffic is conformant with the guaranteed bitrate as long as it follows a token bucket 
	* algorithm where token rate equals Guaranteed bitrate and bucket size equals k*Maximum SDU size. 
	* For release 1999, k=1.
	*/
	IMPORT_C TInt GuaranteedBitrateDownlink() const;


	/**
	 * Sets the traffic class. Traffic class defines the type of application for which  
	 * the UMTS bearer service is optimised.
	 *
	 * @param aTrafficClass Value to which to set the traffic class.
	 */
	IMPORT_C TInt SetTrafficClass(TUmtsTrafficClass aTrafficClass);

	/**
	 * Sets the delivery order. Indicates whether the UMTS bearer shall provide in-sequence 
	 * SDU delivery or not.
	 *
	 * @param aDeliveryOrder Value to which to set the delivery order.
	 */
	IMPORT_C TInt SetDeliveryOrder(TUmtsDeliveryOrder aDeliveryOrder);

	/**
	 * Sets the delivery of erroneous SDUs.
	 *
	 * @param aDeliveryOfErroneousSdu Value to which to set the delivery of erroneous SDUs. Indicates 
	 * whether SDUs detected as erroneous shall be delivered or discarded.
	 */
	IMPORT_C TInt SetDeliveryOfErroneusSdu(TUmtsErroneousSDUDelivery aDeliveryOfErroneousSdu);

	/**
	 * Sets the residual bit error ratio. Indicates the undetected bit error ratio in the 
	 * delivered SDUs. If no error detection is requested, Residual bit error ratio indicates the 
	 * bit error ratio in the delivered SDUs.
	 *
	 * @param aResidualBer Value to which to set the residual bit error ratio.
	 */
	IMPORT_C TInt SetResidualBer(TUmtsBitErrorRatio aResidualBer);


	/**
	 * Sets the error ratio. Indicates the fraction of SDUs lost or detected as erroneous. 
	 * SDU error ratio is defined only for conforming traffic.
	 *
	 * @param aErrorRatio Value to which to set the error ratio.
	 */
	IMPORT_C TInt SetErrorRatio(TUmtsSDUErrorRatio aErrorRatio);

	/**
	 * Sets the traffic handling priority. Specifies the relative importance for handling 
	 * of all SDUs belonging to the UMTS bearer compared to the SDUs of other bearers. This 
	 * is defined only for Interactive traffic class.
	 *
	 * @param aPriority Value to which to set the traffic handling priority.
	 */
	IMPORT_C TInt SetPriority(TUmtsTrafficHandlingPriority aPriority);

	/**
	 * Sets the transfer delay. Indicates maximum delay for 95th percentile of the distribution 
	 * of delay for all delivered SDUs during the lifetime of a bearer service, where delay for an SDU 
	 * is defined as the time from a request to transfer an SDU at one SAP to its delivery at the other SAP.
	 *
	 * @param aTransferDelay Value to which to set the transfer delay.
	 */
	IMPORT_C TInt SetTransferDelay(TUint aTransferDelay);

	/**
	 * Sets the maximum SDU size. Defines the maximum allowed SDU size.
	 *
	 * @param aMaxSduSize Value to which to set the maximum SDU size.
	 */
	IMPORT_C TInt SetMaxSduSize(TUint aMaxSduSize);

	/**
	 * Sets the maximum bitrate for uplink direction. Maximum number of bits delivered by UMTS and to 
	 * UMTS at a SAP within a period of time, divided by the duration of the period. The traffic is conformant 
	 * with Maximum bitrate as long as it follows a token bucket algorithm where token rate equals Maximum 
	 * bitrate and bucket size equals Maximum SDU size.
	 *
	 * @param aMaxBitrate Value to which to set the maximum bitrate for uplink direction.
	 */
	IMPORT_C TInt SetMaxBitrateUplink(TUint aMaxBitrate);

	/**
	 * Sets the maximum bitrate for downlink direction. Maximum number of bits delivered by UMTS and to 
	 * UMTS at a SAP within a period of time, divided by the duration of the period. The traffic is conformant 
	 * with Maximum bitrate as long as it follows a token bucket algorithm where token rate equals Maximum 
	 * bitrate and bucket size equals Maximum SDU size.
	 *
	 * @param aMaxBitrate Value to which to set the maximum bitrate for downlink direction.
	 */
	IMPORT_C TInt SetMaxBitrateDownlink(TUint aMaxBitrate);

	/**
	 * Sets the guaranteed bitrate for uplink direction. Guaranteed number of bits delivered by UMTS at 
	 * a SAP within a period of time (provided that there is data to deliver), divided by the duration of the 
	 * period. The traffic is conformant with the guaranteed bitrate as long as it follows a token bucket 
	 * algorithm where token rate equals Guaranteed bitrate and bucket size equals k*Maximum SDU size. 
	 * For release 1999, k=1.
	 *
	 * @param aGuaBitrate Value to which to set the maximum SDU size.
	 */
	IMPORT_C TInt SetGuaranteedBitrateUplink(TUint aGuaBitrate);

	/**
	 * Sets the guaranteed bitrate for downlink direction. Guaranteed number of bits delivered by UMTS at 
	 * a SAP within a period of time (provided that there is data to deliver), divided by the duration of the 
	 * period. The traffic is conformant with the guaranteed bitrate as long as it follows a token bucket 
	 * algorithm where token rate equals Guaranteed bitrate and bucket size equals k*Maximum SDU size. 
	 * For release 1999, k=1.
	 *
	 * @param aGuaBitrate Value to which to set the maximum SDU size.
	 */
	IMPORT_C TInt SetGuaranteedBitrateDownlink(TUint aGuaBitrate);

protected:
	TUmtsTrafficClass				iTrafficClass;			// Traffic class
	TUmtsDeliveryOrder				iDeliveryOrder;			// Delivery order
	TUmtsErroneousSDUDelivery		iDeliveryOfErroneusSdu;	// Delivery of erroneous SDUs
	TUmtsBitErrorRatio				iResidualBer;			// Residual BER
	TUmtsSDUErrorRatio				iErrorRatio;			// SDU error ratio
	TUmtsTrafficHandlingPriority	iPriority;				// Traffic handling priority
	TInt							iTransferDelay;			// Transfer delay
	TInt							iMaxSduSize;			// Maximum SDU size
	TInt							iMaxBitrateUplink;		// Maximum bit rate for uplink
	TInt							iMaxBitrateDownlink;	// Maximum bit rate for downlink
	TInt							iGuaBitrateUplink;		// Guaranteed bit rate for uplink
	TInt							iGuaBitrateDownlink;	// Guaranteed bit rate for downlink
};

/**
 * This class contains the negotiated QoS parameters that the network has granted.
 *
 * @internalTechnology
 * @prototype
 */
class TNegotiatedUmtsQoSParameters : public TUmtsQoSParameters
{
	friend class TUmtsQoSPolicy;
	friend class CUmtsQoSPolicy;
public:

	IMPORT_C TNegotiatedUmtsQoSParameters();

    /**
    * @return Header compression used in the PDP context. A bitmask indicationg 
	* if header compression (KPdpHeaderCompression)	or data compression (KPdpDataCompression) is used.
	*/
	IMPORT_C TUint HeaderCompression() const;

    /**
    * @return Error code == KErrNone if QoS request was succesful. If error code != KErrNone,
	* the negotiated QoS parameters have no meaning.
	*/
	IMPORT_C TInt ErrorCode() const;

protected:
	TUint iCompression;			// Header compression
	TInt iErrorCode;			// Error code
};



/**
 * This is a Umts policy extension, that can be added to a CQoSParameters object.
 *
 * @internalTechnology
 * @prototype
 */
class CUmtsQoSPolicy : public CExtensionBase
{
public:
	/**
	 * 2 phase constructor. Creates a CUmtsQoSPolicy object.
	 * @return A pointer to CUmtsQoSPolicy object.
	 * @exception Leaves if no memory is available.
	*/
	IMPORT_C static CUmtsQoSPolicy* NewL();

	/**
	 * destructor.
	*/
	IMPORT_C ~CUmtsQoSPolicy();

	/**
	 * Returns Umts policy extension in a descriptor. This is used by QoS API library.
	 * @return Descriptor that contains umts policy.
	*/
	IMPORT_C TDesC8& Data();

	/**
	 * Parses a Umts policy extension given in a descriptor. This is used by QoS API library.
	 * @return KErrNone if aData contains a valid Umts policy extension, otherwise KErrGeneral.
	*/
	IMPORT_C TInt ParseMessage(const TDesC8& aData);

	/**
	 * Creates a CUmtsQoSPolicy object. This is used by QoS API library.
	 * @return A pointer to CUmtsQoSPolicy object.
	 * @exception Leaves if no memory is available.
	*/
	IMPORT_C CExtensionBase* CreateL();

	/**
	 * Copies the parameters from aExtension object to this object. aExtension must be a CUmtsQoSPolicy 
	 * object. If some other extension is given as a parameter, KErrArgument is returned.
	 * @param aExtension A CUmtsQoSPolicy object that is copied into this object.
	 * @return KErrNone, aExtension is a CUmtsQoSPolicy object, otherwise KErrArgument.
	 */
	IMPORT_C TInt Copy(const CExtensionBase& aExtension);

	/**
	 * Sets the requested Umts QoS parameter set.
	 * @param aRequested Contains the requested Umts QoS parameters.
	 */
	IMPORT_C void SetQoSRequested(const TUmtsQoSParameters& aRequested);

	/**
	 * Sets the minimum Umts QoS parameter set.
	 * @param aMinimum Contains the minimum Umts QoS parameters.
	 */
	IMPORT_C void SetQoSMinimum(const TUmtsQoSParameters& aMinimum);

	/**
	 * Gets the requested Umts QoS parameter set.
	 * @param aRequested Requested QoS parameter set will be copied to aRequested.
	 */
	IMPORT_C void GetQoSRequested(TUmtsQoSParameters& aRequested) const;

	/**
	 * Gets the minimum Umts QoS parameter set.
	 * @param aMinimum Minimum QoS parameter set will be copied to aMinimum.
	 */
	IMPORT_C void GetQoSMinimum(TUmtsQoSParameters& aMinimum) const;

	/**
	 * Gets the negotiated Umts QoS parameter set.
	 * @param aNegotiated Negotiated QoS parameter set will be copied to aNegotiated.
	 */
	IMPORT_C void GetQoSNegotiated(TNegotiatedUmtsQoSParameters& aNegotiated) const;

	/**
	 * Sets the requested header compression value.
	 * @param aCompression Contains the requested header compression value: a bitmask indicationg 
	 * if header compression (KPdpHeaderCompression) or data compression (KPdpDataCompression) is used.
	 * @return KErrNone, if aCompression contains supported header compression value, 
	 * KErrNotSupported in other case.
	 */
	IMPORT_C TInt SetHeaderCompression(TUint aCompression);

	/**
	 * Gets the current requested header compression value: a bitmask indicationg 
	 * if header compression (KPdpHeaderCompression) or data compression (KPdpDataCompression) is used.
	 * @return Current requested header compression value.
	 */
	IMPORT_C TUint HeaderCompression() const;

protected:
	CUmtsQoSPolicy();
	void ConstructL();

private:
	TUmtsQoSParameters iMinimum;				// Minimum QoS parameter set
	TUmtsQoSParameters iRequested;				// Requested QoS parameter set
	TNegotiatedUmtsQoSParameters iNegotiated;	// Negotiated QoS parameters
	TUint iCompression;							// Header compression
	TPtr8 iBufPtr;
};

/** 
This class contain the 3GPP R5 Networking QoS support paramters
with the old R99 umts parameters (supported by TUmtsQoSParameters class)

@internalTechnology
@prototype
*/
class TUmtsR5QoSParameters : public TUmtsQoSParameters
{
	friend class CUmtsR5QoSPolicy;
public:

	/**
	Constructor
	*/
	IMPORT_C TUmtsR5QoSParameters();
	
	/**
	Gets the current Signalling Indicator flag 
	
	@return current Signalling Indicator flag
	*/
	IMPORT_C TBool SignallingIndicator() const;
	
	/**
	Gets the current Source Statistics Descriptor value
	
	@return current Source Statistics Descriptor value
	*/
	IMPORT_C TUmtsSourceStatisticsDescriptor SourceStatisticsDescriptor() const;
	
	/**	
    Sets the Signalling Indicator flag
	
	@param aSignallingIndicator Value to which to set the Signalling Indicator flag
	*/
	IMPORT_C TInt SetSignallingIndicator(TBool aSignallingIndicator);
	
	/**	
    Sets the Source Statistics Descriptor
	
	@param aSrcStatisticsDesc Value to which to set the Source Statistics Descriptor
	*/
	IMPORT_C TInt SetSourceStatisticsDescriptor(TUmtsSourceStatisticsDescriptor aSrcStatisticsDesc);

protected:

	TBool iSignallingIndicator;							// Signalling Indicator flag
	TUmtsSourceStatisticsDescriptor iSrcStatisticsDesc; // Source Statistics Descriptor
};

/**
This class contains the negotiated UMTS R5 QoS parameters that the network has granted.

@internalTechnology
@prototype
*/
class TNegotiatedUmtsR5QoSParameters : public TUmtsR5QoSParameters
{
	friend class CUmtsR5QoSPolicy;
public:

	IMPORT_C TNegotiatedUmtsR5QoSParameters();
	
	/**
    @return Header compression used in the PDP context. A bitmask indicationg 
	if header compression (KPdpHeaderCompression)	or data compression (KPdpDataCompression) is used.
	*/
	IMPORT_C TUint HeaderCompression() const;
	
	/**
    @return Error code == KErrNone if QoS request was succesful. If error code != KErrNone,
	the negotiated QoS parameters have no meaning.
	*/
	IMPORT_C TInt ErrorCode() const;

protected:
	TUint iCompression;			// Header compression
	TInt iErrorCode;			// Error code
};


/** 
This is the UMTS R5 policy extension. The UMTS R5 policy can be extended by adding the 
CSubConIMSExtensionParamSet object at the client side

@internalTechnology
@prototype
*/
class CUmtsR5QoSPolicy : public CExtensionBase
{
public:
	
	/** Two phase constructor. Creates a CUmtsR5QoSPolicy object.
	
	@return A pointer to CUmtsR5QoSPolicy object.
	@exception Leaves if no memory is available.
	*/
	IMPORT_C static CUmtsR5QoSPolicy* NewL();
	
	/** destructor.
	*/
	IMPORT_C ~CUmtsR5QoSPolicy();
	
	/** Returns Umts policy r5 extension in a descriptor. This is used by ipscpr.
	
	@return Descriptor that contains umts r5 policy.
	*/
	IMPORT_C TDesC8& Data();
	
	/** Parses a Umts r5 policy extension given in a descriptor. This is used by ipscpr.
	
	@return KErrNone if aData contains a valid Umts r5 policy extension, otherwise KErrGeneral.
	*/
	IMPORT_C TInt ParseMessage(const TDesC8& aData);
	
	/** Creates a CUmtsR5QoSPolicy object. This is used by ipscpr.
	
	@return A pointer to CUmtsR5QoSPolicy object.
	@exception Leaves if no memory is available.
	*/
	IMPORT_C CExtensionBase* CreateL();
	
	/** Copies the parameters from aExtension object to this object. aExtension must be a CUmtsR5QoSPolicy 
	object. If some other extension is given as a parameter, KErrArgument is returned.
	
	@param aExtension A CUmtsR5QoSPolicy object that is copied into this object.
	@return KErrNone, aExtension is a CUmtsR5QoSPolicy object, otherwise KErrArgument.
	*/
	IMPORT_C TInt Copy(const CExtensionBase& aExtension);
	
	/** Sets the requested Umts r5 QoS parameter set.
	
	@param aRequested Contains the requested Umts r5 QoS parameters.
	*/
	IMPORT_C void SetQoSRequested(const TUmtsR5QoSParameters& aRequested);
	
	/** Sets the minimum Umts r5 QoS parameter set.
	
	@param aMinimum Contains the minimum Umts r5 QoS parameters.
	*/
	IMPORT_C void SetQoSMinimum(const TUmtsR5QoSParameters& aMinimum);
	
	/** Gets the requested Umts r5 QoS parameter set.
	
	@param aRequested Requested r5 QoS parameter set will be copied to aRequested.
	*/
	IMPORT_C void GetQoSRequested(TUmtsR5QoSParameters& aRequested) const;
	
	/** Gets the minimum Umts r5 QoS parameter set.
	
	@param aMinimum Minimum r5 QoS parameter set will be copied to aMinimum.
	*/
	IMPORT_C void GetQoSMinimum(TUmtsR5QoSParameters& aMinimum) const;
	
	/** Gets the negotiated Umts r5 QoS parameter set.
	
	@param aNegotiated Negotiated r5 QoS parameter set will be copied to aNegotiated.
	*/
	IMPORT_C void GetQoSNegotiated(TNegotiatedUmtsR5QoSParameters& aNegotiated) const;
	
	/** Sets the requested header compression value.
    
    @param aCompression Contains the requested header compression value: a bitmask indicationg 
	if header compression (KPdpHeaderCompression) or data compression (KPdpDataCompression) is used.
	@return KErrNone, if aCompression contains supported header compression value, 
	KErrNotSupported in other case.
	*/
	IMPORT_C TInt SetHeaderCompression(TUint aCompression);
	
    /** Gets the current requested header compression value: a bitmask indicationg 
	if header compression (KPdpHeaderCompression) or data compression (KPdpDataCompression) is used.
	
	@return Current requested header compression value.
	*/
	IMPORT_C TUint HeaderCompression() const;

protected:
	CUmtsR5QoSPolicy();
	void ConstructL();

private:
	TUmtsR5QoSParameters iMinimum;				// Minimum QoS parameter set
	TUmtsR5QoSParameters iRequested;			// Requested QoS parameter set
	TNegotiatedUmtsR5QoSParameters iNegotiated;	// Negotiated QoS parameters
	TUint iCompression;							// Header compression
	TPtr8 iBufPtr;
};

#endif // __UMTSEXTN_H__
