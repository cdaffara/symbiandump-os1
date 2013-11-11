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

#ifndef __TESTNODE_H__
#define	__TESTNODE_H__

#include <elements/nm_node.h>

class CTestNode : public CBase,
                  public Messages::ANodeIdBase
	{
public:
	static CTestNode* NewLC();

	~CTestNode();
	void StartTest();

	//ANode
	void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

private:
	void ConstructL();
	CTestNode();

private:
	//We need this (at EPriorityIdle) to make sure that all message trafic has completed before we stop the scheduler
	CAsyncCallBack* iStopTestCb;
	};




#endif	// __TESTNODE_H__

