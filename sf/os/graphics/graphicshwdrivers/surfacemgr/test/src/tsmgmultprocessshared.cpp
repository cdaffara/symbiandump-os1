// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// surface manager API
// 
//

#include "tsmgmultprocessshared.h"
#include <e32def_private.h>

/**
Creates a wrapper object for a shared chunk between processes
*/
CChunkWrapper* CChunkWrapper::CreateL(const TDesC& aName, TInt aSize, TInt aMaxSize)
	{
	CChunkWrapper* self = new(ELeave) CChunkWrapper();
	CleanupStack::PushL(self);
	self->CreateConstructL(aName, aSize, aMaxSize);
	CleanupStack::Pop(self);
	return self;
	}

/**
Second phase creation of the shared chunk wrapper
*/
void CChunkWrapper::CreateConstructL(const TDesC& aName, TInt aSize, TInt aMaxSize)
	{
	User::LeaveIfError(iChunk.CreateGlobal(aName, aSize, aMaxSize));
	}

/**
Opens an alerady created wrapper for a chunk that we share between processes
*/
CChunkWrapper* CChunkWrapper::OpenL(const TDesC& aName, TBool aIsReadOnly)
	{
	CChunkWrapper* self = new(ELeave) CChunkWrapper();
	CleanupStack::PushL(self);
	self->OpenConstructL(aName, aIsReadOnly);
	CleanupStack::Pop(self);
	return self;
	}

/**
Second phase opening a previously created chunk
*/	
void CChunkWrapper::OpenConstructL(const TDesC& aName, TBool aIsReadOnly)
	{
	User::LeaveIfError(iChunk.OpenGlobal(aName, aIsReadOnly));
	}

/**
Destructor
*/	
CChunkWrapper::~CChunkWrapper()
	{
	iChunk.Close();
	}
/**
Constructor
*/	
CChunkWrapper::CChunkWrapper()
	{
	}


/**
Sets the results (bitmapped into a TInt) of all the second process tests

Second process results always reside in the first sizeof(TInt) bytes of the chunk
*/
void CChunkWrapper::SetSecondProcessResults(TInt aResult)
	{
	TUint8* ptr = iChunk.Base();
	TInt* intPtr = reinterpret_cast<TInt*>(ptr);
	*intPtr = aResult;
	}

/**
Gets the results (bitmapped into a TInt) of all the second process tests

Second process results always reside in the first sizeof(TInt) bytes of the chunk
*/
TInt CChunkWrapper::GetSecondProcessResults()
	{
	TUint8* ptr = iChunk.Base();
	TInt* intPtr = reinterpret_cast<TInt*>(ptr);
	return *intPtr; 
	}

/**
Sets the results (bitmapped into a TInt) of all the third process tests

Third process results always reside in the second sizeof(TInt) bytes of the chunk
*/
void CChunkWrapper::SetThirdProcessResults(TInt aResult)
	{
	TUint8* ptr = iChunk.Base();
	TInt* intPtr = reinterpret_cast<TInt*>(ptr);
	intPtr++; // Offset to second TInt in the chunk
	*intPtr = aResult;
	}

/**
Gets the results (bitmapped into a TInt) of all the second process tests

Second process results always reside in the first sizeof(TInt) bytes of the chunk
*/
TInt CChunkWrapper::GetThirdProcessResults()
	{
	TUint8* ptr = iChunk.Base();
	TInt* intPtr = reinterpret_cast<TInt*>(ptr);
	intPtr++; // Offset to second TInt in the chunk
	return *intPtr; 
	}


/**
Puts the surfaceId of a surface on the shared chunk
*/
void CChunkWrapper::SetId(const TSurfaceId& aId)
	{
	TUint8* ptr = iChunk.Base();
	TInt* intPtr = reinterpret_cast<TInt*>(ptr);
	intPtr += 2; // Offset past first two TInts in the chunk
	TUint32* uintPtr = reinterpret_cast<TUint32*>(intPtr);
	for(TInt ii = 0; ii < 4; ++ii, ++uintPtr)
		{
		*uintPtr = aId.iInternal[ii];
		}
	}

/**
Reads the surfaceId of a surface off the shared chunk
*/	
TSurfaceId CChunkWrapper::GetId()
	{
	TSurfaceId id;
	TUint8* ptr = iChunk.Base();
	TInt* intPtr = reinterpret_cast<TInt*>(ptr);
	intPtr += 2; // Offset past first two TInts in the chunk
	TUint32* uintPtr = reinterpret_cast<TUint32*>(intPtr);
	for(TInt ii = 0; ii < 4; ++ii, ++uintPtr)
		{
		id.iInternal[ii] = *uintPtr;
		}
	return id;
	}	

CTestDriver::CTestDriver()
	{
	iTestResult = 0;
	}

void CTestDriver::ConstructL()
	{
	User::LeaveIfError(iSurfaceManager.Open());
	TRAPD(err,iChunkWrapper =  CChunkWrapper::OpenL(KSharedChunkName, ETrue));
	if (err == KErrNotFound) //the chunk wrapper is not yet created, cant be opened
		{
		iChunkWrapper =CChunkWrapper::CreateL(KSharedChunkName, KSharedChunkSize, KSharedChunkSize);
		}
	}

CTestDriver::~CTestDriver()
	{
	// Cleanup 
	iSurfaceManager.Close();
	delete iChunkWrapper;
	}
