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
// * Description:  This file contains the implementation for the class CPortFactory.
// *               This class is used to create C32 port interface instances.
//

// PortFactory.cpp

/** @file PortFactory.cpp
 *
 */

/*! \mainpage CSY 27.010 Documentation
 *
 * \section intro Introduction
 *
 * This CSY has been designed and implemented by TapRoot Systems.
 * This CSY is part of the TapRoot Systems Communications Platform (CommP)
 * product family for Symbian OS. Please contact TapRoot Systems for 
 * information on CommP.
 * <p>
 * A CSY is a specialized type of DLL that implements API interfaces
 * required by the C32 Serial Communications Server. "CSY" is the file
 * name extension used by CSY modules as in "ecuart.csy", which is the
 * name of Symbian's generic CSY for non-multiplexed serial ports.  
 * TapRoot Systems 27.010 CSY (gsm0701.csy) is compatible with the
 * 3GPP 27.010 v5.0.0 specification.
 * <p>
 * The CSY has been designed and tested with the Intel Lubbock application
 * processor, TI Condat baseband processor and Symbian OS 8.0a.
 * The CSY is not guaranteed to work with other AP's, BP's or Symbian OS versions.
 * The CSY has been integrated with Symbian OS supplied TSY and NIF IP.
 * The CSY is not guaranteed to work with other TSY's and NIF's.
 *
 * \section feat Features
 *
 * The CSY supports the 27.010 Basic Option. The 3GPP 27.010 specification 
 * is derived from the GSM 07.10 specification. 3GPP 27.010 defines a mechanism
 * whereby multiple commands and data streams can be carried over the same
 * physical link. This specification defines a multiplexing protocol between
 * a Terminal Equipment (TE) and a Mobile Station (MS) allowing multiple 
 * simultaneous sessions over a single start-stop, framed, serial link. 
 * Each session (channel) can carry different types of data including voice,
 * fax, data, SMS, CBS, phonebook maintenance, battery status, GPRS, USSD, etc.
 * <p>
 * The multiplexer has three operating options, basic, advanced without error
 * recovery and advanced with error recovery. CSY 27.010 supports the
 * basic option. The basic option is recommended on highly reliable links. 
 * Advanced options are not supported by the CSY. The characteristics of the 
 * basic option are:
 * <li> Length indicator used instead of the HDLC transparency mechanism
 * <li> Different flag octet from that used by HDLC
 * <li> Cannot be used on links that use XON/XOFF flow control
 * <li> May have longer recovery procedure from loss of synchronization
 * <p>
 * Each channel between the TE and MS is called a Data Link Connection (DLC).
 * A special DLC called "control channel" is used to exchange management 
 * information (e.g. parameter negotiation), power saving control information,
 * testing, flow control, close down, etc. between the TE and MS.
 * <p>
 * The multiplexer is activated with the 3GPP 27.007 AT+CMUX command.
 * <p>
 * 
 * \section install Installation
 *
 * Refer to the readme.txt file for installation and build instructions.
 * <br>
 * Please contact TapRoot Systems for information on product updates and
 * additional support.
 */

/** @file PortFactory.cpp
 * This file contains the implementation for the class CPortFactory.
 * This class is used to create C32 port interface instances.
 */

#include <cs_port.h>
#include <f32file.h>
#include <commsdattypesv1_1.h>
#include <commsdat_partner.h>
using namespace CommsDat;
#include "Portfactory.h"
#include "CsyGlobals.h"
#include "Mux0710Protocol.h"
#include "ChannelMgrCtrl.h"
#include "ChannelMgrCmdData.h"
#include "PortC32InterfaceBase.h"
#include "PortC32Interface.h"
#include "PortC32InterfaceIp.h"
#include "CommFrameWriterAo.h"
#include "CommFrameReaderAo.h"
#include "CsyDebugLogger.h"

CPortFactory* CPortFactory::NewL()
/**
 * This method uses two phase construction and the cleanup stack to create
 * an instance of class CPortFactory. This method is invoked when the user
 * invokes the C32 comm server's LoadCommModule() method to load the CSY.
 * Note that multiple instances of CPortFactory can be created by C32,
 * so no memory allocation or objects should be created when the 
 * port factory is first created.
 *
 * @return Pointer to the created instance of CPortFactory
 */
	{
	_LOG_L4C1("CPortFactory::NewL");

	CPortFactory* self = new(ELeave) CPortFactory;
	TCleanupItem closeSelf(CPortFactory::CloseObject, self);
	CleanupStack::PushL(closeSelf);
	self->ConstructL();
	CleanupStack::Pop(self);
	return (self);
	}

