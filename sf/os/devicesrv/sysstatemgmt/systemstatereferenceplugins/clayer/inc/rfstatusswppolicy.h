// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
*/

#ifndef __RFSTATUSSWPPOLICY_H__
#define __RFSTATUSPSWPOLICY_H__

#include <f32file.h>
#include <ssm/ssmswppolicy.h>
#include <ssm/ssmconditionalcallback.h>

class CSsmCommandListResourceReader;

NONSHARABLE_CLASS(CRFStatusSwPPolicy ) : public CBase, public MSsmSwpPolicy, public MSsmConditionalCallback
	{
public:
	IMPORT_C static MSsmSwpPolicy* NewL();
	
	//from MSsmSwpPolicy
	void Initialize(TRequestStatus& aStatus);
	void InitializeCancel();
	void Release();
	TResponse TransitionAllowed(const TSsmSwp& aSwp, const RMessagePtr2& aMessage);
	void PrepareCommandList(const TSsmSwp& aSwp, TRequestStatus& aStatus);
	void PrepareCommandListCancel();
	CSsmCommandList* CommandList();
	void HandleCleReturnValue(const TSsmSwp& aSwp, TInt aError, TInt aSeverity, TRequestStatus& aStatus);
	void HandleCleReturnValueCancel();
	
	//from MSsmConditionalCallback
	TBool ConditionalCommandAllowedL(CResourceFile& aResourceFile, TInt aResourceId);

private:
	//from CBase
	CRFStatusSwPPolicy ();
	~CRFStatusSwPPolicy ();
	void ConstructL();

private:
	RFs iFs;
	CSsmCommandListResourceReader* iCommandListResourceReader;
	};

#endif //__RFSTATUSSWPPOLICY_H__
