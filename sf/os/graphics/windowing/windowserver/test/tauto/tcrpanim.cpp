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
// Implements  CTCrpAnim
// Test CRP animations & their interaction with overlapping transparent/non-transparent windows 
// & wserv's underlying redraw-store strategies
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <w32stdgraphic.h>
#include "tcrpanim.h"

// RUN_SAMPLE_ON_LEFT allows the demo animation to run in the left-hand window during testing. 
// Used for demonstration purposes only
#define RUN_SAMPLE_ON_LEFT

namespace	//anonymous local scope
	{
	const TInt KAnimationFrameDelayTime = 50000; 				// delay in microseconds between frames
	const TInt KShortDelayLoop = 2*KAnimationFrameDelayTime; 	// delay time in microseconds used in test cases
	const TInt KAnimationTotalFrames = 40;						// total number of frames in a CWsGraphicBitmapAnimation
	const TInt KAnimDimension = 40;								// animation width/height. We're enforcing a square animation here
	const TInt KFrameMissedAnimationsThreshold = 10;			// maximum number of missed frame steps allowed
	const TInt KAnimationTearWidthThreshold = 4; 				// maximum columns permitted between a tear
	const TInt KMinGoodFrameThreshold = 30; 					// percentage threshold for number of good frames detected in a test
	const TInt KMaxXY = 200;									// arbitrary maximum size of square used to invalidate a window
	const TInt KMaxRepeatDraw = 2;								// arbitrary value for DrawLine calls during a Draw
	TUid  KUidTestAnimation2 = {0xBAADF00D};					// unique id. for CWsGraphicBitmapAnimation object
	const TUint32 KWhitePixels = 0xFFFFFFFF;					// 32-bit mask value for rgb white
	const TUint32 KBlackPixels = 0x00000000;					// 32-bit value for rgb black
	const TPoint KPointZero(0,0);								// initial point used for animation creation & manipulation (currently 0,0)
	const TPoint KPointOffsite(1000,1000);						// point used to draw off-screen
	const TDisplayMode KTestDisplayMode = EColor16MU;			// display mode used for testing
	const TInt KFrameStepCalculation = Max(1, KAnimDimension/Max(1, KAnimationTotalFrames)); // determine framestep size in columns
	
	enum TColorDetected
		{
		ECantTell=0,
		EDetRed=1,
		EDetGreen=2,
		EDetBlue=4,
		EDetBlack=0x10,
		EDetGrey=0x20,
		EDetWhite=0x40
		};

	class CCrpAnim;		
	class CAnimRedrawWindow : public CTWin
		{
	public:
		CAnimRedrawWindow(CCrpAnim *aAnimWindow, TBool aIsBase);
		~CAnimRedrawWindow();
		void Draw();
	private:
		CCrpAnim *iAnimWindow;
		TBool iIsBase;
		};
		
	class CCrpAnim : public CBase
		{
		friend class CAnimRedrawWindow;
	public:
		enum TWinType
			{
			ERedraw,
			EBlank,		// note: not currently used in tcrpanim tests
			EBackedUp	// note: not currently used in tcrpanim tests
			};
	public:
		CCrpAnim(TBool aIsBase, TWinType aWinType);
		~CCrpAnim();
		enum 
			{
			ENoTransparency=0x100
			};
		void ConstructL(const TPoint &aPos, const TSize &aSize,const TInt aAlphaValue=ENoTransparency);
		void DoDraw(TBool aBlankIt);
		inline void DoDraw();
		void DoDrawEllipse();
		inline TSize Size() {return iCtWin->Size();};
		inline RWindowBase* BaseWin() const {return iCtWin->BaseWin();};
		inline RWindow* Window() const {return STATIC_CAST(RWindow*, iCtWin->BaseWin());};
		inline CTBaseWin* CtBaseWin() {return iCtWin;};
		inline void Invalidate() {CTUser::Splat(TheClient, TRect(iCtWin->Position(), iCtWin->Size()), KRgbGray);};
		void Invalidate(const TRect &aRect);
		static void SetEllipseDrawMode(CGraphicsContext::TDrawMode aEllipseDrawMode);
		void InvalidateAndRedraw(TBool aUseBlankItMember,TBool aBlankIt,TBool aUseRWindowInvalidate,TRect* aRect=NULL);

		//A bit of an animation interface...
		//I have written this interface to be amenable to playing multiple animations,
		//which I think needs testing,
		//but the underlying implementation assumes one animation at present.
		//Your mission, should you choose to accept it, ....
		
		void SetPosAnimation(const TUid& aUid, const TRect& aRect);
		TRect* GetPosAnimation(const TUid& aUid);
		TWsGraphicAnimation* SetAnimation(TUid);	
		TWsGraphicAnimation* GetAnimation(TUid);	
		TBool RemoveAnimation(TUid);	
		inline void	SetBlankIt(TBool aNewVal) {iBlankIt = aNewVal;};
		inline void SetRepeatDrawMax(TInt aVal) {iRepeatDrawMax = aVal;};
	protected:
		static void Draw(CBitmapContext *aGc, const TSize &aSize, TBool aIsBase,const TRect &aRect, TBool aBlankIt,TInt aRepeat, TInt aAlphaValue);
		static void DrawEllipse(CBitmapContext *aGc, const TRect &aRect, TInt aAlphaValue);
		CTBaseWin *iCtWin;
		TWinType iWinType;
		TBool iIsBase;
		TBool iBlankIt;
		TRect iRect;
		TInt  iRepeatDrawMax;
		static CGraphicsContext::TDrawMode iEllipseDrawMode;
		TUid  iAnimUid;
		TWsGraphicAnimation iAnimData;
		TRect iAnimPos;
		TInt iAlphaValue;
		};

/*	    Using this time delay class in order to allow animations to play in our draw.
		User::Wait does not allow the draw to occur (aparrently)
		Note when using this time-delay class: because other active objects can perform part of their
	    processing whilst we wait, wrapping calls to this in __UHEAP_MARK / __UHEAP_MARKEND
	    is likely to fail.  The data providers and animators are a major cause of this. 
*/
	class CActiveWait : public CActive
		{
	public:
		static CActiveWait* NewL();
		~CActiveWait();
		void Wait(TInt aDelay);
		// From CActive:
		void RunL();
		void DoCancel();
		TInt RunError(TInt aError);
	protected:
		CActiveWait();
		void ConstructL();
	protected:
		RTimer iTimer;
		TTime iFromTime;
		};

	CActiveWait* CActiveWait::NewL()
		{
		CActiveWait* self = new (ELeave) CActiveWait;
		CleanupStack::PushL(self);
		self->ConstructL();
		CleanupStack::Pop(self);
		return self;
		}
		
	void CActiveWait::ConstructL()
		{
		User::LeaveIfError(iTimer.CreateLocal());
		CActiveScheduler::Add(this);
		}
		
	CActiveWait::CActiveWait() : CActive(EPriorityNormal)
		{
		iFromTime.HomeTime();
		}

	CActiveWait::~CActiveWait()
		{
		Cancel();
		iTimer.Close();
		}

	void CActiveWait::DoCancel()
		{
		iTimer.Cancel();
		CActiveScheduler::Stop();
		}

	void CActiveWait::RunL()
		{
		CActiveScheduler::Stop();
		}
		
	TInt CActiveWait::RunError(TInt aError)
		{
		return aError; // exists so a break point can be placed on it.
		}

/*	    Note when using this : because other active objects can perform part of their
	    processing whilst we wait, wrapping calls to this in __UHEAP_MARK / __UHEAP_MARKEND
	    is likely to fail.  The data providers and animators are a major cause of this. 
*/
	void CActiveWait::Wait(TInt aDelay)
		{
		iTimer.After(iStatus, aDelay);
		SetActive();
		CActiveScheduler::Start();
		}
	CGraphicsContext::TDrawMode CCrpAnim::iEllipseDrawMode;

//
	}	//end anonymous local scope
