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
// Legacy Node Messages
// 
//

/**
 @file
 @internalComponent
*/


#ifndef SS_LEGACYINTERFACES_H_INCLUDED
#define SS_LEGACYINTERFACES_H_INCLUDED


#include <es_enum.h>
#include <comms-infras/ss_nodeinterfaces.h>
#include <comms-infras/ss_platsec_apiext.h>
#include <comms-infras/ss_datamonitoringprovider.h>
#include <comms-infras/ss_connprov.h>
#include <elements/responsemsg.h>

namespace ESock
{
enum TLegacyConnEnumClients
	{
	EAll,
	ESocket,
	EHostResolver,
	EConnection
	};

/**
API Extension for Data Access
@internalComponent
*/
class MLinkMCprLegacyDataAccessApiExt
    {
public:
	static const TInt KInterfaceId = 0x102864D8;

	virtual void GetIntSettingL(const TDesC& aSettingName, TUint32& aValue, ESock::MPlatsecApiExt* aPlatsecItf) = 0;
	virtual void GetBoolSettingL(const TDesC& aSettingName, TBool& aValue, ESock::MPlatsecApiExt* aPlatsecItf) = 0;
	virtual void GetDes8SettingL(const TDesC& aSettingName, TDes8& aValue, ESock::MPlatsecApiExt* aPlatsecItf) = 0;
	virtual void GetDes16SettingL(const TDesC& aSettingName, TDes16& aValue, ESock::MPlatsecApiExt* aPlatsecItf) = 0;
	virtual void GetLongDesSettingL(const TDesC& aSettingName, HBufC*& aValue, ESock::MPlatsecApiExt* aPlatsecItf) = 0;
    };


/**
	API Extension for Control

	@internalComponent
*/
class MLegacyControlApiExt
    {
public:
	static const TInt KInterfaceId = 0x102864DB;

    /**
    Legacy control call

    @param aOptionLevel Identifier for the option level
    @param aOptionName Identifier for the option name
    @param aOption A buffer either containing the option data to be set, or to receive option data
    @param aPlatsecItf A pointer to an MPlatsecApiExt interface.

    @return KErrNone if the control option has been processed successfully.
    KErrNotSupported if the control option is not understood. It will be automatically forwarded down
    to the next Control Provider in this case.
    Any other system-wide error code will complete the client message with the error code returned.
    */
    virtual TInt ControlL(TUint aOptionLevel, TUint aOptionName, TDes8& aOption, MPlatsecApiExt* aPlatsecItf) = 0;
    };

class CLegacyEnumerateSubConnectionsResponder;

/**
	API Extension for IsSubConnectionActive

	@internalTechnology
*/
class ALegacyEnumerateSubConnectionsApiExt : public AExtensionInterfaceThickBase
    {
public:
	static const TInt KInterfaceId = ELegacyEnumerateSubConnectionsApiExt;

    virtual void EnumerateSubConnections(CLegacyEnumerateSubConnectionsResponder*& aResponder) = 0;
	virtual void GetSubConnectionInfo(TSubConnectionInfo &aInfo) = 0;
	
protected:
	ALegacyEnumerateSubConnectionsApiExt()
		: AExtensionInterfaceThickBase(Meta::STypeId::CreateSTypeId() /* Null value used here - internal messages */)
		{
		}
    };

const TUint KMicrosecondsInASecond = 1000000;
const TInt KMaxTimerPeriod = KMaxTInt32 / KMicrosecondsInASecond; //< max period of a CTimer using After()
const TUint KActivityTimerPriority = EPriorityLow;	// Checking for connection activity is only a low priority task

struct CClientRequest;
class CLegacySubConnectionActiveResponder;
/*
	An extension interface supporting calls to RConnection::(IsConnectionActive()/
	IsSubConnectionActive()).  Derives from AExtensionInterfaceThickBase to get
	behaviour compatible with other 'real' IPC client supporting extension
	interfaces such as data monitoring and bearer mobility.

	Note: while this class derives from AExtensionInterfaceThickBase, it supplies
	a null value for the message implementation uid. to the base constructor as
	the mechanism for just-in-time loading of messages via ECOM is not required.
	The messages are part of esocksvr.

	@internalTechnology
*/
class ALegacySubConnectionActiveApiExt : public AExtensionInterfaceThickBase
    {
public:
	static const TInt KInterfaceId = ELegacySubConnectionActiveApiExt;

public:
	IMPORT_C void CancelExtensionInterface(TSubSessionUniqueId aClientId);
    void IsSubConnectionActiveRequest(CLegacySubConnectionActiveResponder& aResponder);
    void CancelIsSubConnectionActiveRequest(TSubSessionUniqueId aClientId);
    void CheckSubConnectionActivityL(CClientRequest* aRequest);

protected:
	ALegacySubConnectionActiveApiExt(Messages::ANode* aParent)
		: AExtensionInterfaceThickBase(Meta::STypeId::CreateSTypeId()/* Null value used here - internal messages */), iParent(aParent)
		{
		}

