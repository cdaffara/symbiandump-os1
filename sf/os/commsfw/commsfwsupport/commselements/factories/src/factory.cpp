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

/**
 @file
 @internalComponent
*/

#include <ecom/ecom.h>
#include "factory.h"
#include "factory_log.h"
#include <elements/factorynotify.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemFactoriesfct, "ElemFactoriesfct");
#endif

using namespace Factories;
using namespace Messages;


EXPORT_C CFactoryBase* CFactoryContainer::FindFactory(TUid aFactoryUid) const
/** Search this factory container for a factory with a matching id

@param aFactoryUid Id of the factory the caller wishes to find
@return The matching factory or NULL if not found
*/
	{
	FACTORY_LOG((KFactoryTag, _L8("CFactoryContainer %08x:\tFindFactory(aId %08x)"), this, aFactoryUid.iUid ));
	TUint length = iFactories.Count();
	CFactoryBase* factory = NULL;
	for (TUint index = 0; index < length; ++index)
		{
		if (iFactories[index]->Uid() == aFactoryUid)
			{
			factory = iFactories[index];
			break;
			}
		}
	return factory;
	}

EXPORT_C CFactoryContainer::~CFactoryContainer()
/** Destructor - cleanup Factories (delete them all) */
	{
	iFactories.ResetAndDestroy();
	iFactories.Close();
	}

EXPORT_C CFactoryContainer::CFactoryContainer()
 	{
	}

EXPORT_C void CFactoryContainer::AddL(CFactoryBase& aFactory)
/** Add a factory to this container

@param aFactory Factory to be added to the container
*/
	{
	__ASSERT_DEBUG(iFactories.Find(&aFactory) == KErrNotFound, User::Panic(KSpecAssert_ElemFactoriesfct, 1));
	iFactories.AppendL(&aFactory);
	}

EXPORT_C CFactoryBase* CFactoryContainer::Factory( TInt aIndex ) const
	{
	return aIndex >= 0 && aIndex < iFactories.Count() ? iFactories[aIndex] : NULL;
	}

EXPORT_C void CFactoryContainer::RegisterNotifierL( TUid aFactoryUid, const IFactoryNotify& aFactoryNotify )
/**	Registers a callback for factory container notification

@param aFactoryUid The Id of the factory that will be registered
@param aFactoryNotify The IConnectionFactoryNotify object that implements the callback
@exception Leaves with KErrArgument if the factory specified by aFactoryId is not found
@see IConnectionFactoryNotify */
	{
	CFactoryBase* factory = FindFactory(aFactoryUid);
	if ( !factory )
		{
		User::Leave( KErrArgument );
		}
	factory->RegisterNotifierL(aFactoryNotify);
	}

EXPORT_C NetInterfaces::TInterfaceControl* CFactoryContainer::FetchNodeInterfaceControlL(TInt aInterfaceId)
/**	Iterates through all the held factories and asks them whether they implment the requested interface.
As factory containers are not meant to be derived from, this implementation is an accepted
deviation from the original semantics of FetchNodeInterfaceControlL.

@param aInterfaceId The id of the requested interface extension.
@return A pointer to the interface control object.
@exception Leaves with KErrNotSupported if none of the factories implement the interface
@see NetInterfaces::TInterfaceControl */
    {
    TInt i = 0;
    CFactoryBase* fact = NULL;
    while ((fact = Factory(i++)) != NULL)
        {
        //this is a bit unfortunate as FetchInterfaceL is not v-fn but FetchNodeInterfaceControlL is
        //so CFactoryContainer could only overwrite FetchNodeInterfaceControlL. However to follow
        //TInterfaceControl class logic and get the right control we need to call the FetchInterfaceControlL
        //only in case the object actually implements the interface. TInterfaceControl::FetchInterfaceL only calls
        //FetchInterfaceControlL if registerd for particulat interface in the static list hence
        //FetchInterfaceControlL is not obliged to check aInterfaceId and there's no guarantie that
        //having interface control means that the intarface is supported at all.
    	if (fact->SupportsExtInterface(aInterfaceId))
        	{
            return fact->FetchExtInterfaceControlL(aInterfaceId);
        	};
        }
    User::Leave(KErrNotSupported);
    return NULL;
    }


