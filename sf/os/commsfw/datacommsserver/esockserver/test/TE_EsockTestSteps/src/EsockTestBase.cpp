// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file EsockTestBase.cpp
*/

#include <e32debug.h>
#include "EsockTestBase.h"
#include <e32std.h>
#include <commdbconnpref.h>
#include <cs_subconevents.h>
#include <es_prot.h>
#include <comms-infras/esock_params.h>
#include <dummynif_params.h>

#include <comms-infras/es_parameterbundle.h>
#include <comms-infras/provinfoqueryset.h>
#include "ConnectionServer.TestSteps.h"

#include <dummynifvar.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockTestEsckTst, "ESockTestEsckTst");
#endif

static const TInt KProtocolInetDummy = 253;

// Temporary measure to configure the rconnserv stuff before its backported to 9.4

using namespace ConnectionServ;


namespace ConnectionServ
{
	class RConnectionServ;
	class CConnectionServParameterBundle;
}

class CConnServStatusQuery : public CActive
	{
public:
	CConnServStatusQuery(ConnectionServ::RConnectionServ* aConnServ);
	static CConnServStatusQuery* NewLC(	ConnectionServ::RConnectionServ* aConnServ,
							const ConnectionServ::CConnectionServParameterBundle& aQueryBundle,
							ConnectionServ::CConnectionServParameterBundle& aStatusBundle);
	void ConstructL(const ConnectionServ::CConnectionServParameterBundle& aQueryBundle,
					ConnectionServ::CConnectionServParameterBundle& aStatusBundle);
	void RunL();
	void DoCancel();

	TInt ErrCode() const { return iStatus.Int(); }

public:	
	ConnectionServ::RConnectionServ* iConnServ;
	};


CConnServStatusQuery::CConnServStatusQuery(RConnectionServ* aConnServ)
	: CActive(0), iConnServ(aConnServ)
		{
		}

CConnServStatusQuery* CConnServStatusQuery::NewLC(	ConnectionServ::RConnectionServ* aConnServ,
									const ConnectionServ::CConnectionServParameterBundle& aQueryBundle,
									ConnectionServ::CConnectionServParameterBundle& aStatusBundle)
	{
	CConnServStatusQuery* inst = new CConnServStatusQuery(aConnServ);
	CleanupStack::PushL(inst);
	inst->ConstructL(aQueryBundle, aStatusBundle);
	return inst;
	}

void CConnServStatusQuery::ConstructL(const ConnectionServ::CConnectionServParameterBundle& aQueryBundle,
									ConnectionServ::CConnectionServParameterBundle& aStatusBundle)
	{
	iConnServ->AccessPointStatusQuery(aQueryBundle, aStatusBundle, iStatus );

	// add self to scheduler
	CActiveScheduler::Add(this);
	SetActive();
	}
	

void CConnServStatusQuery::RunL()
	{
	CActiveScheduler::Current()->Stop();
	}

void CConnServStatusQuery::DoCancel()
	{
	iConnServ->CancelAccessPointStatusQuery();
	}
	
	

//
// TMigrateToPreferredCarrierParams - container for parameters for accepting a 
// move for mobility
//

void TMigrateToPreferredCarrierParams::Reset()
	{
	iName.Set(KNullDesC);
	iMobilityApiName.Set(KNullDesC);
	}


//
// TCompareIntegersParams - parameters for comparing to saved integer values
//
void TCompareIntegersParams::Reset()
	{
	iValue1.Set(KNullDesC);
	iValue2.Set(KNullDesC);
    iDifference = 0;
	}

//
//TSocketDataCheckParams - container for parameters for checking data on a socket
void TSocketDataCheckParams::Reset()
	{
	iSocketCheckName.Set(KNullDesC);
    iSocketName.Set(KNullDesC);
    iTransformType = ETransformDataNone;
    iErrorExpected = EFalse;
	}
	
//
//TSocketServerNumProtocolsParams - container for parameters for checking the number of procotols on a socket server instance

void TSocketServerNumProtocolsParams::Reset()
	{
	iSocketServName.Set(KNullDesC);
	iNumberExpected = 0;
	iSavedAsName.Set(KNullDesC);
	iExpected = EFalse;
	iToBeSaved = EFalse;
	}

void TSocketServStartStopProtocolParams::Reset()
	{
	iSocketServName.Set(KNullDesC);
	iAddrFamily = 0;
	iProtocol = 0;
	iSockType = 0;
	}

//
//TRSocketParams - container for parameters (from an .ini file)
//needed for operations on a sockets

void TRSocketParams::Reset()
    {
    iSockServName.Set(KNullDesC);
    iConnectionName.Set(KNullDesC);
    iSocketName.Set(KNullDesC);
    iSubconnectionName.Set(KNullDesC);

    iSocketType = 0;
   	iProtocol = 0;
	iPacketSize = 0;
	iNrOfPackets = 0;
	iLocalIP.FillZ();
   	iRemoteIP.FillZ();
   	iShutdownType = (RSocket::TShutdown)0;

    // Payload colouring
    iPayloadColouringIsEnabled = EFalse;
    iPayloadColour = 0;
    }

void TRSocketGetOptParams::Reset()
	{
    iSocketName.Set(KNullDesC);
	iOptionName = 0;
	iOptionLevel = 0;
	iExpectedOptionOutputText.Set(KNullDesC);
	iTextExpected = EFalse;
	iValueExpected = EFalse;
	}

void TRSocketSetOptParams::Reset()
    {
    iSocketName.Set(KNullDesC);
    iOptionName = 0;
    iOptionLevel = 0;
    iOptionToSetText.Set(KNullDesC);
    iTextSet = EFalse;
    iValueSet = EFalse;
    }

void TRSocketIoctlParams::Reset()
	{
    iSocketName.Set(KNullDesC);
	iIoctlCommand = 0;
	iIoctlLevel = 0;
	iExpectedIoctlOutput.Set(KNullDesC);
	}

TProtocolDescriptionParams::TProtocolDescriptionParams()
	{
	Mem::FillZ(this, sizeof(this));
	}

void TProtocolDescriptionParams::Reset()
	{
	iValuesGiven.Reset();
	}

void TProtocolDescriptionParams::TProtocolDescParamsExpected::Reset()
	{
	iName = EFalse;
	iAddrFamily = EFalse;
	iProtocol = EFalse;
	iSockType = EFalse;
	iVersion = EFalse;
	iByteOrder = EFalse;
	iServiceInfo = EFalse;
	iNamingServices = EFalse;
	iSecurity = EFalse;
	iMessageSize = EFalse;
	}

TBool TProtocolDescriptionParams::MatchesExpected(TProtocolDesc& aProtocolDescription)
	{
	TBool verdict = ETrue;
	
	// Default to pass and then just need to find a single one that does not match
	if(iValuesGiven.iName && (iValues.iName != aProtocolDescription.iName)) verdict = EFalse;
	if(iValuesGiven.iAddrFamily && (iValues.iAddrFamily != aProtocolDescription.iAddrFamily)) verdict = EFalse;
	if(iValuesGiven.iProtocol && (iValues.iProtocol != aProtocolDescription.iProtocol)) verdict = EFalse;
	if(iValuesGiven.iSockType && (iValues.iSockType != aProtocolDescription.iSockType)) verdict = EFalse;
	if(iValuesGiven.iVersion &&
		(
			iValues.iVersion.iMajor != aProtocolDescription.iVersion.iMajor ||
			iValues.iVersion.iMinor != aProtocolDescription.iVersion.iMinor ||
			iValues.iVersion.iBuild != aProtocolDescription.iVersion.iBuild
		))
		verdict = EFalse;
	if(iValuesGiven.iByteOrder && (iValues.iByteOrder != aProtocolDescription.iByteOrder)) verdict = EFalse;
	if(iValuesGiven.iServiceInfo && (iValues.iServiceInfo != aProtocolDescription.iServiceInfo)) verdict = EFalse;
	if(iValuesGiven.iNamingServices && (iValues.iNamingServices != aProtocolDescription.iNamingServices)) verdict = EFalse;
	if(iValuesGiven.iSecurity && (iValues.iSecurity != aProtocolDescription.iSecurity)) verdict = EFalse;
	if(iValuesGiven.iMessageSize && (iValues.iMessageSize != aProtocolDescription.iMessageSize)) verdict = EFalse;

	return verdict;		
	}

_LIT(KName, "Name");
_LIT(KAddrFamily, "AddrFamily");
_LIT(KProtocol, "Protocol");
_LIT(KSockType, "SockType");
_LIT(KVersionMajor, "VersionMajor");
_LIT(KVersionMinor, "VersionMinor");
_LIT(KVersionBuild, "VersionBuild");
_LIT(KByteOrder, "ByteOrder");
_LIT(KServiceInfo, "ServiceInfo");
_LIT(KNamingServices, "NamingServices");
_LIT(KSecurity, "Security");
_LIT(KMessageSize, "MessageSize");

