// Copyright (c) 1995-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Window redraw code, three sorts of redrawing are supported
// Sending a redraw message to the client (see redrawmsgwindow.cpp)
// Drawing from backup bitmap
// Simply clearing the window
// 
//

#include "wnredraw.h"
#include "server.h"
#include "playbackgc.h"
#include "wstop.h"
#include "ANIM.H"
#include "EVQUEUE.H"
#include <s32mem.h>
#include <gdi.h>
#include "panics.h"
#include "inifile.h"
#include "rootwin.h"
#include "EVENT.H"
#include "wstypes.h"
#include <graphics/surface.h>
#include <graphics/wselement.h>
#include <graphics/wsscreendevice.h>
#include "windowelementset.h"

struct TFadingParams
	{
	TUint8 blackMap;
	TUint8 whiteMap;
	};

CWsWindowRedraw::CWsWindowRedraw(CWsWindow *aWin) : iWsWin(aWin)
	{
	}

CWsWindowRedraw::~CWsWindowRedraw()
	{
	if (iWsWin->WsOwner())
		{
		iWsWin->WsOwner()->RedrawQueue()->RemoveInvalid(this);
		}
	if (HasElement())
		{
		iWsWin->Screen()->WindowElements().ReleaseAllElements(*CliWin());
		}
	}

void CWsWindowRedraw::ConstructL()
	{
	}

const TRegion& CWsWindowRedraw::InvalidArea() const
	{
	return(nullRegion);
	}

const TRegion &CWsWindowRedraw::BaseDrawRegion() const
	{
	return(iWsWin->VisibleRegion());
	}

void CWsWindowRedraw::ClipInvalidRegion(const TRect &)
	{
	}

void CWsWindowRedraw::Resize(const TSize &, const TSize &)
	{
	}

void CWsWindowRedraw::SetReply(TInt aReply)
	{
	iWsWin->WsOwner()->SetReply(aReply);
	}

void CWsWindowRedraw::OwnerPanic(TClientPanic aPanic)
	{
	iWsWin->OwnerPanic(aPanic);
	}

CWsBackedUpWindow *CWsWindowRedraw::Backup() const
	{
	return(NULL);
	}

void CWsWindowRedraw::Scroll(const TRect &, const TPoint &,const TRect &)
	{
	}

void CWsWindowRedraw::UpdateAnimArea()
	{
	}

void CWsWindowRedraw::PrepareForResizeL(const TSize& /* aNewSize */, TSize& /* aOldSize */)
	{
	}

TBool CWsWindowRedraw::DrawCommand(CWsGc*,const TAny*)
	{
	return ETrue;
	}

void CWsWindowRedraw::GcAttributeChange(CWsGc*,const TAny*)
	{
	}

void CWsWindowRedraw::GcDeactivate(CWsGc*)
	{
	}

CFbsDevice* CWsWindowRedraw::OutputDevice() const
	{
	return NULL;
	}

void CWsWindowRedraw::ClientExposing()
	{
	}

void CWsWindowRedraw::ClearRedrawStore(TBool)
	{}

void CWsWindowRedraw::PreDrawWindow(MWsGraphicsContext* aGc, const TRegion& aWindowRegion)
	{
	WS_ASSERT_DEBUG(iRedrawRegion == NULL, EWsPanicScheduledRedraw);
	iRedrawRegion = &aWindowRegion;
	CPlaybackGc::Instance()->SetTargetRegion(iRedrawRegion);
	CWsClient::iCurrentCommand.iOpcode=0;
	CPlaybackGc::Instance()->Activate(CliWin(), aGc, iRedrawRegion);
	}

void CWsWindowRedraw::PostDrawWindow(MWsGraphicsContext* aGc, const TRegion& aWindowChildNodeRegion)
	{
	WS_ASSERT_DEBUG(iRedrawRegion, EWsPanicScheduledRedraw);
	CPlaybackGc::Instance()->Deactivate();
	CPlaybackGc::Instance()->SetTargetRegion(NULL);
	
	if(!Screen()->ChangeTracking())
		{
		DoFade(*iRedrawRegion);
		}

	AnnotateWindowRedrawEnd(*iWsWin);	
	
	DrawWindowAnims(aGc, aWindowChildNodeRegion);
	DrawCursorAndSprites(aGc, aWindowChildNodeRegion);
	iRedrawRegion = 0;
	}

