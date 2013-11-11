// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BTTESTCONSOLE_H_
#define BTTESTCONSOLE_H_

#include <e32std.h>
#include <e32cons.h>
#include <e32base.h>
#include <e32test.h>
#include <es_sock.h>
#include <bttypes.h>
#include <bluetooth/pairing.h> 

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <bthci.h>
#endif


#include <bluetooth/btpowercontrol.h>


#include "BTTestHelpers.h"

const TInt KExplicitActiveMode = 0x123456;

class CBTTestConnection;
class CActiveConsole;
class TBTDevAddr;

class TBTTestBuffer
	{
public:
	enum
		{
		EMaxBufferSize = 20,
		};

	TBTTestBuffer();

	void Reset();
	TInt Length() { return iLength; }

	TBool DecGetValue(TInt64 & aValue);
	TBool HexGetValue(TInt64 & aValue);

	TBool Add(TChar aKey, TInt aMaxLength = EMaxBufferSize);
	TBool AddNumber(TChar aKey, TInt aMaxLength = EMaxBufferSize);
	TBool AddHexNumber(TChar aKey, TInt aMaxLength = EMaxBufferSize);

	void SetValue(TInt64 aValue) { iValue1 = aValue; };
	TInt64 Value() const { return iValue1; };
	void SetValue2(TInt64 aValue) { iValue2 = aValue; };
	TInt64 Value2() const { return iValue2; };

	TChar At(TInt aIndex);

	void RemoveChar();
	
	void WriteIntToBuffer(TInt64 aValue);
	void WriteTDesC8ToBuffer(const TDesC8& aDes);
	void PrintBuffer(CConsoleBase& aConsole);

private:
	TChar iInputBuffer[20];
	TInt iLength;
	TInt64 iValue1;
	TInt64 iValue2;
	};

class MMenuUser
	{
public:
	virtual TBool ChildMenuClosed() = 0;
	};

class TInputProcessorBase : public MMenuUser
	{
public:
	enum InputState
		{
		EMenuContinue,
		EMenuComplete,
		};

	virtual ~TInputProcessorBase() {};
	static TInt HandleKeyPress(TAny* aInstance);

	TBool MenuClosed();
	TBool ChildMenuClosed();

	virtual TInt ProcessKey(TChar aKey) = 0;
	virtual void ProcessChildMenuClosed() = 0;
	virtual void Draw() = 0;

	TChar GetKey();
	CConsoleBase& iConsole;

protected:
	TInputProcessorBase(MMenuUser& aParent, TCallBack& aCb, CConsoleBase& aConsole);

	MMenuUser& iParent;
	TCallBack& iCb;
	};

class TInputBTConnDetails;
class XInputMainMenu : public TInputProcessorBase
	{
	public:
		enum SubMenus
			{
			ENone,
			EConnect,
			EListen,
			EBAPConnect,
			ESCOConnect,
			EEchoRequest,
			EPacketTypes,
			EL2CapAPIConfig,
			EListEvents,
			EDebugMenu,
			EDataMenu,
			ERFCommMenu,
			EPhysicalLinkMenu,
			EeSCOConnect,
			EHciServer
			};

	XInputMainMenu(CBTTestConsole & aTestConsole, TCallBack & aCb, CConsoleBase & aConsole);
	~XInputMainMenu();
	TInt ProcessKey(TChar aKey);
	void Draw();

	TInt GetSelectedConn() const { return iSelectedConnection; };
	void SetSelectedConn(TInt aConnIx) { iSelectedConnection = aConnIx; };
	TBool SelectConnection(TInt aConnIx);
	void ProcessChildMenuClosed();
	
private:
	void DeleteSubMenuIfAppropriate();
	TInt ProcessKeyL(TChar aKey);

private:
	CBTTestConsole & iConsoleApp;
	TInt iSelectedConnection;

	SubMenus iCurrentSubMenu;
	TInputProcessorBase* iSubMenu;
	
	// Pointers to store previous connection details in a (slightly) extensible way.
	// If more are needed then follow the special handling pattern in the destructor
	// and DeleteSubMenuIfAppropriate().
	TInputBTConnDetails* iLastConnectDetails;
	TInputBTConnDetails* iLastLegacyConnectDetails;
	TInputBTConnDetails* iLastListenDetails;
	TInputBTConnDetails* iLastLegacyListenDetails;
	TInputBTConnDetails* iLastBapDetails;
	TInputBTConnDetails* iLastEScoDetails;
	TInputBTConnDetails* iLastScoDetails;
	};