EXPORT_C AFactoryObject* CFactoryContainer::FindObject(MFactoryQuery& aQuery) const
    {
    FACTORY_LOG((KFactoryTag, _L8("CFactoryContainer %08x::FindObject()"), this ));
	TUint length = iFactories.Count();
	AFactoryObject* factoryObj = NULL;
	for (TUint index = 0; index < length; ++index)
		{
		factoryObj = iFactories[index]->FindObject(aQuery);
		if (factoryObj)
			{
			break;
			}
		}
	return factoryObj;
    }

EXPORT_C CFactoryContainerNode::~CFactoryContainerNode()
/** Destructor - cleanup Factories (delete them all) */
	{
	NM_LOG_NODE_DESTROY(KFactoryTag, CFactoryContainerNode);
	}

EXPORT_C CFactoryContainerNode::CFactoryContainerNode()
:	CFactoryContainer(),
	ASimpleNodeIdBase()
 	{
 	NM_LOG_NODE_CREATE(KFactoryTag, CFactoryContainerNode);
	}

//==================================================================

EXPORT_C CFactoryBase* CFactoryBase::NewL(const TDesC8& aName, const TUid& aInterfaceUid, CFactoryContainer& aParentContainer)
/** Create a new instance of a factory (cnstructed via ECOM using the name and the interfaceUId specified

@param aName The name (string representation) of the factory
@param aInterfaceUid The interface Uid that ECOM should use to load the implementation
@param aParentContainer The factory container the factory should become a member of
@return A pointer to the new instance of the factory
@exception leaves with KErrNoMemory in out of memory conditions
*/
	{
	TUid destroyUid;
	TEComResolverParams params;

	params.SetDataType(aName);

#ifdef _DEBUG
	TAny* ptr = NULL;
	TRAPD(err, ptr = REComSession::CreateImplementationL(aInterfaceUid, destroyUid, reinterpret_cast<TAny*>(&aParentContainer), params));
	if (err != KErrNone)
		{
		FACTORY_LOG((KFactoryTag, _L8("ERROR %d creating ECOM implementation of '%S'"), err, &aName ));
		User::Leave(err);
		}
#else
	TAny *ptr = REComSession::CreateImplementationL(aInterfaceUid, destroyUid, reinterpret_cast<TAny*>(&aParentContainer), params);
#endif

	CFactoryBase *exPtr = reinterpret_cast<CFactoryBase*>(ptr);
	CleanupStack::PushL(exPtr);
	exPtr->ConstructL(destroyUid);
	CleanupStack::Pop();	// exPtr
	return exPtr;
	}

EXPORT_C CFactoryBase* CFactoryBase::NewL(const TUid& aImplementationUid, CFactoryContainer& aParentContainer)
/** Create a new instance of a factory (cnstructed via ECOM using the name and the interfaceUId specified

@param aImplementationUid The interface Uid that ECOM should use to load the implementation
@param aParentContainer The factory container the factory should become a member of
@return A pointer to the new instance of the factory
@exception leaves with KErrNoMemory in out of memory conditions
*/
	{
	TUid destroyUid;

#ifdef _DEBUG
	TAny* ptr = NULL;
	TRAPD(err, ptr = REComSession::CreateImplementationL(aImplementationUid, destroyUid, reinterpret_cast<TAny*>(&aParentContainer)));
	if (err != KErrNone)
		{
		FACTORY_LOG((KFactoryTag, _L8("ERROR %d creating ECOM implementation of '%08x'"), err, aImplementationUid.iUid ));
		User::Leave(err);
		}
#else
	TAny *ptr = REComSession::CreateImplementationL(aImplementationUid, destroyUid, reinterpret_cast<TAny*>(&aParentContainer));
#endif

	CFactoryBase *exPtr = reinterpret_cast<CFactoryBase*>(ptr);

	// Do some basic sanity checking for malformed plugins.  Those that haven't been recompiled
	// properly sometimes appear to have zero IDs.  This can cause misleading panics later on,
	// so trap this particular (contra)indication here.  Look at aImplementationUid to identity which one.
	 __ASSERT_DEBUG(exPtr->iFactoryUid != TUid::Null(), User::Panic(KSpecAssert_ElemFactoriesfct, 2));

	CleanupStack::PushL(exPtr);
	exPtr->ConstructL(destroyUid);
	CleanupStack::Pop();	// exPtr
	return exPtr;
	}