void CWsWindowRedraw::Fade(MWsGraphicsContext * aGc, const TRegion& aRegion)
	{
	LOG_WINDOW_FADE_START(WsWin());	
	AnnotateWindowRedrawStart(*iWsWin, aRegion);
	
	aGc->Reset();
	DoFade(aRegion);
	
	AnnotateWindowRedrawEnd(*iWsWin);		
	LOG_WINDOW_FADE_END(WsWin());	
	}

void CWsWindowRedraw::DoFade(const TRegion& aRegion)
	{
	if( CWsTop::IsFadeEnabled() && iWsWin && iWsWin->FadeCount()>0 && !(iWsWin->IsNonFading()) && !(iWsWin->FadableRegion().IsEmpty()) && !(iWsWin->IsDSAHost()) )
		{
		MWsFader* fader = static_cast<MWsFader*>(iWsWin->Screen()->ResolveObjectInterface(KMWsFader));
		if(fader)
			{
		  	TFadingParams parameters;
	  		iWsWin->GetFadingParams(parameters.blackMap,parameters.whiteMap);
	      	TPckgBuf<TFadingParams> buf(parameters);
	      	fader->SetFadingParameters(buf);
		  	// Only fade the region that hasn't been faded before
	  		STACK_REGION fdRgn;
	  		fdRgn.Copy( aRegion );
			fdRgn.Intersect( iWsWin->FadableRegion() );
			if(!fdRgn.CheckError())
				{
				fader->FadeArea( fdRgn );
				LOG_WINDOW_FADE_REGION(&fdRgn);
				}
	  		fdRgn.Close();
	      	}
		}
	}

void CWsWindowRedraw::DrawWindowAnims(MWsGraphicsContext * aGc, const TRegion& aRegion)
	{
	if (iWsWin->iAnimList)
		{
		// If an anim panics, it will leave and set the panic flag on the client
		// The client itself won't actually panic yet, and we don't want to leave from here.
		TRAP_IGNORE(DrawWindowAnimsL(aGc, aRegion));
		}
	}

void CWsWindowRedraw::DrawWindowAnimsL(MWsGraphicsContext * aGc, const TRegion& aRegion)
	{
	for (CWsAnim * anim = iWsWin->iAnimList; anim; anim = anim->Next())
		{
		AnnotateWindowAnimRedrawStart(*iWsWin, *anim, aRegion);
		
		//Animate and redraw
		TRAPD(err,anim->RedrawWindowAnimL(Screen()->Now(), aGc, &aRegion));
		if(err!=KErrNone)
			{
			AnnotateWindowAnimRedrawEnd(*iWsWin, *anim);
			anim->Panic(EWservPanicAnimLeave);
			return;
			}
		
		AnnotateWindowAnimRedrawEnd(*iWsWin, *anim);
		}
	}

