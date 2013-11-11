// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Dummy CSY for testing.
// 
//

/**
 @file
*/

#include "CS_PORT.H"
#include <d32comm.h>
#include "C32COMM.H"
#include <f32file.h>
#include "dummy.h"
#include "TTHREAD.H"	// Required for the "Start Server in CSY Test"


//**********************************************************************
//			            DUMMY PORT FACTORY CODE
//**********************************************************************

static void CleanupCObject(TAny* aCObject)
/**
 * Clean up a CObject
 */
    {
    ((CObject*)aCObject)->Close();
    }


CPort* CDummyPortFactory::NewPortL(const TUint aUnit)
/**
 * Create a new port for the supplied unit number
 */
	{
	return CDummyPort::NewL(aUnit);
	}


void CDummyPortFactory::Info(TSerialInfo& aSerialInfo)
/**
 * fill in the supplied structure.
 */
	{
	const TUint KCommLowUnit=0;
	const TUint KCommHighUnit=0;
	aSerialInfo.iDescription=SERIAL_DESCRIPTION;
#if defined(DUMMYCSY16)
	aSerialInfo.iName=SERIAL_NAME_SMALL;
#else
	aSerialInfo.iName=SERIAL_NAME;
#endif
	aSerialInfo.iLowUnit=KCommLowUnit;
	aSerialInfo.iHighUnit=KCommHighUnit;
#ifdef DUMMYCSY10
	LoadAnotherCSY();// Modified to handle stackable csy ie. loading csy by other csy
#endif
#ifdef DUMMYCSY17
	LoadAnotherCSYNoClose();// Modified to handle stackable csy ie. loading csy by other csy. Session is not closed after unload csy
#endif
	}

#ifdef DUMMYCSY10 // Load one csy by another csy. session is closed after unload csy
void CDummyPortFactory::LoadAnotherCSY()
	{
	_LIT(KCSYName,"DUMMY");
	RCommServ commSess;
	TInt res = commSess.Connect();
	if(res == KErrNone)
		{
		res = commSess.LoadCommModule(KCSYName);
		if(res == KErrNone)
			{
			commSess.UnloadCommModule(KCSYName);
			commSess.Close();
			}
		}

	}
#endif

#ifdef DUMMYCSY17 // Load one csy by another csy. session is not closed after unload csy
void CDummyPortFactory::LoadAnotherCSYNoClose()
	{
	_LIT(KCSYName,"DUMMY");
	RCommServ commSess;
	TInt res = commSess.Connect();
	if(res == KErrNone)
		{
		res = commSess.LoadCommModule(KCSYName);
		if(res == KErrNone)
			{
			commSess.UnloadCommModule(KCSYName);
			//Session close statement should be commented to test the whether the function
			//CCommSession::CC32SessionCloseTimer::RunL() is called
			//server thread should panic since the session is not closed

			//commSess.Close();
			}
		}

	}
#endif


void CDummyPortFactory::ConstructL()
/**
 * Construct and init members
 */
	{
	TName name(SERIAL_NAME);
	SetNameL(&name);
	}


CDummyPortFactory* CDummyPortFactory::NewL()
/**
 * 2 phase constructor
 */
	{
	CDummyPortFactory* c=new(ELeave) CDummyPortFactory;
	CleanupStack::PushL(TCleanupItem(CleanupCObject, c));
	c->ConstructL();
	CleanupStack::Pop();
	return c;
	}


CDummyPortFactory::CDummyPortFactory()
/**
 * C'tor
 */
	{
	__DECLARE_NAME(_S("CDummyPortFactory"));
	iVersion=TVersion(KMajorVersion,KMinorVersion,KBuild);
	}

