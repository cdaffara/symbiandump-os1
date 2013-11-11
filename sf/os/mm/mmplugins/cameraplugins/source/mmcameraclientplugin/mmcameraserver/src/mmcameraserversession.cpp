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
// mmcamerasession.cpp
// 
//

/**
 @file
 @internalComponent
*/

#include "mmcameraserversession.h"
#include "mmcameraservercontroller.h"


CMMCameraServerSession::CMMCameraServerSession()
	:iCameraIndex(0),
	 iHandle(KECamHandleNotKnown),
	 iPriority(KECamNoPriority),
	 iMMCapability(ETrue),
	 iCollaborativeClient(EFalse),
	 iReserved(EFalse),
	 iPoweredUp(EFalse)
	{
	}

void CMMCameraServerSession::CreateL()
	{
	iCameraServer = static_cast<CMMCameraServer*>(const_cast<CServer2*>(CSession2::Server()));
	// Get server policymanager
	iPolicyManager = Server()->PolicyManager();

	//load camera control etc in OpenCamera...
	Server()->AddSession();
	}

CMMCameraServerSession::~CMMCameraServerSession()
	{
//	CameraController()->Reset();
	Server()->DropSession();

	//deque the session queued in controller
	iCamSessionLink.Deque();  //safe to deque if it does not exists in the queue.
	iCamSessionLink.iNext = NULL;
	}

void CMMCameraServerSession::Disconnect(const RMessage2 &aMessage)
	{
	if(!iOverthrowMessage.IsNull())
		{
		iOverthrowMessage.Complete(KErrCancel);
		}
	CSession2::Disconnect(aMessage);
	}

void CMMCameraServerSession::ServiceL(const RMessage2& aMessage)
	{
	
	// Do policy manager checks
	iPolicyManager->ServiceHandlerL(aMessage);

	switch (aMessage.Function())
		{
		case ECamQueryCamerasAvailable:
			{
			CamerasAvailable(aMessage);
			break;
			}
		
		case ECamOpenCamera:
			{
			OpenCamera(aMessage);
			break;	
			}

		case ECamCameraAccessControl:
			{
			switch(aMessage.Int0())
				{
				case ECameraRelease:     
					{
					Release(aMessage);
					break;
					}

				case ECameraReservedNotification:    
					{
					Reserve(aMessage);
					break;	
					}

				case ECameraCancelReservedNotification:    
					{
					CancelReserve(aMessage);
					break;	
					}

				case ECameraOverthrowNotification:
					{
					if(iOverthrowMessage.IsNull())
						{
						iOverthrowMessage = aMessage;
						}
					else
						{
						aMessage.Complete(KErrServerBusy);
						}
					break;
					}

				case ECameraCancelOverthrowNotification:
					{
					if(!iOverthrowMessage.IsNull())
						{
						iOverthrowMessage.Complete(KErrCancel);
						}
					CameraController()->Reset(); //Stops VF, VC. Cancels ImageCapture if issued using this session.
					aMessage.Complete(KErrNone);
					break;
					}

				default:
					aMessage.Complete(KErrArgument);
				}
			break;
			}
		case ECamPowerCamera:
			{
			switch(aMessage.Int0())
				{
				case ECameraPowerOnNotification:     
					{
					PowerOn(aMessage);
					break;
					}
				case ECameraPowerOff:
					{
					PowerOff();	
					aMessage.Complete(KErrNone);
					break;
					}
				case ECameraCancelPowerOnNotification:
					{
					CancelPowerOn();
					aMessage.Complete(KErrNone);
					break;
					}
				}
			break;
			}

		case ECamPrepareDirectViewFinder:
			{
			PrepareDirectViewFinder(aMessage);
			break;
			}

		case ECamStartDirectViewFinder:
			{
			StartDirectViewFinder(aMessage);
			break;
			}

		case ECamStopDirectViewFinder:
			{
			StopDirectViewFinder(aMessage);
			break;
			}

		case ECamPauseDirectViewFinder:
			{
			PauseDirectViewFinder(aMessage);
			break;
			}

		case ECamResumeDirectViewFinder:
			{
			ResumeDirectViewFinder(aMessage);
			break;
			}

		case ECamDirectViewFinderState:
			{
			DirectViewFinderState(aMessage);
			break;
			}

		case ECamSetDirectViewFinderMirror:
			{
			SetDirectViewFinderMirror(aMessage);
			break;
			}
			
		case ECamGetDirectViewFinderMirror:
			{
			GetDirectViewFinderMirror(aMessage);
			break;
			}

		case ECamDirectViewFinderProperties:
			{
			DirectViewFinderProperties(aMessage);
			break;
			}
		case ECamSetParameter:
			{
			SetCameraParameter(aMessage);
			break;
			}

		case ECamGetParameter:
			{
			GetCameraParameter(aMessage);
			break;
			}

		case ECamCameraHandle:
			{
			CameraHandle(aMessage);
			break;
			}

		default:
			Server()->PanicClient(aMessage, EPanicNotSupported);
			break;
		}
	}

