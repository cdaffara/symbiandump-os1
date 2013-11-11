// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @released since 9.5
*/

#include <comms-infras/es_parameterbundle.h>
#include "es_flog.h"
#include <es_sock.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCSckCSPrmtB, "ESockCSckCSPrmtB");
#endif



using namespace Meta;





// PARAMETER SET CONTAINER
// -----------------------
/**
Creates a new parameter set container. This class is used as a container for a number of parameter
sets (generic and extension) that are associated with each other. It is a specific instantiatable class and therefore creates
an instance of CParameterSetContainer without using ECOM.

@param aBundle Bundle that this container is to be added to
@param aContainerId identifier for the specific container instance
@return a pointer to a parameter set container if successful, otherwise leaves with system error code.
*/
EXPORT_C CParameterSetContainer* CParameterSetContainer::NewL(CParameterBundleBase& aBundle, TUint32 aContainerId /* =0 */)
	{
	CParameterSetContainer* container = new (ELeave) CParameterSetContainer(aContainerId);
	CleanupStack::PushL(container);
	LOG( ESockLog::Printf(_L8("CParameterSetContainer [this=%08x]:\tCreated() tid %d"), container, (TUint)RThread().Id()));
	container->ConstructL(aBundle);
	CleanupStack::Pop(container);
	return container;
	}


/**
Creates a new parameter set container. This class is used as a container for a number of parameter
sets (generic and extension) that are associated with each other. It is a specific instantiatable class and therefore creates
an instance of CParameterSetContainer without using ECOM.

@param aContainerId identifier for the specific container instance
@return a pointer to a parameter set container if successful, otherwise leaves with system error code.
*/
EXPORT_C CParameterSetContainer* CParameterSetContainer::NewL(TUint32 aContainerId /* =0 */)
	{
	CParameterSetContainer* container = new (ELeave) CParameterSetContainer(aContainerId);
	CleanupStack::PushL(container);
	LOG( ESockLog::Printf(_L8("CParameterSetContainer [this=%08x]:\tCreated() tid %d"), container, (TUint)RThread().Id()));
	CleanupStack::Pop(container);
	return container;
	}


/**
Creates a new parameter set container from a buffer containing the serialised object.

@param aBundle Bundle that this container is to be added to
@param aBuffer Buffer containing the serialised object information
@return a pointer to a parameter set container if successful, otherwise leaves with system error code.
*/
EXPORT_C CParameterSetContainer* CParameterSetContainer::LoadL(CParameterBundleBase& aBundle, TPtrC8& aBuffer)
	{
	CParameterSetContainer* container = new (ELeave) CParameterSetContainer(0);
	CleanupStack::PushL(container);
	LOG( ESockLog::Printf(_L8("CParameterSetContainer [this=%08x]:\tCreated() tid %d"), container, (TUint)RThread().Id()));
	container->ConstructL(aBundle);
	User::LeaveIfError(container->Load(aBuffer));
	CleanupStack::Pop(container);
	return container;
	}


/**
Creates a new parameter set container from a buffer containing the serialised object.

@param aBuffer Buffer containing the serialised object information
@return a pointer to a parameter set container if successful, otherwise leaves with system error code.
*/
EXPORT_C CParameterSetContainer* CParameterSetContainer::LoadL(TPtrC8& aBuffer)
	{
	CParameterSetContainer* container = new (ELeave) CParameterSetContainer(0);
	CleanupStack::PushL(container);
	LOG( ESockLog::Printf(_L8("CParameterSetContainer [this=%08x]:\tCreated() tid %d"), container, (TUint)RThread().Id()));
	User::LeaveIfError(container->Load(aBuffer));
	CleanupStack::Pop(container);
	return container;
	}


EXPORT_C /*virtual*/ CParameterSetContainer::~CParameterSetContainer()
	{
	LOG( ESockLog::Printf(_L8("CParameterSetContainer [this=%08x]:\tDestroyed() tid %d"), this, (TUint)RThread().Id()));
	iParameterSets.ResetAndDestroy();
	}