CPortFactory::CPortFactory()
: iPortC32InterfaceList(_FOFF(CPortC32Interface, iLink)),
  iPortC32InterfaceIter(iPortC32InterfaceList),
  iDataChannelList(_FOFF(CChannelMgrCmdData, iLink)),
  iDataChannelIter(iDataChannelList)
/**
 * Constructor.
 */
	{
	_LOG_L4C1("CPortFactory::CPortFactory");
	}

CPortFactory::~CPortFactory()
/**
 * Destructor. Delete all objects and memory created/allocated by this class.
 */
	{
	_LOG_L4C1(">>CPortFactory::~CPortFactory");

	// check if the CSY's objects have been created 
	// C32 creates a new instance of the port factory each time a client
	// loads the CSY. The C32 recognizes a redundant CSerial name and promptly
	// destroys the redundant CSY.
	if (iMuxObjectsCreated)
		{
		// client channels
		CChannelMgrCmdData* anyChannel = NULL;
		iDataChannelIter.SetToFirst();
		while ((anyChannel = iDataChannelIter++) != NULL)
			{
			iDataChannelList.Remove(*anyChannel);
			delete anyChannel;
			}

		// control channel
		delete iChannelCtrl;

		delete iCommWriterAo;
		delete iCommReaderAo;

		delete iMux0710Protocol;

		// may not be necessary ???
		// MAF /\ what does this comment mean

		// delete any remaining C32 port objects that were not closed by the client
		CPortC32InterfaceBase* port = NULL;
		iPortC32InterfaceIter.SetToFirst();
		while ((port = iPortC32InterfaceIter++) != NULL)
			{
			delete port;
			}

		iCommPort.Close();
		}

	//iMuxObjectsCreated = EFalse;

	_LOG_L4C1("<<CPortFactory::~CPortFactory");
	_LOG_L3C1(" ");
	}

void CPortFactory::ConstructL()
/**
 * 2nd-phase construction. Registers the CSY Name with C32.
 */
	{
	_LOG_L4C1("CPortFactory::ConstructL");

#ifdef _DEBUG
	TInt usedCellCount;
	TInt freeCellCount;
	usedCellCount = User::CountAllocCells(freeCellCount);
	_LOG_L4C3("Cell Counts Used = %d   Free = %d", usedCellCount, freeCellCount);
#endif

	// inform C32 of the CSY name
	TName name(KCSY_Gsm0710ServerName);
	TInt retVal1 = SetName(&name);
	if (retVal1 != KErrNone)
		{
		_LOG_L1C3("** Failed to set CSY:%S in C32. Err: %d **", &name, retVal1);
		User::Leave(retVal1);
		}
	iVersion = TVersion(KCSY_Gsm0710MinorVersionNumber,
						KCSY_Gsm0710MajorVersionNumber,
						KCSY_Gsm0710BuildVersionNumber);
	}

