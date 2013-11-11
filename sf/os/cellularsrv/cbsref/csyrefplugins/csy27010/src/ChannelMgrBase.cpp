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
// * Description:  The CChannelMgrBase class defines a single 3GPP 27.010 based logical channel
// *               called DLC. This base class is inherited by derived control and data channel
// *               classes which add specific channel type behavior.
//

// ChannelMgrBase.cpp

/** @file ChannelMgrBase.cpp
 *
 */

#include "ChannelMgrBase.h"
#include "CsyMsgBufBpFrame.h"
#include "PortC32Interface.h"
#include "Mux0710Protocol.h"
#include "CsyDebugLogger.h"
#include "CommFrameWriterAo.h"

CChannelMgrBase::CChannelMgrBase(const TUint8 aDlcNum,
								 CPortFactory& aPortFactory,
								 CMux0710Protocol& aMux0710Protocol)
: iV24Signals(0x0D),
  iDlcNum(aDlcNum),
#ifndef __CSY_PROTOTYPE__
  iChannelState(ECsyChannelStateDisconnected),
#else
  iChannelState(ECsyChannelStateConnected),
#endif
  iPortFactory(aPortFactory),
  iPortC32Interface(NULL),
#ifndef __CSY_PROTOTYPE__
  iMux0710Protocol(aMux0710Protocol),
#else
  iMux0710Protocol(aMux0710Protocol),
  iChannelReady(ETrue),
  iInitFinished(ETrue),
#endif
  iModemAllowedToSendFrames(EFlowControlOff)
/**
 * Constructor.
 * @param aDlcNum - DLC number for the channel
 * @param aPortFactory - Reference to port factory
 * @param aMux0710Protocol - Reference to the Mux 27.010 object
 */
 	{
	}

CChannelMgrBase::~CChannelMgrBase()
/**
 * Destructor. Delete all resources and memory allocated by this object.
 */
	{
	_LOG_L4C1("CChannelMgrBase::~CChannelMgrBase");

	delete iChannelObserverAo;
	delete iTimeouter;
	}

void CChannelMgrBase::ConstructL()
/**
 * Create any instances and allocate any memory used by this object.
 */
 	{
	_LOG_L4C1("CChannelMgrBase::ConstructL");

	iChannelObserverAo = CChannelObserverAo::NewL();
	CActiveScheduler::Add(iChannelObserverAo); 

	iTimeouter = CActiveTimeouter::NewL(*this);
	iTimeoutVal = KOneSecond;
	iTxCountLimit = 10;
	}

TDes8& CChannelMgrBase::RefToMsgBuffer()
/**
 * This method returns a pointer to the message buffer to use
 * to format a message to send to the baseband.
 *
 * @return pointer
 */
	{
	// only one buffer is available for messages to the baseband
	return iDataToSendToModem;
	}

void CChannelMgrBase::ProcessRecvFrame(CCsyMsgBufBpFrame* aBpFrame)
/**
 * This method is called to process a frame that was received
 *
 * @param aBpFrame - Pointer to the frame buffer
 */
	{
	if (aBpFrame->GetFrameType() != KCsy0710CTLUIH)
		ProcessNonUihRecvFrame(aBpFrame);
	else
		ProcessRecvUihFrame(aBpFrame);
	}

TInt CChannelMgrBase::PlaceOnOutboundQueue()
/**
 * This method is called to process the "to baseband Message Q" event
 * for the channel. This event indicates that there is a message
 * that needs to be sent to the baseband.
 */
	{
	_LOG_L4C2(">>CChannelMgrBase::PlaceOnOutboundQueue [iDlcNum=%d]",iDlcNum);

	TInt ret = KErrNone;

	if (iCsyAllowedToSendFrames == EFlowControlOn)
		{
		_LOG_L4C1("Csy -> modem flow control = ON");
		iDelayedWriteToModem = ETrue;

		// We cannot fragment and place it on the write queue yet since
		// flow control in the direction of Csy to modem is set to ON.
		
		_LOG_L4C1("<<CChannelMgrBase::PlaceOnOutboundQueue - write delayed");
		return KErrNone;	
		}

	if (iChannelState != ECsyChannelStateConnected)
		{
		// this should not happen (should be caught by flow control above)
		// MAF __ASSERT_DEBUG(EFalse,PANIC(KPanicIllegalState));
		_LOG_L4C1("<<CChannelMgrBase::PlaceOnOutboundQueue - write ignored");
		return KErrNone;	
		}

	ret = iMux0710Protocol.Create0710DataFrames(iDataToSendToModem, iDlcNum);

	_LOG_L4C2("<<CChannelMgrBase::PlaceOnOutboundQueue [ret=%d]",ret);
	return ret;
	}

