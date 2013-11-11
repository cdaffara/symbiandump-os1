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



#ifndef MMFAUDIOSERVERSESSION_H
#define MMFAUDIOSERVERSESSION_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
#include "mmfaudioclientserver.h"


// CLASS DECLARATION

/**
*  Server-side session implementation.
*
*  @lib MmfAudioServer.exe
*/
class CMMFAudioServerSession : public CMmfIpcSession
	{
public:  

	/**
	* Constructs, and returns a pointer to, a new CMMFAudioServerSession
	* object.
	* @param aDevSoundSessionHandle. A handle to the DevSound server session.
	* @return CMMFAudioServerSession* a pointer to newly created object.
	*/
	static CMMFAudioServerSession* NewL(
									RHandleBase& aDevSoundSessionHandle);

	/**
	* Destructor.
	*/
	~CMMFAudioServerSession();

	/**
	* Creates a new server-side session.
	* @param aServer - audioserver referencei
	*/
	void CreateL(const CMmfIpcServer& aServer);

	/**
	* Handles the client requestsi
	* @param const aMessage - Request from client.
	*/
	void ServiceL(const RMmfIpcMessage& aMessage);

	/**
	* Returns the AudioServer session ID.
	* @return TInt A integer representing audio server session ID.
	*/
	TInt AudioServerSessionId() {return iAudioServerSessionId;};

private:  
	//Constructor.
	
	CMMFAudioServerSession(RHandleBase& aDevSoundSessionHandle);

	//Second phase constructor.
	
	void ConstructL();

private:    // Data
	// Integer representing audio server session.
	TInt iAudioServerSessionId;
	// This is the handle to the DevSound server session.
	RHandleBase iDevSoundSessionHandle;

	};

#endif // MMFAUDIOSERVERSESSION_H

// End of File
