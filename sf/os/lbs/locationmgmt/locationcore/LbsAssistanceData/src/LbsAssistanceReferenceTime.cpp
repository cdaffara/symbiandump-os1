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

#include <lbs/lbsassistancereferencetime.h>
#include "LbsAssistanceInternal.h"
#include "LbsAssistanceReferenceTimeLayout.h"


const TLbsAssistanceDataNode KGpsTowAssistLayout[] =
	{
		{_FOFF(TGpsTowAssistLayout, iSatID), EUintType},
		{_FOFF(TGpsTowAssistLayout, iTlmMessage), EUintType},
		{_FOFF(TGpsTowAssistLayout, iAlert), EIntType},
		{_FOFF(TGpsTowAssistLayout, iAntiSpoof), EIntType},
		{_FOFF(TGpsTowAssistLayout, iTlmReserved), EUintType},
	};

const TLbsAssistanceDataNode KGpsTowAssistArrayLayout[] =
	{
		{_FOFF(TGpsTowAssistArrayLayout, iArray[0]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[1]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[2]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[3]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[4]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[5]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[6]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[7]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[8]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[9]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[10]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[11]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[12]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[13]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[14]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[15]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[16]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[17]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[18]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[19]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[20]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[21]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[22]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[23]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[24]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[25]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[26]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[27]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[28]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[29]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[30]), EGpsTowAssistType},
		{_FOFF(TGpsTowAssistArrayLayout, iArray[31]), EGpsTowAssistType},
	};

const TLbsAssistanceDataNode KUEPositioningGpsReferenceTimeLayout[] =
	{
		{_FOFF(TUEPositioningGpsReferenceTimeLayout, iGpsWeek), EUintType},
		{_FOFF(TUEPositioningGpsReferenceTimeLayout, iGpsTow1Msec), EUintType},
		{_FOFF(TUEPositioningGpsReferenceTimeLayout, iUtranGpsReferenceTime), EUtranGpsReferenceTimeType},
		{_FOFF(TUEPositioningGpsReferenceTimeLayout, iSfnTowUncertainty), ESfnTowUncertaintyType},
		{_FOFF(TUEPositioningGpsReferenceTimeLayout, iUtranGpsDriftRate), EUtranGpsDriftRateType},
		{_FOFF(TUEPositioningGpsReferenceTimeLayout, iGpsTowAssistArray), EGpsTowAssistArrayType},	
		{_FOFF(TUEPositioningGpsReferenceTimeLayout, iRrlpGpsReferenceTime), ERrlpGpsReferenceTimeType},
	};

const TLbsAssistanceDataLayoutMap KGpsTowAssistLayoutInfo = {sizeof(KGpsTowAssistLayout)/sizeof(KGpsTowAssistLayout[0]), &KGpsTowAssistLayout[0]};
const TLbsAssistanceDataLayoutMap KGpsTowAssistArrayLayoutInfo = {sizeof(KGpsTowAssistArrayLayout)/sizeof(KGpsTowAssistArrayLayout[0]), &KGpsTowAssistArrayLayout[0]};
const TLbsAssistanceDataLayoutMap KUEPositioningGpsReferenceTimeLayoutInfo = {sizeof(KUEPositioningGpsReferenceTimeLayout)/sizeof(KUEPositioningGpsReferenceTimeLayout[0]), &KUEPositioningGpsReferenceTimeLayout[0]};

/**
RUEPositioningGpsReferenceTimeBuilder constructor
 */
EXPORT_C RUEPositioningGpsReferenceTimeBuilder::RUEPositioningGpsReferenceTimeBuilder()
:	RDataBuilder<TUEPositioningGpsReferenceTime, RDataBuilderRootBase>(&KUEPositioningGpsReferenceTimeLayoutInfo)
	{}

/**
Leaving Open method
 */
EXPORT_C void RUEPositioningGpsReferenceTimeBuilder::OpenL()
	{
	DoOpenL(sizeof(TObjectAndIndex<TUEPositioningGpsReferenceTimeLayout>));
	iBody.iIndexPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsReferenceTimeLayout>*>(iData)->iIndex[0]);
	iBody.iBitOffset = 0;	//We're the root
	iBody.iUtcTimeStampPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsReferenceTimeLayout>*>(iData)->iUtcTimeStamp);
	}

/**
Get the data size that is allocated
for this assistance data type
 */
EXPORT_C TUint RUEPositioningGpsReferenceTimeBuilder::DataSize()
	{
	return sizeof(TObjectAndIndex<TUEPositioningGpsReferenceTimeLayout>);
	}

/**
RGpsTowAssistArrayBuilder constructor
 */
EXPORT_C RGpsTowAssistArrayBuilder::RGpsTowAssistArrayBuilder()
:	RDataBuilderArray<TGpsTowAssist>(&KGpsTowAssistArrayLayoutInfo)
	{}

/**
RGpsTowAssistBuilder constructor
 */
EXPORT_C RGpsTowAssistBuilder::RGpsTowAssistBuilder()
:	RDataBuilder<TGpsTowAssist, RDataBuilderBase>(&KGpsTowAssistLayoutInfo)
	{}

/**
RUEPositioningGpsReferenceTimeReader constructor
 */
EXPORT_C RUEPositioningGpsReferenceTimeReader::RUEPositioningGpsReferenceTimeReader()
:	RDataReader<TUEPositioningGpsReferenceTime, RDataReaderRootBase>(&KUEPositioningGpsReferenceTimeLayoutInfo)
	{}

/**
Leaving Open method
 */
EXPORT_C void RUEPositioningGpsReferenceTimeReader::OpenL()
	{
	DoOpenL(sizeof(TObjectAndIndex<TUEPositioningGpsReferenceTimeLayout>));
	iBody.iIndexPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsReferenceTimeLayout>*>(iData)->iIndex[0]);
	iBody.iBitOffset = 0;	//We're the root
	iBody.iUtcTimeStampPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsReferenceTimeLayout>*>(iData)->iUtcTimeStamp);
	}

/**
Get the data size that is allocated
for this assistance data type
 */
EXPORT_C TUint RUEPositioningGpsReferenceTimeReader::DataSize()
	{
	return sizeof(TObjectAndIndex<TUEPositioningGpsReferenceTimeLayout>);
	}

/**
RGpsTowAssistArrayReader constructor
 */
EXPORT_C RGpsTowAssistArrayReader::RGpsTowAssistArrayReader()
:	RDataReaderArray<TGpsTowAssist>(&KGpsTowAssistArrayLayoutInfo)
	{}

/**
RGpsTowAssistReader constructor
 */
EXPORT_C RGpsTowAssistReader::RGpsTowAssistReader()
:	RDataReader<TGpsTowAssist, RDataReaderBase>(&KGpsTowAssistLayoutInfo)
	{}

