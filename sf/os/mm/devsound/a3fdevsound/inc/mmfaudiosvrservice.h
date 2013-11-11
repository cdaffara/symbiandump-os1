/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#ifndef MMFAUDIOSVRSERVICE_H
#define MMFAUDIOSVRSERVICE_H

/**
@publishedPartner 
@file
@released
*/

//  INCLUDES

#include <ecom/ecom.h>
#include <badesca.h>


/**
This is the interface for all audio service 
plugins. So Each Service plugin has to implement this interface.
*/
class MAudioSvrService
	{	
public:
	/**
	This method is used to pass a destructor key for ECom. The passed UID must be retained for the lifetime 
	of the plugin and, on a subsequent call to Release(),REComSession::DestroyedImplementation() must be 
	called by the plugin, using the stored UID.
	@param aUid The Uid returned by REComSession::CreateImplementationL() or similar.
	*/
	virtual void PassDestructorKey(TUid aUid)=0;
	/**
	This method is used to load or create the objects that are internally used by service plugin.
	@return KErrNone if successfull, else corresponding error code
	*/
	virtual TInt Load()=0;
	/**
	This method is used to start the service.
	@return KErrNone if successfull, else corresponding error code
	*/
	virtual TInt Start()=0;
	/**
	This method is used to Stop the service.
	@return KErrNone if successfull, else corresponding error code
	*/
	virtual void Stop()=0;
	/**
	This method is used to destroy plugin. This is equivalent to destructor.
	*/
	virtual void Release()=0;
	};
#endif
