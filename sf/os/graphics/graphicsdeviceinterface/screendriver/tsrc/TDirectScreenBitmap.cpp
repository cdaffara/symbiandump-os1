// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>
#include <hal.h>
#include <bitdraw.h>
#include <bitdrawinterfaceid.h>
#include <bitdrawsurface.h>
#include "cdsb.h"
#include <bitstd.h>
#include <bitdev.h>
#include "BMDRAW.H"

#ifdef SYMBIAN_GRAPHICS_GCE
#undef __WINS__
#elif __WINS__
#include "_WININC.H"
#endif

#include "TDirectScreenBitmap.h"

enum 
	{
	WAIT_TIME = 1*400000
	};

class CCommonInterfaces
	{
public:
	virtual ~CCommonInterfaces();
	TBool ScreenClear();
	TBool ScreenClearPartial();
	TBool DrawColor(const TRect& aRect,const TRgb& aColour);
	CCommonInterfaces(const TRect& aRect, const TDisplayMode& aDispMode,CDirectScreenBitmap* aDSBitmap);
	void BeginDraw();
	void EndDraw(TRequestStatus& aStatus);
	void EndDraw(const TRect& aRect, TRequestStatus& aStatus);
	void Drawing(const TRect& aRect);
	void DrawColour64KPixel(TInt aX,TInt aY, const TRgb& aColour);

protected:
	TRect iRect;
	TDisplayMode iDispMode;
   	CDirectScreenBitmap* iDSBitmap;
   	TAcceleratedBitmapInfo iBitmapInfo;
   	TBool iDrawingRed;
	};

CCommonInterfaces::CCommonInterfaces(const TRect& aRect, const TDisplayMode& aDispMode,CDirectScreenBitmap* aDSBitmap):iRect(aRect),iDispMode(aDispMode),iDSBitmap(aDSBitmap),iDrawingRed(ETrue)
	{
	}

CCommonInterfaces::~CCommonInterfaces()
	{
	
	}

/** If the display mode of iBitmapInfo is not expected, return EFalse
 * or render to the back buffer and returns ETrue
*/
TBool CCommonInterfaces::DrawColor(const TRect& aRect,const TRgb& aColour)
	{
	TRect local = TRect(aRect.iTl-iRect.iTl, aRect.Size());
	TUint16* pBuffer16;
	TUint32* pBuffer32;

	if (iBitmapInfo.iDisplayMode != iDispMode)
		{
		return EFalse;
		}
	for (TInt y = local.iTl.iY; y < local.iBr.iY; y++)
		{
		for (TInt x = local.iTl.iX; x < local.iBr.iX; x++)
			{
			switch (iDispMode)
				{
				case EColor64K:
					pBuffer16 = (TUint16*)iBitmapInfo.iAddress;
					pBuffer16[y*iBitmapInfo.iLinePitch/2+x] = aColour._Color64K();
					break;
				case EColor16M:
					pBuffer16 = (TUint16*)iBitmapInfo.iAddress;
					pBuffer16[y*iBitmapInfo.iLinePitch/2+x] = aColour._Color64K();
					break;
				case EColor16MU:
					pBuffer32 = (TUint32*)iBitmapInfo.iAddress;
					pBuffer32[y*iBitmapInfo.iLinePitch/4+x] = aColour._Color16MU();
					break;
				case EColor16MA:
					pBuffer32 = (TUint32*)iBitmapInfo.iAddress;
					pBuffer32[y*iBitmapInfo.iLinePitch/4+x] = aColour._Color16MA();
					break;
				case EColor4K:
					pBuffer16 = (TUint16*)iBitmapInfo.iAddress;
					pBuffer16[y*iBitmapInfo.iLinePitch/2+x] = aColour._Color4K();
					break;
				case EColor16MAP:
					pBuffer32 = (TUint32*)iBitmapInfo.iAddress;
					pBuffer32[y*iBitmapInfo.iLinePitch/4+x] = aColour._Color16MAP();
					break;
				default:
					break;
				}
			}
		}
	return ETrue;
	}

TBool CCommonInterfaces::ScreenClear()
	{
    iDSBitmap->BeginUpdate(iBitmapInfo);
    TBool ret= DrawColor(iRect, KRgbWhite);
    if (ret)
    	{
        TRequestStatus requestStatus;
        iDSBitmap->EndUpdate(requestStatus);	
    	}
    return ret;
	}

TBool CCommonInterfaces::ScreenClearPartial()
	{
    iDSBitmap->BeginUpdate(iBitmapInfo);
    TBool ret= DrawColor(iRect, KRgbWhite);
    if (ret)
    	{
        TRequestStatus requestStatus;
        iDSBitmap->EndUpdate(iRect,requestStatus);	
    	}
    return ret;
	}

void CCommonInterfaces::BeginDraw()
    {
    iDSBitmap->BeginUpdate(iBitmapInfo);       
    }

void CCommonInterfaces::EndDraw(TRequestStatus& aStatus)
    {
     iDSBitmap->EndUpdate(aStatus);       
    }

void CCommonInterfaces::EndDraw(const TRect& aRect, TRequestStatus& aStatus)
	{
    iDSBitmap->EndUpdate(aRect,aStatus);       
	}


void CCommonInterfaces::Drawing(const TRect& aRect)
	{
    if (iDrawingRed)
    	{
    	DrawColor(aRect,KRgbRed);
    	iDrawingRed = EFalse;
    	}
    else
    	{
    	DrawColor(aRect,KRgbBlack);
    	iDrawingRed = ETrue;
    	}
     }

void CCommonInterfaces::DrawColour64KPixel(TInt aX,TInt aY, const TRgb& aColour) 
	{ 
	TUint16 *pBuffer = (TUint16*)iBitmapInfo.iAddress; 
	pBuffer[aY*iBitmapInfo.iLinePitch/2+aX] = aColour._Color64K(); 
	}

class CRendering : public CActive, public CCommonInterfaces
    {
 	public:
 		static CRendering* NewL(const TRect& aRect, const TDisplayMode& aDispMode, CDirectScreenBitmap* aDSBitmap,CDirectScreenBitmap::TSettingsFlags aSettings);
 		void ProcessFrame();
 		void EndDraw();
 		virtual ~CRendering();
 	protected: // from CActive
    
    	void DoCancel();
    	void RunL();
    
    private:
    
    	CRendering(const TRect& aRect, const TDisplayMode& aDispMode,CDirectScreenBitmap* aDSBitmap,CDirectScreenBitmap::TSettingsFlags aSettings);
    	void ConstructL();
    	
    private:
    	
    	TAcceleratedBitmapInfo iBitmapInfo;
    	TInt iFrames;	
    	CDirectScreenBitmap::TSettingsFlags iSettings;
   
    };

CRendering* CRendering::NewL(const TRect& aRect, const TDisplayMode& aDispMode,CDirectScreenBitmap* aDSBitmap, CDirectScreenBitmap::TSettingsFlags aSettings)
	{
	CRendering * rendering = new (ELeave) CRendering(aRect, aDispMode, aDSBitmap, aSettings);
	CleanupStack::PushL(rendering);
	rendering->ConstructL();
	CleanupStack::Pop(rendering);
	return rendering;
	}