/**
Returns capabilities for requested port
*/
TSecurityPolicy CDummyPortFactory::PortPlatSecCapability(TUint aPort) const
	{
	TSecurityPolicy csySecurityPolicy;

	switch(aPort)
		{
		case PORT_0_NOCAPS:
		case PORT_6_NOCAPS:
		csySecurityPolicy = TSecurityPolicy(TSecurityPolicy::EAlwaysPass);
		break;
		case PORT_1_NETWORKSERVICES:
		csySecurityPolicy = TSecurityPolicy(ECapabilityNetworkServices);
		break;
		case PORT_2_NOTSUPPORT:
		default:
		csySecurityPolicy = TSecurityPolicy(TSecurityPolicy::EAlwaysFail);
		break;
		case PORT_3_ALLCAPS:
		csySecurityPolicy = TSecurityPolicy(ECapabilityTCB);
		break;
		}
	return csySecurityPolicy;
	}

//**********************************************************************
//			            DUMMY PORT CODE
//**********************************************************************


CDummyPort::CDummyPort()
/**
 * C'tor
 */
	:iPtrDataIn(NULL,0)
	,iPtrDataOut(NULL,0)
	,iPtrCompData(NULL,0)
	{
	__DECLARE_NAME(_S("CDummyPort"));
	iDummyCompleteDelayFast = KDummyCompleteDelayFast;
    iDummyCompleteDelaySlow = KDummyCompleteDelaySlow;
	iConfig.iRate=EBps9600;
	iConfig.iDataBits=EData8;
	iConfig.iStopBits=EStop1;
	iConfig.iParity=EParityNone;
	iConfig.iFifo=EFifoEnable;
	iConfig.iHandshake=KConfigObeyCTS;
	iConfig.iParityError=KConfigParityErrorIgnore;
	iConfig.iSIREnable=ESIRDisable;
	iConfig.iTerminatorCount=0;
	iConfig.iXonChar=0x11; // XON
	iConfig.iXoffChar=0x13; // XOFF
	iConfig.iSpecialRate=0;
	iConfig.iParityErrorChar=0;

	iPeerConfig.iRate = EBpsSpecial;
	iPeerConfig.iDataBits = EData8;
	iPeerConfig.iStopBits = EStop1;
	iPeerConfig.iParity = EParityNone;
	iPeerConfig.iHandshake = KConfigObeyCTS;
	}


CDummyPort* CDummyPort::NewL(TUint aUnit)
/**
 * Make a new CPort for the comm server
 */
	{
	CDummyPort *p=new(ELeave) CDummyPort;
	CleanupStack::PushL(TCleanupItem(CleanupCObject, p));
	p->ConstructL(aUnit);

    CleanupStack::Pop();
	return p;
	}


