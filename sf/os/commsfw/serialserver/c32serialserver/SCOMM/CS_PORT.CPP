// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "CS_STD.H"
#include <f32file.h>
#include "C32LOG.H"
/** @file
 *
 * Implements CPort, CSerial and CLibUnloader
 */

class CLibUnloader : public CAsyncOneShot
/** Unloading a C32 library.

This class is responsible for unloading a library loaded by C32. The library
is specified during construction. After the library is unloaded, this object
will destroy itself.

Note: The same class is also implemented in Esock, Etel and Nifman. 

@internalComponent
@released
 */
	{
friend class CSerial;
public:
	static CLibUnloader* NewL(RLibrary &aLib);
protected:
	CLibUnloader();
	virtual void RunL(); // from CActive
private:
	RLibrary iLib;       //< handle to library to unload
	};


//
// implementation of CLibUnloader
//

CLibUnloader* CLibUnloader::NewL(RLibrary& aLib)
/** Creates a new CLibUnloader object

@param aLib reference to the library to unload. */
	{
	CLibUnloader *s = new(ELeave) CLibUnloader;
	s->iLib = aLib;
	aLib.SetHandle(0); // Transfer Complete
	return s;
	}


CLibUnloader::CLibUnloader()
/** Constructor */
	:CAsyncOneShot(CActive::EPriorityHigh)
	{
	C32LOG1(KC32Shutdown, _L8("CLibUnloader::CLibUnloader"));
	}


void CLibUnloader::RunL()
/** Unloads, closes and deletes a library.

This function is called by the Active Scheduler when it is time to unload the
Library. It closes the library and deletes itself. */
	{
	iLib.Close();
	delete this;
	}



//
// implementation of CPort
//

EXPORT_C CPort::CPort()
/** Default constructor. Derived classes should implement a NewL() function to 
perform their two-phase construction. 

@see TSerialNewL */
	{
	C32LOG1(KC32Player, _L8("CPort::CPort()"));
	}


EXPORT_C CPort::~CPort()
/** Destructor.

Closes the owner (which is our CSerial) and removes any timers.

Derived classes can implement the destructor but must only call synchronous 
functions within it. Any cleanup requiring asynchronous operations must be 
done in Destruct(). */
	{
	C32LOG1(KC32Player, _L8("CPort::~CPort()"));
	if (iReadTimerPending)
		CommTimer::Remove(iReadTimer);
	if (iWriteTimerPending)
		CommTimer::Remove(iWriteTimer);
	if (Owner())
		Owner()->Close();
	delete iExtra;
	}


EXPORT_C void CPort::CPort_Reserved1()
/** Reserved virtual function. */
	{
	}


#define MergeModes( a, b ) (((a)<<3) + (b))


void CPort::DoOpenL(CCommSession* aSession, TInternalCommAccess aMode, TCommRole aRole, TBool aIsNew)
/** Set the access mode and signals for this port if the open request does not conflict with existing
 usage of this port by other clients.

@param aSession pointer to the session
@param aMode    access mode
@param aRole    port role; DTE or DCE
@param aIsNew   ETrue if new session
@leave Leave This function may leave. 
(Private fn, so this` doco not built into DevLibrary) */
	{
	C32LOG4(KC32Player,_L8("CPort::DoOpenL(), Session 0x%x, Comm Access Mode : %S, Comm Role : %S"), aSession, &TC32Log::InternalCommAccessStr(aMode), &TC32Log::CommRoleStr(aRole));
	if (aSession == iExtra->iPreemptedSession)
		iExtra->iPreemptedSession = NULL;  // Don't need to inform session session has been preempted any more.

	switch (aMode) 
	{
	case EIntCommExclusive:
	case EIntCommShared:
	case EIntCommWaitUntilAvailable:
	case EIntCommPreemptable:
		break;
	default:
		User::Leave(KErrNotSupported);
	}

	if (aIsNew || AccessCount()==0)
		{
		// ask the CSY to set any signals for DCE/DTE role on this port
		(void)User::LeaveIfError(SetRole(aRole)); // ignoring return value
		iMode = aMode;
		if (aMode == EIntCommPreemptable)
			iExtra->iPreemptableOwner=aSession;
		else if (aMode == EIntCommWaitUntilAvailable)
			iExtra->iWaitAvailableOwner=aSession;
		}


	else switch (MergeModes(iMode, aMode))
		{
	default: 
		User::Leave(KErrAccessDenied);

	case MergeModes(EIntCommPreemptable, EIntCommWaitUntilAvailable):
		User::Leave(KErrNotSupported);

	case MergeModes(EIntCommShared, EIntCommWaitUntilAvailable):
	case MergeModes(EIntCommExclusive, EIntCommWaitUntilAvailable):
		if (iExtra->iWaitAvailableOwner!=NULL)
			User::Leave(KErrAccessDenied);
		iExtra->iWaitAvailableOwner=aSession;
		break;

	case MergeModes(EIntCommWaitUntilAvailable, EIntCommShared):
	case MergeModes(EIntCommWaitUntilAvailable, EIntCommExclusive):
	case MergeModes(EIntCommShared, EIntCommShared):
		TCommRole tempRole;
		(void)GetRole(tempRole); // return value is no use, if it fails temprole is not filled, then the next comparison fails
		if (tempRole!=aRole)
			User::Leave(KErrLocked);
		iMode = aMode;
		break;
	
	case MergeModes(EIntCommPreemptable, EIntCommExclusive):
	case MergeModes(EIntCommPreemptable, EIntCommShared):
		// Time to preempt!
		DoPreemption();
		// ask the CSY to set any signals for DCE/DTE role on this port
		(void)User::LeaveIfError(SetRole(aRole)); // ignoring return value
		iMode = aMode;
		break;
		}
	}


