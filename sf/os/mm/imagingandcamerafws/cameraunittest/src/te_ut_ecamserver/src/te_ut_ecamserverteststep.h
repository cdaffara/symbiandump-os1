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

#if (!defined __UT_ECAM_SERVER2_STEP_H__)
#define __UT_ECAM_SERVER2_STEP_H__
#include <ecamviewfinder.h>
#include <test/testexecutestepbase.h>
#include "te_ut_ecamserverteststepbase.h"

class CCameraCreationOOMTestStep : public CTe_ut_ecam_serverSuiteStepBase, MCameraObserver2
	{
public:
	CCameraCreationOOMTestStep();
	~CCameraCreationOOMTestStep();

// from CTe_ut_ecam_serverSuiteStepBase
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

private:
	CCamera* iCamera;
	};

_LIT(KCameraCreationOOMTestStep, "Camera_Creation_OOM_Test");

//

class CCameraReserveOOMTestStep : public CTe_ut_ecam_serverSuiteStepBase, MCameraObserver2
	{
public:
	CCameraReserveOOMTestStep();
	~CCameraReserveOOMTestStep();

// from CTe_ut_ecam_serverSuiteStepBase
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void PostKickOffTestL(TInt aReason, TVerdict aResult);

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

private:
	CCamera* iCamera;
	TBool iCameraReserve;
	};

_LIT(KCameraReserveOOMTestStep, "Camera_Reserve_OOM_Test");

//

class CCameraPowerOnOOMTestStep : public CTe_ut_ecam_serverSuiteStepBase, MCameraObserver2
	{
public:
	CCameraPowerOnOOMTestStep();
	~CCameraPowerOnOOMTestStep();

// from CTe_ut_ecam_serverSuiteStepBase
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void PostKickOffTestL(TInt aReason, TVerdict aResult);

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

private:
	CCamera* iCamera;
	TBool iCameraReserve;
	TBool iCameraPowerOn;
	};

_LIT(KCameraPowerOnOOMTestStep, "Camera_PowerOn_OOM_Test");

//

class CCameraStartViewFinderTestStep : public CTe_ut_ecam_serverSuiteStepBase, public MCameraObserver2, public MDirectViewFinderObserver
	{
public:
	CCameraStartViewFinderTestStep();
	~CCameraStartViewFinderTestStep();

// from CTe_ut_ecam_serverSuiteStepBase
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void PostKickOffTestL(TInt aReason, TVerdict aResult);

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

// from MDirectViewFinderObserver
	virtual void DirectViewFinderFirstFrameDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, TInt aErrorCode);
	virtual void DirectHistogramDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, CCamera::CCameraV2Histogram& aDirectHistogramDisplayed, TInt aErrorCode);
	virtual void ClientHistogramReady(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, MHistogramV2Buffer* aClientHistogramBuffer, TInt aErrorCode);
	virtual void DirectSnapshotForImageDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, TInt aErrorCode);
	virtual void DirectViewFinderFailed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, TInt aErrorCode);
	virtual TInt CustomInterface(TUid aInterface, TAny*& aPtrInterface);

private:
	CCamera* iCamera;
	CCamera::CCameraV2DirectViewFinder* iCameraDirectVF;
	CAsyncTestShutdown* iTestShutdown;
	TBool iCameraReserve;
	TBool iCameraPowerOn;
	TBool iCameraViewFinder;
	CCamera::CCameraV2DirectViewFinder::TViewFinderState iViewFinderState;
	};

_LIT(KCameraStartViewFinderTestStep, "Camera_Start_ViewFinder_Test");

//

class CCameraRepeatedReserveTestStep : public CTe_ut_ecam_serverSuiteStepBase, public MCameraObserver2
	{
public:
	CCameraRepeatedReserveTestStep();
	~CCameraRepeatedReserveTestStep();

// from CTe_ut_ecam_serverSuiteStepBase
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void PostKickOffTestL(TInt aReason, TVerdict aResult);
	void ChangeState();

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

private:
	CCamera* iCamera;
	CAsyncTestShutdown* iTestShutdown;
	CRetrieveTimer* iRetrieveTimer;
	TBool iCameraReserve;
	};

_LIT(KCameraRepeatedReserveTestStep, "Camera_Repeated_Reserve_Test");

//