TInt TProtocolDescriptionParams::FetchDescription(TProtocolDescriptionParams& aParams, CTestStep& aTestStep)
	{
    // Protocol name
    TPtrC name;
	if (aTestStep.GetStringFromConfig(aTestStep.ConfigSection(), KName, name) == 1)
        {
		aParams.iValues.iName.Copy(name);
		aParams.iValuesGiven.iName = ETrue;
        }

	// Address family
	if (aTestStep.GetIntFromConfig(aTestStep.ConfigSection(), KAddrFamily, (TInt&)aParams.iValues.iAddrFamily) == 1)
        {
		aParams.iValuesGiven.iAddrFamily = ETrue;
        }

	// Protocol
	if (aTestStep.GetIntFromConfig(aTestStep.ConfigSection(), KProtocol, (TInt&)aParams.iValues.iProtocol) == 1)
        {
		aParams.iValuesGiven.iProtocol = ETrue;
        }

	// Socket type
	if (aTestStep.GetIntFromConfig(aTestStep.ConfigSection(), KSockType, (TInt&)aParams.iValues.iSockType) == 1)
        {
		aParams.iValuesGiven.iSockType = ETrue;
        }

	// Version
	TInt versionMajor;
	if (aTestStep.GetIntFromConfig(aTestStep.ConfigSection(), KVersionMajor, versionMajor) == 1)
        {
		aParams.iValues.iVersion.iMajor = (TInt8)versionMajor;

		TInt versionMinor;
		if (aTestStep.GetIntFromConfig(aTestStep.ConfigSection(), KVersionMinor, versionMinor) == 1)
			{
			aParams.iValues.iVersion.iMinor = (TInt8)versionMinor;

			TInt versionBuild;
			if (aTestStep.GetIntFromConfig(aTestStep.ConfigSection(), KVersionBuild, versionBuild) == 1)
				{
				aParams.iValues.iVersion.iBuild = (TInt16)versionBuild;

				// Got all the version info
				aParams.iValuesGiven.iVersion = ETrue;
				}
			}
        }

	// Byte order
	if (aTestStep.GetIntFromConfig(aTestStep.ConfigSection(), KByteOrder, (TInt&)aParams.iValues.iByteOrder) == 1)
        {
		aParams.iValuesGiven.iByteOrder = ETrue;
        }

	// Service info
	if (aTestStep.GetHexFromConfig(aTestStep.ConfigSection(), KServiceInfo, (TInt&)aParams.iValues.iServiceInfo) == 1)
        {
		aParams.iValuesGiven.iServiceInfo = ETrue;
        }

	// Naming services
	if (aTestStep.GetHexFromConfig(aTestStep.ConfigSection(), KNamingServices, (TInt&)aParams.iValues.iNamingServices) == 1)
        {
		aParams.iValuesGiven.iNamingServices = ETrue;
        }

	// Security
	if (aTestStep.GetHexFromConfig(aTestStep.ConfigSection(), KSecurity, (TInt&)aParams.iValues.iSecurity) == 1)
        {
		aParams.iValuesGiven.iSecurity = ETrue;
        }

	// Message size
	if (aTestStep.GetIntFromConfig(aTestStep.ConfigSection(), KMessageSize, (TInt&)aParams.iValues.iMessageSize) == 1)
        {
		aParams.iValuesGiven.iMessageSize = ETrue;
        }

	return aParams.iValuesGiven.AllExpected() ? KErrNone : KErrNotFound;
	}

TBool TProtocolDescriptionParams::TProtocolDescParamsExpected::NoneExpected()
	{
	return (!iName && !iAddrFamily && !iProtocol && !iSockType && !iVersion && !iByteOrder && !iServiceInfo && !iNamingServices && !iSecurity && !iMessageSize) ?
		ETrue :
		EFalse;
	}
	
TBool TProtocolDescriptionParams::TProtocolDescParamsExpected::AllExpected()
	{
	return (iName && iAddrFamily && iProtocol && iSockType && iVersion && iByteOrder && iServiceInfo && iNamingServices && iSecurity && iMessageSize) ?
		ETrue :
		EFalse;
	}

void TSocketProtocolDescriptionParams::Reset()
	{
	iSocketName.Set(KNullDesC);
	iProtocolDescParams.Reset();
	}

void TSocketServFindProtocolParams::Reset()
	{
	iSocketServName.Set(KNullDesC);
	iProtocolDescParams.Reset();
	}

void TSocketServGetProtocolInfoParams::Reset()
	{
	iSocketServName.Set(KNullDesC);
	iProtocolIndex = 0;
	iProtocolDescParams.Reset();
	}


//
//TRConnectionParams - container for parameters (from an .ini file)
//needed for operations on a connections

void TRConnectionParams::Reset()
    {
    iSockServName.Set(KNullDesC);
    iConnectionName.Set(KNullDesC);
    iAttachToConnectionName.Set(KNullDesC);

	iBearer = 0;
	iIAP = 0;
	iNET = 0;
	iSNAP = 0;

    iConnStopType = (RConnection::TConnStopType)0;
    iConnAttachType = (RConnection::TConnAttachType)0;
    iStartWithOldPreferences = 0;
    iStartWithSnapPreferences = 0;
    iAsynch = EFalse;
    }

//
//TRSubConnectionParams - container for parameters (from an .ini file)
//needed for operations on a subconnections

void TRSubConnectionParams::Reset()
    {
    iSockServName.Set(KNullDesC);
	iConnectionName.Set(KNullDesC);
	iSubConnectionName.Set(KNullDesC);
	iSocketName.Set(KNullDesC);

	iSubConnectionParamsName.Set(KNullDesC);
	iSubConnectionEventName.Set(KNullDesC);

    iSubConnType = (RSubConnection::TSubConnType)0;
    }

//
//TEsockTestEventContainer - encapsulates all necessary RConnection
//or RSubConnection event data so that it can be conveniently stored in a map.

void TEventContainerParams::Reset()
    {
    iEventName.Set(KNullDesC);
	iSubConnectionName.Set(KNullDesC);
	iConnectionName.Set(KNullDesC);
	iAvailabilityExtName.Set(KNullDesC);
	iConnectionManagerExtName.Set(KNullDesC);
	iExtensionName.Set(KNullDesC);
	iMobilityExtName.Set(KNullDesC);
	iAllEvents = EFalse;
	iGenericEvents = EFalse;
	iEventMask = 0;
	iTimeoutMiliSecs = 0;
    }

TEsockTestEventContainer::~TEsockTestEventContainer()
    {
    iEventNotifBuf.Zero();
    }

//
// TEsockTestDataMonitoringEventContainer - encapsulates all necessary data monitoring
// event data so that it can be conveniently stored in a map.    

void TDataMonitorEventParams::Reset()
	{
	iEventName.Copy(KNullDesC);
	iConnectionName.Set(KNullDesC);
	iSubConnectionName.Set(KNullDesC);
	iDataMonitorName.Set(KNullDesC);
	
	iDelta = 0;
	iMinExpectedVolume = 0;
	}

TPtrC& TDataMonitorEventParams::ConnOrSubConnName()
	{
	if(iConnectionName.Length() > 0)
		{
		return iConnectionName;
		}
	else 
		{
		// We return this regardless and consider the default KNullDesC
		// to represent an error
		return iSubConnectionName;
		}
	}

//
//TRSimtsyEventParams - container for parameters (from an .ini file)
void TSimTsyEventParams::Reset()
    {

	iConnectionName.Set(KNullDesC);
	iSimTsyEventName.Set(KNullDesC);
	iEventName.Set(KNullDesC);
	iEventKey=0;
	iEventIndex=0;

    }


//
//CCEsockTestBase

CCEsockTestBase::~CCEsockTestBase()
	{
	CleanAll();
	}

//
//socket server functions library

TInt CCEsockTestBase::CreateSocketServer(const TDesC& aSockServName)
	{
    //check if this secket server hasn't been created already
    if (iSockServs.Find(aSockServName)!=NULL)
    	return KErrAlreadyExists;

    RSocketServ* ss = new RSocketServ;
    if (ss==NULL)
    	return KErrNoMemory;

	TInt error = iSockServs.Add(ss,aSockServName);
	if (error!=KErrNone)
		delete ss;


	return error;
	}

TInt CCEsockTestBase::ConnectSocketServer(const TDesC& aSockServName)
	{
    //check if this secket server hasn't been created already
    RSocketServ* ss = iSockServs.Find(aSockServName);
    if (ss==NULL)
    	return KErrNotFound;

	return ss->Connect();
	}

TInt CCEsockTestBase::CloseSocketServer(const TDesC& aSockServName)
	{
    //check if this secket server hasn't been created already
    RSocketServ* ss = iSockServs.Find(aSockServName);
    if (ss==NULL)
    	return KErrNotFound;

	ss->Close();
	return KErrNone;
	}

TInt CCEsockTestBase::SocketServerNumProtocols(TSocketServerNumProtocolsParams& aParams, TInt& aNumProtocols)
	{
    // Fetch the socket server instance which must already exist
    RSocketServ* ss = iSockServs.Find(aParams.iSocketServName);
    if (ss == NULL)
    	return KErrNotFound;

	TInt error = ss->NumProtocols((TUint&)aNumProtocols);
	if(error != KErrNone)
		{
		return error;
		}
	
	if(aParams.iToBeSaved)
		{
		// If we are saving the value then do so
		// If it exists already then update it
		TInt* currentValue = NULL;
		currentValue = iIntegerValues.Find(aParams.iSavedAsName);
		if(currentValue != NULL)
			{
			*currentValue = aNumProtocols;
			return KErrNone;
			}

		// Else create the new variable
		currentValue = new TInt(aNumProtocols);
		if (currentValue == NULL)
			return KErrNoMemory;

		error = iIntegerValues.Add(currentValue, aParams.iSavedAsName);
		}

	return error;
	}

