// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include <bluetooth/logger.h>
#include <comms-infras/eintsock.h>
#include "CBnepBridge.h"
#include "CBnepChannelController.h"
#include "CBnepLink.h"
#include "RBnepSetupConnectionRequestControl.h"
#include "RBnepSetupConnectionResponseControl.h"
#include "bneputils.h"
#include "RBnepFilterNetTypeSetRequestControl.h"
#include "RBnepFilterNetTypeResponseControl.h"
#include "RBnepFilterMultiAddrSetRequestControl.h"
#include "RBnepFilterMultiAddrResponseControl.h"

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
#include "panmessages.h"

using namespace ESock;
using namespace Messages;

#ifdef ESOCK_EXTLOG_ACTIVE
_LIT8(KBnepSubTag, "bnep");
#endif

#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_BNEP);
#endif

/**
Implement cleanup for an RBnepControl object allocated on the heap.
*/
void CleanupControl(TAny* aControl)
    {
    LOG_STATIC_FUNC
    RBnepControl* control = static_cast<RBnepControl*>(aControl);
    control->Free();
    delete control;
    }

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
CBnepChannelController::CBnepChannelController (const TNodeId& aNotify, CPacketDriverOwner& aPktDrvOwner)
    : iPanConnectionNotify(aNotify), iPktDrvOwner(aPktDrvOwner)
    {
    NM_LOG_NODE_CREATE(KBnepSubTag, CBnepChannelController);
    CONNECT_LOGGER
    LOG_FUNC
    iPktDrvOwner.Open();
    }

#else
// !SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
CBnepChannelController::CBnepChannelController (MPanConnectionNotify& aNotify)
        :iNotify(aNotify)
    {
    LOG_FUNC
    }
#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

CBnepChannelController::~CBnepChannelController()
    {
    /**
       Close all objects that we're reponsible for - including the socket.
       @internalComponent
    */
    LOG_FUNC
    if(iLink)
        {
        delete iLink;
        //  iLink must be reset to zero after deletion to prevent a later panic.
        iLink = NULL;
        }


    iProceedCallBack->Cancel();
    delete iProceedCallBack;

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    NM_LOG_NODE_DESTROY(KBnepSubTag, CBnepChannelController);
    CLOSE_LOGGER
    iPktDrvOwner.Close();
#endif
    }

void CBnepChannelController::BnepRoleRequestFromLocalDevice (TBluetoothPanRole aRequestedLocalRole, TBluetoothPanRole aRequestedRemoteRole)
    {
    /**
       Call from the agent to request that we set a certain role.
       @internalComponent
    */
    LOG_FUNC
    iLink->SuspendDataTransfer();

	TRAPD(err, CreateAndQueueConnectionSetupRequestL(aRequestedLocalRole, aRequestedRemoteRole));
	if(!err)
		{
    	iLink->Proceed();
		}
	else
		{
		iLink->ResumeDataTransfer();
		}
    }

void CBnepChannelController::SetUplinkAccessAllowedForBnepLink(TBool aAllowed)
	{
	iLink->SetUplinkAccessAllowedForBnepLink(aAllowed);
	}
	

/**
Response to a remote request for a given set of roles -- generate control
packet and pass it back to the remote device.
@internalComponent
*/
void CBnepChannelController::BnepRoleResponseFromLocalDevice (TBnepSetupConnectionResponseMessage aRoleResponseCode)
    {
	// This may fail if it cannot allocate the response.  However,
	// we will just continue and do the same as if it had succeeded
	// as we won't get another prompt to continue.  The remote may
	// resend its ConnectionSetupRequest when we don't respond, or
	// may disconnect the link.
	TRAP_IGNORE(CreateAndQueueConnectionSetupResponseL(aRoleResponseCode));

    // All control processing has been suspended while awaiting this event, 
    // so we can now free things up again.
    SetAwaitingResponse(EFalse);
    iLink->ResumeDataTransfer();	// enable the link for data transfer	
    
    if(EOperationSuccessful == aRoleResponseCode)
        {
        //In case this is the first negotiation, we only want to trigger LinkLayerUp if the 
	    //negotiation is successful
        iLink->RemoteDeviceReady();
        }
        
	//If iProceedCallBack is already active then BNEP must be processing the next command inside 
	//the same frame. We don't have to don't anything here. The callback will complete when all the
	//commands are processed.
    if(!iProceedCallBack->IsActive())
    	{
    	__ASSERT_DEBUG(!iProceedCallBack->IsAdded(), BnepUtils::Panic(Bnep::ECallBackAlreadyAdded));
		CActiveScheduler::Add(iProceedCallBack);
	    iProceedCallBack->CallBack();
    	}
    }

