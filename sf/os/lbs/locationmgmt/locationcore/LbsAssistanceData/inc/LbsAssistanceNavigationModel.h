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


#ifndef LBSASSISTANCENAVIGATIONMODEL_H
#define LBSASSISTANCENAVIGATIONMODEL_H

#include <lbs/lbsassistancedatabase.h>
#include <e32def.h>

//Builder classes
/**
Ephemeris Parameter. Defined using RRC spec.
@publishedPartner
@released
*/
class TEphemerisParameter
	{
public:
	/**
	TFieldId
	All the Ephemeris items are bit strings stored in LSBs of a TUint buffer.
	All unused MSBs (for bit strings shorter than 32) are set to zeros.
	Information about the format of the bit strings can be found in ICD-GPS-200.
	 */
	enum TFieldId
		{
		/** 2 bits */
		ECodeOnL2,		//TUint
		/** 4 bits */
		EUraIndex,		//TUint
		/** 6 bits */
		ESatHealth,		//TUint
		/** 10 bits */
		EIodc,			//TUint
		/**  1 bit */
		EL2Pflag,		//TUint
		/** 8 bits */
		ETGD,			//TUint
		/** 16 bits */
		EToc,			//TUint
		/** 8 bits */
		EAf2,			//TUint
		/** 16 bits */
		EAf1,			//TUint
		/** 22 bits */
		EAf0,			//TUint
		/** 16 bits */
		ECrs,			//TUint
		/** 16 bits */
		EDeltaN,		//TUint
		/** 32 bits */
		EM0,			//TUint
		/** 16 bits */
		ECuc,			//TUint
		/** 32 bits */
		EE,				//TUint
		/** 16 bits */
		ECus,			//TUint
		/** 32 bits */
		EASqrt,			//TUint
		/** 16 bits */
		EToe,			//TUint
		/** 1 bit */
		EFitInterval,	//TUint
		/** 5 bits */
		EAodo,			//TUint
		/** 16 bits */
		ECic,			//TUint
		/** 32 bits */
		EOmega0,		//TUint
		/** 16 bits */
		ECis,			//TUint
		/** 32 bits */
		EI0,			//TUint
		/** 16 bits */
		ECrc,			//TUint
		/** 32 bits */
		EOmega,			//TUint
		/** 24 bits */
		EOmegaDot,		//TUint
		/** 14 bits */
		EIDot,			//TUint
		ESf1Revd,       //TSubFrame1ReservedArrayLayout - reserved bits in the subframe 1 of the GPS 
						// navigation message - an array of 4 elements (23, 24, 24 and 16 bits in size)
		};
	};


/**
Navigation Model Satellite Info. Defined using RRC spec.
@publishedPartner
@released
*/
class TNavigationModelSatInfo
	{
public:
	/**
	TFieldId
	 */
	enum TFieldId
		{
		/** ESatId */
		ESatId,				//TUint
		/** ESatelliteStatus */
		ESatelliteStatus,	//TSatelliteStatus
		/** EEphemerisParameter */
		EEphemerisParameter	//TEphemerisParameter
		};
	};

/**
UE Positioning Gps Navigation Model. Defined using RRC spec.
@publishedPartner
@released
*/	
class TUEPositioningGpsNavigationModel
	{
public:
	/**
	TFieldId
	 */
	enum TFieldId
		{
		/** ENavigationModelSatelliteInfoArray */
		ENavigationModelSatelliteInfoArray		//TNavigationModelSatInfo array
		};
	};

//
//Builder classes
/**
Builder classes. Defined using RRC spec.
@publishedPartner
@released
*/
class RUEPositioningGpsNavigationModelBuilder : public RDataBuilder<TUEPositioningGpsNavigationModel, RDataBuilderRootBase>
	{
public:
	IMPORT_C RUEPositioningGpsNavigationModelBuilder();
	IMPORT_C void OpenL();
	IMPORT_C static TUint DataSize();
	};

/**
Navigation Model Satellite Info Array Builder class. Defined using RRC spec.
@publishedPartner
@released
*/
class RNavigationModelSatInfoArrayBuilder : public RDataBuilderArray<TNavigationModelSatInfo>
	{
public:
	IMPORT_C RNavigationModelSatInfoArrayBuilder();
	};

/**
Navigation Model Satellute Info Builder classes. Defined using RRC spec.
@publishedPartner
@released
*/
class RNavigationModelSatInfoBuilder : public RDataBuilder<TNavigationModelSatInfo, RDataBuilderBase>
	{
public:
	IMPORT_C RNavigationModelSatInfoBuilder();
	};

/**
Ephemeris Parameter Builder class. Defined using RRC spec.
@publishedPartner
@released
*/
class REphemerisParameterBuilder : public RDataBuilder<TEphemerisParameter, RDataBuilderBase>
	{
public:
	IMPORT_C REphemerisParameterBuilder();
	};

/**
SubFrame1Reserved Array Builder class. These are reserved bits in the subframe 1 of the GPS 
navigation message - an array of 4 elements (23, 24, 24 and 16 bits in size). Defined using RRC spec.

@publishedPartner
@prototype
*/
class RSubFrame1ReservedArrayBuilder : public RDataBuilderArray<TUint>
    {
public:
    IMPORT_C RSubFrame1ReservedArrayBuilder();
    };

//
//Reader classes
/**
Reader classes. Defined using RRC spec.
@publishedPartner
@released
*/
class RUEPositioningGpsNavigationModelReader : public RDataReader<TUEPositioningGpsNavigationModel, RDataReaderRootBase>
	{
public:
	IMPORT_C RUEPositioningGpsNavigationModelReader();
	IMPORT_C void OpenL();
	IMPORT_C static TUint DataSize();
	};

/**
Navigation Model Satellite Info Array Reader class. Defined using RRC spec.
@publishedPartner
@released
*/
class RNavigationModelSatInfoArrayReader : public RDataReaderArray<TNavigationModelSatInfo>
	{
public:
	IMPORT_C RNavigationModelSatInfoArrayReader();
	};

/**
Navigation Model Satellite Info Reader class. Defined using RRC spec.
@publishedPartner
@released
*/
class RNavigationModelSatInfoReader : public RDataReader<TNavigationModelSatInfo, RDataReaderBase>
	{
public:
	IMPORT_C RNavigationModelSatInfoReader();
	};

/**
Ephemeris Parameter Reader class. Defined using RRC spec.
@publishedPartner
@released
*/
class REphemerisParameterReader : public RDataReader<TEphemerisParameter, RDataReaderBase>
	{
public:
	IMPORT_C REphemerisParameterReader();
	};

/**
SubFrame1Reserved Array Reader class. These are reserved bits in the subframe 1 of the GPS 
navigation message - an array of 4 elements (23, 24, 24 and 16 bits in size). Defined using RRC spec.
@publishedPartner
@prototype
*/
class RSubFrame1ReservedArrayReader : public RDataReaderArray<TUint>
    {
public:
    IMPORT_C RSubFrame1ReservedArrayReader();
    };

#endif //LBSASSISTANCENAVIGATIONMODEL_H

