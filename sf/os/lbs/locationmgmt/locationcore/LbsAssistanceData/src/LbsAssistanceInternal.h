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
#ifndef LBSASSISTANCEINTERNAL_H
#define LBSASSISTANCEINTERNAL_H

#include <e32def.h>
#include <lbs/lbsassistancedatabase.h>

enum TSupportedFieldTypes
	{
	EIntType,
	EUintType,
	EDes8Type,
	EAlmanacSatInfoArrayType,
	EAlmanacSatInfoType,
	ESatelliteStatusType,
	EEphemerisParameterType,
	ENavigationModelSatInfoType,
	ENavigationModelSatInfoArrayType,
	EBadSatListArrayType,
	EEllipsoidPointAltitudeEllipsoideType,
	EGpsTowAssistType,
	EUtranGpsReferenceTimeType,
	ESfnTowUncertaintyType,
	EUtranGpsDriftRateType,
	EGpsTowAssistArrayType,
	EDopplerUncertaintyType,
	EExtraDopplerInfoType,
	ECodePhaseSearchWindowType,
	EAzimuthAndElevationType,
	EAcquisitionSatInfoType,
	EAcquisitionSatInfoArrayType,
	EPrimaryCpichInfoLayout,
	ESubFrame1ReservedArrayType,
	ERrlpGpsReferenceTimeType,
	};

struct TDes8PackBase
	{
	TInt iLength;
	};

struct TDes8PackEditor : public TDes8PackBase
	{
	TUint8 iData[1];	//Eeek!!!
	};

template <TInt Count>
struct TDes8PackMarker : public TDes8PackBase
	{
	TUint8 iData[Count];
	};
	
struct TLbsAssistanceDataNode
	{
	TInt iOffset;
	TSupportedFieldTypes iType;
	};

struct TLbsAssistanceDataLayoutMap
	{
	TInt iLayoutCount;
	const TLbsAssistanceDataNode* iLayout;
	};
	
template<typename T, typename BuilderBase>
RDataBuilder<T, BuilderBase>::RDataBuilder(const TLbsAssistanceDataLayoutMap* aLayout)
:	BuilderBase(aLayout)
	{
	}

template<typename T>
RDataBuilderArray<T>::RDataBuilderArray(const TLbsAssistanceDataLayoutMap* aLayout)
:	RDataBuilderArrayBase(aLayout)
	{
	}

template<typename T, typename ReaderBase>
RDataReader<T, ReaderBase>::RDataReader(const TLbsAssistanceDataLayoutMap* aLayout)
:	ReaderBase(aLayout)
	{
	}

template<typename T>
RDataReaderArray<T>::RDataReaderArray(const TLbsAssistanceDataLayoutMap* aLayout)
:	RDataReaderArrayBase(aLayout)
	{
	}

/*
In the code below, we use the strange looking ((sizeof(T)+127)/128) code.
Here's what we're trying to achieve, hopefully it will then make sense.
For each 4 bytes in the buffer we have one entry in the index - we don't support
indexing of objects smaller than 32 bytes, hence there's none in the API.
There are 32 bits in a TUint32, so for every 128 (4*32) bytes in the buffer we need 1
32 bit element in the index.
So for buffers of sizes 1 -> 128 bytes, we need 1 TUint32 in the index
						129 -> 256 bytes, we need 2 TUint32s in the index.
						257 -> 384 bytes, we need 3 TUint32s in the index.
						etc...
As division rounds down for integers in C++, we do (sizeof(T)+127)/128
to achieve the above mapping from buffer size to index size.
*/
template<typename T>
struct TObjectAndIndex
	{
	T iObject;
	TUint32 iIndex[((sizeof(T)+127)/128)];
	TTime iUtcTimeStamp;
	};



#endif //LBSASSISTANCEINTERNAL_H

