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

#include "ReqstProcessor.h"
#include "rpsasciirqstdefs.h"
#include "ReqstDoer.h"
#include "rpscommon.h"
#include "RpsSFlogger.h"
#include <e32debug.h>

CReqstProcessor* CReqstProcessor::NewL(CEtelMngr& aEtel)
	{
	CReqstProcessor* self = new(ELeave)CReqstProcessor(aEtel);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CReqstProcessor::CReqstProcessor(CEtelMngr& aEtel)
	: iEtel(aEtel)
	{
	}
	
CReqstProcessor::~CReqstProcessor()
	{
	CleanupReqstDoer();
	}
	
void CReqstProcessor::ConstructL()
	{
	}
	
//
// Processes the received reqst data by instantiating the appropriate reqst Active Objects
//
TInt CReqstProcessor::ProcessL(CRpsMsg& aRqstMsg)  
	{
	LOGLINE1(_L(">>CReqstProcessor::ProcessL xx"));
	TInt ret(KErrNone);
	
	TPtrC8 opCode8 = aRqstMsg.GetOpCode();

	TInt elements(aRqstMsg.ArraySize());
	TInt numArgs = elements - KRespMsgElementsNoArgs;
	
	if(iReqstDoer)
		{
		LOGLINE1(_L("WARNING: ReqstDoer already active and will be deleted!!!!"));
		LOGLINE1(_L("WARNING: ReqstDoer already active and will be deleted!!!!"));
		CleanupReqstDoer();
		}	
	
	if(!opCode8.Compare(KCallNumber))
		{
		LOGLINE1(_L("KCallNumber"));
		// Expects a line and tel num to call
		__ASSERT_ALWAYS(numArgs==2, User::Panic(KPanicMsgFormat, KBadNumberOfArgs)); 
			
		// Create correct Doer for reqst type, removes previous reqst object		
		iReqstDoer = CCallNumberDoer::NewL(iEtel, aRqstMsg);
		}
		
	// Dial then Hang up after a delay (sec). Note the dial request will complete
	// after the call connects. Only after this will the HangUp request be actioned.
	else if(!opCode8.Compare(KDialHangUp))
		{
		LOGLINE1(_L("KDialHangUp"));
		// Expects a line, tel num to call and a delay in secs 
		__ASSERT_ALWAYS(numArgs==3, User::Panic(KPanicMsgFormat, KBadNumberOfArgs));
			
		// Create correct Doer for reqst type, removes previous reqst object
		iReqstDoer = CCallNumberHangUpDoer::NewL(iEtel, aRqstMsg);
		}
		
	// Dial then Hang up after a delay (sec) but before the dial connects.
	else if(!opCode8.Compare(KDialHangUpBeforeConnect))
		{
		LOGLINE1(_L("KDialHangUpBeforeConnect"));
		// Expects a line, tel num to call and a delay in secs 
		__ASSERT_ALWAYS(numArgs==3, User::Panic(KPanicMsgFormat, KBadNumberOfArgs));
			
		// Create correct Doer for reqst type, removes previous reqst object
		iReqstDoer = CCallNumberHangUpBeforeConnectDoer::NewL(iEtel, aRqstMsg);
		}
		
	// Dial then Hang up after a delay (sec) once the dial connects.
	else if(!opCode8.Compare(KDialHangUpAfterConnect))
		{
		LOGLINE1(_L("KDialHangUpAfterConnect"));
		// Expects a line, tel num to call and a delay in secs
		__ASSERT_ALWAYS(numArgs==3, User::Panic(KPanicMsgFormat, KBadNumberOfArgs));
			
		// Create correct Doer for reqst type, removes previous reqst object
		iReqstDoer = CCallNumberHangUpAfterConnectDoer::NewL(iEtel, aRqstMsg);
		}
	
	else if(!opCode8.Compare(KAnswerNextCall))
		{
		LOGLINE1(_L("KAnswerNextCall"));
		// Expects a line
		__ASSERT_ALWAYS(numArgs==1, User::Panic(KPanicMsgFormat, KBadNumberOfArgs));
			
		// Create correct Doer for reqst type
		iReqstDoer = CAnswerNextCallDoer::NewL(iEtel, aRqstMsg);
		}
		
	else if(!opCode8.Compare(KAnswerNextCallHangUp))
		{
		LOGLINE1(_L("KAnswerNextCallHangUp"));
		// Expects a line and delay in secs
		__ASSERT_ALWAYS(numArgs==2, User::Panic(KPanicMsgFormat, KBadNumberOfArgs));
			
		// Create correct Doer for reqst type
		iReqstDoer = CAnswerNextCallHangUpDoer::NewL(iEtel, aRqstMsg);
		}
		
	else if(!opCode8.Compare(KAnswerNextCallHold))
		{
		LOGLINE1(_L("KAnswerNextCallHold"));
		// Expects a line and delay in secs
		__ASSERT_ALWAYS(numArgs==2, User::Panic(KPanicMsgFormat, KBadNumberOfArgs));
			
		// Create correct Doer for reqst type
		iReqstDoer = CAnswerNextCallHoldDoer::NewL(iEtel, aRqstMsg);
		}

	else if(!opCode8.Compare(KRejectNextCall))
		{
		LOGLINE1(_L("KRejectNextCall"));
		// TODO
		}
		
	else if(!opCode8.Compare(KIgnoreNextCall))
		{
		LOGLINE1(_L("KIgnoreNextCall"));

		// TODO
		}
		
	else if(!opCode8.Compare(KHangUp))
		{
		LOGLINE1(_L("KHangUp"));
		// Expects a line and delay in secs
		__ASSERT_ALWAYS(numArgs==2, User::Panic(KPanicMsgFormat, KBadNumberOfArgs));
			
		// Create correct Doer for reqst type, removes previous reqst object
		iReqstDoer = CHangUpDoer::NewL(iEtel, aRqstMsg);
		}
		
	else if(!opCode8.Compare(KHold))
		{
		LOGLINE1(_L("KHold"));
		// Expects a line and delay in secs
		__ASSERT_ALWAYS(numArgs==2, User::Panic(KPanicMsgFormat, KBadNumberOfArgs));
			
		// Create correct Doer for reqst type, removes previous reqst object
		iReqstDoer = CHoldDoer::NewL(iEtel, aRqstMsg);
		}
	
	else if(!opCode8.Compare(KResume))
		{
		LOGLINE1(_L("KResume"));
		// Expects a line and delay in secs
		__ASSERT_ALWAYS(numArgs==2, User::Panic(KPanicMsgFormat, KBadNumberOfArgs));
			
		// Create correct Doer for reqst type, removes previous reqst object
		iReqstDoer = CResumeDoer::NewL(iEtel, aRqstMsg);
		}
		
	else if(!opCode8.Compare(KCheckRpsPresent))
		{
		LOGLINE1(_L("KCheckRpsPresent"));
		// No Etel action required for this request. 
		}
		
	else if(!opCode8.Compare(KHangUpNextCallBeforeConnect))
		{
		LOGLINE1(_L("KHangUpNextCallBeforeConnect"));
		// Expects a line as args
		__ASSERT_ALWAYS(numArgs==1, User::Panic(KPanicMsgFormat, KBadNumberOfArgs));
			
		// Create correct Doer for reqst type, removes previous reqst object
		iReqstDoer = CHangUpNextCallBeforeConnectDoer::NewL(iEtel, aRqstMsg);
		}
		
	else if(!opCode8.Compare(KSendDataBackOnDataPort))
		{
		// Expects textual data arg
		__ASSERT_ALWAYS(numArgs==1, User::Panic(KPanicMsgFormat, KBadNumberOfArgs));
			
		// Create correct Doer for reqst type, removes previous reqst object
		iReqstDoer = CSendDataBackOnDataPortDoer::NewL(iEtel, aRqstMsg);
		}
	
	else if(!opCode8.Compare(KCleanup))
		{
		LOGLINE1(_L("KCleanup"));
		// Expects textual data arg
		__ASSERT_ALWAYS(numArgs==0, User::Panic(KPanicMsgFormat, KBadNumberOfArgs));
			
		// Create correct Doer for reqst type, removes previous reqst object
		iReqstDoer = CCleanupUpDoer::NewL(iEtel, aRqstMsg);
		}
		
	else
		{
		LOGLINE1(_L("request not found!"));
		ret = KErrNotFound;
		}
	
	if(iReqstDoer)
		{
		//register ourselves as an observer
		LOGLINE1(_L("Registering and kicking of the ReqstDoer"));
		iReqstDoer->RegisterReqstObserver(this);
		iReqstDoer->DoReqstL();
		}
	LOGLINE1(_L("<<CReqstProcessor::ProcessL"));
	return ret;
	}
	
void CReqstProcessor::ReqstCompleted()
	{
	LOGLINE1(_L(">>CReqstProcessor::ReqstCompleted"));
	//a request has finished so we can delete the doer object
	LOGLINE1(_L("ReqstCompleted - deleting iReqstDoer;"));
	CleanupReqstDoer();
	LOGLINE1(_L("<<CReqstProcessor::ReqstCompleted"));
	}

void CReqstProcessor::CleanupReqstDoer()
	{
	LOGLINE1(_L(">>CReqstProcessor::CleanupReqstDoer"));
	LOGLINE1(_L("CReqstProcessor::CleanupReqstDoer()"));
	//deregister ourselves as an observer and deleet the object
	iReqstDoer->DeRegisterReqstObserver();
	delete iReqstDoer;
	iReqstDoer = NULL;
	LOGLINE1(_L("<<CReqstProcessor::CleanupReqstDoer"));
	}
