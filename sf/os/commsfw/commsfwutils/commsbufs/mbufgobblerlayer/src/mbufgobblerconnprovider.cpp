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
// Connection Provider
//  (control plane)
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

#include <comms-infras/ss_connprov.h>

#include "mbufgobblerconnprovider.h"
#include "mbufgobblerconnprovideractivities.h"
#include "mbufgobblerlog.h"
#include "mbufgobblertestflags.h"

using namespace MbufGobblerCprActivities;

_LIT8(KNodeName, "CMbufGobblerConnProvider");

CMbufGobblerConnProvider* CMbufGobblerConnProvider::NewL(ESock::CConnectionProviderFactoryBase& aFactory)
    {
    CMbufGobblerConnProvider* prov = new (ELeave) CMbufGobblerConnProvider(aFactory);
    CleanupStack::PushL(prov);
    prov->ConstructL();
    CleanupStack::Pop(prov);
    return prov;
    }

CMbufGobblerConnProvider::CMbufGobblerConnProvider(ESock::CConnectionProviderFactoryBase& aFactory)
:  CCoreConnectionProvider(aFactory, MbufGobblerCprActivities::MbufGobblerCprActivitiesMap::Self())
	{
	MBUFGOBBLER_LOG_NODE_CREATE(KMbufGobblerCprSubTag, CMbufGobblerConnProvider, KNodeName, this->NodeId().Ptr());
	}



void CMbufGobblerConnProvider::ConstructL()
	{
	CCoreConnectionProvider::ConstructL();
	MBUFGOBBLER_TEST_DATA_INIT
    MBUFGOBBLER_TEST_POINT(KCPrUp,KErrNone)
	}

CMbufGobblerConnProvider::~CMbufGobblerConnProvider()
    {
    MBUFGOBBLER_LOG_NODE_DESTROY(KMbufGobblerCprSubTag, CMbufGobblerConnProvider, KNodeName, this->NodeId().Ptr());
    }

void CMbufGobblerConnProvider::ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage)
	{
	LOGMESSAGE(KMbufGobblerCprSubTag, KNodeName, this, aSender, aRecipient, aMessage);
	LOG_NODE_INFO(KMbufGobblerCprSubTag, KNodeName, *this, iClientCount);

	MeshMachine::TNodeContext<CMbufGobblerConnProvider> ctx(*this, aMessage, aSender, aRecipient);
	Received(ctx);
	User::LeaveIfError(ctx.iReturn);
	}

