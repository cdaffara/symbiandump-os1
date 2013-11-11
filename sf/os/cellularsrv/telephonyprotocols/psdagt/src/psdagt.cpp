// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file PSDAgt.cpp
*/

#include <cdbcols.h>

#include "psdagt.h"
#include "psdstates.h"
#include "psdlogger.h"

CPsdOutSM* CPsdOutSM::NewL(MAgentNotify& aObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess)
/**
Function creates an outbound connection state machine

@param MAgentObserver& aObserver an observer which will be called back to notify changes in the connection
@param CDialogProcessor* aDlgPrc a pointer to a dialog processor object for use by the state machine
@param CCommsDbAccess& aDbAccess a reference to the comm Db access object to allow the state machine to obtain details from CommDb
@return CPsdOutSM* the newly created object (ownership of the object is passed to the function caller)
*/
	{
	__ASSERT_DEBUG(aDlgPrc,User::Invariant());
	CPsdOutSM* sm = new (ELeave) CPsdOutSM(aObserver,aDlgPrc,aDbAccess);
	CleanupStack::PushL(sm);
	sm->ConstructL();
	CleanupStack::Pop();
	return sm;
	}

CPsdOutSM::CPsdOutSM(MAgentNotify& aObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess)
	: CPsdAgentSMBase(aObserver,aDlgPrc,aDbAccess)
/**
Constructor

@param MAgentObserver& aObserver an observer which will be called back to notify changes in the connection
@param CDialogProcessor* aDlgPrc a pointer to a dialog processor object for use by the state machine
@param CCommsDbAccess& aDbAccess a reference to the comm Db access object to allow the state machine to obtain details from CommDb
*/
	{}

CPsdOutSM::~CPsdOutSM()
/**
Destructor
*/
	{}

void CPsdOutSM::ConstructL()
/**
2nd phase construction of the outgoing state machine
creates the initial state of the state machine
*/
	{
	CPsdAgentSMBase::ConstructL();
	iState = new (ELeave) CPsdOutInit(this,this,this);
	}

TCommDbConnectionDirection CPsdOutSM::Direction()
/**
Function indicates the direction of the connection which is in progress

@return TCommDbConnectionDirection the connection direction
*/
	{ return ECommDbConnectionDirectionOutgoing;}

#ifndef INCOMING_NOT_SUPORTED

CPsdInSM* CPsdInSM::NewL(MAgentObserver& aObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess)
/**
Function creates an inbound connection state machine

@param MAgentObserver& aObserver an observer which will be called back to notify changes in the connection
@param CDialogProcessor* aDlgPrc a pointer to a dialog processor object for use by the state machine
@param CCommsDbAccess& aDbAccess a reference to the comm Db access object to allow the state machine to obtain details from CommDb
@return CPsdInSM* the newly created object (ownership of the object is passed to the function caller)
*/
	{
	__ASSERT_DEBUG(aDlgPrc,User::Invariant());
	CPsdInSM* sm = new (ELeave) CPsdInSM(aObserver,aDlgPrc,aDbAccess);
	CleanupStack::PushL(sm);
	sm->ConstructL();
	CleanupStack::Pop();
	return sm;
	}

CPsdInSM::CPsdInSM(MAgentObserver& aObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess)
	: CPsdAgentSMBase(aObserver,aDlgPrc,aDbAccess)
/**
Constructor

@param MAgentObserver& aObserver an observer which will be called back to notify changes in the connection
@param CDialogProcessor* aDlgPrc a pointer to a dialog processor object for use by the state machine
@param CCommsDbAccess& aDbAccess a reference to the comm Db access object to allow the state machine to obtain details from CommDb
*/
	{}

CPsdInSM::~CPsdInSM()
/**
Destructor
*/
	{}

void CPsdInSM::ConstructL()
/**
2nd phase construction of the incoming state machine
creates the initial state of the state machine
*/
	{
	CPsdAgentSMBase::ConstructL();
	iState = new (ELeave) CPsdInInit(this,this,this);
	}

TCommDbConnectionDirection CPsdInSM::Direction()
/**
Function indicates the direction of the connection which is in progress

@return TCommDbConnectionDirection the connection direction
*/
	{ return ECommDbConnectionDirectionIncoming;}
#endif // #ifndef INCOMING_NOT_SUPORTED

CPsdAgentSMBase::CPsdAgentSMBase(MAgentNotify& aObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess)
	: CAgentSMBase(aObserver,aDlgPrc,aDbAccess),
		iIsQoSSupported(ETrue)//let's be optimists 
/**
Constructor
*/
	{}

