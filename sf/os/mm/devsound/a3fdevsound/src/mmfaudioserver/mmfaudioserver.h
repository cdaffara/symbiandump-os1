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




/**
 @file
 @internalTechnology 
*/



#ifndef MMFAUDIOSERVER_H
#define MMFAUDIOSERVER_H

#include <e32base.h>
#include <e32std.h>
#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
#include <a3f/a3ffourcclookup.h>
#include "mglobalproperties.h"


// FORWARD DECLARATIONS
class CStartAndMonitorDevSoundThread;
class CMMFAudioServerFactory;
class RMMFDevSoundServerProxy;

enum TMMFAudioServerPanicCodes
	{
	EMMFAudioServerIsActive = 1
	};

/**
	The Audio Server holds a list of DevSound servers and manages it.
	This class is a container process for all the DevSound Server threads that are
	created for each DevSound proxy and for all other servers that are created
	using Factory.
	
	@lib MmfAudioServer.exe
*/
class CMMFAudioServer : public CMmfIpcServer, public MGlobalProperties
	{
public:

	/**
		Constructs, and returns a pointer to, a new CMMFAudioServer
		object.
		@return CMMFAudioServer* A pointer to newly created object.
	*/
	static CMMFAudioServer* NewL();

	/**
		Destructor.
	*/
	~CMMFAudioServer();



	/**
		From CServer2. Creates a server-side client session object.
		@param aVersion server version requested by the client.
		@return CMmfIpcSession* A pointer to newly created session.
	*/
	CMmfIpcSession* NewSessionL(const TVersion &aVersion) const;

	/**
		Increment the session Id.
	*/
	void IncrementSessionId();

	/**
		Decrement the session Id.
	*/
	void DecrementSessionId();

	/**
		Increment the DevSound server counter.
	*/
	void IncrementDevSoundCount();

	/**
		Decrement the DevSound server counter.
		Once the number of DevSound server's instances becomes zero, Audio
		Server will start its shutdown routine.
	*/
	void DecrementDevSoundCount();

	/**
		Sends Event to DevSound client.
		@param  aSessionToAlert Id of DevSound to be alerted.
		@param  aSessionToBeLaunched Id of DevSound to be launched.
	*/
	void SendEventToClient(TInt aSessionToAlert, TInt aSessionToBeLaunched);

	/**
		Launches the DevSound servers waiting for Audio Policy.
		@param  aSessionToBeLaunched Id of DevSound to be launched.
	*/
	void LaunchRequest(TInt aSessionId);

	/**
		Returns Audio Server session identifier.
		@return a integer representing session id.
	*/
	TInt AudioServerSessionId() {return iAudioServerSessionId;};

	/**
		Starts a new DevSound server instance.
		@param  aDevSoundSessionHandle. A handle to the DevSound server session to be launched.
		@return KErrNone if successfull, else corresponding error code.
	*/
	TInt StartDevSoundServer(RMMFDevSoundServerProxy& aDevSoundSessionHandle) const;

	// From MGlobalProperties
	
	/**
	 * Returns reference to FourCC to format converter
	 * (other items were commented in a header).
	*/
	inline const CFourCCConvertor& GetFourCCConvertor();

private:
	
		//A timer utility class used by Audio Server for shutdown purpose.
	
	class CDelayAudioServerShutDown : public CActive
		{
	public:
	
	
		//Constructs and returns a pointer to a new CDelayAudioServerShutDown object.
		static CDelayAudioServerShutDown* NewL();
	
		//Destructor.
		~CDelayAudioServerShutDown();

		//Request a timeout after aDelay
		void SetDelay(TTimeIntervalMicroSeconds32 aDelay);

	private:
		// From CActive
		void RunL();
		void DoCancel();

	private:
		//Standart Constructor
		CDelayAudioServerShutDown();
		//Second phase contructor.
		void ConstructL();

		RTimer iShutDownTimer;
		};

private:

	
	//Default constructor.
	
	CMMFAudioServer();

	
	//Second phase constructor.

	void ConstructL();

private:	// Data
	// FourCC to format convertor
	CFourCCConvertor* iFourCCConvertor;
	// Current session id
	TInt iAudioServerSessionId;
	// Number of DevSound servers.
	TInt iDevSoundCount;
	// Audio Server shutdown utility
	CDelayAudioServerShutDown* iDelayAudioServerShutDown;
	// Array of DevSound server monitor objects.
	mutable RPointerArray<CStartAndMonitorDevSoundThread> iDevSoundServList;
	
	CMMFAudioServerFactory* iFactory;
	};

/**
	A utility class to monitor the DevSound server state.
	This class is used by Audio Server to monitor the state of DevSound server
	and to manage the client side RThread handle.

	@lib MmfAudioServer.exe
*/
class CStartAndMonitorDevSoundThread: public CActive
	{
public:	// Constructors and destructor

	/**
		Constructs, and returns a pointer to, a new CStartAndMonitorDevSoundThread object.
		
		@return CStartAndMonitorDevSoundThread* a pointer to newly created
		object.
	*/
	static CStartAndMonitorDevSoundThread* NewL(CMMFAudioServer* aAudioServer);

	/**
		Destructor.
	*/
	~CStartAndMonitorDevSoundThread();


	/**
		Starts a new instance of DevSound server. The server is given a unique
		name using aDevSoundName and aUniqueNum passed in by Audio Server.
		
		@param aMessage. A reference to client message requesting to create a new DevSound server.
		@param aDevSoundSessionHandle. A handle to the DevSound server session.
		@return KErrNone if succesful else a systemwide error code.
	*/
	TInt StartDevSoundServer(RMessage2& aMessage,RMMFDevSoundServerProxy& aDevSoundSessionHandle);

private: // Functions from base classes

	// From CActive
	//Called by Active Object framework when DevSound server is destroyed.
	void RunL();
	
	//Called by Active Object framework when client cancels active object.
	void DoCancel();
	
	
	//Constructor.
	CStartAndMonitorDevSoundThread(CMMFAudioServer* aAudioServer);
	
	//Second phase constructor.
	void ConstructL();
	
private:	// Data
	// A pointer reference to Audio Server.
	CMMFAudioServer* iAudioServer;
	// Client side handle to DevSound server thread.
	RThread iServer;
	};


/**
	DevSound server client-side proxy class.
	This class is used by the Audio Server to open new session with
	the DevSound server.
	
	@lib MmfAudioServer.exe
*/
class RMMFDevSoundServerProxy : public RMmfSessionBase
	{
public:
	/**
		Opens new session with the DevSound server. The server is given a
		unique handle passed in by the Audio Server.

		@param aDevSoundServerHandle A handle to the DevSound server.
		@return KErrNone if successfull, else corresponding error code.
	*/
	TInt Open(RServer2& aDevSoundServerHandle);
	};

#include "mmfaudioserver.inl"

#endif // MMFAUDIOSERVER_H

// End of File

