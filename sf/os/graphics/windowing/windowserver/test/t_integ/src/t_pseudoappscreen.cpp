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
//

/**
 @file
 @test
 @internalComponent
*/

#include "t_pseudoappscreen.h"
#include "t_pseudoapputils.h"
#include "t_pseudoapptestresults.h"
#include "t_pseudoappeng.h"

CTestScreen* CTestScreen::NewL(TInt aScreenNo, TDisplayMode aMode, TInt aFrameDuration, const TSize& aScreenSize, TGceTestResults* aGceTestResults,
							   const TDesC& aConfigFileName, CTPseudoAppShared& aPseudoAppShared)
    {
	RDebug::Print(_L("Creating CTestScreen class\n"));
    CTestScreen* self = new (ELeave) CTestScreen(aPseudoAppShared);
    CleanupStack::PushL(self);
    self->ConstructL(aScreenNo, aMode, aFrameDuration, aScreenSize, aGceTestResults, aConfigFileName);
    CleanupStack::Pop(); // self;
    return self;
    }

void CTestScreen::ConstructL(TInt aScreenNo, TDisplayMode aMode, TInt aFrameDuration, const TSize& aScreenSize, TGceTestResults* aGceTestResults,
							const TDesC& aConfigFileName)
	{
	RDebug::Print(_L("Constructing CTestScreen for screen %d\n"), aScreenNo);

	iPopupOccurances = 0;
	iFrameDuration = aFrameDuration;
	iScreenSize = aScreenSize;

	//Create CIniData class for reading in values form ini files
	iUtils = CIniData::NewL(aConfigFileName);

	ImportScreenConfigL(aScreenNo, aMode, aFrameDuration, aScreenSize, aGceTestResults, aConfigFileName);
	}

CTestScreen::~CTestScreen()
	{
	if (iWindAndSurf)
		{
		iWindAndSurf->ResetAndDestroy();
		delete iWindAndSurf;
		}

	delete iUtils;
	}

CTestScreen::CTestScreen(CTPseudoAppShared& aPseudoAppShared)
: iPseudoAppShared(aPseudoAppShared)
	{
	}

EXPORT_C void CTestScreen::UpdateL(SurfaceDetails& aSurfDetails)
	{
	//Update all windowed surfaces changes
	for(TInt index1=0; index1<iWindAndSurf->Count(); index1++)
		{
		(*iWindAndSurf)[index1]->UpdateL(aSurfDetails);
		}

	//Launch popup window if configured by ini file
	for(TInt index2=1; index2<(iPopupOccurances+1); index2++)
		{
		if(iFrameCounter == iFrameDuration/(2*iPopupOccurances*index2))
			{
			CreatePopupWindow();
			}
		else if(iFrameCounter == (iFrameDuration/(2*iPopupOccurances*index2) + iFrameDuration/(4*iPopupOccurances)))
			{
			DestroyPopupWindow();
			}
		}

	iFrameCounter++;
	}

EXPORT_C TBool CTestScreen::Rotation()
	{
	return iRotation;
	}

EXPORT_C void CTestScreen::RotateL(TInt aScreenNo, TInt aFrameNumber)
	{
	//Only rotation of one screen supported
	RWsSession session;
	TInt err = session.Connect();
	if (err!=KErrNone)
		RDebug::Print(_L("Error connecting the session error = %d\n"),err);	

	//Determine the screen dimensions
	CWsScreenDevice* screenDevice = NULL;
	TRAP(err,screenDevice = new(ELeave) CWsScreenDevice(session));
	if (err!=KErrNone)
		RDebug::Print(_L("Error creating screen device = %d\n"),err);	
		
	err = screenDevice->Construct(aScreenNo);
	if (err!=KErrNone)
		RDebug::Print(_L("Error constructing the screen device error = %d\n"),err);	

	TSize aScreenSize = screenDevice->SizeInPixels();

	//if screen dimensions have not changed (eg flipped) take no action
	//Rotation only supported for full screen window and surface
	if(iScreenSize != aScreenSize)
		{
		for(TInt i=0; i<iWindAndSurf->Count(); i++)
			{
			if( (*iWindAndSurf)[i]->RotationSupported() )
				{
				(*iWindAndSurf)[i]->RotateL(aScreenNo, i+1, aScreenSize, aFrameNumber);
				}
			else
				{
				RDebug::Print(_L("Rotation is not supported for this animation type"));
				User::Panic(KTPseudoAppPanicTxt, KErrNotSupported);
				}
			}

		iScreenSize = aScreenSize;
		}
	}

void CTestScreen::ImportScreenConfigL(TInt aScreenNo, TDisplayMode aMode, TInt aFrameDuration, const TSize& aScreenSize, TGceTestResults* aGceTestResults,
							const TDesC& aConfigFileName)
	{
	TInt screenWindowNumber;
	TPtrC windowType;

	TBuf<KMaxUiBitmapNameLength>	tempStore;
	tempStore.Format(KScreenWindow, aScreenNo);

	READ_INI1A(tempStore, number, screenWindowNumber, iUtils);
	READ_INI1A(tempStore, popup_occurances, iPopupOccurances, iUtils);

	tempStore.Format(KScreen, aScreenNo);
	READ_INI2A(tempStore, rotation, True, False, iRotation, ETrue, EFalse, iUtils);

	iWindAndSurf = new (ELeave) CArrayPtrFlat<CTestSurfacedWindow>(1);

	for(TInt i=1; i<screenWindowNumber+1; i++)
		{
		RDebug::Print(_L("Creating surfaced window %d on screen %d\n"), i, aScreenNo);

		iWindAndSurf->ExtendL() = CTestSurfacedWindow::NewL(aScreenNo, i, aMode, aFrameDuration, aScreenSize, aGceTestResults, aConfigFileName, iPseudoAppShared);

		RDebug::Print(_L("Surfaced window %d on screen %d created\n"), i, aScreenNo);
		}
	}

void CTestScreen::CreatePopupWindow()
	{
	RDebug::Print(_L("Starting App: %S"), &KScreenWindowPopupApp);

	TInt createErr = iPopupProcess.Create(KScreenWindowPopupApp, KNullDesC);

	if (createErr == KErrNone)
		{
		iPopupProcess.SetPriority(EPriorityForeground);
		iPopupProcess.Resume();
		}
	}

void CTestScreen::LaunchRotationApp()
	{
	RDebug::Print(_L("Starting App: %S"), &KScreenRotationApp);

	TInt createErr = iRotationProcess.Create(KScreenRotationApp, KNullDesC);

	if (createErr == KErrNone)
		{
		iRotationProcess.SetPriority(EPriorityForeground);
		iRotationProcess.Resume();
		}
	}

void CTestScreen::DestroyRotationApp()
	{
	if(iRotation)
		{
		iRotationProcess.Kill(KErrGeneral);
		}
	}

void CTestScreen::DestroyPopupWindow()
	{
	iPopupProcess.Kill(KErrGeneral);
	}