void CPort::DoPreemption()
/**
 cancel all outstanding requests and setup the preemted session
 */
	{
	C32LOG1(KC32Player, _L8("CPort::DoPreemption()"));
	if (iReadOwner) CommReadCancel( NULL, iReadOwner );
	if (iWriteOwner) CommWriteCancel( NULL, iWriteOwner );
	if (iBreakOwner) CommBreakCancel( NULL, iBreakOwner );
	if (iSignalOwner) CommNotifySignalChangeCancel(NULL, iSignalOwner);
	if (iConfigOwner) CommNotifyConfigChangeCancel(NULL, iConfigOwner);
	if (iFlowControlOwner) CommNotifyFlowControlChangeCancel(NULL, iFlowControlOwner);
	if (iBreakNotifyOwner) CommNotifyBreakCancel(NULL, iBreakNotifyOwner);
	if (iNotifyOutputEmptyOwner) CommNotifyOutputEmptyCancel(NULL, iNotifyOutputEmptyOwner);
	if (iNotifyDataAvailableOwner) CommNotifyDataAvailableCancel(NULL, iNotifyDataAvailableOwner);
	
	iExtra->iPreemptedSession = iExtra->iPreemptableOwner;
	iExtra->iPreemptableOwner = NULL;
	}


TBool CPort::SessionHasBeenPreempted(CCommSession* aSession)
/** returns true if session has been pre-empted

@param aSession session to be questioned
@return TBool pre-empted status of this session. ETrue if session has been
        pre-empted, EFalse otherwise.
*/
	{
	C32LOG2(KC32Player, _L8("CPort::SessionHasBeenPreempted(), Session 0x%x"), aSession);
	return iExtra->iPreemptedSession==aSession;
	}


TBool CPort::SessionIsAwaitingOpen(CCommSession* aSession)
/** returns true if session is waiting for open

@param aSession session to be questioned
@return TBool ETrue if session is awaiting open, EFalse otherwise. */
	{
	C32LOG2(KC32Player, _L8("CPort::SessionIsAwaitingOpen(), Session 0x%x"), aSession);
	return (iExtra->iWaitAvailableOwner==aSession);
	}


void CPort::FreeSession(CCommSession* aSession)
/** Perform CPort based housekeeping before closing a session

@param aSession pointer to the session to free. */
	{
	C32LOG2(KC32Player, _L8("CPort::FreeSession(), Session 0x%x"), aSession);
	// If this session is the waiting owner, NULL the pointer before closure
	if(aSession==iExtra->iWaitAvailableOwner)
		{
		iExtra->iWaitAvailableOwner=NULL;

		// If this session is the waiting owner *and* it has a Blocked Set Access
		// outstanding (usually the case) complete it with KErrCancel
		if(iExtra->iBlockedSetAccess!=RMessagePtr2())
			{
			SafeComplete(iExtra->iBlockedSetAccess, KErrCancel);
			iExtra->iBlockedSetAccess=RMessagePtr2();
			}
		}
	}

EXPORT_C void CPort::Close()
//Replace close so that we can go asynchronous
/** Closes the port. 

The base class implements CObject::Close() to handle reference 
counting on the port. It decrements the reference count, and calls Destruct() 
if the count is 0. */
	{
	C32LOG1(KC32Player, _L8("CPort::Close()"));
	Dec();

	__ASSERT_ALWAYS(AccessCount()>=0, Fault(ECPortEObjNegativeAccessCount));

	if(iExtra)
		{
		if (AccessCount()==1 && iExtra->iWaitAvailableOwner != NULL)  
			{
			if (iExtra->iBlockedSetAccess != RMessagePtr2()) // There's an outstanding SetAccess request
				{
				SafeComplete(iExtra->iBlockedSetAccess, KErrNone);
				iMode = EIntCommPreemptable;  // Only allowed async SetAccess is to Preemptable
				iExtra->iPreemptableOwner = iExtra->iWaitAvailableOwner;
				iExtra->iWaitAvailableOwner = NULL;
				}
			}
		}

	C32LOG2(KC32Player, _L8("CPort::Close() after Dec(), CPort AccessCount is %d "), AccessCount());
	if (AccessCount()==0)
		Destruct();
	}