void CPortFactory::CreateCsyObjectsL()
/**
 * This method is called to create the CSY objects for the multiplexing channels,
 * message queues, i/o interface, etc.  This method should be called when the
 * 1st client request to create a port is received by the CSY via NewPortL().
 */
	{
	_LOG_L4C1(">>CPortFactory::CreateCsyObjectsL");

	// set flag
	iMuxObjectsCreated = ETrue;
	iOpenPortFailed = EFalse;

	// load physical device drivers for serial port(s)

#ifdef __WINS__
	// 1st make sure the file server is running
	RFs fileServer;
	User::LeaveIfError(fileServer.Connect());
	fileServer.Close();

	_LIT(KPDDName, "ECDRV");
	TInt retVal1 = User::LoadPhysicalDevice(KPDDName);
	if ((retVal1 != KErrNone) && (retVal1 != KErrAlreadyExists))
		User::Leave(retVal1);

#else  // target

	TBuf<20> commPddName;
	TInt retVal1;

	commPddName.Copy(KCsyDefaultPDDNameComm1);
	retVal1 = User::LoadPhysicalDevice(commPddName);
	if ((retVal1 != KErrNone) && (retVal1 != KErrAlreadyExists))
		User::Leave(retVal1);
	// only load 2nd PDD if debugging is enabled 

#ifdef __DEBUGSERIALPORT__
	commPddName.Copy(KCsyDefaultPDDNameComm2);
	retVal1 = User::LoadPhysicalDevice(commPddName);
	if ((retVal1 != KErrNone) && (retVal1 != KErrAlreadyExists))
		User::Leave(retVal1);
#endif //__DEBUGSERIALPORT__

#endif  //__WINS__

	// load logical device driver for serial ports
	_LIT(KLDDName, "ECOMM");
	retVal1 = User::LoadLogicalDevice(KLDDName);
	if ((retVal1 != KErrNone) && (retVal1 != KErrAlreadyExists))
		User::Leave(retVal1);

	ConfigurePhysicalCommPortL();
	
	// create 27.010 framer and comm i/o objects
	iMux0710Protocol = CMux0710Protocol::NewL(*this);
	iCommWriterAo    = CCommFrameWriterAo::NewL(this, iMux0710Protocol);
	iMux0710Protocol->SetCommWriter(iCommWriterAo);

	iCommReaderAo    = CCommFrameReaderAo::NewL(this, iMux0710Protocol);
	iCommReaderAo->Read();  // start it
	
	// create the Multiplexer Control Channel
	iChannelCtrl = CChannelMgrCtrl::NewL(*this, *iMux0710Protocol);

	// create All Multiplexer Channels	
	for (TUint8 i=1; i < KCsyDefaultMaxDLCChannels; i++)
		{
		CChannelMgrCmdData* channelMgrCmd =
			CChannelMgrCmdData::NewL(i, *this, *iMux0710Protocol);
		iDataChannelList.AddLast(*channelMgrCmd);
		}

	// switch to multiplexer mode
	iMux0710Protocol->SwitchToMuxMode();

#ifdef _DEBUG
	TInt usedCellCount;
	TInt freeCellCount;
	usedCellCount = User::CountAllocCells(freeCellCount);
	_LOG_L4C3("Cell Counts Used = %d   Free = %d", usedCellCount, freeCellCount);
#endif

	_LOG_L4C1("<<CPortFactory::CreateCsyObjectsL");
	}

void CPortFactory::CloseObject(TAny* aObject)
/**
 * Utility to close objects created by the port factory. 
 * Used during construction with the cleanup stack.
 *
 * @param aObject - The CObject to close.
 * @return void
 */
	{
	reinterpret_cast<CObject*>(aObject)->Close();
	}

CPort* CPortFactory::NewPortL(const TUint aUnit)
/**
 * This method is called by C32 when a port is opened via RComm::Open().
 * An instance of the CSY's class CPortC32Interface is created.
 * The instance is assigned a channel manager and then is added to
 * the list of C32 port interface objects.
 *
 * @param aUnit - Numeric portion of the port identifier
 * @return Pointer to the created C32 port interface object
 */
	{
	_LOG_L4C2(">>CPortFactory::NewPortL [aUnit=%d]", aUnit);

	TC32PortInfo portInfo;

	portInfo.iClientType = GetClientType(aUnit);
	portInfo.iPortNumber = aUnit;

	// check if port number is valid and translates to a client type
	if (portInfo.iClientType == EC32ClientUndefined)
		{
		_LOG_L4C1("<<CPortFactory::NewPortL - ** Unknown client **");
		return NULL;
		}

	// check if this is the 1st port to be opened
	if (!iMuxObjectsCreated)
		{
		_LOG_L4C1("first port to be opened");
		CreateCsyObjectsL();
		}
	else if (iChannelCtrl->GetChannelReady() == EFalse)
		{
		_LOG_L4C1("CSY objects already exist");
		// switch to multiplexer mode
		iMux0710Protocol->SwitchToMuxMode();
		}

	// first make sure control channel is already connected
	iChannelCtrl->WaitForChannelReady();
	
	if (! iOpenPortFailed)
		{
		// create a new C32 Interface Port based on port number
		CPortC32InterfaceBase* port = NULL;
	
		if (portInfo.iClientType == EC32ClientIpNif)
			{
			_LOG_L4C1("Raw IP port");
 			port = CPortC32InterfaceIp::NewL(*this, portInfo);
 			}
		else
			{
			_LOG_L4C1("Normal port");
 			port = CPortC32Interface::NewL(*this, portInfo);
 			}
	
		CleanupStack::PushL(port);
		CChannelMgrCmdData* channel = FindChannelMgr(aUnit);

		if (channel == NULL)
			{
			_LOG_L2C1("** channel is null, delete interface **");
			CleanupStack::PopAndDestroy(port);
			User::Leave(KErrNotFound);
			}
		TName name;
		name.Format(_L("%d"), aUnit);
		port->SetName(&name);

		// channel->Open() will add this port to it's port list, and connect channel if needed
		// Make sure that during channel init we do not tx/rx through channel
		channel->Open(port);
	
		iNumOfOpenPorts++;
		_LOG_L4C2("No. of ports = %d",iNumOfOpenPorts);
 		
		port->SetMuxChannel(channel);

		channel->WaitForChannelReady();

		//Need to wait for the channel to be ready. How to wait for something without blocking whole thread?
		// add it to list of ports
		iPortC32InterfaceList.AddLast(*port);	
		CleanupStack::Pop(port);

#ifdef _DEBUG
		TInt usedCellCount;
		TInt freeCellCount;
		usedCellCount = User::CountAllocCells(freeCellCount);
		_LOG_L4C3("Cell Counts Used = %d   Free = %d", usedCellCount, freeCellCount);
#endif

		_LOG_L4C2("<<CPortFactory::NewPortL [port=%d]", port);
		return port;
		}
	else //iPortOpenFailed == ETrue
		{
		_LOG_L4C1("<<CPortFactory::NewPortL [port=NULL]");
		User::Leave(KErrTimedOut);
		return NULL;
		}
	}
 