CRendering::~CRendering()
    {
    // "Cancel" is a meaningless call, since the service 
    // (video driver's screen update process) is not cancellable.
    // When destroying this active object, you must make sure, 
    // that the last update request (CDirectScreenBitmap::EndUpdate()) is completed.
    // Assuming that LCD refresh rate is not less than 60 Hz,
    // the wait time should be more than 1/60 secs.
    // (Otherwise a stay signal comes.)
    User::After(WAIT_TIME); // to let the GCE return a valid request notification back to CDSB
    						// or surface update panics with EUpdateServPanicDataIntegrity
    Cancel();
    }
 
CRendering::CRendering(const TRect& aRect, const TDisplayMode& aDispMode, CDirectScreenBitmap* aDSBitmap,CDirectScreenBitmap::TSettingsFlags aSettings)
:CActive( CActive::EPriorityStandard ),CCommonInterfaces(aRect,aDispMode,aDSBitmap),iFrames(0),iSettings(aSettings)
    {
    }


void CRendering::ConstructL()
    {
    CActiveScheduler::Add( this );
    // clear the screen, and also check the display mode of iBitmapInfo is the same as expected
    // If not, stop creating the CRendering object. 
    TBool ret = (iSettings & CDirectScreenBitmap::EIncrementalUpdate) ? ScreenClearPartial() : ScreenClear();
    if (!ret)
    	{
    	User::Leave(KErrGeneral);
    	}
    }

void CRendering::RunL()
    {
    // Video driver finished to draw the last frame on the screen
    // You may initiate rendering the next frame from here, 
    // but it would be slow, since there is a delay between CDirectScreenBitmap::EndUpdate() 
    // and the completition of screen refresh by the video driver	
    if (iFrames++ <200)
    	{
    	ProcessFrame();
      	}
    else
    	{
    	CActiveScheduler::Stop();
    	}      
    }
 
void CRendering::DoCancel()
    {
    // Cancel not implemented in service provider, so we can't do anything here
    }
 

void CRendering::EndDraw()
    {
    if (IsActive())
    	{
    	Cancel();
    	}
    CCommonInterfaces::EndDraw(iStatus);
    SetActive();
    }
 
void CRendering::ProcessFrame()
    {
    BeginDraw();
    Drawing(iRect);
    EndDraw();
    }


//A largely visual test to demonstrate regional refreshing of DirectScreenBitmaps and the correct display function
//of the Emulator.
class CTDirectScreenBitmap : public CTGraphicsBase
	{
	public:	
		~CTDirectScreenBitmap();
		void ConstructL(const TRect &aScreenRect, const CDirectScreenBitmap::TSettingsFlags aSettings,TInt aScreenNo=0);	
		void Close();
		void DoRefreshCycle(void);
		void TestOrientRefresh(CFbsDrawDevice *aDev);
		void TestRefreshingWindowsL();
		void ExhaustiveTestMergeL(TBool aInSrcPreMul,TBool aInDestPreMul,TBool aOutDestPreMul,TInt aStepSize=1,TInt aChannelControl=1);
		void ExhaustiveTestMergePerDispModeL(TDisplayMode aDrawDeviceDispMode, TBool aInSrcPreMul,TBool aInDestPreMul,TBool aOutDestPreMul,TInt aStepSize=1,TInt aChannelControl=1,TInt aOtherChannels=0,TDisplayMode aTestMode=EColor16MA);
 		void LogColourEvent(TInt aPreMulDestPixColor,TInt aNonPreMulDestPixColor,TInt aPreMulSrcPixelColor,TInt aNonPreMulSrcPixelColor,TReal aVal1,TReal aVal2,TReal aVal3,TRefByValue<const TDesC> aMsg,TBool aErr);
 		void TestContinuousRefreshingL();
 		void TestRefreshingTimeL();
	private:
	    void TestRefreshingWindowsPerDisplayModeL(TDisplayMode aDisplayMode);
		void TestContinuousRefreshingPerDisplayModeL(const TDisplayMode& aDisplayMode);
		void UpdateFrame();
		void MeasureFrame(const TRect &aScreenRect);
	    CDirectScreenBitmap::TSettingsFlags iSettings;
		CDirectScreenBitmap* iDirectScreenBitmap;
		TAcceleratedBitmapInfo iBitmapInfo;
		TRequestStatus iRequestStatus;
		TRect iFullRect;
		TRect iTopLeft;
		TRect iTopRight;
		TRect iBotLeft; 
		TRect iBotRight;
		TRect iMiddle;
		TInt iScreenType;
		CCommonInterfaces* iInterface; 
		CRendering* iRendering;
	public:
		CTDirectScreenBitmap();
		CTDirectScreenBitmap(CTestStep* aStep);
	protected:
	//from 	CTGraphicsStep
		virtual void RunTestCaseL(TInt aCurTestCase);
	private:
		TDisplayMode iDispMode;
		TInt iFreq;			// For measuring the average time per frame
	    TReal iMeasure ;
	    TUint32 iTimeBefore;
	    TUint32 iTimeAfter;

	};

/**
The runtime code currently contains a special case when dest starts unmultipled and ends premultiplied. 
When src alpha is 0 the dest colour is simply passed through, not converted from unmultiplied to premultiplied. 
That causes this fn to record errors, that will get ignored 
for all source mask (srcMask==0), and most values of bkgrdChannel.
*/
class TIgnoreSpecialCases  
	{ 
	public: 
		TIgnoreSpecialCases (TBool aInSrcPreMul,TBool aInDestPreMul,TBool aOutDestPreMul) ;
		bool operator()(TInt aPreMulDestPixColor,TInt aNonPreMulDestPixColor,TInt aSrcMask,TInt aSrcChannel);
		TInt IgnoreCount();
		TInt ExpectedIgnoreCount(TInt aFreq);
      private: 
	      TInt iIgnoredCount; 
	      TBool iDoIgnore; 
	}; 

CTDirectScreenBitmap::CTDirectScreenBitmap(CTestStep* aStep) :
	CTGraphicsBase(aStep),iMeasure(0)
	{
	}
	
CTDirectScreenBitmap::~CTDirectScreenBitmap()
	{
	Close();
	}
	
//Construct the DirectScreenBitmap and sub-rects for test of the refresh.	
//Constructs a DirectScreenBitmap for EDoubleBuffer and EIncrementalUpdate
void CTDirectScreenBitmap::ConstructL(const TRect &aScreenRect, const CDirectScreenBitmap::TSettingsFlags aSettings, TInt aScreenType)
	{
	Close();
	iScreenType = aScreenType;
	TInt Screenno;
	if (iScreenType==CDirectScreenBitmap::EDoubleBuffer || iScreenType==CDirectScreenBitmap::EIncrementalUpdate)  
		{
		Screenno=iScreenType;
		iDirectScreenBitmap = CDirectScreenBitmap::NewL(Screenno);
		}
	else
		{
		iDirectScreenBitmap = CDirectScreenBitmap::NewL();
		}
	iSettings = aSettings;
	TInt create=iDirectScreenBitmap->Create(aScreenRect, aSettings);
	if (create==KErrNone)
		{
		iFullRect	 = aScreenRect;
		TPoint Pos	 = aScreenRect.iTl;
		TSize hSize	 = TSize(aScreenRect.Width()/2, aScreenRect.Height()/2);
		TSize hSizeX = TSize(aScreenRect.Width()/2, 0);
		TSize hSizeY = TSize(0, aScreenRect.Height()/2);
		TSize qSize	 = TSize(aScreenRect.Width()/4, aScreenRect.Height()/4);
		
		iTopLeft  = TRect(Pos,  hSize);
		iTopRight = TRect(Pos + hSizeX, hSize);
		iBotLeft  = TRect(Pos + hSizeY, hSize);	
		iBotRight = TRect(Pos + hSize,  hSize);
		iMiddle   = TRect(Pos + hSize - qSize,  hSize);

		iInterface = new (ELeave) CCommonInterfaces(iFullRect, iDispMode, iDirectScreenBitmap);
		iRendering = CRendering::NewL(iFullRect,iDispMode,iDirectScreenBitmap,iSettings);
		}
	else 
		{
		User::Leave(create);
		}
	}