/**
 * Queries controller to find out how many cameras are available to clients.
 */
void CMMCameraServerSession::CamerasAvailable(const RMessage2& aMessage)
	{
	TCamerasAvailable info;

	CMMCameraServerControllerQuery* query = NULL;
	TRAPD(error, Server()->GetCameraControllerQueryL(query));
	if (error == KErrNone)
		{
		info.iCameraCount = query->GetCamerasAvailable();
		delete query;

		TCamerasAvailablePckg pckg(info);
		error = aMessage.Write(TInt(0), pckg);
		}

	aMessage.Complete(error);
	}

/**
 * Stores client information and retrieves camera controller for required camera index.
 * 
 * Called when client connects to the server for the first time.
 */
void CMMCameraServerSession::OpenCamera(const RMessage2& aMessage)
	{
	TOpenCamera parameters;
	TOpenCameraPckg parametersBuf(parameters);

	TInt error = aMessage.Read(TInt(0),parametersBuf);
	if (error != KErrNone)
		{
		aMessage.Complete(error);
		}

	parameters = parametersBuf();

	iCameraIndex = parameters.iCameraIndex;   // Store the required member variables 
    iPriority = parameters.iPriority;
    iMMCapability = parameters.iMMCapability;
    iHandle = parameters.iHandle; 
    iCollaborativeClient = parameters.iCollaborativeClient;

    TRAP(error, Server()->GetCameraControllerL(iCameraIndex, iCameraController));

    aMessage.Complete(error);
 	}

/**
 * Reserves the camera.
 * 
 * Delegates the responsibility to the policy manager
 */
void CMMCameraServerSession::Release(const RMessage2& aMessage)
	{
	if(!iReserved)
		{
		aMessage.Complete(KErrAlreadyExists);
		}
	else
		{		
		CameraController()->Reset(); //Stops VF, VC. Cancels ImageCapture if issued using this session.
		
		iPolicyManager->ReleaseClient(this);

		iReserved = EFalse;
		iPoweredUp = EFalse;

		if(!iOverthrowMessage.IsNull())
			{	
			iOverthrowMessage.Complete(KErrCancel);
			}

		aMessage.Complete(KErrNone);
		}
	}

void CMMCameraServerSession::CompleteOverthrow ()
	{
	if(!iOverthrowMessage.IsNull())
		{
		// Complete means sucess
		iOverthrowMessage.Complete(KErrNone);
		}

	iPoweredUp = EFalse;
	}

/**
 * Releases the camera.
 * 
 * Delegates the responsibility to the policy manager
 */
void CMMCameraServerSession::Reserve(const RMessage2& aMessage)
	{
	if (iReserved)
		{
		aMessage.Complete(KErrInUse);
		return;
		}

	iReserved = iPolicyManager->ReserveClient(this);

    if(!iReserved)       	
    	{
    	aMessage.Complete(KErrAccessDenied);
    	}
    else
    	{
   	    aMessage.Complete(KErrNone);
    	}
    }

