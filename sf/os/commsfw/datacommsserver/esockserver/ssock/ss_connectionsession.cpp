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

/**
 @file
 @internalTechnology
 @released since v9.5
*/


#include <comms-infras/ss_log.h>
#include <ss_std.h>
#include <comms-infras/esockmessages.h>
#include <in_sock.h> //for KAfInet
#include <elements/factorynotify.h>

#include <ss_glob.h>
#include <comms-infras/ss_roles.h>

#include <es_enum.h>

#include "ss_connectionsession.h"
#include <comms-infras/es_connectionservermessages.h>

#include <comms-infras/ss_tiermanager.h>
#include "ss_apiext_messages.h"

#include <elements/nm_messages_child.h>
#include <comms-infras/ss_nodemessages_tiermanager.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <comms-infras/ss_nodemessages_tiermanagerfactory.h>

#include <comms-infras/es_connectionservparameterbundletrace.h>
#include <comms-infras/es_connectionservparameterbundletraceimpl.h> // include this once per dll


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSockscnctn, "ESockSSockscnctn");
#endif

using namespace ConnectionServ;
using namespace ESock;
using namespace Messages;
using namespace MeshMachine;

using namespace CommsDat;


//Panic codes
_LIT(KConnectionSessionPanic, "ConnectionSession");
enum TConnectionSessionPanic
	{
	EExpectedServiceProvider = 0,     //
    };


/**
Constructor
*/
CConnectionSession::CConnectionSession(TUidType aUid, Den::TSessionUniqueId aSessionUniqueId)
	: CWorkerSession(aUid, aSessionUniqueId)
	{
//    LOG_NODE_CREATE(KESockConnectionTag, CConnectionSession);
	}


/*static*/ CConnectionSession* CConnectionSession::NewL(TProcessId aProcessId, TUidType aUid, Den::TSessionUniqueId aSessionUniqueId)
	{
	CConnectionSession* self = new (ELeave) CConnectionSession(aUid, aSessionUniqueId);
	CleanupStack::PushL(self);
	self->ConstructL(aProcessId);
	CleanupStack::Pop(self);
	return self;
	}


CConnectionSession::~CConnectionSession()
	{
	// No handshaking code required - cancelling any outstanding requests was already performed by the client side
	//  which synchonously waited for everything to shut down before issuing the close on the session.

	// Best make sure though.. :-)

	__ASSERT_DEBUG(iStatusMessage.IsNull(), User::Panic(KSpecAssert_ESockSSockscnctn, 1));
	__ASSERT_DEBUG(!iStatusBundle, User::Panic(KSpecAssert_ESockSSockscnctn, 2));

	__ASSERT_DEBUG(!iNotificationActive, User::Panic(KSpecAssert_ESockSSockscnctn, 3));
	__ASSERT_DEBUG(iNotificationMessage.IsNull(), User::Panic(KSpecAssert_ESockSSockscnctn, 4));
	__ASSERT_DEBUG(iNotificationQueue.Count()==0, User::Panic(KSpecAssert_ESockSSockscnctn, 5));
	iNotificationQueue.Close();

	LeaveServiceProvider();
    LOG_NODE_DESTROY(KESockConnectionTag, CConnectionSession)
	}



TInt CConnectionSession::CheckPolicy(const TSecurityPolicy& aPolicy, const char *aDiagnostic)
/**
Check the security policy of a process.
Called from a socket or resolver provider to check whether the process conforms to the security policy passed as
argument.
@see MProvdSecurityChecker
*/
	{
	RProcess process;
	TInt ret = aPolicy.CheckPolicy(process, aDiagnostic) ? KErrNone : KErrPermissionDenied;
	process.Close();
	return ret;
	}


void CConnectionSession::CompleteMessage(const RMessage2& aMessage, TInt aResult)
	{
	__ASSERT_DEBUG(!aMessage.IsNull(), User::Panic(KSpecAssert_ESockSSockscnctn, 6));
	if(!aMessage.IsNull())
		{
		LOG(ESockLog::Printf(KESockSessDetailTag, _L8("CConnectionSession(%08x):\tCompleteMessage(%08x) with %d"), this, aMessage.Handle(), aResult) );
		aMessage.Complete(aResult);
		}
	__ASSERT_DEBUG(aMessage.IsNull(), User::Panic(KSpecAssert_ESockSSockscnctn, 7));
	}


