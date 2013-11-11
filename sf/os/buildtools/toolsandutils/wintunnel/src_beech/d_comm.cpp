// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Evil communications corrupt good manners. 
// 1 corinthians  xv. 33.
// 
//


#include <k32std.h>
#include "d_comm.h"
#include <e32hal.h>

const TInt KIdleTimeoutPeriod=20*1000000;  // 20 seconds
const TUint KXoffSignal=0x80000000;
//
const TInt KInputHeldFree=(-1);
const TInt KInputHeld=(-2);
//
const TUint KTransmitting=0x01;
const TUint KBreaking=0x02;
const TUint KBreakPending=0x04;
//
enum TPanic
	{
	ESetConfigWhileRequestPending,
	ESetSignalsSetAndClear,
	EResetBuffers,
	ESetReceiveBufferLength,
//	EDoubleTxBufFill,
	};

EXPORT_C DLogicalDevice *CreateLogicalDevice()
//
// Create a new device
//
	{
	return(new DDeviceComm);
	}

DDeviceComm::DDeviceComm()
//
// Constructor
//
	{
	iParseMask=KDeviceAllowAll;
	iUnitsMask=0xffffffff; // Leave units decision to the driver
	iVersion=TVersion(KCommsMajorVersionNumber,KCommsMinorVersionNumber,KCommsBuildVersionNumber);
	}

TInt DDeviceComm::Install()
//
// Install the device driver.
//
	{
    TPtrC name=_L("Comm");
	return(SetName(&name));
	}

void DDeviceComm::GetCaps(TDes8 &aDes) const
//
// Return the Comm capabilities.
//
	{

	TCapsDevCommV01 b;
	b.version=iVersion;
	aDes.FillZ(aDes.MaxLength());
	aDes.Copy((TUint8 *)&b,Min(aDes.MaxLength(),sizeof(b)));
	}

DLogicalChannel *DDeviceComm::CreateL()
//
// Create a channel on the device.
//
	{

	return(new(ELeave) DChannelComm(this));
	}

TInt DChannelComm::CallDfc(TAny* aDChannelComm)
//
// Dfc call back - cast the pointer to a DChannel comm and call DoDfc
//
	{
	((DChannelComm *)aDChannelComm)->DfcHandler();
	return KErrNone;
	}

//
// Comms power handler
//
// Active tx requests keep power requirement up.
// Completed tx requests keep power requirement up for KIdleTimeoutPeriod.
// Rx requests don't effect power requirement
// 
DCommPowerHandler::DCommPowerHandler(DChannelComm *aLdd) : iLdd(aLdd)
	{}

void DCommPowerHandler::RequestPending()
//
// Tx request made
//
	{
	iTimer.Cancel();
	if (iRequirement == 0)
		SetRequirement(1); /* EPowerCpu */
	}

void DCommPowerHandler::RequestComplete()
//
// Tx request completed, set the timer
//
	{
	iTimer.Cancel();
	iTimer.OneShotInMicroSeconds(TTimeIntervalMicroSeconds32(KIdleTimeoutPeriod), DCommPowerHandler::Timeout, this);
	}

void DCommPowerHandler::Timeout(TAny *aHandler, TInt)
//
// Timeout following last Tx request (in DFC)
//
	{

	DCommPowerHandler& handler=*(DCommPowerHandler*)aHandler;
	handler.SetRequirement(0);  // EPowerNone
	handler.iLdd->DoPowerDown(); 
	handler.iLdd->ResetDevice();
	handler.iLdd->CompleteAll(KErrAbort); // Complete all outstanding client requests
	}

TInt DCommPowerHandler::DoPowerOn()
//
// Machine power up event
// Serial driver ignores machine power up events and instead powers up the
// device in response to user requests 
//
	{
	return(KErrNone);
	}

void DCommPowerHandler::DoPowerEmergencyStandby()
//
// Machine emergency power down event
// Can't ignore these events - need to power down the device
//
	{
	iLdd->DoEmergencyPowerDown();
	}

void DCommPowerHandler::DoPowerStandby()
//
// Machine normal power down event
// Serial driver ignores machine normal power down events and instead uses an
// inactivity timer on Tx requests to control power down.
//
	{
	}

#pragma warning( disable : 4705 )	// statement has no effect
#pragma warning( disable : 4355 )	// this used in intializer list
DChannelComm::DChannelComm(DLogicalDevice *aDevice)
//
// Constructor
//
: DLogicalChannel(aDevice), iDfc(TCallBack(DChannelComm::CallDfc,this))
	{

//
// Require power notifications and 3 valid requests
//
	SetBehaviour(RBusDevComm::ERequestReadCancel|RBusDevComm::ERequestWriteCancel|RBusDevComm::ERequestBreakCancel|RBusDevComm::ERequestNotifySignalChangeCancel);
//
// Setup the default config
//
	iConfig.iRate=EBps9600;
	iConfig.iDataBits=EData8;
	iConfig.iStopBits=EStop1;
	iConfig.iParity=EParityNone;
	iConfig.iFifo=EFifoEnable;
	iConfig.iHandshake=KConfigObeyCTS;
	iConfig.iParityError=KConfigParityErrorFail;
	iConfig.iSIREnable=ESIRDisable;
//	iConfig.iTerminatorCount=0;
//	iConfig.iTerminator[0]=0;
//	iConfig.iTerminator[1]=0;
//	iConfig.iTerminator[2]=0;
//	iConfig.iTerminator[3]=0;
	iConfig.iXonChar=0x11; // XON
	iConfig.iXoffChar=0x13; // XOFF
//	iConfig.iSpecialRate=0;
//	iConfig.iParityErrorChar=0;
//	iDriver=NULL;
	iStatus=ENotActive;
	iInputHeld=KInputHeldFree;
//	iOutputHeld=0;
//	iReceiveOneOrMore=0;
//	iReceiveError=0;
//	iReceiveErrorLength=0;
//	iReceiveLength=0;
//	iReceiveOffset=0;
//	iTransmitLength=0;
//	iTransmitOffset=0;
//	iFlags=0;
//	iSignals=0;
//	iLowWaterMark=0;
//	iHighWaterMark=0;
//	iHalfFullMark=0;
//	iReceivePtr=NULL;
//	iTransmitPtr=NULL;
//	iRxBuf=NULL;
//	iTxBuf=NULL;
//	iIsReading=EFalse;
//	iIsWriting=EFalse;
//	iDrainingRxBuf=EFalse;
//	iFillingTxBuf=EFalse;
//	iRunningDfc=EFalse;
//	iSignalChangeNotification=EFalse;
//	iSignalChangePtr=NULL;
//	iSignalMask=0;
//  iSignalChangeInfo=0;
//	iDataAvailableNotification=EFalse;
	}
