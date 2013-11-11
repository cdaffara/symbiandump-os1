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

#define SYMBIAN_NETWORKING_UPS

#include <es_prot.h>
#include <comms-infras/ss_roles.h>
#include <comms-infras/ss_log.h>
#include "es_mbufif.h"
#include <ss_glob.h>
#include <ss_protprov.h>
#include <comms-infras/ss_sapshim.h>
#include <comms-infras/ss_intsock.h>
#include <in_sock.h>
#include "ss_flowrequest.h"
#include "ss_sapfactshim.h"
#include <comms-infras/ss_nodemessages_dataclient.h>
#include <comms-infras/cfmacro.h>
#include <comms-infras/ss_msgintercept.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_sock_internal.h>
#endif

#ifdef SYMBIAN_NETWORKING_UPS
#include <comms-infras/ss_platsec_apiext.h>		// MPlatSecApiExt
#endif //SYMBIAN_NETWORKING_UPS

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSocksntsck, "ESockSSocksntsck");
#endif


using namespace ESock;
using namespace Messages;
using namespace Factories;
using namespace Den;

/**
 @internalComponent
 */
const TUint KAllSelectFlags = (KSockSelectRead | KSockSelectWrite | KSockSelectExcept);

#define MSG_PRM(prmIndex)		(prmIndex)


ASocket::ASocket(TInt aSocketType)
	: iSocketType(aSocketType),
	  iIsFlowRequestPending(EFalse)
/**
 Constructor - set up default options.
 */
	{
	}

const RNodeInterface* ASocket::ServiceProvider() const
	{
	return iServiceProvider.IsOpen()? &iServiceProvider : NULL;
	}

TBool ASocket::GetFlowAndSCPR(Messages::TNodeId& aFlow, Messages::TNodeId& aSCPR) const
    {
    if (iFlowBinder)
        {
    	aFlow = iFlowBinder->Flow()->Id();
    	aSCPR = iFlowBinder->Flow()->ControlProvider().RecipientId();
    	return ETrue;
        }
    return EFalse;
    }

void ASocket::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
    (void) aRecipient;
	ESOCK_DEBUG_MESSAGE_INTERCEPT(aSender, aMessage, aRecipient);

	if (aMessage.IsMessage<TEBase::TError>())
    	{
 		// The first error would be from the flow request, any other error iIsFlowRequestPending must be false anyway
		if (FlowRequestPending())
			{
			TEBase::TError& errorMsg(static_cast<TEBase::TError&>(aMessage));
			CompleteFlowRequestMessage(errorMsg.iValue);
			SetFlowRequestPending(EFalse);
			}
 		//We are no longer needed and the client has been completed too, tear us down.
		InitiateDestruction();
		}
    else if (aMessage.IsMessage<TCFDataClient::TBindTo>())
		{
		TCFDataClient::TBindTo& bindToMsg(static_cast<TCFDataClient::TBindTo&>(aMessage));

		TInt err;
		if (IsClosing())
			{
			err = KErrAbort;
			}
		else
			{
			TRAP(err,BindToL(bindToMsg));
			}
		
		if(err == KErrNone)
		    {
		    RClientInterface::OpenPostMessageClose(Id(), aSender, TCFDataClient::TBindToComplete().CRef());
		    }
		else
		    {
		    RClientInterface::OpenPostMessageClose(Id(), aSender, TEBase::TError(aMessage.MessageId(), err).CRef());
		    }
		
		
		CompleteFlowRequestMessage(err);
		SetFlowRequestPending(EFalse);

		if ( IsClosing () )
			InitiateDestruction ();
		}
    else
        {
   		__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSocksntsck, 1));
    	}
    }

void ASocket::BindToL(const TCFDataClient::TBindTo& aBindTo)
    {
    __ASSERT_DEBUG(iSSP==NULL, User::Panic(KSpecAssert_ESockSSocksntsck, 2));

#ifdef SYMBIAN_NETWORKING_UPS
	LOG(ESockLog::Printf(_L8("CSocket %08x:\tBindToL(%08x)"), this, &aBindTo.iNodeId.Node()) );
	LOG(ESockLogExternal::Printf(KCFNodeTag, KESockFlowTag, _L8("CSocket %08x:\tSynchronous call: Sender=%08x, Recipient=%08x, Function=BindToL"), this, static_cast<Messages::ANode*>(this), &aBindTo.iNodeId.Node()) );
#endif

#if defined(__GCCXML__)
	CSubConnectionFlowBase* flow = reinterpret_cast<CSubConnectionFlowBase*>(
		reinterpret_cast<Messages::ANode*>(
			aBindTo.iNodeId.Ptr()
			)
		);
#else
	CSubConnectionFlowBase* flow = mcfnode_cast<CSubConnectionFlowBase>(
		reinterpret_cast<Messages::ANode*>(
			aBindTo.iNodeId.Ptr()
			)
		);
#endif

    if (flow==NULL)
        {
	    NM_LOG((KESockServerTag, _L8("CSocket %08x:\tSynchronous call: From=%08x To=%08x Func=BindToL Error flow is null"),
				this, static_cast<Messages::ANode*>(this), &aBindTo.iNodeId.Node()) )
        User::Leave(KErrArgument);
        }

    NM_LOG((KESockServerTag, _L8("CSocket %08x:\tSynchronous call: From=%08x To=%08x Func=BindToL"),
			this, static_cast<Messages::ANode*>(this), &aBindTo.iNodeId.Node()) )

	// Perform the binding to the flow below
	iFlowBinder = flow->GetBinderControlL();
	iServiceProvider.Open(iFlowBinder->Flow()->Id());
	iSSP = iFlowBinder->GetControlL(iProtocolInfo->iSockType, *this);
	iSSPData = iFlowBinder->BindL(*this);

	if(RequiresOwnerInfo())
		{
		CommunicateOwner();
		}

	// ask the socket provider to perform security policy checking
	TInt ret = SecurityCheck();
    NM_LOG((KESockServerTag, _L8("CSocket %08x:\tSynchronous call: From=%08x To=%08x Func=SecurityCheck Result Error Code=%d"),
			this, static_cast<Messages::ANode*>(this), &aBindTo.iNodeId.Node(), ret) )
	User::LeaveIfError(ret);
	iSSP->Start();
    }


void ASocket::AcceptSetupL(const ASocket& aParentSocket, CSubConnectionFlowBase& aFlow)
/**
Copy constructor type thing for Accept
Takes all the useful info from a parent socket, sets up the supplied SSP and
Other default values.
*/
	{
	iOptions = aParentSocket.iOptions;
	iProtocolInfo = aParentSocket.iProtocolInfo;

    __ASSERT_DEBUG(iFlowBinder==NULL, User::Panic(KSpecAssert_ESockSSocksntsck, 3));
	iFlowBinder = aFlow.GetBinderControlL();
	iSSP = iFlowBinder->GetControlL(iProtocolInfo->iSockType, *this);
	iSSPData = iFlowBinder->BindL(*this);
	iIsBound=ETrue;

	iRecBufSize=aParentSocket.iRecBufSize;
	iSendBufSize=aParentSocket.iSendBufSize;
	}

void ASocket::Create(TServerProtocolDesc *aServiceInfo)
/**
Create for all sockets.
*/
	{
	/** We create non connection orinted sockets "Open" and sockets without an ssp "NULL" */
	if (!aServiceInfo)
		{
		SetState(ESStateNull);
		UnregisterSelf();	// Null sockets need no cookie (and subsequently trying to deregister from a different Accept()ing Player is problematic and adds no benefit
		}
	else
		{
		iProtocolInfo=aServiceInfo;
		if (!IsConnectionOriented())
			{
			SetState(ESStateOpen);
			}
		}

	iOptions=KESocketDefaultOptions;
	iIsBound=EFalse;

#ifdef ESOCK_GULP
    // allocate extra memory for experimental gulp implementation
    iRecBufSize=KSocketDefaultBufferSize * 4;
#else
    iRecBufSize=KSocketDefaultBufferSize;
#endif
	iSendBufSize=KSocketDefaultBufferSize;
	}


ASocket::~ASocket()
/**
Socket destructor. Will ensure that the SSP (and any accept queue) is shutdown before destroying it.
Automatically completes all outstanding requests.

*/
	{
	iServiceProvider.Close();

	delete iDisconnectData;
	iDatagramTail.Free();	// we should normally have done this in closing
	iSendData.Free();		// we should normally have done this in closing

	if (iAcceptQ)
		{
		while (iAcceptQ->Count())
			{
			TAcceptQEntry a;
			iAcceptQ->Remove(&a);

			// the SSPs on an accept queue shouldn't have been Start()ed so we don't need to shut them down.
			// a.iSSP->DeleteMeNow();	// destruction no longer synchronous & vertical
			CTransportFlowShim* shimFlow = factoryobject_cast<CTransportFlowShim>(a.iSSP);

		 	// The SAP is no longer owned by the listener socket.
			shimFlow->iListenerControlNotify = NULL;

			shimFlow->InitDestroy();
			delete a.iConnectData;
			}
		delete iAcceptQ;
		}
	delete iNextAcceptQ;
	delete iAllocAsync;

	delete iCurrentMsg;
	delete iReadMsg;
	delete iWriteMsg;
	delete iBlockedCloseMsg;
	delete iBlockedConnectMsg;
	delete iBlockedIoctlMsg;
	delete iBlockedSetLocalNameMsg;
	}

void ASocket::InitiateDestruction()
    {
    SetClosing();

    if (iSSP)
		{
		if ((State()==ESStateOpeningActive || State()==ESStateOpeningPassive
			|| State()==ESStateOpen || State()==ESStateConnected
||  State()==ESStateBinding
			))
		    {
		    iSSP->Shutdown(MSessionControl::EImmediate);
		    }

		__ASSERT_DEBUG(iFlowBinder, User::Panic(KSpecAssert_ESockSSocksntsck, 4));
		iFlowBinder->Unbind();
		}

    /*
		CSubConnectionFlowBase* flow = iFlowBinder->Flow();
		if(!flow->HasControlPlane())
			{
			CNetworkFlow* netFlow = static_cast<CNetworkFlow*>(flow);
			delete netFlow;
			iSSP = NULL;
			delete this;
			}

		}
	else
		{
		// Either stillborn with OOM or after Shutdown() has detached
		delete this;
		}
     */
    }

void ASocket::ShutdownL(RSocket::TShutdown aType, TBool aDisconnectData)
/**
Terminate the protocol
*/
	{
	if (State()==ESStateShuttingDown)
		{
		PanicSocketClient(EShutDownTwice);
		return;
		}

	if (!CheckRunningAndSetReturn())
		{
		return;
		}

	MSessionControl::TCloseType h=MSessionControl::ENormal;
	if (SupportsGracefulClose())
		{
		switch (aType)
			{
		case RSocket::ENormal:
			CompleteWrite(KErrCancel);
			CompleteRead(KErrCancel);
			h=MSessionControl::ENormal;
			iBlockedOperations|=(EReadStopped|EWriteStopped);
			break;
		case RSocket::EStopInput:
			CompleteRead(KErrCancel);
			h=MSessionControl::EStopInput;
			iBlockedOperations|=EReadStopped;
			iDatagramTail.Free();
			break;
		case RSocket::EStopOutput:
			h=MSessionControl::EStopOutput;
			iBlockedOperations|=EWriteStopped;
			CompleteWrite(KErrCancel);
			break;
		case RSocket::EImmediate:
			h=MSessionControl::EImmediate;
			CompleteWrite(KErrCancel);
			CompleteRead(KErrCancel);
			iDatagramTail.Free();
			break;
			}
		}
	else
		{
		CompleteWrite(KErrCancel);
		CompleteRead(KErrCancel);
		h=MSessionControl::EImmediate;
		}

	CompleteConnect(KErrCancel);
	CompleteSetLocalName(KErrCancel);

	SetState(ESStateShuttingDown);

	if(h!=MSessionControl::EImmediate)
	    {
	    SetBlockedClose();
	    DontCompleteCurrentRequest();
	    }

	if (CanSendDisconnectData() && h!=MSessionControl::EImmediate && aDisconnectData)
		{
        TDes8& sendBuf = *BorrowTemporaryBufferL(iSendBufSize);
		ReadParamL(ESocketCurrentMessage,MSG_PRM(1),sendBuf);
		iSSP->Shutdown(h,sendBuf);
		}
	else
		{
		iSSP->Shutdown(h);
		}

	if (iErrorOperationMask & MSessionControlNotify::EErrorClose)
		{
		SetReturn(iError);
		ClearErrorIfNotFatal(); //Sets iError = 0;
		iErrorOperationMask = 0;
		}
	// OK to access socket because a CanClose would set the socket state to dead
	else if (h==MSessionControl::EImmediate)
		{
		// Set state to dead - let close delete the socket
		SetState(ESStateDead);
		}
	else
		{
		if (!(iOptions & KOptBlocking) && IsBlockedClose())
			{
			CompleteClose(KErrWouldBlock);
			}
		}
	}