/**
 Handle messages for this server.
 @param aMessage Standard IPC request from client side
*/
void CConnectionSession::ServiceL(const RMessage2& aMessage)
	{
	LOG(
		TBuf8<64> messName;
		ESockLog::ConnServIPCMessName((TConnectionServerMessage) aMessage.Function(), messName);
		ESockLog::Printf(KESockServerTag, _L8("CConnectionSession(%08x):\tServiceL, Message(%08x) [%S]"),
				this, aMessage.Handle(), &messName);
		);

	const Den::RSafeMessage& safeMessage(static_cast<const Den::RSafeMessage&>(aMessage));

    const TInt function = aMessage.Function();
	LOG_DETAILED( ESockLog::Printf(KESockConnectionTag, _L("CConnectionSession %08x:\tCommand %d"), this, aMessage.Function()) );
    /* Cases handled without a subsession */
	switch (function)
        {
		case ECMAttachToTierManager:
			StartAttachToTierManager(aMessage);
			break;

		case ECMAccessPointStatusQuery_DoThenGetResultOrSize:
			{
			const Messages::TNodeId& sp = ServiceProviderL();
			
			// ensure not already active. Should be guarded by client side code
            if (!iStatusMessage.IsNull())
                {
                CompleteMessage(aMessage, KErrInUse);
                return;
                }

			// unpack bundle
			TInt length = aMessage.GetDesLengthL(0);
			RBuf8 buffer;
			buffer.CreateL(length);
			CleanupClosePushL(buffer);
			aMessage.ReadL(0, buffer);
			CConnectionServParameterBundle* queryBundle = CConnectionServParameterBundle::LoadL(buffer);
			CleanupStack::PopAndDestroy(&buffer);
			CleanupStack::PushL(queryBundle);

			CRefCountOwnedParameterBundle* bundleOwner = new(ELeave)CRefCountOwnedParameterBundle(queryBundle);
			CleanupStack::Pop();
			bundleOwner->Open();
			CleanupClosePushL(*bundleOwner);

			// create and send message
			TNodeCtxId us(ETierStatusActivity, NodeId()); //We pretend to be a MM node :)

			// ownership of bundle travels with message
			RClientInterface::OpenPostMessageClose(us, sp,
				TCFTierStatusProvider::TTierStatusQuery(bundleOwner, &aMessage).CRef());

            CleanupStack::Pop();

			// remember client request
			iStatusMessage = aMessage;
			}
			break;

		case ECMAccessPointStatusQuery_GetResult:
			{
			if (!iStatusBundle)
			    {
			    CompleteMessage(aMessage, KErrNotReady);
			    return;
			    }

			CRefCountOwnedParameterBundle* bundleOwner = iStatusBundle;
			const CConnectionServParameterBundle& bundle = *static_cast<const CConnectionServParameterBundle*>(bundleOwner->PtrL());
			iStatusBundle = NULL;
			CleanupClosePushL(*bundleOwner);

			TInt length = bundle.Length();

			// we should only be calling this after a realloc on the client side.
			//  so there should be no way the bundle won't fit
			if (length > safeMessage.GetDesMaxLengthL(0))
			    {
			    CompleteMessage(aMessage, KErrOverflow);
			    CleanupStack::PopAndDestroy(bundleOwner);
			    return;
			    }

			RBuf8 buffer;
			buffer.CreateL(length);
			CleanupClosePushL(buffer);
			User::LeaveIfError(bundle.Store(buffer));
			aMessage.WriteL(0, buffer);
			CompleteMessage(aMessage, KErrNone);
			CleanupStack::PopAndDestroy(&buffer);
			CleanupStack::PopAndDestroy(bundleOwner);
			}
			break;

		case ECMAccessPointStatusQuery_Cancel:
			{
			// complete the original IPC
			if (iStatusMessage.IsNull())
			    {
			    // nothing to cancel
			    CompleteMessage(aMessage, KErrNone);
			    return;
			    }

			CompleteMessage(iStatusMessage, KErrCancel);

			// create and send message
			TNodeCtxId us(ETierStatusActivity, NodeId()); //Pretend to be a MM node
			__ASSERT_DEBUG(ServiceProvider()!=Messages::TNodeId::NullId(),User::Panic(KConnectionSessionPanic,EExpectedServiceProvider));
			RClientInterface::OpenPostMessageClose(us, ServiceProviderL(), TEBase::TCancel().CRef());

        	if (iStatusBundle)
        	    {
    			iStatusBundle->Close();
    			iStatusBundle = NULL;
        	    }

			// we'll complete when we get the response.
			iStatusMessage = aMessage;
			}
			break;

		case ECMAccessPointNotification_SetupThenAwaitThenGetResultOrSize:
			{
			const Messages::TNodeId& sp = ServiceProviderL();

			// ensure not already active. Should be guarded by client side code
			if (iNotificationActive)
			    {
			    CompleteMessage(aMessage, KErrInUse);
			    return;
			    }

			// unpack bundle
			TInt length = aMessage.GetDesLengthL(0);
			RBuf8 buffer;
			buffer.CreateL(length);
			CleanupClosePushL(buffer);
			aMessage.ReadL(0, buffer);
			CConnectionServParameterBundle* queryBundle = CConnectionServParameterBundle::LoadL(buffer);
			CleanupStack::PopAndDestroy(&buffer);
			CleanupStack::PushL(queryBundle);

			CRefCountOwnedParameterBundle* bundleOwner = new (ELeave)CRefCountOwnedParameterBundle(queryBundle);
			CleanupStack::Pop();
			bundleOwner->Open();
			CleanupClosePushL(*bundleOwner);

			// create and send message
			TNodeCtxId us(ETierNotificationActivity, NodeId()); //Pretend to be a MM node
			RClientInterface::OpenPostMessageClose(us, sp,
				TCFTierStatusProvider::TTierNotificationRegistration(bundleOwner, &aMessage).CRef());

			CleanupStack::Pop();

			// remember client request
			iNotificationActive = ETrue;
			iNotificationMessage = aMessage;
			}
			break;


		case ECMAccessPointNotification_AwaitThenGetResultOrSize:
			{
			// ensure already active but no outstanding request. Should be guarded by client side code
			if (!iNotificationActive)
			    {
			    CompleteMessage(aMessage, KErrNotReady);
			    return;
			    }

            if (!iNotificationMessage.IsNull())
			    {
			    CompleteMessage(aMessage, KErrInUse);
			    return;
			    }

			// pick from queue if anything in it..
			if(iNotificationQueue.Count())
				{
				CRefCountOwnedParameterBundle* bundleOwner = iNotificationQueue[0];
				const CConnectionServParameterBundle* queuedNotification = static_cast<const CConnectionServParameterBundle*>(bundleOwner->PtrL());
				TInt length = queuedNotification->Length();

				if (length > safeMessage.GetDesMaxLengthL(1))
					{
					// we're gonna need a bigger boat
					CompleteMessage(aMessage, length);
					}
				else
					{
					iNotificationQueue.Remove(0);
					CleanupClosePushL(*bundleOwner);
					RBuf8 buffer;
					buffer.CreateL(length);
					CleanupClosePushL(buffer);
					User::LeaveIfError(queuedNotification->Store(buffer));
					aMessage.WriteL(1, buffer);
					CompleteMessage(aMessage,KErrNone);
					CleanupStack::PopAndDestroy(&buffer);
					CleanupStack::PopAndDestroy(bundleOwner); // close ref on bundle
					}
				}
			else // we have to wait for a notification coming back from the tier so park the request
				{
				iNotificationMessage = aMessage;
				}

			}
			break;


		case ECMAccessPointNotification_GetResult:
			{
			if (!iNotificationActive || iNotificationQueue.Count() == 0)
			    {
			    CompleteMessage(aMessage, KErrNotReady);
			    return;
			    }

			CRefCountOwnedParameterBundle* bundleOwner = iNotificationQueue[0];
			const CConnectionServParameterBundle* queuedNotification = static_cast<const CConnectionServParameterBundle*>(bundleOwner->PtrL());
			iNotificationQueue.Remove(0);
			CleanupClosePushL(*bundleOwner);
			TInt length = queuedNotification->Length();

			// we should only be calling this after a realloc on the client side.
			//  so there should be no way the bundle won't fit
			if (length > safeMessage.GetDesMaxLengthL(1))
			    {
			    CompleteMessage(aMessage, KErrOverflow);
			    CleanupStack::PopAndDestroy(&bundleOwner);
			    return;
			    }

			RBuf8 buffer;
			buffer.CreateL(length);
			CleanupClosePushL(buffer);
			User::LeaveIfError(queuedNotification->Store(buffer));
			aMessage.WriteL(1, buffer);
			CompleteMessage(aMessage, KErrNone);
			CleanupStack::PopAndDestroy(&buffer);
			CleanupStack::PopAndDestroy(bundleOwner); // close ref on bundle
			}
			break;


		case ECMAccessPointNotification_Cancel:
			{
			// complete the original IPC
			if (!iNotificationActive)
			    {
			    // nothing to cancel
			    CompleteMessage(aMessage, KErrNone);
			    return;
			    }
			if (!iNotificationMessage.IsNull())
			    {
			    //TODO RZ we should really panic the client here.
			    //raising a defect to do this.
			    CompleteMessage(iNotificationMessage, KErrCancel);
			    }

			// create and send message
			TNodeCtxId us(ETierNotificationActivity, NodeId()); //Pretend to be a MM node
			__ASSERT_DEBUG(ServiceProvider()!=Messages::TNodeId::NullId(),User::Panic(KConnectionSessionPanic,EExpectedServiceProvider));
			RClientInterface::OpenPostMessageClose(us, ServiceProviderL(), TEBase::TCancel().CRef());

			// splat the queue
			iNotificationQueue.ResetAndDestroy();

			// we'll complete when we get the response.
			iNotificationMessage = aMessage;
			}
			break;

        case ECMApiExtBindIface:
        	CommsApiExtBindIfaceL(aMessage);
        	break;
		case ECMApiExtIfaceSend:
        case ECMApiExtIfaceSendReceive:
			CommsApiExtIfaceSendReceiveL(aMessage);
			break;
		case ECMApiExtIfaceClose:
			CloseExtensionInterface(aMessage);
			break;


		default:
			CompleteMessage(aMessage,KErrNotSupported);
		}
	}

