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
//

#include <bluetooth/logger.h>
#include <cs_port.h>
#include "btcomm.h"
#include "btstate.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BT_COMM);
#endif

//**********************************************************************
//			            BTCOMM CSY ENTRY POINT CODE
//**********************************************************************

extern "C"
	{
	IMPORT_C CSerial* LibEntry();	// Force export 
	}

EXPORT_C CSerial* LibEntry()
/**
	Lib main entry point.
**/
	{
	CBTPortFactory* c=NULL;
	TRAPD(ret,c=CBTPortFactory::NewL());
	if (ret!=KErrNone)
		return NULL;
	return c;
	}



//**********************************************************************
//			            BTCOMM PORT FACTORY CODE
//**********************************************************************

#pragma warning (disable : 4100)  // Unreferenced formal parameter

static void CleanupCObject(TAny* aCObject)
/**
	Clean up a CObject.
	This static callback is invoked upon destruction of factory 
	by c32 to close it down.
**/
    {
	LOG_STATIC_FUNC
    ((CObject*)aCObject)->Close();
    }

CPort* CBTPortFactory::NewPortL(const TUint aUnit)
/**
	CBTPortFactory NewPortL.
	Create a new port for the supplied unit number.
**/
	{
	LOG_FUNC
	__ASSERT_ALWAYS(aUnit<=KCommHighUnit,User::Leave(KErrNotSupported));
	// need to pass the factory instance through to the port.
	CBTPort *port=CBTPort::NewL(aUnit,iPortStateFactory);
	return port;
	}

void CBTPortFactory::Info(TSerialInfo& aSerialInfo)
/**
	Fill in the supplied structure.
**/
	{
	LOG_FUNC
	aSerialInfo.iDescription=SERIAL_DESCRIPTION;
	aSerialInfo.iName=SERIAL_NAME;
	aSerialInfo.iLowUnit=KCommLowUnit;
	aSerialInfo.iHighUnit=KCommHighUnit;
	}

void CBTPortFactory::ConstructL()
/**
	Construct CBTPort Factory.
	Also construct the CBTPortStateFactory here too.
**/
	{
	CONNECT_LOGGER
	LOG_FUNC

	iPortStateFactory=CBTPortStateFactory::NewL();
	TName name(SERIAL_NAME);
	SetNameL(&name);
	}

CBTPortFactory* CBTPortFactory::NewL()
/**
	Create new CBTPortFactory.
	This factory is derived from CSerial.
	Corresponds to the BTCOMM in BTCOMM::0 etc.
**/
	{
	LOG_STATIC_FUNC
	CBTPortFactory* c=new(ELeave) CBTPortFactory;
	CleanupStack::PushL(TCleanupItem(CleanupCObject, c));
	c->ConstructL();
	CleanupStack::Pop();
	return c;
	}

CBTPortFactory::CBTPortFactory()
/**
	CBTPortFactory c'tor.
**/
	{
	LOG_FUNC
//	iVersion=TVersion(KMajorVersion,KMinorVersion,KBuild);
	}

CBTPortFactory::~CBTPortFactory()
/**
	CBTPortFactory destructor.
**/
	{
	LOG_FUNC
	delete iPortStateFactory;

	CLOSE_LOGGER
	}

TSecurityPolicy CBTPortFactory::PortPlatSecCapability (TUint /**aPort*/) const
	//return the security policy for the given port number, aPort.  All the ports
	//are treated the same so always return a security policy defining local services
	{
	LOG_FUNC
	return TSecurityPolicy(ECapabilityLocalServices);	
	}
//**********************************************************************
//			            BTCOMM PORT CODE
//**********************************************************************

CBTPort::CBTPort()
/**
	C'Tor sets-up the TCommconfigV01 and TCommCapsV02 structs of the CSY.
**/
	{
	LOG_FUNC

	iConfig.iRate            = EBps9600;
	iConfig.iDataBits        = EData8;
	iConfig.iStopBits        = EStop1;
	iConfig.iParity          = EParityNone;
	iConfig.iFifo            = EFifoEnable;
	iConfig.iHandshake       = 0;	// KConfigObeyCTS
	iConfig.iParityError     = KConfigParityErrorIgnore;
	iConfig.iSIREnable       = ESIRDisable;
	iConfig.iTerminatorCount = 0;
	iConfig.iXonChar         = 0x11; // XON
	iConfig.iXoffChar        = 0x13; // XOFF
	iConfig.iSpecialRate     = 0;
	iConfig.iParityErrorChar = 0;

	// set CSY TCommCapsV02 capabilities struct
	iCaps.iRate              = KCapsBps9600;
	iCaps.iDataBits          = KCapsData8;
	iCaps.iStopBits          = KCapsStop1;
	iCaps.iParity            = KCapsParityNone;
	iCaps.iHandshake         = 0;
	iCaps.iSignals           = 0;
	iCaps.iFifo              = KCapsHasFifo;
	iCaps.iSIR               = 0;
	iCaps.iNotificationCaps  = 0;		
	iCaps.iRoleCaps          = 0;
	iCaps.iFlowControlCaps   = 0;
	}

