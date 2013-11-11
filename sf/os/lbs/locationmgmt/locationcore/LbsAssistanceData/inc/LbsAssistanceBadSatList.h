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

#ifndef LBSASSISTANCEBADSATLIST_H
#define LBSASSISTANCEBADSATLIST_H

#include <lbs/lbsassistancedatabase.h>
#include <e32def.h>


/**
Field Id of bad satellite
@publishedPartner
@released
*/
class TBadSatList
	{
public:
	/**
	TFieldId
	 */
	enum TFieldId
		{
		/** EBadSatIdArray */
		EBadSatIdArray,			//TUint array
		};
	};

//
//Builder classes

/**
Bad Satellite Array Builder class
@publishedPartner
@released
*/
class RBadSatArrayBuilder : public RDataBuilderArray<TUint>
	{
public:
	IMPORT_C RBadSatArrayBuilder();
	};

/**
Bad Satellite List Builder class
@publishedPartner
@released
*/
class RBadSatListBuilder : public RDataBuilder<TBadSatList, RDataBuilderRootBase>
	{
public:
	IMPORT_C RBadSatListBuilder();
	IMPORT_C void OpenL();
	IMPORT_C static TUint DataSize();
	};

//
//Reader classes

/**
Bad Satellite Array Reader class
@publishedPartner
@released
*/
class RBadSatArrayReader : public RDataReaderArray<TUint>
	{
public:
	IMPORT_C RBadSatArrayReader();
	};

/**
Bad Satellite List Reader class
@publishedPartner
@released
*/
class RBadSatListReader : public RDataReader<TBadSatList, RDataReaderRootBase>
	{
public:
	IMPORT_C RBadSatListReader();
	IMPORT_C void OpenL();
	IMPORT_C static TUint DataSize();
	};

#endif //LBSASSISTANCEBADSATLIST_H