void CDummyPort::ConstructL(TUint aUnit)
/**
 * construct and init members
 */
	{
    // To avoid thread lock we cannot actually connect to the socket server now if
    // the socket server thread is calling us
    // Similarly we cannot load IRDA synchronously because this will cause a
    // different kind of thread lock.

    // Should really have some sundry checks
    // i.e. is irda.prt in the ESOCK.INI file
    // is there enough heap
    // will irda be able to access COMM::0
    // Basically we need to be pretty sure IRDA will start but without
    // actually starting it

	/*
	RDevComm comm;
	TInt ret=comm.Open(0);
	if (ret==KErrInUse)
		User::Leave(KErrAccessDenied);
	else
		User::LeaveIfError(ret);
	comm.Close();
	*/
	const TUint16 KDummyPacketMaxSize = 2048;
	iRole=ECommRoleDTE;
	iNotifySignalMask = 0;
	iFlowControl=EFlowControlOff;
	iRole=ECommRoleDTE;
	iSignals = 0;
	iCapsPtr = 0;

	iCompleteSigPtr		= new(ELeave) CDummyCompleter(*this);
	iCompleteFlowPtr	= new(ELeave) CDummyCompleter(*this);
	iCompleteConfPtr	= new(ELeave) CDummyCompleter(*this);
	iCompleteCapsPtr	= new(ELeave) CDummyCompleter(*this);
	iCompleteReadPtr	= new(ELeave) CDummyCompleter(*this);
	iCompleteWritePtr	= new(ELeave) CDummyCompleter(*this);
	iCompleteBreakPtr	= new(ELeave) CDummyCompleter(*this);
	iCompleteSetBreakPtr = new(ELeave) CDummyCompleter(*this);
	iCompleteDataAvailablePtr	= new(ELeave) CDummyCompleter(*this);
	iCompleteOutputEmptyPtr	= new(ELeave) CDummyCompleter(*this);
	iCompleteClosePtr	= new(ELeave) CDummyCompleter(*this);

	iBreakDelay = 1;

	iDataIn = (TUint8*)User::Alloc(KDummyPacketMaxSize);
        if(!iDataIn)
            User::Leave(KErrNoMemory);
	iPtrDataIn.Set(iDataIn, 0, KDummyPacketMaxSize);

	iDataOut = (TUint8*)User::Alloc(KDummyPacketMaxSize);
        if(!iDataOut)
            User::Leave(KErrNoMemory);
	iPtrDataOut.Set(iDataOut, 0, KDummyPacketMaxSize);

	iCompData = (TUint8*)User::Alloc(KDummyPacketMaxSize);
        if(!iCompData)
            User::Leave(KErrNoMemory);
	iPtrCompData.Set(iCompData, 0, KDummyPacketMaxSize);

	TBuf8<64> testbufRead	=_L8("0123456789abcdefghijklmnopqrstuvxyxABCDEFGHIJKLMNOPQRSTUVXYZ[]\r\n");
	TBuf8<100> testbufWrite	=_L8("This string has to be exactly like this othervice write returns KErrCommsLineFail.");
	//ptrDataIn.Copy(testbufRead); /* Client Read */
	TInt i;
	for (i=0;i<KDummyPacketMaxSize/64;i++)
		iPtrDataIn.Append(testbufRead);

	iPtrCompData.Copy(testbufWrite); /* Client Read */
	TName name;
	name.Format(_L("%d"), aUnit);
	SetNameL(&name);

#if defined (_DEBUG_CONSOLE_)
	name.Format(_L("IrCOMM::%d"),aUnit);
	DEBUG_TRACE(iConsole.SetTitle(name);)
    DEBUG_TRACE(iConsole.Control(_L("+S +M"));)
#endif

	}


CDummyPort::~CDummyPort()
/**
 * D'tor called from the active shutterdowner
 */
	{
	//User::After(iDummyCompleteDelaySlow);
	delete iCompleteSigPtr;
	delete iCompleteFlowPtr;
	delete iCompleteConfPtr;
	delete iCompleteCapsPtr;
	delete iCompleteReadPtr;
	delete iCompleteWritePtr;
	delete iCompleteBreakPtr;
	delete iCompleteSetBreakPtr;
	delete iCompleteDataAvailablePtr;
	delete iCompleteOutputEmptyPtr;
	delete iCompleteClosePtr;

	User::Free(iDataIn);
	User::Free(iCompData);
	User::Free(iDataOut);

#if defined (_DEBUG_CONSOLE_)
	DEBUG_TRACE(iConsole.Printf(_L("PortClosed\n\r"));)
    DEBUG_TRACE(iConsole.Close();)
#endif
	}


void CDummyPort::NotifySignalChange(TUint aSignalMask)
/**
 * Stores the pointer which is also used as a flag to indicate
 * that we must notify C32 of signal changes
 */
	{
	iNotifySignalMask = aSignalMask * KSignalChanged;
	if (iRole==ECommRoleDTE)
		{
		iSignals = (KSignalDTEInputs|KCTSChanged);
		}
	else
		{
		iSignals = (KSignalDCEInputs|KRTSChanged);
		}

	if (iNotifySignalMask&iSignals)
		{
		iCompleteSigPtr->Complete(ECommNotifySignals,iDummyCompleteDelayFast,KErrNone,iSignals);
		}
	}


