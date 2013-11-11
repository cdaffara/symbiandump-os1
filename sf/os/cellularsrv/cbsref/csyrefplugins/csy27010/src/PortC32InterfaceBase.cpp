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
// * Description:  This file contains the implementation for the CPortC32Interface class.
// *               This class contains methods which are invoked by C32 when the
// *               associated client RComm public API is invoked. These methods
// *               are used to create, configure, read, write, and close logical
// *               serial ports. Instances of this class are created by the CSY's
// *               Port Factory.
//

/** @file PortC32InterfaceBase.cpp
 * This file contains the implementation for the CPortC32InterfaceBase class.
 * This base class contains methods which are invoked by C32 when the
 * associated client RComm public API is invoked. These methods
 * are used to create, configure, read, write, and close logical
 * serial ports. Instances of this class are created by the CSY's
 * Port Factory.
 */

#include <cdbcols.h>

#include "PortC32InterfaceBase.h"
#include "Portfactory.h"
#include "Mux0710Protocol.h"
#include "CsyMsgBufBPFrame.h"
#include "ChannelMgrCmdData.h"
#include "CommFrameReaderAo.h"
#include "CsyDebugLogger.h"

CPortC32InterfaceBase::CPortC32InterfaceBase(CPortFactory& aPortFactory,
									 CPortFactory::TC32PortInfo& aPortInfo)
/**
 * Constructor.
 *
 * @param aPortFactory - Reference to the port factory
 * @param aPortInfo - Reference to the port information
 */
 : CPort(),
 	iFramesWaitingToBeReadList(_FOFF(CCsyMsgBufBpFrame, iMsgLink)),
    iFramesWaitingToBeReadIter(iFramesWaitingToBeReadList),
    iRole(ECommRoleDTE),
	iPortFactory(aPortFactory),
	iPortInfo(aPortInfo)
	{}

void CPortC32InterfaceBase::ConstructL()
/**
 * Base construction for channels
 */
	{
	_LOG_L4C1("CPortC32InterfaceBase::ConstructL");

	// create active objects to handle events
	iConfig.iTerminatorCount = 0;
	}

CPortC32InterfaceBase::~CPortC32InterfaceBase()
/**
 * Destructor.
 */
	{
	_LOG_L4C1(">>CPortC32InterfaceBase::~CPortC32InterfaceBase");

	// remove all frames from queue and delete
	RemoveWaitingAllFrames(EFalse);

	if (iMuxChannel)
		iMuxChannel->Close(this);

	_LOG_L4C1("<<CPortC32InterfaceBase::~CPortC32InterfaceBase");
	}

void CPortC32InterfaceBase::RemoveWaitingAllFrames(TBool aAddToFreeList)
/**
 * 
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::RemoveWaitingAllFrames [aAddToFreeList=%d]",
		aAddToFreeList);

	CCsyMsgBufBpFrame* frame = NULL;
	iFramesWaitingToBeReadIter.SetToFirst();
	while ((frame = iFramesWaitingToBeReadIter++) != NULL)
		{
		iFramesWaitingToBeReadList.Remove(*frame);
		if (aAddToFreeList)
			iPortFactory.GetMux0710Protocol()->AddFrameFreeQ(frame);
		else
			delete frame;
		}
	}

/********************************************************************************/
/*                       Start of methods from CPort                            */
/********************************************************************************/