TBool ASocket::CloseSocket()
/**
A Client has closed our RSocket - or we're being closed by the session because our client has exited
*/
	{
	SetClosing();
	TBool immediateClose = EFalse;
	switch (State())
		{
	case ESStateNull:
	case ESStateAccepting:
	case ESStateCreated:
	case ESStateError:
	case ESStateDead:
	case ESStateDisconnected:
		immediateClose = ETrue;
		break;

	case ESStateShuttingDown:
        CompleteClose(KErrCancel);
		SetReturn(KErrAlreadyExists);
		SetState(ESStateClosing);
		immediateClose = ETrue;
		break;

	case ESStateOpeningActive:
	case ESStateOpeningPassive:
	case ESStateOpen:
	case ESStateConnected:
	case ESStateBinding:
        {
		SetState(ESStateClosing);

        TBool graceful = SupportsGracefulClose() && iSSP;
		// CanClose will be called irrespective of whether the socket
		// supports graceful close or not.
		// ASocket can take care of the ownership of deletion via InitiateDestruction.
		// Also, CanClose will call InitiateDestruction if we are in ESStateClosing.
		// We already set to ESStateClosing.
	    SetBlockedClose();
	    DontCompleteCurrentRequest(); // Block closes regardless of NonBlocking mode state

        if (graceful)
           	{
	        iSSP->Shutdown(MSessionControl::ENormal);
	        }
        else
           	{
           	if (iSSP)
               	{
               	iSSP->Shutdown(MSessionControl::EImmediate);
               	}
            CanClose(MSessionControlNotify::EDelete);
            }
        }
        break;

	case ESStateClosing:
		PanicSocketClient(ECloseTwice);
		break;
		}
	return immediateClose;
	}

void ASocket::BindL()
/**
Set local address from a client request.
*/
	{
	if (!CheckRunningAndSetReturn())
		{
		return;
		}

	if (IsConnectionOriented() && iIsBound)
		{
		SetReturn(KErrAlreadyExists);
		}
	else
if (State() == ESStateBinding)
		{
		SetReturn(KErrInUse);
		}
		else
		{
		TSockAddr a;
		ReadParamL(ESocketCurrentMessage,MSG_PRM(0),a);
		
		
		// Set to blocked before call to SetLocalName. This allows for immediate callback,
		// as some conditions may not require asynchronous processing.
		SetBlockedSetLocalName();
		DontCompleteCurrentRequest();

		// Save off the state for transition later.
		iNextState = State();
		SetState(ESStateBinding);
		
		iSSP->SetLocalName(a);

		}
	}

void ASocket::AutoBind()
	{
	iSSP->AutoBind();
	iIsBound=ETrue;
	}


void ASocket::ConnectL(TBool aConnectData)
/**
Active open the socket - from a client request.
*/
	{
	TSockAddr a;
	TInt ret=KErrNone;
	switch (State())
		{
	case ESStateOpeningActive:
	case ESStateOpeningPassive:
		PanicSocketClient(EConnectingAlready);
		break;
		
		case ESStateBinding:
		SetReturn(KErrInUse);
		break;

	case ESStateConnected:
		if (IsConnectionOriented())
			{
			if(IsBlockedConnect())
				{
				PanicSocketClient(EConnectingAlready);
				}

			SetReturn(KErrAlreadyExists);
			return;
			}
		// Fall through - non connection oriented sockets can call Connect as much as they like.
	case ESStateOpen:
	case ESStateCreated:
		{
		ReadParamL(ESocketCurrentMessage,MSG_PRM(0),a);

		ret=iSSP->SetRemName(a);
		if (ret!=KErrNone)
			{
			SetReturn(ret);
			return;
			}

		if(!iIsBound)
			{
			SetBlockedConnect();
			DontCompleteCurrentRequest();
			
			if (!IsConnectionOriented())
			    {
			    iNextState = ESStateConnected;
				}
			else
				{
				iConnectData = aConnectData;
			    iNextState = ESStateOpeningActive;
				}
			SetState(ESStateBinding);
			AutoBind();

			if (!(iOptions&KOptBlocking) && IsBlockedConnect())
				{
				CompleteConnect(KErrWouldBlock);
				}
			return;
		
			}

		if (!IsConnectionOriented())
		    {
			SetState(ESStateConnected);
			TryToCompleteSelectIoctl();
		    }
		else
			{

			SetBlockedConnect();
			DontCompleteCurrentRequest();

			SetState(ESStateOpeningActive);
			
			if (CanSendConnectData() && aConnectData)
				{
				TDes8& sendBuf = *BorrowTemporaryBufferL(iSendBufSize);
				ReadParamL(ESocketCurrentMessage,MSG_PRM(1),sendBuf);
				iSSP->ActiveOpen(sendBuf);
				}
			else
				{
				iSSP->ActiveOpen();
				}

			if (!(iOptions&KOptBlocking) && IsBlockedConnect())
				{
				CompleteConnect(KErrWouldBlock);
				}
			}
		}
		break;
	case ESStateDisconnected:
	case ESStateShuttingDown:
	case ESStateClosing:
	case ESStateDead:
		SetReturn(KErrBadHandle);
		break;
	case ESStateError:
		SetReturn(iError);
		return;
	default:
        PanicSocketClient(ECannotConnect);
		//Fault(EBadState);
		break;
		}

	}

//indexes of the data elements in RMessage for Get/SetOpt
#define  OPT_NAME_INDEX		    0
#define  OPT_OPTION_INDEX		1 //for the option value
#define  OPT_OPT_LENGTH_INDEX	2
#define  OPT_OPT_LEVEL_INDEX	3

/**
    Set a socket option from a client request
*/
void ASocket::SetSockOptionL(TInt aOptionName, TInt aOptionLength, TInt aOptionLevel)
	{

	if (aOptionName & KSocketInternalOptionBit)
		{
		SetReturn(KErrAccessDenied);
		return;
		}

	if (aOptionLevel==KSOLSocket)
		{

		TBufC8<sizeof(TUint)> smallOptionBuf;
  		TPtr8 option=smallOptionBuf.Des();

		switch(aOptionName)
			{
			case KSOSendBuf:
				{
				ReadParamL(ESocketCurrentMessage,MSG_PRM(OPT_OPTION_INDEX),option);
				iSendBufSize=*(TUint*)smallOptionBuf.Ptr();

				// Even though we don't respect the sendbuf size for transfers we need to
				// be able to echo the value back to a GetOpt() - also it's used for
				// sizing the Connect/Shutdown_WithData operations
				if (iSendBufSize==KSocketBufSizeUndefined)
					{
					iSendBufSize=KSocketDefaultBufferSize;
					iOptions&=~KOptSendBufSet;
					break;
					}

				iOptions|=KOptSendBufSet;
				break;
				}
			case KSORecvBuf:
				{
	            ReadParamL(ESocketCurrentMessage,MSG_PRM(OPT_OPTION_INDEX),option);
				iRecBufSize=*(TUint*)smallOptionBuf.Ptr();

                // Even though we don't respect the recvbuf size for transfers we need to
                // be able to echo the value back to a GetOpt()
				if (iRecBufSize==KSocketBufSizeUndefined)
					{
					iOptions&=~KOptRecBufSet;
					break;
					}
				iOptions|=KOptRecBufSet;
				break;
				}

			case KSODebug:
				{
				ReadParamL(ESocketCurrentMessage,MSG_PRM(OPT_OPTION_INDEX),option);
				TBool b;
				TPtr8 p((TUint8*)&b,sizeof(iSendBufSize),sizeof(iSendBufSize));
				ReadParamL(ESocketCurrentMessage,MSG_PRM(OPT_OPTION_INDEX),p);
				if(*(TBool*)option.Ptr())
					{
					iOptions|=KOptDebug;
					}
				else
					{
					iOptions&=~KOptDebug;
					}
				break;
				}
			case KSONonBlockingIO:
				iOptions &= ~KOptBlocking;
				break;
			case KSOBlockingIO:
				iOptions |= KOptBlocking;
				break;

			//-- enable socket transfer to another process with capabilities check.
			//-- socket option must be TPckgBuf<TSecurityPolicy>, TSecurityPolicy in turn must contain the capabilities set for the
			//-- process that is going to receive this socket as a result of RSocket::Transfer().
			case    KSOEnableTransfer:
				{
				if(State() == ESStateNull || State() == ESStateAccepting)
					{
					User::Leave(KErrNotReady);
					}
			    //-- read security information (capabilities set) from the client
			    TSecurityPolicyBuf secPolicyBuf;
			    ReadParamL(ESocketCurrentMessage,MSG_PRM(OPT_OPTION_INDEX),secPolicyBuf);

			    //-- store the capabilities set for the future check in ProtocolManager::TransferSocketL()
			    iSecTransferEnabled=EFalse;
			    User::LeaveIfError(iTransferSecPolicy.Set(secPolicyBuf));
			    iSecTransferEnabled=ETrue; //-- indication that the RSocket::Transfer() security information is ready to check
				}
			break;

			//-- Disable socket transfer by indicating that the transfer security information is not valid
			case    KSODisableTransfer:
			    iSecTransferEnabled=EFalse;
			break;


			default:
				SetReturn(KErrNotSupported);
			}
		}
	else
		{
		if (CheckRunningAndSetReturn())
			{
			HBufC8* optionBuf=NULL;
			optionBuf=HBufC8::NewMaxLC(aOptionLength);
			TPtr8 option=optionBuf->Des();
			ReadParamL(ESocketCurrentMessage,MSG_PRM(OPT_OPTION_INDEX),option);
			SetReturn(iSSP->SetOption(aOptionLevel,aOptionName,option));
			CleanupStack::PopAndDestroy(optionBuf);
			}
		}
	}