CBTPort* CBTPort::NewL(TUint aUnit,CBTPortStateFactory* aFactory)
/**
	CBTPort NewL, make a new CPort for the Comm server.
	This port is derived from CPort. This CBTPort is used by 
	the Comms server to serve all the client RComm requests. 
							
	@param		aUnit		Corresponds to the digit in BTCOMM::0 etc.
							Note that each digit corresponds to a different
							legacy CSY port.  
	@param		aFactory    Is the factory object used to reach each state
*/
	{
	LOG_STATIC_FUNC
	CBTPort* p=new(ELeave) CBTPort;
	CleanupStack::PushL(TCleanupItem(CleanupCObject, p));
	p->ConstructL(aUnit,aFactory);
    CleanupStack::Pop();
	return p;
	}

void CBTPort::ConstructL(TUint aUnit,CBTPortStateFactory* aFactory)
/**
	ConstructL(..) is the second phase construction of CBTPort.
	  
	This sets the CSY role to DTE, instantiates the state machine and 
	registers the CPort name to whatever the aUnit is.
	
	@param	aUnit		is the virtual BTComm port number
	@param	aFactory	is the state machine factory used to access the 
						proxy states
**/
	{
	LOG_FUNC
	iRole=ECommRoleDTE;	// set to DTE by default.
	iPortProxy=CBTPortProxy::NewL(TUint32(aUnit),this,aFactory);
	TName name;
	name.Format(_L("%d"), aUnit);
	SetNameL(&name);
	}

CBTPort::~CBTPort()
/**
	The CBTPort destructor is either invoked immediately from destruct or
	via deferred deletion mechanism via the "active shutterdowner".
**/
	{
	LOG_FUNC
	delete iPortProxy;
	}

void CBTPort::Destruct()
/**
	CBTPort Destruct, is invoked by c32 to destroy the port.  	

	At this point it is necessary to either destroy the port immediately or 
	invoke a deferred deletion "active shutterdowner" mechanism.
**/
	{
	LOG_FUNC

	if (iPortProxy)
		iPortProxy->Close();
	}

void CBTPort::DestructNow()
/**
	To be called by CBTPortProxy to signal the shutdown.
*/
	{
	LOG_FUNC
	delete this;
	}

void CBTPort::StartRead(const TAny* aClientBuffer,TInt aLength)
/**
	Reads any inbound data into the client buffer supplied.
	
	The read is passed over to the CBTPortProxy for completion and when it is 
	eventually completed to RFComm; read completion is signalled to the client
	by invoking the CPort::ReadCompleted mechanism.

	@param	aClientBuffer	is the pointer to the client buffer to be filled
	@param	aLength			the desired amount of bytes to be filled in the buffer
**/
	{
	LOG_FUNC
	iPortProxy->Read(aClientBuffer,aLength);
	}

void CBTPort::ReadCancel()
/**
	Cancels the pending read.
**/
	{
	LOG_FUNC
	iPortProxy->ReadCancel();
	}

TInt CBTPort::QueryReceiveBuffer(TInt& aLength) const
/**
	Retrieval of the number of bytes waiting in the receive buffer.
	
	@param		aLength		is set to zero
	@return		KErrNone
*/
	{
	LOG_FUNC
	return iPortProxy->QueryReceiveBuffer(aLength);
	}

void CBTPort::ResetBuffers(TUint aFlags)
/**
	Resets the Rx buffers, does nothing about the Tx buffer.
	The reason why we don't do anything about the Tx buffer is 
	because the CSY passes the client data straight to ESock.

	@param aFlags may be KCommResetRx, KCommResetTx or KCommResetRx|KCommResetTx
**/
	{
	LOG_FUNC
	if((aFlags==KCommResetTx)||(aFlags==0)) //only
		{
		return; //nothing to do
		}
	iPortProxy->ResetRxBuffer();// holds true for the case > KCommResetRx|KCommResetTx
								// as well, since we cannot signal the client
	}

