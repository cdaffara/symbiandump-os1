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
// contentupdateproxy.cpp
// Proxy object for receiving SUS surface content update notifications and rerouting them to nativestream 
//


#include <e32def.h>
#include <graphics/suerror.h>
#include "streammap.h"
#include "surfacestream.h"
#include "contentupdateproxy.h"

CExtensionContainer* CContentUpdateProxy::NewL(TInt aScreenNum, COpenWfcStreamMap*	aSingleton,TInt aInternalVersion,TInt aPriority)
	{
	CContentUpdateProxy* pInstance = new(ELeave) CContentUpdateProxy();
	CleanupStack::PushL(pInstance);
	pInstance->ConstructL(aScreenNum,aSingleton,aInternalVersion,aPriority);
	CleanupStack::Pop(pInstance);
	return pInstance;
	}
void CContentUpdateProxy::ConstructL(TInt aScreenNum, COpenWfcStreamMap*	aSingleton,TInt aInternalVersion,TInt aPriority)
	{
	iScreenNum=aScreenNum;
	iSingleton=aSingleton;
	iInternalVersion=aInternalVersion;
	iPriority=aPriority;
	}
CContentUpdateProxy::CContentUpdateProxy():
iHeap(User::Heap())
	{
	}
CContentUpdateProxy::~CContentUpdateProxy()
	{
	
	}

TInt CContentUpdateProxy::Extension_(TUint aExtensionId, TAny*& aRetPtr, TAny* a1)
	{
	switch (aExtensionId)
		{
		case MCompositionSurfaceUpdate::ETypeId:
			{
			aRetPtr=static_cast<MCompositionSurfaceUpdate*>(this);
			return KErrNone;
			}
		default:
			{
			return CExtensionContainer::Extension_(aExtensionId,aRetPtr,a1);
			}
		}
	}

TInt CContentUpdateProxy::ApiVersion()
	{
	return KContentUpdateProxyVersion;
	}

TVersion CContentUpdateProxy::InternalVersion()
	{
	return TVersion(iInternalVersion>>24,(iInternalVersion>>16)&0xff,iInternalVersion&0xffff);
	}

void CContentUpdateProxy::ContentUpdated      
	(	const TSurfaceId&  aSurface,     		TInt		aBuffer,
		const TRegion*     aRegion,
		TRequestStatus*    aStatusConsumed,
		TRequestStatus*    aStatusDisplayed,	TUint32*	aTimeStamp,
		TRequestStatus*    aStatusDispXTimes,	TInt*		aDisplayedXTimes)
	{
    NFLOG(("### CContentUpdateProxy srf(0x%x:0x%x:0x%x:0x%x),"
            "\n\t\t buff(%d),\n\t\t rg(0x%x),\n\t\t rsConsumed(0x%x),\n\t\t rsDsp(0x%x),\n\t\t ts(0x%x),"
            "\n\t\t rsDXT(0x%x),\n\t\t dXT(%d)", 
            aSurface.iInternal[0], aSurface.iInternal[1], aSurface.iInternal[2], aSurface.iInternal[3],
            aBuffer, aRegion, aStatusConsumed, aStatusDisplayed, aTimeStamp, aStatusDispXTimes, aDisplayedXTimes ? *aDisplayedXTimes: 0));
    
    COpenWfcStreamMap* stream = NULL;
    TRAPD(err,stream = &COpenWfcStreamMap::InstanceL());
    if (err != KErrNone)
        {
        NFLOG(("### ERROR - EARLY EXIT * CContentUpdateProxy::ContentUpdated()"));
        // cancel notifications
        CompleteStatus(
                aStatusConsumed,
                aStatusDisplayed,
                aStatusDispXTimes,
                err);
        return;
        }
    RHeap* oldHeap = NULL;
	if (&iHeap != &(User::Heap()))
	    {
	    oldHeap = User::SwitchHeap(stream->GetMainHeap());
	    }
	CSurfaceStream* ns = NULL;
	ns = stream->Find(aSurface);

	if (ns)
		{
		RegisterNotifications(*ns,
                              aBuffer,
                              aStatusDisplayed, aTimeStamp,
                              aStatusDispXTimes, aDisplayedXTimes,
                              aStatusConsumed, aRegion);
		
		ns->ReleaseReference();
		}
	else
	    {
	    // cancel notifications
	    CompleteStatus(aStatusConsumed,
	            aStatusDisplayed,
	            aStatusDispXTimes,
	            KErrSurfaceNotRegistered);
	    }
	if (oldHeap)
	    {
	    (void)User::SwitchHeap(oldHeap);
	    }
    NFLOG(("### EXIT * CContentUpdateProxy::ContentUpdated()"));
	}

void CContentUpdateProxy::CompleteStatus(
        TRequestStatus*    aStatusConsumed,
        TRequestStatus*    aStatusDisplayed,
        TRequestStatus*    aStatusDispXTimes,
        TInt aReason)
    {
    if (aStatusConsumed)
        {
        User::RequestComplete(aStatusConsumed, aReason);
        }
    if (aStatusDisplayed)
        {
        User::RequestComplete(aStatusDisplayed, aReason);
        }
    if (aStatusDispXTimes)
        {
        User::RequestComplete(aStatusDispXTimes, aReason);
        }
    }

void CContentUpdateProxy::RegisterNotifications(CSurfaceStream& aNs,
                                                 TInt            aBuffer,
                                                 TRequestStatus* aStatusDisplayed, TUint32* aTimeStamp,
                                                 TRequestStatus* aStatusDispXTimes, TInt* aDisplayedXTimes,
                                                 TRequestStatus* aStatusConsumed, const TRegion* aRegion)
    {
    if (KErrNone != aNs.CheckBufferNumber(aBuffer, aStatusDisplayed, aStatusDispXTimes, aStatusConsumed))
        {
        return;
        }
    
    TInt updateFlag = 0;
    if (aStatusConsumed)
        {
        *aStatusConsumed = KRequestPending;
        updateFlag |= ESOWF_EventAvailable;
        }
    if (aStatusDisplayed)
        {
        *aStatusDisplayed = KRequestPending;
        updateFlag |= ESOWF_EventDisplayed;
        }
    if (aStatusDispXTimes)
        {
        *aStatusDispXTimes = KRequestPending;
        updateFlag |= ESOWF_EventDisplayedX;
        }
    
    aNs.SetNewNotifications(aBuffer, 
                            aStatusDisplayed, 
                            aTimeStamp, 
                            aStatusDispXTimes, 
                            aDisplayedXTimes, 
                            aStatusConsumed, 
                            aRegion,
                            iScreenNum);
    }

