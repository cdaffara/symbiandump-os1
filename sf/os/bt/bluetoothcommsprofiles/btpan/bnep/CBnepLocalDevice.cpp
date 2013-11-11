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
#include "CBnepBridge.h"
#include "CBnepLocalDevice.h"
#include "CBTAddrSubscriber.h"
#include <networking/ethinter.h>

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
#include "panprovisioninfo.h"
#endif

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_BNEP);
#endif

/**
   Constructor for the local device.
   @param aFactory CPktDrvFactory that created it. 
   @param aParent pointer to the bottom of Ethint.Nif,
   @internalComponent
*/
CBnepLocalDevice::CBnepLocalDevice (CPktDrvFactory& aFactory, CLANLinkCommon* aParent)
        : CPktDrvBase(aFactory), iLinkLayerUp(EFalse)
    {
    LOG_FUNC
    iParent = aParent; 
    }

/**
   Destroy the local device node and core controller.
   @internalComponent
*/
CBnepLocalDevice::~CBnepLocalDevice()
    {
    LOG_FUNC

    delete (CBnepBridge *)iOwner;

    delete iSubscriber; 
    }

/**
   Upward notification of a change in the local bluetooth device address.
   @internalComponent
*/
void CBnepLocalDevice::BDADDRChanged ()
    {
    LOG_FUNC

	iBDAddressAvailable = ETrue;
	if(iInterfaceStarted)
		{
	    TRAP_IGNORE(iParent->FoundMACAddrL());
		}
    }

void CBnepLocalDevice::ConstructL (CLANLinkCommon* UNUSEDPARAMETER1)
    {
    LOG_FUNC
    iOwner = CBnepBridge::NewL(*this);
    iSubscriber = CBTAddrSubscriber::NewL(iAddr, *this); 
    }

void CBnepLocalDevice::RemoteDeviceReady ()
	{
    LOG_FUNC
	if (!iLinkLayerUp)
		{
		iParent->LinkLayerUp();
		iLinkLayerUp = ETrue;
		}
	}

void CBnepLocalDevice::AllRemoteDevicesDisconnected ()
	{
    LOG_FUNC	
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	iParent->LinkLayerDown(KErrNone, CLANLinkCommon::EReconnect);
#endif
	iLinkLayerUp = EFalse;
	}
	
/**
   Not currently supported
   @internalComponent
*/
TInt CBnepLocalDevice::Control (TUint UNUSEDPARAMETER1, TUint UNUSEDPARAMETER2, TDes8& UNUSEDPARAMETER3, TAny* UNUSEDPARAMETER4)
    {
    LOG_FUNC
    return(KErrNotSupported); 
    }

/**
   Not currently supported
   @internalComponent
*/
TUint8* CBnepLocalDevice::GetInterfaceAddress () const
    {
    LOG_FUNC
    return const_cast<TUint8*>(iAddr.Des().Ptr()); 
    }

/**
   Not currently supported
   @internalComponent
*/
TInt CBnepLocalDevice::GetMulticastList (const THWAddr* UNUSEDPARAMETER1, TInt& UNUSEDPARAMETER2) const
    {
    LOG_FUNC
    return KErrNotSupported; 
    }

/**
   Not currently supported
   @internalComponent
*/
TInt CBnepLocalDevice::GetRxMode () const
    {
    LOG_FUNC
    return KErrNotSupported; 
    }

/**
   Not currently supported
   @internalComponent
*/
TInt CBnepLocalDevice::InterfacePowerDown ()
    {
    LOG_FUNC
    return KErrNotSupported; 
    }

/**
   Not currently supported
   @internalComponent
*/
TInt CBnepLocalDevice::InterfacePowerUp ()
    {
    LOG_FUNC
    return KErrNotSupported; 
    }

/**
   Not currently supported
   @internalComponent
*/
TInt CBnepLocalDevice::InterfaceResume ()
    {
    LOG_FUNC
    return KErrNotSupported; 
    }

/**
   Not currently supported
   @internalComponent
*/
TInt CBnepLocalDevice::InterfaceSleep ()
    {
    LOG_FUNC
    return KErrNotSupported; 
    }

/**
   Create the local device node on the PAN bridge.
   This is also the top interface of the packet driver.
   @internalComponent
*/
CBnepLocalDevice* CBnepLocalDevice::NewL (CPktDrvFactory& aFactory, CLANLinkCommon* aParent)
    {
    LOG_STATIC_FUNC
    CBnepLocalDevice* self = new(ELeave) CBnepLocalDevice(aFactory, aParent);
    CleanupStack::PushL(self);
    self->ConstructL(aParent);
    CleanupStack::Pop(self);
    return(self); 
    }