//

/** This fn allocates an animation frame of the specified dimensions.
	Not tested outside the current limited parameter set (16/2/2007).
	Note the use of 32-bit integers for pixel/colour values. Using display mode lower than 24bpp may not produce correct results
	My attempt to write animation generating code that avoids CIclLoader and Decoder class.
	@param aDelayUs	 	the display time for the frame
	@param aImageType	Colour format for colour plane. 24MA currently not flagged correctly I expect.
	@param aMaskType	Format for mask. ENone for no mask.
	@param aImageSize	Width/height of bitmap area
	@param aImageOffset	Optional offset for bitmap area
	@param aTotalSize	Optional width/height of whole animation (I think)
	@return CFrame		filled in with allocated bitmaps. The get methods for the bitmaps return const type.
**/
static CWsGraphicBitmapAnimation::CFrame* NewFrameLC(TInt aDelayUs,TDisplayMode aImageType,TDisplayMode aMaskType,const TSize& aImageSize,const TPoint& aImageOffset=KPointZero,const TSize& aTotalSize=TSize(0,0))
	{
	TFrameInfo info;
	info.iFrameCoordsInPixels = TRect(aImageOffset,aImageSize);
	info.iFrameSizeInTwips = aImageSize;	  //this is zero in the gif loader
	info.iDelay = TTimeIntervalMicroSeconds(aDelayUs);
	info.iFlags = TFrameInfo::EColor|TFrameInfo::ELeaveInPlace|TFrameInfo::EUsesFrameSizeInPixels;
	if (aMaskType != ENone)
		{
		info.iFlags|=TFrameInfo::ETransparencyPossible;
		}
	if ((aTotalSize.iHeight > 0) && (aTotalSize.iWidth > 0))
		{
		// restrict the size of the frame to specified size of the animation
		info.iOverallSizeInPixels = aTotalSize;
		}
	else
		{
		// assign the size of the frame to the size of the entire bitmap area
		info.iOverallSizeInPixels = info.iFrameCoordsInPixels.iBr.AsSize();
		}
	info.iFrameDisplayMode = aImageType;
	info.iBackgroundColor = KRgbGreen;

	CWsGraphicBitmapAnimation::CFrame* frame = CWsGraphicBitmapAnimation::CFrame::NewL();
	CleanupStack::PushL(frame);
	frame->SetFrameInfo(info);
	CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
	frame->SetBitmap(bitmap); //takes ownership
	TSize frameInfoSize = info.iFrameCoordsInPixels.Size();
	User::LeaveIfError(bitmap->Create(frameInfoSize, aImageType));
	if((TFrameInfo::EAlphaChannel|TFrameInfo::ETransparencyPossible) & info.iFlags)
		{
		CFbsBitmap* mask = new(ELeave) CFbsBitmap;
		frame->SetMask(mask); //takes ownership
		User::LeaveIfError(mask->Create(frameInfoSize, aMaskType));
		}
	return frame;
	}

//
// function called back by TCleanupItem frameListCleanup from within CreateAnimFramesL(..) method
//
void CleanupFrameList(TAny* aPtr)
	{
	RPointerArray<CWsGraphicBitmapAnimation::CFrame>* ptrArray = STATIC_CAST(RPointerArray<CWsGraphicBitmapAnimation::CFrame>*, aPtr);
	ptrArray->ResetAndDestroy();
	ptrArray->Close();
	}

/** Helper function for making animation frames.
	//Called from CreateAnimL(...)
	@param aDelayUs		the delay between frames
	@param aNumFrames	number of frames (approx - image width is a factor)
	@param aImageType	colour format of colour data. This may not work properly for non-32-bit, but I haven't fully understood TBitmapUtil documentation.
	@param aMaskType	format for mask - ENone for no mask.
	@param aImageSize	width/height of animation
	@param aBgCol		background colour for image non-masked areas. Masked areas are black.
	@param aFgCol		foreground colour of animating area
	@param aFrames		frames that the animation is constructed from
**/
static void CreateAnimFramesL(TInt aDelayUs,TInt aNumFrames,TDisplayMode aImageType,TDisplayMode aMaskType,TSize aImageSize,TRgb aBgCol,TRgb aFgCol, RPointerArray<CWsGraphicBitmapAnimation::CFrame>& aFrames)
	{
	const TInt 	animWH = aImageSize.iWidth;
	const TInt	animStep = Max(1,animWH/Max(1,aNumFrames));	//note this intentionally rounds down to avoid overflows
	for (TInt ii = 0 ; ii < animWH ; ii += animStep)
		{
		CWsGraphicBitmapAnimation::CFrame* frame = NewFrameLC(aDelayUs,aImageType,aMaskType,aImageSize,KPointZero,aImageSize);
		aFrames.AppendL(frame);
		CleanupStack::Pop(frame);
		TBitmapUtil utilMask(CONST_CAST(CFbsBitmap*, frame->Mask()));
		TBitmapUtil utilCol(CONST_CAST(CFbsBitmap*, frame->Bitmap()));
		utilCol.Begin(KPointZero);
		
		// cycle through the frame's actual bitmap & assign each pixel a value identical to the specified colours
		TUint32 colback=aBgCol.Internal();
		TUint32 colfront=aFgCol.Internal();
		TInt row = KErrNone;
		TInt col = KErrNone;
		for (row = 0 ; row < aImageSize.iHeight ; row++)
			{
			utilCol.SetPos(TPoint(0, row));
			for (col = 0 ; col < aImageSize.iWidth ; col++)
				{
				utilCol.SetPixel(colback);	
				utilCol.IncXPos();
				}
			utilCol.SetPos(TPoint(ii, row));
			for (col = 0 ; col < animStep ; col++)	  //Note I rely on intentional rounding down here!
				{
				utilCol.SetPixel(colfront);	
				utilCol.IncXPos();
				}			
			}
		
		if (aMaskType)
			{
			// cycle through each pixel of the frame's mask & assign a default pixel a colour value
			utilMask.Begin(KPointZero);
			for (row = 0 ; row < aImageSize.iHeight ; row++)
				{
				utilMask.SetPos(TPoint(0,row));
				for (col = 0 ; col < aImageSize.iWidth ; col++)
					{
					utilMask.SetPixel(KWhitePixels);
					utilMask.IncXPos();
					}
				}
			
			const TInt maxmaskWidth = Min(8,Max(animWH/3,2));

			//cut the corners off the mask
			for (row = 0 ; row < maxmaskWidth ; row++)
				{
				TInt currentX = maxmaskWidth - row;
				TInt xPos = KErrNone;

				utilCol.SetPos(TPoint(0,row));
				utilMask.SetPos(TPoint(0,row));
				for(xPos = currentX ; xPos >= 0 ; xPos--)
					{
					utilCol.SetPixel(KBlackPixels);	
					utilCol.IncXPos();
					utilMask.SetPixel(KBlackPixels);	
					utilMask.IncXPos();
					}

				utilCol.SetPos(TPoint(animWH - 1, row));
				utilMask.SetPos(TPoint(animWH - 1, row));
				for(xPos = currentX ; xPos >= 0 ; xPos--)
					{
					utilCol.SetPixel(KBlackPixels);	
					utilCol.DecXPos();
					utilMask.SetPixel(KBlackPixels);	
					utilMask.DecXPos();
					}

				utilCol.SetPos(TPoint(0, animWH - 1 - row));
				utilMask.SetPos(TPoint(0, animWH - 1 - row));
				for(xPos = currentX ; xPos >= 0 ; xPos--)
					{
					utilCol.SetPixel(KBlackPixels);	
					utilCol.IncXPos();
					utilMask.SetPixel(KBlackPixels);	
					utilMask.IncXPos();
					}

				utilCol.SetPos(TPoint(animWH - 1, animWH - 1 - row));
				utilMask.SetPos(TPoint(animWH - 1, animWH - 1 - row));
				for(xPos = currentX ; xPos >= 0 ; xPos--)
					{
					utilCol.SetPixel(KBlackPixels);	
					utilCol.DecXPos();
					utilMask.SetPixel(KBlackPixels);	
					utilMask.DecXPos();
					}
				}
			utilMask.End();
			}
		utilCol.End();
		}
	}

