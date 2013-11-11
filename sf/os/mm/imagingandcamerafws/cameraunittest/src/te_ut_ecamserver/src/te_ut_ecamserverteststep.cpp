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

#include "te_ut_ecamserverteststep.h"

CCameraCreationOOMTestStep::CCameraCreationOOMTestStep()
	{
	SetTestStepName(KCameraCreationOOMTestStep);
	}

CCameraCreationOOMTestStep::~CCameraCreationOOMTestStep()
	{
	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}
	}

TVerdict CCameraCreationOOMTestStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("***** Begin Camera Creation OOM Test*****"));
	CreateDependencyFileL();

	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPreambleL();
	}

TVerdict CCameraCreationOOMTestStep::doTestStepL()
	{
	TInt cntCamera = CCamera::CamerasAvailable();
	INFO_PRINTF2(_L("The Number Of Camera = %d"), cntCamera);

	for (TInt index = 0; index < cntCamera; ++index)
		{
		INFO_PRINTF2(_L("***** Camera Creation OOM Iteration %d *****"), index);
		TRAPD(error, iCamera = CCamera::New2L(*this, index, 0));
		if (error != KErrNone)
			{
			INFO_PRINTF2(_L("***** Failed to call CCamera::New2L(): err = %d *****"), error);
			REComSession::FinalClose();
			User::Leave(error);
			}
		else
			{
			INFO_PRINTF2(_L("Camera Version = %d"), iCamera->CameraVersion());
			delete iCamera;
			iCamera = NULL;

//			MCameraObserver2* tempObserver = NULL;
//			CCamera* duplicate = NULL;
//			TRAP(error, duplicate = CCamera::NewDuplicate2L(*tempObserver, iCamera->Handle()));
//			if (error != KErrNone)
//				{
//				INFO_PRINTF1(_L("***** Failed to call CCamera::NewDuplicate2L() *****"));
//				delete iCamera;
//				iCamera = NULL;
//				User::Leave(error);
//				}
//			else
//				{
//				INFO_PRINTF2(_L("Duplicated Camera Version = %d"), duplicate->CameraVersion());
//				delete duplicate;
//				duplicate = NULL;
//				delete iCamera;
//				iCamera = NULL;
//				}			
			}
		}

	return TestStepResult();
	}

TVerdict CCameraCreationOOMTestStep::doTestStepPostambleL()
	{
	RemoveDependencyFile();

	INFO_PRINTF1(_L("***** Finish Camera Creation OOM Test *****"));
	REComSession::FinalClose();
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPostambleL();
	}

void CCameraCreationOOMTestStep::HandleEvent(const TECAMEvent& /*aEvent*/)
	{
	}

void CCameraCreationOOMTestStep::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraCreationOOMTestStep::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}

void CCameraCreationOOMTestStep::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}

//

CCameraReserveOOMTestStep::CCameraReserveOOMTestStep()
	: iCameraReserve(EFalse)
	{
	SetTestStepName(KCameraReserveOOMTestStep);
	}

CCameraReserveOOMTestStep::~CCameraReserveOOMTestStep()
	{
	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}
	}

TVerdict CCameraReserveOOMTestStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("***** Begin Camera Reserve OOM Test*****"));
	CreateDependencyFileL();

	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPreambleL();
	}

TVerdict CCameraReserveOOMTestStep::doTestStepL()
	{
	TInt cntCamera = CCamera::CamerasAvailable();
	INFO_PRINTF2(_L("The Number Of Camera = %d"), cntCamera);

	for (TInt index = 0; index < cntCamera; ++index)
		{
		INFO_PRINTF2(_L("***** Camera Reserve OOM Camera Iteration %d *****"), index);
		TRAPD(error, iCamera = CCamera::New2L(*this, index, 0));
		if (error != KErrNone)
			{
			INFO_PRINTF2(_L("***** Failed to call CCamera::New2L(): err = %d *****"), error);
			REComSession::FinalClose();
			User::Leave(error);
			}
		else
			{
			INFO_PRINTF2(_L("Camera Version = %d"), iCamera->CameraVersion());	

			INFO_PRINTF1(_L("***** Reserve Camera *****"));
			iCamera->Reserve();

			CActiveScheduler::Start();
			}
		}

	return TestStepResult();
	}

TVerdict CCameraReserveOOMTestStep::doTestStepPostambleL()
	{
	RemoveDependencyFile();

	INFO_PRINTF1(_L("***** Finish Camera Reserve OOM Test *****"));
	REComSession::FinalClose();
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPostambleL();
	}

void CCameraReserveOOMTestStep::PostKickOffTestL(TInt aReason, TVerdict aResult)
	{
	if (iCameraReserve)
		{
		iCamera->Release();
		iCameraReserve = EFalse;
		INFO_PRINTF1(_L("***** Camera Release *****"));
		}

	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}

	SetTestStepError(aReason);
	SetTestStepResult(aResult);
	CActiveScheduler::Stop();
	}

void CCameraReserveOOMTestStep::HandleEvent(const TECAMEvent& aEvent)
	{
	if (aEvent.iEventType == KUidECamEventReserveComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Reserve *****"));
			iCameraReserve = ETrue;
			TRAPD(error, PostKickOffTestL(KErrNone, EPass));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Reserve Camera: err = %d *****"), aEvent.iErrorCode);
			TRAPD(error, PostKickOffTestL(aEvent.iErrorCode, EFail));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			}
		}
	}

void CCameraReserveOOMTestStep::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraReserveOOMTestStep::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}

void CCameraReserveOOMTestStep::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}

//

CCameraPowerOnOOMTestStep::CCameraPowerOnOOMTestStep()
	: iCameraReserve(EFalse), iCameraPowerOn(EFalse)
	{
	SetTestStepName(KCameraPowerOnOOMTestStep);
	}

CCameraPowerOnOOMTestStep::~CCameraPowerOnOOMTestStep()
	{
	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}
	}

TVerdict CCameraPowerOnOOMTestStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("***** Begin Camera PowerOn OOM Test*****"));
	CreateDependencyFileL();

	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPreambleL();
	}

TVerdict CCameraPowerOnOOMTestStep::doTestStepL()
	{
	TInt cntCamera = CCamera::CamerasAvailable();
	INFO_PRINTF2(_L("The Number Of Camera = %d"), cntCamera);

	for (TInt index = 0; index < cntCamera; ++index)
		{
		INFO_PRINTF2(_L("***** Camera PowerOn OOM Camera Iteration %d *****"), index);
		TRAPD(error, iCamera = CCamera::New2L(*this, index, 0));
		if (error != KErrNone)
			{
			INFO_PRINTF2(_L("***** Failed to call CCamera::New2L(): err = %d *****"), error);
			REComSession::FinalClose();
			User::Leave(error);
			}
		else
			{
			INFO_PRINTF2(_L("Camera Version = %d"), iCamera->CameraVersion());	

			INFO_PRINTF1(_L("***** Reserve Camera *****"));
			iCamera->Reserve();

			CActiveScheduler::Start();
			}
		}

	return TestStepResult();
	}

TVerdict CCameraPowerOnOOMTestStep::doTestStepPostambleL()
	{
	RemoveDependencyFile();

	INFO_PRINTF1(_L("***** Finish Camera PowerOn OOM Test *****"));
	REComSession::FinalClose();
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPostambleL();
	}

void CCameraPowerOnOOMTestStep::PostKickOffTestL(TInt aReason, TVerdict aResult)
	{
	if (iCameraPowerOn)
		{
		iCamera->PowerOff();
		iCameraPowerOn = EFalse;
		INFO_PRINTF1(_L("***** Camera Power Off *****"));
		}
	if (iCameraReserve)
		{
		iCamera->Release();
		iCameraReserve = EFalse;
		INFO_PRINTF1(_L("***** Camera Release *****"));
		}

	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}

	SetTestStepError(aReason);
	SetTestStepResult(aResult);
	CActiveScheduler::Stop();
	}

void CCameraPowerOnOOMTestStep::HandleEvent(const TECAMEvent& aEvent)
	{
	if (aEvent.iEventType == KUidECamEventReserveComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Reserve *****"));
			iCameraReserve = ETrue;

			INFO_PRINTF1(_L("***** Camera Power On *****"));
			iCamera->PowerOn();
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Reserve Camera: err = %d *****"), aEvent.iErrorCode);
			SetTestStepError(aEvent.iErrorCode);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		}
	else if (aEvent.iEventType == KUidECamEventPowerOnComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Power On *****"));
			iCameraPowerOn = ETrue;

			TRAPD(error, PostKickOffTestL(KErrNone, EPass));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Power On Camera: err = %d *****"), aEvent.iErrorCode);
			TRAPD(error, PostKickOffTestL(aEvent.iErrorCode, EFail));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			}
		}
	}

void CCameraPowerOnOOMTestStep::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraPowerOnOOMTestStep::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}

void CCameraPowerOnOOMTestStep::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}

//

CCameraStartViewFinderTestStep::CCameraStartViewFinderTestStep()
	:iCameraReserve(EFalse), iCameraPowerOn(EFalse), iCameraViewFinder(EFalse), iViewFinderState(CCamera::CCameraV2DirectViewFinder::EViewFinderInActive)
	{
	SetTestStepName(KCameraStartViewFinderTestStep);
	}

CCameraStartViewFinderTestStep::~CCameraStartViewFinderTestStep()
	{
	if (iTestShutdown)
		{
		delete iTestShutdown;
		iTestShutdown = NULL;
		}

	if (iCameraDirectVF)
		{
		delete iCameraDirectVF;
		iCameraDirectVF = NULL;
		}
	
	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}
	}

TVerdict CCameraStartViewFinderTestStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("***** Begin Camera Start ViewFinder Test*****"));
	CreateDependencyFileL();

	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPreambleL();
	}

TVerdict CCameraStartViewFinderTestStep::doTestStepL()
	{
	TRAPD(error, iCamera = CCamera::New2L(*this, 0, 0));
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to call CCamera::New2L(): err = %d *****"), error);
		REComSession::FinalClose();
		User::Leave(error);
		}

	INFO_PRINTF1(_L("***** Reserve Camera *****"));
	iCamera->Reserve();

	CActiveScheduler::Start();
	return TestStepResult();
	}

TVerdict CCameraStartViewFinderTestStep::doTestStepPostambleL()
	{
	RemoveDependencyFile();

	INFO_PRINTF1(_L("***** Finish Camera Start ViewFinder Test *****"));
	REComSession::FinalClose();
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPostambleL();
	}

void CCameraStartViewFinderTestStep::PostKickOffTestL(TInt aReason, TVerdict aResult)
	{
	if (iCameraViewFinder)
		{
		iCameraDirectVF->StopDirectViewFinder();
		iCameraViewFinder = EFalse;
		INFO_PRINTF1(_L("***** Stop ViewFinder *****"));
		}
	if (iCameraPowerOn)
		{
		iCamera->PowerOff();
		iCameraPowerOn = EFalse;
		INFO_PRINTF1(_L("***** Camera Power Off *****"));
		}
	if (iCameraReserve)
		{
		iCamera->Release();
		iCameraReserve = EFalse;
		INFO_PRINTF1(_L("***** Camera Release *****"));
		}

	SetTestStepError(aReason);
	SetTestStepResult(aResult);
	CActiveScheduler::Stop();
	}

void CCameraStartViewFinderTestStep::HandleEvent(const TECAMEvent& aEvent)
	{
	if (aEvent.iEventType == KUidECamEventReserveComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Reserve *****"));
			iCameraReserve = ETrue;
			INFO_PRINTF1(_L("***** Camera Power On *****"));
			iCamera->PowerOn();
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Reserve Camera: err = %d *****"), aEvent.iErrorCode);
			SetTestStepError(aEvent.iErrorCode);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		}
	else if (aEvent.iEventType == KUidECamEventPowerOnComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Power On *****"));
			iCameraPowerOn = ETrue;

			TRAPD(error, CreateWindowL());
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to Create Window: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			INFO_PRINTF1(_L("***** Success Create Window *****"));

			TRect rect;
			rect.SetWidth(320);
			rect.SetHeight(240);
			TRAP(error, iCameraDirectVF = CCamera::CCameraV2DirectViewFinder::NewL(*iCamera, *this));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to Create ViewFinder Direct V2: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}

			TInt aHandle;
			TRAP(error, iCameraDirectVF->GetViewFinderHandleL(aHandle));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to Get ViewFinder Handle: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			INFO_PRINTF2(_L("***** ViewFinder Handle = %d"), aHandle);

			CCamera::CCameraV2DirectViewFinder::TViewFinderState vfState;
			TRAP(error, iCameraDirectVF->GetViewFinderStateL(vfState));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to Get State ViewFinder Direct V2: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}

			INFO_PRINTF2(_L("ViewFinderState1 = %d"), vfState);
			if (vfState != iViewFinderState)
				{
				INFO_PRINTF3(_L("***** Current ViewFinder State %d does not match expected ViewFinder State %d *****"), vfState, iViewFinderState);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				}

			TRAP(error, iCameraDirectVF->StartViewFinderDirectL(iWsSession, *iWsSd, *iWindow, rect));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to Start ViewFinder Direct: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				}
			iViewFinderState = CCamera::CCameraV2DirectViewFinder::EViewFinderActive;
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Power On Camera: err = %d *****"), aEvent.iErrorCode);
			TRAPD(error, PostKickOffTestL(aEvent.iErrorCode, EFail));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			}
		}
	}

