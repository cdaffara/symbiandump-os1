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

#include "ReqstDoer.h"
#include "ReqstObject.h"
#include "rpsasciirqstdefs.h"
#include "rpscommon.h"
#include "ReqstProcessor.h"
#include <e32debug.h>
#include "RpsSFlogger.h"

CReqstDoer::CReqstDoer()
: iReqstObserver(NULL)	
	{
	}

CReqstDoer::~CReqstDoer()
	{
	// Delete all objects held in iReqstObjArray
	for(TInt a=0; a < iReqstObjArray.Count(); ++a)
		{
		delete iReqstObjArray[a];
		}
	iReqstObjArray.Close();
	}
	

void CReqstDoer::RegRqstObjL(CReqstObjectBase* aRqstObj)
	{
	aRqstObj->AttachL(this);
	iReqstObjArray.AppendL(aRqstObj);
	}

	
void CReqstDoer::DoReqstL()
	{
	LOGLINE1(_L(">>CReqstDoer::DoReqstL"));
	// Invoke the first rqst object in iReqstObjArray. Any other rqst objects held in 
	// iReqstObjArray will be invoked by UpdateL which is called when the previous object completes.
	if(iReqstObjArray.Count())
		{
		iReqstObjArray[0]->StartL();
		}
	LOGLINE1(_L("<<CReqstDoer::DoReqstL"));
	}
	
// Called when a rqst object completes. Activate the next rqst object in the array.
void CReqstDoer::UpdateL(MSubject* aSubject)
	{
	LOGLINE1(_L(">>CReqstDoer::UpdateL"));
	// We are here when a rqst object's RunL gets called (the Subject).
	// Invoke the next rqst object from iReqstObjArray if it exists.
	TInt indx(0);
	TRAPD(err, indx = iReqstObjArray.FindL(static_cast<CReqstObjectBase*>(aSubject)));
	if(err == KErrNone)
		{
		++indx;
		if(iReqstObjArray.Count() > indx)
			{
			//still got other async calls to make in order to complete this request.
			iReqstObjArray[indx]->StartL();
			}
		else
			{
			//have completed the requests so can call back to the request observer to say we have finished
			if(iReqstObserver)
				iReqstObserver->ReqstCompleted();
			}
		}
	LOGLINE1(_L("<<CReqstDoer::UpdateL"));
	}

// Called when a rqst object (normally one that is is part of a ganged-request) runs into some 
// error condition and decides it's not worth continuing with the remaining request objects.
void CReqstDoer::Abort()
	{
	LOGLINE1(_L(">>CReqstDoer::Abort"));
	for(TInt a=0; a < iReqstObjArray.Count(); ++a)
		{
		delete iReqstObjArray[a];
		iReqstObjArray.Remove(a);
		}
		
	iReqstObserver->ReqstCompleted();
	LOGLINE1(_L("<<CReqstDoer::Abort"));
	}
	
TEtelLine CReqstDoer::GetLine(TDesC8& aLine)
	{
	TEtelLine line = EVoiceLine; //as default;
	
	TBuf16<20> line16;
	line16.Copy(aLine);
	if(line16 == KRpsVoiceLine)
		line = EVoiceLine;
	else if(line16 == KRpsDataLine)
		line = EDataLine;
	else if(line16 == KRpsFaxLine)
		line = EFaxLine;
	
	return line;
	}
	
void CReqstDoer::RegisterReqstObserver(MReqstObserver* aObserver)
//register the single observer.  We only alow one to be registered at a time.  Panic if more than one try
	{
	__ASSERT_ALWAYS(iReqstObserver==NULL, User::Panic(KPanicMsgFormat,KObserverAlreadyRegistered));
	iReqstObserver = aObserver;
	}
void CReqstDoer::DeRegisterReqstObserver()
//deregister observer interest.  Since there can be only one, we do not need to find out which one needs dergistering.
	{
	iReqstObserver = NULL;
	}
	
//
/* 
Below are the concrete Doer objects definitions.
New Doer objects must 
1) 	create the appropriate RqstObj object(s) required to satisfy the 
	type of request.
2) 	Identify the correct argument offset required by the RqstObj object in ConstructL
3)	Register each RqstObj by calling RegRqstObjL(). This stores the object and 
	registers us as an observer.
*/

//
// CCallNumberDoer
//
CCallNumberDoer* CCallNumberDoer::NewL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	CCallNumberDoer* self = new(ELeave)CCallNumberDoer();
	CleanupStack::PushL(self);
	self->ConstructL(aEtel, aRqstMsg);
	CleanupStack::Pop(self);
	return self;
	}
	