void CDummyPort::NotifySignalChangeCancel()
/**
 * Clears the pointer which is also used as a flag to indicate
 * that we must not notify C32 of signal changes
 */
	{
	iCompleteSigPtr->CancelComplete();
	}


void CDummyPort::NotifyConfigChange()
/**
 * Stores the pointer which is also used as a flag to indicate
 * that we must notify C32 of configuration changes
 */
	{
	iPeerConfig.iChangedMembers=KHandshakeChanged;
	iPeerConfig.iHandshake = KConfigObeyCTS;
	iPeerConfig.iRate =EBps9600;
	iPeerConfig.iDataBits =EData8;
	iPeerConfig.iParity =EParityNone;
	iPeerConfig.iStopBits =EStop1;
	iPeerConfig.iHandshake =(KConfigObeyCTS|KConfigObeyRTS);

	TPckg<TCommNotificationV01> cfg(iPeerConfig);
	iCompleteConfPtr->Complete(ECommNotifyConfigChange,iDummyCompleteDelayFast,KErrNone,cfg);

	}


void CDummyPort::NotifyConfigChangeCancel()
/**
 * Clears the pointer which is also used as a flag to indicate
 * that we must not notify C32 of signal changes
 */
	{
	iCompleteConfPtr->CancelComplete();
	}


void CDummyPort::NotifyFlowControlChange()
/**
 * Stores the pointer which is also used as a flag to indicate
 * that we must notify C32 of flow control changes
 */
	{
	iFlowControl=EFlowControlOn;
	iCompleteFlowPtr->Complete(ECommNotifyFlowControl,iDummyCompleteDelayFast,KErrNone,iFlowControl);
	}


void CDummyPort::NotifyFlowControlChangeCancel()
/**
 * Clears the pointer which is also used as a flag to indicate
 * that we must not notify C32 of flow control changes
 */
	{
	iCompleteFlowPtr->CancelComplete();
	}


void CDummyPort::NotifyBreak()
/**
 * Set the flag on that we must notify C32 of break signal changes
 */
	{
	iCompleteBreakPtr->Complete(ECommNotifyBreak,iDummyCompleteDelayFast,KErrNone);
	}


void CDummyPort::NotifyBreakCancel()
/**
 * Set the flag on that we must not notify C32 of break signal changes
 */
	{
	iCompleteBreakPtr->CancelComplete();
	}


TInt CDummyPort::GetFlowControlStatus(TFlowControl& aFlowControl)
/**
 * Returns the current flow control state
 */
	{
	aFlowControl=EFlowControlOn;
	return KErrNone;
	}


TInt CDummyPort::GetRole(TCommRole& aRole)
/**
 * Returns the current role
 */
	{
	aRole = iRole;
	return KErrNone;
	}


TInt CDummyPort::SetRole(TCommRole aRole)
/**
 * Sets the role as DCE or DTE, and also the handshake according the
 * role set
 */
	{

	switch(aRole)
		{
		case ECommRoleDCE:
			iConfig.iHandshake &=~KConfigObeyCTS;
			iConfig.iHandshake |=KConfigObeyRTS;
			iRole = aRole;
			break;
		case ECommRoleDTE:
			iConfig.iHandshake &=~KConfigObeyRTS;
			iConfig.iHandshake |=KConfigObeyCTS;
			iRole = aRole;
			break;

		default:
			break;
		}
	return KErrNone;
	}


void CPort::FreeMemory()
/**
 * We do nothing
 */
	{
	// we do nothing
	}


