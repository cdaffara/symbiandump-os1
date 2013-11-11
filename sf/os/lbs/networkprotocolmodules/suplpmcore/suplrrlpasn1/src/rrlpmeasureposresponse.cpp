// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
 
*/

#include "RRLP-Messages.h"
#include "rrlpmeasureposresponse.h"
#include "supldevloggermacros.h" 
#include "suplrrlpasn1common.h"

#include <lbssatellite.h>
#include <lbs/lbsgpsmeasurement.h>
#include <lbs/lbsextendedsatellite.h>

/*
Modulus used in conversion of GPS TOW in GPS Measurement Information Element.
*/
const TInt KLbsTowMod = 14400000;


/**
CRrlpMeasurePositionResponse

Class for building an outgoing RRLP Measure Position Response Message
Translates content of passed LBS defined data structures to data encoded as per
the RRLP specification 3GPP 44.031 v5.12 and related documents. 
Provides a mechanism to encode the data to a target buffer as an ASN1 PER encoded 
data stream


The RRLP Measure Position Repsonse message consists of the following components:

Multiple Sets (optional):
  allows the mobile terminal to specify the number of sets of location or
  measurement datum included in this message. This is not used by Symbian LBS
  and is left absent, indicating a single measurement set is included.

Reference BTS Identity Element (optional):
  Identifies the Base Transceiver Stations used in the calculation of position.
  This element is conditional to the number of reference BTSs. It is mandatory, 
  if there is more than one reference BTS, and optional otherwise. If this 
  element is not included, the Reference BTS, used in other elements, is the 
  current serving BTS of MS. If this element is included, the BTSs defined here
  are used as Reference BTSs in all other elements.
  Currently not included in this implementation.
  
E-OTD Measurement Information (optional):
  Provides OTD measurements of signals sent from the reference and neighbor base
  stations. Mandatory if E-OTD is the agreed positioning method, omitted otherwise.
  Symbian LBS currently does not support E-OTD positioning, so this component is
  omitted.

Location Information (optional):
  The purpose of Location Information element is to provide the location 
  estimate from the MS to the network, if the MS is capable of determining its 
  own position. Optionally, the element may contain the velocity parameters 
  computed by the MS.
  
  This element is populated via calls to the SetLocationInformation() API

GPS Measurement Information (optional):
  The purpose of the GPS Measurement Information element is to provide GPS 
  measurement information from the MS to the SMLC. This information includes 
  the measurements of code phase and Doppler, which enables the network-based
  GPS method where position is computed in the SMLC.
  
  This element is populated via a call to the SetMeasurementInformation() API

Location Information Error (optional):
  The purpose of Location Information Error element is to provide the indication
  of error and the reason for it, when the MS can not perform the required 
  location or the network can not determine the position estimate. The element 
  may also indicate what further assistance data may be needed by the target MS 
  to produce a successful location estimate or location measurements.
  
  This element is populated via a call to the SetLocationError() APIs

GPS Time Assistance Measurements (optional):
  This IE contains measurements that are used to define an accurate relation 
  between GSM and GPS time or to provide additional GPS TOW information for MS
  Assisted A-GPS.

Extended Reference (optional):
  This IE shall be included in any Measure Position Response if and only if an 
  Extended Reference IE was received in the corresponding previous Measure 
  Position Request message.
  
  This element is populated via a call to the SetReference() API.
  

Uplink RRLP Pseudo Segmentation Indication (optional):
  This element is included by the MS when up-link RRLP pseudo-segmentation is used. 
  In the first segment, 'first of many' is indicated and in the second 'second of 
  many' is indicated. 
  This implementation does not use up-link pseudo-segmentation hence this component
  is not included.

*/