void CPortFactory::Info(TSerialInfo& aSerialInfo)
/**
 * Called by C32 to identify the CSY module.
 * @param aSerialInfo - Reference holder for information to be populated by the function.
 * @return void
 */
	{
	_LOG_L4C1("CPortFactory::Info");

	aSerialInfo.iDescription	= KCSY_Description;
	aSerialInfo.iName			= KCSY_Gsm0710ServerName;
	aSerialInfo.iLowUnit		= KCSY_Gsm0710LowUnit;
	aSerialInfo.iHighUnit		= KCSY_Gsm0710HighUnit;
	}

CPortFactory::TC32ClientType CPortFactory::GetClientType(TInt aC32PortNum)
/**
 * Returns the ClientType for the specified C32 port number.
 * @param aC32PortNum -  C32 port number
 * @return Client type - TSY or NIF
 */
	{ 
	_LOG_L4C2("CPortFactory::GetClientType [aC32PortNum=%d]", aC32PortNum);

	if (aC32PortNum >= KCOMMP_IP_NIF_OFFSET)
		{
		_LOG_L4C1("Raw Ip");
		return CPortFactory::EC32ClientIpNif;
		}
	else if (aC32PortNum == KCOMMP_CSD_PPP_NUMBER)
		{ 
		_LOG_L4C1("Csd PPP");
		return CPortFactory::EC32ClientNif;
		}

	_LOG_L4C1("Tsy");
	return CPortFactory::EC32ClientTsy;
	}

CPortC32InterfaceBase* CPortFactory::FindPortC32Interface(const TUint8 aDlcNum)
/**
 * This method returns a pointer to the C32 port interface object 
 * that is associated with the specified DLC number.
 *
 * @param aDlcNum - DLC number
 * @return Pointer to C32 port interface object or NULL if not found
 */
	{
	_LOG_L4C2(">>CPortFactory::FindPortC32Interface [aDlcNum=%d]", aDlcNum);

	CPortC32InterfaceBase* port = NULL;

	CChannelMgrCmdData* anyChannel = NULL;
	iDataChannelIter.SetToFirst();
	while ((anyChannel = iDataChannelIter++) != NULL)
		{
		if (anyChannel->GetDlcNumber() == aDlcNum)
			{
			_LOG_L4C1("AT channel");
			port = anyChannel->GetC32Port();
			break;
			}
		}
	
	_LOG_L4C2("<<CPortFactory::FindPortC32Interface [port=0x%x]",port);
	return port;
	}

