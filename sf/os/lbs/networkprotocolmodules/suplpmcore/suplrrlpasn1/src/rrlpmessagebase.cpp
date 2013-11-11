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
#include "rrlpmessagebase.h"
#include "supldevloggermacros.h" 


/**
Const definition for the 3D location with uncertainty identifier.
This is the form of location description allowed in RRLP for the
GPS Reference Location element.
*/
const TUint K3DLocationWithUncertainty = 9;


/**
Maximun number of assistance data "elements" (be it almanac,
acquisition, etc) that can be passed on to LBS. If an RRLP
message contains more elements than the maximum, only the 
first 32 are considered.
*/
const TUint8 KAsn1RrlpMaxSatNumber = 32;

/**
Const definition for multiplier to convert 0.08 second resolution to ms..
The Reference Time field of the GPS Assistance Data element contains
a GPS Time Of Week (TOW) element, with resolution 0.08seconds.
The LBS Assistance Data object stores the TOW in ms.
*/
const TUint KGpsTowMultiplier = 80;

/**
Constructor
*/
CRrlpMessageBase::CRrlpMessageBase(TSuplPosPayloadType aType, TBool aIsOutgoingMsg)
 : CSuplPosPayload(aType, aIsOutgoingMsg)
	{
	}


/** 
Second stage constructor 

Outgoing messages: constructs the data encapsulation and control objects.
Incoming message: no action
*/
void CRrlpMessageBase::ConstructL()
	{
	if (iIsOutgoingMessage)
		{
		iData    = new (ELeave) ASN1T_PDU();
		ASN1Context* context = new (ELeave) ASN1Context;
		CleanupDeletePushL(context);
		iControl = new (ELeave) ASN1C_PDU(*context, *iData);
		// construction of iControl successful, pop context off the cleanup stack
		CleanupStack::Pop(context);
		}
	}


/**
Destructor
*/
CRrlpMessageBase::~CRrlpMessageBase()
	{
	SUPLLOG(ELogP1, "CRrlpMessageBase::~CRrlpMessageBase() Begin\n");
	delete iEncodeBuffer;
	delete iData;
	delete iControl;
	// release the STDLIB resources associated with this thread
	CloseSTDLIB();
	SUPLLOG(ELogP1, "CRrlpMessageBase::~CRrlpMessageBase() End\n");
	}

	
/** 
SetDecodedData()

Assign decoded ASN1 data, for received messages.
Takes ownership of passed objects.

@param aData    data structure containing decoded message parameters
@param aControl control structure associated with decoded data structure.
*/
void CRrlpMessageBase::SetDecodedData(ASN1T_PDU* aData, ASN1C_PDU* aControl)
	{
	SUPLLOG(ELogP1, "CRrlpMessageBase::SetDecodedData() Begin\n");
	__ASSERT_DEBUG(!iIsOutgoingMessage, User::Invariant());
	__ASSERT_DEBUG(iData == NULL, User::Invariant());
	__ASSERT_DEBUG(iControl == NULL, User::Invariant());
	iData = aData;
	iControl = aControl;
	SUPLLOG(ELogP1, "CRrlpMessageBase::SetDecodedData() End\n");
	}


/**
SetReference()

Sets the RRLP session reference 
Messages that contain the Rel-5 Extended Reference override the method
SetExtendedReference() to populate this content.

The RRLP Reference, including the Rel-5 extended reference include the
following parameters
	TInt  aRefNum;            // 0..7
	TBool aRel5EntendedRefPresent;
	TInt  aRel5SmlcCode;      // 0..63
	TInt  aRel5TransactionId; // 0..262143

@param  aRrlpRef local copy of the session reference details
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CRrlpMessageBase::SetReference(const TRrlpReference& aRrlpRef)
	{
	SUPLLOG(ELogP1, "CRrlpMessageBase::SetReference() Begin\n");
	__ASSERT_DEBUG(iIsOutgoingMessage, User::Invariant());
	__ASSERT_DEBUG(iData != NULL, User::Invariant());
	
	iData->referenceNumber = aRrlpRef.aRefNum;
	
	TInt err = KErrNone;
	err = SetExtendedReference(aRrlpRef);
	if (err == KErrNotFound)
		{
		SUPLLOG(ELogP1, "CRrlpMessageBase::SetReference() no extended reference present\n");
		}
	
	
	SUPLLOG(ELogP1, "CRrlpMessageBase::SetReference() End\n");
	return KErrNone;
	}

	
/**
SetExtendedReference()

Overridden by derived classes representing messages that include rel-5
extended reference.

@param  aRrlpRef local copy of the session reference details
@return error indication, KErrNone otherwise
*/
TInt CRrlpMessageBase::SetExtendedReference(const TRrlpReference& /*aRrlpRef*/)
	{
	return KErrNone;
	}


/**
Reference()

Populates aRrlpRef with the session reference 

@param aRrlpRef on return, populated with the session reference details
@return error indication, KErrNone otherwise
*/	
EXPORT_C TInt CRrlpMessageBase::GetReference(TRrlpReference& aRrlpRef)
	{
	SUPLLOG(ELogP1, "CRrlpMessageBase::Reference() Begin\n");
	__ASSERT_DEBUG(iData != NULL, User::Invariant());
	aRrlpRef.aRefNum = iData->referenceNumber;
	
	TInt err = KErrNone;
	err = GetExtendedReference(aRrlpRef);
	if (err == KErrNotFound)
		{
		SUPLLOG(ELogP1, "CRrlpMessageBase::Reference() no extended reference present\n");
		}
	
	SUPLLOG(ELogP1, "CRrlpMessageBase::Reference() End\n");
	return KErrNone;
	}


