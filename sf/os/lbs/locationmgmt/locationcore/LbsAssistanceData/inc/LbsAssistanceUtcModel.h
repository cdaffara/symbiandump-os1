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

#ifndef LBSASSISTANCEUTCMODEL_H
#define LBSASSISTANCEUTCMODEL_H

#include <lbs/lbsassistancedatabase.h>
#include <e32def.h>

/**
UE Positioning Gps Utc Model. Defined using RRC spec.
@publishedPartner
@released
*/
class TUEPositioningGpsUtcModel
	{
public:
	/**
	TFieldId
	 */
	enum TFieldId
		{
		/** EA1 */
		EA1,			//TUint
		/** EA0 */
		EA0,			//TUint
		/** ETot */
		ETot,			//TUint
		/** EWnt */
		EWnt,			//TUint
		/** EDeltatLS */
		EDeltatLS,		//TUint
		/** EWnlsf */
		EWnlsf,			//TUint
		/** EDn */
		EDn,			//TUint
		/** EDeltatLSF */
		EDeltatLSF		//TUint
		};
	};

//
//Builder classes

/**
UE Positioning Gps Utc Model Builder class. Defined using RRC spec.
@publishedPartner
@released
*/
class RUEPositioningGpsUtcModelBuilder : public RDataBuilder<TUEPositioningGpsUtcModel, RDataBuilderRootBase>
	{
public:
	IMPORT_C RUEPositioningGpsUtcModelBuilder();
	IMPORT_C void OpenL();
	IMPORT_C static TUint DataSize();
	};


//
//Reader classes

/**
UE Positioning Gps Utc Model Reader class. Defined using RRC spec.
@publishedPartner
@released
*/
class RUEPositioningGpsUtcModelReader : public RDataReader<TUEPositioningGpsUtcModel, RDataReaderRootBase>
	{
public:
	IMPORT_C RUEPositioningGpsUtcModelReader();
	IMPORT_C void OpenL();
	IMPORT_C static TUint DataSize();
	};


#endif //LBSASSISTANCEUTCMODEL_H

