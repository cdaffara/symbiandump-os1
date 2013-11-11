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


#include "ctlbsagpsmoduleeventlistener.h"

EXPORT_C CT_AgpsModuleEventListener* CT_AgpsModuleEventListener::NewL(MT_AgpsModuleEventHandler& aHandler)
	{
	CT_AgpsModuleEventListener* self = new(ELeave) CT_AgpsModuleEventListener(aHandler);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CT_AgpsModuleEventListener::~CT_AgpsModuleEventListener()
	{
	Cancel();
	iModeChangesProperty.Close();
	}

CT_AgpsModuleEventListener::CT_AgpsModuleEventListener(MT_AgpsModuleEventHandler& aHandler)
	: CActive(EPriorityHigh), iHandler(aHandler)
	{
	CActiveScheduler::Add(this);
	}

void CT_AgpsModuleEventListener::ConstructL()
	{
	User::LeaveIfError(iModeChangesProperty.Attach(KUidSystemCategory, ELbsTestAGpsModuleModeChanges));
	iModeChangesProperty.Subscribe(iStatus);
	SetActive();
	}

void CT_AgpsModuleEventListener::RunL()
	{
	TInt err = iStatus.Int();
	iModeChangesProperty.Subscribe(iStatus);
	SetActive();
	if(KErrNone == err)
		{
		TLbsGpsOptionsArray options;
		TPckg<TLbsGpsOptionsArray> pckgOptions(options);
		err = iModeChangesProperty.Get(pckgOptions);
		if(KErrNone == err)
			{
			iHandler.OnSetGpsOptions(options);
			}
		}
	}

void CT_AgpsModuleEventListener::DoCancel()
	{
	iModeChangesProperty.Cancel();
	}
