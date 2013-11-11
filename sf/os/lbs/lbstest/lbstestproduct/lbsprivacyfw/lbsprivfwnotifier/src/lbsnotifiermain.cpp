/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

// INCLUDES
#include <e32base.h>
#include <ecom/implementationproxy.h>

#include "lbsprivnotifier.h"
#include "lbsqnnnotifier.h"

#include "lbsprivfwnotifier.hrh"

LOCAL_C void CreateNotifiersQnNL(CArrayPtrFlat<MEikSrvNotifierBase2>* aNotifiers)
    {

	CQnNPlugin* notifier = CQnNPlugin::NewL();
	CleanupStack::PushL(notifier);
	aNotifiers->AppendL(notifier->NotifierBase());
	CleanupStack::Pop(notifier); // Do not destroy, this is handled by the framework
    
    }    

LOCAL_C void CreateNotifiersPrivNotifierL(CArrayPtrFlat<MEikSrvNotifierBase2>* aNotifiers)
    {
    MEikSrvNotifierBase2* notifier1 = CLbsVerifyNotifier::NewL();
    CleanupStack::PushL(notifier1);
    aNotifiers->AppendL(notifier1);
    CleanupStack::Pop(notifier1);

    MEikSrvNotifierBase2* notifier2 = CLbsNotifyNotifier::NewL();
    CleanupStack::PushL(notifier2);
    aNotifiers->AppendL(notifier2);
    CleanupStack::Pop(notifier2);
    }    

CArrayPtr<MEikSrvNotifierBase2>* NotifierArrayQnN()
    {
    CArrayPtrFlat<MEikSrvNotifierBase2>* notifiers=new CArrayPtrFlat<MEikSrvNotifierBase2>(1);
    if (notifiers)
        {
        TRAPD(err, CreateNotifiersQnNL(notifiers));
        if(err)
            {
            TInt count = notifiers->Count();
            while(count--)
                {
                (*notifiers)[count]->Release();
                }
            delete notifiers;
            notifiers = NULL;
            }
        }
    return(notifiers);
    }

CArrayPtr<MEikSrvNotifierBase2>* NotifierArrayPrivNotifier()
    {
    CArrayPtrFlat<MEikSrvNotifierBase2>* notifiers=new CArrayPtrFlat<MEikSrvNotifierBase2>(2);
    if (notifiers)
        {
        TRAPD(err, CreateNotifiersPrivNotifierL(notifiers));
        if(err)
            {
            TInt count = notifiers->Count();
            while(count--)
                {
                (*notifiers)[count]->Release();
                }
            delete notifiers;
            notifiers = NULL;
            }
        }
    return(notifiers);
    }
    
    
//Adding ECOM Support
const TImplementationProxy ImplementationTable[] =
{
    IMPLEMENTATION_PROXY_ENTRY(KPosPrivacyNotifierImplUid, NotifierArrayQnN),
    IMPLEMENTATION_PROXY_ENTRY(KPrivacyNotiferUid, NotifierArrayPrivNotifier)
};
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
}

//  End of File  