CCallNumberDoer::CCallNumberDoer()
	{
	}
		
CCallNumberDoer::~CCallNumberDoer()
	{
	}
	
void CCallNumberDoer::ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	LOGLINE1(_L(">>CCallNumberDoer::ConstructL"));
	// Create correct reqst object(s) to handle this : CCallNumber
	// arguments expected : line at arg 1. tel. number at arg 2
	
	TEtelLine line;
 	TPtrC8 ptr8 = aRqstMsg.GetArgL(KArgNumber1);
	line = GetLine(ptr8);	
	CReqstObjectBase* rqstObj = CCallNumber::NewL(aEtel, line, aRqstMsg.GetArgL(KArgNumber2));
	// Register rqst object
	RegRqstObjL(rqstObj);
	LOGLINE1(_L("<<CCallNumberDoer::ConstructL"));
	}	

//
// CHangUpDoer
//	
CHangUpDoer* CHangUpDoer::NewL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	CHangUpDoer* self = new(ELeave)CHangUpDoer();
	CleanupStack::PushL(self);
	self->ConstructL(aEtel, aRqstMsg);
	CleanupStack::Pop(self);
	return self;
	}
	
CHangUpDoer::CHangUpDoer()
	{
	}
	
CHangUpDoer::~CHangUpDoer()
	{
	}
	
void CHangUpDoer::ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	LOGLINE1(_L(">>CHangUpDoer::ConstructL"));
	// Create correct reqst object(s) to handle this : CHangUp
	// arguments expected : line at arg 1.  delay in secs at arg 2
	TEtelLine line;
	TPtrC8 ptr8 = aRqstMsg.GetArgL(KArgNumber1);
	line = GetLine(ptr8);
	CReqstObjectBase* rqstObj = CHangUp::NewL(aEtel, line, aRqstMsg.GetArgL(KArgNumber2));
	// Register rqst object
	RegRqstObjL(rqstObj);
	LOGLINE1(_L("<<CHangUpDoer::ConstructL"));
	}
	
//
// CHoldDoer
//	
CHoldDoer* CHoldDoer::NewL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	CHoldDoer* self = new(ELeave)CHoldDoer();
	CleanupStack::PushL(self);
	self->ConstructL(aEtel, aRqstMsg);
	CleanupStack::Pop(self);
	return self;
	}
	
CHoldDoer::CHoldDoer()
	{
	}
	
CHoldDoer::~CHoldDoer()
	{
	}
	
void CHoldDoer::ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	LOGLINE1(_L(">>CHoldDoer::ConstructL"));
	// Create correct reqst object(s) to handle this : CHold
	// arguments expected : line at arg 1. delay in secs at arg 2
	
	TEtelLine line;
	TPtrC8 ptr8 = aRqstMsg.GetArgL(KArgNumber1);
	line = GetLine(ptr8);
	CReqstObjectBase* rqstObj = CHold::NewL(aEtel, line, aRqstMsg.GetArgL(KArgNumber2));
	// Register rqst object
	RegRqstObjL(rqstObj);
	LOGLINE1(_L("<<CHoldDoer::ConstructL"));
	}
	
//
// CResumeDoer
//	
CResumeDoer* CResumeDoer::NewL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	CResumeDoer* self = new(ELeave)CResumeDoer();
	CleanupStack::PushL(self);
	self->ConstructL(aEtel, aRqstMsg);
	CleanupStack::Pop(self);
	return self;
	}

CResumeDoer::CResumeDoer()
	{
	}
	
CResumeDoer::~CResumeDoer()
	{
	}
	
void CResumeDoer::ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	LOGLINE1(_L(">>CResumeDoer::ConstructL"));
	// Create correct reqst object(s) to handle this : CResume
	// arguments expected : line at arg1. delay in secs at arg 2
	TEtelLine line;
	TPtrC8 ptr8 = aRqstMsg.GetArgL(KArgNumber1);
	line = GetLine(ptr8);
	
	CReqstObjectBase* rqstObj = CResume::NewL(aEtel, line, aRqstMsg.GetArgL(KArgNumber2));
	// Register rqst object
	RegRqstObjL(rqstObj);
	LOGLINE1(_L("<<CResumeDoer::ConstructL"));
	}
	
