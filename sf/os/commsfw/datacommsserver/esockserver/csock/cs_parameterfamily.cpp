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
// Implementation for RParameterFamily and RParameterFamilyBundle classes
// 
//
//

/**
 @file
 @internalTechnology
*/

#include <comms-infras/es_parameterfamily.h>
#include <comms-infras/es_parameterbundle.h>
#include <es_sock.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCSckCSPrmtF, "ESockCSckCSPrmtF");
#endif



using namespace Meta;

EXPORT_C void RParameterFamily::CreateL(RParameterFamilyBundle& aBundle, TUint32 aFamilyId)
/** Creates a new parameter set family. This class is used as a container for a number of parameter
    sets that make up a family. It is a specific instantiatable class and therefore creates
	an instance of CParameterFamily without using ECOM.

@param aBundle Family container (bundle) that this family is to be added to
@exception KErrInUse if the container has already been prepared for use, KErrArgument if
aBundle is invalid, or another system-wide error code
*/
	{
	if(iContainer)
		{
		User::Leave(KErrInUse);
		}
	CParameterBundleBase* bundle = aBundle.iBundle;
	if(!bundle)
		{
		User::Leave(KErrArgument);
		}
	iContainer = CParameterSetContainer::NewL(*bundle,aFamilyId);
	}

EXPORT_C void RParameterFamily::LoadL(RParameterFamilyBundle& aBundle, TPtrC8& aBuffer)
	{
	if(iContainer)
		{
		User::Leave(KErrInUse);
		}
	CParameterBundleBase* bundle = aBundle.iBundle;
	if(!bundle)
		{
		User::Leave(KErrArgument);
		}
	iContainer = CParameterSetContainer::LoadL(*bundle,aBuffer);
	}





EXPORT_C void RParameterFamily::Destroy()
/** Sub-connection parameter family destructor. It cleans up the arrays deleting all
    the parameter sets it owns
*/
	{
	delete iContainer;
	iContainer = NULL;
	}


/**
Adds a parameter set to a sub-connection parameter set family.

@param aExtensionSet Parameter Set to be added to the family (family takes ownership)
@param aType The type of the set (requested, acceptable or granted)
@exception leaves with KErrNoMemory in out of memory conditions, or KErrNotReady if the parameter
family hasn't been prepared with LoadL or CreateL, or KErrArgument if aSet is invalid
*/
EXPORT_C void RParameterFamily::AddParameterSetL(XParameterSetBase* aSet, TParameterSetType aType)
	{
	User::LeaveIfError(AddParameterSet(aSet, aType));
	}

/**
Adds a parameter set to a sub-connection parameter set family.

@param aExtensionSet Parameter Set to be added to the family (family takes ownership)
@param aType The type of the set (requested, acceptable or granted)
@returns Returns KErrNoMemory in out of memory conditions, or KErrNotReady if the parameter
family hasn't been prepared with LoadL or CreateL, or KErrArgument if aSet is invalid
*/
EXPORT_C TInt RParameterFamily::AddParameterSet(XParameterSetBase* aSet, TParameterSetType aType)
	{
	if (!aSet)
		{
		return KErrArgument;
		}

	if (IsNull())
		{
		return KErrNotReady;
		}
	
	TInt index = aType;
	for( ; index < iContainer->CountParameterSets() ; index += ENumValues)
		{
		if (iContainer->GetParameterSet(index) == NULL)
			{
			return iContainer->ReplaceParameterSet(index, aSet);
			}
		}

	// if we've got here we have to expand the array as there's no null slots to use
	TInt err;
	err = iContainer->GrowToFit(index + 1);
	if (err == KErrNone)
		{
		err = iContainer->ReplaceParameterSet(index, aSet);
		}
	return err;
	}