void CPort::CommSetAccess(const RMessage2& aMessage, CCommSession& aSession)
/** set the access mode

@param aMessage message from client with the new access mode
@param aSession handle to the session. */
	{
	TInternalCommAccess newMode = (TInternalCommAccess)aMessage.Int0();

	switch (MergeModes(iMode, newMode))
		{
	default: 
		SafeComplete(aMessage, KErrNotSupported);
		break;

	case MergeModes(EIntCommExclusive, EIntCommPreemptable): 
	case MergeModes(EIntCommShared, EIntCommPreemptable):
		if (AccessCount() == 1)  // No - must do close();
			PanicClient(EMustCloseNotChangeAccessMode,aMessage);
		else if (&aSession != iExtra->iWaitAvailableOwner)  // Wrong session.
			PanicClient(EWrongClientForAccessRequest,aMessage);
		else
			iExtra->iBlockedSetAccess=aMessage;
		break;

	case MergeModes(EIntCommWaitUntilAvailable, EIntCommPreemptable):
		ASSERT(AccessCount() == 1);
		iMode = EIntCommPreemptable;
		iExtra->iWaitAvailableOwner=NULL;
		iExtra->iPreemptableOwner=&aSession;
		SafeComplete(aMessage, KErrNone);
		break;
	
	case MergeModes(EIntCommPreemptable, EIntCommExclusive):
	case MergeModes(EIntCommPreemptable, EIntCommShared):
		iMode = newMode;
		SafeComplete(aMessage, KErrNone);
		break;
		}
	}


/**
 *  Checks if a CommSetAccess() request is waiting for the port.
 *
 *  @param aClient  The CCommSession client to check against.
 *  @param aHandle  Handle to the subsession.
 *
 *  @return  True if a request is pending, false otherwise.
 */
TBool CPort::IsBlockedSetAccessWaiting(CCommSession& aClient)
	{
	return (&aClient == iExtra->iWaitAvailableOwner  &&
			iExtra->iBlockedSetAccess!=RMessagePtr2());
	} // CPort::IsBlockedSetAccessWaiting


void CPort::CommSetAccessCancel(TInt /*aHandle*/, CCommSession* aClient)
/** Cancels an outstanding set access request.

@param aClient pointer to the CCommSession client. */
	{
	C32LOG2(KC32Player, _L8("CPort::CommSetAccessCancel(), Client Session : 0x%x"), aClient);
	if (aClient == iExtra->iWaitAvailableOwner )
		SafeComplete(iExtra->iBlockedSetAccess, KErrCancel);
	}


#ifdef _DEBUG
inline TInt Count( CCommSession* a ) { return a ? 1 : 0; }

void CPort::CommDebugState(const RMessage2& aMessage, CCommSession& /*aSession*/)
/** return debug information about the state of the C32 server and CSY

@param aMessage message from client
@param aSession handle to the session. */
	{
	TCommDebugInfo s;
	s.iMode = iMode;
	s.iAccessCount = AccessCount();
	(void)GetRole( s.iRole );
	s.iOutstandingCommands = Count( iReadOwner ) + Count( iWriteOwner ) +
		Count( iBreakOwner ) + Count( iSignalOwner ) + Count( iFlowControlOwner ) + 
		Count( iConfigOwner ) + Count( iBreakNotifyOwner ) + Count( iNotifyDataAvailableOwner ) +
		Count( iNotifyOutputEmptyOwner );

	TPckgC<TCommDebugInfo> pk( s );
	TInt ret = aMessage.Write(0, pk, 0);
	if (ret!=KErrNone)
		{
		PanicClient(EBadDescriptor,aMessage);
		}
	SafeComplete(aMessage, ret);
	}
#endif // _DEBUG


void CPort::CommRead(const RMessage2& aMessage, CCommSession* aClient)
/** Perform a read making sure of the port's owner

@param aMessage message from the client
@param aClient handle to the client session. */
	{ 
	C32LOG2(KC32Player, _L8("CPort::CommRead(), Client 0x%x"), aClient);
	if (TakeOwnershipForReading(aMessage,aClient))
		{
		TInt timeOut = aMessage.Int2();
		if (timeOut > 0)
			{
			iReadTimerPending = ETrue;
			TCallBack c(CPort::ReadTimerExpiredHandler,this);
			iReadTimer.Set(c);
			CommTimer::Queue(timeOut, iReadTimer);
			}
		iBlockedRead = aMessage;
		StartRead(aMessage.Ptr0(), aMessage.Int1());
		}
	else if(aClient != iReadOwner)
		SafeComplete(aMessage, KErrInUse);
	}


