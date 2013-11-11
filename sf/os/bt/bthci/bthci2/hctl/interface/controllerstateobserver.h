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

#ifndef CONTROLLERSTATEOBSERVER_H
#define CONTROLLERSTATEOBSERVER_H

#include <bluetooth/hci/hcitypes.h>

/**
Interface for receiving controller state change events.
Power state and reset events.
*/
class MControllerStateObserver
	{
public:
	enum TControllerChangeType
		/**
		This is primarily to inform the stack what it needs to do. 
		May also be used elsewhere within the HCI framework if required.
		*/
		{
		EBTNonFatalChange, /*!< No state changes in layers above required as a result of function call. */
		EBTFatalChange, /*!< State changes may be required in layers above required as a result of function call. */
		};


	/**
	Called to signal that a change in power state has occurred.
	@param aError Any error condition that has occurred.
	@param aChangeType The impact on the receiver of the call.
	@param aState The new power state (that has just been entered).
	*/
	virtual void McsoProcessPowerChange(TInt aError, TControllerChangeType aChangeType, TBTPowerState aState) = 0;

	/**
	Called to signal that the hard reset process has reached a new state.
	
	@param aError Any error condition that has occurred.
	@param aChangeType The impact on the receiver of the call.
	@param aState The new hard reset state (that has just been entered).
	*/
	virtual void McsoProcessHardResetPhaseChange(TInt aError, TControllerChangeType aChangeType, TBTHardResetState aState) = 0;
	};

#endif // CONTROLLERSTATEOBSERVER_H
