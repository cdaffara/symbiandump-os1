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

#ifndef LBSASSISTANCERRLPREFERENCETIME_H
#define LBSASSISTANCERRLPREFERENCETIME_H

#include <lbs/lbsassistancedatabase.h>
#include <e32def.h>

/**
 @file
 @publishedPartner
*/


/**
Utran Gps Reference Time. Defined using RRC spec.
@publishedPartner
@prototype
*/
class TRrlpGpsReferenceTime
	{
public:
	/**
	TFieldId
	 */	
	enum TFieldId
		{
        EBcchCarrier,               //TUint - this uses 10 bits - from RRLP
        EBsic,                      //TUint - this uses 6 bits - from RRLP
        EFrameNumber,               //TUint - this uses 21 bits and represents the number of frames - from RRLP
        ETimeslotsNumber,           //TUint - this uses 3 bits and represents the number of timeslots - from RRLP
        EBitsNumber,                //TUint - this uses  8 bits and represents the number of bits - from RRLP
		};
	};

//
//Builder classes
/**
Utran Gps Reference Time Builder class. Defined using RRC spec.
@publishedPartner
@prototype
*/
class RRrlpGpsReferenceTimeBuilder : public RDataBuilder<TRrlpGpsReferenceTime, RDataBuilderBase>
	{
public:
	IMPORT_C RRrlpGpsReferenceTimeBuilder();
	};

//
//Reader classes
/**
Utran Gps Reference Time Reader class. Defined using RRC spec.
@publishedPartner
@prototype
*/
class RRrlpGpsReferenceTimeReader : public RDataReader<TRrlpGpsReferenceTime, RDataReaderBase>
	{
public:
	IMPORT_C RRrlpGpsReferenceTimeReader();
	};

#endif //LBSASSISTANCERRLPREFERENCETIME_H