void CPsdAgentSMBase::ConstructL()
/**
2nd phase construction of the state machine base class
to be called in the ConstuctL of derived classes.
The function creates the logging objects and the objects
that contain the parameters for the Context and the QoS
*/
	{
	iEventLogger=CEventLogger::NewL();
	iDataLogger=CDataLogger::NewL(this,this);
	InquireModeL();
	iConfig=CPsdContextConfig::NewL(*iDb,iNetworkMode);
	iQoSConfig=CPsdContextQoSConfig::NewL(*iDb,iNetworkMode);
	}

CPsdAgentSMBase::~CPsdAgentSMBase()
/**
Destructor
*/
	{
	iPacketNetwork.Close();
	iContext.Close();
	iQoS.Close();
	iPhone.Close();
	iTelServer.Close();
	delete iQoSConfig;
	delete iConfig;
	delete iDataLogger;
	delete iEventLogger;
	iEventLogger=0;
	}


TInt CPsdAgentSMBase::GetExcessData(TDes8& /*aBuffer*/)
/**
This is only valid for CSD
To retrieve data for the SM, and accessor functions for the state objects to use.

@param contains data retrieved from the agent for SM.
@return KErrNotSupported if data is not retrieved.
*/
	{
	return KErrNotSupported;
	}

TInt CPsdAgentSMBase::Notification(TNifToAgentEventType /*aEvent*/, TAny* /*aInfo*/)
/**
No NIF to AGENT notifications supported by PSDAGX
Act on / reply to NifMan's notification requests

@param Type of event from Nifman to Agent.
@param Information about the Agent
@return KErrNotSupported if Event type does not exist.
*/
	{
	return KErrNotSupported;
	}

void CPsdAgentSMBase::GetLastError(TInt& aError)
/**
If the context is still open, request last error, otherwise the last error will
have been obtained on closure.

@param error code if the context is not open.
*/
	{
	if (iContext.SubSessionHandle())
		iContext.GetLastErrorCause(aError);
	else
		aError=iErrorCause;
	}
	
TBool CPsdAgentSMBase::IsIncomingAllowedL() const
/**
Function to allow clients to inquire if incomming connections are allowed

@return ETrue if incomming connections allowed else EFalse
*/
	{
	// Hurricane does not support Incoming connections so hard code this response to False
#ifdef INCOMING_NOT_SUPORTED
	return EFalse;
#else
	// Note - the code below should be used when incoming connections are supported
	TUint32 allowed;
	iDb->GetGlobalL(TPtrC(ACCEPT_INCOMING_GPRS),allowed);
	return (TBool)allowed;
#endif
	}

void CPsdAgentSMBase::GetRemotePartyL(TDes& aRemoteParty) const
/**
Function to allow clients to inquire the name of the remote party in the connection

@param TDes& aRemoteParty reference to a descriptor into which the remote party name will be written.
*/
	{
	iDb->GetDesL(TPtrC(KGeneralServiceTable),TPtrC(COMMDB_NAME),aRemoteParty);
	}

void CPsdAgentSMBase::InquireModeL()
/**
Internal function to figure out the current mode and set the iNetworkMode
member appropriately
*/ 
	{
	TBuf<KCommsDbSvrMaxColumnNameLength> serviceType;
	iDb->GetServiceTypeL(serviceType);
	if(serviceType == TPtrC(OUTGOING_GPRS))
		{
		iNetworkMode = EModeWCDMA;
		}
#ifndef INCOMING_NOT_SUPORTED	
	else if(serviceType == TPtrC(INCOMING_GPRS))
		{
		iNetworkMode = EModeWCDMA;
		}
#endif // #ifndef INCOMING_NOT_SUPORTED	
	else
		{
		// The service type is not supported by this AGX so leave
		User::Leave(KErrNotSupported);
		}
	}

MAgentStateMachineEnv& CPsdAgentSMBase::BaseEnv() 
	{ return *this;}

RTelServer& CPsdAgentSMBase::TelServer() 
	{ return iTelServer;}

RPhone& CPsdAgentSMBase::Phone() 
	{ return iPhone;}

RPacketService& CPsdAgentSMBase::PacketNetwork() 
	{ return iPacketNetwork;}

RPacketContext& CPsdAgentSMBase::Context() 
	{ return iContext;}

TBool CPsdAgentSMBase::IsQoSSupported()const
	{return iIsQoSSupported;}

void CPsdAgentSMBase::SetQoSSupported(TBool isSupported)
	{iIsQoSSupported=isSupported;}

RPacketQoS& CPsdAgentSMBase::QoS()
	{ return iQoS;}

const CPsdContextConfig& CPsdAgentSMBase::Config() const 
	{ return *iConfig;}

