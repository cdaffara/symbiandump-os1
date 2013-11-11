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

/**
 @file ND_DLGSV.CPP
*/

#include <c32comm.h>
#include <e32svr.h>
#include <commsdattypesv1_1.h>
using namespace CommsDat;
#include <comms-infras/dbaccess.h>
#include "ND_DLGSL.H"
#include "ND_DLGSV.H"
#include <rsshared.h>
#include <c32root.h>
#include <es_sock.h>

/**
@internalComponent
*/


//
// CNetDialDialogServerChannelHandler
//

/*static*/ CNetDialDialogServerChannelHandler* CNetDialDialogServerChannelHandler::NewL(
		CommsFW::RCFChannel::TMsgQueues aRxQueues, CommsFW::RCFChannel::TMsgQueues aTxQueues, 
			CNetDialDialogServer& aServer)
	{
	CNetDialDialogServerChannelHandler* pHandler = new (ELeave) CNetDialDialogServerChannelHandler(aServer);
	CleanupStack::PushL(pHandler);
	pHandler->ConstructL(aRxQueues, aTxQueues);
	CleanupStack::Pop(pHandler);
	return pHandler;
	}

CNetDialDialogServerChannelHandler::CNetDialDialogServerChannelHandler(CNetDialDialogServer& aServer):
			CCFModuleChannelHandler(CActive::EPriorityStandard),iServer(aServer)
	{
	}

void CNetDialDialogServerChannelHandler::CFMessageShutdown(const CommsFW::TCFShutdownMsg& /*aMessage*/)
	{
	iServer.Shutdown();
	}

//
// CNetDialDialogServer
//
CNetDialDialogServer::CNetDialDialogServer(TInt aPriority)
	: CServer2(aPriority)
	{
	__FLOG_OPEN(KDlgSvrLogSubsys, KDlgSvrLogComponent);
    }

CNetDialDialogServer::~CNetDialDialogServer()
	{
	delete iChannel;
	__FLOG_CLOSE;
	}

CNetDialDialogServer* CNetDialDialogServer::NewL(CommsFW::RCFChannel::TMsgQueues aRxQueues, CommsFW::RCFChannel::TMsgQueues aTxQueues)
/**
Create and start a new count server.
*/
	{
	CNetDialDialogServer* pS=new(ELeave) CNetDialDialogServer(EPrioritySupervisor);
	CleanupStack::PushL(pS);
	pS->ConstructL(aRxQueues, aTxQueues);
	CleanupStack::Pop();
	return pS;
	}

void CNetDialDialogServer::ConstructL(CommsFW::RCFChannel::TMsgQueues aRxQueues, CommsFW::RCFChannel::TMsgQueues aTxQueues)
	{
	__FLOG(_L("CNetDialDialogServer::ConstructL"));
	iChannel = CNetDialDialogServerChannelHandler::NewL(aRxQueues, aTxQueues, *this);
	StartL(KCommsDialogServerName);
	}

void CNetDialDialogServer::Shutdown()
	{
	if(!Message().IsNull())
		{
		__FLOG(_L("CNetDialDialogServer::Shutdown: Completing message pending in the server."));
		Message().Complete(KErrServerTerminated);
		}
	CNetDialDialogSession* pSession;
	iSessionIter.SetToFirst();
	while((pSession=reinterpret_cast<CNetDialDialogSession*>(iSessionIter++))!=NULL)
		{
		delete pSession;
		iSessionIter.SetToFirst();
		__FLOG(_L("CNetDialDialogServer::Shutdown: Found session. Deleted."));
		}

	__FLOG(_L("CNetDialDialogServer::Shutdown: Stopping active scheduler."));
	CActiveScheduler::Stop();
	}

CSession2* CNetDialDialogServer::NewSessionL(const TVersion &aVersion, const RMessage2& /*aMessage*/) const
/**
 Create a new server session
*/
	{
	// check we're the right version
	TVersion v(KDialogServMajorVersionNumber,KDialogServMinorVersionNumber,KDialogServBuildVersionNumber);
	if (!User::QueryVersionSupported(v,aVersion))
		User::Leave(KErrNotSupported);
	return CNetDialDialogSession::NewL((CNetDialDialogServer*)this);
	}

