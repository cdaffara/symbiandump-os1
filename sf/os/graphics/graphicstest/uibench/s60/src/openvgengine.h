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
 //


#ifndef OPENVGENGINE_H_
#define OPENVGENGINE_H_


#include "engine.h"

#include <VG/openvg.h>
#include <EGL/egl.h>


const TInt KMaxDisplayCoversExample3 = 40;
const TInt KMaxDisplayLeftExample3 = -3;
const TInt KMaxDisplayRightExample3 = 3;

// the max number of covers visible on the screen at any given time
const TInt KMaxCoversExample3 = 40;


class  COpenVGEngine : public CBase, public MEngine
	{
public: 
	static COpenVGEngine* NewL(RWindow& aWindow, EGLDisplay& aDisplay, EGLSurface& aSurface, EGLContext& aContext);
	~COpenVGEngine();
	TInt GetSpeed();
	
	// From MEngine
	void ActivateL();
	void Deactivate();	
	void Step();
	TBool IsPending();
	void Refresh();
	TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent);
	
private: 
	COpenVGEngine(RWindow& aWindow, EGLDisplay& aDisplay, EGLSurface& aSurface, EGLContext& aContext);
	void DrawCover(TInt i);
	void NextCover();
	void PreviousCover();		
	void ToggleCoverReflection();
	VGfloat GetMiddleCoverScalingFactor(VGfloat coverPosition);
	
private:
    RWindow& iWindow;
    EGLDisplay& iDisplay;
    EGLSurface& iSurface;
    EGLContext& iContext;
    
	TPoint iOldPos;
	TBool isResize;
		
	VGfloat iCoverLocation[KMaxCoversExample3];
	VGint iWantedCover;
    TBool iHasPendingDraw;
	TBool iShowCoverImage;
	VGfloat iSpeedOffset;
	TBool iShowMirror;
	
	TSize iSurfaceSize;
	
	VGfloat iSpeed;
	RArray<VGImage> iImages;
	TInt iCurrentImageIndex;	

	VGPaint iShadowPaint;
	};

#endif 