void CDummyPort::StartRead(const TAny *aClientBuffer,TInt aLength)
/**
 * Queue a read.  IPC is handled inside of C32 now.
 */
	{

	//_LIT8(KTestLog,"KCSYLoggingName start reading....");
	//__FLOG_STATIC0(KC32LoggingName, KCSYLoggingName, KTestLog);

	#ifdef DUMMYCSY12

		User::After(KDummy12ReadWait); // Added to block other csys

	#endif

	if(aLength==0)
		{
        iCompleteReadPtr->Complete(ECommRead,iDummyCompleteDelaySlow,KErrNone);
		return;
		}
    else
		iClientPtr=(TAny*)aClientBuffer;

    if(aLength<0)
        {
        aLength = -aLength;
        }

    iClientLength=aLength;
	iPtrDataIn.SetLength(aLength);
	this->IPCWrite(aClientBuffer,iPtrDataIn,0);
	iCompleteReadPtr->Complete(ECommRead,iDummyCompleteDelayFast,KErrNone);
	//_LIT8(KTestLog1,"KCSYLoggingName end reading....");
	//__FLOG_STATIC0(KC32LoggingName, KCSYLoggingName, KTestLog1);

    }


void CDummyPort::ReadCancel()
/**
 * cancel a pending read
 */
	{
	iCompleteReadPtr->CancelComplete();
    }


TInt CDummyPort::QueryReceiveBuffer(TInt& aLength) const
/**
 * Get the size of the receive buffer from the real serial port
 */
	{
    aLength=iLength;
	return KErrNone;
	}


void CDummyPort::ResetBuffers(TUint /*aFlags*/)
/**
 * Flush the client buffers
 */
	{
	iLength=0;
	}


void CDummyPort::StartWrite(const TAny* aClientBuffer, TInt aLength)
/**
 * Queue a write.
 */
	{
//	LOG(Log::Printf(_L("	**CDummyPort::StartWrite** :\r\n")));

	TInt offset = 0;
    TInt result=this->IPCRead(aClientBuffer, iPtrDataOut,offset);
	iPtrDataOut.SetLength(aLength);
    if(result==KErrNone)
        {
		result = iPtrCompData.Match(iPtrDataOut);
		if (result==0)
			iCompleteWritePtr->Complete(ECommWrite,iDummyCompleteDelayFast,KErrNone);
		else
			iCompleteWritePtr->Complete(ECommWrite,iDummyCompleteDelayFast,KErrCommsLineFail);
		}
    else
		iCompleteWritePtr->CancelComplete();

	}


void CDummyPort::WriteCancel()
/**
 * cancel a pending write
 */
	{
    iCompleteWritePtr->CancelComplete();
	}


void CDummyPort::Break(TInt aTime)
/**
 * Queue a break
 */
	{
	if (iBreakDelay!=aTime)
		User::Panic(_L("Dummy"),E_ParametersCorrupted);
	else
		iCompleteSetBreakPtr->Complete(ECommBreak,aTime,KErrNone);
	iBreakDelay*=100;
	}


void CDummyPort::BreakCancel()
/**
 * Cancel a pending break
 */
	{
    iCompleteSetBreakPtr->CancelComplete();
	}


TInt CDummyPort::SetConfig(const TDesC8& aPackage)
/**
 * Set up the Comm LDD/PDD
 */
	{
    // line characteristics should be set and the line states checked accordingly
    TCommConfigV01 c;

    Mem::FillZ(&c,sizeof(c));
	TInt len=Min(aPackage.Length(),sizeof(c));
	Mem::Copy(&c, aPackage.Ptr(),len);

	if ((c.iStopBits!=EStop2))
		User::Panic(_L("Dummy"),E_ParametersCorrupted); // Panic if parameter fails

	if(c.iTerminatorCount>KConfigMaxTerminators)
		{
		return KErrNotSupported;
		}
	if (c.iSIREnable==ESIREnable)
		{
		return KErrNotSupported;
		}

	iConfig=c;

//
// Piggy back the "Start Server from CSY" test onto this request
//

	_LIT(KLibraryName,"TThread");
	_LIT(KThreadName,"newserver");
	const TInt kDefaultStackSize=0x2000;
	const TInt kHeapSize=0x1000;
	const TInt kMaxHeapSize=0x40000;

		TInt res=RCommServ::CreateThreadInCommProc(KLibraryName,KThreadName,ServerThread,
				kDefaultStackSize,kHeapSize,kMaxHeapSize);
		 return res;

	}