#pragma warning( default : 4705 )
#pragma warning( disable : 4355 )

DChannelComm::~DChannelComm()
//
// Destructor
//
	{
	// Complete any outstanding requests
	CompleteAll(KErrAbort);

	// Power down h/w, disable interrupts
	iPowerHandler->iTimer.Cancel();
	iPowerHandler->PowerStandby();  	// Doesn't do anything in inactivity power policy case, hence DoPowerDown() call below.
	DoPowerDown(); 

	// clean up the power handler
//	iPowerHandler->SetRequirement(0);	// No need for this, done in DoPowerDown().
	Power::RemovePowerHandler(*iPowerHandler);
	delete iPowerHandler;

	TInt clearMask=0;
	if (!(iConfig.iHandshake&KConfigFreeRTS))
		clearMask|=KSignalRTS;
	if (!(iConfig.iHandshake&KConfigFreeDTR))
		clearMask|=KSignalDTR;
	iDriver->SetSignals(0,clearMask);

	delete iDriver;

	delete iRxBuf;
	delete iTxBuf;
	}

void DChannelComm::DoCreateL(TInt /*aUnit*/,CBase *aDriver,const TDesC * /*anInfo*/,const TVersion &aVer)
//
// Create the channel from the passed info.
//
	{
	iDriver=(DComm *)aDriver; // Must do this to ensure the driver is destroyed by the destructor
	if (!User::QueryVersionSupported(TVersion(KCommsMajorVersionNumber,KCommsMinorVersionNumber,KCommsBuildVersionNumber),aVer))
		User::Leave(KErrNotSupported);
	// get power managed
	iPowerHandler=new (ELeave) DCommPowerHandler(this);
	User::LeaveIfError(Power::AddPowerHandler(*iPowerHandler));
//	iRxBuf=new(ELeave) CCirBuf<TInt>;
//	iTxBuf=new(ELeave) CCirBuffer;
	iTxBuf=new(ELeave) CCommTxBuf;
	iRxBuf=new(ELeave) CCommRxBuf;
	User::LeaveIfError(SetRxBufferSize(KDefaultRxBufferSize));
	iTxBuf->SetLengthL(KTxBufferSize);
	((DComm *)aDriver)->SetComm(this); // Rog
	iDriver->CheckConfig(iConfig);
	StateIsr(iDriver->Signals());

	iPowerHandler->PowerOn();	// Ask the PowerManager to power us up
	}

void DChannelComm::DfcHandler()
//
// Called as a Dfc. Handles all buffer draining and filling and message completions.
//
	{	
	iDriver->DisableInterrupts();
	while (iDfcRequest!=0)
		{
		if (iDfcRequest & EDrainRxBufferRequired)
			{
#ifdef _DEBUG_DEVCOMM
			iDfcHandlerSeq = iRxBuf->iRemSeqNum.Int();
#endif
			iDfcRequest &= ~EDrainRxBufferRequired;
			iDriver->EnableInterrupts();
			if (!iDrainingRxBuf)
				DoDrainRxBuffer();
			iDriver->DisableInterrupts();
			continue;
			}
		if (iDfcRequest & ECompleteWrite)
			{
			iDfcRequest &= ~ECompleteWrite;
			iDriver->EnableInterrupts();
			if (iIsWriting)
				{
				iIsWriting = EFalse;
				iWriteZero = EFalse;
				if (iIsClientWriting)
					Complete(RBusDevComm::ERequestWrite,iWriteError);
				iIsClientWriting = EFalse;
				iWriteError = KErrNone;
				}
			iDriver->DisableInterrupts();
			continue;
			}
		else if (iDfcRequest & EEarlyCompleteWrite)
			{
			iDfcRequest &= ~EEarlyCompleteWrite;
			iDriver->EnableInterrupts();
			if (iIsClientWriting)
				{
				iIsClientWriting = EFalse;
				Complete(RBusDevComm::ERequestWrite,iWriteError);
				iWriteError = KErrNone;
				}
			iDriver->DisableInterrupts();
			continue;
			}
		if (iDfcRequest & ECompleteEmergency)
			{
			iDfcRequest &= ~ECompleteEmergency;
			ResetDevice();
			CompleteAll(KErrBadPower);
			continue;
			}
		if (iDfcRequest & EFillTxBufferRequired)
			{
			iDfcRequest &= ~EFillTxBufferRequired;
			iDriver->EnableInterrupts();
			if (!iFillingTxBuf)
				DoFillTxBuffer();
			iDriver->DisableInterrupts();
			continue;
			}
		if (iDfcRequest & ECompleteSignalChange)
			{
			iDfcRequest &= ~ECompleteSignalChange;
    		iThread->Write(iSignalChangePtr,(TUint8*)&iSignalChangeInfo,(TInt)sizeof(TUint));
			iSignalChangeNotification=EFalse;
			Complete(RBusDevComm::ERequestNotifySignalChange);
			continue;
			}
		if (iDfcRequest & ECompleteDataAvailable)
			{
			iDfcRequest &= ~ECompleteDataAvailable;
			iDataAvailableNotification=EFalse;
			Complete(RBusDevComm::ERequestRead);
			continue;
			}
		}
#ifdef _DEBUG_DEVCOMM
	--iDfcCount;
#endif
	iRunningDfc=EFalse;
	iPowerHandler->RequestComplete();
	iDriver->EnableInterrupts();
	}

