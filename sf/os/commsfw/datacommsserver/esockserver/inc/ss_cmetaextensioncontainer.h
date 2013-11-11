// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ss_cmetaextensioncontainer.h
//

/**
@file
@prototype
@internalTechnology
*/

#ifndef SYMBIAN_SS_CMETAEXTENSIONCONTAINER
#define SYMBIAN_SS_CMETAEXTENSIONCONTAINER

#include <elements/metadata.h>

#ifdef MEC_UNITTEST
// If this file is being built for the unit test we use a simpler
// base class that doesn't use the comms transport and async deletion.
// Functionally CMecUnitTestRefCountOwnerBase is the same as CRefCountOwnerBase
#include "mecunittestrefcountowner.h"

class TMetaExtensionContainerSpy;

#else
// !MEC_UNITTEST
#include <comms-infras/ss_nodemessages.h>

#endif
// MEC_UNITTEST

namespace ESock
{
// Forward declarations - see <comms-infras/ss_rmetaextensioncontainer.h>
class RMetaExtensionContainer;
class RMetaExtensionContainerC;



/**
Base class for implementations of containers holding the meta extensions.
The lifetime of itself and any extensions it owned by it are ref counted.
Extensions owned by the CMetaExtensionContainerImplBase must have been
created on the same heap as the container.
@prototype
@internalTechnology
*/
NONSHARABLE_CLASS(CMetaExtensionContainerImplBase) : public CRefCountOwnerBase
	{
#ifdef MEC_UNITTEST
	friend class ::TMetaExtensionContainerSpy;
#endif

public:
	/**
	@prototype
	@internalTechnology
	*/
	enum TMetaExtensionContainerType
		{
		EMetaExtensionContainer,
		EMetaExtensionContainerArray,
		};

	/**
	Finds the extension specified within the container
	@param aTypeId The STypeId of the meta extension to find
	@return A pointer to the meta extension or NULL if it wasn't found
	*/
	virtual const Meta::SMetaData* FindExtension(const Meta::STypeId& aTypeId) const = 0;

	/**
	Finds the extension specified within the container
	@param aTypeId The STypeId of the meta extension to find
	@return A reference to the meta extension
	@exception KErrNotFound is thrown if the meta extension was not present in the container
	*/
	const Meta::SMetaData& FindExtensionL(const Meta::STypeId& aTypeId) const;
	
	inline TMetaExtensionContainerType Type() const;

protected:
    CMetaExtensionContainerImplBase(TMetaExtensionContainerType aType)
        : CRefCountOwnerBase(NULL), iType(aType)
        {
        Open(); // Open the first reference
        }

    virtual void DeletePtr();
    
    ~CMetaExtensionContainerImplBase()
        {
        }
    
private:
	TMetaExtensionContainerType iType;
	};



/**
An implementation of an appendable container of meta extensions. This implementation can not
be created and used directly. Use RMetaExtensionContainer instead.
@prototype
@internalTechnology
*/
NONSHARABLE_CLASS(CMetaExtensionContainer) : public CMetaExtensionContainerImplBase
	{
	friend class RMetaExtensionContainer;
	friend class RMetaExtensionContainerC;
#ifdef MEC_UNITTEST
	friend class ::TMetaExtensionContainerSpy;
#endif

public:
	/**
	Finds the extension specified within the container
	@param aTypeId The STypeId of the meta extension to find
	@return A pointer to the meta extension or NULL if it wasn't found
	*/
	virtual const Meta::SMetaData* FindExtension(const Meta::STypeId& aTypeId) const;

	/**
	Adds a meta extension to the container, which will take ownership on success.
	@param aExtension The meta extension to add
	@return KErrAlreadyExists If a meta extension of the same type is in the container
			KErrNone If the meta extension was added successfully
			Other system-wide error codes
	*/
	TInt AppendExtension(const Meta::SMetaData* aExtension);

	/**
	Adds a meta extension to the container, which will take ownership on success.
	@param aExtension The meta extension to add
	@exception Leaves with KErrAlreadyExists If a meta extension of the same type is in the container,
			or other system-wide error codes
	*/
	void AppendExtensionL(const Meta::SMetaData* aExtension);

	/**
	Identifies obsolete containers in the chain and closes references to them 
	*/
	void Compact();
	
protected:
    /**
    From CRefCountOwnerBase. Use to release any resources owned by the implementation.
    It is called just before the object is deleted. Always call the DeletePtr in the base
    class from your implementation. 
    */
    virtual void DeletePtr();
    
private:
    // Never deleted directly
    ~CMetaExtensionContainer();
    CMetaExtensionContainer();

	// Only RMetaExtensionContainer/RMetaExtensionContainerC may create this class
	static CMetaExtensionContainer* NewLC(const CMetaExtensionContainerImplBase* aBaseContainer);
	void ConstructL(const CMetaExtensionContainerImplBase* aBaseContainer);

	/**
    Retrieves the base meta extension container
    @return A pointer to the base meta extension container, or NULL if it is a root base container
    */
    const CMetaExtensionContainerImplBase* BaseContainer() const;

    /**
    Sets the base container for branched meta extension containers
    Can only be set once and should be set during construction.
    The refcount on the base container will be incremented and owned by this CMetaExtensionContainerImplBase. 
    @param aBaseContainer The base meta extension container
    */
    void SetBaseContainer(const CMetaExtensionContainerImplBase* aBaseContainer);
    
    /**
    Remove the base container from the chain
    */
    void RemoveBaseContainer();
    
    /**
    Find the specified extension by searching only this container
	@param aTypeId The STypeId of the meta extension to find
	@return A pointer to the meta extension or NULL if it wasn't found
    */
    const Meta::SMetaData* FindExtensionInThisContainer(const Meta::STypeId& aTypeId) const;
	
private:
	const CMetaExtensionContainerImplBase* iBaseContainer;
	mutable RPointerArray<Meta::TMetaDataDeAllocator> iExtensions;
	};



/**
An implementation of a container of meta extension containers. This implementation can not
be created and used directly. RMetaExtensionContainer will create this implementation during
a call to AppendContainerL.
@prototype
@internalTechnology
*/
NONSHARABLE_CLASS(CMetaExtensionContainerArray) : public CMetaExtensionContainerImplBase
	{
	friend class RMetaExtensionContainer;
	friend class RMetaExtensionContainerC;
#ifdef MEC_UNITTEST
	friend class ::TMetaExtensionContainerSpy;
#endif

public:
	/**
	Finds the extension specified within the container
	@param aTypeId The STypeId of the meta extension to find
	@return A pointer to the meta extension or NULL if it wasn't found
	*/
	virtual const Meta::SMetaData* FindExtension(const Meta::STypeId& aTypeId) const;

	/**
	Appends a container to the list of container implementations.
	@aBaseContainer Appends an existing container. Extensions in aBaseContainer container will override
	any extensions of the same type already present in the container this method is called on.
	@return KErrNone on success
			KErrAlreadyExists if the container is already in the array
			or any other system-wide error code.
	*/
	TInt AppendContainer(const CMetaExtensionContainerImplBase& aContainer);
	
protected:
    /**
    From CRefCountOwnerBase. Use to release any resources owned by the implementation.
    It is called just before the object is deleted. Always call the DeletePtr in the base
    class from your implementation. 
    */
    virtual void DeletePtr();
    
private:
    // Never deleted directly
    ~CMetaExtensionContainerArray();
    CMetaExtensionContainerArray();

	// Only RMetaExtensionContainer/RMetaExtensionContainerC may create this class
	static CMetaExtensionContainerArray* NewLC(const CMetaExtensionContainerImplBase* aBaseContainer);
	void ConstructL(const CMetaExtensionContainerImplBase* aBaseContainer);
	
private:
	mutable RPointerArray<CMetaExtensionContainerImplBase> iContainers;
	};

	
	
inline CMetaExtensionContainerImplBase::TMetaExtensionContainerType CMetaExtensionContainerImplBase::Type() const
	{
	return iType;
	}

} // namespace ESock


#endif
// SYMBIAN_SS_CMETAEXTENSIONCONTAINER

