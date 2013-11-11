/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*    This is the definition for CDosServer class.
*
*/


#ifndef __DOSSERVER_H__
#define __DOSSERVER_H__

#include <e32base.h>
#include "dosclientserver.h"
#include "shareddatafilesystemnotifier.h"
#include "SaeThread.h"

// ----------------------------------------------------------------------------------------
// DosServer's policy
// ----------------------------------------------------------------------------------------

//Total number of ranges
const TUint KDosServerRangeCount = 4;

//Definition of the ranges of IPC numbers
const TInt KDosServerRanges[KDosServerRangeCount] = 
        {
        0, // 0th range, Subsession opening and closing
        EGetSimLanguage, // 1st range, all other than request free disk space operations
        ERequestFreeDiskSpace, // 2nd range, request free disk space operations
        ERequestFreeDiskSpaceCancel+1 // non implemented function end of range check ; ENotSupported
        }; 

//Policy to implement for each of the above ranges        
const TUint8 KDosServerElementsIndex[KDosServerRangeCount] = 
        {
        0,	//applies to 0th range
        1,  //applies to 1st range
        2,  //applies to 2nd range
        CPolicyServer::ENotSupported  //applies to 1st range (out of range IPC)
        };

//Specific capability checks
const CPolicyServer::TPolicyElement KDosServerElements[] = 
        {
        {_INIT_SECURITY_POLICY_C2( ECapabilityReadUserData, 
                                   ECapabilityWriteUserData ), CPolicyServer::EFailClient}, //policy "0"
        {_INIT_SECURITY_POLICY_C4( ECapabilityReadUserData, 
                                   ECapabilityWriteUserData,
				                   ECapabilityReadDeviceData,
			                       ECapabilityWriteDeviceData  ), CPolicyServer::EFailClient}, //policy "1"
        {_INIT_SECURITY_POLICY_C2( ECapabilityReadUserData, 
                                   ECapabilityWriteUserData ), CPolicyServer::EFailClient}  //policy "2"
        };

//Package all the above together into a policy
const CPolicyServer::TPolicy KDosServerPolicy =
        {
        CPolicyServer::EAlwaysPass, //specifies all connect attempts should pass
        KDosServerRangeCount,	    //number of ranges                                   
        KDosServerRanges,	        //ranges array
        KDosServerElementsIndex,    //elements<->ranges index
        KDosServerElements,		    //array of elements
        };

class CDosFactoryBase;
class CDosEventNotifier;
class CDosEventManager;

/**
* Panics the client thread.
* @param aMessage Client-server message.
* @param aPanic Panic code.
*/
void PanicClient(const RMessage2& aMessage,TDosPanic aPanic);

_LIT(KDosLibName,"DosSrv.dll");
_LIT(KDosServerExe,"DosServer.exe");
// Startup semaphore name
_LIT( KServerStartupSemaphoreName, "DosServerStartupSem" );

#ifdef __WINS__
const TInt KDosServerStackSize=0x2000; 		//	8KB
const TInt KDosServerInitHeapSize=0x1000;		//	4KB
const TInt KDosServerMaxHeapSize=0x1000000;		// 16MB
#endif

/**
* Server class.
*/
class CDosServer : public CPolicyServer
	{
public:
	/**
	* signal class for waiting while thread/process is being set up.
	*/
	class TSignal  
	{
	public:
		inline TSignal();
		inline TSignal( TRequestStatus& aStatus );

		inline TInt Set( const TDesC& aData );
		inline TPtrC Get() const;

	public:
		TRequestStatus* iStatus;
		TThreadId iId;
	};

public:

    /**
	* Constructor.
	*/
	CDosServer();

	/**
	* Destructor.
	*/
	virtual ~CDosServer();

	/**
	* NewL function that creates the object.
	* @return A pointer to the newly created object.
	*/
	static CDosServer* NewL();

	/**
	* Function that starts the thread.
	* @param aSignal The signal for synchronization.
	*/
	IMPORT_C static TInt ThreadStart( CDosServer::TSignal& aSignal );
    
    /**
	* It adds a session to the server.
	*/
	void AddSession();

	/**
	* It removes a session from the server.
	*/
	void DropSession();

	/**
	* It's called by CActive on error situations.
	* @param aError Symbian error code.
	* @return Returns KErrNone if the error was handled or aError otherwise.
	*/
	TInt RunError(TInt aError);

	/**
	* Gets the Container Index.
	* @return The container index.
	*/
	inline CObjectConIx* ContainerIx() const;

	/**
	* Gets the event manager.
	* @return A pointer to the evetn manager.
	*/
	inline CDosEventManager* EventManager() const;

	/**
	* Gets the Factory.
	* @return A pointer to the factory.
	*/
	inline CDosFactoryBase* DosFactory() const;
	TInt* CurrentFreeDiskSpaceRequest();
	void SetCurrentFreeDiskSpaceRequest(TInt* diskSpace);

public:
    // free disk space notifier
    CSharedDataFileSystemNotifier* iFileSystemNotifier;
private:
	/**
	* Symbian two-phased constructor.
	*/
	void ConstructL();

	/**
	* It creates and sets up a new server side session.
	* @param aVersion Unused.
	* @return A pointer to the created session.
	*/
	CSession2* NewSessionL(const TVersion& aVersion) const;

	/**
	* It creates and sets up a new server side session.
	* @param aVersion Unused.
	* @param aMessage ...
	* @return A pointer to the created session.
	*/
	CSession2* NewSessionL(const TVersion& aVersion, const class RMessage2& aMessage) const;

	/**
	* It reads from z:\system\data\DosServer.ini the Dsy Module and 
	* loads it.
	*/
	void LoadDsyModuleL();

	/**
	* It signals client thread via semaphore that we have started.
	*/
	static void SignalClient();

private:
	//Container Index.
	CObjectConIx* iContainerIx;

	//Number of active sessions in the server.
	TInt iSessionCount;

	//Pointer to the DosServer's event manager.
	CDosEventManager* iEventManager;

	//Pointer to the Service Factory.
	CDosFactoryBase* iDosFactory;

	//Contains the Dsy module.
	RLibrary iLib;

	//Indicates if DSY is Loaded, RLibrary::Close() can not be called if 
	//RLibrary::Load() has not been called.
	TBool iLibLoaded;
	// highest outstanding free disk space request
    TInt* iCurrentFreeDiskSpaceRequest;
    
    CSaeThread* iSaeThread;
    
    friend class CSharedDataFileSystemNotifier;
};

#include "dosserver.inl"

#endif //__DOSSERVER_H__
