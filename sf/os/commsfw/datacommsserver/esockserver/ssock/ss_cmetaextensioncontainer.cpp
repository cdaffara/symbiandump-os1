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
// ss_cmetaextensioncontainer.cpp 
//


#include "ss_cmetaextensioncontainer.h"
#include <comms-infras/ss_log.h>

using namespace ESock;

const TInt KReservedContainerSlots = 4;
const TInt KReservedExtensionSlots = 4;



// ===========================================================
// CMetaExtensionContainerImplBase
// ===========================================================

void CMetaExtensionContainerImplBase::DeletePtr()
    {
    }

const Meta::SMetaData& CMetaExtensionContainerImplBase::FindExtensionL(const Meta::STypeId& aTypeId) const
    {
    const Meta::SMetaData* ext = FindExtension(aTypeId);
    if (!ext)
        {
        User::Leave(KErrNotFound);
        }
    return *ext;
    }



// ===========================================================
// CMetaExtensionContainer
// ===========================================================

CMetaExtensionContainer* CMetaExtensionContainer::NewLC(const CMetaExtensionContainerImplBase* aBaseContainer)
    {
    CMetaExtensionContainer* self = new(ELeave)CMetaExtensionContainer();
    CleanupStack::PushL(self);
    LOG(ESockLog::Printf(_L8("CMetaExtensionContainer::NewLC() - [this=%x] [baseContainer=%x]"), self, aBaseContainer));
    self->ConstructL(aBaseContainer);
    return self;
    }


CMetaExtensionContainer::~CMetaExtensionContainer()
    {
    iExtensions.ResetAndDestroy();
    iExtensions.Close();
    
#ifndef MEC_UNITTEST
    LOG_NODE_DESTROY(KESockComponentTag, CMetaExtensionContainer);
#endif
    }

CMetaExtensionContainer::CMetaExtensionContainer()
	: CMetaExtensionContainerImplBase(EMetaExtensionContainer)
    {
#ifndef MEC_UNITTEST
    LOG_NODE_CREATE(KESockComponentTag, CMetaExtensionContainer);
#endif
    }


void CMetaExtensionContainer::ConstructL(const CMetaExtensionContainerImplBase* aBaseContainer)
	{
	iExtensions.ReserveL(KReservedExtensionSlots);
    if (aBaseContainer)
        {
        SetBaseContainer(aBaseContainer);
        }
	}


void CMetaExtensionContainer::DeletePtr()
    {
    // Releasing references to any base containers
    if (iBaseContainer)
        {
        const_cast<CMetaExtensionContainerImplBase*>(iBaseContainer)->Close();
        iBaseContainer = NULL;
        }    
    }


const CMetaExtensionContainerImplBase* CMetaExtensionContainer::BaseContainer() const
    {
    return iBaseContainer;
    }


void CMetaExtensionContainer::SetBaseContainer(const CMetaExtensionContainerImplBase* aBaseContainer)
    {
    ASSERT(aBaseContainer);
    ASSERT(!iBaseContainer);
    const_cast<CMetaExtensionContainerImplBase*>(aBaseContainer)->Open();
    iBaseContainer = aBaseContainer;
    }


void CMetaExtensionContainer::RemoveBaseContainer()
    {
    // Only call this if the cleanup checks have been done and it makes sense
    ASSERT(iBaseContainer && iBaseContainer->Type() == EMetaExtensionContainer);
    
    const CMetaExtensionContainer* oldBaseMecImpl = static_cast<const CMetaExtensionContainer*>(iBaseContainer);
    const CMetaExtensionContainerImplBase* newBaseMecImpl = oldBaseMecImpl->iBaseContainer;
    if (newBaseMecImpl)
        {
        // Do this before assigning iBaseContainer to ensure the reference is there
        // to hold it open
        const_cast<CMetaExtensionContainerImplBase*>(newBaseMecImpl)->Open();
        }
    iBaseContainer = newBaseMecImpl;
    const_cast<CMetaExtensionContainer*>(oldBaseMecImpl)->Close();
    }


const Meta::SMetaData* CMetaExtensionContainer::FindExtension(const Meta::STypeId& aTypeId) const
    {
	const Meta::SMetaData* ext = FindExtensionInThisContainer(aTypeId);

    if (!ext && BaseContainer())
        {
        ext = BaseContainer()->FindExtension(aTypeId);
        }
    
    return ext;
    }


const Meta::SMetaData* CMetaExtensionContainer::FindExtensionInThisContainer(const Meta::STypeId& aTypeId) const
	{
    TInt length = iExtensions.Count();
    for (TUint i = 0; i < length; ++i)
         {
         if (iExtensions[i]->Object()->GetTypeId() == aTypeId)
             {
             return iExtensions[i]->Object();
             }
         }

	return NULL;
	}



TInt CMetaExtensionContainer::AppendExtension(const Meta::SMetaData* aExtension)
    {
    TRAPD(err, AppendExtensionL(aExtension));
    return err;
    }