void CPortC32InterfaceBase::NotifySignalChange(TUint aSignalMask)
/**
 * Called by C32 when the client requests NotifySignalChange.
 *
 * @param aSignal
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::NotifySignalChange [port=%d]", GetPortNumber());

	TUint reqSigs = aSignalMask & ~KSignalBreak;
	if (!reqSigs)
		{
		// User has only asked to be notified of break.
		SignalChangeCompleted(iSignals, KErrArgument);
		return;
		}
	iSignalMask |= reqSigs;

	iSignalChangeNotifyPending = ETrue;
	}		   

void CPortC32InterfaceBase::NotifySignalChangeCancel()
/**
 * Called by C32 when the client requests to cancel a NotifySignalChange 
 * request.
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::NotifySignalChangeCancel [port=%d]", GetPortNumber());

	iSignalMask &= (KSignalBreak);	// set mask to zero, excluding BREAK
	if (iSignalChangeNotifyPending)
		{
		iSignalChangeNotifyPending = EFalse;
		SignalChangeCompleted(iSignals, KErrCancel);
		}
	}

void CPortC32InterfaceBase::StartRead(const TAny* aClientBuffer, TInt aLength)
/**
 * This method is called by C32 to read data from the CSY in response to a 
 * read request by a client RComm object.
 *
 * @param aClientBuffer - Pointer to where to put the new data
 * @param aLength - Length of data to read
 */
	{
	_LOG_L4C1(" "); // please leave in this separator
	_LOG_L4C3(">>CPortC32InterfaceBase::StartRead [aLength=%d, port=%d] called by client",
			 aLength, GetPortNumber());

	if (iIsReadInProgress)
		{
		// Something has gone wrong - either the flag was not reset correctly
		// or C32 has lost track of a read
		// MAF __ASSERT_DEBUG(EFalse,PANIC(KPanicIllegalState));
		_LOG_L1C1("** Read waiting to complete already set **");
		}

	// Set the flag that there is a read request outstanding
	iIsReadInProgress = ETrue;

	if (aLength == 0)
		{
		_LOG_L3C1("<<CPortC32InterfaceBase::StartRead - Length of 0?");
		CompleteReadRequest(KErrArgument);
		return;
		}

	// save client info
	iClientBuffer = (TAny*) aClientBuffer;
	iPos = 0;

	if (aLength > 0)
		{
		_LOG_L3C1("Normal read");
		iClientLength = aLength;
		iOneOrMore = EFalse;
		}
	else
		{
		_LOG_L3C1("Read one or more");
		iClientLength = -aLength;
		iOneOrMore = ETrue;
		}

	if(!ReadFromBufOrQueue())
		{
		_LOG_L3C1("No data to be read - request is outstanding");
		}

	_LOG_L4C1("<<CPortC32InterfaceBase::StartRead");
	_LOG_L4C1(" "); // please leave in this separator
	}

void CPortC32InterfaceBase::StartWrite(const TAny* aClientBuffer, TInt aLength)
/**
 * This method is called by C32 to write data to the CSY in response to a 
 * write request by a client RComm object.
 *
 * @param aClientBuffer - Pointer to where to put the new data
 * @param aLength - Length of data to read
 */
	{
	_LOG_L4C1(" "); // please leave in this separator
	_LOG_L4C3(">>CPortC32InterfaceBase::StartWrite [aLength=%d, port=%d] called by client",
			 aLength, GetPortNumber());

	if (iIsWriteInProgress)
		{
		// Something has gone wrong - either the flag was not reset correctly
		// or C32 has lost track of a write
		// MAF __ASSERT_DEBUG(EFalse,PANIC(KPanicIllegalState));
		_LOG_L1C1("** Write already waiting to complete **");
		iIsWriteInProgress = EFalse;
		}

	iIsWriteInProgress = ETrue;

	if (aLength > 0)
		{
		// Get the outgoing message buffer for this dlc/port
		TDes8& buffer = iMuxChannel->RefToMsgBuffer();

		// zero the buffer
		buffer.Zero();

		// read data from client's address space
		TInt err = IPCRead(aClientBuffer, buffer); 
		if (err == KErrNone)
			{
			iMuxChannel->SetOwner(this);
			err = iMuxChannel->PlaceOnOutboundQueue();
			}
		else
			{
			_LOG_L1C2("** Read of client buffer failed [err=%d] **",err);
			}

		if (err)
			{
			CompleteWriteRequest(err);
			}
		}
	else
		{
		// no data to write is not an error, so just complete
		CompleteWriteRequest(KErrNone);
		}

	_LOG_L4C1("<<CPortC32InterfaceBase::StartWrite");
	_LOG_L4C1(" "); // please leave in this separator
	}

void CPortC32InterfaceBase::Destruct()
/**
 * Called by C32 in response to a client Close request.
 */
	{
	_LOG_L4C2(">>CPortC32InterfaceBase::Destruct [port=%d]", GetPortNumber());

	// C32 requires us to do this here.
	delete this;

	_LOG_L4C1("<<CPortC32InterfaceBase::Destruct()");
	}

