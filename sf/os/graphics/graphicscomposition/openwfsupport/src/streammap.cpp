// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// streammap.cpp: creates and maintaines the association between native stream and a TSurfaceId
// 
//

#include "streammap.h"
#include <graphics/updateserverprovider.h>
#include <graphics/surfacemanager.h>
#include <e32property.h>
#include <e32std.h>
#include <e32cmn.h>
#include "openwfcpanic.h"
#include "surfacestream.h"
#include "contentupdateproxy.h"

static const TInt KOpenWfcInteropCleanupKey = 0x10286FC5;

COpenWfcStreamMap* COpenWfcStreamMap::pInstance = NULL;

TUint32 COpenWfcStreamMap::HashFunction(const TSurfaceId& aHashKey)
	{
	TPckgC<TSurfaceId> pckg(aHashKey);
	return DefaultHash::Des8(pckg);
	}

EXPORT_C COpenWfcStreamMap& COpenWfcStreamMap::InstanceL()
	{
	// would have been nice to try to protect this area with a mutex, however,
	// the preliminary analyses show we can guarantee the safe creation of the mutex
	// by invoking InstanceL() from the place the CGce instances are created
	// If necessary, in future, the code can be expanded by using a named mutex.
	if (!pInstance)
		{
		COpenWfcStreamMap* newInstance = new(ELeave)COpenWfcStreamMap();
		CleanupStack::PushL(newInstance);
		newInstance->ConstructL();
		CleanupStack::Pop(newInstance);
		pInstance=newInstance;
		}
	return *pInstance;
	}

CSurfaceStream* COpenWfcStreamMap::Find(const TSurfaceId& aSurfaceId)
	{
	Guard g(iMutex);
	CSurfaceStream** ns = iMap.Find(aSurfaceId);
	// used if statement to ease debug help
	if (ns)
		{
		(*ns)->AddReference();
		return *ns;
		}
	return NULL;
	}

CSurfaceStream* COpenWfcStreamMap::AcquireL(const TSurfaceId& aSurfaceId)
	{
	Guard* pGuard = new(ELeave) Guard(iMutex);
	User::LeaveIfNull(pGuard);
	CleanupDeletePushL(pGuard);
	
	CSurfaceStream** ns = NULL;
	CSurfaceStream* ret = NULL;
	ns = iMap.Find(aSurfaceId);
	if (ns)
		{
		WFCI_ASSERT_DEBUG((*ns), EOwfPanicInvalidHasMap);  // should have never happened
		ret = *ns;
		}
	else
		{
		ret = CSurfaceStream::NewLC(aSurfaceId);
		User::LeaveIfError(iMap.Insert(aSurfaceId, ret));
		CleanupStack::Pop();
		}
	ret->AddReference();
	CleanupStack::PopAndDestroy(pGuard);
	return ret;
	}

EXPORT_C TInt COpenWfcStreamMap::Count()
	{
	Guard g(iMutex);
	TInt count = iMap.Count();
	return count;
	}

EXPORT_C RSurfaceManager& COpenWfcStreamMap::SurfaceManager()
	{
    WFCI_ASSERT_DEBUG(iSurfaceManager, EOwfPanicInvalidHasMap);
	return *iSurfaceManager;
	}

TInt COpenWfcStreamMap::LockDestroy(CSurfaceStream* aStream)
	{
	Guard g(iMutex);
	TInt ret = KErrNone;
	if (aStream)
		{
		if (aStream->RemainingReference())
			{
			const TSurfaceId& surfaceId = aStream->SurfaceId();
			CSurfaceStream** ns = iMap.Find(surfaceId);
			// used if statement to ease debug help
			if (ns && ((*ns) == aStream))
				{
				TInt ret = iMap.Remove(surfaceId);
				delete aStream;
				}
			else
				{
				ret = KErrNotFound;
				}
			}
		else	// RemainingReference
			{
			ret = KErrInUse;
			}
		}
	else	// aStream
		{
		ret = KErrArgument;
		}
	return ret;
	}

COpenWfcStreamMap::COpenWfcStreamMap():
iMap(THashFunction32<TSurfaceId>(COpenWfcStreamMap::HashFunction), TIdentityRelation<TSurfaceId>()),
iSurfaceManager(NULL),
iRegisteredUpdaters()
	{
	}

TInt COpenWfcStreamMap::DeleteSingleton(TAny* aData)
/**
 * aData    A pointer to the heap registered for this callback
 */
    {
    // Blank the property for this callback
    RThread t;
    RProperty prop;
    TCallBack cb(NULL, NULL);
    TPckgC<TCallBack> cbPckg(cb);
    prop.Set(TUid::Uid(t.SecureId().iId), KOpenWfcInteropCleanupKey, cbPckg);
    prop.Close();
    t.Close();
    
    if (aData == &User::Heap())
        {
        delete pInstance;
        pInstance = NULL;                
        return ETrue;
        }
    return EFalse;
    }

