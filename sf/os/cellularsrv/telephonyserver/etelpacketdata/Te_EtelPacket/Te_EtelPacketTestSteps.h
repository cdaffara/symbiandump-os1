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
// EtelPacketTestSteps.h
// This is the header file for re-usable test steps for RPacketContext functionality
// 
//

/**
 @file
 @internalComponent 
*/
   
   
#if (!defined __ETELPACKETTESTSTEPS_H__)
#define __ETELPACKETTESTSTEPS_H__

#include <etelpckt.h>
#include "Te_EtelPacketTestStepBase.h"

_LIT(KConnectEtelLoadTSY,"ConnectEtelLoadTSY");
_LIT(KOpenPhone,"OpenPhone");
_LIT(KClosePhone,"ClosePhone");
_LIT(KDisconnectEtelUnloadTSY,"DisconnectEtelUnloadTSY");
_LIT(KOpenPacketService,"OpenPacketService");
_LIT(KClosePacketService,"ClosePacketService");
_LIT(KOpenContext,"OpenContext");
_LIT(KCloseContext,"CloseContext");

/**
	Connect to Etel and load TSY
*/
class CTestConnectEtelLoadTSY : public CTestStep
{
public:
	CTestConnectEtelLoadTSY(RTelServer& aTelServer);
	~CTestConnectEtelLoadTSY(){};
	
	virtual enum TVerdict doTestStepL( void );

private:
	RTelServer& iTelServer;	
};

/**
	Disconnect from Etel and Unload TSY
*/
class CTestDisconnectEtelUnloadTSY : public CTestStep
{
public:
	CTestDisconnectEtelUnloadTSY(RTelServer& aTelServer);
	~CTestDisconnectEtelUnloadTSY(){};
	
	virtual enum TVerdict doTestStepL( void );

private:
	RTelServer& iTelServer;	
};


/**
	Open the phone
*/
class CTestOpenPhone : public CTestStep
{
public:
	CTestOpenPhone(RTelServer& aTelServer, RPhone& aPhone);
	~CTestOpenPhone(){};

	virtual enum TVerdict doTestStepL( void );

private:
	RTelServer& iTelServer;
	RPhone& iPhone;
	
};

/**
	Close the phone
*/
class CTestClosePhone : public CTestStep
{
public:
	CTestClosePhone(RPhone& aPhone);
	~CTestClosePhone(){};

	virtual enum TVerdict doTestStepL( void );
	
private:
	RPhone& iPhone;
};

/**
	Open the packet Service
*/
class CTestOpenPacketService : public CTestStep
{
public:
	CTestOpenPacketService(RPacketService& aService, RPhone& aPhone);
	~CTestOpenPacketService(){};

	virtual enum TVerdict doTestStepL( void );
	
private:
	RPacketService& iPacketService;
	RPhone& iPhone;
};

/**
	Close the packet Service
*/
class CTestClosePacketService : public CTestStep
{
public:
	CTestClosePacketService(RPacketService& aService);
	~CTestClosePacketService(){};

	virtual enum TVerdict doTestStepL( void );
	
private:
	RPacketService& iPacketService;
};



/**
	Open the context
*/
class CTestOpenContext : public CTestStep
{
public:
	CTestOpenContext(RTelServer& aTelServer, RPacketService& aPacketService, RPacketContext& aContext, TName& aContextName);
	~CTestOpenContext(){};

	virtual enum TVerdict doTestStepL( void );
	
private:
	RTelServer& iTelServer;
	RPacketService& iPacketService;
	RPacketContext& iContext;	
	TName& iContextName;
};

/**
	Close the context
*/
class CTestCloseContext : public CTestStep
{
public:
	CTestCloseContext(RPacketContext& aPacketContext);
	~CTestCloseContext(){};

	virtual enum TVerdict doTestStepL( void );
private:
	RPacketContext& iContext;	
};

#endif //(__ETELPACKETTESTSTEPS_H__)