void CChannelMgrBase::WriteCancel()
/**
 * The client has requested the write by this DLC be cancelled.
 */
	{
	_LOG_L4C1(">>CChannelMgrBase::WriteCancel");

	if (iDelayedWriteToModem)
		{
		_LOG_L4C1("Delayed write has been cancelled");
		iDelayedWriteToModem = EFalse;
		}

	_LOG_L4C1("Remove any frames on write queue");

	// remove any frames (if any exist) from the writer Ao and
	// place them on the free queue
	
	CCommFrameWriterAo* writer = iPortFactory.GetCommWriterAo();

	writer->RemoveAnyDlcFramesOnWriteList(iDlcNum, EFalse);
	writer->RemoveAnyDlcFramesFromWaitList(iDlcNum, EFalse);

	_LOG_L4C1("<<CChannelMgrBase::WriteCancel");
	}

TInt CChannelMgrBase::Connect()
/**
 * Request to connect the channel. If the channel state is not "Connected"
 * then issue a SABM frame and change the channel state to "Connecting".
 *
 * @return KErrNone or KErrGeneral
 */
    {
    _LOG_L4C3(">>CChannelMgrBase::Connect [iDlcNum=%d,iChannelState=%d]",
		iDlcNum,iChannelState);

	TInt ret = KErrNone;
	if (iChannelState != ECsyChannelStateConnected)
		{
		_LOG_L4C1("Dlc is not currently connected");
		ret = iMux0710Protocol.Create0710ControlFrame(CMux0710Protocol::ESABM, iDlcNum);
		if (ret == KErrNone)
			{
			iChannelState = ECsyChannelStateConnecting;
			iInitFinished = EFalse;
			iTimeouter->Stop();
			iTimeouter->Start(iTimeoutVal);
			iTxCount = iTxCountLimit;
			}
		else
			{
			_LOG_L1C1("** Cannot send SABM **");
			}
		}
	else
		{
		_LOG_L2C1("** Ignored Connect **");
		ret = KErrGeneral;
		}

    _LOG_L4C2("<<CChannelMgrBase::Connect [ret=%d]",ret);
	return ret;
	}

TInt CChannelMgrBase::ParameterNegotiate()
/**
 * Request to parameter negotiation for the channel. If the channel state is not 
 * "Connected" then issue a UIH frame and with the parameter negotiation.
 *
 * @return KErrNone or KErrGeneral
 */
    {
    _LOG_L4C3(">>CChannelMgrBase::ParameterNegotiate [iDlcNum=%d,iChannelState=%d]",
		iDlcNum,iChannelState);

	TInt ret = KErrNone;
	if (iChannelState != ECsyChannelStateConnected) 
		{
		_LOG_L4C1("Dlc is not currently connected");
		if (iMux0710Protocol.Create0710ControlFrame(CMux0710Protocol::EUIH, iDlcNum) == KErrNone)
			{
			iChannelState = ECsyChannelStateParameterNegotiating;
			iTimeouter->Stop();
			iTimeouter->Start(iTimeoutVal);
			iTxCount = iTxCountLimit;
			}
		else
			{
			_LOG_L1C1("** Cannot send EUIH Param Neg. **");
			ret = KErrGeneral;
			}
		}
	else
		{
		_LOG_L2C1("** Ignored ParameterNegotiate **");
		}

    _LOG_L4C2("<<CChannelMgrBase::ParameterNegotiate [ret=%d]",ret);
	return ret;
	}