TInt CCEsockTestBase::SocketServerFindProtocol(TSocketServFindProtocolParams& aParams, TProtocolDesc& aProtocolDescription)
	{
    // Fetch the socket server instance which must already exist
    RSocketServ* ss = iSockServs.Find(aParams.iSocketServName);
    if (ss == NULL)
    	return KErrNotFound;

	return ss->FindProtocol(aParams.iProtocolDescParams.iValues.iName, aProtocolDescription);
	}

TInt CCEsockTestBase::SocketServerGetProtocolInfo(TSocketServGetProtocolInfoParams& aParams, TProtocolDesc& aProtocolDescription)
	{
    // Fetch the socket server instance which must already exist
    RSocketServ* ss = iSockServs.Find(aParams.iSocketServName);
    if (ss == NULL)
    	return KErrNotFound;

	return ss->GetProtocolInfo(aParams.iProtocolIndex, aProtocolDescription);
	}


TInt CCEsockTestBase::SocketServerStartProtocol(TSocketServStartStopProtocolParams& aParams, TRequestStatus& aStatus)
	{
    // Fetch the socket server instance which must already exist
    RSocketServ* ss = iSockServs.Find(aParams.iSocketServName);
    if (ss == NULL)
    	return KErrNotFound;

	// Start the protocol
	ss->StartProtocol(
		aParams.iAddrFamily,
		aParams.iSockType,
		aParams.iProtocol,
		aStatus);
	return KErrNone;	
	}


TInt CCEsockTestBase::SocketServerStopProtocol(TSocketServStartStopProtocolParams& aParams, TRequestStatus& aStatus)
	{
    // Fetch the socket server instance which must already exist
    RSocketServ* ss = iSockServs.Find(aParams.iSocketServName);
    if (ss == NULL)
    	return KErrNotFound;

	// Stop the protocol
	ss->StopProtocol(
		aParams.iAddrFamily,
		aParams.iSockType,
		aParams.iProtocol,
		aStatus);
	return KErrNone;	
	}


TInt CCEsockTestBase::__DbgFailNext(const TDesC& aSockServName, TInt aCount)
	{
	RSocketServ* ss = iSockServs.Find(aSockServName);
    if (ss==NULL)
		return KErrNotFound;

	return ss->__DbgFailNext(aCount);
	}	

TBool CCEsockTestBase::__DbgCheckFailNext(const TDesC& aSockServName)
    {
	RSocketServ* ss = iSockServs.Find(aSockServName);
    if (ss==NULL)
		return KErrNotFound;

	return ss->__DbgCheckFailNext();
    }


//
//connection server functions library
TInt CCEsockTestBase::CreateConnectionServer(const TDesC& aConnectionServName)
	{
    if (iConnectionServs.Find(aConnectionServName) != NULL)
    	return KErrAlreadyExists;

    RConnectionServ* connServ = new RConnectionServ;
    if (connServ == NULL)
    	return KErrNoMemory;

	TInt error = iConnectionServs.Add(connServ, aConnectionServName);
	if (error != KErrNone)
		delete connServ;

	return error;
	}

TInt CCEsockTestBase::ConnectConnectionServer(const TDesC& aConnectionServName, TUint aTierId)
	{
    //check if this secket server hasn't been created already
    RConnectionServ* connServ = iConnectionServs.Find(aConnectionServName);
    if (connServ == NULL)
    	return KErrNotFound;

	return connServ->Connect(aTierId);
	}

TInt CCEsockTestBase::CloseConnectionServer(const TDesC& aConnectionServName)
	{
    // Find the connection server we are referring to
    RConnectionServ* connServ = iConnectionServs.Find(aConnectionServName);
    if (connServ == NULL)
    	return KErrNotFound;

	connServ->Close();
	return KErrNone;
	}

TInt CCEsockTestBase::ConnectionAccessPointStatusQuery(
		const TDesC& aConnectionServName,
		const ConnectionServ::CConnectionServParameterBundle& aQueryBundle,
		ConnectionServ::CConnectionServParameterBundle& aResultBundle)
	{
	RConnectionServ* connServ = iConnectionServs.Find(aConnectionServName);

	if(connServ == NULL)
		return KErrNotFound;
		
	// Need a local active scheduler for the RConnectionServ API
	CActiveScheduler* as = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(as);
	CActiveScheduler::Install(as);

	CConnServStatusQuery* sq = CConnServStatusQuery::NewLC(connServ, aQueryBundle, aResultBundle);

	TRAP_IGNORE(CActiveScheduler::Start())

	TInt res2 = sq->ErrCode();

	CleanupStack::PopAndDestroy(sq);	
	CleanupStack::PopAndDestroy(as);

	return res2;
	}

class CMyTimer : public CTimer
	{
public:
	static CMyTimer* NewL()
		{
		CMyTimer* timer = new CMyTimer();
		if(timer)
			{
			timer->ConstructL();
			CActiveScheduler::Add(timer);
			}
		return timer;
		}
		
	CMyTimer() : CTimer(EPriorityStandard)
		{
		}
	void RunL()
		{
		// Simply stop the scheduler as this will force the cancellation we are after
		CActiveScheduler::Current()->Stop();		
		}
	};

const TInt KHalfASecondInMicroSeconds = 500000;

TInt CCEsockTestBase::CancelAccessPointStatusQuery(
		const TDesC& aConnectionServName,
		const ConnectionServ::CConnectionServParameterBundle& aQueryBundle,
		ConnectionServ::CConnectionServParameterBundle& aResultBundle)
	{
	RConnectionServ* connServ = iConnectionServs.Find(aConnectionServName);

	if(connServ == NULL)
		return KErrNotFound;
		
	// Need a local active scheduler for the RConnectionServ API
	CActiveScheduler* as = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(as);
	CActiveScheduler::Install(as);

	// Create a timer that will complete almost immediately and stop the scheduler
	// thereby canceling the query while it is in progress
	// This all requires a long running query
	CMyTimer* timer = CMyTimer::NewL();
	CleanupStack::PushL(timer);
	timer->After(KHalfASecondInMicroSeconds);
	
	// Create active object to manage the query
	CConnServStatusQuery* sq = CConnServStatusQuery::NewLC(connServ, aQueryBundle, aResultBundle);
	
	// Start the scheduler
	TRAP_IGNORE(CActiveScheduler::Start())

	sq->Cancel();
	timer->Cancel();

	// Check that the cancel status is what we expected
	TInt res2 = sq->ErrCode();

	CleanupStack::PopAndDestroy(sq);
	CleanupStack::PopAndDestroy(timer);
	CleanupStack::PopAndDestroy(as);
	
	return res2;
	}


//
//socket functions library

RSocket* CCEsockTestBase::FindSocket(const TDesC& aSocketName)
    {
    return iSocks.Find(aSocketName);
    }

TInt CCEsockTestBase::CreateSocket(const TDesC& aSocketName)
	{
    //check if this socket hasn't been created already
    if (iSocks.Find(aSocketName)!=NULL)
    	return KErrAlreadyExists;

    RSocket* s = new RSocket;
    if (s==NULL)
    	return KErrNoMemory;

	TInt error = iSocks.Add(s,aSocketName);
	if (error!=KErrNone)
		delete s;

	return error;
	}

TInt CCEsockTestBase::OpenSocket(const TRSocketParams& aParams)
	{
    RSocketServ* ss = iSockServs.Find(aParams.iSockServName);
    if (ss==NULL)
		return KErrNotFound;

    RSocket* s = iSocks.Find(aParams.iSocketName);
	if (s==NULL)
		return KErrNotFound;

    TInt error;

    //try to find the connection (if specified)
    if (aParams.iConnectionName.Length()>0)
    	{
		RConnection* c = iConns.Find(aParams.iConnectionName);
		if (c==NULL)
			return KErrNotFound;
		error = s->Open(*ss, aParams.iAddrFamily, aParams.iSocketType, aParams.iProtocol, *c);
		}
	else if (aParams.iSubconnectionName.Length()>0)
    	{
		RSubConnection* sc = iSubConns.Find(aParams.iSubconnectionName);
		if (sc==NULL)
			{
			return KErrNotFound;
			}
		error = s->Open(*ss, aParams.iAddrFamily, aParams.iSocketType, aParams.iProtocol, *sc);
		}
	else if (aParams.iProtocolName.Length()>0)
		{
		error = s->Open(*ss,aParams.iProtocolName);
		}
	else
		{
		error = s->Open(*ss, aParams.iAddrFamily, aParams.iSocketType, aParams.iProtocol);
		}


	return error;
	}

TInt CCEsockTestBase::BindSocket(TRSocketParams& aSockeSParams)
	{
    RSocket* s = iSocks.Find(aSockeSParams.iSocketName);
	if (s==NULL)
		return KErrNotFound;

	TInt error = KErrNone;
	if (aSockeSParams.iProtocol==KProtocolInetUdp)
	    error = s->SetOpt(KSoUdpSynchronousSend,KSolInetUdp,1);
	if (error!=KErrNone)
		return error;

	//bind to Local address for TCP or UDP
	
	return s->Bind(aSockeSParams.iLocalIP);
	}

