/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file Sockets.TestSteps.h
*/
#if (!defined SOCKETS_TESTSTEPS_H)
#define SOCKETS_TESTSTEPS_H

#include <test/testexecutestepbase.h>
#include "Te_EsockStepBase.h"
#include "Connections.TestSteps.h"

// General constants
//------------------
_LIT(KTe_SocketName,"SocketName");
_LIT(KTe_Protocol,"Protocol");
_LIT(KTe_ProtocolName,"ProtocolName");
_LIT(KTe_SourceAddressName,"SrcAddr");
_LIT(KTe_SourcePortName,"SrcPort");
_LIT(KTe_SocketType,"SocketType");
_LIT(KTe_SocketDatagram,"SockDatagram");
_LIT(KTe_SocketStream,"SockStream");
_LIT(KTe_DestinationAddressName,"DstAddr");
_LIT(KTe_DestinationPortName,"DstPort");
_LIT(KTe_PacketSizeName,"PacketSize");
_LIT(KTe_NrOfPacketsName,"NrOfPackets");
_LIT(KTe_ShutdownTypeName,"ShutdownType");
_LIT(KTe_BearerName,"Bearer");
_LIT(KTe_IAPIndexName,"IAP");
_LIT(KTe_SNAPIndexName,"SNAP");
_LIT(KTe_ConnPrefListIndexName,"CONNPREFLIST");
_LIT(KTe_AsynchIndexName,"Asynch");
_LIT(KTe_DummyPrefIndexName,"DUMMYPREF");
_LIT(KTe_AutoStartPrefName,"AUTOPREF");

_LIT(KTe_ShutdownNormal,"ENormal");
_LIT(KTe_ShutdownStopInput,"EStopInput");
_LIT(KTe_ShutdownStopOutput,"EStopOutput");
_LIT(KTe_ShutdownImmediate,"EImmediate");

// Protocol types
_LIT(KTe_TCPName,"TCP");
_LIT(KTe_UDPName,"UDP");
_LIT(KTe_DummyNetworkProtocolName,"DummyNetworkProtocol");
_LIT(KTe_DummyProtocolName,"DummyProtocol");


static const TInt KDummyNetworkProtocol = 253;
static const TInt KDummyProtocol = 254;

_LIT(KSocketDataCheckTransformType, "SocketDataCheckTransformType");

/**
Class implementing ConnectRSocketStep

@internalComponent
*/
class CConnectRSocketStep : public CTe_EsockStepBase
	{
public:
	CConnectRSocketStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TBool iCancelFlag;	
    TRSocketParams iParams; //current params (.ini)
	};

_LIT(KConnectRSocketStep,"ConnectRSocketStep");

/**
Class implementing BindRSocketStep

@internalComponent
*/
class CBindRSocketStep : public CTe_EsockStepBase
    {
public:
    CBindRSocketStep(CCEsockTestBase*& aEsockTest);
    TVerdict doSingleTestStep();
    TInt ConfigureFromIni();

private:
    TRSocketParams iParams; //current params (.ini)
    };

_LIT(KBindRSocketStep,"BindRSocketStep");


/**
Class implementing AddRSocketToRSubConnectionOOMStep

@internalComponent
*/

class CAddRSocketToRSubConnectionOOMStep : public CTe_EsockStepBase
	{
public:
	CAddRSocketToRSubConnectionOOMStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();
	TInt AddSocketToSubConnectionOOM();

private:
	TRSubConnectionParams iParams; //current params (.ini)
	};

_LIT(KAddRSocketToRSubConnectionOOMStep,"AddRSocketToRSubConnectionOOMStep");


/**
Class implementing AddRSocketToRSubConnectionStep

@internalComponent
*/
class CAddRSocketToRSubConnectionStep : public CTe_EsockStepBase
	{
public:
	CAddRSocketToRSubConnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TRSubConnectionParams iParams; //current params (.ini)
	};

_LIT(KAddRSocketToRSubConnectionStep,"AddRSocketToRSubConnectionStep");


/**
Class implementing CAddRSocketToRSubConNoWaitStep

@internalComponent
*/
class CAddRSocketToRSubConNoWaitStep : public CTe_EsockStepBase
    {
public:
    CAddRSocketToRSubConNoWaitStep(CCEsockTestBase*& aEsockTest);
    TVerdict doSingleTestStep();
    TInt ConfigureFromIni();

private:
    TRSubConnectionParams iParams; //current params (.ini)
    };

_LIT(KAddRSocketToRSubConNoWaitStep,"AddRSocketToRSubConNoWaitStep");


/**
Class implementing creatersocketStep

@internalComponent
*/
class CCreateRSocketStep : public CTe_EsockStepBase
	{
public:
	CCreateRSocketStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TRSocketParams iParams; //current params (.ini)
	};


_LIT(KCreateRSocketStep,"creatersocketStep");


/**
Class implementing ShutdownRSocketStep

@internalComponent
*/
class CShutdownRSocketStep : public CTe_EsockStepBase
	{
public:
	CShutdownRSocketStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TRSocketParams iParams; //current params (.ini)
	};

