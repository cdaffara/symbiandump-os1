// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mediaclientvideodisplaybody.h"
#include "mediaclientvideotrace.h"
#include "mediaclientpolicyserverclient.h"
#include <surfaceeventhandler.h>
#include <mmf/plugin/mmfmediaclientextdisplayinterface.hrh>
#include <e32cmn.h>
#include <ecom/ecom.h>
#include <centralrepository.h>

const TUid KCRUidTvoutSettings = {0x1020730B};
const TUint32 KSettingsTvAspectRatio = 0x00000001;

#ifdef MEDIA_CLIENT_SURFACE_NOT_REMOVED_FROM_CLIENT_WINDOW
// make sure that off screen is bottom right and not top left. This makes it more efficient for GCE backend
// to render
const TInt KHiddenExtentA = 2000; // rect Ax and Ay co-ordinate used to set extent off screen
const TInt KHiddenExtentB = 2001; // rect Bx and By co-ordinate used to set extent off screen
#endif

CMediaClientVideoDisplayBody* CMediaClientVideoDisplayBody::NewL(TInt aDisplayId, TBool aExtDisplaySwitchingControl)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::NewL +++");
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::NewL - aDisplayId %d", aDisplayId);
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::NewL - aExtDisplaySwitchingControl %d", aExtDisplaySwitchingControl);
    
    CMediaClientVideoDisplayBody* self = new (ELeave) CMediaClientVideoDisplayBody(aDisplayId);
	CleanupStack::PushL(self);
	self->ConstructL(aExtDisplaySwitchingControl);
	CleanupStack::Pop(self);
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::NewL ---");
	return self;
	}

CMediaClientVideoDisplayBody* CMediaClientVideoDisplayBody::NewL(TInt aDisplayId, const TSurfaceId& aSurfaceId,
		const TRect& aCropRect, TVideoAspectRatio aAspectRatio, TBool aExtDisplaySwitchingControl)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::NewL +++");
	DEBUG_PRINTF2("CMediaClientVideoDisplayBody::NewL - aDisplayId %d", aDisplayId);
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::NewL - aSurfaceId %08x:%08x:%08x:%08x", aSurfaceId.iInternal[3], aSurfaceId.iInternal[2], aSurfaceId.iInternal[1], aSurfaceId.iInternal[0]);
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::NewL - aCropRect %d,%d - %d,%d", aCropRect.iTl.iX, aCropRect.iTl.iY, aCropRect.iBr.iX, aCropRect.iBr.iY);
    DEBUG_PRINTF3("CMediaClientVideoDisplayBody::NewL - aAspectRatio %d/%d", aAspectRatio.iNumerator, aAspectRatio.iDenominator);
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::NewL - aExtDisplaySwitchingControl %d", aExtDisplaySwitchingControl);

	if(aSurfaceId.IsNull())
		{
		User::Leave(KErrArgument);
		}
	CMediaClientVideoDisplayBody* self = new(ELeave) CMediaClientVideoDisplayBody(aDisplayId, aSurfaceId, aCropRect, aAspectRatio);
	CleanupStack::PushL(self);
	self->ConstructL(aExtDisplaySwitchingControl);
	CleanupStack::Pop();
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::NewL ---");
	return self;
	}

void CMediaClientVideoDisplayBody::ConstructL(TBool aExtDisplaySwitchingControl)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::ConstructL +++");

	SetWindowArrayPtr2Client();

	// External display switching and wserv events are only possible if there is
	// an active scheduler in client thread
	if(CActiveScheduler::Current())
	    {
        CreateExtDisplayPluginL();
        iWsEventObserver = CMediaClientWsEventObserver::NewL(*this);

        iServerClient = CMediaClientPolicyServerClient::NewL();
        if(iServerClient->Connect() != KErrNone)
            {
            delete iServerClient;
            iServerClient = NULL;
            }
        
        if(IsSurfaceCreated() && iServerClient)
            {
            iServerClient->SetSurface(iSurfaceId);
            }
        
        // Try and enable display switching by default. If this leaves then do so quietly.
        // Either the client has no scheduler installed or the device does not support external
        // switching (i.e. no plugin was found) 
        TRAPD(err, SetExternalDisplaySwitchingL(aExtDisplaySwitchingControl));
        err = err; // remove compile warning
        DEBUG_PRINTF2("CMediaClientVideoDisplayBody::ConstructL SetExternalDisplaySwitchingL returned with %d", err);
	    }
	else
	    {
	    DEBUG_PRINTF("CMediaClientVideoDisplayBody::ConstructL No CActiveScheduler - ext display and focus features disabled ");
	    }
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::ConstructL ---");
	}

CMediaClientVideoDisplayBody::CMediaClientVideoDisplayBody(TInt aDisplayId, const TSurfaceId& aSurfaceId,
		const TRect& aCropRect, TVideoAspectRatio aAspectRatio) :
	iDisplayId(aDisplayId),
	iSurfaceId(aSurfaceId),
	iCropRect(aCropRect),
	iAspectRatio(aAspectRatio)
	{	
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::CMediaClientVideoDisplayBody +++");
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::CMediaClientVideoDisplayBody ---");
	}

CMediaClientVideoDisplayBody::CMediaClientVideoDisplayBody(TInt aDisplayId) :
	iDisplayId(aDisplayId)
	{	
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::CMediaClientVideoDisplayBody +++");
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::CMediaClientVideoDisplayBody ---");
	}

CMediaClientVideoDisplayBody::~CMediaClientVideoDisplayBody()
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::~CMediaClientVideoDisplayBody +++");

	// remove for whichever array is current
	RemoveBackgroundSurface(ETrue);

#ifdef MEDIA_CLIENT_SURFACE_NOT_REMOVED_FROM_CLIENT_WINDOW
    if(iSwitchedToExternalDisplay)
        {
        SetWindowArrayPtr2Client();
        RemoveBackgroundSurface(ETrue);
        }
#endif

	iClientWindows.Close();	
    iExtDisplayWindows.Close(); 

	delete iExtDisplayHandler;
	RemoveExtDisplayPlugin();
	REComSession::FinalClose();
	
    delete iWsEventObserver;

    delete iServerClient;
    
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::~CMediaClientVideoDisplayBody ---");
	}

void CMediaClientVideoDisplayBody::AddDisplayL(MMMFSurfaceEventHandler& aEventHandler)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::AddDisplayL +++");
	if (iEventHandler)
		{
		User::Leave(KErrInUse);
		}
		
	iEventHandler = &aEventHandler;
	
	if (IsSurfaceCreated())
		{
		iEventHandler->MmsehSurfaceCreated(iDisplayId, iSurfaceId, iCropRect, iAspectRatio);
		}
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::AddDisplayL ---");
	}
	
void CMediaClientVideoDisplayBody::AddDisplayWindowL(const RWindowBase* aWindow, const TRect& aClipRect, const TRect& aCropRegion, const TRect& aVideoExtent, 
															TReal32 aScaleWidth, TReal32 aScaleHeight, TVideoRotation aRotation, 
															TAutoScaleType aAutoScaleType, TInt aHorizPos, TInt aVertPos, RWindow* aWindow2)
	{	
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::AddDisplayWindowL +++");
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::AddDisplayWindowL - aWindow WsHandle 0x%X", aWindow->WsHandle());
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::AddDisplayWindowL - aClipRect %d,%d - %d,%d", aClipRect.iTl.iX, aClipRect.iTl.iY, aClipRect.iBr.iX, aClipRect.iBr.iY);
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::AddDisplayWindowL - aCropRegion %d,%d - %d,%d", aCropRegion.iTl.iX, aCropRegion.iTl.iY, aCropRegion.iBr.iX, aCropRegion.iBr.iY);
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::AddDisplayWindowL - aVideoExtent %d,%d - %d,%d", aVideoExtent.iTl.iX, aVideoExtent.iTl.iY, aVideoExtent.iBr.iX, aVideoExtent.iBr.iY);
    DEBUG_PRINTF3("CMediaClientVideoDisplayBody::AddDisplayWindowL - aScaleWidth %f, aScaleHeight %f", aScaleWidth, aScaleHeight);
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::AddDisplayWindowL - aRotation %d", aRotation);
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::AddDisplayWindowL - aAutoScaleType %d", aAutoScaleType);
    DEBUG_PRINTF3("CMediaClientVideoDisplayBody::AddDisplayWindowL - aHorizPos %d, aVertPos %d", aHorizPos, aVertPos);

	if (!IsRotationValid(aRotation))
		{
		User::Leave(KErrArgument);
		}

	if (!IsAutoScaleTypeValid(aAutoScaleType))
		{
		User::Leave(KErrArgument);
		}

	TInt pos = iClientWindows.Find(aWindow->WsHandle(), TWindowData::CompareByWsHandle);
	
	if (pos != KErrNotFound)
		{
		User::Leave(KErrInUse);
		}
	
	TWindowData winData(aWindow, aClipRect, aVideoExtent, aScaleWidth, aScaleHeight, aRotation, aAutoScaleType, aHorizPos, aVertPos, aWindow2);
	iClientWindows.AppendL(winData);
	
	iCropRegion = aCropRegion;
	
	TBool prevClientWindowIsInFocus = iClientWindowIsInFocus;
    UpdateFocus();
	
	if (IsSurfaceCreated())
		{
        // if first window was just added OR the new window has moved us from out of focus to in focus
        if(((iClientWindows.Count() == 1) || !prevClientWindowIsInFocus) && iClientRequestedExtDisplaySwitching &&
                iClientWindowIsInFocus && iExtDisplayConnected)
            {
            TRAPD(err, CreateExtDisplayHandlerL());
            DEBUG_PRINTF2("CMediaClientVideoDisplayBody::AddDisplayWindowL CreateExtDisplayHandlerL error %d", err);
            if(err == KErrNone)
                {
#ifdef MEDIA_CLIENT_SURFACE_NOT_REMOVED_FROM_CLIENT_WINDOW
                // attach surface to client window and hide
                // ignore error - no handling required
                SetHiddenBackgroundSurfaceOnClientWindow(winData);
#endif                
                // handle external display
                SetWindowArrayPtr2Ext();
                User::LeaveIfError(RedrawWindows(aCropRegion));
                }
            }
        
        if(!iSwitchedToExternalDisplay)
            {
            User::LeaveIfError(SetBackgroundSurface(winData, aCropRegion));
            }
		}
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::AddDisplayWindowL ---");
	}

