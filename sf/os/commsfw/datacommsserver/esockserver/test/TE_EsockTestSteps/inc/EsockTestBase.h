/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file EsockTestBase.h
*/

#if (!defined ESOCKTESTBASE_H)
#define ESOCKTESTBASE_H

#include <e32base.h>
#include <in_sock.h>
#include <es_sock.h>
#include "SimpleMapTemplate.h"
#include <ip_subconparams.h>
#include <etel.h>
#include <etelmm.h>
#include <simtsy.h>
#include <rsshared.h>
#include "SimTsySpecificDefs.h"

#include <test/testexecutestepbase.h>

#include <comms-infras/es_mobility_apiext.h>
#include <comms-infras/es_datamon_apiext.h>
//include <comms-infras/cs_availability_apiext.h>
#include <elements/sd_mintercept_macro.h>
//#include "MobilitySimulation.h"
#include <comms-infras/es_connectionserv.h>

class CConnServNotificationWatcher;

enum TSocketDataTransform
	{
	// Basic addition transformations
	ETransformDataNone, ETransformDataAddOne, ETransfromDataAddTwo,
	// Others
	ETransformDataXor = 0x80
	};

enum TBearerAvailabilityChangeType
	{
	EAvailability_MakeAvailable, EAvailability_MakeUnavailable, EAvailability_Disconnect
	};

//to remove
//#include "dummyrsubconnection_todelete.h"


///////////////////////////////////////////////////////////////////////////////
//TMigrateToPreferredCarrierParams - container for parameters for accepting a move for mobility

struct TMigrateToPreferredCarrierParams
	{
	void Reset();

	TPtrC iName;
	TPtrC iMobilityApiName;
	};

///////////////////////////////////////////////////////////////////////////////
//TSocketDataCheckParams - container for parameters for checking data on a socket

struct TSocketDataCheckParams
    {
    void Reset();

    TPtrC iSocketCheckName;
    TPtrC iSocketName;
    TSocketDataTransform iTransformType;
    TBool iErrorExpected;

    };


///////////////////////////////////////////////////////////////////////////////
//TRSocketParams - container for parameters (from an .ini file)
//needed for operations on a sockets

struct TRSocketParams
    {
    void Reset();

    TPtrC iSockServName;
    TPtrC iConnectionName;
    TPtrC iSubconnectionName;
    TPtrC iSocketName;
    TPtrC iProtocolName;

    TUint iSocketType;
   	TUint iProtocol;
   	TUint iAddrFamily;
	TInt iPacketSize;
	TInt iNrOfPackets;
   	TInetAddr iLocalIP;
   	TInetAddr iRemoteIP;
   	RSocket::TShutdown iShutdownType;

    // Payload colouring
    TBool iPayloadColouringIsEnabled;
    TInt iPayloadColour;
    };

struct TRSocketIoctlParams
    {
    void Reset();

    TPtrC iSocketName;
	TInt iIoctlCommand;
	TInt iIoctlLevel;
	TPtrC iExpectedIoctlOutput;
    };

struct TRSocketGetOptParams
    {
    void Reset();

    TPtrC iSocketName;
	TInt iOptionName;
	TInt iOptionLevel;
	TPtrC iExpectedOptionOutputText;
	TInt iExpectedOptionOutputValue;
	TBool iTextExpected;
	TBool iValueExpected;
    };

struct TRSocketSetOptParams
    {
    void Reset();

    TPtrC iSocketName;
    TInt iOptionName;
    TInt iOptionLevel;
    TPtrC iOptionToSetText;
    TInt iOptionToSetValue;
    TBool iTextSet;
    TBool iValueSet;
    };