void ASocket::GetSockOptionL(TInt aOptionName, TInt aOptionLength, TInt aOptionLevel, TInt aWriteBack)
/**
Read a socket option from us (or attempt the protocol if we don't support the option)

*/
	{

	if (aOptionName&KSocketInternalOptionBit)
		{
		SetReturn(KErrAccessDenied);
		return;
		}
	if (aOptionLevel==KSOLSocket)
		{
		// Most KSOLSocket options use argument1 as an out parameter
		switch(aOptionName)
			{
		case KSOSendBuf:
		case KSORecvBuf:
		case KSODebug:
		case KSONonBlockingIO:
		case KSOBlockingIO:
		case KSOSelectPoll:
			{
			if(aWriteBack)
				{
				// Retrieve the required value
				TUint optionVal;
				switch(aOptionName)
					{
				case KSOSendBuf:
					{
					optionVal =iSendBufSize;
					break;
					}
				case KSORecvBuf:
					{
					optionVal =iRecBufSize;
					break;
					}
				case KSODebug:
					{
					optionVal =(iOptions&KOptDebug);
					break;
					}
				case KSONonBlockingIO:
					{
					optionVal =!(iOptions&KOptBlocking);
					break;
					}
				case KSOBlockingIO:
					{
					optionVal =iOptions&KOptBlocking;
					break;
					}
				case KSOSelectPoll:
					{
					if(CheckRunningAndSetReturn())
						{
						optionVal =SelectConditionsReady();
						}
					else
						{
						aWriteBack = EFalse;
						}
					break;
					}
                // coverity [dead_error_begin] :
				default:
			        // the default should never be called and is there to catch coding errors.
					__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockSSocksntsck, 5));
				}
				if(aWriteBack)
					{
					TPckg <TUint> option(optionVal);
					WriteParamL(ESocketCurrentMessage,MSG_PRM(OPT_OPTION_INDEX),option);
					}
				}
			else
				{
				// Somehow they passed a NULL second argument - should we panic them?
				SetReturn(KErrArgument);
				}

			break;
			}
		case KSOSelectLastError:
			{
			SetReturn(iError);
					
			if(aWriteBack)
				{
				TPckg <TUint> option(iError);
				WriteParamL(ESocketCurrentMessage,MSG_PRM(OPT_OPTION_INDEX),option);
				}
			else
				{
				// Somehow they passed a NULL second argument - should we panic them?
				SetReturn(KErrArgument);
				}
						
		    ClearErrorIfNotFatal();
			break;
			}

		case KSOReadBytesPending:
		case KSOUrgentDataOffset:
			{
			if(!CheckRunningAndSetReturn())
				{
				break;
				}

			if(aOptionName==((TInt)KSOUrgentDataOffset) && !CanSendUrgentData())
				{
				SetReturn(KErrNotSupported);
				break;
				}
			TSockXfrLength len;
			len()=iRecOffset;
			SetReturn(iSSP->GetOption(aOptionLevel,aOptionName,len));
			if (aWriteBack)
				{
				WriteParamL(ESocketCurrentMessage,MSG_PRM(OPT_OPTION_INDEX),len);
				}
			break;
			}
		default:
			SetReturn(KErrNotSupported);
		}
		}

	else
		{
		if(CheckRunningAndSetReturn())
			{
			if (aWriteBack)
				{
				RBuf8 optionBuf;
				optionBuf.CreateMaxL(aOptionLength);
				CleanupClosePushL(optionBuf);
				TPtr8 option=optionBuf.MidTPtr(0);
				ReadParamL(ESocketCurrentMessage,MSG_PRM(OPT_OPTION_INDEX),option);
				SetReturn(iSSP->GetOption(aOptionLevel,aOptionName,option));
				WriteParamL(ESocketCurrentMessage,MSG_PRM(OPT_OPTION_INDEX),option);
				CleanupStack::PopAndDestroy(&optionBuf);
				}
			else
        		{
        		SetReturn(KErrArgument);
        		}
			}
		}
	}

void ASocket::IoctlL(TInt aOptionName, TInt aOptionLength, TInt aOptionLevel, TBool aReadOption)
/**
Perform an Ioctl from a user request
*/
	{

	if (IsBlockedIoctl())
		{
		PanicSocketClient(ETwoIoctls);
		return;
		}

	if (aOptionName&KInternalIoctlBit)
		{
		SetReturn(KErrAccessDenied);
		return;
		}

	if (aOptionLevel==KSOLSocket)
		{

		if(aOptionName==(TInt)KIOctlSelect)
		    {

			if(!CheckRunningAndSetReturn())
				{
				return;
				}
            TPckgBuf<TUint> flags;
			ReadParamL(ESocketCurrentMessage,MSG_PRM(1), flags);
			iSelectFlags=flags();
			if((iSelectFlags&KAllSelectFlags)!=0)
			    {
                SetBlockedIoctl();
		        DontCompleteCurrentRequest();
			    TryToCompleteSelectIoctl();
			    }
			else
				{
                SetReturn(KErrArgument);
                }
		    }
		else
			{
		    SetReturn(KErrNotSupported);
		    }
		}
	else
		{
//		if(!(iOptions & KOptBlocking))
//			{
//			SetReturn(KErrWouldBlock);
//			return;
//          }

//		if (CheckRunningAndSetReturn()==EFalse)
//	        return;
		if(iSSP == NULL)
			{
		    SetReturn(KErrNotSupported);
			return;
			}

		if (aReadOption)
			{
			if(aOptionLength < 0)
				{
				User::Leave(aOptionLength);
				}
			HBufC8* ioctlBuf=HBufC8::NewLC(aOptionLength);
			TPtr8 des=ioctlBuf->Des();
			ReadParamL(ESocketCurrentMessage,MSG_PRM(1),des);
			// only set ioctl blocked if successfull allocation
			SetBlockedIoctl();
			DontCompleteCurrentRequest();

			iSSP->Ioctl(aOptionLevel,aOptionName,&des);
            CleanupStack::PopAndDestroy(ioctlBuf);
			}
		else
			{
			SetBlockedIoctl();
			DontCompleteCurrentRequest();
			iSSP->Ioctl(aOptionLevel,aOptionName,NULL);
			}
		}
	LOG(
		if (IsBlockedIoctl())
			{
			ESockLog::Printf(KESockSessDetailTag, _L8("ASocket::IoctlL blocking") );
			}
		)
	}

void ASocket::CancelIoctl()
/**
Cancel a pending Ioctl and notify the protocol.
*/
	{
	if(IsBlockedIoctl())
		{
		TInt optionLevel = 0;
		TInt optionName = 0;
		if(!iSelectFlags)
			{
			optionLevel = iBlockedIoctlMsg->ReadInt(2);
			optionName = iBlockedIoctlMsg->ReadInt(0);
			}

		CompleteIoctl(KErrCancel);

		if(iSelectFlags)
			{
		    iSelectFlags=0;
			}
		else
			{
			__ASSERT_ALWAYS(iSSP!=NULL, Panic(ENullSap));
		    iSSP->CancelIoctl(optionLevel, optionName);
			}
		}
	}

void ASocket::GetDisconnectDataL( ) const
/**
Get disconnect data if any is present on the socket.
*/
	{
	if (CanSendDisconnectData())
		{
		if(iDisconnectDataError!=KErrNone)
			{
			SetReturn(iDisconnectDataError);
			}
		else if (iDisconnectData)
			{
			TPtr8 des=iDisconnectData->Des();
			const_cast<ASocket*>(this)->WriteParamL(ESocketCurrentMessage,MSG_PRM(0),des);
			}
		else
			{
			SetReturn(KErrNotFound);
			}
		}
	else
		{
		SetReturn(KErrNotSupported);
		}
	}

static const TInt KPaddingForHeaders = 48;

void ASocket::SendL(TInt aXferLenArg, TInt aAddrArg, TInt aSendByteCount, TInt aSendFlags, TBool aUseMBufs)
/**
 Common message service for Send, SendTo and Write.
*/
	{
	if (IsBlockedWrite())
		{
		PanicSocketClient(EWritingAlready);
		return;
		}

	if (!CheckRunningAndSetReturn())
		{
		return;
		}

	// Judge If the socket is connected during a connection-oriented service.
	if (IsConnectionOriented() && iState != ESStateConnected)
		{
		LOG( ESockLog::Printf(_L("ASocket %08x SendL() KErrNotReady"), this ) );
		SetReturn(KErrNotReady);
		return;
		}

	if(iBlockedOperations & EWriteStopped)
		{
		if(aXferLenArg >= 0)
			{
			iXferLength() = 0;
			WriteParamL(ESocketCurrentMessage, aXferLenArg, iXferLength);
			}
		SetReturn(KErrEof);
		return;
		}

	// Set whether we are sending RMbufChain or not
	iSendUseMBufs = aUseMBufs;

	iSendByteCount=aSendByteCount;
	iSendXferLenIdx = aXferLenArg;
	iSendToAddrIdx = aAddrArg;
    if (iSendToAddrIdx >= 0)
        {
        if(IsConnectionOriented())
            {
            PanicSocketClient(ECantSendToOnConnection);
            return;
            }
        }
    else    // Not ESendTo...
        {
        if(State() != ESStateConnected  && State() != ESStateShuttingDown && iSendToAddrIdx != ASocket::KWriteNoAddrArg)
            {
            LOG( ESockLog::Printf(_L("ASocket %08x SendL() 2 KErrNotReady"), this ) );
            SetReturn(KErrNotReady);
            return;
            }
        }
	iSendFlags = aSendFlags;
	if(iSendFlags)
	    {
        if(iSendFlags&KSocketInternalWriteBit)
            {
            SetReturn(KErrAccessDenied);
            return;
            }
        if ((iSendFlags&KSockWriteUrgent) && !CanSendUrgentData())
            {
            SetReturn(KErrNotSupported);
            return;
            }
	    }
	__ASSERT_DEBUG(iSendData.IsEmpty(), Fault(EBufsLeftInSendData));	// no pathway should leave this populated after Write completes
	iSendData.Free();	// but paranoia is cheap 

	if(iSendByteCount<0)
		{
		PanicSocketClient(EBadDescriptorLength);
		return;
		}
	
#ifdef SYMBIAN_NETWORKING_PERFMETRICS
	IncludePerformanceData(-1, -1, iSendByteCount);
#endif

	if (iIsBound==EFalse)
		{

		if (State() == ESStateBinding)
			{
			SetReturn(KErrInUse);
			return;
			}
		AutoBind();
		}
	iSendOffset=0;
	DoSend(ETrue);
	}