EXPORT_C void CParameterSetContainer::AddParameterSetL(XParameterSetBase* aParameterSet)
	{
	__ASSERT_DEBUG(FindParameterSet(aParameterSet->GetTypeId()) == NULL, User::Panic(KSpecAssert_ESockCSckCSPrmtB, 1)); // should only be 1 set of each type in the container
	ASSERT(FindParameterSet(aParameterSet->GetTypeId()) == NULL); // should only be 1 set of each type in the container
	iParameterSets.AppendL(aParameterSet);
	}

EXPORT_C TInt CParameterSetContainer::AddParameterSet(XParameterSetBase* aParameterSet)
	{
	ASSERT(FindParameterSet(aParameterSet->GetTypeId()) == NULL); // should only be 1 set of each type in the container
	return iParameterSets.Append(aParameterSet);
	}


/**
Searches a parameter set container for a parameter set.
@param aSetId The STypeId of the set
@return Pointer to the set if found, otherwise a NULL pointer.
*/
EXPORT_C XParameterSetBase* CParameterSetContainer::FindParameterSet(const STypeId& aSetId)
	{
	TUint length = iParameterSets.Count();
	for (TUint i = 0; i < length; ++i)
		{
		if (iParameterSets[i]->GetTypeId() == aSetId)
			{
			return static_cast<XParameterSetBase*>(iParameterSets[i]);
			}
		}
	return NULL; // not found
	}


EXPORT_C TBool CParameterSetContainer::FindParameterSet(const XParameterSetBase& aRhs)
	{
	TUint length = iParameterSets.Count();
	for (TUint i = 0; i < length; ++i)
		{
		if (iParameterSets[i] == &aRhs)
			{
			return ETrue;
			}
		}
	return EFalse; // not found
	}


/**
Searches a parameter set container for a parameter set.
@param aSetId The STypeId of the set
@return Pointer to the set if found, otherwise a NULL pointer.
*/
EXPORT_C XParameterSetBase* CParameterSetContainer::GetParameterSet(TInt aIndex)
	{
	TInt length = iParameterSets.Count();

	return (aIndex >= length || aIndex < 0) ? 0 : static_cast<XParameterSetBase*>(iParameterSets[aIndex]);
	}


/**
Calculates the length of buffer required to serialise this parameter set container.
@return Length of buffer required.
*/
EXPORT_C /*virtual*/ TInt CParameterSetContainer::Length() const
	{
	return sizeof(TInt32) + iParameterSets.Length();
	}


/**
Instructs this container to create a serialised version of itself
and append it to the buffer that has been passed.

@param aDes Buffer to append the serialised object to
@return KErrNone if successful, otherwise system wide error (e.g. KErrOverflow if the descriptor is too small)
If unsuccessful, there is no guarantee as to the buffer or data stored in it.
*/
EXPORT_C /*virtual*/ TInt CParameterSetContainer::Store(TDes8& aBuffer) const
	{
	// Needs to be at least this size to store len and containerId ptrs
	// aDes can only grow upto MaxLength.
	if (aBuffer.MaxLength() < sizeof(TUint32))
		{
		return KErrOverflow;
		}

	aBuffer.Append((TUint8*)&iContainerId, sizeof(TUint32));
	return iParameterSets.Store(aBuffer);
	}


/**
Set up the container with empty sets

@param aBundle Bundle (parent container) that this new parameter set container is to be added to
@exception leaves with KErrNoMemory in out of memory conditions
*/
EXPORT_C void CParameterSetContainer::ConstructL(CParameterBundleBase& aBundle)
	{
	aBundle.AddParamSetContainerL(*this);  // need specific R type.. nngh
	}