/** My attempt to write animation generating code that avoids CIclLoader and Decoder class.
	//It is better if this test class used it's own generated animation
	//rather than relying on the GIF loader in order to reduce the cross-dependencies.
	//The animation generated is a simple vertical line moving from left to right.
	//To prove the masking, I cut the corners off.
	@param aDelayUs		the delay between frames
	@param aNumFrames	number of frames (approx - image width is a factor)
	@param aImageType	colour format of colour data. This may not work properly for non-32-bit, but I haven't fully understood TBitmapUtil documentation.
	@param aMaskType	format for mask - ENone for no mask.
	@param aImageSize	width/height of animation
	@param aBgCol		background colour for image non-masked areas. Masked areas are black.
	@param aFgCol		foreground colour of animating area
	@param aTUid		TUid assigned to animation
	@return CWsGraphicBitmapAnimation allocated to represent the final animation	
**/
static CWsGraphicBitmapAnimation* CreateAnimL(TInt aDelayUs,TInt aNumFrames,TDisplayMode aImageType,TDisplayMode aMaskType,TSize aImageSize,TRgb aBgCol,TRgb aFgCol,TUid& aTUid)
	{
	RPointerArray<CWsGraphicBitmapAnimation::CFrame> frames;
	TCleanupItem frameListCleanup(CleanupFrameList, &frames);
	CleanupStack::PushL(frameListCleanup);
	
	CreateAnimFramesL(aDelayUs, aNumFrames, aImageType, aMaskType, aImageSize,aBgCol, aFgCol, frames);
	
	CWsGraphicBitmapAnimation* anim = CWsGraphicBitmapAnimation::NewL(aTUid,frames.Array());
	CleanupStack::PopAndDestroy(&frames);
	return anim;
	}

//
// Describes the pure colour of the RGB value. yellow/magenta/cyan set 2 bits. White/grey is seperately flagged.
// This method attempts to determine the strongest primary colour present in any given pixel. 
// Note: The algorithm used is known to work for the current test cases only but requires careful review
// for anyone making additional changes to tcrpanim. Given time, improved algorithm should be developed
// to replace the current one
//
TUint PredominantColour(TUint aCol)
	{	 //I don't like all these ifs, but I don't see an easy alternative
		 //Possibly a bit look-up of the deltas from average would work 
		 //(ignoring the bottom 5 bits =32, not 0x30=48. Ignore bottom 4 bits and accept 3-same answers, or divide by delta?)
		 //
	const TInt Kdelta=0x30;
	TInt red=(aCol&0x00ff0000)>>16;
	TInt green=(aCol&0x0000ff00)>>8;
	TInt blue=(aCol&0x000000ff);
	TInt ave=((red+green+blue)*(65536/3))>>16;
	TBool rOverA=(red>ave);
	TBool gOverA=(green>ave);
	TBool bOverA=(blue>ave);
	TInt numOverAve=(rOverA?1:0)+(gOverA?1:0)+(bOverA?1:0);

	if (numOverAve==1)
		{
		if (rOverA)
			{
			if (red>ave+Kdelta)
				{
				if ((green-blue)>-Kdelta && (green-blue)<Kdelta)
					return  EDetRed;
				}
			else
				{
				if (ave<Kdelta)
					return EDetBlack;
				else
					{
					if (green>ave-Kdelta && blue>ave-Kdelta)
						{
						if (ave>256-Kdelta)
							return EDetWhite;
						else
							return EDetGrey;
						}
					}
				}
			}
				
		if (gOverA)
			{
			if (green>ave+Kdelta)
				{
				if ((blue-red)>-Kdelta && (blue-red)<Kdelta)
					return  EDetGreen;
				}
			else
				{
				if (ave<Kdelta)
					return EDetBlack;
				else
					{
					if (red>ave-Kdelta && blue>ave-Kdelta)
						if (ave>256-Kdelta)
							return EDetWhite;
						else
							return EDetGrey;
					}
				}
			}

		if (bOverA)
			{
			if (blue>ave+Kdelta)
				{
				if ((green-red)>-Kdelta && (green-red)<Kdelta)
					return  EDetBlue;
				}
			else
				{
				if (ave<Kdelta)
					return EDetBlack;
				else
					{
					if (red>ave-Kdelta && green>ave-Kdelta)
						if (ave>256-Kdelta)
							return EDetWhite;
						else
							return EDetGrey;
					}
				}
			}
		}
	else	
		{
		if (!rOverA)
			 if (red<ave-Kdelta)
			 	{
			 	if ((green-blue)>-Kdelta && (green-blue)<Kdelta)
			 		return EDetGreen|EDetBlue;
			 	}
			 else
				{
				if (ave>256-Kdelta)
					return EDetWhite;
				else
					{
					if (blue<ave+Kdelta && green<ave+Kdelta)
						{
						if (ave<Kdelta)
							return EDetBlack;
						else
							return EDetGrey;
						}
					}
				}

		if (!gOverA)
			{
			if (green<ave-Kdelta)
				{
				if ((blue-red)>-Kdelta && (blue-red)<Kdelta)
					return  EDetRed|EDetBlue;
				}
			 else
				{
				if (ave>256-Kdelta)
					return EDetWhite;
				else
					{
					if (blue<ave+Kdelta && red<ave+Kdelta)
						if (ave<Kdelta)
							return EDetBlack;
						else
							return EDetGrey;
					}
				}
			}

		if (!bOverA)
			{
			if (blue<ave-Kdelta)
				{
				if ((green-red)>-Kdelta && (green-red)<Kdelta)
					return  EDetGreen|EDetRed;
				}
			 else
				{
				if (ave>256-Kdelta)
					return EDetWhite;
				else
					{
					if (red<ave+Kdelta && green<ave+Kdelta)
						if (ave<Kdelta)
							return EDetBlack;
						else
							return EDetGrey;
					}
				}
			}
		}
	return ECantTell;
	}

