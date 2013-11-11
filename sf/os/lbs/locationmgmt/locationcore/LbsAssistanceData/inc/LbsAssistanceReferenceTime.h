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

#ifndef LBSASSISTANCEREFERENCETIME_H
#define LBSASSISTANCEREFERENCETIME_H

#include <lbs/lbsassistancedatabase.h>
#include <e32def.h>

#include <lbs/lbsassistanceutranreferencetime.h>
#include <lbs/lbsassistancerrlpreferencetime.h>

/**
Gps Tow Assist. Defined using RRC spec.
@publishedPartner
@released
*/
class TGpsTowAssist
	{
public:
	/**
	TFieldId
	 */
	enum TFieldId
		{
		/** ESatID */
		ESatID,				//TUint
		/** ETlmMessage */
		ETlmMessage,		//TUint
		/** EAlert */
		EAlert,				//TBool
		/** EAntiSpoof */
		EAntiSpoof,			//TBool
		ETlmReserved,		//TUint
		};
	};

/**
UE Positioning Gps Reference Time. Defined using RRC spec, except for ERrlpGpsReferenceTime
@publishedPartner
@released
*/
class TUEPositioningGpsReferenceTime
	{
public:
	/**
	TFieldId
	 */
	enum TFieldId
		{
		/** EGpsWeek */
		EGpsWeek,					//TUint
		/** EGpsTow1Msec */
		EGpsTow1Msec,				//TUint
		/** EUtranGpsReferenceTime */
		EUtranGpsReferenceTime,		//TUtranGpsReferenceTime
		/** ESfnTowUncertainty */
		ESfnTowUncertainty,			//TSfnTowUncertainty
		/** EUtranGpsDriftRate */
		EUtranGpsDriftRate,			//TUtranGpsDriftRate
		/** EGpsTowAssistArray */
		EGpsTowAssistArray,			//TGpsTowAssist array
		/** ERrlpGpsReferenceTime */
		ERrlpGpsReferenceTime,       //TRrlpGpsReferenceTime
		};
	};

//
//Builder classes
/**
Builder classes. Defined using RRC spec.
@publishedPartner
@released
*/
class RUEPositioningGpsReferenceTimeBuilder : public RDataBuilder<TUEPositioningGpsReferenceTime, RDataBuilderRootBase>
	{
public:
	IMPORT_C RUEPositioningGpsReferenceTimeBuilder();
	IMPORT_C void OpenL();
	IMPORT_C static TUint DataSize();
	};

/**
Gps Tow Assist Array Builder classes. Defined using RRC spec.
@publishedPartner
@released
*/
class RGpsTowAssistArrayBuilder : public RDataBuilderArray<TGpsTowAssist>
	{
public:
	IMPORT_C RGpsTowAssistArrayBuilder();
	};

/**
Gps Tow Assist Builder classes. Defined using RRC spec.
@publishedPartner
@released
*/
class RGpsTowAssistBuilder : public RDataBuilder<TGpsTowAssist, RDataBuilderBase>
	{
public:
	IMPORT_C RGpsTowAssistBuilder();
	};


//
//Reader classes
/**
Reader classes. Defined using RRC spec.
@publishedPartner
@released
*/
class RUEPositioningGpsReferenceTimeReader : public RDataReader<TUEPositioningGpsReferenceTime, RDataReaderRootBase>
	{
public:
	IMPORT_C RUEPositioningGpsReferenceTimeReader();
	IMPORT_C void OpenL();
	IMPORT_C static TUint DataSize();
	};

/**
Gps Tow Assist Array Reader classes. Defined using RRC spec.
@publishedPartner
@released
*/
class RGpsTowAssistArrayReader : public RDataReaderArray<TGpsTowAssist>
	{
public:
	IMPORT_C RGpsTowAssistArrayReader();
	};

/**
Gps Tow Assist Reader classes. Defined using RRC spec.
@publishedPartner
@released
*/
class RGpsTowAssistReader : public RDataReader<TGpsTowAssist, RDataReaderBase>
	{
public:
	IMPORT_C RGpsTowAssistReader();
	};

/**
typedef to retain SC. RGpsTowAssistReader was originally named incorrectly.
@publishedPartner
@deprecated
*/
typedef RGpsTowAssistReader RGpsTowAssistBuilderReader;

#endif //LBSASSISTANCEREFERENCETIME_H

