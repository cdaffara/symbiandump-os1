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

#include <lbs/lbsassistanceaquisitionassistance.h>
#include "LbsAssistanceAquisitionAssistanceLayout.h"
#include "LbsAssistanceInternal.h"


const TLbsAssistanceDataNode KAzimuthAndElevationLayout[] =
	{
		{_FOFF(TAzimuthAndElevationLayout, iAzimuth), EUintType},
		{_FOFF(TAzimuthAndElevationLayout, iElevation), EUintType}
	};

const TLbsAssistanceDataNode KExtraDopplerInfoLayout[] =
	{
		{_FOFF(TExtraDopplerInfoLayout, iDoppler1stOrder), EIntType},
		{_FOFF(TExtraDopplerInfoLayout, iDopplerUncertainty), EDopplerUncertaintyType}
	};

const TLbsAssistanceDataNode KAcquisitionSatInfoLayout[] =
	{
		{_FOFF(TAcquisitionSatInfoLayout, iSatID), EUintType},
		{_FOFF(TAcquisitionSatInfoLayout, iDoppler0thOrder), EIntType},
		{_FOFF(TAcquisitionSatInfoLayout, iExtraDopplerInfo), EExtraDopplerInfoType},
		{_FOFF(TAcquisitionSatInfoLayout, iCodePhase), EUintType},
		{_FOFF(TAcquisitionSatInfoLayout, iIntegerCodePhase), EUintType},
		{_FOFF(TAcquisitionSatInfoLayout, iGpsBitNumber), EUintType},
		{_FOFF(TAcquisitionSatInfoLayout, iCodePhaseSearchWindow), ECodePhaseSearchWindowType},
		{_FOFF(TAcquisitionSatInfoLayout, iAzimuthAndElevation), EAzimuthAndElevationType},
	};

const TLbsAssistanceDataNode KAcquisitionSatInfoArrayLayout[] =
	{
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[0]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[1]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[2]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[3]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[4]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[5]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[6]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[7]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[8]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[9]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[10]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[11]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[12]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[13]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[14]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[15]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[16]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[17]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[18]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[19]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[20]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[21]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[22]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[23]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[24]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[25]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[26]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[27]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[28]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[29]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[30]), EAcquisitionSatInfoType},
		{_FOFF(TAcquisitionSatInfoArrayLayout, iArray[31]), EAcquisitionSatInfoType},
	};

const TLbsAssistanceDataNode KUEPositioningGpsAcquisitionAssistanceLayout[] =
	{
		{_FOFF(TUEPositioningGpsAcquisitionAssistanceLayout, iGpsReferenceTime), EUintType},
		{_FOFF(TUEPositioningGpsAcquisitionAssistanceLayout, iUtranGpsReferenceTime), EUtranGpsReferenceTimeType},
		{_FOFF(TUEPositioningGpsAcquisitionAssistanceLayout, iSatelliteInformationArray), EAcquisitionSatInfoArrayType},
        {_FOFF(TUEPositioningGpsAcquisitionAssistanceLayout, iRrlpGpsReferenceTime), ERrlpGpsReferenceTimeType},
	};

const TLbsAssistanceDataLayoutMap KAzimuthAndElevationLayoutInfo = {sizeof(KAzimuthAndElevationLayout)/sizeof(KAzimuthAndElevationLayout[0]), &KAzimuthAndElevationLayout[0]};
const TLbsAssistanceDataLayoutMap KExtraDopplerInfoLayoutInfo = {sizeof(KExtraDopplerInfoLayout)/sizeof(KExtraDopplerInfoLayout[0]), &KExtraDopplerInfoLayout[0]};
const TLbsAssistanceDataLayoutMap KAcquisitionSatInfoLayoutInfo = {sizeof(KAcquisitionSatInfoLayout)/sizeof(KAcquisitionSatInfoLayout[0]), &KAcquisitionSatInfoLayout[0]};
const TLbsAssistanceDataLayoutMap KAcquisitionSatInfoArrayLayoutInfo = {sizeof(KAcquisitionSatInfoArrayLayout)/sizeof(KAcquisitionSatInfoArrayLayout[0]), &KAcquisitionSatInfoArrayLayout[0]};
const TLbsAssistanceDataLayoutMap KUEPositioningGpsAcquisitionAssistanceLayoutInfo = {sizeof(KUEPositioningGpsAcquisitionAssistanceLayout)/sizeof(KUEPositioningGpsAcquisitionAssistanceLayout[0]), &KUEPositioningGpsAcquisitionAssistanceLayout[0]};


/**
RUEPositioningGpsAcquisitionAssistanceBuilder constructor
@publishedPartner
@released
 */
