// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef HCTLUARTPOWEROBSERVER_H
#define HCTLUARTPOWEROBSERVER_H

#include <e32def.h>

/**
Interface for monitoring the power state of a UART device and receiving
notification when the power state changes.

This interface can be implemented by any UART based HCTL implementations
and should be used with the CHCTLUartPowerManager class.
*/
class MHCTLUartPowerObserver
	{
public:
	/**
	Called when the power state of the UART device changes. If an error
	is returned then monitoring of the power state will have stopped and
	should be specifically re-started if required.
	
	@param aError Any error that occurred while monitoring power state.
	*/
	virtual void MhupoPowerChange(TInt aError) =0;
	};

#endif // HCTLUARTPOWEROBSERVER_H