void CCameraStartViewFinderTestStep::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraStartViewFinderTestStep::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraStartViewFinderTestStep::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraStartViewFinderTestStep::DirectViewFinderFirstFrameDisplayed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, TInt aErrorCode)
	{
	if (aErrorCode != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to Start ViewFinder Direct: err = %d *****"), aErrorCode);
		TRAPD(error, PostKickOffTestL(aErrorCode, EFail));
		if (error != KErrNone)
			{
			SetTestStepError(error);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		}
	else
		{
		INFO_PRINTF1(_L("***** Success Start ViewFinder Direct *****"));
		iCameraViewFinder = ETrue;
		iViewFinderState = CCamera::CCameraV2DirectViewFinder::EViewFinderActive;

		CCamera::CCameraV2DirectViewFinder::TViewFinderState vfState;
		TRAPD(error, iCameraDirectVF->GetViewFinderStateL(vfState));
		if (error != KErrNone)
			{
			INFO_PRINTF2(_L("***** Failed to Get State ViewFinder Direct V2: err = %d *****"), error);
			TRAPD(error2, PostKickOffTestL(error, EFail));
			if (error2 != KErrNone)
				{
				SetTestStepError(error2);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			return;
			}

		INFO_PRINTF2(_L("ViewFinderState2 = %d"), vfState);
		if (vfState != iViewFinderState)
			{
			INFO_PRINTF3(_L("***** Current ViewFinder State %d does not match expected ViewFinder State %d *****"), vfState, iViewFinderState);
			TRAPD(error2, PostKickOffTestL(error, EFail));
			if (error2 != KErrNone)
				{
				SetTestStepError(error2);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			}

		TInt screenNumber;
		TRect screenRect;
		TRect clipRect;
		TRAP(error, iCameraDirectVF->GetDirectViewFinderPropertiesL(screenNumber, screenRect, clipRect));
		if (error != KErrNone)
			{
			INFO_PRINTF2(_L("***** Failed to Get Properties ViewFinder Direct V2: err = %d *****"), error);
			TRAPD(error2, PostKickOffTestL(error, EFail));
			if (error2 != KErrNone)
				{
				SetTestStepError(error2);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			return;
			}

		INFO_PRINTF2(_L("ViewFinderScreenNumber2 = %d"), screenNumber);
		INFO_PRINTF2(_L("ViewFinderScreenRectWidth2 = %d"), screenRect.Width());
		INFO_PRINTF2(_L("ViewFinderScreenRectHeight2 = %d"), screenRect.Height());
		INFO_PRINTF2(_L("ViewFinderClipRectWidth2 = %d"), clipRect.Width());
		INFO_PRINTF2(_L("ViewFinderClipRectHeight2 = %d"), clipRect.Height());

		INFO_PRINTF1(_L("Shutdown Test"));
		TRAP(error, iTestShutdown = CAsyncTestShutdown::NewL(this));	
		if (error != KErrNone)
			{
			SetTestStepError(error);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		iTestShutdown->Start(KTSU_ECM_OMX_Interval,KErrNone, EPass);
		}
	}

void CCameraStartViewFinderTestStep::DirectHistogramDisplayed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, CCamera::CCameraV2Histogram& /*aDirectHistogramDisplayed*/, TInt /*aErrorCode*/)
	{
	}

void CCameraStartViewFinderTestStep::ClientHistogramReady(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, MHistogramV2Buffer* /*aClientHistogramBuffer*/, TInt /*aErrorCode*/)
	{
	}

void CCameraStartViewFinderTestStep::DirectSnapshotForImageDisplayed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, CCamera::CCameraImageCapture& /*aCaptureImageHandle*/, TPostCaptureControlId /*aPostCaptureControlId*/, TInt /*aErrorCode*/)
	{
	}

void CCameraStartViewFinderTestStep::DirectViewFinderFailed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, TInt /*aErrorCode*/)
	{
	}

TInt CCameraStartViewFinderTestStep::CustomInterface(TUid /*aInterface*/, TAny*& /*aPtrInterface*/)
	{
	return KErrNone;
	}

//

CCameraRepeatedReserveTestStep::CCameraRepeatedReserveTestStep()
	: iCameraReserve(EFalse)
	{
	SetTestStepName(KCameraRepeatedReserveTestStep);
	}

CCameraRepeatedReserveTestStep::~CCameraRepeatedReserveTestStep()
	{
	if (iRetrieveTimer)
		{
		delete iRetrieveTimer;
		iRetrieveTimer = NULL;
		}

	if (iTestShutdown)
		{
		delete iTestShutdown;
		iTestShutdown = NULL;
		}

	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}
	}

TVerdict CCameraRepeatedReserveTestStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("***** Begin Camera Repeated Reserve Test *****"));
	CreateDependencyFileL();

	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPreambleL();
	}

TVerdict CCameraRepeatedReserveTestStep::doTestStepL()
	{
	TRAPD(error, iCamera = CCamera::New2L(*this, 0, 0));
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to call CCamera::New2L(): err = %d *****"), error);
		REComSession::FinalClose();
		User::Leave(error);
		}
	// issue async call
	INFO_PRINTF1(_L("***** Reserve Camera *****"));
	iCamera->Reserve();

	CActiveScheduler::Start();
	return TestStepResult();
	}

TVerdict CCameraRepeatedReserveTestStep::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("***** Finish Camera Repeated Reserve Test *****"));
	RemoveDependencyFile();

	REComSession::FinalClose();
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPostambleL();
	}

void CCameraRepeatedReserveTestStep::PostKickOffTestL(TInt aReason, TVerdict aResult)
	{
	if (iCameraReserve)
		{
		iCamera->Release();
		iCameraReserve = EFalse;
		INFO_PRINTF1(_L("***** Camera Release *****"));
		}

	SetTestStepError(aReason);
	SetTestStepResult(aResult);
	CActiveScheduler::Stop();
	}

void CCameraRepeatedReserveTestStep::ChangeState()
	{
	if (iCameraReserve)
		{
		iCamera->Release();
		iCameraReserve = EFalse;
		INFO_PRINTF1(_L("***** Camera Release *****"));
		}
	else
		{
		INFO_PRINTF1(_L("***** Reserve Camera *****"));
		iCamera->Reserve();
		}
	}

void CCameraRepeatedReserveTestStep::HandleEvent(const TECAMEvent& aEvent)
	{
	if (aEvent.iEventType == KUidECamEventReserveComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Reserve *****"));
			iCameraReserve = ETrue;

			if (!iRetrieveTimer)
				{
				TRAPD(error, iRetrieveTimer = CRetrieveTimer::NewL(this));
				if (error != KErrNone)
					{
					SetTestStepError(error);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}

				iRetrieveTimer->Start(0x100000);
				
				INFO_PRINTF1(_L("Shutdown Test"));
				TRAP(error, iTestShutdown = CAsyncTestShutdown::NewL(this));
				if (error != KErrNone)
					{
					SetTestStepError(error);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				iTestShutdown->Start(KTSU_ECM_OMX_Interval,KErrNone, EPass);
				}
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Reserve Camera: err = %d *****"), aEvent.iErrorCode);
			SetTestStepError(aEvent.iErrorCode);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		}
	}

void CCameraRepeatedReserveTestStep::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraRepeatedReserveTestStep::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraRepeatedReserveTestStep::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

//

CCameraRepeatedPowerOnTestStep::CCameraRepeatedPowerOnTestStep()
	: iCameraReserve(EFalse), iCameraPowerOn(EFalse)
	{
	SetTestStepName(KCameraRepeatedPowerOnTestStep);
	}

CCameraRepeatedPowerOnTestStep::~CCameraRepeatedPowerOnTestStep()
	{
	if (iRetrieveTimer)
		{
		delete iRetrieveTimer;
		iRetrieveTimer = NULL;
		}

	if (iTestShutdown)
		{
		delete iTestShutdown;
		iTestShutdown = NULL;
		}

	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}
	}

TVerdict CCameraRepeatedPowerOnTestStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("***** Begin Camera Repeated PowerOn Test *****"));
	CreateDependencyFileL();

	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPreambleL();
	}

TVerdict CCameraRepeatedPowerOnTestStep::doTestStepL()
	{
	TRAPD(error, iCamera = CCamera::New2L(*this, 0, 0));
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to call CCamera::New2L(): err = %d *****"), error);
		REComSession::FinalClose();
		User::Leave(error);
		}

	INFO_PRINTF1(_L("***** Reserve Camera *****"));
	iCamera->Reserve();

	CActiveScheduler::Start();
	return TestStepResult();
	}

TVerdict CCameraRepeatedPowerOnTestStep::doTestStepPostambleL()
	{
	RemoveDependencyFile();

	REComSession::FinalClose();
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPostambleL();
	}

void CCameraRepeatedPowerOnTestStep::PostKickOffTestL(TInt aReason, TVerdict aResult)
	{
	if (iCameraPowerOn)
		{
		iCamera->PowerOff();
		iCameraPowerOn = EFalse;
		INFO_PRINTF1(_L("***** Camera Power Off *****"));
		}
	if (iCameraReserve)
		{
		iCamera->Release();
		iCameraReserve = EFalse;
		INFO_PRINTF1(_L("***** Camera Release *****"));
		}

	INFO_PRINTF1(_L("***** Finish Camera Repeated PowerOn Test *****"));
	SetTestStepError(aReason);
	SetTestStepResult(aResult);
	CActiveScheduler::Stop();
	}

void CCameraRepeatedPowerOnTestStep::ChangeState()
	{
	if (iCameraPowerOn)
		{
		iCamera->PowerOff();
		iCameraPowerOn = EFalse;
		INFO_PRINTF1(_L("***** Camera Power Off *****"));
		}
	else
		{
		INFO_PRINTF1(_L("***** Camera Power On *****"));
		iCamera->PowerOn();
		}
	}

void CCameraRepeatedPowerOnTestStep::HandleEvent(const TECAMEvent& aEvent)
	{
	if (aEvent.iEventType == KUidECamEventReserveComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Reserve *****"));
			iCameraReserve = ETrue;
			INFO_PRINTF1(_L("***** Camera Power On *****"));
			iCamera->PowerOn();
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Reserve Camera: err = %d *****"), aEvent.iErrorCode);
			SetTestStepError(aEvent.iErrorCode);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		}
	else if (aEvent.iEventType == KUidECamEventPowerOnComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Power On *****"));
			iCameraPowerOn = ETrue;

			if (!iRetrieveTimer)
				{
				TRAPD(error, iRetrieveTimer = CRetrieveTimer::NewL(this));
				if (error != KErrNone)
					{
					SetTestStepError(error);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}

				iRetrieveTimer->Start(0x100000);

				INFO_PRINTF1(_L("Shutdown Test"));
				TRAP(error, iTestShutdown = CAsyncTestShutdown::NewL(this));
				if (error != KErrNone)
					{
					SetTestStepError(error);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				iTestShutdown->Start(KTSU_ECM_OMX_Interval,KErrNone, EPass);
				}
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Power On Camera: err = %d *****"), aEvent.iErrorCode);
			TRAPD(error, PostKickOffTestL(aEvent.iErrorCode, EFail));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			}
		}
	}

void CCameraRepeatedPowerOnTestStep::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraRepeatedPowerOnTestStep::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraRepeatedPowerOnTestStep::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

//

CCameraStartStopViewFinderTestStep::CCameraStartStopViewFinderTestStep()
	: iCameraReserve(EFalse), iCameraPowerOn(EFalse), iCameraViewFinder(EFalse)
	{
	SetTestStepName(KCameraStartStopViewFinderTestStep);
	}

