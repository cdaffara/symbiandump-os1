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

// INCLUDES
#include <e32base.h>
#include <eiknotapi.h>
#include <ecom/ImplementationProxy.h>
#include <EPos_PrivacyNotifier.hrh>
#include "Lcf_CLcfPrivacyDialogPlugin.h"

// ==================== LOCAL FUNCTIONS ====================

// ---------------------------------------------------------
// CreateNotifiersL Creates the notifier.Used
// by the NotifierArray() factory function below
// ---------------------------------------------------------
//
LOCAL_C void CreateNotifiersL(CArrayPtrFlat<MEikSrvNotifierBase2>* aNotifiers)
	{
	CLcfPrivacyDialogPlugin* notifier = CLcfPrivacyDialogPlugin::NewL();
	CleanupStack::PushL(notifier);
	aNotifiers->AppendL(notifier->NotifierBase());
	CleanupStack::Pop(notifier); // Do not destroy, this is handled by the framework
	}


// ---------------------------------------------------------
// Lib main entry point. Notifier entry point.
// Note that function must not leave!
// ---------------------------------------------------------
//

LOCAL_C CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
	{
	CArrayPtrFlat<MEikSrvNotifierBase2>* notifiers =
        new CArrayPtrFlat<MEikSrvNotifierBase2>(1);
        if (notifiers)
        {
          TRAPD(err, CreateNotifiersL(notifiers));
          if (err)
          { // release any notifiers we have created
		          TInt count = notifiers->Count();
              while (--count >= 0)
                {
                (*notifiers)[count]->Release();
                }
              delete notifiers;
              notifiers = NULL;
          }
        }
	return notifiers;
	}

const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KPosPrivacyNotifierImplUid, NotifierArray)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy) ;
	return ImplementationTable;
	}


//  End of File  