/**
   General purpose notification API.
   @see TBnepAgentToNifNotification
   @param aEvent A message type interpreted as a TBnepAgentToNifNotification.
   @param aInfo An arbitrary pointer, meaning determined by aEvent.
   @internalComponent
*/
TInt CBnepLocalDevice::Notification (TAgentToNifEventType aEvent, TAny* aInfo)
    {
    LOG_FUNC
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    TInt retval = KErrNotSupported;
    switch (aEvent)
        {
        case EPanAgentToNifGetPanProvisionInfo:
            {
            LOG(_L8("aEvent=EPanAgentToNifGetPanProvisionInfo"));	

            // Store the connection mananger for PAN
        	CPanProvisionInfo* panInfo = static_cast<CPanProvisionInfo*>(aInfo);
        	CBnepBridge* bnepConnMgr = static_cast<CBnepBridge*>(iOwner);
        	panInfo->SetBnepConnectionMgr(bnepConnMgr->Id());
            
            // Store a reference to the packet driver owner for later
            bnepConnMgr->SetPacketDriverOwner(panInfo->PacketDriverOwner());
            retval = KErrNone;
            }
            break;

        default:
            break;
        }
    return retval; 

#else
// !SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

    TBnepConnectionManager* bnepConnectionManager;
    TInt retval = KErrNotSupported;
    switch (aEvent)
        {
        case EPanAgentToNifGetConnectionManager:
            LOG(_L8("aEvent=EPanAgentToNifGetConnectionManager"));	
            bnepConnectionManager = static_cast<TBnepConnectionManager*>(aInfo);
            bnepConnectionManager->iBnepConnectionManager = iOwner;
            retval = KErrNone;
            break;

        default:
            break;
        }
    return retval; 
#endif
// SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    }

/**
   Called from bridge, pushing data up into Ethint.NIF
   @internalComponent
*/
void CBnepLocalDevice::Process (RBnepFrame& aFrame)
    {
    LOG_FUNC
    RMBufPacket chain;
    TRAPD(err, aFrame.BuildEthernetFrameL(chain));
    if(!err)
        {
        iParent->Process(chain);
        }
    else
        {
        aFrame.Reset();
        chain.Free();
        }
    }

/**
   Not currently implemented
   @internalComponent
*/
TInt CBnepLocalDevice::ResetInterface ()
    {
    LOG_FUNC
    return KErrNone; 
    }

/**
   Push a frame into the PAN network.
   ie. Local device is trying to send a frame to a remote device.
   @internalComponent
*/
TInt CBnepLocalDevice::Send (RMBufChain& aPkt)
    {
    LOG_FUNC
    RBnepFrame frame;
    TInt retval;
    TRAPD(err, frame.ParseEthernetFrameL(aPkt));
    aPkt.Free();  

    if (!err)
        {
        retval = iOwner->Process(frame, iAddr);
        LOG(_L8("Local device: ...frame sent"));
        }
    else
        {
        LOG(_L8("Local device: ...frame dropped - problem with parsing ethernet frame"));
        retval = KReadyForMoreData;
        }
    frame.Reset();
    return retval; 
    }

/**
   Not currently supported
   @internalComponent
*/
TInt CBnepLocalDevice::SetInterfaceAddress (const THWAddr& UNUSEDPARAMETER1)
    {
    LOG_FUNC
    return KErrNotSupported; 
    }

/**
   Not currently supported
   @internalComponent
*/
TInt CBnepLocalDevice::SetMulticastList (const THWAddr* UNUSEDPARAMETER1, TInt UNUSEDPARAMETER2)
    {
    LOG_FUNC
    return KErrNotSupported; 
    }

/**
   Not currently supported
   @internalComponent
*/
TInt CBnepLocalDevice::SetRxMode (TRxMode UNUSEDPARAMETER1)
    {
    LOG_FUNC
    return KErrNotSupported; 
    }

/**
   Not currently implemented
   @internalComponent
*/
TInt CBnepLocalDevice::StartInterface ()
    {
    LOG_FUNC

    iInterfaceStarted = ETrue;
    if(iBDAddressAvailable)
    	{
    	BDADDRChanged();
    	}

    return KErrNone; 
    }

/**
   Stops all links. This call is made on the assumption 
   that the interface is going to be unloaded and no attempt 
   will be made to restart in situ.
   @internalComponent
*/
TInt CBnepLocalDevice::StopInterface ()
    {
    LOG_FUNC

    iInterfaceStarted = EFalse;
    if(iOwner)
        {
        iOwner->StopLinks();
        }
    return KErrNone; 
    }

/**
   Consumes the event but has no other effect.
   @internalComponent
*/
void CBnepLocalDevice::Stop ()
    {
    LOG_FUNC
    }