CCameraStartStopViewFinderTestStep::~CCameraStartStopViewFinderTestStep()
	{
	if (iRetrieveTimer)
		{
		delete iRetrieveTimer;
		iRetrieveTimer = NULL;
		}

	if (iTestShutdown)
		{
		delete iTestShutdown;
		iTestShutdown = NULL;
		}

	if (iCameraDirectVF)
		{
		delete iCameraDirectVF;
		iCameraDirectVF = NULL;
		}

	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}
	}

TVerdict CCameraStartStopViewFinderTestStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("***** Begin Camera Start/Stop ViewFinder Test *****"));
	CreateDependencyFileL();

	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPreambleL();
	}

TVerdict CCameraStartStopViewFinderTestStep::doTestStepL()
	{
	TRAPD(error, iCamera = CCamera::New2L(*this, 0, 0));
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to call CCamera::New2L(): err = %d *****"), error);
		REComSession::FinalClose();
		User::Leave(error);
		}

	INFO_PRINTF1(_L("***** Reserve Camera *****"));
	iCamera->Reserve();	

	CActiveScheduler::Start();
	return TestStepResult();
	}

TVerdict CCameraStartStopViewFinderTestStep::doTestStepPostambleL()
	{
	RemoveDependencyFile();

	INFO_PRINTF1(_L("***** Finish Camera Start/Stop ViewFinder Test *****"));
	REComSession::FinalClose();
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPostambleL();
	}

void CCameraStartStopViewFinderTestStep::PostKickOffTestL(TInt aReason, TVerdict aResult)
	{
	if (iCameraViewFinder)
		{
		iCameraDirectVF->StopDirectViewFinder();
		iCameraViewFinder = EFalse;
		INFO_PRINTF1(_L("***** Stop ViewFinder *****"));
		}
	if (iCameraPowerOn)
		{
		iCamera->PowerOff();
		iCameraPowerOn = EFalse;
		INFO_PRINTF1(_L("***** Camera Power Off *****"));
		}
	if (iCameraReserve)
		{
		iCamera->Release();
		iCameraReserve = EFalse;
		INFO_PRINTF1(_L("***** Camera Release *****"));
		}

	SetTestStepError(aReason);
	SetTestStepResult(aResult);
	CActiveScheduler::Stop();
	}

void CCameraStartStopViewFinderTestStep::ChangeState()
	{
	if (iCameraViewFinder)
		{
		iCameraDirectVF->StopDirectViewFinder();
		iCameraViewFinder = EFalse;
		INFO_PRINTF1(_L("***** Stop ViewFinder *****"));
		CCamera::CCameraV2DirectViewFinder::TViewFinderState vfState;
		TRAPD(error, iCameraDirectVF->GetViewFinderStateL(vfState));
		if (error != KErrNone)
			{
			INFO_PRINTF2(_L("***** Failed to Get State ViewFinder Direct V2: err = %d *****"), error);
			TRAPD(error2, PostKickOffTestL(error, EFail));
			if (error2 != KErrNone)
				{
				SetTestStepError(error2);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			return;
			}
		INFO_PRINTF2(_L("ViewFinderState3 = %d"), vfState);
		}
	else
		{
		INFO_PRINTF1(_L("***** Start ViewFinder *****"));
		TRect rect;
		rect.SetWidth(320);
		rect.SetHeight(240);
		TRAPD(error, iCameraDirectVF->StartViewFinderDirectL(iWsSession, *iWsSd, *iWindow, rect));
		if (error != KErrNone)
			{
			INFO_PRINTF2(_L("***** Failed to Start ViewFinder Direct: err = %d *****"), error);
			TRAPD(error2, PostKickOffTestL(error, EFail));
			if (error2 != KErrNone)
				{
				SetTestStepError(error2);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			}
		}
	}

void CCameraStartStopViewFinderTestStep::HandleEvent(const TECAMEvent& aEvent)
	{
	if (aEvent.iEventType == KUidECamEventReserveComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Reserve *****"));
			iCameraReserve = ETrue;
			INFO_PRINTF1(_L("***** Camera Power On *****"));
			iCamera->PowerOn();
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Reserve Camera: err = %d *****"), aEvent.iErrorCode);
			SetTestStepError(aEvent.iErrorCode);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		}
	else if (aEvent.iEventType == KUidECamEventPowerOnComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Power On *****"));
			iCameraPowerOn = ETrue;

			TRAPD(error, CreateWindowL());
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to Create Window: err = %d *****"), aEvent.iErrorCode);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			INFO_PRINTF1(_L("***** Success Create Window *****"));

			TRect rect;
			rect.SetWidth(320);
			rect.SetHeight(240);
			TRAP(error, iCameraDirectVF = CCamera::CCameraV2DirectViewFinder::NewL(*iCamera, *this));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to Create ViewFinder Direct V2: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}

			CCamera::CCameraV2DirectViewFinder::TViewFinderState vfState;
			TRAP(error, iCameraDirectVF->GetViewFinderStateL(vfState));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to Get State ViewFinder Direct V2: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			INFO_PRINTF2(_L("ViewFinderState1 = %d"), vfState);

			TRAP(error, iCameraDirectVF->StartViewFinderDirectL(iWsSession, *iWsSd, *iWindow, rect));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to Start ViewFinder Direct: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				}
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Power On Camera: error = %d *****"), aEvent.iErrorCode);
			TRAPD(error, PostKickOffTestL(aEvent.iErrorCode, EFail));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			}
		}
	}

void CCameraStartStopViewFinderTestStep::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraStartStopViewFinderTestStep::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraStartStopViewFinderTestStep::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraStartStopViewFinderTestStep::DirectViewFinderFirstFrameDisplayed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, TInt aErrorCode)
	{
	if (aErrorCode != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to Start ViewFinder Direct: err = d *****"), aErrorCode);
		TRAPD(error, PostKickOffTestL(aErrorCode, EFail));
		if (error != KErrNone)
			{
			SetTestStepError(error);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		}
	else
		{
		INFO_PRINTF1(_L("***** Success Start ViewFinder Direct *****"));
		iCameraViewFinder = ETrue;
		CCamera::CCameraV2DirectViewFinder::TViewFinderState vfState;
		TRAPD(error, iCameraDirectVF->GetViewFinderStateL(vfState));
		if (error != KErrNone)
			{
			INFO_PRINTF2(_L("***** Failed to Get State ViewFinder Direct V2: err = %d *****"), error);
			TRAPD(error2, PostKickOffTestL(error, EFail));
			if (error2 != KErrNone)
				{
				SetTestStepError(error2);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			return;
			}
		INFO_PRINTF2(_L("ViewFinderState2 = %d"), vfState);

		if (!iRetrieveTimer)
			{
			TRAPD(error, iRetrieveTimer = CRetrieveTimer::NewL(this));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}

			iRetrieveTimer->Start(0x100000);

			INFO_PRINTF1(_L("Shutdown Test"));
			TRAP(error, iTestShutdown = CAsyncTestShutdown::NewL(this));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			iTestShutdown->Start(KTSU_ECM_OMX_Interval,KErrNone, EPass);
			}
		}
	}

void CCameraStartStopViewFinderTestStep::DirectHistogramDisplayed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, CCamera::CCameraV2Histogram& /*aDirectHistogramDisplayed*/, TInt /*aErrorCode*/)
	{
	}

void CCameraStartStopViewFinderTestStep::ClientHistogramReady(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, MHistogramV2Buffer* /*aClientHistogramBuffer*/, TInt /*aErrorCode*/)
	{
	}

void CCameraStartStopViewFinderTestStep::DirectSnapshotForImageDisplayed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, CCamera::CCameraImageCapture& /*aCaptureImageHandle*/, TPostCaptureControlId /*aPostCaptureControlId*/, TInt /*aErrorCode*/)
	{
	}

void CCameraStartStopViewFinderTestStep::DirectViewFinderFailed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, TInt /*aErrorCode*/)
	{
	}

TInt CCameraStartStopViewFinderTestStep::CustomInterface(TUid /*aInterface*/, TAny*& /*aPtrInterface*/)
	{
	return KErrNone;
	}

//

CCameraReserveThenDestructTestStep::CCameraReserveThenDestructTestStep()
	{
	SetTestStepName(KCameraReserveThenDestructTestStep);
	}

CCameraReserveThenDestructTestStep::~CCameraReserveThenDestructTestStep()
	{
	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}
	}

TVerdict CCameraReserveThenDestructTestStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("***** Begin Camera Reserve then Destruct Test *****"));
	CreateDependencyFileL();

	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPreambleL();
	}

TVerdict CCameraReserveThenDestructTestStep::doTestStepL()
	{
	TRAPD(error, iCamera = CCamera::New2L(*this, 0, 0));
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to call CCamera::New2L(): err = %d *****"), error);
		REComSession::FinalClose();
		User::Leave(error);
		}
	else
		{
		INFO_PRINTF2(_L("Camera Version = %d"), iCamera->CameraVersion());	

		INFO_PRINTF1(_L("***** Camera Reserve *****"));
		iCamera->Reserve();

		CActiveScheduler::Start();
		}

	return TestStepResult();
	}

TVerdict CCameraReserveThenDestructTestStep::doTestStepPostambleL()
	{
	RemoveDependencyFile();

	INFO_PRINTF1(_L("***** Finish Camera Reserve then Destruct Test *****"));
	REComSession::FinalClose();
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPostambleL();
	}

void CCameraReserveThenDestructTestStep::HandleEvent(const TECAMEvent& aEvent)
	{
	if (aEvent.iEventType == KUidECamEventReserveComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Reserve *****"));

			INFO_PRINTF1(_L("***** Destroy Camera *****"));
			delete iCamera;
			iCamera = NULL;

			SetTestStepError(KErrNone);
			SetTestStepResult(EPass);
			CActiveScheduler::Stop();
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Reserve Camera: err = %d *****"), aEvent.iErrorCode);
			SetTestStepError(aEvent.iErrorCode);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		}
	else
		{
		INFO_PRINTF1(_L("***** Unknown event received *****"));
		SetTestStepError(KErrGeneral);
		SetTestStepResult(EFail);
		CActiveScheduler::Stop();		
		}
	}

void CCameraReserveThenDestructTestStep::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraReserveThenDestructTestStep::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}

void CCameraReserveThenDestructTestStep::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}

//

CCameraPowerOnThenDestructTestStep::CCameraPowerOnThenDestructTestStep()
	: iCameraReserve(EFalse)
	{
	SetTestStepName(KCameraPowerOnThenDestructTestStep);
	}

CCameraPowerOnThenDestructTestStep::~CCameraPowerOnThenDestructTestStep()
	{
	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}
	}

TVerdict CCameraPowerOnThenDestructTestStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("***** Begin Camera PowerOn then Destruct Test *****"));
	CreateDependencyFileL();

	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPreambleL();
	}

TVerdict CCameraPowerOnThenDestructTestStep::doTestStepL()
	{
	TRAPD(error, iCamera = CCamera::New2L(*this, 0, 0));
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to call CCamera::New2L(): err = %d *****"), error);
		REComSession::FinalClose();
		User::Leave(error);
		}
	else
		{
		INFO_PRINTF2(_L("Camera Version = %d"), iCamera->CameraVersion());	

		INFO_PRINTF1(_L("***** Camera Reserve *****"));
		iCamera->Reserve();

		CActiveScheduler::Start();
		}

	return TestStepResult();
	}

TVerdict CCameraPowerOnThenDestructTestStep::doTestStepPostambleL()
	{
	RemoveDependencyFile();

	INFO_PRINTF1(_L("***** Finish Camera PowerOn then Destruct Test *****"));
	REComSession::FinalClose();
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPostambleL();
	}

void CCameraPowerOnThenDestructTestStep::PostKickOffTestL(TInt aReason, TVerdict aResult)
	{
	if (iCameraReserve)
		{
		iCamera->Release();
		iCameraReserve = EFalse;
		INFO_PRINTF1(_L("***** Camera Release *****"));
		}

	SetTestStepError(aReason);
	SetTestStepResult(aResult);
	CActiveScheduler::Stop();
	}

void CCameraPowerOnThenDestructTestStep::HandleEvent(const TECAMEvent& aEvent)
	{
	if (aEvent.iEventType == KUidECamEventReserveComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Reserve *****"));
			iCameraReserve = ETrue;
			INFO_PRINTF1(_L("***** Camera PowerOn *****"));
			iCamera->PowerOn();
			delete iCamera;
			iCamera = NULL;

			SetTestStepError(KErrNone);
			SetTestStepResult(EPass);
			CActiveScheduler::Stop();
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Reserve Camera: err = %d *****"), aEvent.iErrorCode);
			SetTestStepError(aEvent.iErrorCode);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		}
	else
		{
		INFO_PRINTF1(_L("***** Unknown event received *****"));
		SetTestStepError(KErrGeneral);
		SetTestStepResult(EFail);
		CActiveScheduler::Stop();		
		}
	}

