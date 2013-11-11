/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "T_DataRSocket.h"
#include "T_ActiveSocketAccept.h"
#include "T_ActiveSocketConnect.h"
#include "T_ActiveSocketIoctl.h"
#include "T_ActiveSocketRead.h"
#include "T_ActiveSocketRecv.h"
#include "T_ActiveSocketRecvFrom.h"
#include "T_ActiveSocketRecvOneOrMore.h"
#include "T_ActiveSocketSend.h"
#include "T_ActiveSocketSendTo.h"
#include "T_ActiveSocketShutdown.h"
#include "T_ActiveSocketWrite.h"

/*@{*/
_LIT(KAccept,						"Accept");
_LIT(KBind,							"Bind");
_LIT(KClose,						"Close");
_LIT(KConnect,						"Connect");
_LIT(KGetOpt,						"GetOpt");
_LIT(KListen,						"Listen");
_LIT(KIoctl,						"Ioctl");
_LIT(KOpen,							"Open");
_LIT(KRead,							"Read");
_LIT(KRecv,							"Recv");
_LIT(KRecvFrom,						"RecvFrom");
_LIT(KRecvOneOrMore,				"RecvOneOrMore");
_LIT(KSend,							"Send");
_LIT(KSendTo,						"SendTo");
_LIT(KSetLocalPort,					"SetLocalPort");
_LIT(KSetOpt,						"SetOpt");
_LIT(KName,							"Name");
_LIT(KTransfer,						"Transfer");
_LIT(KGetDisconnectData,			"GetDisconnectData");
_LIT(KWrite,						"Write");
_LIT(KInfo,							"Info");
_LIT(KLocalName,					"LocalName");
_LIT(KLocalPort,					"LocalPort");
_LIT(KRemoteName,					"RemoteName");
_LIT(KCancelConnect,				"CancelConnect");
_LIT(KCancelAccept,					"CancelAccept");
_LIT(KCancelRead,					"CancelRead");
_LIT(KCancelWrite,					"CancelWrite");
_LIT(KCancelRecv,					"CancelRecv");
_LIT(KCancelSend,					"CancelSend");
_LIT(KCancelIoctl,					"CancelIoctl");
_LIT(KCancelAll,					"CancelAll");
_LIT(KShutdown,						"Shutdown");
_LIT(KConstructor,					"Constructor");
_LIT(KDestructor,					"~");

_LIT(KRSocket,						"socket");
_LIT(KRSocketServ,					"socketserv");
_LIT(KRConnection,					"connection");
_LIT(KRSubConnection,				"subconnection");
_LIT(KIPAddress,					"ipaddress");
_LIT(KPort,							"port");
_LIT(KQSize,						"qsize");
_LIT(KProtocol,						"protocol");
_LIT(KProtocolName,					"protocolname");
_LIT(KSockType,						"socktype");
_LIT(KAddrFamily,					"addrfamily");
_LIT(KData,							"data");
_LIT(KConnectDataIn,				"connectdatain");
_LIT(KConnectDataOut,				"connectdataout");
_LIT(KFlag,							"flag");
_LIT(KGetLength,					"getlength");
_LIT(KGetCheckLength,				"checklength");
_LIT(KOptionName,					"optionname");
_LIT(KOptionLevel,					"optionlevel");
_LIT(KUseDes,						"usedes");
_LIT(KExpectedOption,				"expectedoption");
_LIT(KSetOption,					"setoption");
_LIT(KSetCapabilityOption,			"setcapabilityoption%d");
_LIT(KSocketSource,					"socketsource");
_LIT(KBlank,						"blank");
_LIT(KCommand,						"command");
_LIT(KLevel,						"level");
_LIT(KExpectedName,					"expectedname");
_LIT(KExpectedPort,					"expectedport");
_LIT(KExpectedIpAddress,			"expectedipaddress");
_LIT(KShutdownOption,				"shutdownoption");
_LIT(KDisconnectDataIn,				"disconnectdatain");
_LIT(KDisconnectDataOut,			"disconnectdataout");
_LIT(KReadBufferLength,				"readbufferlength");
_LIT(KSocketName,					"socketname");
_LIT(KFileName,						"filename");
_LIT(KExpectedVersion,				"expectedversion");
_LIT(KExpectedAddrFamily,			"expectedaddrfamily");
_LIT(KExpectedByteOrder,			"expectedbyteorder");
_LIT(KExpectedMessageSize,			"expectedmessagesize");
_LIT(KExpectedNamingServices,		"expectednamingservices");
_LIT(KExpectedProtocolId,			"expectedprotocolid");
_LIT(KExpectedSecurity,				"expectedsecurity");
_LIT(KExpectedSockType,				"expectedsocktype");
_LIT(KUseLocalPort,					"uselocalport");

_LIT(KLocalPortSyntax, 				"[%d]");

const TInt KMaxNoOfCapabilities		= 7;
const TInt KMaxLocalPortDataLength	= 10;
/*@}*/

CT_DataRSocket* CT_DataRSocket::NewL()
	{
	CT_DataRSocket*	ret=new (ELeave) CT_DataRSocket();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_DataRSocket::CT_DataRSocket()
:	CT_DataCommsBase()
,	iSocket(NULL)
,	iSocketNameSet(EFalse)
	{
	}

void CT_DataRSocket::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iFs.ShareProtected();
	}

CT_DataRSocket::~CT_DataRSocket()
	{
	iActiveCommand.ResetAndDestroy();

	delete iSocket;
	iSocket = NULL;

	iFs.Close();
	}

