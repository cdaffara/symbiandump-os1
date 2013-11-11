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
// Reference (empty) implementation file for a SubConnection Provider Factory
//
//

/**
 @file
 @internalComponent
*/

#include <implementationproxy.h>
#include <ss_glob.h>

#include "ReferenceCPR_connProv.h"
#include "ReferenceCPR_connProvFactory.h"

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_RefCprConProvFac, "RefCprConProvFac");
#endif

//-=========================================================
// Data/functions required for instantiating ECOM Plugin
//-=========================================================
const TInt KReferenceConnectionProviderImplementationUid=0x102738D0;

const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KReferenceConnectionProviderImplementationUid, CReferenceProviderFactory::NewL)
	};

/**
ECOM Implementation Factory
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }





//-=========================================================
// CReferenceProviderFactory
//-=========================================================

/**NewL is the actual ECOM Connection Provider interface implemented.
ESOCK will call it to instantiate the factory and store it in the
CConnectionProviderFactoryContainer.

@param aConstructionParameters construction data passed by ECOM
@returns pointer to a constructed factory
*/
CReferenceProviderFactory* CReferenceProviderFactory::NewL(TAny* aParentContainer)
	{
 	return new (ELeave) CReferenceProviderFactory(KReferenceConnectionProviderFactoryId,
 					*(reinterpret_cast<CConnectionFactoryContainer*>(aParentContainer)));
	}

/**C'tor
@param aFactoryId - the id of this factory. The id should represent the type of subconnection
                    providers this factory can produce.
@param aParentContainer - the factory container the new factory object should add itself to.
*/
CReferenceProviderFactory::CReferenceProviderFactory(TUint aFactoryId, CConnectionFactoryContainer& aParentContainer)
:CConnectionProviderFactoryBase(aFactoryId, aParentContainer)
	{
	}

CConnectionProviderBase* CReferenceProviderFactory::DoCreateProviderL()
	{
    return CReferenceConnectionProvider::NewL(*this);
	}


/**A sample/reference derivation of DoSelectProvider. The method is responsible for:
- selecting an instance of the connection provider at this level of the connection provider stack.
- determining the connection provider type at the layer below this
- triggering the selection at the layer below.

@param aPreferences     - connection preferences for the connection to be selected.
@param aSelectionNotify - the notify interface (see class ISelectionNotify and see its usage
                          in the implementation file).
@param aMessage         - the orignal IPC message.
@returns pointer to a MProviderSelector object. The pointer can be used by the caller to cancel the
selection. Please note, this is a self-destructing object - the caller should assume the pointer
becomes invalid after the selection.
*/
MProviderSelector* CReferenceProviderFactory::DoSelectProvider( Meta::SMetaData& aPreferences, ISelectionNotify& aSelectionNotify, const RMessagePtr2* aMessage )
	{
	CReferenceSelector* selector = new CReferenceSelector(aSelectionNotify,*this);
	if (NULL == selector)
		{
		aSelectionNotify.SelectComplete(NULL, KErrNoMemory);
		}
	else if (selector->Select(aPreferences, aMessage) != KErrNone)
		{
		selector = NULL;
		}
	return selector;

  	}

/**A sample/reference derivation of DoSelectNextLayerProvider. The method is responsible for:
- determining the connection provider type at the layer below this
- triggering the selection at the layer below.
One can observe that DoSelectNextLayerProvider has a subset of the responsibilities held by
DoSelectNextLayerProvider.

@param aPreferences     - connection preferences for the connection to be selected.
@param aSelectionNotify - the notify interface (see class ISelectionNotify and see its usage
                          in the implementation file).
@param aMessage         - the orignal IPC message.
@returns pointer to a MProviderSelector object. The pointer can be used by the caller to cancel the
selection. Please note, this is a self-destructing object - the caller should assume the pointer
becomes invalid after the selection.
*/
MProviderSelector* CReferenceProviderFactory::DoSelectNextLayerProvider( Meta::SMetaData& aPreferences, ISelectionNotify& aSelectionNotify, const RMessagePtr2* aMessage )
	{
	//This method should determine the type (factoryId) of the connection below this layer.
	TInt factoryId = KInvalidFactoryId;
	//When done
	CConnectionFactoryContainer* connectionFactories = SockManGlobals::Get()->iConnectionFactories;
	__ASSERT_DEBUG(connectionFactories, User::Panic(KSpecAssert_RefCprConProvFac, 1));
	CConnectionProviderFactoryBase* factory = connectionFactories->FindFactory(factoryId);

	return factory->SelectProvider(aPreferences, aSelectionNotify, aMessage);
	}

