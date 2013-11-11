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

#ifndef LBSASSISTANCEDATAREADERSETIMPL_H
#define LBSASSISTANCEDATAREADERSETIMPL_H

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

class RLbsAssistanceDataReaderSet;

/**
Data Reader Set Implementation class
@publishedPartner
@released
*/
class RLbsAssistanceDataReaderSetImpl
{
public:

    RLbsAssistanceDataReaderSetImpl();
 
 	// Opens all the readers
	void OpenL();

	// Frees memory used by the readers
	void Close();
    
    // IPC methods
    void ExternalizeL(RWriteStream &aStream);
    void InternalizeL(RReadStream &aStream);

	// Acces to readers
	void GetDataReader(RUEPositioningGpsAlmanacReader*& aReader);
	void GetDataReader(RUEPositioningGpsIonosphericModelReader*& aReader);
	void GetDataReader(RUEPositioningGpsNavigationModelReader*& aReader);
	void GetDataReader(RUEPositioningGpsReferenceTimeReader*& aReader);
	void GetDataReader(RUEPositioningGpsUtcModelReader*& aReader);
	void GetDataReader(RUEPositioningGpsAcquisitionAssistanceReader*& aReader);
	void GetDataReader(RBadSatListReader*& aReader);
	void GetDataReader(RReferenceLocationReader*& aReader);


private:

    // Copy not allowed
    RLbsAssistanceDataReaderSetImpl(const RLbsAssistanceDataReaderSetImpl&);
    RLbsAssistanceDataReaderSetImpl& operator=(const RLbsAssistanceDataReaderSetImpl&);

private:

	// List of RDataReaderRootBase objects, one for each type of A-GPS data
	RUEPositioningGpsAlmanacReader iAlmanac;
	RUEPositioningGpsAcquisitionAssistanceReader iAquisitionAssistance;
	RBadSatListReader iBadSatList;
	RUEPositioningGpsIonosphericModelReader iIonosphericModel;
	RUEPositioningGpsNavigationModelReader iNavigationModel;
	RReferenceLocationReader iReferenceLocation;
	RUEPositioningGpsReferenceTimeReader iReferenceTime;
	RUEPositioningGpsUtcModelReader iUtcModel;
};

#endif // LBSASSISTANCEDATAREADERSETIMPL_H

