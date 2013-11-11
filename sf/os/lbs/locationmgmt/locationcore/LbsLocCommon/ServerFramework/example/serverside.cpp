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
// server side exmaple of how to use the abstract server framework
// 
//

#include "serverside.h"
#include "examplemessageenums.h"
#include "opensessionparams.h"
#include <e32debug.h>
#include "lbsdevloggermacros.h"



const TUint myRangeCount = 4;
const TInt myRanges[myRangeCount] = 
    {
    0, //range is 0 to (ELastSessionMessagId-1) inclusive
    ELastSessionMessageId, //range is ELastSessionMessageId to (EExTimeServerGetTimeSync-1) (inclusive) only
    EExTimeServerGetTimeSync, //range is 16383+1 - 16383+4  inclusive
    EExInVaildMessage+5 // range is 16383+5  - KMaxTInt inclusive
    };
    
const TUint8 myElementsIndex[myRangeCount] = 
    {
    CPolicyServer::EAlwaysPass,     //applies to 0th range (req num: 0-1)
    CPolicyServer::ENotSupported,   //applies to 1st range (req num: 2)
    0,                              //applies to 2nd range (req num: 3-10)
    CPolicyServer::ENotSupported	//applies to 3nd range (req num: 11-KMaxTInt)
    };
    
const CPolicyServer::TPolicyElement myElements[] = 
    {
    {_INIT_SECURITY_POLICY_C1(ECapabilityReadDeviceData), CPolicyServer::EFailClient}
    };
    
const CPolicyServer::TPolicy myPolicy =
    {
    CPolicyServer::EAlwaysPass, //specifies all connect attempts should pass
    myRangeCount,                   
    myRanges,
    myElementsIndex,
    myElements,
    };


MCreateServerImpl* CSecureASBase::GetImplLC()
	{
	CExTimeServerImpl* impl = new(ELeave) CExTimeServerImpl();
	CleanupStack::PushL(impl);
	return impl;
	}

void CExTimeServerImpl::CreateServerLC(TServerStartParams& aParams)
	{
	CExTimeServer* s = new(ELeave) CExTimeServer(CActive::EPriorityStandard, myPolicy);
	CleanupStack::PushL(s);
	s->ConstructL(aParams.GetServerName());
	// leave the server object on the CS
	}
	
//-------------------------------------------------------------

CExTimeServer::CExTimeServer(TInt aPriority, const CPolicyServer::TPolicy& aSecurityPolicy) : 
																		CSecureServerBase(aPriority, aSecurityPolicy),
																		iVersion(0,0,1)
	{
	LBSLOG(ELogP1,"->CExTimeServer::CExTimeServer");	
	}
	
CExTimeServer::~CExTimeServer()
	{
	LBSLOG(ELogP1,"->CExTimeServer::CExTimeServer");		
	}
	
void CExTimeServer::ConstructL(const TDesC& aServerName)
	{
	StartL(aServerName);
	BaseConstructL(); // MUST BE CALLED	
	}
	
CSession2* CExTimeServer::DoNewSessionL(const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/) const
	{
	return new (ELeave) CExTimeSession(); // version number already checked at this point
	}
	
	
TVersion CExTimeServer::GetServerVersion() const
	{
	return iVersion;
	}

// called by the CServer2 code to complete construction of the session 
void CExTimeSession::CreateL()
	{
	CSecureSessionSubSessionBase::CreateL(); // MUST do this
	}
	
void CExTimeSession::ServiceMessageL(const RMessage2& /*aMessage*/)
	{
	// service session level messages in here
	}
	
void CExTimeSession::ServiceMessageError(const RMessage2& /*aMessage*/, const TInt /*aError*/)
	{
	// service leaves from the above in here
	}
	
	
//------------------------------------------------------------------	

MSubSessionImpl* CSubSessionBase::GetImplL(const RMessage2& aMessage)
	{
	// need to look at the RMessage to see what sort of subsession
	// that we have been asked to created. NB the type of the object contained in the
	// is the same type of the object in the first template parameter.
	// This can be anything!
	TSubSessionType subSessType;
	TPckg<TSubSessionType> subSessTypeBuf(subSessType);
	MessageUtils::Read(aMessage, 0, subSessTypeBuf); // data is always in slot zero
	
	if(subSessType.GetSubSessionType() == TSubSessionType::EExTimeSubSession)
		{
		// the subsession must implement the MSubSessionImpl mixin
		CExTimeSubSession* subSession = new (ELeave) CExTimeSubSession();
		CleanupStack::PushL(subSession);
		// so other 2nd phase things here
		subSession->ConstructL();
		CleanupStack::Pop(subSession);
		return subSession;
		}
	if(subSessType.GetSubSessionType() == TSubSessionType::EExServerNameSubSession)
		{
		// the subsession must implement the MSubSessionImpl mixin
		CExServerNameGetterSession* subSession = new (ELeave) CExServerNameGetterSession();
		CleanupStack::PushL(subSession);
		// so other 2nd phase things here
		subSession->ConstructL();
		CleanupStack::Pop(subSession);
		return subSession;
		}
	else
		{
		//panic the client - you have no idea what they asked for!
		_LIT(KExServer, "unknown subsession type");
		MessageUtils::PanicClient(aMessage, KExServer, 0);
		return NULL; // wonder if we should cause this to abord the entire
		// sub session creation process. Need to think about this....ToDo
		}
	}

void CExTimeSubSession::ConstructL()
	{
	iTimer=CPeriodic::NewL(0);
	}

CExTimeSubSession::~CExTimeSubSession()
	{
	// cancel ANY outstanding requests with KErrServerTerminated
	// or some other error code to indicate that the session has been closed
	// early
	if(!iStoredMsg.IsNull())
		{
		iStoredMsg.Complete(KErrServerTerminated);
		}
	iTimer->Cancel();
	delete iTimer;
	}