CChannelMgrCmdData* CPortFactory::FindChannelMgr(const TUint aC32PortNum)
/**
 * This method is only called when a port is created.
 *
 * @param aC32PortNum - C32 port number
 * @return Pointer to the channel manager object or NULL
 */
	{
	_LOG_L4C2(">>CPortFactory::FindChannelMgr [aC32PortNum=%d]", aC32PortNum);

	TUint dlcNum = ++iLastDlcNum;
	_LOG_L4C2("iLastDlcNum: %d", iLastDlcNum);

	//
	// MAF Horrible
	// This just counts through the array of channel pointers
	// and returns a pointer when the count gets to the correct value.
	// 
	// We should be able to use an array here and directly access it using
	// the dlc number!?!?
	//

	TUint count  = 1;
	CChannelMgrCmdData* anyChannelCmd;

	iDataChannelIter.SetToFirst();
	while ((anyChannelCmd = iDataChannelIter++) != NULL)
		{
		if (count == dlcNum)
			{
			_LOG_L4C2("<<CPortFactory::FindChannelMgr() - found, count %d",count);
			return anyChannelCmd;
			}
		count++;
		}

	_LOG_L4C1("<<CPortFactory::FindChannelMgr() - *** NOT FOUND ***");
	return NULL;
	}

CChannelMgrCmdData* CPortFactory::FindChannelMgrByDlcNum(const TUint8 aDlcNum)
/**
 * This method returns a pointer to the channel manager object 
 * that has the specified  DLC number.
 *
 * @param aDlcNum -  DLC number
 * @return Pointer to the channel manager object or NULL
 */
	{
	_LOG_L4C2(">>CPortFactory::FindChannelMgrByDlcNum [aDlcNum=%d]", aDlcNum);

    CChannelMgrCmdData* channel = NULL;
	iDataChannelIter.SetToFirst();
	while ((channel = iDataChannelIter++) != NULL)
		{
		if (channel->GetDlcNumber() == aDlcNum)
			{
			_LOG_L4C1("AT channel");
			return channel;
			}
		}

	_LOG_L2C1("<<CPortFactory::FindChannelMgrByDlcNum() - NOT FOUND");
	return NULL;
	}

