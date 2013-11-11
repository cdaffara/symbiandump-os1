// MmfDevSoundCustomInterfaceExtensions.h

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

#ifndef MMFDEVSOUNDCUSTOMINTERFACEEXTENSIONS_H
#define MMFDEVSOUNDCUSTOMINTERFACEEXTENSIONS_H

//INCLUDES
#include <mmf/common/mmfcontrollerframework.h>
#include <mmf/common/mcustomcommand.h>
#include <mmf/common/mcustominterface.h>
#include <a3f/mmfdevsoundcustominterfaceextensions.hrh>

//CLASS DECLARATION	
/**
Mixins for client-side Custom Interface Extension.
*/
class MDevSoundCIClientExtension
	{
public:
	/**
	Must be called after successfully creating ECOM implementation.
	@param aCustomCommand  Reference to implementation of CI Extension Support interface.
	@return an error code KErrNone if successful, otherwise one of the system wide error codes.
	*/
	virtual TInt Setup(MCustomCommand& aCustomCommand) = 0;
	/**
	Called to obtain a pointer to the Custom Interface Extension.
	This allows a Custom or Extension Interface, that uses a pattern different to the standard Custom Interface, to be implemented.
	Note that ownership of the interface will depend on the UID.
	@param aUid UID of the required interface.
	@param aInterface Used to return pointer to the interface. Must be NULL if no matching interface.
	@return an error code KErrNone if successful, otherwise one of the system wide error codes.
	*/
	virtual TInt CustomInterfaceExtension(TUid aUid, TAny*& aInterface) = 0;
	/**
	Equivalent to destructor. Called to destroy plugin.
	*/
	virtual void Release() = 0;
	/**
	Called by framework during construction (after Setup) and is used to pass a destructor key for ECom.
	The passed UID must be retained for the lifetime of the plugin and, on a subsequent call to Release(),
	REComSession::DestroyedImplementation() must be called by the plugin, using the stored UID.
	@param aDestructorKey The Uid returned by REComSession::CreateImplementationL() or similar.
	*/
	virtual void PassDestructorKey(TUid aDestructorKey) = 0;
	};

//CLASS DECLARATION
/**
Mixins for server-side Custom Interface Extension.
*/
class MDevSoundCIServerExtension
	{
 public:
	/**
	Will be called by framework after successfully creating ECOM implementation.
	@param aInterface Reference to implementation of CI Extension Support interface.
	@return KErrNone if successful. System wide error on failure.
	*/
	virtual TInt Setup(MCustomInterface& aInterface) = 0;
	/**
	Called by the Multimedia framework when a message is received.
	The implementation of this method is responsibile for completing the message.
	@param aMessage Message received by MM framework
	@return KErrNone if successfully handled. KErrNotSupported if message is not handled.
	@leave System wide error.
	*/
	virtual TInt HandleMessageL(const RMmfIpcMessage& aMessage) = 0;
	/**
	Equivalent to destructor. Called to destroy plugin.
	*/
	virtual void Release() = 0;
	/**
	Called by framework during construction (after Setup) and is used to pass a destructor key for ECom.
	The passed UID must be retained for the lifetime of the plugin and, on a subsequent call to Release(),
	REComSession::DestroyedImplementation() must be called by the plugin, using the stored UID.
	@param aDestructorKey The Uid returned by REComSession::CreateImplementationL() or similar.
	*/
	virtual void PassDestructorKey(TUid aDestructorKey) = 0;
	};


#endif 