EXPORT_C XParameterSetBase* RParameterFamily::GetParameterSetAtIndex(TUint aIndex, TParameterSetType aType)
	{
	XParameterSetBase* result = NULL;
	if(iContainer)
		{
		TInt index = (aIndex * ENumValues) + aType;
		if(index < iContainer->CountParameterSets())
			{
			result = iContainer->GetParameterSet(index);
			}
		}
	return result;
	}


EXPORT_C TInt RParameterFamily::CountParameterSets()
	{
	return (iContainer?iContainer->CountParameterSets():0);
	}

// returns the size (number of elements needed to span from the first element of type aType to
//  the last such element with a value)
EXPORT_C TInt RParameterFamily::CountParameterSets(TParameterSetType aType)
	{
	// work out the allocated size of the array for this type
	TInt size = ((iContainer?iContainer->CountParameterSets():0) + (ENumValues-aType-1)) / 3;

	for(TInt i=size-1; i>=0; --i)
		{
		TInt index = (i * ENumValues) + aType;
		XParameterSetBase* ps = iContainer->GetParameterSet(index);
		if(ps)
			{
			return size;
			}
		}
	return 0;
	}


EXPORT_C XParameterSetBase* RParameterFamily::FindParameterSet(STypeId aSetId, TParameterSetType aType)
	{
	TInt index = aType;
	for( ; index < (iContainer?iContainer->CountParameterSets():0) ; index += ENumValues)
		{
		XParameterSetBase* thisSet = iContainer->GetParameterSet(index);
		if(thisSet && thisSet->IsTypeOf(aSetId))
			{
			return thisSet;
			}
		}
	return NULL; // not found
	}



/**
Removes a parameter set from a parameter set family.

@param aSetToRemove Parameter Set to be removed from the family returning ownership to the calling method
@param aType The type of the set (requested, acceptable or granted)
@return Returns with KErrNotFound if set is not found, or KErrNotReady if the parameter
family hasn't been prepared with LoadL or CreateL
*/
EXPORT_C TInt RParameterFamily::RemoveParameterSet(XParameterSetBase* aSetToRemove, TParameterSetType aType)
	{
	if (IsNull())
		{
		return KErrNotReady;
		}

	TInt index = aType;
	for( ; index < iContainer->CountParameterSets() ; index += ENumValues)
		{
		if(iContainer->GetParameterSet(index) == aSetToRemove)
			{
			XParameterSetBase* ps = iContainer->RemoveParameterSet(index);
			ASSERT(ps && ps == aSetToRemove);
			(void)ps; // avoid unused variable warning in urel
			return KErrNone;
			}
		}

	return KErrNotFound;
	}


/**
Removes a parameter set from a parameter set family and deletes it

@param aSetToRemove Parameter Set to be removed from the family 
@param aType The type of the set (requested, acceptable or granted)
@return Returns with KErrNotFound if set is not found, or KErrNotReady if the parameter
family hasn't been prepared with LoadL or CreateL
*/
EXPORT_C TInt RParameterFamily::RemoveAndDestroyParameterSet(XParameterSetBase* aSetToRemove, TParameterSetType aType)
	{
	TInt err = RemoveParameterSet(aSetToRemove, aType);
	if (err == KErrNone)
		{
		delete aSetToRemove;
		}
	return err;
	}



EXPORT_C TUint RParameterFamily::Length() const
/** Calculates the length of buffer required to serialise this parameter family.

@return Length of buffer required.
*/
	{
	return iContainer ? iContainer->Length() : 0;
	}


EXPORT_C TInt RParameterFamily::Store(TDes8& aDes) const
/** Instructs this sub-connection family to create a serialised version of itself
    and append it to the buffer that has been passed.

@param aDes Buffer to append the serialised object to
@return KErrNone if successful, otherwise system wide error (e.g. KErrOverflow if the descriptor is too small)
If unsuccessful, there is no guarantee as to the buffer or data stored in it.
*/
	{
	return iContainer ? iContainer->Store(aDes) : KErrNotReady;
	}