void CWsWindowRedraw::DrawCursorAndSprites(MWsGraphicsContext * aGc, const TRegion& aRegion)
	{	
	// Draw standard text cursor if required
	RWsTextCursor* const cursor = CWsTop::CurrentTextCursor();
	if (!iWsWin->Screen()->ChangeTracking() && cursor && cursor->Win() == iWsWin && cursor->IsStandardCursorActive())
		{
		// Standard text cursor is active on this window
		const TBool flashing = cursor->IsFlashing();
		TFlashState flashState = EFlashOn;
		if (flashing)
			{
			flashState = cursor->CurrentCursorFlashState();
			}
		if (flashState == EFlashOn)
			{
			// Cursor should be visible, so draw it
			cursor->Draw(aRegion);
			}
		if (flashing)
			{
			// Reschedule to flash the standard cursor on or off
			Screen()->ScheduleAnimation(ETextCursor, cursor->RectRelativeToScreen(), Screen()->SpriteManager()->NextCursorFlashStateChange(), 0, 0, iWsWin);
			}
		}

	for (CWsSpriteBase * sprite = iWsWin->iSpriteList; sprite; sprite = sprite->Next())
		{
        TBool hasRedrawBegun = EFalse;
		STACK_REGION redrawRegion;
		sprite->CalcRedrawRegion(aRegion, redrawRegion);
		if(redrawRegion.CheckError() || !redrawRegion.IsEmpty())
			{
			if (sprite->IsFlashingEnabled() || sprite->IsDirty() || sprite->HasAnimation())
				{
                if (sprite->IsDirty() || sprite->HasAnimation())
                    {
                    AnnotateSpriteRedrawStart(*iWsWin, *sprite, redrawRegion);
                    hasRedrawBegun = ETrue;
                    }
				
				if(sprite->HasAnimation())
					{
					CWsAnim* anim = static_cast<CWsSprite*>(sprite)->iAnim;
					WS_ASSERT_DEBUG(anim,EWsPanicAnim);
					
					//Animate and...
					TRAPD(err, anim->AnimateSpriteAnimL(Screen()->Now()));
					if(err!=KErrNone)
						{
						AnnotateSpriteRedrawEnd(*iWsWin, *sprite);
						anim->Panic(EWservPanicAnimLeave);
						return;
						}
					}
				
				//...call Redraw on the sprite
				if (hasRedrawBegun)
				    {
                    aGc->Reset();
				    }
				sprite->Redraw(aGc, redrawRegion);
				
				if (hasRedrawBegun)
				    {
                    AnnotateSpriteRedrawEnd(*iWsWin, *sprite);
				    }
				}
			}
		redrawRegion.Close();
		}
	}

TBool CWsWindowRedraw::Contains(const TArray<TGraphicDrawerId>& /*aDrawers*/,const TRegion& aRegion) const
	{
	// if in doubt, assume we do
	return !aRegion.IsEmpty();
	}

TInt CWsWindowRedraw::DrawBackgroundColor(const TRegion& aRegion, TBool	aDoFillColor)
	{
	if (BackColor().Alpha() == 0 && !HasElement())
		return KErrNone;
	
	if(aRegion.IsEmpty())
		return KErrNone;
	
	TRect winAbs(CliWin()->AbsRect());	//fill size for background color fill
	TRect surfaceAbs(0,0,0,0);			//fill size for background surface fill - initially disabled
	
	if (HasElement())
		{
		TBackgroundAttributes* backgroundAttributes = CliWin()->Screen()->WindowElements().FindBackgroundElement(*CliWin());
		WS_ASSERT_DEBUG(backgroundAttributes,EWsPanicNoWindowElement);
		
		if (backgroundAttributes->iElement)
			{
			if (backgroundAttributes->ExplicitExtent())
				{
				backgroundAttributes->iElement->GetDestinationRectangle(surfaceAbs);
				surfaceAbs.Intersection(winAbs);
				if (surfaceAbs==winAbs)
					{
					winAbs.iBr.iX=winAbs.iTl.iX;	//disable background color fill
					}
				}
			else
				{
				surfaceAbs=winAbs;
				winAbs.iBr.iX=winAbs.iTl.iX;	//disable background color fill
				}
			}
		if (!aDoFillColor)
			{
			winAbs.iBr.iX=winAbs.iTl.iX;	//disable background color fill
			}
		}
	
	CPlaybackGc* playback = CPlaybackGc::Instance();
	MWsGraphicsContext* gc = static_cast<MWsGraphicsContext*>(playback->ResolveObjectInterface(KMWsGraphicsContext));
	gc->SetClippingRegion(aRegion);
	gc->SetBrushStyle(MWsGraphicsContext::ESolidBrush);
	gc->SetPenStyle(MWsGraphicsContext::ENullPen);
	TInt err = KErrNone;
	if (!winAbs.IsEmpty())
		{
		gc->SetBrushColor(BackColor());
		gc->DrawRect(winAbs);
		}
	if (!surfaceAbs.IsEmpty())
		{
		gc->SetDrawMode(MWsGraphicsContext::EDrawModeWriteAlpha);
		gc->SetBrushColor(TRgb(0,0,0,0));
		gc->DrawRect(surfaceAbs);
		gc->SetBrushColor(BackColor());		//leave in a sensible state
		gc->SetDrawMode(MWsGraphicsContext::EDrawModePEN);
		}
	gc->ResetClippingRegion();
	return err;
	}

TBool CWsWindowRedraw::ReleaseMemory(MWsMemoryRelease::TMemoryReleaseLevel)
	{
	return EFalse;
	}