class TInputPacketTypes : public TInputProcessorBase
	{
public:
	TInputPacketTypes(TInputProcessorBase & aParent, TCallBack & aCb, CConsoleBase & aConsole);
	TInt ProcessKey(TChar aKey);
	void ProcessChildMenuClosed() {};
	void Draw();

	TUint16 GetPacketTypes();
private:
	TBool iTypes[6];
	TInt iCurrentType;
	};

class TSubMenu : public TInputProcessorBase
	{
public:
	TSubMenu(TInputProcessorBase& aParent, TCallBack& aCb, CConsoleBase& aConsole);
	void ProcessChildMenuClosed() {};
	TInt ProcessKey(TChar aKey);
		
	TUint16 GetInfoType() const;
	TUint16 GetBasicValue() const;

	virtual TInt SubMenuProcessKey(TChar aKey) = 0;	
	
protected:
	enum TSubMenuState
		{
		EBasicMenu,
		EGetBasicValue,
		//Ready to get a choice from user
		EChoiceValue,
		//Get a choice from user
		EGetChoiceValue,
		};

	TUint16 iInfoType;

private:
	TSubMenuState iState;
	TUint16 iValue;

	TBTTestBuffer iBuffer;
	};


class TDataMenu : public TSubMenu
	{
public:
	enum TDataMenuType
		{
		EInvalidDataMenuType,
		ESetIncomingMTU,
		ESetOutgoingMTU,
		ERecvOneShotData,
		ESendOneShotData,
		EDataDlgRecv,
		EDataDlgSend,
		ESendData,
		ERecvData,
		ERecvAnyData,
		ESimultanousSendReceiveData,
		ERecvContinuationData,
		ERTXTimer,
		EERTXTimer,
		EPretendSduQFullOn,
		EPretendSduQFullOff,
		EGetChannelMode,
		EGetLocalPSM
		};
		
	TDataMenu(TInputProcessorBase& aParent, TCallBack& aCb, CConsoleBase& aConsole);
	TInt SubMenuProcessKey(TChar aKey);
	void Draw();
	};

class TRFCommMenu : public TSubMenu
	{
public:
	enum TRFCommMenuType
		{
		EInvalidRFCommMenuType,
		ERequestRFCommConfChgInd,
		ESetLocalModemStatus,
		};
		
	TRFCommMenu(TInputProcessorBase& aParent, TCallBack& aCb, CConsoleBase& aConsole);
	TInt SubMenuProcessKey(TChar aKey);
	void Draw();
	};

class TPhysicalLinkMenu : public TSubMenu
	{
public:
	enum TPhysicalLinkMenuType
		{
		EInvalidPhysicalLinkMenuType,
		EActiveRequest,
		ESniffRequest,
		EParkRequest,
		EToggleSniffModeAllowed,
		EToggleParkModeAllowed,
		EToggleHoldModeAllowed,
		ERequestMasterRole,
		ERequestSlaveRole,
		EToggleRoleSwitchAllowed,
		EKillAllLinks,
		EKillLink,
		EGetConnectedLinks,
		EToggleLPMRequestsIfActive,
		EAuthenticationRequest,
		EToggleAutoSniff,
		EExplicitActiveRequest,
		EPhysicalLinkMetricsDisplay,
		EPhysicalLinkMetricsCancel,
		};
		
	TPhysicalLinkMenu(TInputProcessorBase& aParent, TCallBack& aCb, CConsoleBase& aConsole);
	TInt SubMenuProcessKey(TChar aKey);
	void Draw();
	};

#ifdef _DEBUG
class TDebugMenu : public TSubMenu
	{
public:
	enum TDebugMenuType
		{
		EInvalidDebugType,
		EAllocations,
		ESetPDUTimerCID,
		EGetPDUTimers,
		EGetFlushCounters,
		EIssueManualFlush,
		EGetCurrentDataIndex,
		ECurrentMemoryInfo,
		EDebugMark,
		EDebugMarkEnd,
		EDataPlaneConfig,
		};
		
	TDebugMenu(TInputProcessorBase& aParent, TCallBack& aCb, CConsoleBase& aConsole);
	TInt SubMenuProcessKey(TChar aKey);
	void Draw();
	};
