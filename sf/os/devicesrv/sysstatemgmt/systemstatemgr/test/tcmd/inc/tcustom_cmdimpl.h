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
// Reference code for a custom command used by ssma
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
 

#ifndef __CMDTEST_CUSTOMCOMMANDIMPL_H__
#define __CMDTEST_CUSTOMCOMMANDIMPL_H__

#include <e32base.h>
#include <f32file.h>

#include <ssm/ssmcustomcommand.h>

class RefCustomCommand : public MSsmCustomCommand
	{
public:
	RefCustomCommand();
	~RefCustomCommand();
	TInt Initialize(CSsmCustomCommandEnv* aCmdEnv);
	void Execute(const TDesC8& aParams, TRequestStatus& aRequest);
	void Close();
	void Release();
	void ExecuteCancel();

private:	
	// Members for tcustom_cmd_rleak.dll
	RFs iFs1;
	RFs iFs2;
	RFs iFs3;
		
	TInt iCount;
	};

#endif // __CMDTEST_CUSTOMCOMMANDIMPL_H__