const CPsdContextQoSConfig& CPsdAgentSMBase::QoSConfig() const
	{return *iQoSConfig;}

TInt& CPsdAgentSMBase::ErrorCause()
	{ return iErrorCause;}

CEventLogger* CPsdAgentSMBase::Logger() 
	{ return iEventLogger;}

CDataLogger* CPsdAgentSMBase::DataLogger() 
	{ return iDataLogger;}

TBool& CPsdAgentSMBase::AmLogging()  
/**
@deprecated since logging requests are stored in a queue now
*/
	{ return iAmLogging;}

TBool CPsdAgentSMBase::TsyLoaded() const
	{ return iTsyLoaded;}

void CPsdAgentSMBase::SetTsyLoaded(TBool aStatus)
	{ iTsyLoaded=aStatus;}

MPsdEnv::TNetworkMode CPsdAgentSMBase::Mode() const
	{return iNetworkMode;}

CPsdContextConfig* CPsdContextConfig::NewL(CCommsDbAccess& aDbAccess, MPsdEnv::TNetworkMode aMode)
/**
Function creates a CPsdContextConfig object

@param CCommsDbAccess& aDbAccess a reference to the comm Db access object to allow the class to obtain details from CommDb
@param TNetworkMode aMode the mode of the context config parameters to be created.
@return CPsdContextConfig* the newly created object (ownership of the object is passed to the function caller)
*/
	{
	CPsdContextConfig* self = new(ELeave)CPsdContextConfig(aDbAccess,aMode);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); //self
	return self;
	}

CPsdContextConfig::CPsdContextConfig(CCommsDbAccess& aDbAccess, MPsdEnv::TNetworkMode aMode)
: iDbAccess(aDbAccess), iMode(aMode)
	{	
	}

void CPsdContextConfig::ConstructL()
/**
2nd phase construction of CPsdContextConfig calls the appropriate internal construction
function for the current mode
*/
	{
	if(iMode == MPsdEnv::EModeWCDMA)
		{
		GprsConfigConstructL();
		}
	}

CPsdContextConfig::~CPsdContextConfig()
/**
Destructor
*/
	{
	delete iConfig;
	delete iPackedConfig;
	}

const TPacketDataConfigBase& CPsdContextConfig::Config() const
/**
Function that returns the current context config parameters as a TPacketDataConfigBase

@return TPacketDataConfigBase& reference to the config parameters. (reurned as the mode agnostic base class)
*/
	{
	return *iConfig;
	}

TDesC8& CPsdContextConfig::PackedConfig() const
/**
Function that returns the current context config parameters packed in a descriptor

@return TDesC8& reference to the config parameters packed in a descriptor.
*/
	{
	return *iPackedConfig;
	}

TBool CPsdContextConfig::IsConfigSupportedL(const RPacketService& aPacketData, TCommDbConnectionDirection aDirection) const
/**
Inquiry function to enquire if the TSY is able to support the current context parameters

@param RPacketService& aPacketData handle to a previously initialised RPacketService object
@param TCommDbConnectionDirection aDirection the direction of the current connection
@return TBool returns ETrue if config supported, EFalse otherwise.
*/
	{
	if(!iStaticCaps)
		{
		// Only get the caps if they havn't been retrieved before
		User::LeaveIfError(aPacketData.GetStaticCaps(iStaticCaps,iProtocolType));
		}
	TBool result=ETrue;
	if(iMode == MPsdEnv::EModeWCDMA)
		{
		RPacketContext::TContextConfigGPRS& config = *(STATIC_CAST(RPacketContext::TContextConfigGPRS*, iConfig));

		if ( (config.iAnonymousAccessReqd==RPacketContext::ERequired)
			&&(aDirection == ECommDbConnectionDirectionIncoming) )
			{
			result=EFalse;
			}
		else if (!(iStaticCaps & RPacketService::KCapsAASupported)
			&& (config.iAnonymousAccessReqd==RPacketContext::ERequired))
			{
			result=EFalse;
			}
		else if (!(iStaticCaps & RPacketService::KCapsPdpDataCompSupported)
			&& (config.iPdpCompression & RPacketContext::KPdpDataCompression))
			{
			result=EFalse;
			}
		else if (!(iStaticCaps & RPacketService::KCapsPdpHeaderCompSupported)
			&& (config.iPdpCompression & RPacketContext::KPdpHeaderCompression))
			{
			result=EFalse;
			}
		}
	return result;
	}


