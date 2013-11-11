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


#include "openvgengine.h"
#include "eglrendering.h"

#include <eikenv.h>
#include <e32math.h>


_LIT(KCoverBitmaps, "z:\\resource\\apps\\covers.mbm");


GLDEF_D VGUErrorCode vguComputeWarpQuadToQuadProxy(VGfloat dx0, VGfloat dy0,
                                                   VGfloat dx1, VGfloat dy1,
                                                   VGfloat dx2, VGfloat dy2,
                                                   VGfloat dx3, VGfloat dy3,
                                                   VGfloat sx0, VGfloat sy0,
                                                   VGfloat sx1, VGfloat sy1,
                                                   VGfloat sx2, VGfloat sy2,
                                                   VGfloat sx3, VGfloat sy3,
                                                   VGfloat * matrix)
	{
	vguComputeWarpQuadToQuad(
                                                   sx0, sy0,
                                                   sx1, sy1,
                                                   sx2, sy2,
                                                   sx3, sy3,
                                                   dx0, dy0,
                                                   dx1, dy1,
                                                   dx2, dy2,
                                                   dx3, dy3,
                                                   matrix);
	return VGU_NO_ERROR;
	}

COpenVGEngine* COpenVGEngine::NewL(RWindow& aWindow,EGLDisplay& aDisplay, EGLSurface& aSurface, EGLContext& aContext)
	{
	COpenVGEngine* self = new(ELeave) COpenVGEngine(aWindow, aDisplay, aSurface, aContext);
	return self;
	}

COpenVGEngine::COpenVGEngine(RWindow& aWindow,EGLDisplay& aDisplay, EGLSurface& aSurface, EGLContext& aContext) :
        iWindow(aWindow), iDisplay(aDisplay), iSurface(aSurface), iContext(aContext), iWantedCover(20),
        iHasPendingDraw(EFalse), iShowCoverImage(EFalse), iSpeedOffset(0), iShowMirror(ETrue), iSpeed(0),
        iCurrentImageIndex(0)
    {
#ifdef PORTRAIT_MODE
    iSurfaceSize.iWidth = iWindow.Size().iHeight;
    iSurfaceSize.iHeight = iWindow.Size().iWidth;
#else
    iSurfaceSize = iWindow.Size();
#endif  
    // initiate the location of each cover & make the wanted one the cover at the opposite end
    for(TInt i = 0; i < KMaxCoversExample3; ++i)
        {
        iCoverLocation[i] = i;
        }
    }

COpenVGEngine::~COpenVGEngine()
	{
	Deactivate();
	}

TInt COpenVGEngine::GetSpeed()
	{
	return static_cast<TInt>(iSpeed * 100000);
	}

TBool COpenVGEngine::IsPending()
	{
	return iHasPendingDraw;
	}

void COpenVGEngine::ActivateL()
	{
	CEGLRendering::EGLCheckReturnError(eglMakeCurrent(iDisplay, iSurface, iSurface, iContext));
	
	// Setup initial OpenVG context state
	VGfloat clearColour[] = { 0.1f, 0.1f, 0.2f, 1.0f };
	vgSetfv(VG_CLEAR_COLOR, 4, clearColour);
	vgSeti(VG_IMAGE_QUALITY, VG_IMAGE_QUALITY_NONANTIALIASED);
	vgSeti(VG_RENDERING_QUALITY, VG_RENDERING_QUALITY_NONANTIALIASED);	
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);

	CFbsBitmap* bitmap = new(ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	TInt idx = 0;
	while(bitmap->Load(KCoverBitmaps, idx++) == KErrNone)
		{
		VGint width = bitmap->SizeInPixels().iWidth;
		VGint height = bitmap->SizeInPixels().iHeight;
		// Crate VGImage
		VGImage image = vgCreateImage(VG_sRGB_565, width, height, VG_IMAGE_QUALITY_NONANTIALIASED);
		CEGLRendering::VGCheckError();
		//Load Symbian bitmap into VGImage
		vgImageSubData(image, bitmap->DataAddress(), bitmap->DataStride(), VG_sRGB_565, 0, 0, width, height);
		CEGLRendering::VGCheckError();
		iImages.AppendL(image);		
		}
	CleanupStack::PopAndDestroy(bitmap);
	iHasPendingDraw = ETrue;
	
	//Checks if any images were loaded
	if(iImages.Count() == 0)
		{
		User::Leave(KErrNotFound);
		}
	
	iShadowPaint = vgCreatePaint();
	CEGLRendering::VGCheckError();
	if (iShadowPaint != VG_INVALID_HANDLE)
		{
		VGfloat paintColour[4] = { 0.4f, 0.4f, 0.6f, 1.0f };
		vgSetParameterfv(iShadowPaint, VG_PAINT_COLOR, 4, paintColour); 
		CEGLRendering::VGCheckError();
		}
	}