/**
GetExtendedReference()

Overridden by derived classes representing messages that include rel-5
extended reference.

@param  aRrlpRef local copy of the session reference details
@return KErrNotFound if the extended reference parameters are not present,
        KErrNone otherwise
*/
TInt CRrlpMessageBase::GetExtendedReference(TRrlpReference& /*aRrlpRef*/)
	{
	return KErrNotFound;
	}


/** 
EncodeToL()

Encode a populated outgoing message to the specified buffer.

@param  aBuf buffer pointer 
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CRrlpMessageBase::EncodeToL(TPtr8& aBuf)
	{
	SUPLLOG(ELogP1, "CRrlpMessageBase::EncodeToL() Begin\n");
	__ASSERT_DEBUG(iIsOutgoingMessage, User::Invariant());
	
	// buffer pointer, fixed max length
	TUint8* msgBuf = (TUint8*)aBuf.Ptr();
	TInt maxLength = aBuf.MaxLength();

	// construct the encode buffer control object
	iEncodeBuffer = new (ELeave) ASN1PEREncodeBuffer (msgBuf, (unsigned int)maxLength, FALSE, (OSRTContext*)iControl->getContext());
	
	// Encode the message to the buffer
	TInt stat = KErrNone;
	stat = iControl->EncodeTo(*iEncodeBuffer);

	TInt retval = KErrNone;
	if (stat == 0)
		{
		// Set the length according to reported buffer length
		TInt len = iEncodeBuffer->getMsgLen();
		aBuf.SetLength(len);
		}
	else
		{
		retval = ProcessAsn1Error(stat);
		}

	delete iEncodeBuffer;
	iEncodeBuffer = NULL;

	SUPLLOG(ELogP1, "CRrlpMessageBase::EncodeToL() End\n");
	return retval;
	}


/** 
BuildAssistanceData()

@param  aDataMask populated to indicate what assistance data has been received
@param  aData     reference to assistance data builder object to populate with
                  received assistance data
@param  aGpsData  the received/decoded gps assistance data RRLP element
@return error indication, KErrNone otherwise
*/
TInt CRrlpMessageBase::BuildAssistanceData(TLbsAsistanceDataGroup& aDataMask, RLbsAssistanceDataBuilderSet& aAssistanceDataBuilderSet, ASN1T_ControlHeader& aGpsData)
	{
	SUPLLOG(ELogP1, "CRrlpMessageBase::BuildAssistanceData() Begin\n");
	// clear the data mask
	aDataMask = EAssistanceDataNone;
	
	// Reference Time
	if (aGpsData.m.referenceTimePresent)
		{
		SUPLLOG(ELogP1, "   Reference Time received...\n");
		aDataMask |= EAssistanceDataReferenceTime;
		
		// Fetch a GpsReferenceTime data item.
		RUEPositioningGpsReferenceTimeBuilder* gpsReferenceTimeBuilder;
		aAssistanceDataBuilderSet.GetDataBuilder(gpsReferenceTimeBuilder);

		WriteReferenceTimeData(gpsReferenceTimeBuilder, aGpsData);

		TTime timeNow;
		timeNow.UniversalTime();
		gpsReferenceTimeBuilder->SetTimeStamp(timeNow);
		}

	// Reference Location
	if (aGpsData.m.refLocationPresent)
		{
		SUPLLOG(ELogP1, "   Reference Location received...\n");
		aDataMask |= EAssistanceDataReferenceLocation;
		
		// Fetch a ReferenceLocation data item.
		RReferenceLocationBuilder* gpsReferenceLocationBuilder;
		aAssistanceDataBuilderSet.GetDataBuilder(gpsReferenceLocationBuilder);

		WriteReferenceLocationData(gpsReferenceLocationBuilder, aGpsData);
		
		TTime timeNow;
		timeNow.UniversalTime();
		gpsReferenceLocationBuilder->SetTimeStamp(timeNow);
		}

	// DGPS Corrections
	if (aGpsData.m.dgpsCorrectionsPresent)
		{
		SUPLLOG(ELogP1, "   DGPS Corrections received, ignoring...\n");
		// never requested by LBS, no data builder available. Ignore.
		}
	
	// Navigation Model
	if (aGpsData.m.navigationModelPresent)
		{
		SUPLLOG(ELogP1, "   Navigation Model received...\n");
		aDataMask |= EAssistanceDataNavigationModel;
		
		// Fetch a GpsNavigationModel data item.
		RUEPositioningGpsNavigationModelBuilder* gpsNavigationModelBuilder;
		aAssistanceDataBuilderSet.GetDataBuilder(gpsNavigationModelBuilder);

		WriteNavigationModelData(gpsNavigationModelBuilder, aGpsData);
		
		TTime timeNow;
		timeNow.UniversalTime();
		gpsNavigationModelBuilder->SetTimeStamp(timeNow);	
		}

	// Ionospheric Model	
	if (aGpsData.m.ionosphericModelPresent)
		{
		SUPLLOG(ELogP1, "   Ionospheric Model received...\n");
		aDataMask |= EAssistanceDataIonosphericModel;
		
		//Fetch a GpsIonosphericModel data item.
		RUEPositioningGpsIonosphericModelBuilder* gpsIonosphericModelBuilder;
		aAssistanceDataBuilderSet.GetDataBuilder(gpsIonosphericModelBuilder);
		
		WriteIonosphericModelData(gpsIonosphericModelBuilder, aGpsData);
			
		TTime timeNow;
		timeNow.UniversalTime();
		gpsIonosphericModelBuilder->SetTimeStamp(timeNow);	
		}

	// UTC Model
	if (aGpsData.m.utcModelPresent)
		{
		SUPLLOG(ELogP1, "   UTC Model received...\n");
		aDataMask |= EAssistanceDataPositioningGpsUtcModel;
		
		//Fetch a GpsUTCModel data item.
		RUEPositioningGpsUtcModelBuilder* gpsUtcModelBuilder;
		aAssistanceDataBuilderSet.GetDataBuilder(gpsUtcModelBuilder);
		
		WriteUtcModelData(gpsUtcModelBuilder, aGpsData);
		
		TTime timeNow;
		timeNow.UniversalTime();
		gpsUtcModelBuilder->SetTimeStamp(timeNow);	
		}

	// Almanac
	if (aGpsData.m.almanacPresent)
		{
		SUPLLOG(ELogP1, "   Almanac received...\n");
		aDataMask |= EAssistanceDataAlmanac;
		
		//Fetch a GpsAlmanacBuilder data builder.
		RUEPositioningGpsAlmanacBuilder* gpsAlmanacBuilder;
		aAssistanceDataBuilderSet.GetDataBuilder(gpsAlmanacBuilder);
		
		WriteAlmanacData(gpsAlmanacBuilder, aGpsData);

		TTime timeNow;
		timeNow.UniversalTime();
		gpsAlmanacBuilder->SetTimeStamp(timeNow);	
		}

	// Acquisition Assistance
	if (aGpsData.m.acquisAssistPresent)
		{
		SUPLLOG(ELogP1, "   Aquisition Assistance received...\n");
		aDataMask |= EAssistanceDataAquisitionAssistance;
		
		//Fetch a GpsAcquisitionAssistance data builder.
		RUEPositioningGpsAcquisitionAssistanceBuilder* gpsAcquisitionAssistanceBuilder;
		aAssistanceDataBuilderSet.GetDataBuilder(gpsAcquisitionAssistanceBuilder);

		WriteAcquisitionAssistanceData(gpsAcquisitionAssistanceBuilder, aGpsData);

		TTime timeNow;
		timeNow.UniversalTime();
		gpsAcquisitionAssistanceBuilder->SetTimeStamp(timeNow);
		}

	// Real Time Integrity
	if (aGpsData.m.realTimeIntegrityPresent)
		{
		SUPLLOG(ELogP1, "   Real Time Integrity received...\n");
		aDataMask |= EAssistanceDataBadSatList;
		
		//Fetch a GpsBadSatList data item.
		RBadSatListBuilder* badSatListBuilder;
		aAssistanceDataBuilderSet.GetDataBuilder(badSatListBuilder);
			
		WriteRealTimeIntegrityData(badSatListBuilder, aGpsData);
	
		TTime timeNow;
		timeNow.UniversalTime();
		badSatListBuilder->SetTimeStamp(timeNow);	
		}
	
	SUPLLOG(ELogP1, "CRrlpMessageBase::BuildAssistanceData() End\n");
	return KErrNone;
	}


