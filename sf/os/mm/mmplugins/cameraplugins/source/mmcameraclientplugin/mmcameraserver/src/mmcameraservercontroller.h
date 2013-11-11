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

/**
 @file
 @internalTechnology
*/

#ifndef MMCAMERASERVERCONTROLLER_H
#define MMCAMERASERVERCONTROLLER_H

#include "mmcameraserver.h"
#include <graphics/surfaceconfiguration.h>
#include "w32std.h"
 
#include <mmclient.h>
#include <mmcamerasensorcomponent.h>
#include <mmcameraviewfindercomponent.h>
#include <mmcameracapturecomponent.h>
#include <mmgraphicssinkcomponent.h>


class CMMCameraServerController : public CBase
	{
public:
	static CMMCameraServerController* NewL(TInt aCameraIndex);

	~CMMCameraServerController();

	TInt CameraIndex() const;
	TInt CameraHandle() const;

	void Reset();

	void PrepareDirectViewFinderL(TDirectViewFinderInfo& aViewFinderInfo);
	TInt StartDirectViewFinder();
	void StopDirectViewFinder();
	void ResumeDirectViewFinder();
	void PauseDirectViewFinder();
	CCamera::CCameraV2DirectViewFinder::TViewFinderState GetDirectViewFinderState() const;
	TInt SetDirectViewFinderMirror(const TBool aMirror);
	TInt GetDirectViewFinderMirror(TBool& aMirror);
	TInt DirectViewFinderProperties(TInt& aScreenNum, TRect& aScreenRect, TRect& aClipRect);

	TInt SetZoom(const TInt aZoom);
	TInt SetDigitalZoom(const TInt aDigitalZoom);
	TInt SetContrast(const TInt aContrast);
	TInt SetBrightness(const TInt aBrightness);
	TInt SetFlash(const CCamera::TFlash aFlash);
	TInt SetExposure(const CCamera::TExposure aExposure);
	TInt SetWhiteBalance(const CCamera::TWhiteBalance aWhiteBalance);
	TInt GetZoom(TInt& aZoom);
	TInt GetDigitalZoom(TInt& aDigitalZoom);
	TInt GetContrast(TInt& aContrast);
	TInt GetBrightness(TInt& aBrightness);
	TInt GetFlash(CCamera::TFlash& aFlash);
	TInt GetExposure(CCamera::TExposure& aExposure);
	TInt GetWhiteBalance(CCamera::TWhiteBalance& aWhiteBalance);

private:
	CMMCameraServerController(TInt aCameraIndex); 
	void ConstructL();


public:
	TDblQueLink iCamCntrlLink;    // Link Object for Server

private:
	MMClient::RMMServer 						iMMServer;
	MMClient::RMMStreamContainer 				iMMStreamContainer;
	MMClient::RMMStream							iMMStream;

	MMCameraComponents::RMMCameraViewFinder		iCameraViewFinder;
	MMCameraComponents::RMMCameraSensor			iCameraSensor;
	MMCameraComponents::RMMCameraCapture			iCameraCapture;
	MMVideoComponents::RMMVideoGraphicsSink		iGraphicsSink;

	TInt iCameraIndex;
	TInt iCameraHandle;
	TInt iPriority;
	TBool iMMCapability;
	
	CCamera::CCameraV2DirectViewFinder::TViewFinderState iViewFinderState;
	
	TRect iClipRect; // To store the screen clipping area specified by the client during StartViewFinderDirectL()
					 // Unless this is passed to RMM classes in which case this can be retrieved through Get() Methods?
    TInt iZoom;
    TInt iDigitalZoom;
    TInt iContrast;
    TInt iBrightness;
    CCamera::TFlash iFlash;
    CCamera::TExposure iExposure;
    CCamera::TWhiteBalance iWhiteBalance;	
	};



class CMMCameraServerControllerQuery : public CBase
	{
public:
	static CMMCameraServerControllerQuery* NewL();
	~CMMCameraServerControllerQuery();

	TInt GetCamerasAvailable();

private:
	void ConstructL();
	CMMCameraServerControllerQuery();

private:
	MMClient::RMMServer		iServer;
	};

#endif // MMCAMERASERVERCONTROLLER_H