TInt CPortC32InterfaceBase::SetConfig(const TDesC8& aConfiguration)
/**
 * Called by C32 in response to a client calling RComm::SetConfig().
 * This method is used by the client to tell the CSY what parameters
 * are needed to instantiate the lower-level driver.
 * Note that the CSY reads the config data from CommDb and bpcaps.ini
 * and does not need or want the client to overwrite the config
 * parameters.
 *
 * @param aConfiguration - Descriptor containing the config params
 * @return KErrNone
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::SetConfig [port=%d]",iPortInfo.iPortNumber);

    TCommConfigV01 c;

    Mem::FillZ(&c,sizeof(c));
	TInt len=Min(aConfiguration.Length(),sizeof(c));
	Mem::Copy(&c, aConfiguration.Ptr(),len);

	if(c.iTerminatorCount > KConfigMaxTerminators)
		{
		return KErrNotSupported;
		}
	else
		{
		for (TInt i=0; i < c.iTerminatorCount;i++)
			{
			_LOG_L4C2("Terminator: %d" ,c.iTerminator[i]);
			}
		}

	iConfig = c;
	//we will only use Terminators of this configure
	return KErrNone;
	}

void CPortC32InterfaceBase::ReadCancel()
/**
 * Called by C32 when the client requests to cancel a Read.
 */
	{
	_LOG_L4C2(">>CPortC32InterfaceBase::ReadCancel [port=%d]", GetPortNumber());

	iClientBuffer = NULL;
	iClientLength = 0;

	CompleteReadRequest(KErrCancel);

	_LOG_L4C1("<<CPortC32InterfaceBase::ReadCancel()");
	}

void CPortC32InterfaceBase::WriteCancel()
/**
 * Called by C32 when the client requests to cancel a write operation.
 * Empty the mux queue of a possible message that is destined for the 
 * baseband on the channel associated with this C32 port interface
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::WriteCancel [port=%d]", GetPortNumber());

	iMuxChannel->WriteCancel();

	CompleteWriteRequest(KErrCancel);
	}

/*****************************************************************************/
// Functions below here are the C32 pure virtuals which must be implemented  //
// by the CSY, but may not be supported by the CSY.                          //
/*****************************************************************************/