//
// CNetDialDialogSession
//
CNetDialDialogSession* CNetDialDialogSession::NewL(CNetDialDialogServer* aServer)
/**
CNetDialDialogSession
*/
{
	CNetDialDialogSession* r=new(ELeave) CNetDialDialogSession(aServer);
	CleanupStack::PushL(r);
	r->ConstructL();
	CleanupStack::Pop();
	return r;
	}

CNetDialDialogSession::CNetDialDialogSession(CNetDialDialogServer* aServer)
	: CSession2(),
	iNetDialDialogSvr(aServer),
	iBufPtr(NULL,0),
	iPctWriteBuffer(NULL,0)
/**
Constructor.
*/
	{
	__FLOG_OPEN(KDlgSvrLogSubsys, KDlgSvrLogComponent);
	}

void CNetDialDialogSession::ConstructL()
/**
Completes construction of the CNetDialDialogSession object.
*/
	{
	iDb=CMDBSession::NewL(KCDVersion1_2);
	iBuffer=HBufC::NewL(EMaxWriteBufferSize);
	TPtr temp=iBuffer->Des();
	iBufPtr.Set(temp);
	iPctWriteBuffer.Set(temp);
	}

CNetDialDialogSession::~CNetDialDialogSession()
/**
Destructor.
*/
	{
	delete iBuffer;
	delete iDb;
	CancelEverything();
	// Just in case the cancel fails, delete the objects.
	// This is safe as the pointers are nulled after successful 
	// completition to client.
	delete iIAPSelection;
	delete iAccessPointSelection;
	delete iModemAndLocationSelection;
	delete iIAPWarning;
	delete iLogin;
	delete iAuthenticate;
	delete iReconnect;
	delete iPct;
	delete iQoSWarning;
	__FLOG_CLOSE;
	}

void CNetDialDialogSession::ServiceL(const RMessage2& aMessage)
/**
Services the specified transaction.

@param aMessage A Reference to RMessage class.
*/
	{
	TRAPD(err,DispatchMessageL(aMessage));
	if(err!=KErrNone)
		aMessage.Complete(err);
	}

void CNetDialDialogSession::DispatchMessageL(const RMessage2& aMessage)
/**
Dispatch the Message.

@param aMessage A Reference to RMessage class.
*/
	{
	switch (aMessage.Function())
        {
		case EGetIAP:
			CheckBusyL();
			iMessage=aMessage;
			iState=EGetIAP;
			GetIAPL();
			break;
		case EGetAccessPoint:
			CheckBusyL();
			iMessage=aMessage;
			iState=EGetAccessPointState;
			GetAccessPointL();
			break;
		case EGetAccessPointAP:
			CheckBusyL();
			iMessage=aMessage;
			iState=EGetAccessPointState;
			GetAccessPointAPL();
			break;
		case EGetModemAndLocation:
			CheckBusyL();
			iMessage=aMessage;
			iState=EGetModemAndLocation;
			GetModemAndLocationL();
			break;
		case EWarnNewIAP:
			CheckBusyL();
			iMessage=aMessage;
			iState=EWarnIAP;
			WarnIAPL();
			break;
		case EGetLogin:
			CheckBusyL();
			iMessage=aMessage;
			iState=ELogin;
			GetLoginL();
			break;
		case EGetAuthentication:
			CheckBusyL();
			iMessage=aMessage;
			iState=EAuthentication;
			GetAuthenticationL();
			break;
		case EGetReconnectReq:
			CheckBusyL();
			iMessage=aMessage;
			iState=EReconnectReq;
			ReconnectReqL();
			break;
		case EOpenPct:
			CheckBusyL();
			CheckPctL(aMessage);
			iMessage=aMessage;
			iState=EPct;
			OpenPctL();
			break;
		case EWritePct:
			CheckPctL(aMessage);
			iMessage=aMessage;
			iState=EPct;
			WritePct();
			break;
		case EReadPct:
			CheckPctL(aMessage);
			iReadPctMessage=aMessage;
			iState=EPct;
			ReadPctL();
			break;
		case EDestroyPctNotification:
			CheckPctL(aMessage);
			iDestroyNotificationMessage=aMessage;
			iState=EPct;
			DestroyPctNotificationL();
			break;
		case EClosePct:
			CheckPctL(aMessage);
			iMessage=aMessage;
			iState=EPct;
			ClosePct();
			break;
		case EWarnQoS:
			CheckBusyL();
			iMessage=aMessage;
			iState=EWarnQoSState;
			WarnQoSL();
			break;
		case ECancelGetIAP:
		case ECancelGetAccessPoint:
		case ECancelGetModemAndLocation:
		case ECancelWarnIAP:
		case ECancelLogin:
		case ECancelAuthenticate:
		case ECancelReconnect:
		case ECancelReadPct:
		case ECancelDestroyPctNotification:
		case ECancelWarnQoS:
			Cancel((TDialogServRqst)aMessage.Function());
			aMessage.Complete(KErrNone);
			if(!iMessage.IsNull())
				iMessage.Complete(KErrCancel);
			break;
		default:
			PanicClient(EClientBadRequest);
			break;
	        }
	}

