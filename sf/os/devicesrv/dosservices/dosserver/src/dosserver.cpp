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
*    Implementation for the CDosServer class
*
*/


#include <e32svr.h>
#include <f32file.h>
#include "dosserver.h"
#include "doseventmanager.h"
#include "dosfactorybase.h"
#include "dossession.h"
#include "dos_debug.h"
#include <data_caging_path_literals.hrh>

/*******************************************************
*  DEFINITION OF THE DSY TO BE USED BY THIS DOS SERVER *
********************************************************/

// This file tells the name of the DSY -module to be loaded.
_LIT(KDosServerIniFile,"Z:\\private\\101f6efa\\DosServer.ini");

// The path where the DSY-module binary must recide. This is hardcoded for
// security reasons.
_LIT(KDSYBinaryLocDrive, "z:");

//
// ---------------------------------------------------------
// CDosServer Constructor
// ---------------------------------------------------------
//  
CDosServer::CDosServer()
    : CPolicyServer( CActive::EPriorityStandard, KDosServerPolicy )
{
	iSessionCount=0;
	iContainerIx = NULL;
	iDosFactory = NULL;
	iLibLoaded = EFalse;
	iCurrentFreeDiskSpaceRequest = NULL;
}


//
// ---------------------------------------------------------
// CDosServer Desctructor
// ---------------------------------------------------------
//  
CDosServer::~CDosServer()
{
	COM_TRACE_( "[DOSSERVER]\t CDosServer::~CDosServer()" );

	// Delete our own stuff
	delete iEventManager;

	//We delete here all the open Sessions
	//This is done also by ~CServer but we do it here because it must be done before
	//deleting iContainerIx since iContainerIx will try to delete the CObjects that are
	//referenced by the Session's containers

	CDosSession* session=NULL;
	iSessionIter.SetToFirst();
	while((session = (CDosSession*)(iSessionIter++)) != NULL)
	{
		delete session;
	}
	
	if ( iFileSystemNotifier )
        {
        COM_TRACE_( "CDosServer::~CDosServer(), deleting iFileSytemNotifier" );
        iFileSystemNotifier->Cancel();
        delete iFileSystemNotifier;
        }
	delete iDosFactory;
	delete iContainerIx;

	COM_TRACE_( "[DOSSERVER]\t CDosServer::~CDosServer() - end" );

	//Check if RLibrary::Load() has been called
	if (iLibLoaded) 
	{
		iLib.Close();
		iLibLoaded = EFalse;
	}

    if ( iSaeThread )
    {
    delete iSaeThread;
    }
}  

//
// ---------------------------------------------------------
// CDosServer::ConstructL
// ---------------------------------------------------------
//  

void CDosServer::ConstructL()
{
	User::RenameThread(KDosServerName);
    // set this as a system thread so if it crashes the phone will restart
	User::SetCritical(User::ESystemCritical);
	COM_TRACE_( "CDosServer::ConstructL(), creating iFileSytemNotifier" );
    iFileSystemNotifier = CSharedDataFileSystemNotifier::NewL(*this);

	iEventManager = CDosEventManager::NewL();
	LoadDsyModuleL();
	iContainerIx = CObjectConIx::NewL();
	
	StartL(KDosServerName);
	
    iSaeThread = CSaeThread::NewL();
    iSaeThread->StartThreadL();
}

