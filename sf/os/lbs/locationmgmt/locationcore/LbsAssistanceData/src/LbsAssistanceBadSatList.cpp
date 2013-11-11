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

#include <lbs/lbsassistancebadsatlist.h>
#include "LbsAssistanceInternal.h"
#include "LbsAssistanceBadSatListLayout.h"

const TLbsAssistanceDataNode KBadSatListLayout[] =
	{
		{_FOFF(TBadSatListLayout, iBadSatIdArray), EBadSatListArrayType}
	};

const TLbsAssistanceDataNode KBadSatListArrayLayout[] =
	{
		{_FOFF(TBadSatListArrayLayout, iArray[0]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[1]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[2]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[3]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[4]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[5]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[6]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[7]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[8]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[9]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[10]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[11]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[12]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[13]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[14]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[15]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[16]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[17]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[18]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[19]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[20]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[21]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[22]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[23]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[24]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[25]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[26]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[27]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[28]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[29]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[30]), EUintType},
		{_FOFF(TBadSatListArrayLayout, iArray[31]), EUintType}
	};

const TLbsAssistanceDataLayoutMap KBadSatListLayoutInfo = {sizeof(KBadSatListLayout)/sizeof(KBadSatListLayout[0]), &KBadSatListLayout[0]};
const TLbsAssistanceDataLayoutMap KBadSatListArrayLayoutInfo = {sizeof(KBadSatListArrayLayout)/sizeof(KBadSatListArrayLayout[0]), &KBadSatListArrayLayout[0]};

/**
RBadSatArrayBuilder constructor
 */
EXPORT_C RBadSatArrayBuilder::RBadSatArrayBuilder()
:	RDataBuilderArray<TUint>(&KBadSatListArrayLayoutInfo)
	{}

/**
RBadSatListBuilder constructor
 */
EXPORT_C RBadSatListBuilder::RBadSatListBuilder()
:	RDataBuilder<TBadSatList, RDataBuilderRootBase>(&KBadSatListLayoutInfo)
	{
	}

/**
Leaving Open method
 */
EXPORT_C void RBadSatListBuilder::OpenL()
	{
	DoOpenL(sizeof(TObjectAndIndex<TBadSatListLayout>));
	iBody.iIndexPtr = &(reinterpret_cast<TObjectAndIndex<TBadSatListLayout>*>(iData)->iIndex[0]);
	iBody.iBitOffset = 0;	//We're the root
	iBody.iUtcTimeStampPtr = &(reinterpret_cast<TObjectAndIndex<TBadSatListLayout>*>(iData)->iUtcTimeStamp);
	}
	
/**
Get the data size that is allocated
for this assistance data type
 */
EXPORT_C TUint RBadSatListBuilder::DataSize()
	{
	return sizeof(TObjectAndIndex<TBadSatListLayout>);
	}
	
/**
RBadSatArrayReader constructor
 */
EXPORT_C RBadSatArrayReader::RBadSatArrayReader()
:	RDataReaderArray<TUint>(&KBadSatListArrayLayoutInfo)
	{}

/**
RBadSatListReader constructor
 */
EXPORT_C RBadSatListReader::RBadSatListReader()
:	RDataReader<TBadSatList, RDataReaderRootBase>(&KBadSatListLayoutInfo)
	{}

/**
RBadSatListReader constructor
 */
EXPORT_C void RBadSatListReader::OpenL()
	{
	DoOpenL(sizeof(TObjectAndIndex<TBadSatListLayout>));
	iBody.iIndexPtr = &(reinterpret_cast<TObjectAndIndex<TBadSatListLayout>*>(iData)->iIndex[0]);
	iBody.iBitOffset = 0;	//We're the root
	iBody.iUtcTimeStampPtr = &(reinterpret_cast<TObjectAndIndex<TBadSatListLayout>*>(iData)->iUtcTimeStamp);
	}

/**
Get the data size that is allocated
for this assistance data type
 */
EXPORT_C TUint RBadSatListReader::DataSize()
	{
	return sizeof(TObjectAndIndex<TBadSatListLayout>);
	}

