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

#ifndef __CMDPUBLISHSIMOWNEDANDCHANGED_H__
#define __CMDPUBLISHSIMOWNEDANDCHANGED_H__

#include <etelmm.h>
#include <ssm/ssmcustomcommand.h>
#include <ssm/ssmadaptationcli.h>

NONSHARABLE_CLASS (CCustomCmdPublishSimOwnedAndChanged) : public CActive, public MSsmCustomCommand
	{
public:
	static CCustomCmdPublishSimOwnedAndChanged* NewL();

	// from MSsmcustomCommand
	TInt Initialize(CSsmCustomCommandEnv* aCmdEnv);
	void Execute(const TDesC8& aParams, TRequestStatus& aStatus);
	void Close();
	void Release();
	void ExecuteCancel();

protected:
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	CCustomCmdPublishSimOwnedAndChanged();
	~CCustomCmdPublishSimOwnedAndChanged();
	void ConstructL();
	void CompleteClientRequest(TInt aReason);

	void SimOwned();
	void SimChanged();
	
	void ReadAndUpdateStoredImsiL(RMobilePhone::TMobilePhoneSubscriberId& aReadValue,
	    const RMobilePhone::TMobilePhoneSubscriberId aUpdateValue);
	TInt CompareToLastStoredImsi(const RMobilePhone::TMobilePhoneSubscriberId aNewValue);
#ifdef TEST_CUSTCMD_MACRO
    friend class CCustomCmdTestPublishSimOwnedAndChanged;
#endif
private:
	enum TPublishSimOwnedAndChangedState
		{
		EPublishSimOwnedAndChangedIdle = 0,
		EPublishSimOwned,
		EPublishSimChanged,
		};

	RSsmSimAdaptation iSsmSimAdaptation;
	TPublishSimOwnedAndChangedState iState;
	TRequestStatus* iExecuteRequest;
	TPckgBuf<TBool> iBooleanPckg;

	// Session with telephony server
    RTelServer iServer;

    // Phone sub-session handle
    RMobilePhone iPhone;

    //Value received from Etel will be stored here
    RMobilePhone::TMobilePhoneSubscriberId iSubscriberId;
    HBufC* iTsyModuleName;

	};

#endif // __CMDPUBLISHSIMOWNEDANDCHANGED_H__
