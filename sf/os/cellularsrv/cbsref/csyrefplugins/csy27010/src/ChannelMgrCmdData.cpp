//
// * Copyright 2004 Neusoft America Inc.
// * All rights reserved.
// * This component and the accompanying materials are made available
// * under the terms of the Eclipse Public License v1.0
// * which accompanies this distribution, and is available
// * at the URL "http://www.eclipse.org/legal/epl-v10.html".
// *
// * Contributors:
// * Keith Collins (Neusoft America Inc.)  original software development and additional code and modifications.
// * Thomas Gahagen (Neusoft America Inc.)  additional code and modifications.
// * Zhen Yuan (Neusoft America Inc.)  additional code and modifications.
// *
// * Description:  The CChannelMgrCmdData class defines a single 3GPP 27.010 based logical channel
// *               called DLC. Derived control and data channel add specific behavior.
//

// ChannelMgrCmdData.cpp

/** @file ChannelMgrCmdData.cpp
 *
 */

#include "ChannelMgrCmdData.h"
#include "PortC32InterfaceBase.h"
#include "PortC32Interface.h"
#include "CsyMsgBufBPFrame.h"
#include "CsyDebugLogger.h"

CChannelMgrCmdData* CChannelMgrCmdData::NewL(const TUint8 aDlcNum,
											 CPortFactory& aPortFactory,
											 CMux0710Protocol& aMux0710Protocol)
/**
 * This methods uses two phase construction and the cleanup stack to create
 * an instance of class CChannelMgrCmd.
 * @param aDlcNum - DLC number of this channel
 * @param aPortFactory - Reference to the port factory
 * @param aMux0710Protocol - Pointer to the 27.010 mux protocol
 * @return Pointer to the created instance
 */
    {
	_LOG_L4C2("CChannelMgrCmdData::NewL [aDlcNum=%d]", aDlcNum);

    CChannelMgrCmdData* p = new(ELeave) CChannelMgrCmdData(aDlcNum, aPortFactory,
														   aMux0710Protocol);
 	CleanupStack::PushL(p);
 	p->ConstructL();
	CleanupStack::Pop(p);
 	return p;
	}

CChannelMgrCmdData::~CChannelMgrCmdData()
/**
 * Destructor. Delete all resources and memory allocated by this object.
 * Disconnect the channel from the multiplexer.
 */
	{
    _LOG_L4C1("CChannelMgrData::~CChannelMgrCmdData");
	}


CChannelMgrCmdData::CChannelMgrCmdData(const TUint8 aDlcNum,
									   CPortFactory& aPortFactory,
									   CMux0710Protocol& aMux0710Protocol)
: CChannelMgrBase(aDlcNum, aPortFactory, aMux0710Protocol),
  iCount(0)
/**
 * Constructor.
 * @param aDlcNum - DLC number of this channel
 * @param aPortFactory - Reference to the port factory
 * @param aMux0710Protocol - Reference to the 27.010 mux protocol
 */
 	{
	iCsyAllowedToSendFrames = EFlowControlOn;
	}

void CChannelMgrCmdData::ConstructL()
/**
 * Create any instances and allocate any memory used by this object.
 */
 	{
	CChannelMgrBase::ConstructL();
	}

void CChannelMgrCmdData::Open(CPortC32InterfaceBase* aPort)
/**
 * Open the port
 *
 * @param aPort - Pointer to CPortC32Interface
 */
	{
	_LOG_L4C2(">>CChannelMgrCmdData::Open [aPort=0x%x]",aPort);
	
	iCount++;
	if (iCount == 1)
		{
		_LOG_L4C1("First client of port");
		
		if (aPort->GetClientType() == CPortFactory::EC32ClientIpNif)
			{
			ParameterNegotiate();
			}
		else
			{
			TInt err = Connect();
			if (err != KErrNone)
				_LOG_L4C2("Open Connect error =%d",err);				
			}
	}
	iPortArray.Insert(aPort,0);
	SetOwner(aPort);

	_LOG_L4C2("<<CChannelMgrCmdData::Open [iCount=%d]",iCount);
	}

void CChannelMgrCmdData::Close(CPortC32InterfaceBase* aPort)
/**
 * Close the port
 *
 * @param aPort - Pointer to CPortC32Interface
 */
	{
	_LOG_L4C2(">>CChannelMgrCmdData::Close [aPort=%d]",aPort->GetPortNumber());

	iCount--;
	if (iCount < 1)
		{
		_LOG_L4C1("Last client of port");
		Disconnect();
		iCount = 0; // reset counter
		iChannelReady = EFalse;
		}

	// remove port from the port list, set next port in the queue to be the port owner
	TInt index;
	index = iPortArray.Find(aPort);
	if (index == KErrNotFound)
		{
		_LOG_L1C2("** Close() - Port %d not found **", aPort);
		}
	else
		{
		iPortArray.Remove(index);
		if (iPortC32Interface == aPort)
			{
			// close the current owner of the channel, set owership to the first one in the port list
			// if there are one
			if (iPortArray.Count() > 0)
				SetOwner(iPortArray[0]);
			else
				SetOwner(NULL);
			}
		}

	_LOG_L4C2("<<CChannelMgrCmdData::Close [iCount=%d]",iCount);
	}