void CMediaClientVideoDisplayBody::UpdateDisplayWindowL(const RWindowBase* aWindow, const TRect& aClipRect, const TRect& aCropRegion, const TRect& aVideoExtent, 
                                                            TReal32 aScaleWidth, TReal32 aScaleHeight, TVideoRotation aRotation, 
                                                            TAutoScaleType aAutoScaleType, TInt aHorizPos, TInt aVertPos, RWindow* aWindow2)
    {   
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::UpdateDisplayWindowL +++");
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::UpdateDisplayWindowL - aWindow WsHandle 0x%X", aWindow->WsHandle());
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::UpdateDisplayWindowL - aClipRect %d,%d - %d,%d", aClipRect.iTl.iX, aClipRect.iTl.iY, aClipRect.iBr.iX, aClipRect.iBr.iY);
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::UpdateDisplayWindowL - aCropRegion %d,%d - %d,%d", aCropRegion.iTl.iX, aCropRegion.iTl.iY, aCropRegion.iBr.iX, aCropRegion.iBr.iY);
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::UpdateDisplayWindowL - aVideoExtent %d,%d - %d,%d", aVideoExtent.iTl.iX, aVideoExtent.iTl.iY, aVideoExtent.iBr.iX, aVideoExtent.iBr.iY);
    DEBUG_PRINTF3("CMediaClientVideoDisplayBody::UpdateDisplayWindowL - aScaleWidth %f, aScaleHeight %f", aScaleWidth, aScaleHeight);
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::UpdateDisplayWindowL - aRotation %d", aRotation);
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::UpdateDisplayWindowL - aAutoScaleType %d", aAutoScaleType);
    DEBUG_PRINTF3("CMediaClientVideoDisplayBody::UpdateDisplayWindowL - aHorizPos %d, aVertPos %d", aHorizPos, aVertPos);

    if (!IsRotationValid(aRotation))
        {
        User::Leave(KErrArgument);
        }

    if (!IsAutoScaleTypeValid(aAutoScaleType))
        {
        User::Leave(KErrArgument);
        }

    TInt pos = iClientWindows.Find(aWindow->WsHandle(), TWindowData::CompareByWsHandle);
    User::LeaveIfError(pos);
    iClientWindows.Remove(pos);
    
    TWindowData winData(aWindow, aClipRect, aVideoExtent, aScaleWidth, aScaleHeight, aRotation, aAutoScaleType, aHorizPos, aVertPos, aWindow2);
    iClientWindows.AppendL(winData);
    
    TRect prevCropRegion = iCropRegion;
    iCropRegion = aCropRegion;
    
    if (IsSurfaceCreated())
        {
        if(ClientCropRegionChangeRequiresRedraw(prevCropRegion, aCropRegion, iCropRect))
            {
            // all windows need to be redrawn
            User::LeaveIfError(RedrawWindows(aCropRegion));
            }
        else
            {
            // only redraw affected window as other window data may have changed
            if(!iSwitchedToExternalDisplay)
                {
                User::LeaveIfError(SetBackgroundSurface(winData, aCropRegion));
                }
            }
        }

    DEBUG_PRINTF("CMediaClientVideoDisplayBody::UpdateDisplayWindowL ---");
    }

void CMediaClientVideoDisplayBody::RemoveDisplay()
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::RemoveDisplay +++");
	iEventHandler = NULL;
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::RemoveDisplay ---");
	}	

TInt CMediaClientVideoDisplayBody::RemoveDisplayWindow(const RWindowBase& aWindow)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::RemoveDisplayWindow +++");
	TInt pos = iClientWindows.Find(aWindow.WsHandle(), TWindowData::CompareByWsHandle);
	
	if (pos >= 0)
	    {
#ifdef MEDIA_CLIENT_SURFACE_NOT_REMOVED_FROM_CLIENT_WINDOW
        if(IsSurfaceCreated())
#else
	    if(IsSurfaceCreated() && !iSwitchedToExternalDisplay)
#endif
	        {
            iClientWindows[pos].iWindow->RemoveBackgroundSurface(ETrue);
            // Make sure all window rendering has completed before proceeding
            RWsSession* ws = iClientWindows[pos].iWindow->Session();
            if (ws)
                {
                ws->Finish();
                }
            }

	    iClientWindows.Remove(pos);
	    
	    TBool prevClientWindowIsInFocus = iClientWindowIsInFocus;
        UpdateFocus();

        // if only window was just removed OR removal has moved us from in focus to out of focus
        if((iClientWindows.Count() == 0 || prevClientWindowIsInFocus) && iSwitchedToExternalDisplay &&
                !iClientWindowIsInFocus)
            {
            RemoveBackgroundSurface(ETrue);
            SetWindowArrayPtr2Client();
            RemoveExtDisplayHandler();
            RedrawWindows(iCropRegion);
            }
		}
	
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::RemoveDisplayWindow ---");
	return pos;
	}


TInt CMediaClientVideoDisplayBody::SurfaceCreated(const TSurfaceId& aSurfaceId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio, const TRect& aCropRegion)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::SurfaceCreated +++");
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SurfaceCreated - aSurfaceId %08x:%08x:%08x:%08x", aSurfaceId.iInternal[3], aSurfaceId.iInternal[2], aSurfaceId.iInternal[1], aSurfaceId.iInternal[0]);
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SurfaceCreated - aCropRect %d,%d - %d,%d", aCropRect.iTl.iX, aCropRect.iTl.iY, aCropRect.iBr.iX, aCropRect.iBr.iY);
    DEBUG_PRINTF3("CMediaClientVideoDisplayBody::SurfaceCreated - aAspectRatio %d/%d", aAspectRatio.iNumerator, aAspectRatio.iDenominator);
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SurfaceCreated - aCropRegion %d,%d - %d,%d", aCropRegion.iTl.iX, aCropRegion.iTl.iY, aCropRegion.iBr.iX, aCropRegion.iBr.iY);

    TBool emitEvent = EFalse;
	if((iSurfaceId != aSurfaceId) && (!aSurfaceId.IsNull()))
		{
		emitEvent = ETrue;
		}

	iSurfaceId = aSurfaceId;
	iCropRect = aCropRect;
	iAspectRatio = aAspectRatio;
	iCropRegion = aCropRegion;
	
	if(iServerClient)
	    {
        iServerClient->SetSurface(iSurfaceId);
	    }
	
	if (emitEvent && iEventHandler)
		{
		iEventHandler->MmsehSurfaceCreated(iDisplayId, iSurfaceId, iCropRect, iAspectRatio);
		}

	TInt err = KErrNone;
    if((iClientWindows.Count() > 0) && iClientRequestedExtDisplaySwitching && iClientWindowIsInFocus)
        {
        if(iExtDisplayConnected && !iExtDisplayHandler)
            {
            TRAP(err, CreateExtDisplayHandlerL());
            DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SurfaceCreated CreateExtDisplayHandlerL error %d", err);
            if(err == KErrNone)
                {
#ifdef MEDIA_CLIENT_SURFACE_NOT_REMOVED_FROM_CLIENT_WINDOW
                SetHiddenBackgroundSurfaceOnAllClientWindows();
#endif          
                SetWindowArrayPtr2Ext();
                }
            }
        }
    
    err = RedrawWindows(aCropRegion);
    
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SurfaceCreated --- Return error %d", err);
	return err;
	}

void CMediaClientVideoDisplayBody::RemoveBackgroundSurface(TBool aTriggerRedraw)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::RemoveBackgroundSurface +++");
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::RemoveBackgroundSurface - iSurfaceId %08x:%08x:%08x:%08x", iSurfaceId.iInternal[3], iSurfaceId.iInternal[2], iSurfaceId.iInternal[1], iSurfaceId.iInternal[0]);

	if (IsSurfaceCreated())
		{
		RWsSession* ws = NULL;
		TInt count = iWindowsArrayPtr->Count();
	
		for (TInt i = 0; i < count; ++i)
			{
			(*iWindowsArrayPtr)[i].iWindow->RemoveBackgroundSurface(aTriggerRedraw);
            // Make sure all window rendering has completed before proceeding
            ws = (*iWindowsArrayPtr)[i].iWindow->Session();
            if (ws)
                {
                ws->Finish();
                }
			}
		}
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::RemoveBackgroundSurface ---");
	}

void CMediaClientVideoDisplayBody::RemoveSurface(TBool aControllerEvent)
    {
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::RemoveSurface +++");
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::RemoveSurface - iSurfaceId %08x:%08x:%08x:%08x", iSurfaceId.iInternal[3], iSurfaceId.iInternal[2], iSurfaceId.iInternal[1], iSurfaceId.iInternal[0]);

    if (IsSurfaceCreated())
        {
        RemoveBackgroundSurface(ETrue);

        if (iEventHandler  && aControllerEvent)
            {
            iEventHandler->MmsehRemoveSurface(iSurfaceId);
            }

        if(iSwitchedToExternalDisplay)
            {
            SetWindowArrayPtr2Client();
#ifdef MEDIA_CLIENT_SURFACE_NOT_REMOVED_FROM_CLIENT_WINDOW
            RemoveBackgroundSurface(ETrue);
#endif
            RemoveExtDisplayHandler();
            }
        iSurfaceId = TSurfaceId::CreateNullId();
        }
    
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::RemoveSurface ---");
    }

TInt CMediaClientVideoDisplayBody::SurfaceParametersChanged(const TSurfaceId& aSurfaceId, const TRect& aCropRect, TVideoAspectRatio aAspectRatio)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::SurfaceParametersChanged +++");
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SurfaceCreated - aSurfaceId %08x:%08x:%08x:%08x", aSurfaceId.iInternal[3], aSurfaceId.iInternal[2], aSurfaceId.iInternal[1], aSurfaceId.iInternal[0]);
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SurfaceCreated - aCropRect %d,%d - %d,%d", aCropRect.iTl.iX, aCropRect.iTl.iY, aCropRect.iBr.iX, aCropRect.iBr.iY);
    DEBUG_PRINTF3("CMediaClientVideoDisplayBody::SurfaceCreated - aAspectRatio %d/%d", aAspectRatio.iNumerator, aAspectRatio.iDenominator);

    if (!IsSurfaceCreated())
		{
		DEBUG_PRINTF("CMediaClientVideoDisplayBody::SurfaceParametersChanged --- Return error KErrNotSupproted");
		return KErrNotSupported;
		}

	if (iSurfaceId != aSurfaceId)
		{
		DEBUG_PRINTF("CMediaClientVideoDisplayBody::SurfaceParametersChanged --- Return error KErrInUse");
		return KErrInUse;
		}

	if (iEventHandler)
		{
		iEventHandler->MmsehSurfaceParametersChanged(iSurfaceId, aCropRect, aAspectRatio);
		}

	TInt error = KErrNone;
	if (iCropRect != aCropRect || iAspectRatio != aAspectRatio)
		{
		// We only need to redraw if the aspect ratio has changed, or the area of the video to
		// display (i.e the intersection of client crop region and surface crop rectangle) has changed.
		TBool redraw = EFalse;
		if (iAspectRatio != aAspectRatio || SurfaceCropRectChangeRequiresRedraw(iCropRect, aCropRect, iCropRegion))
			{
			redraw = ETrue;
			}

		iCropRect = aCropRect;
		iAspectRatio = aAspectRatio;

		if (redraw)
			{
			error = RedrawWindows(iCropRegion);
			}
		}
	else
		{
		DEBUG_PRINTF("CMediaClientVideoDisplayBody::SurfaceParametersChanged - Surface parameters have not changed");
		}

	DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SurfaceParametersChanged --- Return error %d", error);
	return error;
	}
	