/**
	Helper fn to ensure I put the anims in the same place each time...
**/
void CalcCentredAnimPosition(TRect& aRect,const TSize& aWinSize)
	{
	aRect.Shrink(aWinSize.iWidth*3/8,aWinSize.iHeight*4/10);
	}

CTCrpAnim::CTCrpAnim(CTestStep* aStep) : 
	CTWsGraphicsBase(aStep)
	{
	}

void CTCrpAnim::ConstructL()
	{
	TheClient->iGroup->WinTreeNode()->SetOrdinalPosition(0);
	iRedrawWin=new(ELeave) CCrpAnim(EFalse, CCrpAnim::ERedraw);
	iBaseWin=new(ELeave) CCrpAnim(EFalse, CCrpAnim::ERedraw);
	iOverWin=new(ELeave) CCrpAnim(EFalse, CCrpAnim::ERedraw);

	TSize screenSize=TheClient->iGroup->Size();
	TInt winWidth=(screenSize.iWidth/3)-10;
	TInt winHeight=screenSize.iHeight-10;
	TSize windowSize(winWidth,winHeight);

	iRedrawWin->ConstructL(TPoint(screenSize.iWidth/3*2+5,5), windowSize);
	iBaseWin->ConstructL(TPoint(screenSize.iWidth/3+5,5), windowSize);
	
	//Create a transparent window that exactly overlaps the test window
	//If transparency is not supported the leave causes the window to be destroyed and set to NULL.
	//There is a test for transparency supported, but that simply creates a temp window to test anyway... 
	
	//Note that when I originally wrote this test to fix PDEF101991, it generated white areas that I detected.
	//However, if this transparent window used for extended tests is created over the test window,
	//that somehow stops the white fill from occurring.
	//The fault still occurs, but the previous screen contents are left behind.
	//So now this window is created at an off-screen location.
	TRAPD(err, iOverWin->ConstructL(KPointOffsite, windowSize, 0x80); iOverWin->SetBlankIt(ETrue); iOverWin->SetRepeatDrawMax(KMaxRepeatDraw););
	if (err)
		{
		delete iOverWin;
		iOverWin = NULL;
		}
	
	iTestWin = iRedrawWin;
	iTestWin->SetRepeatDrawMax(KMaxRepeatDraw);
	iBaseWin->SetRepeatDrawMax(KMaxRepeatDraw);

	// create animation object & share it with everyone
	iAnim = CreateAnimL(KAnimationFrameDelayTime,KAnimationTotalFrames,KTestDisplayMode,EGray256,TSize(KAnimDimension, KAnimDimension),KRgbBlue,KRgbRed,KUidTestAnimation2);
	if (!iAnim)
		{
		User::Leave(KErrNoMemory);
		}
	iAnim->ShareGlobally();
	
	// calculate minimum length of the red line
	const TInt maxmaskHeight = Min(8, Max(KAnimDimension/3,2)); // note this calculation mimics that for the size of the corners cut from the mask in CreateAnimL above
	iMinimumCalcRedLine = KAnimDimension - maxmaskHeight*2; // the height of the image minus the two cut corners

	// create the timer object
	iWaiter = CActiveWait::NewL();

	// create screen bitmap object & scanline buffer
	iScreenBitmap = new (ELeave) CFbsBitmap;
	User::LeaveIfError(iScreenBitmap->Create(TSize(KAnimDimension, KAnimDimension), KTestDisplayMode));
	TInt bufLength = iScreenBitmap->ScanLineLength(windowSize.iHeight, KTestDisplayMode);
	iScanlineBuf = HBufC8::NewL(bufLength);
	
	#ifdef RUN_SAMPLE_ON_LEFT
    	{
		// play animation on iBaseWin window
		iBaseWin->SetAnimation(KUidTestAnimation2)->Play(ETrue);
		TSize subsize1 = iTestWin->BaseWin()->Size();
		TRect subposition1(subsize1);
		CalcCentredAnimPosition(subposition1, subsize1);
		iBaseWin->SetPosAnimation(KUidTestAnimation2, subposition1);
		iBaseWin->InvalidateAndRedraw(ETrue,EFalse,ETrue);
    	}
	#endif
	}

CTCrpAnim::~CTCrpAnim()
	{
	delete iRedrawWin;
	delete iBaseWin;
	delete iOverWin;
	if (iAnim)
		{
		// destroy the animation object
		iAnim->UnShareGlobally();
		iAnim->Destroy();
		delete iAnim;				
		iAnim = NULL;
		}
	if (iWaiter)
		{
		// destroy the timer object
		delete iWaiter;
		iWaiter = NULL;
		}
	if (iScreenBitmap)
		{
		// destroy the screen capture of the animation
		delete iScreenBitmap;
		iScreenBitmap = NULL;
		}
	if (iScanlineBuf)
		{
		// destroy the scanline buffer
		delete iScanlineBuf;
		iScanlineBuf = NULL;
		}
	User::After(200000);
	}

