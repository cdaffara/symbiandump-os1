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
#include <lbs/lbsassistanceutranreferencetime.h>
#include "LbsAssistanceUtranReferenceTimeLayout.h"
#include "LbsAssistanceInternal.h"



const TLbsAssistanceDataNode KPrimaryCpichInfoLayout[] =
	{
		{_FOFF(TPrimaryCpichInfoLayout, iPrimaryScramblingCode), EUintType}
	};

const TLbsAssistanceDataNode KUtranGpsReferenceTimeLayout[] =
	{
		{_FOFF(TUtranGpsReferenceTimeLayout, iUtranGPSTimingOfCell_MsPart), EUintType},
		{_FOFF(TUtranGpsReferenceTimeLayout, iUtranGPSTimingOfCell_LsPart), EUintType},
		{_FOFF(TUtranGpsReferenceTimeLayout, iModeSpecificInfo_Fdd_ReferenceIdentity), EPrimaryCpichInfoLayout},
		{_FOFF(TUtranGpsReferenceTimeLayout, iModeSpecificInfo_Tdd_ReferenceIdentity), EUintType},
		{_FOFF(TUtranGpsReferenceTimeLayout, iSfn), EUintType}
	};

const TLbsAssistanceDataLayoutMap KPrimaryCpichInfoLayoutInfo = {sizeof(KPrimaryCpichInfoLayout)/sizeof(KPrimaryCpichInfoLayout[0]), &KPrimaryCpichInfoLayout[0]};
const TLbsAssistanceDataLayoutMap KUtranGpsReferenceTimeLayoutInfo = {sizeof(KUtranGpsReferenceTimeLayout)/sizeof(KUtranGpsReferenceTimeLayout[0]), &KUtranGpsReferenceTimeLayout[0]};

/**
RUtranGpsReferenceTimeBuilder constructor
 */
EXPORT_C RUtranGpsReferenceTimeBuilder::RUtranGpsReferenceTimeBuilder()
:	RDataBuilder<TUtranGpsReferenceTime, RDataBuilderBase>(&KUtranGpsReferenceTimeLayoutInfo)
	{}

/**
RPrimaryCpichInfoBuilder constructor
 */	
EXPORT_C RPrimaryCpichInfoBuilder::RPrimaryCpichInfoBuilder()
:	RDataBuilder<TPrimaryCpichInfo, RDataBuilderBase>(&KPrimaryCpichInfoLayoutInfo)
	{}

/**
RUtranGpsReferenceTimeReader constructor
 */
EXPORT_C RUtranGpsReferenceTimeReader::RUtranGpsReferenceTimeReader()
:	RDataReader<TUtranGpsReferenceTime, RDataReaderBase>(&KUtranGpsReferenceTimeLayoutInfo)
	{}

/**
RPrimaryCpichInfoReader constructor
 */
EXPORT_C RPrimaryCpichInfoReader::RPrimaryCpichInfoReader()
:	RDataReader<TPrimaryCpichInfo, RDataReaderBase>(&KPrimaryCpichInfoLayoutInfo)
	{}

