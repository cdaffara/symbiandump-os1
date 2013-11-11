// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LBSASSISTANCEDATABUILDERSETIMPL_H_
#define LBSASSISTANCEDATABUILDERSETIMPL_H_

#include <e32def.h>
#include <lbs/lbsassistancedatabase.h>
#include <lbs/lbsassistancealmanac.h>
#include <lbs/lbsassistanceaquisitionassistance.h>
#include <lbs/lbsassistancebadsatlist.h>
#include <lbs/lbsassistanceionosphericmodel.h>
#include <lbs/lbsassistancenavigationmodel.h>
#include <lbs/lbsassistancereferencelocation.h>
#include <lbs/lbsassistancereferencetime.h>
#include <lbs/lbsassistanceutcmodel.h>

class RLbsAssistanceDataBuilderSet;

/**
Data Builder Set Implementation class
@publishedPartner
@released
*/
class RLbsAssistanceDataBuilderSetImpl
{
public:

    RLbsAssistanceDataBuilderSetImpl();
 
 	// Opens all the builders
	void OpenL();

	// Frees memory used by the builders
	void Close();
    
    // Clears all the data from the builders
    // but does not free their buffers
    void ClearAllBuilders();
    
    // Merge another set with this one. Returns an error if either
	// set has not been opened.
	void MergeL(RLbsAssistanceDataBuilderSet& aSet);
    
    // IPC methods
    void ExternalizeL(RWriteStream &aStream);
    void InternalizeL(RReadStream &aStream);

	// Acces to builders
	void GetDataBuilder(RUEPositioningGpsAlmanacBuilder*& aBuilder);
	void GetDataBuilder(RUEPositioningGpsIonosphericModelBuilder*& aBuilder);
	void GetDataBuilder(RUEPositioningGpsNavigationModelBuilder*& aBuilder);
	void GetDataBuilder(RUEPositioningGpsReferenceTimeBuilder*& aBuilder);
	void GetDataBuilder(RUEPositioningGpsUtcModelBuilder*& aBuilder);
	void GetDataBuilder(RUEPositioningGpsAcquisitionAssistanceBuilder*& aBuilder);
	void GetDataBuilder(RBadSatListBuilder*& aBuilder);
	void GetDataBuilder(RReferenceLocationBuilder*& aBuilder);


private:

    // Copy not allowed
    RLbsAssistanceDataBuilderSetImpl(const RLbsAssistanceDataBuilderSetImpl&);
    RLbsAssistanceDataBuilderSetImpl& operator=(const RLbsAssistanceDataBuilderSetImpl&);

private:

	// List of RDataBuilderRootBase objects, one for each type of A-GPS data
	RUEPositioningGpsAlmanacBuilder iAlmanac;
	RUEPositioningGpsAcquisitionAssistanceBuilder iAquisitionAssistance;
	RBadSatListBuilder iBadSatList;
	RUEPositioningGpsIonosphericModelBuilder iIonosphericModel;
	RUEPositioningGpsNavigationModelBuilder iNavigationModel;
	RReferenceLocationBuilder iReferenceLocation;
	RUEPositioningGpsReferenceTimeBuilder iReferenceTime;
	RUEPositioningGpsUtcModelBuilder iUtcModel;
};

#endif // LBSASSISTANCEDATABUILDERSETIMPL_H_