#endif


class TInputAPIL2CapConfig : public TInputProcessorBase
	{
public:
	TInputAPIL2CapConfig(TInputProcessorBase & aParent, TCallBack & aCb, CConsoleBase & aConsole);
	TInt ProcessKey(TChar aKey);
	void ProcessChildMenuClosed() {};
	void Draw();

	TL2CapConfig& GetL2CapConfig();
private:
	enum TL2CapConfigInputState
		{
		ETransmitMTU,
		ETransmitMinMTU,
		EReceiveMTU,
		EReceiveMinMTU,
		EChannelPriority,
		EChannelReliability,
		EReliabilityTimer,
		ESDUFlushTimer,
		};

	void UpdateReliabilityTimer(TUint16 aTimer);
	void UpdateObsolescenceTimer(TUint16 aTimer);
		
	TUint8 iInputState;
	TBTTestBuffer iBuffer;
	TL2CapConfig iL2CapConfig;
	};


class TInputListEvents : public TInputProcessorBase
	{
public:
	enum
		{
		EEventsPerPage = 10,
		};

	TInputListEvents(CBTTestConnection & aConn, TInputProcessorBase & aParent, TCallBack & aCb, CConsoleBase & aConsole);
	TInt ProcessKey(TChar aKey);
	void ProcessChildMenuClosed() {};
	void Draw();

	TUint16 GetPacketTypes();
private:
	CBTTestConnection & iConn;
	TInt iCurrentEvent;
	TBool iComplete;
	};

class TInputEchoRequest : public TInputProcessorBase
	{
public:
	TInputEchoRequest(TInputProcessorBase & aParent, TCallBack & aCb, CConsoleBase & aConsole);
	TInt ProcessKey(TChar aKey);
	void ProcessChildMenuClosed() {}
	void Draw();
	TInt GetEchoRequestLength()			{ return iEchoRequestLength; }

private:	
	TInt NextInput();
	TBTTestBuffer iBuffer;	
	TInt iEchoRequestLength;
	};

class TInputBTConnDetails : public TInputProcessorBase
	{
public:
	enum RequiredDetails
		{
		EDone			= 0,
		EPortNumber		= 0x01,
		EProtocol		= 0x02,
		EAddress		= 0x04,
		ESecurity		= 0x08,
		EL2CAPConfig	= 0x10,
		ELegacySecurity	= 0x20,
		EMaxDetails		= 7
		};

	TInputBTConnDetails(TUint aRequiredDetails, TInputProcessorBase & aParent, TCallBack & aCb, CConsoleBase & aConsole, TInputBTConnDetails* aPreviousDetails);
	TInt ProcessKey(TChar aKey);
	void ProcessChildMenuClosed() {}
	void Draw();

	TUint16 GetPort() const				{ return iPort;				};
	TChar GetProtocol() const			{ return iProtocol;			};
	TBTDevAddr& GetAddress()			{ return iAddress;			};
	TBTServiceSecurity& GetSecurity()	{ return iSecurity;			};
	TBool UseDefaultConfig() const		{ return iUseDefaultConfig;	};

private:
	enum TSecuritySetting
		{
		EAuthenticate,
		EAuthorised,
		EEncrypt,
		EDenied,
		EPasskeyMinLength,
		};

	void PrintUserPrompt();
	TInt CollectPortNumber(TChar aKey);
	TInt CollectProtocol(TChar aKey);
	TInt CollectAddress(TChar aKey);
	TInt CollectSecurity(TChar aKey);
	TInt CollectPasskeyMinLength(TChar aKey);
	TInt CollectConfig(TChar aKey);
	TBool UpdateSecurity(TChar aSetting);

	TInt NextInput();
	void SetupAnyPreviousValues();
	TBool HandleBackspace(TChar aKey);
	void ConsoleBackspace(TUint aNumberOfChars);
	TChar GetSecurityDefault();
	void PrintSecurityDefault();

	TBTTestBuffer iBuffer;

	TChar iProtocol;
	TUint16 iPort;
	TBTDevAddr iAddress;
	TBTServiceSecurity iSecurity;
	TBool iUseDefaultConfig;
	TBool iUseLegacySecurity;
	
	TBool iUsePreviousDetails;
	TChar iPreviousProtocol;
	TUint16 iPreviousPort;
	TBTDevAddr iPreviousAddress;
	TBTServiceSecurity iPreviousSecurity;
	TBool iPreviousUseDefaultConfig;

	TUint iRequiredDetails[EMaxDetails];
	TUint iCurrentDetail;
	TUint iCurrentSecuritySetting;
	};