//
// This method checks the animation contained in the aAnimWin window has progressed. That is 
// that it's drawn a sufficient number of concurrent frames to screen & the animation is 
// drawn properly to screen
// returns a Bool identifying whether the animation is considered 'good' or not
//  
void CTCrpAnim::CheckAnimProgressedL(CAnonAnimWindow* aAnimWin, TInt aAdditionalFrameCount, TBool aCaptureFrameResult)
	{
	TBool goodAnimation = ETrue;

	// retrieve the rect from the screen's bitmap that contains the animation
	CWsScreenDevice* screen = TheClient->iScreen;
	TRect animPos = *aAnimWin->GetPosAnimation(KUidTestAnimation2);
	CTBaseWin* bWin = aAnimWin->CtBaseWin();
	animPos.Move(bWin->Position());
	User::LeaveIfError(screen->CopyScreenToBitmap(iScreenBitmap, animPos));
	
	TInt frameNum = DetermineApproxFrameNum(iScreenBitmap, aCaptureFrameResult); // determines the frame Number & checks quality of animation (no tearing, etc)
	TBool frameIdentified=(frameNum>=0);
	
	if (aCaptureFrameResult)
		{
		if (frameIdentified)
			{
			if (iPreviousFrameNum != KErrNotFound)
			 	{
				if (iPreviousFrameNum < frameNum)
					{
					TInt frameStep = KFrameStepCalculation * aAdditionalFrameCount;
					iPreviousFrameNum += frameStep; // move to our *expected* framenumber
					if (frameNum > iPreviousFrameNum)
						{
						// the frame number is ahead of it's expected position
						// This suggests we've possibly missed animating a frame in wserv
						// or test code isn't getting a chance to execute as crp animations taking all cpu cycles
						// If its significantly outside norms, we log the fact (as a performance metric)
						TInt performance = ((frameNum - iPreviousFrameNum) / frameStep);
						if (performance > KFrameMissedAnimationsThreshold)
							{
							iFrameStatus.iFrameSkipped++;
							goodAnimation = EFalse;	
							}	
						}
					// else we're animating above an acceptable threshold
					}
				else if (iPreviousFrameNum == frameNum) // potentially not animating anymore
					{
					iFrameStatus.iFrameIdentical++;
					goodAnimation = EFalse;
					}
				// else animation is progressing fine
				}
			// ignore iPreviousFrameNum == KErrNotFound
			}
		else
			{
			goodAnimation = EFalse; // couldn't id the red line	
			}

		if (goodAnimation)
			{
			iFrameStatus.iFrameOK++;
			}
		}
	// else we were only interested in calculating the frameNum
	iPreviousFrameNum = frameNum;
	}

//
// method to estimate the framenumber based on the location of the thin, red line. 
// Also checks whether tearing of the animation has occured or the animation
// is only partially drawn. 
// These are known issues with wserv animation performance & so we give some allowance for error
// 
TInt CTCrpAnim::DetermineApproxFrameNum(CFbsBitmap* aBitmap, TBool aCaptureFrameResult)
	{
	TInt colFirstTear = KErrNotFound; 	// column id'ing the first tear in the vertical line
	TPtr8 des = iScanlineBuf->Des();	// ptr to the scanline buffer

	// locate the thin, red line in the bitmap
	for (TInt xPos = 0 ; xPos < aBitmap->SizeInPixels().iWidth ; xPos++)
		{
		aBitmap->GetVerticalScanLine(des, xPos, EColor16MA);
		TUint32* pixel = (TUint32*) des.Ptr();
		TInt colour = KErrNone;
		
		for (TInt ii = 0 ; ii < aBitmap->SizeInPixels().iHeight ; ii++)
			{
			colour = PredominantColour(*pixel);
			if (colour & EDetRed)
				{
				if (colFirstTear < 0)
					{
					// check the length of the red line is a good length
					pixel += (iMinimumCalcRedLine - 1); // minus the one pixel to position on last pixel in red line
					colour = PredominantColour(*pixel);
					if (colour & EDetRed)
						{
						// good line
						return xPos;
						}
					else // we've detected first part of a torn line
						{
						colFirstTear = xPos; 
						}
					}
				else
					{
					// located second part of torn line
					if ((xPos - colFirstTear) > KAnimationTearWidthThreshold)
						{
						if (aCaptureFrameResult)
							{
							iFrameStatus.iFrameTearing++;	
							}
						xPos = KErrNotFound;
						}
					return xPos;
					}	
				break;
				}
			pixel++;
			}
		}
	if (aCaptureFrameResult)
		{
		if (colFirstTear < 0)
			{
			iFrameStatus.iFrameEmpty++; // we never located any red line at all
			}
		else
			{
			iFrameStatus.iFramePartial++; // we only located a single, small part of the red line
			}
		}
	return KErrNotFound;
	}

/**	This internal loop tests that the animation and the foreground interact correctly
	The primary test is that the outline of the animation 
	intersects the lines drawn on the foreground correctly, compared to a reference version.
	The iBaseWin is already showing this reference anim.
	If the animation is not drawn, or the foreground is wiped, then this test will fail.
**/
void CTCrpAnim::TestSpriteLoopL(TBool aAnimForeground,TBool aDrawForeground)
	{
	_LIT(KForegroundInfo,"TestSpriteLoop animForeground [%d] drawForeground [%d]");
	INFO_PRINTF3(KForegroundInfo, aAnimForeground, aDrawForeground);
	
	if (!iOverWin && (aAnimForeground || aDrawForeground))
		{
		User::Leave(KErrGeneral); // unable to run this test without iOverWin
		}

	ResetFrameCounters();
	iTestWin->RemoveAnimation(KUidTestAnimation2);
	iTestWin->SetBlankIt(ETrue);		
	if (iOverWin)
		{
		iOverWin->RemoveAnimation(KUidTestAnimation2);
		iOverWin->SetBlankIt(ETrue);
		}

	// determine which window holds the animation, & which will be invalidated with progressively larger rects
	CCrpAnim* animWin=aAnimForeground?iOverWin:iTestWin;
	CCrpAnim* paintWin=aDrawForeground?iOverWin:iTestWin;
	paintWin->SetBlankIt(EFalse);
	
	// set & play the animation on the specified window (animWin)
	animWin->SetAnimation(KUidTestAnimation2)->Play(ETrue);
	TSize subsize1 = paintWin->BaseWin()->Size();
	TRect subposition1(subsize1);
	CalcCentredAnimPosition(subposition1, subsize1);
	animWin->SetPosAnimation(KUidTestAnimation2, subposition1);
	
	#ifdef RUN_SAMPLE_ON_LEFT
		// play the demo animation in the left-hand window also
		iBaseWin->InvalidateAndRedraw(ETrue, EFalse, ETrue);
	#endif

	iTestWin->InvalidateAndRedraw(ETrue,EFalse,ETrue);
	if (iOverWin)
		{
		iOverWin->InvalidateAndRedraw(ETrue,EFalse,ETrue);
		}
	
	// invalidate increasingly larger squares on paintWin 
	// note, some fully overlap the animation, some partially overlap, and some don't overlap at all
	TInt invalidateWaitTime=KAnimationFrameDelayTime*3/4; // microseconds
	TInt temp = KErrNotFound;
	for (TInt step=30;step<KMaxXY;step+=30)
		{
		for (TInt xx=0;xx<KMaxXY;xx+=step)
			{
			for (TInt yy=10;yy<KMaxXY;yy+=step)
	    		{
				// calculate rectangle & invalidate paintWin with it
	    		TRect invalidRect(xx,yy,xx+step,yy+step);
	    		paintWin->InvalidateAndRedraw(ETrue,EFalse,ETrue,&invalidRect);
				
				// calculate any additional frames that may be drawn by above. Note intentionally ignore frame result
				temp = iPreviousFrameNum;	
				CheckAnimProgressedL(animWin, 1, EFalse);
				
				//new defect DEF101896: Test runs faster with this line removed, but there is evident tearing
				iWaiter->Wait(invalidateWaitTime);			//DEF101896 search string: //interrupt_foreground_draw
				
				if (temp == iPreviousFrameNum)
					{
					// give wserv more time to animate the frame
					iWaiter->Wait(invalidateWaitTime);
					}
				CheckAnimProgressedL(animWin, 1); // calculate the frame drawn. Capture frame result
				}
			}
		}

	// determine whether the animation was successful (ie: enough Good frames were detected) or not
	// Note KMinGoodFrameThreshold is essentially an arbitrary number. This can be adjusted to accommodate
	// performance requirements as needed
	temp = LogResults();
	TInt quality = 100*iFrameStatus.iFrameOK/temp;
	TEST(quality > KMinGoodFrameThreshold);
	
	ResetFrameCounters();
	iWaiter->Cancel();
	iTestWin->RemoveAnimation(KUidTestAnimation2);
	iTestWin->SetBlankIt(ETrue);		
	if (iOverWin)
		{
		iOverWin->RemoveAnimation(KUidTestAnimation2);
		iOverWin->SetBlankIt(ETrue);
		}
	}