TInt CBnepChannelController::ProceedCb(TAny* aPtr)
	{
	static_cast<CBnepChannelController*>(aPtr)->Proceed();
	
	return KErrNone;
	}
	
void CBnepChannelController::Proceed()
	{
	iProceedCallBack->Deque();
    // Carry on processing any outstanding controls and data transfers.
    iLink->Proceed();
    }

/**
   Close this channel. This has the effect of deleting the coupled CBnepLink.
   @internalComponent
*/
void CBnepChannelController::Close ()
    {
    LOG_FUNC
    delete this;
    }

/**
   Create the channel controller and the link object.
   Ownership of all these objects is a little strange -- the channel controller
   owns the underlying link, and the channel controller itself is owned by the
   agent (in another dll...).  Destruction paths can therefore look a little
   confusing.
   @internalComponent
*/
void CBnepChannelController::ConstructL (CBnepBridge& aBridge, RInternalSocket& aConnectedSocket)
    {
    LOG_FUNC
    iLink = CBnepLink::NewL(aBridge, aConnectedSocket, *this);
    
    iProceedCallBack = new (ELeave) CAsyncCallBack(KBnepProceedCallBackPriority);
    TCallBack proceedCb(ProceedCb, this);
    iProceedCallBack->Set(proceedCb);
    //Remove the callback from active scheduler whenever it is not used because it is
    //used only in role request and has a high priority.
    iProceedCallBack->Deque();
    }

/**
   Having received a SetupConnectionRequest from a remote device, the controller 
   must hand off the request to the agent. This may be asynchronous, in which case
   the method returns immediately, having suspended further control processing 
   until the agent responds. However, the agent may call back into the controller 
   synchronously. This has implications for how controls are processed by the link.
   @internalComponent
*/
void CBnepChannelController::Handle (RBnepSetupConnectionRequestControl* aSetupRequest)
    {
    LOG_FUNC
    __ASSERT_DEBUG(aSetupRequest,BnepUtils::Panic(Bnep::ENullPointer));
    if(aSetupRequest)
        {
        TUUID localRole;
        TUUID remoteRole;
        TInt err1 = aSetupRequest->LocalRole(localRole);
        TInt err2 = aSetupRequest->RemoteRole(remoteRole);
        // The control object must be freed and deleted now, before the request is 
        // passed out, to ensure that the controller is re-entrant and doesn't leak memory.
        aSetupRequest->Free();
        delete aSetupRequest;
        if(!err1 && !err2)
            {
            // Suspend further link processing for the time being.
            SetAwaitingResponse(ETrue);
                        
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
            TPanMessage::TRoleRequestFromRemoteDevice msg(localRole, remoteRole);
            RClientInterface::OpenPostMessageClose(Id(),iPanConnectionNotify,msg);
#else
            iNotify.BnepRoleRequestFromRemoteDevice(localRole, remoteRole);
#endif
            }
        else
            {
			// The UUIDSize is not within spec, so we need to tell the remote end.
			// This may leave if we fail to allocate the new response.  Unfortunately
			// there's not a lot we can do in that circumstance.  Just drop our response
			// and wait for the remote to timeout.
			TRAP_IGNORE(CreateAndQueueConnectionSetupResponseL(EInvalidServiceUuidSize));
            }
        }
    }

