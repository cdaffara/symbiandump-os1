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
// Generic parameter bundles:
// Serialisable containers for structured custom data.
// Bundle
// has 0..n
// Parameter Set Containers
// each of which has 0..n
// ParameterSets
// This can be seen as loosely mapping to a "table" of data,
// where the
// "table"
// has 0..n
// "rows"
// each of which has 0..n
// "groups of columns"
// Custom data is implemented as ParameterSet-derived classes in ECom plugins. See esock/conn_params for an example
// 
//
// Note: Every effort should be made to ensure that the classes forming this API be kept consistent with each other
// When making changes to these classes also see the files es_parameterbundle.h, es_parameterfamily.h,
// ss_parameterfamilybundle.h
//
//


/**
 @file
 @internalTechnology
*/


#ifndef ES_PARAMETERBUNDLE_H
#define ES_PARAMETERBUNDLE_H

#include <comms-infras/metacontainer.h>
#include <comms-infras/metatype.h>
#include <es_sock.h>
#include <comms-infras/es_parameterset.h>
	
class CParameterBundleBase;


class CParameterSetContainer : public CBase, public Meta::MMetaType
/**
 * A container for 0..n Parameter Sets.
 * Can be seen as a "row" or "record" of information,
 *  with its own 32 bit Id to be used as a key to the set if required 
 * */
	{
public:
	IMPORT_C static CParameterSetContainer* NewL(CParameterBundleBase& aBundle, TUint32 aContainerId = 0);
	IMPORT_C static CParameterSetContainer* LoadL(CParameterBundleBase& aBundle, TPtrC8& aBuffer);

	IMPORT_C static CParameterSetContainer* NewL(TUint32 aContainerId = 0); // Use this constructor if the container is to live independently of a bundle
	IMPORT_C static CParameterSetContainer* LoadL(TPtrC8& aBuffer);

public:
	IMPORT_C virtual ~CParameterSetContainer();
	
	inline TUint32 Id() const
		{
		return iContainerId;
		}

	IMPORT_C void AddParameterSetL(XParameterSetBase* aParameterSet); // ownership of object is passed
	IMPORT_C TInt AddParameterSet(XParameterSetBase* aParameterSet); // ownership of object is passed

	/**
	Searches a container for a parameter set of the given type.
	@param aSetId The STypeId of the set
	@return Pointer to the set if found, otherwise a NULL pointer.
	*/
	IMPORT_C XParameterSetBase* FindParameterSet(const Meta::STypeId& aSetId);

	const XParameterSetBase* FindParameterSet(const Meta::STypeId& aSetId) const
		{
		return const_cast<CParameterSetContainer*>(this)->FindParameterSet(aSetId);
		}

	/**
	Searches a container for a specific parameter set instance.
	@param aRhs The parameter set to find
	@return ETrue if set was found in parameter set container.
	*/
	IMPORT_C TBool FindParameterSet(const XParameterSetBase& aRhs);
	IMPORT_C XParameterSetBase* GetParameterSet(TInt aIndex);

	const XParameterSetBase* GetParameterSet(TInt aIndex) const
		{
		return const_cast<CParameterSetContainer*>(this)->GetParameterSet(aIndex);
		}

	TInt CountParameterSets() const
		{
		return iParameterSets.Count();
		}

	/** Deletes set at index aIndex and replaces it with aParameterSet.
		Ownership of set passes to this parameter set container.
	@param aIndex - index of container to delete and replace
	@param aParameterSet - new set to replace in array
	@return throws KErrArgument if aIndex out of range
	*/
	IMPORT_C void ReplaceParameterSetL(TInt aIndex, XParameterSetBase* aNewSet);
	IMPORT_C TInt ReplaceParameterSet(TInt aIndex, XParameterSetBase* aNewSet);

	/** Deletes set at index, and reorganises the array so it's 1 smaller
	@param aIndex - index of container to delete and remove from array
	@return throws KErrArgument if aIndex out of range
	*/
	IMPORT_C void DeleteParameterSetL(TInt aIndex);
	IMPORT_C TInt DeleteParameterSet(TInt aIndex);
	
	/**
	Removes the parameter set at the index. Ownership is transfered to the calling method
	@param aIndex - index of container to delete and remove from array
	@return returns NULL if the index was invalid, otherwise a pointer to the
	removed parameter set. 
	*/
	IMPORT_C XParameterSetBase* RemoveParameterSet(TInt aIndex);

	/** Makes room for a bulk setting operation
	*/
	IMPORT_C void GrowToFitL(TInt aMinSize);
	IMPORT_C TInt GrowToFit(TInt aMinSize);
	
	inline void ClearParameterSetPointer(TInt aIndex)
		{
		iParameterSets[aIndex] = 0;
		}

	// From MMetaType
	IMPORT_C virtual TInt Load(TPtrC8& aBuffer);
	IMPORT_C virtual TInt Store(TDes8& aBuffer) const;
	IMPORT_C virtual TInt Length() const;
    			
protected:
	CParameterSetContainer(TUint32 aContainerId)
		: iContainerId(aContainerId)
		{
		}
		
	IMPORT_C void ConstructL(CParameterBundleBase& aBundle);


private:
	// From MMetaType
    virtual void Copy(const TAny* /*aData*/)
    	{
    	// Not supported
    	}

	TUint32 iContainerId;
	Meta::RMetaDataEComContainer iParameterSets;
	};


