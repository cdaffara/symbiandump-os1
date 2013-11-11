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

#include <e32debug.h>
#include <e32math.h>
#include <f32file.h>
#include <s32file.h>

#include "clesrv.h"
#include "clesess.h"
#include <ssm/ssmcmd.hrh>


// This file is used to store the handles of the libraries which has unload option as ENeverUnload.
// This is also defined in cmdcustomcommand.cpp and the handles are written to this file if 
// the unload option is ENeverUnload.
_LIT(KNeverUnloadLibHandleFile, ":\\private\\2000d75b\\temp\\unloadlibhandles.bin");

// ------------------- Policy Server Security Setup ----------------------

const TInt KSsmServerSid = 0x2000D75B; //UID3 of sysstatemgr.exe

const TUint  KRangeCount = 3;
const TInt   KRanges[KRangeCount] =
	{
	0,
	ECleSrvExecuteCmdList,
	EEndOfCleOpCodes
	};

/**
 Maps to index 0 in the array KPolicyElements
 */
const TInt KSsmServerSidCheck = 0;

/**
 Specifies the appropriate action for each range in KRanges.
 The nth element of KElementsIndex specifies the appropriate action for the nth range in KRanges.
 */
const TUint8 KElementsIndex[KRangeCount] =
	{
	CPolicyServer::ENotSupported,
	KSsmServerSidCheck,
	CPolicyServer::ENotSupported,
	};

/**
 Array containing the different security checks performed by this server
 */
const CPolicyServer::TPolicyElement KPolicyElements[] =
	{
	{_INIT_SECURITY_POLICY_S0(KSsmServerSid), CPolicyServer::EFailClient} //lint !e778 suppress Constant expression evaluates to 0 in operation '+'
	};

/**
 Setup a security policy which requires all caps to be used by the SsmServer for all requests
 including creating a connection. We do not yet have a usecase for anyone else other than
 SsmServer using SusUtilServer. The caller's SID is matched against SsmServer's
 SID in each ServiceL call
 */
const CPolicyServer::TPolicy KCleServerPolicy =
	{
	KSsmServerSidCheck,	// map connection attempts as well to index [0] in KPolicyElements[]
	KRangeCount,
	KRanges,
	KElementsIndex,
	KPolicyElements
	};



void CCleServer::ExecuteCommandListL( CSsmCommandList* aCommandList, const RMessage2& aMessage, const TInt aSessionIndex )
	{
	__ASSERT_DEBUG( aCommandList->Count() > 0, User::Panic( KCleCmdListEmpty, KCleSrvBadCmdList) );
	__ASSERT_DEBUG( __IN_RANGE(aSessionIndex, iSessionInfoArray.Count()), User::Panic( KCleSrvBadIdx, KCleSrvArrayIndexInvalid ) );
			
	if( (0 == aCommandList->Count()) || !__IN_RANGE(aSessionIndex, iSessionInfoArray.Count()) )
		{
		User::Leave( KErrArgument );
		}
	
	CSsmCmdListExecutor* executor = CSsmCmdListExecutor::NewL( *this, aSessionIndex );
	
	iSessionInfoArray[ aSessionIndex ].iExecutor = executor;
	iSessionInfoArray[ aSessionIndex ].iMessagePtr = aMessage;

	executor->StartL( aCommandList );
	}



/**
 * Locate the CommandListExecutor associated with the session calling CommandListCancel.
 * Delete the Command list (which cancels its AO) then complete the client's RMessage with KErrCancel.
 */
void CCleServer::ExecuteCommandListCancel( TInt aSessionIndex )
	{
	__ASSERT_DEBUG( __IN_RANGE( aSessionIndex, iSessionInfoArray.Count()), User::Panic( KCleSrvBadIdx, KCleSrvArrayIndexInvalid ) );
	
	if( iSessionInfoArray[ aSessionIndex ].iExecutor )
		{
		iSessionInfoArray[ aSessionIndex ].iMessagePtr.Complete( KErrCancel );
		delete iSessionInfoArray[ aSessionIndex ].iExecutor;
		iSessionInfoArray[ aSessionIndex ].iExecutor = NULL;
		}
	}

	

/**
 * This function is inherited from MCommandListExecutionObserver
 * and called from CSsmCmdListExecutor to signal the completion of command list processing.
 */
void CCleServer::ListExecutionComplete( TInt aComplete, TInt aSessionIndex ,TCmdErrorSeverity /* aSeverity */ )
	{
	__ASSERT_DEBUG( __IN_RANGE(aSessionIndex, iSessionInfoArray.Count()), User::Panic( KCleSrvBadIdx, KCleSrvArrayIndexInvalid ) );
	iSessionInfoArray[ aSessionIndex ].iMessagePtr.Complete( aComplete );
	delete iSessionInfoArray[ aSessionIndex ].iExecutor;
	iSessionInfoArray[ aSessionIndex ].iExecutor = NULL;
	}