struct TProtocolDescriptionParams
	{
	void Reset();
	static TInt FetchDescription(TProtocolDescriptionParams& aProtocolParams, CTestStep& aTestStep);
	TBool MatchesExpected(TProtocolDesc& aProtocolDescription);

	TProtocolDescriptionParams();
	
	struct TProtocolDescParamsExpected
		{
		void Reset();

		TBool NoneExpected();
		TBool AllExpected();

		TBool iName;
		TBool iAddrFamily;
		TBool iProtocol;
		TBool iSockType;
		TBool iVersion;
		TBool iByteOrder;
		TBool iServiceInfo;
		TBool iNamingServices;
		TBool iSecurity;
		TBool iMessageSize;
		};

	TProtocolDesc iValues;
	TProtocolDescParamsExpected iValuesGiven;
	};

struct TCompareIntegersParams
	{
    void Reset();
    TPtrC iValue1;
    TPtrC iValue2;
	TInt iDifference;	
	};

struct TSocketProtocolDescriptionParams
	{
	void Reset();
    TPtrC iSocketName;
	TProtocolDescriptionParams iProtocolDescParams;
	};

struct TSocketServFindProtocolParams
	{
    void Reset();
    TPtrC iSocketServName;
	TProtocolDescriptionParams iProtocolDescParams;
	};

struct TSocketServGetProtocolInfoParams
	{
	void Reset();
    TPtrC iSocketServName;
	TInt iProtocolIndex;
	TProtocolDescriptionParams iProtocolDescParams;
	};

struct TSocketServStartStopProtocolParams
	{
	void Reset();
    TPtrC iSocketServName;
	TInt iAddrFamily;
	TInt iProtocol;
	TInt iSockType;
	};

///////////////////////////////////////////////////////////////////////////////
//TSocketServerNumProtocolsParams - container for paramters for checking the number of procotols on a socket server instance

struct TSocketServerNumProtocolsParams
	{
	void Reset();

	TPtrC iSocketServName;
	TInt iNumberExpected;
	TPtrC iSavedAsName;
	TBool iExpected;
	TBool iToBeSaved;
	};


///////////////////////////////////////////////////////////////////////////////
//TRConnectionParams - container for parameters (from an .ini file)
//needed for operations on a connections

struct TRConnectionParams
    {
    void Reset();

    TPtrC iSockServName;
    TPtrC iConnectionName;
    TPtrC iAttachToConnectionName;
    TInt iConnectionType;

  	TUint iBearer;
	TInt iIAP;
	TInt iNET;
	TInt iSNAP;
	TInt iDummy;
    RConnection::TConnStopType iConnStopType;
	RConnection::TConnAttachType iConnAttachType;
    TBool iStartWithOldPreferences;
    TBool iStartWithSnapPreferences;
    TBool iStartWithConPrefList;
    TBool iStartWithDummy;
    TBool iStartAuto;
    TBool iAsynch;    
    };

struct TCreateBearerAvailabilitySimulatorParams
{
	void Reset();

	TPtrC iAvailSimuName;
	TInt iBearerNumber;
};

struct TChangeBearerAvailabilityParams
{
	void Reset();

	TPtrC iName;
	TPtrC iAvailSimuName;
	TBearerAvailabilityChangeType iChangeToMake;
};


///////////////////////////////////////////////////////////////////////////////
//TRSubConnectionParams - container for parameters (from an .ini file)
//needed for operations on a subconnections

struct TRSubConnectionParams
    {
    void Reset();

    TPtrC iSockServName;
    TPtrC iConnectionName;
    TPtrC iSubConnectionName;
    TPtrC iSocketName;

	TPtrC iSubConnectionParamsName;
	TPtrC iSubConnectionEventName;

    RSubConnection::TSubConnType iSubConnType;
    };

struct TCancelConnectionNotificationParams
	{
	void Reset();

	TPtrC iName;
	TPtrC iConnMgrApiName;
	};

struct TGetConnectionNotificationAPIParams
	{
	void Reset();

	TPtrC iName;
	};

struct TGetRConnMgrBearerCountParams
	{
	void Reset();

	TPtrC iName;
	TInt iExpectedCount;
	};

struct TOpenRCommsManagerServerParams
	{
	void Reset();

	TPtrC iName;
	};

struct TSelectRConnectionManagerTierParams
	{
	void Reset();

	TPtrC iName;
	TInt iTierId;
	};