/** 
WriteReferenceTimeData()

Writes the recevieved GPS Assistance Data (Reference Time) to the Assistance 
Data Builder.

@param aReferenceTimeBuilder, the LBS assistance data builder
@param aGpsData, the received/decoded gps assistance data RRLP element
*/
void CRrlpMessageBase::WriteReferenceTimeData(RUEPositioningGpsReferenceTimeBuilder* aReferenceTimeBuilder, ASN1T_ControlHeader& aGpsData)
	{
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteReferenceTimeData() Begin\n");
	// GPS TIME elements
	TUint gpsWeek = aGpsData.referenceTime.gpsTime.gpsWeek;
	TUint gpsTow = aGpsData.referenceTime.gpsTime.gpsTOW23b * KGpsTowMultiplier;
	aReferenceTimeBuilder->SetField(TUEPositioningGpsReferenceTime::EGpsWeek, gpsWeek);
	aReferenceTimeBuilder->SetField(TUEPositioningGpsReferenceTime::EGpsTow1Msec, gpsTow);
	
	// GSM TIME elements (optional)
	// Not currently supported by this implementation
	// LBS assistance data builder support required.
		
	// GPS TOW ASSIST elements (optional)
	if (aGpsData.referenceTime.m.gpsTowAssistPresent != 0)
		{
		RGpsTowAssistArrayBuilder towAssistArrayBuilder;
		aReferenceTimeBuilder->GetArrayBuilder(TUEPositioningGpsReferenceTime::EGpsTowAssistArray, towAssistArrayBuilder);
		
		// array accessor
		ASN1C_GPSTOWAssist towAssistArray(aGpsData.referenceTime.gpsTowAssist);
		
		TInt count = aGpsData.referenceTime.gpsTowAssist.count;
		for (TInt i=0; i<count && i<KAsn1RrlpMaxSatNumber; i++)
			{
			RGpsTowAssistBuilder towAssistBuilder;	
			towAssistArrayBuilder.GetFieldBuilder(i, towAssistBuilder);

			ASN1T_GPSTOWAssistElement* towElement =  (ASN1T_GPSTOWAssistElement*)towAssistArray.get(i);
			TUint satId = towElement->satelliteID;
			TUint tlmMessage = towElement->tlmWord;
			TBool alert = (towElement->alert !=0 ? ETrue : EFalse);
			TBool antiSpoof = (towElement->antiSpoof !=0 ? ETrue : EFalse);
		
			towAssistBuilder.SetField(TGpsTowAssist::ESatID, satId);
			towAssistBuilder.SetField(TGpsTowAssist::ETlmMessage, tlmMessage);
			towAssistBuilder.SetField(TGpsTowAssist::EAlert, alert);
			towAssistBuilder.SetField(TGpsTowAssist::EAntiSpoof, antiSpoof);
			}
		}
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteReferenceTimeData() End\n");
	}