TInt CMediaClientVideoDisplayBody::RedrawWindows(const TRect& aCropRegion)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::RedrawWindows +++");
	TInt err = KErrNone;	

	iCropRegion = aCropRegion;
	
	if(IsSurfaceCreated())
		{
		TInt count = iWindowsArrayPtr->Count();
	
		for (TInt i = 0; i < count; ++i)
			{
			err = SetBackgroundSurface((*iWindowsArrayPtr)[i], aCropRegion);
		
			if (err != KErrNone)
				{
				break;
				}
			}
		}
		
	DEBUG_PRINTF2("CMediaClientVideoDisplayBody::RedrawWindows --- return with %d", err);
	return err;
	}

void CMediaClientVideoDisplayBody::UpdateCropRegionL(const TRect& aCropRegion, TInt aPos, TBool aRedrawIndividualWindow)
    {
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::UpdateCropRegionL +++");

    TRect prevCropRegion(iCropRegion);
    iCropRegion = aCropRegion;
    
    if (IsSurfaceCreated())
        {
        if(prevCropRegion == aCropRegion)
            {
            if(!iSwitchedToExternalDisplay && aRedrawIndividualWindow)
                {
                User::LeaveIfError(SetBackgroundSurface(iClientWindows[aPos], aCropRegion));
                }
            }
        else
            {
			// We only need to redraw if the area of the video to display (i.e the
			// intersection of client crop region and surface crop rectangle) has changed.
			if (ClientCropRegionChangeRequiresRedraw(prevCropRegion, aCropRegion, iCropRect))
				{
				User::LeaveIfError(RedrawWindows(aCropRegion));
				}
            }
        }
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::UpdateCropRegionL ---");    
    }

void CMediaClientVideoDisplayBody::SetAutoScaleL(const RWindowBase& aWindow, TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos, const TRect& aCropRegion)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetAutoScaleL +++");
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SetAutoScaleL - aWindow WsHandle 0x%X", aWindow.WsHandle());
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SetAutoScaleL - aScaleType %d", aScaleType);
    DEBUG_PRINTF3("CMediaClientVideoDisplayBody::SetAutoScaleL - aHorizPos %d, aVertPos %d", aHorizPos, aVertPos);
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetAutoScaleL - aCropRegion %d,%d - %d,%d", aCropRegion.iTl.iX, aCropRegion.iTl.iY, aCropRegion.iBr.iX, aCropRegion.iBr.iY);

	if (!IsAutoScaleTypeValid(aScaleType))
		{
		User::Leave(KErrArgument);
		}

    TInt pos = iClientWindows.Find(aWindow.WsHandle(), TWindowData::CompareByWsHandle);
	User::LeaveIfError(pos);

	TBool parameterChanged = EFalse;
	if (aScaleType != iClientWindows[pos].iAutoScaleType || aHorizPos != iClientWindows[pos].iHorizPos || aVertPos != iClientWindows[pos].iVertPos)
		{
		iClientWindows[pos].iAutoScaleType = aScaleType;
		iClientWindows[pos].iHorizPos = aHorizPos;
		iClientWindows[pos].iVertPos = aVertPos;
		parameterChanged = ETrue;
		}
	else
		{
		DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetAutoScaleL - Scale parameters have not changed");
		}

	UpdateCropRegionL(aCropRegion, pos, parameterChanged);

	DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetAutoScaleL ---");
	}
	

void CMediaClientVideoDisplayBody::SetRotationL(const RWindowBase& aWindow, TVideoRotation aRotation, const TRect& aCropRegion)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetRotationL +++");
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SetRotationL - aWindow WsHandle 0x%X", aWindow.WsHandle());
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SetRotationL - aRotation %d", aRotation);
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetRotationL - aCropRegion %d,%d - %d,%d", aCropRegion.iTl.iX, aCropRegion.iTl.iY, aCropRegion.iBr.iX, aCropRegion.iBr.iY);

	if (!IsRotationValid(aRotation))
		{
		User::Leave(KErrArgument);
		}

    TInt pos = iClientWindows.Find(aWindow.WsHandle(), TWindowData::CompareByWsHandle);
	User::LeaveIfError(pos);

	TBool parameterChanged = EFalse;
	if (aRotation != iClientWindows[pos].iRotation)
		{
		iClientWindows[pos].iRotation = aRotation;
		parameterChanged = ETrue;
		}
	else
		{
		DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetRotationL - Rotation has not changed");
		}

    UpdateCropRegionL(aCropRegion, pos, parameterChanged);

	DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetRotationL ---");
	}

TVideoRotation CMediaClientVideoDisplayBody::RotationL(const RWindowBase& aWindow)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::RotationL +++");
	TInt pos = iClientWindows.Find(aWindow.WsHandle(), TWindowData::CompareByWsHandle);
	User::LeaveIfError(pos);
	
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::RotationL ---");
	return iClientWindows[pos].iRotation;
	}

void CMediaClientVideoDisplayBody::SetScaleFactorL(const RWindowBase& aWindow, TReal32 aWidthPercentage, TReal32 aHeightPercentage, const TRect& aCropRegion)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetScaleFactorL +++");
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SetScaleFactorL - aWindow WsHandle 0x%X", aWindow.WsHandle());
    DEBUG_PRINTF3("CMediaClientVideoDisplayBody::SetScaleFactorL - aWidthPercentage %f, aHeightPercentage %f", aWidthPercentage, aHeightPercentage);
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetScaleFactorL - aCropRegion %d,%d - %d,%d", aCropRegion.iTl.iX, aCropRegion.iTl.iY, aCropRegion.iBr.iX, aCropRegion.iBr.iY);

    TInt pos = iClientWindows.Find(aWindow.WsHandle(), TWindowData::CompareByWsHandle);
	User::LeaveIfError(pos);
	
	if (aWidthPercentage <= 0.0 || aHeightPercentage <= 0.0)
		{
		User::Leave(KErrArgument);
		}

	TBool parameterChanged = EFalse;
	if (aWidthPercentage != iClientWindows[pos].iScaleWidth ||
	    aHeightPercentage != iClientWindows[pos].iScaleHeight ||
		EAutoScaleNone != iClientWindows[pos].iAutoScaleType)
		{
		iClientWindows[pos].iScaleWidth = aWidthPercentage;
		iClientWindows[pos].iScaleHeight = aHeightPercentage;
		iClientWindows[pos].iAutoScaleType = EAutoScaleNone;
		parameterChanged = ETrue;
		}
	else
		{
		DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetScaleFactorL - Scale parameters have not changed");
		}

    UpdateCropRegionL(aCropRegion, pos, parameterChanged);
	
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetScaleFactorL ---");
	}
	
void CMediaClientVideoDisplayBody::GetScaleFactorL(const RWindowBase& aWindow, TReal32& aWidthPercentage, TReal32& aHeightPercentage)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::GetScaleFactorL +++");
	TInt pos = iClientWindows.Find(aWindow.WsHandle(), TWindowData::CompareByWsHandle);
	User::LeaveIfError(pos);
	
	aWidthPercentage = iClientWindows[pos].iScaleWidth;
	aHeightPercentage = iClientWindows[pos].iScaleHeight;
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::GetScaleFactorL ---");
	}

void CMediaClientVideoDisplayBody::SetAutoScaleL(TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos, const TRect& aCropRegion)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetAutoScaleL +++");
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SetAutoScaleL - aScaleType %d", aScaleType);
    DEBUG_PRINTF3("CMediaClientVideoDisplayBody::SetAutoScaleL - aHorizPos %d, aVertPos %d", aHorizPos, aVertPos);
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetAutoScaleL - aCropRegion %d,%d - %d,%d", aCropRegion.iTl.iX, aCropRegion.iTl.iY, aCropRegion.iBr.iX, aCropRegion.iBr.iY);

	if (!IsAutoScaleTypeValid(aScaleType))
		{
		User::Leave(KErrArgument);
		}

    TRect prevCropRegion(iCropRegion);
	iCropRegion = aCropRegion;
	
	TBool parameterChanged;
	TInt count = iClientWindows.Count();

	for (TInt i = 0; i < count; ++i)
		{
		parameterChanged = EFalse;
		if (aScaleType != iClientWindows[i].iAutoScaleType || aHorizPos != iClientWindows[i].iHorizPos || aVertPos != iClientWindows[i].iVertPos)
			{
			iClientWindows[i].iAutoScaleType = aScaleType;
			iClientWindows[i].iHorizPos = aHorizPos;
			iClientWindows[i].iVertPos = aVertPos;
			parameterChanged = ETrue;
			}
		else
			{
			DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SetAutoScaleL - Scale parameters for window pos %d have not changed", i);
			}

		// We only need to redraw if the scale parameters have changed, or the area of the video
		// to display (i.e the intersection of client crop region and surface crop rectangle) has changed.
		if (IsSurfaceCreated() && !iSwitchedToExternalDisplay && (parameterChanged || ClientCropRegionChangeRequiresRedraw(prevCropRegion, aCropRegion, iCropRect)))
			{
			User::LeaveIfError(SetBackgroundSurface(iClientWindows[i], aCropRegion));
			}
		}

	// We only need to redraw if the area of the video to display (i.e the
	// intersection of client crop region and surface crop rectangle) has changed.
	if (IsSurfaceCreated() && iSwitchedToExternalDisplay && ClientCropRegionChangeRequiresRedraw(prevCropRegion, aCropRegion, iCropRect))
        {
        User::LeaveIfError(RedrawWindows(aCropRegion));
        }
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetAutoScaleL ---");
	}