void CPort::CommWrite(const RMessage2& aMessage, CCommSession* aClient)
/** Perform a write making sure of the port's owner

@param aMessage message from the client
@param aClient handle to the client session. */
	{ 
	C32LOG2(KC32Player, _L8("CPort::CommWrite(), Client 0x%x"), aClient);
	if(!aMessage.Ptr0())
		{
		PanicClient(EBadDescriptor,aMessage);
		return;
		}

	if (TakeOwnershipForWriting(aMessage,aClient))
		{
		TInt timeOut = aMessage.Int2();
		if (timeOut > 0)
			{
			iWriteTimerPending = ETrue;
			TCallBack c(CPort::WriteTimerExpiredHandler, this);
			iWriteTimer.Set(c);
			CommTimer::Queue(timeOut, iWriteTimer);
			}

		iBlockedWrite = aMessage;
		StartWrite(aMessage.Ptr0(), aMessage.Int1());
		}
	else if(aClient != iWriteOwner)
		SafeComplete(aMessage, KErrInUse);
	}


void CPort::CommBreak(const RMessage2& aMessage, CCommSession* aClient)
/** Perform a break making sure of the port's owner

@param aMessage message from the client
@param aClient handle to the client session. */
	{ 
	C32LOG2(KC32Player, _L8("CPort::CommBreak(), Client Session 0x%x"), aClient);
	if (TakeOwnershipForBreaking(aMessage,aClient))
		{
		iBlockedBreak=aMessage;
		Break(aMessage.Int0());
		}
	else if(aClient != iBreakOwner)
		SafeComplete(aMessage, KErrInUse);
	}


void CPort::CommConfig(const RMessage2& aMessage, CCommSession& aSession) const
/** Get config

@param aMessage message from the client
@param aSession handle to the client session. */
	{
	(void)aSession;		// to disable urel warnings
	C32LOG2(KC32Player, _L8("CPort::CommConfig(), Client 0x%x"), &aSession);
	TInt length=aMessage.GetDesLength(0);
	
	if (length < 0)
		{
		PanicClient(EBadDescriptor,aMessage);
		return;
		}

	TInt ret=KErrNoMemory;
	TText8* buf=(TText8*)User::Alloc(length);
	if (buf)
		{
		TPtr8 p(buf,length,length);
		ret=GetConfig(p);
		if (ret==KErrNone)
			{
			ret = aMessage.Write(0, p, 0);
			if (ret!=KErrNone)
				{
				PanicClient(EBadDescriptor,aMessage);
				}
			}
		User::Free(buf);
		}
	SafeComplete(aMessage, ret);
	}


void CPort::CommSetConfig(const RMessage2& aMessage, CCommSession& aSession)
/** Set config

@param aMessage message from the client
@param aSession handle to the client session. */
	{ 
	(void)aSession;		// to disable urel warnings
	C32LOG2(KC32Player, _L8("CPort::CommSetConfig(), Client 0x%x"), &aSession);
	TInt length=aMessage.GetDesLength(0);

	if (length < 0)
		{
		PanicClient(EBadDescriptor,aMessage);
		return;
		}

	TInt ret=KErrNoMemory;
	TText8* buf=(TText8*)User::Alloc(length);
	if (buf)
		{
		TPtr8 p(buf,length,length);
		ret = aMessage.Read(0, p, 0);
		if (ret!=KErrNone)
			{
			C32LOG1(KC32Player, _L8("Error at the time of reading data from client"));
			PanicClient(EBadDescriptor,aMessage);
			}
		if (ret == KErrNone)
			{
			if(AreAnyPending())
				{
				ret=KErrInUse;
				}
			else
				ret=SetConfig(p);
			}
		User::Free(buf);
		}
	SafeComplete(aMessage, ret);
	}


void CPort::CommSetServerConfig(const RMessage2& aMessage, CCommSession& aSession)
/** Set server config

@param aMessage message from the client
@param aSession handle to the client session. */
	{ 
	(void)aSession;		// to disable urel warnings
	C32LOG2(KC32Player, _L8("CPort::CommSetServerConfig(), Client 0x%x"), &aSession);
	TInt length=aMessage.GetDesLength(0);

	if (length < 0)
		{
		PanicClient(EBadDescriptor,aMessage);		
		return;
		}

	TInt ret=KErrNoMemory;
	TText8* buf=(TText8*)User::Alloc(length);
	if (buf)
		{
		TPtr8 p(buf,length,length);
		ret = aMessage.Read(0, p, 0);
		if (ret!=KErrNone)
			{
			C32LOG1(KC32Player, _L8("Error at the time of reading data from client"));
			PanicClient(EBadDescriptor,aMessage);
			}
		if (ret == KErrNone)
			ret=SetServerConfig(p);
		User::Free(buf);
		}
	SafeComplete(aMessage, ret);
	}