class THciServerDetails : public TInputProcessorBase
	{
public:
	THciServerDetails(TInputProcessorBase & aParent, TCallBack & aCb, CConsoleBase & aConsole);
	~THciServerDetails();
	TInt ProcessKey(TChar aKey);
	void ProcessChildMenuClosed() {}
	void Draw();

private:
	void FormatVendorCmd(TDes8& aCmd);
	
private:
	RBTPowerControl iPowerControl;
	RHCIDirectAccess iHCIDirectAccess;
	TBuf8<255> iVendorCommand;
			
	TInt iMenuPos;
	};

	
		

class CBTTestConsole : public CBase, public MMenuUser
	{
public:
	static CBTTestConsole* NewL();
	static CBTTestConsole* NewLC();
	CBTTestConsole();
	virtual ~CBTTestConsole();
	void Start();
	void StartL();

	TBool ChildMenuClosed();
	CBTTestConnection* GetConn(TInt aIndex);

	TInt NewConnection();
	TInt NumberOfConnections();
	TBool IsActiveConnection(TInt aIx);
	TBool ConnInState(TInt aIx, TConnProfile::TConnState aState);
	TBool IsConnSocketBased(TInt aIx);

	void CreatePhyRecord(const TBTDevAddr & aBTAddr);
	TConnProfile* FindPhyRecord(const TBTDevAddr & aBTAddr);

	void Draw();
	void DrawAddress();
	void DrawConnections();
	void DrawConnection(TInt aIx);
	TBool ProcessKey();

	// MAIN MENU METHODS
	TInt Connect(TBTDevAddr aAddr, TUint16 aPort, TChar aProtocol, TBTServiceSecurity & aSecurity, TBool aUseDefaultConfig);
	TInt BAPConnect(TBTDevAddr aAddr);
	TInt Listen(TUint16 aPort, TChar aProtocol, TBTServiceSecurity & aSecurity, TBool aUseDefaultConfig);
	TInt Accept(TInt aConnIx);

	TInt SendEchoRequest(TInt aPayloadSize);
		
	TInt ChangePacketTypes(TUint16 aPacketTypes);
	TInt AllowEDRPackets(TBool aEnable);
	
	TInt SetL2CapAPIConfig(TL2CapConfig& aL2CapAPIConfig);
	TL2CapConfig& L2CapAPIConfig();
	TInt ConfigureLink(TBool aUseIoctl);

	TInt SetEncryption(TBool aOn);
	TInt Authenticate();

	TInt PassiveSCO();
	TInt ActiveSCO(TBTDevAddr& aAddr);
	
	TInt PassiveESCO();
	TInt ActiveESCO(TBTDevAddr& aAddr);

	TInt ShutdownSocket(RSocket::TShutdown aShutdownType);
	TInt CloseAndDelete();

	void CleanAllResources();
	TInt RegisterSDPRecordL();
	TInt SetHostName();
	TInt GetRemoteHostName(TInt aSelectedConnection);
	TInt ReadLocalOobData();
	void RefreshLocalOobData();
	TInt ProvideRemoteOobData();
	TInt ProvideRemoteOobDataFromRNR();

	TInt DedicatedBond();
	
	TInt IncreaseRTXTimer(TUint aTime);
	void SetDiscoverability(TBool aLimitedDiscoverability);
	void SetScanMode(THCIScanEnable aScanMode);
	void SetPairedConnectionsOnly(TBool aPairedConnectionsOnly);
	void SetDebugMode();

	// MISC METHODS
	void ServerDisconnect(TInt aConnIx);
	void StopProtocols();
	void InitialiseL();

	// Sub menu handlers.
#ifdef _DEBUG
	// This menu is for udeb builds only
	TBool DebugMenu(TDebugMenu::TDebugMenuType aInfoType, TUint16 aBasicValue);
#endif

	TBool DataMenu(TDataMenu::TDataMenuType aInfoType, TUint16 aBasicValue);
	TBool RFCommMenu(TRFCommMenu::TRFCommMenuType aInfoType, TUint16 aBasicValue);
	TBool PhysicalLinkMenu(TPhysicalLinkMenu::TPhysicalLinkMenuType aInfoType, TUint16 aBasicValue);

public:
	// PUBLIC DATA.	
	RTest test;
	TBool iLPMRequestsIfActive;

private:
// ----------------------------------------------------------------------------
//							DEBUG METHODS
// ----------------------------------------------------------------------------
#ifdef _DEBUG
	TInt DefinePublications();
	void DeletePublications();

	TInt DebugAllocations();
	TInt DebugGetPDUTimers();
	TInt DebugGetFlushCounters();
	TInt IssueManualFlush();
	TInt GetCurrentDataIndex();
	TInt CurrentMemoryInfo();
	TInt DataPlaneConfig();
	void DebugMark();
	void DebugMarkEnd();
#endif

// ----------------------------------------------------------------------------
//							DATA MENU METHODS
// ----------------------------------------------------------------------------
	TInt SendMTUIoctl(TBool aIsIncomingMTU, TUint16 aNewValue);
	TInt SetPretendIncomingSduQFull(TBool aPretendIncomingSduQFull);
	TInt RecvOneShotData();
	TInt SendOneShotData(TUint16 aDataSize);
	TInt ToggleDataDialog(TConnProfile::TDataState aDataType);
	TInt RecvContinuationData(TUint16 aDataSize);
	TInt GetChannelMode();
	TInt GetLocalPSM();
	TBool ConnectionUsable(TInt aConn);

// ----------------------------------------------------------------------------
//							RFCOMM MENU METHODS
// ----------------------------------------------------------------------------
	TInt RequestRFCommConfChgInd();
	TInt SetLocalModemStatus(TUint16 aModemStatus);
	
// ----------------------------------------------------------------------------
//							PHYSICAL LINK MENU METHODS
// ----------------------------------------------------------------------------
	TInt ActiveRequest();
	TInt SniffRequest();
	TInt ParkRequest();
	TInt ExplicitActiveRequest();

	TInt ToggleSniffModeAllowed();
	TInt ToggleParkModeAllowed();
	TInt ToggleHoldModeAllowed();

	TInt RequestRoleSwitch(TBTBasebandRole aRole);
	TInt ToggleSwitchRoleAllowed();
	TInt ToggleLPMRequestsAllowed();
	
	TInt AuthenticationRequest();
	TInt PlmDisplay();
	TInt PlmCancel();
	
	TInt GetConnectedPhyLinks();

	TInt KillAllLinks();
	TInt KillLink();
	
	TInt ToggleAutoSniff();

// ------------------------------------------------------------------------------
	TInt MakeModeRequest(TConnProfile::TConnMode aMode);
	void CleanupPhyLinks();
		
private:
	XInputMainMenu* 					iMainMenu;

	CActiveConsole* 					iActiveConsole;
	RSocketServ 						iSocketServer;

	RPointerArray<CBTTestConnection> 	iTestConn;
	RArray<TConnProfile> 				iPhyLinks;

	TCallBack 							iInputCallback;
	
	TL2CapConfig 						iL2CapAPIConfig;
	
	RProperty 							iProperty;
	RHostResolver						iHostResolver;

	TBool								iPairingServerAvailable;
	RBluetoothPairingServer				iPairingServer;
	RBluetoothOobData					iOobData;
	};

// Basic console application class.
class CActiveConsole : public CActive
	{
public:
	static CActiveConsole* NewL(CBTTestConsole& aTestConsole);

	void RunL();
	void DoCancel();

	~CActiveConsole();

private:
	CActiveConsole(CBTTestConsole& aTestConsole);
	void ConstructL();
	void RequestKey();

private:
	CBTTestConsole& iTestConsole;
	};

#endif
