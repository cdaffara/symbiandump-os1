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
// Reference Default SubConnection Provider class definition.
// 
//

/**
 @file
 @internalComponent
*/


#if !defined(__REFERENCESCPR_DEFAULTSUBCONNPROV_H__)
#define __REFERENCESCPR_DEFAULTSUBCONNPROV_H__


#include "ReferenceSCPR_subconnProv.h"


class CReferenceDefaultSubConnectionProvider : public CSubConnectionProviderBase, public MConnectionDataClient
/** 
CReferenceDefaultSubConnectionProvider is a reference implementation for a shared subconnection,
that is: either 
* a subconnection explicitly requested by a client openning an RSubConnection in EAttachToDefault mode
* an implicitly created shared subconnection.
Typically reserved and shared subconnection providers, although represent the
same layer/technology, require different implementations of CSubConnectionProviderBase. 
This example code takes that into account and provides two separate classes for reserved and 
shared subconnections respectivelly. Should the implementation differ little or not differ at all,
developers are encouraged to introduce inheritance between the two types or implement just one
universal CSubConnectionProviderBase subclass, if sensible.
The following should be read like a class diagram for a CSubConnectionProviderBase:
===================================================================================

                            [MSubConnectionControlClient]
LAYER N-1                               ^
                                        |
    [MSubConnectionDataClient]<-n-      n         [MSubConnectionDataClient]<-n-
                                  |     |              /\                       |
LAYER N                           1     1             is a                      1
                                  |     |              ||                       |
    [ServiceAccessPoint]<-n--1-[CSubConnectionProviderBase]-1-----n->[CConnectionProviderBase]
                                        |
                                        n
                                        |
                                        1
LAYER N+1                               v
                                     [CSubConnectionProviderBase]
====================================================================================
@internalComponent
@released since v9.2
*/
	{
public:	    

	//-=========================================================
	// MConnectionDataClient methods
	//-=========================================================	
	virtual void Notify(TNotify aNotifyType, CConnectionProviderBase* aConnProvider, TInt aError, const CConNotificationEvent* aConNotificationEvent);
  	virtual void AttachToNext(CSubConnectionProviderBase* aSubConnProvider);
	virtual void ConnectionGoingDown(CConnectionProviderBase& aConnProvider);
	virtual void ConnectionError(TInt aStage, TInt aError);



	//-=========================================================
	// CSubConnectionProviderBase methods
	//-=========================================================
	virtual void DoDataClientJoiningL(MSubConnectionDataClient& aDataClient);
	virtual void DoDataClientLeaving(MSubConnectionDataClient& aDataClient);
	virtual void DoControlClientJoiningL(MSubConnectionControlClient& aControlClient);
	virtual void DoControlClientLeaving(MSubConnectionControlClient& aControlClient);
	virtual void DoSourceAddressUpdate(MSubConnectionDataClient& aDataClient, const TSockAddr& aSource);
	virtual void DoDestinationAddressUpdate(MSubConnectionDataClient& aDataClient, const TSockAddr& aDestination);
	virtual void DoDataClientRouted(MSubConnectionDataClient& aDataClient, const TSockAddr& aSource, const TSockAddr& aDestination, const TDesC8& aConnectionInfo);
	virtual void DoParametersAboutToBeSetL(CSubConParameterBundle& aParameterBundle);
	virtual TInt DoControl(TUint aOptionLevel, TUint aOptionName, TDes8& aOption);
	virtual CConnDataTransfer& DoDataTransferL();
	virtual MConnectionDataClient* DoSelfConnectionDataClient();
    virtual void DoStartL();
	virtual void DoStop();
	virtual CSubConnectionProviderBase* DoNextLayer();
	

protected:
	
	//-=========================================================
	// Custom methods
	//-=========================================================	
	friend class CReferenceSubConnectionProviderFactory;	

	CReferenceDefaultSubConnectionProvider(CSubConnectionProviderFactoryBase& aFactory, 
	                                       CConnectionProviderBase& aConnProvider);
	static CReferenceDefaultSubConnectionProvider* NewL(CSubConnectionProviderFactoryBase& aFactory, 
	                                       CConnectionProviderBase& aConnProvider);
	void ConstructL();
	virtual ~CReferenceDefaultSubConnectionProvider();
	
public:
	__FLOG_DECLARATION_MEMBER;
	
	};

#endif	// __REFERENCESCPR_DEFAULTSUBCONNPROV_H__