void CMediaClientVideoDisplayBody::SetRotationL(TVideoRotation aRotation, const TRect& aCropRegion)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetRotationL +++");
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SetRotationL - aRotation %d", aRotation);
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetRotationL - aCropRegion %d,%d - %d,%d", aCropRegion.iTl.iX, aCropRegion.iTl.iY, aCropRegion.iBr.iX, aCropRegion.iBr.iY);

	if (!IsRotationValid(aRotation))
		{
		User::Leave(KErrArgument);
		}

    TRect prevCropRegion(iCropRegion);
    iCropRegion = aCropRegion;

	TBool parameterChanged;
	TInt count = iClientWindows.Count();
	
	for (TInt i = 0; i < count; ++i)
		{
		parameterChanged = EFalse;
		if (aRotation != iClientWindows[i].iRotation)
			{
			iClientWindows[i].iRotation = aRotation;
			parameterChanged = ETrue;
			}
		else
			{
			DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SetRotationL - Rotation for window pos %d has not changed", i);
			}

		// We only need to redraw if the scale parameters have changed, or the area of the video
		// to display (i.e the intersection of client crop region and surface crop rectangle) has changed.
		if (IsSurfaceCreated() && !iSwitchedToExternalDisplay && (parameterChanged || ClientCropRegionChangeRequiresRedraw(prevCropRegion, aCropRegion, iCropRect)))
			{
			User::LeaveIfError(SetBackgroundSurface(iClientWindows[i], aCropRegion));
			}
		}
	
	// We only need to redraw if the area of the video to display (i.e the
	// intersection of client crop region and surface crop rectangle) has changed.
	if (IsSurfaceCreated() && iSwitchedToExternalDisplay && ClientCropRegionChangeRequiresRedraw(prevCropRegion, aCropRegion, iCropRect))
        {
        User::LeaveIfError(RedrawWindows(aCropRegion));
        }
	   
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetRotationL ---");
	}
		
void CMediaClientVideoDisplayBody::SetScaleFactorL(TReal32 aWidthPercentage, TReal32 aHeightPercentage, const TRect& aCropRegion)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetScaleFactorL +++");
    DEBUG_PRINTF3("CMediaClientVideoDisplayBody::SetScaleFactorL - aWidthPercentage %f, aHeightPercentage %f", aWidthPercentage, aHeightPercentage);
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetScaleFactorL - aCropRegion %d,%d - %d,%d", aCropRegion.iTl.iX, aCropRegion.iTl.iY, aCropRegion.iBr.iX, aCropRegion.iBr.iY);

    if (aWidthPercentage <= 0.0 || aHeightPercentage <= 0.0)
		{
		User::Leave(KErrArgument);
		}
	
    TRect prevCropRegion(iCropRegion);
	iCropRegion = aCropRegion;

	TBool parameterChanged;
	TInt count = iClientWindows.Count();
	
	for (TInt i = 0; i < count; ++i)
		{
		parameterChanged = EFalse;
		
		if (aWidthPercentage != iClientWindows[i].iScaleWidth ||
		    aHeightPercentage != iClientWindows[i].iScaleHeight ||
			EAutoScaleNone != iClientWindows[i].iAutoScaleType)
			{
			iClientWindows[i].iScaleWidth = aWidthPercentage;
			iClientWindows[i].iScaleHeight = aHeightPercentage;
			iClientWindows[i].iAutoScaleType = EAutoScaleNone;
			parameterChanged = ETrue;
			}
		else
			{
			DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SetScaleFactorL - Scale parameters for window pos %d have not changed", i);
			}

		// We only need to redraw if the scale parameters have changed, or the area of the video to
		// display (i.e the intersection of client crop region and surface crop rectangle) has changed.
		if (IsSurfaceCreated() && !iSwitchedToExternalDisplay && (parameterChanged || ClientCropRegionChangeRequiresRedraw(prevCropRegion, aCropRegion, iCropRect)))
			{
			User::LeaveIfError(SetBackgroundSurface(iClientWindows[i], aCropRegion));
			}
		}

	// We only need to redraw if the area of the video to display (i.e the
	// intersection of client crop region and surface crop rectangle) has changed.
	if (IsSurfaceCreated() && iSwitchedToExternalDisplay && ClientCropRegionChangeRequiresRedraw(prevCropRegion, aCropRegion, iCropRect))
        {
        User::LeaveIfError(RedrawWindows(aCropRegion));
        }
	   
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetScaleFactorL ---");
	}

void CMediaClientVideoDisplayBody::SetWindowClipRectL(const RWindowBase& aWindow, const TRect& aWindowClipRect, const TRect& aCropRegion)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetWindowClipRectL +++");
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SetWindowClipRectL - aWindow WsHandle 0x%X", aWindow.WsHandle());
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetWindowClipRectL - aWindowClipRect %d,%d - %d,%d", aWindowClipRect.iTl.iX, aWindowClipRect.iTl.iY, aWindowClipRect.iBr.iX, aWindowClipRect.iBr.iY);
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetWindowClipRectL - aCropRegion %d,%d - %d,%d", aCropRegion.iTl.iX, aCropRegion.iTl.iY, aCropRegion.iBr.iX, aCropRegion.iBr.iY);
	
	TInt pos = iClientWindows.Find(aWindow.WsHandle(), TWindowData::CompareByWsHandle);
	User::LeaveIfError(pos);

	TBool parameterChanged = EFalse;
	if (aWindowClipRect != iClientWindows[pos].iClipRect)
		{
		// We only want to redraw if the change in the clipping rectangle would result
		// in a change to the area of the display used for the video.
		// The video is always displayed in the intersection of the clipping rectangle
		// and the video extent, so check if this has changed.
		parameterChanged = IntersectionAreaChanged(iClientWindows[pos].iClipRect, aWindowClipRect, iClientWindows[pos].iVideoExtent);

		iClientWindows[pos].iClipRect = aWindowClipRect;
		}
	else
		{
		DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetWindowClipRectL - Clip rect parameter has not changed");
		}

    UpdateCropRegionL(aCropRegion, pos, parameterChanged);

    DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetWindowClipRectL ---");
	}
	
void CMediaClientVideoDisplayBody::SetVideoExtentL(const RWindowBase& aWindow, const TRect& aVideoExtent, const TRect& aCropRegion)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetVideoExtentL +++");
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SetVideoExtentL - aWindow WsHandle 0x%X", aWindow.WsHandle());
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetVideoExtentL - aVideoExtent %d,%d - %d,%d", aVideoExtent.iTl.iX, aVideoExtent.iTl.iY, aVideoExtent.iBr.iX, aVideoExtent.iBr.iY);
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetVideoExtentL - aCropRegion %d,%d - %d,%d", aCropRegion.iTl.iX, aCropRegion.iTl.iY, aCropRegion.iBr.iX, aCropRegion.iBr.iY);

    TInt pos = iClientWindows.Find(aWindow.WsHandle(), TWindowData::CompareByWsHandle);
	User::LeaveIfError(pos);

	TBool parameterChanged = EFalse;
	if (aVideoExtent != iClientWindows[pos].iVideoExtent)
		{
		iClientWindows[pos].iVideoExtent = aVideoExtent;
		parameterChanged = ETrue;
		}
	else
		{
		DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetVideoExtentL - Video extent parameter has not changed");
		}

    UpdateCropRegionL(aCropRegion, pos, parameterChanged);
    
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetVideoExtentL ---");
	}
	
TBool CMediaClientVideoDisplayBody::HasWindows() const
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::HasWindows +++");
	DEBUG_PRINTF2("CMediaClientVideoDisplayBody::HasWindows --- return %d", (iClientWindows.Count() > 0));
	return (iClientWindows.Count() > 0);
	}
	
