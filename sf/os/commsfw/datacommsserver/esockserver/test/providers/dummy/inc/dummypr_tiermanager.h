// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef SYMBIAN_DUMMYPR_TIERMANAGER
#define SYMBIAN_DUMMYPR_TIERMANAGER

#include <comms-infras/coretiermanager.h>
#include <comms-infras/ss_datamon_apiext.h>
#include <comms-infras/ss_legacyinterfaces.h>

class CDummyTierManager : public CCoreTierManager, public ESock::MLegacyControlApiExt, public ESock::ADataMonitoringProtocolReq,
			public ITFHIERARCHY_LINK_2(CDummyTierManager, CCoreTierManager, ESock::MLegacyControlApiExt, ESock::ADataMonitoringProtocolReq)
	{
public:
	static CDummyTierManager* NewL(ESock::CTierManagerFactoryBase& aFactory);
	~CDummyTierManager();

	class CAsyncResponder : public CTimer
		{
	public:
		static CAsyncResponder *NewL(CDummyTierManager *aTierManager, ESock::CDataMonitoringResponder *aResponder);
	private:
		CAsyncResponder(CDummyTierManager *aTierManager, ESock::CDataMonitoringResponder *aResponder);
		void ConstructL();
		virtual void RunL();
		virtual void DoCancel();
	private:
		CDummyTierManager *iTierManager;
		ESock::CDataMonitoringResponder *iResponder;
		};

public:
	typedef ITFHIERARCHY_LINK_2(CDummyTierManager, CCoreTierManager, ESock::MLegacyControlApiExt, ESock::ADataMonitoringProtocolReq) TIfStaticFetcherNearestInHierarchy;

    virtual void ReturnInterfacePtrL(ESock::MLegacyControlApiExt*& aInterface);
	virtual TInt ControlL(TUint aOptionLevel, TUint aOptionName, TDes8& aOption, ESock::MPlatsecApiExt* aPlatsecItf);

	virtual void ReturnInterfacePtrL(ADataMonitoringProtocolReq*& aInterface);
	virtual void RequestDataTransferred(ESock::CDataMonitoringResponder*& aResponder, TSubSessionUniqueId aClientId);
	virtual void CancelDataTransferredRequest(TSubSessionUniqueId aClientId);
	virtual void RequestDataReceivedNotification(ESock::CDataMonitoringResponder*& aResponder, TUint32 aDelta, TUint32 aReceivedBytes, TSubSessionUniqueId aClientId);
	virtual void CancelDataReceivedNotificationRequest(TSubSessionUniqueId aClientId);
	virtual void RequestDataSentNotification(ESock::CDataMonitoringResponder*& aResponder, TUint32 aDelta, TUint32 aSentBytes, TSubSessionUniqueId aClientId);
	virtual void CancelDataSentNotificationRequest(TSubSessionUniqueId aClientId);

protected:
	CDummyTierManager(ESock::CTierManagerFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap);
	virtual ESock::MProviderSelector* DoCreateProviderSelectorL(const Meta::SMetaData& aSelectionPreferences);
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

private:
	void ScheduleResponder(ESock::CDataMonitoringResponder*& aResponder);
	void CancelResponder();
	void DeleteResponder();
private:
	CAsyncResponder *iAsyncResponder;
	};

#endif //DUMMYPR_TIERMANAGER