void CMetaExtensionContainer::AppendExtensionL(const Meta::SMetaData* aExtension)
    {
    if (aExtension == NULL)
        {
        LOG(ESockLog::Printf(_L8("CMetaExtensionContainer::AppendExtensionL() - NULL extension pointer [this=%x]"), this));
        User::Leave(KErrArgument);
        }

    if (FindExtensionInThisContainer(aExtension->GetTypeId()) != NULL)
        {        
        LOG(ESockLog::Printf(_L8("CMetaExtensionContainer::AppendExtensionL() - duplicate extension (%08x) ID %x:%x added  [this=%x]"),
                aExtension, aExtension->GetTypeId().iUid.iUid, aExtension->GetTypeId().iType, this));

#if defined(_DEBUG) && !defined(MEC_UNITTEST)
        _LIT(KMECPanic, "esockmec");
        //Included in DEBUG mode as extension should not be added if already part of iExtensions
        //Brings attention to coding error that should be fixed before release.
        __ASSERT_DEBUG(EFalse, User::Panic(KMECPanic(), 0));
#endif
        
        User::Leave(KErrAlreadyExists);
        }

    Meta::TMetaDataDeAllocator* deallocator = Meta::TMetaDataDeAllocator::NewLC(aExtension);
    TInt ret = iExtensions.Append(deallocator);
    if(KErrNone != ret)
        {
        deallocator->LeaveObject();
        User::Leave(ret);
        }

    LOG(ESockLog::Printf(_L8("CMetaExtensionContainer::AppendExtensionL() - extension (%08x) ID %x:%x added  [this=%x]"),
            aExtension, aExtension->GetTypeId().iUid.iUid, aExtension->GetTypeId().iType, this));

    CleanupStack::Pop(deallocator);
    }


void CMetaExtensionContainer::Compact()
    {
    const CMetaExtensionContainer* baseMecImpl = NULL;
    
    while (baseMecImpl != BaseContainer())
        {
        // We have a base container, and its the first time through the loop
        // or its not the same as the last execution of the loop 
        baseMecImpl = static_cast<const CMetaExtensionContainer*>(BaseContainer());
        
        if (!baseMecImpl || baseMecImpl->Type() != EMetaExtensionContainer
        		|| baseMecImpl->iExtensions.Count() > iExtensions.Count())
            {
            // There is no base container, OR the base container isn't an extension
            // container implementation, OR the immediate base container holds
            // more extensions than this container does and it cannot be removed
            // since there is no way this container can be overriding all of them.
            return;
            }
    
        // Check if all of the base container extensions are overridden by ours
        TInt count = baseMecImpl->iExtensions.Count();
        Meta::STypeId typeId; 
        for (TInt i=0; i<count; i++)
            {
            typeId = baseMecImpl->iExtensions[i]->Object()->GetTypeId();
            const Meta::SMetaData* ext = FindExtensionInThisContainer(typeId);
            if (!ext)
                {
                // Not all of the extensions in the base container are overridden
                return;
                }
            }
        
        // If we made it here this container overrides all of the extensions in the
        // immediate base container. We can remove it from our chain.
        RemoveBaseContainer();
        }
    }


// ===========================================================
// CMetaExtensionContainerArray
// ===========================================================

CMetaExtensionContainerArray* CMetaExtensionContainerArray::NewLC(const CMetaExtensionContainerImplBase* aBaseContainer)
    {
    CMetaExtensionContainerArray* self = new(ELeave)CMetaExtensionContainerArray();
    CleanupStack::PushL(self);
    LOG(ESockLog::Printf(_L8("CMetaExtensionContainerArray::NewLC() - [this=%x] [baseContainer=%x]"), self, aBaseContainer));
    self->ConstructL(aBaseContainer);
    return self;
    }


CMetaExtensionContainerArray::~CMetaExtensionContainerArray()
    {
    iContainers.Close();

#ifndef MEC_UNITTEST
    LOG_NODE_DESTROY(KESockComponentTag, CMetaExtensionContainerArray);
#endif
    }


CMetaExtensionContainerArray::CMetaExtensionContainerArray()
	: CMetaExtensionContainerImplBase(EMetaExtensionContainerArray)
    {
#ifndef MEC_UNITTEST
    LOG_NODE_CREATE(KESockComponentTag, CMetaExtensionContainerArray);
#endif
    }


void CMetaExtensionContainerArray::ConstructL(const CMetaExtensionContainerImplBase* aBaseContainer)
	{
	iContainers.ReserveL(KReservedContainerSlots);
	if (aBaseContainer)
		{
		iContainers.AppendL(aBaseContainer);
		const_cast<CMetaExtensionContainerImplBase*>(aBaseContainer)->Open();
		}
	}


void CMetaExtensionContainerArray::DeletePtr()
    {
	TInt i = iContainers.Count();
	while (i--)
		{
		const_cast<CMetaExtensionContainerImplBase*>(iContainers[i])->Close();
		}  
    }


const Meta::SMetaData* CMetaExtensionContainerArray::FindExtension(const Meta::STypeId& aTypeId) const
	{
	const Meta::SMetaData* ext = NULL;
	
	TInt i = iContainers.Count();
	while (!ext && i--)
		{
		ext = iContainers[i]->FindExtension(aTypeId);
		}
	
	return ext;
	}    


TInt CMetaExtensionContainerArray::AppendContainer(const CMetaExtensionContainerImplBase& aContainer)
	{
    TInt length = iContainers.Count();
    for (TUint i = 0; i < length; ++i)
         {
         if (iContainers[i] == &aContainer)
             {
             return KErrAlreadyExists;
             }
         }

	TInt err = iContainers.Append(&aContainer);
	if (err == KErrNone)
		{
		// Add a reference to the container
		const_cast<CMetaExtensionContainerImplBase&>(aContainer).Open();
		}
	return err;
	}




