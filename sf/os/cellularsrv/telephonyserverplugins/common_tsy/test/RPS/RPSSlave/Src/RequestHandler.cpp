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

#include "RequestHandler.h"
#include "RespBuilder.h"
#include "ReqstParser.h"
#include "Listener.h"
#include "EtelMngr.h"
#include "ReqstProcessor.h"
#include "rpscommon.h"
#include "RpsSFlogger.h"
#include <e32debug.h>

CRequestHandler* CRequestHandler::NewL()
	{
	CRequestHandler* self = new(ELeave)CRequestHandler();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CRequestHandler::CRequestHandler()
	{
	}
	
CRequestHandler::~CRequestHandler()
	{
	delete iListener;
	delete iRespBuilder;
	delete iResponseBuf;
	delete iReqstProcessor;
	delete iEtelMngr;
	}
	
void CRequestHandler::ConstructL()
	{
	iRespBuilder = CRespBuilder::NewL();
	iListener = CListener::NewL(*this);
	iEtelMngr = CEtelMngr::NewL();
	iReqstProcessor = CReqstProcessor::NewL(*iEtelMngr);
	}
	
void CRequestHandler::WaitForRemoteConnectionL()
	{
	LOGLINE1(_L("CRequestHandler::WaitForRemoteConnectionL"));
	// Set Listener to wait for someone to connect to it. This blocks.
	iListener->AcceptL();
		
	CActiveScheduler::Start();
	}
	
//
// Called by Clistener when it receives a msg
//
const TDesC8& CRequestHandler::HandleRequestL(const TDesC8& aReqst)
	{
	LOGLINE1(_L(">>CRequestHandler::HandleRequestL"));
	// Defend against unsolicited/rubbish reqsts
	if(aReqst== KNullDesC8)
		return aReqst;
	
	// Request msg
	CRpsMsg* reqstMsg = CRpsMsg::NewLC();
	TInt errCode = reqstMsg->InternalizeL(aReqst);
	
	// Output some rqst details
	TBuf16<8*KMaxElementSize> fullbuf16;
	fullbuf16 = _L("Request received:");
	TBuf16<KMaxElementSize> buf16;
	for(TInt a(0); a < reqstMsg->ArraySize(); ++a)
		{
		TPtrC8 ptr8 = reqstMsg->GetRawDataL(a);
		//covert to 16 bit for display on console
		buf16.Copy(ptr8);
		fullbuf16.Append(buf16);
		fullbuf16.Append(_L(": "));
		}
	LOGLINE1(fullbuf16);
		
	// Check if this is a handshake request and get the requested data
	CDesCArrayFlat* hsResponseDataArray = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(hsResponseDataArray);
	TBool isHSReqst=EFalse;
	if(errCode == KErrNone)
		{
		isHSReqst = CHandShakeReqstParser::ParseHSReqstL(*this, *reqstMsg, hsResponseDataArray);
		}
	
	//
	// Create response msg to indicate validity of reqst and return any handshake data
	//
	TInt msgNum = reqstMsg->GetMsgNum();
	
	delete iResponseBuf;
	iResponseBuf = 0;
	
	iRespBuilder->BuildResponseL(errCode, msgNum, iResponseBuf, hsResponseDataArray);
	CleanupStack::PopAndDestroy(hsResponseDataArray);
	
	if(errCode == KErrNone && !isHSReqst)
		{
		iReqstProcessor->ProcessL(*reqstMsg);
		}
	
	CleanupStack::PopAndDestroy(reqstMsg);
	
	// Give Listener the response
	LOGLINE1(_L("<<CRequestHandler::HandleRequestL"));
	return *iResponseBuf;
	}