///////////////////////////////////////////////////////////////////////////////
//TEsockTestEventContainer - encapsulates all necessary RConnection
//or RSubConnection event data so that it can be conveniently stored in a map.
struct TEventContainerParams
    {
	void Reset();

	TPtrC iEventName;
	TPtrC iSubConnectionName;
	TPtrC iConnectionName;
	TPtrC iMobilityExtName;
	TPtrC iAvailabilityExtName;
	TPtrC iConnectionManagerExtName;
	TPtrC iExtensionName;
	TBool iAllEvents;
	TBool iGenericEvents;
	TUint32 iEventMask;
	TInt iTimeoutMiliSecs;
    };

struct TCheckAvailabilityParams
{
	void Reset();

	TPtrC iAvailabilityCheckName;
	TPtrC iExtensionApiName;
	TBool iExpectedAvailability;
};

///////////////////////////////////////////////////////////////////////////////
// TEsockTestDataMonitoringEventContainer - encapsulates all necessary data monitoring
// event data so that it can be conveniently stored in a map.
struct TDataMonitorEventParams
	{
	void Reset();
	TPtrC& ConnOrSubConnName();

	TBuf<100> iEventName;
	TPtrC iConnectionName;
	TPtrC iSubConnectionName;
	TPtrC iDataMonitorName;

	TUint iDelta;
	TUint iMinExpectedVolume;
	TBool iExpectTimeout;
	TInt iReceivedDataVolumeCondition;
	TInt iSentDataVolumeCondition;
	TPtrC iCompareAgainstName;
	TPtrC iSaveResultsName;
	};


///////////////////////////////////////////////////////////////////////////////
// TEsockTestDataMonitoringResponse - encapsulates the information required
// to receive and process a response to a data monitoring event.
class TEsockTestDataMonitoringResponse
	{
public:
	TEsockTestDataMonitoringResponse() : iComplete(EFalse)  {}
	TBuf8<KNotificationEventMaxSize> iBuffer;
	TInt iTimeout;
	TRequestStatus iStatus;
	TBool iComplete;
	};


class TEsockTestEventContainer
	{
public:
    ~TEsockTestEventContainer();

    RSubConnection::TEventFilter iSubconnEventFilter;
    TBuf8<KNotificationEventMaxSize> iEventNotifBuf;
    TNifProgressBuf iProgressBuf;
    TRequestStatus iEventReceived;
	};

class TDataMonitorStats
	{
public:
	TUint iReceivedBytes;
	TUint iSentBytes;
	};

///////////////////////////////////////////////////////////////////////////////
//TSimTsy - encapsulates all necessary sim tsy objects and data
//so that it can be conveniently stored in a map


struct TSimTsy
    {
    TInt Start();
    void Stop();
    TInt SetTestNumber(TInt aTestSectionNumber);

    RTelServer iServer;
    RMobilePhone iPhone;
    };


///////////////////////////////////////////////////////////////////////////////
//TRSimtsyEventParams - container for parameters (from an .ini file)

struct TSimTsyEventParams
    {
    void Reset();

    TPtrC iConnectionName;
    TPtrC iSimTsyEventName;
    TPtrC iEventName;
    TUint iEventKey;
    TInt  iEventIndex;

    };