void CWsWindowRedraw::VisibleRegionChange()
	{
	}

TBool CWsWindowRedraw::ReadyToDraw() const
	{
	return ETrue;
	}

TBool CWsWindowRedraw::RedrawingInProgress() const
	{
	return EFalse;	
	}

void CWsWindowRedraw::WindowClosing()
	{
	ReleaseBackgroundElement();
	}

TBool CWsWindowRedraw::HasDsaElement() const
	{
	TBool hasDsaElement = EFalse;

	if (HasElement())
		{
		CWsClientWindow* cliWin = CliWin();
		CWindowElementSet& set = cliWin->Screen()->WindowElements();
		TBackgroundAttributes* backgroundAttributes = set.FindBackgroundElement(*cliWin);
		WS_ASSERT_DEBUG(backgroundAttributes,EWsPanicNoWindowElement);

		if (backgroundAttributes->iElement)
			{
			MWsElement& element = *(backgroundAttributes->iElement);
			hasDsaElement = (element.ConnectedSurface() == cliWin->Screen()->DsaSurface());
			}
		}

	return hasDsaElement;
	}

void CWsWindowRedraw::SetDsaElementL()
	{
	TRect extent(TPoint(0,0), WsWin()->Screen()->DSASizeInPixels());
	MWsDisplayMapping *dispMap = WsWin()->Screen()->DisplayMapping();
	TRect extentOut;
	TRect extentInDSA;
	if(dispMap)
		{
		dispMap->MapCoordinates(EDirectScreenAccessSpace,extent,EApplicationSpace,extentOut);
		//DSA extent in application space intersects window extent in application space
		extentOut.Intersection(WsWin()->FullRect());		
		if(extentOut.IsEmpty())
			{
			extentOut.SetRect(0,0,0,0);
			}
		//use DSA coordinates to determine the viewport
		dispMap->MapCoordinates(EApplicationSpace, extentOut, EDirectScreenAccessSpace, extentInDSA);
		}
	else
		{
		extentOut = extent;
		extentInDSA = extent;
		extentOut.Intersection(WsWin()->FullRect());
		}
	if (!HasDsaElement())
		{
		WsWin()->Screen()->ClearDsaSurface(extent, BackColor());
		}

	TSurfaceConfiguration sc;
	sc.SetSurfaceId(WsWin()->Screen()->DsaSurface());
	sc.SetExtent(extentOut.Size());
	sc.SetViewport(extentInDSA);

	SetBackgroundSurfaceL(sc, ETrue, ETrue);
	}

TBackgroundAttributes& CWsWindowRedraw::AcquireBackgroundElementL()
	{
	// Only client windows can have elements set
	WS_ASSERT_DEBUG(iWsWin->WinType() == EWinTypeClient,EWsPanicWindowType);
	CWsClientWindow* cliWin = static_cast<CWsClientWindow*>(iWsWin);
	CScreen* screen = cliWin->Screen();
	WS_ASSERT_DEBUG(screen,EWsPanicNoScreen);

	CWindowElementSet& set = screen->WindowElements();
	SetHasElement(EFalse);
	TBackgroundAttributes& backgroundAttributes = set.AcquireBackgroundElementL(*cliWin);
	MWsElement& element = *(backgroundAttributes.iElement);
	element.SetGlobalAlpha(cliWin->IsVisible() ? 0xFF : 0);
	SetHasElement(ETrue);
	screen->ElementAdded();
	
	return backgroundAttributes;
	}