/**
Instructs this container to set its members based on the serialised data
in the buffer passed.

@param aBuffer Buffer containing the serialised container object
@return KErrNone if successful, otherwise system wide error
*/
EXPORT_C /*virtual*/ TInt CParameterSetContainer::Load(TPtrC8& aBuffer)
	{
	// Guard to ensure length of data, pointed to by the descriptor, is not too small,
	// I.e. we are pointing to something!
	if (aBuffer.Length() < sizeof(TUint32))
		{
		return KErrArgument;
		}

 	// Obtain the ContainerId value of the data pointed to by the descriptor
	Mem::Copy(&iContainerId, aBuffer.Ptr(),sizeof(TInt32));
	aBuffer.Set(aBuffer.Ptr() + sizeof(TInt32), aBuffer.Length() - sizeof(TInt32));

	// Load the parameter sets
	// -----------------------
	// This type Id is the type id of the RMetaDataContainerBase, it needs
	// to stripped by its containing object due to an asymetric Load/Store.
	// This defect has so far been ignored/dismissed due to the potential
	// large amount of work to fix and possible wide spread BC issues.
	TInt ret = iParameterSets.GetTypeId().Check(aBuffer);
	if (ret == KErrNone)
		{
		ret = iParameterSets.Load(aBuffer);
		}

	if (ret != KErrNone)
		{
		return ret;
		}

	return KErrNone;
	}


EXPORT_C void CParameterSetContainer::ReplaceParameterSetL(TInt aIndex, XParameterSetBase* aNewSet)
	{
	User::LeaveIfError(ReplaceParameterSet(aIndex, aNewSet));
	}

EXPORT_C TInt CParameterSetContainer::ReplaceParameterSet(TInt aIndex, XParameterSetBase* aNewSet)
	{
	if (aIndex > iParameterSets.Count())
		{
		return KErrNotFound;
		}

	XParameterSetBase* oldSet = static_cast<XParameterSetBase*>(iParameterSets[aIndex]);
	if (oldSet == aNewSet)
		{
		// same thing.. so do nothing
		return KErrNone;
		}

	delete oldSet;
	iParameterSets[aIndex] = aNewSet;
	return KErrNone;
	}


EXPORT_C void CParameterSetContainer::DeleteParameterSetL(TInt aIndex)
	{
	User::LeaveIfError(DeleteParameterSet(aIndex));
	}

EXPORT_C TInt CParameterSetContainer::DeleteParameterSet(TInt aIndex)
	{
	if(aIndex > iParameterSets.Count())
		{
		return KErrNotFound;
		}

	delete iParameterSets[aIndex];
	iParameterSets.Remove(aIndex);
	return KErrNone;
	}


EXPORT_C XParameterSetBase* CParameterSetContainer::RemoveParameterSet(TInt aIndex)
	{
	if(aIndex > iParameterSets.Count())
		{
		return NULL;
		}

	XParameterSetBase* ps = static_cast<XParameterSetBase*>(iParameterSets[aIndex]);
	iParameterSets.Remove(aIndex);
	return ps;
	}


EXPORT_C void CParameterSetContainer::GrowToFitL(TInt aMinSize)
	{
	User::LeaveIfError(GrowToFit(aMinSize));
	}

EXPORT_C TInt CParameterSetContainer::GrowToFit(TInt aMinSize)
	{
	if(iParameterSets.Count() < aMinSize)
		{
		TInt err = iParameterSets.Reserve(aMinSize);
		if (err != KErrNone)
			{
			return err;
			}

		while(iParameterSets.Count() < aMinSize)
			{
			// Append can't fail - we reserved the space
			iParameterSets.Append(NULL);
			}
		}
	return KErrNone;
	}




// PARAMETER BUNDLE

EXPORT_C /*virtual*/ CParameterBundleBase::~CParameterBundleBase()
	{
	iSetContainers.ResetAndDestroy();
	}


/**
Calculates the length of buffer required to serialise this parameter set bundle.
@return Length of buffer required.
*/
EXPORT_C TUint CParameterBundleBase::Length() const
	{
	TInt length = 0;
	TUint count = iSetContainers.Count();
	for (TUint i = 0; i < count; ++i)
		{
		length += iSetContainers[i]->Length();
		}
	return length;
	}


/**
Instructs this sub-connection parameter bundle to create a serilised version of itself
and append it to the buffer that has been passed.

@param aDes Buffer to append the serialised object to
@return KErrNone if successful, otherwise system wide error (e.g. EKrrOverflow if the descriptor is too small)
If unsuccessful, there is no guarantee as to the buffer or data stored in it.
*/
EXPORT_C TInt CParameterBundleBase::Store(TDes8& aDes) const
	{
	TUint length = iSetContainers.Count();

	for (TUint i = 0; i < length; ++i)
		{
		TInt ret = iSetContainers[i]->Store(aDes);
		if (ret != KErrNone)
			{
			aDes.Zero();
			return ret;
			}
		}
	return KErrNone;
	}