//
// resets the frame trackers to intial values
//
void CTCrpAnim::ResetFrameCounters()
	{
	iPreviousFrameNum = KErrNotFound;
	iFrameStatus.iFrameOK = 0;
	iFrameStatus.iFramePartial = 0;
	iFrameStatus.iFrameIdentical = 0;
	iFrameStatus.iFrameEmpty = 0;
	iFrameStatus.iFrameTearing = 0;
	iFrameStatus.iFrameSkipped = 0;
	}

//
// Log the current frame results & return the total number of frame calculations
//
// Calculated : the total number of frame-checks run
// Good: 		the frame was successfully drawn to screen & within specified tolerances for tearing, expected position & colour
// Partial: 	the frame was only partially drawn to screen. Specifcally the animated red line was only partially drawn
// Identical: 	the frame was in the same position as the last frame
// Empty: 		no redline was detected at all in the frame
// Skipped: 	the position of the frame was beyond the expected position
//
// There is a dependency on the timing as to when the frame is animated hence tolerances are used to allow
// for this. 
// 
TInt CTCrpAnim::LogResults()
	{
	TInt result = iFrameStatus.iFrameOK + iFrameStatus.iFramePartial + iFrameStatus.iFrameIdentical + 
				iFrameStatus.iFrameEmpty + iFrameStatus.iFrameTearing + iFrameStatus.iFrameSkipped;
	INFO_PRINTF4(_L("\tAnimation results:  Calculated[%d], Good[%d], Partial[%d]"), result, iFrameStatus.iFrameOK, iFrameStatus.iFramePartial);
	INFO_PRINTF5(_L("\tAnimation results:  Identical[%d], Empty[%d], Tearing[%d], Skipped[%d]"), iFrameStatus.iFrameIdentical, iFrameStatus.iFrameEmpty, iFrameStatus.iFrameTearing, iFrameStatus.iFrameSkipped);
	return result;
	}

/** This test tests the result of drawing an animation and main draw to two windows that overlap.
	The two windows are placed in exactly the same location, so the result of splitting the drawing across them should be "identical".
	Note that when the anim and the draw are on different screens the lines are seen merged over the anim.
**/
void CTCrpAnim::TestOverlappingWindowsL()
	{
	if (!iOverWin)
		{
		INFO_PRINTF1(_L("- Test skipped - transparency not supported"));
		return;
		}
		
	// setup necessary params
	// Note we place the overlapping transparent window (iOverWin) directly on top of the test window (iTestWin)
	iOverWin->BaseWin()->SetPosition(iTestWin->BaseWin()->Position());
		
	enum 
		{
		 EAllBackground=0,
		 EForegroundDraw=1,
		 EForegroundAnim=2,
		 EAllForeGround=3,
		 ECountModes,
		 EFirstMode=EAllBackground,
		};
	
	// test the various permutations of overlapping vs animated windows
	for (TInt mode = EFirstMode ; mode < ECountModes ; mode++)	    
		{
		INFO_PRINTF2(_L("TestOverlappingWindowsL [%d]"), mode);
		TestSpriteLoopL((mode&EForegroundAnim)!=0,(mode&EForegroundDraw)!=0);
		}
	}
	
/**
	This method demonstrates clipping of an animation running behind a transparent window.
	No main window redraw takes place here.
**/	
void CTCrpAnim::DemoClippingWindowsL()
	{
	if (!iOverWin)
		{
		INFO_PRINTF1(_L("- Test skipped - transparency not supported"));
		return;
		}

	// setup test case params. Note we calculate three different positions for the overlapping window
	RWindow* win = iTestWin->Window();
	
	TPoint	screenPos= win->Position();
	TSize screenSize = win->Size();	
	TRect subposition1(screenSize);
	CalcCentredAnimPosition(subposition1, screenSize);

	TPoint testPositions[]=
		{	
		//first test: window clips corner of anim
			TPoint(screenPos.iX+screenSize.iWidth/2-10,screenPos.iY+screenSize.iHeight/2-10),
		//test: window clips all of anim
			TPoint(screenPos.iX+screenSize.iWidth/3,screenPos.iY+screenSize.iHeight/3),
		//test: window clips none of anim
			TPoint(screenPos.iX+screenSize.iWidth*2/3,screenPos.iY+screenSize.iHeight*2/3),
		};

	// calculate roughly number of frames we expect to have drawn
	TInt loopWaitTime = KShortDelayLoop; // time given to allow animation to progress (arbitrary number)
	float expectedFrameCount = 1;
	if (loopWaitTime > KAnimationFrameDelayTime)
		{
		expectedFrameCount = loopWaitTime/KAnimationFrameDelayTime;
		}

	for (TInt ii = 0; ii < ((sizeof testPositions)/(sizeof testPositions[0])) ; ii++)
		{
		// initialise test windows to known state with no active animations
		ResetFrameCounters();
		iTestWin->RemoveAnimation(KUidTestAnimation2);
		iTestWin->SetBlankIt(EFalse);		
		iOverWin->SetBlankIt(ETrue);		
		iOverWin->RemoveAnimation(KUidTestAnimation2);

		// position animation windows
		iTestWin->SetAnimation(KUidTestAnimation2)->Play(ETrue);
		iTestWin->SetPosAnimation(KUidTestAnimation2, subposition1);
		iOverWin->BaseWin()->SetPosition(testPositions[ii]); // positions the transparent overlapping window

		// redraw both test windows
		iTestWin->InvalidateAndRedraw(ETrue,EFalse,ETrue);
		iOverWin->InvalidateAndRedraw(ETrue,EFalse,ETrue);

		// run the animation for an arbitrary period
		for (TInt loopit = 0 ; loopit < 20 ; loopit++)
			{
			iWaiter->Wait(loopWaitTime);	
			CheckAnimProgressedL(iTestWin,static_cast<TInt>(expectedFrameCount)); // log the frame result
			}

		// calculate & log frame results. Test an acceptable number of frames were successfully animated
		TInt total = LogResults();
		TInt qA = 100*iFrameStatus.iFrameOK/total;
		TEST(qA > KMinGoodFrameThreshold);
		}
	}