/** 
WriteReferenceLocationData()

Writes the recevieved GPS Assistance Data (Reference Location) to the Assistance 
Data Builder.

@param aReferenceLocationBuilder, the LBS assistance data builder
@param aGpsData, the received/decoded gps assistance data RRLP element
*/
void CRrlpMessageBase::WriteReferenceLocationData(RReferenceLocationBuilder* aReferenceLocationBuilder, ASN1T_ControlHeader& aGpsData)
	{
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteReferenceLocationData() Begin\n");
	// Allowed shape type is ellipsoid point with altitude and uncertainty ellipsoid
	TUint shapeType = 0;
	shapeType |= aGpsData.refLocation.threeDLocation.data[0]>>4;
	
	// handle latitude and longitude whatever the shape type
	TUint latitude = 0;
	latitude |= aGpsData.refLocation.threeDLocation.data[1];
	latitude = latitude<<8;
	latitude |= aGpsData.refLocation.threeDLocation.data[2];
	latitude = latitude<<8;
	latitude |= aGpsData.refLocation.threeDLocation.data[3];
	latitude &= 0x7FFFFF; // clear the sign of latitude bit.

	// latitude sign, 0 == north, 1 == south
	TEllipsoidPointAltitudeEllipsoide::TLatitudeSign latitudeSign = TEllipsoidPointAltitudeEllipsoide::ENorth;
	if ((aGpsData.refLocation.threeDLocation.data[1] & 128) != 0)
		{
		latitudeSign = TEllipsoidPointAltitudeEllipsoide::ESouth;
		}
	
	TInt longitude = 0;
	longitude |= aGpsData.refLocation.threeDLocation.data[4];
	longitude = longitude<<8;
	longitude |= aGpsData.refLocation.threeDLocation.data[5];
	longitude = longitude<<8;
	longitude |= aGpsData.refLocation.threeDLocation.data[6];
	
	TUint altitude = 0;
	altitude |= aGpsData.refLocation.threeDLocation.data[7];
	altitude = altitude<<8;
	altitude |= aGpsData.refLocation.threeDLocation.data[8];
	
	// handling of altitude and uncertainty dependant on shape type.
	// note that RRLP specifies usage of 3D location with uncertainty, so
	// no need to handle other types.
	if (shapeType == K3DLocationWithUncertainty)
		{
		// altitude direction, 0 == above, 1 == below WGS84 ellipsoid
		TEllipsoidPointAltitudeEllipsoide::TAltitudeDirection altitudeDirection = TEllipsoidPointAltitudeEllipsoide::EHeight;
		if ((aGpsData.refLocation.threeDLocation.data[7] & 128) != 0)
			{
			altitudeDirection = TEllipsoidPointAltitudeEllipsoide::EDepth;
			}

		// uncertainty etc...
		TUint uncertSemiMaj  = aGpsData.refLocation.threeDLocation.data[9];
		TUint uncertSemiMin  = aGpsData.refLocation.threeDLocation.data[10];
		TUint orientation    = aGpsData.refLocation.threeDLocation.data[11];
		TUint uncertAltitude = aGpsData.refLocation.threeDLocation.data[12];
		TUint confidence     = aGpsData.refLocation.threeDLocation.data[13];
			
		REllipsoidPointAltitudeEllipsoideBuilder ellipsoidPointAltBuilder;	
		aReferenceLocationBuilder->GetFieldBuilder(TReferenceLocation::EEllipsoidPointAltitudeEllipsoide, ellipsoidPointAltBuilder);

		ellipsoidPointAltBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::ELatitudeSign, latitudeSign);
		ellipsoidPointAltBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::ELatitude, latitude);
		ellipsoidPointAltBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::ELongitude, longitude);
		ellipsoidPointAltBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::EAltitudeDirection, altitudeDirection);
		ellipsoidPointAltBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::EAltitude, altitude);
		ellipsoidPointAltBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::EUncertaintySemiMajor, uncertSemiMaj);
		ellipsoidPointAltBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::EUncertaintySemiMinor, uncertSemiMin);
		ellipsoidPointAltBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::EOrientationMajorAxis, orientation);
		ellipsoidPointAltBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::EUncertaintyAltitude, uncertAltitude);
		ellipsoidPointAltBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::EConfidence, confidence);
		}
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteReferenceLocationData() End\n");
	}


/** 
WriteNavigationModelData()

Writes the recevieved GPS Assistance Data (Navigation Model) to the Assistance 
Data Builder.

@param aGpsNavigationModelBuilder, the LBS assistance data builder
@param aGpsData, the received/decoded gps assistance data RRLP element
*/
void CRrlpMessageBase::WriteNavigationModelData(RUEPositioningGpsNavigationModelBuilder* aGpsNavigationModelBuilder, ASN1T_ControlHeader& aGpsData)
	{
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteNavigationModelData() Begin\n");
	// Fetch a GpsNavigationModelSatInfoArray data item.
	RNavigationModelSatInfoArrayBuilder navigationModelSatInfoArrayBuilder;
	aGpsNavigationModelBuilder->GetArrayBuilder(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray, navigationModelSatInfoArrayBuilder);		

	// array accessor
	ASN1C_SeqOfNavModelElement satInfoArray(aGpsData.navigationModel.navModelList);

	TInt count = aGpsData.navigationModel.navModelList.count;
	for (TInt i=0 ; i<count && i<KAsn1RrlpMaxSatNumber; ++i)
		{
		// Fetch a GpsNavigationModelSatInfo data item.
		RNavigationModelSatInfoBuilder navigationModelSatInfoBuilder;	
		navigationModelSatInfoArrayBuilder.GetFieldBuilder(i, navigationModelSatInfoBuilder);
		
		// ASN1T_ControlHeader
		// Fetch the received satellite info
		ASN1T_NavModelElement* satInfo = (ASN1T_NavModelElement*)satInfoArray.get(i);
		
		WriteNavigationModelSatInfo(&navigationModelSatInfoBuilder, *satInfo);	
		}		
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteNavigationModelData() End\n");
	}