TInt CCEsockTestBase::ConnectSocket(TRSocketParams& aParams, TRequestStatus& aRequestStatus)
	{
    RSocket* s = iSocks.Find(aParams.iSocketName);
	if (s==NULL)
	    {
		return KErrNotFound;
	    }

    TInt err = KErrNone;

    if (aParams.iProtocol==KProtocolInetUdp)
        {
        err = s->SetOpt(KSoUdpSynchronousSend,KSolInetUdp,1);
        if (err != KErrNone)
            {
            return err;
            }
        }

    if (!aParams.iLocalIP.IsUnspecified() || !aParams.iLocalIP.IsWildPort())
        {
        err = s->Bind(aParams.iLocalIP);
        }

    if (err == KErrNone)
        {
        s->Connect(aParams.iRemoteIP, aRequestStatus);
        }

    return err;
	}

TInt CCEsockTestBase::CancelConnectSocket(const TRSocketParams& aParams)
	{
    RSocket* s = iSocks.Find(aParams.iSocketName);
	if (s==NULL)
		return KErrNotFound;

    s->CancelConnect();
    return KErrNone;
	}

void CCEsockTestBase::SendAndReceiveDataL(const TRSocketParams& aParams)
	{
    RSocket* s = iSocks.Find(aParams.iSocketName);
	if (s==NULL)
		User::Leave(KErrNotFound);

	TInt recvCount = 0;
	TRequestStatus stat;
	TBuf8<50> Data;

	//set up buffers
	HBufC8* writebuf = HBufC8::NewMaxLC(aParams.iPacketSize);
	HBufC8* readbuf  = HBufC8::NewMaxLC(aParams.iPacketSize);
	
	// In the case of armv5, HBufC8 MaxLength is set as 1504 or 1512
	// To sort it out we use TPtr8(TUint8* ,MaxLenghth) constructor
	// instead of using
	// TPtr8 ptrwritebuf = writebuf->Des();
	// TPtr8 ptrreadbuf = readbuf->Des();
	TPtr8 ptrwritebuf(const_cast<TUint8*>(writebuf->Ptr()), aParams.iPacketSize);
	TPtr8 ptrreadbuf(const_cast<TUint8*>(readbuf->Ptr()), aParams.iPacketSize);
	
	// NewMaxLC may give a max size bigger than specified. Make sure length and max length are in sync so that
	// the Read will complete. For example, NewMaxLC(1500) may give back a buffer 1504 as the max size. if you Write
	// then Read on that, it'll write the length (1500) but try to read the max length (1504). this'll never complete
	// because we'll only get back as many bytes as we wrote the the echo server.
	ptrwritebuf.SetLength(ptrwritebuf.MaxLength());
	ptrreadbuf.SetLength(ptrreadbuf.MaxLength());

    if (aParams.iProtocol==KProtocolInetTcp)
        {
   		for (TInt i = 0; i < aParams.iNrOfPackets; i++)
    		{
	    	// initialise data
		    Data.Format(_L8("TCP-packet:%d helloworld"),i);
		    ptrwritebuf.Repeat( Data );

		    // write data
		    s->Write(ptrwritebuf,stat);
		    User::WaitForRequest(stat);
		    User::LeaveIfError(stat.Int());

		    // read data
		    s->Read(ptrreadbuf, stat);
		    User::WaitForRequest(stat);
			User::LeaveIfError(stat.Int());

		    // compare the data
		    if (ptrwritebuf.Compare(ptrreadbuf) != 0)
		    	User::Leave(KErrGeneral);

		    recvCount+=ptrreadbuf.Length();
	    	}
		}
	else if (aParams.iProtocol==KProtocolInetUdp || aParams.iProtocol == KProtocolInetDummy)
		{
        for (TInt i = 0; i < aParams.iNrOfPackets; i++)
		    {
		    // initialise data
		    Data.Format(_L8("UDP-packet:%d helloworld"),i);
		    ptrwritebuf.Repeat( Data );

		    // If we are colouring the payload then first prefix outgoing payload with command byte to tell the stack so
		    if(aParams.iPayloadColouringIsEnabled)
		    	{
				ptrwritebuf[0] = 0xF9; // KColourDataByLinkTierAccessPointId; TODO_CDG
				}
		    
		    // write data
		    s->Send(ptrwritebuf, 0, stat);
		    User::WaitForRequest(stat);
		    User::LeaveIfError(stat.Int());

		    // If the returned data will be coloured by the IP stack for test purposes
		    // then we had better change what we compare it with
		    if(aParams.iPayloadColouringIsEnabled)
		    	{
		    	ptrwritebuf[0] += static_cast<TUint8>(aParams.iPayloadColour);
				}
		    
		    s->Recv(ptrreadbuf, 0, stat);
		    User::WaitForRequest(stat);
		    User::LeaveIfError(stat.Int());

		    // compare the data
		    if (ptrwritebuf.Compare(ptrreadbuf) != 0 )
		    	User::Leave(KErrGeneral);

		    recvCount += ptrreadbuf.Length();
		    }
		}
	else
        User::Leave(KErrNotFound);

    CleanupStack::PopAndDestroy(2, writebuf);	//	writebuf and readbuf

	// check the total received (95 per cent is allowable for us)
	if (recvCount*aParams.iPacketSize < (0.95*(aParams.iNrOfPackets*aParams.iPacketSize)))
		User::Leave(KErrGeneral);
	}

TInt CCEsockTestBase::CloseSocket(const TDesC& aSocketName)
	{
    RSocket* s = iSocks.Find(aSocketName);
	if (s==NULL)
		return KErrNotFound;

	s->Close();
    return KErrNone;
	}

TInt CCEsockTestBase::ShutdownSocket(const TRSocketParams& aParams, TRequestStatus& aRequestStatus)
	{
    RSocket* s = iSocks.Find(aParams.iSocketName);
	if (s==NULL)
		return KErrNotFound;

	s->Shutdown(aParams.iShutdownType,aRequestStatus);
    return KErrNone;
	}

TInt CCEsockTestBase::IoctlSocket(const TRSocketIoctlParams& aParams, TDes8& aIoctlOutput, TRequestStatus& aRequestStatus)
	{
    RSocket* s = iSocks.Find(aParams.iSocketName);
	if (s == NULL)
		return KErrNotFound;

	s->Ioctl(aParams.iIoctlCommand, aRequestStatus, &aIoctlOutput, aParams.iIoctlLevel);
	User::WaitForRequest(aRequestStatus);

    return KErrNone;
	}

TInt CCEsockTestBase::GetOptSocket(const TRSocketGetOptParams& aParams, TDes8& aGetOptOutput)
	{
    RSocket* s = iSocks.Find(aParams.iSocketName);
	if (s == NULL)
		return KErrNotFound;

	return s->GetOpt(aParams.iOptionName, aParams.iOptionLevel, aGetOptOutput);
	}

TInt CCEsockTestBase::GetOptSocket(const TRSocketGetOptParams& aParams, TInt& aGetOptOutput)
	{
    RSocket* s = iSocks.Find(aParams.iSocketName);
	if (s == NULL)
		return KErrNotFound;

	return s->GetOpt(aParams.iOptionName, aParams.iOptionLevel, aGetOptOutput);
	}

TInt CCEsockTestBase::SetOptSocket(const TRSocketSetOptParams& aParams, TDes8& aSetOptInput)
    {
    RSocket* s = iSocks.Find(aParams.iSocketName);
    if (s == NULL)
        return KErrNotFound;

    return s->SetOpt(aParams.iOptionName, aParams.iOptionLevel, aSetOptInput);
    }

TInt CCEsockTestBase::SetOptSocket(const TRSocketSetOptParams& aParams, TInt& aSetOptInput)
    {
    RSocket* s = iSocks.Find(aParams.iSocketName);
    if (s == NULL)
        return KErrNotFound;

    return s->SetOpt(aParams.iOptionName, aParams.iOptionLevel, aSetOptInput);
    }

TInt CCEsockTestBase::ProtocolDescription(
	const TSocketProtocolDescriptionParams& aParams,
	TProtocolDesc& aProtocolDescriptionOutput)
	{
    RSocket* s = iSocks.Find(aParams.iSocketName);
	if (s == NULL)
		return KErrNotFound;

	return s->Info(aProtocolDescriptionOutput);
	}


//
//connection functions library

RConnection* CCEsockTestBase::FindConnection(const TDesC& aConnectionName)
    {
    return iConns.Find(aConnectionName);
    }

TInt CCEsockTestBase::CreateConnection(const TDesC& aConnectionName)
	{
    //check if this connection hasn't been created already
    if (iConns.Find(aConnectionName)!=NULL)
    	return KErrAlreadyExists;

    RConnection* c = new RConnection;
    if (c==NULL)
    	return KErrNoMemory;

	TInt error = iConns.Add(c,aConnectionName);
	if (error!=KErrNone)
		delete c;

	return error;
	}

TInt CCEsockTestBase::RemoveConnection(const TDesC& aConnectionName)
    {
    //check if this connection hasn't been created already
    if (iConns.Find(aConnectionName)==NULL)
        return KErrNotFound;

    iConns.Remove(aConnectionName);
    return KErrNone;
    }



TInt CCEsockTestBase::OpenConnection(const TRConnectionParams& aParams)
	{
    RSocketServ* ss = iSockServs.Find(aParams.iSockServName);
    if (ss==NULL)
		return KErrNotFound;

    RConnection* c = iConns.Find(aParams.iConnectionName);
	if (c==NULL)
		return KErrNotFound;

	if(aParams.iConnectionType != -1)
	   return c->Open(*ss,aParams.iConnectionType);

	return c->Open(*ss);
	}