//
// ---------------------------------------------------------
// CDosServer::NewL
// ---------------------------------------------------------
//  
CDosServer* CDosServer::NewL()
{
    CDosServer* self=new(ELeave) CDosServer;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

//
// ---------------------------------------------------------
// CDosServer::LoadDsyModuleL
// ---------------------------------------------------------
//  

void CDosServer::LoadDsyModuleL()
{
	COM_TRACE_( "[DOSSERVER]\t CDosServer::LoadDsyModuleL()" );

	//Finding out the name of the Dsy file.
	RFs fs;
	RFile file;

	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	COM_TRACE_( "[DOSSERVER]\t CDosServer::LoadDsyModuleL() - fs connected" );

	COM_TRACE_( "[DOSSERVER]\t Opening file: Z:\\system\\data\\DosServer.ini" );
	COM_TRACE_( "[DOSSERVER]\t error occurs if above file is missing from build!" );

	User::LeaveIfError(file.Open(fs,KDosServerIniFile,EFileRead));
	CleanupClosePushL(file);

	COM_TRACE_( "[DOSSERVER]\t CDosServer::LoadDsyModuleL() - file opened" );

	TBuf8<KMaxFileName> rawFileName;
	
	User::LeaveIfError(file.Read(rawFileName));

	CleanupStack::PopAndDestroy(2); //fs and file are closed

	COM_TRACE_( "[DOSSERVER]\t CDosServer::LoadDsyModuleL() - file readed" );

	HBufC* unicodeFileName = HBufC::NewLC(rawFileName.Length());
	TPtr fileNamePtr(unicodeFileName->Des());
	fileNamePtr.Copy(rawFileName);

	COM_TRACE_1( "[DOSSERVER]\t CDosServer::LoadDsyModuleL() - name: %S", unicodeFileName );

	//Loading of the Dsy module
	TLibraryFunction entryPoint;

	// Check for data caging support;
	// Template parameter defines the maximum length for descriptor. 
	// After copy the length is set to same as the copied descriptor.
	TBuf<128> dsyBinaryLoc(KDC_SHARED_LIB_DIR);
	dsyBinaryLoc.Insert(0, KDSYBinaryLocDrive);

#ifdef _DEBUG // Trace used binary path.
	HBufC* dsyFileName = HBufC::NewLC(dsyBinaryLoc.Length());
	TPtr fNPtr(dsyFileName->Des());
	fNPtr.Copy(dsyBinaryLoc);

	COM_TRACE_1( "[DOSSERVER]\t CDosServer::LoadDsyModuleL() - bin path: %S", dsyFileName );
	CleanupStack::PopAndDestroy(); //dsyFileName
#endif
	User::LeaveIfError(iLib.Load(fileNamePtr, dsyBinaryLoc));

	iLibLoaded = ETrue; //RLibrary::Load() has been called
	entryPoint = iLib.Lookup(1); //Polymorphic DLL only 1 entry point.
	iDosFactory = (CDosFactoryBase *)entryPoint();
	iDosFactory->iEventManager = CONST_CAST(CDosEventManager*,iEventManager);
	CleanupStack::PopAndDestroy(); //unicodeFileName

	COM_TRACE_( "[DOSSERVER]\t CDosServer::LoadDsyModuleL() completed" );
}


//
// ---------------------------------------------------------
// CDosServer::RunError
// ---------------------------------------------------------
//  
TInt CDosServer::RunError(TInt aError)
{
	//
	// Handles an error from CDosSession::ServiceL()
	// Report the error to the client
	//
	COM_TRACE_1( "[DOSSERVER]\t CDosServer::RunError() error: %d", aError );

	Message().Complete(aError);
	ReStart();
	return KErrNone;	// handled the error fully
}

//
// ---------------------------------------------------------
// CDosServer::NewSessionL
// ---------------------------------------------------------
//  

CSession2* CDosServer::NewSessionL(const TVersion&) const
{
	COM_TRACE_( "CDosServer::NewSessionL()" );
	//
	// Create a new client session.
	//
	return new(ELeave) CDosSession();
}

CSession2* CDosServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
{
	return NewSessionL(aVersion);
}
//
// ---------------------------------------------------------
// CDosServer::AddSession
// ---------------------------------------------------------
//  

void CDosServer::AddSession()
{
	//
	// A new session is being created
	//
	++iSessionCount;
	COM_TRACE_1( "[DOSSERVER]\t ** CDosServer - Add session, count: %d", iSessionCount );
}

//
// ---------------------------------------------------------
// CDosServer::DropSession
// ---------------------------------------------------------
//  

void CDosServer::DropSession()
{
	//
	// A session is being destroyed
	//
    iSessionCount--;
	COM_TRACE_1( "[DOSSERVER]\t ** CDosServer - Drop session, count: %d", iSessionCount );
}

//
// ---------------------------------------------------------
// CDosServer::ThreadStart
//
// It Starts the Server's Thread and the Server
// ---------------------------------------------------------
//  

EXPORT_C TInt CDosServer::ThreadStart( CDosServer::TSignal& /*aSignal*/ )
{
    // Get thread that started us
	__UHEAP_MARK;
	TInt err(KErrNone);

    CTrapCleanup* cleanup = NULL;
    CActiveScheduler* scheduler = NULL;
    CDosServer* server = NULL;
	TInt srvErr=KErrNoMemory;

    cleanup = CTrapCleanup::New();
    scheduler = new CActiveScheduler;

    if ( cleanup && scheduler )
    {
        CActiveScheduler::Install( scheduler );
        TRAP(srvErr, server = CDosServer::NewL());      // adds server in scheduler
    }

    if ( !cleanup || !scheduler || srvErr!=KErrNone )
    {
        if ( scheduler ) 
		{
			delete scheduler;
			scheduler = NULL;
		}
		if ( server )
		{
			delete server;
			server = NULL;
		}
		if ( cleanup )
		{
	        delete cleanup;
			cleanup = NULL;
		}
        err = srvErr;
    }

    // signal that we've started
	COM_TRACE_( "CDosServer::ThreadStart - calling SignalClient()" );
	SignalClient();

    if ( err == KErrNone )
    {
        // start fielding requests from clients
        CActiveScheduler::Start();

        // comes here if server gets shut down

        delete scheduler;
		scheduler = NULL;
        delete server;
		server = NULL;
        delete cleanup;
		cleanup = NULL;
    }

    // thread/process exit
	__UHEAP_MARKEND;
    return err;
}

//
// ---------------------------------------------------------
// CDosServer::SignalClient
//
// Signal client we've started.
// ---------------------------------------------------------
//  
void CDosServer::SignalClient()
{
	COM_TRACE_( "[DOSSERVER]\t CDosServer::SignalClient() called" );
	RSemaphore startupSemaphore;
	if ( startupSemaphore.OpenGlobal( KServerStartupSemaphoreName ) == KErrNone )
	{
		//Signal the client:The server might have started up successfully or not
		startupSemaphore.Signal();
		COM_TRACE_( "[DOSSERVER]\t CDosServer::SignalClient() semaphore signalled" );
	}

	startupSemaphore.Close();
	COM_TRACE_( "[DOSSERVER]\t CDosServer::SignalClient() semaphore closed" );

	// No point to return any error; 
	// if signal client fails, startup will freeze anyway.
}

//
// RMessage::Panic() also completes the message. This is:
// (a) important for efficient cleanup within the kernel
// (b) a problem if the message is completed a second time
//
void PanicClient(const RMessage2& aMessage,TDosPanic aPanic)
{
	COM_TRACE_( "[DOSSERVER]\t CDosServer::RMessage::PanicClient() called" );

	aMessage.Panic(KDosServerName,aPanic);
}

//
// ---------------------------------------------------------
// CDosServer::CurrentDiskFreeSpace
//
// Returns current disk space
// ---------------------------------------------------------
//  
TInt* CDosServer::CurrentFreeDiskSpaceRequest()
{
    COM_TRACE_( "CDosServer::CurrentFreeDiskSpaceRequest()" );
    return iCurrentFreeDiskSpaceRequest;    
}

//
// ---------------------------------------------------------
// CDosServer::SetCurrentFreeDiskSpace
//
// Sets new current disk space
// ---------------------------------------------------------
//  
void CDosServer::SetCurrentFreeDiskSpaceRequest(TInt* diskSpace)
{
 COM_TRACE_( "CDosServer::SetCurrentFreeDiskSpaceRequest()" );
    iCurrentFreeDiskSpaceRequest = diskSpace;    
}

