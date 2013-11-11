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
// * Description:  The file contains the implementation for class CChannelMgrCtrl.
// *               Class CChannelMgrCtrl implements a 3GPP 27.010 control channel (DLC).
// *               A control channel is used to initialize the multiplexer and to
// *               create/monitor DLC's.
//

// ChannelMgrCtrl.cpp

/** @file ChannelMgrCtrl.cpp
 *
 * The file contains the implementation for class CChannelMgrCtrl.
 * Class CChannelMgrCtrl implements a 3GPP 27.010 control channel (DLC).
 * A control channel is used to initialize the multiplexer and to
 * create/monitor DLC's.
 */

#include "ChannelMgrCtrl.h"
#include "Portfactory.h"
#include "Mux0710Protocol.h"
#include "CsyMsgBufBPFrame.h"
#include "CommFrameWriterAo.h"
#include "CsyDebugLogger.h"
#include "ChannelMgrBase.h"
#include "PortC32InterfaceBase.h"
#include "PortC32Interface.h"
#include "ChannelMgrCmdData.h"
#include "CsyGlobals.h"
#include "d32comm.h"

CChannelMgrCtrl* CChannelMgrCtrl::NewL(CPortFactory& aPortFactory,
									   CMux0710Protocol& aMux0710Protocol)
/**
 * This methods uses two phase construction and the cleanup stack to create
 * an instance of class CChannelMgrCtrl.
 *
 * @param aPortFactory - Refrence to the port factory
 * @param aMux0710Protocol - Reference to the Mux 27.010 object
 * @return Pointer to the created instance
 */
    {
	_LOG_L4C1("CChannelMgrCtrl::NewL");

    CChannelMgrCtrl* p = new(ELeave) CChannelMgrCtrl(aPortFactory, aMux0710Protocol);
 	CleanupStack::PushL(p);
 	p->ConstructL();
	CleanupStack::Pop(p);
 	return p;
	}

CChannelMgrCtrl::~CChannelMgrCtrl()
/**
 * Destructor. Delete all resources and memory allocated by this object.
 */
	{
	_LOG_L4C1("CChannelMgrCtrl::~CChannelMgrCtrl");

	// Disconnect the DLC Control Channel 
	Disconnect();
	}

CChannelMgrCtrl::CChannelMgrCtrl(CPortFactory& aPortFactory,
								 CMux0710Protocol& aMux0710Protocol)
: CChannelMgrBase(KCtrlChannelDlcNum, aPortFactory, aMux0710Protocol)
/**
 * Constructor.
 *
 * @param aPortFactory - Reference to the port factory
 * @param aMux0710Protocol - Reference to the 27.010 mux protocol
 */
 	{
	}

void CChannelMgrCtrl::ConstructL()
/**
 * This method initializes the memory and data used by this object.
 */
	{
	_LOG_L4C1("CChannelMgrCtrl::ConstructL");
	CChannelMgrBase::ConstructL();
	}