TInt CCEsockTestBase::WaitforIncomingConnection(const TRSubConnectionParams& aParams)
	{
    RSubConnection* sc = iSubConns.Find(aParams.iSubConnectionName);
	if (sc==NULL)
		return KErrNotFound;

	RConnection* c = iConns.Find(aParams.iConnectionName);
	if (c==NULL)
		return KErrNotFound;

	return c->WaitForIncoming(*sc);
	}

TInt CCEsockTestBase::WaitforIncomingConnection(const TRSubConnectionParams& aParams,TRequestStatus& reqStat)
	{
    RSubConnection* sc = iSubConns.Find(aParams.iSubConnectionName);
	if (sc==NULL)
		return KErrNotFound;

	RConnection* c = iConns.Find(aParams.iConnectionName);
	if (c==NULL)
		return KErrNotFound;

	c->WaitForIncoming(*sc,reqStat);

	return KErrNone;
	}

TInt CCEsockTestBase::AcceptIncomingConnection(const TRSubConnectionParams& aParams)
	{
    RSubConnection* sc = iSubConns.Find(aParams.iSubConnectionName);
	if (sc==NULL)
		return KErrNotFound;

	sc->Start();
	
	return KErrNone;
	}
	
TInt CCEsockTestBase::AsyncAcceptIncomingConnection(const TRSubConnectionParams& aParams, TRequestStatus& reqStat)
	{
    RSubConnection* sc = iSubConns.Find(aParams.iSubConnectionName);
	if (sc==NULL)
		return KErrNotFound;

	sc->Start(reqStat);
	
	return KErrNone;
	}
	
TInt CCEsockTestBase::RejectIncomingConnection(const TRSubConnectionParams& aParams)
	{
    RSubConnection* sc = iSubConns.Find(aParams.iSubConnectionName);
	if (sc==NULL)
		return KErrNotFound;

	sc->Stop();

	return KErrNone;
	}

TInt CCEsockTestBase::CancelIncomingConnection(const TRSubConnectionParams& aParams)
	{
    RSubConnection* sc = iSubConns.Find(aParams.iSubConnectionName);
	if (sc==NULL)
		return KErrNotFound;

	RConnection* c = iConns.Find(aParams.iConnectionName);
	if (c==NULL)
		return KErrNotFound;

	c->CancelWaitForIncoming();

	return KErrNone;
	}


TInt CCEsockTestBase::StartConnection(RConnection& aConn, TConnPref& aPrefs, TRequestStatus* aRequestStatus)
	{
	if (aRequestStatus)
		{
		aConn.Start(aPrefs, *aRequestStatus);
		}
	else
		{
		return aConn.Start(aPrefs);
		}
	return KErrNone;
	}

TInt CCEsockTestBase::GetParameters(TPtrC aConnectionName, ESock::CCommsDataObjectBase& aDataObject)
    {
    RConnection* c = iConns.Find(aConnectionName);
    if (c==NULL)
        return KErrNotFound;
    return c->GetParameters(aDataObject);
    }

TInt CCEsockTestBase::GetIntSetting(TPtrC aConnectionName, const TDesC& aSettingName, TUint32& aValue)
    {
    RConnection* c = iConns.Find(aConnectionName);
    if (c==NULL)
        return KErrNotFound;
    return c->GetIntSetting(aSettingName, aValue);
    }


TInt CCEsockTestBase::StartConnection(TRConnectionParams& aParams)
	{
    RConnection* c = iConns.Find(aParams.iConnectionName);
	if (c==NULL)
		return KErrNotFound;
	TRequestStatus* requestStatus = NULL;
	if (aParams.iAsynch)
		{
				//The request status may well already be there (created by the previous iteration?)
        requestStatus = iRequestStatuses.Find(aParams.iConnectionName);
        if (requestStatus == NULL)
            {
            requestStatus = new TRequestStatus;
            if (requestStatus==NULL)
                return KErrNoMemory;
            
            TInt error = iRequestStatuses.Add(requestStatus, aParams.iConnectionName);
            if (error!=KErrNone)
                {
                delete requestStatus;
                return error;
                }
            }
		}

	TInt error;
	if (aParams.iStartWithSnapPreferences)
		{
		TCommSnapPref prefs;
		prefs.SetSnap(aParams.iSNAP);

		error = StartConnection(*c, prefs, requestStatus);
		}
	else if (aParams.iStartWithOldPreferences)
		{
		TCommDbConnPref prefs;
		prefs.SetBearerSet(aParams.iBearer);
		prefs.SetIapId(aParams.iIAP);
		error = StartConnection(*c, prefs, requestStatus);
		}
	else if(aParams.iStartWithConPrefList)
		{
		TConnPrefList* prefs = TConnPrefList::NewL();
		CleanupStack::PushL(prefs);
		TConnAPPref* APPref = TConnAPPref::NewL(aParams.iSNAP);
		CleanupStack::PushL(APPref);
		TDummyPref* dummyPref = TDummyPref::NewL(aParams.iDummy);
		CleanupStack::PushL(dummyPref);
		if(aParams.iStartWithDummy)
			prefs->AppendL(dummyPref);
		
		TConnAutoStartPref* autoPref = TConnAutoStartPref::NewL();
		CleanupStack::PushL(autoPref);
		if(aParams.iStartAuto)
			prefs->AppendL(autoPref);
		
		prefs->AppendL(APPref);
		error = StartConnection(*c, *prefs, requestStatus);
		
		delete APPref;
		delete dummyPref;
		delete autoPref;
		if (aParams.iStartAuto)
			prefs->Remove(0);
		if (aParams.iStartWithDummy)
			prefs->Remove(0);
		prefs->Remove(0);
		delete prefs;
		
		CleanupStack::Pop(4);
		}
	else
		{
		if (requestStatus)
			{
			c->Start(*requestStatus);
			}
		else
			{
			error = c->Start();
			}
		}
	
	return error;
	}

TInt CCEsockTestBase::AttachToConnection(const TRConnectionParams& aParams)
	{
	RConnection* c = iConns.Find(aParams.iConnectionName);
	if (c==NULL)
		return KErrNotFound;

	TUint count = 0;
		c->EnumerateConnections(count);
	
	if(count < 1)
		return KErrNotFound;

	//
	TInt error = KErrNotSupported;
	RConnection* atc = iConns.Find(aParams.iAttachToConnectionName);
	if (atc!=NULL)
		{ //Currently this is only for the new attach (TConnProviderInfo)
		//If you want to extend this test step for legacy attach (TConnectionInfo),
		//you need to add a parameter to recognise which attach it is.

		ESock::XProviderInfoQuerySet* querySet = ESock::XProviderInfoQuerySet::NewL();
		CleanupDeletePushL(querySet);
		
		// CProviderInfoQuerySet takes ownership of the X-class
		ESock::CProviderInfoQuerySet* queryContainer = ESock::CProviderInfoQuerySet::NewL(querySet);
		CleanupStack::Pop(querySet);
		CleanupStack::PushL(queryContainer);
		
		error = atc->GetParameters(*queryContainer);
		if (error != KErrNone)
			return error;

		ESock::XProviderInfoQuerySet& outputQuerySet = queryContainer->DataObject();

		TPckg<TConnProviderInfo> pkgConnInfo(outputQuerySet.ConnProviderInfo());
		error = c->Attach(pkgConnInfo, aParams.iConnAttachType);
		
		CleanupStack::PopAndDestroy(queryContainer);
		}
	else
		{
		TConnectionInfo connInfo;
		TPckg<TConnectionInfo> pkgConnInfo(connInfo);

		TBool found = EFalse;
		for(TInt index = 1; index <= count; ++index)
			{
			error = c->GetConnectionInfo(index, pkgConnInfo);
			if(error != KErrNone)
				return error;
			
			if(aParams.iIAP == 0 && aParams.iNET == 0)
				{
				found = ETrue;
				}
			else if(aParams.iIAP > 0 && aParams.iNET > 0)	
				{
				found = (aParams.iIAP == connInfo.iIapId && aParams.iNET == connInfo.iNetId) ? ETrue : EFalse;
				}
			else if(aParams.iIAP > 0)
				{
				found = aParams.iIAP == connInfo.iIapId ? ETrue : EFalse;
				}
			else if(aParams.iNET > 0)	
				{
				found = aParams.iNET == connInfo.iNetId ? ETrue : EFalse;
				}
			
			if(found) 
				break;	
			}
			if(!found)	
				return KErrNotFound;
			
			error = c->Attach(pkgConnInfo,aParams.iConnAttachType);
		}
	
	return error;
	}

TInt CCEsockTestBase::StopConnection(const TRConnectionParams& aParams)
    {
    RConnection* c = iConns.Find(aParams.iConnectionName);
	if (c==NULL)
		return KErrNotFound;

	c->Stop(aParams.iConnStopType);
	return KErrNone;
    }

TInt CCEsockTestBase::CloseConnection(const TDesC& aConnectionName)
	{
    RConnection* c = iConns.Find(aConnectionName);
	if (c==NULL)
		return KErrNotFound;

	c->Close();
	return KErrNone;
	}

//
//connection events
//(each event notification request is encapsulated in a "EventContainer" that can easily be stored in a map.)

