// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Class implementation of CLbsPrivacyNotifier
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#include <e32std.h>
#include <e32def.h>
#include <e32base.h>
#include <lbs/lbslocclasstypes.h>

#include "lbsdevloggermacros.h"
#include "lbsprivacynotifierimpl.h"
#include "lbsprivacynotifier.h"



/**
Static constructor
*/
EXPORT_C CLbsPrivacyNotifier* CLbsPrivacyNotifier::NewL(TPrivacyDialogType aDlg)
	{
	LBSLOG(ELogP9, "->S CLbsPrivacyNotifier::NewL(1) lbsnetworkrequesthandler.exe\n");
	LBSLOG2(ELogP9, "  > TPrivacyDialogType aDlg  = %d\n", aDlg);
	CLbsPrivacyNotifier* self=new (ELeave) CLbsPrivacyNotifier;
	CleanupStack::PushL(self);
	self->ConstructL(aDlg);
	CleanupStack::Pop();
	return self;
	}
	
/**
Static constructor
*/
EXPORT_C CLbsPrivacyNotifier* CLbsPrivacyNotifier::NewL(TPrivacyDialogType aDlg, TTimeIntervalMicroSeconds32 aTimeout)
	{
	LBSLOG(ELogP9, "->S CLbsPrivacyNotifier::NewL(2) lbsnetworkrequesthandler.exe\n");
	LBSLOG2(ELogP9, "  > TPrivacyDialogType aDlg  = %d\n", aDlg);
	LBSLOG2(ELogP9, "  > TTimeIntervalMicroSeconds32 aTime  = %d\n", aTimeout.Int());

	CLbsPrivacyNotifier* self=new (ELeave) CLbsPrivacyNotifier;
	CleanupStack::PushL(self);
	self->ConstructL(aDlg, aTimeout);
	CleanupStack::Pop();
	return self;
	}


/**
1st phase Constructor
*/
CLbsPrivacyNotifier::CLbsPrivacyNotifier()
	{
	}

/**
2nd phase Constructor
*/
void CLbsPrivacyNotifier::ConstructL(TPrivacyDialogType aDlg)
	{
	iBody = CLbsPrivacyNotifierImpl::NewL(aDlg);
	}

/**
2nd phase Constructor
*/
void CLbsPrivacyNotifier::ConstructL(TPrivacyDialogType aDlg,
TTimeIntervalMicroSeconds32 aTimeout)
	{
	iBody = CLbsPrivacyNotifierImpl::NewL(aDlg, aTimeout);
	}

/**
Destructor
*/
EXPORT_C CLbsPrivacyNotifier::~CLbsPrivacyNotifier()
	{
	LBSLOG(ELogP9, "->S CLbsPrivacyNotifier::~CLbsPrivacyNotifier() lbsnetworkrequesthandler.exe\n");
	if (iBody)
		delete iBody;		
	}

/**
Set the observer that will receive the result of the dialog.
*/
EXPORT_C void CLbsPrivacyNotifier::SetObserver(MLbsPrivacyNotifierObserver* aObserver)
{
	LBSLOG(ELogP9, "->S CLbsPrivacyNotifier::SetObserver() lbsnetworkrequesthandler.exe\n");
	ASSERT(iBody != NULL);
	iBody->SetObserver(aObserver);	
}

/**
Start the display of the dialog. 
*/
EXPORT_C void CLbsPrivacyNotifier::DisplayL(TInt& aErr, const TLbsExternalRequestInfo& aRequestInfo)
{
	LBSLOG(ELogP9, "->S CLbsPrivacyNotifier::DisplayL() lbsnetworkrequesthandler.exe\n");
	LBSLOG2(ELogP9, "  > TInt aErr  = %d\n", aErr);
	LBSLOG(ELogP9, "  > TLbsExternalRequestInfo aRequestInfo  =\n");
	//LBSLOG_TLBSEXTERNALREQ(aRequestInfo);

	ASSERT(iBody != NULL);
	iBody->DisplayL(aErr, aRequestInfo);
}

/**
Cancel the outstanding dialog.
*/
EXPORT_C void CLbsPrivacyNotifier::Cancel()
{
	LBSLOG(ELogP9, "->S CLbsPrivacyNotifier::Cancel() lbsnetworkrequesthandler.exe\n");
	ASSERT(iBody != NULL);
	iBody->Cancel();	
}