class CCameraRepeatedPowerOnTestStep : public CTe_ut_ecam_serverSuiteStepBase, public MCameraObserver2
	{
public:
	CCameraRepeatedPowerOnTestStep();
	~CCameraRepeatedPowerOnTestStep();

// from CTe_ut_ecam_serverSuiteStepBase
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void PostKickOffTestL(TInt aReason, TVerdict aResult);
	void ChangeState();

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

private:
	CCamera* iCamera;
	CAsyncTestShutdown* iTestShutdown;
	CRetrieveTimer* iRetrieveTimer;
	TBool iCameraReserve;
	TBool iCameraPowerOn;
	};

_LIT(KCameraRepeatedPowerOnTestStep, "Camera_Repeated_PowerOn_Test");

//

class CCameraStartStopViewFinderTestStep : public CTe_ut_ecam_serverSuiteStepBase, public MCameraObserver2, public MDirectViewFinderObserver
	{
public:
	CCameraStartStopViewFinderTestStep();
	~CCameraStartStopViewFinderTestStep();

// from CTe_ut_ecam_serverSuiteStepBase
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void PostKickOffTestL(TInt aReason, TVerdict aResult);
	void ChangeState();

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

// from MDirectViewFinderObserver
	virtual void DirectViewFinderFirstFrameDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, TInt aErrorCode);
	virtual void DirectHistogramDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, CCamera::CCameraV2Histogram& aDirectHistogramDisplayed, TInt aErrorCode);
	virtual void ClientHistogramReady(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, MHistogramV2Buffer* aClientHistogramBuffer, TInt aErrorCode);
	virtual void DirectSnapshotForImageDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, TInt aErrorCode);
	virtual void DirectViewFinderFailed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, TInt aErrorCode);
	virtual TInt CustomInterface(TUid aInterface, TAny*& aPtrInterface);

private:
	CCamera* iCamera;
	CCamera::CCameraV2DirectViewFinder* iCameraDirectVF;
	CAsyncTestShutdown* iTestShutdown;
	CRetrieveTimer* iRetrieveTimer;
	TBool iCameraReserve;
	TBool iCameraPowerOn;
	TBool iCameraViewFinder;
	};

_LIT(KCameraStartStopViewFinderTestStep, "Camera_StartStop_ViewFinder_Test");

//

class CCameraReserveThenDestructTestStep : public CTe_ut_ecam_serverSuiteStepBase, MCameraObserver2
	{
public:
	CCameraReserveThenDestructTestStep();
	~CCameraReserveThenDestructTestStep();

// from CTe_ut_ecam_serverSuiteStepBase
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

private:
	CCamera* iCamera;
	};

_LIT(KCameraReserveThenDestructTestStep, "Camera_Reserve_Then_Destruct_Test");

//

class CCameraPowerOnThenDestructTestStep : public CTe_ut_ecam_serverSuiteStepBase, MCameraObserver2
	{
public:
	CCameraPowerOnThenDestructTestStep();
	~CCameraPowerOnThenDestructTestStep();

// from CTe_ut_ecam_serverSuiteStepBase
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void PostKickOffTestL(TInt aReason, TVerdict aResult);

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

private:
	CCamera* iCamera;
	TBool iCameraReserve;
	};

_LIT(KCameraPowerOnThenDestructTestStep, "Camera_PowerOn_Then_Destruct_Test");

//

class CCameraStartViewFinderThenDestructTestStep : public CTe_ut_ecam_serverSuiteStepBase, MCameraObserver2, MDirectViewFinderObserver
	{
public:
	CCameraStartViewFinderThenDestructTestStep();
	~CCameraStartViewFinderThenDestructTestStep();

// from CTe_ut_ecam_serverSuiteStepBase
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void PostKickOffTestL(TInt aReason, TVerdict aResult);

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

// from MDirectViewFinderObserver
	virtual void DirectViewFinderFirstFrameDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, TInt aErrorCode);
	virtual void DirectHistogramDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, CCamera::CCameraV2Histogram& aDirectHistogramDisplayed, TInt aErrorCode);
	virtual void ClientHistogramReady(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, MHistogramV2Buffer* aClientHistogramBuffer, TInt aErrorCode);
	virtual void DirectSnapshotForImageDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, TInt aErrorCode);
	virtual void DirectViewFinderFailed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, TInt aErrorCode);
	virtual TInt CustomInterface(TUid aInterface, TAny*& aPtrInterface);