_LIT(KShutdownRSocketStep,"ShutdownRSocketStep");


/**
Class implementing CCloseRSocketStep

@internalComponent
*/
class CCloseRSocketStep : public CTe_EsockStepBase
	{
public:
	CCloseRSocketStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TPtrC iSocketName;
	};

_LIT(KCloseRSocketStep,"CloseRSocketStep");


/**
Class implementing OpenRSocketStep

@internalComponent
*/
class COpenRSocketStep : public CTe_EsockStepBase
	{
public:
	COpenRSocketStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TRSocketParams iParams; //current params (.ini)
	};

_LIT(KOpenRSocketStep,"OpenRSocketStep");


/**
Class implementing CConfirmSocketDataTransferStep

@internalComponent
*/
class CConfirmSocketDataTransferStep : public CTe_EsockStepBase
	{
public:
	CConfirmSocketDataTransferStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TSocketDataCheckParams iParams; //current params (.ini)
	};

_LIT(KConfirmSocketDataTransferStep,"ConfirmSocketDataTransferStep");
_LIT(KErrorExpected,"ErrorExpected");


/**
Class implementing sendreceiversocketStep

@internalComponent
*/
class CSendReceiveRSocketStep : public CTe_EsockStepBase
	{
public:
	CSendReceiveRSocketStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TRSocketParams iParams; //current params (.ini)
	};

_LIT(KSendReceiveRSocketStep,"SendReceiveRSocketStep");


/**
Class implementing MultipleAddRSubConnectionStep

@internalComponent
*/
class CMultipleAddRSubConnectionStep : public CTe_EsockStepBase
	{
public:
	CMultipleAddRSubConnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TRSubConnectionParams iParams; //current params (.ini)
	};

_LIT(KMultipleAddRSubConnectionStep,"MultipleAddRSubConnectionStep");


/**
Class implementing RemoveRSocketFromRSubConnectionOOMStep

@internalComponent
*/
class CRemoveRSocketFromRSubConnectionOOMStep : public CTe_EsockStepBase
	{
public:
	CRemoveRSocketFromRSubConnectionOOMStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();
	TInt RemoveSocketFromSubConnectionOOM();

private:
	TRSubConnectionParams iParams; //current params (.ini)
	};

_LIT(KRemoveRSocketFromRSubConnectionOOMStep,"RemoveRSocketFromRSubConnectionOOMStep");


/**
Class implementing RemoveRSocketFromRSubConnectionStep

@internalComponent
*/
class CRemoveRSocketFromRSubConnectionStep : public CTe_EsockStepBase
	{
public:
	CRemoveRSocketFromRSubConnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
	TRSubConnectionParams iParams; //current params (.ini)
	};

_LIT(KRemoveRSocketFromRSubConnectionStep,"RemoveRSocketFromRSubConnectionStep");


/**
Class implementing moveConnectedSocketToSubconnectionStep

@internalComponent
*/
class CmoveConnectedSocketToSubconnectionStep : public CTe_EsockStepBase
	{
public:
	CmoveConnectedSocketToSubconnectionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doTestStepPreambleL();
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TInt moveConnectedSocketToSubconnection();

private:
	TRSubConnectionParams iParams; //current params (.ini)
	};

_LIT(KmoveConnectedSocketToSubconnectionStep,"moveConnectedSocketToSubconnectionStep");


/**
Class implementing control and confirmation of RSocket::Ioctl() calls

@internalComponent
*/
class CSendReceiveIoctlStep : public CTe_EsockStepBase
	{
public:
	CSendReceiveIoctlStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TRSocketIoctlParams iParams; //current params (.ini)
	};

_LIT(KSendReceiveIoctlStep, "SendReceiveIoctlStep");


/**
Class implementing control and confirmation of RSocket::Ioctl() calls

@internalComponent
*/
class CSocketGetOptionStep : public CTe_EsockStepBase
	{
public:
	CSocketGetOptionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TRSocketGetOptParams iParams;
	};

_LIT(KSocketGetOptionStep, "SocketGetOptionStep");

/**
Class implementing control and confirmation of RSocket::SetOpt() calls

@internalComponent
*/
class CSocketSetOptionStep : public CTe_EsockStepBase
    {
public:
    CSocketSetOptionStep(CCEsockTestBase*& aEsockTest);
    TVerdict doSingleTestStep();
    TInt ConfigureFromIni();

private:
    TRSocketSetOptParams iParams;
    };

_LIT(KSocketSetOptionStep, "SocketSetOptionStep");

/**
Class implementing confirmation of protocol description retrieved from a socket with:
	RSocket::Info()

@internalComponent
*/
class CSocketProtocolDescriptionStep : public CTe_EsockStepBase
	{
public:
	CSocketProtocolDescriptionStep(CCEsockTestBase*& aEsockTest);
	TVerdict doSingleTestStep();
	TInt ConfigureFromIni();

private:
    TSocketProtocolDescriptionParams iParams;
	};

_LIT(KSocketProtocolDescriptionStep, "SocketProtocolDescriptionStep");


#endif // SOCKETS_TESTSTEPS_H