void CTDirectScreenBitmap::Close()
	{
	User::After(WAIT_TIME);
	delete iDirectScreenBitmap;
	iDirectScreenBitmap = NULL;
	delete iInterface;
	iInterface = NULL;
	delete iRendering;
	iRendering = NULL;
	}


//A refresh cycle fills a 2x2 region of RGBW rects and draws it to the screen. If the mode is incremental
//The different colour rects are updated seperately to test the sub-region update.
void CTDirectScreenBitmap::DoRefreshCycle(void)
	{
	if ((iSettings & CDirectScreenBitmap::EIncrementalUpdate)&&(!iInterface->ScreenClearPartial()))
		{
		INFO_PRINTF1(_L("The display mode of the iBitmapInfo is not expected"));
		}
	else if (!(iSettings & CDirectScreenBitmap::EIncrementalUpdate)&&(!iInterface->ScreenClear()))
		{
		INFO_PRINTF1(_L("The display mode of the iBitmapInfo is not expected"));
		}
	else 
		{
		iInterface->BeginDraw();
		iInterface->DrawColor(iTopLeft,KRgbRed);
		iInterface->DrawColor(iTopRight,KRgbGreen);
		iInterface->DrawColor(iBotLeft,KRgbBlue);
		iInterface->DrawColor(iBotRight,KRgbBlack);
							
		if (!(iSettings & CDirectScreenBitmap::EIncrementalUpdate))
			{		
			iInterface->EndDraw(iRequestStatus);	
			User::After(WAIT_TIME);
			}
		else
			{		
			iInterface->EndDraw(iTopLeft, iRequestStatus);	
			User::After(WAIT_TIME);
			iInterface->BeginDraw();
			iInterface->EndDraw(iTopRight, iRequestStatus);	
			User::After(WAIT_TIME);
			iInterface->BeginDraw();
			iInterface->EndDraw(iBotLeft, iRequestStatus);	
			User::After(WAIT_TIME);
			iInterface->BeginDraw();
			iInterface->EndDraw(iBotRight, iRequestStatus);	
			User::After(WAIT_TIME);
			
			iInterface->DrawColor(iMiddle,KRgbCyan);
			iInterface->BeginDraw();
			iInterface->EndDraw(iMiddle, iRequestStatus);
			User::After(WAIT_TIME);

			}
		__ASSERT_DEBUG(iInterface->ScreenClear(), User::Invariant());
		}
	}
	
//Tests the Orientation and Refresh operations
void CTDirectScreenBitmap::TestOrientRefresh(CFbsDrawDevice *aDev)
	{

	MSurfaceId* surfaceIdInterface;
	if (KErrNone != aDev->GetInterface(KSurfaceInterfaceID, reinterpret_cast <TAny*&> (surfaceIdInterface)))
		{
		// GCE is off
		TBool orientations[4];
		aDev->OrientationsAvailable(orientations);
		if ((!orientations[0])&&(!orientations[1])&&(!orientations[2])&&(!orientations[3]))
			{
			INFO_PRINTF1(_L("TestOrientRefresh(): All 4 orientations not supported? (error for non-gce)"));
			}
		for (TUint i = 0; i<4; i++)
			{
			if (orientations[i])
				{
				if (aDev->SetOrientation((CFbsDrawDevice::TOrientation)i))
					{
					DoRefreshCycle();	
					}
				else                                                                                                                                                                                                                                   
					{
					TEST(EFalse);
					}
				}
			}
		aDev->SetOrientation((CFbsDrawDevice::TOrientation)0);
		}
	else
		{
		//GCE is on
		TInt orientationAvailable = surfaceIdInterface->DeviceOrientationsAvailable();
		for (TUint32 i = (TUint32)EDeviceOrientationNormal; i <= (TUint32)EDeviceOrientation270CW; i *= 2)
			{
			if (orientationAvailable&i)
				{
				surfaceIdInterface->SetDeviceOrientation((TDeviceOrientation)i);
				DoRefreshCycle();	
				}
			}
		surfaceIdInterface->SetDeviceOrientation(EDeviceOrientationNormal);
		}
	}	


/** 
@SYMTestCaseID GRAPHICS-CTDirectScreenBitmap-TestRefreshingWindowsL-0001
@SYMDEF	PDEF103006
@SYMTestCaseDesc Test orientation and Tests refreshing windows for various display modes
on all type of screens( full screen non-incremental, full screen double buffer, full screen incremental,
sub-region non incremental, sub region double-buffer, sub region incremental). 
@SYMTestPriority Critical
@SYMTestStatus Implemented
@SYMTestActions Create a screen device for all type of screens. Set and test screen orientation 
and finally test refreshing windows.
@SYMTestExpectedResults
The orientation and windows refresh test shall succeed on all screen type for various display mode
**/ 
void CTDirectScreenBitmap::TestRefreshingWindowsL()
{     
	 TestRefreshingWindowsPerDisplayModeL(EColor64K);
	 TestRefreshingWindowsPerDisplayModeL(EColor16M);   
	 TestRefreshingWindowsPerDisplayModeL(EColor16MU);
	 TestRefreshingWindowsPerDisplayModeL(EColor16MA);
	 TestRefreshingWindowsPerDisplayModeL(EColor4K);     	   
	 TestRefreshingWindowsPerDisplayModeL(EColor16MAP); 	   
}
	