//
// CCallNumberHangUpDoer
//	
CCallNumberHangUpDoer* CCallNumberHangUpDoer::NewL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	CCallNumberHangUpDoer* self = new(ELeave)CCallNumberHangUpDoer();
	CleanupStack::PushL(self);
	self->ConstructL(aEtel, aRqstMsg);
	CleanupStack::Pop(self);
	return self;
	}
	
CCallNumberHangUpDoer::CCallNumberHangUpDoer()
	{
	}
	
CCallNumberHangUpDoer::~CCallNumberHangUpDoer()
	{
	}
	
void CCallNumberHangUpDoer::ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	LOGLINE1(_L(">>CCallNumberHangUpDoer::ConstructL"));
	// Create correct reqst object(s) to handle this : CCallNumber and CHangUp
	// arguments expected : line at arg1. tel. number at arg 2, delay at arg 3
	
	TEtelLine line;
	TPtrC8 ptr8 = aRqstMsg.GetArgL(KArgNumber1);
	line = GetLine(ptr8);
	CReqstObjectBase* rqstObj1 = CCallNumber::NewL(aEtel, line, aRqstMsg.GetArgL(KArgNumber2));
	// Register rqst object
	RegRqstObjL(rqstObj1);
	
	CReqstObjectBase* rqstObj2 = CHangUp::NewL(aEtel, line, aRqstMsg.GetArgL(KArgNumber3));
	// Register rqst object
	RegRqstObjL(rqstObj2);
	LOGLINE1(_L("<<CCallNumberHangUpDoer::ConstructL"));
	}
	

//
// CCallNumberHangUpBeforeConnectDoer
//
CCallNumberHangUpBeforeConnectDoer* CCallNumberHangUpBeforeConnectDoer::NewL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	CCallNumberHangUpBeforeConnectDoer* self = new(ELeave)CCallNumberHangUpBeforeConnectDoer();
	CleanupStack::PushL(self);
	self->ConstructL(aEtel, aRqstMsg);
	CleanupStack::Pop(self);
	return self;
	}
	
CCallNumberHangUpBeforeConnectDoer::CCallNumberHangUpBeforeConnectDoer()
	{
	}
	
CCallNumberHangUpBeforeConnectDoer::~CCallNumberHangUpBeforeConnectDoer()
	{
	}
	
void CCallNumberHangUpBeforeConnectDoer::ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	LOGLINE1(_L(">>CCallNumberHangUpBeforeConnectDoer::ConstructL"));
	// Create correct reqst object(s) to handle this : CCallNumberNotifyStatusChange, CDialCancel
	// arguments expected : line at arg1.  tel. number at arg 2, delay at arg 3
	TEtelLine line;
	TPtrC8 ptr8 = aRqstMsg.GetArgL(KArgNumber1);
	line = GetLine(ptr8);
	
	CReqstObjectBase* rqstObj1 = CCallNumberNotifyStatusChange::NewL(aEtel, line, aRqstMsg.GetArgL(KArgNumber2), RCall::EStatusConnecting);
	RegRqstObjL(rqstObj1);
	
	CReqstObjectBase* rqstObj2 = CDialCancel::NewL(aEtel, line, aRqstMsg.GetArgL(KArgNumber3));
	RegRqstObjL(rqstObj2);
	LOGLINE1(_L("<<CCallNumberHangUpBeforeConnectDoer::ConstructL"));
	}
	
//
// CCallNumberHangUpAfterConnectDoer
//
CCallNumberHangUpAfterConnectDoer* CCallNumberHangUpAfterConnectDoer::NewL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	CCallNumberHangUpAfterConnectDoer* self = new(ELeave)CCallNumberHangUpAfterConnectDoer();
	CleanupStack::PushL(self);
	self->ConstructL(aEtel, aRqstMsg);
	CleanupStack::Pop(self);
	return self;
	}
	
CCallNumberHangUpAfterConnectDoer::CCallNumberHangUpAfterConnectDoer()
	{
	}
	
CCallNumberHangUpAfterConnectDoer::~CCallNumberHangUpAfterConnectDoer()
	{
	}
	
