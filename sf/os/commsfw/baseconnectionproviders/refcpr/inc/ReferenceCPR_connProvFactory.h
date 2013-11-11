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
// Reference Connection Provider Factory class definition.
// 
//

/**
 @file
 @internalComponent
*/

#if !defined(__REFERENCECPR_CONNPROVFACTORY_H__)
#define __REFERENCECPR_CONNPROVFACTORY_H__


#include <comms-infras/ss_connprov.h>
#include <ss_select.h>


//phony protocol family id, replace with the actual
const TUint KReferenceConnectionProviderFactoryId = 0xF0F0; 

class CReferenceProviderFactory : public CConnectionProviderFactoryBase
/**This is a sample/reference derivation of the connection provider factory.
Each such derivation should be able to produce a specific type of connection providers,
where the type usually denotes a particular technology (e.g.: IP, UMTS, SIP).
A connection provider factory is also responsible for handling selection,
i.e.: given or having access to all sorts of input (connection preferences, policies, 
bearer availablity) the factory should be able to select the provider on the given 
level and select the provider type on the level below.

@internalComponent
@released since v9.2
*/
 	{
public:

	static CReferenceProviderFactory* NewL(TAny* aParentContainer);

protected:   
	CReferenceProviderFactory(TUint aFactoryId, CConnectionFactoryContainer& aParentContainer);
	virtual CConnectionProviderBase* DoCreateProviderL();	
	virtual MProviderSelector* DoSelectProvider( Meta::SMetaData& aPreferences, ISelectionNotify& aSelectionNotify, const RMessagePtr2* aMessage );
	virtual MProviderSelector* DoSelectNextLayerProvider( Meta::SMetaData& aPreferences, ISelectionNotify& aSelectionNotify, const RMessagePtr2* aMessage );
	virtual void DoEnumerateConnectionsL(RPointerArray<TConnectionInfo>& aConnectionInfoPtrArray);
	};




class XConnectionFindSameLowerLayerQuery : public MCommsFactoryQuery
/**

@internalComponent
@released since v9.2
*/
 	{
public:
	XConnectionFindSameLowerLayerQuery( CConnectionProviderBase* aConnectionProviderBase ) :
		iConnectionProviderBase( aConnectionProviderBase )
		{
		}

protected:
	CConnectionProviderBase* iConnectionProviderBase;

public:
	virtual TMatchResult Match( TFactoryObjectInfo& aConnectionInfo );
	};





class CReferenceSelector : public CBase, public MProviderSelector
/**

@internalComponent
@released since v9.2
*/
	{
public:	
	CReferenceSelector(ISelectionNotify& aNotify, CReferenceProviderFactory& aFactory);

	TInt Select(Meta::SMetaData& aPreferences, const RMessagePtr2* aMessage);
	void SelectComplete(CConnectionProviderBase* aConnProvider, TInt aError);
	void ProgressNotification(TInt aStage, TInt aError);
	void LayerUp(TInt aError);
	void ServiceChangeNotification(TUint32 aId, const TDesC& aType);
	void SubConnectionEvent(CSubConnectionProviderBase* aSubConnNextLayerProvider, const TSubConnectionEvent& aSubConnectionEvent);
	void Detach();
	
	virtual TInt Cancel();
	virtual TInt Cancel(TInt aReason, const RMessage2* aMessage);
	
protected:
    	
	virtual ~CReferenceSelector();	
private:
    //ISelectionNotify must be stored by value, cos' it's just a short-lived wrapper class.
    //It doesn't exist as a someone that waits for the completion, but stores refereneces
    //to the one that does.
    ISelectionNotify iNotify;
    CReferenceProviderFactory& iFactory;
    MProviderSelector* iNextLayerSelector;
	};


#endif // __REFERENCECPR_CONNPROVFACTORY_H__
