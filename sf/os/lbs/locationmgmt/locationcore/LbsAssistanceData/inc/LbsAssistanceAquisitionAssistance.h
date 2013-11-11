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

#ifndef LBSASSISTANCEAQUISITIONASSISTANCE_H
#define LBSASSISTANCEAQUISITIONASSISTANCE_H

#include <lbs/lbsassistancedatabase.h>
#include <lbs/lbsassistanceutranreferencetime.h>
#include <lbs/lbsassistancerrlpreferencetime.h>
#include <e32def.h>


/**
Azimuth and Elevation. Defined using RRC spec.
@publishedPartner
@released
*/
class TAzimuthAndElevation
	{
public:
	/**
	TFieldId
	 */
	enum TFieldId
		{
		/** EAzimuth */
		EAzimuth,		//TUint
		/** EElevation */
		EElevation		//TUint
		};
	};

/**
Doppler Information. Defined using RRC spec.
@publishedPartner
@released
*/
class TExtraDopplerInfo
	{
public:
	/**
	TFieldId
	 */
	enum TFieldId
		{
		/** EDoppler1stOrder */
		EDoppler1stOrder,			//TInt
		/** EDopplerUncertainty */
		EDopplerUncertainty			//TDopplerUncertainty
		};
	};

/**
Satellite Acquisition Information. Defined using RRC spec.
@publishedPartner
@released
*/
class TAcquisitionSatInfo
	{
public:
	/**
	TFieldId
	 */
	enum TFieldId
		{
		/** ESatID */
		ESatID,						//TUint
		/** EDoppler0thOrder */
		EDoppler0thOrder,			//TInt
		/** EExtraDopplerInfo */
		EExtraDopplerInfo,			//TExtraDopplerInfo
		/** ECodePhase */
		ECodePhase,					//TUint
		/** EIntegerCodePhase */
		EIntegerCodePhase,			//TUint
		/** EGpsBitNumber */
		EGpsBitNumber,				//TUint
		/** ECodePhaseSearchWindow */
		ECodePhaseSearchWindow,		//TCodePhaseSearchWindow,
		/** EAzimuthAndElevation */
		EAzimuthAndElevation,		//TAzimuthAndElevation
		};
	};


/**
GPS Sattelite Acquisition Assistance. Defined using RRC spec.
@publishedPartner
@released
*/
class TUEPositioningGpsAcquisitionAssistance
	{
public:
	/**
	TFieldId
	 */
	enum TFieldId
		{
		/** EGpsReferenceTime */
		EGpsReferenceTime,					//TUint
		/** EUtranGpsReferenceTime */
		EUtranGpsReferenceTime,				//TUtranGpsReferenceTime
		/** ESatelliteInformationArray */
		ESatelliteInformationArray,			//TAcquisitionSatInfo array
        /** ERrlpGpsReferenceTime */
        ERrlpGpsReferenceTime,       //TRrlpGpsReferenceTime
		};
	};

//
//Builder classes

/**
Builder class. Defined using RRC spec.
@publishedPartner
@released
*/
class RUEPositioningGpsAcquisitionAssistanceBuilder : public RDataBuilder<TUEPositioningGpsAcquisitionAssistance, RDataBuilderRootBase>
	{
public:
	IMPORT_C RUEPositioningGpsAcquisitionAssistanceBuilder();
	IMPORT_C void OpenL();
	IMPORT_C static TUint DataSize();
	};

/**
Builder class. Defined using RRC spec.
@publishedPartner
@released
*/	
class RAcquisitionSatInfoArrayBuilder : public RDataBuilderArray<TAcquisitionSatInfo>
	{
public:
	IMPORT_C RAcquisitionSatInfoArrayBuilder();
	};

/**
Builder class. Defined using RRC spec.
@publishedPartner
@released
*/
class RAcquisitionSatInfoBuilder : public RDataBuilder<TAcquisitionSatInfo, RDataBuilderBase>
	{
public:
	IMPORT_C RAcquisitionSatInfoBuilder();
	};

/**
Builder class. Defined using RRC spec.
@publishedPartner
@released
*/
class RExtraDopplerInfoBuilder : public RDataBuilder<TExtraDopplerInfo, RDataBuilderBase>
	{
public:
	IMPORT_C RExtraDopplerInfoBuilder();
	};

/**
Builder class. Defined using RRC spec.
@publishedPartner
@released
*/	
class RTAzimuthAndElevationBuilder : public RDataBuilder<TAzimuthAndElevation, RDataBuilderBase>
	{
public:
	IMPORT_C RTAzimuthAndElevationBuilder();
	};

//
//Reader classes

/**
Reader class. Defined using RRC spec.
@publishedPartner
@released
*/
class RUEPositioningGpsAcquisitionAssistanceReader : public RDataReader<TUEPositioningGpsAcquisitionAssistance, RDataReaderRootBase>
	{
public:
	IMPORT_C RUEPositioningGpsAcquisitionAssistanceReader();
	IMPORT_C void OpenL();
	IMPORT_C static TUint DataSize();	
	};

/**
Reader class. Defined using RRC spec.
@publishedPartner
@released
*/	
class RAcquisitionSatInfoArrayReader : public RDataReaderArray<TAcquisitionSatInfo>
	{
public:
	IMPORT_C RAcquisitionSatInfoArrayReader();
	};

/**
Reader class. Defined using RRC spec.
@publishedPartner
@released
*/
class RAcquisitionSatInfoReader : public RDataReader<TAcquisitionSatInfo, RDataReaderBase>
	{
public:
	IMPORT_C RAcquisitionSatInfoReader();
	};

/**
Reader class. Defined using RRC spec.
@publishedPartner
@released
*/
class RExtraDopplerInfoReader : public RDataReader<TExtraDopplerInfo, RDataReaderBase>
	{
public:
	IMPORT_C RExtraDopplerInfoReader();
	};

/**
Reader class. Defined using RRC spec.
@publishedPartner
@released
*/	
class RTAzimuthAndElevationReader : public RDataReader<TAzimuthAndElevation, RDataReaderBase>
	{
public:
	IMPORT_C RTAzimuthAndElevationReader();
	};


#endif //LBSASSISTANCEAQUISITIONASSISTANCE_H

