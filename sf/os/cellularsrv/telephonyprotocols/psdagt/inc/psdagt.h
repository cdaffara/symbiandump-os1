/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Header for Psd State Machines
* 
*
*/



/**
 @file PSDAGT.H
 @internalComponent
*/

#ifndef __PSDAGT_H__
#define __PSDAGT_H__

#include <etelpckt.h>
#include <etelqos.h>
#include <pcktcs.h>
#include <comms-infras/cagentsmbase.h>
#include <comms-infras/dbaccess.h>

#include "psdpanic.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <comms-infras/nifprvar_internal.h>
#endif

#define INCOMING_NOT_SUPORTED 1	///< Comment out to enable incoming connections

//class CCommsDbPsdAccess;
class CEventLogger;
class CDataLogger;
class CPsdContextConfig;
class CPsdContextQoSConfig;

class MPsdEnv
/** 
This class defines the interface  for a set of utility functions which must
be implemented by a derived class providing those services

@internalComponent
*/
	{
public:
	/**
	Network modes
	*/
	enum TNetworkMode
		{
		/** Unknown  Mode
		*/
		EModeUnknown,
		/** WCDMA Mode - 3GPP family i.e. GSM, GPRS, UMTS
		*/
		EModeWCDMA
		};
public:
	/**
	Provides a reference to the class that provides the base environment functions

	@return MAgentStateMachineEnv& the base environment class
	*/
	virtual MAgentStateMachineEnv& BaseEnv()=0;

	/**
	Provides a reference to a centrally held RTelServer handle

	@return RTelServer& the RTelServer handle
	*/
	virtual RTelServer& TelServer()=0;

	/**
	Provides a reference to a centrally held RPhone handle

	@return RPhone& the RPhone handle
	*/
	virtual RPhone& Phone()=0;

	/**
	Provides a reference to a centrally held RPacketService handle

	@return RPacketService& the RPacketService handle
	*/
	virtual RPacketService& PacketNetwork()=0;

	/**
	Provides a reference to a centrally held RPacketContext handle

	@return RPacketContext& the RPacketContext handle
	*/
	virtual RPacketContext& Context()=0;

	/**
	Function to allow clients to inquire if QoS is supported.

	@return TBool flag indicating if QoS is supported.
	*/
	virtual TBool  IsQoSSupported()const=0;

	/**
	Function to allow clients to set if QoS is supported.

	@param TBool isSupported reference to if QoS is supported or not.
	*/
	virtual void  SetQoSSupported(TBool isSupported)=0;

	/**
	Provides a reference to a centrally held RPacketQoS handle

	@return RPacketQoS& the RPacketQoS handle
	*/
	virtual RPacketQoS& QoS()=0;

	/**
	Provides a reference to a centrally held context config parameters object

	@return CPsdContextConfig& the context config parameter object
	*/
	virtual	const CPsdContextConfig& Config() const =0;

	/**
	Provides a reference to a centrally held QoS config parameters object

	@return CPsdContextQoSConfig& the QoS config parameter object
	*/
	virtual	const CPsdContextQoSConfig& QoSConfig() const =0;

	/**
	Provides a pointer to a centrally held CEventLogger object

	@return CEventLogger* the event logger object
	*/
	virtual CEventLogger* Logger()=0;

	/**
	Provides a pointer to a centrally held CDataLogger object

	@return CDataLogger* the data logger object
	*/
	virtual CDataLogger* DataLogger()=0;

	/**
	Provides a reference to a centrally held TInt that allows clients
	to set or get the cause of the last error

	@return TInt& reference to the last error
	*/
	virtual TInt& ErrorCause() =0;

	/**
	Function indicates the direction of the connection which is in progress

	@return TCommDbConnectionDirection the connection direction
	*/
	virtual TCommDbConnectionDirection Direction()=0;

	/**
	Function to allow clients to set whether logging is in progress 
	and also to inquire if logging is in progress

	@return TBool& reference to the Flag indicating if logging in progress or not
	@deprecated Depracated since logging requests are stored in a queue now
	*/
	virtual TBool& AmLogging() =0;

	/**
	Function to allow clients to inquire whether the TSY has been loaded 

	@return TBool Flag indicating if the TSY has been loaded or not
	*/
	virtual TBool TsyLoaded() const =0;

	/**
	Function to allow clients to set whether the TSY has been loaded or not

	@param TBool aStatus Flag indicating if the TSY has been loaded or not
	*/
	virtual void SetTsyLoaded(TBool aStatus)=0;

	/**
	Function to allow clients to inquire the mode of the current connection

	@return TNetworkMode the current mode
	*/
	virtual TNetworkMode Mode() const =0;
		};


class MPsdCommDbAccess
/**
This class defines the interface  for a set of database enquiry functions which must
be implemented by a derived class providing those services

@internalComponent
*/
	{
public:
	/**
	Function to allow clients to inquire if incomming connections are allowed

	@return TBool flag indicating if incomming connections allowed
	*/
	virtual TBool IsIncomingAllowedL() const =0;

	/**
	Function to allow clients to inquire the name of the remote party in the connection

	@param TDes& aRemoteParty reference to a descriptor into which the remote party name will be written.
	*/
	virtual void GetRemotePartyL(TDes& aRemoteParty) const=0;
	};