void ASocket::DoSend(TBool aInitialRequest)
	{
	AMessage* msg = aInitialRequest? iCurrentMsg: iWriteMsg;
	ASSERT(msg);
	
	TInt ret = KErrNone;
	TBool completeReq = ETrue;
	
	if (IsStream())
		{
		ret = FillStreamProtocol(msg);

		if(iSendByteCount > 0)
			{
			// Still data to write - flow-off or error
			if(ret != KErrNone)
				{
				if(ret == KErrNoMBufs)
					{
					ret = RequestAsyncMBufAllocation(CWaitForMBufs::ECanSend, iSendByteCount);
					}
				if(ret != KErrNone)
					{
					ret = KErrNoMemory;
					}
				}
			if(ret == KErrNone)
			    {
                if(iOptions & KOptBlocking)
                    {
                    completeReq = EFalse;
                    }
                else if(iBlockedOperations & EWriteFlowedOff)
                    {
                    //it should return KErrWouldBlock instead of KErrNone
                    //but it returns KErrNone for backward compatibility.
                    iSendData.Free();
                    }
			    }
			}
		}
	else	// !IsStream()
		{
		if (aInitialRequest && ((iProtocolInfo->iMessageSize!=KSocketMessageSizeNoLimit) && (iProtocolInfo->iMessageSize!=KSocketMessageSizeUndefined) && (iSendByteCount>iProtocolInfo->iMessageSize)) ||
					((iOptions&KOptSendBufSet) && iSendByteCount>iSendBufSize))
			{
			SetReturn(KErrTooBig);
			return;
			}

		RMBufChain data;
	   	TInt nwr = KErrNone;
		TInt actualSendLen = 0;		// total size of data accepted by protocol upon datagram write
		TInt actualBufSize = 0;
		if(aInitialRequest || iSendData.IsEmpty())
			{
			if (iSendUseMBufs)
				{
				ret = msg->ReadMBuf(MSG_PRM(2),data);
				actualSendLen = data.Length();
				actualBufSize = data.First()->Size();
				}
			else
				{
	 			// Possible outcomes:
	 			//	(1) We fail to create the MBuf chain with the client data:
	 			//		(a) with KErrNoMbufs - we start an asynchronous allocation and treat it as a flow-off
	 			//		(b) with some other -ve error code - we treat this as KErrNoMemory and complete the client
	 			//	(2) SAP fails to write the mbuf chain:
	 			//		(a) with KErrNoMbufs - we start an asynchronous allocation on its behalf and treat it as a flow-off
	 			//		(b) with zero, ie flow-off: we don't complete client
	 			//		(c) with some other -ve error code - we treat this as KErrNoMemory and complete the client
	 			//	(3) SAP write succeeds and returns > 1 - we complete the client with KErrNone
				// get MBuf chain immediately, copy avoiding iSendBuf if possible

				ret = data.Alloc(iSendByteCount + KPaddingForHeaders, iAllocator);
				if(ret == KErrNone)
					{
					RMBuf* first = data.First();
					if(first->Size() >= (iSendByteCount + KPaddingForHeaders))
						{
						// Read directly into buffer
						first->SetData(KPaddingForHeaders, iSendByteCount);
					   	TPtr8 des(NULL, 0);
						des.Set(first->Ptr(), iSendByteCount, iSendByteCount);
						ret = msg->ReadDes(MSG_PRM(2),des);
						actualSendLen = des.Length();
						actualBufSize = actualSendLen;
						}
					else
						{
						// Have to use local descriptor to avoid multiple IPC round trips
						TDes8* sendBuf = BorrowTemporaryBuffer(iSendByteCount);
						if(!sendBuf)
							{
							// We can't allocate a buffer as big as the client so risk losing data
							// they've indicated as precious if it overflows. Failing immediately
							// without trying is the compatible option
							ret = KErrNoMemory;
							}
						else
							{
							if(first->Length() > KPaddingForHeaders)
								{
								first->AdjustStart(KPaddingForHeaders);
								}
							else
								{
								data.TrimStart(KPaddingForHeaders);
								}
							ret = msg->ReadDes(MSG_PRM(2), *sendBuf);	// read failure exceedingly rare so don't bother checking return yet
							actualSendLen = sendBuf->Length();
							actualBufSize = actualSendLen;
							data.CopyIn(*sendBuf);
							}
						}
					}
				}
			}
		else
			{
			// Resume with the RMBufChain we already fabricated 
			data.Assign(iSendData);
			if (iSendUseMBufs)
				{
				actualSendLen = data.Length();
				actualBufSize = data.First()->Size();
				}
			else
				{
				actualSendLen = data.Length();
				actualBufSize = actualSendLen;
				}
			}

		if (ret == KErrNone)
			{
			// Clear the error operation mask.  If a protocol errors the socket whilst in the
			// context of the CServProviderBase::Write(...) down call, we will see it below.
			iErrorOperationMask = 0;
			if (iSendToAddrIdx < 0)
				{
				nwr = iSSPData->Write(data, iSendFlags, NULL);
				}
			else
				{
				TSockAddr addr;
				ret = msg->ReadDes(iSendToAddrIdx, addr);
				if(ret == KErrNone)
					{
					nwr = iSSPData->Write(data, iSendFlags, &addr);
					}
				}

			if (nwr == 0)
				{
				// Flow unable to accept data but not erroring
                // WriteFlowedOff
                if(iOptions & KOptBlocking)
                    {
                    completeReq = EFalse;
                    iSendData.Assign(data);
                    }
				}

			if (iErrorOperationMask & (MSessionControlNotify::EErrorSend))
				{
				// Protocol has errored the socket.  Complete the current operation with the error.
				// If the error is not fatal, then it is assumed to be for the current operation only,
				// so clear down the error.
				nwr = iError;
				completeReq = ETrue;
				ClearErrorIfNotFatal(); //Sets iError = 0;
				iErrorOperationMask = 0;
				iSendData.Free();
				}

			if(nwr < 0)
				{
				ret = nwr;
				}
			else if(iSendXferLenIdx >= 0)
				{
				iSendOffset = actualSendLen - data.Length();
				}
			if (!iSendUseMBufs)
				{
				// nwr > 0 == Sending using RMBufChain where we have to ensure the data is
				// freed once the Send is succeeded
				// Protocols must remove buffers they want to use
				data.Free();
				}
			else
				{
				if (nwr > 0)
					{
					msg->InitMBuf(MSG_PRM(2));
					}
				}
			}
		if(ret != KErrNone)
			{
			if(ret == KErrNoMBufs)
				{
				ret = RequestAsyncMBufAllocation(CWaitForMBufs::ECanSend, actualBufSize);
				if(ret != KErrNone)
					{
					ret = KErrNoMemory;
					}
				}
			if(ret == KErrNone)
				{
				if(iOptions & KOptBlocking)
					{
					completeReq = EFalse;
					}
				else
					{
					ret = KErrWouldBlock;
					}
				}
			}
		}

	if(completeReq)
		{
		// Write completed
		if(iSendXferLenIdx >= 0)
			{
			iXferLength() = iSendOffset;
			TInt lenRet = msg->WriteDes(iSendXferLenIdx, iXferLength);
			if(lenRet != KErrNone && ret == KErrNone)
				{
				ret = lenRet;
				}
			}
		// KErrBadDescriptor means that the client has already been panicked; we must
		// avoid further completion of this message 
		if(ret == KErrBadDescriptor)
			{
			iBlockedOperations&=~EBlockedWrite;
			}
		else
			{
			if(aInitialRequest)
				{
				SetReturn(ret);
				}
			else
				{
				CompleteWrite(ret);
				}
			}
		}
	else
		{
		if(aInitialRequest)
			{
			// Doesn't matter if the client has already been panicked; we're acquiring the now-NULL msg
			iWriteMsg->AcquireMessage(msg);
			SetBlockedWrite();
			iBlockedOperations|=EWriteFlowedOff;
			DontCompleteCurrentRequest();
			}
		}
	}

void ASocket::RecvL(TInt aXferLenArg, TInt aAddrArg, TInt aReadByteCount, TInt aReadFlags, TBool aUseMBufs, TInt aRecvOneOrMore)
/**
 Common service routine from Recv, RecvFrom and Read.
*/
	{
	if (IsBlockedRead())
		{
		PanicSocketClient(EReadingAlready);
		return;
		}

	if (!CheckRunningAndSetReturn())
		{
		return;
		}

	if (IsConnectionOriented())
		{
		if (State()!=ESStateConnected && State()!=ESStateShuttingDown)
			{
			LOG( ESockLog::Printf(_L("ASocket %08x RecvL() KErrNotReady"), this ) );
			SetReturn(KErrNotReady);
			return;
			}
		if (iIsBound==EFalse)
			{
			if (State() == ESStateBinding)
    			{
    			SetReturn(KErrInUse);
    			return;
				}
			AutoBind();
			}
		if (aAddrArg >= 0)
			{
			SetReturn(KErrNotSupported);
			return;
			}
		}

	iRecvOneOrMore = aRecvOneOrMore;
	if(iRecvOneOrMore && !IsStream())
		{
		SetReturn(KErrNotSupported);
		return;
		}

	// Set whether we are going to receive in RMBufChain way
	iRecvUseMBufs = aUseMBufs;

	iRecByteCount=aReadByteCount;
    if(iRecByteCount<0)
        {
        PanicSocketClient(EBadDescriptorLength);
        User::Leave(KErrBadDescriptor);
        }
	iRecvFlags=aReadFlags;
	if(iRecvFlags)
	    {
        if(iRecvFlags&KSocketInternalReadBit)
            {
            SetReturn(KErrAccessDenied);
            return;
            }
        if (iRecvFlags&KSockReadPeek && !SupportsPeek())
            {
            SetReturn(KErrNotSupported);
            return;
            }
	    }
	iRecvFromAddrIdx = aAddrArg;
	iRecvXferLenIdx=aXferLenArg;

#ifdef SYMBIAN_NETWORKING_PERFMETRICS
	IncludePerformanceData(-1, iRecByteCount, -1);
#endif

	iRecOffset=0;
	DoRecv(ETrue);
	}

void ASocket::DoRecv(TBool aInitialRequest)
	{
	AMessage* msg = aInitialRequest? iCurrentMsg: iReadMsg;

	if(aInitialRequest || IsBlockedRead())
		{
		TInt errReadProtocol = KErrNone;
		TBool completeReq = ETrue;
	
		if((iBlockedOperations&EReadStopped) && iDataAvailable<=0)
			{
			TInt ret = msg->WriteDes(MSG_PRM(2), KNullDesC8);
			iRecOffset = 0;
			errReadProtocol = KErrEof;
			}
		else if (IsStream())
		    {
			errReadProtocol = DrainStreamProtocol(msg);
			if(errReadProtocol == KErrNone)
			    {
				if(iRecvOneOrMore && iRecOffset > 0) // i.e. data received
                    {
                    iRecByteCount=0;
                    }
				if (iRecByteCount>0) // Still data expected
                    {
                    if(!(iBlockedOperations&EReadStopped) || iDataAvailable>0)
                        {
                        if ((iOptions&KOptBlocking))
                            {
			    // This error condition can never be reached under the current structure. Further investigation into how other teams,
			    // such as Bluetooth, handle memory errors in this situation is needed before restructuring this method.

                            //if(errReadProtocol == KErrNoMBufs)
                            //    {
                            //    // Block unless request fails
							//	errReadProtocol = RequestAsyncMBufAllocation(CWaitForMBufs::ENewData, iRecByteCount/*iRecBuf.MaxLength()*/) == KErrNone;
							//	completeReq = errReadProtocol == KErrNone;
                            //    }
                            //else
                            if(iError != KErrNone)
                                {
                                // Don't block as protocol is already erroring client
								errReadProtocol = iError;
                                // If the socket error is not fatal, then reset it so that only the current
                                // receive operation is completed with the error (not every subsequent one).
                                ClearErrorIfNotFatal();
                                }
                            else
                                {
                                completeReq = EFalse;
                                }
                            }
                        else
                            {
                            errReadProtocol = KErrWouldBlock;
                            }
                        }
					else
						{
						errReadProtocol = KErrEof;
						}
                    }
			    }
		    }
		else
			{	// !IsStream()

			// If the previous call to Recv used the KSockReadContinuation method and left iDatagramTail empty (everything
			// up to the end of the packet was read) then tailContinuation will be false this time round - this will cause
			// the reading of new data from iSSP below.
			TBool tailContinuation = (iRecvFlags & KSockReadContinuation) != 0 && !iDatagramTail.IsEmpty();
			TBool needToBlock = !tailContinuation && iDataAvailable <= 0;
			errReadProtocol = KErrNone;
	
			if(aInitialRequest)
				{
				//-- if we have a read from the socket with flag  KSockReadPeek set
				//-- and if we need to block, set the NewData() recursion counter to 1.
				//-- otherwise, reset it, it will not be checked then.
				if(needToBlock && (iRecvFlags & KSockReadPeek))
					{
					iBlockOnPeekReadCnt = 1;
					}
				else
					{
					iBlockOnPeekReadCnt = 0;
					}
				}	
	
			if (!needToBlock)
				{
				TDes8* recBuf = BorrowTemporaryBuffer(iRecByteCount);
				if(!recBuf)
					{
					// We can't allocate a buffer as big as the client so risk losing data
					// they've indicated as precious if it overflows. Failing immediately
					// without trying is the compatible option
					errReadProtocol = KErrNoMemory;
					}
				// Only read from the provider if we can't use the previous tail
				else if(!tailContinuation)
					{
					// Free it only if the chain is empty. For RMBufChain read, the receiveer must have been freed it.
					// and we don't need to free it
					if(!iDatagramTail.IsEmpty() && !iRecvUseMBufs)
						{
						iDatagramTail.Free();
						}
					if (iRecvFromAddrIdx < 0)
						{
						errReadProtocol = iSSPData->GetData(iDatagramTail, iRecByteCount | KGetDataWholeDatagram, iRecvFlags);
						}
					else
						{
						errReadProtocol = iSSPData->GetData(iDatagramTail, iRecByteCount | KGetDataWholeDatagram, iRecvFlags, &iRecvFromAddr);
						}
					}
				// Protocols must either deliver the request or return an error and deliver nothing
				//__ASSERT_DEBUG( (errReadProtocol < 0 && data.IsEmpty()), Panic(EBadDataCount));
				TInt trimTailAmount = 0;
				if(errReadProtocol < 0)
					{
					// Protocol has failed to deliver, so we'll have to block. If it's a KErrNoMBufs (a likely case) then we kick off
					// an asynch allocation for amount and upon its completion try again. Of course we cannot know whether this is the
					// number of mbufs that the protocol wanted but it's a good guess. If it's some other error then it's up to the
					// protocol to tell us when to retry (by signalling with NewData(0)). If it was some error that the protocol
					// couldn't handle at all then it should have set the error upon the socket rather than expecting us to magically
					// "do the right thing"
					needToBlock = ETrue;
					}
				else
					{
					if (iRecvUseMBufs)
						{
						// We don't trim anything, when we do a RMBufChain read with KSockReadPeek is set
						// KSockReadPeek is upto the SAP shim
						errReadProtocol = msg->WriteMBuf(MSG_PRM(2),iDatagramTail);
						if(errReadProtocol == KErrNone)
							{
							iDatagramTail.Init();
							// Just remove the chain that we have given back from the tail. When using MBufs the
							// client always gets the entire datagram anyway (the datagram tail mechanism is there
							// just so someone using a descriptor can do a continuation read if it overflows, so they
							// don't have to use 64k buffer just in case some loony soaks them with a jumbogram).
							// So there's no trimming done & no need to discriminate between peek & normal.
							}
						}
					else
						{
						TPtr8 des(recBuf->MidTPtr(0, iRecByteCount));
	
						if (!iDatagramTail.IsEmpty())
							{
							// intermediate descriptor copy
							RMBuf* first = iDatagramTail.First();
							if(!first->Next())
								{
								TInt copyLen = Min(first->Length(), iRecByteCount);
								des.Set(first->Ptr(), copyLen, copyLen);
								}
							else
								{
								iDatagramTail.CopyOut(des, 0);
								}
							}
						else
							{
							des.SetLength(0);
							}
						errReadProtocol = msg->WriteDes(MSG_PRM(2),des);
						if((errReadProtocol == KErrNone && iRecvFlags & KSockReadPeek) == 0)
							{
							// Note the amount of data to trim after writing it to the client
							trimTailAmount = des.Length();
							}
						}
	
	#ifdef SYMBIAN_NETWORKING_PERFMETRICS
					IncludePerformanceData(trimTailAmount, -1, -1);
	#endif
	
					if(errReadProtocol == KErrNone)
						{
						if (trimTailAmount != 0)
							{
							iDatagramTail.TrimStart(trimTailAmount);
							}
						if(iRecvXferLenIdx >= 0)
							{
							// Number of bytes remaining. (store for later write)
							iRecOffset = iDatagramTail.Length();
							}
						if(iRecvFromAddrIdx >= 0)
							{
							errReadProtocol = msg->WriteDes(iRecvFromAddrIdx, iRecvFromAddr);
							}
						if(errReadProtocol == KErrNone && !tailContinuation)
							{
							// When using a non-stream based socket the value iDataAvailable refers to the number of packets available.
							// In the case where tail continuation is in use the packet may not fully have been dealt with yet (more
							// could be read from it next time) and so the value should not be decremented yet. This is the case even
							// if all the data up to the end of the packet has been read (the above call to TrimStart will leave
							// iDatagramTail empty and then on the next call of this method tailContinuation will be set to false).
							iDataAvailable--;
							}
						}
					}
				}
	
			if(needToBlock)
				{
				if (iOptions & KOptBlocking)
					{
					if (errReadProtocol == KErrNoMBufs)
						{
						// Block unless request fails
						needToBlock = RequestAsyncMBufAllocation(CWaitForMBufs::ENewData, iRecByteCount) == KErrNone;
						}
					else if (iError != KErrNone)
                        {
                        // Don't block as protocol is already erroring client
                        needToBlock = EFalse;
                        errReadProtocol = iError;
                        // If the socket error is not fatal, then reset it so that only the current
                        // receive operation is completed with the error (not every subsequent one).
                        ClearErrorIfNotFatal();
                        }
                    else if (errReadProtocol != KErrNone)
                        {
                        needToBlock = EFalse;
						}
					completeReq = !needToBlock;
					}
				else
					{
					errReadProtocol = KErrWouldBlock;
					}
				}
			}
		
		if(completeReq)
			{
			// Read completed
			if(iRecvXferLenIdx >= 0)
				{
				iXferLength() = iRecOffset;
				TInt ret = msg->WriteDes(iRecvXferLenIdx, iXferLength);
				if(ret != KErrNone && errReadProtocol == KErrNone)
					{
					errReadProtocol = ret;
					}
				}
			// KErrBadDescriptor means that the client has already been panicked; we must
			// avoid further completion of this message 
			if(errReadProtocol == KErrBadDescriptor)
				{
				iBlockedOperations&=~EBlockedRead;
				}
			else
				{
				if(aInitialRequest)
					{
					SetReturn(errReadProtocol);
					}
				else
					{
					CompleteRead(errReadProtocol);
					}
				}
			}
		else
			{
			if(aInitialRequest)
				{
				// Doesn't matter if the client has already been panicked; we're acquiring the now-NULL msg
				iReadMsg->AcquireMessage(msg);
				SetBlockedRead();
				DontCompleteCurrentRequest();
				}
			}
		}
    TryToCompleteSelectIoctl();
	}