TBool CPsdContextConfig::TsySupportsDataTransferInfoL(const RPacketService& aPacketData) const
/**
Inquiry function to enquire if the TSY is able to support data transfer inquiries

@param RPacketService& aPacketData handle to a previously initialised RPacketService object
@return TBool returns ETrue if data transfer inquiry supported, EFalse otherwise.
*/
	{
	if(!iStaticCaps)
		{
		// Only get the caps if they havn't been retrieved before
		User::LeaveIfError(aPacketData.GetStaticCaps(iStaticCaps,iProtocolType));
		}
	return(iStaticCaps & RPacketService::KCapsGetDataTransferredSupported);
	}

void CPsdContextConfig::GprsConfigConstructL()

/**
Internal function that constructs the object if the mode is GPRS
interogates CommDb and reads the appropriate parameters from there

Relies on SetCurrentSettingsL() having already been called by the controller
Also expects the service setting to be opened on the correct GPRS Table (outgoing/incoming)
*/
	{
	iConfig=new(ELeave)RPacketContext::TContextConfigGPRS();

	RPacketContext::TContextConfigGPRS& config = *(STATIC_CAST(RPacketContext::TContextConfigGPRS*, iConfig));
	TUint32 pdptype;
	iDbAccess.GetIntL(TPtrC(KGeneralServiceTable),TPtrC(GPRS_PDP_TYPE),pdptype);
	config.iPdpType = (RPacketContext::TProtocolType)pdptype;

	// Save the protocol type seperately for easy access when retreiving static caps
	iProtocolType = config.iPdpType;

	iDbAccess.GetDesL(TPtrC(KGeneralServiceTable),TPtrC(GPRS_APN),config.iAccessPointName);
	iDbAccess.GetDesL(TPtrC(KGeneralServiceTable),TPtrC(GPRS_PDP_ADDRESS),config.iPdpAddress);
	
	GetGprsCompressionL(config.iPdpCompression);
	GetGprsAAL(config.iAnonymousAccessReqd);

	iPackedConfig = new(ELeave)TPckg<RPacketContext::TContextConfigGPRS>(config);
	}

void CPsdContextConfig::GetGprsCompressionL(TUint& aCompression) const
	{
	aCompression = 0;
	TBool isCompression;
	iDbAccess.GetBoolL(TPtrC(KGeneralServiceTable),TPtrC(GPRS_DATA_COMPRESSION),isCompression);
	if (isCompression)
		{
		aCompression |= RPacketContext::KPdpDataCompression;
		}

	iDbAccess.GetBoolL(TPtrC(KGeneralServiceTable),TPtrC(GPRS_HEADER_COMPRESSION),isCompression);
	if (isCompression)
		{
		aCompression |= RPacketContext::KPdpHeaderCompression;
		}
	}

void CPsdContextConfig::GetGprsAAL(RPacketContext::TAnonymousAccess& aAnonymous) const
	{	
	TBool isAnonymous;
	iDbAccess.GetBoolL(TPtrC(KGeneralServiceTable),TPtrC(GPRS_ANONYMOUS_ACCESS),isAnonymous);
	if (isAnonymous)
		aAnonymous=RPacketContext::ERequired;
	else
		aAnonymous=RPacketContext::ENotRequired;
	}

void CPsdContextConfig::GetGprsEdgeL(TBool& aUseEdge) const
	{	
	TBool isUseEdge;
	iDbAccess.GetBoolL(TPtrC(KGeneralServiceTable),TPtrC(GPRS_USE_EDGE),isUseEdge);
	aUseEdge=isUseEdge;
	}

TBool CPsdContextConfig::QueryIfConfigAcceptable(TDesC8& aNewConfig) const
/**
Function to check if the config passed in the parameter meets the minimum requiremnts
set in CommDb

@param TDesC8& aNewConfig the config to be checked packed in a descriptor
@return TBool ETrue if the config passed in is above or equal to the minimum values, EFalse otherwise.
*/
	{
	TBool ret = ETrue;
	if(iMode == MPsdEnv::EModeWCDMA)
		{
		TPckg<RPacketContext::TContextConfigGPRS> configPckg = *((TPckg<RPacketContext::TContextConfigGPRS>*)&aNewConfig);
		RPacketContext::TContextConfigGPRS& newConfig = configPckg();
		ret = QueryIfGprsConfigAcceptable(newConfig);
		}
	return ret;
	}

TBool CPsdContextConfig::QueryIfGprsConfigAcceptable(const RPacketContext::TContextConfigGPRS& /*aNewConfig*/) const
/**
Internal function to check if the config passed in the parameter meets the minimum required for GPRS

@param RPacketContext::TContextConfigGPRS& aNewConfig the config to be checked against the minimum values
@return TBool ETrue if the config passed in is above or equal to the minimum values, EFalse otherwise.
*/
	{
	// Apparently the GPRS network has to give you what you asked for otherwise
	// it will not complete the connection, so just return ETrue
	return ETrue;
	}