void CConnectionSession::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
/**
Entry point to CConnectionSession node
*/
	{
    // void to remove build warning
    (void)aSender;

	if ( aMessage.IsMessage<TEBase::TError>() )
		{
		TUint16 activityId = address_cast<TNodeCtxId>(&aRecipient)? address_cast<TNodeCtxId>(aRecipient).NodeCtx() : KActivityNull;
        ProcessError(activityId, aMessage);
		}
	else if ( aMessage.IsMessage<TCFTierStatusProvider::TTierStatus>() )
	    {
		HandleTierStatus(aMessage);
		}
	else if ( aMessage.IsMessage<TCFTierStatusProvider::TTierNotification>() )
		{
		HandleTierNotification(aMessage);
		}
	else if ( aMessage.IsMessage<TCFFactory::TPeerFoundOrCreated>() )
		{
		TCFFactory::TPeerFoundOrCreated& msg = message_cast<TCFFactory::TPeerFoundOrCreated>(aMessage);
		AttachToTierManager(msg.iNodeId);
		}
	else if ( aMessage.IsMessage<TCFServiceProvider::TJoinComplete>() )
		{
		__ASSERT_DEBUG(aSender == iTierManager, User::Panic(KSpecAssert_ESockSSockscnctn, 8));
		CompleteMessage(iAttachMessage, KErrNone);
		}
	else
		{
		LOG(ESockLog::Printf(KESockSessDetailTag, _L8("CConnectionSession(%08x):\tReceivedL - Unexpected message"), this));
		__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockSSockscnctn, 9));
		}
	aMessage.ClearMessageId();
	}



