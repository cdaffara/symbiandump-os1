/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/



#if (!defined __T_MM_DATA_CAMERA_H__)
#define __T_MM_DATA_CAMERA_H__

//	User includes
#include "DataWrapperBase.h"

//	EPOC includes
#include <ecam.h>
#include <w32std.h>

/**
 * Test Active Notification class
 *
 */
class CT_MMDataCamera : public CDataWrapperBase, public MCameraObserver, public MCameraObserver2
	{
public:
	~CT_MMDataCamera();

	static	CT_MMDataCamera*	NewL();

	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

	virtual TAny*	GetObject();

	virtual void	SetObjectL(TAny* aObject);

	virtual void	DisownObjectL();

	// from the MCameraObserverInterface
	virtual void	ReserveComplete(TInt aError);
	virtual void	PowerOnComplete(TInt aError);
	virtual void	ViewFinderFrameReady(CFbsBitmap &aFrame);
	virtual void	ImageReady(CFbsBitmap *aBitmap, HBufC8 *aData, TInt aError);
	virtual void	FrameBufferReady(MFrameBuffer *aFrameBuffer, TInt aError);

	// from the MCameraObserver2 interface
	virtual void	HandleEvent(const TECAMEvent &aEvent);
	virtual void	ViewFinderReady(MCameraBuffer &aCameraBuffer, TInt aError);
	virtual void	ImageBufferReady(MCameraBuffer &aCameraBuffer, TInt aError);
	virtual void	VideoBufferReady(MCameraBuffer &aCameraBuffer, TInt aError);

protected:
	CT_MMDataCamera();

	void	ConstructL();

private:
	inline void			DoCmdBrightness(const TDesC& aSection);
	inline void			DoCmdSetBrightness(const TDesC& aSection);
	inline void			DoCmdContrast(const TDesC& aSection);
	inline void			DoCmdSetContrastL(const TDesC& aSection);
	inline void			DoCmdDigitalZoomFactor(const TDesC& aSection);
	inline void			DoCmdSetDigitalZoomFactorL(const TDesC& aSection);
	inline void			DoCmdExposure(const TDesC& aSection);
	inline void			DoCmdSetExposureL(const TDesC& aSection);
	inline void			DoCmdFlash(const TDesC& aSection);
	inline void			DoCmdSetFlashL(const TDesC& aSection);
	inline void			DoCmdViewFinderMirror(const TDesC& aSection);
	inline void			DoCmdSetViewFinderMirrorL(const TDesC& aSection);
	inline void			DoCmdWhiteBalance(const TDesC& aSection);
	inline void			DoCmdSetWhiteBalanceL(const TDesC& aSection);
	inline void			DoCmdZoomFactor(const TDesC& aSection);
	inline void			DoCmdSetZoomFactorL(const TDesC& aSection);
	inline void			DoCmdJpegQuality(const TDesC& aSection);
	inline void			DoCmdSetJpegQuality(const TDesC& aSection);
	inline void			DoCmdBuffersInUse(const TDesC& aSection);
	inline void			DoCmdCameraInfo(const TDesC& aSection);
	inline void			DoCmdCamerasAvailable(const TDesC& aSection);
	inline void			DoCmdCancelCaptureImage();
	inline void			DoCmdCaptureImage(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void			DoCmdCustomInterface(const TDesC& aSection);
	inline void			DoCmdEnumerateCaptureSizes(const TDesC& aSection);
	inline void			DoCmdEnumerateVideoFrameRates(const TDesC& aSection);
	inline void			DoCmdEnumerateVideoFrameSizes(const TDesC& aSection);
	inline void			DoCmdFrameRate(const TDesC& aSection);
	inline void			DoCmdFramesPerBuffer(const TDesC& aSection);
	inline void			DoCmdGetFrameSize(const TDesC& aSection);
	inline void			DoCmdHandle();
	inline void			DoCmdNewDuplicateL(const TDesC& aSection);
	inline void			DoCmdNewL(const TDesC& aSection);
	inline void			DoCmdPowerOff();
	inline void			DoCmdPowerOn(const TInt aAsyncErrorIndex);
	inline void			DoCmdPrepareImageCaptureL(const TDesC& aSection);
	inline void			DoCmdPrepareVideoCaptureL(const TDesC& aSection);
	inline void			DoCmdRelease();
	inline void			DoCmdReserve(const TInt aAsyncErrorIndex);
	inline void			DoCmdStartVideoCapture(const TDesC& aSection, const TInt aAsyncErrorIndex);
	inline void			DoCmdStartViewFinderBitmapsL(const TDesC& aSection);
	inline void			DoCmdStartViewFinderDirectL(const TDesC& aSection);
	inline void			DoCmdStartViewFinderL(const TDesC& aSection);
	inline void			DoCmdStopVideoCapture();
	inline void			DoCmdStopViewFinder();
	inline void			DoCmdVideoCaptureActive(const TDesC& aSection);
	inline void			DoCmdViewFinderActive(const TDesC& aSection);
	inline void			DoCmdDestructor();

	void				DestroyData();
	TInt				GetDefaultDigitalZf();
	TBool				GetParam(
							CCamera::TFormat&					aFormat,
							const TDesC&						aSection);
	TBool				GetParam(
							TRect&								aRect,
							const TDesC&						aTag,
							const TDesC&						aSection);
	TBool				GetParam(
							TSize&								aSize,
							const TDesC&						aTag,
							const TDesC&						aSection);
	TBool				GetParam(
							CCamera::TExposure&					aExposure,
							const TDesC&						aSection);
	TBool				GetParam(
							CCamera::TFlash&					aFlash,
							const TDesC&						aSection);
	TBool				GetParam(
							CCamera::TWhiteBalance&				aWhiteBalance,
							const TDesC&						aSection);
	TBool				GetParam(
							TCameraInfo::TCameraOrientation&	aOrientation,
							const TDesC&						aSection);
	static TBool		ConvertToCameraFormat(
							const TDesC&						aFormatDes,
							CCamera::TFormat&					aFormat);
	static TBool		ConvertToCameraExposure(
							const TDesC&						aExposureDes,
							CCamera::TExposure&					aExposure);
	static TBool		ConvertToCameraFlash(
							const TDesC&						aFlashDes,
							CCamera::TFlash&					aFlash);
	static TBool		ConvertToCameraWhiteBalance(
							const TDesC&						aWhiteBalanceDes,
							CCamera::TWhiteBalance&				aWhiteBalance);
	static const TDesC&	GetParamString(const CCamera::TWhiteBalance aWb);
	static const TDesC&	GetParamString(const CCamera::TFlash aFlash);
	static const TDesC&	GetParamString(const CCamera::TExposure aExposure);
	static const TDesC&	GetParamString(const CCamera::TFormat aFormat);

	// TCameraOrientation
	static TBool		ConvertToOrientation(
							const TDesC& aOrientationDes,
							TCameraInfo::TCameraOrientation& aOrientation);
	static const TDesC&	GetParamString(TCameraInfo::TCameraOrientation aOrientation);

private:
	CCamera*				iCamera;
	TCameraInfo				iCameraInfo;
	TBool					iPrepareCaptureOk;
	TInt					iPrepareCaptureErr;
	TBool					iCapturing;
	TPtrC					iBitmapFile;
	TBool					iHasBitmapFile;
	TInt					iAsyncErrorIndex;

	// vars neccessary for CCamera
	RFbsSession				iFbsSession;
	TBool					iFbsSessionConnected;

	// vars neccessary for StartViewFinderDirectL
	RWsSession				iWsSession;
	TBool					iWsSessionConnected;
	CWsScreenDevice* 		iScrDevice;
	RWindowGroup*			iWindowGroup;
	RWindow*				iWindow;
	};

#endif /* __T_MM_DATA_CAMERA_H__ */