/**
Static factory constructor
*/
EXPORT_C CRrlpMeasurePositionResponse* CRrlpMeasurePositionResponse::NewL()
	{
	SUPLLOG(ELogP1, "CRrlpMeasurePositionResponse::NewL() Begin\n");
	CRrlpMeasurePositionResponse* self = CRrlpMeasurePositionResponse::NewLC();
	SUPLLOG(ELogP1, "CRrlpMeasurePositionResponse::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}

/**
Static factory constructor
*/
EXPORT_C CRrlpMeasurePositionResponse* CRrlpMeasurePositionResponse::NewLC()
	{
	SUPLLOG(ELogP1, "CRrlpMeasurePositionResponse::NewLC() Begin\n");
	CRrlpMeasurePositionResponse* self = new (ELeave) CRrlpMeasurePositionResponse();
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CRrlpMeasurePositionResponse::NewLC() End\n");
	return self;
	}

/** 
Default constructor 
*/
CRrlpMeasurePositionResponse::CRrlpMeasurePositionResponse()
 : CRrlpMessageBase(ERrlpMeasurePositionResp, ETrue)
	{
	}

/** 
Second stage constructor 
*/
void CRrlpMeasurePositionResponse::ConstructL()
	{
	// outgoing message - call base class ConstructL to create data structures.
	CRrlpMessageBase::ConstructL();
		
	// local reference to context object
	OSCTXT* pctxt = iControl->getCtxtPtr();

	iData->component.t = T_RRLP_Component_msrPositionRsp;
	iData->component.u.msrPositionRsp = (ASN1T_MsrPosition_Rsp*)rtxMemAllocZ(pctxt, sizeof(ASN1T_MsrPosition_Rsp));
   	LeaveIfAllocErrorL();
	}

/** 
Destructor 
*/
CRrlpMeasurePositionResponse::~CRrlpMeasurePositionResponse()
	{
	SUPLLOG(ELogP1, "CRrlpMeasurePositionResponse::~CRrlpMeasurePositionResponse() Begin\n");
	SUPLLOG(ELogP1, "CRrlpMeasurePositionResponse::~CRrlpMeasurePositionResponse() End\n");
	}
	

/**	
SetLocationInformation()

Sets the content of the location information component of the RRLP Measure 
Position Response Message.

RRLP Location Information component consists of the following:

Reference Frame (mandatory):
  This field specifies the reference BTS Reference Frame number during which
  the location estimate was measured. This information is not available at the
  Protocol Module, hence this is set to a value which is ignored by the SMLC.

GPS TOW (optional):
  This field specifies the GPS TOW for which the location estimate is valid, 
  rounded down to the nearest millisecond unit. This field is optional but 
  shall be included if GPS Time Assistance Measurements are included. 
  Currently omitted in this implementation.
 
Fix Type (mandatory)
  This field contains an indication as to the type of measurements performed 
  by the MS: 2D or 3D. 
  Set to 3D if altitude information is available, 2D otherwise.

Position Estimate (mandatory)
  This field contains the calculated position estimate in the format defined 
  in 3GPP TS 23.032. The allowed shapes are ellipsoid Point, ellipsoid point 
  with uncertainty circle, ellipsoid point with uncertainty ellipse, ellipsoid 
  point with altitude and uncertainty ellipsoid.


@param  aPosInfo reference to the reported position data from LBS
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CRrlpMeasurePositionResponse::SetLocationInformation(TPositionInfoBase& aPosInfo)
	{
	SUPLLOG(ELogP1, "CRrlpMeasurePositionResponse::SetLocationInformation() Begin\n");
	__ASSERT_DEBUG(iData->component.u.msrPositionRsp != NULL, User::Invariant());
	
	ASN1T_MsrPosition_Rsp* msgBody = iData->component.u.msrPositionRsp;
	
	// handle Position Info class type
	if (aPosInfo.PositionClassType() & EPositionInfoClass)
		{
		TPositionInfo& posInfo = reinterpret_cast <TPositionInfo&>(aPosInfo);

		// the position
		TPosition position;
		posInfo.GetPosition(position);
		
		// indicate that location information is present
		msgBody->m.locationInfoPresent = 1;

		// BTS Reference Frame (mandatory) 0-65535 - however is ignored if 
		// is in the range 42432..65535
		// In this case GPS TOW should be provided if available. Which it is not.
		msgBody->locationInfo.refFrame = 65535;
		
		// GPS Time Of Week (optional) should be included if GPS Time Assistance 
		// Measurements  are included in the Measure Position Response, or if the
		// BTS Reference Frame is in the "ignore" range. Not available.
		msgBody->locationInfo.m.gpsTOWPresent = 0;
		
		// Position Estimate 
		// ASN1T_Ext_GeographicalInformation is a 20 byte array, populated as 
		// described in 3GPP 23.032. 

		// Latitude/Longitude description is common to all supported position types
		// Latitude is 23 bits of value and 1 bit describing direction (0 = north, 1 = south)
		TReal64 latitudeDegrees = position.Latitude();
		TInt latitudeEncoded;
		TBool isSouth = EFalse;
		if (latitudeDegrees < 0)
			{
			latitudeDegrees *= -1;
			isSouth = ETrue;
			}
		 
		latitudeEncoded = latitudeDegrees * KLbsLatitudeConst;	// * 2^23 / 90
		if (isSouth)
			{
			// set the 24th bit
			latitudeEncoded |= KBit23;
			}

		// Longitude is 24 bit 2's complimentary binary
		TReal64 longitudeDegrees = position.Longitude();
		TInt longitudeEncoded;
		TBool isNegative = EFalse;
		if (longitudeDegrees < 0)
			{
			longitudeDegrees *= -1;
			isNegative = ETrue;
			}
		
		longitudeEncoded = longitudeDegrees *  16777216 / 360; // * 2^24 / 360
		if (isNegative)
			{
			// invert and add 1 for 2's complimentary binary
			longitudeEncoded = ~longitudeEncoded;
			longitudeEncoded += 1;
			}

		// insert into the octet array
		const TInt bottom8bits = 255;
		msgBody->locationInfo.posEstimate.data[1] = (latitudeEncoded >> 16) & bottom8bits;
		msgBody->locationInfo.posEstimate.data[2] = (latitudeEncoded >> 8) & bottom8bits;
		msgBody->locationInfo.posEstimate.data[3] = (latitudeEncoded) & bottom8bits;

		msgBody->locationInfo.posEstimate.data[4] = (longitudeEncoded >> 16) & bottom8bits;
		msgBody->locationInfo.posEstimate.data[5] = (longitudeEncoded >> 8) & bottom8bits;
		msgBody->locationInfo.posEstimate.data[6] = (longitudeEncoded) & bottom8bits;
		
		// encode the horizontal uncertainty, if present;
		TInt horizontalUncertaintyEncoded = 0;
		if (position.HorizontalAccuracy() > 0)
			{
			horizontalUncertaintyEncoded = MetersToHorizontalUncertainty(position.HorizontalAccuracy());
			}
				
		// encode altitude if present in the position estimate:
		TInt altitudeEncoded = 0;
		TInt altitudeUncertaintyEncoded = 0;
		if(!Math::IsNaN(position.Altitude()))
			{
			TBool isUp = ETrue;
			TReal32 altitude = position.Altitude();
			if (altitude < 0)
				{
				isUp = EFalse;
				altitude *= -1;
				}
			if (altitude > 32767) // = (2^15)-1  maximum size is 15 bits.
				{
				altitude = 32767;
				}
			
			altitudeEncoded = altitude;
			// 16th bit indicates direction of altitude, 1 indicates below surface
			if (!isUp)
				{
				altitudeEncoded |= KBit15;
				}
			
			// is vertical accuracy defined?
			if (position.VerticalAccuracy() != 0)
				{
				altitudeUncertaintyEncoded = MetersToVerticalUncertainty(position.VerticalAccuracy());
				}
			} // end of altitude conversion
		
		
		// clear the first byte and set remaining content according to data available
		if (altitudeEncoded)
			{
			// EEllipsoidPointWithAltitudeAndUncertaintyEllipsoid
			msgBody->locationInfo.posEstimate.data[0]  = EGeoInfoEllipsoidPointWithAltitudeAndUncertaintyEllipsoid<<4;
			msgBody->locationInfo.posEstimate.data[7]  = altitudeEncoded>>8;
			msgBody->locationInfo.posEstimate.data[8]  = altitudeEncoded;
			msgBody->locationInfo.posEstimate.data[9]  = horizontalUncertaintyEncoded; // semi-major axis
			msgBody->locationInfo.posEstimate.data[10] = horizontalUncertaintyEncoded; // semi-major axis
			msgBody->locationInfo.posEstimate.data[11] = 0; // orientation
			msgBody->locationInfo.posEstimate.data[12] = altitudeUncertaintyEncoded;
			msgBody->locationInfo.posEstimate.data[13] = 0; // confidence, 0 == "no information"
			msgBody->locationInfo.posEstimate.numocts = 14;
			
			msgBody->locationInfo.fixType = FixType::threeDFix;
			}
		else if (horizontalUncertaintyEncoded)
			{
			// EEllipsoidPointWithUncertaintyCircle
			msgBody->locationInfo.posEstimate.data[0] = EGeoInfoEllipsoidPointWithUncertaintyCircle<<4;
			msgBody->locationInfo.posEstimate.data[7] = horizontalUncertaintyEncoded;
			msgBody->locationInfo.posEstimate.numocts = 8;

			msgBody->locationInfo.fixType = FixType::twoDFix;
			}
		else
			{
			// EEllipsoidPoint
			msgBody->locationInfo.posEstimate.data[0] = EGeoInfoEllipsoidPoint<<4;
			msgBody->locationInfo.posEstimate.numocts = 7;

			msgBody->locationInfo.fixType = FixType::twoDFix;
			}
		}
	else
		{
		SUPLLOG(ELogP1, "CRrlpMeasurePositionResponse::SetLocationInformation() End (not a EPositionInfoClass)\n");
		return KErrNotSupported;
		}
	
	// additional handling for extended satellite info 
	if (aPosInfo.PositionClassType() & EPositionExtendedSatelliteInfoClass)
	    {
	    TPositionExtendedSatelliteInfo& extSatInfo = reinterpret_cast <TPositionExtendedSatelliteInfo&>(aPosInfo);

	    // GPS Timing Measurements
        TGpsTimingMeasurementData timingData;
        if  (KErrNone == extSatInfo.GetGpsTimingData(timingData))
            {
            SetGpsTimingData(timingData);
            }
	    }


	SUPLLOG(ELogP1, "CRrlpMeasurePositionResponse::SetLocationInformation() End\n");
	return KErrNone;
	}

/**	
MetersToHorizontalUncertainty()

Converts from meters to uncertainty code, as defined in 3GPP TS 23.032:
uncertainty r = C( (1+x)^k - 1 )

r = uncertainty in meters
C = 10;
x = 0.1
k = uncertainty code

hence k = ln(h/C + 1) / ln(1+x), limited to 7 bits
*/
TInt CRrlpMeasurePositionResponse::MetersToHorizontalUncertainty(const TReal32& aDistance)
	{
	SUPLLOG(ELogP1, "CRrlpMeasurePositionResponse::MetersToHorizontalUncertainty() Begin\n");
	TReal uncert;
	Math::Ln(uncert,  (aDistance/10) + 1 );
	uncert /= KLbsLogOnePointOne;
	if (uncert>KLbsMaxUncert)
		{
		uncert = KLbsMaxUncert;
		}

	SUPLLOG(ELogP1, "CRrlpMeasurePositionResponse::MetersToHorizontalUncertainty() End\n");
	return (TInt)uncert;	
	}

/**	
MetersToVerticalUncertainty()

Converts from meters to uncertainty code, as defined in 3GPP TS 23.032:
uncertainty h = C( (1+x)^k - 1 )

h = uncertainty in meters
C = 45;
x = 0.025
k = uncertainty code

hence k = ln(h/C + 1) / ln(1+x), limited to 7 bits
*/
TInt CRrlpMeasurePositionResponse::MetersToVerticalUncertainty(const TReal32& aDistance)
	{
	SUPLLOG(ELogP1, "CRrlpMeasurePositionResponse::MetersToVerticalUncertainty() Begin\n");
	TReal uncert;
	Math::Ln(uncert,  (aDistance/45) + 1 );
	uncert /= KLbsLogOnePointZeroTwoFive;
	if (uncert>KLbsMaxUncert)
		{
		uncert = KLbsMaxUncert;
		}

	SUPLLOG(ELogP1, "CRrlpMeasurePositionResponse::MetersToVerticalUncertainty() End\n");
	return (TInt)uncert;
	}

/** 
Sets measurement information

@param  aPosInfo reference to the reported position data from LBS
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CRrlpMeasurePositionResponse::SetMeasurementInformation(const TPositionInfoBase& aPosInfo)
	{
	SUPLLOG(ELogP1, "CRrlpMeasurePositionResponse::SetMeasurementInformation() Begin\n");
	__ASSERT_DEBUG(iData->component.u.msrPositionRsp != NULL, User::Invariant());
	
	ASN1T_MsrPosition_Rsp* msgBody = iData->component.u.msrPositionRsp;
	
	// handle Position Info class type
	if ((aPosInfo.PositionClassType() & EPositionGpsMeasurementInfoClass) != 0)
		{
		const TPositionGpsMeasurementInfo& measurementInfo = reinterpret_cast <const TPositionGpsMeasurementInfo&>(aPosInfo);
		msgBody->m.gps_MeasureInfoPresent = 1;
		
		// only support the sending of a single set of GPS measurements.
		ASN1C_SeqOfGPS_MsrSetElement msrSetArray(*iControl, msgBody->gps_MeasureInfo.gpsMsrSetList);
		msrSetArray.init();
		ASN1T_GPS_MsrSetElement* gpsMsrSetElement = msrSetArray.NewElement();
		if (gpsMsrSetElement == NULL)
			{
			return KErrNoMemory;
			}
		
		// LBS API does not provide optional Reference Frame information.
		gpsMsrSetElement->m.refFramePresent = 0;
		
		// GPS Time Of Week - RRLP parameter is described as the least significant 
		// 24 bits of the GPS TOW. Further study indicates it is in fact the result
		// of the TOW within a 4 hour window period (144000000ms). Therefore the
		// value is calculated as GPS_TOW_MS mod 144000000.
		TReal tow = 0;
		Math::Mod(tow, measurementInfo.GpsTimeOfWeek(), KLbsTowMod);
		gpsMsrSetElement->gpsTOW = tow;
		
		// build the array of measurements
		ASN1C_SeqOfGPS_MsrElement msrElementArray(*iControl, gpsMsrSetElement->gps_msrList);
		msrElementArray.init();

		for (TInt i = 0; i < measurementInfo.NumMeasurements(); ++i)
			{
			TPositionGpsMeasurementData data;
			if (measurementInfo.GetMeasurementData(i, data) == KErrNone)
				{
				ASN1T_GPS_MsrElement* gpsMsrElement = msrElementArray.NewElement();
				if (gpsMsrElement == NULL)
					{
					return KErrNoMemory;
					}

				gpsMsrElement->satelliteID     = data.SatelliteId();
				gpsMsrElement->cNo             = data.CarrierNoiseRatio();
				gpsMsrElement->doppler         = data.Doppler();
				gpsMsrElement->wholeChips      = data.WholeGpsChips();
				gpsMsrElement->fracChips       = data.FractionalGpsChips();
				gpsMsrElement->mpathIndic      = data.MultiPathIndicator();
				gpsMsrElement->pseuRangeRMSErr = data.PseudoRangeRmsError();
				
				msrElementArray.Append(gpsMsrElement);
				}
			}
		msrSetArray.Append(gpsMsrSetElement);
		
		// append any GPS Timing Measurement Data
		TGpsTimingMeasurementData timingData;
		if  (KErrNone == measurementInfo.GetGpsTimingData(timingData))
		    {
		    SetGpsTimingData(timingData);
		    }
		}
	
	SUPLLOG(ELogP1, "CRrlpMeasurePositionResponse::SetMeasurementInformation() End\n");
	return KErrNone;
	}


/** 
SetLocationError()

Sets Location Error element

@param  aLocError - location error
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CRrlpMeasurePositionResponse::SetLocationError(TRrlpLocError aLocError)
	{
	SUPLLOG(ELogP1, "CRrlpMeasurePositionResponse::SetLocationError() Begin\n");
	SUPLLOG2(ELogP1, "    - TRrlpLocError aLocError = %d", aLocError);
	
	__ASSERT_DEBUG(iData->component.u.msrPositionRsp != NULL, User::Invariant());
	ASN1T_MsrPosition_Rsp* msgBody = iData->component.u.msrPositionRsp;
	msgBody->m.locationErrorPresent = 1;
	msgBody->locationError.locErrorReason = aLocError;
	
	SUPLLOG(ELogP1, "CRrlpMeasurePositionResponse::SetLocationError() End\n");
	return KErrNone;
	}

	
/** 
SetLocationError()

Sets Location Error element with request for additional assistance data 

@see TRrlpLocError
@param  aLocError - location error
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CRrlpMeasurePositionResponse::SetLocationError(TRrlpLocError aLocError, const TLbsAsistanceDataGroup& aDataReqMask)
	{
	SUPLLOG(ELogP1, "CRrlpMeasurePositionResponse::SetLocationError() Begin\n");
	SUPLLOG2(ELogP1, "    - TRrlpLocError aLocError = %d", aLocError);
	SUPLLOG2(ELogP1, "    - TLbsAsistanceDataGroup aDataReqMask  = 0x%08X\n", aDataReqMask);
	
	__ASSERT_DEBUG(iData->component.u.msrPositionRsp != NULL, User::Invariant());
	ASN1T_MsrPosition_Rsp* msgBody = iData->component.u.msrPositionRsp;
	msgBody->m.locationErrorPresent = 1;
	msgBody->locationError.locErrorReason = aLocError;

	// Additional Assistance Data is encoded according to 3GPP TS 49.031, excluding
	// the IEI and length octets.
	if (aDataReqMask != EAssistanceDataNone)
		{
		msgBody->locationError.m.additionalAssistanceDataPresent = 1;
		//ASN1T_AdditionalAssistanceData
		msgBody->locationError.additionalAssistanceData.m.extensionContainerPresent = 0;
		msgBody->locationError.additionalAssistanceData.m.gpsAssistanceDataPresent = 1;

		// short name reference
		ASN1T_GPSAssistanceData& requestedData = msgBody->locationError.additionalAssistanceData.gpsAssistanceData;

		// ensure the data masks are clear and set number of octets to be sent
		requestedData.data[0] = 0;
		requestedData.data[1] = 0;
		requestedData.numocts = 2;
				
		// set requested assistance types
		// we do not request Ephemeris Extension or Ephemeris Extension Check or inform
		// the SMLC of satellite specific information we already have.
		// data is described by bit location, as per the following:

		//         | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 |
		// octet 1 | H | G | F | E | D | C | B | A | 
		// octet 2 | P | O | N | M | L | K | J | I |
		if (EAssistanceDataAquisitionAssistance & aDataReqMask)
			{
			// bit H
			requestedData.data[0] |= 1<<7;
			}
		if (EAssistanceDataBadSatList & aDataReqMask)
			{
			// bit I
			requestedData.data[1] |= 1;
			}
		if (EAssistanceDataNavigationModel & aDataReqMask)
			{
			// bit D
			requestedData.data[0] |= 1<<3;
			}
		if (EAssistanceDataReferenceTime & aDataReqMask)
			{
			// bit G
			requestedData.data[0] |= 1<<6;
			}
		if (EAssistanceDataIonosphericModel & aDataReqMask)
			{
			// bit C
			requestedData.data[0] |= 1<<2;
			}
		if (EAssistanceDataDgpsCorrections & aDataReqMask)
			{
			// bit E
			requestedData.data[0] |= 1<<4;
			}
		if (EAssistanceDataReferenceLocation & aDataReqMask)
			{
		 	// bit F
		 	requestedData.data[0] |= 1<<5;
			}
		if (EAssistanceDataAlmanac & aDataReqMask)
			{
			// bit A
			requestedData.data[0] |= 1;
			}
		if (EAssistanceDataPositioningGpsUtcModel & aDataReqMask)
			{
			// bit B
			requestedData.data[0] |= 1<<1;
			}
		}
	
	SUPLLOG(ELogP1, "CRrlpMeasurePositionResponse::SetLocationError() End\n");
	return KErrNone;
	}

/** 
SetGpsTimingData()

Sets GPS Fine Timing Data

@see TGpsTimingMeasurementData
@param  aTimingData - timing data
@return error indication, KErrNone otherwise
*/
void CRrlpMeasurePositionResponse::SetGpsTimingData(const TGpsTimingMeasurementData& aTimingData)
    {
    SUPLLOG(ELogP1, "CRrlpMeasurePositionResponse::SetGpsTimingData() Begin\n");
    
    // check that the timing structure contains GPS data
    if (TGpsTimingMeasurementData::EGpsTimingDataTypeGsm != aTimingData.DataType())
        {
        SUPLLOG(ELogP1, "CRrlpMeasurePositionResponse::SetGpsTimingData() End Unsupported Data Type\n");
        return;
        }
        
    __ASSERT_DEBUG(iData->component.u.msrPositionRsp != NULL, User::Invariant());
    ASN1T_MsrPosition_Rsp* msgBody = iData->component.u.msrPositionRsp;

    // mark r98 extension and timing measurements parameter present
    msgBody->m.rel_98_MsrPosition_Rsp_ExtensionPresent = 1;
    msgBody->rel_98_MsrPosition_Rsp_Extension.m.timeAssistanceMeasurementsPresent = 1;
    
    // reference to the data member to be populated
    ASN1T_GPSTimeAssistanceMeasurements& measurements = msgBody->rel_98_MsrPosition_Rsp_Extension.timeAssistanceMeasurements;

    // populate the data structure. 
    measurements.referenceFrameMSB = aTimingData.ReferenceFrameMsb();
    if (aTimingData.GpsTowSubms() >= 0)
        {
        measurements.m.gpsTowSubmsPresent = 1;
        measurements.gpsTowSubms = aTimingData.GpsTowSubms();
        }
    if (aTimingData.DeltaTow() >= 0)
        {
        measurements.m.deltaTowPresent = 1;
        measurements.deltaTow = aTimingData.DeltaTow();
        }
    if (aTimingData.GpsReferenceTimeUncertainty() >= 0)
        {
        measurements.m.gpsReferenceTimeUncertaintyPresent = 1;
        measurements.gpsReferenceTimeUncertainty = aTimingData.GpsReferenceTimeUncertainty();
        }

    SUPLLOG(ELogP1, "CRrlpMeasurePositionResponse::SetGpsTimingData() End\n");
    return;
    }


/**
SetExtendedReference()

Sets the extended reference parameters in the outgoing message, if they are set
in the passed container.

@param aRrlpRef on return, populated with the session reference details
@return KErrNotFound if no extended reference data is present, 
		KErrNone otherwise
*/	
TInt CRrlpMeasurePositionResponse::SetExtendedReference(const TRrlpReference& aRrlpRef)
	{
	__ASSERT_DEBUG(iData->component.u.msrPositionRsp != NULL, User::Invariant());
	
	// if present, populate the optional Rel-5 extended reference
	if (aRrlpRef.aRel5EntendedRefPresent)
		{
		// mark the optional component present
		iData->component.u.msrPositionRsp->m.rel_5_MsrPosition_Rsp_ExtensionPresent = 1;
		iData->component.u.msrPositionRsp->rel_5_MsrPosition_Rsp_Extension.m.extended_referencePresent = 1;
		ASN1T_Extended_reference* extendedRef = &iData->component.u.msrPositionRsp->rel_5_MsrPosition_Rsp_Extension.extended_reference;
		extendedRef->smlc_code = aRrlpRef.aRel5SmlcCode;
		extendedRef->transaction_ID = aRrlpRef.aRel5TransactionId;
		}
	else
		{
		return KErrNotFound;
		}
	
	return KErrNone;
	}