void DChannelComm::RestoreDtrRrs()
//
// Check if driver is configured not to auto control RTS/DTR and restore state
// of these signals acording to current config. (as specified by driver
// client) if necessary
//
	{

	TUint hand=iConfig.iHandshake;
	if ((hand&KConfigFreeRTS))
		{
		if (iSignals&KSignalRTS)
			iDriver->SetSignals(KSignalRTS,0);
		else
			iDriver->SetSignals(0,KSignalRTS);
		}
	if (!iPowerHandler->PowerGood())
		return;
	if (hand&KConfigFreeDTR)
		{
		if (iSignals&KSignalDTR)
			iDriver->SetSignals(KSignalDTR,0);
		else
			iDriver->SetSignals(0,KSignalDTR);
		}
	}

void DChannelComm::Start()
//
// Start the driver receiving.
//
	{

	// If waiting for driver to be closed then just return
	if (iStatus==EClosed)
		return;

	if (iStatus!=EActive)
		ResetBuffers();	// Reset RX/TX buffers

	iDriver->Configure(iConfig);
	iDriver->Start();
	if (iConfig.iHandshake & KConfigSendXoff && iInputHeld>=0)
		EnableTransmit(); // Send XOn if there is one

	if (iStatus!=EActive)
		{
		// If driver configured not to auto control RTS/DTR then restore state of
		// these signals acording to current config. (as specified by driver client).
		TUint hand=iConfig.iHandshake;
		if (hand&(KConfigFreeRTS|KConfigFreeDTR))
			RestoreDtrRrs();
		}
	iStatus=EActive;
	}

void DChannelComm::Stop(TStopMode aMode)
//
// Stop the driver.
//
	{
	iDriver->Stop(aMode);
	}

void DChannelComm::BreakOn()
//
// Start the driver breaking.
//
	{

	if (iFlags&KTransmitting)
		iFlags|=KBreakPending;
	else
		{
		iFlags&=(~KBreakPending);
		iFlags|=KBreaking;
		iDriver->Break(ETrue);
		}
	}

void DChannelComm::BreakOff()
//
// Stop the driver breaking.
//
	{

	iDriver->Break(EFalse);
	iFlags&=(~(KBreakPending|KBreaking));
	}

void DChannelComm::EnableTransmit()
//
// Make sure we are transmitting
//
	{

	if ((iFlags & KTransmitting)==0)
		{
		iFlags |= KTransmitting; // Signal the fact the we are transmitting
		iDriver->EnableTransmit(); // Start transmitting
		}
	}

TInt DChannelComm::SetRxBufferSize(TInt aSize)
//
// Set the receive buffer size.
//
	{

	TRAPD(r,iRxBuf->SetLengthL(aSize));
	if (r==KErrNone)
		{
		iLowWaterMark=(aSize>>2); // 25 %
		iHalfFullMark=(aSize>>1); // 50 %
		if (iConfig.iParityError & KConfigXonXoffDebug)
			{
			iHighWaterMark=iHalfFullMark-5;
			}
		else
			iHighWaterMark=aSize-iLowWaterMark; // 75%
		}
	return(r);
	}

void DChannelComm::ResetBuffers()
//
// Reset the receive and transmit buffers.
//
	{

	iIsWriting=EFalse;
	iRxBuf->Reset();
	iTxBuf->Reset();

	iReceiveError=KErrNone;
	if (iStatus==EActive)
		{
		if ((iConfig.iHandshake & KConfigFreeRTS)==0)
            iDriver->SetSignals(KSignalRTS,0); // Assert RTS
		if ((iConfig.iHandshake & KConfigFreeDTR)==0)
            iDriver->SetSignals(KSignalDTR,0); // Assert DTR
        if (iConfig.iHandshake&KConfigSendXoff) // Doing input XON/XOFF
            {
            iInputHeld=iConfig.iXonChar; // set up to send Xon
			EnableTransmit(); // Make sure we are transmitting
            }
		else
			iInputHeld=KInputHeldFree;
		}
	iReceiveErrorLength=0;
	}

