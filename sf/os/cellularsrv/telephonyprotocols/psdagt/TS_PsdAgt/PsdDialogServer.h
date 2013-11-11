// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#if (!defined __PSDDIALOGSERVER_H__)
#define __PSDDIALOGSERVER_H__

#include <e32base.h>
#include <dummyagentdialog.h>

class CDialogServer : public CActive
	{
public:
	class MObserver
		{
	public:
		virtual void DialogHasAppeared()=0;
		};
public:
	CDialogServer();
	~CDialogServer();
	TBool Open();
	void Close();
	void RequestDialogAppearanceNotification(MObserver* aObserver);
	void SetReconnectResponse(TBool aResponse);
	void SetQoSWarnResponse(TBool aResponse);
private:
	void RunL();
	void DoCancel();
private:
	RGenConAgentDialogServer iDlgSvr;
	MObserver* iObserver;
	};

#endif /* __PSDDIALOGSERVER_H__*/
