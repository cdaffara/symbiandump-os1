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

#include "LbsPrivacyNotifier.h"
#include "LbsPrivacyNotifierImpl.h"

#include <e32std.h>
#include <e32def.h>
#include <e32base.h>

/**
Static constructor
*/
EXPORT_C CLbsPrivacyNotifier* CLbsPrivacyNotifier::NewL(TPrivacyDialogType aDlg)
	{
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
	if (iBody)
		delete iBody;		
	}

/**
Set the observer that will receive the result of the dialog.
*/
EXPORT_C void CLbsPrivacyNotifier::SetObserver(MLbsPrivacyNotifierObserver* aObserver)
{
	ASSERT(iBody != NULL);
	iBody->SetObserver(aObserver);	
}

/**
Start the display of the dialog. 
*/
EXPORT_C void CLbsPrivacyNotifier::DisplayL(TInt& aErr, const TLbsExternalRequestInfo& aRequestInfo)
{
	ASSERT(iBody != NULL);
	iBody->DisplayL(aErr, aRequestInfo);
}

/**
Cancel the outstanding dialog.
*/
EXPORT_C void CLbsPrivacyNotifier::Cancel()
{
	ASSERT(iBody != NULL);
	//iBody->Cancel();	
}

