// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This dll will be used by tsus_loadplugin to publish the SWP
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#ifndef __TSUS_SWPPOLICY_SUSTEST_H__
#define __TSUS_SWPPOLICY_SUSTEST_H__


#include <ssm/ssmstate.h>
#include <ssm/ssmcommandlistresourcereader.h>
#include <ssm/ssmcmd.hrh>
#include <ssm/ssmconditionalcallback.h>
#include <ssm/ssmswppolicy.h>


NONSHARABLE_CLASS(CSsmSwpPolicyTest) : public CBase, public MSsmSwpPolicy, public MSsmConditionalCallback
	{
public:
	IMPORT_C static MSsmSwpPolicy* NewL(); //lint !e1511
	
	//from MSsmSwpPolicy
	void ConstructL();
	void Initialize(TRequestStatus& aStatus);
	void InitializeCancel();
	void Release();
	TResponse TransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage);
	void PrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus);
	void PrepareCommandListCancel();
	CSsmCommandList* CommandList();
	void HandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus);
	void HandleCleReturnValueCancel();

	// from MSsmConditionalCallback
	TBool ConditionalCommandAllowedL(CResourceFile& aResourceFile, TInt aResourceId);

private:
	//from CBase
	CSsmSwpPolicyTest();
	~CSsmSwpPolicyTest();

private:
	CSsmCommandList* iCommandList;
	CSsmCommandListResourceReader* iCommandListResourceReader;
	RFs iFs;
	};

#endif
