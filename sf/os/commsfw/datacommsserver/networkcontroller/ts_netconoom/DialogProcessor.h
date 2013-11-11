// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__DIALOGPROCESSOR_H__)
#define __DIALOGPROCESSOR_H__

#include <e32base.h>
#include <comms-infras/connectionsettings.h>
#include <agentdialog.h>

class MDialogProcessorObserver
	{
public:
	virtual void MDPOSelectComplete(TInt aError, const TConnectionSettings& aSettings);
	virtual void MDPOSelectModemAndLocationComplete(TInt aError, const TConnectionSettings& aSettings);
	virtual void MDPOWarnComplete(TInt aError, TBool aResponse);
	virtual void MDPOLoginComplete(TInt aError);
	virtual void MDPOAuthenticateComplete(TInt aError);
	virtual void MDPOReconnectComplete(TInt aError);
	virtual void MDPOReadPctComplete(TInt aError);
	virtual void MDPODestroyPctComplete(TInt aError);
	virtual void MDPOQoSWarningComplete(TInt aError, TBool aResponse);
	};

class CCommsDbAccess;
class CDialogProcessor : public CBase
	{
public:
	~CDialogProcessor();
	static CDialogProcessor* NewL(TInt aPriority = 0);

	void SelectConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs);
	void SelectConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs, TInt aLastError);
	void SelectModemAndLocation(MDialogProcessorObserver& aObserver);
	void Reconnect(MDialogProcessorObserver& aObserver);
	void WarnNewConnection(MDialogProcessorObserver& aObserver, const TConnectionPrefs& aPrefs, const TDesC* aNewIapName, const TIspConnectionNames* aNewConnectionNames, TInt aLastError);

private:
	CDialogProcessor();
	void ConstructL();

	static TInt DialogCb(TAny* aThisPtr);
	void DialogComplete();

private:
	enum TDialogAction
		{
		ESelectModemAndLocation,
		ESelectConnection,
		EWarnNewConnection,
		EReconnect
		};

private:
	CAsyncCallBack* iDialogCb;
	MDialogProcessorObserver* iObserver;
	TDialogAction iAction;
	TConnectionSettings iSettings;
	};

#endif // __DIALOGPROCESSOR_H__