void CPortC32InterfaceBase::Break(TInt /*aTime*/)
/**
 * Called by C32.
 * Note that this method is not supported.
 *
 * @param aTime
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::Break [port=%d]", GetPortNumber());
	}

void CPortC32InterfaceBase::BreakCancel()
/**
 * Called by C32.
 * Note that this method is not supported.
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::BreakCancel [port=%d]", GetPortNumber());
	}

TInt CPortC32InterfaceBase::GetConfig(TDes8& /*aPackage*/) const
/**
 * Called by C32.
 * Note that this method is not supported.
 * @param aPackage - Reference to package
 * @return KErrNotSupported
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::GetConfig [port=%d]", GetPortNumber());
	return KErrNotSupported;
	}

TInt CPortC32InterfaceBase::SetServerConfig(const TDesC8& /*aPackage*/)
/**
 * Called by C32.
 * Note that this method is not supported.
 * @param aPackage - Reference to package
 * @return KErrNotSupported
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::SetServerConfig [port=%d]", GetPortNumber());
	return KErrNotSupported;
	}

TInt CPortC32InterfaceBase::GetServerConfig(TDes8& /*aPackage*/)
/**
 * Called by C32.
 * Note that this method is not supported.
 * @param aPackage - Reference to package
 * @return KErrNotSupported
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::GetServerConfig [port=%d]", GetPortNumber());
	return KErrNotSupported;
	}

TInt CPortC32InterfaceBase::GetCaps(TDes8& aPackage)
/**
 * Called by C32.
 * @param aPackage - Reference to package
 * @return KErrNone
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::GetCaps [port=%d]", GetPortNumber());

	// create a Package pointer
	TCommCaps* pckg = (TCommCaps*) &aPackage;

	// create a V01 reference
	TCommCapsV01& caps = (*pckg)();

	// fill in the capabilities
	caps.iRate       = KCapsBps19200 | KCapsBps115200;

	caps.iDataBits   = KCapsData8;
	caps.iStopBits   = KCapsStop1;
	caps.iParity     = KCapsParityNone;
	caps.iHandshake  = KCapsSignalCTSSupported |
		               KCapsSignalDSRSupported |
					   KCapsSignalDCDSupported |
					   KCapsSignalRTSSupported |
					   KCapsSignalDTRSupported;
	caps.iSignals    = KCapsObeyCTSSupported | KCapsObeyDSRSupported;
	caps.iFifo       = KCapsHasFifo;
	caps.iSIR        = KCapsSIR115kbps;

	// TCommCapsV02 support
	if (aPackage.Length()==sizeof(TCommCapsV02))
		{
		TCommCapsV02* commcaps = (TCommCapsV02*)(aPackage.Ptr());
		commcaps->iNotificationCaps = 
			KNotifyFlowControlChangeSupported |
			KNotifyDataAvailableSupported;
		commcaps->iFlowControlCaps = KCapsFlowControlStatusSupported;
		}

	return KErrNone;
	}

TInt CPortC32InterfaceBase::GetSignals(TUint& aSignals)
/**
 * get the status of the signal pins
 *
 * @param aSignals signals will be written to this descriptor
 * @return KErrNone always
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::GetSignals [port=%d]", GetPortNumber());
	aSignals = iSignals;
	return KErrNone;
	}

TInt CPortC32InterfaceBase::SetSignalsToMark(TUint aSignals)
/**
 * set selected signals to high (logical 1)
 *
 * @param aSignals bitmask with the signals to be set
 * @return KErrNone always
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::SetSignalsToMark [port=%d]", GetPortNumber());

	TInt ret = KErrNone;
	TUint alreadySent = iSentSignals & aSignals;
	if (alreadySent == aSignals)
		{
		_LOG_L4C1("Signals already set to high");
		}
	else
		{
		TUint temp = iSentSignals | aSignals;
		ret = SetV24Signals(temp);
		if (ret == KErrNone)
			{
			iSentSignals = temp;
			_LOG_L4C2("iSentSignals 0x%08x",iSentSignals);
			}
		}
	
	return ret;
	}

TInt CPortC32InterfaceBase::SetSignalsToSpace(TUint aSignals)
/**
 * set selected signals to low (logical 0)
 *
 * @param aSignals bitmask with the signals to be cleared
 * @return KErrNone always
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::SetSignalsToSpace [port=%d]", GetPortNumber());

	TInt ret = KErrNone;
	TUint change = iSentSignals & aSignals;
	if (change)
		{
		TUint temp = iSentSignals & ~aSignals;
		ret = SetV24Signals(temp);
		if (ret == KErrNone)
			{
			iSentSignals = temp;
			_LOG_L4C2("iSentSignals 0x%08x",iSentSignals);
			}
		}
	else
		{
		_LOG_L4C1("Signals already set to low");
		}
	
	return ret;
	}

TInt CPortC32InterfaceBase::GetReceiveBufferLength(TInt& aLength) const
/**
 * Called by C32.
 * @param aLength
 * @return KErrNone
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::GetReceiveBufferLength [port=%d]", GetPortNumber());

	aLength = KMaxIpPacketSize;
	return KErrNone;
	}

TInt CPortC32InterfaceBase::SetReceiveBufferLength(TInt /*aLength*/)
/**
 * Called by C32.
 * Note that this method is not supported.
 * @param aLength
 * @return KErrNotSupported
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::SetReceiveBufferLength [port=%d]", GetPortNumber());
	return KErrNotSupported;
	}

void CPortC32InterfaceBase::NotifyConfigChange()
/**
 * Called by C32.
 * Note that this method is not supported.
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::NotifyConfigChange [port=%d]", GetPortNumber());
 	}

void CPortC32InterfaceBase::NotifyDataAvailable()
/**
 * Called by C32.
 * Note that this method is not supported.
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::NotifyDataAvailable [port=%d]", GetPortNumber());

	iDataAvailableNotifyPending = ETrue;
	}

void CPortC32InterfaceBase::NotifyDataAvailableCancel()
/**
 * Called by C32.
 * Note that this method is not supported.
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::NotifyDataAvailableCancel [port=%d]", GetPortNumber());

	if (iDataAvailableNotifyPending)
		{
		iDataAvailableNotifyPending = EFalse;
		NotifyDataAvailableCompleted(KErrCancel);
		}
	}

void CPortC32InterfaceBase::NotifyConfigChangeCancel()
/**
 * Called by C32.
 * Note that this method is not supported.
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::NotifyConfigChangeCancel [port=%d]", GetPortNumber());
	}

void CPortC32InterfaceBase::NotifyFlowControlChange()
/**
 * Called by C32.
 * Note that this method is not supported.
 */
	{
	_LOG_L3C2("CPortC32InterfaceBase::NotifyFlowControlChange [port=%d]", GetPortNumber());

	iFlowControlNotifyPending = ETrue;
	}

