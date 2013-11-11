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
// Implementation file for the the Reference SubConnection Provider Factory
// 
//

/**
 @file
 @internalComponent
*/


#include "ReferenceSCPR_subconnProvFactory.h"
#include "ReferenceSCPR_defaultSubconnProv.h"
#include "ReferenceSCPR_subconnProv.h"
#include "ReferenceSCPR_subconparams.h"
#include <implementationproxy.h>

//The following represents the Factory ID for the Reference subconnection
//provider. See the assisting *.rss file in the ../src directory.
const TUint KReferenceSubConnectionProviderImplementationUid = 0x102738C3;

//-=========================================================
// Data/functions required for instantiating ECOM Plugin
//-=========================================================
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KReferenceSubConnectionProviderImplementationUid, CReferenceSubConnectionProviderFactory::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KSubConReferenceParamsUid, CReferenceSubConnExtensionParamsFactory::NewL)
	};


EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }





//-=========================================================
// CReferenceSubConnectionProviderFactory
//-=========================================================

CReferenceSubConnectionProviderFactory* CReferenceSubConnectionProviderFactory::NewL(TAny* aConstructionParameters)
/**NewL is the actual ECOM SubConnection Provider interface implemented.
ESOCK with call it to instantiate the factory and store it the 
CSubConnectionProviderFacoryContainer.
   
@param aConstructionParameters construction data passed by ECOM
@returns pointer to a constructed factory 
*/
	{
	CReferenceSubConnectionProviderFactory* ptr = 
	    new (ELeave) CReferenceSubConnectionProviderFactory(KReferenceSubConnectionProviderFactoryId, 
	    *(reinterpret_cast<CSubConnectionFactoryContainer*>(aConstructionParameters)));
	return ptr;
	}

	

CReferenceSubConnectionProviderFactory::CReferenceSubConnectionProviderFactory(TUint aFactoryId, 
                                        CSubConnectionFactoryContainer& aParentContainer)
:CSubConnectionProviderFactoryBase(aFactoryId, aParentContainer)
/**C'tor
@param aFactoryId - the id of this factory. The id should represent the type of subconnection 
                    providers this factory can produce.
@param aParentContainer - the factory container the new factory object should add itself to.
*/
	{
	}
	


/**D'tor

*/	
CReferenceSubConnectionProviderFactory::~CReferenceSubConnectionProviderFactory()
    {
    }
    

/**A sample/reference derivation of DoCreateProviderL. The method must be overriden to create 
(instantiate) the actual CSubConnectionProviderBase objects.

@param aConnProvider - the connection, the subconnection provider object should belong to. 
@param aType - the type of the subconnection provider object within the meaning defined by 
			   RSubConnection::TSubConnType:
	           * ECreateNew - the subconnection provider object shall represent a new, reserved, 
	           	 private flow.
	           * EAttachToDefault - the subconnection provider object shall represent the 
	             default channel - a singular channel that each connection always has.
*/
CSubConnectionProviderBase* CReferenceSubConnectionProviderFactory::DoCreateProviderL(
                                   CConnectionProviderBase& aConnProvider, 
                                   RSubConnection::TSubConnType aType)
    {
    CSubConnectionProviderBase* provider = NULL;
    if (aType == RSubConnection::EAttachToDefault)
        {
        provider = CReferenceDefaultSubConnectionProvider::NewL(*this, aConnProvider);
        }
    else if (aType == RSubConnection::ECreateNew)
        {
        provider = CReferenceSubConnectionProvider::NewL(*this, aConnProvider);
        }
    else
        {
        User::Leave(KErrNotSupported);
        }
    return provider;
    }