private:
	CCamera* iCamera;
	CCamera::CCameraV2DirectViewFinder* iCameraDirectVF;
	TBool iCameraReserve;
	TBool iCameraPowerOn;
	};

_LIT(KCameraStartViewFinderThenDestructTestStep, "Camera_Start_ViewFinder_Then_Destruct_Test");

//

class CCameraReserveTwiceTestStep : public CTe_ut_ecam_serverSuiteStepBase, MCameraObserver2
	{
public:
	CCameraReserveTwiceTestStep();
	~CCameraReserveTwiceTestStep();

// from CTe_ut_ecam_serverSuiteStepBase
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void PostKickOffTestL(TInt aReason, TVerdict aResult);

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

private:
	CCamera* iCamera;
	TBool iCameraReserve;
	};

_LIT(KCameraReserveTwiceTestStep, "Camera_Reserve_Twice_Test");

//

class CCameraPowerOnTwiceTestStep : public CTe_ut_ecam_serverSuiteStepBase, MCameraObserver2
	{
public:
	CCameraPowerOnTwiceTestStep();
	~CCameraPowerOnTwiceTestStep();

// from CTe_ut_ecam_serverSuiteStepBase
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void PostKickOffTestL(TInt aReason, TVerdict aResult);

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

private:
	CCamera* iCamera;
	TBool iCameraReserve;
	TBool iCameraPowerOn;
	};

_LIT(KCameraPowerOnTwiceTestStep, "Camera_PowerOn_Twice_Test");

//

class CCameraPowerOnWithoutReserveTestStep : public CTe_ut_ecam_serverSuiteStepBase, MCameraObserver2
	{
public:
	CCameraPowerOnWithoutReserveTestStep();
	~CCameraPowerOnWithoutReserveTestStep();

// from CTe_ut_ecam_serverSuiteStepBase
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void PostKickOffTestL(TInt aReason, TVerdict aResult);

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

private:
	CCamera* iCamera;
	TBool iCameraReserve;
	TBool iCameraPowerOn;
	};

_LIT(KCameraPowerOnWithoutReserveTestStep, "Camera_PowerOn_Without_Reserve_Test");

//

class CCameraPowerOnAfterReleaseTestStep : public CTe_ut_ecam_serverSuiteStepBase, MCameraObserver2
	{
public:
	CCameraPowerOnAfterReleaseTestStep();
	~CCameraPowerOnAfterReleaseTestStep();

// from CTe_ut_ecam_serverSuiteStepBase
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void PostKickOffTestL(TInt aReason, TVerdict aResult);

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

private:
	CCamera* iCamera;
	TBool iCameraReserve;
	TBool iCameraPowerOn;
	};

_LIT(KCameraPowerOnAfterReleaseTestStep, "Camera_PowerOn_After_Release_Test");

//

class CCameraStartViewFinderWithoutReserveTestStep : public CTe_ut_ecam_serverSuiteStepBase, public MCameraObserver2, public MDirectViewFinderObserver
	{
public:
	CCameraStartViewFinderWithoutReserveTestStep();
	~CCameraStartViewFinderWithoutReserveTestStep();

// from CTe_ut_ecam_serverSuiteStepBase
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void PostKickOffTestL(TInt aReason, TVerdict aResult);

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

// from MDirectViewFinderObserver
	virtual void DirectViewFinderFirstFrameDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, TInt aErrorCode);
	virtual void DirectHistogramDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, CCamera::CCameraV2Histogram& aDirectHistogramDisplayed, TInt aErrorCode);
	virtual void ClientHistogramReady(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, MHistogramV2Buffer* aClientHistogramBuffer, TInt aErrorCode);
	virtual void DirectSnapshotForImageDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, TInt aErrorCode);
	virtual void DirectViewFinderFailed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, TInt aErrorCode);
	virtual TInt CustomInterface(TUid aInterface, TAny*& aPtrInterface);

private:
	CCamera* iCamera;
	CCamera::CCameraV2DirectViewFinder* iCameraDirectVF;
	CAsyncTestShutdown* iTestShutdown;
	TBool iCameraReserve;
	TBool iCameraPowerOn;
	TBool iCameraViewFinder;
	};