/**
Perform TestRefreshingWindowsL test as describe above for individual display mode type.
@param aDisplayMode : relate to the window color mode to be displayed
**/
void CTDirectScreenBitmap::TestRefreshingWindowsPerDisplayModeL(TDisplayMode aDisplayMode)
{
	CFbsDrawDevice *pDev = NULL;	
	iDispMode = aDisplayMode;
		
	TRAPD(err,pDev = CFbsDrawDevice::NewScreenDeviceL(0, aDisplayMode));
	if (err)
		{
		INFO_PRINTF2(_L("Note: Failed to create screen device for display mode %i - not supported on this config?"),aDisplayMode);

		}
	else
		{
		INFO_PRINTF2(_L("Testing RefreshingWindows for display mode %i"),aDisplayMode);
		CleanupDeletePushL(pDev);
		pDev->InitScreen();

		TSize screenSize = pDev->SizeInPixels();
		TRect directRect;
		
		directRect = TRect(TPoint(), screenSize);
		//Full screen non-incremental run
		INFO_PRINTF1(_L("Full screen non-incremental"));
		TRAPD(err,ConstructL(directRect, CDirectScreenBitmap::ENone));
		if (err)
			{
			INFO_PRINTF1(_L("The Direct Screen Bitmap object is not created"));
			}
		else
			{
			TestOrientRefresh(pDev);
			}
	
		//Full screen double buffer
		INFO_PRINTF1(_L("Full screen double-buffer"));
		TRAP(err,ConstructL(directRect,CDirectScreenBitmap::EDoubleBuffer,1));
		if (err)
			{
			INFO_PRINTF1(_L("The Direct Screen Bitmap object is not created"));
			}
		else
			{
			TestOrientRefresh(pDev);
			}
			
		//Full screen incremental run
		INFO_PRINTF1(_L("Full screen incremental"));
		TRAP(err,ConstructL(directRect, CDirectScreenBitmap::EIncrementalUpdate,2));
		if (err)
			{
			INFO_PRINTF1(_L("The Direct Screen Bitmap object is not created"));
			}
		else
			{
			TestOrientRefresh(pDev);
			}
		
		//Quarter screen non-incremental run
		INFO_PRINTF1(_L("Sub region non-incremental"));
		directRect = TRect(TPoint(screenSize.iWidth/2, screenSize.iHeight/2), TSize(screenSize.iWidth/2, screenSize.iHeight/2));
		TRAP(err,	ConstructL(directRect, CDirectScreenBitmap::ENone));
		if (err)
			{
			INFO_PRINTF1(_L("The Direct Screen Bitmap object is not created"));
			}
		else
			{
			TestOrientRefresh(pDev);
			}
	
		//Quarter screen double buffer
		INFO_PRINTF1(_L("Sub region double-buffer"));
		TRAP(err,ConstructL(directRect,CDirectScreenBitmap::EDoubleBuffer,1));
		if (err)
			{
			INFO_PRINTF1(_L("The Direct Screen Bitmap object is not created"));
			}
		else
			{
			TestOrientRefresh(pDev);
			}
						
		//Quarter screen incremental run
		INFO_PRINTF1(_L("Sub region incremental"));
		TRAP(err,ConstructL(directRect, CDirectScreenBitmap::EIncrementalUpdate,2));
		if (err)
			{
			INFO_PRINTF1(_L("The Direct Screen Bitmap object is not created"));
			}
		else
			{
			TestOrientRefresh(pDev);
			}
			
		CleanupStack::PopAndDestroy(pDev);
		}
}

void CTDirectScreenBitmap::TestContinuousRefreshingL()
	{
	TestContinuousRefreshingPerDisplayModeL(EColor64K);
	TestContinuousRefreshingPerDisplayModeL(EColor16M);   
	TestContinuousRefreshingPerDisplayModeL(EColor16MU);
	TestContinuousRefreshingPerDisplayModeL(EColor16MA);
	TestContinuousRefreshingPerDisplayModeL(EColor4K);     	   
	TestContinuousRefreshingPerDisplayModeL(EColor16MAP); 	 
	}

void CTDirectScreenBitmap::TestContinuousRefreshingPerDisplayModeL(const TDisplayMode& aDisplayMode)
	{
	
	CFbsDrawDevice *pDev = NULL;	
	iDispMode = aDisplayMode;
		
	TRAPD(err,pDev = CFbsDrawDevice::NewScreenDeviceL(0, aDisplayMode));
	if (err)
		{
		INFO_PRINTF2(_L("Note: Failed to create screen device for display mode %i - not supported on this config?"),iDispMode);

		}
	else
		{
		INFO_PRINTF2(_L("Testing Continuous Refreshing for display mode %i"),iDispMode);
		CleanupDeletePushL(pDev);
		User::LeaveIfError(pDev->InitScreen());

		TSize screenSize = pDev->SizeInPixels();
		TRect directRect;

		// Quarter screen ENone
		INFO_PRINTF1(_L("Consecutively update frames at left bottom corner and setting ENone"));
		directRect = TRect(TPoint(0, screenSize.iHeight/2), TSize(screenSize.iWidth/2, screenSize.iHeight/2));
		TRAPD(err,ConstructL(directRect, CDirectScreenBitmap::ENone));
		if (err)
			{
			INFO_PRINTF1(_L("The Direct Screen Bitmap object is not created"));
			}
		else
			{
			// use the same iDirectScreenBitmap as CTDirectScreenBitmap 
			UpdateFrame();
			}
		// Quarter screen EDoubleBuffer
		INFO_PRINTF1(_L("Consecutively update frames at up right corner and setting EDoubleBuffer"));
		directRect = TRect(TPoint(screenSize.iWidth/2, 0), TSize(screenSize.iWidth/2, screenSize.iHeight/2));
		TRAP(err,ConstructL(directRect,CDirectScreenBitmap::EDoubleBuffer,1));
		if (err)
			{
			INFO_PRINTF1(_L("The Direct Screen Bitmap object is not created"));
			}
		else
			{
			// use the same iDirectScreenBitmap as CTDirectScreenBitmap 
			UpdateFrame();
			}
		// Quarter screen EIncremental
		INFO_PRINTF1(_L("Consecutively update frames at right bottom corner and setting EIncremental"));
		directRect = TRect(TPoint(screenSize.iWidth/2, screenSize.iHeight/2), TSize(screenSize.iWidth/2, screenSize.iHeight/2));
		TRAP(err,ConstructL(directRect,CDirectScreenBitmap::EIncrementalUpdate,2));
		if (err)
			{
			INFO_PRINTF1(_L("The Direct Screen Bitmap object is not created"));
			}
		else
			{
			// use the same iDirectScreenBitmap as CTDirectScreenBitmap 
			UpdateFrame();
			}
		
		// Full screen EIncremental
		INFO_PRINTF1(_L("Consecutively update frames at full screen and setting EIncremental"));
		directRect = TRect(TPoint(0,0), screenSize);
		TRAP(err,ConstructL(directRect,CDirectScreenBitmap::EIncrementalUpdate,2));
		if (err)
			{
			INFO_PRINTF1(_L("The Direct Screen Bitmap object is not created"));
			}
		else
			{
			// use the same iDirectScreenBitmap as CTDirectScreenBitmap 
			UpdateFrame();
			}
		
		// Full screen ENone
		INFO_PRINTF1(_L("Consecutively update frames at full screen and setting ENone"));
		directRect = TRect(TPoint(0,0), screenSize);
		TRAP(err,ConstructL(directRect,CDirectScreenBitmap::ENone));
		if (err)
			{
			INFO_PRINTF1(_L("The Direct Screen Bitmap object is not created"));
			}
		else
			{
			// use the same iDirectScreenBitmap as CTDirectScreenBitmap 
			UpdateFrame();
			}
		
		// Full screen EDoubleBuffering
		INFO_PRINTF1(_L("Consecutively update frames at full screen and setting EDoubleBuffering"));
		directRect = TRect(TPoint(0,0), screenSize);
		TRAP(err,ConstructL(directRect,CDirectScreenBitmap::EDoubleBuffer,1));
		if (err)
			{
			INFO_PRINTF1(_L("The Direct Screen Bitmap object is not created"));
			}
		else
			{
			// use the same iDirectScreenBitmap as CTDirectScreenBitmap 
			UpdateFrame();
			}
		CleanupStack::PopAndDestroy(pDev);
		}
	}

void CTDirectScreenBitmap::UpdateFrame()
	{
	iRendering->ProcessFrame();
	CActiveScheduler::Start();
	}

void CTDirectScreenBitmap::MeasureFrame(const TRect &aScreenRect)
	{
	TInt count = 0;
	iMeasure = 0;
	while (count++<200)
		{
		__ASSERT_DEBUG(iInterface->ScreenClear(), User::Invariant());
		iInterface->BeginDraw();
		iInterface->Drawing(aScreenRect);
		TRequestStatus status;
		iTimeBefore = User::FastCounter();
		iInterface->EndDraw(status);
		iTimeAfter = User::FastCounter(); 
		iMeasure += 1000 * ((TReal)(iTimeAfter - iTimeBefore)) / ((TReal) iFreq);
		}
	}

