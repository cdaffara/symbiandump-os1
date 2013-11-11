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

#ifndef LBSASSISTANCEUTRANREFERENCETIME_H
#define LBSASSISTANCEUTRANREFERENCETIME_H

#include <lbs/lbsassistancedatabase.h>
#include <e32def.h>

/**
Primary Cpich Info. Defined using RRC spec.
@publishedPartner
@released
*/
class TPrimaryCpichInfo
	{
public:
	/**
	TFieldId
	 */
	enum TFieldId
		{
		/** EPrimaryScramblingCode */
		EPrimaryScramblingCode			//TUint
		};
	};

/**
Utran Gps Reference Time. Defined using RRC spec.
@publishedPartner
@released
*/
class TUtranGpsReferenceTime
	{
public:
	/**
	TFieldId
	 */	
	enum TFieldId
		{
		/** EUtranGPSTimingOfCell_MsPart */
		EUtranGPSTimingOfCell_MsPart,				//TUint
		/** EUtranGPSTimingOfCell_LsPart */
		EUtranGPSTimingOfCell_LsPart,				//TUint
		/** EModeSpecificInfo_Fdd_ReferenceIdentity */
		EModeSpecificInfo_Fdd_ReferenceIdentity,	//TPrimaryCpichInfo
		/** EModeSpecificInfo_Tdd_ReferenceIdentity */
		EModeSpecificInfo_Tdd_ReferenceIdentity,	//TUint
		/** ESfn */
		ESfn										//TUint
		};
	};

//
//Builder classes
/**
Utran Gps Reference Time Builder class. Defined using RRC spec.
@publishedPartner
@released
*/
class RUtranGpsReferenceTimeBuilder : public RDataBuilder<TUtranGpsReferenceTime, RDataBuilderBase>
	{
public:
	IMPORT_C RUtranGpsReferenceTimeBuilder();
	};

/**
Primary Cpich Info Builder class. Defined using RRC spec.
@publishedPartner
@released
*/
class RPrimaryCpichInfoBuilder : public RDataBuilder<TPrimaryCpichInfo, RDataBuilderBase>
	{
public:
	IMPORT_C RPrimaryCpichInfoBuilder();
	};

//
//Reader classes
/**
Utran Gps Reference Time Reader class. Defined using RRC spec.
@publishedPartner
@released
*/
class RUtranGpsReferenceTimeReader : public RDataReader<TUtranGpsReferenceTime, RDataReaderBase>
	{
public:
	IMPORT_C RUtranGpsReferenceTimeReader();
	};

/**
Primary Cpich Info Reader class. Defined using RRC spec.
@publishedPartner
@released
*/
class RPrimaryCpichInfoReader : public RDataReader<TPrimaryCpichInfo, RDataReaderBase>
	{
public:
	IMPORT_C RPrimaryCpichInfoReader();
	};

#endif //LBSASSISTANCEUTRANREFERENCETIME_H