void ASocket::ListenL(TInt aQlen, TBool aConnectionData)
/**
Listen request service
Listen is a dead and, clients can't do anything with the socket other than Accept after a listen.
*/
	{

	switch (State())
		{
	case ESStateConnected:
	case ESStateOpeningActive:
	case ESStateBinding:
		SetReturn(KErrInUse);
		return;
	case ESStateOpeningPassive:
		PanicSocketClient(EAlreadyListening);
		//lint -fallthrough
	case ESStateShuttingDown:
	case ESStateClosing:
	case ESStateDead:
		SetReturn(KErrBadHandle);
		return;
	case ESStateDisconnected:
		SetReturn(KErrDisconnected);
		return;
	case ESStateError:
		SetReturn(iError);
		return;
	case ESStateNull:
	case ESStateAccepting:
		PanicSocketClient(ESockBadHandle);
		return;
	case ESStateCreated:
	case ESStateOpen:
	default:
		break;
		}

	if (!IsConnectionOriented())
		{
		PanicSocketClient(EListenNeedsConnection);
		return;
		}

	if (!iIsBound)
		{
		SetReturn(KErrBadName);
		return;
		}

	if(!iAcceptQ)
		{
		iAcceptQ = new (ELeave) CCirBuf<TAcceptQEntry>;
		}
	CleanupStack::PushL(iAcceptQ);
	iAcceptQ->SetLengthL(aQlen);

	// Create a second queue we can transfer the contents to if we need to remove an SAP
	// from the middle of the ring buffer.
	if(!iNextAcceptQ)
		{
		iNextAcceptQ = new (ELeave) CCirBuf<TAcceptQEntry>;
		}
	CleanupStack::PushL(iNextAcceptQ);
	iNextAcceptQ->SetLengthL(aQlen);

	TInt ret;
	if (aConnectionData)
		{
		TDes8& xferBuf = *BorrowTemporaryBufferL(iSendBufSize);
		xferBuf.SetLength(iSendBufSize);
		ReadParamL(ESocketCurrentMessage, MSG_PRM(1), xferBuf);
		ret=iSSP->PassiveOpen(aQlen, xferBuf);
		}
	else
		{
		ret=iSSP->PassiveOpen(aQlen);
		}

	if (ret==KErrNone)
		{
		SetState(ESStateOpeningPassive);
		}
	else
		{
		SetReturn(ret);
		}

	CleanupStack::Pop(iNextAcceptQ);
	CleanupStack::Pop(iAcceptQ);
	}

void ASocket::Accept()
/**
Accept a connection on a listening socket.
*/
	{

	if ( State()!=ESStateOpeningPassive ) // protocol may have generated an error on the accept
	    {                    // SAP for some bizzare reason. We can't test just (State()==ESStateError)
		                     // 'cos if reconnection iState could be whatever (see ASocket::Error)
                             //so check if we'r listenning at all
        SetReturn(iError != KErrNone ? iError : KErrNotReady);
		return;
	    }

	if ( !iAcceptQ || !iNextAcceptQ )
		{
		PanicSocketClient(ENotListening);
		return;
		}

	if (IsBlockedConnect())
		{
		PanicSocketClient(EAcceptTwice);
		return;
		}


	ASocket* acceptingSocket = InitiateAcceptingSocket();

	if ( !acceptingSocket ) // The acceptingSocket will be NULL if we got a panic. return here.
		return;

	if (!iAcceptQ->Count())
		{
		if (!(iOptions & KOptBlocking))
			{
			SetReturn(KErrWouldBlock);
			return;
			}

		SetBlockedConnect();
		DontCompleteCurrentRequest();
		return;
		}

	// If the queue isn't empty setting the blocked connect and calling DoCompleteAccept will work it's magic - this saves us code
	SetBlockedConnect();
	DoCompleteAccept();
	DontCompleteCurrentRequest();	// Don't want to complete twice.
	}

void ASocket::LocalNameL()
/**
Get local name for a client request
*/
	{

	if (!CheckRunningAndSetReturn())
		{
		return;
		}

	if (!iIsBound)
		{
		SetReturn(KErrNotFound);
		return;
		}
	else
		{
		TSockAddr addr;
		iSSP->LocalName(addr);
		WriteParamL(ESocketCurrentMessage,MSG_PRM(0),addr);
		}

	}

void ASocket::RemoteNameL()
/**
Get remote name for a cleient request
*/
	{

	if (!CheckRunningAndSetReturn())
		{
		return;
		}

	if (State()==ESStateConnected || State()==ESStateOpeningActive || State()==ESStateShuttingDown)
		{
		TSockAddr addr;
		iSSP->RemName(addr);
		WriteParamL(ESocketCurrentMessage,MSG_PRM(0),addr);
		}
	else
		{
		SetReturn(KErrNotFound);
		return;
		}
	}

void ASocket::NewData(TUint aCount)
/**
Called from a protocol to indicate that new data is available for reading

*/
	{
	__ASSERT_DEBUG(State()!=ESStateDead,Panic(EBadStateUpCall));
//	__ASSERT_DEBUG(State()!=ESStateCreated,Panic(EBadStateUpCall));		// no longer forced into shuttingdown state upon half-close
	__ASSERT_DEBUG(State()!=ESStateClosing,Panic(EBadStateUpCall));
	__ASSERT_DEBUG(!(iBlockedOperations&EReadStopped),Panic(EBadStateUpCall));

	// The socket can now be errored from the control side as well as from the protocol below.  Hence, the socket
	// can receive an upcall while in the errored state as the protocol is unaware of the socket errored state.
	// Specific scenario is that TransportFlowShim receives a DataClientStop(KErrForceDisconnect), places the socket
	// in the errored state and, subsequently, the TCP/IP stack issues NewData(KNewDataEndofData) as a result of an
	// incoming FIN.
	//__ASSERT_DEBUG(State()!=ESStateError,Panic(EBadStateUpCall));

	__ASSERT_DEBUG(State()!=ESStateDisconnected,Panic(EBadStateUpCall));
// zero is used in v1.5 to resume after	reneging __ASSERT_DEBUG(aCount>0,Panic(EBadDataCount));

	if(aCount==KNewDataEndofData)
		{
		iBlockedOperations |= EReadStopped;
		}
	else
		{
		iDataAvailable+=aCount;
		__ASSERT_DEBUG(iDataAvailable>=0,Panic(EBadDataCount));
		}

    //-- check the recursion level counter.
    //-- if it is 0, there were no recursion entry to itself, continue.
    //-- if it is 1, there was blocked read from socket with KSockReadPeek set, increase it and continue.
    //-- if it is 2, it means that function has been called from itself via NewData()-> [iSSP->CetData()] -> NewData()
    //-- return to stop the infinite recursion.
    if(iBlockOnPeekReadCnt >0)
        {
        if(iBlockOnPeekReadCnt >= 2)
            {
            iBlockOnPeekReadCnt = 0;
            return;
            }
        else
            {
            iBlockOnPeekReadCnt ++;
            }
	    }

    DoRecv(EFalse);
	}

void ASocket::CanSend()
/**
Called from protocol to indicate new buffer space has become available
*/
	{
	__ASSERT_DEBUG(State()!=ESStateDead,Panic(EBadStateUpCall));
	__ASSERT_DEBUG(State()!=ESStateCreated,Panic(EBadStateUpCall));
	__ASSERT_DEBUG(State()!=ESStateClosing,Panic(EBadStateUpCall));
	__ASSERT_DEBUG(State()!=ESStateShuttingDown,Panic(EBadStateUpCall));
	// See comment in ASocket::NewData() as to why the errored state is valid here.
	//__ASSERT_DEBUG(State()!=ESStateError,Panic(EBadStateUpCall));
	__ASSERT_DEBUG(State()!=ESStateDisconnected,Panic(EBadStateUpCall));

	TBool writeflowedoff = iBlockedOperations&EWriteFlowedOff;
	iBlockedOperations &= ~EWriteFlowedOff;
	if(IsBlockedWrite())
		{
		DoSend(EFalse);
		}
	if(writeflowedoff && !(iBlockedOperations&EWriteFlowedOff))
		{
		TryToCompleteSelectIoctl();
		}
	}