EXPORT_C void CFactoryBase::ConstructL(const TUid& aDestroyUid)
	{
	iDestroyUid = aDestroyUid;
	ConstructL();
	}

EXPORT_C void CFactoryBase::ConstructL()
	{
	iParentContainer.AddL(*this);
	}

EXPORT_C CFactoryBase::~CFactoryBase()
/** Destructor */
	{
	FACTORY_LOG_NODE_DESTROY(KFactoryTag, CFactoryBase);

	TUint length;
	do
		{
		length = iObjects.Count();
		if(length > 0)
			{
			iObjects[length - 1].iFactoryObject->DeleteMeNow();
			}
		} while(length > 0);
	iObjects.Close();
	iFactoryNotify.Close();
	
	if (iDestroyUid.iUid)
		{
		REComSession::DestroyedImplementation(iDestroyUid);
		}
	}

EXPORT_C CFactoryBase::CFactoryBase(TUid aFactoryUid, CFactoryContainer& aParentContainer)
/** Constructor */
:	iParentContainer(aParentContainer),
	iFactoryUid(aFactoryUid)
	{
	FACTORY_LOG_NODE_CREATE(KFactoryTag, CFactoryBase);
	}

TBool NotifierMatchFunction(const IFactoryNotify& aLhs, const IFactoryNotify& aRhs)
	{
	return (aLhs==aRhs);
	}

EXPORT_C void CFactoryBase::RegisterNotifierL( const IFactoryNotify& aFactoryNotify )
/**	Registers a callback for factory notification

@param aFactoryNotify The IFactoryNotify object that implements the callback
@see IFactoryNotify */
	{
	__ASSERT_DEBUG( iFactoryNotify.Find(aFactoryNotify/*,TIdentityRelation<IFactoryNotify>(NotifierMatchFunction)*/) == KErrNotFound, User::Panic(KSpecAssert_ElemFactoriesfct, 3));
	iFactoryNotify.AppendL(aFactoryNotify);
	}

EXPORT_C void CFactoryBase::DeRegisterNotifier( const IFactoryNotify& aFactoryNotify )
/**	De-registers a callback for factory notification

@param aFactoryNotify The IFactoryNotify object that implements the callback
@see IFactoryNotify
@see CFactoryBase::RegisterNotifierL */
	{
	TInt n = iFactoryNotify.Find(aFactoryNotify/*,TIdentityRelation<IFactoryNotify>(NotifierMatchFunction)*/);
	if (n != KErrNotFound)
		{
		iFactoryNotify.Remove(n);
		}
	}

EXPORT_C void CFactoryBase::SignalDeletion( AFactoryObject& aObject )
	{
	for (TInt i = iFactoryNotify.Count() - 1; i >= 0; i--)
		{
		iFactoryNotify[i].NotifyDeletion(aObject,*this);
		}
	}

EXPORT_C TInt CFactoryBase::SignalCreation( AFactoryObject& aObject )
	{
	TInt err = KErrNone;
	for (TInt i = iFactoryNotify.Count() - 1; err == KErrNone && i >= 0; i--)
		{
		err = iFactoryNotify[i].NotifyCreation(aObject,*this);
		}
	return err;
	}

EXPORT_C void CFactoryBase::RemoveManagedObject(AFactoryObject& aObject)
	{
	TUint length = iObjects.Count();
	for (TInt index = length - 1; index >= 0; --index)
		{
		AFactoryObject* obj = &aObject;
		if (iObjects[index].iFactoryObject == obj)
			{
			RemoveManagedObject(index);
			}
		}
	}

void CFactoryBase::RemoveManagedObject(TInt aIndex)
    {
    FACTORY_LOG((KFactoryTag, _L8("CFactoryBase %08x:\tRemoveManagedObject(object %08x) factory Uid %08x"), this, iObjects[aIndex].iFactoryObject, Uid().iUid ));
    if (!(iObjects[aIndex].iFlag & TFactoryObjectEntry::EBeingDeleted))
        {
        //Signalling deletion happens as soon as deltion is advertised.
        //AFactoryObject _can_ call AFactoryObject::MarkMeForDeletion 
        //and if it does, SignallingDeletion happens then. If it hadn't called
        //AFactoryObject::MarkMeForDeletion SignallingDeletion happens now.
        SignalDeletion(*iObjects[aIndex].iFactoryObject);
        }
    iObjects.Remove(aIndex);
    }

