// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Screen device test code
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TSCRDEVRESSWITCH.H"

//Define this to get visible pauses (in test 6: DeviceResSwitchL)
//#define VISIBLE_PAUSES

//Define this to get extra logging which may be useful in tracking down a fail
//#define EXTRA_LOGGING

CTScreenDeviceResSwitch::CTScreenDeviceResSwitch(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{}

CTScreenDeviceResSwitch::~CTScreenDeviceResSwitch()
	{}

void CTScreenDeviceResSwitch::ConstructL()
	{
	//The following is just another test... it doesn't leave any resources for use by the test class AFAICT...
	RWsSession aSession;
	CWsScreenDevice *device1;
	CWsScreenDevice *device2;
	CWsScreenDevice *device3;

	aSession.Connect();
	device1=new(ELeave) CWsScreenDevice(aSession);
	device1->Construct(iTest->iScreenNumber);
	delete device1;
	device1=new(ELeave) CWsScreenDevice(aSession);
	device1->Construct(iTest->iScreenNumber);
	device2=new(ELeave) CWsScreenDevice(aSession);
	device2->Construct(iTest->iScreenNumber);
	device3=new(ELeave) CWsScreenDevice(aSession);
	device3->Construct(iTest->iScreenNumber);
	delete device3;
	CFbsFont *font;
	User::LeaveIfError(device1->GetNearestFontToDesignHeightInTwips((CFont *&)font,TFontSpec()));
	RWindowGroup group(aSession);
	group.Construct(777);
	group.SetOwningWindowGroup(TheClient->iGroup->GroupWin()->Identifier());
	RWindow win(aSession);
	win.Construct(group,77);
	CWindowGc *gc=new(ELeave) CWindowGc(device1);
	gc->Construct();
	gc->Activate(win);
	gc->UseFont(font);
	device1->ReleaseFont(font);
	aSession.Flush();
	delete gc;
	win.Close();
	group.Close();
	delete device1;
	delete device2;
	aSession.Close();
	}

	
class CPrimaryColoursWin : public CTWin
	{
public:
	enum	//various size factors	
		{  	
			kShrinkFactor=5,
			kPlotSize=16,
			kPlotMargin=4,
			kPlotWithMargin=kPlotSize+kPlotMargin,
			kPlotsAccross=3,
			kPlotsDown=1,
			kMinWidth=kPlotWithMargin*kPlotsAccross+kPlotMargin,
			kMinHeight=kPlotWithMargin*kPlotsDown+kPlotMargin,
		    KNumChannels=3,
		    KNumColours=256
		};
		
	CPrimaryColoursWin();
	~CPrimaryColoursWin();
	//Virtual Function from CTBaseWin
	void Draw();
	TInt CountUniquePlottedColours();
	TInt iDrawn;
	TInt iNumColours;
	TBool iBadPixels[KNumChannels][KNumColours];
	TBuf<0x40> iDisplayText;
	};
	
CPrimaryColoursWin::CPrimaryColoursWin()
	{
	for (TInt channelnum=0;channelnum<KNumChannels;channelnum++)
		{
		for (TInt colour=0;colour<KNumColours;colour++)
			{
			iBadPixels[channelnum][colour]=EFalse;
			}
		}
		
	iDisplayText.Zero();
	iDisplayText.Format(_L("Test text"));
	}
	
CPrimaryColoursWin::~CPrimaryColoursWin()
	{

	}
	
void CPrimaryColoursWin::Draw()
	{
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iGc->SetPenStyle(CGraphicsContext::ESolidPen);
	iGc->SetPenColor(TRgb(255, 255, 255));
	iGc->SetBrushColor(TRgb(0, 0, 0));
	TSize winSize = Size();
	iGc->DrawRect(TRect(winSize));
	
	CFont* font;
	TFontSpec fontSpec(_L(""), 300);
	TheClient->iScreen->GetNearestFontInTwips(font, fontSpec);
	
	if (font)
		{
		iGc->UseFont(font);
		TRect r(TPoint(0, 0), Size());
		r.Shrink(kMinHeight, kMinHeight);
		iGc->DrawText(iDisplayText, r, font->AscentInPixels(), iGc->ECenter, 0);
		iGc->DiscardFont();
		TheClient->iScreen->ReleaseFont(font);
		}
	
	iNumColours = 0;
	TPoint lhsAbs = Win()->AbsPosition();
	
	for(TInt channelnum = 0, channelmul = 1, xoordinate = kPlotMargin; channelnum < KNumChannels; channelnum++, channelmul <<= 8, xoordinate += kPlotWithMargin)
		{
		TRgb lastPixel(255, 255, 255, 255);
		
		for(TInt colour = 0; colour < KNumColours; colour++)
			{
			if(!iBadPixels[channelnum][colour])
				{
				iGc->SetPenColor(TRgb(colour * channelmul));				
				}
			else
				{
				iGc->SetPenColor(TRgb(255, 255, 255));			
				}

			TPoint point = TPoint(xoordinate + (colour & 0x0f), kPlotMargin + (colour >> 4));
			iGc->Plot(point);
			}
		}
		
	iDrawn=ETrue;
	}
	
TInt CPrimaryColoursWin::CountUniquePlottedColours()
	{
	iNumColours = 0;
	TPoint lhsAbs = Win()->AbsPosition();
	
	for(TInt channelnum = 0, channelmul = 1, xoordinate = kPlotMargin; channelnum < 3; channelnum++, channelmul <<=8 , xoordinate += kPlotWithMargin)
		{
		TRgb lastPixel(255, 255, 255, 255);
		
		for(TInt colour = 0; colour < 256; colour++)
			{
			TRgb readPixel;
 			TPoint point = TPoint(xoordinate + (colour & 0x0f), kPlotMargin + (colour >> 4));
			TheClient->iScreen->GetPixel(readPixel, lhsAbs + point);
			
			if(readPixel != lastPixel)
				{
				lastPixel = readPixel;
				iNumColours++;
				iBadPixels[channelnum][colour] = EFalse;
				}
			else
				{
				iBadPixels[channelnum][colour] = ETrue;
				}
			} // for loop 
		} // for loop
		
	return iNumColours;
	}
	
/**
	Intended primarily as a visual check that the mode is displayed correctly,
	for each rotated mode.
**/
void    CTScreenDeviceResSwitch::DeviceResSwitchWithRotationsL()
	{
	CWsScreenDevice *screen=TheClient->iScreen;
	TInt originalScreenMode = screen->CurrentScreenMode();
	TPixelsTwipsAndRotation originalModeSettings;
	screen->GetScreenModeSizeAndRotation(originalScreenMode,originalModeSettings);
	CArrayFixFlat<TInt> *rotations=new(ELeave) CArrayFixFlat<TInt>(1);
	CleanupStack::PushL(rotations);
	INFO_PRINTF2(_L("ScreenMode and rotation with colour depth. ScreenModes=%i"),TheClient->iScreenModes.Count());
	
	for (TInt rr=0,maxrr=3;rr<maxrr;++rr)
	for (TInt ii=0,maxii=TheClient->iScreenModes.Count();ii<maxii;++ii)
		{
		TInt newMode=TheClient->iScreenModes[ii];
		if (	screen->GetScreenModeOrigin(ii)!=TPoint(0,0)	||	screen->GetScreenModeScale(ii)!=TSize(1,1)	)
			{
			INFO_PRINTF2(_L("ScreenMode %i skipped: has scalind and/ or rotation"),ii);
			INFO_PRINTF1(_L("This scaling code is known to be broken if all modes do not support scaling"));
			INFO_PRINTF1(_L("See defect DEF111847 and break request 2226"));
			INFO_PRINTF5(_L("Origin: %i %i Scale %i %i"),
					screen->GetScreenModeOrigin(ii).iX,screen->GetScreenModeOrigin(ii).iY,
					screen->GetScreenModeScale(ii).iWidth,screen->GetScreenModeScale(ii).iHeight
				);
			continue;
			}
			
		screen->SetAppScreenMode(newMode);
		screen->SetScreenMode(newMode);
		TInt currentScreenMode = screen->CurrentScreenMode();
		TEST(currentScreenMode == newMode);

		User::LeaveIfError(screen->GetRotationsList(newMode,rotations));
		INFO_PRINTF4(_L("ScreenMode and rotation with colour depth. Mode#%i=%i. Rotations=%i"),ii,newMode,rotations->Count());
		TPixelsTwipsAndRotation currentModeSettings;
		screen->GetDefaultScreenSizeAndRotation(currentModeSettings);
		screen->SetScreenSizeAndRotation(currentModeSettings);
		for (TInt jj=0,maxjj=rotations->Count();jj<maxjj;jj++)
			{
			CFbsBitGc::TGraphicsOrientation newOrientation=STATIC_CAST(CFbsBitGc::TGraphicsOrientation,rotations[0][jj]);
			INFO_PRINTF3(_L("ScreenMode and rotation with colour depth. Rotation#%i=%i(0..3)"),jj,newOrientation);
			screen->SetCurrentRotations(newMode,newOrientation);
			TheClient->iWs.Flush();
			
			TPixelsAndRotation modeSettings;
			TheClient->iScreen->GetDefaultScreenSizeAndRotation(modeSettings);
			TRect screenRect(TPoint(0,0),modeSettings.iPixelSize);

			//move the debug windows to inside the area 
			//actually don't bother yet!
			if (screenRect.Width()>screenRect.Height())
				{
				}
			else
				{
				}
			DeviceResSwitchL();
			}
		screen->SetCurrentRotations(newMode,currentModeSettings.iRotation);
		screen->SetScreenSizeAndRotation(currentModeSettings);
		}
	
	CleanupStack::PopAndDestroy();
	screen->SetScreenMode(originalScreenMode);
	screen->SetCurrentRotations(originalScreenMode,originalModeSettings.iRotation);
	screen->SetScreenSizeAndRotation(originalModeSettings);
	}
	
/**
	Intended primarily as a visual check that the mode is displayed correctly, 
	this code also verifies that the number of physical colours matches the reported mode.
	Note that although written to test GCE, this test is general and applies to all display versions.
**/	
void CTScreenDeviceResSwitch::DeviceResSwitchL()
	{		
	INFO_PRINTF1(_L("DeviceResSwitchL: Entering function"));
	TInt error = KErrNone;
	TInt isTransparencySupportedResult = KErrNone;
		
	TRAP(error, isTransparencySupportedResult = IsTransparencySupportedL());
	
	if(error != KErrNone)
		{
		INFO_PRINTF1(_L("DeviceResSwitchL: Transparency is not supported. Exits."));
		return;
		}	
	
	TRAP(error, CalculateDisplayPropertiesL());
	
	if(error != KErrNone)
		{
		INFO_PRINTF1(_L("DeviceResSwitchL: Could not calculate display properties. Test not supported. Exits."));
		return;		
		}	

	TDisplayMode startDisplayMode = TheClient->iScreen->DisplayMode();
	TInt startColoursPixel = TDisplayModeUtils::NumDisplayModeColors(startDisplayMode);
		
	TPixelsAndRotation modeSettings;
	TheClient->iScreen->GetDefaultScreenSizeAndRotation(modeSettings);
	TRect r(TPoint(0, 0), modeSettings.iPixelSize);

	// Starts off full-screen. Only shrink it if it will still be large enough to run the test
	// It should be... the test only needs 60x20 pixels
	
	if(r.Width() > r.Height())
		{
		if(r.Width() > (CPrimaryColoursWin::kMinWidth) * (CPrimaryColoursWin::kShrinkFactor-2) * 3 / (CPrimaryColoursWin::kShrinkFactor * 2))
			{
			r.iTl.iX = r.iBr.iX / 3;				
			}
		}
	else
		{
		if (r.Height() > (CPrimaryColoursWin::kMinHeight) * (CPrimaryColoursWin::kShrinkFactor - 2) * 3/ (CPrimaryColoursWin::kShrinkFactor * 2))
			{
			r.iTl.iY = r.iBr.iY / 3;				
			}
		}
	if(r.Width() > (CPrimaryColoursWin::kMinWidth) * (CPrimaryColoursWin::kShrinkFactor - 2) / CPrimaryColoursWin::kShrinkFactor)
		{
		if(r.Height() > (CPrimaryColoursWin::kMinHeight) * (CPrimaryColoursWin::kShrinkFactor - 2) / CPrimaryColoursWin::kShrinkFactor)
			{
			r.Shrink(r.Width() / CPrimaryColoursWin::kShrinkFactor, r.Height() / CPrimaryColoursWin::kShrinkFactor);
			}
		}
		
	for(TInt i = 0; i < EColorLast; i++)
		{
		TDisplayMode tryMode = TDisplayMode(i);
		TInt tryColoursPixel = NumDisplayModeColors(tryMode);		
		
		INFO_PRINTF3(_L("DeviceResSwitchL: tryColoursPixel = %d, tryMode = %d"), tryColoursPixel, tryMode);
		
		if(TDisplayModeUtils::IsDisplayModeColor(tryMode) && startColoursPixel <= tryColoursPixel)
			{
				  //Create a test window at this mode, and see if it changes the screen mode
				{ // The braces define the lifetime of testWin. It must be destroyed before we check if mode changed back successfully.
				
				CPrimaryColoursWin* testWin1 = new (ELeave) CPrimaryColoursWin;
				CleanupStack::PushL(testWin1);
				
				TInt expectedColoursPerChannel = 1;
				TInt tt;
				
				testWin1->SetUpL(r.iTl, r.Size(), TheClient->iGroup, *TheClient->iGc, &tryMode);
				TheClient->iWs.Flush();
				TDisplayMode newDisplayMode = TheClient->iScreen->DisplayMode();
				
				TEST(TDisplayModeUtils::NumDisplayModeColors(newDisplayMode) >= tryColoursPixel);	
				
				if(!(TDisplayModeUtils::NumDisplayModeColors(newDisplayMode) >= tryColoursPixel))
					{
					ERR_PRINTF3(_L("testWin1: newDisplayMode = %d, tryColoursPixel = %d"), newDisplayMode, tryColoursPixel);
					}
				
				// Estimate the minimum number of shades of primary colours given the bits per pixel.
				// The maximum is twice this. Very appoximate but seems to work OK for 256 colours. Probably not good for grey modes. 
				for(tt = tryColoursPixel; tt >= 8; tt >>= 3)
					{
					expectedColoursPerChannel <<= 1;
					}
					
				// Draw some test data on the test window.
			    testWin1->DrawNow();
				TheClient->iWs.Flush();
				 
#ifdef VISIBLE_PAUSES
			    TheClient->StdLogWindow().LogMessage(EFalse, _L("Mode: "), tryMode);
				User::After(1000000);
#endif 
				TInt numUniqueColours = testWin1->CountUniquePlottedColours();
				INFO_PRINTF2(_L("testWin1: numUniqueColours = %d"), numUniqueColours);
			    testWin1->DrawNow();
			    TheClient->StdLogWindow().LogMessage(EFalse, _L("Channel Colours: "), numUniqueColours);
				TheClient->iWs.Flush();
				
#ifdef VISIBLE_PAUSES
				User::After(1000000); 
#endif
				// Read it back and see if it has the expected quality
				TEST(numUniqueColours >= (expectedColoursPerChannel * 3));
				
				if(!(numUniqueColours >= (expectedColoursPerChannel * 3)))
					{
					ERR_PRINTF3(_L("testWin1: numUniqueColours = %d, (expectedColoursPerChannel * 3) = %d"), numUniqueColours, (expectedColoursPerChannel * 3));
					}				
		
				/*
				 * Defect 107176 was rejected. This test for transparency is therefore removed.
				 * 
				 */
				if (false) //newDisplayMode != startDisplayMode)	 // Hide the window under a startmode window and see if we switch back?
				if (isTransparencySupportedResult==KErrNone && newDisplayMode != startDisplayMode)	 // Hide the window under a startmode window and see if we switch back?
				/*
				 * Defect 107176 was rejected. This test for transparency is therefore removed.
				 * The crash demonstrated by this code is related to the inconsistant support for origin and scale
				 * See defect DEF111847 and break request 2226
				 */
				if (false) //newDisplayMode != startDisplayMode)	 // Hide the window under a startmode window and see if we switch back?
					{
					// Demonstration of defect 107176
					// Create a translucent window which obscures the high-colour window
					// The existing code reduces the display colour depth because it thinks the obscured window is not visible any more
					// However, the obscured window is actually visible trough the transparency
					CPrimaryColoursWin* testWin2 = new (ELeave) CPrimaryColoursWin;
					CleanupStack::PushL(testWin2);
	
					testWin2->SetUpL(r.iTl - TPoint(20,20), r.Size() + TSize(40,40), TheClient->iGroup, *TheClient->iGc, &startDisplayMode, ETrue, 0x80);
					TheClient->iWs.Flush();
				    testWin2->DrawNow();
					TheClient->iWs.Flush();
					
					TDisplayMode newnewDisplayMode = TheClient->iScreen->DisplayMode();
					TInt newNumUniqueColours = testWin2->CountUniquePlottedColours();	
					INFO_PRINTF2(_L("testWin2: newNumUniqueColours = %d"), newNumUniqueColours);		
					
					TEST(newnewDisplayMode == newDisplayMode);				
				  
					if(!(newnewDisplayMode == newDisplayMode))
						{
						ERR_PRINTF3(_L("testWin2: newnewDisplayMode = %d, newDisplayMode = %d"), newnewDisplayMode, newDisplayMode);
						}
					  
				    testWin2->DrawNow();
					TheClient->iWs.Flush();
					
#ifdef VISIBLE_PAUSES
					User::After(1000000); 
#endif
					TheClient->iWs.Flush();
					
					CleanupStack::PopAndDestroy(testWin2);
					testWin2 = NULL;						
					}
					
					CleanupStack::PopAndDestroy(testWin1);
					testWin1 = NULL;
				}
			 
				TDisplayMode afterDisplayMode = TheClient->iScreen->DisplayMode();
				TEST(afterDisplayMode == startDisplayMode);
				
				if(afterDisplayMode != startDisplayMode)
					{
					ERR_PRINTF3(_L("DeviceResSwitchL: Original colour depth not restored. Was %i, now %i (TDisplayMode)"), startDisplayMode, afterDisplayMode);						
					}

#ifdef VISIBLE_PAUSES
				User::After(1000000); 
#endif			
			} // context 
		} // for loop ends
		
	INFO_PRINTF1(_L("DeviceResSwitchL: Returning from function"));		
	}
	
void CTScreenDeviceResSwitch::CalculateDisplayPropertiesL()
	{
	INFO_PRINTF1(_L("CalculateDisplayPropertiesL: Entering function"));
	
	TDisplayMode tryMode = (TDisplayMode) (EColorLast - 1);
	TPixelsAndRotation modeSettings;
	TheClient->iScreen->GetDefaultScreenSizeAndRotation(modeSettings);
	TRect r(TPoint(0, 0), modeSettings.iPixelSize);

	CPrimaryColoursWin* tempWin = new (ELeave) CPrimaryColoursWin;
	CleanupStack::PushL(tempWin);
	
	tempWin->SetUpL(r.iTl, r.Size(), TheClient->iGroup, *TheClient->iGc, &tryMode);
	TheClient->iWs.Flush();
	
	iMaxDisplayMode = TheClient->iScreen->DisplayMode();
	INFO_PRINTF2(_L("CalculateDisplayPropertiesL: iMaxDisplayMode %d"), iMaxDisplayMode);
	iMaxDisplayModeColors = TDisplayModeUtils::NumDisplayModeColors(iMaxDisplayMode);
	INFO_PRINTF2(_L("CalculateDisplayPropertiesL: iMaxDisplayModeColors %d"), iMaxDisplayModeColors);	
	
	CleanupStack::PopAndDestroy(tempWin);
	INFO_PRINTF1(_L("CalculateDisplayPropertiesL: Returning from function"));
	}
	
TInt CTScreenDeviceResSwitch::NumDisplayModeColors(TDisplayMode aDispMode)
	{
	TInt dispModeColors = TDisplayModeUtils::NumDisplayModeColors(aDispMode);
	
	if(dispModeColors > iMaxDisplayModeColors)
		{
		return iMaxDisplayModeColors;
		}
		
	return dispModeColors;
	}
	
TInt CTScreenDeviceResSwitch::IsTransparencySupportedL()
	{
	INFO_PRINTF1(_L("IsTransparencySupportedL: Entering function"));
	// Creates a window and sets the transparency, if this feature
	// is not enabled, KErrNotSupported will be returned
	const TRgb KTransparencyColor(85,85,85);		
	RWindow win(TheClient->iWs);
	win.Construct(*TheClient->iGroup->GroupWin(), ENullWsHandle);	
	win.SetExtent(TPoint(0,0), TSize(50,50));	
	win.SetRequiredDisplayMode(EColor256);
	TInt ret = win.SetTransparencyFactor(KTransparencyColor);
	win.Close();
	
	if(!ret)
		{
		INFO_PRINTF1(_L("IsTransparencySupportedL: Transparency is supported"));
		}
	else
		{
		INFO_PRINTF1(_L("IsTransparencySupportedL: Transparency is not supported"));
		}
	
	INFO_PRINTF1(_L("IsTransparencySupportedL: Returning from function"));
	return ret;
	}

void CTScreenDeviceResSwitch::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	((CTScreenDeviceResSwitchStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0440

@SYMDEF             DEF107176

@SYMTestCaseDesc    Intended primarily as a visual check that the mode is displayed correctly. Checks correct display mode is set.
                    The test also verifies that the number of physical colours matches the reported mode.
                    Note that although written to test GCE, this test is general and applies to all display versions.               

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates a windows with a higher display mode. Then creates a transparent
                    window with lower display mode that completely covers the first window.
                    Checks the display mode is not changed to the lower diplay mode when 
                    the higher display mode window is visible through transparency.

@SYMTestExpectedResults The higher display mode should still be set after the second window is drawn.
*/
		case 1:
			((CTScreenDeviceResSwitchStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0440"));
			iTest->LogSubTest(_L("Device resolution switching"));
			DeviceResSwitchL();		
			break;			
/**
@SYMTestCaseID		GRAPHICS-WSERV-0520

@SYMTestCaseDesc	Test for device switching when mode increased.

@SYMTestActions     Windows are created in increasing modes and pixel colours written and read back.
					This code verifies that the mode change actually takes place.
					I am using this visually to verify that the new GCE is actually changing the mode.

**/
		case 2:
			((CTScreenDeviceResSwitchStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0520"));
			iTest->LogSubTest(_L("More device resolution switching"));
			DeviceResSwitchWithRotationsL();
			break;
		default:
			((CTScreenDeviceResSwitchStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTScreenDeviceResSwitchStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
		}
	((CTScreenDeviceResSwitchStep*)iStep)->RecordTestResultL();
	}

__WS_CONSTRUCT_STEP__(ScreenDeviceResSwitch)