TBool DChannelComm::IsLineFail(TUint aHandshake)
//
// True if any line fail errors occured.
//
    {

	if ((iSignals&KSignalCTS)==0 &&
		(aHandshake&(KConfigObeyCTS|KConfigFailCTS))==(KConfigObeyCTS|KConfigFailCTS) ||
		(iSignals&KSignalDSR)==0 &&
		(aHandshake&(KConfigObeyDSR|KConfigFailDSR))==(KConfigObeyDSR|KConfigFailDSR) ||
		(iSignals&KSignalDCD)==0 &&
		(aHandshake&(KConfigObeyDCD|KConfigFailDCD))==(KConfigObeyDCD|KConfigFailDCD))
		return(TRUE);
	return(FALSE);
    }


void DChannelComm::DoDrainRxBuffer()
//
// Drain the receive buffer
//
	{
	iDriver->DisableInterrupts();
	if (!IsReading())
		{
		iDriver->EnableInterrupts();
		return;
		}
	if (iDrainingRxBuf)
		{
		iDriver->EnableInterrupts();
		return;
		}
	iDrainingRxBuf = ETrue;

#ifdef _DEBUG_DEVCOMM
	iDoDrainSeq = iRxBuf->iRemSeqNum.Int();
#endif

	TUint status;
	TInt res = iRxBuf->ClientWrite(status, iDriver, iThread, iReceivePtr, iReceiveLength, iReceiveOffset, KChunkShiftBy0);
	if (res<0 && iReceiveError==KErrNone) // Buffer empty
		{
		iDrainingRxBuf = EFalse;
		iDriver->EnableInterrupts();
		return;
		}

	if (res>0 || (status & KReceiveIsrMaskComplete) || iReceiveError!=KErrNone) // Complete client op
		{
		if (iReceiveError==KErrNone && (status & KReceiveIsrMaskError))
			{
			if (status & KReceiveIsrOverrunError)
				iReceiveError = KErrCommsOverrun;
			else if (status & KReceiveIsrFrameError)
				iReceiveError = KErrCommsFrame;
			else if ((status & KReceiveIsrParityError) && !(iConfig.iParityError & KConfigParityErrorIgnore))
				iReceiveError = KErrCommsParity;
			}
		
		Complete(RBusDevComm::ERequestRead, iReceiveError);
		iReceiveError = KErrNone;
		iIsReading=EFalse;
		}

	if (iRxBuf->Count()<=iLowWaterMark && iInputHeld==KInputHeld) // At or below the low water mark and input held
        {
		if ((iConfig.iHandshake & KConfigFreeRTS)==0)
            iDriver->SetSignals(KSignalRTS,0); // Assert RTS
		if ((iConfig.iHandshake & KConfigFreeDTR)==0)
            iDriver->SetSignals(KSignalDTR,0); // Assert DTR
        if (iConfig.iHandshake&KConfigSendXoff) // Doing input XON/XOFF
            {
            iInputHeld=iConfig.iXonChar; // set up to send Xon
			EnableTransmit(); // Make sure we are transmitting
            }
        else
            iInputHeld=KInputHeldFree; // Mark input free 
		}
	iDrainingRxBuf = EFalse;
	iDriver->EnableInterrupts();
	}


void DChannelComm::DoFillTxBuffer()
//
// Fill the transmit buffer
//
	{
	iDriver->DisableInterrupts();
	if (iFillingTxBuf)
		{
		iDriver->EnableInterrupts();
		return;
		}
	iFillingTxBuf=ETrue;
	iDriver->EnableInterrupts();

	if (iTxBuf->ClientRead(iDriver, iThread, iTransmitPtr, iTransmitLength, iTransmitOffset, KChunkShiftBy0)==KErrNone)
		EnableTransmit();

	if (iIsClientWriting && (iTransmitPtr==NULL || iTransmitLength==0) && (iConfig.iHandshake & KConfigWriteBufferedComplete))
		EarlyCompleteWrite();
	iFillingTxBuf=EFalse;
	}

void DChannelComm::ReceiveIsr(TUint aChar)
//
// Handle a received character from the ISR.
//
	{

	if (iDataAvailableNotification)
		CompleteDataAvailable();

	if (iConfig.iHandshake & KConfigObeyXoff) // Doing output XON/XOFF
        {
        if (aChar==iConfig.iXonChar) // Output now allowed
            { // May get multiple XON's in quick succession
            iOutputHeld &= ~KXoffSignal; // Mark output ok. for XON/XOFF
#ifdef _DEBUG_DEVCOMM
			iRxXon++;
#endif
			EnableTransmit();
			return;
            }
        if (aChar==iConfig.iXoffChar) // Output now disallowed
            {
#ifdef _DEBUG_DEVCOMM
			iRxXoff++;
#endif
            iOutputHeld |= KXoffSignal; // Mark output held for XON/XOFF
			DrainRxBuffer();
			return;
            }
        }

#ifdef _DEBUG_DEVCOMM
	iRxChars++;
#endif

	TBool doDrainRxBuffer=EFalse;

	TInt count = iRxBuf->Count();

    if (count>=iHighWaterMark && (count & 1)) // At or above the high water mark send xoff every other character
        {
		// Copes with the S/W and H/W handshaking
        if ((iConfig.iHandshake & KConfigFreeRTS)==0) // RTS enabled
			iDriver->SetSignals(0,KSignalRTS); // Drop RTS
		else if ((iConfig.iHandshake & KConfigFreeDTR)==0) // DTR enabled, but not RTS
			iDriver->SetSignals(0,KSignalDTR); // Drop DTR
        
		if (iConfig.iHandshake & KConfigSendXoff) // Doing input XON/XOFF
			{
			iInputHeld=iConfig.iXoffChar; // Mark input held
			EnableTransmit(); // Make sure we are transmitting
            }
		else
			iInputHeld=KInputHeld; // Mark input held

		doDrainRxBuffer=ETrue;
        }

	// Check for parity errors and replace char if so configured.
	if (aChar & KReceiveIsrParityError)
		{
		// Replace bad character
		if (iConfig.iParityError==KConfigParityErrorReplaceChar)
			aChar = aChar & ~(0xff|KReceiveIsrParityError) | iConfig.iParityErrorChar;
		// Ignore parity error
		if (iConfig.iParityError==KConfigParityErrorIgnore)
			aChar = aChar & ~KReceiveIsrParityError;
		}
	
	// Check for terminating character
	for (TInt i=0; i<iConfig.iTerminatorCount; i++)
		{
		if (iConfig.iTerminator[i]==aChar)
			{
			aChar |= KReceiveIsrTermChar;
			doDrainRxBuffer=ETrue;
			break;
			}
		}

	TInt e;
	if (e = iRxBuf->PutChar(aChar), e!=0)
		{
		if (e<0)
			{
			if (e==KErrOverflow)
				e = KErrCommsOverrun;
			iReceiveError = e;
			}
		doDrainRxBuffer = ETrue;
		}

	// Determine if the request will be completed by the arrival of the character
    if (IsReading() && iReceiveLength>0)
        {
		// If the buffer is full enough to complete the request
		// or over the half full mark the drain the buffer
		TInt count=iRxBuf->Count();
        if (iReceiveLength<=count || count>=iHalfFullMark || (aChar & KReceiveIsrMaskComplete))
            doDrainRxBuffer=ETrue;
        }

	if (doDrainRxBuffer)
		DrainRxBuffer();
	}

	
