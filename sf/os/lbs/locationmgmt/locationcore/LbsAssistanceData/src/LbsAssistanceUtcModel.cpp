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
#include <lbs/lbsassistanceutcmodel.h>
#include "LbsAssistanceInternal.h"
#include "LbsAssistanceUtcModelLayout.h"

	
const TLbsAssistanceDataNode KUEPositioningGpsUtcModelLayout[] =
	{
		{_FOFF(TUEPositioningGpsUtcModelLayout, iA1), EUintType},
		{_FOFF(TUEPositioningGpsUtcModelLayout, iA0), EUintType},
		{_FOFF(TUEPositioningGpsUtcModelLayout, iTot), EUintType},
		{_FOFF(TUEPositioningGpsUtcModelLayout, iWnt), EUintType},
		{_FOFF(TUEPositioningGpsUtcModelLayout, iDeltatLS), EUintType},
		{_FOFF(TUEPositioningGpsUtcModelLayout, iWnlsf), EUintType},
		{_FOFF(TUEPositioningGpsUtcModelLayout, iDn), EUintType},
		{_FOFF(TUEPositioningGpsUtcModelLayout, iDeltatLSF), EUintType}
	};

const TLbsAssistanceDataLayoutMap KUEPositioningGpsUtcModelLayoutInfo = {sizeof(KUEPositioningGpsUtcModelLayout)/sizeof(KUEPositioningGpsUtcModelLayout[0]), &KUEPositioningGpsUtcModelLayout[0]};


/**
RUEPositioningGpsUtcModelBuilder constructor
 */
EXPORT_C RUEPositioningGpsUtcModelBuilder::RUEPositioningGpsUtcModelBuilder()
:	RDataBuilder<TUEPositioningGpsUtcModel, RDataBuilderRootBase>(&KUEPositioningGpsUtcModelLayoutInfo)
	{
	}

/**
Leaving Open method
 */
EXPORT_C void RUEPositioningGpsUtcModelBuilder::OpenL()
	{
	DoOpenL(sizeof(TObjectAndIndex<TUEPositioningGpsUtcModelLayout>));
	iBody.iIndexPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsUtcModelLayout>*>(iData)->iIndex[0]);
	iBody.iBitOffset = 0;	//We're the root
	iBody.iUtcTimeStampPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsUtcModelLayout>*>(iData)->iUtcTimeStamp);
	}
	
/**
Get the data size that is allocated
for this assistance data type
 */
EXPORT_C TUint RUEPositioningGpsUtcModelBuilder::DataSize()
	{
	return sizeof(TObjectAndIndex<TUEPositioningGpsUtcModelLayout>);
	}

/**
RUEPositioningGpsUtcModelReader constructor
 */
EXPORT_C RUEPositioningGpsUtcModelReader::RUEPositioningGpsUtcModelReader()
:	RDataReader<TUEPositioningGpsUtcModel, RDataReaderRootBase>(&KUEPositioningGpsUtcModelLayoutInfo)
	{
	}

/**
Leaving Open method
 */
EXPORT_C void RUEPositioningGpsUtcModelReader::OpenL()
	{
	DoOpenL(sizeof(TObjectAndIndex<TUEPositioningGpsUtcModelLayout>));
	iBody.iIndexPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsUtcModelLayout>*>(iData)->iIndex[0]);
	iBody.iBitOffset = 0;	//We're the root
	iBody.iUtcTimeStampPtr = &(reinterpret_cast<TObjectAndIndex<TUEPositioningGpsUtcModelLayout>*>(iData)->iUtcTimeStamp);
	}
	
/**
Get the data size that is allocated
for this assistance data type
 */
EXPORT_C TUint RUEPositioningGpsUtcModelReader::DataSize()
	{
	return sizeof(TObjectAndIndex<TUEPositioningGpsUtcModelLayout>);
	}

