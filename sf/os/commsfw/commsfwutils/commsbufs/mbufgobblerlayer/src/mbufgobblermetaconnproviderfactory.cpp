// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Factory which creates the Meta Connection Provider
//  (management plane)
//
//
//  This is a 3-plane comms layer implementation example, which has been customised to be a test layer which gobbles and releases ESOCK MBUFs.
//  The MBuf gobbling functionality can be disabled by undefining the macro SYMBIAN_COMMSFW_MBUF_GOBBLER which is specified in mbufgobblerproviders.mmp.
//  When SYMBIAN_COMMSFW_MBUF_GOBBLER is undefined, the source code specified by mbufgobblerproviders.mmp becomes a pass through layer i.e. it passes the data
//  through to the layer above or below without altering it. This makes it useful as a starting point for implementing your own layers / providers;
//  useful documentation on how to customise your own passthrough layer can be found in ..\docs\MbufGobblerLayer.doc
//

/**
 @file
 @internalComponent
*/

#include "mbufgobblermetaconnproviderfactory.h"
#include "mbufgobblermetaconnprovider.h"
#include "mbufgobblerlog.h"


CMbufGobblerMetaConnectionProviderFactory* CMbufGobblerMetaConnectionProviderFactory::NewL(TAny* aParentContainer)
	{
 	return new (ELeave) CMbufGobblerMetaConnectionProviderFactory(TUid::Uid(CMbufGobblerMetaConnectionProviderFactory::iUid), 
 			                         *(reinterpret_cast<ESock::CMetaConnectionFactoryContainer*>(aParentContainer)));
	}

CMbufGobblerMetaConnectionProviderFactory::CMbufGobblerMetaConnectionProviderFactory(TUid aFactoryUid, ESock::CMetaConnectionFactoryContainer& aParentContainer)
	: CMetaConnectionProviderFactoryBase(aFactoryUid,aParentContainer)
	{
 	//LOG_NODE_CREATE(KVccMCprFactoryTag, CVccMetaConnectionProviderFactory);
	}

ESock::ACommsFactoryNodeId* CMbufGobblerMetaConnectionProviderFactory::DoCreateObjectL(ESock::TFactoryQueryBase& aQuery)
	{ 
	//LOG1(_L8("CMbufGobblerMetaConnectionProviderFactory %08x:\tDoCreateObjectL()"), this);
	const ESock::TMetaConnectionFactoryQuery& query = static_cast<const ESock::TMetaConnectionFactoryQuery&>(aQuery);
	return CMbufGobblerMetaConnectionProvider::NewL (*this, query.iProviderInfo);
	}

void CMbufGobblerMetaConnectionProviderFactory::DoEnumerateConnectionsL (RPointerArray<TConnectionInfo>& /*aConnectionInfoPtrArray*/)
	{
	User::Leave(KErrNotSupported);
	}






