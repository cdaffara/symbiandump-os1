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
// Meta Connection Provider
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

#include "mbufgobblermetaconnprovider.h"
#include "mbufgobblermetaconnprovideractivities.h"
#include "mbufgobblerlog.h"
#include "mbufgobblertestflags.h"

_LIT8(KNodeName, "CMbufGobblerMetaConnectionProvider");

CMbufGobblerMetaConnectionProvider* CMbufGobblerMetaConnectionProvider::NewL(ESock::CMetaConnectionProviderFactoryBase& aFactory,
                                   const ESock::TProviderInfo& aProviderInfo)
	{
	CMbufGobblerMetaConnectionProvider* self = new (ELeave) CMbufGobblerMetaConnectionProvider(aFactory,aProviderInfo,MbufGobblerMCprActivities::MbufGobblerMCprActivitiesMap::Self());
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CMbufGobblerMetaConnectionProvider::CMbufGobblerMetaConnectionProvider(ESock::CMetaConnectionProviderFactoryBase& aFactory,
				const ESock::TProviderInfo& aProviderInfo, const MeshMachine::TNodeActivityMap& aActivityMap)
	: CCoreMetaConnectionProvider(aFactory, aProviderInfo, aActivityMap)
	{
	MBUFGOBBLER_LOG_NODE_CREATE(KMbufGobblerMcprSubTag, CMbufGobblerMetaConnectionProvider, KNodeName, this->NodeId().Ptr());	
	}

void CMbufGobblerMetaConnectionProvider::ConstructL()
	{
	CCoreMetaConnectionProvider::ConstructL();
	MBUFGOBBLER_TEST_DATA_INIT
    MBUFGOBBLER_TEST_POINT(KMCPrUp,KErrNone)
	}

CMbufGobblerMetaConnectionProvider::~CMbufGobblerMetaConnectionProvider()
	{
	MBUFGOBBLER_LOG_NODE_DESTROY(KMbufGobblerMcprSubTag, CMbufGobblerMetaConnectionProvider, KNodeName, this->NodeId().Ptr());
	}

void CMbufGobblerMetaConnectionProvider::ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aCFMessage)
	{   
	LOGMESSAGE(KMbufGobblerMcprSubTag, KNodeName, this, aSender, aRecipient, aCFMessage);
	LOG_NODE_INFO(KMbufGobblerMcprSubTag, KNodeName, *this, iClientCount);
	MeshMachine::TNodeContext<CMbufGobblerMetaConnectionProvider> ctx(*this, aCFMessage, aSender, aRecipient);
	CCoreMetaConnectionProvider::Received(ctx);
	User::LeaveIfError(ctx.iReturn);
	}