/**
   The remote device has sent a response to our Setup request. If it has 
   been successful we can resume data transfer operations on the link. 
   The agent is notified of the response.
   @param aSetupResponse A control containing the remote end's response to our 
   request to set up a connection.
   @internalComponent
*/
void CBnepChannelController::Handle (RBnepSetupConnectionResponseControl* aSetupResponse)
    {
    LOG_FUNC
    __ASSERT_DEBUG(aSetupResponse,BnepUtils::Panic(Bnep::ENullPointer));
    if(aSetupResponse)
        {
        TBnepSetupConnectionResponseMessage responseCode = EOperationSuccessful;
        TRAPD(err,aSetupResponse->ConnectionSetupResponseL(responseCode));
		
		if(!err)
			{
			iLink->ResumeDataTransfer();
	        if(EOperationSuccessful == responseCode)
	            {
	            //In case this is the first negotiation, we only want to trigger LinkLayerUp if the 
	            //negotiation is successful
	            iLink->RemoteDeviceReady();
	            }
	        // The control object must be freed and deleted now, before the request is 
	        // passed out, to ensure that the controller is re-entrant and doesn't leak memory.            
	        aSetupResponse->Free();
	        delete aSetupResponse;        
	        
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
            TPanMessage::TRoleResponseFromRemoteDevice msg(responseCode);
            RClientInterface::OpenPostMessageClose(Id(),iPanConnectionNotify,msg);	        
#else
	        iNotify.BnepRoleResponseFromRemoteDevice(responseCode);
#endif
			}
		else
			{
			// They sent us bobbins, just drop it.
	        aSetupResponse->Free();
	        delete aSetupResponse; 
			}
        }
    }

/**
   Handle an attempt, by a remote device, to set network packet type filters.
   @param aFilterNetTypeSetRequest A control containing a network packet type 
   filter set message.
   @internalComponent
*/
void CBnepChannelController::Handle (RBnepFilterNetTypeSetRequestControl* aFilterNetTypeSetRequest)
    {
    LOG_FUNC
    __ASSERT_DEBUG(aFilterNetTypeSetRequest,BnepUtils::Panic(Bnep::ENullPointer));

    if(aFilterNetTypeSetRequest)
        {
        TFilterNetTypeResponse response;        
        // Specification requires that we check the validity of the filter request 
        // before it is implementeted in BNEP.                                     
        TUint16 typeListSize ;
        TInt err1 = aFilterNetTypeSetRequest->NetworkProtocolTypeListLength(typeListSize);
        if(!err1)
            {
            LOG1(_L8("NetworkProtocolTypeListLength = %d"),typeListSize); 
            if((2*KMaxNetTypeFilters*KSizeOfNetType) < typeListSize)
                {
                response = EFilterNetTypeTooManyFilters;
                }
            else
                {
                CNetTypeFilterTable* netTypeFilterTable = new CNetTypeFilterTable;

                if(netTypeFilterTable)
                    {
                    TUint index = 0;
                    do
                        {
                        TUint16 startValue;
                        TUint16 endValue;
                        err1 = aFilterNetTypeSetRequest->NetworkProtocolTypeRange(startValue, endValue, index);
                        index++;
                        LOG2(_L8("Net type filter request: startValue: %d , endValue: %d"),startValue, endValue);
                        if( KErrGeneral == err1 )
                            {
                            // This one is because the advertised length doesn't match what's 
                            // actually in the packet.
                            response = EFilterNetTypeUnsupportedRequest;
                            break;
                            }
                        if( KErrArgument == err1 )
                            {
                            // This one means they've somehow got their start and end 
                            // addresses in the wrong order.
                            response = EFilterNetTypeInvalidNetworkingProtocolTypeRange;
                            break;
                            }
                        if(KErrNotFound != err1)
                            {
                            TInt err2 = netTypeFilterTable->Insert(startValue, endValue);
                            if( KErrArgument == err2 )
                                {
                                // This one means they've somehow got their start and end 
                                // addresses in the wrong order, and we didn't catch it earlier.
                                response = EFilterNetTypeInvalidNetworkingProtocolTypeRange;
                                break;
                                }  
                            if( KErrTooBig == err2 )
                                {
                                // This one means that they've asked for more filters than 
                                // we currently support.
                                response = EFilterNetTypeTooManyFilters;
                                break;
                                }  
                            response = EFilterNetTypeOperationSuccessful;  
                            }
                        else
                            {
                            netTypeFilterTable->Reset(); // This is going to be an empty table
                            response = EFilterNetTypeOperationSuccessful; // This is supported
                            err1 = KErrEof; // Nothing more to be done
                            }
                        }while( KErrEof != err1 );
                    if(EFilterNetTypeOperationSuccessful == response)
                        {
                        // We can now adopt the new filter table.
                        // The link will own it and be responsible for it's deletion.
                        iLink->Install(netTypeFilterTable);
                        }
                    else
                        {
                        // That request failed for some reason, so drop the new table.
                        delete netTypeFilterTable;
                        }
                    }
                else
                    {
                    // If we get here, then we have failed to construct a CNetTypeFilterTable. 
                    // There is nothing in the protocol that allows us to pass back a generic 
                    // "Request Unsuccessful" message, so the nearest we could come up with was 
                    // "Too Many Filters", which is a bit more truthful that "Unsupported Request".
                    response = EFilterNetTypeTooManyFilters;
                    }
                }
            }
        else
            {
            response = EFilterNetTypeUnsupportedRequest;
            }
        // The control object must be freed and deleted now, before the response is 
        // generated, to ensure that the controller is re-entrant and doesn't leak memory.
        aFilterNetTypeSetRequest->Free();
        delete aFilterNetTypeSetRequest;

		// This may leave if we fail to allocate the new response.  Unfortunately
		// there's not a lot we can do in that circumstance.  Just drop our response
		// and wait for the remote to timeout.
		TRAP_IGNORE(CreateAndQueueFilterNetTypeResponseL(response));
        }
    }