void CBTPort::StartWrite(const TAny* aClientBuffer, TInt aLength)
/**
	Uses the supplied buffer to read out data from. 
	These data are destined to be send to the RFComm layer of the Bluetooth stack.

	@param	aClientBuffer	the pointer to the clients buffer from which to read from
	@param	aLength			the number of desired bytes to be read out.
*/
	{
	LOG_FUNC
	iPortProxy->Write(aClientBuffer,aLength);
	}

void CBTPort::WriteCancel()
/**
	Cancels the last requested Write if possible.
*/
	{
	LOG_FUNC
	iPortProxy->WriteCancel();
	}

void CBTPort::Break(TInt /*aTime*/)
/** 
	CBTPort Break is Not supported.
**/
	{
	LOG_FUNC
	}

void CBTPort::BreakCancel()
/** 
	CBTPort BreakCancel is not supported.
**/
	{
	LOG_FUNC
	LOG(_L("CBTPort: [WARNING] Non-implemented feature (* BreakCancel *) used by client"));
	}

TInt CBTPort::GetConfig(TDes8& aPackage) const
/**
	CBTPort GetConfig gives the CSY's configuration TCommConfigV01 to the client.

	@param	aPackage	is the packaged TCommConfigV01 supplied by the client, to be filled.
	@return		KErrNone always
*/
	{
	LOG_FUNC
    // copy back local iConfig
	aPackage.FillZ();
    TPckg<TCommConfigV01> cfg(iConfig);
	aPackage.Copy(cfg);
	return KErrNone;
	}

TInt CBTPort::SetConfig(const TDesC8& aPackage)
/**
	CBTPort SetConfig allows for the configuration of the CSY.
	Currently the only configuration which is allowed and is meaningful, is the
	configuration associated with terminated reads; the rest are not honoured.

	@param aPackage		is a packaged TCommConfigV01 struct.
	@return		KErrNone or KErrNotSupported if the terminator count is 
				greater than KConfigMaxTerminators
	
*/
	{
	LOG_FUNC
    TCommConfigV01 c;
    Mem::FillZ(&c,sizeof(c));
	TInt len=Min(aPackage.Length(),sizeof(c));
	Mem::Copy(&c, aPackage.Ptr(),len);
	if(c.iTerminatorCount)
		{
		if (c.iTerminatorCount>KConfigMaxTerminators)
			{
			return KErrNotSupported;
			}
		else
			{
			iPortProxy->iTerminatedReads=ETrue;
			iPortProxy->iTerm.iTerminatorCount=c.iTerminatorCount;
			for (TInt i=0;i<c.iTerminatorCount;i++)
				{
				iPortProxy->iTerm.iTerminatorChars[i]=c.iTerminator[i];
				}
			}
		}
	else
		{
		iPortProxy->iTerminatedReads=EFalse;
		}
	iConfig=c;
	return KErrNone;
	}

TInt CBTPort::SetServerConfig(const TDesC8& /*aTCommServerConfigV01-Package*/)
/** 
	CBTPort SetServerConfig is not supported.
	SetServerConfig is otherwise used to mandate the buffering strategy within 
	the CSY. The Bluetooth CSY uses it's own buffering startegy which is not
	exposed to the client nor it is reconfigurable by it.
	
	@return KErrNotSupported
**/
	{
	LOG_FUNC
	LOG(_L("CBTPort: [WARNING] Non-implemented feature (* SetServerConfig *) used by client"));
	return KErrNotSupported;
	}

TInt CBTPort::GetServerConfig(TDes8& /*aPackage*/)
/**
	CBTPort GetServerConfig is not supported

	@return KErrNotSupported
**/
	{
	LOG_FUNC
	LOG(_L("CBTPort: [WARNING] Non-implemented feature (* GetServerConfig *)  used by client"));
	return KErrNotSupported;
	}

TInt CBTPort::GetCaps(TDes8& aPackage)
/**
	Get current Bluetooth CSY serial port emulation capabilities.
	Capabilities are defined in c32comm.h.

	@param aPackage		can either be a packaged TCommCapsV01 or TCommCapsV02
	@return				KErrNone
*/
	{
	LOG_FUNC
    aPackage.FillZ();
	TPckg<TCommCapsV02> caps(iCaps);
	TInt len=Min(caps.Length(),aPackage.Length());
	//to be on the safe side just in case the package is v01
	aPackage.Copy(caps.Left(len));
	return KErrNone;
	}