void CPort::CommGetServerConfig(const RMessage2& aMessage, CCommSession& aSession)
/** Get server config

@param aMessage message from the client
@param aSession handle to the client session. */
	{
	(void)aSession;		// to disable urel warnings
	C32LOG2(KC32Player, _L8("CPort::CommGetServerConfig(), Client 0x%x"), &aSession);
	TInt length=aMessage.GetDesLength(0);

	if (length < 0)
		{
		PanicClient(EBadDescriptor,aMessage);		
		return;
		}

	TInt ret=KErrNoMemory;
	TText8* buf=(TText8*)User::Alloc(length);
	if (buf)
		{
		TPtr8 p(buf,length,length);
		ret=GetServerConfig(p);
		if (ret==KErrNone)
			{
			ret = aMessage.Write(0, p, 0);
			if (ret!=KErrNone)
				{
				PanicClient(EBadDescriptor,aMessage);
				}
			}
		User::Free(buf);
		}
	SafeComplete(aMessage, ret);
	}


void CPort::CommCaps(const RMessage2& aMessage, CCommSession& aSession)
/** Read capabilities

@param aMessage message from the client
@param aSession handle to the client session. */
	{ 
	(void)aSession;		// to disable urel warnings
	C32LOG2(KC32Player, _L8("CPort::CommCaps(), Client 0x%x"), &aSession);
	TInt length=aMessage.GetDesLength(0);

	if (length < 0)
		{
		PanicClient(EBadDescriptor,aMessage);		
		return;
		}

	TInt ret=KErrNoMemory;
	TText8* buf=(TText8*)User::Alloc(length);
	if (buf)
		{
		TPtr8 p(buf,length,length);
		ret=GetCaps(p);
		if (ret==KErrNone)
			{
			ret = aMessage.Write(0, p, 0);
			if (ret!=KErrNone)
				{
				PanicClient(EBadDescriptor,aMessage);
				}
			}
		User::Free(buf);
		}
	SafeComplete(aMessage, ret);
	}


void CPort::CommSignals(const RMessage2& aMessage, CCommSession& aSession)
/** Read signals

@param aMessage message from the client
@param aSession handle to the client session. */
	{
	(void)aSession;		// to disable urel warnings
	C32LOG2(KC32Player, _L8("CPort::CommSignals(), Client 0x%x"), &aSession);
	TUint s;
	TPckg<TUint> signals(s);
	TInt ret=GetSignals(s);
	if (ret==KErrNone)
		{
		ret = aMessage.Write(0, signals, 0);
		if (ret!=KErrNone)
			{
			PanicClient(EBadDescriptor,aMessage);
			}
		}
	SafeComplete(aMessage, ret);
	}


void CPort::CommSetSignalsToMark(const RMessage2& aMessage, CCommSession& /*aSession*/)
/** Set signal lines 

@param aMessage message from the client. */
	{
	C32LOG1(KC32Player, _L8("CPort::CommSetSignalsToMark()"));
	SafeComplete(aMessage, SetSignalsToMark(aMessage.Int0()));
	}


void CPort::CommSetSignalsToSpace(const RMessage2& aMessage, CCommSession& /*aSession*/)
/** Clear signal lines

@param aMessage message from the client. */
	{ 
	C32LOG1(KC32Player, _L8("CPort::CommSetSignalsToSpace()"));
	SafeComplete(aMessage, SetSignalsToSpace(aMessage.Int0()));
	}


void CPort::CommReceiveBufferLength(const RMessage2& aMessage, CCommSession& aSession) const
/** read the receive buffer length

@param aMessage message from the client
@param aSession handle to the client session. */
	{
	(void)aSession;		// to disable urel warnings
	C32LOG2(KC32Player, _L8("CPort::CommReceiveBufferLength(), Client 0x%x"), &aSession);
	TInt l;
	TPckg<TInt> len(l);
	TInt ret=GetReceiveBufferLength(l);
	if (ret==KErrNone)
		{
		ret = aMessage.Write(0, len, 0);
		if (ret!=KErrNone)
			{
			PanicClient(EBadDescriptor,aMessage);
			}
		}
	SafeComplete(aMessage, ret);
	}


void CPort::CommSetReceiveBufferLength(const RMessage2& aMessage, CCommSession& aSession)
/** Set the receive buffer length

@param aMessage message from the client
@param aSession handle to the client session. */
	{ 
	(void)aSession;		// to disable urel warnings
	C32LOG2(KC32Player, _L8("CPort::CommSetReceiveBufferLength(), Client 0x%x"), &aSession);
	if(AreAnyPending()) // Stop RDevComm panicking us and panic client instead
		{
		PanicClient(ESetReceiveBufferLengthWhilePendingRequests,aMessage);
		}
	else
		{
		//
		// Check for negative or zero length buffer size...
		//
		if(aMessage.Int0() <= 0)
			{
			PanicClient(EBadDescriptor,aMessage);			
			}
		else
			{
			SafeComplete(aMessage, SetReceiveBufferLength(aMessage.Int0()));
			}
		}
	}