TInt CMediaClientVideoDisplayBody::SetBackgroundSurface(TWindowData& aWindowData,  
									const TRect& aCropRegion)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetBackgroundSurface +++");
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SetBackgroundSurface - iWindow WsHandle 0x%X", aWindowData.iWindow->WsHandle());
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetBackgroundSurface - iWindow abs pos %d,%d - width %d, height %d", aWindowData.iWindow->AbsPosition().iX, aWindowData.iWindow->AbsPosition().iY, aWindowData.iWindow->Size().iWidth, aWindowData.iWindow->Size().iHeight);
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetBackgroundSurface - iClipRect %d,%d - %d,%d", aWindowData.iClipRect.iTl.iX, aWindowData.iClipRect.iTl.iY, aWindowData.iClipRect.iBr.iX, aWindowData.iClipRect.iBr.iY);
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetBackgroundSurface - iVideoExtent %d,%d - %d,%d", aWindowData.iVideoExtent.iTl.iX, aWindowData.iVideoExtent.iTl.iY, aWindowData.iVideoExtent.iBr.iX, aWindowData.iVideoExtent.iBr.iY);
    DEBUG_PRINTF3("CMediaClientVideoDisplayBody::SetBackgroundSurface - iScaleWidth %f, iScaleHeight %f", aWindowData.iScaleWidth, aWindowData.iScaleHeight);
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SetBackgroundSurface - iRotation %d", aWindowData.iRotation);
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SetBackgroundSurface - iAutoScaleType %d", aWindowData.iAutoScaleType);
    DEBUG_PRINTF3("CMediaClientVideoDisplayBody::SetBackgroundSurface - iHorizPos %d, iVertPos %d", aWindowData.iHorizPos, aWindowData.iVertPos);
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetBackgroundSurface - aCropRegion %d,%d - %d,%d", aCropRegion.iTl.iX, aCropRegion.iTl.iY, aCropRegion.iBr.iX, aCropRegion.iBr.iY);
	
	// required as this private function is called directly from external friend class
	iCropRegion = aCropRegion;

    // viewport is the viewable area of surface
    TRect viewport(iCropRect);
    if (aCropRegion.Width() > 0 || aCropRegion.Height() > 0)
        {
        viewport.Intersection(aCropRegion);
        }
    
    // Viewport is 0 size, don't show any video
    if (viewport.Width() <= 0 || viewport.Height() <= 0)
        {
        DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SetBackgroundSurface --- Returned with error %d", KErrArgument);
        return KErrArgument;
        }

    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetBackgroundSurface - viewport1 %d,%d - %d,%d", viewport.iTl.iX, viewport.iTl.iY, viewport.iBr.iX, viewport.iBr.iY);

    TRect videoExtent(aWindowData.iVideoExtent);

    TReal32 inputWidth = 0.0f;
    TReal32 inputHeight = 0.0f;
    TReal32 pixelAspectRatio = 0.0f;
    switch (aWindowData.iRotation)
        {
        case EVideoRotationNone:
        case EVideoRotationClockwise180:
            inputWidth = static_cast<TReal32>(viewport.Width());
            inputHeight = static_cast<TReal32>(viewport.Height());
            pixelAspectRatio = static_cast<TReal32>(iAspectRatio.iNumerator) / iAspectRatio.iDenominator;
            break;
        case EVideoRotationClockwise90:
        case EVideoRotationClockwise270:
            inputWidth = static_cast<TReal32>(viewport.Height());
            inputHeight = static_cast<TReal32>(viewport.Width());
            pixelAspectRatio = static_cast<TReal32>(iAspectRatio.iDenominator) / iAspectRatio.iNumerator;
            break;
        default:
            // Should never get to default unless there's been some programming error.
			DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SetBackgroundSurface --- Returned with error %d", KErrArgument);
			return KErrArgument;
        }
    
    TReal32 viewportAspect = pixelAspectRatio * inputWidth / inputHeight;
    TReal32 vidextAspect = static_cast<TReal32>(videoExtent.Width()) / static_cast<TReal32>(videoExtent.Height());
    
    DEBUG_PRINTF4("CMediaClientVideoDisplayBody::SetBackgroundSurface - inputWidth %f, inputHeight %f, PAR %f", inputWidth, inputHeight, pixelAspectRatio);
    DEBUG_PRINTF3("CMediaClientVideoDisplayBody::SetBackgroundSurface - viewportAspect %f, vidextAspect %f", viewportAspect, vidextAspect);

    // Set the extent to the display area in the window.  The final height and with is to 
    // be changed by deltaHeight and deltaWidth respectively.
    TRect extent(videoExtent);

    TInt deltaHeight = 0;
    TInt deltaWidth = 0;
    
    if (aWindowData.iAutoScaleType == EAutoScaleBestFit)
        {
        if (viewportAspect > vidextAspect)
            {
            // Shrink height to get the correct aspect ratio
            deltaHeight = (TInt) (extent.Width() / viewportAspect - extent.Height());
            }
        else
            {
            // Shrink width to get the correct aspect ratio
            deltaWidth = (TInt) (extent.Height() * viewportAspect - extent.Width());
            }
        }
    else if (aWindowData.iAutoScaleType == EAutoScaleClip)
        {
        if (viewportAspect > vidextAspect)
            {
            // Expand width to get the correct aspect ratio
            deltaWidth = (TInt) (extent.Height() * viewportAspect - extent.Width());
            }
        else
            {
            // Expand height to get the correct aspect ratio
            deltaHeight = (TInt) (extent.Width() / viewportAspect - extent.Height());
            }
        }
    else if (aWindowData.iAutoScaleType == EAutoScaleStretch)
        {
        if(iSwitchedToExternalDisplay)
            {
            UpdateDeltaForExtDisplay(viewportAspect, videoExtent, deltaHeight, deltaWidth);
            }
        }
    else if (aWindowData.iAutoScaleType == EAutoScaleNone)
        {
        // for non-square pixels, reduce one dimension
        // XXX other option is to enlarge in the other dimension
        if(pixelAspectRatio > 1)
            {
            inputHeight /= pixelAspectRatio;
            }
        else if(pixelAspectRatio < 1)
            {
            inputWidth *= pixelAspectRatio;
            }
        deltaHeight = (TInt) (inputHeight * aWindowData.iScaleHeight * 0.01 - videoExtent.Height());
        deltaWidth = (TInt) (inputWidth * aWindowData.iScaleWidth * 0.01 - videoExtent.Width());
        }
    
    DEBUG_PRINTF3("CMediaClientVideoDisplayBody::SetBackgroundSurface - deltaWidth %d, deltaHeight %d", deltaWidth, deltaHeight);

    // Change the width of the extent in the proper directions and propertions.
    switch (aWindowData.iHorizPos)
        {
    case EHorizontalAlignCenter:
        extent.iTl.iX -= deltaWidth / 2;
        extent.iBr.iX += deltaWidth / 2;
        break;
    case EHorizontalAlignLeft:
        extent.iBr.iX += deltaWidth;
        break;
    case EHorizontalAlignRight:
        extent.iTl.iX -= deltaWidth;
        break;
    default:
        TInt width = extent.Width() + deltaWidth;
        extent.iTl.iX += aWindowData.iHorizPos;
        extent.iBr.iX = extent.iTl.iX + width;
        break;
        }
    
    // Change the height of the extent in the proper directions and propertions.
    switch (aWindowData.iVertPos)
        {
    case EVerticalAlignCenter:
        extent.iTl.iY -= deltaHeight / 2;
        extent.iBr.iY += deltaHeight / 2;
        break;
    case EVerticalAlignTop:
        extent.iBr.iY += deltaHeight;
        break;
    case EVerticalAlignBottom:
        extent.iTl.iY -= deltaHeight;
        break;
    default:
        TInt height = extent.Height() + deltaHeight;
        extent.iTl.iY += aWindowData.iVertPos;
        extent.iBr.iY = extent.iTl.iY + height;
        break;
        }
        
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetBackgroundSurface - extent1 %d,%d - %d,%d", extent.iTl.iX, extent.iTl.iY, extent.iBr.iX, extent.iBr.iY);

    // The video should not be displayed outside the intended video extent or clipping rectangle.  
    // The extent already has the correct size and position for displaying the entire viewport.  
    // The viewport is clipped such that the video is not moved/distorted when we take the extent 
    // to be the intersection of itself and the intended video extent.
    
    TRect viewableArea(videoExtent);
    viewableArea.Intersection(aWindowData.iClipRect);
    
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetBackgroundSurface - viewableArea %d,%d - %d,%d", viewableArea.iTl.iX, viewableArea.iTl.iY, viewableArea.iBr.iX, viewableArea.iBr.iY);

    // Number of pixels (in window coordinates) to be clipped on the right, bottom, top and left sides of
    // the video.
    TInt dr = Max(0, extent.iBr.iX - viewableArea.iBr.iX);
    TInt db = Max(0, extent.iBr.iY - viewableArea.iBr.iY);
    TInt dt = Max(0, viewableArea.iTl.iY - extent.iTl.iY);
    TInt dl = Max(0, viewableArea.iTl.iX - extent.iTl.iX);
    
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetBackgroundSurface - dr %d, db %d, dt %d, dl %d", dr, db, dt, dl);

    // Calculate the number of pixels in the video per window pixel in both x and y directions.
    TReal32 wRatio = 0.0f; 
    TReal32 hRatio = 0.0f;

    // Make sure we don't divide by 0
    if (extent.Width() != 0)
        {
        wRatio = inputWidth / static_cast<TReal32>(extent.Width());
        }
    
    if (extent.Height() != 0)
        {
        hRatio = inputHeight / static_cast<TReal32>(extent.Height());
        }
    
    DEBUG_PRINTF3("CMediaClientVideoDisplayBody::SetBackgroundSurface - wRatio %f, hRatio %f", wRatio, hRatio);

    // Clip the viewport 
    switch (aWindowData.iRotation)
    {
    case EVideoRotationNone:
        viewport.iBr.iX -= (TInt) (wRatio * static_cast<TReal32>(dr));
        viewport.iBr.iY -= (TInt) (hRatio * static_cast<TReal32>(db));
        viewport.iTl.iX += (TInt) (wRatio * static_cast<TReal32>(dl));
        viewport.iTl.iY += (TInt) (hRatio * static_cast<TReal32>(dt));
        break;
    case EVideoRotationClockwise180:
        viewport.iBr.iX -= (TInt) (wRatio * static_cast<TReal32>(dl));
        viewport.iBr.iY -= (TInt) (hRatio * static_cast<TReal32>(dt));
        viewport.iTl.iX += (TInt) (wRatio * static_cast<TReal32>(dr));
        viewport.iTl.iY += (TInt) (hRatio * static_cast<TReal32>(db));
        break;
    case EVideoRotationClockwise90:
        viewport.iBr.iX -= (TInt) (wRatio * static_cast<TReal32>(db));
        viewport.iBr.iY -= (TInt) (hRatio * static_cast<TReal32>(dl));
        viewport.iTl.iX += (TInt) (wRatio * static_cast<TReal32>(dt));
        viewport.iTl.iY += (TInt) (hRatio * static_cast<TReal32>(dr));
        break;
    case EVideoRotationClockwise270:
        viewport.iBr.iX -= (TInt) (wRatio * static_cast<TReal32>(dt));
        viewport.iBr.iY -= (TInt) (hRatio * static_cast<TReal32>(dr));
        viewport.iTl.iX += (TInt) (wRatio * static_cast<TReal32>(db));
        viewport.iTl.iY += (TInt) (hRatio * static_cast<TReal32>(dl));
        break;
    default:
        // Should never get to default unless there's been some programming error.
		DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SetBackgroundSurface --- Returned with error %d", KErrArgument);
        return KErrArgument;
    }
    
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetBackgroundSurface - viewport2 %d,%d - %d,%d", viewport.iTl.iX, viewport.iTl.iY, viewport.iBr.iX, viewport.iBr.iY);

    // Clip the extent.
    extent.Intersection(viewableArea);
    
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetBackgroundSurface - extent2 %d,%d - %d,%d", extent.iTl.iX, extent.iTl.iY, extent.iBr.iX, extent.iBr.iY);

    aWindowData.iSurfaceConfig.SetViewport(viewport);
    aWindowData.iSurfaceConfig.SetExtent(extent);
    aWindowData.iSurfaceConfig.SetOrientation(ConvertRotation(aWindowData.iRotation));
    
    aWindowData.iSurfaceConfig.SetSurfaceId(iSurfaceId);

    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetBackgroundSurface - iSurfaceId %08x:%08x:%08x:%08x",
            iSurfaceId.iInternal[3], iSurfaceId.iInternal[2], iSurfaceId.iInternal[1], iSurfaceId.iInternal[0]);
    
    // Get the rectangle that bounds the crop rectangle and the viewport.  This should be
    // the same as the crop rectangle as long as the viewport does not go outside this area.
    TRect rect(iCropRect);
    rect.BoundingRect(viewport);
    TInt err = KErrNone;
    
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetBackgroundSurface - rect %d,%d - %d,%d", rect.iTl.iX, rect.iTl.iY, rect.iBr.iX, rect.iBr.iY);

    // Check if the viewport and extent can be displayed as a background surface. The viewport 
    // is valid if it is within the crop rectangle and is not empty.  The extent is valid if 
    // it is not empty. 
    if (rect == iCropRect && !viewport.IsEmpty() && !extent.IsEmpty())
        {
        DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetBackgroundSurface - Calling SetBackgroundSurface");
        err = aWindowData.iWindow->SetBackgroundSurface(aWindowData.iSurfaceConfig, ETrue);
        }

    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SetBackgroundSurface --- err %d", err);
    return err;
    }

