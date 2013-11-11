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
#include <lbs/lbsassistancerrlpreferencetime.h>
#include "LbsAssistanceRrlpReferenceTimeLayout.h"
#include "LbsAssistanceInternal.h"



const TLbsAssistanceDataNode KRrlpGpsReferenceTimeLayout[] =
	{
	        {_FOFF(TRrlpGpsReferenceTimeLayout, iBcchCarrier), EUintType},
	        {_FOFF(TRrlpGpsReferenceTimeLayout, iBsic), EUintType},
	        {_FOFF(TRrlpGpsReferenceTimeLayout, iFrameNumber), EUintType},
	        {_FOFF(TRrlpGpsReferenceTimeLayout, iTimeslotsNumber), EUintType},
	        {_FOFF(TRrlpGpsReferenceTimeLayout, iBitsNumber), EUintType},
	};

const TLbsAssistanceDataLayoutMap KRrlpGpsReferenceTimeLayoutInfo = {sizeof(KRrlpGpsReferenceTimeLayout)/sizeof(KRrlpGpsReferenceTimeLayout[0]), &KRrlpGpsReferenceTimeLayout[0]};

/**
RUtranGpsReferenceTimeBuilder constructor
 */
EXPORT_C RRrlpGpsReferenceTimeBuilder::RRrlpGpsReferenceTimeBuilder()
:	RDataBuilder<TRrlpGpsReferenceTime, RDataBuilderBase>(&KRrlpGpsReferenceTimeLayoutInfo)
	{}

/**
RUtranGpsReferenceTimeReader constructor
 */
EXPORT_C RRrlpGpsReferenceTimeReader::RRrlpGpsReferenceTimeReader()
:	RDataReader<TRrlpGpsReferenceTime, RDataReaderBase>(&KRrlpGpsReferenceTimeLayoutInfo)
	{}