/**	In this version, the background window is updated in patches. 
	If the animation intersects the transparent window then the whole transparent window is redrawn.
**/	
void CTCrpAnim::TestClippingWindowsL()
	{
	if (!iOverWin)
		{
		INFO_PRINTF1(_L("- Test skipped - transparency not supported"));
		return;
		}
	// setup test case params. Note we calculate three different positions for the overlapping window
	RWindow* win = iTestWin->Window();
	TPoint screenPos= win->Position();
	TSize screenSize = win->Size();	
	
	TPoint testPositions[]=
		{	
		//first test: window clips corner of anim
			TPoint(screenPos.iX+screenSize.iWidth/2-10,screenPos.iY+screenSize.iHeight/2-10),
		//test: window clips all of anim
			TPoint(screenPos.iX+screenSize.iWidth/3,screenPos.iY+screenSize.iHeight/3),
		//test: window clips none of anim
			TPoint(screenPos.iX+screenSize.iWidth*2/3,screenPos.iY+screenSize.iHeight*2/3),
		};

	for (TInt loopIt = 0; loopIt < ((sizeof testPositions)/(sizeof testPositions[0])) ; loopIt++)
		{
	    iOverWin->BaseWin()->SetPosition(testPositions[loopIt]); // position the overlapping window
	    TestSpriteLoopL(EFalse,EFalse);
		}
	}