/** 
WriteNavigationModelSatInfo()

Writes the recevieved GPS Assistance Data (Navigation Model Satellite Info Array) 
to the Assistance Data Builder.

@param aGpsNavigationModelBuilder, the LBS assistance data builder
@param aGpsData, the received/decoded gps assistance data RRLP element
*/
void CRrlpMessageBase::WriteNavigationModelSatInfo(RNavigationModelSatInfoBuilder* aNavigationModelSatInfoBuilder, ASN1T_NavModelElement& aSatInfo)
	{
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteNavigationModelSatInfo() Begin\n");
	// satellite ID, status
	aNavigationModelSatInfoBuilder->SetField(TNavigationModelSatInfo::ESatId, (TUint)aSatInfo.satelliteID);
	aNavigationModelSatInfoBuilder->SetField(TNavigationModelSatInfo::ESatelliteStatus, (TSatelliteStatus)(aSatInfo.satStatus.t-EZeroOffsetAdjust));
	
	// ephemeris data if either "new sat, new nav model" OR "existing sat, new nav model"
	if (aSatInfo.satStatus.t == T_SatStatus_newSatelliteAndModelUC)
		{
		WriteSatInfoEphemeris(aNavigationModelSatInfoBuilder, *aSatInfo.satStatus.u.newSatelliteAndModelUC);
		}
	else if (aSatInfo.satStatus.t == T_SatStatus_newNaviModelUC)
		{
		WriteSatInfoEphemeris(aNavigationModelSatInfoBuilder, *aSatInfo.satStatus.u.newNaviModelUC);
		}
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteNavigationModelSatInfo() End\n");
	}


/** 
WriteSatInfoEphemeris()

Writes the recevieved GPS Assistance Data (Navigation Model Satellite Info Element) 
to the Assistance Data Builder.

@param aNavigationModelSatInfoBuilder, the LBS assistance data builder
@param aGpsData, the received/decoded gps assistance data RRLP element
*/
void CRrlpMessageBase::WriteSatInfoEphemeris(RNavigationModelSatInfoBuilder* aNavigationModelSatInfoBuilder, ASN1T_UncompressedEphemeris& aSatEphemeris)
	{
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteSatInfoEphemeris() Begin\n");
	REphemerisParameterBuilder ephemerisBuilder;
	aNavigationModelSatInfoBuilder->GetFieldBuilder(TNavigationModelSatInfo::EEphemerisParameter, ephemerisBuilder);
	
	TUint value = aSatEphemeris.ephemCodeOnL2;
	ephemerisBuilder.SetField(TEphemerisParameter::ECodeOnL2, value);
	
	value = aSatEphemeris.ephemURA;
	ephemerisBuilder.SetField(TEphemerisParameter::EUraIndex, value);
	
	value = aSatEphemeris.ephemSVhealth;
	ephemerisBuilder.SetField(TEphemerisParameter::ESatHealth, value);
	
	value = aSatEphemeris.ephemIODC;
	ephemerisBuilder.SetField(TEphemerisParameter::EIodc, value);
	
	value = aSatEphemeris.ephemL2Pflag;
	ephemerisBuilder.SetField(TEphemerisParameter::EL2Pflag, value);
	
	value = aSatEphemeris.ephemTgd;
	ephemerisBuilder.SetField(TEphemerisParameter::ETGD, value);
	
	value = aSatEphemeris.ephemToc;
	ephemerisBuilder.SetField(TEphemerisParameter::EToc, value);
	
	value = aSatEphemeris.ephemAF2;
	ephemerisBuilder.SetField(TEphemerisParameter::EAf2, value);
	
	value = aSatEphemeris.ephemAF1;
	ephemerisBuilder.SetField(TEphemerisParameter::EAf1, value);
	
	value = aSatEphemeris.ephemAF0;
	ephemerisBuilder.SetField(TEphemerisParameter::EAf0, value);
	
	value = aSatEphemeris.ephemCrs;
	ephemerisBuilder.SetField(TEphemerisParameter::ECrs, value);
	
	value = aSatEphemeris.ephemDeltaN;
	ephemerisBuilder.SetField(TEphemerisParameter::EDeltaN, value);
	
	value = aSatEphemeris.ephemM0;
	ephemerisBuilder.SetField(TEphemerisParameter::EM0, value);
	
	value = aSatEphemeris.ephemCuc;
	ephemerisBuilder.SetField(TEphemerisParameter::ECuc, value);
	
	value = aSatEphemeris.ephemE;
	ephemerisBuilder.SetField(TEphemerisParameter::EE, value);
	
	value = aSatEphemeris.ephemCus;
	ephemerisBuilder.SetField(TEphemerisParameter::ECus, value);
	
	value = aSatEphemeris.ephemAPowerHalf;
	ephemerisBuilder.SetField(TEphemerisParameter::EASqrt, value);
	
	value = aSatEphemeris.ephemToe;
	ephemerisBuilder.SetField(TEphemerisParameter::EToe, value);
	
	value = aSatEphemeris.ephemFitFlag;
	ephemerisBuilder.SetField(TEphemerisParameter::EFitInterval, value);
	
	value = aSatEphemeris.ephemAODA;
	ephemerisBuilder.SetField(TEphemerisParameter::EAodo, value);
	
	value = aSatEphemeris.ephemCic;
	ephemerisBuilder.SetField(TEphemerisParameter::ECic, value);
	
	value = aSatEphemeris.ephemOmegaA0;
	ephemerisBuilder.SetField(TEphemerisParameter::EOmega0, value);
	
	value = aSatEphemeris.ephemCis;
	ephemerisBuilder.SetField(TEphemerisParameter::ECis, value);
	
	value = aSatEphemeris.ephemI0;
	ephemerisBuilder.SetField(TEphemerisParameter::EI0, value);
	
	value = aSatEphemeris.ephemCrc;
	ephemerisBuilder.SetField(TEphemerisParameter::ECrc, value);
	
	value = aSatEphemeris.ephemW;
	ephemerisBuilder.SetField(TEphemerisParameter::EOmega, value);
	
	value = aSatEphemeris.ephemOmegaADot;
	ephemerisBuilder.SetField(TEphemerisParameter::EOmegaDot, value);
	
	value = aSatEphemeris.ephemIDot;
	ephemerisBuilder.SetField(TEphemerisParameter::EIDot, value);
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteSatInfoEphemeris() End\n");
	}