TInt CDummyPort::GetConfig(TDes8& aPackage) const
/**
 * Pass a config request
 */
	{
    // simple - copy back iConfig
    TPckg<TCommConfigV01> cfg(iConfig);
    aPackage.Copy(cfg);
	return KErrNone;
	}


TInt CDummyPort::GetCaps(TDes8& aPackage)
/**
 * Read capabilities from the driver
 */
	{
	TCommCapsV01 *c;
	TCommCapsV02 *c2=0;

	c = (TCommCapsV01*)aPackage.Ptr();

	if (aPackage.Size()==sizeof(TCommCapsV02))
		{
		c2 = (TCommCapsV02*)aPackage.Ptr();
	    aPackage.SetLength(sizeof(TCommCapsV02));
		}
	else
		{
		aPackage.SetLength(sizeof(TCommCapsV01));
		}

	c->iRate=
    KCapsBps50 | KCapsBps75 | KCapsBps110 | KCapsBps134 | KCapsBps150 | KCapsBps300
    | KCapsBps600 | KCapsBps1200 | KCapsBps1800 | KCapsBps2000 | KCapsBps2400 | KCapsBps3600
    | KCapsBps4800 | KCapsBps7200 | KCapsBps9600 | KCapsBps19200 | KCapsBps38400
    | KCapsBps57600 | KCapsBps115200 ;

	c->iDataBits=0xf; // All data sizes
	c->iStopBits=0x3; // 1 and 2 stop bits
	c->iParity=0x7; // None, Even and Odd
	c->iHandshake=0x1ff; // All handshakes
	c->iSignals=0x3f; // All signals
	c->iFifo=KCapsHasFifo;
	c->iSIR=0;

	// Version 02 extended capabilities
	if (c2!=0)
		{
		c2->iNotificationCaps=
			(KNotifySignalsChangeSupported		| KNotifyRateChangeSupported
			| KNotifyDataFormatChangeSupported	| KNotifyHandshakeChangeSupported
			| KNotifyBreakSupported				| KNotifyFlowControlChangeSupported);
		c2->iRoleCaps=KCapsRoleSwitchSupported;
		c2->iFlowControlCaps=KCapsFlowControlStatusSupported;
		}

	return KErrNone;
	}


TInt CDummyPort::SetServerConfig(const TDesC8& aDes)
/**
 *	set server configurations
 */
	{
	TCommServerConfigV01 *c;
	c = (TCommServerConfigV01*)aDes.Ptr();
	if (aDes.Size()!=sizeof(TCommServerConfigV01))
		return KErrGeneral;

	if (!((c->iBufFlags&KCommBufferPartial) && (c->iBufSize&0x10)))
		User::Panic(_L("Dummy"),E_ParametersCorrupted);

	iBufFlags=c->iBufFlags;
	iBufSize=c->iBufSize;
	return KErrNone;
	}


TInt CDummyPort::GetServerConfig(TDes8& aDes)
/**
 *	get server configurations
 */
	{

	TCommServerConfigV01 *c;
	c = (TCommServerConfigV01*)aDes.Ptr();
	if (aDes.Size()!=sizeof(TCommServerConfigV01))
		return KErrGeneral;

	c->iBufFlags = iBufFlags;
	c->iBufSize = iBufSize;

	return KErrNone;
	}


TInt CDummyPort::GetSignals(TUint& aSignals)
/**
 * get the status of the signal pins
 */
	{
	if (iRole==ECommRoleDTE)
		aSignals = (KSignalDTEInputs);
	else
		aSignals = (KSignalDCEInputs);


	return KErrNone;
	}