void CNetDialDialogSession::CheckBusyL()
/**
Check whether the request is Busy.
*/
	{
	if(iBusy)
		{
		User::Leave(KErrInUse);
		}

	iBusy=ETrue;
	}

TInt CNetDialDialogSession::GeneralCallBackL(TAny* aSession)
	{
	__ASSERT_ALWAYS(aSession,PanicServer(ESvrGeneralCallBackNoSession));

	CNetDialDialogSession* self = STATIC_CAST(CNetDialDialogSession*,aSession);

	switch (self->iState)
        {
		case EGetIAP:
			self->GetIAPCompleteL(self->iIAPSelection->Status());
			break;
		case EGetModemAndLocation:
			self->GetModemAndLocationCompleteL(self->iModemAndLocationSelection->Status());
			break;
		case EWarnIAP:
			self->WarnIAPCompleteL(self->iIAPWarning->Status());
			break;
		case ELogin:
			self->CompleteLoginL(self->iLogin->Status());
			break;
		case EAuthentication:
			self->CompleteAuthentication(self->iAuthenticate->Status());
			break;
		case EReconnectReq:
			self->CompleteReconnectReq(self->iReconnect->Status());
			break;
		case EWarnQoSState:
			self->WarnQoSCompleteL(self->iQoSWarning->Status());
			break;
		case EGetAccessPointState:
			self->GetAccessPointCompleteL(self->iAccessPointSelection->Status());
			break;
		default:
			PanicServer(ESvrGeneralCallBackError);
			break;
        }

	if (self->iDb)
		{
		delete self->iDb;
		self->iDb = NULL;
		}

	return KErrNone;
	}

void CNetDialDialogSession::GetModemAndLocationL()
/**
Shows a dialog to enable the user to choose which modem and location to use for data transactions
*/
	{
	iModemAndLocationSelection = CModemAndLocationSelection::NewL(iDb,this);
	
	TCallBack callBack(GeneralCallBackL,this);
	iModemAndLocationSelection->GetModemAndLocationL(iModemId(),iLocationId(),callBack);
	}

void CNetDialDialogSession::GetModemAndLocationCompleteL(TInt aStatus)
/**
Shows a dialog to indicate whether data transcations is completed.
@param aStatus whether sucess or failure.
*/
	{
	if (aStatus == KErrNone)
		{
		iMessage.WriteL(0,iModemId);
		iMessage.WriteL(1,iLocationId);
		}

	delete iModemAndLocationSelection;
	iModemAndLocationSelection = NULL;

	iBusy=EFalse;

	iMessage.Complete(aStatus);
	}

void CNetDialDialogSession::GetIAPL()
/**
Get IAP
*/
	{
	iMessage.ReadL(1,iPrefs);

	const TInt lastError = iMessage.Int2();

	iIAPSelection=CIAPSelection::NewL(iDb,this);

	TCallBack callBack(GeneralCallBackL,this);
	iIAPSelection->GetIAPL(iIAP(),iPrefs(),lastError,callBack);
	}

void CNetDialDialogSession::GetIAPCompleteL(TInt aStatus)
	{
	__FLOG_1(_L("CNetDialDialogSession::GetIAPCompleteL - Returned %d"),aStatus);
	if (aStatus == KErrNone)
		{
		iMessage.WriteL(0,iIAP);
		}

	delete iIAPSelection;
	iIAPSelection = NULL;

	iBusy=EFalse;

	iMessage.Complete(aStatus);
	}

