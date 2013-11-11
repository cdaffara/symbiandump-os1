// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file 
 @internalTechnology
*/

#include <elements/metacontainer.h>
#include <elements/metadata.h>

namespace Meta
{
const TInt KContainerUid = 0x10205A04;
}

using namespace Meta;


EXPORT_C RMetaDataContainerBase::RMetaDataContainerBase()
/**
 * Constructor
 */
    {	
    }

EXPORT_C STypeId RMetaDataContainerBase::GetTypeId() const
	{
	return STypeId::CreateSTypeId( KContainerUid, KNetMetaTypeAny );
	}

EXPORT_C void RMetaDataContainerBase::Copy(const RMetaDataContainerBase& aMetaContainer)
/**
 * Copies a content of a meta container into this container.
 * Copies only pointers (does not do a deep copy).
 */
	{
	TUint count = aMetaContainer.Count();

	for (TUint i = 0; i < count; ++i)
		{
		Append(aMetaContainer[i]);
		}
	}

EXPORT_C TInt RMetaDataContainerBase::Store(TDes8& aDes) const
/**
 * Stores the content of this container in a descriptor.
 */
    {
	if (aDes.MaxLength()-aDes.Length() < this->Length())
		{
		return KErrArgument;
		}

	const STypeId& typeId = GetTypeId();
	aDes.Append((TUint8*)&typeId,sizeof(STypeId)); //store TID

    TInt count = Count();
	TInt length = Length();
	aDes.Append((TUint8*)&length, sizeof(TInt32));	// Store Length of container

    for (TInt i = 0; i < count; ++i)
        {
		SMetaData* element = (*this)[i];
		if (element)
			{
	        TInt ret = element->Store(aDes);    //store the object
			if (ret != KErrNone)
				{
				return ret;
				}
			}
		else
			{
			if (aDes.MaxLength() - aDes.Length() < (TInt) sizeof(TUint32))
				{
				return KErrOverflow;
				}
			const TUint32 KNullInt = 0;
			aDes.Append((TUint8*)&KNullInt, sizeof(TUint32));
			}
        }
	return KErrNone;
    }

EXPORT_C TInt RMetaDataContainerBase::Load(TPtrC8& aDes)
/**
 * Loads a content from a descriptor into this container.
 */
	{
	ResetAndDestroy();
    TInt32 containerLength;
    Mem::Copy(&containerLength, aDes.Ptr() ,sizeof(TInt32));

	if (containerLength < 0 || (containerLength - sizeof(STypeId))> aDes.Length())
		{
		// Fatal error, descriptor is corrupt
		return KErrArgument;
		}

	// Construct a buffer to work on for this container
	TPtrC8 containerBuffer(aDes.Ptr() + sizeof(TInt32),	
		containerLength - sizeof (STypeId) - sizeof (TInt32));

	// Move descriptor pointer past the container buffer
	aDes.Set(aDes.Ptr() + containerLength - sizeof (STypeId), aDes.Length() - containerLength + sizeof (STypeId));

	while (containerBuffer.Length() > 0)
		{
		if (containerBuffer.Length() < (TInt) sizeof(TUint32))
			{
			return KErrArgument;
			}

		// Check for a NULL pointer when stored (First four bytes == 0)
		TUint32 firstFourBytes;
        Mem::Copy(&firstFourBytes, containerBuffer.Ptr(), sizeof(TUint32));
		if (firstFourBytes == 0)
			{
			// The pointer was NULL when it was stored
			containerBuffer.Set(containerBuffer.Ptr() + sizeof(TUint32), containerBuffer.Length() - sizeof(TUint32));
			TRAPD(error, AppendL(NULL));
			if (error != KErrNone)
				{
				return error;
				}
			}
		else
			{
			SMetaData* containerItem = NULL;
			TRAPD(error, containerItem = LoadElementL(containerBuffer));
			if (error == KErrNone)
				{
				TRAP(error, AppendL(containerItem));
				}
			if (error != KErrNone && error != KErrNotFound)
				{
				delete containerItem;
				return error;
				}
			}
		}
	return KErrNone;
	}

EXPORT_C TInt RMetaDataContainerBase::Length() const
/**
 * Returns the length of the content of this container if it was to be stored.
 */
	{
    TInt length = 0, count = this->Count();
    for (TInt i = 0; i < count; ++i)
		{
		SMetaData* element = (*this)[i];
		if (element)
			{
			// UID + Object Type Id + Length Parameter + Length of Data
			length += element->Length();
			}
		else
			{
			// Add length of null word (Used to represent a NULL pointer)
			length += sizeof(TUint32);
			}
		}
	return length + sizeof(TInt32) + sizeof(STypeId);
	}