TInt CCEsockTestBase::RegisterForProgressNotificationEvent(const TEventContainerParams& aParams)
	{
	TEsockTestEventContainer* ec = iConnEvents.Find(aParams.iEventName);
	if (ec==NULL)
		{
		ec = new TEsockTestEventContainer;
		if (ec==NULL)
			return KErrNoMemory;

		TInt error = iConnEvents.Add(ec,aParams.iEventName);
		if (error!=KErrNone)
			{
			delete ec;
			return error;
			}
		}

	__ASSERT_DEBUG(ec->iEventReceived != KRequestPending, User::Panic(KSpecAssert_ESockTestEsckTst, 1));
	RConnection* conn = iConns.Find(aParams.iConnectionName);
	if (conn==NULL)
		{
		return KErrNotFound;
		}
	
	if (aParams.iEventMask == 0)
		return KErrArgument;

	conn->ProgressNotification(ec->iProgressBuf, ec->iEventReceived, aParams.iEventMask);
	return KErrNone;
	}

TInt CCEsockTestBase::ReceiveProgressNotificationEvent(TNifProgress*&aNifProgress, const TDesC& aEventName, TUint aTimeoutInMiliSec)
	{
	TEsockTestEventContainer* ec = iConnEvents.Find(aEventName);
	if (ec==NULL)
		return KErrNotFound;
	if (aTimeoutInMiliSec)
	    {
	    RTimer timer;
	    timer.CreateLocal();	    
	    TRequestStatus timerStatus;
	    timer.After(timerStatus, aTimeoutInMiliSec * 1000);
	    User::WaitForRequest(ec->iEventReceived, timerStatus);
	    }
	else
	    {
	    User::WaitForRequest(ec->iEventReceived);
	    }
	aNifProgress = (ec->iEventReceived.Int()==KErrNone)? &ec->iProgressBuf() : NULL;
	return ec->iEventReceived.Int() == KRequestPending ? KErrNone : ec->iEventReceived.Int();
	}


//mobility extensions
RCommsMobilityApiExt* CCEsockTestBase::FindMobilityExtension(const TDesC& aMobilityExtensionName)
    {
    return iMobilityExts.Find(aMobilityExtensionName);
    }

TInt CCEsockTestBase::CreateMobilityExtension(const TDesC& aMobilityExtensionName)
    {
    //check if this extension hasn't been created already
    if (iMobilityExts.Find(aMobilityExtensionName)!=NULL)
    	return KErrAlreadyExists;

    RCommsMobilityApiExt* me = new RCommsMobilityApiExt;
    if (me==NULL)
    	return KErrNoMemory;

	TInt error = iMobilityExts.Add(me,aMobilityExtensionName);
	if (error!=KErrNone)
		delete me;

	return error;
    }


// Data monitoring
//----------------

RCommsDataMonitoringApiExt* CCEsockTestBase::FindDataMonitoringExtension(const TDataMonitorEventParams& aParams)
   {
   return iDataMonitoringExts.Find(aParams.iDataMonitorName);
   }


TEsockTestDataMonitoringResponse* CCEsockTestBase::FindDataMonitoringResponder(const TDataMonitorEventParams& aParams)
	{
	return iDataMonitoringEvents.Find(aParams.iEventName);
	}

TInt CCEsockTestBase::CreateDataMonitoringExtension(const TDataMonitorEventParams& aParams)
   {
   if(iDataMonitoringExts.Find(aParams.iDataMonitorName) != NULL)
      {
      return KErrAlreadyExists;
      }

   RCommsDataMonitoringApiExt* ext = new RCommsDataMonitoringApiExt();

   if(!ext)
      {
    	return KErrNoMemory;
      }

   TInt error = iDataMonitoringExts.Add(ext, aParams.iDataMonitorName);
   
	if (error != KErrNone)
      {
      delete ext;
      }

	return error;
   }

TEsockTestDataMonitoringResponse* CCEsockTestBase::FindOrCreateDataMonitoringResponder(const TDataMonitorEventParams& aParams)
	{
	TEsockTestDataMonitoringResponse* response = FindDataMonitoringResponder(aParams);
	if(response)
		{
		return response;
		}
	
	if(NULL == (response = new TEsockTestDataMonitoringResponse()))
		{
		return NULL;
		}

	TInt err = iDataMonitoringEvents.Add(response, aParams.iEventName);
	if(err != KErrNone)
		{
		delete response;
		response = NULL;
		}
		
	return response;
	}
	
TInt CCEsockTestBase::OpenDataMonitoringExtension(const TDataMonitorEventParams& aParams)
	{
	RCommsSubSession* connOrSubConn = NULL;
	
	if(aParams.iConnectionName.Length() > 0)
		{ // Open extension on an RConnection if specified
		connOrSubConn = iConns.Find(aParams.iConnectionName);
		if(!connOrSubConn) 
			{
			return KErrNotFound;
			}
		}
	else if(aParams.iSubConnectionName.Length() > 0)
		{ // Otherwise open extension on an RSubConnection
		connOrSubConn = iSubConns.Find(aParams.iSubConnectionName);
		if(!connOrSubConn) 
			{
			return KErrNotFound;
			}
		}
	else
		{ // Neither an RConnection or an RSubConnection was specified
		return KErrArgument;
		}
	
	RCommsDataMonitoringApiExt* ext = iDataMonitoringExts.Find(aParams.iDataMonitorName);
	if(!ext)
		{
		return KErrNotFound;
		}
		
	return ext->Open(*connOrSubConn);
	}

TInt CCEsockTestBase::CloseDataMonitoringExtension(const TDataMonitorEventParams& aParams)
	{
	RCommsDataMonitoringApiExt* ext = iDataMonitoringExts.Find(aParams.iDataMonitorName);
	if(!ext)
		{
		return KErrNotFound;
		}
		
	ext->Close();
	
	return KErrNone;
	}

TInt CCEsockTestBase::RequestDataTransferred(const TDataMonitorEventParams& aParams, TDes8& aResponseBuf)
	{
	RCommsDataMonitoringApiExt* ext = iDataMonitoringExts.Find(aParams.iDataMonitorName);
	if(!ext)
		{
		return KErrNotFound;
		}

	TRequestStatus status;	
	ext->RequestDataTransferred(aResponseBuf, status);
	User::WaitForRequest(status);
	
	return KErrNone;
	}

// Find saved stats from a previous DataTransferred request
TDataMonitorStats* CCEsockTestBase::FetchSavedDataMonitorStats(const TDesC& aDataMonitorStatsSavedName)
	{
	return iSavedDataMonitorStats.Find(aDataMonitorStatsSavedName);
	}

// Save data monitor stats for future reference
TInt CCEsockTestBase::SaveDataMonitorStats(const TDesC& aSavedName, TDataMonitorStats& aStatsToSave)
	{
	TInt err;
	
	// Simply add if doesn't already exist
	if(iSavedDataMonitorStats.Find(aSavedName) == NULL)
		{
		err = iSavedDataMonitorStats.Add(&aStatsToSave, aSavedName);
		}
	else
		{
		// Already exists so delete and add in order to replace
		iSavedDataMonitorStats.Remove(aSavedName);
		err = iSavedDataMonitorStats.Add(&aStatsToSave, aSavedName);
		}
		
	return err;
	}

TInt CCEsockTestBase::RequestDataReceivedNotification(const TDataMonitorEventParams& aParams)
	{
	RCommsDataMonitoringApiExt* ext = FindDataMonitoringExtension(aParams);
	if(!ext)
		{
		return KErrNotFound;
		}

	TEsockTestDataMonitoringResponse* response =	FindOrCreateDataMonitoringResponder(aParams);
	if(!response)
		{
		return KErrNoMemory;
		}

	ext->RequestDataReceivedNotification(aParams.iDelta, aParams.iMinExpectedVolume, response->iBuffer, response->iStatus);
	
	return KErrNone;
	}

TInt CCEsockTestBase::CancelDataReceivedNotificationRequest(const TDataMonitorEventParams& aParams)
	{
	RCommsDataMonitoringApiExt* ext = FindDataMonitoringExtension(aParams);	
	if(!ext)
		{
		return KErrNotFound;
		}

	TEsockTestDataMonitoringResponse* response = FindOrCreateDataMonitoringResponder(aParams);
	if(!response)
		{
		return KErrNoMemory;
		}

	ext->CancelDataReceivedNotificationRequest();
	User::WaitForRequest(response->iStatus);
	
	// Return the status code from the cancellation
	return response->iStatus.Int();
	}

TInt CCEsockTestBase::ReceiveDataReceivedNotification(const TDataMonitorEventParams& aParams, TDesC8*& aResponseBuf)
	{
	TEsockTestDataMonitoringResponse* response =	FindDataMonitoringResponder(aParams);
	if(!response)
		{
		return KErrNotFound;
		}

	// Check that its not complete already
	if(response->iComplete)
		{
		return KErrGeneral;
		}

	RTimer timer;
	timer.CreateLocal();
	
	TRequestStatus timerStatus;
	timer.After(timerStatus, response->iTimeout * 1000000);
	
	User::WaitForRequest(response->iStatus, timerStatus);
	
	TInt result;
	if(response->iStatus.Int() != KErrNone)
		{
		result = KErrTimedOut;
		}
	else
		{
		timer.Cancel();
		User::WaitForRequest(timerStatus);

	   aResponseBuf = NULL;
	   if(response->iStatus.Int() == KErrNone) 
	   	{
	   	aResponseBuf = &response->iBuffer;
	   	}
   	result = response->iStatus.Int();
   	response->iComplete = ETrue;
		}
		
	timer.Close();
	
	return result;
	}