void COpenVGEngine::Deactivate()
	{
	CEGLRendering::EGLCheckReturnError(eglMakeCurrent(iDisplay, iSurface, iSurface, iContext));
	for (TInt i = iImages.Count() - 1; i >= 0; --i)
		{
		vgDestroyImage(iImages[i]);
		}
	vgDestroyPaint(iShadowPaint);
	eglWaitClient();
	iImages.Reset();
	iHasPendingDraw = EFalse;
	}

void COpenVGEngine::Step()
	{
	CEGLRendering::EGLCheckReturnError(eglMakeCurrent(iDisplay, iSurface, iSurface, iContext));
		
#ifdef PORTRAIT_MODE	
	vgClear(0, 0, iSurfaceSize.iHeight, iSurfaceSize.iWidth);
#else
	vgClear(0, 0, iSurfaceSize.iWidth, iSurfaceSize.iHeight);
#endif
	
	if (Abs(iCoverLocation[iWantedCover]) < 0.03)
		{
		iSpeed = 0.0f;
		iHasPendingDraw = EFalse;
		}
	else if (Abs(iCoverLocation[iWantedCover]) < 0.5)
		{
		iSpeed *= 0.7;
		}
	else
		{
		iSpeed = 0.05 * (2 + Abs(iCoverLocation[iWantedCover]) + (Abs(iCoverLocation[iWantedCover]) + 1)
                    * (Abs(iCoverLocation[iWantedCover]) + 1) / 2);
		}
	// For each Cover, update its location in the correct direction
	// Check if the wanted cover is already at the CenterStage point

	VGfloat moveEachCover = iSpeed;
	if (iCoverLocation[iWantedCover] > 0.0)
		{
		moveEachCover *= -1;
		}	
	
	for (TInt i = 0; i < KMaxCoversExample3; ++i)
		{
		iCoverLocation[i] += moveEachCover;
		}
	
	TInt coverClippingCount = 10;
	TInt middleCoverPos = 0;
	VGfloat threshold = 0.50f;

	while(Abs(iCoverLocation[middleCoverPos]) > threshold)
		{
		++middleCoverPos;
		}
	
	
	//left
	TInt cutOff = middleCoverPos - coverClippingCount;
	if (cutOff <0 )
		{
		cutOff = 0;
		}
	for (TInt i = cutOff; i < middleCoverPos; ++i)
		{
		DrawCover(i);
		}
	
	//right
	cutOff = coverClippingCount + middleCoverPos;
	if (cutOff >= KMaxCoversExample3)
		{
		cutOff = KMaxCoversExample3-1;
		}
	
	for (TInt j = cutOff; j >= middleCoverPos; --j)
	    {
		DrawCover(j);
        }

	static TInt dir = 1;
	if (iWantedCover == (KMaxCoversExample3 - 1))
		{
		dir = -1;
		}
	else if (iWantedCover == 0)
		{
		dir = 1;
		}

	iWantedCover += dir;
	iHasPendingDraw = ETrue;
	}