void CNetDialDialogSession::WarnIAPL()
/**
Warn IAP
*/
	{
	iMessage.ReadL(0,iPrefs);
	const TInt lastError = iMessage.Int1();
	iMessage.ReadL(2,iNewIAPName);

	iIAPWarning = CIAPWarning::NewL(this);

	TCallBack callBack(GeneralCallBackL,this);
	iIAPWarning->WarnIAPL(iPrefs(),lastError,iNewIAPName,iBool(),callBack);
	}

void CNetDialDialogSession::WarnIAPCompleteL(TInt aStatus)
	{
	if (aStatus == KErrNone)
		{
		iMessage.WriteL(3,iBool);
		}

	delete iIAPWarning;
	iIAPWarning = NULL;

	iBusy = EFalse;

	iMessage.Complete(aStatus);
	}

void CNetDialDialogSession::GetAccessPointL()
/**
Get AccessPoint
*/
	{
	// licensees specific implementation, this is only a reference
	ASSERT(iAccessPointSelection==NULL);
	const TInt tierId = iMessage.Int1();
	iAccessPointSelection=CAccessPointSelection::NewL(iDb,this);
	TCallBack callBack(GeneralCallBackL,this);
	iAccessPointSelection->GetAccessPointL(iAccessPoint(),tierId,callBack);
    }

void CNetDialDialogSession::GetAccessPointAPL()
/**
Get AccessPoint filtered on bearer-related info
*/
	{
    // licensees specific implementation, this is only a reference
	ASSERT(iAccessPointSelection==NULL);
	const TInt tierId = iMessage.Int1();
	iAccessPointSelection=CAccessPointSelection::NewL(iDb,this);
	TCallBack callBack(GeneralCallBackL,this);
	iAccessPointSelection->GetAccessPointL(iAccessPoint(),tierId,callBack);
	}

void CNetDialDialogSession::GetAccessPointCompleteL(TInt aStatus)
	{
	__FLOG_1(_L("CNetDialDialogSession::GetAccessPointCompleteL - Returned %d"),aStatus);
	if (aStatus == KErrNone)
		{
		iMessage.WriteL(0,iAccessPoint);
		}

	delete iAccessPointSelection;
	iAccessPointSelection = NULL;
	iBusy=EFalse;
	iMessage.Complete(aStatus);
	}

void CNetDialDialogSession::GetLoginL()
/**
Login Dialog
*/
	{
	iLogin=CLogin::NewL(this);

	iMessage.ReadL(0,iUsername);
	iMessage.ReadL(1,iPassword);

	TCallBack callBack(GeneralCallBackL,this);
	iLogin->GetUserPassL(iUsername,iPassword,callBack);
	}

void CNetDialDialogSession::CompleteLoginL(TInt aStatus)
/**
Shows the dialog to indicate the Login Complete.
*/
	{
	if(aStatus==KErrNone)
		{
		iMessage.WriteL(0,iUsername);
		iMessage.WriteL(1,iPassword);
		}

	delete iLogin;
	iLogin=NULL;
	iBusy=EFalse;

	iMessage.Complete(aStatus);
	}

void CNetDialDialogSession::GetAuthenticationL()
/**
Authentication Dialog
*/
	{
	iAuthenticate=CAuthenticate::NewL(this);

	iMessage.ReadL(0,iUsername);
	iMessage.ReadL(1,iPassword);

	TCallBack callBack(GeneralCallBackL,this);
	iAuthenticate->GetUserPassL(iUsername,iPassword,callBack);
	}

void CNetDialDialogSession::CompleteAuthentication(TInt aStatus)
/**
Complete the Authentication process.
*/
	{
	if(aStatus==KErrNone)
		{
		TInt ret = iMessage.Write(0,iUsername);
		if (ret!=KErrNone)
			PanicClient(EClientBadDescriptor);
		ret = iMessage.Write(1,iPassword);
		if (ret!=KErrNone)
			PanicClient(EClientBadDescriptor);
		}

	delete iAuthenticate;
	iAuthenticate=NULL;
	iBusy=EFalse;

	iMessage.Complete(aStatus);
	}

void CNetDialDialogSession::ReconnectReqL()
/**
Reconnection Request
*/
	{
	iReconnect=CReconnect::NewL(this);

	TCallBack callBack(GeneralCallBackL,this);
	iReconnect->ReconnectL(callBack);
	}

