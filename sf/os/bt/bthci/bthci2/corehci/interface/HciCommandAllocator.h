// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef HCICOMMANDALLOCATOR_H
#define HCICOMMANDALLOCATOR_H

#include <e32def.h>

class CHctlCommandFrame;

/** 
The UID of the HCI Command Allocator API. If this API ever has to change,
a new UID and associated M- class will be created. 

Old (non-updated) HCI Command Allocator Plugins will still work as long as the old API 
is supported
*/
const TInt KHCICommandAllocatorInterfaceUid = 0x102736EB;

/**
Mixin for the HCI Command Allocator API.
This interface is to be used via CCoreHCIPlugin.
@see CCoreHCIPlugin::Interface(TUid)
*/
class MHCICommandAllocator
	{
public:
	/**
	Creates a new command frame object.

	@return A pointer to a newly constructed instance representing the command frame.
	*/
	virtual CHctlCommandFrame* MhcaNewFrameL() = 0;
	};

#endif // HCICOMMANDALLOCATOR_H