TInt CBTPort::GetSignals(TUint& /*aSignals*/)
/**
	GetSignal is not supported.
	Would otherwise gets current signal emulation states.

	@return		KErrNotSupported
*/
	{
	//	Signals are set as follows (from c32comm.h):
	//	KSignalCTS, KSignalDSR, KSignalDCD, KSignalRNG, KSignalRTS, KSignalDTR.

	LOG_FUNC
	LOG(_L("CBTPort: [WARNING] Non-implemented feature (* GetSignals *) used by client"));
	return KErrNotSupported;
	}

TInt CBTPort::SetSignalsToMark(TUint /*aSignals*/)
/**
	SetSignalsToMark is not supported.

	@return		KErrNotSupported
**/
	{
	// Set signals to Mark (high).
	// Signals are defined as follows (from c32comm.h):
	// KSignalCTS, KSignalDSR, KSignalDCD, KSignalRNG, KSignalRTS, KSignalDTR.

	LOG_FUNC
	LOG(_L("CBTPort: [WARNING] Non-implemented feature (* SetSignalsToMark *) used by client"));
	return KErrNotSupported;
	}

TInt CBTPort::SetSignalsToSpace(TUint /*aSignals*/)
/**
	Set Signals to Space is not supported.
	
	@return KErrNotSupported
**/
	{
	LOG_FUNC
	LOG(_L("CBTPort: [WARNING] Non-implemented feature (* SetSignalsToSpace *) used by client"));
	//	Signals are defined as follows (from c32comm.h):
	//	KSignalCTS, KSignalDSR, KSignalDCD, KSignalRNG, KSignalRTS, KSignalDTR.

	return KErrNotSupported;
	}

TInt CBTPort::SetReceiveBufferLength(TInt /*aLength*/)
/**
	Setting of the  receiver buffer length is not allowed.

	@return KErrNotSupported
**/
	{
	LOG_FUNC
	LOG(_L("CBTPort: [WARNING] Non-implemented feature (* SetReceiveBufferLength *) used by client"));
	return KErrNotSupported;
	}

TInt CBTPort::GetReceiveBufferLength(TInt& aLength) const
/**
	Assigns to the parameter the max length of the CSY's receive buffer.

	@param		aLength		the max size of the receive buffer in the CSY
	@return		KErrNone or an appropriate error code
*/
	{
	LOG_FUNC
	return iPortProxy->GetReceiveBufferLength(aLength);
	}

void CBTPort::FreeMemory()
/**
	CBTPort FreeMemory, this function is called by nothing and is not supported.
**/
	{
	LOG_FUNC
	LOG(_L("CBTPort: [WARNING] Non-implemented feature (* FreeMemory *) used by client"));
	}

//*********************** EXTRAS FOR C32 v6 ************************

void CBTPort::NotifySignalChange(TUint /*aChange*/)
/**
	Signal Change Notification is not supported by the Bluetooth CSY.
	RComm::Caps(..) should be called to read the CSY's capabilities,
	which indicate that notification is not supported.
*/
	{
	LOG_FUNC
	LOG(_L("CBTPort: [WARNING] Non-implemented feature (* NotifySignalChange *) used by client"));
	}


void CBTPort::NotifySignalChangeCancel(void)
/**
	Notify Signal Change Cancel is not supported.
	RComm::Caps(..) should be called to read the CSY's capabilities,
	which indicate that notification is not supported.

	Note:	this method may be called implicitly by RComm::Cancel and will
			have no effect.

*/
	{
	LOG_FUNC
	LOG(_L("CBTPort: [WARNING] Non-implemented feature (* NotifySignalChangeCancel *) used by client"));
	}

void CBTPort::NotifyConfigChange(void)
/**
	NotifyConfigChange is not supported.
	RComm::Caps(..) should be called to read the CSY's capabilities,
	which indicate that notification is not supported.
*/
	{
	LOG_FUNC
	LOG(_L("CBTPort: [WARNING] Non-implemented feature (* NotifyConfigChange *) used by client"));
	}

void CBTPort::NotifyConfigChangeCancel(void)
/**
	NotifyConfigChangeCancel is not supported.
	RComm::Caps(..) should be called to read the CSY's capabilities,
	which indicate that notification is not supported.

  	Note:	this method may be called implicitly by RComm::Cancel and will
			have no effect.

*/
	{
	LOG_FUNC
	LOG(_L("CBTPort: [WARNING] Non-implemented feature (* NotifyConfigChangeCancel *) used by client"));
	}

void CBTPort::NotifyFlowControlChange(void)
/**
	NotifyFlowControlChange is not supported.
	RComm::Caps(..) should be called to read the CSY's capabilities,
	which indicate that notification is not supported.

*/
	{
	LOG_FUNC
	LOG(_L("CBTPort: [WARNING] Non-implemented feature (* NotifyFlowControlChange *) used by client"));
	}

