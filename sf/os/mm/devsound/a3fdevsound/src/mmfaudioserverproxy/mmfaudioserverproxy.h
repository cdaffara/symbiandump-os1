/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef MMFAUDIOSERVERPROXY_H
#define MMFAUDIOSERVERPROXY_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
#include "mmfaudioclientserver.h"

// FORWARD DECLARATIONS
class RMMFAudioServerProxy; // declared here;

// CLASS DECLARATION

/**
*  Client side session for AudioServer.
*
*  @lib
*  @since 
*/
NONSHARABLE_CLASS( RMMFAudioServerProxy ): public RMmfSessionBase
	{
public: // New functions

	/**
	* Creates a new session to AudioServer. The very first call will start
	* AudioServer. Upon successful completion of this method, a new instance
	* of DevSound server will be created.
	* @since 
	* @return KErrNone if successful, otherwise one of the other system-wide
	* error codes.
	*/
	IMPORT_C TInt Open();

	/**
	* Sets the DevSound info. This method is no longer used.
	* @since 
	* @return KErrNone if successful, otherwise one of the other system-wide
	* error codes.
	*/
	IMPORT_C TInt SetDevSoundInfo();

	/**
	* Returns handle to the DevSound server created by AudioServer session.
	* @since 
	* @return TInt handle to the DevSound server.
	*/
	IMPORT_C TInt GetDevSoundSessionHandle();
	
private:

	TInt OpenSessionToTrustedAudioServer();
	};

#endif      // MMFAUDIOSERVERPROXY_H

// End of File