///////////////////////////////////////////////////////////////////////////////
//CCEsockTestBase
class CCEsockTestBase
	{
public:
	virtual ~CCEsockTestBase();

public:
    //socket server functions library
	TInt CreateSocketServer(const TDesC& aSockServName);
	TInt ConnectSocketServer(const TDesC& aSockServName);
	TInt CloseSocketServer(const TDesC& aSockServName);
	TInt SocketServerNumProtocols(TSocketServerNumProtocolsParams& aParams, TInt& aNumProtocols);
	TInt SocketServerFindProtocol(TSocketServFindProtocolParams& aParams, TProtocolDesc& aProtocolDescription);
	TInt SocketServerGetProtocolInfo(TSocketServFindProtocolParams& aParams, TProtocolDesc& aProtocolDescription);
	TInt SocketServerGetProtocolInfo(TSocketServGetProtocolInfoParams& aParams, TProtocolDesc& aProtocolDescription);
	TInt SocketServerStartProtocol(TSocketServStartStopProtocolParams& aParams, TRequestStatus& aStatus);
	TInt SocketServerStopProtocol(TSocketServStartStopProtocolParams& aParams, TRequestStatus& aStatus);

	TInt __DbgFailNext(const TDesC& aSockServName, TInt aCount);
	TBool __DbgCheckFailNext(const TDesC& aSockServName);

    //socket functions library
    RSocket* FindSocket(const TDesC& aSocketName);
    TInt CreateSocket(const TDesC& aSocketName);
    TInt OpenSocket(const TRSocketParams& aParams);
    TInt BindSocket(TRSocketParams& aParams);
    TInt ConnectSocket(TRSocketParams& aParams, TRequestStatus& aRequestStatus);
    TInt CancelConnectSocket(const TRSocketParams& aParams);
    void SendAndReceiveDataL(const TRSocketParams& aParams);
    TInt CloseSocket(const TDesC& aSocketName);
    TInt ShutdownSocket(const TRSocketParams& aParams, TRequestStatus& aRequestStatus);
	TInt IoctlSocket(const TRSocketIoctlParams& aParams, TDes8& aIoctlOutput, TRequestStatus& aRequestStatus);
	TInt SendAndReceiveDataAndVerifyL(const TSocketDataCheckParams& /*aParams*/);
	TInt ProtocolDescription(
		const TSocketProtocolDescriptionParams& aParams,
		TProtocolDesc& aProtocolDescriptionOutput);
	TInt GetOptSocket(const TRSocketGetOptParams& aParams, TDes8& aGetOptOutput);
	TInt GetOptSocket(const TRSocketGetOptParams& aParams, TInt& aGetOptOutput);
	TInt SetOptSocket(const TRSocketSetOptParams& aParams, TDes8& aSetOptOutput);
    TInt SetOptSocket(const TRSocketSetOptParams& aParams, TInt& aSetOptOutput);

    //connection functions library
    RConnection* FindConnection(const TDesC& aConnectionName);
    TInt CreateConnection(const TDesC& aConnectionName);
    TInt RemoveConnection(const TDesC& aConnectionName);
    TInt OpenConnection(const TRConnectionParams& aParams);
    TInt StartConnection(RConnection& aConn, TConnPref& aPrefs, TRequestStatus* aRequestStatus);
    TInt StartConnection(TRConnectionParams& aParams);
	TInt AttachToConnection(const TRConnectionParams& aParams);
    TInt WaitforIncomingConnection(const TRSubConnectionParams& aParams);
    TInt WaitforIncomingConnection(const TRSubConnectionParams& aParams,TRequestStatus& aStatus);
    TInt AcceptIncomingConnection(const TRSubConnectionParams& aParams);
   	TInt AsyncAcceptIncomingConnection(const TRSubConnectionParams& aParams, TRequestStatus& reqStat);
    TInt RejectIncomingConnection(const TRSubConnectionParams& aParams);
    TInt CancelIncomingConnection(const TRSubConnectionParams& aParams);
    TInt StopConnection(const TRConnectionParams& aParams);
    TInt CloseConnection(const TDesC& aConnectionName);
	TInt SelectConnection(const TRConnectionParams& aParams, TRequestStatus aStatus);
    TInt CreateAvailabilityExtension(const TDesC& aAvailabilityExtensionName);
    TInt RegisterForProgressNotificationEvent(const TEventContainerParams& aParams);
    TInt ReceiveProgressNotificationEvent(TNifProgress*& aNifProgress, const TDesC& aEventName, TUint aTimeoutInMiliSec = 0);
    TInt GetParameters(TPtrC aConnectionName, ESock::CCommsDataObjectBase& aDataObject);
    TInt GetIntSetting(TPtrC aConnectionName, const TDesC& aSettingName, TUint32& aValue);

    //mobility extensions
    RCommsMobilityApiExt* FindMobilityExtension(const TDesC& aMobilityExtensionName);
    TInt CreateMobilityExtension(const TDesC& aMobilityExtensionName);

	// Availability extensions
#if 0
	CCommsAvailabilityApiExt* FindAvailabilityExtension(const TDesC& aAvailabilityExtensionName);
#endif

    // Data monitoring extensions
    RCommsDataMonitoringApiExt* FindDataMonitoringExtension(const TDataMonitorEventParams& aParams);
    TInt CreateDataMonitoringExtension(const TDataMonitorEventParams& aParams);
    TEsockTestDataMonitoringResponse* FindDataMonitoringResponder(const TDataMonitorEventParams& aParams);
	TEsockTestDataMonitoringResponse* FindOrCreateDataMonitoringResponder(const TDataMonitorEventParams& aParams);
	TInt OpenDataMonitoringExtension(const TDataMonitorEventParams& aParams);
	TInt CloseDataMonitoringExtension(const TDataMonitorEventParams& aParams);
	TInt RequestDataTransferred(const TDataMonitorEventParams& aParams, TDes8& aResponseBuf);
	TInt RequestDataReceivedNotification(const TDataMonitorEventParams& aParams);
	TInt CancelDataReceivedNotificationRequest(const TDataMonitorEventParams& aParams);
	TInt RequestDataSentNotification(const TDataMonitorEventParams& aParams);
	TInt CancelDataSentNotificationRequest(const TDataMonitorEventParams& aParams);
	TInt ReceiveDataSentNotification(const TDataMonitorEventParams& aParams, TDesC8*& aResponseBuf);
	TInt ReceiveDataReceivedNotification(const TDataMonitorEventParams& aParams, TDesC8*& aResponseBuf);
	TDataMonitorStats* FetchSavedDataMonitorStats(const TDesC& aDataMonitorStatsSavedName);
	TInt SaveDataMonitorStats(const TDesC& aSavedName, TDataMonitorStats& aStatsToSave);
    //subconnection functions library
    RSubConnection* FindSubConnection(const TDesC& aSubConnectionName);
    TInt CreateSubConnection(const TDesC& aSubConnectionName);
    TInt OpenSubConnection(const TRSubConnectionParams& aParams);
    TInt CloseSubConnection(const TDesC& aSubConnectionName);
    TInt AddSocketToSubConnection(const TRSubConnectionParams& aParams, TRequestStatus& aStatus);
    TInt RemoveSocketFromSubConnection(const TRSubConnectionParams& aParams, TRequestStatus& aStatus);
	TInt SetSubConnectionParameters(const TRSubConnectionParams& aParams);
	TInt GetSubConnectionParameters(const TRSubConnectionParams& aParams);

	//Sblp functions
	TFlowId* FindTFlowId(const TDesC& aFlowId);
	TInt CreateTFlowId(const TDesC& aSubConSblpParamsName);

	//subconnection parameters
	TInt CreateRSubConnectionParams(const TDesC& aSubConParamsName);
	TInt CloseRSubConnectionParams(const TDesC& aSubConParamsName);
	RSubConParameterBundle* FindRSubConnectionParams(const TDesC& aSubConParamsName);
	CSubConParameterFamily* FindRSubConnectionParameterFamily(const TDesC& aSubConParamsName, const TInt aFamilyId);

    //subconnection events
    //(each event notification request is encapsulated in an "EventContainer"
    //that can be easily stored in a map.)
	TInt RegisterForRSubConnectionEvent(const TEventContainerParams& aParams);
	TInt ReceiveRSubConnectionEvent(TNotificationEventBuf*& aBufferPtr, const TDesC& aSubConEventName);


    // General
    //--------
	TInt CompareIntegerValues(const TCompareIntegersParams& aParams, TInt& aDifference);
	TInt AwaitRequestStatus(const TDesC& aStatusName);
	
    // Mobility
    //---------
    // Mobility notifications
    TInt RegisterForMobilityNotification(const TEventContainerParams& aParams);
	TInt ReceiveMobilityNotification(TDesC8*& aBufferPtr, const TEventContainerParams& aParams);

	TInt IgnorePreferredCarrier(const TMigrateToPreferredCarrierParams& aParams);
	TInt MigrateToPreferredCarrier(const TMigrateToPreferredCarrierParams& aParams);
	TInt NewCarrierAccepted(const TMigrateToPreferredCarrierParams& aParams);
	TInt NewCarrierRejected(const TMigrateToPreferredCarrierParams& aParams);

	// Availability
	//-------------
	// Availability notifications
	TInt RegisterForAvailabilityNotification(const TEventContainerParams& aParams);
	TInt ReceiveAvailabilityNotification(TDesC8*& aBufferPtr, const TDesC& aEventName);
	TInt CheckConnectionForAvailability(TDesC8*& aBufferPtr, const TCheckAvailabilityParams& aParams);

	// Availability simulation
	TInt ChangeBearerAvailability(const TChangeBearerAvailabilityParams& aParams);
	TInt CreateBearerAvailabilitySimulator(const TCreateBearerAvailabilitySimulatorParams& aParams);
	TInt TransformData(TDes8& dataIn, TDes8& dataOut, TSocketDataTransform transform);

	// Connection Server (RConnectionServ)
	//------------------------------------
	TInt CreateConnectionServer(const TDesC& aConnectionServName);
	TInt ConnectConnectionServer(const TDesC& aConnectionServName, TUint aTierId);
	TInt CloseConnectionServer(const TDesC& aConnectionServName);
	TInt ConnectionAccessPointStatusQuery(
		const TDesC& aConnectionServName,
		const ConnectionServ::CConnectionServParameterBundle& aQuery,
		ConnectionServ::CConnectionServParameterBundle& aResult);
	TInt CancelAccessPointStatusQuery(
		const TDesC& aConnectionServName,
		const ConnectionServ::CConnectionServParameterBundle& aQuery,
		ConnectionServ::CConnectionServParameterBundle& aResult);

	TInt GetConnectionNotificationAPI(const TDesC& aConnectionManagerExtensionName);
	TInt CreateRConnectionManager(const TPtrC& aServerName);
	TInt GetRConnMgrBearerCount(const TGetRConnMgrBearerCountParams& iParams, TInt& count);
	TInt OpenRConnectionManager(const TPtrC& aName);
	TInt SelectRConnectionManagerTier(TSelectRConnectionManagerTierParams& iParams);
	TInt CancelConnectionNotification(TCancelConnectionNotificationParams& aParams);

    //clean all
    void CleanAll();

	// Trigger simtsy events
	TInt EtelRequestL(TUint aEventKey, TInt aNewValue);

public:
    //store the elements in maps (look-up-able by name)
    RPointerDesMap<RSocketServ> iSockServs;
    RPointerDesMap<RSocket> iSocks;
	RPointerDesMap<RConnection> iConns;
	RPointerDesMap<RSubConnection> iSubConns;
	RPointerDesMap<RSubConParameterBundle> iSubConnParams;
	RPointerDesMap<TEsockTestEventContainer> iSubConnEvents;
	RPointerDesMap<TEsockTestEventContainer> iConnEvents;
	RPointerDesMap<TFlowId> iFlowId;
	RPointerDesMap<ConnectionServ::RConnectionServ> iConnectionServs;
	RPointerDesMap<CConnServNotificationWatcher> iNotifWatchers;
	RPointerDesMap<TInt> iIntegerValues;
	RPointerDesMap<TRequestStatus> iRequestStatuses;

	// API extensions
	RPointerDesMap<RCommsMobilityApiExt> iMobilityExts;

	TSimTsy iSimTsy;

	// Data Monitoring
	RPointerDesMap<RCommsDataMonitoringApiExt> iDataMonitoringExts;
	RPointerDesMap<TEsockTestDataMonitoringResponse> iDataMonitoringEvents;
	RPointerDesMap<TDataMonitorStats> iSavedDataMonitorStats;
   };

#endif //ESOCKTESTBASE_H

