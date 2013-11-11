// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef REMCONERROROBSERVER_H
#define REMCONERROROBSERVER_H

/** 
@file
@publishedAll
@released
*/

#include <e32base.h>

/**
This class defines an error notification interface.  This is
intended for use in the situation where a client application
needs to know about an error that has occurred passively.  
This is not used in situations where a client application 
makes a call that returns an error.

The main use is where a target application is sitting waiting
to receive commands and the server dies.  It needs to know if
this has happened so that it can restart the server if it
wishes.
*/
class MRemConErrorObserver
	{
public:
	/**
	This is called in the case of a session error that has
	occurred passively and cannot be notified via completion
	of an outstanding call made by the client application.
	
	@param aError The error that has occurred.  If this is 
		   KErrServerTerminated, the error is fatal and the 
		   server must be restarted before any new 
		   messages can be received.
	*/
	virtual void MrceoError(TInt aError) = 0;
	};

#endif // REMCONERROROBSERVER_H
