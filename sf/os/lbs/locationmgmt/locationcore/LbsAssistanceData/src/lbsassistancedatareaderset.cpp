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
#include <lbs/lbsassistancedatareaderset.h>
#include "lbsassistancedatareadersetimpl.h"

/**

@param
@return
@released
*/
EXPORT_C RLbsAssistanceDataReaderSet::RLbsAssistanceDataReaderSet()
	{
	iAssistanceDataReaderSetImplPtr = NULL;
	}


/**

@param
@return
@released
*/
EXPORT_C void RLbsAssistanceDataReaderSet::OpenL()
	{
	if (iAssistanceDataReaderSetImplPtr == NULL)
		{
		iAssistanceDataReaderSetImplPtr = new(ELeave) RLbsAssistanceDataReaderSetImpl;
		}
	else
		{
		iAssistanceDataReaderSetImplPtr->Close();
		}
		
	iAssistanceDataReaderSetImplPtr->OpenL();
	}

/**

@param
@return
@released
*/
EXPORT_C void RLbsAssistanceDataReaderSet::Close()
	{
	if(iAssistanceDataReaderSetImplPtr != NULL)
		{
			iAssistanceDataReaderSetImplPtr->Close();
			delete iAssistanceDataReaderSetImplPtr;
			iAssistanceDataReaderSetImplPtr = NULL;
		}
	}

/**

@param
@return
@released
*/
EXPORT_C void RLbsAssistanceDataReaderSet::ExternalizeL(RWriteStream &aStream)
	{
	if(iAssistanceDataReaderSetImplPtr != NULL)
		{
		return iAssistanceDataReaderSetImplPtr->ExternalizeL(aStream);
		}
	}
		
EXPORT_C void RLbsAssistanceDataReaderSet::InternalizeL(RReadStream &aStream)
	{
	if(iAssistanceDataReaderSetImplPtr != NULL)
		{
		return iAssistanceDataReaderSetImplPtr->InternalizeL(aStream);
		}
	}

/**

@param
@return
@released
*/
EXPORT_C TUint RLbsAssistanceDataReaderSet::MaxExternalizedBufferSize()
	{
	TUint size = 0;

	size += RUEPositioningGpsAlmanacReader::DataSize();
	size += RBadSatListReader::DataSize();
	size += RUEPositioningGpsIonosphericModelReader::DataSize();
	size += RUEPositioningGpsNavigationModelReader::DataSize();
	size += RReferenceLocationReader::DataSize();
	size += RUEPositioningGpsReferenceTimeReader::DataSize();
	size += RUEPositioningGpsUtcModelReader::DataSize();
	size += RUEPositioningGpsAcquisitionAssistanceReader::DataSize();

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
EXPORT_C TInt RLbsAssistanceDataReaderSet::DoGet(RUEPositioningGpsAlmanacReader*& aReader)
{
    TInt error = KErrNone;
	if(iAssistanceDataReaderSetImplPtr != NULL)
	{
		iAssistanceDataReaderSetImplPtr->GetDataReader(aReader);
	}
	else
	{
		aReader = NULL;
		error = KErrNotFound;
	}
	
	return error;
}

/**

@param
@return
@released
*/
EXPORT_C TInt RLbsAssistanceDataReaderSet::DoGet(RUEPositioningGpsIonosphericModelReader*& aReader)
{
    TInt error = KErrNone;

	if(iAssistanceDataReaderSetImplPtr != NULL)
	{
		iAssistanceDataReaderSetImplPtr->GetDataReader(aReader);
	}
	else
	{
		aReader = NULL;
		error = KErrNotFound;
	}
	return error;	
}

/**

@param
@return
@released
*/
EXPORT_C TInt RLbsAssistanceDataReaderSet::DoGet(RUEPositioningGpsNavigationModelReader*& aReader)
{
    TInt error = KErrNone;

	if(iAssistanceDataReaderSetImplPtr != NULL)
	{
		iAssistanceDataReaderSetImplPtr->GetDataReader(aReader);
	}
	else
	{
		aReader = NULL;
		error = KErrNotFound;		
	}
	return error;
}

/**

@param
@return
@released
*/
EXPORT_C TInt RLbsAssistanceDataReaderSet::DoGet(RUEPositioningGpsReferenceTimeReader*& aReader)
{
    TInt error = KErrNone;

	if(iAssistanceDataReaderSetImplPtr != NULL)
	{
		iAssistanceDataReaderSetImplPtr->GetDataReader(aReader);
	}
	else
	{
		aReader = NULL;
		error = KErrNotFound;		
	}
	return error;
}

/**

@param
@return
@released
*/
EXPORT_C TInt RLbsAssistanceDataReaderSet::DoGet(RUEPositioningGpsUtcModelReader*& aReader)
{
    TInt error = KErrNone;

	if(iAssistanceDataReaderSetImplPtr != NULL)
		{
		iAssistanceDataReaderSetImplPtr->GetDataReader(aReader);
		}
	else
		{
		aReader = NULL;
		error = KErrNotFound;
		}
	return error;
}

/**

@param
@return
@released
*/
EXPORT_C TInt RLbsAssistanceDataReaderSet::DoGet(RUEPositioningGpsAcquisitionAssistanceReader*& aReader)
{
    TInt error = KErrNone;

	if(iAssistanceDataReaderSetImplPtr != NULL)
		{
		iAssistanceDataReaderSetImplPtr->GetDataReader(aReader);
		}
	else
		{
		aReader = NULL;
		error = KErrNotFound;
		}
	return error;
}

/**

@param
@return
@released
*/
EXPORT_C TInt RLbsAssistanceDataReaderSet::DoGet(RBadSatListReader*& aReader)
{
    TInt error = KErrNone;

	if(iAssistanceDataReaderSetImplPtr != NULL)
		{
		iAssistanceDataReaderSetImplPtr->GetDataReader(aReader);
		}
	else
		{
		aReader = NULL;
		error = KErrNotFound;
		}
	return error;
}

/**

@param
@return
@released
*/
EXPORT_C TInt RLbsAssistanceDataReaderSet::DoGet(RReferenceLocationReader*& aReader)
{
    TInt error = KErrNone;

	if(iAssistanceDataReaderSetImplPtr != NULL)
		{
		iAssistanceDataReaderSetImplPtr->GetDataReader(aReader);
		}
	else
		{
		aReader = NULL;
		error = KErrNotFound;
		}
	return error;
}
