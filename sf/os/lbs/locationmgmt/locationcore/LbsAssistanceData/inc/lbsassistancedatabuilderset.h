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

#ifndef LBSASSISTANCEDATABUILDERSET_H_
#define LBSASSISTANCEDATABUILDERSET_H_

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

class RLbsAssistanceDataBuilderSetImpl;

//-------------------------------------------------------------------------------

/**
Data Builder Set class
@publishedPartner
@released
*/
class RLbsAssistanceDataBuilderSet
	{
	
public:

	IMPORT_C RLbsAssistanceDataBuilderSet();
	
 	// Creates and opens all the builders
	IMPORT_C void OpenL();
	
	// Frees memory used by the builders and deletes them.
	IMPORT_C void Close();
	
    // Clears all the data fields in all the data items 
    // without freeing their data buffers.
    IMPORT_C void ClearAllBuilders();

	// Get a builder
	template <typename T>
    inline TInt GetDataBuilder(T*& aBuilderPtr);
    
	// Merge another set with this one. Returns an error if either
	// set has not been opened.
	IMPORT_C TInt MergeL(RLbsAssistanceDataBuilderSet& aSet);
	
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

	IMPORT_C TInt DoGet(RUEPositioningGpsAlmanacBuilder*& aBuilder);
	IMPORT_C TInt DoGet(RUEPositioningGpsIonosphericModelBuilder*& aBuilder);
	IMPORT_C TInt DoGet(RUEPositioningGpsNavigationModelBuilder*& aBuilder);
	IMPORT_C TInt DoGet(RUEPositioningGpsReferenceTimeBuilder*& aBuilder);
	IMPORT_C TInt DoGet(RUEPositioningGpsUtcModelBuilder*& aBuilder);
	IMPORT_C TInt DoGet(RUEPositioningGpsAcquisitionAssistanceBuilder*& aBuilder);
	IMPORT_C TInt DoGet(RBadSatListBuilder*& aBuilder);
	IMPORT_C TInt DoGet(RReferenceLocationBuilder*& aBuilder);
	
    // Copy not allowed
    RLbsAssistanceDataBuilderSet(const RLbsAssistanceDataBuilderSet&);
    RLbsAssistanceDataBuilderSet& operator=(const RLbsAssistanceDataBuilderSet&);
    
private:

	RLbsAssistanceDataBuilderSetImpl* iAssistanceDataBuilderSetImplPtr;
	};
	
#include <lbs/lbsassistancedatabuilderset.inl>
#endif // LBSASSISTANCEDATABUILDERSET_H_