EXPORT_C void RParameterFamily::ClearAllParameters(TParameterSetType aType)
/** Clears (removes and deletes) all parameter sets with the type specified

@param aType The type of the set to be deleted(requested, acceptable or granted)
*/
	{
	TInt index = aType;
	for( ; index < iContainer->CountParameterSets() ; index += ENumValues)
		{
		TRAP_IGNORE(iContainer->ReplaceParameterSetL(index,NULL));
		}
	}


EXPORT_C void RParameterFamily::DeleteParameterSetL(TInt aIndex)
	{
    User::LeaveIfError(DeleteParameterSet(aIndex));
	}

EXPORT_C TInt RParameterFamily::DeleteParameterSet(TInt aIndex)
	{
	if(iContainer)
		{
		return iContainer->DeleteParameterSet(aIndex);
		}
	else
		{
		return KErrNotReady;
		}
	}

EXPORT_C TInt RParameterFamily::ClearParameterSetPointer(TInt aIndex, TParameterSetType aType)
	{
	if(iContainer)
		{
		iContainer->ClearParameterSetPointer(aIndex * ENumValues + aType);
		return KErrNone;
		}
	else
		return KErrNotReady;
	}

EXPORT_C TUint32 RParameterFamily::Id() const
/**
Fetch the identity of the sub-connection parameter family
*/
	{
	return (iContainer == NULL) ? 0 : iContainer->Id();
	}

EXPORT_C RParameterFamily RParameterFamilyBundle::CreateFamilyL(TUint32 aFamilyId)
	{
	if(iBundle == NULL)
		{
		User::Leave(KErrNotReady);
		}
	RParameterFamily newFamily;
	newFamily.CreateL(*this,aFamilyId);
	return newFamily;
	}


EXPORT_C RParameterFamily RParameterFamilyBundle::GetFamilyAtIndex(TUint aIndex)
	{
	RParameterFamily result;
	if(iBundle != NULL)
		{
		CParameterSetContainer* psc = iBundle->GetParamSetContainer(aIndex);
		if(psc != NULL)
			{
			result.iContainer = psc;
			}
		}
	return result;
	}

EXPORT_C RParameterFamily RParameterFamilyBundle::FindFamily(TUint32 aFamilyId)
/** Search the sub-connection parameter bundle for a parameter family

@param aFamilyId Id of the family to match against
@return Matching family or NULL pointer if not found
*/
	{
	RParameterFamily result;
	if(iBundle != NULL)
		{
		CParameterSetContainer* psc = iBundle->FindParamSetContainer(aFamilyId);
		if(psc != NULL)
			{
			result.iContainer = psc;
			}
		}
	return result;
	}


EXPORT_C void RParameterFamilyBundle::ClearAllParameters(RParameterFamily::TParameterSetType aType)
/** Clears (removes and deletes) all parameter sets with the type specified (delegated to the families)

@param aType The type of the set to be deleted(requested, acceptable or granted)
*/
	{
	if(iBundle == NULL)
		{
		return;
		}

	TUint length = iBundle->CountParamSetContainers();
	RParameterFamily pf;
	for (TUint i = 0; i < length; ++i)
		{
		pf.iContainer = iBundle->GetParamSetContainer(i);
		pf.ClearAllParameters(aType);
		}
	}

EXPORT_C TInt RParameterFamilyBundle::CountParameterFamilies() const
	{
	return iBundle->CountParamSetContainers();
	}

EXPORT_C TInt RParameterFamilyBundle::DeleteFamilyAtIndex(TInt aIndex)
	{
	if (aIndex < iBundle->CountParamSetContainers())
		{
		CParameterSetContainer* aFamily = iBundle->GetParamSetContainer (aIndex);
		delete aFamily;
		RemoveFamilyAtIndex (aIndex);
		return KErrNone;
		}

	return KErrNotFound;
	}