EXPORT_C RUEPositioningGpsAcquisitionAssistanceBuilder::RUEPositioningGpsAcquisitionAssistanceBuilder()
:	RDataBuilder<TUEPositioningGpsAcquisitionAssistance, RDataBuilderRootBase>(&KUEPositioningGpsAcquisitionAssistanceLayoutInfo)
	{
	}

/**
Leaving Open method
 */
EXPORT_C void RUEPositioningGpsAcquisitionAssistanceBuilder::OpenL()
	{
	DoOpenL(sizeof(TObjectAndIndex<TUEPositioningGpsAcquisitionAssistanceLayout>));
	iBody.iIndexPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsAcquisitionAssistanceLayout>*>(iData)->iIndex[0]);
	iBody.iBitOffset = 0;	//We're the root;
	iBody.iUtcTimeStampPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsAcquisitionAssistanceLayout>*>(iData)->iUtcTimeStamp);
	}
	
/**
Get the data size that is allocated
for this assistance data type
@released
 */
EXPORT_C TUint RUEPositioningGpsAcquisitionAssistanceBuilder::DataSize()
	{
	return sizeof(TObjectAndIndex<TUEPositioningGpsAcquisitionAssistanceLayout>);
	}


/**
RAcquisitionSatInfoArrayBuilder constructor
 */		
EXPORT_C RAcquisitionSatInfoArrayBuilder::RAcquisitionSatInfoArrayBuilder()
:	RDataBuilderArray<TAcquisitionSatInfo>(&KAcquisitionSatInfoArrayLayoutInfo)
	{}

/**
RAcquisitionSatInfoBuilder constructor
 */
EXPORT_C RAcquisitionSatInfoBuilder::RAcquisitionSatInfoBuilder()
:	RDataBuilder<TAcquisitionSatInfo, RDataBuilderBase>(&KAcquisitionSatInfoLayoutInfo)
	{}

/**
RExtraDopplerInfoBuilder constructor
 */
EXPORT_C RExtraDopplerInfoBuilder::RExtraDopplerInfoBuilder()
:	RDataBuilder<TExtraDopplerInfo, RDataBuilderBase>(&KExtraDopplerInfoLayoutInfo)
	{}

/**
RTAzimuthAndElevationBuilder constructor
 */	
EXPORT_C RTAzimuthAndElevationBuilder::RTAzimuthAndElevationBuilder()
:	RDataBuilder<TAzimuthAndElevation, RDataBuilderBase>(&KAzimuthAndElevationLayoutInfo)
	{}

/**
RUEPositioningGpsAcquisitionAssistanceReader constructor
 */
EXPORT_C RUEPositioningGpsAcquisitionAssistanceReader::RUEPositioningGpsAcquisitionAssistanceReader()
:	RDataReader<TUEPositioningGpsAcquisitionAssistance, RDataReaderRootBase>(&KUEPositioningGpsAcquisitionAssistanceLayoutInfo)
	{
	}

/**
Leaving Open method
 */
EXPORT_C void RUEPositioningGpsAcquisitionAssistanceReader::OpenL()
	{
	DoOpenL(sizeof(TObjectAndIndex<TUEPositioningGpsAcquisitionAssistanceLayout>));
	iBody.iIndexPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsAcquisitionAssistanceLayout>*>(iData)->iIndex[0]);
	iBody.iBitOffset = 0;	//We're the root
	iBody.iUtcTimeStampPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsAcquisitionAssistanceLayout>*>(iData)->iUtcTimeStamp);
	}

/**
Get the data size that is allocated
for this assistance data type
 */
EXPORT_C TUint RUEPositioningGpsAcquisitionAssistanceReader::DataSize()
	{
	return sizeof(TObjectAndIndex<TUEPositioningGpsAcquisitionAssistanceLayout>);
	}


/**
RAcquisitionSatInfoArrayReader constructor
 */	
EXPORT_C RAcquisitionSatInfoArrayReader::RAcquisitionSatInfoArrayReader()
:	RDataReaderArray<TAcquisitionSatInfo>(&KAcquisitionSatInfoArrayLayoutInfo)
	{}

/**
RAcquisitionSatInfoReader constructor
 */
EXPORT_C RAcquisitionSatInfoReader::RAcquisitionSatInfoReader()
:	RDataReader<TAcquisitionSatInfo, RDataReaderBase>(&KAcquisitionSatInfoLayoutInfo)
	{}

/**
RExtraDopplerInfoReader constructor
 */
EXPORT_C RExtraDopplerInfoReader::RExtraDopplerInfoReader()
:	RDataReader<TExtraDopplerInfo, RDataReaderBase>(&KExtraDopplerInfoLayoutInfo)
	{}

/**
RTAzimuthAndElevationReader constructor
 */
EXPORT_C RTAzimuthAndElevationReader::RTAzimuthAndElevationReader()
:	RDataReader<TAzimuthAndElevation, RDataReaderBase>(&KAzimuthAndElevationLayoutInfo)
	{}
