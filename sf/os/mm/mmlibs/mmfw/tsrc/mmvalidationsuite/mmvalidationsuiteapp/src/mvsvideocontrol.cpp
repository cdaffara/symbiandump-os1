// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <eikenv.h>
#include <mvs/videoplayagent.h>

#include "mvsvideocontrol.h"

CMVSVideoControl* CMVSVideoControl::NewLC(CMVSVideoPlayAgent& aVideoPlayAgent, TInt aDisplay, RWindowTreeNode& aParent)
	{
	CMVSVideoControl* self = new (ELeave) CMVSVideoControl(aVideoPlayAgent, aDisplay);
	CleanupStack::PushL(self);
	self->ConstructL(aParent);
	return self;
	}

CMVSVideoControl* CMVSVideoControl::NewL(CMVSVideoPlayAgent& aVideoPlayAgent, TInt aDisplay, RWindowTreeNode& aParent)
	{
	CMVSVideoControl* self = CMVSVideoControl::NewLC(aVideoPlayAgent, aDisplay, aParent);
	CleanupStack::Pop(self);
	return self;
	}

CMVSVideoControl::CMVSVideoControl(CMVSVideoPlayAgent& aVideoPlayAgent, TInt aDisplay) :
	iVideoPlayAgent(aVideoPlayAgent),
	iDisplay(aDisplay)
	{
	}

void CMVSVideoControl::ConstructL(RWindowTreeNode& aParent)
	{
	CreateWindowL(aParent);
	iPip = CMVSPipControl::NewL(Window());
	
  	Window().SetBackgroundColor(KRgbDarkGray);
	}

CMVSVideoControl::~CMVSVideoControl()
	{
	iOverlayText.Close();
	delete iPip;
	}

RWindow& CMVSVideoControl::ControlWindow() const
	{
	return Window();
	}

TInt CMVSVideoControl::ScreenNumber() const
	{
	return iDisplay;
	}
	
CMVSPipControl& CMVSVideoControl::Pip() const
	{
	return *iPip;
	}

void CMVSVideoControl::EnablePip(CMVSPipControl::TRenderType aRenderType)
	{
	iPip->ControlWindow().SetVisible(ETrue);
	iPip->SetRenderType(aRenderType);
	}

void CMVSVideoControl::DisablePip()
	{
	iPip->ControlWindow().SetVisible(EFalse);
	}

void CMVSVideoControl::SetOverlayTextL(const TDesC& aOverlayText)
	{
	iOverlayText.Close();
	iOverlayText.CreateL(aOverlayText);
	}

void CMVSVideoControl::ClearOverlayText()
	{
	iOverlayText.Close();
	}

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
void CMVSVideoControl::Draw(const TRect& aRect) const
#else
void CMVSVideoControl::Draw(const TRect& /*aRect*/) const
#endif
	{
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	iVideoPlayAgent.RenderSubtitle(aRect);
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
      
	if (0 < iOverlayText.Length())
		{
		//Establish a Graphics Context
		CWindowGc& gc = SystemGc();

	    //Establish a drawing rectangle
		TRect rect=Rect();

		// Draw the overlay text if needed
        gc.SetBrushStyle(CGraphicsContext::ENullBrush);
    	gc.SetBrushColor(KRgbRed);
         
        gc.SetPenStyle(CGraphicsContext::ESolidPen);
        gc.SetPenColor(KRgbBlue);

        // Get a font
        const CFont* appFont = CEikonEnv::Static()->AnnotationFont();         
        gc.UseFont(appFont);        

		// Rect will always be at lest 3 in size
        rect.Shrink(3, 3);
	    TInt baseline = (rect.Height() + appFont->AscentInPixels()) >> 1;

        gc.DrawText(iOverlayText, rect, baseline, CGraphicsContext::ECenter);

	   	//Done with our font
	    gc.DiscardFont();	
		}
	}

TInt CMVSVideoControl::CountComponentControls() const
	{
	return 1;
	}

CCoeControl* CMVSVideoControl::ComponentControl(TInt aIndex) const
	{
	if (aIndex == 0) 
		{
		return iPip;
		}
	else
		{
		return NULL;
		}
	}

CMVSPipControl* CMVSPipControl::NewL(RWindowTreeNode& aParent)
	{
	CMVSPipControl* self = new (ELeave) CMVSPipControl();
	CleanupStack::PushL(self);
	self->ConstructL(aParent);
	CleanupStack::Pop(self);
	return self;
	}

CMVSPipControl::CMVSPipControl() 
	{
	}

void CMVSPipControl::ConstructL(RWindowTreeNode& aParent)
	{
	CreateWindowL(aParent);
	Window().SetBackgroundColor(TRgb(0, 0));
	}

CMVSPipControl::~CMVSPipControl()
	{
	}

void CMVSPipControl::Draw(const TRect& /*aRect*/) const
	{
	}

void CMVSPipControl::MmsehSurfaceCreated(TInt aDisplayId, const TSurfaceId& /*aId*/, const TRect& aCropRect, TVideoAspectRatio /*aAspectRatio*/)
	{
	__ASSERT_ALWAYS(iRenderType == EUseCrp, User::Invariant());
	iDisplayId = aDisplayId;
	iCropRect = aCropRect;
	}

void CMVSPipControl::MmsehSurfaceParametersChanged(const TSurfaceId& /*aId*/, const TRect& /*aCropRect*/, TVideoAspectRatio /*aAspectRatio*/)
	{
	__ASSERT_ALWAYS(iRenderType == EUseCrp, User::Invariant());
	}

void CMVSPipControl::MmsehRemoveSurface(const TSurfaceId& /*aId*/)
	{
	__ASSERT_ALWAYS(iRenderType == EUseCrp, User::Invariant());	
	}

void CMVSPipControl::SetRenderType(TRenderType aRenderType)
	{
	iRenderType = aRenderType;
	}

void CMVSPipControl::Clear()
	{
	DrawNow();
	}

RWindow& CMVSPipControl::ControlWindow() const
	{
	return Window();
	}
