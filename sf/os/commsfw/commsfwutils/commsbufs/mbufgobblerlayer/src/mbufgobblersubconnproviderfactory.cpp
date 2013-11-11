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
// Sub Connection Provider Factory
//  (control plane)
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

#include "mbufgobblersubconnproviderfactory.h"
#include "mbufgobblersubconnprovider.h"
#include "mbufgobblerlog.h"

CMbufGobblerSubconnProviderFactory* CMbufGobblerSubconnProviderFactory::NewL(TAny* aParentContainer)
    {
    return new (ELeave) CMbufGobblerSubconnProviderFactory(TUid::Uid(CMbufGobblerSubconnProviderFactory::iUid), 
                                            *reinterpret_cast<ESock::CSubConnectionFactoryContainer*>(aParentContainer));
    }
    
CMbufGobblerSubconnProviderFactory::CMbufGobblerSubconnProviderFactory(TUid aFactoryId, ESock::CSubConnectionFactoryContainer& aParentContainer)
	: CSubConnectionProviderFactoryBase(aFactoryId, aParentContainer)
    {
    }

ESock::ACommsFactoryNodeId* CMbufGobblerSubconnProviderFactory::DoCreateObjectL(ESock::TFactoryQueryBase& aQuery)
    {
    const ESock::TDefaultSCPRFactoryQuery& query = static_cast<const ESock::TDefaultSCPRFactoryQuery&>(aQuery);
    if (query.iSCPRType == RSubConnection::EAttachToDefault)
        {
        return CMbufGobblerSubconnProvider::NewL(*this);
        }
    else if(query.iSCPRType == RSubConnection::ECreateNew)
    	{
        return CMbufGobblerSubconnProvider::NewL(*this);    	
    	}
    else
        {
        User::Leave(KErrNotSupported);
        }
    return NULL;
    }
