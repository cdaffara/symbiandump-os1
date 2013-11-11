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
// ss_rmetaextensioncontainer.h
// 

/**
@file
@prototype 
@publishedPartner 
*/

#ifndef SYMBIAN_SS_RMETAEXTENSIONCONTAINER
#define SYMBIAN_SS_RMETAEXTENSIONCONTAINER

#include <e32cmn.h>
#include <elements/metadata.h>


#ifdef MEC_UNITTEST
class TMetaExtensionContainerSpy;
#endif

namespace ESock
{

class CMetaExtensionContainerImplBase;
class CMetaExtensionContainer;
class CMetaExtensionContainerArray;
class TMetaExtensionContainerMetaType;


enum TMecPanic
    {
    ENoImplementation = 1,      // No implementation opened for the RMEC instance
    EInvalidSelfReference = 2,  // Trying to open an RMEC using itself as the source RMEC 
    EAlreadyOpen = 3,           // Trying to open a reference against a new RMEC without closing this one
    EUsingEmptyContainer = 4,   // An explicit attempt was made to open an RMEC which was created with no extensions (wasteful - RMECs should always hold something of value)
    EIllegalState = 5, 			// Should never happen. Indicates bad logic or bad code.
    };

/**
Represents a read only meta extension container.

@prototype
@publishedPartner
*/
class RMetaExtensionContainerC
	{
#ifdef MEC_UNITTEST
	friend class ::TMetaExtensionContainerSpy;
#endif
	friend class ESock::TMetaExtensionContainerMetaType;

public:
    /**
    Default c'tor. 
    */
    IMPORT_C RMetaExtensionContainerC();
    
    /**
    D'tor
    */
    IMPORT_C ~RMetaExtensionContainerC();

    /**
    Opens a reference against a specific container. This RMetaExtensionContainerC must not be open already.
    @param aSource Container to open this instance against. A reference will be automatically added to the source container.
    */
    IMPORT_C void Open(const RMetaExtensionContainerC& aSource);
    
    /**
	Releases a reference to the underlying container implementation
	*/
	IMPORT_C void Close();

	/**
	Finds the extension specified within the container
	@param aTypeId The STypeId of the meta extension to find
	@return A pointer to the meta extension or NULL if it wasn't found
	*/
	IMPORT_C const Meta::SMetaData* FindExtension(const Meta::STypeId& aTypeId) const;

	/**
	Finds the extension specified within the container
	@param aTypeId The STypeId of the meta extension to find
	@return A reference to the meta extension
	@exception Leaves with KErrNotFound if the meta extension was not present in the container
	*/
	IMPORT_C const Meta::SMetaData& FindExtensionL(const Meta::STypeId& aTypeId) const;
	
protected:
    /**
    Opens an additional reference to the underlying container implementation.
    */
    IMPORT_C void Open();

    /**
	Returns a pointer to the container implementation being represented by this interface
	@internalTechnology
	*/
	inline CMetaExtensionContainerImplBase* Container() const;

	/**
	Sets the initial container implementation to be represented by this interface
	@internalTechnology
	*/
	inline void SetContainer(CMetaExtensionContainerImplBase* aContainer);
        
private:
	// A pointer to the container implementation
	mutable CMetaExtensionContainerImplBase* iContainerImpl;
	};


/**
Represents an appendable meta extension container.
@prototype
@publishedPartner
*/
class RMetaExtensionContainer : public RMetaExtensionContainerC
	{
#ifdef MEC_UNITTEST
    friend class ::TMetaExtensionContainerSpy;
#endif

public:
    /**
    Default c'tor
    */
    IMPORT_C RMetaExtensionContainer();
    
    /**
    If the container is already open a reference is added to the container.
    If the container is not open a new appendable root meta extension container will be created and a reference added.
    */
    IMPORT_C void Open();

    /**
    A new appendable meta extension container will be created using the supplied container as the base container to build on.
	The container must not already be open.
	@param aBaseContainer Container to use as the base meta extension container. A reference will be automatically added to this.
	*/
	IMPORT_C void Open(const RMetaExtensionContainerC& aBaseContainer);
	
    /**
    Releases a reference to the underlying container implementation
    */
    IMPORT_C void Close();
    
	/**
	Finds the extension specified within the container
	@param aTypeId The STypeId of the meta extension to find
	@return A pointer to the meta extension or NULL if it wasn't found
	*/
	IMPORT_C const Meta::SMetaData* FindExtension(const Meta::STypeId& aTypeId) const;

	/**
	Finds the extension specified within the container
	@param aTypeId The STypeId of the meta extension to find
	@return A reference to the meta extension
	@exception Leaves with KErrNotFound if the meta extension was not present in the container
	*/
	IMPORT_C const Meta::SMetaData& FindExtensionL(const Meta::STypeId& aTypeId) const;
	
	/**
	Adds a meta extension to the container, which will take ownership on success.
	@param aExtension The meta extension to add
	@return KErrAlreadyExists If a meta extension of the same type is in the container
			KErrNone If the meta extension was added successfully
			Other system-wide error codes
	*/
	IMPORT_C TInt AppendExtension(const Meta::SMetaData* aExtension);

	/**
	Adds a meta extension to the container, which will take ownership on success.
	@param aExtension The meta extension to add
	@exception Leaves with KErrAlreadyExists If a meta extension of the same type is in the container,
			or other system-wide error codes
	*/
	IMPORT_C void AppendExtensionL(const Meta::SMetaData* aExtension);

	/**
	Appends a container to the list of container implementations.
	@aBaseContainer Appends an existing container. Extensions in aBaseContainer container will override
	any extensions of the same type already present in the container this method is called on.
	@exception Leaves with KErrNone on success, 
			KErrAlreadyExists if the container is already present, 
			or any other system-wide error code.
	*/
	IMPORT_C void AppendContainerL(const RMetaExtensionContainerC& aBaseContainer);

private:
	/**
	@return Returns an implementation of an appendable extension container instance or
	NULL if one isn't available at present
	*/
	CMetaExtensionContainer* AppendableExtensionContainer() const;
	
	/**
	@return Returns an implementation of an appendable container array instance or
	NULL if one isn't available at present
	*/
	CMetaExtensionContainerArray* AppendableContainerArray() const;

	/**
    Creates a new container implementations
    @exception Any system wide error code
    */
    void CreateExtensionContainerL();

	/**
    Creates a new container implementations
    @exception Any system wide error code
    */
    void CreateContainerArrayL();

private:
    TBool iIsLazyCreationDone:1; // Used for lazy creation of the container impl.
	TBool iIsOpen:1;
    mutable CMetaExtensionContainer* iCurrentExtensionContainer;	// Not owned
	};



/**
@internalTechnology
*/
CMetaExtensionContainerImplBase* RMetaExtensionContainerC::Container() const
	{
	return iContainerImpl;
    }


/**
@internalTechnology
*/
inline void RMetaExtensionContainerC::SetContainer(CMetaExtensionContainerImplBase* aContainer)
	{
	ASSERT(aContainer);		 // Don't allow setting it to NULL
	iContainerImpl = aContainer;
	}



} // namespace ESock

#endif
// SYMBIAN_SS_RMETAEXTENSIONCONTAINER