TBool CMediaClientVideoDisplayBody::IsUsed() const
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::IsUsed +++");
	DEBUG_PRINTF2("CMediaClientVideoDisplayBody::IsUsed --- return %d", (iEventHandler || iClientWindows.Count() > 0));
	return (iEventHandler || iClientWindows.Count() > 0);
	}
	

TBool CMediaClientVideoDisplayBody::IsSurfaceCreated() const
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::IsSurfaceCreated +++");
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::IsSurfaceCreated --- return %d", !(iSurfaceId.IsNull()));
	return !(iSurfaceId.IsNull());
	}
		
TInt CMediaClientVideoDisplayBody::DisplayId() const
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::DisplayId +++");
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::DisplayId --- return %d", iDisplayId);
	return iDisplayId;
	}
	
TInt CMediaClientVideoDisplayBody::CompareByDisplay(const TInt* aDisplayId, const CMediaClientVideoDisplayBody& aDisplay)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::CompareByDisplay +++");
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::CompareByDisplay --- return %d", (*aDisplayId - aDisplay.DisplayId()));
	return (*aDisplayId - aDisplay.DisplayId());
	}
		
TInt CMediaClientVideoDisplayBody::Compare(const CMediaClientVideoDisplayBody& aLeft, const CMediaClientVideoDisplayBody& aRight)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::Compare +++");
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::Compare --- return %d", (aLeft.DisplayId() - aRight.DisplayId()));
	return (aLeft.DisplayId() - aRight.DisplayId());
	}
	
CFbsBitGc::TGraphicsOrientation CMediaClientVideoDisplayBody::ConvertRotation(TVideoRotation aRotation)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::ConvertRotation +++");
	
	CFbsBitGc::TGraphicsOrientation orientation;
	
	switch(aRotation)
		{
		case EVideoRotationNone:
		    orientation = CFbsBitGc::EGraphicsOrientationNormal;
		    break;
		case EVideoRotationClockwise90:
		    orientation = CFbsBitGc::EGraphicsOrientationRotated270;
		    break;
		case EVideoRotationClockwise180:
		    orientation = CFbsBitGc::EGraphicsOrientationRotated180;
		    break;
		case EVideoRotationClockwise270:
		    orientation = CFbsBitGc::EGraphicsOrientationRotated90;
		    break;
		default:
			// Should never get to default unless there's been some programming error.
			DEBUG_PRINTF2("CMediaClientVideoDisplayBody::ConvertRotation --- Failed due to %d bad aRotation argument", aRotation);
			__ASSERT_DEBUG(FALSE, User::Invariant());
			// Use the normal option otherwise
			orientation = CFbsBitGc::EGraphicsOrientationNormal;
		}
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::ConvertRotation --- return %d", orientation);	
    return orientation;
	}

CMediaClientVideoDisplayBody* CMediaClientVideoDisplayBody::FindDisplayWithWindowL(const RPointerArray<CMediaClientVideoDisplayBody>& aDisplays, const RWindowBase& aWindow)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::FindDisplayWithWindowL +++");
	TInt count = aDisplays.Count();
	
	for (TInt i = 0; i < count; ++i)
		{
		CMediaClientVideoDisplayBody* display = aDisplays[i];
	
		if (display->iClientWindows.Find(aWindow.WsHandle(), TWindowData::CompareByWsHandle) != KErrNotFound)
			{
			DEBUG_PRINTF2("CMediaClientVideoDisplayBody::FindDisplayWithWindowL window found at position ", i);
			DEBUG_PRINTF("CMediaClientVideoDisplayBody::FindDisplayWithWindowL ---");
			return display;	
			}
		}
		
	User::Leave(KErrNotFound);
	return NULL;
	}
	
RArray<CMediaClientVideoDisplayBody::TWindowData>& CMediaClientVideoDisplayBody::Windows()
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::Windows +++");
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::Windows ---");
	return iClientWindows;	
	}

void CMediaClientVideoDisplayBody::SetExternalDisplaySwitchingL(TBool aControl)
    {
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SetExternalDisplaySwitchingL +++ aControl=%d", aControl);

    // not supported
    if(!iExtDisplaySwitchingSupported)
        {
        User::Leave(KErrNotSupported);
        }
    
    // need active scheduler installed
    if(!CActiveScheduler::Current())
        {
        User::Leave(KErrNotReady);
        }

    if(iClientRequestedExtDisplaySwitching != aControl)
        {
        iClientRequestedExtDisplaySwitching = aControl;     
        SwitchSurface();
        }
    
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetExternalDisplaySwitchingL ---");
    }

void CMediaClientVideoDisplayBody::MedcpcExtDisplayNotifyConnected(TExtDisplayConnectionProviderConnType aExtDisplayConnType)
	{
	DEBUG_PRINTF2("CMediaClientVideoDisplayBody::MedcpcExtDisplayNotifyConnected +++ aExtDisplayConnType=%d", aExtDisplayConnType);
	
	if(aExtDisplayConnType != iExtDisplayConnType)
	    {
        TExtDisplayConnectionProviderConnType prevExtDisplayConnType = iExtDisplayConnType;
        iExtDisplayConnType = aExtDisplayConnType;
        
        if(prevExtDisplayConnType == EExtDisplayConnectionProviderConnTypeDisconnected)
            {
            // disconnected -> connected  - don't care which type it is
            DEBUG_PRINTF2("CMediaClientVideoDisplayBody::MedcpcExtDisplayNotifyConnected disconnected -> connected(type %d)", iExtDisplayConnType);
            iExtDisplayConnected = ETrue;
            SwitchSurface();
            }
        else if(iExtDisplayConnType == EExtDisplayConnectionProviderConnTypeDisconnected)
            {
            // connected -> disconnected  - don't care from which type it is
            DEBUG_PRINTF2("CMediaClientVideoDisplayBody::MedcpcExtDisplayNotifyConnected connected(type %d) -> disconnected", prevExtDisplayConnType);
            iExtDisplayConnected = EFalse;
            SwitchSurface();
            }
        else
            {
            // If we get this far then the connection type has changed from "AV Out -> HDMI" or "HDMI -> AV Out"
            // Both are likely. "AV Out -> HDMI" occurs if AV Out cable is connected and HDMI cable is then connected.
            // "HDMI -> AV Out" occurs if both AV Out and HDMI cables are connected and HDMI cable is then disconnected.
            // HDMI is preferred over AV Out.
        
            // update external display window data
            iExtDisplayHandler->UpdateWindow();
            TRect externalDisplayRect(TPoint(0, 0), iExtDisplayHandler->DisplaySizeInPixels());
            (*iWindowsArrayPtr)[0].iClipRect = externalDisplayRect;
            (*iWindowsArrayPtr)[0].iVideoExtent = externalDisplayRect;
            TRAPD(err, (*iWindowsArrayPtr)[0].iAutoScaleType = ExtDisplayAutoScaleTypeL());
            if(err == KErrNone)
                {
                RemoveBackgroundSurface(ETrue);
                RedrawWindows(iCropRegion);
                }
            else
                {
                // Not a lot we can do. Just keep as it is but external display output will be incorrect. 
                DEBUG_PRINTF2("CMediaClientVideoDisplayBody::MedcpcExtDisplayNotifyConnected ExtDisplayAutoScaleTypeL failed %d", err);
                }
            }
	    }
	else
        {
        DEBUG_PRINTF("CMediaClientVideoDisplayBody::MedcpcExtDisplayNotifyConnected No change to connection type");
        }
	
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::MedcpcExtDisplayNotifyConnected ---");
	}

void CMediaClientVideoDisplayBody::SetWindowArrayPtr2Client()
    {
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetWindowArrayPtr2Client +++");

    iWindowsArrayPtr = &iClientWindows;
    iSwitchedToExternalDisplay = EFalse;

    DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetWindowArrayPtr2Client ---");
    }

void CMediaClientVideoDisplayBody::SetWindowArrayPtr2Ext()
    {
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetWindowArrayPtr2Ext +++");

    iWindowsArrayPtr = &iExtDisplayWindows;
    iSwitchedToExternalDisplay = ETrue;

    DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetWindowArrayPtr2Ext ---");
    }

void CMediaClientVideoDisplayBody::CreateExtDisplayHandlerL()
    {
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::CreateExtDisplayHandlerL +++");

    CMediaClientExtDisplayHandler* extDisplayHandler = CMediaClientExtDisplayHandler::NewL(iExtDisplayConnectionProvider->ExtDisplayId());
    CleanupStack::PushL(extDisplayHandler);
    
    TWindowData windowData;
    windowData.iWindow = extDisplayHandler->Window();
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::CreateExtDisplayHandlerL - iWindow WsHandle 0x%X", windowData.iWindow->WsHandle());
    
    TRect externalDisplayRect(TPoint(0, 0), extDisplayHandler->DisplaySizeInPixels());
    windowData.iClipRect = externalDisplayRect;
    windowData.iVideoExtent = externalDisplayRect;
    // windowData.iScaleWidth only required for EAutoScaleNone
    // windowData.iScaleWidth only required for EAutoScaleNone
    windowData.iRotation = EVideoRotationNone;
    windowData.iAutoScaleType = ExtDisplayAutoScaleTypeL();
    windowData.iHorizPos = EHorizontalAlignCenter;
    windowData.iVertPos = EVerticalAlignCenter;
    // windowData.iWindow2 not used        
    
    iExtDisplayWindows.AppendL(windowData);
    CleanupStack::Pop(extDisplayHandler);
    iExtDisplayHandler = extDisplayHandler;
    
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::CreateExtDisplayHandlerL ---");
    }

void CMediaClientVideoDisplayBody::RemoveExtDisplayHandler()
    {
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::RemoveExtDisplayHandler +++");

    delete iExtDisplayHandler;
    iExtDisplayHandler = NULL;
    iExtDisplayWindows.Reset();

    DEBUG_PRINTF("CMediaClientVideoDisplayBody::RemoveExtDisplayHandler ---");
    }