EXPORT_C void CFactoryBase::ManagedObjectBeingDeleted(AFactoryObject& aObject)
	{
	FACTORY_LOG((KFactoryTag, _L8("CFactoryBase %08x:\tManagedObjectBeingDeleted(object %08x) factory id %08x"), this, &aObject, Uid().iUid ));

	// Look for it in our active list and remove it if found
	TUint length = iObjects.Count();
	for (TUint index = 0; index < length; ++index)
		{
		if (iObjects[index].iFactoryObject == &aObject)
			{
			iObjects[index].iFlag |= TFactoryObjectEntry::EBeingDeleted;
			SignalDeletion(aObject);
			break;
			}
		}
	}

#ifdef _DEBUG
TBool CFactoryBaseMatch( const TFactoryObjectEntry& o1, const TFactoryObjectEntry& o2)
	{
	return o1.iFactoryObject == o2.iFactoryObject;
	}
#endif

EXPORT_C TFactoryObjectEntry& CFactoryBase::AddManagedObjectL(AFactoryObject& aObject)
/**	Add a AFactoryObject to the managed collection
@param aObject the AFactoryObject object to be added
@returns reference to the object added
@see AFactoryObject
@released Since 9.0 */
	{
	FACTORY_LOG((KFactoryTag, _L8("CFactoryBase %08x:\tAddManagedObjectL(object %08x) factory id %08x"), this, &aObject, Uid().iUid ));
	User::LeaveIfError(SignalCreation(aObject));

	TFactoryObjectEntry entry( &aObject, 0 );
#ifdef _DEBUG
	TIdentityRelation<TFactoryObjectEntry> ind(CFactoryBaseMatch);
	__ASSERT_DEBUG(iObjects.Find(entry,ind) == KErrNotFound, User::Panic(KSpecAssert_ElemFactoriesfct, 4));
#endif
	iObjects.AppendL(entry);
	return iObjects[iObjects.Count() - 1];
	}

EXPORT_C TUid CFactoryBase::Uid() const
/** Getter for the identifier of the object specified during construction
@returns The Id for the factory
@released Since 9.1
*/
	{
	return iFactoryUid;
	}


EXPORT_C AFactoryObject* CFactoryBase::FindObject(MFactoryQuery& aQuery) const
/**	Find a factory based on criteria specified by aQuery
@param aQuery a MFactoryQuery derived object implementing MFactoryQuery::Match
@see MFactoryQuery
@returns pointer to the found AFactoryObject or NULL otherwise
@released Since 9.1 */
	{
	TInt count = iObjects.Count();
	AFactoryObject* obj = NULL;
	for ( TInt ind = 0; ind < count; ind++ )
		{
		if (!(iObjects[ind].iFlag & TFactoryObjectEntry::EBeingDeleted))
    		{
    		TFactoryObjectInfo info( iObjects[ind] );
    		MFactoryQuery::TMatchResult match = aQuery.Match( info );
    		switch (match)
    			{
    			case MFactoryQuery::EMatch:
    				obj = info.iSelectedFactoryObject;
    				ind = count;
    				break;
    			case MFactoryQuery::EContinue:
    				break;
    			case MFactoryQuery::ECancel:
    				break;
    			default:
					FACTORY_LOG((KFactoryTag, _L8("CFactoryBase %08x:\tFindObject() Match returns unknown value %d- continue looking..."), this, match ));
    				break;
    			}
    		}
		}
	return obj;
	}

EXPORT_C void CFactoryBase::InterfaceStateChange(const TDesC8& aInfo)
    {
    for (TInt i = iFactoryNotify.Count() - 1; i >= 0; --i)
        {
        iFactoryNotify[i].InterfaceStateChange(aInfo);
        }
    }

EXPORT_C AFactoryObject::AFactoryObject(CFactoryBase& aFactory)
:	iFactory(aFactory)
	{
	}

EXPORT_C void AFactoryObject::DeleteMeNow()
	{
	delete this;
	}

EXPORT_C void AFactoryObject::MarkMeForDeletion()
/**	Instructs the parent container to delete this managed object */
	{
	iFactory.ManagedObjectBeingDeleted(*this);
	}

EXPORT_C AFactoryObject::~AFactoryObject()
/** Destructor. Notifies the parent container that this managed object is being destroyed */
	{
    iFactory.RemoveManagedObject(*this);
	}