void CWsWindowRedraw::SetBackgroundSurfaceL(const TSurfaceId& aSurface)
	{
	if (aSurface.Type() == TSurfaceId::EScreenSurface || aSurface.IsNull())
		{
		OwnerPanic(EWservPanicInvalidSurface);
		}

	CWsClientWindow* cliWin = CliWin();
	CScreen* screen = cliWin->Screen();
	CWindowElementSet& set = screen->WindowElements();
	TBackgroundAttributes& backgroundAttributes = AcquireBackgroundElementL();
	MWsElement& element = *(backgroundAttributes.iElement);
    TInt err = set.RegisterSurface(aSurface);
    if (err != KErrNone)
        {
        ReleaseBackgroundElement();
        User::Leave(err);
        }
	err = element.ConnectSurface(aSurface); 
	if (err != KErrNone)
		{
		set.UnregisterSurface(aSurface);
		ReleaseBackgroundElement();
		User::Leave(err);
		}

	TRect winExtent = cliWin->FullRect();
	element.SetDestinationRectangle(winExtent);
	
	// By default Element's source rectangle is set to its surface rectangle
	TRect srcRect;
	element.GetSourceRectangle(srcRect);
	cliWin->SetOriginalSrcElementRect(srcRect);
	cliWin->SetOriginalDestElementRect(winExtent);

	SetMayContainElementFlags();
	
	MWsWindowTreeObserver* windowTreeObserver = Screen()->WindowTreeObserver();
	if (windowTreeObserver)
		{
		windowTreeObserver->ElementAdded(*iWsWin, element);
		}
	}

void CWsWindowRedraw::SetBackgroundSurfaceL(const TSurfaceConfiguration& aConfiguration, TBool aTriggerRedraw, TBool aAllowScreenSurface)
	{
	if (aConfiguration.Size() < sizeof(TSurfaceConfiguration))
		{
		__ASSERT_COMPILE(sizeof(TSurfaceConfiguration2)==sizeof(TSurfaceConfiguration));
		if (aConfiguration.Size() != sizeof(TSurfaceConfiguration1))
			{
			OwnerPanic(EWservPanicInvalidSurfaceConfiguration);
			}
		}

	TSurfaceId surfaceId;
	aConfiguration.GetSurfaceId(surfaceId);
	if ((surfaceId.Type() == TSurfaceId::EScreenSurface && !aAllowScreenSurface) || surfaceId.IsNull())
		{
		OwnerPanic(EWservPanicInvalidSurface);
		}

	CFbsBitGc::TGraphicsOrientation tempOrientation = aConfiguration.Orientation();
	__ASSERT_COMPILE(CFbsBitGc::EGraphicsOrientationNormal==0 && 
			CFbsBitGc::EGraphicsOrientationRotated270 == 3);
	if(tempOrientation < CFbsBitGc::EGraphicsOrientationNormal || 
			tempOrientation > CFbsBitGc::EGraphicsOrientationRotated270)
		{
		OwnerPanic(EWservPanicInvalidSurfaceConfiguration);
		}

	CWsClientWindow* cliWin = CliWin();
	CScreen* screen = cliWin->Screen();
	__ASSERT_DEBUG(screen, Panic(EWsPanicNoScreen));
	
	CWindowElementSet& set = screen->WindowElements();
	TBool mustRegister = ETrue;
	TRect oldExtent(0,0,0,0);
	MWsElement::TElementRotation oldRotation = MWsElement::EElementAntiClockwise0;
	TBool oldFlip = EFalse;
	TRect oldViewport(0,0,0,0);
	TSurfaceId oldSurfaceId = TSurfaceId::CreateNullId();

	// If a element has already been set
	if (HasElement())
		{
		TBackgroundAttributes* backgroundAttributes = set.FindBackgroundElement(*cliWin);
		WS_ASSERT_DEBUG(backgroundAttributes,EWsPanicNoWindowElement);
		
		if (backgroundAttributes->iElement)
			{
			MWsElement& element = *(backgroundAttributes->iElement);
			element.GetDestinationRectangle(oldExtent);
			element.GetSourceRectangle(oldViewport);
			oldRotation = element.SourceRotation();
			oldFlip = element.SourceFlipping();
			oldSurfaceId = element.ConnectedSurface();
			mustRegister = EFalse;
			// If it is a different surface, flag to register the new surface
			if (element.ConnectedSurface() != surfaceId)
				{
				mustRegister = ETrue;
				}
			}
		}
	
	//the call to AcquireBackgroundElementL() will remove any existing background element
	TBackgroundAttributes& backgroundAttributes = mustRegister ? 
			AcquireBackgroundElementL() : *(set.FindBackgroundElement(*cliWin));
	MWsElement& element = *(backgroundAttributes.iElement);
	TInt err = KErrNone;
	if (mustRegister)
		{
        err = set.RegisterSurface(surfaceId);
        switch(err)
            {
        case KErrBadHandle:
            // Invalid surface IDs have to return KErrArgument
            err = KErrArgument;
            // drop through
        case KErrNoMemory:
        case KErrArgument:
            ReleaseBackgroundElement();
            User::Leave(err);
        case KErrNone:
            break;
        default:
            // No need to release layer here since session closure will do it
            // automatically when the client thread is panicked.
            OwnerPanic(EWservPanicInvalidSurface);
            }

		err = element.ConnectSurface(surfaceId);
		if (err != KErrNone)
			{
			set.UnregisterSurface(surfaceId);
			ReleaseBackgroundElement();	//Releasing new empty element
			User::Leave(err);
			}

		if (screen->DsaSurface() == surfaceId)
			{
	         TUint32 flags; 
	         element.GetRenderStageFlags(flags);
	         flags |= MWsElement::EElementIsDirectlyRenderedUserInterface;
	         element.SetRenderStageFlags(flags);
			}
		}

	SetHasElement(ETrue);		//set element flag
	SetMayContainElementFlags();
	
	err = CWindowElement::SetElement(element,aConfiguration,ETrue);	//set viewport and orientation
	if (err == KErrArgument)
		{
		OwnerPanic(EWservPanicInvalidSurfaceConfiguration);
		}
    TRect srcRect;
    aConfiguration.GetViewport(srcRect);
    if (!srcRect.IsEmpty())
        backgroundAttributes.SetExplicitViewPort();
    element.GetSourceRectangle(srcRect);
    cliWin->SetOriginalSrcElementRect(srcRect);

	//Set Extent
	TRect newExtent;
	aConfiguration.GetExtent(newExtent);
	SetElementExtentL(newExtent, backgroundAttributes);
	cliWin->SetOriginalDestElementRect(newExtent);
	
	MWsWindowTreeObserver* windowTreeObserver = Screen()->WindowTreeObserver();
	if (windowTreeObserver && mustRegister)
		{
		windowTreeObserver->ElementAdded(*iWsWin, element);
		}
	
	//If set, redraw
	if (aTriggerRedraw)
		{
		TRect newViewport;
		aConfiguration.GetViewport(newViewport);
		CFbsBitGc::TGraphicsOrientation orientation = aConfiguration.Orientation();
		MWsElement::TElementRotation newRotation = GcToElementRotation(orientation);
		TBool newFlip = aConfiguration.Flip();
		
		//The following parameter guarantees that update will be scheduled. 
		//This will trigger the composition.
		TBool alwaysScheduleUpdate = (oldSurfaceId != surfaceId) || 
									(oldExtent != newExtent) ||
									(oldViewport != newViewport) ||
									(oldRotation != newRotation)||
									(oldFlip != newFlip);
									
		ElementRedraw(oldExtent,newExtent,alwaysScheduleUpdate);
		}
	}