void CConnectionSession::HandleTierStatus(const Messages::TSignatureBase& aCFMessage)
    {
	const TCFTierStatusProvider::TTierStatus& msg = message_cast<const TCFTierStatusProvider::TTierStatus>(aCFMessage);

	CRefCountOwnedParameterBundle* bundle = msg.iBundle;
	if( bundle == NULL || bundle->Ptr() == NULL)
		{
		__CFLOG_VAR((KTierTag,KConnServSubTag,_L8("Null bundle received. Discarding it.")));
		if(bundle)
			{
			bundle->Close();
			}
		return;
		}

    const CConnectionServParameterBundle& statusBundle = *static_cast<const CConnectionServParameterBundle*>(bundle->Ptr());
	if (iStatusMessage.IsNull())
		{
		_LOG_BUNDLE("Received status bundle when no outstanding request.", &statusBundle);
		// shouldn't have received this.
		__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockSSockscnctn, 10));
		// swallow it in release mode

		bundle->Close();
		return;
		}

	TInt function = iStatusMessage.Function();
	switch (function)
		{
		case ECMAccessPointStatusQuery_DoThenGetResultOrSize:
			{
			__ASSERT_DEBUG(iStatusBundle == NULL, User::Panic(KSpecAssert_ESockSSockscnctn, 11));

			TInt length = statusBundle.Length();

            const Den::RSafeMessage& safeStatusMessage(static_cast<const Den::RSafeMessage&>(iStatusMessage));
			if (length > safeStatusMessage.GetDesMaxLengthL(1))
				{
				// we're gonna need a bigger boat
				iStatusBundle = msg.iBundle;
				CompleteMessage(iStatusMessage, length);
				}
			else
				{
				RBuf8 buffer;
				TInt err = buffer.Create(length);
				if (err == KErrNone)
				    {
    				err = statusBundle.Store(buffer);
    				if (err == KErrNone)
    				    {
        				err = iStatusMessage.Write(1, buffer);
    				    }
				    }
				CompleteMessage(iStatusMessage, err);
				buffer.Close();
				msg.iBundle->Close();
				}
			}
			break;

		case ECMAccessPointStatusQuery_Cancel:
			__CFLOG_VAR((KTierTag,KConnServSubTag,_L8("Received bundle when already cancelling. Discarding it.")));
			msg.iBundle->Close();
			break;

		case ECMAccessPointStatusQuery_GetResult:
		default:
		    // Consume the bundle if we didn't expect it
			msg.iBundle->Close();
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockSSockscnctn, 12));
		}
    }


