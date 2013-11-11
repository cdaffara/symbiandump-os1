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
//

#include "ssmdebug.h"
#include "ssmswppolicymap.h"


//
// The CSsmSwpPolicyMapping implementation
//

// Construction/destruction
	
/**
Create a mapping from an swp and a file name

@param aSwpKey The swp key to set 
@param aFilename The file name associated with this swp key
@return A new CSsmSwpPolicyMapping containing the swp and file name
*/
CSsmSwpPolicyMapping* CSsmSwpPolicyMapping::NewLC(TUint aSwpKey, const TDesC& aFilename)
	{
	CSsmSwpPolicyMapping* self = new (ELeave) CSsmSwpPolicyMapping(aSwpKey);
	CleanupStack::PushL(self);
	self->ConstructL(aFilename);
	return self;
	}  //lint !e1746 Suppress parameter 'aSwpKey' could be made const reference

/**
Construct a mapping onbject

@param aSwpKey swp key to set 

*/
CSsmSwpPolicyMapping::CSsmSwpPolicyMapping(TUint aSwpKey) :
	iSwpKey(aSwpKey)
	{
	}  //lint !e1746 Suppress parameter 'aSwpKey' could be made const reference

/**
Set file on a constructed object

@param aFile File name to set 
*/
void CSsmSwpPolicyMapping::ConstructL(const TDesC& aFilename)
	{
	iFilename = aFilename.AllocL();
	}

/**
Trivial destructor
*/
CSsmSwpPolicyMapping::~CSsmSwpPolicyMapping()
	{
	delete iFilename;
	}

// Accessors

/**
Retrieve this mapping's swp key

@return An swp key
*/
TUint CSsmSwpPolicyMapping::SwpKey() const
	{
	return iSwpKey;
	}

/**
Retrieve this mapping's file name

@return A reference to a descriptor containing the file name
*/
const TDesC& CSsmSwpPolicyMapping::Filename() const
	{
	return *iFilename;
	}
 
/**
Method for comparing two mappings.
Used by the map's sort and search functions
*/
TInt CSsmSwpPolicyMapping::CompareBySwp(const CSsmSwpPolicyMapping& aFirst, const CSsmSwpPolicyMapping& aSecond)
	{
	return aFirst.SwpKey() - aSecond.SwpKey();
	}

/**
This linear order encapsulates the function used for sorting mappings by their swp as
well as determining equivalence when searching

@see TLinearOrder
*/
	
static TLinearOrder<CSsmSwpPolicyMapping> bySwp( CSsmSwpPolicyMapping::CompareBySwp );


//
// The CSsmSwpPolicyMap implementation
//

// Construction/destruction

/**
Trivial factory methods
*/
CSsmSwpPolicyMap* CSsmSwpPolicyMap::NewLC()
	{
	CSsmSwpPolicyMap* self = new (ELeave) CSsmSwpPolicyMap();
	CleanupStack::PushL(self);
	return self;
	}

CSsmSwpPolicyMap* CSsmSwpPolicyMap::NewL()
	{
	CSsmSwpPolicyMap* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CSsmSwpPolicyMap::CSsmSwpPolicyMap()
	{
	}

/**
Destroy item list
*/
CSsmSwpPolicyMap::~CSsmSwpPolicyMap()
	{
	iItems.ResetAndDestroy();
	}

/**
Add a mapping to the mapping list
After this call, the map owns the mapping and the API user must pop the mapping 
from the cleanup stack

@param aSwpKey The swp key to set 
@param aFilename The file name to be associated with this swp key
@leave KErrAlreadyExists if this swp key is already in the list

*/
void CSsmSwpPolicyMap::AddL(TUint aSwpKey, const TDesC& aFilename)
	{
	CSsmSwpPolicyMapping* mapping = CSsmSwpPolicyMapping::NewLC(aSwpKey, aFilename);
	iItems.InsertInOrderL(mapping, bySwp);
	CleanupStack::Pop(mapping);
	}  //lint !e1746 Suppress parameter 'aSwpKey' could be made const reference

/**
 * Delete a mapping from the mapping list
 * After this call the mapping corresponding to the key is deleted 
 * @param aSwpKey The swp key to be deleted
 * @leave KErrNotFound if this swp key is not present in the list
*/
#ifdef _DEBUG
void CSsmSwpPolicyMap::DeleteSwpMapL(TUint aSwpKey)
	{
	CSsmSwpPolicyMapping* mapping = CSsmSwpPolicyMapping::NewLC(aSwpKey, KNullDesC);
	TInt mappingarrayindex = iItems.FindInOrderL(mapping, bySwp);
	CleanupStack::PopAndDestroy(mapping);
	CSsmSwpPolicyMapping* found = iItems[mappingarrayindex];
	delete found;
	iItems.Remove(mappingarrayindex);
	iItems.Compress();
	}  //lint !e1746 Suppress parameter 'aSwpKey' could be made const reference
#endif

/**
Find a file name for the supplied swp
@param aSwpKey The swp key to search for in the mapping
@return A descriptor containing the file name associated with the swp
@leave KErrNotFound if there is no item matching this swp

*/
const TDesC& CSsmSwpPolicyMap::FilenameL(TUint aSwpKey) const
	{
	const CSsmSwpPolicyMapping* found = MappingL(aSwpKey);
	return found->Filename();
	}  //lint !e1746 Suppress parameter 'aSwpKey' could be made const reference


/**
Find a mapping object for the supplied swp
@param aSwp The swp key to search for
@return The mapping object corresponding to the swp
@leave KErrNotFound if there is no item matching this swp

*/
const CSsmSwpPolicyMapping* CSsmSwpPolicyMap::MappingL(TUint aSwpKey) const
	{
	CSsmSwpPolicyMapping* search = CSsmSwpPolicyMapping::NewLC(aSwpKey, KNullDesC);
	TInt pos = iItems.FindInOrderL(search, bySwp);
	CleanupStack::PopAndDestroy(search);
	const CSsmSwpPolicyMapping* const& found = iItems[pos];
	return found;
	}  //lint !e1746 Suppress parameter 'aSwpKey' could be made const reference