TInt DChannelComm::TransmitIsr()
//
// Return the next character to be transmitted to the ISR
//
	{

	TInt tChar=KTransmitIrqEmpty; // No character to be sent
    if (iInputHeld>=0) // Control character to send
        {
        tChar=iInputHeld; // Send the control character
#ifdef _DEBUG_DEVCOMM
		if (tChar==iConfig.iXonChar)
			iTxXon++;
		else
			iTxXoff++;
#endif
        iInputHeld=(tChar==iConfig.iXonChar ? KInputHeldFree : KInputHeld);        
		}
    else if (iOutputHeld==0) // Is the output free
        {
        if (IsWriting()) // Anything to be transmitted
            {
			tChar=iTxBuf->Get();
		    if (tChar<0 && iTransmitLength==0) // No characters left to send
				{
				tChar=KTransmitIrqEmpty;
				if (iWriteZero)
					{
					TUint stat = iDriver->Signals();
					stat &= (KSignalCTS|KSignalDSR|KSignalDCD|KXoffSignal);
					stat ^= (KSignalCTS|KSignalDSR|KSignalDCD);
					if (!(iConfig.iHandshake & KConfigObeyCTS)) // If ignoring CTS
						stat &= (~KSignalCTS);
					if (!(iConfig.iHandshake & KConfigObeyDSR)) // If ignoring DSR
						stat &= (~KSignalDSR);
					if (!(iConfig.iHandshake & KConfigObeyDCD)) // If ignoring DCD
						stat &= (~KSignalDCD);
					if (iOutputHeld & KXoffSignal)
						stat |= KXoffSignal; // Leave the xon/xoff handshake bit
					if ((stat & ~KXoffSignal)==0)
						CompleteWrite();
					}
				else
					{
					if (!(iConfig.iHandshake&KConfigWriteBufferedComplete))
						CompleteWrite();
					}
				}
			else if (iTransmitLength!=0 && iTxBuf->Count()<(KTxBufferSize>>1)) // Less than half full
				FillTxBuffer();
#ifdef _DEBUG_DEVCOMM
			if (tChar!=KTransmitIrqEmpty)
				iTxChars++;
#endif
            }
        }

    if (tChar!=KTransmitIrqEmpty) // Character to be sent
        iFlags|=KTransmitting;
	else
		iFlags&=(~KTransmitting);

    return(tChar);
	}

void DChannelComm::StateIsr(TUint aStatus)
//
// Handle a state change from the ISR
//
	{
	if (iSignalChangeNotification && ((iSignals^aStatus)&iSignalMask))
		{
        iSignalChangeInfo=(((iSignals^aStatus)&iSignalMask)<<12)|(aStatus&iSignalMask);
		CompleteSignalChange();
		}
    iSignals=aStatus; // Update the status
    TUint handshake=iConfig.iHandshake; // Get the handshake characteristic

    if (IsLineFail(handshake))
		{
//		CompleteAll(KErrCommsLineFail);	// Can't just complete reads from interrupts.
//		iIsReading=EFalse;
		iReceiveError=KErrCommsLineFail;
		DrainRxBuffer();

		if (IsWriting())
			{
			iWriteError = KErrCommsLineFail;
			CompleteWrite();
			}
		}
		
//
// Now we must determine if output is to be held
//
	aStatus &= (KSignalCTS|KSignalDSR|KSignalDCD|KXoffSignal);
    aStatus ^= (KSignalCTS|KSignalDSR|KSignalDCD);

    if (!(handshake & KConfigObeyCTS)) // If ignoring CTS
        aStatus &= (~KSignalCTS);

    if (!(handshake & KConfigObeyDSR)) // If ignoring DSR
        aStatus &= (~KSignalDSR);

    if (!(handshake & KConfigObeyDCD)) // If ignoring DCD
        aStatus &= (~KSignalDCD);

    if (iOutputHeld & KXoffSignal)
        aStatus |= KXoffSignal; // Leave the xon/xoff handshake bit

//
// For write zero, we complete if the lines indicate we could write....
//
	if (iWriteZero && (aStatus & ~KXoffSignal)==0)
		{
		if (iIsClientWriting && iTxBuf->Count()==0)
			{
			CompleteWrite();
			return;
			}
		}

//
// If request outstanding AND we were holding AND not held anymore
//
    if (aStatus==0 && /*iTransmitPtr!=0*/ IsWriting() /*&& iOutputHeld!=0*/) 
        {
        iOutputHeld=0; // Not holding any more
        EnableTransmit(); // Start transmitting again
        }
    else
        iOutputHeld=aStatus; // Set the held bits
	}