void CCallNumberHangUpAfterConnectDoer::ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	LOGLINE1(_L(">>CCallNumberHangUpAfterConnectDoer::ConstructL"));
	// Create correct reqst object(s) to handle this : CCallNumberNotifyStatusChange, CHangUp
	// arguments expected : line at arg1. tel. number at arg 2, delay at arg 3
	TEtelLine line;
	TPtrC8 ptr8 = aRqstMsg.GetArgL(KArgNumber1);
	line = GetLine(ptr8);
	
	CReqstObjectBase* rqstObj1 = CCallNumberNotifyStatusChange::NewL(aEtel, line, aRqstMsg.GetArgL(KArgNumber2), RCall::EStatusConnected);
	RegRqstObjL(rqstObj1);
	
	CReqstObjectBase* rqstObj2 = CHangUp::NewL(aEtel, line, aRqstMsg.GetArgL(KArgNumber3));
	// Register rqst object
	RegRqstObjL(rqstObj2);
	LOGLINE1(_L("<<CCallNumberHangUpAfterConnectDoer::ConstructL"));
	}
	
//
// CAnswerNextCallDoer
//	
CAnswerNextCallDoer* CAnswerNextCallDoer::NewL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	CAnswerNextCallDoer* self = new(ELeave)CAnswerNextCallDoer();
	CleanupStack::PushL(self);
	self->ConstructL(aEtel, aRqstMsg);
	CleanupStack::Pop(self);
	return self;
	}
	
CAnswerNextCallDoer::CAnswerNextCallDoer()
	{
	}
	
CAnswerNextCallDoer::~CAnswerNextCallDoer()
	{
	}
	
void CAnswerNextCallDoer::ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	LOGLINE1(_L(">>CAnswerNextCallDoer::ConstructL"));
	// Create correct reqst object(s) to handle this : CAnswerNextCall
	// arguments expected : line at arg 1
	TEtelLine line;
	TPtrC8 ptr8 = aRqstMsg.GetArgL(KArgNumber1);
	line = GetLine(ptr8);
	CReqstObjectBase* rqstObj = CAnswerNextCall::NewL(aEtel, line);
	// Register rqst object
	RegRqstObjL(rqstObj);
	LOGLINE1(_L("<<CAnswerNextCallDoer::ConstructL"));
	}
	
//	
// CAnswerNextCallHangUpDoer
//
CAnswerNextCallHangUpDoer* CAnswerNextCallHangUpDoer::NewL( CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	CAnswerNextCallHangUpDoer* self = new(ELeave)CAnswerNextCallHangUpDoer();
	CleanupStack::PushL(self);
	self->ConstructL(aEtel, aRqstMsg);
	CleanupStack::Pop(self);
	return self;
	}
	
CAnswerNextCallHangUpDoer::CAnswerNextCallHangUpDoer()
	{
	}
	
CAnswerNextCallHangUpDoer::~CAnswerNextCallHangUpDoer()
	{
	}
	
void CAnswerNextCallHangUpDoer::ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	LOGLINE1(_L(">>CAnswerNextCallHangUpDoer::ConstructL"));
	// Create correct reqst object(s) to handle this : CAnswerNextCall, CHangUp
	// arguments expected : line at arg1, delay at arg 2
	
	TEtelLine line;
	TPtrC8 ptr8 = aRqstMsg.GetArgL(KArgNumber1);
	line = GetLine(ptr8);
	
	CReqstObjectBase* rqstObj1 = CAnswerNextCall::NewL(aEtel, line);
	RegRqstObjL(rqstObj1);
	
	CReqstObjectBase* rqstObj2 = CHangUp::NewL(aEtel, line, aRqstMsg.GetArgL(KArgNumber2));
	RegRqstObjL(rqstObj2);
	LOGLINE1(_L("<<CAnswerNextCallHangUpDoer::ConstructL"));
	}
	

//	
// CAnswerNextCallHoldDoer
//
CAnswerNextCallHoldDoer* CAnswerNextCallHoldDoer::NewL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	CAnswerNextCallHoldDoer* self = new(ELeave)CAnswerNextCallHoldDoer();
	CleanupStack::PushL(self);
	self->ConstructL(aEtel, aRqstMsg);
	CleanupStack::Pop(self);
	return self;
	}
	
CAnswerNextCallHoldDoer::CAnswerNextCallHoldDoer()
	{
	}
	
CAnswerNextCallHoldDoer::~CAnswerNextCallHoldDoer()
	{
	}
	
void CAnswerNextCallHoldDoer::ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	LOGLINE1(_L(">>CAnswerNextCallHoldDoer::ConstructL"));
	// Create correct reqst object(s) to handle this : CAnswerNextCall, CHold
	// arguments expected : line at arg1. delay at arg 2
	TEtelLine line;
	TPtrC8 ptr8 = aRqstMsg.GetArgL(KArgNumber1);
	line = GetLine(ptr8);
	
	CReqstObjectBase* rqstObj1 = CAnswerNextCall::NewL(aEtel, line);
	RegRqstObjL(rqstObj1);

	CReqstObjectBase* rqstObj2 = CHold::NewL(aEtel, line, aRqstMsg.GetArgL(KArgNumber2));
	RegRqstObjL(rqstObj2);
	LOGLINE1(_L("<<CAnswerNextCallHoldDoer::ConstructL"));
	}