void CChannelMgrCmdData::SetOwner(CPortC32InterfaceBase* aPort)
/**
 * Set owner to the pointer
 * @param aPort - Pointer to new owner, which is a CPortC32Interface instance
 */
	{
	_LOG_L4C1(">>CChannelMgrCmdData::SetOwner");
	
	if (iPortC32Interface == aPort)
		return;

	if (iPortC32Interface != NULL)
		iPortC32Interface->CompleteOutstandingRequest();

	if (aPort == NULL)
		{
		iPortC32Interface = aPort;
		_LOG_L4C1("<<CChannelMgrCmdData::SetOwner aPort was Null");
		return;
		}

	TInt index;
	index = iPortArray.Find(aPort);
	if (index == KErrNotFound)
		{
		_LOG_L1C2("** Port %d not found **", aPort);
		}
	else
		{
		iPortC32Interface = aPort;
		}

	_LOG_L4C1("<<CChannelMgrCmdData::SetOwner");
	}

void CChannelMgrCmdData::SetupCmdChannelsForATCmdsL()
/**
 * Set up and prepare the ports for AT commands
 */
	{
	_LOG_L4C1("CChannelMgrCmdData::SetupCmdChannelsForATCmdsL");
#if defined DSAMPLE || defined H2
	_LOG_L4C1("Setting up for DSample/H2 (or P2Sample on Wins)");
#endif
#ifdef P2SAMPLE
	_LOG_L4C1("Setting up for P2Sample on TARGET");
#endif

	TBuf8<20> temp;
	temp.Copy(KATCmdSetup);
	iMux0710Protocol.Create0710DataFrames(temp, (TInt8)GetDlcNumber());
	}

void CChannelMgrCmdData::ProcessRecvUihFrame(CCsyMsgBufBpFrame* aBpFrame)
/**
 * Process a received UIH frame. Set flow control off if an "ok" is received
 * and we are still initializing.
 *
 * @param aBpFrame - Pointer to the frame received from the baseband
 */
	{
#ifdef _DEBUG
	// The dlcNum is only needed for logging when _DEBUG is set.
	// Otherwise we get a warning for ARM v5
	TInt dlcNum;
	dlcNum = aBpFrame->GetDlcNum();
#endif

	if(!iInitFinished)
		{
		if (iChannelState != ECsyChannelStateConnected)
			{
			_LOG_L4C1("Waiting for AT interpreter");
			if (aBpFrame->iMsg.Find(KATInitialised) != KErrNotFound)
				{
				_LOG_L3C2E("AT interpreter ready [dlcNum=%d]", dlcNum);
				iChannelState = ECsyChannelStateConnected;
				// Send initial AT command
				TRAP_IGNORE(SetupCmdChannelsForATCmdsL());
				}
			else
				{
				_LOG_L3C2E("Throw away unexpected response dlcNum=%d", dlcNum);
				}
			}
		else
			{
			_LOG_L4C1("Waiting for response to initial AT command");
			if (aBpFrame->iMsg.Find(_L8("OK")) != KErrNotFound)
				{
				_LOG_L1C2E("**** Init completed [dlcNum=%d] ****", dlcNum);
				NotifyChannelReady();
				ModemAndCsyToClientFlowCtrl(EFlowControlOff);
				}
			/*else if (aBpFrame->iMsg.Find(_L8("ERROR")) != KErrNotFound)
				{
				_LOG_L1C2E("**** Init completed [dlcNum=%d] ****ERROR****", dlcNum);
				NotifyChannelReady();
				ModemAndCsyToClientFlowCtrl(EFlowControlOff);
				}
			*/
			else
				{
				_LOG_L3C2E("Throw away unexpected response dlcNum=%d", dlcNum);
				}
			}
		iMux0710Protocol.AddFrameFreeQ(aBpFrame);
		}
	else if (iPortC32Interface)
		{
		// don't need to add to the mux's queue if we are not defragging messages
		// simply add to the port object's queue
		// send frame to client
		iPortC32Interface->SendFrameToClient(aBpFrame);
		}
	else
		{
		_LOG_L1C2E("** NULL C32 PORT ** dlcNum=%d", dlcNum);
		iMux0710Protocol.AddFrameFreeQ(aBpFrame);
		// MAF __ASSERT_DEBUG(EFalse, PANIC(KPanicIllegalState));
		}
	}