/**
   This overloaded method is a placeholder for the code that will handle 
   responses from the remote end after BNEP hass emitted a Filter Net Type 
   Request.This version of BNEP does not emit these requests.
   @internalComponent
*/
void CBnepChannelController::Handle (RBnepFilterNetTypeResponseControl* aFilterNetTypeResponse)
    {
    LOG_FUNC
    __ASSERT_DEBUG(aFilterNetTypeResponse,BnepUtils::Panic(Bnep::ENullPointer));
    // We don't do anything with this yet
    if(aFilterNetTypeResponse)
        {
        aFilterNetTypeResponse->Free();
        delete aFilterNetTypeResponse;
        }
    }

/**
   Handle an attempt by a remote device to set multicast adddress filters
   @internalComponent
*/
void CBnepChannelController::Handle (RBnepFilterMultiAddrSetRequestControl* aFilterMultiAddrSetRequest)
    {
    LOG_FUNC
    __ASSERT_DEBUG(aFilterMultiAddrSetRequest,BnepUtils::Panic(Bnep::ENullPointer));

    if(aFilterMultiAddrSetRequest)  
        {
        TInt err1;
        TBnepBTDevAddr startAddr;
        TBnepBTDevAddr endAddr;
        TFilterMultiAddrResponse response;
        // Specification requires that we check the validity of the filter request 
        // before it is implementeted in BNEP.                                     
        TUint16 typeListSize = aFilterMultiAddrSetRequest->MulticastAddressListLength();
        LOG1(_L8("MulticastAddressListLength = %d"),typeListSize); 
        if((2*KMaxMultiAddrFilters*KSizeOfMultiAddr) < typeListSize)
            {
            LOG(_L8("Too many filters"));
            response = EFilterMultiAddrTooManyFilters;
            }
        else
            {
            CMultiAddrFilterTable* multiAddrFilterTable = new CMultiAddrFilterTable;

            if(multiAddrFilterTable)
                {
                TUint index = 0;
                do
                    {
                    err1 = aFilterMultiAddrSetRequest->MultiAddrRange(startAddr, endAddr, index);
                    if(KErrArgument == err1)
                        {
                        response = EFilterMultiAddrUnsupportedRequest;
                        break; // Stop processing this request
                        }
                    if(KErrNotFound != err1)
                        {
                        TInt err2 = multiAddrFilterTable->Insert(startAddr, endAddr);
                        if( KErrArgument == err2 )
                            {
                            response = EFilterMultiAddrUnsupportedRequest;
                            break; // Stop processing this request
                            }  
                        if( KErrTooBig == err2 )
                            {
                            response = EFilterMultiAddrTooManyFilters;
                            break; // Stop processing this request
                            }  
                        response = EFilterMultiAddrOperationSuccessful; // Must be good if we got here.
                        ++index;
                        }
                    else
                        {
                        multiAddrFilterTable->Reset(); // This is going to be an empty table
                        response = EFilterMultiAddrOperationSuccessful; // This is supported
                        err1 = KErrEof; // Nothing more to be done
                        }
                    } while( KErrEof != err1 );
                if(EFilterMultiAddrOperationSuccessful == response)
                    {
                    // We can adopt the new filter table.
                    // The link will own it and be responsible for it's deletion.
                    iLink->Install(multiAddrFilterTable);
                    }
                else
                    {
                    // That request failed for some reason, so drop the new table unused.
                    delete multiAddrFilterTable;
                    }
                }
            else
                {
                // If we get here, then we have failed to construct a CMultiAddrFilterTable. 
                // There is nothing in the protocol that allows us to pass back a generic 
                //  "Request Unsuccessful" message, so the nearest we could come up with was 
                //  "Too Many Filters", which is a bit more truthful that "Unsupported Request".
                response = EFilterMultiAddrTooManyFilters;
                }
            }
        // The control object must be freed and deleted now, before the request is 
        // honoured, to ensure that the controller is re-entrant and doesn't leak memory.
        aFilterMultiAddrSetRequest->Free();
        delete aFilterMultiAddrSetRequest;    

		// This may leave if we fail to allocate the new response.  Unfortunately
		// there's not a lot we can do in that circumstance.  Just drop our response
		// and wait for the remote to timeout.
		TRAP_IGNORE(CreateAndQueueFilterMultiAddrResponseL(response));
        }
    }