/** 
WriteIonosphericModelData()

Writes the recevieved GPS Assistance Data (Ionospheric Model) to the Assistance 
Data Builder.

@param aGpsIonosphericModelBuilder, the LBS assistance data builder
@param aGpsData, the received/decoded gps assistance data RRLP element
*/
void CRrlpMessageBase::WriteIonosphericModelData(RUEPositioningGpsIonosphericModelBuilder* aGpsIonosphericModelBuilder, ASN1T_ControlHeader& aGpsData)
	{
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteIonosphericModelData() Begin\n");
	// Ionospheric Model Data
	aGpsIonosphericModelBuilder->SetField(TUEPositioningGpsIonosphericModel::EAlfa0, (TUint)aGpsData.ionosphericModel.alfa0);
	aGpsIonosphericModelBuilder->SetField(TUEPositioningGpsIonosphericModel::EAlfa1, (TUint)aGpsData.ionosphericModel.alfa1);
	aGpsIonosphericModelBuilder->SetField(TUEPositioningGpsIonosphericModel::EAlfa2, (TUint)aGpsData.ionosphericModel.alfa2);
	aGpsIonosphericModelBuilder->SetField(TUEPositioningGpsIonosphericModel::EAlfa3, (TUint)aGpsData.ionosphericModel.alfa3);
	aGpsIonosphericModelBuilder->SetField(TUEPositioningGpsIonosphericModel::EBeta0, (TUint)aGpsData.ionosphericModel.beta0);
	aGpsIonosphericModelBuilder->SetField(TUEPositioningGpsIonosphericModel::EBeta1, (TUint)aGpsData.ionosphericModel.beta1);
	aGpsIonosphericModelBuilder->SetField(TUEPositioningGpsIonosphericModel::EBeta2, (TUint)aGpsData.ionosphericModel.beta2);
	aGpsIonosphericModelBuilder->SetField(TUEPositioningGpsIonosphericModel::EBeta3, (TUint)aGpsData.ionosphericModel.beta3);
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteIonosphericModelData() End\n");
	}
		

/** 
WriteUtcModelData()

Writes the recevieved GPS Assistance Data (UTC Model) to the Assistance 
Data Builder.

@param aGpsUtcModelBuilder, the LBS assistance data builder
@param aGpsData, the received/decoded gps assistance data RRLP element
*/
void CRrlpMessageBase::WriteUtcModelData(RUEPositioningGpsUtcModelBuilder* aGpsUtcModelBuilder, ASN1T_ControlHeader& aGpsData)
	{
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteUtcModelData() Begin\n");
	// Universal Time Coordinate Model
	aGpsUtcModelBuilder->SetField(TUEPositioningGpsUtcModel::EA1, (TUint)aGpsData.utcModel.utcA1);
	aGpsUtcModelBuilder->SetField(TUEPositioningGpsUtcModel::EA0, (TUint)aGpsData.utcModel.utcA0);
	aGpsUtcModelBuilder->SetField(TUEPositioningGpsUtcModel::ETot, (TUint)aGpsData.utcModel.utcTot);
	aGpsUtcModelBuilder->SetField(TUEPositioningGpsUtcModel::EWnt, (TUint)aGpsData.utcModel.utcWNt);	
	aGpsUtcModelBuilder->SetField(TUEPositioningGpsUtcModel::EDeltatLS, (TUint)aGpsData.utcModel.utcDeltaTls);	
	aGpsUtcModelBuilder->SetField(TUEPositioningGpsUtcModel::EWnlsf, (TUint)aGpsData.utcModel.utcWNlsf);	
	aGpsUtcModelBuilder->SetField(TUEPositioningGpsUtcModel::EDn, (TUint)aGpsData.utcModel.utcDN);	
	aGpsUtcModelBuilder->SetField(TUEPositioningGpsUtcModel::EDeltatLSF, (TUint)aGpsData.utcModel.utcDeltaTlsf);	
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteUtcModelData() End\n");
	}
	