void CConnectionSession::HandleTierNotification(const Messages::TSignatureBase& aCFMessage)
    {
	const TCFTierStatusProvider::TTierNotification& msg = message_cast<const TCFTierStatusProvider::TTierNotification>(aCFMessage);

	CRefCountOwnedParameterBundle* bundle = msg.iBundle;
	if( bundle == NULL || bundle->Ptr() == NULL)
		{
		__CFLOG_VAR((KTierTag,KConnServSubTag,_L8("Null bundle received. Discarding it.")));
		if(bundle)
			{
			bundle->Close();
			}
		return;
		}

	const CConnectionServParameterBundle* newNotification = static_cast<const CConnectionServParameterBundle*>(bundle->Ptr());

	if (!iNotificationActive)
		{
		// shouldn't have received this.
		__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockSSockscnctn, 13));
		// swallow it in production mode
		// RJL log here
		msg.iBundle->Close();
		return;
		}

	TInt function = iNotificationMessage.Function();
	if (iNotificationMessage.IsNull())
		{
		iNotificationQueue.Append(msg.iBundle);
		}
	else
		{
		switch (function)
			{
			case ECMAccessPointNotification_SetupThenAwaitThenGetResultOrSize:
			case ECMAccessPointNotification_AwaitThenGetResultOrSize:
				{
				__ASSERT_DEBUG(iNotificationQueue.Count() == 0, User::Panic(KSpecAssert_ESockSSockscnctn, 14));

				TInt length = newNotification->Length();

                const Den::RSafeMessage& safeNotificationMessage(static_cast<const Den::RSafeMessage&>(iNotificationMessage));
				if (length > safeNotificationMessage.GetDesMaxLengthL(1))
					{
					// we're gonna need a bigger boat
					iNotificationQueue.Append(msg.iBundle);
					CompleteMessage(iNotificationMessage, length);
					}
				else
					{
					RBuf8 buffer;
					TInt err = buffer.Create(length);
					if (err == KErrNone)
					    {
					    err = newNotification->Store(buffer);
					    if (err == KErrNone)
					        {
        					err = iNotificationMessage.Write(1, buffer);
					        }
					    }
					CompleteMessage(iNotificationMessage, err);
    				buffer.Close();

					msg.iBundle->Close();
					}
				}
				break;

			case ECMAccessPointNotification_Cancel:
				// swallow it. RJL log here
				msg.iBundle->Close();
				break;

			case ECMAccessPointNotification_GetResult:
			default:
        		msg.iBundle->Close();
				__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockSSockscnctn, 15));
			}
		}
    }


