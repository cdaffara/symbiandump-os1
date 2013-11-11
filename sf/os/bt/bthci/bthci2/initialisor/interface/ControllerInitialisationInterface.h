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

#ifndef CONTROLLERINITIALISATIONINTERFACE_H
#define CONTROLLERINITIALISATIONINTERFACE_H

#include <e32def.h>

/**
The UID of the Controller Initialisation API. If the Controller Initialisation API ever 
has to change, a new UID and associated M- class will be created. New implementations of 
CControllerInitialisationPlugin may implement the new API. Old (non-updated) Controller Initialisation Plugins 
will still work as long as the stack supports the old API.
*/
const TInt KControllerInitialisationInterfaceUid = 0x1027407C;

/**
The UID of the Controller Initialisation Abort Extension API.
*/
const TInt KControllerInitialisationAbortExtenstionInterfaceUid = 0x10286A7F;

class MHCICommandQueue;
class MControllerInitialisationObserver;
class MCoreHci;

/**
Mixin for the Controller Initialisation API.
This interface is to be used via the Controller Initialisation Plugin.
@see CControllerInitialisationPlugin::Interface(TUid)
*/
class MControllerInitialisationInterface
	{
public:
	/**
	The function that shall be called immediately before calling the reset command.
	*/
	virtual void MciiPreResetCommand() = 0;

	/**
	The function to be called immediately after calling the reset command.
	*/
	virtual void MciiPostResetCommand() = 0;
	
	/**
	The initialisation interface uses this function to be given a reference to MHCICommandQueue 
	which allows it to reset the command queue

	@param aHCICommandQueue provides interface to command queue
	*/
	virtual void MciiSetHCICommandQueue(MHCICommandQueue& aHCICommandQueue) = 0;

	/**
	The initialisation interface uses the supplied reference to notify
	the stack when initialisation is complete.

	@param aObserver Reference to the initialisation observer
	*/
	virtual void MciiSetControllerInitialisationObserver(MControllerInitialisationObserver& aObserver) = 0;

	/**
	Allows the initiator access to the Core HCI.  It can then request any of
	the interface the Core HCI exposes.
	
	@param aCoreHci Reference to the Core HCI
	*/
	virtual void MciiSetCoreHci(MCoreHci& aCoreHci) = 0;
	};

/**
Mixin for the Controller Initialisation Abort Extension API.
This interface is to be used via the Controller Initialisation Plugin.
@see CControllerInitialisationPlugin::Interface(TUid)
*/
class MControllerInitialisationAbortExtensionInterface
	{
public:
	/**
	This function will be called if either MciiPreResetCommand or MciiPostResetCommand have
	been called previously but the associated callback from MControllerInitialisationObserver has not 
	yet been called and the stack has to abort the initialisation sequence, due to another power 
	request for example. After this function has been called any outstanding callback must not 
	be made until a new call to either MciiPreResetCommand or MciiPostResetCommand. 
	*/
	virtual void MciaeiAbortInitialisation() = 0;
	};

#endif // CONTROLLERINITIALISATIONINTERFACE_H 
