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
// ss_rmetaextensioncontainer.cpp 
//


#include "ss_rmetaextensioncontainer.h"
#include "ss_cmetaextensioncontainer.h"

using namespace ESock;

_LIT(KMecPanic, "mecpanic");

void PanicMec(TMecPanic aCode)
    {
    User::Panic(KMecPanic, aCode);
    }

// ===========================================================
// RMetaExtensionContainerC
// ===========================================================

EXPORT_C RMetaExtensionContainerC::RMetaExtensionContainerC()
    : iContainerImpl(NULL)
    {
    }

EXPORT_C RMetaExtensionContainerC::~RMetaExtensionContainerC()
    {
    }

EXPORT_C void RMetaExtensionContainerC::Open()
    {
    __ASSERT_ALWAYS(iContainerImpl, PanicMec(ENoImplementation));
    iContainerImpl->Open();
    }

EXPORT_C void RMetaExtensionContainerC::Open(const RMetaExtensionContainerC& aSource)
    {
    __ASSERT_ALWAYS(&aSource != this, PanicMec(EInvalidSelfReference));
    __ASSERT_ALWAYS(aSource.Container(), PanicMec(ENoImplementation));
    __ASSERT_ALWAYS(!iContainerImpl, PanicMec(EAlreadyOpen));
    
    aSource.Container()->Open();
    iContainerImpl = aSource.Container();
    }

EXPORT_C void RMetaExtensionContainerC::Close()
    {
    if (iContainerImpl)
        {
        iContainerImpl->Close();
        iContainerImpl = NULL;
        }
    }

EXPORT_C const Meta::SMetaData* RMetaExtensionContainerC::FindExtension(const Meta::STypeId& aTypeId) const
    {
    if (Container())
        {
        return Container()->FindExtension(aTypeId);
        }
    else
        {
        return NULL;
        }
    }


EXPORT_C const Meta::SMetaData& RMetaExtensionContainerC::FindExtensionL(const Meta::STypeId& aTypeId) const
    {
    if (!Container())
        {
        User::Leave(KErrNotFound);
        }
    return Container()->FindExtensionL(aTypeId);
    }


// ===========================================================
// RMetaExtensionContainer
// ===========================================================

EXPORT_C RMetaExtensionContainer::RMetaExtensionContainer()
    : iIsLazyCreationDone(EFalse), iIsOpen(EFalse), iCurrentExtensionContainer(NULL)
    {
    }


EXPORT_C void RMetaExtensionContainer::Open()
    {
    __ASSERT_ALWAYS(!iIsOpen, PanicMec(EAlreadyOpen));
    
    // Using lazy creation of the implementation so that Open() is infallible
    // This boolean ensures that Open() is still called before use of the container
    iIsOpen = ETrue;
    }


EXPORT_C void RMetaExtensionContainer::Open(const RMetaExtensionContainerC& aBaseContainer)
    {
    __ASSERT_ALWAYS(!iIsOpen, PanicMec(EAlreadyOpen));
	RMetaExtensionContainerC::Open(aBaseContainer);
    iIsOpen = ETrue;
    }


EXPORT_C void RMetaExtensionContainer::Close()
    {
    if (iIsOpen)
        {
        if (AppendableExtensionContainer())
        	{
        	AppendableExtensionContainer()->Compact();
        	}
        iIsOpen = EFalse;
        }
    
    RMetaExtensionContainerC::Close();
    }


void RMetaExtensionContainer::CreateExtensionContainerL()
    {
    // Don't call it unless it should be created
    __ASSERT_DEBUG(!AppendableExtensionContainer(), PanicMec(EIllegalState));
    
	CMetaExtensionContainerImplBase* pendingBaseContainerImpl = NULL;
	if (!iIsLazyCreationDone)
		{
		// Retrieve the pending base container implementation if there is one
		pendingBaseContainerImpl = Container();
		}
	
    CMetaExtensionContainer* mecImpl = CMetaExtensionContainer::NewLC(pendingBaseContainerImpl);
    if (!iIsLazyCreationDone)
    	{
    	// If there is no proper container implementation this normal extension container
    	// will become it.
    	// Base class takes ownership of the new container implementation reference
    	SetContainer(mecImpl);

        if (pendingBaseContainerImpl)
            {
            // Remove the holding reference - the new container implementation has its own
            pendingBaseContainerImpl->Close();
            }
        
        iIsLazyCreationDone = ETrue;
        }
    else
    	{
    	// There is an existing container implementation. Given the fact that the
    	// CreateExtensionContainerL method has been called it must be of the
    	// container array type. This will own (the reference on) our new extension
    	// container.
    	__ASSERT_DEBUG(Container()->Type() == CMetaExtensionContainerImplBase::EMetaExtensionContainerArray, PanicMec(EIllegalState));
    	__ASSERT_DEBUG(!pendingBaseContainerImpl, PanicMec(EIllegalState));
    	User::LeaveIfError(AppendableContainerArray()->AppendContainer(*mecImpl));
    	}
    
	__ASSERT_DEBUG(!iCurrentExtensionContainer, PanicMec(EIllegalState));
	iCurrentExtensionContainer = mecImpl;
    CleanupStack::Pop(mecImpl);
    }