void CPort::CommQueryReceiveBuffer(const RMessage2& aMessage, CCommSession& aSession) const
/** Set the receive buffer length

@param aMessage message from the client
@param aSession handle to the client session. */
	{
	(void)aSession;		// to disable urel warnings
	C32LOG2(KC32Player, _L8("CPort::CommQueryReceiveBuffer(), Client 0x%x"), &aSession);
	TInt l;
	TPckg<TInt> len(l);
	TInt ret=QueryReceiveBuffer(l);
	if (ret==KErrNone)
		{
		ret = aMessage.Write(0, len, 0);
		if (ret!=KErrNone)
			{
			PanicClient(EBadDescriptor,aMessage);
			}
		}
	SafeComplete(aMessage, ret);
	}


void CPort::CommResetBuffers(const RMessage2& aMessage, CCommSession& aSession)
/** Set the receive buffer length

@param aMessage message from the client
@param aSession handle to the client session. */
	{
	(void)aSession;		// to disable urel warnings
	C32LOG2(KC32Player, _L8("CPort::CommResetBuffers(), Client 0x%x"), &aSession);
	if(AreAnyPending()) // Stop RDevComm panicking us and panic client instead
		{
		PanicClient(EResetBuffersWhilePendingRequests,aMessage);		
		}
	else
		{
		ResetBuffers(aMessage.Int0());
		SafeComplete(aMessage, KErrNone);
		}
	}


void CPort::CommReadCancel(TInt aHandle, CCommSession* aClient)
/** Cancel outstanding read

@param aHandle handle to the client
@param aClient handle to the client session. */
	{
	C32LOG2(KC32Player, _L8("CPort::CommReadCancel(), Client 0x%x"), aClient);
	if (iReadOwner==aClient && (!aHandle || aHandle==iReadOwnerHandle))
		{
		ReadCancel();
		ReadCompleted(KErrCancel);
		}
	}


void CPort::CommWriteCancel(TInt aHandle, CCommSession* aClient)
/** Cancel outstanding write

@param aHandle handle to the client
@param aClient handle to the client session. */
	{
	C32LOG2(KC32Player, _L8("CPort::CommWriteCancel(), Client 0x%x"), aClient);
	if (iWriteOwner==aClient && (!aHandle || aHandle==iWriteOwnerHandle))
		{
		WriteCancel();
		WriteCompleted(KErrCancel);
		}
	}


void CPort::CommBreakCancel(TInt aHandle, CCommSession* aClient)
/** Cancel outstanding break

@param aHandle handle to the client
@param aClient handle to the client session. */
	{
	C32LOG2(KC32Player, _L8("CPort::CommBreakCancel(), Client 0x%x"), aClient);
	if (iBreakOwner==aClient && (!aHandle || aHandle==iBreakOwnerHandle))
		{
		BreakCancel();
		BreakCompleted(KErrCancel);
		}
	}


void CPort::CommCancel(TInt aHandle, CCommSession* aClient)
/** Cancel any outstanding requests

@param aHandle handle to the client
@param aClient handle to the client session. */
	{ 
	C32LOG2(KC32Player, _L8("CPort::CommCancel(), Client 0x%x"), aClient);
	CommWriteCancel(aHandle, aClient);
	CommBreakCancel(aHandle, aClient);
	CommReadCancel(aHandle, aClient);

	/* Extended asynchronous calls */
	CommNotifySignalChangeCancel(aHandle, aClient);
	CommNotifyConfigChangeCancel(aHandle, aClient);
	CommNotifyFlowControlChangeCancel(aHandle, aClient);
	CommNotifyBreakCancel(aHandle, aClient);
	CommNotifyOutputEmptyCancel(aHandle, aClient);
	CommNotifyDataAvailableCancel(aHandle, aClient);

	CommSetAccessCancel( aHandle, aClient );
	}


void CPort::FreeMemory()
/** Specifies a protocol by which the comms server can request the protocol to 
release memory. Typically, an implementation may be able to do this by reducing 
internal buffers. 

The default behaviour is to do nothing. */
	{
	}


TInt CPort::WriteTimerExpiredHandler(TAny* aPtr)
/** This static function is called when the write timer expires.
installed by CPort::CommWrite as TCallBack

@param aPtr pointer to the CPort with the timedout write
@return KErrNone always. */
	{
	C32LOG1(KC32Player, _L8("CPort::WriteTimerExpiredHandler()"));
	((CPort *)aPtr)->iWriteTimerPending=EFalse;
	((CPort *)aPtr)->WriteCancel();
	((CPort *)aPtr)->WriteCompleted(KErrTimedOut);

	return KErrNone;
	}