TInt CChannelMgrBase::Disconnect()
/**
 * Request to disconnect the channel. If the channel is not "Disconnected"
 * then issue a DISC frame and change the channel state to "Disconnecting".
 *
 * @return KErrNone or error code
 */
    {
    _LOG_L4C3(">>CChannelMgrBase::Disconnect [iDlcNum=%d,iChannelState=%d]",
		iDlcNum,iChannelState);
		
	TInt ret = KErrNone;
	if (iChannelState == ECsyChannelStateConnected)
		{
		_LOG_L4C1("Dlc is currently connected");
		if (iMux0710Protocol.Create0710ControlFrame(CMux0710Protocol::EDISC, iDlcNum) == KErrNone)
			{
			iChannelState = ECsyChannelStateDisconnecting;
			_LOG_L1C2("** Setting Channel Ready to False for DLC %d **",iDlcNum);
			iChannelReady = EFalse;
			iTimeouter->Stop();
			
			// Allow time for the channel to disconnect...
			User::After(1);
			}
		else
			{
			_LOG_L1C1("** Cannot send EDISC **");
			ret = KErrGeneral;
			}
		}
	else if (iChannelState == ECsyChannelStateTransmitError)
		{
		_LOG_L2C1("** Disconnect request for channel with transmit error **");
		ret = KErrNone;
		}
	else
		{
		_LOG_L2C1("** Ignored disconnect **");
		ret = KErrGeneral;
		}

    _LOG_L4C2("<<CChannelMgrBase::Disconnect [ret=%d]",ret);
	return ret;
	}

void CChannelMgrBase::ProcessNonUihRecvFrame(CCsyMsgBufBpFrame* aBpFrame)
/**
 * This method is called to process a Non-UIH frame which the channel
 * has received from the baseband for the channel.
 *
 * @param aBpFrame - Pointer to the received Non-UIH frame
 */
	{
	_LOG_L4C2(">>CChannelMgrBase::ProcessNonUihRecvFrame [iDlcNum=%d]", iDlcNum);

	TUint8 frameType = aBpFrame->GetFrameType();
	iMux0710Protocol.AddFrameFreeQ(aBpFrame);

	switch (frameType)
		{
	case KCsy0710CTLSABM:
		_LOG_L4C1("SABM");
		// MAF we should respond to this
		break;

	case KCsy0710CTLUA:
		{
		_LOG_L4C1("UA");
		iTimeouter->Stop();

		if (iChannelState == ECsyChannelStateConnecting)
			{
			_LOG_L4C1("SABM received by remote end - send MSC command");
			if (SendMscCommand(iV24Signals) == KErrNone)
				iChannelState = ECsyChannelStateMSCsent;
			}
		else if (iChannelState == ECsyChannelStateDisconnecting)
			{
			_LOG_L4C2("Channel disconnected %d", iDlcNum);
			iChannelState = ECsyChannelStateDisconnected;

			if (iPortFactory.DecrementNumOfOpenPorts())
				{
				NotifyChannelReady();
				}
			}
		}
		break;

	case KCsy0710CTLDM:    // 0x0F
		_LOG_L4C1("DM");
		break;

	case KCsy0710CTLDISC:  // 0x43
		_LOG_L4C1("DISC");
		// MAF we should respond to this
		break;

	case KCsy0710CTLUIH:
	case KCsy0710CTLUI:
		// MAF __ASSERT_DEBUG(EFalse,PANIC(KPanicIllegalState)); here
		break;
	
	default:
		_LOG_L1C2("** Unknown FrameType = 0x%02x **", frameType);
		break;
		}

	_LOG_L4C1("<<CChannelMgrBase::ProcessNonUihRecvFrame");
	}

TInt CChannelMgrBase::SendMscCommand(TUint8 aV24Signals)
/**
 * Sends an MSC command
 *
 * @param aV24Signals
 * @return error if failed to send
 */
	{
	_LOG_L4C2(">>CChannelMgrBase::SendMscCommand [aV24Signals=0x%02x]",
		aV24Signals);

	if (iMscReplyExpected)
		{
		_LOG_L4C1("<<CChannelMgrBase::SendMscCommand - already doing MSC");
		return KErrNotReady;
		}

	// Keep the signals in case of resend
	iV24Signals = aV24Signals;
	TInt ret = iMux0710Protocol.Create0710ControlFrame(
		CMux0710Protocol::EUIH, iDlcNum, CMux0710Protocol::EMSC, iV24Signals);
	
	if (ret != KErrNone)
		{
		_LOG_L1C1("** Could not send MSC command **");
		}
	else
		{
		iTimeouter->Stop();
		iTimeouter->Start(iTimeoutVal);
		iTxCount = iTxCountLimit;
		iMscReplyExpected = ETrue;
		}

	_LOG_L4C2("<<CChannelMgrBase::SendMscCommand [ret=%d]",ret);
	return ret;
	}

