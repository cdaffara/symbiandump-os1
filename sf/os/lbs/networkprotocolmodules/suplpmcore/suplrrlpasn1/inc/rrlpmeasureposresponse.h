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

#ifndef RRLP_MEASURE_POSITION_RESPONSE_H
#define RRLP_MEASURE_POSITION_RESPONSE_H

#include "rrlpmessagebase.h"
#include <lbspositioninfo.h>
#include <lbs/lbsassistancedatabase.h> 
#include <lbs/lbsgpsmeasurement.h>
#include <lbs/lbsgpstimingmeasurement.h>

/** TRrlpLocError typedef.
@internalTechnology
*/
typedef TUint32 TRrlpLocError;

/**
Enumeration for location error Code parameter.

@internalTechnology
*/
enum _TRrlpLocError
	{
	ERrlpLocErrorUnDefined = 0,	
	ERrlpLocErrorNotEnoughBTSs = 1,
	ERrlpLocErrorNotEnoughSats = 2,
	ERrlpLocErrorEotdLocCalAssDataMissing = 3,
	ERrlpLocErrorEotdAssDataMissing = 4,
	ERrlpLocErrorGpsLocCalAssDataMissing = 5,
	ERrlpLocErrorGpsAssDataMissing = 6,
	ERrlpLocErrorMethodNotSupported = 7,
	ERrlpLocErrorNotProcessed = 8,
	ERrlpLocErrorRefBTSForGPSNotServingBTS = 9,
	ERrlpLocErrorRefBTSForEOTDNotServingBTS = 10
	};

/** TGeoInfo typedef.
@internalTechnology
*/
typedef TUint32 TGeoInfo;

/**
Enumeration for location estimate type

@internalTechnology

*/
enum _TGeoInfo
	{
	EGeoInfoEllipsoidPoint = 0,
	EGeoInfoEllipsoidPointWithUncertaintyCircle = 1,
	EGeoInfoEllipsoidPointWithUncertaintyEllipse = 3,
	EGeoInfoPolygon = 5,                    // not supported by RRLP
	EGeoInfoEllipsoidPointWithAltitude = 8, // not supported by RRLP
	EGeoInfoEllipsoidPointWithAltitudeAndUncertaintyEllipsoid = 9,
	EGeoInfoEllipsoidArc = 10               // not supported by RRLP
	};


/**
Encapsulation for received CRrlpMeasurePositionResponse messages.

@internalTechnology

*/
NONSHARABLE_CLASS(CRrlpMeasurePositionResponse) : public CRrlpMessageBase
	{
public:
	IMPORT_C static CRrlpMeasurePositionResponse* NewL();
	IMPORT_C static CRrlpMeasurePositionResponse* NewLC();
		
public:
	virtual ~CRrlpMeasurePositionResponse();
	
public:

	/**	Sets location information parameters */
	IMPORT_C TInt SetLocationInformation(TPositionInfoBase& aPosInfo);
	
	/** Sets measurement information parameters */
	IMPORT_C TInt SetMeasurementInformation(const TPositionInfoBase& aPosInfo);
	
	/** Sets Location Error Parameters */
	IMPORT_C TInt SetLocationError(TRrlpLocError aLocError);
	
	/** Sets Location Error Parameters with request for additional assistance data */
	IMPORT_C TInt SetLocationError(TRrlpLocError aLocError, const TLbsAsistanceDataGroup& aDataReqMask);

protected:

	/** Sets the extended reference parameters */
	TInt SetExtendedReference(const TRrlpReference& aRrlpRef);

    /** Sets GPS Fine Timing Data */
    void SetGpsTimingData(const TGpsTimingMeasurementData& aTimingData);
	
private:

	/** Constructor */
	CRrlpMeasurePositionResponse();

	/** second stage constructor */
	void ConstructL();
	
	/** Prohibit copy constructor */
	CRrlpMeasurePositionResponse(const CRrlpMeasurePositionResponse&);

	/** Prohibit assigment operator */
	CRrlpMeasurePositionResponse& operator= (const CRrlpMeasurePositionResponse&);
	
private:

	/** Calculates horizontal uncertainty constant from value in meters */
	TInt MetersToHorizontalUncertainty(const TReal32& aDistance);

	/** Calculates vertical uncertainty constant from value in meters */
	TInt MetersToVerticalUncertainty(const TReal32& aDistance);
	
	};
	

#endif // RRLP_MEASURE_POSITION_RESPONSE_H