void CNetDialDialogSession::CompleteReconnectReq(TBool aBool)
/**
Shows Complete Reconnect Request
*/
	{
	TPckg<TBool> boolDes(aBool);
	TInt ret = iMessage.Write(0,boolDes);
	if (ret!=KErrNone)
		PanicClient(EClientBadDescriptor);

	delete iReconnect;
	iReconnect=NULL;
	iBusy=EFalse;

	iMessage.Complete(KErrNone);
	}

//
// Post Connection Terminal
//
void CNetDialDialogSession::CheckPctL(const RMessage2& aMessage)
/**
Post Connection Terminal

@param aMessage A Reference to RMessage class.
*/
	{

	if (aMessage.Function()==EOpenPct)
		{
		if (iPctOpen)
			PanicClient(EClientPctAlreadyOpen);
		}
	else
		{
		if (!iPctOpen)
			PanicClient(EClientPctNotOpen);
		if (!iBusy)
			User::Leave(KErrGeneral);
		}
	}

void CNetDialDialogSession::OpenPctL()
/**
Open the Post Connection Terminal.
*/
	{
	iPct=CPct::NewL(this);
	iPctOpen=ETrue;
	iMessage.Complete(KErrNone);
	}

void CNetDialDialogSession::WritePct()
/**
Write to Post Connection Terminal.
*/
	{
	__ASSERT_DEBUG(iPct, PanicServer(ESvrPctNotOpen));

	TPtr temp(iBufPtr);
	temp.Copy(iPctWriteBuffer);

	TBuf<EMaxWriteBufferSize> newBuffer;
	TInt ret = iMessage.Read(0,newBuffer);
	if (ret!=KErrNone)
		PanicClient(EClientBadDescriptor);
	else
		{
		if (newBuffer.Length()<=EMaxWriteBufferSize-3)
			{
			_LIT(KEllipsis,"...");
			newBuffer.Append(KEllipsis);
			}
		TInt len=temp.Length()+newBuffer.Length();
		if (len>EMaxWriteBufferSize)
			{
			temp.Copy(temp.Right(temp.Length()-(len-EMaxWriteBufferSize)));
			len=EMaxWriteBufferSize;
			}
		iPctWriteBuffer.Set((TText*)temp.Ptr(),temp.Length(),len);
		iPctWriteBuffer.Append(newBuffer);
		}

	iPct->WritePct(iPctWriteBuffer);
	iMessage.Complete(KErrNone);
	}

void CNetDialDialogSession::ReadPctL()
/**
Read from Post Connection Terminal.
*/
	{

	__ASSERT_DEBUG(iPct, PanicServer(ESvrPctNotOpen));

	iPctReadBuffer.Zero();
	iReadPctMessage.ReadL(0,iPctReadBuffer);
	
	TCallBack readPctCallBack(ReadPctCallBack,this);
	iPct->ReadPctL(iPctReadBuffer,readPctCallBack);
	iReadPctOutstanding=ETrue;
	}
	
TInt CNetDialDialogSession::ReadPctCallBack(TAny* aContext)
	{
	
	CNetDialDialogSession* This=(CNetDialDialogSession*)aContext;
	if (This)
		This->ReadPctComplete(This->iPct->Status());
	return KErrNone;
	}	
	
void CNetDialDialogSession::ReadPctComplete(TInt aStatus)
/**
Complete the Read form Post Connection Terminal.

@param aStatus if success KErrNone else any other error code.
@panic ESvrNoReadPctOutstanding if no ReadPct is outstanding.
*/
	{
	
	__ASSERT_DEBUG(iReadPctOutstanding, PanicServer(ESvrNoReadPctOutstanding));

	if(aStatus==KErrNone)
		{
		TInt pos=iPctReadBuffer.Locate('.');
		if (pos>=0)
			{
			TPtrC carriageRet(&KCarriageReturn);
			iPctReadBuffer.Insert(pos,carriageRet);
			}
		TInt ret = iReadPctMessage.Write(0,iPctReadBuffer);
		if (ret!=KErrNone)
			PanicClient(EClientBadDescriptor);
		}

	iReadPctMessage.Complete(aStatus);
	iReadPctOutstanding=EFalse;
	}
	