void CChannelMgrBase::MscReceived(TUint8 aV24Signals)
/**
 * Processes a received MSC command
 *
 * @param aV24Signals
 */
	{
	_LOG_L4C2(">>CChannelMgrBase::MscReceived [aV24Signals=0x%x]",
		aV24Signals);

	if (!iMscReplyExpected)
		{
		_LOG_L2C1("*** Not expecting MscReceived? ***");
		return;
		}

	iMscReplyExpected = EFalse;
	iTimeouter->Stop();

	if (iChannelState == ECsyChannelStateMSCsent)
		{
		_LOG_L4C1("MSC cmd part of start up sequence");
		iChannelState = ECsyChannelStateWaitingForChannelReady;

		//different type of channel may need different init process.
		if (iPortC32Interface)
			{
#ifdef _27010ADVANCEOPTION
			if (iPortC32Interface->GetClientType() == CPortFactory::EC32ClientIpNif)
				{
				_LOG_L4C1(" IpNif Channel Initialized");
				// Don't need to wait for AT command interpreter ready since not AT
				iChannelState = ECsyChannelStateConnected;
				NotifyChannelReady();
				ModemAndCsyToClientFlowCtrl(EFlowControlOff);
				}
#endif
			}
		else
			{
			// Command channel
			iChannelState = ECsyChannelStateConnected;
			NotifyChannelReady();
			}
		}

	if ((iDlcNum)&&(iV24Signals != aV24Signals))
		{
		_LOG_L4C3("Data Dlc - Response different to sent [%x != %x]",
			iV24Signals, aV24Signals);

		ReceivedV24Signals(aV24Signals);
		}

	_LOG_L4C1("<<CChannelMgrBase::MscReceived");
	}

void CChannelMgrBase::ModemAndCsyToClientFlowCtrl(const TFlowControl aFlowControl)
/**
 * Set the channel's flow control to the specified value.
 * Update the associate port object's flow control if the port object 
 * exists.
 *
 * @param aFlowControl - New flow control state
 */
 	{
	_LOG_L4C3(">>CChannelMgrBase::ModemAndCsyToClientFlowCtrl [aFlowControl=%d,iDlcNum=%d]", 
		aFlowControl,iDlcNum);

	iCsyAllowedToSendFrames = aFlowControl;
	if ((aFlowControl == EFlowControlOff)&&(iDelayedWriteToModem))
		{
		_LOG_L4C1("Flow control now OFF - delayed frame to send");

		TInt ret = iMux0710Protocol.Create0710DataFrames(iDataToSendToModem, iDlcNum);
		if (ret)
			{
			_LOG_L1C2("** Fragmentation failed [ret=%d] **",ret);
			}
		else
			{
			_LOG_L4C1("Delayed write placed on write queue");
			iDelayedWriteToModem = EFalse;
			}
		}

	CPortC32InterfaceBase* port = GetC32Port();
	if (port)
		port->ModemAndCsyToClientFlowCtrl(aFlowControl);
	else
		{
		_LOG_L2C1("** GetC32Port() returned NULL **");
		}

	_LOG_L4C1("<<CChannelMgrBase::ModemAndCsyToClientFlowCtrl");
	}

void CChannelMgrBase::ReceivedV24Signals(const TUint8 aV24Signals)
/**
 * Set the channel's signals to the specified value.
 * Update the associate port object's flow control if the port object 
 * exists.
 *
 * @param aSignals - new signal state
 */
 	{
	_LOG_L4C3(">>CChannelMgrBase::ReceivedV24Signals [aV24Signals=0x%x,iDlcNum=%d]", 
		aV24Signals,iDlcNum);

	CPortC32InterfaceBase* port = GetC32Port();
	if (port)
		{
		// Flow control
		TFlowControl flowControl = EFlowControlOff;

		// FC (bit 2)
		if (aV24Signals & 0x02)
			{
			_LOG_L4C2("FC=1 - modem dlc %d unable to accept frames",iDlcNum);
			flowControl = EFlowControlOn;
			_LOG_L4C1("flowControl On  (i.e stop flow)");

			// move frames for this dlc from write list to wait list
			iPortFactory.GetCommWriterAo()->RemoveAnyDlcFramesOnWriteList(iDlcNum);
			}
		else
			{
			// move any frames for this dlc from wait to write list
			iPortFactory.GetCommWriterAo()->RemoveAnyDlcFramesFromWaitList(iDlcNum);
			}

		ModemAndCsyToClientFlowCtrl(flowControl);

		port->ReceivedV24Signals(aV24Signals);
		}
	else
		{
		_LOG_L2C1("** GetC32Port() returned NULL **");
		}

	_LOG_L4C1("<<CChannelMgrBase::ReceivedV24Signals");
	}