void RMetaExtensionContainer::CreateContainerArrayL()
    {
    ASSERT(!AppendableContainerArray()); // Don't call it unless it should be created
    
	CMetaExtensionContainerImplBase* pendingBaseContainerImpl = NULL;
	if (!iIsLazyCreationDone)
		{
		// Retrieve the pending base container implementation if there is one
		pendingBaseContainerImpl = Container();
		}
	
	CMetaExtensionContainerArray* mecImpl = CMetaExtensionContainerArray::NewLC(pendingBaseContainerImpl);
    if (!iIsLazyCreationDone)
    	{
    	// If there is no proper container implementation this container array implementation
    	// will become it.
    	// Base class takes ownership of the new container implementation reference
    	SetContainer(mecImpl);
        
    	if (pendingBaseContainerImpl)
            {
            // Remove the holding reference - the new container implementation has its own
            pendingBaseContainerImpl->Close();
            }
    	
    	iIsLazyCreationDone = ETrue;
    	}
    else
    	{
    	// There is an existing container implementation. Given the fact that the
    	// CreateContainerArrayL method has been called it must be of the
    	// normal extension container type. This will be replaced with a container
    	// array implementation which will own (the reference on) the existing
    	// container.
    	__ASSERT_DEBUG(Container()->Type() == CMetaExtensionContainerImplBase::EMetaExtensionContainer, PanicMec(EIllegalState));
    	__ASSERT_DEBUG(!pendingBaseContainerImpl, PanicMec(EIllegalState));
    	__ASSERT_DEBUG(iCurrentExtensionContainer, PanicMec(EIllegalState));
    	User::LeaveIfError(mecImpl->AppendContainer(*iCurrentExtensionContainer));

    	// Close our reference on the extension container, the container array will open its own
    	iCurrentExtensionContainer->Close();
    	iCurrentExtensionContainer = NULL;
    	SetContainer(mecImpl);
    	}

    CleanupStack::Pop(mecImpl);
    }


EXPORT_C const Meta::SMetaData* RMetaExtensionContainer::FindExtension(const Meta::STypeId& aTypeId) const
    {
    __ASSERT_ALWAYS(iIsOpen, PanicMec(ENoImplementation));
    if (Container())
    	{
    	return Container()->FindExtension(aTypeId);
    	}
    return NULL;
    }


EXPORT_C const Meta::SMetaData& RMetaExtensionContainer::FindExtensionL(const Meta::STypeId& aTypeId) const
    {
    __ASSERT_ALWAYS(iIsOpen, PanicMec(ENoImplementation));
    if (!Container())
    	{
        User::Leave(KErrNotFound);
    	}
	return Container()->FindExtensionL(aTypeId);
    }


EXPORT_C TInt RMetaExtensionContainer::AppendExtension(const Meta::SMetaData* aExtension)
    {
    __ASSERT_ALWAYS(iIsOpen, PanicMec(ENoImplementation));

    if (!aExtension)
        {
        return KErrArgument;
        }
    
    CMetaExtensionContainer* mecImpl = AppendableExtensionContainer();
    if (!mecImpl)
        {
        // Do lazy creation
        TRAPD(err, CreateExtensionContainerL());
        if (err != KErrNone)
            {
            return err;
            }
        mecImpl = AppendableExtensionContainer();
        }

    return mecImpl->AppendExtension(aExtension);
    }


EXPORT_C void RMetaExtensionContainer::AppendExtensionL(const Meta::SMetaData* aExtension)
    {
    __ASSERT_ALWAYS(iIsOpen, PanicMec(ENoImplementation));

    if (!aExtension)
        {
        User::Leave(KErrArgument);
        }
    
    CMetaExtensionContainer* mecImpl = AppendableExtensionContainer();
    if (!mecImpl)
        {
        // Do lazy creation
        CreateExtensionContainerL();
        mecImpl = AppendableExtensionContainer();
        }

    mecImpl->AppendExtensionL(aExtension);
    }


EXPORT_C void RMetaExtensionContainer::AppendContainerL(const RMetaExtensionContainerC& aContainer)
	{
    __ASSERT_ALWAYS(&aContainer != this, PanicMec(EInvalidSelfReference));
	__ASSERT_ALWAYS(iIsOpen, PanicMec(ENoImplementation));
	
	CMetaExtensionContainerImplBase* containerImpl = static_cast<const RMetaExtensionContainer&>(aContainer).Container();
	__ASSERT_ALWAYS(containerImpl, PanicMec(ENoImplementation));
	__ASSERT_ALWAYS(containerImpl != Container(), PanicMec(EInvalidSelfReference));
		
	CMetaExtensionContainerArray* mecImpl = AppendableContainerArray();
	if (!mecImpl)
		{
		// Do lazy creation
		CreateContainerArrayL();
		mecImpl = AppendableContainerArray();
		}

    const RMetaExtensionContainer& containerIfc = static_cast<const RMetaExtensionContainer&>(aContainer);
	User::LeaveIfError(mecImpl->AppendContainer(*containerIfc.Container()));
	
	// Null the current extension container pointer forcing a new extension container to be
	// created and appended to the array after the one that has been added
	iCurrentExtensionContainer = NULL;
	}


CMetaExtensionContainer* RMetaExtensionContainer::AppendableExtensionContainer() const
    {
	return iIsLazyCreationDone ? iCurrentExtensionContainer : NULL;
    }


CMetaExtensionContainerArray* RMetaExtensionContainer::AppendableContainerArray() const
    {
    if (iIsLazyCreationDone)
    	{
		const CMetaExtensionContainerImplBase* mecImpl = Container();
		if (mecImpl && mecImpl->Type() == CMetaExtensionContainerImplBase::EMetaExtensionContainerArray)
			{
			return const_cast<CMetaExtensionContainerArray*>(static_cast<const CMetaExtensionContainerArray*>(mecImpl));
			}
    	}
    return NULL;
    }


