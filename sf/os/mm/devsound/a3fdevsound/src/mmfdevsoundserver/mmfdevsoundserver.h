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

#ifndef MMFDEVSOUNDSERVER_H
#define MMFDEVSOUNDSERVER_H
	
#include <e32base.h>
#include <e32def.h>
#include <e32std.h>
#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
#include "mmfaudioserver.h"
#include "mmfdevsoundserverstart.h"

/**
	Panic category and codes that MmfDevSoundServer raises on the client
*/
_LIT(KMMFDevSoundClientPanicCategory, "DevSoundClient");

enum TMMFDevSoundClientPanicCodes
	{
	EMMFDevSoundPlayDataWithoutInitialize,
	EMMFDevSoundRecordDataWithoutInitialize
	};

enum TMMFDevSoundServerPanicCodes
	{
	EMMFDevSoundServerIsActive = 1
	};

// FORWARD DECLARATIONS
class CMMFDevSoundServer;
class MGlobalProperties;

// CLASS DECLARATION

/**
	DevSound server implementation.
	This class implements the DevSound server thread. The main purpose of this
	class is to create a new session when requested by DevSound proxy.
	
	@lib MmfDevSoundServer.lib
	@since 
*/
NONSHARABLE_CLASS( CMMFDevSoundServer ): public CMmfIpcServer
	{
public:  // Constructors and destructor

	/**
		Constructs, and returns a pointer to, a new CMMFDevSoundServer object.
		Leaves on failure.
		@param CMMFAudioServer* aAudioServer A pointer reference to Audio
		Server launchining this.
		@param TProcessId& aClientPID A reference to process id of the client
		launching DevSound server.
		@param MGlobalProperties a reference to global properties such as FourCC to format converter
		@return CMMFDevSoundServer* A pointer to newly created object.
	*/
	IMPORT_C static CMMFDevSoundServer* NewL(MGlobalProperties* aGlobalProperties,
						TProcessId& aClientPID);

	/**
		Destructor.
	*/
	~CMMFDevSoundServer();

	// New functions

	/**
		From CServer2. Creates a server-side client session object.
		@since 
		@param TVersion - server version requested by the client.
		@return CMmfIpcSession* A pointer to newly created session.
	*/
	CMmfIpcSession* NewSessionL(const TVersion &aVersion) const;

	/**
		Increment the session Id.
		@since 
		@param ?arg1 ?description
		@return ?description
		@return void
	*/
	void IncrementSessionId();

	/**
		Decrement the session Id.
		Once the number of DevSound sessions becomes zero, DevSound
		Server will start its shutdown routine.
		@since 
		@param ?arg1 ?description
		@return void
	*/
	void DecrementSessionId();

	/**
		Sends Event to DevSound client.
		@since 
		@param TInt aSessionToAlert Id of DevSound to be alerted.
		@param TInt aSessionToBeLaunched Id of DevSound to be launched.
		@return void
	*/
	void SendEventToClient(TInt aSessionToAlert, TInt aSessionToBeLaunched);

	/**
		Launches the DevSound servers waiting for Audio Policy.
		@since 
		@param TInt aSessionToBeLaunched Id of DevSound to be launched.
		@return void
	*/
	void LaunchRequest(TInt aSessionId);

	/**
		Returns DevSound Server session identifier.
		@since 
		@return TInt A integer representing session id.
	*/
	TInt DevSoundSessionId() {return iDevSoundSessionId;};


	/**
		Static entry point to launch the thread.
		@since 
		@param TAny* aParams
		@return KErrNone if successfull, else corresponding error code
	*/
	IMPORT_C static TInt StartThread(TAny* aParams);


	/**
		Returns process id of the client application.
		@since 
		@return TProcessId Process id of the client application.
	*/
	TProcessId ProcessId() {return iClientPID;};

	/**
		
		@since 
		@return void.
	*/
	void SetClientProcessIdL(TThreadId aTid);

	/**
		Returns the actual process id of the client application.
		@since 
		@return TProcessId Process id of the client application.
	*/
	TProcessId ActualProcessId() {return iActualClientPID;};
	
private:

	/**
		C++ default constructor.
	*/
	CMMFDevSoundServer(MGlobalProperties* aGlobalProperties,
					TProcessId& aClientPID);

	/**
		By default Symbian 2nd phase constructor is private.
	*/
	void ConstructL();
	static void RenamePrioritizeServer();
	static void StartThreadL(TDevSoundServerStart& aStart);
	
	CMmfIpcSession* CreateNewSessionL(const TVersion& aVersion) const;
	void CheckForNoSessionsLeft();

private:
	/**
		A Timer utility class used by DevSound Server for shutdown purpose.
	*/
	class CDelayDevSoundShutDown : public CActive
		{
	public:
		// Construct/destruct
		static CDelayDevSoundShutDown* NewL();
		~CDelayDevSoundShutDown();

	public:
		// Request a timeout after aDelay
		void SetDelay(TTimeIntervalMicroSeconds32 aDelay);

	protected:
		// From CActive
		void RunL();
		void DoCancel();

	private:
		// Construct/destruct
		CDelayDevSoundShutDown();
		void ConstructL();

	private:
		RTimer iTimer;      // Has
		};

private:    // Data
	// Id allocated to this DevSound server.
	TInt iDevSoundSessionId;
	// Session to be launched
	TInt iSessionToBeLaunched;
	// Process id of client
	TProcessId iClientPID;
	// DevSound server shutdown utlilty timer object
	CDelayDevSoundShutDown* iDelayDevSoundShutDown;
	// A pointer reference to Audio Server
	MGlobalProperties* iGlobalProperties;
	// Process id of client
	TProcessId iActualClientPID;
	};

GLDEF_C void PanicClient(const RMmfIpcMessage& aMessage,
					TMMFDevSoundClientPanicCodes aPanicCode);


#endif // MMFDEVSOUNDSERVER_H

// End of File