COpenWfcStreamMap::~COpenWfcStreamMap()
	{
	iMutex.Wait();
        {
        THashMapIter<TSurfaceId, CSurfaceStream*> iter(iMap);
        const TSurfaceId* nextKey = iter.NextKey();
        CSurfaceStream* const* ns = NULL;
        while (nextKey)
            {
            ns = iter.CurrentValue();
            if (ns && *ns)
                {
                delete (*ns);
                }
            nextKey = iter.NextKey();		
            }
        }
	iMap.Close();
    if (iSurfaceManager)
        {
        iSurfaceManager->Close();
        delete iSurfaceManager;
        iSurfaceManager = NULL;
        }
	iMutex.Signal();
	iMutex.Close();
	
        {
        THashMapIter<TInt32, CExtensionContainer*> iter(iRegisteredUpdaters);
        const TInt32* nextKey = iter.NextKey();
        CExtensionContainer* const* extensionContainer = NULL;
        while (nextKey)
            {
            extensionContainer = iter.CurrentValue();
            if (extensionContainer && *extensionContainer)
                {
                delete (*extensionContainer);
                }
            nextKey = iter.NextKey();       
            }
        }
	iRegisteredUpdaters.Close();
	}

void COpenWfcStreamMap::ConstructL()
	{
	User::LeaveIfError(iMutex.CreateLocal());
	iMap.Reserve(iInitialSize);
	TSurfaceId surface = TSurfaceId::CreateNullId();
	User::LeaveIfError(iMap.Insert(surface, NULL));

	iSurfaceManager = new(ELeave) RSurfaceManager();
	User::LeaveIfError(iSurfaceManager->Open());
	RProcess process;
	TUidType uidType = process.Type();
	const TInt32 KWservUid = 268450592;
	const TUid& uid1 = uidType[2];

	if(uid1.iUid == KWservUid) //only wserv process can start the server
		{
		StartSurfaceUpdateServer(iSurfUpdateServ);
		}
	
	// Register the cleanup function in a property defined by WServ
	RThread t;		
	TUid category = {t.SecureId().iId};
	RProperty prop;
	TCallBack cb(DeleteSingleton, &User::Heap());
    TPckgC<TCallBack> cbPckg(cb);
    
    // If the property cannot be set the assumption is that the cleanup is not needed
    (void) prop.Set(category, KOpenWfcInteropCleanupKey, cbPckg);
    prop.Close();
	t.Close();	
	
	// StreamMap is constructed from main thread
	SetMainHeap();
	}

COpenWfcStreamMap::Guard::Guard(RFastLock& aLock):
iLock(aLock)
	{
	iLock.Wait();
	}

COpenWfcStreamMap::Guard::~Guard()
	{
	iLock.Signal();
	}

EXPORT_C RHeap* COpenWfcStreamMap::GetMainHeap()
	{
	return iMainHeap;
	}

void COpenWfcStreamMap::SetMainHeap()
	{
	iMainHeap = &User::Heap();
	}

TInt COpenWfcStreamMap::RegisterScreenNotifications(TInt aScreenNum, TInt aPriority,TInt aInternalVersion)
	{
	COpenWfcStreamMap* pInstance = NULL;
	TRAPD(err,pInstance = &COpenWfcStreamMap::InstanceL());
	if (err != KErrNone)
	    {
	    return err;
	    }
	
	if (iRegisteredUpdaters.Find(aScreenNum)!= NULL)
		{
		return KErrAlreadyExists;
		}
	
	CExtensionContainer* updateProxy = NULL;
	TRAP(err, updateProxy = CContentUpdateProxy::NewL(aScreenNum, pInstance,aInternalVersion,aPriority));
	if (err)
		{
		return err;
		}
	
	if ((err = iRegisteredUpdaters.Insert(aScreenNum, updateProxy)) != KErrNone)
	    {
        delete updateProxy;
	    return err;
	    }

    if (!pInstance->iSurfUpdateServ)
        {
        return KErrNotReady;    //For testing purposes the backend proxy still exists.
        }
	err = iSurfUpdateServ->Register(aScreenNum, updateProxy, aPriority);
	if (err!=KErrNone)
	    {
        delete updateProxy;
        iRegisteredUpdaters.Remove(aScreenNum);
	    }
	return err;
	}

CExtensionContainer* COpenWfcStreamMap::RegisteredScreenNotifications(TInt aScreenNum)
    {
    return *iRegisteredUpdaters.Find(aScreenNum);
    }

TInt COpenWfcStreamMap::UnregisterScreenNotifications(TInt aScreenNum)
	{
	TInt err = KErrNone;
	CExtensionContainer** backend = iRegisteredUpdaters.Find(aScreenNum);
	if (backend)
	    {
	    delete *backend;
	    iRegisteredUpdaters.Remove(aScreenNum);
        if (iSurfUpdateServ)
            {
            err = iSurfUpdateServ->Register(aScreenNum, NULL, 0);
            }
        else
            {
            err = KErrNotReady;
            }
	    }
	else
	    {
	    err = KErrNotFound;
	    }

	return err;
	}