/**
Sets the EMayContainElement flag for parent window.
Sets the flag for all ancestor windows.
**/
void CWsWindowRedraw::SetMayContainElementFlags()
	{
	CWsWindowBase* parent = CliWin()->BaseParent();
	TInt type = parent->WinType();
	while(type ==EWinTypeClient)
			{
			CWsClientWindow* win = static_cast<CWsClientWindow*>(parent);
			win->Redraw()->iStateFlags |= EMayContainElement;
			parent=parent->BaseParent();
			type = parent->WinType();
			}
	}

void CWsWindowRedraw::SetElementExtentL(TRect& aNewExtent, TBackgroundAttributes& aAttributes)
	{
	CWsClientWindow* cliWin = CliWin();
	MWsElement& element = *(aAttributes.iElement);
	if (aNewExtent.IsEmpty())
		{
		aNewExtent = cliWin->FullRect();
		aAttributes.SetExplicitExtent(EFalse);
		}
	else
		{
		TRect tempWindowPosition = cliWin->FullRect();		//get window absolute coordinates
		aNewExtent.Move(tempWindowPosition.iTl);				//shift user defined extent to absolute coordinates
		aAttributes.SetExplicitExtent(ETrue);
		}
	element.SetDestinationRectangle(aNewExtent);
	}