void CPortC32InterfaceBase::NotifyFlowControlChangeCancel()
/**
 * Called by C32 to cancel a previous notification request.
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::NotifyFlowControlChangeCancel [port=%d]", GetPortNumber());

	if (iFlowControlNotifyPending)
		{
		iFlowControlNotifyPending = EFalse;
		FlowControlChangeCompleted(iCsyAllowedToSendFrames, KErrCancel);
		}
	}

void CPortC32InterfaceBase::NotifyBreak()
/**
 * Called by C32.
 * Note that this method is not supported.
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::NotifyBreak [port=%d]", GetPortNumber());
	}

void CPortC32InterfaceBase::NotifyBreakCancel()
/**
 * Called by C32.
 * Note that this method is not supported.
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::NotifyBreakCancel [port=%d]", GetPortNumber());
	}

void CPortC32InterfaceBase::NotifyOutputEmpty()
/**
 * Called by C32.
 * Note that this method is not supported.
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::NotifyOutputEmpty [port=%d]", GetPortNumber());
	}

void CPortC32InterfaceBase::NotifyOutputEmptyCancel()
/**
 * Called by C32.
 * Note that this method is not supported.
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::NotifyOutputEmptyCancel [port=%d]", GetPortNumber());
	}

TInt CPortC32InterfaceBase::GetFlowControlStatus(TFlowControl& aFlowControl)
/**
 * Called by C32 to return the port's flow control state.
 *
 * @param aFlowControl Reference to location to write the flow control value
 * @return KErrNone
 */
	{
	_LOG_L3C2("CPortC32InterfaceBase::GetFlowControlStatus [Port=%d]", GetPortNumber());

	aFlowControl = EFlowControlOff;
	if ((iCsyAllowedToSendFrames == EFlowControlOn) ||
		(iMuxChannel->CsyToModemFlowControl() == EFlowControlOn))
		aFlowControl = EFlowControlOn;

	return KErrNone;
	}

TInt CPortC32InterfaceBase::GetRole(TCommRole& aRole)
/**
 * Called by C32 to return the port's role. The role will be either
 * DTE or DCE.
 * @param aRole - Reference to location to write the port role (DTE or DCE).
 * @return KErrNone
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::GetRole [port=%d]", GetPortNumber());

	aRole = iRole;  // or get from RDevComm instance ???
	return KErrNone;
	}

TInt CPortC32InterfaceBase::SetRole(TCommRole aRole)
/**
 * Called by C32 to set the port role to either DTE or DCE.
 * @param aRole - DTE or DCE
 * @return KErrNone.
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::SetRole [aRole=%d]", aRole);

#if defined _DEBUG
	if (aRole == ECommRoleDTE)
		{
		_LOG_L4C2("CPortC32InterfaceBase::SetRole() - DTE for Port %d", iPortInfo.iPortNumber);
		}
	else if (aRole == ECommRoleDCE)
		{
		_LOG_L4C2("CPortC32InterfaceBase::SetRole() - DCE for Port %d", iPortInfo.iPortNumber);
		}
	else
		{
		_LOG_L4C3("CPortC32InterfaceBase::SetRole() - Unknown Role %d for Port %d", aRole, iPortInfo.iPortNumber);
		}
#endif

	iRole = aRole;
	return KErrNone;
	}


void CPortC32InterfaceBase::FreeMemory()
/**
 * Called by C32 when the client wishes the CSY to free some memory. 
 * Note that this method does nothing. The CSY uses statically-allocated 
 * memory. In principle there is no memory that can be freed within the CSY.
 *
 *
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::FreeMemory [port=%d]", GetPortNumber());

	// MAF missing functionality
	}

/********************************************************************************/
/*                       End of methods from CPort                              */
/********************************************************************************/