void CChannelMgrBase::WaitForChannelReady()
/**
 * This method starts an active object and waits for the channel ready
 * call back signal.
 */
	{
	_LOG_L4C1(">>CChannelMgrBase::WaitForChannelReady");

	if (iMux0710Protocol.MaxRetriesReached())
		{
		_LOG_L4C1("Mux Max Retries reached - so skip Channel Ready");
		}
	else
		{
		// kick off an active object and wait for channel ready call back
		if (iChannelReady)
			{
			_LOG_L4C2("iChannelReady=%d",iChannelReady);
			}
		else if (!iChannelObserverAo->IsActive())
			{
			// start the active object. CSY will wait here until channel is ready
			// nest active scheduler is used here as we did not want to block whole thread (reading/writing)

			iChannelObserverAo->StartWait();

			// start a nested scheduling; blocks until CActiveScheduler::Stop() 
			// is called in DoCancel()

			// MAF look at use CActiveSchedulerWait instead of this

			// One basic assumption here is that CSY will process port open request one by one
			// (although there may be mutiple port open requests from clients, c32 will call PortFactory::NewPortL
			// one by one.) This assumption should be valid as it is only one thread for c32
			// if this assumption does not hold, headache follows.
			_LOG_L4C1("Start wait for channel ready");

			CActiveScheduler::Start();
			_LOG_L4C1("End wait for channel ready");
			}
		else
			{
			_LOG_L4C1("We are here only when others are already waiting");
			}
		}
	_LOG_L4C1("<<CChannelMgrBase::WaitForChannelReady");
	}

void CChannelMgrBase::TimedOut()
/**
 * This method will be called when we have a timeout During receive a reply.
 * It will retransmit if needed.
 */
	{
	_LOG_L4C1(">>CChannelMgrBase::TimedOut");

	iTxCount--;
	if (iTxCount > 0)  // retransmit
		{
		_LOG_L4C2("iTxCount=%d",iTxCount);

		TInt ret = KErrNone;
		if (iMscReplyExpected)
			{
			iMscReplyExpected = EFalse;
			ret = SendMscCommand(iV24Signals);
			}
		else
			{
			switch(iChannelState)
				{
			case ECsyChannelStateParameterNegotiating:
				ret = iMux0710Protocol.Create0710ControlFrame(
					CMux0710Protocol::EUIH, iDlcNum, CMux0710Protocol::EParamNeg);
				break;
			case ECsyChannelStateConnecting:
				ret = iMux0710Protocol.Create0710ControlFrame(
					CMux0710Protocol::ESABM, iDlcNum);
				break;
			case ECsyChannelStateDisconnecting:
				ret = iMux0710Protocol.Create0710ControlFrame(
					CMux0710Protocol::EDISC, iDlcNum);
				break;
			default:
				// should never reach here
				_LOG_L1C1("** unexpected receiving timeout **");
				}
			}
		if (ret)
			{
			// Error sending - we shall just wait until the timeout and try again
			_LOG_L1C2("** Error sending on dlc 0 [ret=%d] **",ret);
			}
		iTimeouter->Stop();
		iTimeouter->Start(iTimeoutVal);
		}
	else
		{
		_LOG_L1C1("** Retries expired **");
		if (iMscReplyExpected)
			{
			_LOG_L1C1("** Retries expired for MSC command **");
			iMscReplyExpected = EFalse;
			}

		// we have problem communication, may need to reset?
		iChannelState = ECsyChannelStateTransmitError;
		NotifyChannelReady();
		}

	_LOG_L4C1("<<CChannelMgrBase::TimedOut");
	}

