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

#ifndef LBSASSISTANCEALMANAC_H
#define LBSASSISTANCEALMANAC_H

#include <lbs/lbsassistancedatabase.h>
#include <e32def.h>

/**
TAlmanacSatInfo 
@publishedPartner
@released
*/
class TAlmanacSatInfo
	{
public:
	/** 
	TFieldId
	 */
	enum TFieldId
		{
		/** EDataID */
		EDataID,		//TUint
		/** ESatID */
		ESatID,			//TUint
		/** EE */
		EE,				//TUint
		/** EToa */
		EToa,			//TUint
		/** EDelta */
		EDeltaI,		//TUint
		/** EOmegaDot */
		EOmegaDot,		//TUint
		/** ESatHealth */
		ESatHealth,		//TUint
		/** EASqrt */
		EASqrt,			//TUint
		/** EOmegaO */
		EOmega0,		//TUint
		/** EM0 */
		EM0,			//TUint
		/** EOmega */
		EOmega,			//TUint
		/** EAf0 */
		EAf0,			//TUint
		/** EAf1 */
		EAf1			//TUint
		};
	};

/**
TUEPositioningGpsAlmanac
@publishedPartner
@released
*/
class TUEPositioningGpsAlmanac
	{
public:
	/**
	TFieldId
	 */
	enum TFieldId
		{
		/** EWna */
		EWnA,					//TUint
		/** EAlmanacInfoSatArray */
		EAlmanacInfoSatArray,	//TAlmanacSatInfo array
		/** ESvGlobalHealth */
		ESvGlobalHealth			//TBuf8<46>
		};
	};

//
//Builder classes

/**
Builder class base
@publishedPartner
@released
*/
class RAlmanacSatInfoBuilder : public RDataBuilder<TAlmanacSatInfo, RDataBuilderBase>
	{
public:
	IMPORT_C RAlmanacSatInfoBuilder();
	};

/**
Builder class array builder
@publishedPartner
@released
*/
class RAlmanacSatInfoArrayBuilder : public RDataBuilderArray<TAlmanacSatInfo>
	{
public:
	IMPORT_C RAlmanacSatInfoArrayBuilder();
	};

/**
Builder class builder
@publishedPartner
@released
*/
class RUEPositioningGpsAlmanacBuilder : public RDataBuilder<TUEPositioningGpsAlmanac, RDataBuilderRootBase>
	{
public:
	IMPORT_C RUEPositioningGpsAlmanacBuilder();
	IMPORT_C void OpenL();
	IMPORT_C static TUint DataSize();
	};

//
//Reader classes
/**
Almanac Satellite Info Reader
@publishedPartner
@released
*/
class RAlmanacSatInfoReader : public RDataReader<TAlmanacSatInfo, RDataReaderBase>
	{
public:
	IMPORT_C RAlmanacSatInfoReader();
	};

/**
Almanac Satellite Info Array Reader
@publishedPartner
@released
*/
class RAlmanacSatInfoArrayReader : public RDataReaderArray<TAlmanacSatInfo>
	{
public:
	IMPORT_C RAlmanacSatInfoArrayReader();
	};

/**
UE Positioning Gps Almanac Reader
@publishedPartner
@released
*/
class RUEPositioningGpsAlmanacReader : public RDataReader<TUEPositioningGpsAlmanac, RDataReaderRootBase>
	{
public:
	IMPORT_C RUEPositioningGpsAlmanacReader();
	IMPORT_C void OpenL();
	IMPORT_C static TUint DataSize();
	};

#endif //LBSASSISTANCEALMANAC_H

