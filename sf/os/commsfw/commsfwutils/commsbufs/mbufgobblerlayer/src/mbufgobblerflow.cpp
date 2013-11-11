// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Flow / Binder
//  (data plane)
//
//
//  This is a 3-plane comms layer implementation example, which has been customised to be a test layer which gobbles and releases ESOCK MBUFs.
//  The MBuf gobbling functionality can be disabled by undefining the macro SYMBIAN_COMMSFW_MBUF_GOBBLER which is specified in mbufgobblerproviders.mmp.
//  When SYMBIAN_COMMSFW_MBUF_GOBBLER is undefined, the source code specified by mbufgobblerproviders.mmp becomes a pass through layer i.e. it passes the data
//  through to the layer above or below without altering it. This makes it useful as a starting point for implementing your own layers / providers;
//  useful documentation on how to customise your own passthrough layer can be found in ..\docs\MbufGobblerLayer.doc
//
/**
 @file
 @internalComponent
*/

#include <comms-infras/ss_activities.h>
#include <comms-infras/ss_logext.h>
#include <es_mbman.h>
#include <mbufgobblerpubsub.h>
#include "mbufgobblerflow.h"
#include "mbufgobblerlog.h"

// for panics
_LIT(KPanicCategory, "MbufGobblerLayer");
enum
	{
	KPanic_DestroyReceivedBeforeUnbind = 2001
	};

_LIT8(KNodeName, "CMbufGobblerFlow");


///////////////////////////
// class CMbufGobblerFlow  //
///////////////////////////

CMbufGobblerFlow* CMbufGobblerFlow::NewL(ESock::CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConnId, ESock::CProtocolIntfBase* aProtocolIntf)
    {
    CMbufGobblerFlow* inst = new(ELeave) CMbufGobblerFlow(aFactory, aSubConnId, aProtocolIntf);
    CleanupStack::PushL(inst);
    inst->ConstructL();
    CleanupStack::Pop(inst);
    return inst;    
    }
    
CMbufGobblerFlow::CMbufGobblerFlow(ESock::CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConnId, ESock::CProtocolIntfBase* aProtocolIntf)
/**
 * Constructor.
 *
 * @param aFactory Reference to the factory which created this object.
 * @param aTheLogger The logging object, ownership is passed to this object
 */
	:ESock::CSubConnectionFlowBase(aFactory, aSubConnId, aProtocolIntf)
	{
	MBUFGOBBLER_LOG_NODE_CREATE(KMbufGobblerFlowSubTag, CMbufGobblerFlow, KNodeName, this->NodeId().Ptr());
	}

void CMbufGobblerFlow::ConstructL()
    {
#ifdef SYMBIAN_COMMSFW_MBUF_GOBBLER
    iMBufGobbler = CMBufGobbler::NewL();
#endif    
    }
    
CMbufGobblerFlow::~CMbufGobblerFlow()
	{
	MBUFGOBBLER_LOG_NODE_DESTROY(KMbufGobblerFlowSubTag, CMbufGobblerFlow, KNodeName, this->NodeId().Ptr());
	iBinders.ResetAndDestroy();
#ifdef SYMBIAN_COMMSFW_MBUF_GOBBLER	
	delete iMBufGobbler;
#endif 
	}



///////////////////////////////////////////
// Methods from CSubConnectionFlowBase:  //
///////////////////////////////////////////

ESock::MFlowBinderControl* CMbufGobblerFlow::DoGetBinderControlL()
	{
	return this;
	}


// Messages::ANode
void CMbufGobblerFlow::ReceivedL(
	const Messages::TRuntimeCtxId& aSender,
	const Messages::TNodeId& aRecipient,
	Messages::TSignatureBase& aMessage
	)