void CBTPort::NotifyFlowControlChangeCancel(void)
/**
	NotifyFlowControlChangeCancel is not supported.
	RComm::Caps(..) should be called to read the CSY's capabilities,
	which indicate that notification is not supported.

	Note:	this method may be called implicitly by RComm::Cancel and will
			have no effect.

*/
	{
	LOG_FUNC
	LOG(_L("CBTPort: [WARNING] Non-implemented feature  (* NotifyFlowControlChangeCancel *) used by client"));
	}

void CBTPort::NotifyBreak(void)
/**
	Notify Break is not supported.
	RComm::Caps(..) should be called to read the CSY's capabilities,
	which indicate that notification is not supported.
*/
	{
	LOG_FUNC
	LOG(_L("CBTPort: [WARNING] Non-implemented feature (* NotifyBreak *) used by client"));
	}

void CBTPort::NotifyBreakCancel(void)
/**
	NotifyBreakCancel is not supported.
	RComm::Caps(..) should be called to read the CSY's capabilities,
	which indicate that notification is not supported.

	Note:	this method may be called implicitly by RComm::Cancel and will
			have no effect.
  
*/
	{
	LOG_FUNC
	LOG(_L("CBTPort: [WARNING] Non-implemented feature (* NotifyBreakCancel *) used by client"));
	}

void CBTPort::NotifyDataAvailable(void)
/**
	NotifyDataAvailable is not supported.
	RComm::Caps(..) should be called to read the CSY's capabilities,
	which indicate that notification is not supported.
*/
	{
	LOG_FUNC
	LOG(_L("CBTPort: [WARNING] Non-implemented feature (* NotifyDataAvailable *) used by client"));
	}

void CBTPort::NotifyDataAvailableCancel(void)
/**
	NotifyDataAvailableCancel is not supported.
	RComm::Caps(..) should be called to read the CSY's capabilities,
	which indicate that notification is not supported.

	Note:	this method may be called implicitly by RComm::Cancel and will
			have no effect.

*/
	{
	LOG_FUNC
	LOG(_L("CBTPort: [WARNING] Non-implemented feature (* NotifyDataAvailableCancel *)used by client"));
	}

void CBTPort::NotifyOutputEmpty(void)
/**
	NotifyOutputEmptyIndication is not supported.
	RComm::Caps(..) should be called to read the CSY's capabilities,
	which indicate that notification is not supported.
*/
	{
	LOG_FUNC
	LOG(_L("CBTPort: [WARNING] Non-implemented feature (* NotifyOutputEmpty *) used by client"));
	}

void CBTPort::NotifyOutputEmptyCancel(void)
/**
	NotifyOutputEmptyCancel.
	RComm::Caps(..) should be called to read the CSY's capabilities,
	which indicate that notification is not supported.

	Note:	this method may be called implicitly by RComm::Cancel and will
			have no effect.
  
*/
	{
	LOG_FUNC
	LOG(_L("CBTPort: [WARNING] Non-implemented feature (* NotifyOutputEmptyCancel *) used by client"));
	}

TInt CBTPort::GetFlowControlStatus(TFlowControl& /*aFlowControl*/)
/**
	Getting of serial port emulation flow control status is not supported.

	@return KErrNotSupported
**/
	{
	LOG_FUNC
	LOG(_L("CBTPort: [WARNING] Non-implemented feature (* GetFlowControlStatus *) used by client"));
	return KErrNotSupported;
	}

TInt CBTPort::GetRole(TCommRole& aRole)
/**
	Get serial port emulation mode.
	Options in general are: ECommRoleDTE or ECommRoleDCE but the Bluetooth CSY
	only supports the DTE role, hence aRole will always be set to DTE for now.
**/
	{
	LOG_FUNC
	aRole=iRole;
	return KErrNone;
	}

TInt CBTPort::SetRole(TCommRole aRole)
/**
	Set serial port emulation mode, options are: ECommRoleDTE only.
	If the client tries to open the RComm session as a DCE then the client call
	will cause a Leave in CPort::DoOpenL(..), this will subsequently be traped
	and the error will be propagated to the client (which may Panic with a 
	EReqActiveObjectLeave if the error doesn't get handled).

	@param		aRole	should be ECommRoleDTE
	@return		KErrNone or KErrNotSupported
*/
	{
	LOG_FUNC
	if(aRole==ECommRoleDTE)
		{
		iRole=aRole;
		return KErrNone;
		}
	return KErrNotSupported;
	}

#pragma warning (default : 4100)  // Unreferenced formal parameter