void CCameraPowerOnThenDestructTestStep::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraPowerOnThenDestructTestStep::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}

void CCameraPowerOnThenDestructTestStep::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}
//

CCameraStartViewFinderThenDestructTestStep::CCameraStartViewFinderThenDestructTestStep()
	: iCameraReserve(EFalse), iCameraPowerOn(EFalse)
	{
	SetTestStepName(KCameraStartViewFinderThenDestructTestStep);
	}

CCameraStartViewFinderThenDestructTestStep::~CCameraStartViewFinderThenDestructTestStep()
	{
	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}
	}

TVerdict CCameraStartViewFinderThenDestructTestStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("***** Begin Camera Start ViewFinder then Destruct Test *****"));
	CreateDependencyFileL();

	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPreambleL();
	}

TVerdict CCameraStartViewFinderThenDestructTestStep::doTestStepL()
	{
	TRAPD(error, iCamera = CCamera::New2L(*this, 0, 0));
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to call CCamera::New2L(): err = %d *****"), error);
		REComSession::FinalClose();
		User::Leave(error);
		}
	else
		{
		INFO_PRINTF2(_L("Camera Version = %d"), iCamera->CameraVersion());	

		INFO_PRINTF1(_L("***** Camera Reserve *****"));
		iCamera->Reserve();

		CActiveScheduler::Start();
		}

	return TestStepResult();
	}


TVerdict CCameraStartViewFinderThenDestructTestStep::doTestStepPostambleL()
	{
	RemoveDependencyFile();

	INFO_PRINTF1(_L("***** Finish Camera Start ViewFinder then Destruct Test *****"));
	REComSession::FinalClose();
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPostambleL();
	}

void CCameraStartViewFinderThenDestructTestStep::PostKickOffTestL(TInt aReason, TVerdict aResult)
	{
	if (iCameraPowerOn)
		{
		iCamera->PowerOff();
		iCameraPowerOn = EFalse;
		INFO_PRINTF1(_L("***** Camera Power Off *****"));
		}
	if (iCameraReserve)
		{
		iCamera->Release();
		iCameraReserve = EFalse;
		INFO_PRINTF1(_L("***** Camera Release *****"));
		}

	SetTestStepError(aReason);
	SetTestStepResult(aResult);
	CActiveScheduler::Stop();
	}

void CCameraStartViewFinderThenDestructTestStep::HandleEvent(const TECAMEvent& aEvent)
	{
	if (aEvent.iEventType == KUidECamEventReserveComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Reserve *****"));
			iCameraReserve = ETrue;
			INFO_PRINTF1(_L("***** Camera Power On *****"));
			iCamera->PowerOn();
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Reserve Camera: err = %d *****"), aEvent.iErrorCode);
			SetTestStepError(aEvent.iErrorCode);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		}
	else if (aEvent.iEventType == KUidECamEventPowerOnComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Power On *****"));
			iCameraPowerOn = ETrue;

			TRAPD(error, CreateWindowL());
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to Create Window: err = %d *****"), aEvent.iErrorCode);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			INFO_PRINTF1(_L("***** Success Create Window *****"));

			TRect rect;
			rect.SetWidth(320);
			rect.SetHeight(240);
			TRAP(error, iCameraDirectVF = CCamera::CCameraV2DirectViewFinder::NewL(*iCamera, *this));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to Create ViewFinder Direct V2: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}

			TRAP(error, iCameraDirectVF->StartViewFinderDirectL(iWsSession, *iWsSd, *iWindow, rect));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to Start ViewFinder Direct: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				}
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Power On Camera: err = %d *****"), aEvent.iErrorCode);
			TRAPD(error, PostKickOffTestL(aEvent.iErrorCode, EFail));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			}
		}
	else
		{
		INFO_PRINTF1(_L("***** Unknown event received *****"));
		SetTestStepError(KErrGeneral);
		SetTestStepResult(EFail);
		CActiveScheduler::Stop();		
		}
	}

void CCameraStartViewFinderThenDestructTestStep::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraStartViewFinderThenDestructTestStep::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}

void CCameraStartViewFinderThenDestructTestStep::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}

void CCameraStartViewFinderThenDestructTestStep::DirectViewFinderFirstFrameDisplayed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, TInt aErrorCode)
	{
	if (aErrorCode != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to Start ViewFinder Direct: err = %d *****"), aErrorCode);
		TRAPD(error, PostKickOffTestL(aErrorCode, EFail));
		if (error != KErrNone)
			{
			SetTestStepError(error);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		}
	else
		{
		INFO_PRINTF1(_L("***** Success Start ViewFinder Direct *****"));

		delete iCamera;
		iCamera = NULL;

		SetTestStepError(KErrNone);
		SetTestStepResult(EPass);
		CActiveScheduler::Stop();
		}
	}

void CCameraStartViewFinderThenDestructTestStep::DirectHistogramDisplayed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, CCamera::CCameraV2Histogram& /*aDirectHistogramDisplayed*/, TInt /*aErrorCode*/)
	{
	}

void CCameraStartViewFinderThenDestructTestStep::ClientHistogramReady(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, MHistogramV2Buffer* /*aClientHistogramBuffer*/, TInt /*aErrorCode*/)
	{
	}

void CCameraStartViewFinderThenDestructTestStep::DirectSnapshotForImageDisplayed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, CCamera::CCameraImageCapture& /*aCaptureImageHandle*/, TPostCaptureControlId /*aPostCaptureControlId*/, TInt /*aErrorCode*/)
	{
	}

void CCameraStartViewFinderThenDestructTestStep::DirectViewFinderFailed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, TInt /*aErrorCode*/)
	{
	}

TInt CCameraStartViewFinderThenDestructTestStep::CustomInterface(TUid /*aInterface*/, TAny*& /*aPtrInterface*/)
	{
	return KErrNone;
	}

//

CCameraReserveTwiceTestStep::CCameraReserveTwiceTestStep()
	: iCameraReserve(EFalse)
	{
	SetTestStepName(KCameraReserveTwiceTestStep);
	}

CCameraReserveTwiceTestStep::~CCameraReserveTwiceTestStep()
	{
	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}
	}

TVerdict CCameraReserveTwiceTestStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("***** Begin Camera Reserve twice Test *****"));
	CreateDependencyFileL();

	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPreambleL();
	}

TVerdict CCameraReserveTwiceTestStep::doTestStepL()
	{
	TRAPD(error, iCamera = CCamera::New2L(*this, 0, 0));
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to call CCamera::New2L(): err = %d *****"), error);
		REComSession::FinalClose();
		User::Leave(error);
		}
	else
		{
		INFO_PRINTF2(_L("Camera Version = %d"), iCamera->CameraVersion());	

		INFO_PRINTF1(_L("***** Camera Reserve *****"));
		iCamera->Reserve();

		CActiveScheduler::Start();
		}

	return TestStepResult();
	}


TVerdict CCameraReserveTwiceTestStep::doTestStepPostambleL()
	{
	RemoveDependencyFile();

	INFO_PRINTF1(_L("***** Finish Camera Reserve twice Test *****"));
	REComSession::FinalClose();
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPostambleL();
	}

void CCameraReserveTwiceTestStep::PostKickOffTestL(TInt aReason, TVerdict aResult)
	{
	if (iCameraReserve)
		{
		iCamera->Release();
		iCameraReserve = EFalse;
		INFO_PRINTF1(_L("***** Camera Release *****"));
		}

	SetTestStepError(aReason);
	SetTestStepResult(aResult);
	CActiveScheduler::Stop();
	}

void CCameraReserveTwiceTestStep::HandleEvent(const TECAMEvent& aEvent)
	{
	if (aEvent.iEventType == KUidECamEventReserveComplete)
		{
		if (aEvent.iErrorCode == KErrNone && !iCameraReserve)
			{
			INFO_PRINTF1(_L("***** Success Camera Reserve *****"));
			iCameraReserve = ETrue;

			INFO_PRINTF1(_L("***** Repeat Camera Reserve *****"));
			iCamera->Reserve();
			}
		else
			{
			if (aEvent.iErrorCode == KErrNone)
				{
				INFO_PRINTF1(_L("***** Second Reserve attempt succeeded *****"));
				}
			else
				{
				INFO_PRINTF2(_L("***** Failed Second Reserve attempt: err = %d *****"), aEvent.iErrorCode);
				}
			TRAPD(error, PostKickOffTestL(aEvent.iErrorCode, EFail));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			}
		}
	else
		{
		INFO_PRINTF1(_L("***** Unknown event received *****"));
		SetTestStepError(KErrGeneral);
		SetTestStepResult(EFail);
		CActiveScheduler::Stop();		
		}
	}

void CCameraReserveTwiceTestStep::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraReserveTwiceTestStep::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}

void CCameraReserveTwiceTestStep::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}

//

CCameraPowerOnTwiceTestStep::CCameraPowerOnTwiceTestStep()
	: iCameraReserve(EFalse), iCameraPowerOn(EFalse)
	{
	SetTestStepName(KCameraPowerOnTwiceTestStep);
	}

CCameraPowerOnTwiceTestStep::~CCameraPowerOnTwiceTestStep()
	{
	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}
	}

TVerdict CCameraPowerOnTwiceTestStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("***** Begin Camera PowerOn twice Test *****"));
	CreateDependencyFileL();

	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPreambleL();
	}

TVerdict CCameraPowerOnTwiceTestStep::doTestStepL()
	{
	TRAPD(error, iCamera = CCamera::New2L(*this, 0, 0));
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to call CCamera::New2L(): err = %d *****"), error);
		REComSession::FinalClose();
		User::Leave(error);
		}
	else
		{

		INFO_PRINTF2(_L("Camera Version = %d"), iCamera->CameraVersion());

		INFO_PRINTF1(_L("***** Camera Reserve *****"));
		iCamera->Reserve();

		CActiveScheduler::Start();
		}

	return TestStepResult();
	}


TVerdict CCameraPowerOnTwiceTestStep::doTestStepPostambleL()
	{
	RemoveDependencyFile();

	INFO_PRINTF1(_L("***** Finish Camera PowerOn twice Test *****"));
	REComSession::FinalClose();
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPostambleL();
	}

void CCameraPowerOnTwiceTestStep::PostKickOffTestL(TInt aReason, TVerdict aResult)
	{
	if (iCameraPowerOn)
		{
		iCamera->PowerOff();
		iCameraPowerOn = EFalse;
		INFO_PRINTF1(_L("***** Camera Power Off *****"));
		}
	if (iCameraReserve)
		{
		iCamera->Release();
		iCameraReserve = EFalse;
		INFO_PRINTF1(_L("***** Camera Release *****"));
		}

	SetTestStepError(aReason);
	SetTestStepResult(aResult);
	CActiveScheduler::Stop();
	}

void CCameraPowerOnTwiceTestStep::HandleEvent(const TECAMEvent& aEvent)
	{
	if (aEvent.iEventType == KUidECamEventReserveComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Reserve *****"));
			iCameraReserve = ETrue;

			INFO_PRINTF1(_L("***** Camera Power On *****"));
			iCamera->PowerOn();
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Reserve Camera: err = %d *****"), aEvent.iErrorCode);
			SetTestStepError(aEvent.iErrorCode);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		}
	else if (aEvent.iEventType == KUidECamEventPowerOnComplete)
		{
		if (aEvent.iErrorCode == KErrNone && !iCameraPowerOn)
			{
			INFO_PRINTF1(_L("***** Success Camera Power On *****"));
			iCameraPowerOn = ETrue;

			iCamera->PowerOn();
			}
		else
			{
			if (aEvent.iErrorCode == KErrNone)
				{
				INFO_PRINTF1(_L("***** Second Power On attempt succeeded *****"));
				}
			else
				{
				INFO_PRINTF2(_L("***** Failed Second Power On attempt: err = %d *****"), aEvent.iErrorCode);
				}
			TRAPD(error, PostKickOffTestL(aEvent.iErrorCode, EFail));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			}
		}
	else
		{
		INFO_PRINTF1(_L("***** Unknown event received *****"));
		SetTestStepError(KErrGeneral);
		SetTestStepResult(EFail);
		CActiveScheduler::Stop();		
		}
	}

