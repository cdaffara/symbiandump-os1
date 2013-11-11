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
// Reference Connection Provider class definition.
// 
//

/**
 @file
 @internalComponent
*/

#if !defined(__REFERENCECPR_CONNPROV_H__)
#define __REFERENCECPR_CONNPROV_H__

#include <comms-infras/ss_connprov.h>
#include <comms-infras/commsdebugutility.h>

#ifdef __FLOG_ACTIVE
// CommsDebugUtility logging tags. Use them to enable tracing for ReferenceCPR
_LIT8(KReferenceLogTag1,"ConnProvd");
_LIT8(KReferenceLogTag2,"Reference");
#endif

class CConnectionSettings;


class CReferenceConnectionProvider : public CConnectionProviderBase, public MConnectionControlClient
/**

@internalComponent
@released since v9.2
 */
	{  
protected:

	//-=========================================================
	// Custom methods
	//-=========================================================

	CReferenceConnectionProvider(CConnectionProviderFactoryBase& aFactory);
			
	~CReferenceConnectionProvider();

	static CReferenceConnectionProvider* NewL(CConnectionProviderFactoryBase& aFactory);
	friend class CReferenceProviderFactory;
	
	
	//-=========================================================
	// MConnectionControlClient methods
	//-=========================================================
	virtual void ConnectionGoingDown(CConnectionProviderBase& aConnProvider);
	virtual void ProgressNotification(TInt aStage, TInt aError);
	virtual void ConnectionError(TInt aStage,  TInt aError);
	virtual void ServiceChangeNotification(TUint32 aId, const TDesC& aType);
	virtual void SubConnectionEvent(CSubConnectionProviderBase* aSubConnNextLayerProvider, const TSubConnectionEvent& aSubConnectionEvent);
	virtual void LayerUp(TInt aError);
	virtual TCtlType CtlType() const;


	//-=========================================================
	// CConnectionProviderBase methods
	//-=========================================================
	virtual void DoDataClientJoiningL(MConnectionDataClient& aDataClient);
	virtual void DoDataClientLeaving(MConnectionDataClient& aDataClient);
	virtual void DoControlClientJoiningL(MConnectionControlClient& aControlClient);
	virtual void DoControlClientLeaving(MConnectionControlClient& aControlClient);

	virtual void DoStartL(Meta::SMetaData& aParams, const RMessagePtr2* aMessage);	
	virtual TInt DoStop(TInt aError, const RMessagePtr2* aMessage);   

	virtual void DoProgressL(Meta::SMetaData& aBuffer) const;
	virtual void DoLastProgressError(Meta::SMetaData& aBuffer);
	virtual void DoRequestServiceChangeNotificationL();
	virtual void DoCancelServiceChangeNotification();
	virtual void DoControlL(TUint aOptionLevel, TUint aOptionName, Meta::SMetaData& aOption, const RMessagePtr2* aMessage);
	virtual TInt DoAllSubConnectionNotificationEnable();
	virtual TInt DoCancelAllSubConnectionNotification();
	virtual void DoSendIoctlMessageL(const RMessage2& aMessage);
	virtual void DoSendCancelIoctl();   
	virtual TInt DoEnumerateSubConnectionsL(TUint& aCount);
	virtual TUint DoEnumerateClientsL(HBufC8*& aClientInfoBuffer, TEnumClients aClientType);
	virtual void DoConnectionControlActivityL( TControlActivity aControlActivity, const Meta::SMetaData* aData, const RMessagePtr2* aMessage );
	virtual CConnectionSettings& DoSettingsAccessL();
	virtual TInt DoCanDoSubConnection(RSubConnection::TSubConnType aSubConnType) const;
	virtual CConnectionProviderBase* DoNextLayer() const;
	virtual void DoJoinNextLayerL(CConnectionProviderBase* aNextLayer);
	
	CConnectionProviderBase* iNextLayer;
	
public:
	__FLOG_DECLARATION_MEMBER;	
	};

#endif // __REFERENCECPR_CONNPROV_H__