void CChannelMgrBase::NotifyChannelReady()
/**
 * This method stops the waiting for a channel.
 */
	{
	_LOG_L4C1("CChannelMgrBase::NotifyChannelReady");

	iChannelReady = ETrue;
	iInitFinished = ETrue;

	// complete the active object if it is running
	if (iChannelObserverAo->IsActive())
		{
		iChannelObserverAo->ChannelReady();
		}
	}

TInt CChannelMgrBase::SetCsyToModemFlowControl(TFlowControl aFlowControl)
/**
 * 
 */
	{
	_LOG_L4C3(">>CChannelMgrBase::SetCsyToModemFlowControl [aFlowControl=%d,iDlcNum=%d]",
		aFlowControl,iDlcNum);

	TInt ret = KErrNone;
	if (aFlowControl != iModemAllowedToSendFrames)
		{
		TUint8 v24Signals = iV24Signals;
		TBool sendCommand = EFalse;
		if (aFlowControl == EFlowControlOn)
			{
			_LOG_L4C1("aFlowControl = ON");
			if (v24Signals & 0x02)
				{
				_LOG_L4C1("** Flow control already set **");
				}
			else
				{
				v24Signals |= 0x02;
				sendCommand = ETrue;
				}
			}
		else
			{
			_LOG_L4C1("aFlowControl = OFF");
			if (v24Signals & 0x02)
				{
				v24Signals ^= 0x02;
				sendCommand = ETrue;
				}
			else
				{
				_LOG_L4C1("Flow control already off");
				}
			}
		if (sendCommand)
			{
			ret = SendMscCommand(v24Signals);
			if (ret)
				{
				_LOG_L4C1("SendMscCommand failed");
				}
			else
				{
				_LOG_L4C3("Csy to modem flow control changed %d -> %d",
					iModemAllowedToSendFrames,aFlowControl);
				iModemAllowedToSendFrames = aFlowControl;
				}
			}
		}

	_LOG_L4C2("<<CChannelMgrBase::SetCsyToModemFlowControl [ret=%d]",ret);
	return ret;
	}

//
// class CChannelObserverAo
//

CChannelObserverAo::CChannelObserverAo()
: CActive(CActive::EPriorityStandard)
/**
 * Constructor.  Pass priority of active object to base class.
 *
 */
	{}

CChannelObserverAo* CChannelObserverAo::NewL()
/**
 * This method uses two phase construction and the cleanup stack
 * to create an instance of class CChannelObserverAo.
 *
 * @return Pointer to the created instance
 */
	{
	_LOG_L4C1("CChannelObserverAo::NewL");
	return new (ELeave) CChannelObserverAo();
	}

CChannelObserverAo::~CChannelObserverAo()
/**
 * Destructor. Cancel any outstanding request.
 */
	{	
	if (IsActive())
		Cancel();
	}

void CChannelObserverAo::RunL()
/**
 * This method should not be called. Request of this 
 * active object will only be canceled.
 */
	{
	// MAF __ASSERT_DEBUG(EFalse,PANIC(KPanicIllegalState));
	_LOG_L1C1("** CChannelObserverAo::RunL() - should not be called **");
	}

void CChannelObserverAo::DoCancel()
/**
 * Cancel an outstanding request. This will complete request
 * with KErrCancel and stop waiting.
 */
	{
	_LOG_L4C1("CChannelObserverAo::DoCancel");

	iStatus = KErrCancel;
	TRequestStatus* temp = &iStatus;
	User::RequestComplete(temp, KErrCancel);

	_LOG_L4C1("CChannelObserverAo stop scheduler");
	CActiveScheduler::Stop();
	}

void CChannelObserverAo::StartWait()
/**
 * Set active and start waiting.
 */
	{
	_LOG_L4C1("CChannelObserverAo::StartWait");
	if (!IsActive())
		{
		iStatus = KRequestPending;
		SetActive();
		}
	}

void CChannelObserverAo::ChannelReady()
/**
 * Called when expected event occured to stop waiting. 
 */
	{
	_LOG_L4C1("CChannelObserverAo::ChannelReady");

	// complete current request
	if (IsActive())
		{
		_LOG_L4C1("CChannelObserverAo Active");
		Cancel();
		}
	}