void CCameraPowerOnTwiceTestStep::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraPowerOnTwiceTestStep::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}

void CCameraPowerOnTwiceTestStep::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}

//

CCameraPowerOnWithoutReserveTestStep::CCameraPowerOnWithoutReserveTestStep()
	: iCameraReserve(EFalse), iCameraPowerOn(EFalse)
	{
	SetTestStepName(KCameraPowerOnWithoutReserveTestStep);
	}

CCameraPowerOnWithoutReserveTestStep::~CCameraPowerOnWithoutReserveTestStep()
	{
	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}
	}

TVerdict CCameraPowerOnWithoutReserveTestStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("***** Begin Camera PowerOn without Reserve Test *****"));
	CreateDependencyFileL();

	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPreambleL();
	}

TVerdict CCameraPowerOnWithoutReserveTestStep::doTestStepL()
	{
	TRAPD(error, iCamera = CCamera::New2L(*this, 0, 0));
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to call CCamera::New2L(): err = %d *****"), error);
		REComSession::FinalClose();
		User::Leave(error);
		}
	else
		{

		INFO_PRINTF2(_L("Camera Version = %d"), iCamera->CameraVersion());

		INFO_PRINTF1(_L("***** Camera Power On *****"));
		iCamera->PowerOn();

		CActiveScheduler::Start();
		}

	return TestStepResult();
	}

TVerdict CCameraPowerOnWithoutReserveTestStep::doTestStepPostambleL()
	{
	RemoveDependencyFile();

	INFO_PRINTF1(_L("***** Finish Camera PowerOn without Reserve Test *****"));
	REComSession::FinalClose();
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPostambleL();
	}

void CCameraPowerOnWithoutReserveTestStep::PostKickOffTestL(TInt aReason, TVerdict aResult)
	{
	if (iCameraPowerOn)
		{
		iCamera->PowerOff();
		iCameraPowerOn = EFalse;
		INFO_PRINTF1(_L("***** Camera Power Off *****"));
		}
	if (iCameraReserve)
		{
		iCamera->Release();
		iCameraReserve = EFalse;
		INFO_PRINTF1(_L("***** Camera Release *****"));
		}

	SetTestStepError(aReason);
	SetTestStepResult(aResult);
	CActiveScheduler::Stop();
	}

void CCameraPowerOnWithoutReserveTestStep::HandleEvent(const TECAMEvent& aEvent)
	{
	if (aEvent.iEventType == KUidECamEventReserveComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Reserve *****"));
			iCameraReserve = ETrue;

			INFO_PRINTF1(_L("***** Camera Power On *****"));
			iCamera->PowerOn();
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Reserve Camera: err = %d *****"), aEvent.iErrorCode);
			SetTestStepError(aEvent.iErrorCode);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		}
	else if (aEvent.iEventType == KUidECamEventPowerOnComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Power On *****"));
			iCameraPowerOn = ETrue;

			SetTestStepResult(EPass);
			CActiveScheduler::Stop();
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Power On Camera: err = %d *****"), aEvent.iErrorCode);
			TRAPD(error, PostKickOffTestL(aEvent.iErrorCode, EFail));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			}
		}
	}

void CCameraPowerOnWithoutReserveTestStep::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraPowerOnWithoutReserveTestStep::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}

void CCameraPowerOnWithoutReserveTestStep::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}

//

CCameraPowerOnAfterReleaseTestStep::CCameraPowerOnAfterReleaseTestStep()
	: iCameraReserve(EFalse), iCameraPowerOn(EFalse)
	{
	SetTestStepName(KCameraPowerOnAfterReleaseTestStep);
	}

CCameraPowerOnAfterReleaseTestStep::~CCameraPowerOnAfterReleaseTestStep()
	{
	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}
	}

TVerdict CCameraPowerOnAfterReleaseTestStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("***** Begin Camera PowerOn after Release Test *****"));
	CreateDependencyFileL();

	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPreambleL();
	}

TVerdict CCameraPowerOnAfterReleaseTestStep::doTestStepL()
	{
	TRAPD(error, iCamera = CCamera::New2L(*this, 0, 0));
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to call CCamera::New2L(): err = %d *****"), error);
		REComSession::FinalClose();
		User::Leave(error);
		}
	else
		{
		INFO_PRINTF2(_L("Camera Version = %d"), iCamera->CameraVersion());

		INFO_PRINTF1(_L("***** Reserve Camera *****"));
		iCamera->Reserve();

		CActiveScheduler::Start();
		}

	return TestStepResult();
	}

TVerdict CCameraPowerOnAfterReleaseTestStep::doTestStepPostambleL()
	{
	RemoveDependencyFile();

	INFO_PRINTF1(_L("***** Finish Camera PowerOn after Release Test *****"));
	REComSession::FinalClose();
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPostambleL();
	}

void CCameraPowerOnAfterReleaseTestStep::PostKickOffTestL(TInt aReason, TVerdict aResult)
	{
	if (iCameraPowerOn)
		{
		iCamera->PowerOff();
		iCameraPowerOn = EFalse;
		INFO_PRINTF1(_L("***** Camera Power Off *****"));
		}
	if (iCameraReserve)
		{
		iCamera->Release();
		iCameraReserve = EFalse;
		INFO_PRINTF1(_L("***** Camera Release *****"));
		}

	SetTestStepError(aReason);
	SetTestStepResult(aResult);
	CActiveScheduler::Stop();
	}

void CCameraPowerOnAfterReleaseTestStep::HandleEvent(const TECAMEvent& aEvent)
	{
	if (aEvent.iEventType == KUidECamEventReserveComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Reserve *****"));
			iCameraReserve = ETrue;

			iCamera->Release();
			iCameraReserve = EFalse;
			INFO_PRINTF1(_L("***** Camera Release *****"));

			INFO_PRINTF1(_L("***** Camera Power On *****"));
			iCamera->PowerOn();
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Reserve Camera: err = %d *****"), aEvent.iErrorCode);
			SetTestStepError(aEvent.iErrorCode);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		}
	else if (aEvent.iEventType == KUidECamEventPowerOnComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Power On *****"));
			iCameraPowerOn = ETrue;

			SetTestStepResult(EPass);
			CActiveScheduler::Stop();
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Power On Camera: err = %d *****"), aEvent.iErrorCode);
			TRAPD(error, PostKickOffTestL(aEvent.iErrorCode, EFail));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			}
		}
	}

void CCameraPowerOnAfterReleaseTestStep::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraPowerOnAfterReleaseTestStep::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}

void CCameraPowerOnAfterReleaseTestStep::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}

//

CCameraStartViewFinderWithoutReserveTestStep::CCameraStartViewFinderWithoutReserveTestStep()
	: iCameraReserve(EFalse), iCameraPowerOn(EFalse), iCameraViewFinder(EFalse)
	{
	SetTestStepName(KCameraStartViewFinderWithoutReserveTestStep);
	}

CCameraStartViewFinderWithoutReserveTestStep::~CCameraStartViewFinderWithoutReserveTestStep()
	{
	if (iCameraDirectVF)
		{
		delete iCameraDirectVF;
		iCameraDirectVF = NULL;
		}

	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}
	}

TVerdict CCameraStartViewFinderWithoutReserveTestStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("***** Begin Camera Start ViewFinder without Reserve Test *****"));
	CreateDependencyFileL();

	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPreambleL();
	}

TVerdict CCameraStartViewFinderWithoutReserveTestStep::doTestStepL()
	{
	TRAPD(error, iCamera = CCamera::New2L(*this, 0, 0));
	if (error != KErrNone)
		{
		INFO_PRINTF1(_L("***** Failed to call CCamera::New2L() *****"));
		REComSession::FinalClose();
		User::Leave(error);
		}

	TRAP(error, CreateWindowL());
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to Create Window: err = %d *****"), error);
		TRAPD(error2, PostKickOffTestL(error, EFail));
		if (error2 != KErrNone)
			{
			SetTestStepError(error2);
			SetTestStepResult(EFail);
			return TestStepResult();
			}
		return TestStepResult();
		}
	INFO_PRINTF1(_L("***** Success Create Window *****"));

	TRect rect;
	rect.SetWidth(320);
	rect.SetHeight(240);
	TRAP(error, iCameraDirectVF = CCamera::CCameraV2DirectViewFinder::NewL(*iCamera, *this));
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to Create ViewFinder Direct V2: err = %d *****"), error);
		TRAPD(error2, PostKickOffTestL(error, EFail));
		if (error2 != KErrNone)
			{
			SetTestStepError(error2);
			SetTestStepResult(EFail);
			return TestStepResult();
			}
		return TestStepResult();
		}

	CCamera::CCameraV2DirectViewFinder::TViewFinderState vfState;
	TRAP(error, iCameraDirectVF->GetViewFinderStateL(vfState));
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to Get State ViewFinder Direct V2: err = %d *****"), error);
		TRAPD(error2, PostKickOffTestL(error, EFail));
		if (error2 != KErrNone)
			{
			SetTestStepError(error2);
			SetTestStepResult(EFail);
			return TestStepResult();
			}
		return TestStepResult();
		}
	INFO_PRINTF2(_L("ViewFinderState1 = %d"), vfState);

	TRAP(error, iCameraDirectVF->StartViewFinderDirectL(iWsSession, *iWsSd, *iWindow, rect));
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to Start ViewFinder Direct: err = %d *****"), error);
		TRAPD(error2, PostKickOffTestL(error, EFail));
		if (error2 != KErrNone)
			{
			SetTestStepError(error2);
			SetTestStepResult(EFail);
			return TestStepResult();
			}
		return TestStepResult();
		}

	return TestStepResult();
	}

TVerdict CCameraStartViewFinderWithoutReserveTestStep::doTestStepPostambleL()
	{
	RemoveDependencyFile();

	INFO_PRINTF1(_L("***** Finish Camera Start ViewFinder without Reserve Test *****"));
	REComSession::FinalClose();
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPostambleL();
	}

void CCameraStartViewFinderWithoutReserveTestStep::PostKickOffTestL(TInt aReason, TVerdict aResult)
	{
	if (iCameraViewFinder)
		{
		iCameraDirectVF->StopDirectViewFinder();
		iCameraViewFinder = EFalse;
		INFO_PRINTF1(_L("***** Stop ViewFinder *****"));
		}
	if (iCameraPowerOn)
		{
		iCamera->PowerOff();
		iCameraPowerOn = EFalse;
		INFO_PRINTF1(_L("***** Camera Power Off *****"));
		}
	if (iCameraReserve)
		{
		iCamera->Release();
		iCameraReserve = EFalse;
		INFO_PRINTF1(_L("***** Camera Release *****"));
		}

	SetTestStepError(aReason);
	SetTestStepResult(aResult);
	}

void CCameraStartViewFinderWithoutReserveTestStep::HandleEvent(const TECAMEvent& /*aEvent*/)
	{
	}

void CCameraStartViewFinderWithoutReserveTestStep::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraStartViewFinderWithoutReserveTestStep::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraStartViewFinderWithoutReserveTestStep::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraStartViewFinderWithoutReserveTestStep::DirectViewFinderFirstFrameDisplayed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, TInt /*aErrorCode*/)
	{
	}

void CCameraStartViewFinderWithoutReserveTestStep::DirectHistogramDisplayed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, CCamera::CCameraV2Histogram& /*aDirectHistogramDisplayed*/, TInt /*aErrorCode*/)
	{
	}

void CCameraStartViewFinderWithoutReserveTestStep::ClientHistogramReady(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, MHistogramV2Buffer* /*aClientHistogramBuffer*/, TInt /*aErrorCode*/)
	{
	}

void CCameraStartViewFinderWithoutReserveTestStep::DirectSnapshotForImageDisplayed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, CCamera::CCameraImageCapture& /*aCaptureImageHandle*/, TPostCaptureControlId /*aPostCaptureControlId*/, TInt /*aErrorCode*/)
	{
	}

void CCameraStartViewFinderWithoutReserveTestStep::DirectViewFinderFailed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, TInt /*aErrorCode*/)
	{
	}

TInt CCameraStartViewFinderWithoutReserveTestStep::CustomInterface(TUid /*aInterface*/, TAny*& /*aPtrInterface*/)
	{
	return KErrNone;
	}

//

CCameraStartViewFinderWithoutPowerOnTestStep::CCameraStartViewFinderWithoutPowerOnTestStep()
	: iCameraReserve(EFalse), iCameraPowerOn(EFalse), iCameraViewFinder(EFalse)
	{
	SetTestStepName(KCameraStartViewFinderWithoutPowerOnTestStep);
	}