/**
Method called on incoming SCPR messages

@param aCFMessage message base
*/
    {
   
    LOGMESSAGE(KMbufGobblerFlowSubTag, KNodeName, this, aSender, aRecipient, aMessage);
    //LOG_NODE_INFO(KMbufGobblerFlowSubTag, KNodeName, *this); does not compile as flow inheritance is different to other nodes
    CSubConnectionFlowBase::ReceivedL(aSender, aRecipient, aMessage);

	// Dispatch the message locally
	if (ESock::TCFDataClient::ERealmId == aMessage.MessageId().Realm())
		{
		switch (aMessage.MessageId().MessageId())
			{

			case ESock::TCFDataClient::TStart::EId :
				{
				iSubConnectionProvider.RNodeInterface::PostMessage(
					Id(),
					ESock::TCFDataClient::TStarted().CRef()
					);
				}
				break;

			case ESock::TCFDataClient::TStop::EId :
				{
				TInt i;
				for (i=iBinders.Count()-1;i>=0;--i)
					{
				    CMbufGobblerBinder* binder = iBinders[i];
				    binder->UnbindFromLowerFlow();
					delete binder;
					iBinders.Remove(i);
					}
				iSubConnectionProvider.PostMessage(Id(), ESock::TCFDataClient::TStopped(KErrNone).CRef());
				}
				break;

			case ESock::TCFDataClient::TProvisionConfig::EId :
				{
				ESock::TCFDataClient::TProvisionConfig& aMess = Messages::message_cast<ESock::TCFDataClient::TProvisionConfig>(aMessage);
				iAccessPointConfig.Close();
				iAccessPointConfig.Open(aMess.iConfig);
				}
				break;

			case ESock::TCFDataClient::TBindTo::EId :
				{
				ESock::TCFDataClient::TBindTo& bindToReq = Messages::message_cast<ESock::TCFDataClient::TBindTo>(aMessage);
				if (bindToReq.iNodeId == Messages::TNodeId::NullId())
					{
					User::Leave(KErrNotSupported);
					}

				const Messages::TNodeId& commsId = bindToReq.iNodeId;
				CSubConnectionFlowBase* lowerFlow = Messages::mnode_cast<CSubConnectionFlowBase>(&commsId.Node());

				MFlowBinderControl* lowerBinderControl = lowerFlow->GetBinderControlL();
				ASSERT(lowerBinderControl);

				TInt i;
				for (i=0;i<iBinders.Count();++i)
					{
					// binder for each protocol will request binder for same protocol from lower binder controller using this fn.
					iBinders[i]->BindToLowerFlowL(*lowerBinderControl);
					}
				ASSERT(i); // there should be some binders!

				Messages::RClientInterface::OpenPostMessageClose(
					Id(),
					aSender,
					ESock::TCFDataClient::TBindToComplete().CRef()
					);
				}
				break;

			default:
				ASSERT(EFalse);
			}
		}
	else if (Messages::TEChild::ERealmId == aMessage.MessageId().Realm())
		{
		switch (aMessage.MessageId().MessageId())
			{
			case Messages::TEChild::TDestroy::EId :
				{
				TInt i;
				for (i=0;i<iBinders.Count();++i)
					{
					// ensure all binders unbound
					if (iBinders[i]->InUse())
						{
						//__CFLOG_0(KLogTag1, KLogTag2,_L("something is sending TDestroy to CMbufGobblerFlow before unbinding."));
						User::Panic(KPanicCategory,KPanic_DestroyReceivedBeforeUnbind);
						}
						
					// EXAMPLE CODE: cancel requests here if necessary...
					//iBinders[i]->Cancel();
					
					}
				if (i==iBinders.Count()) // all unbound
					{
					DeleteThisFlow();
					}
				}
				break;

			default:
				ASSERT(EFalse);
			}
		}
	// realm != TCFMessage::ERealmId
	else
		{
		ASSERT(EFalse);
		}
    }


///////////////////////////////////////
// Methods from MFlowBinderControl:  //
///////////////////////////////////////

ESock::MLowerControl* CMbufGobblerFlow::GetControlL(const TDesC8& aProtocol)
/**
Create and return an MLowerControl instance of the specified binder type.

Called from upper layer during binding procedure.

@param aProtocol Protocol type of the binder
@return MLowerControl instance of the protocol type
*/
	{
	ESock::MLowerControl* lowerControl = FindOrCreateBinderL(aProtocol);
	ASSERT(lowerControl);
	return lowerControl;		
	}


ESock::MLowerDataSender* CMbufGobblerFlow::BindL(const TDesC8& aProtocol, ESock::MUpperDataReceiver* aReceiver, ESock::MUpperControl* aControl)
	{
	CMbufGobblerBinder* binder = FindOrCreateBinderL(aProtocol);
	ASSERT(binder);
	binder->BindToUpperL( *aReceiver, *aControl );

	iSubConnectionProvider.RNodeInterface::PostMessage(
		Id(),
		ESock::TCFControlProvider::TActive().CRef()
		);

	return binder;
	}