TInt CDummyPort::SetSignalsToMark(TUint aSignals)
/**
 * set selected signals to high (logical 1)
 *
 * @param aSignals bitmask with the signals to be set
 * @return KErrNone always
 */
	{

	if (iRole==ECommRoleDTE)
		{
		if (aSignals!=KSignalDTEOutputs)
			User::Panic(_L("Dummy"),E_ParametersCorrupted);
		aSignals &= KSignalDTEOutputs;
		}
	else
		{
		if (aSignals!=KSignalDCEOutputs)
			User::Panic(_L("Dummy"),E_ParametersCorrupted);
		aSignals &= KSignalDCEOutputs;
		}

    if(aSignals)
        {
        iSignals |= aSignals;
		}
	return KErrNone;
	}


TInt CDummyPort::SetSignalsToSpace(TUint aSignals)
/**
 * set selected signals to low (logical 0)
 *
 * @param aSignals bitmask with the signals to be cleared
 * @return KErrNone always
 */
	{


	if (iRole==ECommRoleDTE)
		{
		if (aSignals!=KSignalDTEOutputs)
			User::Panic(_L("Dummy"),E_ParametersCorrupted);
	    aSignals &= KSignalDTEOutputs;
		}
	else
		{
		if (aSignals!=KSignalDCEOutputs)
			User::Panic(_L("Dummy"),E_ParametersCorrupted);
		aSignals &= KSignalDCEOutputs;
		}

    if(aSignals)
        {
        iSignals &= ~aSignals;
        }
	return KErrNone;
	}


TInt CDummyPort::GetReceiveBufferLength(TInt& aLength) const
/**
 * get size of Tx and Rx buffer
 *
 * @param aLength reference to where the length will be written to
 * @return KErrNone always
 */
	{

    aLength = iLength;
	return KErrNone;
	}


TInt CDummyPort::SetReceiveBufferLength(TInt aLength)
/**
 * set the size of Tx and Rx buffer
 *
 * @param aLength new length of Tx and Rx buffer
 * @return TInt error code
 */
	{
	iLength	= aLength;
    return KErrNone;
	}


void CDummyPort::NotifyDataAvailable()
/**
 * Set the flag on that we must notify C32 when there has appeared data into
 * incoming buffer
 */
	{
	iCompleteDataAvailablePtr->Complete(ECommNotifyDataAvailable,iDummyCompleteDelaySlow,KErrNone);
	}


void CDummyPort::NotifyDataAvailableCancel()
/**
 * Set the flag on that we must not notify C32 when there has appeared data
 * into incoming buffer
 */
	{
	iCompleteDataAvailablePtr->CancelComplete();
	}


void CDummyPort::NotifyOutputEmpty()
/**
 * Set the flag on that we must not notify C32 when outgoing buffer is empty
 */
	{
	iCompleteOutputEmptyPtr->Complete(ECommNotifyOutputEmpty,iDummyCompleteDelaySlow,KErrNone);
	}


void CDummyPort::NotifyOutputEmptyCancel()
/**
 * Set the flag on that we must not notify C32 when outgoing buffer is empty
 */
	{
	iCompleteOutputEmptyPtr->CancelComplete();
	}


void CDummyPort::Destruct()
/**
 * Called from Port manager to close us down - we must call delete this sometime
 * soon (unless AcessCount goes positive again)
 */
	{
	if(iCompleteClosePtr)
		{
		if (!iCompleteClosePtr->IsActive()) // Don't want two completes at once
			iCompleteClosePtr->Complete(ECommClose,iDummyCompleteDelaySlow,KErrNone);

		}
	}



//**********************************************************************
//			            DUMMY DLL ENTRY POINT CODE
//**********************************************************************


extern "C"
	{
	IMPORT_C CSerial* LibEntry(); // Force export
	}