void CNetDialDialogSession::DestroyPctNotificationL()
/**
Destroy Pct Notification

@panic ESvrPctNotOpen if Pct is not open.
*/
	{
	__ASSERT_DEBUG(iPct, PanicServer(ESvrPctNotOpen));

	TCallBack destroyPctCallBack(DestroyPctCallBack,this);
	iPct->DestroyPctNotificationL(destroyPctCallBack);
	iDestroyPctNotfcnOutstanding = ETrue;
	}
	
TInt CNetDialDialogSession::DestroyPctCallBack(TAny* aContext)
	{
	CNetDialDialogSession* session=(CNetDialDialogSession*)aContext;
	if (session)
		session->DestroyPctComplete(session->iPct->Status());
	return KErrNone;
	}	
	
void CNetDialDialogSession::DestroyPctComplete(TInt aStatus)
/**
Destroy Pct Complete.

@param aStatus if success KErrNone else any other error code.
*/
	{
	__ASSERT_DEBUG(iDestroyPctNotfcnOutstanding, PanicServer(ESvrNoDestroyPctNotfcnOutstanding));

	iDestroyNotificationMessage.Complete(aStatus);
	iDestroyPctNotfcnOutstanding = EFalse;
	}
	
void CNetDialDialogSession::ClosePct()
/**
Close Pct.
*/
	{
	__ASSERT_DEBUG(iPct, PanicServer(ESvrPctNotOpen));
	__ASSERT_ALWAYS(!iReadPctOutstanding, PanicClient(EClientReadPctOutstandingOnClose));
	__ASSERT_ALWAYS(!iDestroyPctNotfcnOutstanding, PanicClient(EClientDestroyPctNotfcnOutstandingOnClose));

	iPctReadBuffer.Zero();
	iPctWriteBuffer.Zero();
	iPct->ClosePct();
	iBusy=EFalse;
	iPctOpen=EFalse;
	iMessage.Complete(KErrNone);
	}

void CNetDialDialogSession::WarnQoSL()
/**
Warn QoS
*/
	{
	iQoSWarning = CQoSWarning::NewL(this);

	TCallBack callBack(GeneralCallBackL,this);
	iQoSWarning->WarnQoSL(iBool(),callBack);
	}

void CNetDialDialogSession::WarnQoSCompleteL(TInt aStatus)
/**
Complete Warn QoS.

@param aStatus if success KErrNone else any other error code.
*/
	{
	if (aStatus == KErrNone)
		{
		iMessage.WriteL(0,iBool);
		}

	delete iQoSWarning;
	iQoSWarning = NULL;
	
	iBusy = EFalse;

	iMessage.Complete(aStatus);
	}

void CNetDialDialogSession::Cancel(TDialogServRqst aRequest)
/**
Call DoCancel() not Cancel() because there is no cancel for the notifier
so we must set a flag and and cancel when it next returns to the RunL()
*/
	{
	if(!iBusy)
		return;

	switch (aRequest)
		{
		case ECancelGetIAP:
			if (iState==EGetIAP)
				{
				if (iIAPSelection)
					iIAPSelection->Cancel();
				iBusy=EFalse;
				}
			break;
		case ECancelGetModemAndLocation:
			if (iState==EGetModemAndLocation)
				{
				if (iModemAndLocationSelection)
					iModemAndLocationSelection->Cancel();
				iBusy=EFalse;
				}
			break;
		case ECancelWarnIAP:
			if (iState==EWarnIAP)
				{
				if (iIAPWarning)
					iIAPWarning->Cancel();
				iBusy=EFalse;
				}
			break;
		case ECancelLogin:
			if (iState==ELogin)
				{
				if (iLogin)
					iLogin->Cancel();
				iBusy=EFalse;
				}
			break;
		case ECancelAuthenticate:
			if (iState==EAuthentication)
				{	
				if (iAuthenticate)
					iAuthenticate->Cancel();
				iBusy=EFalse;
				}
			break;
		case ECancelReconnect:
			if (iState==EReconnectReq)
				{
				if (iReconnect)
					iReconnect->Cancel();
				iBusy=EFalse;
				}
			break;
		case ECancelReadPct:
			if (iState==EPct && iPctOpen && iReadPctOutstanding)
				{
				if (iPct)
					iPct->Cancel();		// cancels read only
				iReadPctOutstanding=EFalse;
				}
			break;
		case ECancelDestroyPctNotification:
			if (iState==EPct && iDestroyPctNotfcnOutstanding)
				{
				if (iPct)
					iPct->DoNotificationCancel();
				}
			break;
		case ECancelWarnQoS:
			if(iState == EWarnQoSState)
				{
				if (iQoSWarning)
					iQoSWarning->Cancel();
				iBusy=EFalse;
				}
			break;
		case ECancelGetAccessPoint:
			if (iState==EGetAccessPoint)
				{
				if (iAccessPointSelection)
					iAccessPointSelection->Cancel();
				iBusy=EFalse;
				}
			break;
		default:
			break;
		}
	}

