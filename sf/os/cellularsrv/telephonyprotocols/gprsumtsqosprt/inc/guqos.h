// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __GUQOS_H__
#define __GUQOS_H__


#include <e32base.h>
#include <e32std.h>

#include <networking/module_if.h>

class CFlowData;
class CFlowContext;
class TQoSRequested;

const TUint KModuleGUQoS = 0x168; 

const TUint KGUQoSPriority = 10;
const TUint KModuleCapabilites = KQoSModuleBindToInterface	|
									KQoSModuleScheduler		|
									KQoSModulePolicer		|
									KQoSModuleShaper		|
									KQoSModuleSerialize		|
									KQoSFatalFailure		|
									KQoSModulePartialSignaling;


const TUint KPdpContextDeleteTimeout = 6000000;

//
// Configurable options
class TConfigurableOptions
	{
public:
	TInt iTimeout;			// Delay for Pdp context deletion [unit: seconds]
	};


const TUint KMajorVersionNumber=0;
const TUint KMinorVersionNumber=1;
const TUint KBuildVersionNumber=1;

// Forward decls
class CNif;
class CPdpContext;
class CNifManager;
class MTimeoutManager;

class CModuleGuqos : public CModuleBase
	{
public:
	static CModuleGuqos* NewL();
	~CModuleGuqos();

	void InitModuleL(MEventInterface& aEventInterface, CExtension* aData);
	void OpenL(CFlowContext &aFlow, CNifIfBase* aIf);
	void Close(CFlowContext &aFlow);
	void Negotiate(CFlowContext &aFlow, const TQoSParameters& aParams, MQoSNegotiateEvent& aNotify);
	void Release(CFlowContext &aFlow);
	void OpenChannel(TInt aChannelId, const TQoSParameters& aParams, CExtensionPolicy& aPolicy, MQoSNegotiateEvent& aNotify, CFlowContext& aFlow);
	void CloseChannel(TInt aChannelId);
	void NegotiateChannel(TInt aChannelId, const TQoSParameters& aParams, CExtensionPolicy& aPolicy, MQoSNegotiateEvent& aNotify);
	void Join(TInt aChannelId, CFlowContext& aFlow, MQoSNegotiateEvent& aNotify);
	void Leave(TInt aChannelId, CFlowContext& aFlow, MQoSNegotiateEvent& aNotify);
	TInt Configure(TUint aLevel, TUint aName, TDes8& aOption, TAny* aSource=0);
	void Identify(TServerProtocolDesc* aProtocolDesc)const;
	//lint -e{1411} It doesn't matter that CProtocolBase:Identify() looks similar
	static void Identify(TServerProtocolDesc& aDesc);
	TInt Send(RMBufChain& buf, CProtocolBase* aSourceProtocol=NULL);
	void InterfaceAttached(const TDesC& aName, CNifIfBase *aIf);
	void InterfaceDetached(const TDesC& aName, CNifIfBase *aIf);
	void Unbind(CProtocolBase* aProtocol, TUint aId = 0);

	TInt GetDefaultParameters(TQoSRequested& aParameters, TUint32 aIapId);
	CFlowData* FindFlow(const CFlowContext* aFlow);

	inline CNifManager* IfManager();
	inline MEventInterface* QoSEvent();
	inline TUint FlowCount() const;
	inline MTimeoutManager* TimeoutManager() const;
	inline const TConfigurableOptions& Options() const;

protected:
	CModuleGuqos();
	void ConstructL();

private:
	MEventInterface *iNotify;

	CFlowData* NewFlowL(CFlowContext& aFlow, CNif& aNif);
	void ReadConfigOptions();

private:
	TConfigurableOptions iOptions;			// Ini options.	
	TUint iFlowCount;
	CNifManager* iIfManager;				// interface manager
	MTimeoutManager* iTimeoutManager;		// Timeout manager
	CFlowData* iList;						// All flows.
	};


// Inline methods
inline CNifManager* CModuleGuqos::IfManager()
	{ return iIfManager; };

inline MEventInterface* CModuleGuqos::QoSEvent()
	{ return iNotify; }

inline TUint CModuleGuqos::FlowCount() const
	{ return iFlowCount;};
	
inline MTimeoutManager* CModuleGuqos::TimeoutManager() const
	{ return iTimeoutManager; }
	
inline const TConfigurableOptions& CModuleGuqos::Options() const
	{ return iOptions; }
	
typedef TSglQueIter<CNif> TNifIter;

#endif