void CMbufGobblerFlow::Unbind( ESock::MUpperDataReceiver* aReceiver, ESock::MUpperControl* aControl)
	{
	ASSERT(aReceiver);
	ASSERT(aControl);
	TInt i;
	TInt numberUnbound=0;
	for (i=0;i<iBinders.Count();++i)
		{
		numberUnbound += (iBinders[i]->UnbindFromUpper(*aReceiver,*aControl) ? 1 : 0);
		}
	ASSERT(i); // there should be some binders!
	ASSERT(numberUnbound<=1); // only 1 unbind should have happened

	iSubConnectionProvider.RNodeInterface::PostMessage(
		Id(),
		ESock::TCFControlProvider::TIdle().CRef()
		);
	}

ESock::CSubConnectionFlowBase* CMbufGobblerFlow::Flow()
	{
	return this;
	}


/////////////////
// Own methods //
/////////////////

CMbufGobblerBinder* CMbufGobblerFlow::FindOrCreateBinderL(const TDesC8& aProtocol)
	{
	
	// EXAMPLE CODE, should you want your: perhaps your protocol to work with IPv4
	//if (aProtocol.Compare(KIp4ProtocolName))
	//	{  // only work with IPv4
	//	User::Leave(KErrNotSupported);
	//	}

	for (TInt i=0;i<iBinders.Count();++i)
		{
		if(iBinders[i]->ProtocolName() == aProtocol)
			{
			return iBinders[i];
			}
		}
	// not found.. create it.
	CMbufGobblerBinder* newBinder = CMbufGobblerBinder::NewL(aProtocol);
	CleanupStack::PushL(newBinder);
	iBinders.AppendL(newBinder);
	CleanupStack::Pop(newBinder);
	return newBinder;
	}
	
    

	

//##################################################################################################
	
/////////////////////////////
// class CMbufGobblerBinder  //
/////////////////////////////


////////////////////
// My constructor //
////////////////////

CMbufGobblerBinder::CMbufGobblerBinder(const TDesC8& aProtocolName):
	iLowerControl(NULL),
	iLowerDataSender(NULL),
	iUpperControl(NULL),
	iUpperDataReceiver(NULL),
	iProtocolName(aProtocolName)
	{}

CMbufGobblerBinder* CMbufGobblerBinder::NewL(const TDesC8& aProtocolName)
	{
	CMbufGobblerBinder* inst = new(ELeave) CMbufGobblerBinder(aProtocolName);
	CleanupStack::PushL(inst);
	inst->ConstructL();
	CleanupStack::Pop(inst);
	return inst;
	}

void CMbufGobblerBinder::ConstructL()
	{
	MBUFGOBBLER_TEST_DATA_INIT
#if 0
	// EXAMPLE CODE - set up everything you need to in this method.
	//  Perhaps you have some kind of test control interface using pub/sub.. if so you'd do something like below..
	//   Diff this file with networking/netperf/delaymeterproto/src/delaymeterflow.cpp for a full working implementation of such a control interface...
	CActiveScheduler::Add(this);
	DefinePubSubKeysL();
	// watch for incoming commands
	User::LeaveIfError(iProperty.Attach(TUid::Uid(KDelayMeterControlLevel), KCommandToDelayMeter));
	iProperty.Subscribe(iStatus);
	SetActive();
#endif	
	}

/*virtual*/
CMbufGobblerBinder::~CMbufGobblerBinder()
	{
	}



////////////////////////////////////////
// Methods from ESock::MLowerControl: //
////////////////////////////////////////

TInt CMbufGobblerBinder::GetName(TDes& aName)
	{
	TBuf16<10> tmp;
	tmp.Copy(ProtocolName());
	aName.Format(_L("mbufgobbler[%S][0x%08x]"), &tmp, this);
	
	return KErrNone;
	}

TInt CMbufGobblerBinder::BlockFlow(TBlockOption aOption)
	{
	if (iLowerControl==NULL)
		{
		return KErrNotReady;
		}
	return iLowerControl->BlockFlow(aOption) ;
	}

TInt CMbufGobblerBinder::GetConfig(TBinderConfig& aConfig)
	{
	if (iLowerControl==NULL)
		{
		return KErrNotReady;
		}
	return iLowerControl->GetConfig(aConfig) ;
	}

