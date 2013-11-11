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
// Meta Connection Provider Mesh States
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

#include "mbufgobblermetaconnproviderstates.h"
#include "mbufgobblerlog.h"

CMbufGobblerMetaConnectionProviderStates::CMbufGobblerMetaConnectionProviderStates()
	{
	// No implementation required
	}

CMbufGobblerMetaConnectionProviderStates::~CMbufGobblerMetaConnectionProviderStates()
	{
	}

CMbufGobblerMetaConnectionProviderStates* CMbufGobblerMetaConnectionProviderStates::NewLC()
	{
	CMbufGobblerMetaConnectionProviderStates* self =
			new (ELeave) CMbufGobblerMetaConnectionProviderStates();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CMbufGobblerMetaConnectionProviderStates* CMbufGobblerMetaConnectionProviderStates::NewL()
	{
	CMbufGobblerMetaConnectionProviderStates* self =
			CMbufGobblerMetaConnectionProviderStates::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CMbufGobblerMetaConnectionProviderStates::ConstructL()
	{

	}