/**	This just demonstrates that an animation plays - for about 1 second.
**/
void CTCrpAnim::BasicCRPDemo()
	{
	// draw the animation in two positions
	TSize subsize1 = iTestWin->BaseWin()->Size();
	TRect subposition1(subsize1);
	CalcCentredAnimPosition(subposition1, subsize1);
	
	if (iOverWin)
		{
		iOverWin->BaseWin()->SetPosition(KPointOffsite);	//way away!
		iOverWin->InvalidateAndRedraw(EFalse,EFalse,ETrue);
		}
		
	CCrpAnim *animWin= iTestWin;
	animWin->SetAnimation(KUidTestAnimation2)->Play(ETrue);
	animWin->SetPosAnimation(KUidTestAnimation2, subposition1);
	iTestWin->InvalidateAndRedraw(ETrue,EFalse,ETrue);
	iBaseWin->InvalidateAndRedraw(ETrue,EFalse,ETrue);

	// allow the animation to play for ~1 second. Purpose is to demonstrate animation to an observer	
	iWaiter->Wait(KShortDelayLoop);
	
	ResetFrameCounters();
	iWaiter->Cancel();
	iTestWin->RemoveAnimation(KUidTestAnimation2);
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-CRP01-0001

@SYMDEF             DEF100356

@SYMTestCaseDesc    CRP animation test for redraw storing interrupting main draw

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates a CRP animation and runs it on the server scheduler 
					while also running redraws of the window.
					
					With Redraw storing this has been known to cause problems 
					sharing and resetting the window iDisplayRegion.
					This is evidenced by white areas.


@SYMTestExpectedResults 
					The LHS window shows what the animation should look like just animating,
					while the RHS window demonstrates the simultanious animation and redraw.
					No White patches should be in evidence, and no missing fragments of animation.
					The TEST should detect white patches.
*/
void CTCrpAnim::TestSpriteInterruptsForegroundL()
	{	
	// setup test params
	TSize subsize1(iTestWin->BaseWin()->Size());
	TRect subposition1(subsize1);
	CalcCentredAnimPosition(subposition1, subsize1);
	if (iOverWin)
		{
		iOverWin->BaseWin()->SetPosition(KPointOffsite);	// ensure overlapping transparent window DOESN'T overlap the test window
		}

	// execute test loop
	TestSpriteLoopL(EFalse,EFalse);
	}

void CTCrpAnim::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(KTest1,"1: Basic CRP demo");
	_LIT(KTest2,"2: sprite anim interrupts foreground");
	_LIT(KTest3,"3: translucent windows");
	_LIT(KTest4,"4: CRP clipping windows");
	_LIT(KTest5,"5: CRP & redraw clipping windows");
	_LIT(KTest6,"6: CRP Invalidation");

	((CTCrpAnimStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
		case 1:
/**
@SYMTestCaseID		GRAPHICS-WSERV-CRP01-0002
*/
			((CTCrpAnimStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-CRP01-0002"));
			iTest->LogSubTest(KTest1);
			BasicCRPDemo();
			break;
		case 2:
			((CTCrpAnimStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-CRP01-0001"));
			iTest->LogSubTest(KTest2);
			TestSpriteInterruptsForegroundL();
			break;
		case 3:
/**
@SYMTestCaseID		GRAPHICS-WSERV-CRP01-0003
*/
			((CTCrpAnimStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-CRP01-0003"));
			iTest->LogSubTest(KTest3);
			TestOverlappingWindowsL();
			break;
		case 4:
/**
@SYMTestCaseID		GRAPHICS-WSERV-CRP01-0004
*/
			((CTCrpAnimStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-CRP01-0004"));
			iTest->LogSubTest(KTest4);
			DemoClippingWindowsL();
			break;
		case 5:
/**
@SYMTestCaseID		GRAPHICS-WSERV-CRP01-0005
*/
			((CTCrpAnimStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-CRP01-0005"));
			iTest->LogSubTest(KTest5);
			TestClippingWindowsL();
			break;
		case 6:
/**
@SYMTestCaseID		GRAPHICS-WSERV-CRP01-0006
*/
			((CTCrpAnimStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-CRP01-0006"));
			iTest->LogSubTest(KTest6);
			//this testcase is removed, because invalidation is removed from CWsGraphicDrawer destructor (due to flickering)
			break;
		default:
			((CTCrpAnimStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTCrpAnimStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
		}
	((CTCrpAnimStep*)iStep)->RecordTestResultL();
	}

//
namespace	//anonymous namespace
	{
//
	CAnimRedrawWindow::CAnimRedrawWindow(CCrpAnim *aAnimWindow, TBool aIsBase) : CTWin(),
		iAnimWindow(aAnimWindow),
		iIsBase(aIsBase)
		{
		}

	CAnimRedrawWindow::~CAnimRedrawWindow()
		{
		}

	void CAnimRedrawWindow::Draw()
		{
		CCrpAnim::Draw(Gc(),Size(),iIsBase,iAnimWindow->iRect,iAnimWindow->iBlankIt,iAnimWindow->iRepeatDrawMax,iAnimWindow->iAlphaValue);
		if (iAnimWindow->iAnimUid!=TUid::Null())
			{
			TheClient->Flush();
			Gc()->DrawWsGraphic(iAnimWindow->iAnimUid,iAnimWindow->iAnimPos,iAnimWindow->iAnimData.Pckg());
			TheClient->Flush();
			}
		}

	//

	CCrpAnim::CCrpAnim(TBool aIsBase, TWinType aWinType) 
	:	iWinType(aWinType), 
		iIsBase(aIsBase),
		iBlankIt(EFalse),
		iRepeatDrawMax(1),
		iAnimUid(TUid::Null()),
		iAlphaValue(ENoTransparency)
		{
		}

	CCrpAnim::~CCrpAnim()
		{
		delete iCtWin;
		}

	void CCrpAnim::ConstructL(const TPoint &aPos, const TSize &aSize, TInt aAlphaValue)
		{
		TDisplayMode reqMode = EColor16MA; //for transparency we need 16ma or 16map mode
		TDisplayMode *pReqMode=&reqMode;	
		switch(iWinType)
			{
			case ERedraw:
				iCtWin = new(ELeave) CAnimRedrawWindow(this, iIsBase);
				break;
			case EBlank:
				iCtWin = new(ELeave) CTBlankWindow();
				break;
			case EBackedUp:
				iCtWin = new(ELeave) CTBackedUpWin(EColor64K);
				pReqMode = NULL;
				break;
			}
		iCtWin->SetUpL(aPos, aSize, TheClient->iGroup, *TheClient->iGc, pReqMode, ETrue);
		if (aAlphaValue != ENoTransparency)
			{
			User::LeaveIfError(Window()->SetTransparencyAlphaChannel());
			//the window itself should be completely transparent, the draw commands will use the alpha value
			Window()->SetBackgroundColor(TRgb(0, 0, 0, 0));
			iAlphaValue = aAlphaValue;
			}
		}

	void CCrpAnim::SetEllipseDrawMode(CGraphicsContext::TDrawMode aEllipseDrawMode)
		{
		iEllipseDrawMode = aEllipseDrawMode;
		}

	void CCrpAnim::DrawEllipse(CBitmapContext *aGc, const TRect &aRect, TInt aAlphaValue)
		{
		if(aAlphaValue != ENoTransparency)
			{
			aGc->SetBrushColor(TRgb(85,85,85, aAlphaValue));
			aGc->SetPenColor(TRgb(170,170,170, aAlphaValue));
			}
		else
			{
			aGc->SetBrushColor(TRgb(85,85,85));
			aGc->SetPenColor(TRgb(170,170,170));
			}
		aGc->SetDrawMode(iEllipseDrawMode);
		aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		aGc->DrawEllipse(aRect);
		}

	void CCrpAnim::Draw(CBitmapContext *aGc, const TSize &aSize, TBool aIsBase, const TRect &aRect, TBool aBlankIt,TInt aRepeat, TInt aAlphaValue)
		{
		static TInt sGrey=0;
		sGrey+=3;
		if (sGrey>0x40)
			sGrey-=0x40;		
		sGrey=sGrey^0x20;
		if(aAlphaValue != ENoTransparency)
			{
			aGc->SetBrushColor(TRgb(sGrey, sGrey, sGrey, aAlphaValue));
		    aGc->SetPenColor(TRgb(KRgbGreen.Value(), aAlphaValue));
			}
		else
			{		
			aGc->SetBrushColor(TRgb::Gray256(sGrey));
		    aGc->SetPenColor(KRgbGreen);
			}
		aGc->Clear();
	    TInt xPos=aSize.iHeight,yPos=aSize.iWidth;
	    
	    // The test windows are created relative to screen size. The
	    // number of green lines generated needs to be tied into the
	    // window size to prevent green becoming the dominant colour
	    // when blended with the second animation, which would
	    // prevent the PredominantColour() algorithm from discovering
	    // the red line.
	    TInt yStep = aSize.iHeight/14;
	    TInt xStep = aSize.iWidth/6;
	    
	    //This paint is intentionally complex and slow so that the animation timer is likely to interrupt it.
		if (!aBlankIt)
		 for (TInt nn = 0 ; nn < aRepeat ; nn++)
			for(yPos=0 ; yPos < aSize.iHeight ; yPos += yStep)
				for(xPos=0 ; xPos < aSize.iWidth ; xPos += xStep)
					aGc->DrawLine(aRect.Center(),TPoint(xPos,yPos));
		if (aIsBase)
			DrawEllipse(aGc, aRect, aAlphaValue);			
		}

	//This simple API may need replacing by a list and search if multiple anims are played together
	TWsGraphicAnimation* CCrpAnim::SetAnimation(TUid aUid)
		{	//currently only have 1 animation - it gets replaced. It could get refiused
		iAnimUid=aUid;
		return &iAnimData;	
		}

	TWsGraphicAnimation* CCrpAnim::GetAnimation(TUid aUid)
		{   //currently only have 1 animation
		if (iAnimUid==aUid)
			return &iAnimData;	
		else
			return NULL;
		}
		
	void CCrpAnim::SetPosAnimation(const TUid& aUid, const TRect& aRect)
		{   //currently only have 1 animation
		if (iAnimUid==aUid)
			iAnimPos = aRect;
		}
	
	TRect* CCrpAnim::GetPosAnimation(const TUid& aUid)
		{   //currently only have 1 animation
		if (iAnimUid==aUid)
			return &iAnimPos;	
		else
			return NULL;
		}
	
	TBool CCrpAnim::RemoveAnimation(TUid)
		{
		iAnimUid=TUid::Null();
		iAnimData.Stop(EFalse);		
		return ETrue;
		}	

	void CCrpAnim::DoDraw()
		{
		DoDraw(iBlankIt);
		}

	inline void CCrpAnim::DoDraw(TBool aBlankIt)
		{
		__ASSERT_ALWAYS(iWinType!=EBlank,AutoPanic(EAutoPanicWindowType));
		iCtWin->Gc()->Activate(*Window());
		Draw(iCtWin->Gc(),Size(),iIsBase,iRect,aBlankIt,iRepeatDrawMax,iAlphaValue);
		if (iAnimUid!=TUid::Null())
			iCtWin->Gc()->DrawWsGraphic(iAnimUid,iAnimPos,iAnimData.Pckg());
		iCtWin->Gc()->Deactivate();
		}
		
	void CCrpAnim::DoDrawEllipse()
		{
		__ASSERT_ALWAYS(iWinType!=EBlank,AutoPanic(EAutoPanicWindowType));
		iCtWin->Gc()->Activate(*Window());
		DrawEllipse(iCtWin->Gc(),iRect,iAlphaValue);
		iCtWin->Gc()->Deactivate();
		}

	void CCrpAnim::InvalidateAndRedraw(TBool /*aUseBlankItMember*/,TBool /*aBlankIt*/,TBool aUseRWindowInvalidate,TRect* aRect)
		{
		RWindow& win = *Window();
		if (aRect)
			{
			if (aUseRWindowInvalidate)
				win.Invalidate(*aRect);
			else
				Invalidate(*aRect);
			}
		else
			{
			if (aUseRWindowInvalidate)
				win.Invalidate();
			else
				Invalidate();
			}
		if (aRect)
			win.BeginRedraw(*aRect);
		else
			win.BeginRedraw();
		DoDraw();
		win.EndRedraw();
		TheClient->Flush();
		}
		
	void CCrpAnim::Invalidate(const TRect &aRect)
		{
		TRect rect(aRect);
		rect.Move(iCtWin->Position());
		CTUser::Splat(TheClient,rect,TRgb::Gray256(0));
		}

//
	}	//end anonymous namespace
//
__WS_CONSTRUCT_STEP__(CrpAnim)
