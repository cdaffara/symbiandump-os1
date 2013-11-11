/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
#include <lbs/lbsassistancereferencelocation.h>
#include "LbsAssistanceInternal.h"
#include "LbsAssistanceReferenceLocationLayout.h"


const TLbsAssistanceDataNode KEllipsoidPointAltitudeEllipsoideLayout[] =
	{
		{_FOFF(TEllipsoidPointAltitudeEllipsoideLayout, iLatitudeSign), EIntType},
		{_FOFF(TEllipsoidPointAltitudeEllipsoideLayout, iLatitude), EUintType},
		{_FOFF(TEllipsoidPointAltitudeEllipsoideLayout, iLongitude), EIntType},
		{_FOFF(TEllipsoidPointAltitudeEllipsoideLayout, iAltitudeDirection), EIntType},
		{_FOFF(TEllipsoidPointAltitudeEllipsoideLayout, iAltitude), EUintType},
		{_FOFF(TEllipsoidPointAltitudeEllipsoideLayout, iUncertaintySemiMajor), EUintType},
		{_FOFF(TEllipsoidPointAltitudeEllipsoideLayout, iUncertaintySemiMinor), EUintType},
		{_FOFF(TEllipsoidPointAltitudeEllipsoideLayout, iOrientationMajorAxis), EUintType},
		{_FOFF(TEllipsoidPointAltitudeEllipsoideLayout, iUncertaintyAltitude), EUintType},
		{_FOFF(TEllipsoidPointAltitudeEllipsoideLayout, iConfidence), EUintType}
	};	

const TLbsAssistanceDataNode KReferenceLocationLayout[] =
	{
		{_FOFF(TReferenceLocationLayout, iEllipsoidPointAltitudeEllipsoide), EEllipsoidPointAltitudeEllipsoideType}
	};	

const TLbsAssistanceDataLayoutMap KEllipsoidPointAltitudeEllipsoideLayoutInfo = {sizeof(KEllipsoidPointAltitudeEllipsoideLayout)/sizeof(KEllipsoidPointAltitudeEllipsoideLayout[0]), &KEllipsoidPointAltitudeEllipsoideLayout[0]};
const TLbsAssistanceDataLayoutMap KReferenceLocationLayoutInfo = {sizeof(KReferenceLocationLayout)/sizeof(KReferenceLocationLayout[0]), &KReferenceLocationLayout[0]};


/**
RReferenceLocationBuilder constructor
 */
EXPORT_C RReferenceLocationBuilder::RReferenceLocationBuilder()
:	RDataBuilder<TReferenceLocation, RDataBuilderRootBase>(&KReferenceLocationLayoutInfo)
	{
	}

/**
Leaving Open method
 */
EXPORT_C void RReferenceLocationBuilder::OpenL()
	{
	DoOpenL(sizeof(TObjectAndIndex<TReferenceLocationLayout>));
	iBody.iIndexPtr = &(reinterpret_cast<TObjectAndIndex<TReferenceLocationLayout>*>(iData)->iIndex[0]);
	iBody.iBitOffset = 0;	//We're the root
	iBody.iUtcTimeStampPtr = &(reinterpret_cast<TObjectAndIndex<TReferenceLocationLayout>*>(iData)->iUtcTimeStamp);
	};

/**
Get the data size that is allocated
for this assistance data type
 */
EXPORT_C TUint RReferenceLocationBuilder::DataSize()
	{
	return sizeof(TObjectAndIndex<TReferenceLocationLayout>);
	}
	
/**
REllipsoidPointAltitudeEllipsoideBuilder constructor
 */
EXPORT_C REllipsoidPointAltitudeEllipsoideBuilder::REllipsoidPointAltitudeEllipsoideBuilder()
:	RDataBuilder<TEllipsoidPointAltitudeEllipsoide, RDataBuilderBase>(&KEllipsoidPointAltitudeEllipsoideLayoutInfo)
	{}

/**
RReferenceLocationReader constructor
 */
EXPORT_C RReferenceLocationReader::RReferenceLocationReader()
:	RDataReader<TReferenceLocation, RDataReaderRootBase>(&KReferenceLocationLayoutInfo)
	{}

/**
Leaving Open method
 */
EXPORT_C void RReferenceLocationReader::OpenL()
	{
	DoOpenL(sizeof(TObjectAndIndex<TReferenceLocationLayout>));
	iBody.iIndexPtr = &(reinterpret_cast<TObjectAndIndex<TReferenceLocationLayout>*>(iData)->iIndex[0]);
	iBody.iBitOffset = 0;	//We're the root
	iBody.iUtcTimeStampPtr = &(reinterpret_cast<TObjectAndIndex<TReferenceLocationLayout>*>(iData)->iUtcTimeStamp);
	}

/**
Get the data size that is allocated
for this assistance data type
 */
EXPORT_C TUint RReferenceLocationReader::DataSize()
	{
	return sizeof(TObjectAndIndex<TReferenceLocationLayout>);
	}

/**
REllipsoidPointAltitudeEllipsoideReader constructor
 */
EXPORT_C REllipsoidPointAltitudeEllipsoideReader::REllipsoidPointAltitudeEllipsoideReader()
:	RDataReader<TEllipsoidPointAltitudeEllipsoide, RDataReaderBase>(&KEllipsoidPointAltitudeEllipsoideLayoutInfo)
	{}

