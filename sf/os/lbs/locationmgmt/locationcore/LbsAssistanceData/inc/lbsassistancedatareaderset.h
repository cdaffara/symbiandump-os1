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

#ifndef LBSASSISTANCEDATAREADERSET_H
#define LBSASSISTANCEDATAREADERSET_H

#include <s32strm.h>
#include <lbs/lbsassistancedatabase.h>
#include <lbs/lbsassistancealmanac.h>
#include <lbs/lbsassistanceaquisitionassistance.h>
#include <lbs/lbsassistancebadsatlist.h>
#include <lbs/lbsassistanceionosphericmodel.h>
#include <lbs/lbsassistancenavigationmodel.h>
#include <lbs/lbsassistancereferencelocation.h>
#include <lbs/lbsassistancereferencetime.h>
#include <lbs/lbsassistanceutcmodel.h>

//-------------------------------------------------------------------------------

class RLbsAssistanceDataReaderSetImpl;

//-------------------------------------------------------------------------------

/**
Data Reader Set class
@publishedPartner
@released
*/
class RLbsAssistanceDataReaderSet
	{
	
public:

	IMPORT_C RLbsAssistanceDataReaderSet();
	
 	// Creates and opens all the readers
	IMPORT_C void OpenL();
	
	// Frees memory used by the readers and deletes them.
	IMPORT_C void Close();
	
	// Get a reader
	template <typename T>
    inline TInt GetDataReader(T*& aReaderPtr);
    
public:
	
	// IPC methods
    IMPORT_C void ExternalizeL(RWriteStream& aStream);
	IMPORT_C void InternalizeL(RReadStream& aStream);
	// Get the size of a buffer large enough for all
	// the information that is externalized by the set
    // (assistance data, time stamps, mask,...)
    //
    IMPORT_C static TUint MaxExternalizedBufferSize();
	
private:

	IMPORT_C TInt DoGet(RUEPositioningGpsAlmanacReader*& aReader);
	IMPORT_C TInt DoGet(RUEPositioningGpsIonosphericModelReader*& aReader);
	IMPORT_C TInt DoGet(RUEPositioningGpsNavigationModelReader*& aReader);
	IMPORT_C TInt DoGet(RUEPositioningGpsReferenceTimeReader*& aReader);
	IMPORT_C TInt DoGet(RUEPositioningGpsUtcModelReader*& aReader);
	IMPORT_C TInt DoGet(RUEPositioningGpsAcquisitionAssistanceReader*& aReader);
	IMPORT_C TInt DoGet(RBadSatListReader*& aReader);
	IMPORT_C TInt DoGet(RReferenceLocationReader*& aReader);
	
    // Copy not allowed
    RLbsAssistanceDataReaderSet(const RLbsAssistanceDataReaderSet&);
    RLbsAssistanceDataReaderSet& operator=(const RLbsAssistanceDataReaderSet&);
    
private:

	RLbsAssistanceDataReaderSetImpl* iAssistanceDataReaderSetImplPtr;
	};
	
#include <lbs/lbsassistancedatareaderset.inl>
#endif // LBSASSISTANCEDATAREADERSET_H