CCameraStartViewFinderWithoutPowerOnTestStep::~CCameraStartViewFinderWithoutPowerOnTestStep()
	{
	if (iTestShutdown)
		{
		delete iTestShutdown;
		iTestShutdown = NULL;
		}

	if (iCameraDirectVF)
		{
		delete iCameraDirectVF;
		iCameraDirectVF = NULL;
		}

	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}
	}

TVerdict CCameraStartViewFinderWithoutPowerOnTestStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("***** Begin Camera Start ViewFinder without PowerOn Test *****"));
	CreateDependencyFileL();

	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPreambleL();
	}

TVerdict CCameraStartViewFinderWithoutPowerOnTestStep::doTestStepL()
	{
	TRAPD(error, iCamera = CCamera::New2L(*this, 0, 0));
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to call CCamera::New2L(): err = %d *****"), error);
		REComSession::FinalClose();
		User::Leave(error);
		}

	INFO_PRINTF1(_L("***** Reserve Camera *****"));
	iCamera->Reserve();

	CActiveScheduler::Start();
	return TestStepResult();
	}

TVerdict CCameraStartViewFinderWithoutPowerOnTestStep::doTestStepPostambleL()
	{
	RemoveDependencyFile();

	INFO_PRINTF1(_L("***** Finish Camera Start ViewFinder without PowerOn Test *****"));
	REComSession::FinalClose();
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPostambleL();
	}

void CCameraStartViewFinderWithoutPowerOnTestStep::PostKickOffTestL(TInt aReason, TVerdict aResult)
	{
	if (iCameraViewFinder)
		{
		iCameraDirectVF->StopDirectViewFinder();
		iCameraViewFinder = EFalse;
		INFO_PRINTF1(_L("***** Stop ViewFinder *****"));
		}
	if (iCameraPowerOn)
		{
		iCamera->PowerOff();
		iCameraPowerOn = EFalse;
		INFO_PRINTF1(_L("***** Camera Power Off *****"));
		}
	if (iCameraReserve)
		{
		iCamera->Release();
		iCameraReserve = EFalse;
		INFO_PRINTF1(_L("***** Camera Release *****"));
		}

	SetTestStepError(aReason);
	SetTestStepResult(aResult);
	CActiveScheduler::Stop();
	}

void CCameraStartViewFinderWithoutPowerOnTestStep::HandleEvent(const TECAMEvent& aEvent)
	{
	if (aEvent.iEventType == KUidECamEventReserveComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Reserve *****"));
			iCameraReserve = ETrue;

			TRAPD(error, CreateWindowL());
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to Create Window: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			INFO_PRINTF1(_L("***** Success Create Window *****"));

			TRect rect;
			rect.SetWidth(320);
			rect.SetHeight(240);
			TRAP(error, iCameraDirectVF = CCamera::CCameraV2DirectViewFinder::NewL(*iCamera, *this));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to Create ViewFinder Direct V2: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}

			CCamera::CCameraV2DirectViewFinder::TViewFinderState vfState;
			TRAP(error, iCameraDirectVF->GetViewFinderStateL(vfState));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to Get State ViewFinder Direct V2: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			INFO_PRINTF2(_L("ViewFinderState1 = %d"), vfState);

			TRAP(error, iCameraDirectVF->StartViewFinderDirectL(iWsSession, *iWsSd, *iWindow, rect));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to Start ViewFinder Direct: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Reserve Camera: err = %d *****"), aEvent.iErrorCode);
			SetTestStepError(aEvent.iErrorCode);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		}
	else if (aEvent.iEventType == KUidECamEventPowerOnComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Power On *****"));
			iCameraPowerOn = ETrue;
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Power On Camera: err = %d *****"), aEvent.iErrorCode);
			TRAPD(error, PostKickOffTestL(aEvent.iErrorCode, EFail));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			return;
			}
		}
	}

void CCameraStartViewFinderWithoutPowerOnTestStep::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraStartViewFinderWithoutPowerOnTestStep::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraStartViewFinderWithoutPowerOnTestStep::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraStartViewFinderWithoutPowerOnTestStep::DirectViewFinderFirstFrameDisplayed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, TInt aErrorCode)
	{
	if (aErrorCode != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to Start ViewFinder Direct: err = %d *****"), aErrorCode);
		TRAPD(error, PostKickOffTestL(aErrorCode, EFail));
		if (error != KErrNone)
			{
			SetTestStepError(error);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		return;
		}
	else
		{
		INFO_PRINTF1(_L("***** Success Start ViewFinder Direct *****"));
		iCameraViewFinder = ETrue;
		CCamera::CCameraV2DirectViewFinder::TViewFinderState vfState;
		TRAPD(error, iCameraDirectVF->GetViewFinderStateL(vfState));
		if (error != KErrNone)
			{
			INFO_PRINTF2(_L("***** Failed to Get State ViewFinder Direct V2: err = %d *****"), error);
			TRAPD(error2, PostKickOffTestL(error, EFail));
			if (error2 != KErrNone)
				{
				SetTestStepError(error2);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			return;
			}
		INFO_PRINTF2(_L("ViewFinderState2 = %d"), vfState);
		INFO_PRINTF1(_L("Shutdown Test"));
		TRAP(error, iTestShutdown = CAsyncTestShutdown::NewL(this));
		if (error != KErrNone)
			{
			SetTestStepError(error);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		iTestShutdown->Start(KTSU_ECM_OMX_Interval,KErrNone, EPass);
		}
	}

void CCameraStartViewFinderWithoutPowerOnTestStep::DirectHistogramDisplayed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, CCamera::CCameraV2Histogram& /*aDirectHistogramDisplayed*/, TInt /*aErrorCode*/)
	{
	}

void CCameraStartViewFinderWithoutPowerOnTestStep::ClientHistogramReady(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, MHistogramV2Buffer* /*aClientHistogramBuffer*/, TInt /*aErrorCode*/)
	{
	}

void CCameraStartViewFinderWithoutPowerOnTestStep::DirectSnapshotForImageDisplayed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, CCamera::CCameraImageCapture& /*aCaptureImageHandle*/, TPostCaptureControlId /*aPostCaptureControlId*/, TInt /*aErrorCode*/)
	{
	}

void CCameraStartViewFinderWithoutPowerOnTestStep::DirectViewFinderFailed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, TInt /*aErrorCode*/)
	{
	}

TInt CCameraStartViewFinderWithoutPowerOnTestStep::CustomInterface(TUid /*aInterface*/, TAny*& /*aPtrInterface*/)
	{
	return KErrNone;
	}

//

CCameraStartViewFinderAfterPowerOffTestStep::CCameraStartViewFinderAfterPowerOffTestStep()
	: iCameraReserve(EFalse), iCameraPowerOn(EFalse), iCameraViewFinder(EFalse)
	{
	SetTestStepName(KCameraStartViewFinderAfterPowerOffTestStep);
	}

CCameraStartViewFinderAfterPowerOffTestStep::~CCameraStartViewFinderAfterPowerOffTestStep()
	{
	if (iTestShutdown)
		{
		delete iTestShutdown;
		iTestShutdown = NULL;
		}

	if (iCameraDirectVF)
		{
		delete iCameraDirectVF;
		iCameraDirectVF = NULL;
		}

	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}
	}

TVerdict CCameraStartViewFinderAfterPowerOffTestStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("***** Begin Camera Start ViewFinder after PowerOff Test *****"));
	CreateDependencyFileL();

	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPreambleL();
	}

TVerdict CCameraStartViewFinderAfterPowerOffTestStep::doTestStepL()
	{
	TRAPD(error, iCamera = CCamera::New2L(*this, 0, 0));
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to call CCamera::New2L(): err = %d *****"), error);
		REComSession::FinalClose();
		User::Leave(error);
		}

	INFO_PRINTF1(_L("***** Reserve Camera *****"));
	iCamera->Reserve();

	CActiveScheduler::Start();
	return TestStepResult();
	}

TVerdict CCameraStartViewFinderAfterPowerOffTestStep::doTestStepPostambleL()
	{
	RemoveDependencyFile();

	INFO_PRINTF1(_L("***** Finish Camera Start ViewFinder after PowerOff Test *****"));
	REComSession::FinalClose();
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPostambleL();
	}

void CCameraStartViewFinderAfterPowerOffTestStep::PostKickOffTestL(TInt aReason, TVerdict aResult)
	{
	if (iCameraViewFinder)
		{
		iCameraDirectVF->StopDirectViewFinder();
		iCameraViewFinder = EFalse;
		INFO_PRINTF1(_L("***** Stop ViewFinder *****"));
		}
	if (iCameraPowerOn)
		{
		iCamera->PowerOff();
		iCameraPowerOn = EFalse;
		INFO_PRINTF1(_L("***** Camera Power Off *****"));
		}
	if (iCameraReserve)
		{
		iCamera->Release();
		iCameraReserve = EFalse;
		INFO_PRINTF1(_L("***** Camera Release *****"));
		}

	SetTestStepError(aReason);
	SetTestStepResult(aResult);
	CActiveScheduler::Stop();
	}

void CCameraStartViewFinderAfterPowerOffTestStep::HandleEvent(const TECAMEvent& aEvent)
	{
	if (aEvent.iEventType == KUidECamEventReserveComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Reserve *****"));
			iCameraReserve = ETrue;
			
			INFO_PRINTF1(_L("***** Camera Power On *****"));
			iCamera->PowerOn();
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Reserve Camera: err = %d *****"), aEvent.iErrorCode);
			SetTestStepError(aEvent.iErrorCode);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		}
	else if (aEvent.iEventType == KUidECamEventPowerOnComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Power On *****"));
			iCameraPowerOn = ETrue;

			iCamera->PowerOff();
			iCameraPowerOn = EFalse;
			INFO_PRINTF1(_L("***** Camera Power Off *****"));

			TRAPD(error, CreateWindowL());
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to Create Window: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			INFO_PRINTF1(_L("***** Success Create Window *****"));

			TRect rect;
			rect.SetWidth(320);
			rect.SetHeight(240);
			TRAP(error, iCameraDirectVF = CCamera::CCameraV2DirectViewFinder::NewL(*iCamera, *this));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to Create ViewFinder Direct V2: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}

			CCamera::CCameraV2DirectViewFinder::TViewFinderState vfState;
			TRAP(error, iCameraDirectVF->GetViewFinderStateL(vfState));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to Get State ViewFinder Direct V2: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			INFO_PRINTF2(_L("ViewFinderState1 = %d"), vfState);

			TRAP(error, iCameraDirectVF->StartViewFinderDirectL(iWsSession, *iWsSd, *iWindow, rect));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to Start ViewFinder Direct: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Power On Camera: err = %d *****"), aEvent.iErrorCode);
			TRAPD(error, PostKickOffTestL(aEvent.iErrorCode, EFail));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			return;
			}
		}
	}

void CCameraStartViewFinderAfterPowerOffTestStep::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraStartViewFinderAfterPowerOffTestStep::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraStartViewFinderAfterPowerOffTestStep::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraStartViewFinderAfterPowerOffTestStep::DirectViewFinderFirstFrameDisplayed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, TInt aErrorCode)
	{
	if (aErrorCode != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to Start ViewFinder Direct: err = %d *****"), aErrorCode);
		TRAPD(error, PostKickOffTestL(aErrorCode, EFail));
		if (error != KErrNone)
			{
			SetTestStepError(error);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		return;
		}
	else
		{
		INFO_PRINTF1(_L("***** Success Start ViewFinder Direct *****"));
		iCameraViewFinder = ETrue;
		CCamera::CCameraV2DirectViewFinder::TViewFinderState vfState;
		TRAPD(error, iCameraDirectVF->GetViewFinderStateL(vfState));
		if (error != KErrNone)
			{
			INFO_PRINTF2(_L("***** Failed to Get State ViewFinder Direct V2: err = %d *****"), error);
			TRAPD(error2, PostKickOffTestL(error, EFail));
			if (error2 != KErrNone)
				{
				SetTestStepError(error2);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			return;
			}
		INFO_PRINTF2(_L("ViewFinderState2 = %d"), vfState);
		INFO_PRINTF1(_L("Shutdown Test"));
		TRAP(error, iTestShutdown = CAsyncTestShutdown::NewL(this));
		if (error != KErrNone)
			{
			SetTestStepError(error);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		iTestShutdown->Start(KTSU_ECM_OMX_Interval,KErrNone, EPass);
		}
	}

void CCameraStartViewFinderAfterPowerOffTestStep::DirectHistogramDisplayed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, CCamera::CCameraV2Histogram& /*aDirectHistogramDisplayed*/, TInt /*aErrorCode*/)
	{
	}

void CCameraStartViewFinderAfterPowerOffTestStep::ClientHistogramReady(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, MHistogramV2Buffer* /*aClientHistogramBuffer*/, TInt /*aErrorCode*/)
	{
	}

void CCameraStartViewFinderAfterPowerOffTestStep::DirectSnapshotForImageDisplayed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, CCamera::CCameraImageCapture& /*aCaptureImageHandle*/, TPostCaptureControlId /*aPostCaptureControlId*/, TInt /*aErrorCode*/)
	{
	}

void CCameraStartViewFinderAfterPowerOffTestStep::DirectViewFinderFailed(CCamera::CCameraV2DirectViewFinder& /*aDirectViewFinderHandle*/, TInt /*aErrorCode*/)
	{
	}

TInt CCameraStartViewFinderAfterPowerOffTestStep::CustomInterface(TUid /*aInterface*/, TAny*& /*aPtrInterface*/)
	{
	return KErrNone;
	}

//

CCameraSecondClient::CCameraSecondClient(CCameraMultiClientReservePriorityTestStep* aTestStep)
	: iTestStep(aTestStep), iCameraReserve(EFalse), iIsSecondCameraReserved(EFalse)
	{
	}

CCameraSecondClient::~CCameraSecondClient()
	{
	if (iTestShutdown)
		{
		delete iTestShutdown;
		iTestShutdown = NULL;
		}

	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}	
	}