TInt CPortFactory::ConfigurePhysicalCommPortL()
/**
 * This method is used to configure the specified physical comm port.
 * The parameters are read from CommDb or supplied by C32.
 * @param aPhysPortNum - Physical port number
 * @return KErrorNone or KErrAccessDenied.
 */
	{
	_LOG_L4C1(">>CPortFactory::ConfigurePhysicalCommPortL");

	// create a package pointer
	TPckgBuf<TCommConfigV01> newCfg;
	TInt ret, len;

	// get the modem table	- Get port settings from CommDB.
	CMDBSession* db = CMDBSession::NewL(KCDLatestVersion);

	CleanupStack::PushL(db);

	TUint32 modemId = 0;

	// We are using the same modem as TSY
	// Accessing the ModemPhoneServicesSMS field in the Global Settings Table
	CMDBField<TUint32>* globalSettingsField = new(ELeave) CMDBField<TUint32>(KCDTIdModemPhoneServicesSMS);
	CleanupStack::PushL(globalSettingsField);
	globalSettingsField->SetRecordId(1);
	globalSettingsField->LoadL(*db);
	modemId = *globalSettingsField;	// The field value gives the record id in the Modem Bearer Table.
	CleanupStack::PopAndDestroy(globalSettingsField);
	
	_LOG_L4C2("Accessing modem entry %d in the modem table", modemId);

	TUint32 rate;
	TUint32 dataBits;
	TUint32 stopBits;
	TUint32 parity;
	TUint32 handshake;
	TPckgBuf<TCommConfigV01> config;
	
	TUint port = 0;
	TBuf<KCommsDbSvrMaxFieldLength> controlPort;
	
	// Accessing the ControlChannelPort Name from the ModemBearer Table in the record number 
	//corresponding to modemId
	CMDBField<TDesC>* portField = new(ELeave) CMDBField<TDesC>(KCDTIdControlChannelPortName);
	CleanupStack::PushL(portField);
	portField->SetRecordId(modemId);
	portField->SetMaxLengthL(KMaxTextLength);
	portField->LoadL(*db);
	controlPort = *portField;
	CleanupStack::PopAndDestroy(portField);
	// Diagnostics - control port
	if (controlPort.Length())
		{
		TBuf8<16> portName;
		portName.Copy(controlPort.Left(16));
		_LOG_L4C2("Control port(MODEM_CONTROL_CHANNEL_PORT_NAME) '%S'",
			&portName);

		// Get number of port out of the port descriptor.
		_LIT(KDoubleColon, "::");
		TInt len = controlPort.Find(KDoubleColon);
		if (len < KErrNone)
			{
			_LOG_L1C1("** finding :: in control port failed **");
			User::Leave(KErrGeneral);
			}
		TUint numPos = len + KDoubleColon.iTypeLength;
		TPtrC numPtr(&controlPort[numPos], controlPort.Length() - numPos);
		TLex lexer(numPtr);
		TInt ret = lexer.Val(port);
		if (ret)
			{
			_LOG_L1C1("** lexing to get port number failed **");
			User::Leave(ret);
			}
		}
	else
		{
		_LOG_L1C1("** Control port not set in commDb **");
		User::Leave(KErrGeneral);
		}
	
	// Accessing the Rate from the ModemBearer Table in the record number 
	//corresponding to modemId
	CMDBField<TUint32>* rateField = new(ELeave) CMDBField<TUint32>(KCDTIdRate);
	CleanupStack::PushL(rateField);
	rateField->SetRecordId(modemId);
	rateField->LoadL(*db);
	rate = *rateField;
	CleanupStack::PopAndDestroy(rateField);
	
	// Accessing the DataBits from the ModemBearer Table in the record number 
	//corresponding to modemId
	CMDBField<TUint32>* dataBitsField = new(ELeave) CMDBField<TUint32>(KCDTIdDataBits);
	CleanupStack::PushL(dataBitsField);
	dataBitsField->SetRecordId(modemId);
	dataBitsField->LoadL(*db);
	dataBits = *dataBitsField;
	CleanupStack::PopAndDestroy(dataBitsField);

	// Accessing the StopBits from the ModemBearer Table in the record number 
	//corresponding to modemId
	CMDBField<TUint32>* stopBitsField = new(ELeave) CMDBField<TUint32>(KCDTIdStopBits);
	CleanupStack::PushL(stopBitsField);
	stopBitsField->SetRecordId(modemId);
	stopBitsField->LoadL(*db);
	stopBits = *stopBitsField;
	CleanupStack::PopAndDestroy(stopBitsField);
	
	// Accessing the Parity from the ModemBearer Table in the record number 
	//corresponding to modemId
	CMDBField<TUint32>* parityField = new(ELeave) CMDBField<TUint32>(KCDTIdParity);
	CleanupStack::PushL(parityField);
	parityField->SetRecordId(modemId);
	parityField->LoadL(*db);
	parity = *parityField;
	CleanupStack::PopAndDestroy(parityField);
	
	// Accessing the HandShake from the ModemBearer Table in the record number 
	//corresponding to modemId
	CMDBField<TUint32>* handshakeField = new(ELeave) CMDBField<TUint32>(KCDTIdHandshaking);
	CleanupStack::PushL(handshakeField);
	handshakeField->SetRecordId(modemId);
	handshakeField->LoadL(*db);
	handshake = *handshakeField;
	CleanupStack::PopAndDestroy(handshakeField);

	config().iRate		  = (TBps)rate;
	config().iDataBits    = (TDataBits)dataBits;
	config().iStopBits    = (TStopBits)stopBits;
	config().iParity      = (TParity)parity;
	config().iHandshake   = (TUint)handshake;
	config().iParityError = KConfigParityErrorIgnore;
	config().iFifo        = static_cast<TUint>(EFifoEnable);

	newCfg.Copy(config);

	ret = iCommPort.Open(port);
	if (ret)
		{
		_LOG_L1C3("** Failed CommPort.Open(%d) ret=%d **", port,ret);
		User::Leave(ret);
		}

	// Configure the port
	len = iCommPort.ReceiveBufferLength();
	
	if (len < KErrNone)
		{
		_LOG_L1C2("** Leave: CommPort.ReceiveBufferLength() len=%d **", len);
		User::Leave(KErrGeneral);
		}

	// Increase the receive buffer to make sure we can operate as slowly as we 
	// can without filling up the LDD's receive buffer with incoming signals 
	len *= KCSY_ReceiveBufferSizeIncreaseFactor; 
	ret = iCommPort.SetReceiveBufferLength(len);

	TCommConfig cbuf;
	TCommConfigV01 &cfg = cbuf();
	iCommPort.Config(cbuf);					// Get the Configuration Parameters

	cfg.iRate        = newCfg().iRate;
	cfg.iDataBits    = newCfg().iDataBits;
	cfg.iStopBits    = newCfg().iStopBits;
	cfg.iParity      = newCfg().iParity;
	cfg.iHandshake   = newCfg().iHandshake;
	cfg.iParityError = newCfg().iParityError;
	cfg.iFifo        = newCfg().iFifo;

	_LOG_L4C2("  Set Phys Port Config for Port %d", port);
	_LOG_L4C2("    Rate = 0x%x", cfg.iRate);
	_LOG_L4C2("    DataBits = 0x%x", cfg.iDataBits);
	_LOG_L4C2("    Stop Bits = 0x%x", cfg.iStopBits);
	_LOG_L4C2("    Parity = 0x%x", cfg.iParity);
	_LOG_L4C2("    Handshake = 0x%x", cfg.iHandshake);
	_LOG_L4C2("    Parity Error = 0x%x", cfg.iParityError);
	_LOG_L4C2("    Fifo = 0x%x", cfg.iFifo);

	ret = iCommPort.SetConfig(cbuf);		// Set the Configuration Parameters
	if (ret)
		{
		_LOG_L1C2("** CommPort.SetConfig ret=%d **", ret);
		User::Leave(KErrNotSupported);
		}

	// Clean up the stack
	CleanupStack::PopAndDestroy(db);

	_LOG_L4C2("<<CPortFactory::ConfigurePhysicalCommPortL [ret=%d]", ret);
	return ret;
	}