/** 
WriteUtcModelData()

Writes the recevieved GPS Assistance Data (UTC Model) to the Assistance 
Data Builder.

@param aGpsUtcModelBuilder, the LBS assistance data builder
@param aGpsData, the received/decoded gps assistance data RRLP element
*/
void CRrlpMessageBase::WriteAlmanacData(RUEPositioningGpsAlmanacBuilder* aGpsAlmanacBuilder, ASN1T_ControlHeader& aGpsData)
	{
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteAlmanacData() Begin\n");
	
	// Almanac
	aGpsAlmanacBuilder->SetField(TUEPositioningGpsAlmanac::EWnA, (TUint)aGpsData.almanac.alamanacWNa);

	// source array accessor
	ASN1C_SeqOfAlmanacElement almanacArray(aGpsData.almanac.almanacList);

	// Populate almanac satellite info array
	RAlmanacSatInfoArrayBuilder almanacSatInfoArrayBuilder;
	aGpsAlmanacBuilder->GetArrayBuilder(TUEPositioningGpsAlmanac::EAlmanacInfoSatArray, almanacSatInfoArrayBuilder);
		
	TInt count = aGpsData.almanac.almanacList.count;
	// Copy assistance data for a maximun of KAsn1RrlpMaxSatNumber
	// satellites (LBS data structures cannot accomodate more)
	for (TInt i=0 ; i<count && i<KAsn1RrlpMaxSatNumber; ++i)
		{
		RAlmanacSatInfoBuilder almanacSatInfoBuilder;	
		almanacSatInfoArrayBuilder.GetFieldBuilder(i, almanacSatInfoBuilder);

		ASN1T_AlmanacElement* almanacElement =  (ASN1T_AlmanacElement*)almanacArray.get(i);
		
		WriteAlmanacSatInfo(&almanacSatInfoBuilder, *almanacElement);
		}
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteAlmanacData() End\n");
	}


/** 
WriteAlmanacSatInfo()

Writes the recevieved GPS Assistance Data (UTC Model) to the Assistance 
Data Builder.

@param aGpsUtcModelBuilder, the LBS assistance data builder
@param aGpsData, the received/decoded gps assistance data RRLP element
*/
void CRrlpMessageBase::WriteAlmanacSatInfo(RAlmanacSatInfoBuilder* aAlmanacSatInfoBuilder, ASN1T_AlmanacElement& aAlmanacElement)
	{
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteAlmanacSatInfo() Begin\n");
	// Almanac

	// Note: no RRLP data to populate TAlmanacSatInfo::EDataID field
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::ESatID, (TUint)aAlmanacElement.satelliteID);
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::EE, (TUint)aAlmanacElement.almanacE);
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::EToa, (TUint)aAlmanacElement.alamanacToa);
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::EDeltaI, (TUint)aAlmanacElement.almanacKsii);
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::EOmegaDot, (TUint)aAlmanacElement.almanacOmegaDot);
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::ESatHealth, (TUint)aAlmanacElement.almanacSVhealth);
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::EASqrt, (TUint)aAlmanacElement.almanacAPowerHalf);
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::EOmega0, (TUint)aAlmanacElement.almanacOmega0);
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::EM0, (TUint)aAlmanacElement.almanacM0);
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::EOmega, (TUint)aAlmanacElement.almanacW);
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::EAf0, (TUint)aAlmanacElement.almanacAF0);
	aAlmanacSatInfoBuilder->SetField(TAlmanacSatInfo::EAf1, (TUint)aAlmanacElement.almanacAF1);
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteAlmanacSatInfo() End\n");
	}


/** 
WriteAcquisitionAssistanceData()

Writes the recevieved GPS Assistance Data (Acquisition Assistance Data) to 
the Assistance Data Builder.

@param aGpsAcquisitionAssistanceBuilder, the LBS assistance data builder
@param aGpsData, the received/decoded gps assistance data RRLP element
*/
void CRrlpMessageBase::WriteAcquisitionAssistanceData(RUEPositioningGpsAcquisitionAssistanceBuilder* aGpsAcquisitionAssistanceBuilder, ASN1T_ControlHeader& aGpsData)
	{
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteAcquisitionAssistanceData() Begin\n");
	// Aquisition Assistance

	// GSM TIME elements (optional)
	// Not currently supported by this implementation
	// LBS assistance data builder support required.
		
	// source array accessor
	ASN1C_SeqOfAcquisElement acquisAssistArray(aGpsData.acquisAssist.acquisList);

	// Populate almanac satellite info array
	RAcquisitionSatInfoArrayBuilder acquisitionSatInfoArrayBuilder;
	aGpsAcquisitionAssistanceBuilder->GetArrayBuilder(TUEPositioningGpsAcquisitionAssistance::ESatelliteInformationArray, acquisitionSatInfoArrayBuilder);
		
	TInt count = aGpsData.acquisAssist.acquisList.count;
	for (TInt i=0 ; i<count && i<KAsn1RrlpMaxSatNumber; ++i)
		{
		RAcquisitionSatInfoBuilder acquisitionSatInfoBuilder;	
		acquisitionSatInfoArrayBuilder.GetFieldBuilder(i, acquisitionSatInfoBuilder);

		ASN1T_AcquisElement* acquisAssistElement =  (ASN1T_AcquisElement*)acquisAssistArray.get(i);
		
		WriteAcquisAssistSatInfo(&acquisitionSatInfoBuilder, *acquisAssistElement);
		}
		
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteAcquisitionAssistanceData() End\n");
	}