TBool CPsdContextConfig::QueryIfIncommingConnectionAcceptable(const RPacketContext::TProtocolType& aProtocolType, const RPacketContext::TProtocolAddress& aProtocolAddress) const
/**
Inquiry function to enquire if incoming connection is valid

@param RPacketContext::TProtocolType& aProtocolType the protocol type of the incomming connection
@param RPacketContext::TProtocolAddress& aProtocolAddress the address of the incomming connection
@return TBool returns ETrue if connection OK, EFalse otherwise.
*/
	{
	if(iMode == MPsdEnv::EModeWCDMA)
		{
		RPacketContext::TContextConfigGPRS& config = *(STATIC_CAST(RPacketContext::TContextConfigGPRS*, iConfig));
		// For GPRS incoming connection the connection must be IP based and the IP address must be static.
		// The IP address should be the one stored in CommDb
		if( ((aProtocolType==RPacketContext::EPdpTypeIPv4)||(aProtocolType==RPacketContext::EPdpTypeIPv6)) 
			&& config.iPdpAddress==aProtocolAddress)
			{
			return ETrue;
			}
		else
			{
			return EFalse;
			}
		}
	return EFalse;
	}

//======================================================
//
// CPsdContextQoSConfig
//
//======================================================

CPsdContextQoSConfig* CPsdContextQoSConfig::NewL(CCommsDbAccess& aDbAccess, MPsdEnv::TNetworkMode aMode)
/**
Function creates a CPsdContextQoSConfig object

@param CCommsDbAccess& aDbAccess a reference to the comm Db access object to allow the class to obtain details from CommDb
@param TNetworkMode aMode the mode of the context config parameters to be created.
@return CPsdContextQoSConfig* the newly created object (ownership of the object is passed to the function caller)
*/
	{
	CPsdContextQoSConfig* self = new(ELeave)CPsdContextQoSConfig(aDbAccess,aMode);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); //self
	return self;
	}

CPsdContextQoSConfig::CPsdContextQoSConfig(CCommsDbAccess& aDbAccess, MPsdEnv::TNetworkMode aMode)
: iDbAccess(aDbAccess), iMode(aMode)
	{
	}

void CPsdContextQoSConfig::ConstructL()
/**
2nd phase construction of CPsdContextQoSConfig calls the appropriate internal construction
function for the current mode
*/
	{
	if(iMode == MPsdEnv::EModeWCDMA)
		{
		GprsQoSConfigConstructL();
		}
	}

CPsdContextQoSConfig::~CPsdContextQoSConfig()
/**
Destructor
*/
	{
	delete iPackedQoSConfig;
	delete iQoSConfig;
	}

const TPacketDataConfigBase& CPsdContextQoSConfig::QoSConfig() const
/**
Function that returns the current QoS config parameters as a TPacketDataConfigBase

@return TPacketDataConfigBase& reference to the QoS config parameters. (reurned as the mode agnostic base class)
*/
	{
	return *iQoSConfig;
	}

TDes8& CPsdContextQoSConfig::PackedQoSConfig() const
/**
Function that returns the current QoS config parameters packed in a descriptor

@return TDesC8& reference to the QoS config parameters packed in a descriptor.
*/
	{
	return *iPackedQoSConfig;
	}