TInt CPort::ReadTimerExpiredHandler(TAny *aPtr)
/** This static function is called when the read timer expires.
installed by CPort::CommRead as TCallBack

@param aPtr pointer to the CPort with the timedout read
@return KErrNone always. */
	{
	C32LOG1(KC32Player, _L8("CPort::ReadTimerExpiredHandler()"));
	((CPort *)aPtr)->iReadTimerPending = EFalse;
	((CPort *)aPtr)->ReadCancel();
	((CPort *)aPtr)->ReadCompleted(KErrTimedOut);
	return KErrNone;
	}


EXPORT_C void CPort::ReadCompleted(TInt aError)
/** Tells the comms server that a read request initiated through StartRead() is 
complete. 

The comms server will then notify the client that its read request is complete. 

Called by C32 server or CSY. 
	
@param aError Return code to be passed back to the client through its TRequestStatus 
argument. */
	{
	C32LOG2(KC32Player, _L8("CPort::ReadCompleted(), Error Code %d"), aError);
	if (iReadOwner)
		{
		if(iBlockedRead.Handle())
			SafeComplete(iBlockedRead, aError);
		}
	iReadOwner = 0;
	if (iReadTimerPending)
		{
		CommTimer::Remove(iReadTimer);
		iReadTimerPending = EFalse;
		}
	}


EXPORT_C void CPort::WriteCompleted(TInt aError)
/** Tells the comms server that a write request initiated through StartWrite() 
is complete. 

The comms server will then notify the client that its write request is complete. 

Called by C32 server or CSY.
	
@param aError Return code to be passed back to the client through its TRequestStatus 
argument. */
	{
	C32LOG2(KC32Player, _L8("CPort::WriteCompleted(), Error Code %d"), aError);
	if (iWriteOwner)
		{
		if(iBlockedWrite.Handle())
			SafeComplete(iBlockedWrite, aError);
	}
	iWriteOwner=0;
	if (iWriteTimerPending)
		{
		CommTimer::Remove(iWriteTimer);
		iWriteTimerPending=EFalse;
		}
    }


EXPORT_C void CPort::BreakCompleted(TInt aError)
/** Tells the comms server that a break request initiated through Break() is complete. 

The comms server will then notify the client that its break request is complete. 

Called by C32 server or CSY.
	
@param aError Return code to be passed back to the client through its TRequestStatus 
argument. */
	{
	C32LOG2(KC32Player, _L8("CPort::BreakCompleted(), Error Code %d"), aError);
	if (iBreakOwner)
		{
		if(iBlockedBreak.Handle())
			SafeComplete(iBlockedBreak, aError);		
		}
	iBreakOwner=0;
	}


TBool CPort::TakeOwnershipForReading(const RMessage2& aMessage,CCommSession* aClient)
/** Check if a Read request is valid and take ownership of port

@param aClient handle to the client session
@return TBool ETrue if ownership granted, EFalse otherwise. */
	{
	C32LOG2(KC32Player, _L8("CPort::TakeOwnershipForReading(), Client 0x%x"), aClient);
	if (!iReadOwner)
		{
		iReadOwner=aClient;
		iReadOwnerHandle=aMessage.Int3();
		return ETrue;
		}
	else
		{
		if (aClient==iReadOwner)
			PanicClient(EReadTwice,aMessage);
		return EFalse;
		}
	}


TBool CPort::TakeOwnershipForWriting(const RMessage2& aMessage,CCommSession* aClient)
/** Check if a Write request is valid and take ownership of port

@param aClient handle to the client session
@return TBool ETrue if ownership granted, EFalse otherwise. */
	{
	C32LOG2(KC32Player, _L8("CPort::TakeOwnershipForWriting(), Client 0x%x"), aClient);
	if (!iWriteOwner)
		{
		iWriteOwner=aClient;
		iWriteOwnerHandle=aMessage.Int3();
		return ETrue;
		}
	else
		{
		if (aClient==iWriteOwner)
			PanicClient(EWriteTwice,aMessage);
		return EFalse;
		}
	}


TBool CPort::TakeOwnershipForBreaking(const RMessage2& aMessage,CCommSession* aClient)
/** Check if a Break request is valid and take ownership of port

@param aClient handle to the client session
@return TBool ETrue if ownership granted, EFalse otherwise. */
	{
	C32LOG2(KC32Player, _L8("CPort::TakeOwnershipForBreaking(), Client 0x%x"), aClient);
	if (!iBreakOwner)
		{
		iBreakOwner=aClient;
		iBreakOwnerHandle=aMessage.Int3();
		return ETrue;
		}
	else
		{
		if (aClient==iBreakOwner)
			PanicClient(EBreakTwice,aMessage);
		return EFalse;
		}
	}


