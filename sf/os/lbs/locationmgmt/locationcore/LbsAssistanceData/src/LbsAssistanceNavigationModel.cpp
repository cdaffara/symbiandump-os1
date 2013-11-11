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

#include <lbs/lbsassistancenavigationmodel.h>
#include "LbsAssistanceNavigationModelLayout.h"
#include "LbsAssistanceInternal.h"


const TLbsAssistanceDataNode KEphemerisParameterLayout[] =
	{
		{_FOFF(TEphemerisParameterLayout, iCodeOnL2), EUintType},
		{_FOFF(TEphemerisParameterLayout, iUraIndex), EUintType},
		{_FOFF(TEphemerisParameterLayout, iSatHealth), EUintType},
		{_FOFF(TEphemerisParameterLayout, iIodc), EUintType},
		{_FOFF(TEphemerisParameterLayout, iL2Pflag), EUintType},
		{_FOFF(TEphemerisParameterLayout, iTGD), EUintType},
		{_FOFF(TEphemerisParameterLayout, iToc), EUintType},
		{_FOFF(TEphemerisParameterLayout, iAf2), EUintType},
		{_FOFF(TEphemerisParameterLayout, iAf1), EUintType},
		{_FOFF(TEphemerisParameterLayout, iAf0), EUintType},
		{_FOFF(TEphemerisParameterLayout, iCrs), EUintType},
		{_FOFF(TEphemerisParameterLayout, iDeltaN), EUintType},
		{_FOFF(TEphemerisParameterLayout, iM0), EUintType},
		{_FOFF(TEphemerisParameterLayout, iCuc), EUintType},
		{_FOFF(TEphemerisParameterLayout, iE), EUintType},
		{_FOFF(TEphemerisParameterLayout, iCus), EUintType},
		{_FOFF(TEphemerisParameterLayout, iASqrt), EUintType},
		{_FOFF(TEphemerisParameterLayout, iToe), EUintType},
		{_FOFF(TEphemerisParameterLayout, iFitInterval), EUintType},
		{_FOFF(TEphemerisParameterLayout, iAodo), EUintType},
		{_FOFF(TEphemerisParameterLayout, iCic), EUintType},
		{_FOFF(TEphemerisParameterLayout, iOmega0), EUintType},
		{_FOFF(TEphemerisParameterLayout, iCis), EUintType},
		{_FOFF(TEphemerisParameterLayout, iI0), EUintType},
		{_FOFF(TEphemerisParameterLayout, iCrc), EUintType},
		{_FOFF(TEphemerisParameterLayout, iOmega), EUintType},
		{_FOFF(TEphemerisParameterLayout, iOmegaDot), EUintType},
		{_FOFF(TEphemerisParameterLayout, iIDot), EUintType},
		{_FOFF(TEphemerisParameterLayout, iSf1Revd), ESubFrame1ReservedArrayType},
	};

const TLbsAssistanceDataNode KSubFrame1ReservedArrayLayout[] =
    {
        {_FOFF(TSubFrame1ReservedArrayLayout, iArray[0]), EUintType},
        {_FOFF(TSubFrame1ReservedArrayLayout, iArray[1]), EUintType},
        {_FOFF(TSubFrame1ReservedArrayLayout, iArray[2]), EUintType},
        {_FOFF(TSubFrame1ReservedArrayLayout, iArray[3]), EUintType},
    };
	
const TLbsAssistanceDataNode KNavigationModelSatInfoLayout[] =
	{
		{_FOFF(TNavigationModelSatInfoLayout, iSatId), EUintType},
		{_FOFF(TNavigationModelSatInfoLayout, iSatelliteStatus), ESatelliteStatusType},
		{_FOFF(TNavigationModelSatInfoLayout, iEphemerisParameter), EEphemerisParameterType},
	};

const TLbsAssistanceDataNode KNavigationModelSatInfoArrayLayout[] =
	{
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[0]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[1]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[2]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[3]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[4]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[5]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[6]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[7]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[8]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[9]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[10]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[11]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[12]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[13]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[14]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[15]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[16]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[17]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[18]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[19]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[20]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[21]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[22]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[23]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[24]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[25]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[26]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[27]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[28]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[29]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[30]), ENavigationModelSatInfoType},
		{_FOFF(TNavigationModelSatInfoArrayLayout, iArray[31]), ENavigationModelSatInfoType}
	};

const TLbsAssistanceDataNode KUEPositioningGpsNavigationModelLayout[] =
	{
		{_FOFF(TUEPositioningGpsNavigationModelLayout, iNavigationModelSatelliteInfoArray), ENavigationModelSatInfoArrayType}
	};

	