class CParameterBundleBase : public CBase
/** Bundle of (i.e. container for) parameter set containers.

The basic object shape and base type container manipulators.
To be used only by CParameterBundle<PARAMETERSETCONTAINERTYPE,SUBCLASS>

May contain and 0..N parameter families.
*/
	{
public:
	static inline CParameterBundleBase* NewL()
		{
		return new(ELeave) CParameterBundleBase;
		}

	IMPORT_C virtual ~CParameterBundleBase();

	IMPORT_C TUint Length() const;

	IMPORT_C TInt Store(TDes8& aDes) const;
	
	// Load() can't be in base type as it requires static call to specific parameter set container type

	IMPORT_C void AddParamSetContainerL(CParameterSetContainer& aContainer);
	IMPORT_C TInt AddParamSetContainer(CParameterSetContainer& aContainer);

	IMPORT_C CParameterSetContainer* RemoveParamSetContainer(TInt aIndex);
	
	TInt CountParamSetContainers() const
		{
		return iSetContainers.Count();
		}

	IMPORT_C CParameterSetContainer* GetParamSetContainer(TInt aIndex);

	const CParameterSetContainer* GetParamSetContainer(TInt aIndex) const
		{
		return const_cast<CParameterBundleBase*>(this)->GetParamSetContainer(aIndex);
		}

	IMPORT_C CParameterSetContainer* FindParamSetContainer(TUint32 aContainerId);
	inline const CParameterSetContainer* FindParamSetContainer(TUint32 aContainerId) const
		{
		const CParameterSetContainer* r = const_cast<CParameterBundleBase*>(this)->FindParamSetContainer(aContainerId);
		return r;
		}

	// deep search for parameter set in bundle
	IMPORT_C XParameterSetBase* FindParameterSet(const Meta::STypeId& aTypeId);
	inline const XParameterSetBase* FindParameterSet(const Meta::STypeId& aTypeId) const
		{
		const XParameterSetBase* r = const_cast<CParameterBundleBase*>(this)->FindParameterSet(aTypeId);
		return r;
		}

	// finds aOld in array, deletes it and changes that pointer in array to be aNew instead.
	// throws KErrArgument if aOld is not found
	IMPORT_C void ReplaceParamSetContainerL(CParameterSetContainer* aOld, CParameterSetContainer* aNew);
	IMPORT_C TInt ReplaceParamSetContainer(CParameterSetContainer* aOld, CParameterSetContainer* aNew);

protected:
	CParameterBundleBase()
		{
		}

private:
	CParameterBundleBase(const CParameterBundleBase& aBundle);
	CParameterBundleBase& operator=(const CParameterBundleBase& aBundle);

protected:
	RPointerArray<CParameterSetContainer> iSetContainers;
};