	IMPORT_C virtual ~ALegacySubConnectionActiveApiExt();

private:
	Messages::ANode* iParent;
	RPointerArray<CClientRequest> iRequests;
    };


NONSHARABLE_CLASS(CActivityTimer) : public CTimer
	{
public:
	static CActivityTimer* NewL(ALegacySubConnectionActiveApiExt& aOwner, CClientRequest* aRequest, TInt aPriority);
	~CActivityTimer() {};
	void RunL();

private:
	inline CActivityTimer(ALegacySubConnectionActiveApiExt& aOwner, CClientRequest* aRequest, TInt aPriority) :
		CTimer(aPriority), iOwner(aOwner), iRequest(aRequest)
		{
		CActiveScheduler::Add(this);
		}

private:
	ALegacySubConnectionActiveApiExt& iOwner;
	CClientRequest* iRequest;
	};


NONSHARABLE_STRUCT(CClientRequest) : public CBase
	{
	CClientRequest(TSubSessionUniqueId aClientId, CLegacySubConnectionActiveResponder& aResponder)
		: iClientId(aClientId), iResponder(&aResponder)
		{
		}

	~CClientRequest();
	
	void SubConnectionActive(TBool aState);

	TSubSessionUniqueId iClientId;
	TUint iInitialSentVolume;
	TUint iInitialReceivedVolume;
	CActivityTimer* iTimer;
	CLegacySubConnectionActiveResponder* iResponder;
	};

/**
API Extension for Progress
@internalComponent
*/
class MLinkCprApiExt
    {
public:
	static const TInt KInterfaceId = 0x102864D6;

    typedef TPckgBuf<Elements::TStateChange> TProgressBuf;

    virtual void ProgressL(TProgressBuf& aBuffer) const = 0;
    virtual void LastProgressError(TProgressBuf& aBuffer) = 0;
	virtual TInt AllSubConnectionNotificationEnable() = 0;
    };

/**
API Extension for Service Change Notification.
@internalComponent
*/
class MLinkCprServiceChangeNotificationApiExt
    {
public:
	static const TInt KInterfaceId = 0x102864DF;

    virtual void RequestServiceChangeNotificationL(const Messages::TNodeId& aSender, RLegacyResponseMsg& aResponse) = 0;
    virtual void CancelServiceChangeNotification(const Messages::TNodeId& aSender) = 0;
    };


/** Utility class pairing the legacy connection info for IPC client enumeration with the connection provider that provided it
*/
class TSourcedConnectionInfo : public TConnectionInfoV2
	{
public:
	inline TSourcedConnectionInfo(TUint32 aIapId, TUint32 aNetId, TConnectionType aConnectionType, const Messages::TNodeId& aCPRId)
		: TConnectionInfoV2(aIapId, aNetId, aConnectionType),
		  iProvider(aCPRId)
		{ }
	inline const Messages::TNodeId& Provider() const
		{ return iProvider; }
private:
	Messages::TNodeId iProvider;
	};


/**
API Extension for Enumerating Connections.
@internalComponent
*/
class MLinkCprFactApiExt
    {
public:
	static const TInt KInterfaceId = 0x102864D7;

	virtual void EnumerateConnectionsL(RPointerArray<TSourcedConnectionInfo>& aConnectionInfoPtrArray) = 0;
    };

} //namespace ESock



#endif
// SS_LEGACYINTERFACES_H_INCLUDED