void ASocket::ConnectComplete()
/**
Called from protocol to indicate that an active open has completed
*/
	{
	//__ASSERT_DEBUG(State()==ESStateOpeningActive,Panic(EBadStateUpCall));
	if (IsConnectionOriented())
		{
	   // See comment in ASocket::NewData() as to why the errored state is valid here.
        if (State() != ESStateError)
            {
            SetState(ESStateConnected);
            }
		//__ASSERT_DEBUG(IsBlockedConnect(), Panic(EUnexpectedConnect));
		// assertion is invalid because operation could have been cancelled MarkT
		CompleteConnect(KErrNone);
		}
	TryToCompleteSelectIoctl();
	}

void ASocket::ConnectComplete(const TDesC8& aConnectData)
/**
Called from protocol to indicate that an active open has completed with connection data
*/
	{
	__ASSERT_ALWAYS(CanSendConnectData(),Panic(EUnexpectedConnectData));
	//__ASSERT_DEBUG(IsBlockedConnect(),Panic(EUnexpectedConnect));
	// assertion is invalid because operation could have been cancelled
	if ( !iBlockedConnectMsg->IsNull (MSG_PRM(2)) )
		{
		TInt ret = iBlockedConnectMsg->WriteDes(MSG_PRM(2),aConnectData);
		if(ret!= KErrNone)
			{
			return;
			}
		}
	ConnectComplete();
	}

void ASocket::ConnectComplete(CSubConnectionFlowBase& anSSP)
/**
Called from protocol to indicate that a passive open has completed
*/
	{
    // See comment in ASocket::NewData() as to why the errored state is valid here.
	__ASSERT_DEBUG(State()==ESStateOpeningPassive || State()==ESStateError,Panic(EBadStateUpCall));
	__ASSERT_ALWAYS(iAcceptQ, Panic(ENotListeningSocket));
	__ASSERT_ALWAYS(iAcceptQ->Count()<iAcceptQ->Length(), Panic(EAcceptQueFull));

	TAcceptQEntry n;
	n.iSSP = &anSSP;
	n.iConnectData=NULL;
	n.iConnectDataError=KErrNone;
	iAcceptQ->Add(&n);

	DoCompleteAccept();
	//LOG( ESockLog::Printf(_L("ASocket::ConnectComplete iAcceptQ->=%d, provider =%u)"), iAcceptQ->Count(), (void*)&anSSP ) );
	TryToCompleteSelectIoctl();
	}

void ASocket::ConnectComplete(CSubConnectionFlowBase& anSSP,const TDesC8& aConnectData)
/**
Called from protocol to indicate that a passive open has completed with connection data
*/
	{
	__ASSERT_ALWAYS(CanSendConnectData(),Panic(EUnexpectedConnectData));
	__ASSERT_ALWAYS(iAcceptQ, Panic(ENotListeningSocket));
	__ASSERT_ALWAYS(iAcceptQ->Count()<iAcceptQ->Length(), Panic(EAcceptQueFull));

	TAcceptQEntry n;
	n.iSSP = &anSSP;
	n.iConnectDataError = KErrNone;
	n.iConnectData=aConnectData.Alloc();
	if(!n.iConnectData)
		{
		n.iConnectDataError = KErrNoMemory;
		}
	iAcceptQ->Add(&n);

	DoCompleteAccept();
	TryToCompleteSelectIoctl();
	}

void ASocket::DoCompleteAccept()
/**
Common dequeue for accept upcalls
*/
	{
	__ASSERT_DEBUG(iAcceptQ->Count(),Fault(EBadAcceptQueue));

	if (!IsBlockedConnect())
		{
		return;
		}

    TAcceptQEntry a;
	iAcceptQ->Remove(&a);

	ASocket* newCon=GetAcceptingSocket();
	CTransportFlowShim* shimFlow = factoryobject_cast<CTransportFlowShim>(a.iSSP);
	__ASSERT_DEBUG(shimFlow, User::Panic(KSpecAssert_ESockSSocksntsck, 6));

	// The SAP is no longer owned by the listener socket.
	shimFlow->iListenerControlNotify = NULL;

	if (newCon == NULL)
		{
		// Delete the flow created for accept
		shimFlow->InitDestroy();
		CompleteConnect(KErrCancel);
		return;
		}

	TRAPD(ret, newCon->AcceptSetupL(*this,*a.iSSP));
	//!!!there was a defect where by deleting iSSP here the TCP PRT would crash. Steve Butler
	//fixed it => check it snce aiSSP would be deleted had AcceptSetupL left!!!
	if (ret != KErrNone)
		{
		CompleteConnect(ret);
		return;
		}
/* goes into AcceptSetupL vitual fn
	if (newCon->State()!=ESStateAccepting && newCon->State()!=ESStateNull)
		{
		//Session()->PanicSocketClient(iBlockedConnect,KESockClientPanic,EBadAccept);
		CWorkerSession::PanicSocketClient(iBlockedConnect,KESockClientPanic,EBadAccept);
		iSession->DontCompleteCurrentRequest();
		return;
		}
*/
	newCon->SetState(ESStateConnected);

	if (a.iConnectData)// && iBlockedConnect.Ptr2()) checked in WriteParam
		{
		TPtr8 des=a.iConnectData->Des();
		ret = iBlockedConnectMsg->WriteDes(MSG_PRM(2),des,0);
		if(ret!= KErrNone)
			{
			return;
			}
		}

	delete a.iConnectData;

	// Call SecurityCheck() on the new SAP to communicate the MProvdSecurityChecker
	// pointer to it.  It is a somewhat bizarre situation if the PRT returns an error
	// to this as, in reality, the PRT  should not make a ConnectComplete() upcall in the
	// first place only to fail the subsequent SecurityCheck() downcall.  The PRT should check
	// security beforehand and, on failure, simply not call ConnectComplete().  However, if we
	// get an error, then complete the RSocket::Accept() operation on the listening socket with
	// the error and place the accept socket in the error state.
	ret = newCon->SecurityCheck();
	if (ret != KErrNone)
		{
		LOG( ESockLog::Printf(_L("ASocket %08x:\tDoCompleteAccept(): socket %08x returned security check error %d"), this, a.iSSP, ret ) );
		a.iConnectDataError = ret;
		newCon->Error(ret, MSessionControlNotify::EErrorFatal);
		}

	newCon->iSSP->Start();
	CompleteConnect(a.iConnectDataError);
	}

void ASocket::CanClose(MSessionControlNotify::TDelete aDelete)
/**
Called from protocol to indicate that a graceful close has completed
*/
	{
	(void)aDelete;

	//__ASSERT_ALWAYS(IsBlockedClose(),Panic(EUnexpectedClose));
	// assertion is invalid because operation could have been cancelled MarkT
	__ASSERT_DEBUG(State()==ESStateClosing||State()==ESStateShuttingDown,Panic(EBadStateUpCall));

	CompleteClose(KErrNone);
	if (iFlowBinder)
		{
		iFlowBinder->Unbind();
		}
	// Flowbinder takes care of itself from now onwards; forget all downwards pointers
	iSSP = NULL;
	iSSPData = NULL;
	iFlowBinder = NULL;
	iServiceProvider.Close();
/* Flow binder takes care of this SAP behaviour
	if(aDelete==MSessionControlNotify::EDetach)
	    {
		iSSP = NULL;		// Stop d'tor deleting the SSP.
		iFlowBinder = NULL;iSSPData = NULL;
		}
*/
	if (State()==ESStateClosing)
		{
		SetState(ESStateDead);

		// delete this; must not access member data after executing this statement
	    InitiateDestruction();
		}
	else
	    {
		SetState(ESStateDead);
		}

	}

void ASocket::CanClose(const TDesC8& aDisconnectData, MSessionControlNotify::TDelete aDelete)
/**
Called from protocol to indicate that a graceful close has completed with disconnect data
*/
	{

	//__ASSERT_ALWAYS(IsBlockedClose(),Panic(EUnexpectedClose));
	// assertion is invalid because operation could have been cancelled MarkT
	// or could be non-blocking close
	__ASSERT_ALWAYS(CanSendDisconnectData(),Panic(EUnexpectedDisconnectData));
	TInt ret = iBlockedCloseMsg->WriteDes(MSG_PRM(2),aDisconnectData);
	if(ret!= KErrNone)
		{
		return;
		}
	CanClose(aDelete);
	}

void ASocket::Disconnect()
/**
Called from protocol to indicate that a disconnect has been initiated - prob from the remote host
*/
	{
	__ASSERT_ALWAYS(IsConnectionOriented(),Panic(EBadDisconnect));
   // See comment in ASocket::NewData() as to why the errored state is valid here.
	__ASSERT_DEBUG(State()==ESStateConnected || State()==ESStateShuttingDown || State()==ESStateError, Panic(EBadStateUpCall));

	iDisconnectDataError=KErrNone;

	if (State() != ESStateError)
	    {
        SetState(CanReconnect() ? ESStateCreated : ESStateDisconnected);
	    }
	CompleteWrite(KErrDisconnected);
	CompleteRead(KErrDisconnected);
	CompleteConnect(KErrDisconnected);
	TryToCompleteSelectIoctl();
	}

void ASocket::Disconnect(TDesC8& aDisconnectData)
/**
Called from protocol to indicate that a disconnect has been initiated - prob from the remote host - with disconnect data
*/
	{
	__ASSERT_DEBUG(IsConnectionOriented(),Panic(EBadDisconnect));
	   // See comment in ASocket::NewData() as to why the errored state is valid here.
	__ASSERT_DEBUG(State()==ESStateConnected || State()==ESStateShuttingDown || State()==ESStateError,Panic(EBadStateUpCall));

	Disconnect();
	iDisconnectData=aDisconnectData.Alloc();
	if(!iDisconnectData)
		{
		iDisconnectDataError = KErrNoMemory;
		}
	else
		{
		iDisconnectDataError = KErrNone;
		}
	}

void ASocket::DisconnectFromListener(CSubConnectionFlowBase& aSSP)
/**
Called from protocol to indicate that a disconnect has been initiated for an SAP which has not been
accepted yet and is waiting in the accept queue of a listening socket
*/
  	{
  	#ifdef _DEBUG
  		TBool found = EFalse;
  	#endif

  	while(iAcceptQ->Count())
  		{
  		// Remove the entry from the old queue.
  		TAcceptQEntry a;
  		iAcceptQ->Remove(&a);

  		// Add it to the new queue if it is not the one we want to remove.
  		if(a.iSSP != &aSSP)
  			{
  			iNextAcceptQ->Add(&a);
  			}
#ifdef _DEBUG
		else
			{
			found = ETrue;
			}
#endif
  		}

  	// Swap the queues.
  	CCirBuf<TAcceptQEntry>* temp = iNextAcceptQ;
  	iNextAcceptQ = iAcceptQ;
  	iAcceptQ = temp;

  	__ASSERT_DEBUG(found,Panic(EBadDisconnectFromListener));
  	}

void ASocket::IoctlComplete(TDesC8 *aBuf)
/**
Called from protocol to indicate that an outstanding ioctl has completes
*/
	{

	if (aBuf && !iBlockedIoctlMsg->IsNull (MSG_PRM(1)))
		{
		TInt ret = iBlockedIoctlMsg->WriteDes(MSG_PRM(1),*aBuf);
		if(ret!= KErrNone)
			{
			return;
			}
		}
	CompleteIoctl(KErrNone);
	}
	