void CMediaClientVideoDisplayBody::CreateExtDisplayPluginL()
    {
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::CreateExtDisplayPluginL +++");
    
    iExtDisplayConnectionProvider = CExtDisplayConnectionProviderInterface::NewL();
    
    if(iExtDisplayConnectionProvider)
        {
        iExtDisplaySwitchingSupported = ETrue;
        iExtDisplayConnectionProvider->SetExtDisplayConnectionProviderCallback(*this);
        iExtDisplayConnType = iExtDisplayConnectionProvider->ExtDisplayConnType();
        iExtDisplayConnected = (iExtDisplayConnType != EExtDisplayConnectionProviderConnTypeDisconnected);
        }

    DEBUG_PRINTF("CMediaClientVideoDisplayBody::CreateExtDisplayPluginL ---");
    }

void CMediaClientVideoDisplayBody::RemoveExtDisplayPlugin()
    {
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::RemoveExtDisplayPlugin +++");
    
    if(iExtDisplaySwitchingSupported)
        {
        delete iExtDisplayConnectionProvider;
        iExtDisplayConnectionProvider = NULL;
        iExtDisplaySwitchingSupported = EFalse;
        iExtDisplayConnected = EFalse;
        }    
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::RemoveExtDisplayPlugin ---");
    }

void CMediaClientVideoDisplayBody::MmcweoFocusWindowGroupChanged()
    {
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::MweocFocusWindowGroupChanged +++");
    
    TBool prevClientWindowIsInFocus = iClientWindowIsInFocus;
    UpdateFocus();
    
    if(prevClientWindowIsInFocus != iClientWindowIsInFocus)
        {
        SwitchSurface();
        }
    
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::MweocFocusWindowGroupChanged ---");
    }

TBool CMediaClientVideoDisplayBody::MmcweoIgnoreProcess(TSecureId aId)
    {
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::MmcweoIgnoreProcess +++");
    
    TBool ignore = ETrue;
    if (iServerClient)
        {
        ignore = iServerClient->IgnoreProcess(aId);
        }
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::MmcweoIgnoreProcess --- return %d", ignore);
    return ignore;
    }

void CMediaClientVideoDisplayBody::UpdateFocus()
    {
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::UpdateFocus +++");
    
    if(!iWsEventObserver)
        {
        iClientWindowIsInFocus = ETrue;
        DEBUG_PRINTF("CMediaClientVideoDisplayBody::UpdateFocus Event Observer is NULL");
        DEBUG_PRINTF("CMediaClientVideoDisplayBody::UpdateFocus ---");
        return;
        }
    
    TBool prevClientWindowIsInFocus = iClientWindowIsInFocus;
    
    TInt focusGroupId;
    if(iWsEventObserver->FocusWindowGroupId(focusGroupId) == KErrNone)
        {
        iClientWindowIsInFocus = EFalse;
        TInt count = iClientWindows.Count();
        for(TInt i = 0; i < count; i++)
            {
            if(iClientWindows[i].iWindow->WindowGroupId() == focusGroupId)
                {
                iClientWindowIsInFocus = ETrue;
                break;
                }
            }
        }
    else
        {
        DEBUG_PRINTF("CMediaClientVideoDisplayBody::UpdateFocus Error retrieving focus WgId from observer");
        iClientWindowIsInFocus = ETrue;
        }

    if(iServerClient && (prevClientWindowIsInFocus != iClientWindowIsInFocus))
        {
        DEBUG_PRINTF2("CMediaClientVideoDisplayBody::MweocFocusWindowGroupChanged calling server, focus %d", iClientWindowIsInFocus);
        iServerClient->FocusChanged(iClientWindowIsInFocus);
        }
    
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::UpdateFocus Client window in focus %d", iClientWindowIsInFocus);
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::UpdateFocus ---");
    }

void CMediaClientVideoDisplayBody::SwitchSurface()
    {
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::SwitchSurface +++");

    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SwitchSurface SurfaceCreated %d", IsSurfaceCreated());
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SwitchSurface Client window count %d", iClientWindows.Count());
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SwitchSurface Client Requested Ext Display Switching %d", iClientRequestedExtDisplaySwitching);
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SwitchSurface Client Window in Focus %d", iClientWindowIsInFocus);
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SwitchSurface External Display Connected %d", iExtDisplayConnected);

    if(IsSurfaceCreated() && (iClientWindows.Count() > 0) && iClientRequestedExtDisplaySwitching &&
            iClientWindowIsInFocus && iExtDisplayConnected)
        {
        TRAPD(err, CreateExtDisplayHandlerL());
        DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SwitchSurface CreateExtDisplayHandlerL error %d", err);
        if(err == KErrNone)
            {
            SetWindowArrayPtr2Ext();
            RedrawWindows(iCropRegion); 
            
#ifdef MEDIA_CLIENT_SURFACE_NOT_REMOVED_FROM_CLIENT_WINDOW
            // Hide surface so that video is not seen on client window
            HideAllClientWindows();
#else
            // Surface removed from window
            SetWindowArrayPtr2Client();
            RemoveBackgroundSurface(ETrue);
            SetWindowArrayPtr2Ext();
#endif
            }
        }
    else if(iSwitchedToExternalDisplay)
        {
        // Set background surface for clientwindows before removing from external display window.
        SetWindowArrayPtr2Client();
        // RedrawWindows handles both the case where the surface was removed from client window and 
        // also the case where the surface was hidden from client window
        RedrawWindows(iCropRegion);
        SetWindowArrayPtr2Ext();
        RemoveBackgroundSurface(ETrue);
        RemoveExtDisplayHandler();
        SetWindowArrayPtr2Client();
        }

    DEBUG_PRINTF("CMediaClientVideoDisplayBody::SwitchSurface ---");
    }

TBool CMediaClientVideoDisplayBody::IsRotationValid(TVideoRotation aVideoRotation)
	{
	if (aVideoRotation == EVideoRotationNone ||
	    aVideoRotation == EVideoRotationClockwise90 ||
		aVideoRotation == EVideoRotationClockwise180 ||
		aVideoRotation == EVideoRotationClockwise270)
		{
		return ETrue;
		}

	DEBUG_PRINTF2("CMediaClientVideoDisplayBody::IsRotationValid - Rotation %d not valid", aVideoRotation);
	return EFalse;
	}

TBool CMediaClientVideoDisplayBody::IsAutoScaleTypeValid(TAutoScaleType aAutoScaleType)
	{
	if (aAutoScaleType == EAutoScaleNone ||
	    aAutoScaleType == EAutoScaleBestFit ||
	    aAutoScaleType == EAutoScaleClip ||
	    aAutoScaleType == EAutoScaleStretch)
		{
		return ETrue;
		}

	DEBUG_PRINTF2("CMediaClientVideoDisplayBody::IsAutoScaleTypeValid - Auto scale %d not valid", aAutoScaleType);
	return EFalse;
	}

/**
Check whether a change in the surface crop rectangle would mean that the surface viewport calculated in SetBackgroundSurface would change.
The surface viewport is the intersection of the surface crop rectangle and the client crop region
*/
TBool CMediaClientVideoDisplayBody::SurfaceCropRectChangeRequiresRedraw(TRect aOldSurfaceCropRect, TRect aNewSurfaceCropRect, TRect aClientCropRegion)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::SurfaceCropRectChangeRequiresRedraw +++");
	
	// If aClientCropRegion is empty then it is not currently being used in the SetBackgroundSurface calculations.
	// This means that only aOldSurfaceCropRect is being used to decide which part of the video is displayed.
	// By setting aClientCropRect to the same as aOldSurfaceCropRect we ensure that only aOldSurfaceCropRect is
	// used in the subsequent intersection area checks.
	if (aClientCropRegion.IsEmpty())
		{
		DEBUG_PRINTF("CMediaClientVideoDisplayBody::SurfaceCropRectChangeRequiresRedraw - Client crop region is empty");		
		aClientCropRegion = aOldSurfaceCropRect;
		}

	TBool ret = IntersectionAreaChanged(aOldSurfaceCropRect, aNewSurfaceCropRect, aClientCropRegion);

	DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SurfaceCropRectChangeRequiresRedraw --- ret = %d", ret);

	return ret;
	}

/**
Check whether a change in the client crop region would mean that the surface viewport calculated in SetBackgroundSurface would change.
The surface viewport is the intersection of the surface crop rectangle and the client crop region
*/
TBool CMediaClientVideoDisplayBody::ClientCropRegionChangeRequiresRedraw(TRect aOldClientCropRegion, TRect aNewClientCropRegion, TRect aSurfaceCropRect)
	{
	DEBUG_PRINTF("CMediaClientVideoDisplayBody::ClientCropRegionChangeRequiresRedraw +++");

	// If aOldClientCropRegion is empty then it is not currently being used in the SetBackgroundSurface calculations.
	// This means that only aSurfaceCropRect is being used to decide which part of the video is displayed. By
	// setting aOldClientCropRegion to the same as aSurfaceCropRect we ensure that only aSurfaceCropRect is
	// used in the subsequent intersection area checks.
	if (aOldClientCropRegion.IsEmpty())
		{
		DEBUG_PRINTF("CMediaClientVideoDisplayBody::ClientCropRegionChangeRequiresRedraw - Old client crop region is empty");
		aOldClientCropRegion = aSurfaceCropRect;
		}

	// If aNewClientCropRegion is empty then it will not be used in the SetBackgroundSurface calculations.
	// This means that only aSurfaceCropRect will impact which part of the video is displayed. By
	// setting aNewClientCropRegion to the same as aSurfaceCropRect we ensure that only aSurfaceCropRect is
	// used in the subsequent intersection area checks.
	if (aNewClientCropRegion.IsEmpty())
		{
		DEBUG_PRINTF("CMediaClientVideoDisplayBody::ClientCropRegionChangeRequiresRedraw - New client crop region is empty");
		aNewClientCropRegion = aSurfaceCropRect;
		}

	TBool ret = IntersectionAreaChanged(aOldClientCropRegion, aNewClientCropRegion, aSurfaceCropRect);

	DEBUG_PRINTF2("CMediaClientVideoDisplayBody::ClientCropRegionChangeRequiresRedraw --- ret = %d", ret);

	return ret;
	}