/**
   This overloaded method is a placeholder for the code that will handle 
   responses from the remote end after BNEP hass emitted a Multicast Address Filter 
   Request.This version of BNEP does not emit these requests.
   @internalComponent
*/
void CBnepChannelController::Handle (RBnepFilterMultiAddrResponseControl* aFilterMultiAddrResponse)
    {
    LOG_FUNC
    __ASSERT_DEBUG(aFilterMultiAddrResponse,BnepUtils::Panic(Bnep::ENullPointer));
    // We don't do anything with this yet.
    if(aFilterMultiAddrResponse)
        {
        aFilterMultiAddrResponse->Free();
        delete aFilterMultiAddrResponse;
        }
    }

/**
   Handle a control that we don't understand.
   @internalComponent
*/
void CBnepChannelController::HandleUnknownCommandPacket (RBnepControl* aControl)
    {
    LOG_FUNC
    __ASSERT_DEBUG(aControl,BnepUtils::Panic(Bnep::ENullPointer));

    if(aControl)
        {
        TUint8 controlType = static_cast<TUint8>(aControl->ControlType());
        // The control object must be freed and deleted now, before the response is 
        // generated, to ensure that the controller is re-entrant and doesn't leak memory.      
        aControl->Free();
        delete aControl;
		
		// This may leave if we fail to allocate the new response.  Unfortunately
		// there's not a lot we can do in that circumstance.  Just drop our response
		// and wait for the remote to timeout.
		TRAP_IGNORE(CreateAndQueueNotUnderstoodResponseL(controlType));
		}
    }

void CBnepChannelController::CreateAndQueueConnectionSetupRequestL(TBluetoothPanRole aRequestedLocalRole, TBluetoothPanRole aRequestedRemoteRole)
	{
	LOG_FUNC

    RBnepSetupConnectionRequestControl* controlSetup = new(ELeave)RBnepSetupConnectionRequestControl();
	CleanupStack::PushL(TCleanupItem(CleanupControl, controlSetup));

    controlSetup->InitL();

    TUUID localRole(aRequestedLocalRole);
    TUUID remoteRole(aRequestedRemoteRole);
	controlSetup->SetRolesL(localRole, remoteRole);

	CleanupStack::Pop(controlSetup);
    iLink->QueueOnOutput(controlSetup);
	}

void CBnepChannelController::CreateAndQueueConnectionSetupResponseL(TBnepSetupConnectionResponseMessage aRoleResponseCode)
	{
	LOG_FUNC

	RBnepSetupConnectionResponseControl* responseControl = new(ELeave)RBnepSetupConnectionResponseControl();
	CleanupStack::PushL(TCleanupItem(CleanupControl, responseControl));

	responseControl->InitL();
	responseControl->SetConnectionSetupResponseL(aRoleResponseCode);

	CleanupStack::Pop(responseControl);
	iLink->QueueOnOutput(responseControl);
	}