const TLbsAssistanceDataLayoutMap KEphemerisParameterLayoutInfo = {sizeof(KEphemerisParameterLayout)/sizeof(KEphemerisParameterLayout[0]), &KEphemerisParameterLayout[0]};
const TLbsAssistanceDataLayoutMap KSubFrame1ReservedArrayLayoutInfo = {sizeof(KSubFrame1ReservedArrayLayout)/sizeof(KSubFrame1ReservedArrayLayout[0]), &KSubFrame1ReservedArrayLayout[0]};
const TLbsAssistanceDataLayoutMap KNavigationModelSatInfoLayoutInfo = {sizeof(KNavigationModelSatInfoLayout)/sizeof(KNavigationModelSatInfoLayout[0]), &KNavigationModelSatInfoLayout[0]};
const TLbsAssistanceDataLayoutMap KNavigationModelSatInfoArrayLayoutInfo = {sizeof(KNavigationModelSatInfoArrayLayout)/sizeof(KNavigationModelSatInfoArrayLayout[0]), &KNavigationModelSatInfoArrayLayout[0]};
const TLbsAssistanceDataLayoutMap KUEPositioningGpsNavigationModelLayoutInfo = {sizeof(KUEPositioningGpsNavigationModelLayout)/sizeof(KUEPositioningGpsNavigationModelLayout[0]), &KUEPositioningGpsNavigationModelLayout[0]};



/**
RUEPositioningGpsNavigationModelBuilder constructor
 */
EXPORT_C RUEPositioningGpsNavigationModelBuilder::RUEPositioningGpsNavigationModelBuilder()
:	RDataBuilder<TUEPositioningGpsNavigationModel, RDataBuilderRootBase>(&KUEPositioningGpsNavigationModelLayoutInfo)
	{}

/**
Leaving Open method
 */
EXPORT_C void RUEPositioningGpsNavigationModelBuilder::OpenL()
	{
	DoOpenL(sizeof(TObjectAndIndex<TUEPositioningGpsNavigationModelLayout>));
	iBody.iIndexPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsNavigationModelLayout>*>(iData)->iIndex[0]);
	iBody.iBitOffset = 0;	//We're the root
	iBody.iUtcTimeStampPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsNavigationModelLayout>*>(iData)->iUtcTimeStamp);
	}

/**
Get the data size that is allocated
for this assistance data type
 */
EXPORT_C TUint RUEPositioningGpsNavigationModelBuilder::DataSize()
	{
	return sizeof(TObjectAndIndex<TUEPositioningGpsNavigationModelLayout>);
	}

/**
RNavigationModelSatInfoArrayBuilderconstructor
 */
EXPORT_C RNavigationModelSatInfoArrayBuilder::RNavigationModelSatInfoArrayBuilder()
:	RDataBuilderArray<TNavigationModelSatInfo>(&KNavigationModelSatInfoArrayLayoutInfo)
	{}

/**
RNavigationModelSatInfoBuilder constructor
 */
EXPORT_C RNavigationModelSatInfoBuilder::RNavigationModelSatInfoBuilder()
:	RDataBuilder<TNavigationModelSatInfo, RDataBuilderBase>(&KNavigationModelSatInfoLayoutInfo)
	{}

/**
REphemerisParameterBuilder constructor
 */
EXPORT_C REphemerisParameterBuilder::REphemerisParameterBuilder()
:	RDataBuilder<TEphemerisParameter, RDataBuilderBase>(&KEphemerisParameterLayoutInfo)
	{}

EXPORT_C RSubFrame1ReservedArrayBuilder::RSubFrame1ReservedArrayBuilder()
:   RDataBuilderArray<TUint>(&KSubFrame1ReservedArrayLayoutInfo)
    {}
/**
RUEPositioningGpsNavigationModelReader constructor
 */
EXPORT_C RUEPositioningGpsNavigationModelReader::RUEPositioningGpsNavigationModelReader()
:	RDataReader<TUEPositioningGpsNavigationModel, RDataReaderRootBase>(&KUEPositioningGpsNavigationModelLayoutInfo)
	{}

/**
Leaving Open method
 */
EXPORT_C void RUEPositioningGpsNavigationModelReader::OpenL()
	{
	DoOpenL(sizeof(TObjectAndIndex<TUEPositioningGpsNavigationModelLayout>));
	iBody.iIndexPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsNavigationModelLayout>*>(iData)->iIndex[0]);
	iBody.iBitOffset = 0;	//We're the root
	iBody.iUtcTimeStampPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsNavigationModelLayout>*>(iData)->iUtcTimeStamp);
	}

/**
Get the data size that is allocated
for this assistance data type
 */
EXPORT_C TUint RUEPositioningGpsNavigationModelReader::DataSize()
	{
	return sizeof(TObjectAndIndex<TUEPositioningGpsNavigationModelLayout>);
	}


/**
RNavigationModelSatInfoArrayReader constructor
 */
EXPORT_C RNavigationModelSatInfoArrayReader::RNavigationModelSatInfoArrayReader()
:	RDataReaderArray<TNavigationModelSatInfo>(&KNavigationModelSatInfoArrayLayoutInfo)
	{}

/**
RNavigationModelSatInfoReader constructor
 */
EXPORT_C RNavigationModelSatInfoReader::RNavigationModelSatInfoReader()
:	RDataReader<TNavigationModelSatInfo, RDataReaderBase>(&KNavigationModelSatInfoLayoutInfo)
	{}

/**
REphemerisParameterReader constructor
 */
EXPORT_C REphemerisParameterReader::REphemerisParameterReader()
:	RDataReader<TEphemerisParameter, RDataReaderBase>(&KEphemerisParameterLayoutInfo)
	{}

EXPORT_C RSubFrame1ReservedArrayReader::RSubFrame1ReservedArrayReader()
:   RDataReaderArray<TUint>(&KSubFrame1ReservedArrayLayoutInfo)
    {}