void COpenVGEngine::DrawCover(TInt coverIndex)
	{ 	
		VGImage image = iImages[coverIndex % iImages.Count()];
		// Starting at the outside, render each visible (+/-  KMaxDisplayCoversExample3/2) Cover
		// Calculate its path
		vgLoadIdentity();
#ifdef PORTRAIT_MODE
		vgTranslate(iSurfaceSize.iHeight, 0);
		vgRotate(90);
#endif
		VGfloat coverPosition = iCoverLocation[coverIndex];
		VGfloat tempMatrix[3][3];

		//flip coords
		VGfloat flipmatrix[] = 
			{
			1.0f, 0.0f, 0.0f, 
			0.0f, -1.0f, 0.0f,
			0.0f, 0.0f, 1.0f
			};
		vgMultMatrix(flipmatrix);		
		VGint imageWidth = vgGetParameteri(image, VG_IMAGE_WIDTH);
		VGint imageHeight = vgGetParameteri(image, VG_IMAGE_HEIGHT);

		//VGint yTrans = -200;	
		
		//factors which must be multiplied with side of image which will be projected towards back
		//valid if projecting right image side to back
		//opposite is (1 - factor) for projecting left image side.

		VGfloat bottomProjectXFactor= (0.75f);
		VGfloat bottomProjectYFactor = (0.20f);
		
		VGfloat topProjectXFactor = (0.75f);
		VGfloat topProjectYFactor = (0.90f);
				
		VGfloat imageSpacingFactor = 0.16;
		
		VGfloat translationOffset = 0.0;
		

		//float yscale = 1.7;
		//float xscale = 4.4;
		//imageHeight = Min(iSurfaceSize.iWidth/xscale, iSurfaceSize.iHeight/yscale);
		//imageWidth = imageHeight;
		//TInt KImageSize = imageHeight/1.125;
		//VGint yTrans = iSurfaceSize.iHeight/-1.2;	
		
		TInt KImageSize = (imageHeight * 8) / 9; //KImageSize - secondary covers should be 8/9 of the size of the middle cover
		//VGint yTrans = -200;	
		VGint yTrans = - (iWindow.Size().iHeight * 5) / 6;	
		
		
		VGfloat middleTranslationOffset = KImageSize / 2;		
		VGfloat coverProjectionLimit = 10;
		
		if (coverPosition >= 1)
			{
			//if considering an image on the right side, this is offset from middle to place image on screen			
			translationOffset = middleTranslationOffset- KImageSize/2 + KImageSize*imageSpacingFactor * (coverPosition -1);
						
			//left side of image goes back.
			vguComputeWarpQuadToQuadProxy(0.0f, 0.0f, 
					   imageWidth, 0.0f, 
					   0.0f, imageHeight, 
					   imageWidth, imageHeight,					   
					   KImageSize * (1 - bottomProjectXFactor*(1-Abs(coverPosition)/coverProjectionLimit)),KImageSize * bottomProjectYFactor,//left vertex
					   KImageSize, 0.0f,
					   KImageSize * (1 - topProjectXFactor*(1-Abs(coverPosition)/coverProjectionLimit)), KImageSize * topProjectYFactor,//left vertex
					   KImageSize, KImageSize,			   
					   &tempMatrix[0][0]);
			}
		else if (coverPosition < -1)
			{			
			//must move an extra image width from center , as coordinates from bottom left corner of image. 
			translationOffset = - (middleTranslationOffset + (KImageSize * imageSpacingFactor) * ( -coverPosition - 1) + KImageSize/2) ;
			
			vguComputeWarpQuadToQuadProxy(  0.0f, 0.0f, 
					   imageWidth, 0.0f, 
					   0.0f, imageHeight, 
					   imageWidth, imageHeight,
					   
					   0.0f, 0.0f,
					   (bottomProjectXFactor*(1-Abs(coverPosition)/coverProjectionLimit))* KImageSize, bottomProjectYFactor * KImageSize, //Right Vertex
					   0.0f, (KImageSize),
					   (topProjectXFactor*(1-Abs(coverPosition)/coverProjectionLimit)) * KImageSize, topProjectYFactor * KImageSize, //Right Vertex
					   
					   &tempMatrix[0][0]);			
			}
		else if((coverPosition > -1) && (coverPosition <= 0))// -0.07))
			{			
			translationOffset = -middleTranslationOffset * Abs(coverPosition) - KImageSize/2 ;
						
			vguComputeWarpQuadToQuadProxy(  0.0f, 0.0f, 
	    						   imageWidth, 0.0f, 
	    						   0.0f, imageHeight, 
	    						   imageWidth, imageHeight,	    						   
	    						   0.0f, 0.0f,
	    						   KImageSize * (1 - (1-bottomProjectXFactor) * Abs(coverPosition)), KImageSize * bottomProjectYFactor * Abs(coverPosition),
	    						   0.0f, KImageSize,
	    						   (KImageSize * (1 - ( 1 - topProjectXFactor) * Abs(coverPosition))) , KImageSize * (1 - (1 - topProjectYFactor) * Abs(coverPosition)),	    						   
	    						   &tempMatrix[0][0]);		
			}				
		else if ((coverPosition >=0) && (coverPosition <= 1))
			{
			translationOffset = middleTranslationOffset * Abs(coverPosition) - KImageSize / 2;				    						   			
			vguComputeWarpQuadToQuadProxy( 0.0f, 0.0f, 
	    						   imageWidth, 0.0f, 
	    						   0.0f, imageHeight, 
	    						   imageWidth, imageHeight,	    						   
	    						   KImageSize * (1-bottomProjectXFactor)* (coverPosition), KImageSize * (bottomProjectYFactor) * (coverPosition),
	    						   KImageSize, 0,
	    						   KImageSize * ( 1 - topProjectXFactor) * (coverPosition) , KImageSize * (1 - (1 - topProjectYFactor) * Abs(coverPosition)),
	    						   KImageSize, KImageSize,	    						   
	    						   &tempMatrix[0][0]);
			}
		iSpeedOffset = 140*(iSpeed)*(iSpeed);		
        if (iCoverLocation[iWantedCover] < 0)
            {
            iSpeedOffset *= -1;
            }
		vgTranslate(iWindow.Size().iWidth/2 + translationOffset + iSpeedOffset, yTrans);
						
		vgMultMatrix(&tempMatrix[0][0]);
		if (Abs(coverPosition) <= 1)
			{
			VGfloat scale = GetMiddleCoverScalingFactor(coverPosition);
			vgScale(scale,scale);
			vgTranslate(-(scale-1)/2 * KImageSize,-(scale-1)/2 * KImageSize);
			}
		vgDrawImage(image);
		CEGLRendering::VGCheckError();
		if (iShowMirror)
			{
			vgScale(1,-1);
			vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_MULTIPLY);
			vgTranslate(0,-4*KImageSize+226); 
			//vgTranslate(0,iSurfaceSize.iHeight/-.839); 
					
			vgSetPaint(iShadowPaint, VG_FILL_PATH);
			vgDrawImage(image);
			CEGLRendering::VGCheckError();
				
			vgSeti(VG_IMAGE_MODE,VG_DRAW_IMAGE_NORMAL);
			}
	}