void DChannelComm::DoCancel(TInt aReqNo)
//
// Cancel an outstanding request.
//
	{

	switch (aReqNo)
		{
	case RBusDevComm::ERequestRead:
		iIsReading=EFalse;
		iDataAvailableNotification=EFalse;
		break;
	case RBusDevComm::ERequestWrite:
		iIsClientWriting=EFalse;
		iWriteZero=EFalse;
		iIsWriting=EFalse;
		iTxBuf->Reset();
		break;
	case RBusDevComm::ERequestBreak:
		BreakOff();
		break;
	case RBusDevComm::ERequestNotifySignalChange:
		iSignalChangeNotification=EFalse;
		break;
		}
	}

void DChannelComm::DoRequest(TInt aReqNo,TAny *a1,TAny *a2)
//
// Async requests.
//
	{

	TBool dataAvailReq=aReqNo&KDataAvailableNotifyFlag;
	aReqNo&=(~KChanRequestNoReservedMask); // Mask off device specific flags

	//
	// First check if we have started
	//
	if (iStatus==ENotActive)
		{
		Start();
		if (!(iConfig.iHandshake & KConfigFreeDTR))
			iDriver->SetSignals(KSignalDTR,0); // Assert DTR
		if (!(iConfig.iHandshake & KConfigFreeRTS))
			iDriver->SetSignals(KSignalRTS,0); // Assert RTS
		if (iConfig.iHandshake & KConfigSendXoff)
			iInputHeld = iConfig.iXonChar;
		else
			iInputHeld = KInputHeldFree;
		}

	//
	// Next we check for a line fail
	//
	if (IsLineFail(iConfig.iHandshake))
		{
		Complete(aReqNo,KErrCommsLineFail);
		return;
		}

	//
	// Now we can dispatch the request
	//
	TInt len;
	switch (aReqNo)
		{
	case RBusDevComm::ERequestRead:
		{
		if (dataAvailReq)
			{
			iPowerHandler->RequestPending();
			iDataAvailableNotification=ETrue;
			}
		else
			{
			iDriver->DisableInterrupts();
			iReceivePtr=(TUint8 *)a1;
			len=(*((TInt *)a2));
			if (len<0)
				{
				iReceiveOneOrMore=TRUE;
				iReceiveLength=Min(-len,iRxBuf->Count());
				if (iReceiveLength==0)
					iReceiveLength=1;
				}
			else
				{
				TInt maxLength=iThread->GetDesMaxLength(a1);
				if(len>maxLength || maxLength<0)
					{
					iDriver->EnableInterrupts();
					Complete(RBusDevComm::ERequestRead,KErrGeneral);
					break;
					}
				iReceiveOneOrMore=FALSE;
				iReceiveLength=len;
				if (iReceiveLength==0)
					{
					iDriver->EnableInterrupts();
					Complete(RBusDevComm::ERequestRead);
					break;
					}
				}
			if (iReceiveError && iReceiveLength>iReceiveErrorLength)
				iReceiveLength=iReceiveErrorLength;
			iReceiveOffset=0;
			iIsReading=ETrue;
			iPowerHandler->RequestPending();
			iDriver->EnableInterrupts();
			if(iConfig.iHandshake&KConfigSendXoff)
				{
				iInputHeld=iConfig.iXonChar;
				EnableTransmit();
				}
			
			DoDrainRxBuffer();
			}
		}
		break;
	case RBusDevComm::ERequestWrite:
		if (iStatus==EClosed)
			{
			Complete(aReqNo,KErrNotReady);
			iPowerHandler->RequestComplete();
			break;
			}
		iPowerHandler->RequestPending();
		iDriver->DisableInterrupts();
		if ((iTransmitLength=(*((TInt *)a2)))==0)
			{
			iWriteZero=ETrue;
			iIsWriting=ETrue;
			iIsClientWriting=ETrue;
			iDriver->EnableInterrupts();
			}
		else
			{
			iTransmitPtr=(TUint8 *)a1;
			iTransmitOffset=0;
			iIsClientWriting=ETrue;
			if((iFlags&KTransmitting) && iIsWriting) // let the transmit isr deal with it
			    {
				iDriver->EnableInterrupts();
				StateIsr(iDriver->Signals());
				break;
			    }
			iIsWriting=EFalse;
			iDriver->EnableInterrupts();
			DoFillTxBuffer();
			iIsWriting=ETrue;
			}
		StateIsr(iDriver->Signals());
		break;
	case RBusDevComm::ERequestBreak:
		Complete(aReqNo,KErrNotSupported);
		break;
	case RBusDevComm::ERequestNotifySignalChange:
		iSignalChangePtr=(TUint*)a1;
		iSignalMask=*(TUint*)a2;
		iSignalChangeNotification=ETrue;
		break;
		}
	}

