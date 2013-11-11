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
//Custom command to validate RTC and publish the result using Publish and Subscribe mechanism

/**
 @file
 @internalComponent
 @released
*/

#ifndef __CMDVALIDATERTC_H__
#define __CMDVALIDATERTC_H__

#include <ssm/ssmcustomcommand.h>
#include <ssm/ssmadaptationcli.h>

NONSHARABLE_CLASS (CCustomCmdValidateRTC) :  public CActive, public MSsmCustomCommand
	{
public:
	static CCustomCmdValidateRTC* NewL();

	// from MSsmcustomCommand
	TInt Initialize(CSsmCustomCommandEnv* aCmdEnv);
	void Execute(const TDesC8& aParams, TRequestStatus& aStatus);
	void Close();
	void Release();
	void ExecuteCancel();

protected:
    virtual void RunL();
    virtual void DoCancel();
    virtual TInt RunError(TInt aError);

private:
	CCustomCmdValidateRTC();
	~CCustomCmdValidateRTC();
	void CompleteClientRequest(TInt aError);
private:
	RSsmRtcAdaptation iSsmRtcAdaptation;
	TPckgBuf<TBool> iPckgValidateRtc;
	TRequestStatus* iExecuteRequest;
	};

#endif // __CMDVALIDATERTC_H__
