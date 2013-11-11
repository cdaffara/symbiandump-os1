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
//

#ifndef LBSASSISTANCEREFERENCELOCATION_H
#define LBSASSISTANCEREFERENCELOCATION_H

#include <lbs/lbsassistancedatabase.h>
#include <e32def.h>

/**
Ellipsoid Point Altitude Ellipsoide. Defined using RRC spec.
@publishedPartner
@released
*/
class TEllipsoidPointAltitudeEllipsoide
	{
public:
	/**
	TLatitudeSign
	 */
	enum TLatitudeSign
		{
		/** ENorth */
		ENorth,
		/** ESouth */
		ESouth
		};

	/**
	TAltitudeDirection
	 */
	enum TAltitudeDirection
		{
		/** EHeight */
		EHeight,
		/** EDepth */
		EDepth
		};
		
	/**
	TFieldId
	 */
	enum TFieldId
		{
		/** ELatitudeSign */
		ELatitudeSign,			//TInt		takes values from TLatitudeSign
		/** ELatitude */
		ELatitude,				//TUint
		/** ELongitude */
		ELongitude,				//TInt
		/** EAltitudeDirection */
		EAltitudeDirection,		//TInt		takes values from TAltitudeDirection
		/** EAltitude */
		EAltitude,				//TUint
		/** EUncertaintySemiMajor */
		EUncertaintySemiMajor,	//TUint
		/** EUncertaintySemiMinor */
		EUncertaintySemiMinor,	//TUint
		/** EOrientationMajorAxis */
		EOrientationMajorAxis,	//TUint
		/** EUncertaintyAltitude */
		EUncertaintyAltitude,	//TUint
		/** EConfidence */
		EConfidence				//TUint
		};
	};

/**
Reference Location. Defined using RRC spec.
@publishedPartner
@released
*/	
class TReferenceLocation
	{
public:
	/**
	TFieldId
	 */
	enum TFieldId
		{
		/** EEllipsoidPointAltitudeEllipsoide */
		EEllipsoidPointAltitudeEllipsoide		//TEllipsoidPointAltitudeEllipsoide
		};
	};


//
//Builder classes
/**
Builder classes. Defined using RRC spec.
@publishedPartner
@released
*/	
class RReferenceLocationBuilder : public RDataBuilder<TReferenceLocation, RDataBuilderRootBase>
	{
public:
	IMPORT_C RReferenceLocationBuilder();
	IMPORT_C void OpenL();
	IMPORT_C static TUint DataSize();
	};

/**
Builder classes. Defined using RRC spec.
@publishedPartner
@released
*/
class REllipsoidPointAltitudeEllipsoideBuilder : public RDataBuilder<TEllipsoidPointAltitudeEllipsoide, RDataBuilderBase>
	{
public:
	IMPORT_C REllipsoidPointAltitudeEllipsoideBuilder();
	};


//
//Reader classes
/**
Reader classes. Defined using RRC spec.
@publishedPartner
@released
*/
class RReferenceLocationReader : public RDataReader<TReferenceLocation, RDataReaderRootBase>
	{
public:
	IMPORT_C RReferenceLocationReader();
	IMPORT_C void OpenL();
	IMPORT_C static TUint DataSize();
	};

/**
Ellipsoid Point Altitude EllipsoideReader. Defined using RRC spec.
@publishedPartner
@released
*/
class REllipsoidPointAltitudeEllipsoideReader : public RDataReader<TEllipsoidPointAltitudeEllipsoide, RDataReaderBase>
	{
public:
	IMPORT_C REllipsoidPointAltitudeEllipsoideReader();
	};


#endif //LBSASSISTANCEREFERENCELOCATION_H