void CPortFactory::ConnectControlChannel()
/**
 * This method is called to connect the 27.010 control channel,
 * which is DLC 0.
 */
	{
	_LOG_L4C1("CPortFactory::ConnectControlChannel");

	TInt ret = 	iChannelCtrl->Connect();	
	if (ret != KErrNone)
		{
		_LOG_L4C2(("<<CCPortFactory::ConnectControlChannel ret=%d"),ret);
		}
	}

void CPortFactory::DisconnectControlChannel()
/**
 * This method is called to set the 27.010 control channel to the "disconnect" state.
 */
	{
	_LOG_L4C1("CPortFactory::DisconnectAllChannels");
	iChannelCtrl->Disconnect();
	}

void CPortFactory::ChannelCtrlDoCancel()
/**
 * This method is called when the port has been opened and the max number of retries has been reached.
 */
	{
	_LOG_L4C1(">>CPortFactory::ChannelCtrlDoCancel");
	
	iOpenPortFailed = ETrue;
	iChannelCtrl->NotifyChannelReady();

	_LOG_L4C1("<<CPortFactory::ChannelCtrlDoCancel");
	}
	

void CPortFactory::RemoveC32Port(CPortC32InterfaceBase* aPort)
/**
 * This method removes the specified C32 port object from the
 * port factory's list of current/open C32 port objects.
 * This will allow another client to open that port.
 * @param aPort - Pointer to C32 port object
 * @return void
 */
	{
	_LOG_L4C2(">>CPortFactory::RemoveC32Port [aPort=%d]", aPort->GetPortNumber());

	CPortC32InterfaceBase* port = NULL;
	iPortC32InterfaceIter.SetToFirst();
	while ((port = iPortC32InterfaceIter++) != NULL)
		{
		if (port == aPort)
			{
			iPortC32InterfaceList.Remove(*port);
			_LOG_L4C1("<<CPortFactory::RemoveC32Port - okay");
			return;
			}
		}

	// MAF this is a bad condition
	_LOG_L4C1("<<CPortFactory::RemoveC32Port ** Not Removed? **");
	}

void CPortFactory::ConnectIpNifPort(const TUint8 aDlcNum)
/**
 * This method connects the port interface object
 * in the CSY that are used for a data interface to a NIF.
 * @param void
 */
	{
	_LOG_L4C2("CPortFactory::ConnectIpNifPort [aDlcNum=%d]",aDlcNum);

	CPortC32InterfaceBase* port = FindPortC32Interface(aDlcNum);

	if (port)
		{
		CChannelMgrCmdData* iMuxChannel = port->GetMuxChannel();
		if (iMuxChannel)
			{
			
			TInt err = iMuxChannel->Connect();
			if (err != KErrNone)
				{
				_LOG_L1C2("iMuxChannel->Connect() error =%d",err);
				}
			}
		else
			{
			// MAF assert here
			_LOG_L1C2("** Could not find channel for port [0x%x] **",port);
			}
		}
	else
		{
		// MAF assert here
		_LOG_L1C1("** Could not find port for dlcnumber **");
		}
	}