_LIT(KCameraStartViewFinderWithoutReserveTestStep, "Camera_Start_ViewFinder_Without_Reserve_Test");

//

class CCameraStartViewFinderWithoutPowerOnTestStep : public CTe_ut_ecam_serverSuiteStepBase, public MCameraObserver2, public MDirectViewFinderObserver
	{
public:
	CCameraStartViewFinderWithoutPowerOnTestStep();
	~CCameraStartViewFinderWithoutPowerOnTestStep();

// from CTe_ut_ecam_serverSuiteStepBase
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void PostKickOffTestL(TInt aReason, TVerdict aResult);

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

// from MDirectViewFinderObserver
	virtual void DirectViewFinderFirstFrameDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, TInt aErrorCode);
	virtual void DirectHistogramDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, CCamera::CCameraV2Histogram& aDirectHistogramDisplayed, TInt aErrorCode);
	virtual void ClientHistogramReady(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, MHistogramV2Buffer* aClientHistogramBuffer, TInt aErrorCode);
	virtual void DirectSnapshotForImageDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, TInt aErrorCode);
	virtual void DirectViewFinderFailed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, TInt aErrorCode);
	virtual TInt CustomInterface(TUid aInterface, TAny*& aPtrInterface);

private:
	CCamera* iCamera;
	CCamera::CCameraV2DirectViewFinder* iCameraDirectVF;
	CAsyncTestShutdown* iTestShutdown;
	TBool iCameraReserve;
	TBool iCameraPowerOn;
	TBool iCameraViewFinder;
	};

_LIT(KCameraStartViewFinderWithoutPowerOnTestStep, "Camera_Start_ViewFinder_Without_PowerOn_Test");

//

class CCameraStartViewFinderAfterPowerOffTestStep : public CTe_ut_ecam_serverSuiteStepBase, public MCameraObserver2, public MDirectViewFinderObserver
	{
public:
	CCameraStartViewFinderAfterPowerOffTestStep();
	~CCameraStartViewFinderAfterPowerOffTestStep();

// from CTe_ut_ecam_serverSuiteStepBase
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void PostKickOffTestL(TInt aReason, TVerdict aResult);

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

// from MDirectViewFinderObserver
	virtual void DirectViewFinderFirstFrameDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, TInt aErrorCode);
	virtual void DirectHistogramDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, CCamera::CCameraV2Histogram& aDirectHistogramDisplayed, TInt aErrorCode);
	virtual void ClientHistogramReady(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, MHistogramV2Buffer* aClientHistogramBuffer, TInt aErrorCode);
	virtual void DirectSnapshotForImageDisplayed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, CCamera::CCameraImageCapture& aCaptureImageHandle, TPostCaptureControlId aPostCaptureControlId, TInt aErrorCode);
	virtual void DirectViewFinderFailed(CCamera::CCameraV2DirectViewFinder& aDirectViewFinderHandle, TInt aErrorCode);
	virtual TInt CustomInterface(TUid aInterface, TAny*& aPtrInterface);

private:
	CCamera* iCamera;
	CCamera::CCameraV2DirectViewFinder* iCameraDirectVF;
	CAsyncTestShutdown* iTestShutdown;
	TBool iCameraReserve;
	TBool iCameraPowerOn;
	TBool iCameraViewFinder;
	};

_LIT(KCameraStartViewFinderAfterPowerOffTestStep, "Camera_Start_ViewFinder_After_PowerOff_Test");

//

class CCameraMultiClientReservePriorityTestStep;

class CCameraSecondClient : public CBase, public MCameraObserver2
	{
	friend class CCameraMultiClientReservePriorityTestStep;
public:
	CCameraSecondClient(CCameraMultiClientReservePriorityTestStep* aTestStep);
	~CCameraSecondClient();

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

private:
	CAsyncTestShutdown* iTestShutdown;
	CCameraMultiClientReservePriorityTestStep* iTestStep;
	CCamera* iCamera;
	TBool iCameraReserve;
	TBool iIsSecondCameraReserved;
	};