void CPortC32InterfaceBase::CompleteWriteRequest(TInt aStatus)
/**
 * This method is called by the CSY object writing to the LDD and is used to
 * signal C32 that its message has been sent to the BP.  C32 is now free to
 * write another message to the CSY on this port interface.
 * @param aStatus - Status of message write to BP
 *
 */
	{
	_LOG_L4C3("CPortC32InterfaceBase::CompleteWriteRequest [aStatus=%d, Port=%d]", aStatus, GetPortNumber());

	if (iIsWriteInProgress)
		{
		iIsWriteInProgress = EFalse;
		WriteCompleted(aStatus);
		}
	else
		{
		_LOG_L3C1("* No write outstanding to complete *");
		}
	}

void CPortC32InterfaceBase::CompleteReadRequest(TInt aStatus)
/**
 * This method is called to complete the C32 client's read request.
 *
 * @param aStatus - Status read result
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::CompleteReadRequest [aStatus=%d]", aStatus);

	if (iIsReadInProgress)
		{
		iIsReadInProgress = EFalse;
		ReadCompleted(aStatus);
		}
	else
		{
		_LOG_L3C1("* No read outstanding to complete *");
		}
	}

void CPortC32InterfaceBase::Shutdown()
/**
 * Returns all the used memory 
 *
 *
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::Shutdown [port=%d]", GetPortNumber());

	// MAF missing functionality
	}

void CPortC32InterfaceBase::ModemAndCsyToClientFlowCtrl(const TFlowControl aFlowControl)
/**
 * This method is called to update the port's flow control state.
 * If a notification is pending then complete it.
 * @param aFlowControl New flow control state
 *
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::ModemAndCsyToClientFlowCtrl [aFlowControl=%d]", aFlowControl);
	_LOG_L4C2("[port=%d]", GetPortNumber());

	if (iCsyAllowedToSendFrames != aFlowControl)
		{
		iCsyAllowedToSendFrames = aFlowControl;
		if (iFlowControlNotifyPending)
			{
			iFlowControlNotifyPending = EFalse;
			FlowControlChangeCompleted(iCsyAllowedToSendFrames, KErrNone);
			}
		}
	}

TInt CPortC32InterfaceBase::SetV24Signals(const TUint aRs232Signals)
	{
	_LOG_L4C2("CPortC32InterfaceBase::SetV24Signals [aRs232Signals=0x%08x]",
		aRs232Signals);
	_LOG_L4C2("port=%d", GetPortNumber());

	// From 27.010 Sending
	//
	// Bit number/name	DTE			DCE
	//
	// 3, RTC			DTR			DSR
	// 4, RTR			RTS			CTS	
	// 7, IC			always 0	RI
	// 8, DV			always 1	DCD

	TUint8 v24signals = 0;
	if (iRole == ECommRoleDTE)
		{
		_LOG_L3C1("DTE sending entity");

		if (aRs232Signals & KSignalDTR)
			{
			_LOG_L3C1("DTR set (setting RTC)");
			v24signals |= KV24SignalRTC;
			}

		if (aRs232Signals & KSignalRTS)
			{
			_LOG_L3C1("RTS set (setting RTR)");
			v24signals |= KV24SignalRTR;
			}

		// DV always 1
		v24signals |= 0x80;
		}
	else if (iRole == ECommRoleDCE)
		{
		_LOG_L3C1("DCE sending entity");

		if (aRs232Signals & KSignalDSR)
			{
			_LOG_L3C1("DSR set (setting RTC)");
			v24signals |= KV24SignalRTC;
			}

		if (aRs232Signals & KSignalCTS)
			{
			_LOG_L3C1("CTS set (setting RTR)");
			v24signals |= KV24SignalRTR;
			}

		if (aRs232Signals & KSignalRNG)
			{
			_LOG_L3C1("RNG set (setting IC)");
			v24signals |= KV24SignalIC;
			}

		if (aRs232Signals & KSignalDCD)
			{
			_LOG_L3C1("DCD set (setting DV)");
			v24signals |= KV24SignalDV;
			}
		}
	else
		{
		_LOG_L3C1("** Unknown role **");
		}

	return iMuxChannel->SendMscCommand(v24signals);
	}

void CPortC32InterfaceBase::ReceivedV24Signals(const TUint8 aV24Signals)
	{
	_LOG_L4C2("CPortC32InterfaceBase::ReceivedV24Signals [aV24Signals=0x%x]",
		aV24Signals);
	_LOG_L4C2("port=%d", GetPortNumber());

	// From 27.010 Receiving (mapping from control signal octet)
	//
	// Bit number/name	DTE		DCE
	// 	
	// 3, RTC			DSR		DTR
	// 4, RTR			CTS		RFR/RTS
	// 7, IC			RI		-ignored
	// 8, DV			DCD		-ignored

	TUint signals = 0;
	if (iRole == ECommRoleDTE)
		{
		_LOG_L3C1("DTE receiving entity");

		// RTC (bit 3)
		if (aV24Signals & KV24SignalRTC)
			{
			_LOG_L3C1("Device ready to communicate (setting DSR)");
			signals |= KSignalDSR;
			}
		// RTR (bit 4)
		if (aV24Signals & KV24SignalRTR)
			{
			_LOG_L3C1("Device ready to receive (setting CTS)");
			signals |= KSignalCTS;
			}
		// IC (bit 7)
		if (aV24Signals & KV24SignalIC)
			{
			_LOG_L3C1("Incoming call (setting RNG)");
			signals |= KSignalRNG;
			}
		// DV (bit 8)
		if (aV24Signals & KV24SignalDV)
			{
			_LOG_L3C1("Data valid (setting DCD)");
			signals |= KSignalDCD;
			}
		}
	else if (iRole == ECommRoleDCE)
		{
		_LOG_L3C1("DCE receiving entity");

		// RTC (bit 3)
		if (aV24Signals & KV24SignalRTC)
			{
			_LOG_L3C1("Device ready to communicate (setting DTR)");
			signals |= KSignalDTR;
			}
		// RTR (bit 4)
		if (aV24Signals & KV24SignalRTR)
			{
			_LOG_L3C1("Device ready to receive (setting RTS)");
			signals |= KSignalRTS;
			}
		}
	else
		{
		_LOG_L3C1("** Unknown role **");
		}

	signals &= iSignalMask;
	if (signals != iSignals)
		{
		iSignals = signals;

		// notification processing
		if (iSignalChangeNotifyPending)
			SignalChangeCompleted(iSignals, KErrNone);
		}
	}

void CPortC32InterfaceBase::SetMuxChannel(CChannelMgrCmdData* aMuxChannel)
/**
 * This method sets the flow control state for the port object
 * by reading the specified channel's flow control state.
 * @param aMuxChannel - Pointer to the channel
 *
 */
	{
	iMuxChannel = aMuxChannel;
	if (iMuxChannel != NULL)
		{
		ModemAndCsyToClientFlowCtrl(iMuxChannel->
			GetModemAndCsyToClientFlowControl());
		}
	else
		{
		_LOG_L1C1("** SetMuxChannel - channel not ready **");
		}
	}

void CPortC32InterfaceBase::SetDataAvailable()
/**
 * This method is called to set the port's data available state.
 * If the data available notification is pending then complete it.
 */
	{
	_LOG_L4C2("CPortC32InterfaceBase::SetDataAvailable [port=%d]", GetPortNumber());

	if (iDataAvailableNotifyPending)
		{
		iDataAvailableNotifyPending = EFalse;
		NotifyDataAvailableCompleted(KErrNone);
		}
	}

void CPortC32InterfaceBase::CompleteOutstandingRequest()
/**
 * This method is called to complete an outstanding request.
 * There can be read and write requests that are outstanding.
 */
	{
	_LOG_L4C1("CPortC32InterfaceBase::CompleteOutstandingRequest");

	if (iIsReadInProgress)
		CompleteReadRequest(KErrCancel);

	if (iIsWriteInProgress)
		CompleteWriteRequest(KErrCancel);
	}
	