template<class PARAMSETCONTAINERTYPE, class SUBCLASS, TInt UID, TInt TYPE>
class MParameterSetTemplateMethods
/**
 * methods brought in as a template, to avoid having to rewrite
 * identical-looking code in every derived type of XParameterSetBase 
*/
	{
public:
	static inline SUBCLASS* NewL(PARAMSETCONTAINERTYPE& aContainer)
		{
		SUBCLASS* obj = NewL();
		CleanupStack::PushL(obj);
		aContainer.AddParameterSetL(obj);
		CleanupStack::Pop(obj);
		return obj;
		}

	static inline Meta::STypeId Type()
		{
		return Meta::STypeId::CreateSTypeId(UID,TYPE);
		}

	static inline SUBCLASS* NewL()
		{
		// As it's a plugin we must instantiate via the plugin interface!
		//  Directly calling the constructor would cause a link error as
		//   the vtable won't be visible by whoever uses this.
		//  So we need ECom to import it for us.
		return static_cast<SUBCLASS*>(Meta::SMetaDataECom::NewInstanceL(Type()));
		}
		
	static inline SUBCLASS* FindInParamSetContainer(PARAMSETCONTAINERTYPE& aContainer)
		{
		return static_cast<SUBCLASS*>(aContainer.FindParameterSet(Type()));
		}
		
	static inline const SUBCLASS* FindInParamSetContainer(const PARAMSETCONTAINERTYPE& aContainer)
		{
		return static_cast<const SUBCLASS*>(aContainer.FindParameterSet(Type()));
		}		

	static inline SUBCLASS* FindInBundle(CParameterBundleBase& aBundle)
		{
		return static_cast<SUBCLASS*>(aBundle.FindParameterSet(Type()));
		}
		
	static inline const SUBCLASS* FindInBundle(const CParameterBundleBase& aBundle)
		{
		return static_cast<const SUBCLASS*>(aBundle.FindParameterSet(Type()));
		}		
	};


template<class PARAMSETCONTAINERTYPE,class SUBCLASS>
class CParameterBundle : public CParameterBundleBase
/** Container for (bundle of) parameter set containers.

This class can't be used directly, but must be subclassed, as the NewL/LoadL refer to
the subclass.

May contain and 0..N parameter families of type PARAMSETCONTAINERTYPE.
*/
{
public:

	static SUBCLASS* NewL()
	/** Creates a new instance of a parameter bundle (heap object).
	
	@return newly created instance of a parameter bundle
	@exception leaves with KErrNoMemory in out of memory conditions
	*/
		{
		return new (ELeave) SUBCLASS();
		}

	static SUBCLASS* LoadL(TDesC8& aDes)
	/** Creates a new parameter set bundle from a buffer containing the serialised object.
	
	@param aDes Buffer containing the serialised object information
	@return a pointer to a parameter bundle if successful, otherwise leaves with system error code.
	*/
		{
		SUBCLASS* obj = new (ELeave) SUBCLASS();
		CleanupStack::PushL(obj);
		User::LeaveIfError(obj->Load(aDes));
		CleanupStack::Pop(obj);
		return obj;
		}

	TInt Load(const TDesC8& aDes)
	/** Instructs this sub-connection bundle to set its members based on the serialiesd data
		in the buffer passed.

	@param aDes Buffer containing the serialised bundle object
	@return KErrNone if successful, otherwise system wide error
	*/
		{
		TPtrC8 buf(aDes);
		while (buf.Length() > 0)
			{
			TRAPD(ret, PARAMSETCONTAINERTYPE::LoadL(*(static_cast<SUBCLASS*>(this)), buf));
			if (ret != KErrNone)
				{
				iSetContainers.ResetAndDestroy();
				return ret;
				}
			}
		return KErrNone;
		}

	inline PARAMSETCONTAINERTYPE* GetParamSetContainer(TInt aIndex)
		{
		return static_cast<PARAMSETCONTAINERTYPE*>(CParameterBundleBase::GetParamSetContainer(aIndex));
		}

	inline const PARAMSETCONTAINERTYPE* GetParamSetContainer(TInt aIndex) const
		{
		return static_cast<const PARAMSETCONTAINERTYPE*>(CParameterBundleBase::GetParamSetContainer(aIndex));
		}

	inline PARAMSETCONTAINERTYPE* FindParamSetContainer(TUint32 aContainerId)
		{
		return static_cast<PARAMSETCONTAINERTYPE*>(CParameterBundleBase::FindParamSetContainer(aContainerId));
		}

protected:
	CParameterBundle()
		{
		}
};


class CGenericParameterBundle : public CParameterBundle<CParameterSetContainer,CGenericParameterBundle>
	{
	};




#endif
// ES_PARAMETERBUNDLE_H