/**
Check whether the change in size of a rectangle means its intersection with another rectangle has changed.
This is used to check whether changes in things like the surface crop rectangle, client crop region, and window clip rectangle, would mean
the area of video displayed would change.
*/
TBool CMediaClientVideoDisplayBody::IntersectionAreaChanged(TRect aOldRect, TRect aNewRect, TRect aOtherRect)
	{
	DEBUG_PRINTF5("CMediaClientVideoDisplayBody::IntersectionAreaChanged - aOldRect %d,%d - %d,%d", aOldRect.iTl.iX, aOldRect.iTl.iY, aOldRect.iBr.iX, aOldRect.iBr.iY);
	DEBUG_PRINTF5("CMediaClientVideoDisplayBody::IntersectionAreaChanged - aNewRect %d,%d - %d,%d", aNewRect.iTl.iX, aNewRect.iTl.iY, aNewRect.iBr.iX, aNewRect.iBr.iY);
	DEBUG_PRINTF5("CMediaClientVideoDisplayBody::IntersectionAreaChanged - aOtherRect %d,%d - %d,%d", aOtherRect.iTl.iX, aOtherRect.iTl.iY, aOtherRect.iBr.iX, aOtherRect.iBr.iY);

	aOldRect.Intersection(aOtherRect);
	aNewRect.Intersection(aOtherRect);

	if (aOldRect != aNewRect)
		{
		DEBUG_PRINTF("CMediaClientVideoDisplayBody::IntersectionAreaChanged --- Intersection area has changed");
		return ETrue;
		}

	DEBUG_PRINTF("CMediaClientVideoDisplayBody::IntersectionAreaChanged --- Intersection area has not changed");
	return EFalse;
	}

/**
* This function calculates the delta width and delta height for AV out when the TV-Out setting is set to "widescreen".
*
* AV out has fixed resolution whether TV-Out is set to "normal" or "widescreen". The TV-Out setting indicates
* that the video should be scaled so that when displayed on a corresponding TV the aspect looks correct.
* 
* When displaying video on a widescreen TV through AV out, because the resolution is the same the TV stretches
* the video horizontally. When displaying on a normal TV no stretching takes place.
* 
* For "normal" TAutoScaleType::EAutoScaleClip is used.
* 
* For "widescreen" this function calculates the width delta and height delta required so that when the video is stretched
* the aspect looks correct on a widescreen TV.
* 
* This function must only be called when autoscale is set to TAutoScaleType::EAutoScaleStretch and an external display is
* connected.
**/
void CMediaClientVideoDisplayBody::UpdateDeltaForExtDisplay(TReal32 aViewportAspect, const TRect& aVideoExtent, TInt& aDeltaHeight, TInt& aDeltaWidth)
    {
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::UpdateDeltaForExtDisplay +++");
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::UpdateDeltaForExtDisplay aViewportAspect %f", aViewportAspect);
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::UpdateDeltaForExtDisplay aVideoExtent %d,%d - %d,%d", aVideoExtent.iTl.iX, aVideoExtent.iTl.iY, aVideoExtent.iBr.iX, aVideoExtent.iBr.iY);
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::UpdateDeltaForExtDisplay aDeltaHeight %d", aDeltaHeight);
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::UpdateDeltaForExtDisplay aDeltaWidth %d", aDeltaWidth);

    aDeltaWidth = 0;
    aDeltaHeight = 0;
    
    TReal32 wideScreenAspect = (TReal32)16 / (TReal32)9;
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::UpdateDeltaForExtDisplay wideScreenAspect %f", wideScreenAspect);

    if(aViewportAspect == wideScreenAspect)
        {
        // no need to calculate
        DEBUG_PRINTF("CMediaClientVideoDisplayBody::UpdateDeltaForExtDisplay - Viewport Aspect equals wideScreenAspect");
        DEBUG_PRINTF("CMediaClientVideoDisplayBody::UpdateDeltaForExtDisplay - width delta and height delta not changed");
        }
    else if(aViewportAspect < wideScreenAspect)
        {
        DEBUG_PRINTF("CMediaClientVideoDisplayBody::UpdateDeltaForExtDisplay - Viewport Aspect is less than wideScreenAspect");
        
        // calculate video width for viewport that when stretched looks ok on widescreen
        TReal32 correctedWidth = (TReal32)aVideoExtent.Width() * aViewportAspect / wideScreenAspect;
        DEBUG_PRINTF2("CMediaClientVideoDisplayBody::UpdateDeltaForExtDisplay corrected viewport width %f", correctedWidth);
        
        aDeltaWidth = correctedWidth - aVideoExtent.Width();
        }
    else // aViewportAspect > wideScreenAspect
        {
        DEBUG_PRINTF("CMediaClientVideoDisplayBody::UpdateDeltaForExtDisplay - Viewport Aspect is greater than wideScreenAspect");

        // calculate video height for viewport that when stretched looks ok on widescreen
        TReal32 correctedHeight = (TReal32)aVideoExtent.Height() * wideScreenAspect / aViewportAspect;
        DEBUG_PRINTF2("CMediaClientVideoDisplayBody::UpdateDeltaForExtDisplay corrected viewport height %f", correctedHeight);
        
        aDeltaHeight = aVideoExtent.Height() - correctedHeight;
        }        
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::UpdateDeltaForExtDisplay --- aDeltaHeight %d", aDeltaHeight);
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::UpdateDeltaForExtDisplay --- aDeltaWidth %d", aDeltaWidth);
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::UpdateDeltaForExtDisplay ---");
    }

TBool CMediaClientVideoDisplayBody::IsWideScreenL()
    {
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::IsWideScreenL +++");
    
    CRepository* repo = CRepository::NewLC(KCRUidTvoutSettings);
    TInt value;
    User::LeaveIfError(repo->Get(KSettingsTvAspectRatio, value));

    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::IsWideScreenL Tv Apect Ratio set to %d, 0=4x3 1=16x9", value);

    CleanupStack::PopAndDestroy(repo);
    
    TBool ret = value > 0;
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::IsWideScreenL --- return %d", ret);
    return ret;
    }

TAutoScaleType CMediaClientVideoDisplayBody::ExtDisplayAutoScaleTypeL()
    {
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::ExtDisplayAutoScaleTypeL +++");
    
    // EExtDisplayConnectionProviderConnTypeHdmi - EAutoScaleBestFit
    // EExtDisplayConnectionProviderConnTypeAnalog / normal - EAutoScaleBestFit
    // EExtDisplayConnectionProviderConnTypeAnalog / widescreen - EAutoScaleStretch
    
    TAutoScaleType autoScaleType;
    if((iExtDisplayConnType == EExtDisplayConnectionProviderConnTypeAnalog) && IsWideScreenL())
        {
        DEBUG_PRINTF("CMediaClientVideoDisplayBody::ExtDisplayAutoScaleTypeL External display scale type EAutoScaleStretch");
        autoScaleType = EAutoScaleStretch;
        }
    else
        {
        DEBUG_PRINTF("CMediaClientVideoDisplayBody::ExtDisplayAutoScaleTypeL External display scale type EAutoScaleBestFit");
        autoScaleType = EAutoScaleBestFit;
        }
    
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::ExtDisplayAutoScaleTypeL --- return %d", autoScaleType);
    return autoScaleType;
    }

#ifdef MEDIA_CLIENT_SURFACE_NOT_REMOVED_FROM_CLIENT_WINDOW

void CMediaClientVideoDisplayBody::HideAllClientWindows()
    {
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::HideAllClientWindows +++");

    TInt count = iClientWindows.Count();
    for (TInt i = 0; i < count; ++i)
        {
        // ignore error - cannot be handled
        HideWindow(iClientWindows[i].iWindow);
        }
    
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::HideAllClientWindows ---");
    }

TInt CMediaClientVideoDisplayBody::HideWindow(RWindowBase* aWindow)
    {
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::HideWindow +++");

    TSurfaceConfiguration config;
    TInt err = aWindow->GetBackgroundSurface(config);
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::HideWindow GetBackgroundSurface error %d", err);
    
#ifdef _DEBUG
    TSurfaceId surface;
    config.GetSurfaceId(surface);
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::HideWindow - Retrieved Surface %08x:%08x:%08x:%08x", 
            surface.iInternal[3], surface.iInternal[2], surface.iInternal[1], surface.iInternal[0]);
#endif
    
    if (err == KErrNone)
        {
        config.SetExtent(TRect(KHiddenExtentA, KHiddenExtentA, KHiddenExtentB, KHiddenExtentB));
        err = aWindow->SetBackgroundSurface(config, ETrue);
        // Make sure all window rendering has completed before proceeding
        RWsSession* ws = aWindow->Session();
        if (ws)
           {
           ws->Finish();
           }
        DEBUG_PRINTF2("CMediaClientVideoDisplayBody::HideWindow SetBackgroundSurface error %d", err);
        }
    
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::HideWindow ---");
    return err;
    }

void CMediaClientVideoDisplayBody::SetHiddenBackgroundSurfaceOnAllClientWindows()
    {
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetHiddenBackgroundSurfaceOnAllClientWindows +++");

    TInt count = iClientWindows.Count();
    for (TInt i = 0; i < count; ++i)
        {
        // ignore error - cannot be handled
        SetHiddenBackgroundSurfaceOnClientWindow(iClientWindows[i]);
        }
    
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetHiddenBackgroundSurfaceOnAllClientWindows ---");
    }


TInt CMediaClientVideoDisplayBody::SetHiddenBackgroundSurfaceOnClientWindow(TWindowData& aWindowData)
    {
    DEBUG_PRINTF("CMediaClientVideoDisplayBody::SetHiddenBackgroundSurfaceOnClientWindow +++");    
    DEBUG_PRINTF5("CMediaClientVideoDisplayBody::SetHiddenBackgroundSurfaceOnClientWindow - iSurfaceId %08x:%08x:%08x:%08x",
                iSurfaceId.iInternal[3], iSurfaceId.iInternal[2], iSurfaceId.iInternal[1], iSurfaceId.iInternal[0]);
    
    TSurfaceConfiguration config;
    config.SetExtent(TRect(KHiddenExtentA, KHiddenExtentA, KHiddenExtentB, KHiddenExtentB));   
    config.SetSurfaceId(iSurfaceId);
    TInt err = aWindowData.iWindow->SetBackgroundSurface(config, ETrue);
    
    DEBUG_PRINTF2("CMediaClientVideoDisplayBody::SetHiddenBackgroundSurfaceOnClientWindow ---, return %d", err);
    return err;
    }
#endif