void CCameraSecondClient::HandleEvent(const TECAMEvent& aEvent)
	{
	if (aEvent.iEventType == KUidECamEventReserveComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			iTestStep->INFO_PRINTF1(_L("***** Success Second Camera Reserve *****"));
			iCameraReserve = ETrue;
			iCamera->Release();
			iCameraReserve = EFalse;
			iTestStep->INFO_PRINTF1(_L("***** Second Camera Release *****"));
			
			if(iTestStep->iIsFirstCameraOverthrown)
				{
				iTestStep->INFO_PRINTF1(_L("Shutdown Test2"));
				TRAPD(error, iTestShutdown = CAsyncTestShutdown::NewL(iTestStep));
				if (error != KErrNone)
					{
					iTestStep->SetTestStepError(error);
					iTestStep->SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				iTestShutdown->Start(KTSU_ECM_OMX_Interval, KErrNone, EPass);
				}
			}
		else
			{
			iTestStep->INFO_PRINTF2(_L("***** Failed to Reserve Second Camera: err = %d *****"), aEvent.iErrorCode);
			iTestStep->INFO_PRINTF1(_L("Shutdown Test2"));
			TRAPD(error, iTestShutdown = CAsyncTestShutdown::NewL(iTestStep));
			if (error != KErrNone)
				{
				iTestStep->SetTestStepError(error);
				iTestStep->SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			iTestShutdown->Start(KTSU_ECM_OMX_Interval, aEvent.iErrorCode, EFail);
			}
		}	
	}

void CCameraSecondClient::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraSecondClient::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraSecondClient::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

CCameraMultiClientReservePriorityTestStep::CCameraMultiClientReservePriorityTestStep()
	: iCameraReserve(EFalse), iIsFirstCameraOverthrown(EFalse)
	{
	SetTestStepName(KCameraMultiClientReservePriorityTestStep);
	}

CCameraMultiClientReservePriorityTestStep::~CCameraMultiClientReservePriorityTestStep()
	{
	if (iSecondClient)
		{
		delete iSecondClient;
		iSecondClient = NULL;
		}

	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}
	}

TVerdict CCameraMultiClientReservePriorityTestStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("***** Begin Camera Multi-Client Reserve Priority Test *****"));
	CreateDependencyFileL();

	if (!GetIntFromConfig(ConfigSection(), KCameraFirstPriorityString, iFirstPriority))
		{
		INFO_PRINTF1(_L("***** Fail to read data from ini file *****"));
		SetTestStepError(KErrGeneral);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	if (!GetIntFromConfig(ConfigSection(), KCameraSecondPriorityString, iSecondPriority))
		{
		INFO_PRINTF1(_L("***** Fail to read data from ini file *****"));
		SetTestStepError(KErrGeneral);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPreambleL();
	}





TVerdict CCameraMultiClientReservePriorityTestStep::doTestStepL()
	{
	if( !iCameraReserve )
		{
		TRAPD(error, iCamera = CCamera::New2L(*this, 0, iFirstPriority));
		if (error != KErrNone)
			{
			INFO_PRINTF2(_L("***** Failed to call CCamera::New2L(): err = %d *****"), error);
			REComSession::FinalClose();
			User::Leave(error);
			}

		// issue async call
		INFO_PRINTF1(_L("***** Camera Reserve *****"));
		iCamera->Reserve();
		}	

	CActiveScheduler::Start();
	return TestStepResult();
	}

TVerdict CCameraMultiClientReservePriorityTestStep::doTestStepPostambleL()
	{
	RemoveDependencyFile();

	INFO_PRINTF1(_L("***** Finish Camera Multi-Client Reserve Priority Test *****"));
	REComSession::FinalClose();
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPostambleL();
	}

void CCameraMultiClientReservePriorityTestStep::PostKickOffTestL(TInt aReason, TVerdict aResult)
	{
	if (iCameraReserve)
		{
		iCamera->Release();
		iCameraReserve = EFalse;
		INFO_PRINTF1(_L("***** Camera Release *****"));
		}

	SetTestStepError(aReason);
	SetTestStepResult(aResult);
	CActiveScheduler::Stop();
	}

void CCameraMultiClientReservePriorityTestStep::HandleEvent(const TECAMEvent& aEvent)
	{
	if (aEvent.iEventType == KUidECamEventReserveComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Reserve *****"));
			iCameraReserve = ETrue;

			TRAPD(error, iSecondClient = new(ELeave) CCameraSecondClient(this));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			TRAP(error, iSecondClient->iCamera = CCamera::New2L(*iSecondClient, 0, iSecondPriority));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to call Second CCamera::New2L(): err = %d *****"), error);
				REComSession::FinalClose();
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}

			INFO_PRINTF1(_L("***** Second Camera Reserve *****"));
			iSecondClient->iCamera->Reserve();
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Reserve Camera: err = %d *****"), aEvent.iErrorCode);
			SetTestStepError(aEvent.iErrorCode);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		}
	else if (aEvent.iEventType == KUidECamEventCameraNoLongerReserved)
		{
		INFO_PRINTF1(_L("******************************"));
		INFO_PRINTF1(_L("***** No Longer Reserved *****"));
		INFO_PRINTF1(_L("******************************"));
		iIsFirstCameraOverthrown = ETrue;
		iCameraReserve = EFalse;
		
		if(iSecondClient->iIsSecondCameraReserved)
			{
			//call test shutdown
			INFO_PRINTF1(_L("Shutdown Test1"));
			TRAPD(error, iTestShutdown = CAsyncTestShutdown::NewL(this));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			iTestShutdown->Start(KTSU_ECM_OMX_Interval, KErrNone, EPass);
			}
		}
	}

void CCameraMultiClientReservePriorityTestStep::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraMultiClientReservePriorityTestStep::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraMultiClientReservePriorityTestStep::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}

//

CCameraConfigurationTestStep::CCameraConfigurationTestStep()
	: 	iCameraReserve(EFalse), iCameraPowerOn(EFalse)
	{
	SetTestStepName(KCameraConfigurationTestStep);
	}

CCameraConfigurationTestStep::~CCameraConfigurationTestStep()
	{	
	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}
	}