EXPORT_C void RParameterFamilyBundle::RemoveFamilyAtIndex(TInt aIndex)
	{
	__ASSERT_DEBUG (aIndex < iBundle->CountParamSetContainers (), User::Invariant());
	iBundle->RemoveParamSetContainer(aIndex);
	}

RParameterFamilyBundle& RParameterFamilyBundle::operator=(const RParameterFamilyBundle& /*aBundle*/)
/** Assignment operator
*/
	{
	__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockCSckCSPrmtF, 1));//can be Unreachable in future
	return *this;
	}



RParameterFamilyBundle::RParameterFamilyBundle(const RParameterFamilyBundle& /*aBundle*/)
/** Copy Constructor
*/
	{
	__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockCSckCSPrmtF, 2));//can be Unreachable in future
	}


EXPORT_C /* static */ void RParameterFamilyBundleC::CreateL()
	{
	iBundle = CGenericParameterBundle::NewL();
	}

EXPORT_C /* static */ void RParameterFamilyBundleC::LoadL(TDesC8& aDes)
/** Creates a new parameter family bundle from a buffer containing the serialised object.

@param aDes Buffer containing the serialised object information
@return a pointer to a parameter family bundle if successful, otherwise leaves with system error code.
*/
	{
	iBundle = CGenericParameterBundle::LoadL(aDes);
	}
	
EXPORT_C TInt RParameterFamilyBundleC::Load(const TDesC8& aDes)
	{
	return iBundle->Load(aDes);
	}

EXPORT_C void RParameterFamilyBundleC::Destroy()
/** Sub-connection parameter bundle destructor, clear up the families
*/
	{
	delete iBundle;
	iBundle=NULL;
	}


EXPORT_C TUint RParameterFamilyBundleC::Length() const
/** Calculates the length of buffer required to serialise this parameter set bundle.

@return Length of buffer required.
*/
	{
	return iBundle ? iBundle->Length() : 0;
	}


EXPORT_C TInt RParameterFamilyBundleC::Store(TDes8& aDes) const
/** Instructs this parameter family bundle to create a serilised version of itself
    and append it to the buffer that has been passed.

@param aDes Buffer to append the serialised object to
@return KErrNone if successful, otherwise system wide error (e.g. EKrrOverflow if the descriptor is too small)
If unsuccessful, there is no guarantee as to the buffer or data stored in it.
*/
	{
	return iBundle ? iBundle->Store(aDes) : KErrNotReady;
	}


EXPORT_C const RParameterFamily RParameterFamilyBundleC::GetFamilyAtIndex(TUint aIndex) const
	{
	RParameterFamily result;
	if(iBundle != NULL)
		{
		CParameterSetContainer* psc = iBundle->GetParamSetContainer(aIndex);
		if(psc != NULL)
			{
			result.iContainer = psc;
			}
		}
	return result;
	}

EXPORT_C const RParameterFamily RParameterFamilyBundleC::FindFamily(TUint32 aFamilyId) const
/** Search the sub-connection parameter bundle for a parameter family

@param aFamilyId Id of the family to match against
@return Matching family or NULL pointer if not found
*/
	{
	RParameterFamily result;
	if(iBundle != NULL)
		{
		CParameterSetContainer* psc = iBundle->FindParamSetContainer(aFamilyId);
		if(psc != NULL)
			{
			result.iContainer = psc;
			}
		}
	return result;
	}



RParameterFamilyBundleC& RParameterFamilyBundleC::operator=(const RParameterFamilyBundleC& /*aBundle*/)
/** Assignment operator
*/
	{
	__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockCSckCSPrmtF, 3));//can be Unreachable in future
	return *this;
	}



RParameterFamilyBundleC::RParameterFamilyBundleC(const RParameterFamilyBundleC& /*aBundle*/)
/** Copy Constructor
*/
	{
	__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockCSckCSPrmtF, 4));//can be Unreachable in future
	}