//
// CHangUpNextCallBeforeConnectDoer
//
CHangUpNextCallBeforeConnectDoer* CHangUpNextCallBeforeConnectDoer::NewL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	CHangUpNextCallBeforeConnectDoer* self = new(ELeave)CHangUpNextCallBeforeConnectDoer();
	CleanupStack::PushL(self);
	self->ConstructL(aEtel, aRqstMsg);
	CleanupStack::Pop(self);
	return self;
	}
	
CHangUpNextCallBeforeConnectDoer::CHangUpNextCallBeforeConnectDoer()
	{
	}
	
CHangUpNextCallBeforeConnectDoer::~CHangUpNextCallBeforeConnectDoer()
	{
	}
	
void CHangUpNextCallBeforeConnectDoer::ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	LOGLINE1(_L(">>CHangUpNextCallBeforeConnectDoer::ConstructL"));
	// Create correct reqst object(s) to handle this : CCallNotifyStatusChangeWrapper, CHangUp
	// arguments expected : line 
	TEtelLine line;
	TPtrC8 ptr8 = aRqstMsg.GetArgL(KArgNumber1);
	line = GetLine(ptr8);
	
	CReqstObjectBase* rqstObj1 = CLineNotifyStatusChangeWrapper::NewL(aEtel, line, RCall::EStatusRinging);
	RegRqstObjL(rqstObj1);
	
	TBuf8<1> delay = _L8("0");//0 secs
	CReqstObjectBase* rqstObj2 = CHangUp::NewL(aEtel, line, delay);
	RegRqstObjL(rqstObj2);
	LOGLINE1(_L("<<CHangUpNextCallBeforeConnectDoer::ConstructL"));
	}


//
// CSendDataBackOnDataPortDoer
//
CSendDataBackOnDataPortDoer* CSendDataBackOnDataPortDoer::NewL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	CSendDataBackOnDataPortDoer* self = new(ELeave)CSendDataBackOnDataPortDoer();
	CleanupStack::PushL(self);
	self->ConstructL(aEtel, aRqstMsg);
	CleanupStack::Pop(self);
	return self;
	}
	
CSendDataBackOnDataPortDoer::CSendDataBackOnDataPortDoer()
	{
	}
	
CSendDataBackOnDataPortDoer::~CSendDataBackOnDataPortDoer()
	{
	}
	
void CSendDataBackOnDataPortDoer::ConstructL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	LOGLINE1(_L(">>CSendDataBackOnDataPortDoer::ConstructL"));
	// Create correct reqst object(s) to handle this : CSendDataBackOnDataPort
	// arguments expected : textual data to send back

	// NB. It is assumed the Etel line being used for this operation is the Data line
	CReqstObjectBase* rqstObj1 = CSendDataBackOnDataPort::NewL(aEtel, aRqstMsg.GetArgL(KArgNumber1));
	RegRqstObjL(rqstObj1);
	LOGLINE1(_L("<<CSendDataBackOnDataPortDoer::ConstructL"));
	}
	
//
// CCleanupUpDoer
//	
CCleanupUpDoer* CCleanupUpDoer::NewL(CEtelMngr& aEtel, CRpsMsg& aRqstMsg)
	{
	CCleanupUpDoer* self = new(ELeave)CCleanupUpDoer();
	CleanupStack::PushL(self);
	self->ConstructL(aEtel, aRqstMsg);
	CleanupStack::Pop(self);
	return self;
	}
	
CCleanupUpDoer::CCleanupUpDoer()
	{
	}
	
CCleanupUpDoer::~CCleanupUpDoer()
	{
	}
	
void CCleanupUpDoer::ConstructL(CEtelMngr& aEtel, CRpsMsg& /*aRqstMsg*/)
	{
	LOGLINE1(_L(">>CCleanupUpDoer::ConstructL"));
	// Hangup voice call only
	CReqstObjectBase* rqstObj;
	rqstObj  = CHangUp::NewL(aEtel, EVoiceLine, _L8(""));
	RegRqstObjL(rqstObj);

	// add more cleanup operation if require
	LOGLINE1(_L("<<CCleanupUpDoer::ConstructL"));
	}


