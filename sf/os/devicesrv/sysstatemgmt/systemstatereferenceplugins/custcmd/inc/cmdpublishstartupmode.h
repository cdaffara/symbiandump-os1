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
//

/**
 @file
 @internalComponent
 @released
*/

#ifndef __CMDPUBLISHSTARTUPMODE_H__
#define __CMDPUBLISHSTARTUPMODE_H__

#include <ssm/ssmcustomcommand.h>
#include <ssm/ssmadaptationcli.h>

class CRepository;

NONSHARABLE_CLASS (CCustomCmdPublishStartupMode) : public CActive, public MSsmCustomCommand
	{
public:
	static CCustomCmdPublishStartupMode* NewL();

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
	CCustomCmdPublishStartupMode();
	~CCustomCmdPublishStartupMode();
	
	void PublishStartupModeL();
	void PublishHiddenResetL();

private:
    enum TCmdPublishStartupModeState
        {
        EInitial = 1,
        ERequestingStartupMode,
        ERequestingHiddenReset
        };

	RSsmMiscAdaptation iSsmMiscAdaptation;
	TRequestStatus* iReturnStatus;
	CRepository* iRepository;
	TPckgBuf<TInt> iStartupModePckg;

	//Return package to fetch Hidden reset indication from adaptation
	TPckgBuf<TBool> iHiddenResetPckg;

    // Internal state of the command
    TCmdPublishStartupModeState iState;
	};

#endif // __CMDPUBLISHSTARTUPMODE_H__