void CTDirectScreenBitmap::TestRefreshingTimeL()
	{
	// As other display modes have been tested in other test cases
	// Only EColor16MAP is tested here
	CFbsDrawDevice *pDev = NULL;	
	TDisplayMode aDisplayMode = EColor16MAP;
	iDispMode = aDisplayMode;
			
	TRAPD(err,pDev = CFbsDrawDevice::NewScreenDeviceL(0, aDisplayMode));
	if (err)
		{
		INFO_PRINTF2(_L("Note: Failed to create screen device for display mode %i - not supported on this config?"),iDispMode);
		}
	else
		{
		INFO_PRINTF2(_L("Testing Continuous Refreshing for display mode %i"),iDispMode);
		CleanupDeletePushL(pDev);
		User::LeaveIfError(pDev->InitScreen());
		TSize screenSize = pDev->SizeInPixels();
		TRect directRect;

		// Quarter screen ENone
		INFO_PRINTF1(_L("Consecutively update frames at left bottom corner and setting ENone"));
		directRect = TRect(TPoint(0, screenSize.iHeight/2), TSize(screenSize.iWidth/2, screenSize.iHeight/2));
		TRAPD(err,ConstructL(directRect, CDirectScreenBitmap::ENone));
		if (err)
			{
			INFO_PRINTF1(_L("The Direct Screen Bitmap object is not created"));
			}
		else
			{
			// use the same iDirectScreenBitmap as CTDirectScreenBitmap 
		    User::LeaveIfError(HAL::Get(HALData::EFastCounterFrequency, iFreq));

			MeasureFrame(directRect);
			
			INFO_PRINTF2(_L("The total time to render 200 frames is %f"),iMeasure);

			}
		CleanupStack::PopAndDestroy(pDev);

		}
	}

// TIgnoreSpecialCases 
/**
The runtime code currently contains a special case when dest starts unmultipled and ends premultiplied. 
When src alpha is 0 the dest colour is simply passed through, not converted from unmultiplied to premultiplied. 
That causes this fn to record errors, that will get ignored for all Source Mask (srcMask==0), and most values of background channel(bkgrdChannel).
*/
TIgnoreSpecialCases ::TIgnoreSpecialCases (TBool /*aInSrcPreMul*/,TBool aInDestPreMul,TBool aOutDestPreMul) 
	{
  	iDoIgnore=(!aInDestPreMul && aOutDestPreMul); 
  	iIgnoredCount=0; 
  	} 
	  
/**
The function returns true if the error should be ignored and  keeps the count of the no. of errors ignored.
*/	      
bool TIgnoreSpecialCases ::operator()(TInt /*aPreMulDestPixColor*/,TInt /*aNonPreMulDestPixColor*/,TInt aSrcMask,TInt /*aSrcChannel*/)
	{
	if (!iDoIgnore || aSrcMask!=0)
		{
		return EFalse;
		}
	iIgnoredCount++;
	return ETrue;
	} 

/**
The function returns the no. of errors ignored.
*/
TInt TIgnoreSpecialCases ::IgnoreCount()
	{
	return iIgnoredCount;
	}

/**
If the special-case is generating ignored errors then it should generate at least this many
If not generating errors then the count should be zero. Any other value should be noted to the log.
*/
TInt TIgnoreSpecialCases ::ExpectedIgnoreCount(TInt aFreq)
	{
	TInt samples=240/aFreq;
	return samples*samples*200*2; //This is an approximation.
	}
		
/**
This function is used to write to the log file
@param  aNonPreMulDestPixColor non pre multiplied destination pixel colour
@param  aPreMulSrcPixelColor pre multiplied source pixel colour
@param  aNonPreMulSrcPixelColor	non pre multiplied source pixel colour
@param  aVal1 it contains the value of the first variable of the message to be displayed
@param  aVal2 it contains the value of the second variable of the message to be displayed
@param  aVal3 it contains the value of the third variable of the message to be displayed
@param  aMsg it contains the message to be printed to the log file
@param  aErr if true then the test case fails, if false test passes. log is printed in both the case. TEST does not abort, just reports test case failure
*/
void CTDirectScreenBitmap::LogColourEvent(TInt aPreMulDestPixColor,TInt aNonPreMulDestPixColor,TInt aPreMulSrcPixelColor,TInt aNonPreMulSrcPixelColor,TReal aVal1,TReal aVal2,TReal aVal3,TRefByValue<const TDesC> aMsg,TBool aErr)
	{
	TEST(aErr==EFalse); // if aErr=True, then the previous test step failed.
	INFO_PRINTF4(aMsg,aVal1,aVal2,aVal3);
	if (aNonPreMulSrcPixelColor<0)
		{
		INFO_PRINTF4(_L("Processing source colours for MDest=%i, CDest=%i, MSrc=%i"),aPreMulDestPixColor,aNonPreMulDestPixColor,aPreMulSrcPixelColor);
		}
	else
		{
		INFO_PRINTF5(_L("Processing colour set MDest=%i, CDest=%i, MSrc=%i, CSrc=%i"),aPreMulDestPixColor,aNonPreMulDestPixColor,aPreMulSrcPixelColor,aNonPreMulSrcPixelColor);
		}
	}
	
/** 
@SYMTestCaseID GRAPHICS-CTDirectScreenBitmap-ExhaustiveTestMergeL-0001
@SYMDEF	PDEF099416
@SYMTestCaseDesc Test that the alpha merge methods work accurately
@SYMTestPriority High
@SYMTestStatus Implemented
@SYMTestActions Create an alpha capable bitmap and throw sample values at the merge function
@SYMTestExpectedResults
1a)     The merge resultant alpha values do not wrap through 256
1b)     The merge resultant alpha values closely match an equivalent floating-point calculation
2a)     The merge resultant colours do not wrap through 256
2b)     The merge resultant colours do not exceed the resultant alpha if premultiplied output is expected
2c)     The merge resultant colours closely match an equivalent floating-point calculation

Exhaustive test of Blend function for every pixel value. Tests combinations of source and destination
colour and alpha for a particular colour channel (0=red, 1= green, 2=blue). This method emulates the merge
operation in floating piont and compares the result with the real merge function. Generally the method is
intended to error-test premultiplied or non-multiplied, or permutations, based on the flags aInSrcPreMul,
aInDestPreMul,aOutDestPreMul.
@param  aInSrcPreMul if true the source pixel colour will be clipped and treated as premultiplied
@param  aInDestPreMul if true the destination pixel colour will be clipped and treated as premultiplied
@param  aOutDestPreMul if true the calculated pixel colour will be clipped and treated as premultiplied
@param  aStepSize specifies how many brightness levels are skipped. 1,3,5,15,17 are good values
@param  aChannelControl which channel is controlled  (0=red, 1= green, 2=blue)
@param  aOtherChannels value used for other channels. In PM modes this will be clipped to the mask value
@param  aTestMode mode of bitmap device. I presume it is consistant with the PreMul flags above
**/ 
void  CTDirectScreenBitmap::ExhaustiveTestMergeL(TBool aInSrcPreMul,TBool aInDestPreMul,TBool aOutDestPreMul,
         TInt aStepSize,TInt aChannelControl)
	{
	ExhaustiveTestMergePerDispModeL(EColor64K,aInSrcPreMul,aInDestPreMul, aOutDestPreMul, aStepSize,aChannelControl);
	ExhaustiveTestMergePerDispModeL(EColor4K,aInSrcPreMul,aInDestPreMul, aOutDestPreMul, aStepSize,aChannelControl);
	ExhaustiveTestMergePerDispModeL(EColor16M,aInSrcPreMul,aInDestPreMul, aOutDestPreMul, aStepSize,aChannelControl);
	ExhaustiveTestMergePerDispModeL(EColor16MU,aInSrcPreMul,aInDestPreMul, aOutDestPreMul, aStepSize,aChannelControl);
	ExhaustiveTestMergePerDispModeL(EColor16MA,aInSrcPreMul,aInDestPreMul, aOutDestPreMul, aStepSize,aChannelControl);
	ExhaustiveTestMergePerDispModeL(EColor16MAP,aInSrcPreMul,aInDestPreMul, aOutDestPreMul, aStepSize,aChannelControl);
	}