TInt CCEsockTestBase::RequestDataSentNotification(const TDataMonitorEventParams& aParams)
	{
	// Find the data monitoring extension to start with
	RCommsDataMonitoringApiExt* ext = FindDataMonitoringExtension(aParams);
	if(!ext)
		{
		return KErrNotFound;
		}
		
	// Find the data monitoring responder that goes with it
	TEsockTestDataMonitoringResponse* response =	FindDataMonitoringResponder(aParams);
	if(!response)
		{
		return KErrNotFound;
		}

	// Make the request on the data monitor
	ext->RequestDataSentNotification(aParams.iDelta, aParams.iMinExpectedVolume, response->iBuffer, response->iStatus);
	
	return KErrNone;
	}

TInt CCEsockTestBase::CancelDataSentNotificationRequest(const TDataMonitorEventParams& aParams)
	{
	RCommsDataMonitoringApiExt* ext = FindDataMonitoringExtension(aParams);
	if(!ext)
		{
		return KErrNotFound;
		}

	TEsockTestDataMonitoringResponse* response =	FindDataMonitoringResponder(aParams);
	if(!response)
		{
		return KErrNotFound;
		}

	ext->CancelDataSentNotificationRequest();
	User::WaitForRequest(response->iStatus);
	
	// Return the status code from the cancellation
	return response->iStatus.Int();
	}

TInt CCEsockTestBase::ReceiveDataSentNotification(const TDataMonitorEventParams& aParams, TDesC8*& aResponseBuf)
	{
	TEsockTestDataMonitoringResponse* response =	FindDataMonitoringResponder(aParams);
	if(!response)
		{
		return KErrNotFound;
		}

	// Check that its not complete already
	if(response->iComplete)
		{
		return KErrGeneral;
		}

	RTimer timer;
	timer.CreateLocal();
	
	TRequestStatus timerStatus;
	timer.After(timerStatus, response->iTimeout * 1000000);
	
	User::WaitForRequest(response->iStatus, timerStatus);
	
	TInt result;
	if(response->iStatus.Int() != KErrNone)
		{
		result = KErrTimedOut;
		}
	else
		{
		timer.Cancel();
		User::WaitForRequest(timerStatus);

	   aResponseBuf = NULL;
	   if(response->iStatus.Int() == KErrNone) 
	   	{
	   	aResponseBuf = &response->iBuffer;
	   	}
   	result = response->iStatus.Int();
   	response->iComplete = ETrue;
		}
		
	timer.Close();
	
	return result;
	}

#if 0
//
//CommsServer functions library

TInt CCEsockTestBase::CreateCommsServer(const TDesC& aCommsServerName)
	{
    //check if this comms server hasn't been created already
    if (iCommsServers.Find(aCommsServerName)!=NULL)
    	return KErrAlreadyExists;

    RConnection* c = new RConnection;
    if (c==NULL)
    	return KErrNoMemory;

	TInt error = iConns.Add(c,aConnectionName);
	if (error!=KErrNone)
		delete c;

	return error;
	}

TInt CCEsockTestBase::StartCommsServer(const TRCommsServerParams& aParams, TRequestStatus& aRequestStatus)
	{
	RConnManager* c = iCommsServers.Find(aParams.iCommsServerName);
	if (c==NULL)
		return KErrNotFound;
	
	c->Start(prefs,aRequestStatus);
		}
	else if (aParams.iStartWithOldPreferences)
		{
		TCommDbConnPref prefs;
		prefs.SetBearerSet(aParams.iBearer);
		prefs.SetIapId(aParams.iIAP);
		c->Start(prefs,aRequestStatus);
		}
	else
		c->Start(aRequestStatus);

	return KErrNone;
	}

TInt CCEsockTestBase::StartConnection(const TRConnectionParams& aParams)
	{
    RConnection* c = iConns.Find(aParams.iConnectionName);
	if (c==NULL)
		return KErrNotFound;

	TInt error;
	if (aParams.iStartWithSnapPreferences)
		{
		TCommSnapPref prefs;
		prefs.SetSnap(aParams.iSNAP);
		error = c->Start(prefs);
		}
	else if (aParams.iStartWithOldPreferences)
		{
		TCommDbConnPref prefs;
		prefs.SetBearerSet(aParams.iBearer);
		prefs.SetIapId(aParams.iIAP);
		error = c->Start(prefs);
		}
	else
		error = c->Start();

	return error;
	}

TInt CCEsockTestBase::StopConnection(const TRConnectionParams& aParams)
    {
    RConnection* c = iConns.Find(aParams.iConnectionName);
	if (c==NULL)
		return KErrNotFound;

	c->Stop(aParams.iConnStopType);
	return KErrNone;
    }
#endif


// SubConnections
//---------------

RSubConnection* CCEsockTestBase::FindSubConnection(const TDesC& aSubConnectionName)
    {
    return iSubConns.Find(aSubConnectionName);
    }

TInt CCEsockTestBase::CreateSubConnection(const TDesC& aSubConnectionName)
	{
    //check if this sub connection hasn't been created already
    if (iSubConns.Find(aSubConnectionName)!=NULL)
    	return KErrAlreadyExists;

    RSubConnection* sc = new RSubConnection;
    if (sc==NULL)
    	return KErrNoMemory;

	TInt error = iSubConns.Add(sc,aSubConnectionName);
	if (error!=KErrNone)
		delete sc;

	return error;
	}

TInt CCEsockTestBase::OpenSubConnection(const TRSubConnectionParams& aParams)
	{
    RSocketServ* ss = iSockServs.Find(aParams.iSockServName);
    if (ss==NULL)
		return KErrNotFound;

    RSubConnection* sc = iSubConns.Find(aParams.iSubConnectionName);
	if (sc==NULL)
		return KErrNotFound;

	RConnection* c = iConns.Find(aParams.iConnectionName);
	if (c==NULL)
		return KErrNotFound;

	return sc->Open(*ss, aParams.iSubConnType, *c);
	}

TInt CCEsockTestBase::CloseSubConnection(const TDesC& aSubConnectionName)
	{
    RSubConnection* sc = iSubConns.Find(aSubConnectionName);
	if (sc==NULL)
		return KErrNotFound;

	sc->Close();
	return KErrNone;
	}

TInt CCEsockTestBase::AddSocketToSubConnection(const TRSubConnectionParams& aParams, TRequestStatus& aStatus)
    {
    RSubConnection* sc = iSubConns.Find(aParams.iSubConnectionName);
	if (sc==NULL)
		return KErrNotFound;

    RSocket* s = iSocks.Find(aParams.iSocketName);
	if (s==NULL)
		return KErrNotFound;

	sc->Add(*s,aStatus);
	return KErrNone;
	}

TInt CCEsockTestBase::RemoveSocketFromSubConnection(const TRSubConnectionParams& aParams, TRequestStatus& aStatus)
    {
    RSubConnection* sc = iSubConns.Find(aParams.iSubConnectionName);
	if (sc==NULL)
		return KErrNotFound;

    RSocket* s = iSocks.Find(aParams.iSocketName);
	if (s==NULL)
		return KErrNotFound;

	sc->Remove(*s,aStatus);
	return KErrNone;
    }

TInt CCEsockTestBase::SetSubConnectionParameters(const TRSubConnectionParams& aParams)
	{
    RSubConnection* sc = iSubConns.Find(aParams.iSubConnectionName);
	if (sc==NULL)
		return KErrNotFound;

	RSubConParameterBundle* scp = iSubConnParams.Find(aParams.iSubConnectionParamsName);
	if (scp==NULL)
		return KErrNotFound;

	return sc->SetParameters(*scp);
	}

TInt CCEsockTestBase::GetSubConnectionParameters(const TRSubConnectionParams& aParams)
	{
    if (iSubConnParams.Find(aParams.iSubConnectionParamsName)!=NULL)
    	return KErrAlreadyExists;

    RSubConParameterBundle* scp = new RSubConParameterBundle;
    if (scp==NULL)
    	return KErrNoMemory;

	TInt error = iSubConnParams.Add(scp,aParams.iSubConnectionParamsName);
	if (error!=KErrNone)
	    {
		delete scp;
		return error;
	    }

    RSubConnection* sc = iSubConns.Find(aParams.iSubConnectionName);
	if (sc==NULL)
		return KErrNotFound;

	return sc->GetParameters(*scp);
	}

TInt CCEsockTestBase::CompareIntegerValues(const TCompareIntegersParams& aParams, TInt& aDifference)
	{
    TInt* value1;
    value1 = iIntegerValues.Find(aParams.iValue1);
    if(value1 == NULL)
    	{
		return KErrNotFound;
		}

    TInt* value2;
    value2 = iIntegerValues.Find(aParams.iValue2);
	if (value2 == NULL)
    	{
		return KErrNotFound;
		}

	// Confirm the values
	aDifference = (*value1 - *value2 + aParams.iDifference);
	return KErrNone;
	}

TInt CCEsockTestBase::AwaitRequestStatus(const TDesC& aStatusName)
	{
    TRequestStatus* req = iRequestStatuses.Find(aStatusName);
    ASSERT(req);
   	User::WaitForRequest(*req);
   	return req->Int();
	}

