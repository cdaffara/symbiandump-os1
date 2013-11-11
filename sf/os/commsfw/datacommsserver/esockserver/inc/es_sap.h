// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @internalTechnology
 @released
*/

#if !defined(__ES_SAP_H__)
#define __ES_SAP_H__

#define SYMBIAN_NETWORKING_UPS

#include <es_prot.h>
#include <es_sock.h>
#include <comms-infras/ss_subconnflow.h>
#include <comms-infras/ss_flowbinders.h>
#include <comms-infras/ss_nodemessages_dataclient.h>

namespace ESock
{

class CNetworkFlow : public CSubConnectionFlowBase, protected MSessionData, protected MSessionControl,
                        protected MFlowBinderControl
/** Service Access Point for SAPs that have a seperate control side.

Provides transport services to a single protocol. Several of the calls to
CServProviderBase have pre-conditions attached to them - for example
a connection oriented protocol must have its local address set (either by
a SetLocalName() or AutoBind()) before it is opened. If the socket server
calls the CServProviderBase in such an erroneous way, the protocol should panic.

It also provides an interface towards subconnection providers to allow
SAPs to work with a seperate control path.

@internalTechnology
@released Since 9.0 */
	{
	friend class ProtocolManager;

public:

	IMPORT_C virtual ~CNetworkFlow();

protected:
	void SetLocalName(TSockAddr& anAddr);
	TInt SetRemName(TSockAddr& anAddr);
	TUint Write(const TDesC8& aDesc,TUint aOptions, TSockAddr* anAddr);
	TInt Write(RMBufChain& aData, TUint aOptions, TSockAddr* anAddr);
    void BindToL(const TCFDataClient::TBindTo& aBindTo);
    NetInterfaces::TInterfaceControl* FetchInterfaceControlL(TSupportedCommsApiExt aInterfaceId);

protected:

	virtual MFlowBinderControl* DoGetBinderControlL();
	//MFlowBinderControl
	virtual MSessionControl* GetControlL(TInt aSessionType,MSessionControlNotify& aSessionControlNotify);
	virtual void Unbind();
	virtual MSessionData* BindL(MSessionDataNotify& aNotify);
	virtual CSubConnectionFlowBase* Flow(); //returns its own flow

    TBool IsBoundToSession() const
        {
        return  iSessionControlNotify || iSessionDataNotify;
        }

	void ProcessDCIdleState();

	
protected:
	IMPORT_C CNetworkFlow(CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConn, CProtocolIntfBase* aProtocolIntf);
	void UpdateDestinationAddress(const TSockAddr& aDest);

    #define SAP_FLAG_FUNCTIONS(name, flag) \
		inline TBool name() { return iStateFlags & flag; } \
		inline void Set##name() { iStateFlags |= flag; }	\
		inline void Clear##name() { iStateFlags &= ~flag; }

#ifdef SYMBIAN_NETWORKING_UPS
	virtual TBool ActivityRunning() = 0;
#endif


protected:
	TSockAddr iLocalAddress;
	TSockAddr iRemoteAddress;

	//upper flow
	MSessionControlNotify* iSessionControlNotify;
	MSessionDataNotify* iSessionDataNotify;
	//lower flow
	MFlowBinderControl* iLowerFlow;
	MLowerControl* iLowerControl; //just to keep the lower flow up

	enum TStateFlag
		{
		ELocalAddressSet = 0x00000001,
		ERemoteAddressSet = 0x00000002,
		ENoBearerRunning = 0x00000004,
		EStarted = 0x00000008,
		EIdle = 0x00000080,
		EIdleSent = 0x00000100
		};
	SAP_FLAG_FUNCTIONS(LocalAddressSet, ELocalAddressSet)
	SAP_FLAG_FUNCTIONS(RemoteAddressSet, ERemoteAddressSet)
	SAP_FLAG_FUNCTIONS(NoBearerGuard, ENoBearerRunning)
	SAP_FLAG_FUNCTIONS(Started, EStarted)
	SAP_FLAG_FUNCTIONS(Idle, EIdle)
	SAP_FLAG_FUNCTIONS(IdleSent, EIdleSent)
		
	TUint iStateFlags;
	};

} //namespace ESock

#endif	// __ES_SAP_H__