/**
 * Cancels on ongoing Reserve() operation.
 */
void CMMCameraServerSession::CancelReserve(const RMessage2& aMessage)
	{
	Release(aMessage);
	}

/**
 * Powers on the device.
 * 
 * Simply returns with KErrNone or KErrNotReady since device is not powered on until VF is activated.
 */
void CMMCameraServerSession::PowerOn(const RMessage2& aMessage)
	{
	if(!iReserved)
		{
		aMessage.Complete(KErrNotReady);
		}
	else if (iPoweredUp)
		{
		aMessage.Complete(KErrInUse);
		}
	else
		{
		iPoweredUp = ETrue;
		aMessage.Complete(KErrNone);
		}
	}

/**
 * Powers off the device.
 * 
 * Simply stops any viewfinder activity.
 */
void CMMCameraServerSession::PowerOff()
	{
	CameraController()->Reset();

	iPoweredUp = EFalse;
	}

void CMMCameraServerSession::CancelPowerOn()
	{
	iPoweredUp = EFalse;
	}

/**
 * Prepares the view finder.
 * 
 * First step before viewfinder is started.
 */
void CMMCameraServerSession::PrepareDirectViewFinder(const RMessage2& aMessage)
	{
	TDirectViewFinderInfo viewFinderInfo;
	TDirectViewFinderInfoPckg viewFinderInfoPckg(viewFinderInfo);

	TInt error = aMessage.Read(TInt(0), viewFinderInfoPckg);
	if(error == KErrNone)
		{
		viewFinderInfo = viewFinderInfoPckg();
		TRAP(error, CameraController()->PrepareDirectViewFinderL(viewFinderInfo));
		if(error == KErrNone)
			{
			TDirectViewFinderInfoPckg pckg(viewFinderInfo);
			error = aMessage.Write(TInt(0), pckg);
			}
		}

	aMessage.Complete(error); 
	}

/**
 * Starts the viewfinder
 */
void CMMCameraServerSession::StartDirectViewFinder(const RMessage2& aMessage)
	{
	TInt error = CameraController()->StartDirectViewFinder();
	aMessage.Complete(error);
	}

/**
 * Stops the viewfinder.
 * 
 * Returns KErrNone even if stop is called when the viewfinder is not active, as client API must fail silently if error occurs.
 */
void CMMCameraServerSession::StopDirectViewFinder(const RMessage2& aMessage)
	{
	if (CameraController()->GetDirectViewFinderState() == CCamera::CCameraV2DirectViewFinder::EViewFinderActive)
		{
		CameraController()->StopDirectViewFinder();
		}

	aMessage.Complete(KErrNone);
	}

/**
 * Pauses the viewfinder.
 * 
 * Returns KErrNone even if pause is called when the viewfinder is not active, as client API must fail silently if error occurs.
 */
void CMMCameraServerSession::PauseDirectViewFinder(const RMessage2& aMessage)
	{
	if (CameraController()->GetDirectViewFinderState() == CCamera::CCameraV2DirectViewFinder::EViewFinderActive)
		{
		CameraController()->PauseDirectViewFinder();
		}

	aMessage.Complete(KErrNone);
	}

/**
 * Resumes the viewfinder if previously paused.
 * 
 * Returns KErrNone even if resume is called when the viewfinder is not paused, as client API must fail silently if error occurs.
 */
void CMMCameraServerSession::ResumeDirectViewFinder(const RMessage2& aMessage)
	{
	if (CameraController()->GetDirectViewFinderState() == CCamera::CCameraV2DirectViewFinder::EViewFinderPause)
		{
		CameraController()->ResumeDirectViewFinder();
		}

	aMessage.Complete(KErrNone);
	}

/**
 * Retrieves the state of the viewfinder
 */