TInt CMbufGobblerBinder::Control(TUint aLevel, TUint aName, TDes8& aOption)
	{
	// Pass it on..
	if (iLowerControl==NULL)
		{
		return KErrNotReady;
		}
	return iLowerControl->Control(aLevel,aName,aOption);
	}
	

///////////////////////////////////////////
// Methods from ESock::MLowerDataSender: //
///////////////////////////////////////////

ESock::MLowerDataSender::TSendResult CMbufGobblerBinder::Send(RMBufChain& aData)
	{
	// EXAMPLE NOTE:
	//  This is where a protocol implementation will do its work on outgoing data.

    MBUFGOBBLER_TEST_POINT(KBinderSend,KErrNone)

	if (iLowerControl==NULL)
		{
		return ESendBlocked; // returning this obliges us to send an unblock later..
							 // so perhaps it'd be better to just swallow the packet?
		}
	return iLowerDataSender->Send(aData);
	}

////////////////////////////////////////
// Methods from ESock::MUpperControl: //
////////////////////////////////////////

/*virtual*/
void CMbufGobblerBinder::StartSending()
	{
	if (iUpperControl)
		{
		iUpperControl->StartSending();
		}
	else
		{
		ASSERT(1); // to allow setting a breakpoint
		//__CFLOG_0(KLogTag1, KLogTag2,_L("CMbufGobblerBinder::StartSending: upper control not yet known!"));
		}
	}

/*virtual*/
void CMbufGobblerBinder::Error(TInt anError)
	{
	if (iUpperControl)
		{
		iUpperControl->Error(anError);
		}
	else
		{
		ASSERT(1); // to set a breakpoint
		//__CFLOG_0(KLogTag1, KLogTag2,_L("CMbufGobblerBinder::Error: upper control not yet known!"));
		}
	}


/////////////////////////////////////////////
// Methods from ESock::MUpperDataReceiver: //
/////////////////////////////////////////////

/*virtual*/
void CMbufGobblerBinder::Process(RMBufChain& aData)
	{
	// EXAMPLE NOTE:
	//  This is where a protocol implementation will do its work on incoming data.

    MBUFGOBBLER_TEST_POINT(KBinderReceive,KErrNone)

	if (iUpperDataReceiver == NULL)
		{
		// Why is the guy below still sending data to me when I'm not bound above?
		//   Try to ignore it
		ASSERT(1); // so a breakpoint can be set if necessary
		//__CFLOG_0(KLogTag1, KLogTag2,_L("CMbufGobblerBinder::Process: incoming traffic discarded as upper data receiver not (or no longer) set"));
		return;
		}
	iUpperDataReceiver->Process(aData);
	}


//////////////////////////
// and my own methods.. //
//////////////////////////

// called by layer above calling my flow's BindL
void CMbufGobblerBinder::BindToUpperL(MUpperDataReceiver& aUpperDataReceiver, MUpperControl& aUpperControl)
	{
	if(iUpperDataReceiver || iUpperControl) {User::Leave(KErrInUse);}
	iUpperDataReceiver=&aUpperDataReceiver;
	iUpperControl=&aUpperControl;
    MBUFGOBBLER_TEST_POINT(KBind,KErrNone)
	}

// called by layer above calling my flow's Unbind. Returns ETrue if unbind happened here, EFalse otherwise
TBool CMbufGobblerBinder::UnbindFromUpper(MUpperDataReceiver& aUpperDataReceiver, MUpperControl& aUpperControl)
	{
	if(&aUpperDataReceiver == iUpperDataReceiver && &aUpperControl == iUpperControl)
		{
		iUpperDataReceiver=0;
		iUpperControl=0;
		return ETrue;
		}
	return EFalse;
	}

// called by my flow receiving a BinderRequest
void CMbufGobblerBinder::BindToLowerFlowL(ESock::MFlowBinderControl& aLowerBinderControl)
	{
//	__CFLOG_0(KLogTag1, KLogTag2, _L("CMbufGobblerBinder::BindToLowerFlowL")); 
	if(iLowerControl || iLowerDataSender)
		{
		User::Leave(KErrInUse);
		}
	
	iBinderControl = &aLowerBinderControl;
	iLowerControl = aLowerBinderControl.GetControlL(ProtocolName());
	iLowerDataSender = aLowerBinderControl.BindL(ProtocolName(), this, this);
	}