void CChannelMgrCtrl::ProcessRecvUihFrame(CCsyMsgBufBpFrame* aBpFrame)
/**
 * This method is called to process a UIH frame that was received
 * on the control channel.  Based on the message type and contents
 * the channel state machine is updated and a response is issued back
 * to the baseband.
 *
 * @param aBpFrame - Pointer to the frame buffer
 */
	{
	_LOG_L4C2(">>CChannelMgrCtrl::ProcessRecvUihFrame [aBpFrame=0x%x]",
		aBpFrame);

	// adv option:   address | control | Type   | LengthData | Value(s)
	// basic option: address | control | length | Type  | LengthData | Value(s)

	TUint8 ctrlTypeField;

#ifdef _27010ADVANCEOPTION
	ctrlTypeField = KAdvOptionCtrlStart;   // 2
#else
	ctrlTypeField = KBasicOptionCtrlStart; // 3
#endif

	TUint8 frameLength = (TUint8) aBpFrame->iMsg.Length();
	if (frameLength <= ctrlTypeField)
		{
		_LOG_L1C3("** Frame length %d <= minimum ctrl frame length %d **",
			frameLength,ctrlTypeField);
		_LOG_L4C1("<<CChannelMgrCtrl::ProcessRecvUihFrame - bad frame");
		return;
		}

	TBool confirmBack = ETrue;
	switch (aBpFrame->iMsg[ctrlTypeField])
		{
	case KCsy0710CTLUIH_DlcParamNegotiate:
		_LOG_L4C1( "DlcParamNegotiate");
		break;
	case KCsy0710CTLUIH_DlcParamNegotiateRsp:
		{
		_LOG_L3C1("DlcParamNegotiate response");
		confirmBack = EFalse; // No response to send back

		// The position of the values field in the ctrl data
		TUint8 firstValueFieldIndex  =
			(TUint8) (ctrlTypeField + KPositionOfValueFieldInCtrlData);

		if (frameLength > firstValueFieldIndex)
			{
			const TAny* msgValue =
				reinterpret_cast<const TAny*> (&(aBpFrame->iMsg.Ptr()[firstValueFieldIndex]));

			typedef struct TPnMsgValue
				{
				TUint8 iDlci:6;
				TUint8 iRes1:2;
				TUint8 iFrameType:4;
				TUint8 iCreditFlow:4;
				TUint8 iPrior:6;
				TUint8 iRes2:2;
				TUint8 iAckTimer;
				TUint16 iFrameSize;
				TUint8 iMaxNbrofRetrans;
				TUint8 iCredits;
				} TPnMsgValue;

			const TPnMsgValue* pnMsgValue =
				reinterpret_cast<const TPnMsgValue*> (msgValue);

			// now we should be able to use pnMsgValue.iDlci, etc
			if ((pnMsgValue->iDlci            == iMux0710Protocol.PreviousNegotiateDlcNum())  &&
				(pnMsgValue->iFrameType       == KPNFrameType)          &&
				(pnMsgValue->iCreditFlow      == KPNClBits)             &&
				(pnMsgValue->iPrior           == KPNDlcPriority)        &&
				(pnMsgValue->iAckTimer        == KPNAckTimer)           &&
				(pnMsgValue->iFrameSize       == KPNMaxFrameSize)       &&
				(pnMsgValue->iMaxNbrofRetrans == KPNMaxRetransmissions) &&
				(pnMsgValue->iCredits         == KPNWindowSize))
				{
				_LOG_L3C1("ParamNegotiate passed");

				TUint8 dlcNum = pnMsgValue->iDlci;
				iPortFactory.ConnectIpNifPort(dlcNum);
				}
			else
				{
				// MAF If this fails try again or forget trying PN.
				_LOG_L3C1("ParamNegotiate Failed");
				}
			}
		else
			{
			_LOG_L1C3("** Frame length %d <= first ctrl value %d **",
				frameLength,firstValueFieldIndex);
			}
		}
		break;
	case KCsy0710CTLUIH_PowerSaveCtl:
		_LOG_L3C1("PowerSaveCtrl");
		break;
	case KCsy0710CTLUIH_MultCloseDown:
		_LOG_L3C1("MultCloseDown");
		break;
	case KCsy0710CTLUIH_TestCmd:
		_LOG_L3C1("TestCmd");
		break;
	case KCsy0710CTLUIH_FlowControlOn:
		_LOG_L3C1("FlowControlOn");
		break;
	case KCsy0710CTLUIH_FlowControlOff:
		_LOG_L3C1("FlowControlOff");
		break;
	case KCsy0710CTLUIH_ModemStatusRsp:
		{
		_LOG_L3C1("MSC response");
		confirmBack = EFalse; // No response to send back

		// From 27.010
		// RTC	DTR	108/2	DSR	107
		// RTR	RFR (note)	133	CTS	106
		// IC	always 0-	-	RI	125
		// DV	always 1-	-	DCD	109
		// NOTE	Circuit 133, RFR (Ready for Receiving) is commonly assigned to the connector pin that is alternatively used for circuit 105, RTS. It is sometimes referred to by that name.

		// The position of the values field in the ctrl data
		TUint8 firstValueFieldIndex  =
			(TUint8) (ctrlTypeField + KPositionOfValueFieldInCtrlData);
		TUint8 secondValueIndex = (TUint8) (firstValueFieldIndex + 1);

		if (frameLength > secondValueIndex)
			{
			TUint8 dlcNum = (TUint8) (((TUint8) aBpFrame->iMsg[firstValueFieldIndex]) >> 2); // remove EA and CR
			_LOG_L4C2("dlcNum=%d", dlcNum);

			TUint8 v24signals = (TUint8) aBpFrame->iMsg[secondValueIndex];
			_LOG_L4C2("V24 Signals=0x%x", v24signals);

			if (dlcNum)
				{
				_LOG_L3C1("For a data DLC");
				CChannelMgrCmdData* channel = iPortFactory.FindChannelMgrByDlcNum(dlcNum);
				channel->MscReceived(v24signals);
				}
			else
				MscReceived(v24signals);
			}
		else
			{
			_LOG_L1C3("** Frame length %d <= second ctrl value %d **",
				frameLength,secondValueIndex);
			}
		}
		break;
	case KCsy0710CTLUIH_ModemStatusCmd:
		{
		// The modem side has informed us of its state
		_LOG_L3C1("MSC command");

		// The position of the values field in the ctrl data
		TUint8 firstValueFieldIndex  =
			(TUint8) (ctrlTypeField + KPositionOfValueFieldInCtrlData);

		TUint8 secondValueIndex = (TUint8) (firstValueFieldIndex + 1);

		if (frameLength > secondValueIndex)
			{
			TUint8 dlcNum = (TUint8) (((TUint8) aBpFrame->iMsg[firstValueFieldIndex]) >> 2); // remove EA and CR
			_LOG_L4C2("dlcNum=%d", dlcNum);

			TUint8 v24signals = (TUint8) aBpFrame->iMsg[secondValueIndex];
			_LOG_L4C2("V24 Signals=0x%x", v24signals);

			if (dlcNum)
				{
				_LOG_L3C1( "For a data DLC");
				CChannelMgrCmdData* channel = iPortFactory.FindChannelMgrByDlcNum(dlcNum);
				channel->ReceivedV24Signals(v24signals);
				}
			else
				{
				_LOG_L3C1( "FC & v24 signals ignored for Ctrl Channel");
				}
			}
		else
			{
			_LOG_L1C3("** Frame length %d <= second ctrl value %d **",
				frameLength,secondValueIndex);
			confirmBack = EFalse;
			}
		}
		break;
	case KCsy0710CTLUIH_NonSupportedCmdResp:
		confirmBack = EFalse;
		_LOG_L3C1("NonSupportedCmdResp");
		break;
	case KCsy0710CTLUIH_RemotePortNegotiate:
		_LOG_L3C1("RemotePortNegotiate");
		break;
	case KCsy0710CTLUIH_RemoteLineStatus:
		_LOG_L3C1("RemoteLineStatus");
		break;
	case KCsy0710CTLUIH_ServiceNegotiate:
		_LOG_L3C1("ServiceNegotiate");
		break;
	default:
		_LOG_L1C2("** Unknown ctrltype 0x%02x **", aBpFrame->iMsg[ctrlTypeField]);
		break;
		}

	if (confirmBack)
		{
		_LOG_L3C1( "Send a response ...");
		ConfirmCtrlFrame(aBpFrame);
		// MAF what if send of response fails
		}
	else
		{
		iMux0710Protocol.AddFrameFreeQ(aBpFrame);
		}

	_LOG_L4C1("<<CChannelMgrCtrl::ProcessRecvUihFrame");
	}