void CPsdContextQoSConfig::GprsQoSConfigConstructL()
/**
Internal function that constructs the object if the mode is GPRS
interogates CommDb and reads the appropriate parameters from there
*/
	{
	iQoSConfig=new(ELeave)RPacketQoS::TQoSGPRSRequested();
	RPacketQoS::TQoSGPRSRequested& qoSConfig = *(STATIC_CAST(RPacketQoS::TQoSGPRSRequested*, iQoSConfig));


	TUint32 data;
	iDbAccess.GetIntL(TPtrC(KGeneralServiceTable),TPtrC(GPRS_REQ_PRECEDENCE),data);
	qoSConfig.iReqPrecedence	= (RPacketQoS::TQoSPrecedence)data;

	iDbAccess.GetIntL(TPtrC(KGeneralServiceTable),TPtrC(GPRS_REQ_DELAY),data);
	qoSConfig.iReqDelay		= (RPacketQoS::TQoSDelay)data;

	iDbAccess.GetIntL(TPtrC(KGeneralServiceTable),TPtrC(GPRS_REQ_RELIABILITY),data);
	qoSConfig.iReqReliability	= (RPacketQoS::TQoSReliability)data;

	iDbAccess.GetIntL(TPtrC(KGeneralServiceTable),TPtrC(GPRS_REQ_PEAK_THROUGHPUT),data);
	qoSConfig.iReqPeakThroughput = (RPacketQoS::TQoSPeakThroughput)data;

	iDbAccess.GetIntL(TPtrC(KGeneralServiceTable),TPtrC(GPRS_REQ_MEAN_THROUGHPUT),data);
	qoSConfig.iReqMeanThroughput = (RPacketQoS::TQoSMeanThroughput)data;
	
	iDbAccess.GetIntL(TPtrC(KGeneralServiceTable),TPtrC(GPRS_MIN_PRECEDENCE),data);
	qoSConfig.iMinPrecedence		= (RPacketQoS::TQoSPrecedence)data;

	iDbAccess.GetIntL(TPtrC(KGeneralServiceTable),TPtrC(GPRS_MIN_DELAY),data);
	qoSConfig.iMinDelay			= (RPacketQoS::TQoSDelay)data;

	iDbAccess.GetIntL(TPtrC(KGeneralServiceTable),TPtrC(GPRS_MIN_RELIABILITY),data);
	qoSConfig.iMinReliability	= (RPacketQoS::TQoSReliability)data;

	iDbAccess.GetIntL(TPtrC(KGeneralServiceTable),TPtrC(GPRS_MIN_PEAK_THROUGHPUT),data);
	qoSConfig.iMinPeakThroughput = (RPacketQoS::TQoSPeakThroughput)data;

	iDbAccess.GetIntL(TPtrC(KGeneralServiceTable),TPtrC(GPRS_MIN_MEAN_THROUGHPUT),data);
	qoSConfig.iMinMeanThroughput = (RPacketQoS::TQoSMeanThroughput)data;

	iPackedQoSConfig = new(ELeave)TPckg<RPacketQoS::TQoSGPRSRequested>(qoSConfig);
	}

TBool CPsdContextQoSConfig::QueryIfQoSAcceptable(TDesC8& aNewQoS) const
/**
Function to check if the QoS config passed in the parameter meets the minimum requiremnts
set in CommDb

@param TDesC8& aNewQoS the QoS config to be checked packed in a descriptor
@return TBool ETrue if the QoS config passed in is above or equal to the minimum values, EFalse otherwise.
*/
	{
	TBool ret = ETrue;
	if(iMode == MPsdEnv::EModeWCDMA)
		{
		TPckg<RPacketQoS::TQoSGPRSNegotiated> qoSPckg = *((TPckg<RPacketQoS::TQoSGPRSNegotiated>*)&aNewQoS);
		RPacketQoS::TQoSGPRSNegotiated& newQoS = qoSPckg();
		ret = QueryIfGprsQoSAcceptable(newQoS);
		}
	return ret;
	}

TBool CPsdContextQoSConfig::QueryIfGprsQoSAcceptable(const RPacketQoS::TQoSGPRSNegotiated& /*aNewQoS*/) const
/**
Internal function to check if the QoS config passed in the parameter meets the minimum required for GPRS

@param RPacketQoS::TQoSGPRSNegotiated& aNewQoS the QoS config to be checked against the minimum values
@return TBool ETrue if the config passed in is above or equal to the minimum values, EFalse otherwise.
*/
	{
	// Apparently the GPRS network has to give you what you asked for otherwise
	// it will not complete the connection, so just return ETrue
	return ETrue;
	}

//
//
// CPsdContextChangeMonitor
//

CPsdContextChangeMonitor* CPsdContextChangeMonitor::NewL(MPsdEnv* aEnv, MPsdContextChangeObserver* aObserver)

/**
Function creates a CPsdContextChangeMonitor object

@param MPsdEnv* aEnv a pointer to a object that provides the MPsdEnv interface
@param MPsdContextChangeObserver* aObserver a pointer to the observing object that should be called back when the quality drops below the minimum value
@return CPsdContextChangeMonitor* the newly created object (ownership of the object is passed to the function caller)
*/
	{
	__ASSERT_DEBUG(aEnv && aObserver,User::Invariant());
	CPsdContextChangeMonitor* self = new(ELeave)CPsdContextChangeMonitor(aEnv,aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); //self
	return self;
	}

CPsdContextChangeMonitor::~CPsdContextChangeMonitor()
/**
Destructor
*/
	{
	Cancel();
	iDebounceTimer.Close();
	delete iQoSWatcher;
	delete iConfigWatcher;
	
	delete iPackedActualQoSConfig;
	delete iActualQoSConfig;
	delete iPackedActualConfig;
	delete iActualConfig;	
	}

CPsdContextChangeMonitor::CPsdContextChangeMonitor(MPsdEnv* aEnv, MPsdContextChangeObserver* aObserver)
: CActive(EPriorityStandard), iSM(aEnv), iObserver(aObserver), iContextState(CPsdContextChangeMonitor::EContextQualityAcceptable)
	{
	CActiveScheduler::Add(this);
	}