/** 
WriteAcquisAssistSatInfo()

Writes the recevieved GPS Assistance Data (Acquisition Assistance Satellite Info) 
to the Assistance Data Builder.

@param aAcquisitionSatInfoBuilder, the LBS assistance data builder
@param aGpsData, the received/decoded gps assistance data RRLP element
*/
void CRrlpMessageBase::WriteAcquisAssistSatInfo(RAcquisitionSatInfoBuilder* aAcquisitionSatInfoBuilder, ASN1T_AcquisElement& aAcquisAssistElement)
	{
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteAcquisAssistSatInfo() Begin\n");
	aAcquisitionSatInfoBuilder->SetField(TAcquisitionSatInfo::ESatID, (TUint)aAcquisAssistElement.svid);
	aAcquisitionSatInfoBuilder->SetField(TAcquisitionSatInfo::EDoppler0thOrder, (TInt)aAcquisAssistElement.doppler0);
	aAcquisitionSatInfoBuilder->SetField(TAcquisitionSatInfo::ECodePhase, (TUint)aAcquisAssistElement.codePhase);
	aAcquisitionSatInfoBuilder->SetField(TAcquisitionSatInfo::EIntegerCodePhase, (TUint)aAcquisAssistElement.intCodePhase);
	aAcquisitionSatInfoBuilder->SetField(TAcquisitionSatInfo::EGpsBitNumber, (TUint)aAcquisAssistElement.gpsBitNumber);
	aAcquisitionSatInfoBuilder->SetField(TAcquisitionSatInfo::ECodePhaseSearchWindow, (TCodePhaseSearchWindow)aAcquisAssistElement.codePhaseSearchWindow);

	if (aAcquisAssistElement.m.addionalDopplerPresent != 0)
		{
		RExtraDopplerInfoBuilder extraDopplerInfoBuilder;
		aAcquisitionSatInfoBuilder->GetFieldBuilder(TAcquisitionSatInfo::EExtraDopplerInfo, extraDopplerInfoBuilder);
		extraDopplerInfoBuilder.SetField(TExtraDopplerInfo::EDoppler1stOrder, (TInt)aAcquisAssistElement.addionalDoppler.doppler1);
		extraDopplerInfoBuilder.SetField(TExtraDopplerInfo::EDopplerUncertainty, (TDopplerUncertainty)aAcquisAssistElement.addionalDoppler.dopplerUncertainty);
		}
		
	if (aAcquisAssistElement.m.addionalAnglePresent)
		{
		RTAzimuthAndElevationBuilder azimuthAndElevationBuilder;
		aAcquisitionSatInfoBuilder->GetFieldBuilder(TAcquisitionSatInfo::EAzimuthAndElevation, azimuthAndElevationBuilder);
		azimuthAndElevationBuilder.SetField(TAzimuthAndElevation::EAzimuth, (TUint)aAcquisAssistElement.addionalAngle.azimuth);
		azimuthAndElevationBuilder.SetField(TAzimuthAndElevation::EElevation, (TUint)aAcquisAssistElement.addionalAngle.elevation);			
		}
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteAcquisAssistSatInfo() End\n");
	}


/** 
WriteRealTimeIntegrityData()

Writes the recevieved GPS Assistance Data (Real Time Integrity) to the 
Assistance Data Builder.

@param aBadSatListBuilder, the LBS assistance data builder
@param aGpsData, the received/decoded gps assistance data RRLP element
*/
void CRrlpMessageBase::WriteRealTimeIntegrityData(RBadSatListBuilder* aBadSatListBuilder, ASN1T_ControlHeader& aGpsData)
	{
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteRealTimeIntegrityData() Begin\n");
	// Real Time Integrity
	RBadSatArrayBuilder badSatArrayBuilder;
	aBadSatListBuilder->GetArrayBuilder(TBadSatList::EBadSatIdArray, badSatArrayBuilder);
	
	TInt count = aGpsData.realTimeIntegrity.n;
			
	for (TInt i=0 ; i<count && i<KAsn1RrlpMaxSatNumber; ++i)
		{	
		badSatArrayBuilder.SetElement(i, aGpsData.realTimeIntegrity.elem[i]);
		}
	SUPLLOG(ELogP1, "CRrlpMessageBase::WriteRealTimeIntegrityData() End\n");
	}


/**
LeaveIfAllocErrorL()

Calls User::Leave(<error code>) if a memory allocation has failed.
*/
void CRrlpMessageBase::LeaveIfAllocErrorL()
	{
	SUPLLOG(ELogP1, "CRrlpMessageBase::LeaveIfAllocErrorL() Begin\n");
	if (iControl->getStatus() == RTERR_NOMEM)
		{
		User::Leave(KErrNoMemory);
		}
	else if (iControl->getStatus() != RT_OK)
		{
		User::Leave(KErrGeneral);
		}
	SUPLLOG(ELogP1, "CRrlpMessageBase::LeaveIfAllocErrorL() End\n");
	}


/**
Translates error codes returned by the ASN1 runtime library to distinguish
from Symbian global error codes.

Errors are simply translated to positive error codes. They maintain their
meaning as described in rtxErrCodes.h and asn1ErrCodes.h.

Exceptions:
  RTERR_NOMEM is translated to global error code KErrNoMemory

@see rtxErrCodes.h
@see asn1ErrCodes.h
*/
TInt CRrlpMessageBase::ProcessAsn1Error(TInt aError)
	{
	SUPLLOG(ELogP1, "CRrlpMessageBase::ProcessAsn1Error() Begin\n");
	if (aError == RTERR_NOMEM)
		{
		SUPLLOG(ELogP1, "CRrlpMessageBase::ProcessAsn1Error() End (Out Of Memory)\n");
		return KErrNoMemory;
		}
	else
		{
		SUPLLOG2(ELogP1, "CRrlpMessageBase::ProcessAsn1Error() End (ASN1Error runtime error %d)\n", aError);
		return aError * -1;
		}
	}

/**
Prints the content payload data structure to the logger 
*/
void CRrlpMessageBase::LogMessageContent()
	{
	SUPLLOG(ELogP9, "RRLP PAYLOAD CONTENT\n");
	//SUPLLOG_PDU(iControl);
	}