void CMbufGobblerBinder::UnbindFromLowerFlow()
    {
    if (!iBinderControl)
        return;
    
    iBinderControl->Unbind(this, this);
    iBinderControl = NULL;

    iLowerControl = NULL;
    iLowerDataSender = NULL;
    }

const TDesC8& CMbufGobblerBinder::ProtocolName() const
	{
	return iProtocolName;
	}

#ifdef SYMBIAN_COMMSFW_MBUF_GOBBLER 

CMBufGobbler::CMBufGobbler():
    CActive(CActive::EPriorityStandard)
    {
    }   
    
CMBufGobbler* CMBufGobbler::NewL()
    {
    CMBufGobbler* inst = new(ELeave) CMBufGobbler;
    CleanupStack::PushL(inst);
    inst->ConstructL();
    CleanupStack::Pop(inst);
    return inst;
    }

void CMBufGobbler::ConstructL()
    {
    iChain.AllocL(128);
    
    CActiveScheduler::Add(this);

    TInt result = RProperty::Define(TUid::Uid(EMBufGobbler), EAdjustNumberOfMBufsRemainingInPool, RProperty::EInt);
    // Only want 1 instance of a MBufGobbler Layer, so just leave if KErrAlreadyExists returned
    User::LeaveIfError(result);

    // watch for incoming commands
    User::LeaveIfError(iProperty.Attach(TUid::Uid(EMBufGobbler), EAdjustNumberOfMBufsRemainingInPool));

    iProperty.Subscribe(iStatus);
    SetActive();
    }

/*virtual*/ CMBufGobbler::~CMBufGobbler()
    {
    if (IsActive())
        {
        Cancel();   
        }

    iChain.Free();
    
    TInt result = RProperty::Delete(TUid::Uid(EMBufGobbler), EAdjustNumberOfMBufsRemainingInPool);  
    if (result != KErrNone)
        {
        RDebug::Print(_L("CMBufGobbler::~CMBufGobbler() %d"), result);
        }
    }

/*virtual*/ void CMBufGobbler::DoCancel()
    {
    iProperty.Cancel();
    }

/*virtual*/ void CMBufGobbler::RunL()
    {
    // Resubscribe to ensure that next pub/sub notification is picked up
    iProperty.Subscribe(iStatus);
    SetActive();
    
    TInt request=EGobbleAllMBufs;
    TInt pubSubRet = iProperty.Get(request);
    if (pubSubRet == KErrNone)
        {
        switch(request)
            {
            case EGobbleAllMBufs:
                {
                RMBufAllocator allocator;
                RMBufChain chain;
                TInt size = allocator.NextMBufSize(0);
                while (size != KErrNotFound)
                    {
                    TInt ret = KErrNone;
                    while (ret == KErrNone)
                        {
                        ret = chain.Alloc(size);
                        if (ret==KErrNone )
                            {
                            iChain.Append(chain);
                            }
                        }
                    size = allocator.NextMBufSize(size);
                    }
                TInt length = iChain.Length();
                RDebug::Print(_L("Out of MBuf Memory... Total MBuf memory in use %d"), length);
                TInt numBufs = iChain.NumBufs();
                RDebug::Print(_L("Out of MBuf Memory... Total MBufs in use %d"), numBufs);
                break;
                }
            case EReleaseAllMBufs:
                {
                if(!iChain.IsEmpty())
                     {
                     iChain.Free();
                     ASSERT(!iChain.Length());
                    }
                break;
                }
            case EReleaseASingleMBuf:
                {            
                TInt length = iChain.Length();
                if (length != 0)
                    {
                    TInt trimOffset = length - iChain.Last()->Size();
                    iChain.TrimEnd(trimOffset);
                    }
    
                length = iChain.Length();
                RDebug::Print(_L("MBufMemory De-Allocated... Total MBuf memory in use %d"), length);
                break;
                }
            default:
                {
                RDebug::Print(_L("CMBufGobbler::RunL(), invalid request %d"), request);
                break;
                }
            }
        }
    else
        {
        RDebug::Print(_L("Attempt to process MBufGobbler publish/subscribe failed with value for  %d"), pubSubRet);
        }
    }

#endif