TBool CPort::AreAnyPending()
/** Return true if there is an outstanding Read/Write/Break on the CSY. */
	{
	return (iBreakOwner || iWriteOwner || iReadOwner);
	}


EXPORT_C TInt CPort::IPCRead(const TAny* /*aPtr*/, TDes8& aDes, TInt aOffset) const
/** Reads data from the client's (the user of the port's) address space.

The client address space pointer is obtained from the aClientBuffer argument to
StartRead().

Note:

This function is normally called by the CSY

Note:

Used as part of RComm::Write

@see RThread::ReadL() for more information on reading data in other address spaces.
@param aPtr    Not Used. The client data is now obtained from iBlockedWrite.
@param aDes    A descriptor (8 bit variant) into which the result of the client
               address space read operation will be stored
@param aOffset The read offset from the start of the client's descriptor data.
@return TInt   KErrNone: success
               KErrNotReady: there is no read or write request outstanding
               KErrBadDescriptor: the client's descriptor is not valid. 
*/
	{
	if (!iWriteOwner)
		{
		return KErrNotReady;
		}
	TInt ret = iBlockedWrite.Read(0, aDes, aOffset);
	if (ret!=KErrNone)
		{
		C32LOG1(KC32Player, _L8("Error at the time of reading data from client"));
		PanicClient(EBadDescriptor,iBlockedWrite);
		}
	return ret;
	}


EXPORT_C TInt CPort::IPCWrite(const TAny* /*aPtr*/, const TDesC8& aDes, TInt aOffset) const
/** Writes into the client's (the user of the port's) address space. 

The client address space pointer is obtained from the aClientBuffer argument to
StartWrite().

Note:

This function is normally called by the CSY

Note:

Used as part of RComm::Read

@see RThread::WriteL() for more information on writing data to other address spaces.
@param aPtr    Client address space pointer
@param aDes    A descriptor (8 bit variant) into which the result of the client 
address space read will be stored
@param aOffset The offset from aPtr at which to start reading
@return TInt   KErrNone: success
               KErrNotReady: there is no read or write request outstanding 
               KErrBadDescriptor: the client's descriptor is not valid. */
	{
	if (!iReadOwner)
		return KErrNotReady;
	TInt ret = iBlockedRead.Write(0, aDes, aOffset);
	if (ret!=KErrNone)
		{
		PanicClient(EBadDescriptor, iBlockedRead);
		}
	return ret;
	}



//
// implementation of CSerial
//

EXPORT_C CSerial::CSerial()
/** Default constructor. 

Derived classes can implement a NewL() function if they require 
two-phase construction. */
	{
	}


EXPORT_C CSerial::~CSerial()
/** Destructor. */
	{
	C32LOG1(KC32Player, _L8("CSerial::~CSerial()"));
	if (iLibUnloader)
		{
		iLibUnloader->Call();
		}
	// During shutdown, if a client still has a CSY loaded,
	// this destructor's calling of its base destructor (CObject)
	// will cause a panic when the kernel attempts to delete the CObject due
	// to the kernel seeing the outstanding access count.
	// There could be a better way of aiding this diagnosis than just spotting this comment
	// when the call stack is analysed, but we haven't found such a way yet.
	// In a debugger, inspecting the name of the object should reveal the CSY involved.
	}


EXPORT_C TBool CSerial::QueryVersionSupported(const TVersion& aVersion ) const
// Pass through to default QVS
/** Specifies the protocol for checking the supplied TVersion against the protocol's 
module version. 

The default implementation calls User::QueryVersionSupported() to perform the check 
against the object's iVersion member.

@param aVersion The version to be checked.
@return ETrue if the version is supported. */
	{
	return(User::QueryVersionSupported(iVersion,aVersion));
	}

EXPORT_C TSecurityPolicy CSerial::PortPlatSecCapability(TUint /*aPort*/) const
/**
Retrieve the security policy for a requested port.
This base implementation sets the default policy to be an 'AlwaysFail' policy.

@param aPort Port number.
@return TSecurityPolicy Security policy associated with opeing this port
*/
	{
	return TSecurityPolicy(TSecurityPolicy::EAlwaysFail);
	}

void CSerial::ConstructL(RLibrary& aLib)
/** Allocate a CLibrary unloader

@param aLib reference to the library to be unloaded in the future
@leave OOM this function may leave if out of memory. */
	{
	iLibUnloader = CLibUnloader::NewL(aLib);
	}


EXPORT_C void CSerial::CSerial_Reserved1()
/** Reserved virtual function. */
	{
	}


void CSerial::ModuleName(TDes& aName)
/** Get the module name from the libary reference

@param aName Module name will be written to this descriptor. */
    {
	TFileName filename = iLibUnloader->iLib.FileName();
	TParsePtrC pc(filename);
	aName = pc.Name();
    }


// EOF - CS_PORT.CPP