void CWsWindowRedraw::ElementRedraw(const TRect& aOldExtent, const TRect& aNewExtent, TBool aAlwaysScheduleUpdate)
	{
	if (!aOldExtent.IsEmpty())
		{
		//If the previous extent was different
		if (aOldExtent != aNewExtent)
			{
			STACK_REGION tempRegion;
			tempRegion.AddRect(aOldExtent);
			tempRegion.AddRect(aNewExtent);

			//Calculate the difference between
			TRect tempRect = aOldExtent;
			tempRect.Intersection(aNewExtent);			//intersect both regions
			tempRegion.SubRect(tempRect);		//cut unaltered region
			Screen()->ScheduleRegionUpdate(&tempRegion);

			tempRegion.Close();
			}
		else
			{
			if(aAlwaysScheduleUpdate)
				{
				TTimeIntervalMicroSeconds interval(0);
				Screen()->ScheduleRender(interval);
				}
			}
		}
	else
		{
		TRegionFix<1> region(aNewExtent);
		Screen()->ScheduleRegionUpdate(&region);
		}
	}

void CWsWindowRedraw::RemoveBackgroundSurface(TBool aTriggerRedraw)
	{
	if (HasElement())
		{
		TBackgroundAttributes* backgroundAttributes = CliWin()->Screen()->
				WindowElements().FindBackgroundElement(*CliWin());
		WS_ASSERT_DEBUG(backgroundAttributes,EWsPanicNoWindowElement);
		if (backgroundAttributes->iElement)
			{
			RemoveBackgroundElement(aTriggerRedraw);
			}
		}
	}

void CWsWindowRedraw::RemoveBackgroundElement(TBool aTriggerRedraw)
	{
	CWsClientWindow* cliWin = CliWin();
	CScreen* screen = cliWin->Screen();
	TRect tempRect;
	if (aTriggerRedraw)
		{
		TBackgroundAttributes* backgroundAttributes = screen->WindowElements().FindBackgroundElement(*CliWin());
		WS_ASSERT_DEBUG(backgroundAttributes,EWsPanicNoWindowElement);
		if (backgroundAttributes->ExplicitExtent())
			{
			backgroundAttributes->iElement->GetDestinationRectangle(tempRect);
			backgroundAttributes->SetExplicitExtent(EFalse);
			}
		else
			{
			tempRect = cliWin->FullRect();
			}
		}
	ReleaseBackgroundElement();
    if (aTriggerRedraw)
        {
        if (screen->ChangeTracking())
            {
            TTimeIntervalMicroSeconds interval(0);
            screen->ScheduleRender(interval);
            }
        else
            {
            TRegionFix<1> region(tempRect);
            screen->ScheduleRegionUpdate(&region);
            }
        }
	}

void CWsWindowRedraw::GetBackgroundSurfaceL(TSurfaceConfiguration& aConfiguration)
	{
	if (aConfiguration.Size() < sizeof(TSurfaceConfiguration))
		{
		__ASSERT_COMPILE(sizeof(TSurfaceConfiguration2)==sizeof(TSurfaceConfiguration));
		if (aConfiguration.Size() != sizeof(TSurfaceConfiguration1))
			{
			OwnerPanic(EWservPanicInvalidSurfaceConfiguration);
			}
		}
	
	CWsClientWindow* cliWin = CliWin();
	TBackgroundAttributes* backgroundAttributes = NULL;

	if (HasElement())
		{
		backgroundAttributes = cliWin->Screen()->WindowElements().FindBackgroundElement(*cliWin);
		WS_ASSERT_DEBUG(backgroundAttributes,EWsPanicNoWindowElement);
		if (!backgroundAttributes->iElement)
			{
			User::Leave(KErrNotFound);
			}
		}
	else
		{
		User::Leave(KErrNotFound);
		}

	MWsElement& element = *(backgroundAttributes->iElement);

	TInt errCode=CWindowElementSet::GetConfiguration(aConfiguration,element);

	// Get source rect
	if (errCode>=KErrNone)
	    {
	    if (!backgroundAttributes->ExplicitViewPort())
	        {
	        aConfiguration.SetViewport(TRect());
	        }
	    else
	        {
	        TRect tempExtent = cliWin->GetOriginalSrcElementRect();
	        aConfiguration.SetViewport(tempExtent);
	        }
	    }

	//Convert and copy extent
	if (errCode>=KErrNone)
		{
		if (!backgroundAttributes->ExplicitExtent())
			{
			aConfiguration.SetExtent(TRect());
			}
		else	//translate to window coordinates
			{
			TRect tempExtent = cliWin->GetOriginalDestElementRect();
			tempExtent.Move(-cliWin->Origin());
			aConfiguration.SetExtent(tempExtent);
			}
		}
	}