/**
Delegates the commands to function

@internalComponent
@param aCommand Current command
@param aEntry Current ini file command entry
@param aSynchronous Updated if an asynchronous call is used
@return TBool Returns whether the command was executed or not
*/
TBool CT_DataRSocket::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
    TBool	ret=ETrue;
    
	if( aCommand==KAccept )
		{
		DoCmdAcceptL(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KBind )
		{
		DoCmdBind(aSection);
		}
	else if( aCommand==KClose )
		{
		DoCmdClose();
		}
	else if( aCommand==KConnect )
		{
		DoCmdConnectL(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KGetOpt )
		{
		DoCmdGetOpt(aSection);
		}
	else if( aCommand==KListen )
		{
		DoCmdListenL(aSection);
		}
	else if( aCommand==KIoctl )
		{
		DoCmdIoctlL(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KOpen )
		{
		DoCmdOpenL(aSection);
		}
	else if( aCommand==KRead )
		{
		DoCmdReadL(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KRecv )
		{
		DoCmdRecvL(aSection,  aAsyncErrorIndex);
		}
	else if( aCommand==KRecvFrom )
		{
		DoCmdRecvFromL(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KRecvOneOrMore )
		{
		DoCmdRecvOneOrMoreL(aSection,  aAsyncErrorIndex);
		}
	else if( aCommand==KSetLocalPort )
		{
		DoCmdSetLocalPort(aSection);
		}
	else if( aCommand==KSend )
		{
		DoCmdSendL(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KSendTo )
		{
		DoCmdSendToL(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KSetOpt )
		{
		DoCmdSetOpt(aSection);
		}
	else if( aCommand==KName )
		{
		DoCmdName();
		}
	else if( aCommand==KTransfer )
		{
		DoCmdTransferL(aSection);
		}
	else if( aCommand==KGetDisconnectData )
		{
		DoCmdGetDisconnectData();
		}
	else if( aCommand==KWrite )
		{
		DoCmdWriteL(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KInfo )
		{
		DoCmdInfo(aSection);
		}
	else if( aCommand==KLocalName )
		{
		DoCmdLocalName(aSection);
		}
	else if( aCommand==KLocalPort )
		{
		DoCmdLocalPort(aSection);
		}
	else if( aCommand==KRemoteName )
		{
		DoCmdRemoteName(aSection);
		}
	else if( aCommand==KCancelConnect )
		{
		DoCmdCancelConnect();
		}
	else if( aCommand==KCancelAccept )
		{
		DoCmdCancelAccept();
		}
	else if( aCommand==KCancelRead )
		{
		DoCmdCancelRead();
		}
	else if( aCommand==KCancelWrite )
		{
		DoCmdCancelWrite();
		}
	else if( aCommand==KCancelRecv )
		{
		DoCmdCancelRecv();
		}
	else if( aCommand==KCancelSend )
		{
		DoCmdCancelSend();
		}
	else if( aCommand==KCancelIoctl )
		{
		DoCmdCancelIoctl();
		}
	else if( aCommand==KCancelAll )
		{
		DoCmdCancelAll();
		}
	else if( aCommand==KShutdown )
		{
		DoCmdShutdownL(aSection, aAsyncErrorIndex);
		}
	else if( aCommand==KConstructor )
		{
		DoCmdConstructorL();
		}
	else if( aCommand==KDestructor )
		{
		DoCmdDestructor();
		}
	else
		{
		ret=EFalse;
		}

	return ret;
	}

TAny* CT_DataRSocket::GetObject()
	{
	return iSocket;
	}


/**
Test code for RSocket Constructor

Ini file options:
	none

@internalComponent
*/
void CT_DataRSocket::DoCmdConstructorL()
	{
	delete iSocket;
	iSocket = NULL;

	INFO_PRINTF1(_L("Calling RSocket Constructor"));
	TRAPD(err, iSocket = new (ELeave) RSocket());
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("RSocket Constructor failed with error %d"), err);
		SetBlockResult(EFail);
		}
	}


/**
Test code for RSocket Destructor

Ini file options:
	none

@internalComponent
*/
void CT_DataRSocket::DoCmdDestructor()
	{
	if( iSocket )
		{
		INFO_PRINTF1(_L("Calling RSocket Destructor"));
		delete iSocket;
		iSocket = NULL;
		}
	else
		{
		ERR_PRINTF1(_L("RSocket object invalid, can't call destructor"));
		SetBlockResult(EFail);
		}
	}


/**
Test code for RSocket::Close

Ini file options:
	none

@internalComponent
*/
void CT_DataRSocket::DoCmdClose()
	{
	INFO_PRINTF1(_L("Calling RSocket::Close()"));
	iSocket->Close();
	}

/**
Test code for RSocket::Open

Ini file options:
	socketserv - an RSocketServ dictionary entry
	connection - an RConnection dictionary entry
	subconnection - an RSubConnection dictionary entry
	protocolname - a protocol name to use
	protocol - a protocol to use
	socktype - a sock type to use
	addressfamily - address family to use

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdOpenL(const TDesC& aSection)
	{
	TBool								err = KErrNone;

	TPtrC	socketServName;
	if( GetStringFromConfig(aSection, KRSocketServ(), socketServName) )
		{
		RSocketServ*	socketServ = static_cast<RSocketServ*>(GetDataObjectL(socketServName));

		if( socketServ!=NULL )
			{
			TPtrC protocolName;

			if( GetStringFromConfig(aSection, KProtocolName(), protocolName) )
				{
				INFO_PRINTF3(_L("Calling RSocket::Open(%S, %S)"), &socketServName, &protocolName);
				err = iSocket->Open(*socketServ, protocolName);
				}
			else
				{
				TBool	useBlankOpen = EFalse;
				GetBoolFromConfig(aSection, KBlank(), useBlankOpen);

				TUint	sockType = 0;
				TUint	protocol = 0;
				TUint	addrFamily = 0;
				TPtrC	connectionName;
				TBool	hasConnectionName = EFalse;
				TBool	hasSubConnectionName = EFalse;

				if( !useBlankOpen )
					{
					useBlankOpen = ETrue;

					TPtrC	sockTypeStr;

					if( GetStringFromConfig(aSection, KSockType(), sockTypeStr) )
						{
						useBlankOpen = EFalse;
						if( GetSockType( sockTypeStr, sockType ) )
							{
							INFO_PRINTF3(_L("RSocket::Open() - Using sockType %S (%d)"), &sockTypeStr, sockType);
							}
						else
							{
							TInt	sockTypeInt;
							if( GetIntFromConfig(aSection, KSockType(), sockTypeInt) )
								{
								sockType = sockTypeInt;
								INFO_PRINTF2(_L("RSocket::Open() - Using sockType %d"), sockType);
								}
							else
								{
								ERR_PRINTF2(_L("RSocket::Open() - Given sockType (%S) is invalid"), &sockTypeStr);
								SetBlockResult(EFail);
								}
							}
						}

					TPtrC	protocolStr;

					if( GetStringFromConfig(aSection, KProtocol(), protocolStr) )
						{
						useBlankOpen = EFalse;
						if( GetProtocol( protocolStr, protocol ) )
							{
							INFO_PRINTF3(_L("RSocket::Open() - Using protocol %S (%d)"), &protocolStr, protocol);
							}
						else
							{
							TInt	protocolInt;
							if( GetIntFromConfig(aSection, KProtocol(), protocolInt) )
								{
								protocol = protocolInt;
								INFO_PRINTF2(_L("RSocket::Open() - Using protocol %d"), protocol);
								}
							else
								{
								ERR_PRINTF2(_L("RSocket::Open() - Given protocol (%S) is invalid"), &protocolStr);
								SetBlockResult(EFail);
								}
							}
						}

					TPtrC	addrFamilyStr;

					if( GetStringFromConfig(aSection, KAddrFamily(), addrFamilyStr) )
						{
						useBlankOpen = EFalse;
						if( GetAddrFamily( addrFamilyStr, addrFamily ) )
							{
							INFO_PRINTF3(_L("RSocket::Open() - Using addrFamily %S (%d)"), &addrFamilyStr, addrFamily);
							}
						else
							{
							TInt	addrFamilyInt;
							if( GetIntFromConfig(aSection, KAddrFamily(), addrFamilyInt) )
								{
								addrFamily = addrFamilyInt;
								INFO_PRINTF2(_L("RSocket::Open() - Using addrFamily %d"), addrFamily);
								}
							else
								{
								ERR_PRINTF2(_L("RSocket::Open() - Given addrFamily (%S) is invalid"), &addrFamilyStr);
								SetBlockResult(EFail);
								}
							}
						}

					hasConnectionName = GetStringFromConfig(aSection, KRConnection(), connectionName);

					hasSubConnectionName = GetStringFromConfig(aSection, KRSubConnection(), connectionName);

					if (hasConnectionName && hasSubConnectionName)
						{
						ERR_PRINTF1(_L("RSocket::Open() - Specified both Conection and SubConnection - may only specify one"));
						SetBlockResult(EFail);
						}
					}

				if( useBlankOpen )
					{
					INFO_PRINTF2(_L("Calling RSocket::Open(%S)"), &socketServName);
					err = iSocket->Open(*socketServ);
					}
				else if( hasConnectionName )
					{
					RConnection*	connection = static_cast<RConnection*>(GetDataObjectL(connectionName));
					if( connection!=NULL )
						{
						INFO_PRINTF6(_L("Calling RSocket::Open(%S, %d, %d, %d, %S)"), &socketServName, addrFamily, sockType, protocol, &connectionName);
						err = iSocket->Open(*socketServ, addrFamily, sockType, protocol, *connection);
						}
					else
						{
						ERR_PRINTF1(_L("RSocket::Open() - RConnection is NULL"));
						SetBlockResult(EFail);
						}
					}
				else if( hasSubConnectionName )
					{
					RSubConnection*	subConnection = static_cast<RSubConnection*>(GetDataObjectL(connectionName));
					if( subConnection!=NULL )
						{
						INFO_PRINTF6(_L("Calling RSocket::Open(%S, %d, %d, %d, %S)"), &socketServName, addrFamily, sockType, protocol, &connectionName);
						err = iSocket->Open(*socketServ, addrFamily, sockType, protocol, *subConnection);
						}
					else
						{
						ERR_PRINTF1(_L("RSocket::Open() - RSubConnection is NULL"));
						SetBlockResult(EFail);
						}
					}
				else
					{
					INFO_PRINTF5(_L("Calling RSocket::Open(%S, %d, %d, %d)"), &socketServName, addrFamily, sockType, protocol);
					err = iSocket->Open(*socketServ, addrFamily, sockType, protocol);
					}
				}
			}
		else
			{
			ERR_PRINTF1(_L("RSocket::Open() - RSocketServ is NULL"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF1(_L("RSocket::Open() No SocketServ name"));
		SetBlockResult(EFail);
		}

	if( err != KErrNone)
		{
		ERR_PRINTF2(_L("RSocket::Open() returned error %d"), err);
		SetError(err);
		}
	}

/**
Test code for RSocket::SetLocalPort

Ini file options:
	port - a port number

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdSetLocalPort(const TDesC& aSection)
	{

	TInt	port = 0;
	GetIntFromConfig(aSection, KPort(), port);
	INFO_PRINTF2(_L("Calling RSocket::SetLocalPort(%d)"), port);
	TInt	err = iSocket->SetLocalPort(port);

	if( err!=KErrNone )
		{
		ERR_PRINTF3(_L("RSocket::SetLocalPort(%d) failed with error %d"), port, err);
		SetError(err);
		}
	}

/**
Test code for RSocket::Listen

Ini file options:
	qsize - the queue size
	connectdatain - the connect data to send

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdListenL(const TDesC& aSection)
	{
	TInt								err = KErrNone;

	TInt	queueSize = 0;
	GetIntFromConfig(aSection, KQSize(), queueSize);

	TPtrC	connectDataIn;
	if( GetStringFromConfig(aSection, KConnectDataIn(), connectDataIn) )
		{
		HBufC8*	connectDataIn8 = HBufC8::NewLC(connectDataIn.Length());
		TPtr8	connectDataIn8Ptr(connectDataIn8->Des());
		connectDataIn8Ptr.Copy(connectDataIn);
		INFO_PRINTF3(_L("Calling RSocket::Listen(%d, %S)"), queueSize, &connectDataIn);
		err = iSocket->Listen(queueSize, connectDataIn8Ptr);
		CleanupStack::PopAndDestroy(connectDataIn8);
		}
	else
		{
		INFO_PRINTF2(_L("Calling RSocket::Listen(%d)"), queueSize);
		err = iSocket->Listen(queueSize);
		}

	if( err!=KErrNone )
		{
		ERR_PRINTF2(_L("RSocket::Listen() failed with error %d"), err);
		SetError(err);
		}
	}

/**
Test code for RSocket::Ioctl

Ini file options:
	command - the Ioctl command
	level - the Ioctl level
	flag - flags passed

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdIoctlL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	// Increase the size of iActiveCommand
	CActiveSocketIoctl*	activeIoctl = CActiveSocketIoctl::NewL(*iSocket, *this);
	TInt				err = iActiveCommand.Append(activeIoctl);
	if( err!=KErrNone )
		{
		delete activeIoctl;
		ERR_PRINTF2(_L("Creating CActiveCallback failed with error %d"), err);
		SetBlockResult(EFail);
		}
	else
		{

		// Get the command
		TInt	command = 0;
		TPtrC	commandName;
		if (GetStringFromConfig(aSection, KCommand(), commandName))
			{
			if( GetIntValue(commandName, command) )
				{
				INFO_PRINTF3(_L("RSocket::Ioctl() - Using command %S (%d)" ), &commandName, command);
				}
			else if( GetIntFromConfig(aSection, KCommand(), command) )
				{
				INFO_PRINTF2(_L("RSocket::Ioctl() - Using command %d"), command);
				}
			else
				{
				ERR_PRINTF2(_L("RSocket::Ioctl() - Given command (%S) is invalid"), &commandName);
				SetBlockResult(EFail);
				}
			}

		// Get the level
		TInt	level = KLevelUnspecified;
		TPtrC	levelName;
		if (GetStringFromConfig(aSection, KLevel(), levelName))
			{
			if( GetIntValue(levelName, level) )
				{
				INFO_PRINTF3(_L("RSocket::Ioctl() - Using level %S (%d)" ), &levelName, level);
				}
			else if( GetIntFromConfig(aSection, KLevel(), level) )
				{
				INFO_PRINTF2(_L("RSocket::Ioctl() - Using level %d"), level);
				}
			else
				{
				ERR_PRINTF2(_L("RSocket::Ioctl() - Given level (%S) is invalid"), &levelName);
				SetBlockResult(EFail);
				}
			}

		// Get the flags
		TUint	ioctlFlags=GetFlags(aSection, KFlag());

		INFO_PRINTF4(_L("Calling RSocket::Ioctl(%S, %d, %S)"), &commandName, ioctlFlags, &levelName);
		activeIoctl->ExecuteL(command, ioctlFlags, level, aAsyncErrorIndex);
		IncOutstanding();
		}
	}

/**
Test code for RSocket::Accept

Ini file options:
	socket - an RSocketServ dictionary entry
	connectdataout - the connect data to receive

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdAcceptL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{

	TPtrC	socketName;
	if( GetStringFromConfig(aSection, KRSocket(), socketName) )
		{
		RSocket*	blankSocket = static_cast<RSocket*>(GetDataObjectL(socketName));
		if( blankSocket!=NULL )
			{
			// Increase the size of iActivesAccept
			CActiveSocketAccept*	activeAccept = CActiveSocketAccept::NewL(*iSocket, *this);
			TInt					err = iActiveCommand.Append(activeAccept);
			if( err!=KErrNone )
				{
				delete activeAccept;
				ERR_PRINTF2(_L("Creating CActiveCallback failed with error %d"), err);
				SetBlockResult(EFail);
				}
			else
				{
				TInt	readBufferLength=KMaxBufferLength;
				GetIntFromConfig(aSection, KReadBufferLength(), readBufferLength);

				TPtrC	connectDataOut;
				if( GetStringFromConfig(aSection, KConnectDataOut(), connectDataOut) )
					{
					INFO_PRINTF3(_L("Calling RSocket::Accept(%S, %S)"), &socketName, &connectDataOut);
					activeAccept->ExecuteL(blankSocket, &connectDataOut, readBufferLength, aAsyncErrorIndex);
					}
				else
					{
					INFO_PRINTF2(_L("Calling RSocket::Accept(%S)"), &socketName);
					activeAccept->ExecuteL(blankSocket, aAsyncErrorIndex);
					}
				IncOutstanding();
				}
			}
		else
			{
			ERR_PRINTF2(_L("RSocket::Accept() - RSocket %S is NULL"), &socketName);
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF1(_L("RSocket::Accept() No Socket name"));
		SetBlockResult(EFail);
		}
	}

/**
Test code for RSocket::Connect

Ini file options:
	ipaddress - the ipaddress to connect to
	port - the port number to connect to
	connectdatain - the connect data to send
	connectdataout - the connect data to receive

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdConnectL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	// Increase the size of iActivesConnect
	CActiveSocketConnect*	activeConnect = CActiveSocketConnect::NewL(*iSocket, *this);
	TInt					err = iActiveCommand.Append(activeConnect);
	if( err!=KErrNone )
		{
		delete activeConnect;
		ERR_PRINTF2(_L("Creating CActiveCallback failed with error %d"), err);
		SetBlockResult(EFail);
		}
	else
		{
		TPtrC	ipaddress = _L("");
		if( GetIPAddress(aSection, KIPAddress(), ipaddress) )
			{

			TInt	port = 0;
			GetIntFromConfig(aSection, KPort(), port);

			TInetAddr	destination(port);
			destination.Input( ipaddress );

			TPtrC	connectDataIn(_L(""));
			TBool	hasConnectDataIn = GetStringFromConfig(aSection, KConnectDataIn(), connectDataIn);

			TPtrC	connectDataOut(_L(""));
			TBool	hasConnectDataOut = GetStringFromConfig(aSection, KConnectDataOut(), connectDataOut);

			TInt	readBufferLength=KMaxBufferLength;
			GetIntFromConfig(aSection, KReadBufferLength(), readBufferLength);

			if( hasConnectDataIn || hasConnectDataOut )
				{
				INFO_PRINTF5(_L("Calling RSocket::Connect(%S:%d, %S, %S)"), &ipaddress, port, &connectDataOut, &connectDataIn);
				activeConnect->ExecuteL(destination, connectDataIn, hasConnectDataOut ? &connectDataOut : NULL, readBufferLength, aAsyncErrorIndex);
				}
			else
				{
				INFO_PRINTF3(_L("Calling RSocket::Connect(%S:%d)"), &ipaddress, port);
				activeConnect->ExecuteL(destination, aAsyncErrorIndex);
				}
			IncOutstanding();
			}
		else
			{
			ERR_PRINTF1(_L("RSocket::Connect() No IP address"));
			SetBlockResult(EFail);
			}
		}
	}

/**
Test code for RSocket::Bind

Ini file options:
	ipaddress - the ipaddress to bind to
	port - the port number to bind to

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdBind(const TDesC& aSection)
	{

	TPtrC	ipaddress = _L("");
	TBool	addressGiven = GetIPAddress(aSection, KIPAddress(), ipaddress);

	TInt	port = 0;
	TBool	portGiven = GetIntFromConfig(aSection, KPort(), port);

	TInetAddr	addr;

	if( addressGiven )
		{
		addr.Input(ipaddress);
		}
	else
		{
		addr.SetAddress(KInetAddrAny);
		}

	if( portGiven )
		{
		addr.SetPort(port);
		}

	TBuf<KMaxIPAddressLength>	tempaddr(_L(""));
	addr.Output(tempaddr);

	INFO_PRINTF3(_L("Calling RSocket::Bind(%S:%d)"), &tempaddr, addr.Port());
	TInt err = iSocket->Bind(addr);

	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("RSocket::Bind Error = %d"), err);
		SetError(err);
		}
	}

/**
Test code for RSocket::Send

Ini file options:
	data - data to send
	flag - flags passed through to protocol
	getlength - use overload to return amount of data actually sent

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdSendL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	// Increase the size of iActivesSend
	CActiveSocketSend*	activeSend = CActiveSocketSend::NewL(*iSocket, *this);
	TInt				err = iActiveCommand.Append(activeSend);
	if( err!=KErrNone )
		{
		delete activeSend;
		ERR_PRINTF2(_L("Creating CActiveCallback failed with error %d"), err);
		SetBlockResult(EFail);
		}
	else
		{

		HBufC8*	data=GetTextDataL(aSection, KFileName, KData);

		INFO_PRINTF2(_L("RSocket::Send() length of data = %d"), data->Length());

		TUint	flags = GetFlags(aSection, KFlag());

		TBool	useSendLength = EFalse;
		GetBoolFromConfig(aSection, KGetLength(), useSendLength);

		if( useSendLength )
			{
			// Should we check the length
			TBool	checkSendLength = ETrue;
			GetBoolFromConfig(aSection, KGetCheckLength(), checkSendLength);

			INFO_PRINTF2(_L("Calling RSocket::Send(data, %d, &aLen)"), flags);
			activeSend->ExecuteL(data, flags, checkSendLength, aAsyncErrorIndex);
			}
		else
			{
			INFO_PRINTF2(_L("Calling RSocket::Send(data, %d)"), flags);
			activeSend->ExecuteL(data, flags, aAsyncErrorIndex);
			}

		IncOutstanding();
		}
	}

/**
Test code for RSocket::SendTo

Ini file options:
	data - data to send
	ipaddress - address to send to
	port - port to use
	flag - flags passed through to protocol
	getlength - use overload to return amount of data actually sent

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdSendToL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	// Increase the size of iActivesSendTo
	CActiveSocketSendTo*	activeSendTo = CActiveSocketSendTo::NewL(*iSocket, *this);
	TInt					err = iActiveCommand.Append(activeSendTo);

	if( err!=KErrNone )
		{
		delete activeSendTo;
		ERR_PRINTF2(_L("Creating CActiveCallback failed with error %d"), err);
		SetBlockResult(EFail);
		}
	else
		{
		TPtrC	ipaddress = _L("");
		if( GetIPAddress(aSection, KIPAddress(), ipaddress) )
			{

			TInt	port = 0;
			GetIntFromConfig(aSection, KPort(), port);

			TInetAddr	destination(port);
			destination.Input( ipaddress );

			HBufC8*	data=GetTextDataL(aSection, KFileName, KData);

			TUint	flags = GetFlags(aSection, KFlag());

			TBool	useSendToLength = EFalse;
			GetBoolFromConfig(aSection, KGetLength(), useSendToLength);

			if( useSendToLength )
				{
				// Should we check the length
				TBool	checkSendToLength = ETrue;
				GetBoolFromConfig(aSection, KGetCheckLength(), checkSendToLength);
				INFO_PRINTF4(_L("Calling RSocket::SendTo(data, %S:%d, %d, &aLen)"), &ipaddress, port, flags);
				activeSendTo->ExecuteL(data, destination, flags, checkSendToLength, aAsyncErrorIndex);
				}
			else
				{
				INFO_PRINTF4(_L("Calling RSocket::SendTo(data, %S:%d, %d)"), &ipaddress, port, flags);
				activeSendTo->ExecuteL(data, destination, flags, aAsyncErrorIndex);
				}
			IncOutstanding();
			}
		else
			{
			ERR_PRINTF1(_L("RSocket::SendTo() No IP address"));
			SetBlockResult(EFail);
			}
		}
	}

/**
Test code for RSocket::Name

Ini file options:

@internalComponent
@param aEntry Current ini file command entry
*/

void CT_DataRSocket::DoCmdName()
	{
	INFO_PRINTF1(_L("Calling RSocket::Name()"));

	TInt	err = iSocket->Name(iSocketName);
	INFO_PRINTF2(_L("RSocket::Name() returned %S"), &iSocketName);
	if( KErrNone != err )
		{
		ERR_PRINTF2(_L("RSocket::Name() Error = %d"), err);
		SetError(err);
		iSocketNameSet = EFalse;
		}
	else
		{
		iSocketNameSet = ETrue;
		}
	}

/**
Test code for RSocket::Transfer

Ini file options:
	socketserv - the RSocketServ to pass
	socketsource - the source RSocket

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdTransferL(const TDesC& aSection)
	{
	TName	socketName;
	TPtrC	givenSocketName = _L("");
	if (!GetStringFromConfig(aSection, KSocketName(), givenSocketName) )
		{
		TPtrC	socketSourceName;
		if ( GetStringFromConfig(aSection, KSocketSource(), socketSourceName) )
			{
			CT_DataRSocket*	sourceSocket = (CT_DataRSocket*)(GetDataWrapperL(socketSourceName));
			if (sourceSocket)
				{
				// Get the name of the source socket
				if( sourceSocket->GetSocketName(socketName) )
					{
					INFO_PRINTF3(_L("RSocket::Transfer() Using socket name [%S] from %S"), &socketName, &socketSourceName);
					}
				else
					{
					ERR_PRINTF2(_L("RSocket::Transfer() Could not get socket name from %S"), &socketSourceName);
					SetBlockResult(EFail);
					}
				}
			else
				{
				ERR_PRINTF2(_L("RSocket::Transfer() - %S is NULL"), &socketSourceName);
				SetBlockResult(EFail);
				}
			}
		else
			{
			ERR_PRINTF1(_L("RSocket::Transfer() socket source or name must be specified"));
			SetBlockResult(EFail);
			}
		}
	else
		{
		socketName = givenSocketName;
		INFO_PRINTF2(_L("RSocket::Transfer() Using socket name [%S]"), &socketName);
		}
	TPtrC	socketServName;
	if ( GetStringFromConfig(aSection, KRSocketServ(), socketServName) )
		{
		RSocketServ*	socketServ = static_cast<RSocketServ*>(GetDataObjectL(socketServName));
		if (socketServ)
			{
			// Call transfer
			INFO_PRINTF3(_L("Calling RSocket::Transfer(%S, %S)"), &socketServName, &socketName);
			TInt	err = iSocket->Transfer(*socketServ, socketName);
			if( KErrNone != err )
				{
				ERR_PRINTF2(_L("RSocket::Transfer() error = %d"), err);
				SetError(err);
				}
			}
		else
			{
			ERR_PRINTF2(_L("RSocket::Transfer() - %S is NULL"), &socketServName);
			SetBlockResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF1(_L("RSocket::Transfer() socketserv not specified"));
		SetBlockResult(EFail);
		}
	}


/**
Test code for RSocket::GetDisconnectData

Ini file options:

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdGetDisconnectData()
	{
	INFO_PRINTF1(_L("Calling RSocket::GetDisconnectData()"));

	TBuf8<KMaxConnectDataLength> 	disconnectData;
	TInt	err = iSocket->GetDisconnectData(disconnectData);
	if (KErrNone == err )
		{
		TBuf<KMaxConnectDataLength> 	disconnectData16;
		disconnectData16.Copy(disconnectData);
		INFO_PRINTF2(_L("RSocket::GetDisconnectData() returned %S"), &disconnectData16);
		}
	else
		{
		ERR_PRINTF2(_L("RSocket::GetDisconnectData(). Error = %d"), err);
		SetError(err);
		}
	}

/**
Test code for RSocket::Write

Ini file options:
	data - the data to write
	expectederror - the expected error value when the RunL is called (defaults to KErrNone)

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdWriteL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	// Increase the size of iActivesWrite
	CActiveSocketWrite*	activeWrite = CActiveSocketWrite::NewL(*iSocket, *this);
	TInt				err = iActiveCommand.Append(activeWrite);
	if( err!=KErrNone )
		{
		delete activeWrite;
		ERR_PRINTF2(_L("Creating CActiveCallback failed with error %d"), err);
		SetBlockResult(EFail);
		}
	else
		{
		HBufC8*	data=GetTextDataL(aSection, KFileName, KData);

		INFO_PRINTF2(_L("RSocket::Write() length of data = %d"), data->Length());

		INFO_PRINTF1(_L("Calling RSocket::Write(data)"));
		activeWrite->ExecuteL(data, aAsyncErrorIndex);
		IncOutstanding();
		}
	}

/**
Test code for RSocket::Info

Ini file options:
	expectedname - the expected protocol name

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdInfo(const TDesC& aSection)
	{

	TProtocolDesc	info;
	INFO_PRINTF1(_L("Calling RSocket::Info()"));
	TInt	err = iSocket->Info(info);
	if (KErrNone != err)
		{
		ERR_PRINTF2(_L("RSocket::Info(). Error = %d"), err);
		SetError(err);
		}
	else
		{
		// Only continue if the err was KErrNone
		// If we were expecting a different error - just end the test
		PrintProtocol(_L("RSocket::Info()"), info);

		// Check the protocol info is what was expected
	 	TPtrC					expectedString;
	 	TInt					expectedInt;

		if (GetStringFromConfig(aSection, KExpectedName(), expectedString))
			{
			if (info.iName != expectedString)
				{
				ERR_PRINTF3(_L("RSocket::Info(). Protocol Name %S, expected name %S"), &info.iName, &expectedString);
				SetBlockResult(EFail);
				}
			}

		if (GetStringFromConfig(aSection, KExpectedVersion(), expectedString))
			{
			TBuf<KMaxVersionName>	versionName(info.iVersion.Name());
			if (versionName != expectedString)
				{
				ERR_PRINTF3(_L("RSocket::Info(). Version %S, expected version %S"), &versionName, &expectedString);
				SetBlockResult(EFail);
				}
			}

		if (GetIntFromConfig(aSection, KExpectedAddrFamily(), expectedInt))
			{
			if ( ((TInt)info.iAddrFamily) != expectedInt )
				{
				ERR_PRINTF3(_L("RSocket::Info(). Protocol addr family %d, expected addr family %d"), info.iAddrFamily, expectedInt);
				SetBlockResult(EFail);
				}
			}

		if (GetIntFromConfig(aSection, KExpectedByteOrder(), expectedInt))
			{
			if (info.iByteOrder != expectedInt)
				{
				ERR_PRINTF3(_L("RSocket::Info(). Protocol byte order %d, expected byte order %d"), info.iByteOrder, expectedInt);
				SetBlockResult(EFail);
				}
			}

		if (GetIntFromConfig(aSection, KExpectedMessageSize(), expectedInt))
			{
			if (info.iMessageSize != expectedInt)
				{
				ERR_PRINTF3(_L("RSocket::Info(). Protocol Message Size %d, expected Message Size %d"), info.iMessageSize, expectedInt);
				SetBlockResult(EFail);
				}
			}

		if (GetIntFromConfig(aSection, KExpectedNamingServices(), expectedInt))
			{
			if ( ((TInt)info.iNamingServices) != expectedInt )
				{
				ERR_PRINTF3(_L("RSocket::Info(). Protocol Naming Services %d, expected Naming Services %d"), info.iNamingServices, expectedInt);
				SetBlockResult(EFail);
				}
			}

		if (GetIntFromConfig(aSection, KExpectedProtocolId(), expectedInt))
			{
			if ( ((TInt)info.iProtocol) != expectedInt )
				{
				ERR_PRINTF3(_L("RSocket::Info(). Protocol id %d, expected Protocol id %d"), info.iProtocol, expectedInt);
				SetBlockResult(EFail);
				}
			}

		if (GetIntFromConfig(aSection, KExpectedSecurity(), expectedInt))
			{
			if ( ((TInt)info.iSecurity) != expectedInt )
				{
				ERR_PRINTF3(_L("RSocket::Info(). Protocol Security %d, expected Security %d"), info.iSecurity, expectedInt);
				SetBlockResult(EFail);
				}
			}

		if (GetIntFromConfig(aSection, KExpectedSockType(), expectedInt))
			{
			if ( ((TInt)info.iSockType) != expectedInt )
				{
				ERR_PRINTF3(_L("RSocket::Info(). Protocol Sock Type %d, expected Sock Type %d"), info.iSockType, expectedInt);
				SetBlockResult(EFail);
				}
			}
		}
	}

/**
Test code for RSocket::LocalName

Ini file options:
	expectedipaddress - the expected ip address
	expectedname - the expected returned value

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdLocalName(const TDesC& aSection)
	{

	TInetAddr	socketAddress;
	INFO_PRINTF1(_L("Calling RSocket::LocalName()"));
	iSocket->LocalName(socketAddress);

	TBuf<KMaxIPAddressLength>	ipaddr;
	socketAddress.Output(ipaddr);
	INFO_PRINTF4(_L("RSocket::LocalName() returned family %d, port %d, ipaddr %S"), socketAddress.Family(), socketAddress.Port(), &ipaddr);

	TInt	expectedPort;
	if (GetIntFromConfig(aSection, KExpectedPort(), expectedPort))
		{
		if ( expectedPort != ((TInt)socketAddress.Port()) )
			{
			ERR_PRINTF3(_L("RSocket::LocalName() returned port %d, expected %d"), socketAddress.Port(), expectedPort);
			SetBlockResult(EFail);
			}
		}

	TPtrC	expectedIpAddress = _L("");
	if( GetIPAddress(aSection, KExpectedIpAddress(), expectedIpAddress) )
		{
		if (expectedIpAddress != ipaddr)
			{
			ERR_PRINTF3(_L("RSocket::LocalName() returned ipaddr %S, expected %S"), &ipaddr, &expectedIpAddress);
			SetBlockResult(EFail);
			}
		}
	}

/**
Test code for RSocket::RemoteName

Ini file options:
	expectedipaddress - the expected ip address
	expectedname - the expected returned value

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdRemoteName(const TDesC& aSection)
	{

	TInetAddr	socketAddress;
	INFO_PRINTF1(_L("Calling RSocket::RemoteName()"));
	iSocket->RemoteName(socketAddress);

	TBuf<KMaxIPAddressLength>	ipaddr;
	socketAddress.Output(ipaddr);
	INFO_PRINTF4(_L("RSocket::RemoteName() returned family %d, port %d, ipaddr %S"), socketAddress.Family(), socketAddress.Port(), &ipaddr);

	TInt	expectedPort = 0;
	if (GetIntFromConfig(aSection, KExpectedPort(), expectedPort))
		{
		if ( expectedPort != ((TInt)socketAddress.Port()) )
			{
			ERR_PRINTF3(_L("RSocket::RemoteName() returned port %d, expected %d"), socketAddress.Port(), expectedPort);
			SetBlockResult(EFail);
			}
		}

	TPtrC	expectedIpAddress = _L("");
	if( GetIPAddress(aSection, KExpectedIpAddress(), expectedIpAddress) )
		{
		if (expectedIpAddress != ipaddr)
			{
			ERR_PRINTF3(_L("RSocket::RemoteName() returned ipaddr %S, expected %S"), &ipaddr, &expectedIpAddress);
			SetBlockResult(EFail);
			}
		}
	}

/**
Test code for RSocket::CancelConnect

Ini file options:

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdCancelConnect()
	{
	INFO_PRINTF1(_L("Calling RSocket::CancelConnect()"));
	iSocket->CancelConnect();
	}

/**
Test code for RSocket::CancelAccept

Ini file options:

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdCancelAccept()
	{
	INFO_PRINTF1(_L("Calling RSocket::CancelAccept()"));
	iSocket->CancelAccept();
	}

/**
Test code for RSocket::CancelWrite

Ini file options:

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdCancelWrite()
	{
	INFO_PRINTF1(_L("Calling RSocket::CancelWrite()"));
	iSocket->CancelWrite();
	}

/**
Test code for RSocket::CancelRead

Ini file options:

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdCancelRead()
	{
	INFO_PRINTF1(_L("Calling RSocket::CancelRead()"));
	iSocket->CancelRead();
	}

/**
Test code for RSocket::CancelRecv

Ini file options:

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdCancelRecv()
	{
	INFO_PRINTF1(_L("Calling RSocket::CancelRecv()"));
	iSocket->CancelRecv();
	}

/**
Test code for RSocket::CancelSend

Ini file options:

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdCancelSend()
	{
	INFO_PRINTF1(_L("Calling RSocket::CancelSend()"));
	iSocket->CancelSend();
	}

/**
Test code for RSocket::CancelIoctl

Ini file options:

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdCancelIoctl()
	{
	INFO_PRINTF1(_L("Calling RSocket::CancelIoctl()"));
	iSocket->CancelIoctl();
	}

/**
Test code for RSocket::CancelAll

Ini file options:

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdCancelAll()
	{
	INFO_PRINTF1(_L("Calling RSocket::CancelAll()"));
	iSocket->CancelAll();
	}

/**
Test code for RSocket::LocalPort

Ini file options:
	expectedport - the expected returned value

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdLocalPort(const TDesC& aSection)
	{

	// Call LocalName
	INFO_PRINTF1(_L("Calling RSocket::LocalPort()"));
	TUint port = iSocket->LocalPort();
	INFO_PRINTF2(_L("RSocket::LocalPort() returned port %d"), port);

 	TInt	expectedPort;
	if (GetIntFromConfig(aSection, KExpectedPort(), expectedPort))
		{
		if ( expectedPort != ((TInt)port) )
			{
			INFO_PRINTF3(_L("RSocket::LocalPort() returned %d, expected %d"), port, expectedPort);
			SetBlockResult(EFail);
			}
		}
	}

/**
Test code for RSocket::Read

Ini file options:
	data - expected data to receive

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdReadL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	// Increase the size of iActivesRead
	CActiveSocketRead*	activeRead = CActiveSocketRead::NewL(*iSocket, *this);
	TInt				err = iActiveCommand.Append(activeRead);
	if( err!=KErrNone )
		{
		delete activeRead;
		ERR_PRINTF2(_L("Creating CActiveSocketRead failed with error %d"), err);
		SetBlockResult(EFail);
		}
	else
		{

		HBufC8*	expected=GetTextDataL(aSection, KFileName, KData);
		TInt	readBufferLength=1;
		if ( expected!=NULL )
			{
			readBufferLength=expected->Length();
			}
		if ( readBufferLength==0 )
			{
			GetIntFromConfig(aSection, KReadBufferLength(), readBufferLength);
			}

		INFO_PRINTF2(_L("Calling RSocket::Read(%d)"), readBufferLength);
		activeRead->ExecuteL(expected, readBufferLength, aAsyncErrorIndex);
		IncOutstanding();
		}
	}

/**
Test code for RSocket::Recv

Ini file options:
	data - expected data to receive

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdRecvL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	// Increase the size of iActivesRecv
	CActiveSocketRecv*	activeRecv = CActiveSocketRecv::NewL(*iSocket, *this);
	TInt				err = iActiveCommand.Append(activeRecv);

	if( err!=KErrNone )
		{
		delete activeRecv;
		ERR_PRINTF2(_L("Creating CActiveCallback failed with error %d"), err);
		SetBlockResult(EFail);
		}
	else
		{
		// Get the flag info
		TUint	flags = GetFlags(aSection, KFlag());

		HBufC8*	expected=GetTextDataL(aSection, KFileName, KData);
		TInt	readBufferLength=1;		
		if ( expected!=NULL )
			{
			readBufferLength=expected->Length();
			}
		if ( readBufferLength==0 )
			{
			GetIntFromConfig(aSection, KReadBufferLength(), readBufferLength);
			}

		TBool	useRecvLength = EFalse;
		GetBoolFromConfig(aSection, KGetLength(), useRecvLength);

		if ( useRecvLength )
			{
			// Should we check the length
			TBool	checkRecvLength = ETrue;
			GetBoolFromConfig(aSection, KGetCheckLength(), checkRecvLength);

			INFO_PRINTF2(_L("Calling RSocket::Recv(%d, &aLen)"), flags);
			activeRecv->ExecuteL(expected, flags, checkRecvLength, readBufferLength, aAsyncErrorIndex);
			}
		else
			{
			INFO_PRINTF2(_L("Calling RSocket::Recv(%d)"), flags);
			activeRecv->ExecuteL(expected, flags, readBufferLength, aAsyncErrorIndex);
			}
		IncOutstanding();
		}
	}

/**
Test code for RSocket::RecvOneOrMore

Ini file options:
	data - expected data to receive

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdRecvOneOrMoreL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	// Increase the size of iActivesRecvOneOrMore
	CActiveSocketRecvOneOrMore*	activeRecvOneOrMore = CActiveSocketRecvOneOrMore::NewL(*iSocket, *this);
	TInt						err = iActiveCommand.Append(activeRecvOneOrMore);
	if( err!=KErrNone )
		{
		delete activeRecvOneOrMore;
		ERR_PRINTF2(_L("Creating CActiveCallback failed with error %d"), err);
		SetBlockResult(EFail);
		}
	else
		{

		// Get the flag info
		TUint	flags = GetFlags(aSection, KFlag());

		HBufC8*	expected=GetTextDataL(aSection, KFileName, KData);

		TInt	readBufferLength=0;
		if ( expected!=NULL )
			{
			readBufferLength=expected->Length();
			}
		if ( readBufferLength==0 )
			{
			readBufferLength=1;
			GetIntFromConfig(aSection, KReadBufferLength(), readBufferLength);
			}

		// Should we check the length
		TBool	checkRecvOneOrMoreLength = ETrue;
		GetBoolFromConfig(aSection, KGetCheckLength(), checkRecvOneOrMoreLength);

		activeRecvOneOrMore->ExecuteL(expected, flags, checkRecvOneOrMoreLength, readBufferLength, aAsyncErrorIndex);
		IncOutstanding();
		}
	}

/**
Test code for RSocket::RecvFrom

Ini file options:
	length - size of data to receive

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdRecvFromL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	
	// Increase the size of iActivesRecvFrom
	CActiveSocketRecvFrom*	activeRecvFrom = CActiveSocketRecvFrom::NewL(*iSocket, *this);
	TInt					err = iActiveCommand.Append(activeRecvFrom);
	if( err!=KErrNone )
		{
		delete activeRecvFrom;
		ERR_PRINTF2(_L("Creating CActiveCallback failed with error %d"), err);
		SetBlockResult(EFail);
		}
	else
		{

		// Get the flag info
		TUint	flags = GetFlags(aSection, KFlag());

		TBool	useRecvFromLength = EFalse;
		GetBoolFromConfig(aSection, KGetLength(), useRecvFromLength);

		HBufC8*	expected=GetTextDataL(aSection, KFileName, KData);

		TInt	readBufferLength=1;
		if ( expected!=NULL )
			{
			readBufferLength=expected->Length();
			}
		if ( readBufferLength==0 )
			{
			GetIntFromConfig(aSection, KReadBufferLength(), readBufferLength);
			}

		// Get expected ip address
		TPtrC	expectedAddress = _L("");
		TBool	hasExpectedAddress = GetIPAddress(aSection, KExpectedIpAddress(), expectedAddress);

		// Get expected port
		TInt	expectedPort = 0;
		TBool	hasExpectedPort = GetIntFromConfig(aSection, KExpectedPort(), expectedPort);

		TInetAddr	expectedAddr;
		// Set the expected IP address and port
		if ( hasExpectedAddress )
			{
			expectedAddr.Input(expectedAddress);
			}
		if ( hasExpectedPort)
			{
			expectedAddr.SetPort(expectedPort);
			}

		if (useRecvFromLength)
			{
			// Should we check the length
			TBool	checkRecvFromLength = ETrue;
			GetBoolFromConfig(aSection, KGetCheckLength(), checkRecvFromLength);

			INFO_PRINTF2(_L("Calling RSocket::RecvFrom(%d, &aLen)"), flags);
			activeRecvFrom->ExecuteL(expected, flags, expectedAddr, hasExpectedAddress, hasExpectedPort, checkRecvFromLength, readBufferLength, aAsyncErrorIndex);
			}
		else
			{
			INFO_PRINTF2(_L("Calling RSocket::RecvFrom(%d)"), flags);
			activeRecvFrom->ExecuteL(expected, flags, expectedAddr, hasExpectedAddress, hasExpectedPort, readBufferLength, aAsyncErrorIndex);
			}
		IncOutstanding();
		}
	}

/**
Test code for RSocket::GetOpt

Ini file options:
	optionname - the name of the option to get
	optionlevel - the level of the option to get
	usedes - use overload to return option in a descriptor form
	expectedoption - the option expected to be returned

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdGetOpt(const TDesC& aSection)
	{
	TInt								err = KErrNone;

	TPtrC	optionName;
	TInt	optionNameValue = 0;
	if( GetStringFromConfig(aSection, KOptionName(), optionName) )
		{
		if( GetIntValue(optionName, optionNameValue) )
			{
			INFO_PRINTF3(_L("RSocket::GetOpt() - Using option name %S (%d)" ), &optionName, optionNameValue);
			}
		else if( GetIntFromConfig(aSection, KOptionName(), optionNameValue) )
			{
			INFO_PRINTF2(_L("RSocket::GetOpt() - Using option name %d"), optionNameValue);
			}
		else
			{
			ERR_PRINTF2(_L("RSocket::GetOpt() - Given option name (%S) is invalid"), &optionName);
			SetBlockResult(EFail);
			}
		}

	TPtrC	optionLevel;
	TInt	optionLevelValue = 0;
	if( GetStringFromConfig(aSection, KOptionLevel(), optionLevel) )
		{
		if( GetIntValue(optionLevel, optionLevelValue) )
			{
			INFO_PRINTF3(_L("RSocket::GetOpt() - Using option level %S (%d)" ), &optionLevel, optionLevelValue);
			}
		else if( GetIntFromConfig(aSection, KOptionLevel(), optionLevelValue) )
			{
			INFO_PRINTF2(_L("RSocket::GetOpt() - Using option level %d"), optionLevelValue);
			}
		else
			{
			ERR_PRINTF2(_L("RSocket::GetOpt() - Given option level (%S) is invalid"), &optionLevel);
			SetBlockResult(EFail);
			}
		}

	TBool	useDes=EFalse;
	GetBoolFromConfig(aSection, KUseDes(), useDes);

	if( useDes )
		{
		TBuf8<KMaxOptionNameLength>	actualOption8(_L8(""));
		INFO_PRINTF3(_L("Calling RSocket::GetOpt(%d, %d, TDes8)"), optionNameValue, optionLevelValue);
		err = iSocket->GetOpt(optionNameValue, optionLevelValue, actualOption8);
		if( err != KErrNone )
			{
			ERR_PRINTF2(_L("RSocket::GetOpt() return error %d" ), err);
			SetError(err);
			}
		else
			{
			TBuf<KMaxOptionNameLength>	actualOption;
			actualOption.Copy(actualOption8);
			INFO_PRINTF4(_L("RSocket::GetOpt(%d, %d, TDes8) = %S" ), optionNameValue, optionLevelValue, &actualOption);

			TPtrC	expectedOption;
			if( GetStringFromConfig(aSection, KExpectedOption(), expectedOption))
				{
				if( expectedOption!=actualOption )
					{
					ERR_PRINTF3(_L("RSocket::GetOpt() Expected option (%S) != Actual option (%S)"), &expectedOption, &actualOption);
					SetBlockResult(EFail);
					}
				}
			}
		}
	else
		{
		TInt	actualOption;
		INFO_PRINTF3(_L("Calling RSocket::GetOpt(%d, %d, TInt)"), optionNameValue, optionLevelValue);
		err = iSocket->GetOpt(optionNameValue, optionLevelValue, actualOption);
		INFO_PRINTF2(_L("Option = %d"), actualOption);
		if( err != KErrNone )
			{
			ERR_PRINTF2(_L("RSocket::GetOpt() return error %d" ), err);
			SetError(err);
			}
		else
			{
			TInt	expectedIntOption;
			if( GetIntFromConfig(aSection, KExpectedOption(), expectedIntOption))
				{
				if( expectedIntOption!=actualOption )
					{
					ERR_PRINTF1(_L("RSocket::GetOpt() Expected option != Actual option"));
					SetBlockResult(EFail);
					}
				}
			else
				{
				TBool	expectedBoolOption;
				if( GetBoolFromConfig(aSection, KExpectedOption(), expectedBoolOption))
					{
					TBool	actualBoolOption = (actualOption != 0);

					if( expectedBoolOption!=actualBoolOption )
						{
						ERR_PRINTF1(_L("RSocket::GetOpt() Expected option != Actual option"));
						SetBlockResult(EFail);
						}
					}
				}
			}
		}
	}

/**
Test code for RSocket::SetOpt

Ini file options:
	optionname - the name of the option to set
	optionlevel - the level of the option to set
	usedes - use overload to set option in a descriptor form
	setoption - the option to set

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdSetOpt(const TDesC& aSection)
	{
	TInt								err = KErrNone;

	TPtrC	optionName;
	TInt	optionNameValue = 0;
	if( GetStringFromConfig(aSection, KOptionName(), optionName) )
		{
		if( GetIntValue(optionName, optionNameValue) )
			{
			INFO_PRINTF3(_L("RSocket::SetOpt() - Using option name %S (%d)" ), &optionName, optionNameValue);
			}
		else if( GetIntFromConfig(aSection, KOptionName(), optionNameValue) )
			{
			INFO_PRINTF2(_L("RSocket::SetOpt() - Using option name %d"), optionNameValue);
			}
		else
			{
			ERR_PRINTF2(_L("RSocket::SetOpt() - Given option name (%S) is invalid"), &optionName);
			SetBlockResult(EFail);
			}
		}

	TPtrC	optionLevel;
	TInt	optionLevelValue = 0;
	if( GetStringFromConfig(aSection, KOptionLevel(), optionLevel) )
		{
		if( GetIntValue(optionLevel, optionLevelValue) )
			{
			INFO_PRINTF3(_L("RSocket::SetOpt() - Using option level %S (%d)" ), &optionLevel, optionLevelValue);
			}
		else if( GetIntFromConfig(aSection, KOptionLevel(), optionLevelValue) )
			{
			INFO_PRINTF2(_L("RSocket::SetOpt() - Using option level %d"), optionLevelValue);
			}
		else
			{
			ERR_PRINTF2(_L("RSocket::SetOpt() - Given option level (%S) is invalid"), &optionLevel);
			SetBlockResult(EFail);
			}
		}

	// Get capability options
	TBool				foundCapability = EFalse;
	TInt				noOfCapabilities = 0;
	RArray<TCapability>	capabilities;

	for( TInt index=1; index<=KMaxNoOfCapabilities; index++ )
		{
		TCapability	capability = ECapability_None;
		TPtrC		capabilityName;
		
		TBuf<KMaxTestExecuteCommandLength>	tempStore;
		
		tempStore.Format(KSetCapabilityOption(), index);
		if ( GetStringFromConfig(aSection, tempStore, capabilityName) )
			{
			foundCapability = GetCapability(capabilityName, capability);
			if( foundCapability )
				{
				noOfCapabilities++;
				INFO_PRINTF2(_L("RSocket::SetOpt() Using capability %S"), &capabilityName);
				}
			else
				{
				ERR_PRINTF2(_L("RSocket::SetOpt() Unknown capability %S"), &capabilityName);
				SetBlockResult(EFail);
				}
			}
		capabilities.Append(capability);
		}

	if( noOfCapabilities > 0 )
		{
		TSecurityPolicy procPolicy(capabilities[0], capabilities[1], capabilities[2], capabilities[3], capabilities[4], capabilities[5], capabilities[6]);
		INFO_PRINTF3(_L("Calling RSocket::SetOpt(%d, %d, Capabilities)"), optionNameValue, optionLevelValue);
		err = iSocket->SetOpt(optionNameValue, optionLevelValue, procPolicy.Package());
		}
	else
		{
		TBool	useDes=EFalse;
		GetBoolFromConfig(aSection, KUseDes(), useDes);

		if( useDes )
			{
			TPtrC	desOption(_L(""));
			if (GetStringFromConfig(aSection, KSetOption(), desOption))
				{
				TBuf8<KMaxOptionNameLength>	desOption8(_L8(""));
				desOption8.Copy(desOption);
				INFO_PRINTF4(_L("Calling RSocket::SetOpt(%d, %d, %S)"), optionNameValue, optionLevelValue, &desOption);
				err = iSocket->SetOpt(optionNameValue, optionLevelValue, desOption8);
				}
			else
				{
				INFO_PRINTF3(_L("Calling RSocket::SetOpt(%d, %d)"), optionNameValue, optionLevelValue);
				err = iSocket->SetOpt(optionNameValue, optionLevelValue);
				}
			}
		else
			{
			TInt	intOption = 0;
			if (GetIntFromConfig(aSection, KSetOption(), intOption))
				{
				INFO_PRINTF4(_L("Calling RSocket::SetOpt(%d, %d, %d)"), optionNameValue, optionLevelValue, intOption);
				err = iSocket->SetOpt(optionNameValue, optionLevelValue, intOption);
				}
			else
				{
				INFO_PRINTF3(_L("Calling RSocket::SetOpt(%d, %d)"), optionNameValue, optionLevelValue);
				err = iSocket->SetOpt(optionNameValue, optionLevelValue);
				}
			}
		}

	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("RSocket::SetOpt() return error %d" ), err);
		SetError(err);
		}
	}

/**
Test code for RSocket::Shutdown

Ini file options:
	shutdownoption - the shutdown option to use
	disconnectdatain - the disconnect data to send
	disconnectdataout - the disconnect data to receive

@internalComponent
@param aEntry Current ini file command entry
*/
void CT_DataRSocket::DoCmdShutdownL(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	CActiveSocketShutdown*	activeShutdown = CActiveSocketShutdown::NewL(*iSocket, *this);
	TInt					err = iActiveCommand.Append(activeShutdown);
	if( err!=KErrNone )
		{
		delete activeShutdown;
		ERR_PRINTF2(_L("Creating CActiveCallback failed with error %d"), err);
		SetBlockResult(EFail);
		}
	else
		{

		RSocket::TShutdown	shutdownOption = RSocket::ENormal;
		TPtrC				shutdownOptionStr;
		if( GetStringFromConfig(aSection, KShutdownOption(), shutdownOptionStr) )
			{
			TInt	shutdownOptionInt = 0;
			if( GetTShutdown(shutdownOptionStr, shutdownOption) )
				{
				INFO_PRINTF2(_L("RSocket::Shutdown() Using shutdown option %S"), &shutdownOptionStr);
				}
			else if( GetIntFromConfig(aSection, KShutdownOption(), shutdownOptionInt) )
				{
				INFO_PRINTF2(_L("RSocket::Shutdown() - Using shutdown option %d"), shutdownOptionInt);
				shutdownOption = (RSocket::TShutdown) shutdownOptionInt;
				}
			else
				{
				ERR_PRINTF2(_L("RSocket::Shutdown() Unknown shutdown option %S"), &shutdownOptionStr);
				SetBlockResult(EFail);
				}
			}

		TPtrC	disconnectDataIn(_L(""));
		TBool	hasDisconnectDataIn = GetStringFromConfig(aSection, KDisconnectDataIn(), disconnectDataIn);

		TPtrC	disconnectDataOut(_L(""));
		TBool	hasDisconnectDataOut = GetStringFromConfig(aSection, KDisconnectDataOut(), disconnectDataOut);

		TInt	readBufferLength=KMaxBufferLength;
		GetIntFromConfig(aSection, KReadBufferLength(), readBufferLength);

		if( hasDisconnectDataIn || hasDisconnectDataOut )
			{
			INFO_PRINTF4(_L("Calling RSocket::Shutdown(%d, %S, %S)"), shutdownOption, &disconnectDataOut, &disconnectDataIn);
			activeShutdown->ExecuteL(shutdownOption, disconnectDataOut, hasDisconnectDataIn ? &disconnectDataIn : NULL, readBufferLength, aAsyncErrorIndex);
			}
		else
			{
			INFO_PRINTF2(_L("Calling RSocket::Shutdown(%d)"), shutdownOption);
			activeShutdown->ExecuteL(shutdownOption, aAsyncErrorIndex);
			}
		IncOutstanding();
		}
	}


TBool CT_DataRSocket::GetIPAddress(const TDesC& aSection, const TDesC& aIpAddrKeyName, TPtrC& ipAddress)
	{
	return GetStringFromConfig(aSection, aIpAddrKeyName, ipAddress);
	}

TBool CT_DataRSocket::GetSocketName(TName& aSocketName)
	{
	if (iSocketNameSet)
		{
		aSocketName = iSocketName;
		}
	return iSocketNameSet;
	}

HBufC8* CT_DataRSocket::GetTextDataL(const TDesC& aSection, const TDesC& aFileName, const TDesC& aData)
	{
	TInt								lengthOfData=0;

	HBufC8*	ret=NULL;

	TPtrC	filename;
	if( GetStringFromConfig(aSection, aFileName, filename) )
		{
		// Filename is given in INI file, so read in data from that file
		RFile	file;
		User::LeaveIfError(file.Open(iFs, filename, EFileRead | EFileShareAny));
		CleanupClosePushL(file);
		User::LeaveIfError(file.Size(lengthOfData));
		ret=HBufC8::NewLC(lengthOfData);

		TPtr8	ptr(ret->Des());
		User::LeaveIfError(file.Read(ptr));
		CleanupStack::Pop(ret);
		CleanupStack::PopAndDestroy(&file);
		}
	else
		{
		// Read in data from INI file
	 	TPtrC	dataFromIni = _L("");
		GetStringFromConfig(aSection, aData, dataFromIni);
		INFO_PRINTF2(_L("Using data (%S)"), &dataFromIni);

		TBuf<KMaxLocalPortDataLength>	localPortData(_L(""));
		TUint	localPort = GetLocalPortFromSocketL(aSection);
		if( localPort > 0 )
			{
			// Use local port
			localPortData.Format(KLocalPortSyntax, localPort);
			INFO_PRINTF2(_L("Using local port string (%S)"), &localPortData);

			lengthOfData = localPortData.Length() + dataFromIni.Length();
			ret = HBufC8::NewLC(lengthOfData);
			TPtr8	ptr(ret->Des());
			ptr.Copy(localPortData);
			ptr.Append(dataFromIni);
			CleanupStack::Pop(ret);
			}
		else
			{
			ret = HBufC8::NewLC(dataFromIni.Length());
			TPtr8	ptr(ret->Des());
			ptr.Copy(dataFromIni);
			CleanupStack::Pop(ret);
			}
		}

	return ret;
	}

TUint CT_DataRSocket::GetLocalPortFromSocketL(const TDesC& aSection)
	{
	TUint								localPort=0;

	TPtrC	socketName;
	if( GetStringFromConfig(aSection, KUseLocalPort(), socketName) )
		{
		RSocket* socket = static_cast<RSocket*>(GetDataObjectL(socketName));
		if( socket!=NULL )
			{
			localPort = socket->LocalPort();
			INFO_PRINTF3(_L("GetLocalPortFromSocketL(%S) = %d"), &socketName, localPort);
			}
		else
			{
			ERR_PRINTF2(_L("GetLocalPortFromSocketL() - RSocket %S is NULL"), &socketName);
			SetBlockResult(EFail);
			}
		}
	return localPort;
	}

TBool CT_DataRSocket::ConvertToFlag(const TDesC& aFlagStr, TUint& aFlag)
	{
	TBool	ret=ETrue;
	TInt	location=aFlagStr.Match(_L("*|*"));
	if ( location!=KErrNotFound )
		{
		//Converting Left part of the data
		TPtrC		tempStr=aFlagStr.Left(location);
		ret=ConvertToFlag(tempStr, aFlag);

		//Converting right data can be with another "|"
		tempStr.Set(aFlagStr.Mid(location+1));

		TUint	flagTmp;
		if ( ConvertToFlag(tempStr, flagTmp) )
			{
			aFlag |= flagTmp;
			}
		else
			{
			ret = EFalse;
			}
		}
	else
		{
		TInt	flagInt;
		if ( GetIntValue(aFlagStr, flagInt) )
			{
			aFlag=flagInt;
			}
		else
			{
			TLex	lex(aFlagStr);
			if (lex.Val(flagInt)==KErrNone)
				{
				aFlag=flagInt;
				}
			else
				{
				ret=EFalse;
				ERR_PRINTF2(_L("Given flag (%S) is invalid"), &aFlagStr);
				SetBlockResult(EFail);
				}
			}
		}

	return ret;
	}

TUint CT_DataRSocket::GetFlags(const TDesC& aSection, const TDesC& aFlagKeyName)
	{

	TUint	flags = 0;

	TPtrC	flagStr;
	if ( GetStringFromConfig(aSection, aFlagKeyName, flagStr) )
		{
		ConvertToFlag(flagStr, flags);
		}

	return flags;
	}

CActiveSocket* CT_DataRSocket::RemoveActiveTypeFromArray(CActive* aActive)
	{
	CActiveSocket*	ret=NULL;

	// See if it is in iActiveCommand
	for ( TInt i = 0; (i<iActiveCommand.Count()) && (ret==NULL); ++i )
		{
		CActiveCallback*	activeCommand = iActiveCommand[i];
		if( aActive == activeCommand )
			{
			ret=iActiveCommand[i];
			iActiveCommand.Remove(i);
			}
		}

	return ret;
	}

void CT_DataRSocket::RunL(CActive* aActive, TInt aIndex)
	{
	INFO_PRINTF1(_L("Calling RunL"));
	CActiveSocket*	activeFound=RemoveActiveTypeFromArray(aActive);

	if( activeFound==NULL )
		{
 		ERR_PRINTF1(_L("Stray RunL signal"));
		SetBlockResult(EFail);
		}
	else
		{
		TInt	err=aActive->iStatus.Int();
		if( err != KErrNone )
			{
			ERR_PRINTF2(_L("RunL Error = %d"), err);
			SetAsyncError(aIndex, err);
			}
		else
			{
			activeFound->ValidateCompletionL(*this);
			}
		// Reset the outstanding request states
		DecOutstanding();
		delete aActive;
		}
	}

void CT_DataRSocket::DoCancel(CActive* aActive, TInt aIndex)
	{
	INFO_PRINTF1(_L("Calling DoCancel"));
	CActiveSocket*	activeFound=RemoveActiveTypeFromArray(aActive);

	if( activeFound==NULL )
		{
 		ERR_PRINTF1(_L("Stray DoCancel signal"));
 		SetBlockResult(EFail);
		}
	else
 		{
		TInt	err = aActive->iStatus.Int();
		if( err != KErrNone )
			{
			ERR_PRINTF2(_L("DoCancel Error %d"), err);
			SetAsyncError(aIndex, err);
			}

		// Reset the outstanding request state
		DecOutstanding();
		delete aActive;
		}
	}
