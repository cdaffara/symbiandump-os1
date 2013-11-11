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
// Helper class supplying useful logging functions to the providers in the layer
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

#ifndef MBUFGOBBLERHELPER_H
#define MBUFGOBBLERHELPER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <elements/mm_context.h>

// CLASS DECLARATION

/**
 *  CMbufGobblerHelper
 * 
 */
class MbufGobblerHelper
	{
public:
	static void LogCreateDestroy(const TDesC8& aSubTag, const TDesC8& aNodeName, TAny* aNodePtr, TBool aIsCreate);
	static void LogMessage(const TDesC8& aSubTag, const TDesC8& aNodeName, TAny* aNodePtr, const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
	static void PrintClientNodes(const TDesC8& aSubTag, const TDesC8& aNodeName, MeshMachine::AMMNodeBase& aNode, TInt& aClientCount);
	};

#endif // MBUFGOBBLERHELPER_H