void CBnepChannelController::CreateAndQueueNotUnderstoodResponseL(TUint8 aControlType)
	{	
	LOG_FUNC

    RBnepNotUnderstoodResponseControl* responseControl = new(ELeave)RBnepNotUnderstoodResponseControl();
	CleanupStack::PushL(TCleanupItem(CleanupControl, responseControl));

	responseControl->InitL();
	responseControl->SetUnknownControlType(aControlType);

	CleanupStack::Pop(responseControl);
	iLink->QueueOnOutput(responseControl);
	}

void CBnepChannelController::CreateAndQueueFilterNetTypeResponseL(TFilterNetTypeResponse aResponse)
	{	
	LOG_FUNC

	RBnepFilterNetTypeResponseControl* responseControl = new(ELeave)RBnepFilterNetTypeResponseControl();
	CleanupStack::PushL(TCleanupItem(CleanupControl, responseControl));

	responseControl->InitL();
	responseControl->SetNetTypeResponseL(aResponse);

	CleanupStack::Pop(responseControl);
	iLink->QueueOnOutput(responseControl);
	}

void CBnepChannelController::CreateAndQueueFilterMultiAddrResponseL(TFilterMultiAddrResponse aResponse)
	{	
	LOG_FUNC

	RBnepFilterMultiAddrResponseControl* responseControl = new(ELeave)RBnepFilterMultiAddrResponseControl();
	CleanupStack::PushL(TCleanupItem(CleanupControl, responseControl));

	responseControl->InitL();
	responseControl->SetMultiAddrResponseL(aResponse);

	CleanupStack::Pop(responseControl);
	iLink->QueueOnOutput(responseControl);
	}

/**
   Create a command parser/channel controller for a specific BNEP link.
   The controller is owned by the agent objects.
   @param aBridge The packet router
   @param aConnectedSocket The socket to run the link over (already connected to the remote device)
   @internalComponent
*/
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
CBnepChannelController* CBnepChannelController::NewL (CBnepBridge& aBridge, RInternalSocket& aConnectedSocket, const TNodeId& aNotify, CPacketDriverOwner& aPktDrvOwner)
    {
    LOG_STATIC_FUNC
    CBnepChannelController* self = new(ELeave) CBnepChannelController(aNotify, aPktDrvOwner);
    CleanupStack::PushL(self);
    self->ConstructL(aBridge, aConnectedSocket);
    CleanupStack::Pop(self);
    return(self);
    }

#else
// !SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
CBnepChannelController* CBnepChannelController::NewL (CBnepBridge& aBridge, RInternalSocket& aConnectedSocket, MPanConnectionNotify& aNotify)
    {
    LOG_FUNC
    CBnepChannelController* self = new(ELeave) CBnepChannelController(aNotify);
    CleanupStack::PushL(self);
    self->ConstructL(aBridge, aConnectedSocket);
    CleanupStack::Pop(self);
    return(self);
    }
#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

/**
   Process incoming BNEP control commands.
   This upcall is called repeatedly by the packet parsing code if multiple
   commands are present in the packet.
   @internalComponent
*/
void CBnepChannelController::Execute (RBnepControl* aControl)
    {
    LOG_FUNC
    __ASSERT_DEBUG(aControl,BnepUtils::Panic(Bnep::ENullPointer));

    if(aControl)
        {
        switch (aControl->ControlType())
            {                                                                                                  
            case EBnepSetupConnectionRequestMessage:  
                LOG1(_L8("CBnepChannelController[%x]: EBnepSetupConnectionRequestMessage"), this);         	
                Handle(static_cast<RBnepSetupConnectionRequestControl*>(aControl));
                break;

            case EBnepSetupConnectionResponseMessage:
                LOG1(_L8("CBnepChannelController[%x]: EBnepSetupConnectionResponseMessage"), this);
                Handle(static_cast<RBnepSetupConnectionResponseControl*>(aControl));
                break;

            case EBnepFilterNetTypeSetMsg:
                LOG1(_L8("CBnepChannelController[%x]: EBnepFilterNetTypeSetMsg"), this);
                Handle(static_cast<RBnepFilterNetTypeSetRequestControl*>(aControl));
                break;

            case EBnepFilterNetTypeResponse:
                LOG1(_L8("CBnepChannelController[%x]: EBnepFilterNetTypeResponse"), this);
                Handle(static_cast<RBnepFilterNetTypeResponseControl*>(aControl));
                break;

            case EBnepFilterMultiAddrSetMsg:
                LOG1(_L8("CBnepChannelController[%x]: EBnepFilterMultiAddrSetMsg"), this);
                Handle(static_cast<RBnepFilterMultiAddrSetRequestControl*>(aControl));
                break;

            case EBnepFilterMultiAddrResponseMsg:
                LOG1(_L8("CBnepChannelController[%x]: EBnepFilterMultiAddrResponseMsg"), this);
                Handle(static_cast<RBnepFilterMultiAddrResponseControl*>(aControl));
                break;

            case EBnepControlCommandNotUnderstood:
                LOG1(_L8("CBnepChannelController[%x]: EBnepControlCommandNotUnderstood"), this);
                aControl->Free();
                delete aControl;
                break;

            default:
                LOG1(_L8("CBnepChannelController[%x]: default"), this);
                HandleUnknownCommandPacket(aControl);
            }
        }
    }