void CTDirectScreenBitmap::ExhaustiveTestMergePerDispModeL(TDisplayMode aDrawDeviceDispMode, TBool aInSrcPreMul,TBool aInDestPreMul,TBool aOutDestPreMul,
 		                                                                        TInt aStepSize,TInt aChannelControl,TInt aOtherChannels,
        	                                                                      TDisplayMode aTestMode)
	{
	CFbsDrawDevice *pDev = NULL;
	iDispMode = aDrawDeviceDispMode;
	TRAPD(err,pDev = CFbsDrawDevice::NewScreenDeviceL(0,aDrawDeviceDispMode));
	if (err !=KErrNone)
		{
		INFO_PRINTF2(_L("Note: Failed to create screen device for display mode %i - not supported on this config?"),iDispMode);
		}
	else
		{
		INFO_PRINTF2(_L("Test Exhaustive Merge for display mode %i "),iDispMode);
		User::LeaveIfError(pDev->InitScreen());
		pDev->SetAutoUpdate(ETrue);
		TSize screenSize = pDev->SizeInPixels();

		//Full screen non-incremental run
		TRect directRect(TPoint(0, 0), screenSize);
		INFO_PRINTF1(_L("PDEF099416: Test of pixel merge over full colour and alpha range"));
		
		TRAPD(err, ConstructL(directRect, CDirectScreenBitmap::ENone));
		if (err)
		   	{
		   	delete pDev;
		   	INFO_PRINTF1(_L("The Direct Screen Bitmap object is not created"));
		   	}
		else
		   	{
			if (!iInterface->ScreenClear())
				{
				INFO_PRINTF1(_L("The display mode of the iBitmapInfo is not expected"));
				delete pDev; 
				return;
				}
			else 
				{
				TSize bitmapSize(256,1);
				TUint32 bitmapMem[256];
				TUint32 srcLine[256];
				CFbsDrawDevice* bmd = CFbsDrawDevice::NewBitmapDeviceL(bitmapSize, aTestMode, CFbsBitmap::ScanLineLength(256, aTestMode));
				CleanupStack::PushL(bmd);
				//initialize
				bmd->SetAutoUpdate(EFalse);
				bmd->SetBits(bitmapMem);
				//Eveything printed to the screen is just to show the error levels graphically.
				//Patterns are very interesting sometimes
				if (screenSize.iHeight>200 && screenSize.iWidth>600)
					{
					iInterface->BeginDraw();
					iInterface->DrawColor(TRect(260,0,290,30),TRgb(255,0,0));
					iInterface->DrawColor(TRect(260,30,290,60),TRgb(100,0,0));
					iInterface->DrawColor(TRect(260,60,290,90),TRgb(100,0,0));
					iInterface->DrawColor(TRect(260,100,290,130),TRgb(0,255,0));
					iInterface->DrawColor(TRect(260,130,290,160),TRgb(0,200,0));
					iInterface->DrawColor(TRect(260,160,290,190),TRgb(0,100,0));
					iInterface->DrawColor(TRect(560,100,590,130),TRgb(0,0,255));
					iInterface->DrawColor(TRect(560,130,590,160),TRgb(0,0,200));
					iInterface->DrawColor(TRect(560,160,590,190),TRgb(0,0,100));
					iInterface->EndDraw(iRequestStatus);
					}
				//const
				TInt channelMask;
				if (aChannelControl<1)
					{
					channelMask=0x0000FF;	
					}
				else
					{
					if(aChannelControl==1) 
						{
						channelMask=0x00FF00;	
						}
					else
						{
						channelMask=0xFF0000;	
						}
					}
				const TInt otherMask=0xFFFFFF^channelMask;
				const TInt channelMul=channelMask&0x01010101;
				const TInt addFactor=aOtherChannels & otherMask;        //Use to set the other colour channels with a fixed test value
				TInt passFlag=0;
						
				INFO_PRINTF3(_L("AddFactor pass: channel %i others %x "),aChannelControl,addFactor);
				passFlag=passFlag^0x8000;
				const TReal KIgnore=0.00000001;//0.3;
				const TReal KGross=1.51;
				const TReal KMultiplyErrorBrightness=200.0;
				TReal errMax=20;
				TReal errAMax256=0;
				TReal totErrCol=0;
				TReal totErrAlpha=0;
				TReal zeroErrCol=0;
				TReal zeroErrAlpha=0;
				TInt  countAlphas=0;
				TInt countColours=0;
				const TInt stepFactor=aStepSize;     //1, 3, 5, 15, 17;  //This has a ^3 effect on speed
				TIgnoreSpecialCases  ignoreSpecialCases(aInSrcPreMul,aInDestPreMul,aOutDestPreMul);
				//bkgrdMask is background mask/alpha input value
					
				for (TInt bkgrdMask=0;bkgrdMask<256;bkgrdMask+=stepFactor)
				    {
					TInt logLine=-1;
					iInterface->BeginDraw();
					iInterface->DrawColour64KPixel(270+bkgrdMask/screenSize.iHeight,bkgrdMask%screenSize.iHeight, TRgb(addFactor|passFlag));
					TInt maxChannels=256;
					if (aInDestPreMul)
						{
						maxChannels=bkgrdMask+1;	
						}
					TInt clippedother=((addFactor-((bkgrdMask*0x010101)&otherMask))>>8)&otherMask;
					//clippedother is now 0x00FF00FF or 0x00000000 (or 0x00FF0000 or 0x000000FF)
					clippedother=(addFactor&(clippedother^otherMask))|(((bkgrdMask*0x010101)&clippedother));
					//bkgrdChannel is background channel input value. In PM it is 0...bkgrdMask. In NP it is 0...255
					for (TInt bkgrdChannel=0,colour=(bkgrdMask<<24)|clippedother,stepChannel=stepFactor*channelMul;bkgrdChannel<maxChannels;bkgrdChannel+=stepFactor,colour+=stepChannel) 
						{
						TInt failsPerPass=10;
					    logLine++;
					    if (logLine>=screenSize.iHeight)
					       	{
					       	logLine=0;	
					       	}
					    //srcMask is the source mask/alpha
					    for (TInt srcMask=0;srcMask<256;srcMask+=stepFactor)                //0 and 255 are special cases, but need testing anyway!
					    	{
					        TInt maxChannels=256;     //nested
					        if (aInSrcPreMul)
					           	{
					           	maxChannels=srcMask+1;      //In PM-PM source colour passes through unchanged, so skip the tests	
					           	}
					        bmd->WriteRgbMulti(0,0,maxChannels,1,TRgb(colour,bkgrdMask),CGraphicsContext::EDrawModeWriteAlpha);
					        TInt clippedother=((addFactor-((srcMask*0x010101)&otherMask))>>8)&otherMask;
					        //clippedother is now 0x00FF00FF or 0x00000000 (or 0x00FF0000 or 0x000000FF)
					        clippedother=(addFactor&(clippedother^otherMask))|(((srcMask*0x010101)&clippedother));

					        //srcChannel1 is the source  channel input value. In PM it is 0...srcMask. In NP it is 0...255
					        for (TInt srcChannel1=0,C=(srcMask<<24)+clippedother;srcChannel1<maxChannels;srcChannel1++,C+=channelMul)
					           	{
					            srcLine[srcChannel1]=C;
					            }
					        bmd->WriteLine(0,0,maxChannels,srcLine,CGraphicsContext::EDrawModePEN);
					        TReal errPos=0;
					        TReal errNeg=0;
					        TReal errGross=0;
					        TReal errAPos=0;
					        TReal errANeg=0;
					        TReal errAGross=0;
					        //source multiplier factor for alpha that can then be used to optimise non-multiplied input calculations.
					        TReal srcMultiplier=srcMask/255.0;
					        //destination/background multiplier factor for alpha that can then be used to optimise non-multiplied input calculations.
					        TReal destMultiplier=(bkgrdMask/255.0)*(1.0-srcMultiplier);
							//alphaPixelValue is the alpha pixel value as generated from the library code under test
					        TUint alphaPixelValue=bmd->ReadPixel(0,0).Alpha();
					        //alphaDiff is the difference in alpha between pixel and float calcuation, i.e. the error. This can be less than 1 level of brightness, i.e. insignificant.
					        TReal alphaDiff=0.0;
					        //pre-mul mode does not effect the alpha calculation
					        //alphaOutputValue is a floating-point calculation of the alpha output value using 255.0 as the scaling factor.
					        TReal alphaOutputValue=(srcMultiplier+destMultiplier)*255.0;
					        alphaDiff=alphaOutputValue-alphaPixelValue;
					        zeroErrAlpha+=alphaDiff;
					        if (alphaDiff>KIgnore || alphaDiff<-KIgnore)
					           	{
					           	if (alphaDiff>0)
					               	{
					                if (alphaDiff>KGross)
					                   	{
					                    if (--failsPerPass>0)
					                    	LogColourEvent(bkgrdMask,bkgrdChannel,srcMask,-1,alphaOutputValue,alphaPixelValue,alphaDiff,_L("Big Alpha error: expected %f, got %f"),ETrue);
					                    errAGross+=(alphaDiff-KIgnore)*KMultiplyErrorBrightness;
					                    }
					                else
					                   	{
					                   	errAPos+=(alphaDiff-KIgnore)*KMultiplyErrorBrightness;	
					                   	}
					                totErrAlpha+=alphaDiff;
					                }
					            else
					               	{
					                if(alphaDiff<-KGross)
					                  	{
					                   	if (--failsPerPass>0)
					                   		LogColourEvent(bkgrdMask,bkgrdChannel,srcMask,-1,alphaOutputValue,alphaPixelValue,alphaDiff,_L("Big Alpha error: expected %f, got %f"),ETrue);	
					                   	errAGross-=(alphaDiff+KIgnore)*KMultiplyErrorBrightness;
					                    }
					                else
					                   	{
					                   	errANeg-=(alphaDiff+KIgnore)*KMultiplyErrorBrightness;	
					                   	}
					                totErrAlpha-=alphaDiff;
					                }
					           	}
								TInt errA= STATIC_CAST(TInt,(errAPos-errANeg));
					            countAlphas++;
					            countColours+=maxChannels;
					            //The other channel values should not change while the tested channel is modulated...
					            //So I grab it's value for the zero case to compare.
					            TUint  otherChannels0=bmd->ReadPixel(0,0).Color16MA()&(otherMask|0xff000000);

					            //srcChannel is the source  channel input value. In PM it is 0...srcMask. In NP it is 0...255
					            for (TInt srcChannel=0;srcChannel<maxChannels;srcChannel++)
					              	{
					                //channelOutputValue is a floating-point calculation of the channel output value using 255.0 as the scaling factor.
					                TReal channelOutputValue;
					                if (aInSrcPreMul)
					                  	{
					                   	channelOutputValue=srcChannel;	
					                   	}
					                else
					                  	{
					                   	channelOutputValue=srcChannel*srcMultiplier;	
					                   	}
					                    	
					                if (aInDestPreMul)
					                   	{
					                   	channelOutputValue+=bkgrdChannel*(1.0-srcMultiplier);	
					                   	}
					                else
					                   	{
					                   	channelOutputValue+=bkgrdChannel*destMultiplier;	
					                   	}

					                if (!aOutDestPreMul)
					                   	{
					                   	if ((srcMultiplier+destMultiplier)!=0)
					                   		{
					                   		channelOutputValue=channelOutputValue/(srcMultiplier+destMultiplier);	
					                   		}
					                   	}
									TUint readPixel=bmd->ReadPixel(srcChannel,0).Color16MA();
									//channelPixelValue is the channel pixel value as generated from the library code under test
					                TUint channelPixelValue=(readPixel&channelMask)/channelMul;
					                if (aOutDestPreMul)
					                   	{
					                   	if (channelPixelValue>alphaPixelValue)
					                       	{
					                           if (!ignoreSpecialCases(bkgrdMask,bkgrdChannel,srcMask,srcChannel))
					                           	{
					                           	if (--failsPerPass>0)
					                           		{
					                           		LogColourEvent(bkgrdMask,bkgrdChannel,srcMask,srcChannel,alphaPixelValue,channelOutputValue,channelPixelValue,_L("Output multiplied colour exceeds alpha %f: expected %f got %f"),!ignoreSpecialCases(bkgrdMask,bkgrdChannel,srcMask,srcChannel));
					                           			
					                           		}
					                           	}
					                           errGross+=10;   //output value out of range - bright red spot!
					                           }	
					                    	}
					                    TUint   otherChannels=readPixel&(otherMask|0xff000000);
					                    if (otherChannels!=otherChannels0)
					                    	{       //Other channels should all be constant here!
					                    	LogColourEvent(bkgrdMask,bkgrdChannel,srcMask,srcChannel,otherChannels0,otherChannels,0,_L("Output for other channels changed when modulating test channel only - Inter-channel leakage? srcChannel=0 value = %f, this value = %f"),ETrue);
					                    	}
									//channelDiff is the difference in channel between pixel and float calcuation, i.e. the error. This can be less than 1 level of brightness, i.e. insignificant.
					                    TReal channelDiff=channelOutputValue-channelPixelValue;
					                    zeroErrCol+=channelDiff;

					                    if (channelDiff>KIgnore || channelDiff<-KIgnore)
					                     	if (channelDiff>0)
					                        	{
					                            if (channelDiff>KGross)
					                               	{
					                                if (!ignoreSpecialCases(bkgrdMask,bkgrdChannel,srcMask,srcChannel))
					                                	{
					                                	if (--failsPerPass>0)
					                                		{
					                                		LogColourEvent(bkgrdMask,bkgrdChannel,srcMask,srcChannel,channelOutputValue,channelPixelValue,channelDiff,_L("Big Colour error: expected %f, got %f"),!ignoreSpecialCases(bkgrdMask,bkgrdChannel,srcMask,srcChannel));		
					                                		}
					                                	}
					                                errGross+=channelDiff-KIgnore;
					                                }
					                            else
					                            	{
					                            	errPos+=channelDiff-KIgnore;
					                            	}
					                            totErrCol+=channelDiff;
					                            }
					                         else
					                         	{
					                            if (channelDiff<-KGross)
					                            	{
					                            	if (!ignoreSpecialCases(bkgrdMask,bkgrdChannel,srcMask,srcChannel))
					                            		{
					                            		if (--failsPerPass>0)
					                            			{
					                            			LogColourEvent(bkgrdMask,bkgrdChannel,srcMask,srcChannel,channelOutputValue,channelPixelValue,channelDiff,_L("Big Colour error: expected %f, got %f"),!ignoreSpecialCases(bkgrdMask,bkgrdChannel,srcMask,srcChannel));	
					                            			}
					                            		}
					                                errGross-=channelDiff+KIgnore;
					                                }
					                             else
					                             	{
					                             	errNeg-=channelDiff+KIgnore;	
					                             	}
					                             	
					                             totErrCol-=channelDiff;
					                         	}
					                     	}

									TReal err=errPos-errNeg;
					                errGross+=errAGross;
					                if (errA>errAMax256)
					                	{
					      	            //LogColourEvent(bkgrdMask,bkgrdChannel,srcMask,-1,errA,0,0,_L("Row alpha error level increase, now: %f"),EFalse);
					        			errAMax256=errA;
					                    }
					                errPos=Min(TReal(255),(errPos*KMultiplyErrorBrightness/TReal(maxChannels)));
					                errNeg=Min(TReal(255),(errNeg*KMultiplyErrorBrightness/TReal(maxChannels)));
					                TReal err256=Min(TReal(255),err*KMultiplyErrorBrightness/TReal(maxChannels));
					                if (err256>errMax)
					                	{
					      	            //LogColourEvent(bkgrdMask,bkgrdChannel,srcMask,-1,err256,err,0,_L("Row colour error level increase, per call now: %f value for row: %f"),EFalse);
					        			errMax=err256;
					                    }

					               	errAPos=Min(TReal(255),(errAPos));
					                errANeg=Min(TReal(255),(errANeg));
					                errA=Min(255,errA);
					                if (errGross>10)
					                	{
					                	errGross=TReal(255);	
					                	}
					               	else
					               		{
					               		errGross*=TReal(20);	
					               		}
					            	TRect pix(TPoint(),TSize(1,1)); 
					                if (screenSize.iWidth>260) 
					                	{
					                	iInterface->DrawColour64KPixel(srcMask,logLine, TRgb(STATIC_CAST(TInt,errGross),STATIC_CAST(TInt,errNeg),STATIC_CAST(TInt,errPos)));	
					                	}
									if (screenSize.iWidth>360)
										{
										iInterface->DrawColour64KPixel(srcMask+300,logLine, TRgb(STATIC_CAST(TInt,errGross),STATIC_CAST(TInt,errNeg),STATIC_CAST(TInt,errPos)));	
										}
					    			}
					    		
								if (failsPerPass<0) 
					    			{       //note that this count may be out by 1... 
					      			INFO_PRINTF2(_L("Additional %i errors not reported in this pass."),-failsPerPass); 
					      			} 
								} 

							iInterface->EndDraw(iRequestStatus);
				    		}
							 
					    if (ignoreSpecialCases.IgnoreCount() && ignoreSpecialCases.IgnoreCount()<ignoreSpecialCases.ExpectedIgnoreCount(aStepSize)) 
					       	{ 
					        TEST(ignoreSpecialCases.IgnoreCount()<ignoreSpecialCases.ExpectedIgnoreCount(aStepSize)); 
					        INFO_PRINTF3(_L("There were less ignored special-case errors than exepected (but more than zero):  Expected: 0 or %i, got %i"), 
								ignoreSpecialCases.ExpectedIgnoreCount(aStepSize),ignoreSpecialCases.IgnoreCount()			
							);
					       	} 
					    INFO_PRINTF4(_L("Highest error rows (normalised @%f per row):  Alpha: %f, Colour: %f "),KMultiplyErrorBrightness,errMax,errAMax256); 
					    INFO_PRINTF4(_L("Alpha:  Samples: %i, total abs= %f, total signed=%f (should be 0)"),countAlphas,totErrAlpha,zeroErrAlpha); 
					    INFO_PRINTF4(_L("Colour: Samples: %i, total abs= %f, total signed=%f (should be 0)"),countColours,totErrCol,zeroErrCol); 

					    CleanupStack::PopAndDestroy(bmd); 
					  	delete pDev; 
				}
		
			}
		}
	} 