void ASocket::SetLocalNameComplete()
/**
Called from protocol to indicate that an outstanding Bind  has completed
*/
    {
    if (State() == ESStateBinding)
      	{
      	// A SetLocalName or Autobind had just completed.
    	SetState(iNextState); // Move to next state.
    	iIsBound=ETrue;
    	
    	switch(iNextState)
    		{
		case ESStateCreated:
		case ESStateOpen:
			CompleteSetLocalName(KErrNone);
			break;
		case ESStateConnected:
			// UDP case
			TryToCompleteSelectIoctl();
			CompleteConnect(KErrNone);
			break;
		case ESStateOpeningActive:
			if (CanSendConnectData() && iConnectData)
				{
		        TDes8* sendBuf = BorrowTemporaryBuffer(KMaxStreamChunk);
		        TInt ret;
		        if(!sendBuf)
		            {
		            ret = KErrNoMemory;
		            }
		        else
		            {
		            ret = GetUserMessage(ESocketConnectMessage)->ReadDes(MSG_PRM(1), *sendBuf);
		            }
				if (ret == KErrNone)
					{
					iSSP->ActiveOpen(*sendBuf);
					}
				else
					{
					CompleteConnect(ret);
					}
				}
			else
				{
				iSSP->ActiveOpen();
				}
			break;
		default:
			break;
    		}
      	}
    }

void ASocket::CancelConnect()
	{
	if (State()==ESStateDead)
 		{
 		PanicSocketClient(ESockBadHandle);
 		return;
 		}

	CompleteConnect(KErrCancel);
	iSSP->Shutdown(MSessionControl::EImmediate);

	if (CanReconnect())
		{
		// Reset socket state
		if (IsConnectionOriented())
			{
			SetState(ESStateCreated);
			}
		else
			{
			SetState(ESStateOpen);
			}
		}
	else
		{
		SetState(ESStateError);
		}

	}

void ASocket::CancelAccept()
	{
	CompleteConnect(KErrCancel);
	}

void ASocket::CancelAll()
/**
Cancel All pending requests

*/
	{
	CancelIoctl();
	CompleteWrite(KErrCancel);
	CompleteRead(KErrCancel);
	if (IsBlockedConnect())
		{
		if (State()==ESStateOpeningPassive)
			{
			CancelAccept();
			}
		else
			{
			CancelConnect();
			}
		}
    CompleteClose(KErrCancel);
       CompleteSetLocalName(KErrCancel);
	}

void ASocket::GetInfoL()
/**
Get protocol info for this socket

*/
	{
	if (State()==ESStateNull || State()==ESStateAccepting)
		{
		SetReturn(KErrNotFound);
		return;
		}

	__ASSERT_DEBUG(iProtocolInfo, User::Panic(KSpecAssert_ESockSSocksntsck, 7));
	TPckg<TProtocolDesc> p(*iProtocolInfo);
	WriteParamL(ESocketCurrentMessage,MSG_PRM(0),p,0);
	}

TInt ASocket::Error(TInt aError, TUint aOperationMask)
/**
Error up call from SSP, carries a bitmask indicating exactly which operations to signal the error on.
*/
	{
#if defined(SYMBIAN_NETWORKING_LEGACY_COMPATIBILITY_SUPPORT)
	// Support for the IP stack's just-in-time resetting of a shutdown socket's state is is provided by allowing it
	// to notify via a pseudo-error. This is all in support of regrettable behaviour that after a shutdown TCP and UDP
	// sockets must complete reads & writes with a different error code to all other sockets.
	if(aOperationMask == MSessionControlNotify::EErrorLegacySupportRequest)
		{
		SetState((TSocketState) aError);
		return KErrNone;
		}
#endif
	if (aOperationMask == MSessionControlNotify::EErrorCompleteConnect)
	    {
	    CompleteConnect(aError);
	    }
	else
	    {
    	DoError(aError, aOperationMask, ETrue);
	    }

	return State() == ESStateError ? KErrGeneral : KErrNone;
	}

void ASocket::DoError(TInt aError, TUint aOperationMask, TBool /*aDeleteInterface*/)
	{
	__ASSERT_DEBUG(iState!=ESStateDead,Panic(EBadStateUpCall));
//	__ASSERT_DEBUG(iState!=ESStateError,Panic(EBadStateUpCall));
	//__ASSERT_DEBUG(iState!=ESStateDisconnected,Panic(EBadStateUpCall));
	//__ASSERT_DEBUG(anError!=KErrNone,Panic(EBadErrorCall));

	// completionMask remembers whether there were any pending operations that were not completed,
	// and whether the error is fatal (i.e. the socket permanently returns an error).
    TUint completionMask = aOperationMask & MSessionControlNotify::EErrorFatal;
	if (aOperationMask & MSessionControlNotify::EErrorClose)
		{
		if (!CompleteClose(aError))
			{
			completionMask |= MSessionControlNotify::EErrorClose;
			}
		}

	if (aOperationMask & MSessionControlNotify::EErrorRecv)
		{
        if (!CompleteRead(aError))
        	{
	        completionMask |= MSessionControlNotify::EErrorRecv;
        	}
		}

	if (aOperationMask & MSessionControlNotify::EErrorSend)
		{
		if (!CompleteWrite(aError))
			{
	        completionMask |= MSessionControlNotify::EErrorSend;
			}
		}

	if ((aOperationMask&MSessionControlNotify::EErrorIoctl) && !iSelectFlags)
		{
		CompleteIoctl(aError);
		}
	if (aOperationMask & MSessionControlNotify::EErrorCompleteSetLocalName)
		{
		CompleteSetLocalName(aError);
		}

    iError = aError;
    if (completionMask)
        {
        iErrorOperationMask = aOperationMask & completionMask;
        }
        
	if (State() == ESStateBinding && ((iNextState == ESStateCreated) || (iNextState == ESStateOpen)))
		{
		// If SetLocalName in progress then do not put the socket to
		// a sticky error state.
		iError = KErrNone;
		// Reset socket state.
		if (!IsConnectionOriented())
			{
			SetState(ESStateOpen);
			}
		else
			{
			SetState(ESStateCreated);
			}
		}
		else

	if (aOperationMask & MSessionControlNotify::EErrorConnect || aOperationMask&MSessionControlNotify::EErrorFatal)
		{
		CompleteConnect(aError);

		// KErrNoMemory not treated as a fatal error
		if (aError == KErrBindersInvalid || (!CanReconnect() && aError != KErrNoMemory))
			{
			// Fatal error
			iError = aError;
			SetState(ESStateError);
			}
		else
			{
			// Reset socket state
			if (!IsConnectionOriented())
				{
				SetState(ESStateOpen);
				}
			else
				{
				SetState(ESStateCreated);
				}
			}
		}

    if (iError == KErrBindersInvalid)
        {
        __ASSERT_DEBUG(iFlowBinder, User::Panic(KSpecAssert_ESockSSocksntsck, 8));
        iFlowBinder->Unbind();
      	iFlowBinder = NULL;
       	iSSP = NULL;
       	iSSPData = NULL;
       	iServiceProvider.Close();
        }
	TryToCompleteSelectIoctl();
	}

TInt ASocket::DrainStreamProtocol(AMessage* aMsg)
/**
 Drain a stream based protocol into client buffers.
*/
	{
	__ASSERT_DEBUG(iDataAvailable>=0,Panic(EBadByteCount));
	__ASSERT_DEBUG(iRecByteCount>=0,Panic(EBadByteCount));

	TDes8* recBuf = NULL;
	if(!iRecvUseMBufs)
		{
		TInt xferBufSize = Min((TInt) KMaxStreamChunk, Min(iDataAvailable, iRecByteCount));
		recBuf = BorrowTemporaryBuffer(xferBufSize);
		if(!recBuf)
			{
			return KErrNoMemory;
			}
		}

#ifdef SYMBIAN_NETWORKING_PERFMETRICS
	TInt rxActual = 0;
#endif

	RMBufChain data;
	TInt ret = KErrNone;
	do
		{
        TInt xfrLen;
        if (iRecvUseMBufs)
			{
			// We are using RMBufChain.	So we can read the complete data that is available
			xfrLen = iDataAvailable;
			}
	     else
			{
			xfrLen = Min(recBuf->MaxLength(), Min(iDataAvailable, iRecByteCount));
			}

		if (xfrLen==0)
			{
			return KErrNone;
			}


		RMBufChain data;
		ret = iSSPData->GetData(data, xfrLen, iRecvFlags);
		if (ret == 0 && iBlockedOperations&EReadStopped)
			{
			/* Look for the special case, where the SSP renegs on the amount of
			 * data available after an EStopInput Shutdown, as that can stop us
			 * ever exiting this while loop. */
			iDataAvailable = 0;
			return KErrEof;
			}
		if(ret >= 0)
			{
			// Now we received something.
			if (iRecvUseMBufs)
				{
				ret = aMsg->WriteMBuf(MSG_PRM(2), data );
				// No need to delete as the write operation has taken the ownership of our RMBufChain
				}
			else
				{
				// Default assumption is multi-buf chain
				TPtr8 des(const_cast<TUint8*>(recBuf->Ptr()), xfrLen, xfrLen);
				if(!data.First()->Next())
					{
					// Actually data is all in one buf; repoint descriptor at that
					des.Set(data.First()->Ptr(), data.First()->Length(), data.First()->Length()); 
					}
				else
					{
					data.CopyOut(des, 0);
					}
				xfrLen = des.Length();
            	ret = aMsg->WriteDes(MSG_PRM(2), des, iRecOffset);
				if ( ret == KErrNone )
					{
					data.Free(); // Free the chain
					}
				}
            if(ret == KErrNone)
				{
#ifdef SYMBIAN_NETWORKING_PERFMETRICS
				rxActual += ipcLen;
#endif
				iRecOffset += xfrLen;
                iDataAvailable -= xfrLen;
				iRecByteCount -= xfrLen;
				}
			}
		else if(xfrLen <= 0)
			{
			// Failed to read or write on this iteration; we're done
			break;
			}
		} while(ret == KErrNone);

#ifdef SYMBIAN_NETWORKING_PERFMETRICS
	IncludePerformanceData(rxActual, -1, -1);
#endif
	return ret;
	}

TInt ASocket::FillStreamProtocol(AMessage* aMsg)
/**
 Fill a stream protocol from client buffers
*/
	{
	__ASSERT_DEBUG(iSendByteCount>=0,Panic(EBadByteCount));
	TPtr8 des(NULL, 0);
	TDes8* sendBuf;
	if(!iSendUseMBufs)
		{
		sendBuf = BorrowTemporaryBuffer(Min((TInt) KMaxStreamChunk, iSendByteCount));
		if(!sendBuf)
			{
			return KErrNoMemory;
			}
		}
	else
		{
		sendBuf = NULL;
		}

	RMBufChain data;
	data.Assign(iSendData);	// pick up any flowed-off chain
	TInt xfrLen = data.Length();

	do
		{
		TInt err = KErrNone;
		if(data.IsEmpty())
			{
			// Need to pick up more data from client
			if (!iSendUseMBufs)
				{
				xfrLen = Min(sendBuf->Size(), iSendByteCount);
				err = data.Alloc(xfrLen, iAllocator);
				if(err == KErrNone)
					{
					RMBuf* first = data.First();
					if(first->Length() >= xfrLen)
						{
						des.Set(first->Ptr(), xfrLen, xfrLen);
						err = aMsg->ReadDes(MSG_PRM(2),des, iSendOffset);
						}
					else
						{
						des.Set(const_cast<TUint8*>(sendBuf->Ptr()), xfrLen, xfrLen);
						err = aMsg->ReadDes(MSG_PRM(2), des, iSendOffset);
						data.CopyIn(des);	// read failure exceedingly rare so don't bother checking return yet
						}
					if(err != KErrNone)
						{
						data.Free();
						}
					}
				}
			else
				{
				err = aMsg->ReadMBuf(MSG_PRM(2), data);
				if(err == KErrNone)
					{
					xfrLen = data.Length();
					}
				}
			}
		else
		    {
		    xfrLen = data.Length();
		    }
		if(err != KErrNone)
			{
			// Failed getting data from the client; bail now without further disturbing the state
			return err;
			}
		
		err = iSSPData->Write(data, iSendFlags, NULL);
		
		// If a protocol takes only part of the chain they must update it so that we have the new head
		__ASSERT_DEBUG(err <= 0 || err == (xfrLen - data.Length()), Panic(EBadByteCount));  
		
		if(err <= 0)	// protocol error or flow-off
			{
			if(err == 0)
			    {
                iSendData.Assign(data);	// keep the MBuf chain for flow-on. In future we could be cleverer and later free this when MBufs are running low
                iBlockedOperations|=EWriteFlowedOff;
			    }
			else 
			    {
			    data.Free();
			    }
			return err;
			}
		iSendOffset += err;
		iSendByteCount -= err;
		} while(iSendByteCount > 0);

	// We successfully completed.
	if ( iSendUseMBufs )
		{
		// Protection against possible non-deletion by SAP
		data.Free ();
		// We have to init the RMBufChain, so that caller see that everything send
		aMsg->InitMBuf(MSG_PRM(2));
		}
	return KErrNone;
	}