void CMMCameraServerSession::DirectViewFinderState(const RMessage2& aMessage)
	{
	TDirectViewFinderState state;

	state.iState = CameraController()->GetDirectViewFinderState();

	TDirectViewFinderStatePckg pckg(state);
	TInt error = aMessage.Write(TInt(0), pckg);
	
	aMessage.Complete(error);
	}

/**
 * Sets viewfinder mirror mode on or off.
 */
void CMMCameraServerSession::SetDirectViewFinderMirror(const RMessage2& aMessage)
	{
	TViewFinderMirror viewFinderMirror;
	TViewFinderMirrorPckg pckg(viewFinderMirror);

	TInt error = aMessage.Read(TInt(0), pckg);
	if (error == KErrNone)
		{
		viewFinderMirror = pckg();
		error = CameraController()->SetDirectViewFinderMirror(viewFinderMirror.iMirror);
		}

	aMessage.Complete(error);
	}

/**
 * Retrieves whether viewfinder mirror mode is activated or not.
 */
void CMMCameraServerSession::GetDirectViewFinderMirror(const RMessage2& aMessage)
	{
	TViewFinderMirror viewFinderMirror;

	TInt error = CameraController()->GetDirectViewFinderMirror(viewFinderMirror.iMirror);
	if (error == KErrNone)
		{
		TViewFinderMirrorPckg pckg(viewFinderMirror);
		error = aMessage.Write(TInt(0), pckg);
		}

	aMessage.Complete(error);	
	}

/**
 * Retrieves the viewfinder properties
 */
void CMMCameraServerSession::DirectViewFinderProperties(const RMessage2& aMessage)
	{
	TDirectViewFinderInfo info;

	TInt error = CameraController()->DirectViewFinderProperties(info.iScreenNum, info.iScreenRect, info.iClipRect);
	if (error == KErrNone)
		{
		TDirectViewFinderInfoPckg pckg(info);
		error = aMessage.Write(TInt(0), pckg);
		}

	aMessage.Complete(error);
	}

/**
 * Sets various camera parameters depending on what the client wishes to set.
 */
void CMMCameraServerSession::SetCameraParameter(const RMessage2& aMessage)
	{
	TInt error = KErrNone;

	switch(aMessage.Int0())
		{
		case EZoomFactor:
			{
			TInt zoom = aMessage.Int1();
			error = CameraController()->SetZoom(zoom);
			aMessage.Complete(error);
			break;
			}

		case EDigitalZoomFactor:
			{
			TInt digitalZoom = aMessage.Int1();
			error = CameraController()->SetDigitalZoom(digitalZoom);
			aMessage.Complete(error);
			break;
			}

		case EContrast:
			{
			TInt contrast = aMessage.Int1();
			error = CameraController()->SetContrast(contrast);
			aMessage.Complete(error);
			break;
			}

		case EBrightness:
			{
			TInt brightness = aMessage.Int1();
			error = CameraController()->SetBrightness(brightness);
			aMessage.Complete(error);
			break;
			}

		case EFlash:
			{
			TCameraFlash flash;
			TCameraFlashPckg pckg(flash);

			error = aMessage.Read(TInt(1), pckg);
			if (error == KErrNone)
				{
				flash = pckg();
				error = CameraController()->SetFlash(flash.iFlash);
				}
			aMessage.Complete(error);
			break;
			}

		case EExposure:
			{
			TCameraExposure exposure;
			TCameraExposurePckg pckg(exposure);

			error = aMessage.Read(TInt(1), pckg);
			if (error == KErrNone)
				{
				exposure = pckg();
				error = CameraController()->SetExposure(exposure.iExposure);
				}
			aMessage.Complete(error);
			break;
			}

		case EWhiteBalance:
			{
			TCameraWhiteBalance whiteBalance;
			TCameraWhiteBalancePckg pckg(whiteBalance);

			error = aMessage.Read(TInt(1), pckg);
			if (error == KErrNone)
				{
				whiteBalance = pckg();
				error = CameraController()->SetWhiteBalance(whiteBalance.iWhiteBalance);
				}
			aMessage.Complete(error);
			break;
			}

		default:
			{
			Server()->PanicClient(aMessage, EPanicNotSupported);
			break;
			}
		}
	}

