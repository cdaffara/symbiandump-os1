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

#ifndef LBSASSISTANCEIONOSPHERICMODEL_H
#define LBSASSISTANCEIONOSPHERICMODEL_H

#include <lbs/lbsassistancedatabase.h>
#include <e32def.h>


/**
UE Positioning Gps Ionospheric Model. Defined using RRC spec.
@publishedPartner
@released
*/
class TUEPositioningGpsIonosphericModel
	{
public:
	/**
	TFieldId
	 */
	enum TFieldId
		{
		/** EAlfa0 */
		EAlfa0,		//TUint
		/** EAlfa1 */
		EAlfa1,		//TUint
		/** EAlfa2 */
		EAlfa2,		//TUint
		/** EAlfa3 */
		EAlfa3,		//TUint
		/** EBeta0 */
		EBeta0,		//TUint
		/** EBeta1 */
		EBeta1,		//TUint
		/** EBeta2 */
		EBeta2,		//TUint
		/** EBeta3 */
		EBeta3		//TUint
		};
	};

//
//Builder classes
/**
Builder classes. Defined using RRC spec.
@publishedPartner
@released
*/
class RUEPositioningGpsIonosphericModelBuilder : public RDataBuilder<TUEPositioningGpsIonosphericModel, RDataBuilderRootBase>
	{
public:
	IMPORT_C RUEPositioningGpsIonosphericModelBuilder();
	IMPORT_C void OpenL();
	IMPORT_C static TUint DataSize();
	};


//
//Reader classes
/**
Reader classes. Defined using RRC spec.
@publishedPartner
@released
*/
class RUEPositioningGpsIonosphericModelReader : public RDataReader<TUEPositioningGpsIonosphericModel, RDataReaderRootBase>
	{
public:
	IMPORT_C RUEPositioningGpsIonosphericModelReader();
	IMPORT_C void OpenL();
	IMPORT_C static TUint DataSize();
	};

#endif //LBSASSISTANCEIONOSPHERICMODEL_H

