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
#include <lbs/lbsassistanceionosphericmodel.h>
#include "LbsAssistanceInternal.h"
#include "LbsAssistanceIonosphericModelLayout.h"

const TLbsAssistanceDataNode KUEPositioningGpsIonosphericModelLayout[] =
	{
		{_FOFF(TUEPositioningGpsIonosphericModelLayout, iAlfa0), EUintType},
		{_FOFF(TUEPositioningGpsIonosphericModelLayout, iAlfa1), EUintType},
		{_FOFF(TUEPositioningGpsIonosphericModelLayout, iAlfa2), EUintType},
		{_FOFF(TUEPositioningGpsIonosphericModelLayout, iAlfa3), EUintType},
		{_FOFF(TUEPositioningGpsIonosphericModelLayout, iBeta0), EUintType},
		{_FOFF(TUEPositioningGpsIonosphericModelLayout, iBeta1), EUintType},
		{_FOFF(TUEPositioningGpsIonosphericModelLayout, iBeta2), EUintType},
		{_FOFF(TUEPositioningGpsIonosphericModelLayout, iBeta3), EUintType}

	};

const TLbsAssistanceDataLayoutMap KUEPositioningGpsIonosphericModelLayoutInfo = {sizeof(KUEPositioningGpsIonosphericModelLayout)/sizeof(KUEPositioningGpsIonosphericModelLayout[0]), &KUEPositioningGpsIonosphericModelLayout[0]};


/**
RUEPositioningGpsIonosphericModelReader constructor
 */	
EXPORT_C RUEPositioningGpsIonosphericModelReader::RUEPositioningGpsIonosphericModelReader()
:	RDataReader<TUEPositioningGpsIonosphericModel, RDataReaderRootBase>(&KUEPositioningGpsIonosphericModelLayoutInfo)
	{}

/**
Leaving Open method
 */
EXPORT_C void RUEPositioningGpsIonosphericModelReader::OpenL()
	{
	DoOpenL(sizeof(TObjectAndIndex<TUEPositioningGpsIonosphericModelLayout>));
	iBody.iIndexPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsIonosphericModelLayout>*>(iData)->iIndex[0]);
	iBody.iBitOffset = 0;	//We're the root
	iBody.iUtcTimeStampPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsIonosphericModelLayout>*>(iData)->iUtcTimeStamp);
	}

/**
Get the data size that is allocated
for this assistance data type
 */
EXPORT_C TUint RUEPositioningGpsIonosphericModelReader::DataSize()
	{
	return sizeof(TObjectAndIndex<TUEPositioningGpsIonosphericModelLayout>);
	}


/**
RUEPositioningGpsIonosphericModelBuilder constructor
 */
EXPORT_C RUEPositioningGpsIonosphericModelBuilder::RUEPositioningGpsIonosphericModelBuilder()
:	RDataBuilder<TUEPositioningGpsIonosphericModel, RDataBuilderRootBase>(&KUEPositioningGpsIonosphericModelLayoutInfo)
	{}

/**
Leaving Open method
 */
EXPORT_C void RUEPositioningGpsIonosphericModelBuilder::OpenL()
	{
	DoOpenL(sizeof(TObjectAndIndex<TUEPositioningGpsIonosphericModelLayout>));
	iBody.iIndexPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsIonosphericModelLayout>*>(iData)->iIndex[0]);
	iBody.iBitOffset = 0;	//We're the root
	iBody.iUtcTimeStampPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsIonosphericModelLayout>*>(iData)->iUtcTimeStamp);
	}

/**
Get the data size that is allocated
for this assistance data type
 */
EXPORT_C TUint RUEPositioningGpsIonosphericModelBuilder::DataSize()
	{
	return sizeof(TObjectAndIndex<TUEPositioningGpsIonosphericModelLayout>);
	}