//
//subconnection parameters
TInt CCEsockTestBase::CreateRSubConnectionParams(const TDesC& aSubConParamsName)
	{
	RSubConParameterBundle* scp = new RSubConParameterBundle;
    if (scp==NULL)
    	return KErrNoMemory;

	TInt error = iSubConnParams.Add(scp,aSubConParamsName);
	if (error!=KErrNone)
		delete scp;

	return error;
	}
// SBLP functions

TInt CCEsockTestBase::CreateTFlowId(const TDesC& aFlowIdName)
	{
	TFlowId* fid = new TFlowId;
	if (fid==NULL)
    	return KErrNoMemory;

	TInt error = iFlowId.Add(fid,aFlowIdName);
	if (error!=KErrNone)
		delete fid;

	return error;

	}

TFlowId* CCEsockTestBase::FindTFlowId(const TDesC& aFlowId)
	{
		return iFlowId.Find(aFlowId);
	}

TInt CCEsockTestBase::CloseRSubConnectionParams(const TDesC& aSubConParamsName)
	{
	RSubConParameterBundle* scp = iSubConnParams.Find(aSubConParamsName);
	if (scp==NULL)
		return KErrNotFound;

	scp->Close();
	return KErrNone;
	}


RSubConParameterBundle* CCEsockTestBase::FindRSubConnectionParams(const TDesC& aSubConParamsName)
	{
		return iSubConnParams.Find(aSubConParamsName);
	}

CSubConParameterFamily* CCEsockTestBase::FindRSubConnectionParameterFamily(const TDesC& aSubConParamsName, const TInt aFamilyId)
    {
    RSubConParameterBundle* bundle = FindRSubConnectionParams(aSubConParamsName);
    if (bundle==NULL)
        return NULL;

    return bundle->FindFamily(aFamilyId);
    }

//
//subconnection events
//(each event notification request is encapsulated in a "EventContainer" that can easily be stored in a map.)

TInt CCEsockTestBase::RegisterForRSubConnectionEvent(const TEventContainerParams& aParams)
    {
    TEsockTestEventContainer* ec = iSubConnEvents.Find(aParams.iEventName);
	if (ec==NULL)
	    {
    	ec = new TEsockTestEventContainer;
        if (ec==NULL)
    	    return KErrNoMemory;

    	TInt error = iSubConnEvents.Add(ec,aParams.iEventName);
	    if (error!=KErrNone)
	        {
		    delete ec;
		    return error;
	        }
	    }

    __ASSERT_DEBUG(ec->iEventReceived != KRequestPending, User::Panic(KSpecAssert_ESockTestEsckTst, 2));
    RSubConnection* sc = iSubConns.Find(aParams.iSubConnectionName);
	if (sc==NULL)
	    {
		return KErrNotFound;
	    }

	//check if we should simply register for all event
	if (aParams.iAllEvents)
	    {
        sc->EventNotification(static_cast<TNotificationEventBuf&>(ec->iEventNotifBuf), EFalse, ec->iEventReceived);
	    return KErrNone;
	    }

	//no.. what about only generic ones?
	if (aParams.iGenericEvents)
	    {
        sc->EventNotification(static_cast<TNotificationEventBuf&>(ec->iEventNotifBuf), ETrue, ec->iEventReceived);
	    return KErrNone;
	    }

	//no.. reqister for specific events
    if (aParams.iEventMask == 0)
        return KErrArgument;

    (ec->iSubconnEventFilter).iEventGroupUid = KSubConnGenericEventsImplUid;
    (ec->iSubconnEventFilter).iEventMask = aParams.iEventMask;
    sc->EventNotification(static_cast<TNotificationEventBuf&>(ec->iEventNotifBuf), &(ec->iSubconnEventFilter), 1, ec->iEventReceived);
    return KErrNone;
    }

TInt CCEsockTestBase::ReceiveRSubConnectionEvent(TNotificationEventBuf*& aBufferPtr, const TDesC& aSubConEventName)
    {
    TEsockTestEventContainer* ec = iSubConnEvents.Find(aSubConEventName);
	if (ec==NULL)
		return KErrNotFound;

    User::WaitForRequest(ec->iEventReceived);
    aBufferPtr = (ec->iEventReceived.Int()==KErrNone)? &static_cast<TNotificationEventBuf&>(ec->iEventNotifBuf) : NULL;
    __ASSERT_DEBUG(aBufferPtr->GroupId(), User::Panic(KSpecAssert_ESockTestEsckTst, 3));
    return ec->iEventReceived.Int();
    }

//
//mobility notifications
//(each notification request is encapsulated in a "EventContainer" that can easily be stored in a map.)

TInt CCEsockTestBase::RegisterForMobilityNotification(const TEventContainerParams& aParams)
    {
    TEsockTestEventContainer* ec = iConnEvents.Find(aParams.iEventName);
	if (ec==NULL)
	    {
    	ec = new TEsockTestEventContainer;
        if (ec==NULL)
    	    return KErrNoMemory;

    	TInt error = iConnEvents.Add(ec,aParams.iEventName);
	    if (error!=KErrNone)
	        {
		    delete ec;
		    return error;
	        }
	    }

    RCommsMobilityApiExt* me = iMobilityExts.Find(aParams.iMobilityExtName);
	if (me==NULL)
	    {
		return KErrNotFound;
	    }

	//reqister
    __ASSERT_DEBUG(ec->iEventReceived != KRequestPending, User::Panic(KSpecAssert_ESockTestEsckTst, 4));
	me->RegisterForMobilityNotification(ec->iEventNotifBuf,ec->iEventReceived);

    return KErrNone;
    }

TInt CCEsockTestBase::ReceiveMobilityNotification(TDesC8*& aBufferPtr, const TEventContainerParams& aParams)
    {
    TEsockTestEventContainer* ec = iConnEvents.Find(aParams.iEventName);
	if (ec==NULL)
		return KErrNotFound;

    RCommsMobilityApiExt* me = iMobilityExts.Find(aParams.iMobilityExtName);
	if (me==NULL)
		return KErrNotFound;

	if (aParams.iTimeoutMiliSecs!=0)
		{
		TRequestStatus timeoutStatus;
		RTimer timer;
		timer.CreateLocal();
		timer.After(timeoutStatus,aParams.iTimeoutMiliSecs*1000);
    	User::WaitForRequest(ec->iEventReceived,timeoutStatus);
    	timer.Cancel();
    	timer.Close();
    	if (ec->iEventReceived==KRequestPending)
    		{
    		me->CancelMobilityNotification();
    		}
    	}
    else
    	{
    	User::WaitForRequest(ec->iEventReceived);
    	
    	}

    aBufferPtr = (ec->iEventReceived.Int()==KErrNone)? &ec->iEventNotifBuf : NULL;
    return ec->iEventReceived.Int();
    }



//
//cleanup

void CCEsockTestBase::CleanAll()
	{
    iSockServs.ResetAndDestroy();
    iSocks.ResetAndDestroy();
	iConns.ResetAndDestroy();
	iSubConns.ResetAndDestroy();
	iConnectionServs.ResetAndDestroy();
	iMobilityExts.ResetAndDestroy();
	iDataMonitoringExts.ResetAndDestroy();
	iDataMonitoringEvents.ResetAndDestroy();
	iNotifWatchers.ResetAndDestroy();
    TInt i = 0;
    for (i = 0; i < iSubConnParams.Count(); i++)
        {
        iSubConnParams[i]->Close();
        }
	iSubConnParams.ResetAndDestroy();
	iSubConnEvents.ResetAndDestroy();
	iConnEvents.ResetAndDestroy();
	iFlowId.ResetAndDestroy();
	iIntegerValues.ResetAndDestroy();
	iRequestStatuses.ResetAndDestroy();
	}

//
//general utils


//
// Simtsy Trigger Event functionality

/**
 Instructs the simtsy to complete a given request/notification. The supported commands are given in TEtelRequestType.
 The parameter supplied in aNewValue instructs the simtsy what data from the config.txt to use in the completion of the request.
 The aNewValue is the index to the entry to be used to complete the request for the current test step section, ie if aNewValue
 is 0, iTestNumber is 3, and aEtelCommand is ENetworkQoSChange, the entry will complete any NotifyNetworkQoSChange calls
 with data from the first QosNetworkNotification= entry of section [test3] in the config.txt file
*/


TInt CCEsockTestBase::EtelRequestL(TUint aEventKey, TInt aNewValue)
	{
  	// simtsy will listen for any changes to the property, and complete the corresponding request
	TInt error = RProperty::Set(KUidPSSimTsyCategory, aEventKey, aNewValue);

	return error;
	}


//



//
//simtsy functionality

TInt TSimTsy::Start()
    {
    //define name of the Phone module to be used for the RTelServer
    _LIT(KMtsyName, "SIM");
    TInt res = StartC32();
    if (res!=KErrNone)
            return res;
    TInt error = iServer.Connect();
    if (error!=KErrNone)
            return error;
    error = iServer.LoadPhoneModule(KMtsyName);
   	if (error!=KErrNone)
            return error;
   	error = iPhone.Open(iServer, KPhoneName);
    if (error!=KErrNone)
            return error;
    error = SetTestNumber(0);
	return error;
    }


void TSimTsy::Stop()
    {
    iPhone.Close();
    iServer.Close();
    }


TInt TSimTsy::SetTestNumber(TInt aTestSectionNumber)
	{
   	return RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, aTestSectionNumber);
	}

//