// error from comms framework
//
void CConnectionSession::ProcessError(TUint16 aActId, Messages::TSignatureBase& aCFMessage)
	{
	TEBase::TError& inMsg = message_cast<TEBase::TError>(aCFMessage);

	if (inMsg.iMsgId == TCFFactory::TFindOrCreatePeer::Id())
	    {
	    __ASSERT_DEBUG(!iAttachMessage.IsNull(), User::Panic(KSpecAssert_ESockSSockscnctn, 16));
	    CompleteMessage(iAttachMessage, inMsg.iValue);
	    }
	else
	    {
    	TInt errCode = inMsg.iValue;
    	ProcessError(aActId, errCode);
	    }
	}


void CConnectionSession::ProcessError(TUint16 aActId, TInt aErrCode)
	{
	switch(aActId)
		{
		case ETierStatusActivity:
			{
			__ASSERT_DEBUG( ! iStatusMessage.IsNull(), User::Panic(KSpecAssert_ESockSSockscnctn, 17));
			switch(iStatusMessage.Function())
				{
				case ECMAccessPointStatusQuery_DoThenGetResultOrSize:
				case ECMAccessPointStatusQuery_Cancel:
					__ASSERT_DEBUG(iStatusBundle == 0, User::Panic(KSpecAssert_ESockSSockscnctn, 18));
					// haven't yet allocated anything..
					//  so complete with the error code.
					CompleteMessage(iStatusMessage,aErrCode);
					break;

				case ECMAccessPointStatusQuery_GetResult:
				default:
					__CFLOG_VAR((KTierTag,KConnServSubTag,_L8("Unexpected state: outstanding IRC: cancel, TCFMsg: error %d"),aErrCode));
					__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockSSockscnctn, 19));
				}
			}
			break;

		case ETierNotificationActivity:
			{
			if (iNotificationMessage.IsNull())
			    {
			    //TODO RZ This is all wrong and the bad clients need panicked
			    //raising a defect.
                iNotificationQueue.ResetAndDestroy();
			    }
			else
			    {
                switch(iNotificationMessage.Function())
                    {
                    case ECMAccessPointNotification_SetupThenAwaitThenGetResultOrSize:
                    case ECMAccessPointNotification_AwaitThenGetResultOrSize:
                    case ECMAccessPointNotification_Cancel:
                        // ok, delete queue in case we didn't already..
                        iNotificationQueue.ResetAndDestroy();
                        CompleteMessage(iNotificationMessage,aErrCode);
                        break;
    
                    case ECMAccessPointNotification_GetResult:
                    default:
                        __CFLOG_VAR((KTierTag,KConnServSubTag,_L8("Unexpected state: outstanding IRC: cancel, TCFMsg: error %d"),aErrCode));
                        __ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockSSockscnctn, 20));
                    }
			    }
			iNotificationActive = EFalse;
			}
			break;

		default:
			// eek! shouldn't be receiving anything to any other activity
			__ASSERT_DEBUG(0, User::Panic(KSpecAssert_ESockSSockscnctn, 21));
		}
	}


