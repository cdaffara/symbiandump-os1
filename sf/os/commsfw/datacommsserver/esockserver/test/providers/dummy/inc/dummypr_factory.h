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
// TestDummy Factory class definition.
// 
//

/**
 @file
 @internalComponent
*/

#if !defined(__DummyPR_FACTORY_H__)
#define __DummyPR_FACTORY_H__

#include <comms-infras/ss_protflow.h>
#include <comms-infras/ss_subconnflow.h>
#include <comms-infras/ss_subconnprov.h>
#include <comms-infras/ss_metaconnprov.h>
#include <comms-infras/ss_connprov.h>
#include <comms-infras/ss_tiermanager.h>
#include <comms-infras/ss_nodemessages.h>

using namespace ESock;

_LIT(KDummyProtocolName, "dummy");
_LIT(KDumberProtocolName, "dumber");

//-=========================================================
//
// CDummyTierManagerFactory
//
//-=========================================================
class CDummyTierManagerFactory : public ESock::CTierManagerFactoryBase
	{
public:
    enum { iUid = 0x1028302B };
	static CDummyTierManagerFactory* NewL(TAny* aParentContainer);

protected:
	CDummyTierManagerFactory(TUid aTierTypeId, TUid aFactoryUid, ESock::CTierManagerFactoryContainer& aParentContainer);
	virtual ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	};

//-=========================================================
//
// CDummyMetaConnectionProviderFactory
//
//-=========================================================
class CDummyMetaConnectionProviderFactory : public ESock::CMetaConnectionProviderFactoryBase
	{
public:
    enum { iUid = 0x1028302C };
	static CDummyMetaConnectionProviderFactory* NewL(TAny* aParentContainer);
	
protected:
	CDummyMetaConnectionProviderFactory(TUid aFactoryUid, ESock::CMetaConnectionFactoryContainer& aParentContainer);
	ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	};
	
//-=========================================================
//
// CDummyConnectionProviderFactory
//
//-=========================================================
class CDummyConnectionProviderFactory : public ESock::CConnectionProviderFactoryBase
	{
public:
    enum { iUid = 0x1028302D };
	static CDummyConnectionProviderFactory* NewL(TAny* aParentContainer);

protected:
	CDummyConnectionProviderFactory(TUid aFactoryId, ESock::CConnectionFactoryContainer& aParentContainer);
	virtual ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	};
	
class CDummyHangingConnectionProviderFactory : public ESock::CConnectionProviderFactoryBase
	{
public:
    enum { iUid = 0x1028302F };
	static CDummyHangingConnectionProviderFactory* NewL(TAny* aParentContainer);

protected:
	CDummyHangingConnectionProviderFactory(TUid aFactoryId, ESock::CConnectionFactoryContainer& aParentContainer);
	virtual ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	};

class CDummyVanillaConnectionProviderFactory : public ESock::CConnectionProviderFactoryBase
    {
public:
    enum { iUid = 0x10283030 };
    static CDummyVanillaConnectionProviderFactory* NewL(TAny* aParentContainer);

protected:
    CDummyVanillaConnectionProviderFactory(TUid aFactoryId, ESock::CConnectionFactoryContainer& aParentContainer);
    virtual ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
    };
	
//-=========================================================
//
// CDummySubConnectionProviderFactory
//
//-=========================================================
class CDummySubConnectionProviderFactory : public ESock::CSubConnectionProviderFactoryBase
	{
public:
    enum { iUid = 0x1028302E };
	static CDummySubConnectionProviderFactory* NewL(TAny* aParentContainer);

protected:
	CDummySubConnectionProviderFactory(TUid aFactoryId, ESock::CSubConnectionFactoryContainer& aParentContainer);
	virtual ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	};


//-=========================================================
//
// CDummyExtendedSubConnectionProviderFactory
//
//-=========================================================
class CDummyExtendedSubConnectionProviderFactory : public ESock::CSubConnectionProviderFactoryBase
	{
public:
    enum { iUid = 0x10285D8D };
	static CDummyExtendedSubConnectionProviderFactory* NewL(TAny* aParentContainer);

protected:
	CDummyExtendedSubConnectionProviderFactory(TUid aFactoryId, ESock::CSubConnectionFactoryContainer& aParentContainer);
	virtual ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	};


//-=========================================================
//
// CDummyVanillaSubConnectionProviderFactory
//
//-=========================================================
class CDummyVanillaSubConnectionProviderFactory : public ESock::CSubConnectionProviderFactoryBase
    {
public:
    enum { iUid = 0x10283031 };
    static CDummyVanillaSubConnectionProviderFactory* NewL(TAny* aParentContainer);

protected:
    CDummyVanillaSubConnectionProviderFactory(TUid aFactoryId, ESock::CSubConnectionFactoryContainer& aParentContainer);
    virtual ESock::ACommsFactoryNodeId* DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
    };



//-=========================================================
//
// CDummyFlowFactory
//
//-=========================================================
class CDummyFlowFactory : public ESock::CSubConnectionFlowFactoryBase
	{
public:
	enum { iUid = 0x10285D8F };
	static CDummyFlowFactory* NewL(TAny* aConstructionParameters);
	~CDummyFlowFactory();

protected:
	CDummyFlowFactory(TUid aFactoryId, ESock::CSubConnectionFlowFactoryContainer& aParentContainer);

	// from CSubConnectionFlowFactoryBase
	virtual ESock::CSubConnectionFlowBase* DoCreateFlowL(CProtocolIntfBase* aProtocolIntf, ESock::TFactoryQueryBase& aQuery);
	virtual ESock::ACommsFactoryNodeId* DoFindOrCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	virtual TServerProtocolDesc* DoCreateFlowDescriptionL(TInt aProtocol);
	};


#endif	// __DummyPR_FACTORY_H__