void CWsWindowRedraw::ReleaseBackgroundElement()
	{
	if (HasElement())
		{
		CWsClientWindow* cliWin = CliWin();
		CScreen* screen = cliWin->Screen();
		screen->WindowElements().ReleaseBackgroundElement(*cliWin, ETrue);
		screen->ElementRemoved();
		}
	}

//
// Blank window //
//

CWsBlankWindow::CWsBlankWindow(CWsWindow *aWin) : CWsWindowRedraw(aWin), iColor(iWsWin->RootWindow()->DefaultBackgroundColor()), iNoColor(EFalse)
	{
	}

CWsBlankWindow::~CWsBlankWindow()
	{
	}

void CWsBlankWindow::ConstructL()
	{
	CWsWindowRedraw::ConstructL();
	if (Screen()->ChangeTracking())
		{
		STACK_REGION dirtyRegion;
		dirtyRegion.Copy(iWsWin->WindowArea());
		dirtyRegion.Offset(-iWsWin->Origin());
		iWsWin->AddDirtyWindowRegion(dirtyRegion);
		dirtyRegion.Close();
		}
	}

void CWsBlankWindow::SetColor(TRgb aColor)
	{
	iColor=aColor;
	iNoColor=EFalse;
	if (Screen()->ChangeTracking())
		{
		STACK_REGION dirtyRegion;
		dirtyRegion.Copy(iWsWin->WindowArea());
		dirtyRegion.Offset(-iWsWin->Origin());
		iWsWin->AddDirtyWindowRegion(dirtyRegion);
		dirtyRegion.Close();

		if (iWsWin->IsActive() && iWsWin->IsVisible())
			{
			Screen()->ScheduleWindow(iWsWin);
			}
		}
	else
		{
		Screen()->AddRedrawRegion(iWsWin->VisibleRegion());
		}
	}

TBool CWsBlankWindow::CommandL(TInt aOpcode, TWsWinCmdUnion &aCmd)
	{
	switch(aOpcode)
		{
		case EWsWinOpSetBackgroundSurface:
			SetBackgroundSurfaceL(*aCmd.Surface);
			break;
		case EWsWinOpSetBackgroundSurfaceConfig:
			SetBackgroundSurfaceL(aCmd.SurfaceConfigurationAndTrigger->surfaceConfig, aCmd.SurfaceConfigurationAndTrigger->triggerRedraw, EFalse);
			break;
		case EWsWinOpRemoveBackgroundSurface:
			RemoveBackgroundSurface(*aCmd.Bool);
			break;
		case EWsWinOpGetBackgroundSurfaceConfig:
			{
			TSurfaceConfiguration tempConfiguration = *aCmd.SurfaceConfiguration;
			GetBackgroundSurfaceL(tempConfiguration);
			TInt tempSize = aCmd.SurfaceConfiguration->Size();
			if (sizeof(TSurfaceConfiguration)<tempSize)
				tempSize = sizeof(TSurfaceConfiguration);
			CWsClient::ReplyBuf(&tempConfiguration,tempSize);
			}
			break;
		case EWsWinOpSetColor:
			SetColor(*aCmd.rgb);
			break;
		case EWsWinOpSetNoBackgroundColor:
			SetBackgroundClear();
			break;
		default:
			return(EFalse);
		}
	return(ETrue);
	}

TRgb CWsBlankWindow::BackColor() const
	{
	return(iColor);
	}

TBool CWsBlankWindow::GetRedrawRect(TRect &) const
	{
	if (!iNoColor || iWsWin->iAnimList)
		iWsWin->Screen()->AddRedrawRegion(iWsWin->VisibleRegion());
	return(EFalse);
	}

TBool CWsBlankWindow::NeedsRedraw() const
	{
	return(EFalse);
	}

void CWsBlankWindow::DrawWindow()
	{
	if ((!iNoColor)||HasElement())
		{
		DrawBackgroundColor(*iRedrawRegion,!iNoColor);
		}
	}