EXPORT_C CSerial * LibEntry()
/**
 * Lib main entry point
 */
	{
#ifdef DUMMYCSY15
	return NULL;   // returning NULL ptr here so that load fails
#else
	CDummyPortFactory *c=NULL;
	TRAPD(ret,c=CDummyPortFactory::NewL());
	if (ret!=KErrNone)
		{
		return NULL;
		}
	return c;
#endif
	}


//
// implementation of CDummyCompleter
//


CDummyCompleter::CDummyCompleter(CDummyPort& aPort)
/**
 * C'tor
 */
	:CActive(EPriorityStandard)
	,iPort(aPort)
	,iReason(KErrNone)
	{
	CActiveScheduler::Add(this);
	}


void CDummyCompleter::Complete(TCommMess aMess,TTimeIntervalMicroSeconds32 anInterval, TInt aReason, TUint aValue)
/**
 * complete a request
 */
	{
	iReason = aReason;
	iMess = aMess;
	iValue = aValue;
	TInt ret=iTimer.CreateLocal();
	if (ret!=KErrNone)
		User::Panic(_L("Dummy"),E_ParametersCorrupted);
	iTimer.After(iStatus,anInterval); // 2 seconds timeout
	SetActive();
	}


void CDummyCompleter::Complete(TCommMess aMess,TTimeIntervalMicroSeconds32 anInterval, TInt aReason, const TDesC8& aDescriptor)
/**
 * complete a request
 */
	{
	iReason = aReason;
	iMess = aMess;
	//iConfPtr = (TAny *)&aDescriptor;
	Mem::FillZ(&iPeerConfig,sizeof(iPeerConfig));
	TInt len=Min(aDescriptor.Length(),sizeof(iPeerConfig));
	Mem::Copy(&iPeerConfig, aDescriptor.Ptr(),len);
	TInt ret=iTimer.CreateLocal();
	if (ret!=KErrNone)
		User::Panic(_L("Dummy"),E_ParametersCorrupted);
	iTimer.After(iStatus,anInterval); // 2 seconds timeout
	SetActive();
	}


void CDummyCompleter::CancelComplete()
/**
 * cancel a complete
 */
	{
	Cancel();
	}


CDummyCompleter::~CDummyCompleter()
/**
 * D'tor
 */
	{
	CActive::Cancel();
	}


void CDummyCompleter::RunL()
/**
 * a completer request has completed
 */
	{
	TInt ret = iStatus.Int();

	if (ret!=KErrNone)
		{
		return;
		}

	switch(iMess)
		{
		case ECommNotifySignals:

			iPort.SignalChangeCompleted(iValue,iReason);
			break;
		case ECommNotifyFlowControl:
			iPort.FlowControlChangeCompleted((TFlowControl)iValue,iReason);
			break;
		case ECommNotifyConfigChange:
			{
			TPckg<TCommNotificationV01> cfg(iPeerConfig);
			iPort.ConfigChangeCompleted(cfg,iReason);
			}
			break;
		case ECommNotifyBreak:
			iPort.BreakNotifyCompleted(iReason);
			break;
		case ECommBreak:
			iPort.BreakCompleted(iReason);
			break;
		case ECommRead:
			iPort.ReadCompleted(iReason);
			break;
		case ECommWrite:
			iPort.WriteCompleted(iReason);
			break;
		case ECommNotifyDataAvailable:
			iPort.NotifyDataAvailableCompleted(iReason);
			break;
		case ECommNotifyOutputEmpty:
			iPort.NotifyOutputEmptyCompleted(iReason);
			break;
		case ECommClose:
			if (iPort.AccessCount()==0)
				delete &iPort; // will delete this also
			break;
		default:
			break;

		}

	}


void CDummyCompleter::DoCancel()
/**
 * called when a cancelling the request
 */
	{
	iTimer.Cancel();
	}

// EOF - DUMMY.CPP