void CPsdContextChangeMonitor::ConstructL()
/**
2nd phase constructor. Initialises the member variables as appropriate for the current mode.
Constructs a CPsdContextConfigChangeWatcher and a CPsdQoSConfigChangeWatcher object to watch for
cahnges in context config and qos config.
*/
	{
	TInt r=iDebounceTimer.CreateLocal();
	if (r!=KErrNone)
		User::Leave(r);
	TUint32 timeOut = 0; // keeps compiler happy; we know it's always set below
	if(iSM->Mode() == MPsdEnv::EModeWCDMA)
		{
		iActualConfig=new(ELeave)RPacketContext::TContextConfigGPRS();
		RPacketContext::TContextConfigGPRS& config = *(STATIC_CAST(RPacketContext::TContextConfigGPRS*, iActualConfig));
		iPackedActualConfig = new(ELeave)TPckg<RPacketContext::TContextConfigGPRS>(config);

		iActualQoSConfig=new(ELeave)RPacketQoS::TQoSGPRSNegotiated();
		RPacketQoS::TQoSGPRSNegotiated& qoSConfig = *(STATIC_CAST(RPacketQoS::TQoSGPRSNegotiated*, iActualQoSConfig));
		iPackedActualQoSConfig = new(ELeave)TPckg<RPacketQoS::TQoSGPRSNegotiated>(qoSConfig);
		// Get timeOut from the Db; default to 0 on error (eg. Db value is NULL)
		TRAPD(rOk,iSM->BaseEnv().Db()->GetIntL(TPtrC(KGeneralServiceTable),TPtrC(GPRS_QOS_WARNING_TIMEOUT),timeOut));
		if (rOk != KErrNone)
			timeOut = 0;
		}
	iConfigWatcher = new(ELeave)CPsdContextConfigChangeWatcher(iSM->Context(), *this, *iPackedActualConfig);

	if (iSM->IsQoSSupported())//the tsy might not support QoS
		iQoSWatcher = new(ELeave)CPsdQoSConfigChangeWatcher(iSM->QoS(), *this, *iPackedActualQoSConfig);
	else
		iQoSWatcher=0;
	
	iDebounceTimeout = timeOut;
	if(iDebounceTimeout.Int()>0)
		{
		// The timeout will be greater than zero if the service loss dialogs are enabled
		iConfigWatcher->StartL();
		if (iQoSWatcher)//the tsy might not support QoS
			iQoSWatcher->StartL();
		}
	}

void CPsdContextChangeMonitor::ReportConfigChange()
/**
Callback function, called when the CPsdContextConfigChangeWatcher object reports
a change in context config parameters
Checks to see if the changed parameters are below the minimum and if so
starts the timeout which will cause the observer to be informed of the 
connection going below quality.
If the context config returns to acceptable values before the timeout completes
the timeout is cancelled and the observer will not be notified of the change
*/
	{
	// The config has changed check if it is acceptable
	TBool acceptable = iSM->Config().QueryIfConfigAcceptable(*iPackedActualConfig);
	// Only report if the context was previously of acceptable quality and now it is not
	if(!acceptable && (iContextState == EContextQualityAcceptable))
		{
		// the context config has just gone below quality - don't report again until it becomes acceptable
		iContextState |= EConfigQualityUnAcceptable;
		StartTimeout();
		}
	else if(acceptable && (iContextState & EConfigQualityUnAcceptable))
		{
		// the context config has become of acceptable quality again
		iContextState &= ~EConfigQualityUnAcceptable;
		CancelTimeout();
		}
	}

void CPsdContextChangeMonitor::ReportQoSChange()
/**
Callback function, called when the CPsdQoSConfigChangeWatcher object reports
a change in QoS config parameters
Checks to see if the changed parameters are below the minimum and if so
starts the timeout which will cause the observer to be informed of the 
connection going below quality.
If the QoS config returns to acceptable values before the timeout completes
the timeout is cancelled and the observer will not be notified of the change
*/
	{
	// The QoS config has changed check if it is acceptable
	TBool acceptable = iSM->QoSConfig().QueryIfQoSAcceptable(*iPackedActualQoSConfig);
	// Only report if the context was previously of acceptable quality and now it is not
	if(!acceptable && (iContextState == EContextQualityAcceptable))
		{
		// the QoS config has just gone below quality - don't report again until it becomes acceptable
		iContextState |= EQoSQualityUnAcceptable;
		StartTimeout();
		}
	else if(acceptable && (iContextState & EQoSQualityUnAcceptable))
		{
		// the context config has become of acceptable quality again
		iContextState &= ~EQoSQualityUnAcceptable;
		CancelTimeout();
		}
	}