TInt DChannelComm::DoControl(TInt aFunction,TAny *a1,TAny *a2)
//
// Sync requests.
//
	{

	TCommConfigV01 c;
	TInt len;
	TInt r=KErrNone;

	TBool restart = EFalse;
	TBool purge = EFalse;
	TBool rescan = EFalse;

	switch (aFunction)
		{
	case RBusDevComm::EControlConfig:
		((TDes8 *)a1)->FillZ(((TDes8 *)a1)->MaxLength());
		len=Min(((TDes8 *)a1)->MaxLength(),sizeof(iConfig));
		((TDes8 *)a1)->Copy((TUint8 *)&iConfig,len);
		break;
	case RBusDevComm::EControlSetConfig:
		if (AreAnyPending())
//			iThread->Panic(_L("D32COMM"),ESetConfigWhileRequestPending);
			Kern::PanicCurrentThread(_L("D32COMM"), ESetConfigWhileRequestPending);
		else
			{
			Mem::FillZ(&c,sizeof(c));
			len=Min(((TDesC8 *)a1)->Length(),sizeof(c));
			Mem::Copy(&c,((TDesC8 *)a1)->Ptr(),len);
			if(c.iTerminatorCount>KConfigMaxTerminators)
				{
				r=KErrNotSupported;
				break;
				}
			if (c.iRate==EBpsAutobaud)
				{
				r=KErrNotSupported;
				break;
				}
			if ((r=iDriver->Validate(c))!=KErrNone)
				break;
			if (IsLineFail(c.iHandshake))
				{
				r=KErrCommsLineFail;
				break;
				}
			if (c.iSIREnable==ESIREnable)
				{
				c.iHandshake=0;
				c.iStopBits=EStop1;
				c.iDataBits=EData8;
				}
			if (iConfig.iRate != c.iRate
				|| iConfig.iDataBits != c.iDataBits
				|| iConfig.iStopBits != c.iStopBits
				|| iConfig.iParity != c.iParity
				|| iConfig.iFifo != c.iFifo
				|| iConfig.iSpecialRate != c.iSpecialRate
				|| iConfig.iSIREnable != c.iSIREnable
				|| iConfig.iSIRSettings != c.iSIRSettings)
				{
				restart = ETrue;
				}
			else if (iConfig.iParityErrorChar != c.iParityErrorChar
				|| iConfig.iParityError != c.iParityError
				|| iConfig.iXonChar != c.iXonChar
				|| iConfig.iXoffChar != c.iXoffChar
				|| (iConfig.iHandshake&(KConfigObeyXoff|KConfigSendXoff))
					!= (c.iHandshake&(KConfigObeyXoff|KConfigSendXoff)))
				{
				purge = ETrue;
				}
			else
				{
				if (iConfig.iTerminatorCount==c.iTerminatorCount)
					{
					for (TInt i=0; i<iConfig.iTerminatorCount; i++)
						{
						if (iConfig.iTerminator[i]!=c.iTerminator[i])
							{
							rescan=ETrue;
							break;
							}
						}
					}
				else
					rescan=ETrue;
				
				if (!rescan && c.iHandshake == iConfig.iHandshake)
					break;	// nothing to do.
				}
			if (iStatus==EActive && (restart || purge))
				{
				if (!(iConfig.iHandshake&KConfigFreeRTS))
					iDriver->SetSignals(0,KSignalRTS); // Drop RTS
				Stop(EStopNormal);
				if(purge)
					ResetBuffers();
				iConfig=c;
				Start();
				if (!(iConfig.iHandshake&KConfigFreeRTS))
					iDriver->SetSignals(KSignalRTS,0); // Assert RTS
				iDriver->DisableInterrupts();
				}
			else
				{
				iDriver->DisableInterrupts();
				if(purge)
					ResetBuffers();
				iConfig=c;
				}				
			if(!(iConfig.iHandshake&KConfigObeyXoff))
				iOutputHeld&=~KXoffSignal;
			if(iConfig.iHandshake&KConfigSendXoff)
				{
				iInputHeld=iConfig.iXonChar;
				iHighWaterMark=iHalfFullMark-5;
				}
			else
				{
				iInputHeld=KInputHeldFree;
				iHighWaterMark=iRxBuf->Length()-iLowWaterMark; // 75%
				}
			if(iConfig.iHandshake&KConfigObeyCTS || iConfig.iHandshake&KConfigObeyDSR)
				iInputHeld=KInputHeld;
			iDriver->EnableInterrupts();

			if (rescan)
				iRxBuf->RescanTerminators(iDriver, iConfig.iTerminatorCount, iConfig.iTerminator);
			}
		break;
	case RBusDevComm::EControlCaps:
		iDriver->Caps(*((TDes8 *)a1));
		break;
	case RBusDevComm::EControlSignals:
		r=iDriver->Signals();
		break;
	case RBusDevComm::EControlSetSignals:
		if (((TUint)a1)&((TUint)a2))
//			iThread->Panic(_L("D32COMM"),ESetSignalsSetAndClear);
			Kern::PanicCurrentThread(_L("D32COMM"), ESetSignalsSetAndClear);
		else
			{
			TUint set=(TUint)a1;
			if (iStatus==ENotActive)
				{
				Start();
				if (!(iConfig.iHandshake & KConfigFreeDTR) && !((TUint)a2 & KSignalDTR))
					set|=KSignalDTR; // Assert DTR
				if (!(iConfig.iHandshake & KConfigFreeRTS) && !((TUint)a2 & KSignalRTS))
					set|=KSignalRTS; // Assert RTS
				if (iConfig.iHandshake & KConfigSendXoff)
					iInputHeld = iConfig.iXonChar;
				else
					iInputHeld = KInputHeldFree;
				}
			iDriver->SetSignals(set,(TUint)a2);
			iSignals=(iSignals|set)&(~(TUint)a2);
			}
		break;
	case RBusDevComm::EControlQueryReceiveBuffer:
		r=iRxBuf->Count();
		break;
	case RBusDevComm::EControlResetBuffers:
		if (AreAnyPending())
//			iThread->Panic(_L("D32COMM"),EResetBuffers);
			Kern::PanicCurrentThread(_L("D32COMM"), EResetBuffers);
		else
			ResetBuffers();
		break;
	case RBusDevComm::EControlReceiveBufferLength:
		r=iRxBuf->Length();
		break;

	// ***************************************
	// This one runs in the supervisor context
	// ***************************************
	case RBusDevComm::EControlSetReceiveBufferLength:
		if (AreAnyPending())
			iThread->Panic(_L("D32COMM"),ESetReceiveBufferLength);
		else
			r=SetRxBufferSize((TInt)a1);
		break;
	// ***************************************

	case KChannelControlLateOpen:
		r=iDriver->CompleteSlowOpen((DThread*)a1,(TRequestStatus*)a2);
		break;
#ifdef _DEBUG_DEVCOMM
	case RBusDevComm::EControlDebugInfo:
		{
		TCommDebugInfo info;
		iDriver->DisableInterrupts();
		info.iRxBusy = iIsReading;
		info.iRxHeld = iOutputHeld!=0;
		info.iRxLength = iReceiveLength;
		info.iRxOffset = iReceiveOffset;
		info.iRxIntCount = iDriver->iRxIntCount;
		info.iRxErrCount = iDriver->iRxErrCount;
		info.iRxBufCount = iRxBuf->Count();
		info.iTxBusy = iIsWriting;
		info.iTxHeld = iInputHeld!=KInputHeldFree;
		info.iTxLength = iTransmitLength;
		info.iTxOffset = iTransmitOffset;
		info.iTxIntCount = iDriver->iTxIntCount;
		info.iTxErrCount = iDriver->iTxErrCount;
		info.iTxBufCount = iTxBuf->Count();
		info.iDrainingRxBuf = iDrainingRxBuf!=0;
		info.iFillingTxBuf = iFillingTxBuf!=0;
		info.iRunningDfc = iRunningDfc!=0;
		info.iDfcCount = iDfcCount;
		info.iDfcReqSeq = iDfcReqSeq;
		info.iDfcHandlerSeq = iDfcHandlerSeq;
		info.iRxXon = iRxXon;
		info.iRxXoff = iRxXoff;
		info.iTxXon = iTxXon;
		info.iTxXoff = iTxXoff;
		info.iRxChars = iRxChars;
		info.iTxChars = iTxChars;
		info.iDoDrainSeq = iDoDrainSeq;
		info.iTxDfcPend = (iDfcRequest&EFillTxBufferRequired)!=0;
		info.iRxDfcPend = (iDfcRequest&EDrainRxBufferRequired)!=0;
		iDriver->EnableInterrupts();
		((TDes8 *)a1)->FillZ(((TDes8 *)a1)->MaxLength());
		len=Min(((TDes8 *)a1)->MaxLength(),sizeof(TCommDebugInfo));
		((TDes8 *)a1)->Copy((TUint8 *)&info,len);
		}
#endif
	default:
		r=KErrNotSupported;
		}
	return(r);
	}