void CNetDialDialogSession::CancelEverything()
/**
Cancel Every Request.
*/
	{	
	switch (iState)
		{
		case EGetIAP:
			Cancel(ECancelGetIAP);
			break;
		case EGetModemAndLocation:
			Cancel(ECancelGetModemAndLocation);
			break;
		case EWarnIAP:
			Cancel(ECancelWarnIAP);
			break;
		case ELogin:
			Cancel(ECancelLogin);
			break;
		case EAuthentication:
			Cancel(ECancelAuthenticate);
			break;
		case EReconnectReq:
			Cancel(ECancelReconnect);
			break;
		case EPct:
			Cancel(ECancelReadPct);
			Cancel(ECancelDestroyPctNotification);
			break;
		case EWarnQoSState:
			Cancel(ECancelWarnQoS);
			break;
		case EGetAccessPointState:
			Cancel(ECancelGetAccessPoint);
			break;
		default:
			break;
		}
	}

void CNetDialDialogSession::PanicClient(TInt aPanic) const
/**
Panic the client
*/
	{
	_LIT(KPanicClientText,"AgentDialog Client");
	User::Panic(KPanicClientText,aPanic);
	}

EXPORT_C TInt ThreadFunction(TAny* aArg)
/**
Global functions
*/
	{
	/* Now shares heap with ESock, so we can rely on heap checking here,
	   must reply on the heap checking in ESock instead. Which is OK as
	   leaks are still detected and HookLogger can still find them. */
	//__UHEAP_MARK;

	CTrapCleanup* cleanup=NULL;
	cleanup = CTrapCleanup::New();
	CActiveScheduler* pA=NULL;
	pA = new CActiveScheduler;
	if ( pA && cleanup )
		{
		CActiveScheduler::Install(pA);

		CNetDialDialogServer* dlgServer=NULL;
		TCFModuleInfo* pParameters = (TCFModuleInfo*) aArg;
		TRAPD(ret,(dlgServer=CNetDialDialogServer::NewL(pParameters->iRxQueues, pParameters->iTxQueues)));
		
		RThread::Rendezvous(ret);
		if (ret==KErrNone)
			{
			CActiveScheduler::Start();
			}

		delete dlgServer;
		}
	else
		{
		RThread::Rendezvous(KErrNoMemory);
		}

	delete pA;
	delete cleanup;

	return KErrNone;
	}


/**
Panic the server
*/
GLDEF_C void PanicServer(TAgentDialogPanic aPanic)
	{
	User::Panic(KCommsDialogServerName, aPanic);
	}

/**
Attempt to start a thread for the dialog server in the C32 process.
*/
EXPORT_C TInt StartDialogThread()
    {
	RRootServ rootserver;
	TInt err=rootserver.Connect();
	
	if(err==KErrNone) 
		{
		// Load CNetDialDialogServer
		TCFModuleName name;
		name.Copy(KCommsDialogServerName);
		TRSStartModuleParams params(name, TFileName(_L("agentdialog.dll")), EPriorityNormal, 
									/*StackSize*/ 8192, EShareHeap, /*ThreadFunctionOrdinal*/ 29);
		params.iParams.iShareHeapWith.Copy(SOCKET_SERVER_MAIN_MODULE_NAME);
		TRequestStatus status;
		rootserver.LoadCpm(status, params, KNullDesC8);
		User::WaitForRequest(status);
		rootserver.Close();
		if(status.Int()!=KErrNone && status.Int()!=KErrRSModuleAlreadyExist)
			{
			err = status.Int();
			}
		}
	return err;
	}
