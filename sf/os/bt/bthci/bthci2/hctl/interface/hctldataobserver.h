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

#ifndef HCTLDATAOBSERVER_H
#define HCTLDATAOBSERVER_H

#include <e32def.h>

class TDesC8;

/** The UID associated with this version of the incoming data interface. */ 
const TInt KHCTLDataObserverUid = 0x102736F0;

/**
Interface for receiving HCI data (ACL and Synchronous) from the HCTL.

Licensee is free to replace this interface because it is used only between
licensee components.
*/
class MHCTLDataObserver
	{
public:
	/**
	Called when ACL data has been received from over the HCTL.

	@param aData The ACL data.
	*/
	virtual void MhdoProcessAclData(const TDesC8& aData) =0;
	
	/**
	Called when Synchronous data has been received from over the HCTL.

	@param aData The Synchronous data.
	*/
	virtual void MhdoProcessSynchronousData(const TDesC8& aData) =0;
	};

#endif // HCTLDATAOBSERVER_H