void CConnectionSession::StartAttachToTierManager(const RMessage2& aMessage)
	{
	if(	iNotificationActive ||
		! iNotificationMessage.IsNull() ||
		! iStatusMessage.IsNull())
		{
    	CompleteMessage(aMessage, KErrInUse);
		}

    CSockManData* globals = SockManGlobals::Get();
    const TNodeId tmfc = globals->GetPlaneFC(TCFPlayerRole(TCFPlayerRole::ETierMgrPlane));
    if (tmfc.IsNull())
        {
        LOG(ESockLog::Printf(KESockSessDetailTag, _L8("CConnectionSession(%08x):\tStartAttachToTierManager - No TierMgr FC"), this));

        // If the tier manager FC is null, and esock has completed booting something is very wrong.
        __ASSERT_DEBUG(globals->iWorkerThread && !globals->iWorkerThread->PitBoss().ModuleConfigurationComplete(), User::Panic(KSpecAssert_ESockSSockscnctn, 22));

        // Someone is trying to use the Connection Server before esock has finished booting
		ParkIfIndeterminateRequest(aMessage, KErrNotFound);
        return;
        }

    iAttachMessage = aMessage;
    LeaveServiceProvider();
    TAlwaysFindFactoryQuery query;
	RClientInterface::OpenPostMessageClose(Id(), tmfc,
		TCFFactory::TFindOrCreatePeer(TCFPlayerRole::ETierMgrPlane, TUid::Uid(aMessage.Int0()), &query).CRef());
	}

void CConnectionSession::AttachToTierManager(const TNodeId& aTierMgrFactory)
    {
	iTierManager = aTierMgrFactory;
	RClientInterface::OpenPostMessageClose(Id(), iTierManager,
		TCFServiceProvider::TJoinRequest(Id(), TCFClientType::ECtrl).CRef());
    }

void CConnectionSession::LeaveServiceProvider()
	{
	const TNodeId& sp = ServiceProvider();

	if(!sp.IsNull())
		{
		const TNodeId& selfId(NodeId());
		RClientInterface::OpenPostMessageClose(selfId, sp, TEChild::TLeft().CRef());
		iTierManager.SetNull();
		}
	}


void CConnectionSession::CommsApiExtBindIfaceL(const RMessage2& aMessage)
	{
	const Messages::TNodeId& sp = ServiceProvider();
	if(!sp.IsNull())
		{
		TSupportedCommsApiExt interfaceId = static_cast<TSupportedCommsApiExt>(aMessage.Int0());
		sp.PostTo(Id(), TOpenExtensionInterface(UniqueId(), interfaceId, aMessage));
		}
	}

void CConnectionSession::CommsApiExtIfaceSendReceiveL(const RMessage2& aMessage)
	{
	const Messages::TNodeId& sp = ServiceProvider();
	if(!sp.IsNull())
    	{
		Elements::RResponseMsg responseMsg(aMessage, aMessage.Int0(), 1, 2);
		sp.PostTo(Id(), TApiExtMsgDispatcher(UniqueId(), responseMsg));
    	}
    else
        {
        User::Leave(KErrNotReady);
        }

	}

void CConnectionSession::CloseExtensionInterface(const RMessage2& aMessage)
	{
	const Messages::TNodeId& sp = ServiceProvider();
	if(!sp.IsNull())
    	{
		TSupportedCommsApiExt interfaceId = static_cast<TSupportedCommsApiExt>(aMessage.Int0());
		sp.PostTo(Id(), TCloseExtensionInterface(UniqueId(), interfaceId, aMessage));
    	}
	}

void CConnectionSession::CancelAndCloseAllClientExtIfaces()
	{
	const Messages::TNodeId& sp = ServiceProvider();
	if(!sp.IsNull())
		{
		sp.PostTo(Id(), TCancelAndCloseAllClientExtItf(UniqueId()));
		}
	}