TBool CPortFactory::FindDlcToEnable()
/**
 * We have resources again. Find a dlc that has had its flow control
 * set and re-enable.
 *
 * @return ETrue if there are still dlcs that are stopped due to flow control.
 */
	{
	_LOG_L4C1(">>CPortFactory::FindDlcToEnable");

	TInt numberOfStopped = 0;
	TBool dlcsStillToEnable = EFalse;

    CChannelMgrCmdData* channel = NULL;

	iDataChannelIter.SetToFirst();
	while ((channel = iDataChannelIter++) != NULL)
		{
		if (channel->CsyToModemFlowControl() == EFlowControlOn)
			{
			numberOfStopped++;
			if (numberOfStopped == 1)
				{
				_LOG_L4C1("Found a dlc to re-enable");
				if (channel->SetCsyToModemFlowControl(EFlowControlOff))
					{
					_LOG_L4C1("Re-enable failed");
					dlcsStillToEnable = ETrue;
					break;
					}
				}
			else
				{
				_LOG_L4C1("Other dlc still to re-enable");
				dlcsStillToEnable = ETrue;
				}
			}
		}

	_LOG_L4C2("<<CPortFactory::FindDlcToEnable numberOfStopped=%d",numberOfStopped);
	return dlcsStillToEnable;
	}

void CPortFactory::FindActiveDataDlcToStop()
/**
 * MAF come up with a better algorithm. Currently this scans sequentially for a data
 * DLC to stop.
 * Check the data dlcs to see if one of them can be stopped
 */
	{
	_LOG_L4C1(">>CPortFactory::FindActiveDataDlcToStop");

    CChannelMgrCmdData* channel = NULL;
	CPortC32InterfaceBase* port = NULL;

	iDataChannelIter.SetToFirst();
	while ((channel = iDataChannelIter++) != NULL)
		{
		port = channel->GetC32Port();
		if ((port)&&(port->GetClientType() == EC32ClientIpNif))
			{
			if (channel->CsyToModemFlowControl() == EFlowControlOff)
				{
				_LOG_L4C1("Found a data dlc to stop");
				if (channel->SetCsyToModemFlowControl(EFlowControlOn))
					{
					_LOG_L4C1("Failed to stop dlc");
					}
				else
					break;
				}
			}
		}

	_LOG_L4C1("<<CPortFactory::FindActiveDataDlcToStop");
	}

void CPortFactory::StopAnyDlc()
/**
 * Time for drastic action - enforce flow control on any DLC (except ctrl).
 */
	{
	_LOG_L4C1(">>CPortFactory::StopAnyDlc");

    CChannelMgrCmdData* channel = NULL;

	iDataChannelIter.SetToFirst();
	while ((channel = iDataChannelIter++) != NULL)
		{
		if (channel->CsyToModemFlowControl() == EFlowControlOff)
			{
			_LOG_L4C1("Found a dlc to stop");
			if (channel->SetCsyToModemFlowControl(EFlowControlOn))
				{
				_LOG_L4C1("Failed to stop dlc");
				}
			else
				break;
			}
		}

	_LOG_L4C1("<<CPortFactory::StopAnyDlc");
	}

TBool CPortFactory::DecrementNumOfOpenPorts()
/**
 * Used to decrement the number of open ports and disconnect if number = 0
 *
 * aReturn returns EFalse if Num of Ports is 0; ETrue otherwise
 */
{
	if (iNumOfOpenPorts==0)
		return EFalse;
	else if (--iNumOfOpenPorts == 0)
		{
		_LOG_L4C1("******* DISCONNECT ALL *********");
		DisconnectControlChannel();
		iLastDlcNum = 0;
		return EFalse;
		}	
	return ETrue;
}


TSecurityPolicy CPortFactory::PortPlatSecCapability(TUint /*aPort*/) const
/**
 * This method returns the capabilities required to open the given port number
 * This 27.010 CSY requires the same capabilities for all port numbers
 *
 * @param aPort - The port number to check against
 * @return TSecurityPolicy - containing the required capabilities to use this CSY
 */
{
	return TSecurityPolicy(ECapabilityNetworkServices, ECapabilityNetworkControl, ECapabilityReadUserData, ECapabilityWriteUserData);
}