void CChannelMgrCtrl::ConfirmCtrlFrame(CCsyMsgBufBpFrame* aBpFrame)
/**
 * This method initializes the control channel.
 * @param a pointer to the Frame
 */
	{
	_LOG_L4C2(">>CChannelMgrCtrl::ConfirmCtrlFrame [aBpFrame=0x%x]",
		aBpFrame);

	// echo the UIH frame back to the BP, except clear the C/R bit (bit 1)
    // the checksum is unchanged
	// we must also insert a start flag and end flag
	TBuf8<4> flag;
	flag.SetLength(1);
	flag[0] = KCsy0710StartEndFlag;
	aBpFrame->iMsg.Insert(0, flag);
	TInt length = aBpFrame->iMsg.Length();
	aBpFrame->iMsg.SetLength(length + 1);
	aBpFrame->iMsg[length] = KCsy0710StartEndFlag;

	// clear the C/R bit in the msg type field, not the frame type field
#ifdef _27010ADVANCEOPTION
	aBpFrame->iMsg[3] &= ~KCsy0710CRBit;
#else
	aBpFrame->iMsg[4] &= ~KCsy0710CRBit;
#endif

	// put the frame at the front of the baseband's queue
	aBpFrame->CompleteWhenSent() = ETrue;
	TInt ret = iPortFactory.GetCommWriterAo()->Write(aBpFrame, ETrue);
	if (ret)
		{
		// MAF what to do here
		_LOG_L1C2("** Failed to send response [ret=%d] **",ret);
		}

	_LOG_L4C1("<<CChannelMgrCtrl::ConfirmCtrlFrame");
	}