/**
  Used to create a new server-side session.
 @return A pointer to the new instance of CSession2.
 @leave KErrNotSupported if versions are incompatible.
 @publishedPartner
 @released
*/
EXPORT_C CSession2* CCleServer::NewSessionL( const TVersion& aVersion, const RMessage2& /*aMessage*/ ) const
	{
	if( !User::QueryVersionSupported(iVersion, aVersion) )
		{
		User::Leave( KErrNotSupported );
		}
	
	CCleServer& mutatedSelf = const_cast< CCleServer& >( *this );
	
	return CCleSession::NewL( mutatedSelf ); 
	}
	


/**
 Iterate through iSessionInfoArray to find an unused array element
 If found, use it. Otherwise, Append() a new SCleSessionInfo.
 This function is called during CSession construction.
*/
void CCleServer::RegisterSessionL( TInt& aSessionIndex )
	{
	const TInt count = iSessionInfoArray.Count();
	TBool slotFound = EFalse;
	
	for(TInt i = 0; i < count; i++ )
		{
		if( !iSessionInfoArray[ i ].iInUse )
			{
			iSessionInfoArray[ i ].iInUse = ETrue;
			ASSERT( NULL == iSessionInfoArray[ i ].iExecutor );
			aSessionIndex = i;
			slotFound = ETrue;
			iSessionCount++;			
			break;
			}
		}
	
	if( !slotFound )
		{
		SCleSessionInfo sessionInfo;
		sessionInfo.iExecutor = NULL;
		sessionInfo.iInUse = ETrue;
		iSessionInfoArray.AppendL( sessionInfo );
		aSessionIndex = iSessionCount++;
		}
	
	DEBUGPRINT2( _L("***** Registered in slot %d"), aSessionIndex );
	}



/**
 Called from CSession destructor.
*/
void CCleServer::DeregisterSession( const TInt& aSessionIndex )
	{
	__ASSERT_DEBUG( __IN_RANGE(aSessionIndex, iSessionInfoArray.Count()), User::Panic(KCleSrvBadIdx, KCleSrvArrayIndexInvalid) );
	
	if( iSessionInfoArray[ aSessionIndex ].iExecutor )
		{
		delete iSessionInfoArray[ aSessionIndex ].iExecutor;
		iSessionInfoArray[ aSessionIndex ].iExecutor = NULL;
		}

	iSessionInfoArray[ aSessionIndex ].iInUse = EFalse;
	iSessionCount--;

   // Remove unused elements at the end of the array, so iSessionInfoArray can be (granular) shrunk
	const TInt count = iSessionInfoArray.Count();
	TBool slotRemoved = EFalse;
	for(TInt i = count-1; i >= 0 ; i-- )
		{
		if( !iSessionInfoArray[ i ].iInUse )
			{
      		iSessionInfoArray.Remove(i);
      		slotRemoved = ETrue;
			}
		else
			{
			break;
			}
		}
	if (slotRemoved)
		{
		iSessionInfoArray.GranularCompress();
		}

	DEBUGPRINT2( _L("***** De-registered slot %d"), aSessionIndex );
 	}

// Release the libraries loaded by custom commands which has the unload option set to 'ENeverUnload'
TInt CCleServer::CloseNeverUnloadLibrariesL()
	{
	const TChar sysDrive = RFs::GetSystemDriveChar();
	RBuf filename;
	filename.CreateL(KNeverUnloadLibHandleFile().Length() + 1);
	filename.Append(sysDrive);
	filename.Append(KNeverUnloadLibHandleFile());
	filename.CleanupClosePushL();
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	RFile file;
	CleanupClosePushL(file);
	TInt libsReleased = 0;
	const TInt error=file.Open(fs, filename, EFileShareReadersOnly|EFileStream|EFileRead);
	if (error==KErrNone)
		{
		RFileReadStream readStream;
		readStream.Attach(file);
		CleanupClosePushL(readStream);
		TInt handle=0;
		RLibrary lib;
		TRAPD(ret, handle = readStream.ReadInt32L());
		while(ret != KErrEof && handle > 0)
			{
			lib.SetHandle(handle);
			lib.Close();
			libsReleased++;
			TRAP(ret, handle = readStream.ReadInt32L());
			}
		CleanupStack::PopAndDestroy(&readStream);
		}
	fs.Delete(filename);//Ignore the error. Perhaps the file does not exist.
	CleanupStack::PopAndDestroy(3);
	return libsReleased;
	}


/**
 * Used to create a new CCleServer
 * @return A pointer to the CCleServer
 * @leave One of the system-wide error codes if construction fails.
 * @publishedPartner
 * @released
 */