TVerdict CCameraConfigurationTestStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("***** Begin Camera Configuration Test *****"));
	CreateDependencyFileL();

	if (!GetIntFromConfig(ConfigSection(), KCameraZoomFactorString, iZoomFactor))
		{
		INFO_PRINTF1(_L("***** Failed to read data from ini file *****"));
		SetTestStepError(KErrGeneral);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	if (!GetIntFromConfig(ConfigSection(), KCameraDigitalZoomFactorString, iDigitalZoomFactor))
		{
		INFO_PRINTF1(_L("***** Failed to read data from ini file *****"));
		SetTestStepError(KErrGeneral);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	if (!GetIntFromConfig(ConfigSection(), KCameraContrastString, iContrast))
		{
		INFO_PRINTF1(_L("***** Failed to read data from ini file *****"));
		SetTestStepError(KErrGeneral);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	if (!GetIntFromConfig(ConfigSection(), KCameraBrightnessString, iBrightness))
		{
		INFO_PRINTF1(_L("***** Failed to read data from ini file *****"));
		SetTestStepError(KErrGeneral);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	TInt temp = 0;
	
	if (!GetIntFromConfig(ConfigSection(), KCameraFlashString, temp))
		{
		INFO_PRINTF1(_L("***** Failed to read data from ini file *****"));
		SetTestStepError(KErrGeneral);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	iFlash = static_cast<CCamera::TFlash>(temp);
	
	if (!GetIntFromConfig(ConfigSection(), KCameraExposureString, temp))
		{
		INFO_PRINTF1(_L("***** Failed to read data from ini file *****"));
		SetTestStepError(KErrGeneral);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	iExposure = static_cast<CCamera::TExposure>(temp);
	
	if (!GetIntFromConfig(ConfigSection(), KCameraWhiteBalanceString, temp))
		{
		INFO_PRINTF1(_L("***** Failed to read data from ini file *****"));
		SetTestStepError(KErrGeneral);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	iWhiteBalance = static_cast<CCamera::TWhiteBalance>(temp);
	
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPreambleL();
	}

TVerdict CCameraConfigurationTestStep::doTestStepL()
	{
	TRAPD(error, iCamera = CCamera::New2L(*this, 0, 0));
	if (error != KErrNone)
		{
		INFO_PRINTF2(_L("***** Failed to call CCamera::New2L(): err = %d *****"), error);
		REComSession::FinalClose();
		User::Leave(error);
		}

	INFO_PRINTF1(_L("***** Reserve Camera *****"));
	iCamera->Reserve();
	
	CActiveScheduler::Start();
	return TestStepResult();
	}

TVerdict CCameraConfigurationTestStep::doTestStepPostambleL()
	{
	RemoveDependencyFile();

	INFO_PRINTF1(_L("***** Finish Camera Configuration Test *****"));
	REComSession::FinalClose();
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPostambleL();
	}

void CCameraConfigurationTestStep::PostKickOffTestL(TInt aReason, TVerdict aResult)
	{
	if (iCameraPowerOn)
		{
		iCamera->PowerOff();
		iCameraPowerOn = EFalse;
		INFO_PRINTF1(_L("***** Camera Power Off *****"));
		}
	if (iCameraReserve)
		{
		iCamera->Release();
		iCameraReserve = EFalse;
		INFO_PRINTF1(_L("***** Camera Release *****"));
		}

	SetTestStepError(aReason);
	SetTestStepResult(aResult);
	CActiveScheduler::Stop();
	}

void CCameraConfigurationTestStep::HandleEvent(const TECAMEvent& aEvent)
	{
	if (aEvent.iEventType == KUidECamEventReserveComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Reserve *****"));
			iCameraReserve = ETrue;
			INFO_PRINTF1(_L("***** Camera Power On *****"));
			iCamera->PowerOn();
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Reserve Camera: err = %d *****"), aEvent.iErrorCode);
			SetTestStepError(aEvent.iErrorCode);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		}
	else if (aEvent.iEventType == KUidECamEventPowerOnComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Power On *****"));
			iCameraPowerOn = ETrue;
			
			// ZoomFactor
			TRAPD(error, iCamera->SetZoomFactorL(iZoomFactor));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to call SetZoomFactorL: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			TInt value = iCamera->ZoomFactor();
			INFO_PRINTF2(_L("ZoomFactor = %d"), value);
			if (value != iZoomFactor)
				{
				INFO_PRINTF1(_L("***** Set / Get values are not same *****"));
				TRAPD(error2, PostKickOffTestL(KErrGeneral, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			
			// DigitalZoomFactor
			TRAP(error, iCamera->SetDigitalZoomFactorL(iDigitalZoomFactor));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to call SetDigitalZoomFactorL: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			value = iCamera->DigitalZoomFactor();
			INFO_PRINTF2(_L("DigitalZoomFactor = %d"), value);
			if (value != iDigitalZoomFactor)
				{
				INFO_PRINTF1(_L("***** Set / Get values are not same *****"));
				TRAPD(error2, PostKickOffTestL(KErrGeneral, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			
			// Contrast
			TRAP(error, iCamera->SetContrastL(iContrast));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to call SetContrastL: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			value = iCamera->Contrast();
			INFO_PRINTF2(_L("Contrast = %d"), value);
			if (value != iContrast)
				{
				INFO_PRINTF1(_L("***** Set / Get values are not same *****"));
				TRAPD(error2, PostKickOffTestL(KErrGeneral, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			
			// Brightness
			TRAP(error, iCamera->SetBrightnessL(iBrightness));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to call SetBrightnessL: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			value = iCamera->Brightness();
			INFO_PRINTF2(_L("Brightness = %d"), value);
			if (value != iBrightness)
				{
				INFO_PRINTF1(_L("***** Set / Get values are not same *****"));
				TRAPD(error2, PostKickOffTestL(KErrGeneral, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			
			// Flash
			TRAP(error, iCamera->SetFlashL(iFlash));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to call SetFlashL: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			CCamera::TFlash value2 = iCamera->Flash();
			INFO_PRINTF2(_L("Flash = %d"), value2);
			if (value2 != iFlash)
				{
				INFO_PRINTF1(_L("***** Set / Get values are not same *****"));
				TRAPD(error2, PostKickOffTestL(KErrGeneral, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			
			// Exposure
			TRAP(error, iCamera->SetExposureL(iExposure));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to call SetExposureL: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			CCamera::TExposure value3 = iCamera->Exposure();
			INFO_PRINTF2(_L("Exposure = %d"), value3);
			if (value3 != iExposure)
				{
				INFO_PRINTF1(_L("***** Set / Get values are not same *****"));
				TRAPD(error2, PostKickOffTestL(KErrGeneral, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			
			// WhiteBalance
			TRAP(error, iCamera->SetWhiteBalanceL(iWhiteBalance));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to call SetWhiteBalanceL: err = %d *****"), error);
				TRAPD(error2, PostKickOffTestL(error, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			CCamera::TWhiteBalance value4 = iCamera->WhiteBalance();
			INFO_PRINTF2(_L("WhiteBalance = %d"), value4);
			if (value4 != iWhiteBalance)
				{
				INFO_PRINTF1(_L("***** Set / Get values are not same *****"));
				TRAPD(error2, PostKickOffTestL(KErrGeneral, EFail));
				if (error2 != KErrNone)
					{
					SetTestStepError(error2);
					SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				return;
				}
			
			TRAP(error, PostKickOffTestL(KErrNone, EPass));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Power On Camera: err = %d *****"), aEvent.iErrorCode);
			TRAPD(error, PostKickOffTestL(aEvent.iErrorCode, EFail));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			}
		}
	}

void CCameraConfigurationTestStep::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraConfigurationTestStep::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraConfigurationTestStep::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

//

CCameraCreationStressTestStep::CCameraCreationStressTestStep()
	{
	SetTestStepName(KCameraCreationStressTestStep);
	}

CCameraCreationStressTestStep::~CCameraCreationStressTestStep()
	{
	iCameraArray.Reset();
	iCameraArray.Close();
	}

TVerdict CCameraCreationStressTestStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("***** Begin Camera Creation Stress Test *****"));
	CreateDependencyFileL();

	if (!GetIntFromConfig(ConfigSection(), KCameraNumberString, iNumberOfCamera))
		{
		INFO_PRINTF1(_L("***** Fail to read data from ini file *****"));
		SetTestStepError(KErrGeneral);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPreambleL();
	}

TVerdict CCameraCreationStressTestStep::doTestStepL()
	{
	TInt cntCamera = CCamera::CamerasAvailable();
	INFO_PRINTF2(_L("The Number Of Camera = %d"), cntCamera);
	
	CCamera* camera = NULL;

	for (TInt index = 0; index < iNumberOfCamera; ++index)
		{
		INFO_PRINTF2(_L("***** Stress Camera Iteration %d *****"), index);
		TRAPD(error, camera = CCamera::New2L(*this, 0, index));
		if (error != KErrNone)
			{
			INFO_PRINTF2(_L("***** Failed to call CCamera::New2L(): err = %d *****"), error);
			REComSession::FinalClose();
			User::Leave(error);
			}
		else
			{
			INFO_PRINTF2(_L("Camera Version = %d"), camera->CameraVersion());	
			iCameraArray.AppendL(*camera);
			}
		}

	return TestStepResult();
	}

TVerdict CCameraCreationStressTestStep::doTestStepPostambleL()
	{
	RemoveDependencyFile();

	INFO_PRINTF1(_L("***** Finish Camera Creation Stress Test *****"));
	REComSession::FinalClose();
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPostambleL();
	}

void CCameraCreationStressTestStep::HandleEvent(const TECAMEvent& /*aEvent*/)
	{
	}

void CCameraCreationStressTestStep::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraCreationStressTestStep::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}

void CCameraCreationStressTestStep::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}

//

CCameraSecondClient2::CCameraSecondClient2(CCameraMultiClientPowerOnPriorityTestStep* aTestStep)
	: iTestStep(aTestStep), iCameraReserve(EFalse), iIsSecondCameraReserved(EFalse)
	{
	}

CCameraSecondClient2::~CCameraSecondClient2()
	{
	if (iTestShutdown)
		{
		delete iTestShutdown;
		iTestShutdown = NULL;
		}

	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}	
	}

void CCameraSecondClient2::HandleEvent(const TECAMEvent& aEvent)
	{
	if (aEvent.iEventType == KUidECamEventReserveComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			iTestStep->INFO_PRINTF1(_L("***** Success Second Camera Reserve *****"));
			iCameraReserve = ETrue;
			iCamera->Release();
			iCameraReserve = EFalse;
			iTestStep->INFO_PRINTF1(_L("***** Second Camera Release *****"));
			
			if(iTestStep->iIsFirstCameraOverthrown)
				{
				iTestStep->INFO_PRINTF1(_L("Shutdown Test2"));
				TRAPD(error, iTestShutdown = CAsyncTestShutdown::NewL(iTestStep));
				if (error != KErrNone)
					{
					iTestStep->SetTestStepError(error);
					iTestStep->SetTestStepResult(EFail);
					CActiveScheduler::Stop();
					}
				iTestShutdown->Start(KTSU_ECM_OMX_Interval, KErrNone, EPass);
				}
			}
		else
			{
			iTestStep->INFO_PRINTF2(_L("***** Failed to Reserve Second Camera: err = %d *****"), aEvent.iErrorCode);
			iTestStep->INFO_PRINTF1(_L("Shutdown Test2"));
			TRAPD(error, iTestShutdown = CAsyncTestShutdown::NewL(iTestStep));
			if (error != KErrNone)
				{
				iTestStep->SetTestStepError(error);
				iTestStep->SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			iTestShutdown->Start(KTSU_ECM_OMX_Interval, aEvent.iErrorCode, EFail);
			}
		}	
	}

void CCameraSecondClient2::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraSecondClient2::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraSecondClient2::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

CCameraMultiClientPowerOnPriorityTestStep::CCameraMultiClientPowerOnPriorityTestStep()
	: iCameraReserve(EFalse), iCameraPowerOn(EFalse), iIsFirstCameraOverthrown(EFalse)
	{
	SetTestStepName(KCameraMultiClientPowerOnPriorityTestStep);
	}

CCameraMultiClientPowerOnPriorityTestStep::~CCameraMultiClientPowerOnPriorityTestStep()
	{
	if (iSecondClient)
		{
		delete iSecondClient;
		iSecondClient = NULL;
		}

	if (iCamera)
		{
		delete iCamera;
		iCamera = NULL;
		}
	}

TVerdict CCameraMultiClientPowerOnPriorityTestStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("***** Begin Camera Multi-Client PowerOn Priority Test *****"));
	CreateDependencyFileL();

	if (!GetIntFromConfig(ConfigSection(), KCameraFirstPriorityString, iFirstPriority))
		{
		INFO_PRINTF1(_L("***** Fail to read data from ini file *****"));
		SetTestStepError(KErrGeneral);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	if (!GetIntFromConfig(ConfigSection(), KCameraSecondPriorityString, iSecondPriority))
		{
		INFO_PRINTF1(_L("***** Fail to read data from ini file *****"));
		SetTestStepError(KErrGeneral);
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPreambleL();
	}

TVerdict CCameraMultiClientPowerOnPriorityTestStep::doTestStepL()
	{
	if( !iCameraReserve )
		{
		TRAPD(error, iCamera = CCamera::New2L(*this, 0, iFirstPriority));
		if (error != KErrNone)
			{
			INFO_PRINTF2(_L("***** Failed to call CCamera::New2L(): err = %d *****"), error);
			REComSession::FinalClose();
			User::Leave(error);
			}

		// issue async call
		INFO_PRINTF1(_L("***** Camera Reserve *****"));
		iCamera->Reserve();
		}	

	CActiveScheduler::Start();
	return TestStepResult();
	}

TVerdict CCameraMultiClientPowerOnPriorityTestStep::doTestStepPostambleL()
	{
	RemoveDependencyFile();

	INFO_PRINTF1(_L("***** Finish Camera Multi-Client PowerOn Priority Test *****"));
	REComSession::FinalClose();
	return CTe_ut_ecam_serverSuiteStepBase::doTestStepPostambleL();
	}

void CCameraMultiClientPowerOnPriorityTestStep::PostKickOffTestL(TInt aReason, TVerdict aResult)
	{
	if (iCameraPowerOn)
		{
		iCamera->PowerOff();
		iCameraPowerOn = EFalse;
		INFO_PRINTF1(_L("***** Camera Power Off *****"));
		}

	if (iCameraReserve)
		{
		iCamera->Release();
		iCameraReserve = EFalse;
		INFO_PRINTF1(_L("***** Camera Release *****"));
		}

	SetTestStepError(aReason);
	SetTestStepResult(aResult);
	CActiveScheduler::Stop();
	}

void CCameraMultiClientPowerOnPriorityTestStep::HandleEvent(const TECAMEvent& aEvent)
	{
	if (aEvent.iEventType == KUidECamEventReserveComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Reserve *****"));
			iCameraReserve = ETrue;

			INFO_PRINTF1(_L("***** Camera Power On *****"));
			iCamera->PowerOn();
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Reserve Camera: err = %d *****"), aEvent.iErrorCode);
			SetTestStepError(aEvent.iErrorCode);
			SetTestStepResult(EFail);
			CActiveScheduler::Stop();
			}
		}
	else if (aEvent.iEventType == KUidECamEventPowerOnComplete)
		{
		if (aEvent.iErrorCode == KErrNone)
			{
			INFO_PRINTF1(_L("***** Success Camera Power On *****"));
			iCameraPowerOn = ETrue;
			
			TRAPD(error, iSecondClient = new(ELeave) CCameraSecondClient2(this));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			TRAP(error, iSecondClient->iCamera = CCamera::New2L(*iSecondClient, 0, iSecondPriority));
			if (error != KErrNone)
				{
				INFO_PRINTF2(_L("***** Failed to call Second CCamera::New2L(): err = %d *****"), error);
				REComSession::FinalClose();
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}

			INFO_PRINTF1(_L("***** Second Camera Reserve *****"));
			iSecondClient->iCamera->Reserve();
			}
		else
			{
			INFO_PRINTF2(_L("***** Failed to Power On Camera: err = %d *****"), aEvent.iErrorCode);
			TRAPD(error, PostKickOffTestL(aEvent.iErrorCode, EFail));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			}
		}
	else if (aEvent.iEventType == KUidECamEventCameraNoLongerReserved)
		{
		INFO_PRINTF1(_L("******************************"));
		INFO_PRINTF1(_L("***** No Longer Reserved *****"));
		INFO_PRINTF1(_L("******************************"));
		iIsFirstCameraOverthrown = ETrue;
		iCameraReserve = EFalse;
		
		if(iSecondClient->iIsSecondCameraReserved)
			{
			//call test shutdown
			INFO_PRINTF1(_L("Shutdown Test1"));
			TRAPD(error, iTestShutdown = CAsyncTestShutdown::NewL(this));
			if (error != KErrNone)
				{
				SetTestStepError(error);
				SetTestStepResult(EFail);
				CActiveScheduler::Stop();
				}
			iTestShutdown->Start(KTSU_ECM_OMX_Interval, KErrNone, EPass);
			}
		}
	}

void CCameraMultiClientPowerOnPriorityTestStep::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraMultiClientPowerOnPriorityTestStep::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	}

void CCameraMultiClientPowerOnPriorityTestStep::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{	
	}