TBool ASocket::CheckRunningAndSetReturn()
/**
Check a common set of states and set the return value is required.
*/
{
	switch(State())
		{
	case ESStateNull:
	case ESStateAccepting:
		PanicSocketClient(ENullSocket);
		//lint -fallthrough
	case ESStateDead:
	case ESStateClosing:
		SetReturn(KErrBadHandle);
		return EFalse;
	case ESStateDisconnected:
		SetReturn(KErrDisconnected);
		return EFalse;
	case ESStateError:
		SetReturn(iError);
		return EFalse;
	default:
		return ETrue;
		}
	}

/**
Complete any outstanding read operation.
@param aError error code with which to complete the pending operation
@return ETrue if a pending operation was completed, else EFalse.
*/
TBool ASocket::CompleteRead(TInt aError)
	{
	if (IsBlockedRead())
		{
		if(iAllocAsync)
			{
			iAllocAsync->CancelNewData();
			}
		iBlockedOperations&=~EBlockedRead;
		iReadMsg->CompleteMessage(aError);
        return ETrue;
        }
    else
        {
        return EFalse;
   		}
	}

/**
Complete any outstanding write operation.
@param aError error code with which to complete the pending operation
@return ETrue if a pending operation was completed, else EFalse.
*/
TBool ASocket::CompleteWrite(TInt aError)
	{
	if (IsBlockedWrite())
		{
		if(iAllocAsync)
			{
			iAllocAsync->CancelCanSend();
			}
		iBlockedOperations&=~EBlockedWrite;
		iWriteMsg->CompleteMessage(aError);
		iSendData.Free();
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

void ASocket::CompleteConnect(TInt aError)
	{
	if (IsBlockedConnect())
		{
		iBlockedOperations&=~EBlockedConnect;
		iBlockedConnectMsg->CompleteMessage(aError);
		}
	}

TBool ASocket::CompleteClose(TInt aError)
	{
	if (IsBlockedClose())
		{
		iBlockedOperations&=~EBlockedClose;
		iBlockedCloseMsg->CompleteMessage(aError);
		return ETrue;
		}
	return EFalse;
	}


void ASocket::CompleteIoctl(TInt aError)
	{
	if (IsBlockedIoctl())
		{
		iBlockedOperations&=~EBlockedIoctl;
		iBlockedIoctlMsg->CompleteMessage(aError);
//		LOG(ESockLog::Printf(KESockSessDetailTag, _L8("ASocket::CompleteIoctl completing %08x "), iBlockedIoctl.Handle()) );
		}
	}
	
void ASocket::CompleteSetLocalName( TInt aError )
    {
    if (IsBlockedSetLocalName())
		{
		// SetLocalName has completed.
		iBlockedOperations &= ~EBlockedSetLocalName;
		iBlockedSetLocalNameMsg->CompleteMessage(aError);
		}
    }	

TUint ASocket::SelectConditionsReady()
/**
Which select conditions are ready
*/
    {

	TUint flags=0;
	// Exceptional condition
	if(iError!=KErrNone)
		{
		flags|=KSockSelectExcept;
		}

	// Ready to read - either data or an accept is ready
	if(iDataAvailable
			|| ((iSelectFlags & KSockReadContinuation) != 0 && !iDatagramTail.IsEmpty())
			|| (State()==ESStateOpeningPassive && iAcceptQ->Count())
		    || (iBlockedOperations&EReadStopped))
		{
		flags|=KSockSelectRead;
		}

	// Ready to write - must be connected and cannot be blocking
	if((State()==ESStateConnected || State()==ESStateOpen || State()==ESStateShuttingDown)
		&& !(iBlockedOperations&EWriteFlowedOff) )
		{
		flags|=KSockSelectWrite;
		}

	return flags;
    }

void ASocket::TryToCompleteSelectIoctl()
/**
have a go at completing the select ioctl
*/
    {

	if(iSelectFlags)
	   {
	   TUint ready = (iSelectFlags & SelectConditionsReady());
	   if(ready)
	       {
	       TPckg<TUint> buf(ready);
           IoctlComplete(&buf);
	       iSelectFlags=0;
	       }
	   }
    }

TBool ASocket::CheckReadStopped()
/**
Has reading stopped
*/
	{
	if((iBlockedOperations&EReadStopped) && iDataAvailable<=0)
		{
		TPtrC8 ptr(0,0);
		TInt ret = iReadMsg->WriteDes(MSG_PRM(2), ptr);
		if(iRecvXferLenIdx >= 0)
			{
			iXferLength() = 0;
			ret = iReadMsg->WriteDes(iRecvXferLenIdx, iXferLength);
			if(ret!= KErrNone)
				{
				return EFalse;
				}
			}
		return ETrue;
		}
	return EFalse;
	}

void ASocket::CommunicateOwner()
/**
Communicate Thread ID, Process ID and UID to SAP.

*/
	{
	if (RequiresOwnerInfo())
		{

		TSoOwnerInfo info;

		// Not using more Networking specific macro SYMBIAN_NETWORKING_UPS because this support
		// is required by UPS functionality outside of Networking subsystem as well.
		GetOwnerInfo(info.iProcessId, info, info.iThreadId);

		iSSP->SetOption(KSOLProvider, KSoOwnerInfo, TPckgC<TSoOwnerInfo>(info));

#ifdef SYMBIAN_NETWORKING_UPS
		iSSP->SetOption(KSOLProvider, KSoSetPlatSecApi, TPckgC<const MPlatsecApiExt*>(this));
#endif
		}
	}

TInt ASocket::GetConnectionSocketInfo(TConnectionSocketInfo& aInfo) const
/**
Return information about this socket
*/
	{
	__ASSERT_DEBUG(iSSP, User::Panic(KSpecAssert_ESockSSocksntsck, 10));

	if(iIsBound)
		{
		iSSP->LocalName(aInfo.iSourceAddress);
		}
	else
		{
		return KErrNotFound;
		}

	if (State()==ESStateConnected || State()==ESStateOpeningActive || State()==ESStateShuttingDown)
		{
		iSSP->RemName(aInfo.iDestinationAddress);
		}
	else
		{
		return KErrNotFound;
		}

	__ASSERT_DEBUG(iProtocolInfo, User::Panic(KSpecAssert_ESockSSocksntsck, 11));

	aInfo.iAddressFamily = iProtocolInfo->iAddrFamily;
	aInfo.iProtocol = iProtocolInfo->iProtocol;

	return KErrNone;
	}

TInt ASocket::RequestAsyncMBufAllocation(TInt aSignal, TUint aSize)
	{
	TInt ret = KErrNone;
	if(!iAllocAsync)
		{
		TRAP(ret, iAllocAsync = CWaitForMBufs::NewL(this));
		if(ret != KErrNone)
			{
			// This second-chance mechanism has failed; it's now reasonable just to error the client
			Error(KErrNoMemory, aSignal == CWaitForMBufs::ECanSend? MSessionControlNotify::EErrorSend: MSessionControlNotify::EErrorRecv);
			}
		}

	if(aSignal == CWaitForMBufs::ENewData)
		{
		iAllocAsync->AllocWaitAndSignalNewData(aSize);
		}
	else
		{
		if(aSignal == CWaitForMBufs::ECanSend)
			{
			iAllocAsync->AllocWaitAndSignalCanSend(aSize);
			}
		}

	return ret;
	}

void ASocket::ReadParamL(TSocketMessage aMessage, TInt aSrcParamIndex,TDes8 &aDes,TInt anOffset)
	{
	User::LeaveIfError(GetUserMessage(aMessage)->ReadDes(aSrcParamIndex, aDes, anOffset));
	}

void ASocket::ReadParamL(TSocketMessage aMessage, TInt aSrcParamIndex, RMBufChain& aBufChain)
	{
	User::LeaveIfError(GetUserMessage(aMessage)->ReadMBuf(aSrcParamIndex, aBufChain));
	}

void ASocket::WriteParamL(TSocketMessage aMessage, TInt aDstParamIndex,const TDesC8& aDes,TInt anOffset)
	{
	User::LeaveIfError(GetUserMessage(aMessage)->WriteDes(aDstParamIndex, aDes, anOffset));
	}

void ASocket::WriteParamL(TSocketMessage aMessage, TInt aDstParamIndex, RMBufChain& aBufChain)
	{
	User::LeaveIfError(GetUserMessage(aMessage)->WriteMBuf(aDstParamIndex, aBufChain));
	}

AMessage* ASocket::GetUserMessage( TSocketMessage aMessage ) const
	{
	AMessage* msg = NULL;
	switch (aMessage)
		{
			case ESocketCurrentMessage:
			msg = iCurrentMsg;
			break;

			case ESocketReadMessage:
			msg = iReadMsg;
			break;

			case ESocketWriteMessage:
			msg = iWriteMsg;
			break;

			case ESocketCloseMessage:
			msg = iBlockedCloseMsg;
			break;

			case ESocketIoCtlMessage:
			msg = iBlockedIoctlMsg;
			break;

			case ESocketConnectMessage:
			msg = iBlockedConnectMsg;
			break;
			case ESocketSetLocalNameMessage:
			msg = iBlockedSetLocalNameMsg;
			break;

			default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockSSocksntsck, 12));
		}
	return msg;
	}

void ASocket::SetUserMessage ( TSocketMessage aMessageType, AMessage* aMessage )
	{
	switch (aMessageType)
		{
			case ESocketCurrentMessage:
			iCurrentMsg = aMessage;
			break;

			case ESocketReadMessage:
			iReadMsg = aMessage;
			break;

			case ESocketWriteMessage:
			iWriteMsg = aMessage;
			break;

			case ESocketCloseMessage:
			iBlockedCloseMsg = aMessage;
			break;

			case ESocketIoCtlMessage:
			iBlockedIoctlMsg = aMessage;
			break;

			case ESocketConnectMessage:
			iBlockedConnectMsg = aMessage;
			break;
			case ESocketSetLocalNameMessage:
			iBlockedSetLocalNameMsg = aMessage;
			break;

			default:
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockSSocksntsck, 13));
		}
	}


#ifdef SYMBIAN_NETWORKING_UPS
TInt ASocket::GetProcessAndThreadId(TProcessId& aProcessId, TThreadId& aThreadId) const
/**
Retrieve the process and thread id associated with the pending request.
*/
	{
	TSocketMessage msg = ESocketCurrentMessage;

	if (IsBlockedConnect())
		{
		msg = ESocketConnectMessage;
		}
	else
	if (IsBlockedWrite())
		{
		msg = ESocketWriteMessage;
		}
	if (msg != ESocketCurrentMessage || (iState == ESStateCreated || iState == ESStateOpen))
		{
		// Guard against an invalid current message - especially for a Connect/CancelConnect scenario
		// whereby the socket is in the ESStateError state.  Needs revisiting.
		return GetUserMessage(msg)->GetProcessAndThreadId(aProcessId, aThreadId);
		}
	else
		{
		return KErrUnknown;
		}
	}

TInt AMessage::GetProcessAndThreadId(TProcessId& /*aProcessId*/, TThreadId& /*aThreadId*/) const
	{
	return KErrNotSupported;
	}

#endif

