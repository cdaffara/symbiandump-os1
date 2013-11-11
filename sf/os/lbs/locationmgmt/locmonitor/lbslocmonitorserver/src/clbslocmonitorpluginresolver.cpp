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
* Description: Implementation of CLbsLocMonitorPluginResolver class
*
*/

#include <centralrepository.h>
#include <lbs/lbslocinfoconverterpluginbase.hrh>
#include "clbslocmonitorpluginresolver.h"
#include "locmonitorsettingskeys.h"
#include "lbsdevloggermacros.h"

// Constants
static const TInt KLeastPluginPriority = 0;

//----------------------------------------------------------------------------
// CLbsLocMonitorPluginResolver::NewL
//
//-----------------------------------------------------------------------------
CLbsLocMonitorPluginResolver* CLbsLocMonitorPluginResolver::NewL()
    {
    LBSLOG(ELogP1,"CLbsLocMonitorPluginResolver::NewL()");
    CLbsLocMonitorPluginResolver* self = new (ELeave) CLbsLocMonitorPluginResolver;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

//----------------------------------------------------------------------------
// CLbsLocMonitorPluginResolver::CLbsLocMonitorPluginResolver
//
//-----------------------------------------------------------------------------
CLbsLocMonitorPluginResolver::CLbsLocMonitorPluginResolver():
CActive(EPriorityStandard), iDefaultConverterPluginId(KNullUid) 
    {
    LBSLOG(ELogP1,"CLbsLocMonitorPluginResolver::CLbsLocMonitorPluginResolver()");
    CActiveScheduler::Add(this);
    }

//----------------------------------------------------------------------------
// CLbsLocMonitorPluginResolver::ConstructL
//
//-----------------------------------------------------------------------------
void CLbsLocMonitorPluginResolver::ConstructL()
    {
    LBSLOG(ELogP1,"CLbsLocMonitorPluginResolver::ConstructL()");
    iEcomSession = &(REComSession::OpenL());
    // Find the default plugin.
    FindDefaultConverterPluginL();
    // Register for ecom data change.
    iEcomSession->NotifyOnChange(iStatus);
    SetActive();
    }

//----------------------------------------------------------------------------
// CLbsLocMonitorPluginResolver::~CLbsLocMonitorPluginResolver
//
//-----------------------------------------------------------------------------
CLbsLocMonitorPluginResolver::~CLbsLocMonitorPluginResolver()
    {
    LBSLOG(ELogP1,"CLbsLocMonitorPluginResolver::~CLbsLocMonitorPluginResolver()");
    // Cancel outstanding notify request.
    Cancel();
    iEcomSession->Close();
    REComSession::FinalClose();
    }

//----------------------------------------------------------------------------
// CLbsLocMonitorPluginResolver::DefaultConverterUid
//
//-----------------------------------------------------------------------------
TInt CLbsLocMonitorPluginResolver::DefaultConverterUid(TUid& aConverterPluginUid)
    {
    LBSLOG(ELogP1,"CLbsLocMonitorPluginResolver::DefaultConverterUid()");
    if(iDefaultConverterPluginId == KNullUid)
        {
        return KErrNotFound;
        }
    aConverterPluginUid = iDefaultConverterPluginId;
    return KErrNone;
    }

//----------------------------------------------------------------------------
// CLbsLocMonitorPluginResolver::FindDefaultConverterPluginL
//
//-----------------------------------------------------------------------------
void CLbsLocMonitorPluginResolver::FindDefaultConverterPluginL()
    {
    LBSLOG(ELogP1,"CLbsLocMonitorPluginResolver::FindDefaultConverterPluginL()");
    RImplInfoPtrArray implementationArray;
    CleanupClosePushL(implementationArray);
    
    iEcomSession->ListImplementationsL(TUid::Uid(KLbsLocInfoConverterPlugInInterfaceId),
                                       implementationArray);
    
    TInt count = implementationArray.Count();
    // Leave if there are no plugin available.
    if(!count)
        {
        CleanupStack::Pop(&implementationArray); 
        implementationArray.ResetAndDestroy();
        // Reset the default id.
        iDefaultConverterPluginId = KNullUid;
        return;
        }
    
    // Initialise both phoneVendorImpl and otherVendorImpl to NULL.
    CImplementationInformation* phoneVendorImpl = NULL;
    CImplementationInformation* otherVendorImpl = NULL;
    
    // Get the phone's vendor id from the cenrep.
    CRepository* repository = NULL;
    TVendorId phoneVendorId(0);
   
    TRAPD(error,repository = CRepository::NewL(KCRUidLocMonitorSettings));
    if(error == KErrNone)
        {
        TInt vendorId;
        error = repository->Get(EPhoneVendorId,vendorId);
        if(error == KErrNone)
            {
            phoneVendorId.iId = vendorId;
            }
        delete repository;
        }
    
    if(error == KErrNone)
        {
        // The default plugin to be used is the highest priority plugin provided by the 
        // phone vendor if any else the highest priority plugin from other vendor. 
        for(TInt i=0;i<count;i++)
            {
            // Check if the implementation belongs to phone vendor. If it belongs to phone vendor 
            // and if it is the first implementation that we have emcountered,we store the handle
            // to it else the high priority among the previously stored and current one is stored. 
            if(phoneVendorId == implementationArray[i]->VendorId())
                {
                if(!phoneVendorImpl)
                    {
                    phoneVendorImpl = implementationArray[i];
                    }
                else
                    {
                    TInt currentHighestPriority,currentImplPriority;
                    currentHighestPriority = PriorityInfo(phoneVendorImpl->OpaqueData());
                    currentImplPriority = PriorityInfo(implementationArray[i]->OpaqueData());
                    
                    // Store the handle to current implementation if its priority  
                    // is higher than currently stored implementation.
                    if(currentImplPriority > currentHighestPriority)
                        {
                        phoneVendorImpl = implementationArray[i];
                        }
                    }
                }
            else
                {
                // Other vendor implementation is required only if current vendor implementation
                // is not available. Hence the search for the high priority other vendor implementation
                // should continue till the phone vendor plug in is found.
                if(!phoneVendorImpl)
                    {
                    if(!otherVendorImpl)
                        {
                        otherVendorImpl = implementationArray[i];
                        }
                    else
                        {
                        TInt currentHighestPriority,currentImplPriority;
                        currentHighestPriority = PriorityInfo(otherVendorImpl->OpaqueData());
                        currentImplPriority = PriorityInfo(implementationArray[i]->OpaqueData());
                        
                        // Store the handle to current implementation if its priority  
                        // is higher than currently stored implementation.
                        if(currentImplPriority > currentHighestPriority)
                            {
                            otherVendorImpl = implementationArray[i];
                            }
                        }
                    }
                }
            }
        }
    else
        {
        for(TInt i=0;i<count;i++)
            {
            if(!otherVendorImpl)
                {
                otherVendorImpl = implementationArray[i];
                }
            else
                {
                TInt currentHighestPriority,currentImplPriority;
                currentHighestPriority = PriorityInfo(otherVendorImpl->OpaqueData());
                currentImplPriority = PriorityInfo(implementationArray[i]->OpaqueData());
                
                // Store the handle to current implementation if its priority  
                // is higher than currently stored implementation.
                if(currentImplPriority > currentHighestPriority)
                    {
                    otherVendorImpl = implementationArray[i];
                    }
                }

            }
        }

    if(phoneVendorImpl)
        {
        iDefaultConverterPluginId = phoneVendorImpl->ImplementationUid();
        }
    else if(otherVendorImpl)
        {
        iDefaultConverterPluginId = otherVendorImpl->ImplementationUid();
        }

    CleanupStack::Pop(&implementationArray); 
    implementationArray.ResetAndDestroy();
    }


//----------------------------------------------------------------------------
// CLbsLocMonitorPluginResolver::RunL
//
//-----------------------------------------------------------------------------
void CLbsLocMonitorPluginResolver::RunL()
    {
    LBSLOG(ELogP1,"CLbsLocMonitorPluginResolver::RunL()");
    if(KErrNone == iStatus.Int())
        {
        FindDefaultConverterPluginL();
        }
    // Register for ecom data change.
    iEcomSession->NotifyOnChange(iStatus);
    SetActive();
    }


//----------------------------------------------------------------------------
// CLbsLocMonitorPluginResolver::DoCancel
//
//-----------------------------------------------------------------------------
void CLbsLocMonitorPluginResolver::DoCancel()
    {
    LBSLOG(ELogP1,"CLbsLocMonitorPluginResolver::DoCancel()");
    iEcomSession->CancelNotifyOnChange(iStatus);
    }

//----------------------------------------------------------------------------
// CLbsLocMonitorPluginResolver::PriorityInfo
//
//-----------------------------------------------------------------------------
TInt CLbsLocMonitorPluginResolver::PriorityInfo(const TDesC8& aOpaqueData)
    {
    LBSLOG(ELogP1,"CLbsLocMonitorPluginResolver::PriorityInfo()");
    TInt opaqueDataLength = aOpaqueData.Length();
    if(!opaqueDataLength)
        {
        return KLeastPluginPriority;
        }
    
    TLex8 lex(aOpaqueData);
    
    TInt iterator = 0;
    // Iterate through the descriptor till we get colon.
    while(lex.Get() != ':')
        {
        iterator++;
        if(iterator == opaqueDataLength)
            {
            return KLeastPluginPriority;
            }
        }
    // Neglect space if any
    while(lex.Peek() == ' ')
        {
        lex.Inc();
        if(iterator == opaqueDataLength)
            {
            return KLeastPluginPriority;
            }
        }
    
    TInt priority;
    lex.Val(priority);
    return priority;
    }

//End of file
