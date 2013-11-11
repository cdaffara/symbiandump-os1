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
// CORECPR.h
// Core CPR
// 
//

/**
 @file
 @internalComponent
*/

#ifndef DUMMYNODE_H
#define DUMMYNODE_H

#include <elements/mm_activities.h>
#include <elements/mm_node.h>
#include <elements/factory.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemTestingdmynd, "ElemTestingdmynd");
#endif

namespace Dummy
{

enum TDummyNodeActivityId
	{
	EActivityStart                  = MeshMachine::KActivityNull + 1,
	EActivityStop                   = 2,
	EActivityDestroy                = 3,
	EActivityClientJoin             = 4,
	EActivityClientLeave            = 5,
	EActivityError                  = 6,//if found in node activity map is ran by the MM
	};

class TDummyClientType : public Messages::TClientType
	{
public:
    enum TType
        {
        //-=============================================
        //
        // 1. Types 32bits (a client can only have one type)
        //
        // TClientType = BIT0..BIT7
        //-=============================================
        EClient       = 0x00000100,
        EServProvider = 0x00000200
		};

	enum TFlags
		{
        //-=============================================
        //
        // 2. Flags 32bits (a client can have many flags)
        //
        // TClientType = BIT0..BIT7
        //-=============================================
        EActive       = 0x00000100, //Client that has started us
        EStarting     = 0x00000200, //Service Provider that is starting
		EStopping     = 0x00000400, //Service Provider that is stopping
		EStarted      = 0x00000800  //Service Provider that is started (stopped if not set)
        };

public:
	TDummyClientType()
	:	Messages::TClientType(EUnknown)
		{
		}

	TDummyClientType(TUint32 aClientType, TUint32 aClientFlags)
	:	Messages::TClientType(aClientType, aClientFlags)
		{
		}
	};

class CTestNodeBase : public CBase,
                      public Factories::AFactoryObject,
                      public MeshMachine::AMMNodeIdBase
    {
    friend class CTestFactoryBase;

public:
	IMPORT_C virtual ~CTestNodeBase();
	virtual TUid ServiceProviderUid() const = 0;

	void CommenceJoin()
		{
		--iOutstandingJoinCount;
		__ASSERT_DEBUG(iOutstandingJoinCount>=0, User::Panic(KSpecAssert_ElemTestingdmynd, 1)); //It should never get below 0
		}

	TBool IsJoinOutstanding()
		{
		return iOutstandingJoinCount > 0;
		}

protected:
    IMPORT_C CTestNodeBase(Factories::CFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap);
    IMPORT_C void Received(MeshMachine::TNodeContextBase& aContext);
    IMPORT_C void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

private:
	TInt iOutstandingJoinCount; //In our demo the Factory increments this, node decrements
    };

} //Dummy

#endif
//DUMMYNODE_H