/**
 * Gets various camera parameters depending on what information the client wishes to retrieve.
 */
void CMMCameraServerSession::GetCameraParameter(const RMessage2& aMessage)
	{
	TInt error = KErrNone;

	switch(aMessage.Int0())
		{
		case EZoomFactor:
			{
			TCameraZoom zoom;
			error = CameraController()->GetZoom(zoom.iZoom);
			if (error == KErrNone)
				{
				TCameraZoomPckg pckg(zoom);
				error = aMessage.Write(TInt(1), pckg);
				}
			break;
			}

		case EDigitalZoomFactor:
			{
			TCameraDigitalZoom digitalZoom;
			error = CameraController()->GetDigitalZoom(digitalZoom.iDigitalZoom);
			if (error == KErrNone)
				{
				TCameraDigitalZoomPckg pckg(digitalZoom);
				error = aMessage.Write(TInt(1), pckg);
				}
			break;
			}

		case EContrast:
			{
			TCameraContrast contrast;
			error = CameraController()->GetContrast(contrast.iContrast);
			if (error == KErrNone)
				{
				TCameraContrastPckg pckg(contrast);
				error = aMessage.Write(TInt(1), pckg);
				}
			break;
			}

		case EBrightness:
			{
			TCameraBrightness brightness;
			error = CameraController()->GetBrightness(brightness.iBrightness);
			if (error == KErrNone)
				{
				TCameraBrightnessPckg pckg(brightness);
				error = aMessage.Write(TInt(1), pckg);
				}
			break;
			}

		case EFlash:
			{
			TCameraFlash flash;
			error = CameraController()->GetFlash(flash.iFlash);
			if (error == KErrNone)
				{
				TCameraFlashPckg pckg(flash);
				error = aMessage.Write(TInt(1), pckg);		
				}
			break;
			}

		case EExposure:
			{
			TCameraExposure exposure;
			error = CameraController()->GetExposure(exposure.iExposure);
			if (error == KErrNone)
				{
				TCameraExposurePckg pckg(exposure);
				error = aMessage.Write(TInt(1), pckg);
				}
			break;
			}

		case EWhiteBalance:
			{
			TCameraWhiteBalance whiteBalance;
			error = CameraController()->GetWhiteBalance(whiteBalance.iWhiteBalance);
			if (error == KErrNone)
				{
				TCameraWhiteBalancePckg pckg(whiteBalance);
				error = aMessage.Write(TInt(1), pckg);
				}
			break;
			}

		default:
			{
			Server()->PanicClient(aMessage, EPanicNotSupported);
			break;
			}
		}
	
	aMessage.Complete(error);
	}

/**
 * Retrives the camera handle
 */
void CMMCameraServerSession::CameraHandle(const RMessage2& aMessage)
	{
	TCameraHandle cameraHandle;

	cameraHandle.iHandle = CameraController()->CameraHandle();
	TCameraHandlePckg pckg(cameraHandle);
	
	aMessage.Write(TInt(0), pckg);
	aMessage.Complete(KErrNone);
	}

void CMMCameraServerSession::SetHandle (TInt aHandle)
	{
	iHandle = aHandle;
	}

void CMMCameraServerSession::SetPriority (TInt aPriority)
	{
	iPriority = aPriority;
	}

void CMMCameraServerSession::SetReserved (TBool aReserved)
	{
	iReserved = aReserved;
	}

CMMCameraServer* CMMCameraServerSession::Server()
	{
	return iCameraServer;
	}

CMMCameraServerController* CMMCameraServerSession::CameraController()
	{
	return iCameraController;
	}