class CAgentSMBase;

class CPsdAgentSMBase : public CAgentSMBase, public MPsdEnv, public MPsdCommDbAccess
/**
CPsdAgentSMBase is the base class for Psd state machines, both inbound and outbound.
It contains data which must be persistent for the lifetime of the SM, and accessor functions
for the state objects to use.

@internalComponent
*/
	{
public:
	// These virtuals are from MPsdEnv

	virtual MAgentStateMachineEnv& BaseEnv();
	virtual RTelServer& TelServer();
	virtual RPhone& Phone();
	virtual RPacketService& PacketNetwork();
	virtual RPacketContext& Context();
	virtual TBool IsQoSSupported()const;
	virtual void SetQoSSupported(TBool isSupported);
	virtual RPacketQoS& QoS();
	virtual const CPsdContextConfig& Config() const;
	virtual const CPsdContextQoSConfig& QoSConfig() const;
	virtual TInt& ErrorCause();
	virtual CEventLogger* Logger();
	virtual CDataLogger* DataLogger();
	virtual TBool& AmLogging();  //* Depracated since logging requests are stored in a queue now
	virtual TBool TsyLoaded() const;
	virtual void SetTsyLoaded(TBool aStatus);
	virtual TNetworkMode Mode() const;

	// These virtuals are from MPsdCommDbAccess
	virtual TBool IsIncomingAllowedL() const;
	virtual void GetRemotePartyL(TDes& aRemoteParty) const;

	// These virtuals are from CAgentSMBase
	virtual TInt GetExcessData(TDes8& aBuffer);
	virtual TInt Notification(TNifToAgentEventType aEvent, TAny* aInfo);
	virtual void GetLastError(TInt& aError);

protected:
	CPsdAgentSMBase(MAgentNotify& aObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess);
	virtual ~CPsdAgentSMBase();
	void ConstructL();
private:
	void InquireModeL();
private:
	RTelServer iTelServer;
	TBool iTsyLoaded;
	RPhone iPhone;
	RPacketService iPacketNetwork;
	RPacketContext iContext;
	RPacketQoS iQoS;
	CPsdContextConfig* iConfig;
	CPsdContextQoSConfig* iQoSConfig;
	CEventLogger* iEventLogger;
	CDataLogger* iDataLogger;
	TBool iAmLogging;  	 //* Depracated since logging requests are stored in a queue now
	TInt iErrorCause;
	TNetworkMode iNetworkMode;
	TBool iIsQoSSupported;
	};

	
class CPsdOutSM : public CPsdAgentSMBase
/**
CPsdOutSM is the outbound state machine. It instantiates the first state appropriate to
this SM, and implements the Direction() function so states can discover which SM they are
currently part of, outbound or inbound.

@internalComponent
*/
	{
public:
	static CPsdOutSM* NewL(MAgentNotify& aObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess);
	virtual ~CPsdOutSM();
	virtual TCommDbConnectionDirection Direction();
protected:
	void ConstructL();
	CPsdOutSM(MAgentNotify& aObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess);
	};


#ifndef INCOMING_NOT_SUPORTED

class CPsdInSM : public CPsdAgentSMBase
/**
CPsdInSM is the inbound state machine.

@internalComponent
*/
	{
public:
	static CPsdInSM* NewL(MAgentObserver& aObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess);
	virtual ~CPsdInSM();
	virtual TCommDbConnectionDirection Direction();
protected:
	void ConstructL();
	CPsdInSM(MAgentObserver& aObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess);
	};
#endif // #ifndef INCOMING_NOT_SUPORTED


class CPsdContextConfig : public CBase
/**
CPsdContextConfig encapulates the context config parameters

@internalComponent
*/
	{
public:
	static CPsdContextConfig* NewL(CCommsDbAccess& aDbAccess, MPsdEnv::TNetworkMode aMode);
	~CPsdContextConfig();
public: // Access functions
	const TPacketDataConfigBase& Config() const;
	TDesC8& PackedConfig() const;
	TBool IsConfigSupportedL(const RPacketService& aPacketData, TCommDbConnectionDirection aDirection) const;
	TBool TsySupportsDataTransferInfoL(const RPacketService& aPacketData) const;
	TBool QueryIfConfigAcceptable(TDesC8& aNewConfig) const;
	TBool QueryIfIncommingConnectionAcceptable(const RPacketContext::TProtocolType& aProtocolType, const RPacketContext::TProtocolAddress& aProtocolAddress) const;
private: // Creational functions
	CPsdContextConfig(CCommsDbAccess& aDbAccess, MPsdEnv::TNetworkMode aMode);
	void ConstructL();
private: // internal functions
	void GprsConfigConstructL();
	void GetGprsCompressionL(TUint& aCompression) const;
	void GetGprsAAL(RPacketContext::TAnonymousAccess& aAnonymous) const;
	void GetGprsEdgeL(TBool& aUseEdge) const;
	TBool QueryIfGprsConfigAcceptable(const RPacketContext::TContextConfigGPRS& aNewConfig) const;
private:
	CCommsDbAccess& iDbAccess;
	MPsdEnv::TNetworkMode iMode;
	TPacketDataConfigBase* iConfig;
	TPtr8* iPackedConfig;
	mutable TUint iStaticCaps;
	RPacketContext::TProtocolType iProtocolType;
	};


