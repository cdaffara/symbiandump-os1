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
 @internalAll
*/
  

#ifndef SYMBIAN_FACTORY_H
#define SYMBIAN_FACTORY_H

#include <elements/nm_node.h>
#include <elements/factoryobject.h>

namespace Factories
{
class CFactoryBase;
class CFactoryContainer;
typedef CFactoryBase* (*TFACTORYCONSTRUCTOR)(const TDesC8&, CFactoryContainer&);
class IFactoryNotify;


class CFactoryContainer : public CBase
/** Factory Factory Container class is used to store factory objects of a
    specific type. It provides basic functionality to find, add and remove factories.

@internalAll
*/
	{
	friend class CFactoryBase;

public:
	IMPORT_C CFactoryBase* FindFactory(TUid aFactoryUid) const;
	IMPORT_C ~CFactoryContainer();
	IMPORT_C void RegisterNotifierL(TUid aFactoryUid, const IFactoryNotify& aFactoryNotify);
	IMPORT_C virtual NetInterfaces::TInterfaceControl* FetchNodeInterfaceControlL(TInt aInterfaceId);

protected:
	IMPORT_C explicit CFactoryContainer();
	IMPORT_C void AddL(CFactoryBase& aFactory);
	IMPORT_C CFactoryBase* Factory(TInt aIndex) const; //checks if the aIndex is within the range
    IMPORT_C AFactoryObject* FindObject(MFactoryQuery& aQuery) const;

private:
	RPointerArray<CFactoryBase> iFactories;
	};


class CFactoryContainerNode : public CFactoryContainer,
                              public Messages::ASimpleNodeIdBase
/** Factory Factory Container class is used to store factory objects of a
    specific type. It provides basic functionality to find, add and remove factories.

@internalAll
*/
	{
public:
	IMPORT_C ~CFactoryContainerNode();

protected:
	IMPORT_C explicit CFactoryContainerNode();
	};


class CFactoryBase : public CBase,
                     public NetInterfaces::AApiExtBase
/** Factory Base class provides basic object managment
	functionlity and the ability for a factory to be load from another
	Dynamic Linked Library via the ECOM framework.

    It also provides asynchronous destruction for it's managed objects (if
	required) but also copes with synchronous destruction, should a managed
	object get deleted by another entity.

@internalAll
*/
	{
	friend class AFactoryObject;
	friend class CFactoryContainer;
	friend class RPointerArray<CFactoryBase>;

public:
	IMPORT_C static CFactoryBase* NewL(const TDesC8& aName, const TUid& aInterfaceUid, CFactoryContainer& aParentContainer);
	IMPORT_C static CFactoryBase* NewL(const TUid& aImplementationUid, CFactoryContainer& aParentContainer);
	IMPORT_C TUid Uid() const;
	CFactoryContainer& ParentContainer() {return iParentContainer;}

	IMPORT_C void RegisterNotifierL(const IFactoryNotify& aFactoryNotify);
	IMPORT_C void DeRegisterNotifier(const IFactoryNotify& aFactoryNotify);
	// Called by a node to indicate that an interface state change has taken place.  The factory then propagates
	// this to all observers that have registered to be notified.
    IMPORT_C void InterfaceStateChange(const TDesC8& aInfo);

protected:
	IMPORT_C explicit CFactoryBase(TUid aFactoryUid, CFactoryContainer& aParentContainer);
	IMPORT_C ~CFactoryBase();
	IMPORT_C void ConstructL(const TUid& aDestroyUid);
	IMPORT_C void ConstructL();

	//Removes aObject from the factory (doesn't delete the object).
	IMPORT_C void RemoveManagedObject(AFactoryObject& aObject);
	//Marks aObject for deletion. The object will not be served by 'this' from now on.
	IMPORT_C void ManagedObjectBeingDeleted(AFactoryObject& aObject);
	IMPORT_C TFactoryObjectEntry& AddManagedObjectL(AFactoryObject& aObject);
	IMPORT_C AFactoryObject* FindObject(MFactoryQuery& aQuery) const;

private:
	IMPORT_C void SignalDeletion(AFactoryObject& aObject);
	IMPORT_C TInt SignalCreation(AFactoryObject& aObject);
	void RemoveManagedObject(TInt aIndex);
	
private:
	CFactoryContainer& iParentContainer;
	TUid iFactoryUid;
	RArray<TFactoryObjectEntry> iObjects;
	TUid iDestroyUid;
protected:
	RArray<IFactoryNotify> iFactoryNotify;	// accessed by shimcpr for InterfaceStateChange
	};

template <class FACTORY_OBJECT_TYPE>
FACTORY_OBJECT_TYPE* factoryobject_cast(AFactoryObject* aFactoryObject)
    {
    return aFactoryObject && TUid::Uid(FACTORY_OBJECT_TYPE::FactoryType::iUid) == aFactoryObject->Factory().Uid() ?
           static_cast<FACTORY_OBJECT_TYPE*>(aFactoryObject) : NULL;
    };

template <class FACTORY_OBJECT_TYPE>
FACTORY_OBJECT_TYPE* factoryobject_cast(const AFactoryObject* aFactoryObject)
    {
    return aFactoryObject && TUid::Uid(FACTORY_OBJECT_TYPE::FactoryType::iUid) == aFactoryObject->Factory().Uid() ?
           static_cast<FACTORY_OBJECT_TYPE*>(aFactoryObject) : NULL;
    }

} //namespace Factories

#endif
//SYMBIAN_FACTORY_H