EXPORT_C CCleServer* CCleServer::NewL(const TDesC& aServerName)
	{
	CCleServer* self = NewLC(aServerName);
	CleanupStack::Pop();
	return self;	
	}



/**
 * Used to create a new CCleServer
 * @return A pointer to the CCleServer
 * @leave One of the system-wide error codes if construction fails.
 * @publishedPartner
 * @released
 */
EXPORT_C CCleServer* CCleServer::NewLC(const TDesC& aServerName)
	{
	CCleServer* self = new(ELeave) CCleServer;	
	CleanupStack::PushL( self );
	self->ConstructL(aServerName);
	return self;	
	}
	


/**
 * Static function used to create and start CCleServer
 * 
 * @return KErrAlreadyExists if the server is already running
 * @publishedPartner
 * @released
 */
EXPORT_C TInt CCleServer::StartCleSrv(const TDesC& aServerName)
	{
	TAny* threadParam = reinterpret_cast<TAny*>(const_cast<TDesC*>(&aServerName));
	// Create a Unique name for the thread name
	TName name(aServerName);
	_LIT(KUnderScore, "_");
	name.Append(KUnderScore);
	name.AppendNum(Math::Random(), EHex);

	const TInt KCleSrvMinHeapSize = 0x2000;
	const TInt KCleSrvMaxHeapSize = 10 * KCleSrvMinHeapSize;
	RThread srvThread;
	TInt err = srvThread.Create( name, &CCleServer::CleSrvThreadFn, 
								 KDefaultStackSize, KCleSrvMinHeapSize, KCleSrvMaxHeapSize, 
								 threadParam, EOwnerProcess );

	if( KErrNone == err )
		{
		TRequestStatus trs;
		
		srvThread.Rendezvous( trs );
		srvThread.Resume();
		srvThread.Close();
		
		User::WaitForRequest( trs );	
		err = trs.Int();
		}

	return err;
	}



TInt CCleServer::CleSrvThreadFn( TAny* aAny )
	{
	const TDesC* serverName = const_cast<const TDesC*>(static_cast<TDesC*>(aAny));
	TInt err = KErrNoMemory;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if( cleanup )
    	{
	    TRAP( err,
	        {
	        CActiveScheduler* sched = new(ELeave) CActiveScheduler();
	        CleanupStack::PushL( sched );
	        CActiveScheduler::Install( sched );
	
	        CCleServer* cleSrv = CCleServer::NewLC(*serverName);
	        RThread::Rendezvous( KErrNone );
	    	// Must not use serverName after rendezvousing with client
	        serverName = NULL;
	        
	        CActiveScheduler::Start();
	
	        CleanupStack::PopAndDestroy( cleSrv );
	        CleanupStack::PopAndDestroy( sched );
	        });

	    delete cleanup;
    	}
 
    return err;
	}


/**

 */
CCleServer::CCleServer()
: CPolicyServer( EPriorityHigh, KCleServerPolicy, EUnsharableSessions ),
  iVersion( KCleSrvVersionMajor, KCleSrvVersionMinor, KCleSrvVersionBuild )
	{
	}
	
	
	
void CCleServer::ConstructL(const TDesC& aServerName)
	{
	//If the ENeverUnload file is left over, then it should be cleaned up.
	//Ignore the error. We don't want the server to be stopped due to this error.
	TRAP_IGNORE(CleanHandlesFileL());
	
	StartL( aServerName );
	// The starting thread is signalled in the thread function.
	}
	

// Delete the file which stores the handles of those libraries used by custom commands which has unload option set to ENeverUnload.
// Generally it will be deleted from the destructor. If it could not be deleted for some reason from the destructor, 
// then it should be deleted while starting the server.
void CCleServer::CleanHandlesFileL()
	{
	const TChar sysDrive = RFs::GetSystemDriveChar();
	RBuf filename;
	filename.CreateL(KNeverUnloadLibHandleFile().Length() + 1);
	filename.Append(sysDrive);
	filename.Append(KNeverUnloadLibHandleFile());
	filename.CleanupClosePushL();
	RFs fs;
	User::LeaveIfError(fs.Connect());
	fs.Delete(filename);//Ignore the error. Perhaps the file does not exist.
	fs.Close();
	CleanupStack::PopAndDestroy();
	}

CCleServer::~CCleServer()
	{
	const TInt count = iSessionInfoArray.Count();	
	for(TInt i = 0; i < count; i++ )
		{
		delete iSessionInfoArray[ i ].iExecutor;
		iSessionInfoArray[ i ].iExecutor = NULL;
		}
	iSessionInfoArray.Close();

	//Server is being destructed, so no need to leave from here.
	TRAP_IGNORE(CloseNeverUnloadLibrariesL());
	}