void CReferenceProviderFactory::DoEnumerateConnectionsL(RPointerArray<TConnectionInfo>& /*aConnectionInfoPtrArray*/)
	{
	//TODO: enumerate connections.
	}








//-=========================================================
// CReferenceSelector
//-=========================================================

CReferenceSelector::CReferenceSelector(ISelectionNotify& aNotify, CReferenceProviderFactory& aFactory)
:iNotify(aNotify),
 iFactory(aFactory),
 iNextLayerSelector(NULL)
	{
	}

//CReferenceSelector
TInt CReferenceSelector::Cancel()
	{
	//TODO: cancel the selection
    if (iNextLayerSelector)
        {
        return iNextLayerSelector->Cancel();
        }
    return KErrNotReady;
	}

TInt CReferenceSelector::Cancel(TInt aReason, const RMessage2* aMessage)
	{
	//TODO: cancel the selection
    if (iNextLayerSelector)
        {
        return iNextLayerSelector->Cancel(aReason, aMessage);
        }
    return KErrNotReady;
	}

TInt CReferenceSelector::Select(Meta::SMetaData& aPreferences, const RMessagePtr2* aMessage)
    {
	//Prepare ISelectionNotify. MProviderSelector may choose the events it wishes to be notified
	//about by implementing a subset of the possible upcalls. The list of the actually implemented
	//is used to create an ISelectionNotify object (a loose function pointer holder) and passed
	//to the factory below to perform the selection at the lower level.
	ISelectionNotify selectNotify( this, TSelectionNotify<CReferenceSelector>::SelectComplete,
	                                     TProgressNotify<CReferenceSelector>::ProgressNotification,
	                                     TServiceChangeNotify<CReferenceSelector>::ServiceChangeNotification,
	                                     TLayerUp<CReferenceSelector>::LayerUp,
	                                     TSubConnectionEventTmpl<CReferenceSelector>::SubConnectionEvent,
	                                     TDetachNotify<CReferenceSelector>::Detach);

	iNextLayerSelector = iFactory.SelectNextLayerProvider(aPreferences, selectNotify, aMessage);
	return (iNextLayerSelector ? KErrNone : KErrNotReady);
    }


void CReferenceSelector::SelectComplete(CConnectionProviderBase* aConnProvider, TInt aError)
    {
    CReferenceConnectionProvider* connProvider = NULL;
    if (aError == KErrNone)
        {
	    __ASSERT_DEBUG(aConnProvider, User::Panic(KSpecAssert_RefCprConProvFac, 2));
        XConnectionFindSameLowerLayerQuery query(aConnProvider);

        TRAP( aError, connProvider = static_cast<CReferenceConnectionProvider*>(iFactory.FindOrCreateProviderL(query)));
        if (aError == KErrNone && connProvider->NextLayer() == NULL)
            {
            //the factory returned a new instance - must set the lower layer
            TRAP(aError,connProvider->JoinNextLayerL(aConnProvider));
            }
        }
    iNotify.SelectComplete(connProvider, aError);
    }

void CReferenceSelector::ProgressNotification(TInt aStage, TInt aError)
    {
    iNotify.ProgressNotification(aStage, aError);
    }

void CReferenceSelector::LayerUp(TInt aError)
	{
    iNotify.LayerUp(aError);
	}

void CReferenceSelector::SubConnectionEvent(CSubConnectionProviderBase* aSubConnNextLayerProvider, const TSubConnectionEvent& aSubConnectionEvent)
	{
	iNotify.SubConnectionEvent(aSubConnNextLayerProvider, aSubConnectionEvent);
	}

void CReferenceSelector::ServiceChangeNotification(TUint32 aId, const TDesC& aType)
	{
    iNotify.ServiceChangeNotification(aId, aType);
	}

void CReferenceSelector::Detach()
	{
	iNotify.Detach();
	iNextLayerSelector = NULL;
	delete this;
	}

CReferenceSelector::~CReferenceSelector()
    {
	if (iNextLayerSelector)
		{
		iNextLayerSelector->Cancel();
		}
    }




//-=========================================================
// XConnectionFindSameLowerLayerQuery
//-=========================================================

MCommsFactoryQuery::TMatchResult XConnectionFindSameLowerLayerQuery::Match( TFactoryObjectInfo& aProviderInfo )
	{
	CConnectionProviderBase* prov = static_cast<CConnectionProviderBase*>(aProviderInfo.iInfo.iFactoryObject);
	return prov->NextLayer() == iConnectionProviderBase ? EMatch : EContinue;
	}

