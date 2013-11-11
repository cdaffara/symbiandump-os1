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

#include <s32strm.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include "lbsassistancedatabuildersetimpl.h"

/**

@param
@return
@released
*/
EXPORT_C RLbsAssistanceDataBuilderSet::RLbsAssistanceDataBuilderSet()
	{
	iAssistanceDataBuilderSetImplPtr = NULL;
	}


/**

@param
@return
@released
*/
EXPORT_C void RLbsAssistanceDataBuilderSet::OpenL()
	{
	if (iAssistanceDataBuilderSetImplPtr == NULL)
		{
		iAssistanceDataBuilderSetImplPtr = new(ELeave) RLbsAssistanceDataBuilderSetImpl;
		}
	else
		{
		iAssistanceDataBuilderSetImplPtr->Close();
		}
		
	iAssistanceDataBuilderSetImplPtr->OpenL();
	}

/**

@param
@return
@released
*/
EXPORT_C void RLbsAssistanceDataBuilderSet::Close()
	{
	if(iAssistanceDataBuilderSetImplPtr != NULL)
		{
			iAssistanceDataBuilderSetImplPtr->Close();
			delete iAssistanceDataBuilderSetImplPtr;
			iAssistanceDataBuilderSetImplPtr = NULL;
		}
	}

/**

@param
@return
@released
*/
EXPORT_C void RLbsAssistanceDataBuilderSet::ClearAllBuilders()
	{
	if(iAssistanceDataBuilderSetImplPtr != NULL)
		{
		iAssistanceDataBuilderSetImplPtr->ClearAllBuilders();
		}
	}

/**

@param
@return
@released
*/
EXPORT_C void RLbsAssistanceDataBuilderSet::ExternalizeL(RWriteStream &aStream)
	{
	if(iAssistanceDataBuilderSetImplPtr != NULL)
		{
		return iAssistanceDataBuilderSetImplPtr->ExternalizeL(aStream);
		}
	}
		
EXPORT_C void RLbsAssistanceDataBuilderSet::InternalizeL(RReadStream &aStream)
	{
	if(iAssistanceDataBuilderSetImplPtr != NULL)
		{
		return iAssistanceDataBuilderSetImplPtr->InternalizeL(aStream);
		}
	}

/**

@param
@return
@released
*/
EXPORT_C TInt RLbsAssistanceDataBuilderSet::MergeL(RLbsAssistanceDataBuilderSet& aSet)
{
	if((iAssistanceDataBuilderSetImplPtr != NULL))
		{
		iAssistanceDataBuilderSetImplPtr->MergeL(aSet);
		}
	else 
		{
		// this set has not been opened yet
		return KErrNotFound;
		}
		
	return KErrNone;	
}


/**

@param
@return
@released
*/
EXPORT_C TUint RLbsAssistanceDataBuilderSet::MaxExternalizedBufferSize()
	{
	TUint size = 0;

	size += RUEPositioningGpsAlmanacBuilder::DataSize();
	size += RBadSatListBuilder::DataSize();
	size += RUEPositioningGpsIonosphericModelBuilder::DataSize();
	size += RUEPositioningGpsNavigationModelBuilder::DataSize();
	size += RReferenceLocationBuilder::DataSize();
	size += RUEPositioningGpsReferenceTimeBuilder::DataSize();
	size += RUEPositioningGpsUtcModelBuilder::DataSize();
	size += RUEPositioningGpsAcquisitionAssistanceBuilder::DataSize();

	// Add the size of 8 time stamps
	size += (8*sizeof(TTime));

	// Add the size of the TInt32 mask
	size += sizeof(TUint32);
	
	return size;
	}

/**

@param
@return
@released
*/
EXPORT_C TInt RLbsAssistanceDataBuilderSet::DoGet(RUEPositioningGpsAlmanacBuilder*& aBuilder)
{
    TInt error = KErrNone;
	if(iAssistanceDataBuilderSetImplPtr != NULL)
	{
		iAssistanceDataBuilderSetImplPtr->GetDataBuilder(aBuilder);
	}
	else
	{
		aBuilder = NULL;
		error = KErrNotFound;
	}
	
	return error;
}

/**

@param
@return
@released
*/
EXPORT_C TInt RLbsAssistanceDataBuilderSet::DoGet(RUEPositioningGpsIonosphericModelBuilder*& aBuilder)
{
    TInt error = KErrNone;

	if(iAssistanceDataBuilderSetImplPtr != NULL)
	{
		iAssistanceDataBuilderSetImplPtr->GetDataBuilder(aBuilder);
	}
	else
	{
		aBuilder = NULL;
		error = KErrNotFound;
	}
	return error;	
}

/**

@param
@return
@released
*/
EXPORT_C TInt RLbsAssistanceDataBuilderSet::DoGet(RUEPositioningGpsNavigationModelBuilder*& aBuilder)
{
    TInt error = KErrNone;

	if(iAssistanceDataBuilderSetImplPtr != NULL)
	{
		iAssistanceDataBuilderSetImplPtr->GetDataBuilder(aBuilder);
	}
	else
	{
		aBuilder = NULL;
		error = KErrNotFound;		
	}
	return error;
}

/**

@param
@return
@released
*/
EXPORT_C TInt RLbsAssistanceDataBuilderSet::DoGet(RUEPositioningGpsReferenceTimeBuilder*& aBuilder)
{
    TInt error = KErrNone;

	if(iAssistanceDataBuilderSetImplPtr != NULL)
	{
		iAssistanceDataBuilderSetImplPtr->GetDataBuilder(aBuilder);
	}
	else
	{
		aBuilder = NULL;
		error = KErrNotFound;		
	}
	return error;
}

/**

@param
@return
@released
*/
EXPORT_C TInt RLbsAssistanceDataBuilderSet::DoGet(RUEPositioningGpsUtcModelBuilder*& aBuilder)
{
    TInt error = KErrNone;

	if(iAssistanceDataBuilderSetImplPtr != NULL)
		{
		iAssistanceDataBuilderSetImplPtr->GetDataBuilder(aBuilder);
		}
	else
		{
		aBuilder = NULL;
		error = KErrNotFound;
		}
	return error;
}

/**

@param
@return
@released
*/
EXPORT_C TInt RLbsAssistanceDataBuilderSet::DoGet(RUEPositioningGpsAcquisitionAssistanceBuilder*& aBuilder)
{
    TInt error = KErrNone;

	if(iAssistanceDataBuilderSetImplPtr != NULL)
		{
		iAssistanceDataBuilderSetImplPtr->GetDataBuilder(aBuilder);
		}
	else
		{
		aBuilder = NULL;
		error = KErrNotFound;
		}
	return error;
}

/**

@param
@return
@released
*/
EXPORT_C TInt RLbsAssistanceDataBuilderSet::DoGet(RBadSatListBuilder*& aBuilder)
{
    TInt error = KErrNone;

	if(iAssistanceDataBuilderSetImplPtr != NULL)
		{
		iAssistanceDataBuilderSetImplPtr->GetDataBuilder(aBuilder);
		}
	else
		{
		aBuilder = NULL;
		error = KErrNotFound;
		}
	return error;
}

/**

@param
@return
@released
*/
EXPORT_C TInt RLbsAssistanceDataBuilderSet::DoGet(RReferenceLocationBuilder*& aBuilder)
{
    TInt error = KErrNone;

	if(iAssistanceDataBuilderSetImplPtr != NULL)
		{
		iAssistanceDataBuilderSetImplPtr->GetDataBuilder(aBuilder);
		}
	else
		{
		aBuilder = NULL;
		error = KErrNotFound;
		}
	return error;
}