class CPsdContextQoSConfig : public CBase
/**
CPsdContextQoSConfig encapulates the QoS config parameters

@internalComponent
*/
	{
public:
	static CPsdContextQoSConfig* NewL(CCommsDbAccess& aDbAccess, MPsdEnv::TNetworkMode aMode);
	~CPsdContextQoSConfig();
public: // Access functions
	const TPacketDataConfigBase& QoSConfig() const;
	TDes8& PackedQoSConfig() const;
	TBool QueryIfQoSAcceptable(TDesC8& aNewQoS) const;
private: // Creational functions
	CPsdContextQoSConfig(CCommsDbAccess& aDbAccess, MPsdEnv::TNetworkMode aMode);
	void ConstructL();
private: // internal functions
	void GprsQoSConfigConstructL();
	TBool QueryIfGprsQoSAcceptable(const RPacketQoS::TQoSGPRSNegotiated& aNewQoS) const;
private:
	CCommsDbAccess& iDbAccess;
	MPsdEnv::TNetworkMode iMode;
	TPacketDataConfigBase* iQoSConfig;
	TPtr8* iPackedQoSConfig;
	};


class CPsdContextConfigChangeWatcher : public CActive
/**
This class is responsible for being notified of changes in the context
configuration parameters and notifying its observer when such a change
occurs

@internalComponent
*/
	{
public:
	class MObserver
		{
	public:
		virtual void ReportConfigChange()=0;
		virtual void ReportConfigWatcherError(TInt aError)=0;
		};
public:
	CPsdContextConfigChangeWatcher(const RPacketContext& aContext, MObserver& aObserver, TPtr8& aContextParams);
	void StartL();
	~CPsdContextConfigChangeWatcher();
protected:
	void RunL();
	void DoCancel();	
private:
	MObserver& iObserver;
	const RPacketContext& iContext;
	TPtr8& iContextParams;
	};


class CPsdQoSConfigChangeWatcher : public CActive
/**
This class is responsible for being notified of changes in the QoS
configuration parameters and notifying its observer when such a change
occurs

@internalComponent
*/
	{
public:
	class MObserver
		{
	public:
		virtual void ReportQoSChange()=0;
		virtual void ReportQoSWatcherError(TInt aError)=0;
		};
public:
	CPsdQoSConfigChangeWatcher(const RPacketQoS& aQoS, MObserver& aObserver, TPtr8& aQoSParams);
	void StartL();
	~CPsdQoSConfigChangeWatcher();
protected:
	void RunL();
	void DoCancel();	
private:
	MObserver& iObserver;
	const RPacketQoS& iQoS;
	TPtr8& iQoSParams;
	};


class MPsdContextChangeObserver
/**
This class defines the callback functions that must be implemented by an
observer of a CPsdContextChangeMonitor object

@internalComponent
*/
	{
public:
	virtual void ReportContextBelowAcceptableQuality() = 0;
	virtual void ReportError(TInt aError)=0;
	};

class CPsdContextChangeMonitor : public CActive, public CPsdContextConfigChangeWatcher::MObserver, public CPsdQoSConfigChangeWatcher::MObserver
/**
This class is reponsible for notifying clients when the quality of the
Context or QoS falls below the minimum required values

@internalComponent
*/
	{
public:
	static CPsdContextChangeMonitor* NewL(MPsdEnv* aEnv, MPsdContextChangeObserver* aObserver);
	~CPsdContextChangeMonitor();
	// callbacks
	virtual void ReportConfigChange();
	virtual void ReportConfigWatcherError(TInt aError);
	virtual void ReportQoSChange();
	virtual void ReportQoSWatcherError(TInt aError);
protected:
	void RunL();
	void DoCancel();
private:
	CPsdContextChangeMonitor(MPsdEnv* aEnv, MPsdContextChangeObserver* aObserver);
	void ConstructL();
	void StartTimeout();
	void CancelTimeout();
private:
	enum TContextState
		{
		EContextQualityAcceptable = 0x0,
		EConfigQualityUnAcceptable = 0x1,
		EQoSQualityUnAcceptable = 0x2
		};
private:
	CPsdContextConfigChangeWatcher* iConfigWatcher;
	CPsdQoSConfigChangeWatcher* iQoSWatcher;
	TPacketDataConfigBase* iActualConfig;
	TPtr8* iPackedActualConfig;
	TPacketDataConfigBase* iActualQoSConfig;
	TPtr8* iPackedActualQoSConfig;
	MPsdEnv* iSM;
	MPsdContextChangeObserver* iObserver;
	TUint iContextState;
	TTimeIntervalMicroSeconds32 iDebounceTimeout;
	RTimer iDebounceTimer;
	};

#endif