/**
   Remote device has terminated the connection. 
   We have to dismantle the link and notify the agent.
   @internalComponent
*/
void CBnepChannelController::RemoteDeviceDisconnect (TInt aError)
    {
    LOG_FUNC

	// This call can result in the CBnepChannelController being deleted.
	// No code should be placed after this call.

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    TPanMessage::TRemoteDeviceDisconnect msg(aError);
    RClientInterface::OpenPostMessageClose(Id(),iPanConnectionNotify,msg);	        
#else
    iNotify.RemoteDeviceDisconnect(aError);
#endif
    }


#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

void CBnepChannelController::ReceivedL(const Messages::TRuntimeCtxId& /* aSender */, const Messages::TNodeId& /* aRecipient */, Messages::TSignatureBase& aCFMessage)
    {
    // IMPORTANT NOTE:
    // We register/deregister the pan messages for this thread with the
    // Comms Transport in CBnepBridge and therefore the lifetime of the
    // registration is the same as CBnepBridge. Instances of
    // CBnepChannelController can live past the deletion ofthe CBnepBridge.
    // After CBnepBridge has gone no custom message signatures can be
    // received, but this is ok since the only message we expect to receive
    // is TCloseChannelController which uses the SigVoid signature from
    // ESock. If this expectation changes an additional registration/deregistration
    // will need to be added to CBnepChannelController's c'tor/d'tor.
    
	if (aCFMessage.MessageId().Realm() == TPanMessage::ERealmId)
    	{
        switch (aCFMessage.MessageId().MessageId())
            {
            case TPanMessage::TRoleRequestFromLocalDevice::EId:
                {
                TPanMessage::TRoleRequestFromLocalDevice& msg = message_cast<TPanMessage::TRoleRequestFromLocalDevice>(aCFMessage);
                BnepRoleRequestFromLocalDevice(msg.iRequestedLocalRole, msg.iRequestedRemoteRole);
                }
                break;            
            
            case TPanMessage::TRoleResponseFromLocalDevice::EId:
                {
                TPanMessage::TRoleResponseFromLocalDevice& msg = message_cast<TPanMessage::TRoleResponseFromLocalDevice>(aCFMessage);
                BnepRoleResponseFromLocalDevice(msg.iSetupResponse);
                }
                break;            

            case TPanMessage::TCloseChannelController::EId:
                {
                // Don't put anything accessing "this" after the call to Close() - it wont be there
                Close();
                }
                break;            

            case TPanMessage::TSetUplinkAccessAllowedForBnepLink::EId:
                {
                TPanMessage::TSetUplinkAccessAllowedForBnepLink& msg = message_cast<TPanMessage::TSetUplinkAccessAllowedForBnepLink>(aCFMessage);
                SetUplinkAccessAllowedForBnepLink(msg.iValue);
                }
                break;            

            default:
                __ASSERT_DEBUG(EFalse, BnepUtils::Panic(Bnep::EUnexpectedMessage));
            }
    	}
    else
        {
        __ASSERT_DEBUG(EFalse, BnepUtils::Panic(Bnep::EUnexpectedMessage));
        }
    
    // Absorb messages
    aCFMessage.ClearMessageId();
    }
#endif