class CCameraMultiClientReservePriorityTestStep : public CTe_ut_ecam_serverSuiteStepBase, public MCameraObserver2
	{
	friend class CCameraSecondClient;
public:
	CCameraMultiClientReservePriorityTestStep();
	~CCameraMultiClientReservePriorityTestStep();

// from CTe_ut_ecam_serverSuiteStepBase
	TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void PostKickOffTestL(TInt aReason, TVerdict aResult);

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

private:
	CAsyncTestShutdown* iTestShutdown;
	CCameraSecondClient* iSecondClient;
	CCamera* iCamera;
	TBool iCameraReserve;
	TInt iFirstPriority;
	TInt iSecondPriority;
	TBool iIsFirstCameraOverthrown;
	};

_LIT(KCameraMultiClientReservePriorityTestStep, "Camera_MultiClient_Reserve_Priority_Test");
_LIT(KCameraFirstPriorityString, "FirstPriority");
_LIT(KCameraSecondPriorityString, "SecondPriority");

//

class CCameraConfigurationTestStep : public CTe_ut_ecam_serverSuiteStepBase, public MCameraObserver2
	{
public:
	CCameraConfigurationTestStep();
	~CCameraConfigurationTestStep();

// from CTe_ut_ecam_serverSuiteStepBase
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	void PostKickOffTestL(TInt aReason, TVerdict aResult);

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

private:
	CCamera* iCamera;
	TBool iCameraReserve;
	TBool iCameraPowerOn;
	
	TInt iZoomFactor;
	TInt iDigitalZoomFactor;
	TInt iContrast;
	TInt iBrightness;
	CCamera::TFlash iFlash;
	CCamera::TExposure iExposure;
	CCamera::TWhiteBalance iWhiteBalance;
	};

_LIT(KCameraConfigurationTestStep, "Camera_Configuration_Test");
_LIT(KCameraZoomFactorString, "ZoomFactor");
_LIT(KCameraDigitalZoomFactorString, "DigitalZoomFactor");
_LIT(KCameraContrastString, "Contrast");
_LIT(KCameraBrightnessString, "Brightness");
_LIT(KCameraFlashString, "Flash");
_LIT(KCameraExposureString, "Exposure");
_LIT(KCameraWhiteBalanceString, "WhiteBalance");

//

class CCameraCreationStressTestStep : public CTe_ut_ecam_serverSuiteStepBase, MCameraObserver2
	{
public:
	CCameraCreationStressTestStep();
	~CCameraCreationStressTestStep();

// from CTe_ut_ecam_serverSuiteStepBase
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

private:
	RArray<CCamera> iCameraArray;
	TInt iNumberOfCamera;
	};

_LIT(KCameraCreationStressTestStep, "Camera_Creation_Stress_Test");
_LIT(KCameraNumberString, "NumberOfCamera");

//

class CCameraMultiClientPowerOnPriorityTestStep;
class CCameraSecondClient2 : public CBase, public MCameraObserver2
	{
	friend class CCameraMultiClientPowerOnPriorityTestStep;
public:
	CCameraSecondClient2(CCameraMultiClientPowerOnPriorityTestStep* aTestStep);
	~CCameraSecondClient2();

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

private:
	CAsyncTestShutdown* iTestShutdown;
	CCameraMultiClientPowerOnPriorityTestStep* iTestStep;
	CCamera* iCamera;
	TBool iCameraReserve;
	TBool iIsSecondCameraReserved;
	};

class CCameraMultiClientPowerOnPriorityTestStep : public CTe_ut_ecam_serverSuiteStepBase, public MCameraObserver2
	{
	friend class CCameraSecondClient2;
public:
	CCameraMultiClientPowerOnPriorityTestStep();
	~CCameraMultiClientPowerOnPriorityTestStep();

// from CTe_ut_ecam_serverSuiteStepBase
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	void PostKickOffTestL(TInt aReason, TVerdict aResult);

// from MCameraObserver2
	virtual void HandleEvent(const TECAMEvent& aEvent);
	virtual void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
	virtual void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

private:
	CAsyncTestShutdown* iTestShutdown;
	CCameraSecondClient2* iSecondClient;
	CCamera* iCamera;
	TBool iCameraReserve;
	TBool iCameraPowerOn;
	TInt iFirstPriority;
	TInt iSecondPriority;
	TBool iIsFirstCameraOverthrown;
	};

_LIT(KCameraMultiClientPowerOnPriorityTestStep, "Camera_MultiClient_PowerOn_Priority_Test");

#endif // TE_UT_ECAMSERVERTESTSTEP_H