void CTDirectScreenBitmap::RunTestCaseL(TInt aCurTestCase)
	{
		((CTDirectScreenBitmapStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		INFO_PRINTF1(_L("INC0594703 : TestRefreshingWindowsL"));
		((CTDirectScreenBitmapStep*)iStep)->SetTestStepID(_L(" GRAPHICS-CTDirectScreenBitmap-TestRefreshingWindowsL-0001"));
		TestRefreshingWindowsL();
		break;
	case 2:
		INFO_PRINTF1(_L("Test Continuous Refreshing Frames"));
		((CTDirectScreenBitmapStep*)iStep)->SetTestStepID(_L(" GRAPHICS-CTDirectScreenBitmap-TestRefreshingWindowsL-0001"));
		TestContinuousRefreshingL();
		break;
	case 3:
		INFO_PRINTF1(_L("Exhaustive merge test"));
		((CTDirectScreenBitmapStep*)iStep)->SetTestStepID(_L(" GRAPHICS-CTDirectScreenBitmap-ExhaustiveTestMergeL-0001"));
		ExhaustiveTestMergeL(EFalse,EFalse,ETrue,5,0);
		((CTDirectScreenBitmapStep*)iStep)->RecordTestResultL();
		((CTDirectScreenBitmapStep*)iStep)->SetTestStepID(_L(" GRAPHICS-CTDirectScreenBitmap-ExhaustiveTestMergeL-0001"));
		ExhaustiveTestMergeL(EFalse,EFalse,ETrue,5,1);
		((CTDirectScreenBitmapStep*)iStep)->RecordTestResultL();
		((CTDirectScreenBitmapStep*)iStep)->SetTestStepID(_L(" GRAPHICS-CTDirectScreenBitmap-ExhaustiveTestMergeL-0001"));
		ExhaustiveTestMergeL(EFalse,EFalse,ETrue,5,2);
		((CTDirectScreenBitmapStep*)iStep)->RecordTestResultL();
		break;
	case 4:
		INFO_PRINTF1(_L("Test refreshing time"));
        	((CTDirectScreenBitmapStep*)iStep)->SetTestStepID(_L(" GRAPHICS-CTDirectScreenBitmap-TestRefreshingWindowsL-0001"));
		TestRefreshingTimeL();
		break;
	default:
		((CTDirectScreenBitmapStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTDirectScreenBitmapStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTDirectScreenBitmapStep*)iStep)->RecordTestResultL();
	}

//--------------
__CONSTRUCT_STEP__(DirectScreenBitmap)

void CTDirectScreenBitmapStep::TestSetupL()
	{
	TInt temp = 0;
	User::LeaveIfError(HAL::Get(HALData::EDisplayColors, temp));//force HAL memory allocation
	}