TKeyResponse COpenVGEngine::HandleKeyEventL(const TKeyEvent& aKeyEvent)
	{
	TKeyResponse response = EKeyWasConsumed;
	switch (aKeyEvent.iCode)
		{
	case EKeyRightArrow:
		NextCover();
		break;	
	case EKeyLeftArrow:
		PreviousCover();
		break;		
	case EKeyBackspace: 
		ToggleCoverReflection();
		break;		
	default:
		response = EKeyWasNotConsumed;
		break;
		};
	return response;
	}

void COpenVGEngine::NextCover()
	{
	if (iWantedCover < (KMaxCoversExample3 - 1))
	    {
	    ++iWantedCover;
	    iHasPendingDraw = ETrue;
	    }		
	}

void COpenVGEngine::PreviousCover()
	{
	if (iWantedCover > 0)
	    {
	    --iWantedCover;
	    iHasPendingDraw = ETrue;
	    }
	}

void COpenVGEngine::ToggleCoverReflection()
	{
	iShowMirror = !iShowMirror;
	}

VGfloat COpenVGEngine::GetMiddleCoverScalingFactor(VGfloat aCoverPosition)
	{
	if(Abs(aCoverPosition) > 1)
	    {
	    return 0.0f;
	    }
	return (-0.125 * Abs(aCoverPosition) + 1.125);	
	}

void COpenVGEngine::Refresh()
	{
	iHasPendingDraw = ETrue;
	}