/**
Add a parameter set container to the bundle.
@param aContainer Container to be added (bundle takes ownership)
@exception Leaves with KErrNoMemory in out of memory conditions
*/
EXPORT_C void CParameterBundleBase::AddParamSetContainerL(CParameterSetContainer& aContainer)
	{
	iSetContainers.AppendL(&aContainer);
	}

/**
Add a parameter set container to the bundle.
@param aContainer Container to be added (bundle takes ownership)
@return Returns with KErrNoMemory in out of memory conditions
*/
EXPORT_C TInt CParameterBundleBase::AddParamSetContainer(CParameterSetContainer& aContainer)
	{
	return iSetContainers.Append(&aContainer);
	}

/**
Remove a parameter set container from the bundle, returning ownership to the calling method
@param aIndex index of the container to remove
@return The removed parameter set container
*/
EXPORT_C CParameterSetContainer* CParameterBundleBase::RemoveParamSetContainer(TInt aIndex)
	{
	CParameterSetContainer* psc = iSetContainers[aIndex];
	iSetContainers.Remove(aIndex);
	return psc;
	}


/**
Pick out the parameter set container at the given index in the bundle.
@param aIndex index of the container to get
@return Matching container or NULL pointer if index out of range
*/
EXPORT_C CParameterSetContainer* CParameterBundleBase::GetParamSetContainer(TInt aIndex)
	{
	TInt length = iSetContainers.Count();
	if(aIndex < 0 || aIndex >= length)
		{
		return NULL;
		}
	return iSetContainers[aIndex];
	}


/**
Search the parameter bundle for a parameter set container
@param aContainerId Id of the container to match against
@return Matching container or NULL pointer if not found
*/
EXPORT_C CParameterSetContainer* CParameterBundleBase::FindParamSetContainer(TUint32 aContainerId)
	{
	TUint length = iSetContainers.Count();
	for (TUint i = 0; i < length; ++i)
		{
		if (iSetContainers[i]->Id() == aContainerId)
			{
			return iSetContainers[i];
			}
		}
	return NULL;
	}


/**
Deep search the parameter bundle for a parameter set of given type
@param aType Id of the set type to match against
@return Matching set or NULL pointer if not found
*/
EXPORT_C XParameterSetBase* CParameterBundleBase::FindParameterSet(const Meta::STypeId& aType)
	{
	TUint length = iSetContainers.Count();
	for (TUint i = 0; i < length; ++i)
		{
		XParameterSetBase* ps = iSetContainers[i]->FindParameterSet(aType);
		if(ps)
			{
			return ps;
			}
		}
	return NULL;
	}


/**
Replaces a given parameter set container with another. The old parameter set container is deleted,
and the parameter bundle takes ownership of the new one
@param aOld A parameter set container that exists in the parameter bundle
@param aNew The parameter set container that will take the place of the old one
@exception Leaves with KErrNotFound if the parameter set container was not found in the bundle
*/
EXPORT_C void CParameterBundleBase::ReplaceParamSetContainerL(CParameterSetContainer* aOld, CParameterSetContainer* aNew)
	{
	User::LeaveIfError(ReplaceParamSetContainer(aOld, aNew));
	}

/**
Replaces a given parameter set container with another. The old parameter set container is deleted,
and the parameter bundle takes ownership of the new one
@param aOld A parameter set container that exists in the parameter bundle
@param aNew The parameter set container that will take the place of the old one
@return KErrNotFound if the parameter set container was not found in the bundle, otherwise KErrNone.
*/
EXPORT_C TInt CParameterBundleBase::ReplaceParamSetContainer(CParameterSetContainer* aOld, CParameterSetContainer* aNew)
	{
	if (!aOld)
		{
		// NULL containers will not exist in the array
		return KErrNotFound;
		}

	TUint length = iSetContainers.Count();
	for (TUint i = 0; i < length; ++i)
		{
		if (iSetContainers[i] == aOld)
			{
			delete aOld;
			iSetContainers[i] = aNew;
			return KErrNone;
			}
		}

	return KErrNotFound;
	}


