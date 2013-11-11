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



#include "LbsAssistanceAlmanacLayout.h"
#include "LbsAssistanceInternal.h"
#include <lbs/lbsassistancealmanac.h>

const TLbsAssistanceDataNode KUEPositioningGpsAlmanacLayout[] =
	{
		{_FOFF(TUEPositioningGpsAlmanacLayout, iWnA), EUintType},
		{_FOFF(TUEPositioningGpsAlmanacLayout, iAlmanacInfoSatArray), EAlmanacSatInfoArrayType},
		{_FOFF(TUEPositioningGpsAlmanacLayout, iSvGlobalHealth), EDes8Type}
	};
	
const TLbsAssistanceDataNode KAlmanacSatInfoLayout[] =
	{
		{_FOFF(TAlmanacSatInfoLayout, iDataID), EUintType},
		{_FOFF(TAlmanacSatInfoLayout, iSatID), EUintType},
		{_FOFF(TAlmanacSatInfoLayout, iE), EUintType},
		{_FOFF(TAlmanacSatInfoLayout, iToa), EUintType},
		{_FOFF(TAlmanacSatInfoLayout, iDeltaI), EUintType},
		{_FOFF(TAlmanacSatInfoLayout, iOmegaDot), EUintType},
		{_FOFF(TAlmanacSatInfoLayout, iSatHealth), EUintType},
		{_FOFF(TAlmanacSatInfoLayout, iASqrt), EUintType},
		{_FOFF(TAlmanacSatInfoLayout, iOmega0), EUintType},
		{_FOFF(TAlmanacSatInfoLayout, iM0), EUintType},
		{_FOFF(TAlmanacSatInfoLayout, iOmega), EUintType},
		{_FOFF(TAlmanacSatInfoLayout, iAf0), EUintType},
		{_FOFF(TAlmanacSatInfoLayout, iAf1), EUintType}
	};

//To do - find a nicer way to express this...

const TLbsAssistanceDataNode KAlmanacSatInfoArrayLayout[] =
	{
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[0]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[1]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[2]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[3]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[4]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[5]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[6]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[7]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[8]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[9]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[10]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[11]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[12]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[13]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[14]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[15]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[16]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[17]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[18]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[19]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[20]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[21]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[22]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[23]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[24]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[25]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[26]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[27]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[28]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[29]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[30]), EAlmanacSatInfoType},
		{_FOFF(TAlmanacSatInfoArrayLayout, iArray[31]), EAlmanacSatInfoType}
	};

const TLbsAssistanceDataLayoutMap KUEPositioningGpsAlmanacLayoutInfo = {sizeof(KUEPositioningGpsAlmanacLayout)/sizeof(KUEPositioningGpsAlmanacLayout[0]), &KUEPositioningGpsAlmanacLayout[0]};
const TLbsAssistanceDataLayoutMap KAlmanacSatInfoArrayLayoutInfo = {sizeof(KAlmanacSatInfoArrayLayout)/sizeof(KAlmanacSatInfoArrayLayout[0]), &KAlmanacSatInfoArrayLayout[0]};
const TLbsAssistanceDataLayoutMap KAlmanacSatInfoLayoutInfo = {sizeof(KAlmanacSatInfoLayout)/sizeof(KAlmanacSatInfoLayout[0]), &KAlmanacSatInfoLayout[0]};

/**
RAlmanacSatInfoBuilder constructor
 */
EXPORT_C RAlmanacSatInfoBuilder::RAlmanacSatInfoBuilder()
:	RDataBuilder<TAlmanacSatInfo, RDataBuilderBase>(&KAlmanacSatInfoLayoutInfo)
	{
	}

/**
RUEPositioningGpsAlmanacBuilder constructor
 */
EXPORT_C RUEPositioningGpsAlmanacBuilder::RUEPositioningGpsAlmanacBuilder()
:	RDataBuilder<TUEPositioningGpsAlmanac, RDataBuilderRootBase>(&KUEPositioningGpsAlmanacLayoutInfo)
	{
	}

/**
Leaving Open method
 */
EXPORT_C void RUEPositioningGpsAlmanacBuilder::OpenL()
	{
	DoOpenL(sizeof(TObjectAndIndex<TUEPositioningGpsAlmanacLayout>));
	iBody.iIndexPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsAlmanacLayout>*>(iData)->iIndex[0]);
	iBody.iBitOffset = 0;	//We're the root
	iBody.iUtcTimeStampPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsAlmanacLayout>*>(iData)->iUtcTimeStamp);
	}

/**
Get the data size that is allocated
for this assistance data type
 */
EXPORT_C TUint RUEPositioningGpsAlmanacBuilder::DataSize()
	{
	return sizeof(TObjectAndIndex<TUEPositioningGpsAlmanacLayout>);
	}

/**
RAlmanacSatInfoArrayBuilder constructor
 */
EXPORT_C RAlmanacSatInfoArrayBuilder::RAlmanacSatInfoArrayBuilder()
:	RDataBuilderArray<TAlmanacSatInfo>(&KAlmanacSatInfoArrayLayoutInfo)
	{
	}
	
///////////////////////////////////////////

/**
RAlmanacSatInfoReader constructor
 */
EXPORT_C RAlmanacSatInfoReader::RAlmanacSatInfoReader()
:	RDataReader<TAlmanacSatInfo, RDataReaderBase> (&KAlmanacSatInfoLayoutInfo)
	{}

/**
RAlmanacSatInfoArrayReader constructor
 */
EXPORT_C RAlmanacSatInfoArrayReader::RAlmanacSatInfoArrayReader()
:	RDataReaderArray<TAlmanacSatInfo>(&KAlmanacSatInfoArrayLayoutInfo)
	{}

/**
RUEPositioningGpsAlmanacReader constructor
 */
EXPORT_C RUEPositioningGpsAlmanacReader::RUEPositioningGpsAlmanacReader()
:	RDataReader<TUEPositioningGpsAlmanac, RDataReaderRootBase>(&KUEPositioningGpsAlmanacLayoutInfo)
	{}

/**
Leaving Open method
 */
EXPORT_C void RUEPositioningGpsAlmanacReader::OpenL()
	{
	DoOpenL(sizeof(TObjectAndIndex<TUEPositioningGpsAlmanacLayout>));
	iBody.iIndexPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsAlmanacLayout>*>(iData)->iIndex[0]);
	iBody.iBitOffset = 0;	//We're the root
	iBody.iUtcTimeStampPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsAlmanacLayout>*>(iData)->iUtcTimeStamp);
	}
	
/**
Get the data size that is allocated
for this assistance data type
 */
EXPORT_C TUint RUEPositioningGpsAlmanacReader::DataSize()
	{
	return sizeof(TObjectAndIndex<TUEPositioningGpsAlmanacLayout>);
	}