void DChannelComm::QueueDfc(TUint aRequestMask)
//
// Set the flag to drain RX Buffer and queue the DFC if required.
//
	{
    if (aRequestMask!=ECompleteEmergency)
        iDriver->DisableInterrupts();

	iDfcRequest |= aRequestMask;

	if (!iRunningDfc)
		{
#ifdef _DEBUG_DEVCOMM
		if (aRequestMask & EDrainRxBufferRequired)
			{
			iDfcReqSeq = iRxBuf->iRemSeqNum.Int();
			++iDfcCount;
			}
#endif
		iRunningDfc=ETrue;
		Kern::Add(iDfc);
		}

    if (aRequestMask!=ECompleteEmergency)
	    iDriver->EnableInterrupts();
	}

void DChannelComm::ResetDevice()
//
// Reset driver variables following device power down.
//
    {

	iIsClientWriting=EFalse;
//
// Must leave iDfcRunning set as the Dfc may remain pending
// This is no problem as the DfcHandler is happy if it finds
// it has nothing to do
//
	iIsReading=iIsWriting=iDrainingRxBuf=iFillingTxBuf=EFalse;
	iFlags=iDfcRequest=0;

	iStatus=ENotActive;
	}

void DChannelComm::DoPowerDown()
//
// Called at switch off and on Closing.
//
    {

	if (iStatus==EActive)
		Stop(EStopPwrDown);
	iPowerHandler->SetRequirement(0);
	}

void DChannelComm::DoEmergencyPowerDown()
//
// Called when the batteries run out or fall out.
//
    {

	if (iStatus==EActive)
		Stop(EStopEmergency);
	QueueDfc(ECompleteEmergency);
    }

GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
//
// DLL entry point
//
	{

	return KErrNone;
	}