void CExTimeSubSession::DispatchL(const RMessage2& aMessage)
	{
	switch(aMessage.Function())
		{
	case EExTimeServerGetTimeSync:
		GetTime(aMessage);
		break;
	case EExTimeServerGetTimeAsync:
		GetTimeAsync(aMessage);
		break;
	case EExTimeServerCancelGetTime:
		CancelGetTime(aMessage);
		break;
		}
	}
	
void CExTimeSubSession::DispatchError(const RMessage2& /*aMessage*/, TInt /*aError*/)
	{
	// errors from DispatchL end up in here
	}
	
void CExTimeSubSession::CreateSubSessionL(const RMessage2& aMessage, const CSecureServerBase* /*aServer*/)
	{
	// this is really the class constructor!
	// Check that the params arrived...
	TTimeOpenSessionParams params;
	TPckg<TTimeOpenSessionParams> openSessionParamsBuf(params);
	MessageUtils::Read(aMessage, 1, openSessionParamsBuf);
	}
	
void CExTimeSubSession::CloseSubSession()
	{
	// undo anything you did in the Create - is you allocated property etc
	// In case this there is nothing to do.
	}
	
void CExTimeSubSession::VirtualRelease()
	{
	LBSLOG(ELogP1,"->CExTimeSubSession::VirtualRelease");	
	
	//delete ourselves - can be called on a via CS on a leave
	delete this; // we are allocated via the GetImpl above, and a handle to "us" is returned
	}
	
void CExTimeSubSession::GetTime(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1,"->CExTimeSubSession::GetTime");	
	TTime time;
	time.UniversalTime();
	TDateTime dateTime(time.DateTime());
	TPckg<TDateTime> dateTimeBuf(dateTime);
	MessageUtils::Write(aMessage, 0, dateTimeBuf);
	aMessage.Complete(KErrNone);
	LBSLOG(ELogP1,"<-CExTimeSubSession::GetTime");
#endif	

	}
	
void CExTimeSubSession::GetTimeAsync(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1,"->CExTimeSubSession::GetTimeAsync");		
	// grab the RMssaeg and store it now. Mega important, if we don't
	// do this the next message from the client will trmple on this and 
	// we loose the message
	iStoredMsg = aMessage; // in reality you would put this on a list...
	// do do the operation - in this case we'll simulate by answering the request
	// in one sec
	const TInt delay=1000000; // one sec
	iTimer->Start(delay, delay, TCallBack(TimerExpired, this));
	LBSLOG(ELogP1,"<-CExTimeSubSession::GetTimeAsync");	
	}

void CExTimeSubSession::CancelGetTime(const RMessage2& aMessage)
	{
	// 1) cancel the outstanding async request - if oen exists with KErrCancel
	if(!iStoredMsg.IsNull())
		{
		iStoredMsg.Complete(KErrCancel);
		}
	// 2) regardless complete this message i.e. the cancel request
	// with KErrNone
	aMessage.Complete(KErrNone);
	}

// must be static
TInt CExTimeSubSession::TimerExpired(TAny* aObject)
    {
    // cast, and call non-static function
    ((CExTimeSubSession*)aObject)->DoTimerExpired();
    return 0; // return value is ignored!
    }
    
void CExTimeSubSession::DoTimerExpired()
	{
	LBSLOG(ELogP1,"->CExTimeSubSession::DoTimerExpired");	
	TTime time;
	time.UniversalTime();
	TDateTime dateTime(time.DateTime());
	TPckgBuf<TDateTime> dateTimeBuf(dateTime);
	MessageUtils::Write(iStoredMsg, 0, dateTimeBuf);
	iStoredMsg.Complete(KErrNone);
	iTimer->Cancel(); // stop the timer
	LBSLOG(ELogP1,"<-CExTimeSubSession::DoTimerExpired");
	
	
	}

//-------------------------------------------------------------------------
// action methods
void CExServerNameGetterSession::ConstructL()
	{
	// nothing to do in this example
	}
	
CExServerNameGetterSession::~CExServerNameGetterSession()
	{
	// nothing to do in this example
	}
	
// from MSubSessionImpl
void CExServerNameGetterSession::DispatchL(const RMessage2& aMessage)
	{
	switch(aMessage.Function())
		{
	case EExTimeServerGetServerName:
		GetNames(aMessage);
		break;
		}	
	}
	
void CExServerNameGetterSession::DispatchError(const RMessage2& /*aMessage*/, TInt /*aError*/)
	{
	// errors from DispatchL end up in here
	}
	
void CExServerNameGetterSession::CreateSubSessionL(const RMessage2& aMessage,const CSecureServerBase* /*aServer*/)
	{
	// this is really the class constructor!
	// Check that the params arrived...as a dummy operation
	TServerNameOpenSessionParams params;
	TPckg<TServerNameOpenSessionParams> openSessionParamsBuf(params);
	MessageUtils::Read(aMessage, 1, openSessionParamsBuf);
	}
	
void CExServerNameGetterSession::CloseSubSession()
	{
	}
// via MSubSessionImpl::MRelease	
void CExServerNameGetterSession::VirtualRelease()
	{
	// delete ourselves
	delete this;
	}
	
void CExServerNameGetterSession::GetNames(const RMessage2& aMessage)
	{
	TName shortName;	
	TFullName longName;
	RProcess p;
	shortName = p.Name();
	longName = p.FullName();
	p.Name();
	p.FullName();
	MessageUtils::Write(aMessage, 0, shortName);
	MessageUtils::Write(aMessage, 1, longName);
	aMessage.Complete(KErrNone);
	}
	