void CPsdContextChangeMonitor::RunL()
/**
The RunL will be called when the timeout completes following a drop below acceptable
QoS or context quality. It informs the observer of the quality drop.
*/
	{
	// The timeout has completed this means the context quality has been below acceptable
	// for the duration of the timeout, so report it
	iObserver->ReportContextBelowAcceptableQuality();
	}

void CPsdContextChangeMonitor::ReportConfigWatcherError(TInt aError)
/**
Callback function called when an error occurs in the CPsdContextConfigChangeWatcher object

@param error code for error occurs in the CPsdContextConfigChangeWatcher object
*/
	{
	iObserver->ReportError(aError);
	}

void CPsdContextChangeMonitor::ReportQoSWatcherError(TInt aError)
/**
Callback function called when an error occurs in the CPsdQoSConfigChangeWatcher object

@param error code for error occurs in the CPsdQoSConfigChangeWatcher object
*/
	{
	iObserver->ReportError(aError);
	}

void CPsdContextChangeMonitor::StartTimeout()
	{
	if(!IsActive())
		{
		iDebounceTimer.After(iStatus, iDebounceTimeout);
		SetActive();
		}
	}

void CPsdContextChangeMonitor::CancelTimeout()
	{
	if(iContextState == EContextQualityAcceptable)
		{
		Cancel();
		}
	}

void CPsdContextChangeMonitor::DoCancel()
	{	
	iDebounceTimer.Cancel();
	}
//
//
// CPsdContextConfigChangeWatcher
//

CPsdContextConfigChangeWatcher::CPsdContextConfigChangeWatcher(const RPacketContext& aContext, MObserver& aObserver, TPtr8& aContextParams)
: CActive(EPriorityStandard), iObserver(aObserver), iContext(aContext), iContextParams(aContextParams)
/**
Overloaded Constructor adding itself to the active scheduler
*/
	{
	CActiveScheduler::Add(this);
	}

void CPsdContextConfigChangeWatcher::StartL()
/**
Start notifying observer of changes in context config parameters
*/
	{
	iContext.NotifyConfigChanged(iStatus, iContextParams);
	SetActive();
	}

CPsdContextConfigChangeWatcher::~CPsdContextConfigChangeWatcher()
/**
Destructor
*/
	{
	Cancel();
	}

void CPsdContextConfigChangeWatcher::RunL()
/**
RunL completes when the context config parameters have changed - call back
the observer to inform them of the event
*/
	{
	if(iStatus != KErrNone)
		{
		iObserver.ReportConfigWatcherError(iStatus.Int());
		}
	else
		{
		iObserver.ReportConfigChange();
		}
	// Renew Request
	iContext.NotifyConfigChanged(iStatus, iContextParams);
	SetActive();
	}

void CPsdContextConfigChangeWatcher::DoCancel()
/**
Cancell the changes in the context configuration parameters and notifying its observer when such a change occurs.
*/
	{
	iContext.CancelAsyncRequest(EPacketContextNotifyConfigChanged);
	}

//
//
// CPsdQoSConfigChangeWatcher
//

CPsdQoSConfigChangeWatcher::CPsdQoSConfigChangeWatcher(const RPacketQoS& aQoS, MObserver& aObserver, TPtr8& aQoSParams)
: CActive(EPriorityStandard), iObserver(aObserver), iQoS(aQoS), iQoSParams(aQoSParams)
/**
Overloaded Constructor adding itself to the active scheduler
*/
	{
	CActiveScheduler::Add(this);
	}

void CPsdQoSConfigChangeWatcher::StartL()
/**
Start notifying observer of changes in QoS config parameters
*/
	{
	iQoS.NotifyProfileChanged(iStatus, iQoSParams);
	SetActive();
	}

CPsdQoSConfigChangeWatcher::~CPsdQoSConfigChangeWatcher()
/**
Destructor
*/
	{
	Cancel();
	}

void CPsdQoSConfigChangeWatcher::RunL()
/**
RunL completes when the QoS config parameters have changed - call back
the observer to inform them of the event
*/
	{
	if(iStatus != KErrNone)
		{
		iObserver.ReportQoSWatcherError(iStatus.Int());
		}
	else
		{
		iObserver.ReportQoSChange();
		}
	// Renew Request
	iQoS.NotifyProfileChanged(iStatus, iQoSParams);
	SetActive();
	}

void CPsdQoSConfigChangeWatcher::DoCancel()
	{
	iQoS.CancelAsyncRequest(EPacketQoSNotifyProfileChanged);
	}

