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
// Part of the MVS Application for TechView
//

#include <techview/eikfnlab.h>
#include <techview/eikcfdlg.h>
#include <techview/eikbtgpc.h>
#include <techview/eikdialg.h>
#include <techview/eikon.rsg>
#include <eikmobs.h>
#include <coecntrl.h>
#include <eikenv.h>
#include "MVSAppUI.h"
#include "MVSApp.hrh"
#include "MVSViewTicker.h"
#include "MVSConstants.h"
#include "MVSVolumeDialog.h"
#include "MVSBalanceDialog.h"
#include "MVSRepeatDialog.h"
#include "MVSSelectPluginsDialog.h"
#include "MVSSaveAsDialog.h"
#include "MVSEditMetaInfoDialog.h"
#include "MVSConfigAudioFormatDialog.h"
#include "MVSConfigVideoFormatDialog.h"
#include "MVSSystemInfoDialog.h"
#include "MVSVideoSettingDialog.h"
#include "MVSAudioSettingDialog.h"
#include "MVSSetPlayWindowDialog.h"
#include "MVSViewMetaInfoDialog.h"
#include "MVSCropDialog.h"
#include "MVSMultipleScreenDialog.h"
#include "MVSResourceNotifyDialog.h"

const TInt KMaxSupportedPlugins = 10;
const TInt KMaxSupportedExtension = 10;
const TInt KMaxSupportedCodecs = 10;
const TInt KLabelCaptionMaxLength = 16;
const TInt KTimeValueLength = 12;
 
void CMVSAppUi::ConstructL()
	{
   	CEikAppUi::ConstructL();
  	User::LeaveIfError(iLogs.Connect());
  	iLogs.MkDirAll(KLogsPath);
  	iButtons=iEikonEnv->AppUiFactory()->ToolBar();
  	//Meta entry initialization	                                         
  	iMetaTitle = CMMFMetaDataEntry::NewL(KTitle,Ka);
    iMetaAuthor = CMMFMetaDataEntry::NewL(KAuthor,Ka);
  	iMetaCopyright = CMMFMetaDataEntry::NewL(KCopyrt,Ka);
  	iMetaRevision = CMMFMetaDataEntry::NewL(KRevision,Ka);
  	iMetaCategory = CMMFMetaDataEntry::NewL(KCategory,Ka);
  	iMetaComments = CMMFMetaDataEntry::NewL(KComments,Ka);
  	iVideoPlayAgent = CMVSVideoPlayAgent::NewL(*this);
  	iAppView = CMVSAppView::NewL(ClientRect(), *iVideoPlayAgent);
  	iPluginArray = new(ELeave) CDesCArrayFlat(KMaxSupportedPlugins);
  	iExtArray = new(ELeave) CDesCArrayFlat(KMaxSupportedExtension);
  	iCodecArray = new(ELeave) CDesCArrayFlat(KMaxSupportedCodecs);
  	iAudioPlayAgent = CMVSAudioPlayAgent::NewL(*this);
  	iAudioRecordAgent = CMVSAudioRecordAgent::NewL(*this);
  	iVideoRecordAgent = CMVSVideoRecordAgent::NewL(*this);
  	iPluginInfo=CMVSControllerPluginInfo::NewL();
  	iAutoplay = EFalse;
  	iMode=EIdle;
  	User::LeaveIfError(iLogger.Connect());
  	iLogger.CreateLog(KLogsDir,KLogsFile,EFileLoggingModeAppend);
  	iSettingManager = CSettingsManager::NewL();
  	iRepeatDelay = 1000000;//initial value of 1 second in microseconds.
  	iPriority = 1;//default settings
    iPriorityPreference = EMdaPriorityPreferenceNone;//default settings
    iRotation = EVideoRotationNone;//default settings
    iNoRepeats = 0;
	iScreenNumber = 0;
  	if(!iSettingManager->FileExists())
  		{
  		iSettingManager->MakeSeedIndexL();
  		}
  	iSettingManager->ReadGeneralSettingsL(this);
 	iSelectedPluginIdx = -1;
  	iMediaType = 0; //set audio media initially
 	iPluginsCnt = iPluginInfo->GetPluginListL(iPluginArray,iUidArray );
  	iControllerUid = TUid::Null();
  	iViewTicker = CMVSViewTicker::NewL(this);

	// Add one video control per screen
	// except for screen 0 which already has a video control
	// via the app view
	const TInt numScreens = CCoeEnv::Static()->WsSession().NumberOfScreens();

	for(TInt i = 1; i < numScreens; ++i)
		{
		CMVSVideoControl* videoCtl = CMVSVideoControl::NewLC(*iVideoPlayAgent, i, *(CCoeEnv::Static()->RootWin(i)));
		CWsScreenDevice* screenDevice = new (ELeave) CWsScreenDevice(CCoeEnv::Static()->WsSession());
		CleanupStack::PushL(screenDevice);
		User::LeaveIfError(screenDevice->Construct(i));
		videoCtl->SetExtent(TPoint(0, 0), screenDevice->SizeInPixels());
		CleanupStack::PopAndDestroy(screenDevice);
		videoCtl->ActivateL();
		iVideoCtls.AppendL(videoCtl);
		CleanupStack::Pop(videoCtl);
		}

	iVideoCtl = iAppView->iMainWindowControl;
	iLogger.Write(_L("Application START"));
  	iSrcFName.Copy(KDefault);
  	iDestFilename = KDefault;
  	iRecordLimitSet = EFalse;
	iAlreadyRegistered = EFalse;
  	}

CMVSAppUi::CMVSAppUi()
:iVolumeOrGain(10),iNoRepeats(0),iMainWindowText(8), iMaxFileBuffer(0)
#ifdef SYMBIAN_BUILD_GCE
	,iAutoScaleType(EAutoScaleNone),
    iAutoScaleHorizPos(EHorizontalAlignCenter),
    iAutoScaleVertPos(EVerticalAlignCenter)
#endif
	{		
	}

CMVSAppUi::~CMVSAppUi()
	{
	delete iAudioPlayAgent;
	delete iAudioRecordAgent;
	delete iVideoPlayAgent;
	delete iVideoRecordAgent;
	iUidArray.Close();
	iAudioUidArray.Close();
	iVideoUidArray.Close();
	iAudioPlayUidArray.Close();
	iOverlayText.Close();
	delete iPluginInfo;
	delete iPluginArray;
	delete iExtArray;
	delete iCodecArray;
	delete iMetaTitle;
	delete iMetaAuthor;   
	delete iMetaCopyright;
	delete iMetaRevision;
	delete iMetaCategory;
	delete iMetaComments;
	iMainWindowText.Reset();
	if(iLogger.Handle())
		{
		iLogger.CloseLog();
		iLogger.Close();	
		}
	iLogs.Close();
	delete iViewTicker;
	delete iSettingManager;
	iVideoCtls.ResetAndDestroy();
    delete iAppView;
	}

//All the user actvated actions in the UI is captured and handled 
//through here
void CMVSAppUi::HandleCommandL(TInt aCommand)
	{
	TBool needsUpdating = ETrue;
	switch (aCommand)
    	{
     	case EMVSCmdNew:
           	{
           	if( iMode != EIdle )
           		{
           		return;//dont get into record mode,as the system is still in a previous active state,Playing/Recording.
           		}
           	FileOpenForRecordL();
           	}
   	    	break;
        
        
        case EMVSCmdOpen:
        	{
        	if(iState == EAudioPlaying || iState == EVideoPlaying || iState == EAudioRecording || iState == EVideoRecording || iMode == EVideoRecord || iMode == EAudioRecord)
           		{
           		return;//dont get into record mode,as the system is still in a previous active state,Playing/Recording.
           		}
        	Reset();
        	FileOpenL();
    	 	iLogger.Write(_L("Opened for Play"));
       	  	}
       	  	break;	
        	
    	case EMVSCmdMultiScreen:
    		{
    	 	iSetScreen->SetupDialogLD(iScreenNumber,this);	
    		}
    		break;
    		
    	case EMVSRegisterForNotification:
    		{
    		iRegisterForNotification->SetupDialogLD(this);
    		}
    		break;
    		
        case EMVSCmdSetVolume:
        	{
        	if((iMode == EAudioPlay) || (iMode == EVideoPlay))
	        	{
	        	iVolumeDialog->SetupDialogLD(iVolumeOrGain,0,0, ETrue, this);	
	        	iLogger.Write(_L("volume set"));
	        	}
        	else if((iMode == EAudioRecord)||(iMode == EVideoRecord))
        		{
        		iVolumeDialog->SetupDialogLD(iVolumeOrGain,0,0, EFalse, this);
        		iLogger.Write(_L("gain set"));
        		}
           	}
           	break;
        	
        	
        case EMVSCmdSetPlayBalance:
         	{
         	iBalanceDialog->SetupDialogLD(iBalance,this, ETrue);    
         	iLogger.Write(_L("Balance Set"));    
           	}
           	break;
         	
        
        case EMVSCmdSetRecordBalance:
         	{
         	iBalanceDialog->SetupDialogLD(0,this, EFalse);    
        	}
        	break;
         	
         	     
        case EMVSCmdSetRepeats:
          	{
         	iRepeatDialog->SetupDialogLD(iNoRepeats,iRepeatDelay, this);
         	iLogger.Write(_L("Repeats Set"));	
         	}
        	break;
       
        case EMVSCmdEditMetadata:
            {
            iMetaArray.Reset();
            if(iMetaArray.Count() == 0)
            	{
              	InitializeMetaArrayL();
              	}
            switch(iMode)
            	{
            	case EAudioRecord:
            	case EVideoRecord:
            		{
            		iEditMetaInfoDialog->SetupDialogLD(iMetaArray,this);
         			iLogger.Write(_L("MetaData Edited"));
            		iMetaArray.Close();	
            		}
            	break;
            	
            	case EAudioPlay:
            	case EVideoPlay:
            		{
            		iViewMetaInfoDialog->SetupDialogLD(iMetaArray,this);
         			iLogger.Write(_L("MetaData View"));
            		iMetaArray.Close();	
            		}
            	}
            }
            break;
        
        
        case EMVSCmdExit:
            {
			if (iDisplayAdded)
              	{
				iVideoPlayAgent->RemoveDisplayWindowL(iVideoCtl->ControlWindow());
				iVideoPlayAgent->RemoveDisplayWindowL(iVideoCtl->Pip().ControlWindow());
				iVideoPlayAgent->RemoveDisplay(iVideoCtl->ScreenNumber());
				iVideoCtl->Pip().Clear();
				iDisplayAdded = EFalse;
              	}
            
            iLogger.Write(_L("Application EXIT"));
  		  	iSettingManager->WriteGeneralSettingsL(this);
        	Exit();
            }
            break;
    	
    	
    	case EMVSCmdPlay:
    		{
	    	if(!iButtons->IsCommandDimmed(EMVSCmdPlay))
	    		{
	    	 	switch(iMode)
					{
					case EAudioPlay:
						{
						iAppView->iStatusWindowTimeControl->ResetText2L();
						SetVolumeL(iVolumeOrGain,0);
						iButtons->DimCommand(EMVSCmdRecord, ETrue);
						iButtons->DrawNow();
						if(iState == ENotReady)
							{
							FileOpenL();	
							iAutoplay=ETrue;
							}
						else
							{
							iAudioPlayAgent->Play();	
							iLogger.Write(_L("Playing using AudioPlayAgent"));
							}	
						}
					break;

					case EAudioRecord:
						{
						iAppView->iStatusWindowTimeControl->ResetText2L();
						SetVolumeL(iVolumeOrGain,0);
						iAudioRecordAgent->PlayL();
						iLogger.Write(_L("Playing using AudioRecordAgent"));
						}
					break;

					case EVideoPlay:
						{						
						if (iVideoPlayAgent->AudioEnabledL())
							{
							SetVolumeL(iVolumeOrGain, 0);
							}
						iButtons->DimCommand(EMVSCmdSetRepeats, ETrue);
						iButtons->DrawNow();
						
#ifdef SYMBIAN_WSERV_AND_CONE_MULTIPLE_SCREENS
                        if (iVideoPlayAgent->SupportVideoPlayerUtility2())
                            {
							PrepareVideoControlsL();                        				
		                    }
#endif // SYMBIAN_WSERV_AND_CONE_MULTIPLE_SCREENS
					    iVideoPlayAgent->Play();
                        iLogger.Write(_L("Playing using VideoPlayAgent"));						    
						}
					    break;
					
					case EIdle:
					case EVideoRecord:
						break;
					
					default:
						ASSERT(EFalse);
					break;
					}
    			}
    		else
    			{
    			needsUpdating = EFalse;
    			}
    		}
    		break;
    	
    	
    	case EMVSCmdRecord:
    	 	{
    	 	if(!iButtons->IsCommandDimmed(EMVSCmdRecord))
    	 		{
    	 		switch(iMode)
	    			{
	    			case EAudioRecord:
		    			{
		    			iAudioRecordAgent->RecordL();
		    			iLogger.Write(_L("Recording using AudioRecordAgent"));	
		    			iButtons->DimCommand(EMVSCmdRecord, ETrue);
	                  	iButtons->DimCommand(EMVSCmdPlay, ETrue);
	                  	iButtons->DrawNow();
		    			}
	    				break;	
	    			case EVideoRecord:
		    			{
		    			//start view finder and record
		    			CCamera *camera = iVideoRecordAgent->Camera();
		    			TSize imageSize;
						camera->EnumerateCaptureSizes(imageSize,1,CCamera::EFormatFbsBitmapColor16M);
						TPoint pos = DrawBorders(imageSize);
						TRect clientRect = ClientRect();
						pos.iY += clientRect.iTl.iY;//add y-offset for menu bar
						TRect screenRect(pos,imageSize);
						TRect clipRect = screenRect ;
						if (!camera->ViewFinderActive())
							{
							CCoeEnv *coenv = iAppView->iMainWindowControl->ControlEnv();
							camera->StartViewFinderDirectL( coenv->WsSession(),
															*coenv->ScreenDevice(),
															*iAppView->DrawableWindow(),
															screenRect,
															clipRect);
							}
		    			if (!iFileReplace)
		    				{
		    				SetVideoFrameRateL(iVideoFrameRate);
		    				SetVideoFrameSizeL(iFrameSize);
		    				SetAudioEnabledL(iAudioEnabled);
		    				}
		    			iVideoRecordAgent->Record();
		    			iLogger.Write(_L("Recording using VideoRecordAgent"));	
		    			iButtons->DimCommand(EMVSCmdRecord, ETrue);
	                  	iButtons->DimCommand(EMVSCmdPlay, ETrue);
	                  	iButtons->DrawNow();
		    			}
		    			break;
	    			default:
	    				ASSERT(EFalse);
	    				break;	
	    			} 			
    	 	 	}
    	 	else
    	 		{
    	 		needsUpdating = EFalse;
    	 		}
    	 	}
    	 	break;	
        
        
        case EMVSCmdPause:
        	{
        	if(!iButtons->IsCommandDimmed(EMVSCmdPause))
	        	{
	        	switch(iMode)
	        		{
	        		case EAudioPlay:
	        			{
	        			iAudioPlayAgent->Pause();
	        			iLogger.Write(_L("AudioPlayAgent Paused"));	
	        			iButtons->DimCommand(EMVSCmdPlay, EFalse);
	        			iButtons->DrawNow();
	        			}
	        			break;
	        		
	        		case EAudioRecord:
	          	    	{
	          	    	User::InfoPrint(KNotsupported);		
	          	    	}
	        			break;
	        			
	        		case EVideoPlay:
	        		    {
	        		    iVideoPlayAgent->PauseL();
	        		    iLogger.Write(_L("AudioPlayAgent Paused"));	
	        		    iButtons->DimCommand(EMVSCmdPlay, EFalse);
	        			iButtons->DrawNow();
	        		    }
	        		    break;
	        		    
	        		case EVideoRecord:
	          	    	{
	          	    	iVideoRecordAgent->PauseL();
	          	    	iLogger.Write(_L("VideoRecordAgent Paused"));	
	          	    	iButtons->DimCommand(EMVSCmdRecord, EFalse);
	          	        iButtons->DrawNow();
	          	    	}
	        			break;
	        		
	        		case EIdle:
						break;
	        		
	        		default:
	        			ASSERT(EFalse);
	        			break;
	        		}	
	        	}
        	else
        		{
        		needsUpdating = EFalse;
        		}	
        	}
        	break;
    	
    	
    	case EMVSCmdStop:
    	    {
    	    if(!iButtons->IsCommandDimmed(EMVSCmdStop))
	    	    {
	    	    switch(iMode)
	        		{
	        		case EAudioPlay:
	        			{
	        			iAudioPlayAgent->Stop();
	        			iLogger.Write(_L("AudioPlayAgent Stopped"));	
	        			iButtons->DimCommand(EMVSCmdPlay, EFalse);
	        			iButtons->DrawNow();
	        			}
	        			break;
	        		
	        		case EAudioRecord:
	        			{
	        			iAudioRecordAgent->Stop();
	        		   	iLogger.Write(_L("AudioRecordAgent Stopped"));	
	        			iButtons->DimCommand(EMVSCmdRecord, EFalse);
	        			iButtons->DrawNow();
	        			iButtons->DimCommand(EMVSCmdPlay, EFalse);
	        			iButtons->DrawNow();
	        			}
	        			break;
	        		
	        		case EVideoPlay:
	        		    {
	        		    iVideoPlayAgent->Stop();	
	        		    iLogger.Write(_L("VideoPlayAgent Stopped"));	
	        		    iButtons->DimCommand(EMVSCmdPlay, EFalse);
	        			iButtons->DrawNow();
	        		    iAppView->DrawNow();
	        		    }
	        			break;
	        			
	        		case EVideoRecord:
	        			{
	        			StopViewFinder();
	        			iVideoRecordAgent->Stop();
	        			iLogger.Write(_L("VideoRecordAgent Stopped"));		
	        			iButtons->DimCommand(EMVSCmdRecord, EFalse);
	        			iButtons->DrawNow();
	        			}
	        			break;
	        		
	        		case EIdle:
	        			break;
	        		
	        		default:
	        			ASSERT(EFalse);
	        			break;
	        		}		
	    	    }
    	    else
    	    	{
    	    	needsUpdating = EFalse;
    	    	}
    	    }
         	break;
     
     
        case EMVSCmdSetPlayWindow:
       		{
       		iSetPlayWindow->SetupDialogLD(iStart,iEnd,iDuration,this);
       	    iLogger.Write(_L("AudioPlayAgent SetPlayWindow"));	
       		}
    		break;
    
    
    	case EMVSCmdClearPlayWindow:
    		{
    		iStart = 0;
    		iEnd = iAudioPlayAgent->Duration();
    		iAudioPlayAgent->ClearPlayWindow();
    	    iLogger.Write(_L("AudioPlayAgent ClearPlayWindow"));	
    		}
    		break;

    
       	case EMVSCmdSystemInfo:
    		{
    		iSystemInfo->SetupDialogLD(this);
    		iLogger.Write(_L("System Info"));
    		}
    		break;
    	
    	
    	case EMVSVideoCmdSettings:
    		{
            if (iVideoPlayAgent->SupportVideoPlayerUtility2())
                {
	    		iVideoSetting->SetupDialogLD(iRotation, this, iVideoExtent, iWindowClipRect, ETrue, iPip, iCrp);
                }
            else
                {
	    		iVideoSetting->SetupDialogLD(iRotation,this, TRect(0, 0, 0, 0), TRect(0, 0, 0, 0), EFalse, EFalse, iCrp);
                }
    		iLogger.Write(_L("Video Settings"));
    		}
    		break;
    	
    	
    	case EMVSCmdAudioSetting:
    		{
    		iAudioSetting->SetupDialogLD(iNoRepeats,iRepeatDelay,iPriority,iPriorityPreference,iMaxFileBuffer, this);
    		iLogger.Write(_L("Audio Setting"));	
    		}
    		break; 
    	
    	
    	case EMVSCmdClose:
    		{
    		FileClose(); 
    		iRecordLimitSet = EFalse;	
    		}
    		break;
    	
    	
    	case EMVSCmdSetCropWindow:
    	    {
    	    iCropPosition=0;
    	    iCropStart=GetCropStart();
        	iClipLength = iAudioRecordAgent->Duration();
    	    iCropEnd=iClipLength;
    	    iCropDialog->SetupDialogLD(iCropPosition,iClipLength,0,iCropEnd,this);
    	    iLogger.Write(_L("CropWindow set"));	
    	    }
    	    break;
    		
    	case EMVSCmdClearLog:
    		{
    		
       		}
    		break;
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
    	case EMVSCmdEnableSubtitles :
    		{
    		iVideoPlayAgent->EnableSubtitlesL();
    		}
    		break;
    	case EMVSCmdDisableSubtitles :
    		{
    		iVideoPlayAgent->DisableSubtitlesL();
    		}
    		break;
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
    	}
    if(needsUpdating)
    	{
    	UpdateDisplayL(iState);
    	}
  	}

void CMVSAppUi::PrepareVideoControlsL()
	{
	if (iDisplayAdded)
		{
		iVideoPlayAgent->RemoveDisplayWindowL(iVideoCtl->ControlWindow());
		iVideoPlayAgent->RemoveDisplay(iVideoCtl->ScreenNumber());
		TRAP_IGNORE(iVideoPlayAgent->RemoveDisplayWindowL(iVideoCtl->Pip().ControlWindow()));
		iDisplayAdded = EFalse;
		}

	iVideoCtl = iAppView->iMainWindowControl;

	CCoeEnv* env = CCoeEnv::Static();

	if (iScreenNumber > 0)
		{
		// Video ctls start at 0 in index for display 1
		iVideoCtl = iVideoCtls[iScreenNumber - 1];
		}

	TRect extent = iVideoExtentSet ? iVideoExtent : iVideoCtl->Rect();
	TRect clip = iWindowClipRectSet ? iWindowClipRect : iVideoCtl->Rect();
	iVideoPlayAgent->AddDisplayWindowL(env->WsSession(), *(env->ScreenDevice(iScreenNumber)), iVideoCtl->ControlWindow(), extent, clip);
	iDisplayAdded = ETrue;

	if (iPip)
		{
		// Picture-in-picture has been enabled.
		TSize videoFrameSize;
		iVideoPlayAgent->VideoFrameSizeL(videoFrameSize);
		TSize winSize = iVideoCtl->Size();
	
		// Picture-in-picture should be an 8th of original	
		videoFrameSize.iHeight /= 4;
		videoFrameSize.iWidth /= 4;
		
		TPoint pipLoc(winSize.iWidth - (videoFrameSize.iWidth + 4), 4);

		iVideoCtl->Pip().SetExtent(pipLoc, videoFrameSize);

		if (iCrp)
			{
			iVideoCtl->EnablePip(CMVSPipControl::EUseCrp);
			iVideoPlayAgent->AddDisplayL(env->WsSession(), iScreenNumber, iVideoCtl->Pip());
			}
		else
			{
			iVideoCtl->EnablePip(CMVSPipControl::EUseMmf);
			iVideoPlayAgent->AddDisplayWindowL(env->WsSession(), *(env->ScreenDevice(iScreenNumber)), iVideoCtl->Pip().ControlWindow());
			iVideoPlayAgent->SetAutoScaleL(iVideoCtl->Pip().ControlWindow(), EAutoScaleBestFit, EHorizontalAlignCenter, EVerticalAlignCenter);
			}
		}
	else
		{
		// Picture-in-picture has been disabled.
		iVideoCtl->DisablePip();
		}
	}

void CMVSAppUi::SetPosition(TTimeIntervalMicroSeconds& aPos)
	{
	if(iMode == EAudioPlay)
		{
		iAudioPlayAgent->SetPosition(aPos);
		}
	else if(iMode == EAudioRecord)
		{
		iAudioRecordAgent->SetPosition(aPos);
		}
	}
  	
//Opens a file for playing
void CMVSAppUi::FileOpenL()
	{
	//Open the file dialog
    CEikFileOpenDialog* dialog = new (ELeave) CEikFileOpenDialog(&iSrcFName);  
    TInt val = dialog->ExecuteLD(R_EIK_DIALOG_FILE_OPEN);
	if(val != 0)
	    {
	    TUid mediatype = iPluginInfo->GetMediaTypeL(iSrcFName);
	    if(mediatype == KUidMediaTypeVideo)
			{
		    TRect rect, clipRect;
		    rect = clipRect = TRect(iAppView->iMainWindowControl->PositionRelativeToScreen(), iAppView->iMainWindowControl->Size());

			rect.Shrink(2,2);
		    clipRect.Shrink(2,2);
		    
            // Clear the area where the video will be displayed 
#ifdef SYMBIAN_WSERV_AND_CONE_MULTIPLE_SCREENS
        	iVideoPlayAgent->SetVideoOutputL(iCoeEnv->WsSession(),
											 *iCoeEnv->ScreenDevice(iScreenNumber),
											 iVideoCtl->ControlWindow(),
											 rect, 
											 clipRect);
#else
        	iVideoPlayAgent->SetVideoOutputL(iCoeEnv->WsSession(),
											 *iCoeEnv->ScreenDevice(),
											 iVideoCtl->ControlWindow(),
											 rect, 
											 clipRect);
#endif // SYMBIAN_WSERV_AND_CONE_MULTIPLE_SCREENS

		    TInt res = iVideoPlayAgent->SetScreenNumber(iScreenNumber);
			if(res == KErrNotSupported)
				{
				User::InfoPrint(KMultiScreenNotsupported);
				}
			//Enable AP&R for register notification made before opening the file
			if(iRegistered)
				{
				TInt regerr = iVideoPlayAgent->RegisterForNotification();	
				if(regerr == KErrNone)
					{
					User::InfoPrint(KRegisterSuccess);
					}
				else
					{
					User::InfoPrint(KRegisterFailure);	
					}
				}
			else if(iAlreadyRegistered)
				{
				TInt cancelerr = iVideoPlayAgent->CancelNotification();
				if(cancelerr == KErrNone)
					{
					iAlreadyRegistered = EFalse;	
					User::InfoPrint(KRegisterCancelation);
					}
				else
					{
					User::InfoPrint(KCancelationFailure);	
					}
				}	
		    iVideoPlayAgent->OpenFileL(iSrcFName);
		    iMode = EVideoPlay;	
		    }
	    else if(mediatype == KUidMediaTypeAudio)
		    {
			iAudioPlayAgent->OpenFileL(iSrcFName);
			iMode = EAudioPlay;
	    	iButtons->DimCommand(EMVSCmdSetVolume,EFalse);
			iButtons->DrawNow();
	    	}
	    else
	    	{
			User::InfoPrint(KNotSupported);    	
	    	}   
	 	}
    return;
	}

//Opens a new or existing file to record.	
void CMVSAppUi::FileOpenForRecordL()
	{
   	iExtArray->Reset();
  	iPluginArray->Reset();
    iCodecArray->Reset();
   	iSelectedPluginIdx = KBeforeSelectingController;
    iSelectPluginsDialog->SetupDialogLD(iPluginArray,iExtArray, iCodecArray, this);
    //Indicates that the controller wasnt selected, leave it to the MMF to select
    //tha appropriate controller.
    if(iSelectedPluginIdx == KControllerNotSelected)
    	{
    	iControllerUid = TUid::Null();
    	}
    else
    	{
    	if(iMediaType)
    		{
    		iControllerUid = iVideoUidArray[iSelectedPluginIdx];
    		}
    	else
    		{
    		iControllerUid = iAudioUidArray[iSelectedPluginIdx];				
    		}
    	}
    if(!iMediaType)
    	{
        iMode = EAudioRecord;
        }
    else if(iMediaType)
    	{
		iMode = EVideoRecord;        		
        }	
    SaveAsDialogL();
    }
    
void CMVSAppUi::FileClose()    
	{
	iButtons->DimCommand(EMVSCmdSetVolume, ETrue);
    iButtons->DimCommand(EMVSCmdRecord, ETrue);
	switch(iMode)
   		{
	    case EAudioPlay:
	    	{
	        iAudioPlayAgent->Stop();
	        iLogger.Write(_L("AudioPlayAgent Close"));	
	        }
	        break;
	        		
	    case EAudioRecord:
	     	{
	     	iAudioRecordAgent->Stop();
	        iLogger.Write(_L("AudioRecordAgent Close"));	
	        }
	        break;
	        
	    case EVideoPlay:
	    	{
	        iVideoPlayAgent->Close();
	        iLogger.Write(_L("VideoPlayAgent Close"));		
	        iAppView->DrawNow();
	        }
	        break;
	        	    
	    case EVideoRecord:
	    	{
	        iVideoRecordAgent->Close();	
	        iLogger.Write(_L("VideoRecordAgent Close"));		
	        }
	        break;
	        		
	    default:
	        break;
	    }
	Reset();	
	}
	
	
void CMVSAppUi::SaveAsDialogL()
	{
	iFileReplace = ETrue;
	TInt errVal = iSaveAsDialog->SetupDialogLD(iDestFilename, &iFileReplace);
    if(errVal)
    	{
    	DetermineFinalFNameL();
    	if(iProceed)
    		{
        	iOpeningForRecord=ETrue;
        	RFs fs = CCoeEnv::Static()->FsSession();
        	if(iMode==EAudioRecord)
	    		{
	    		TInt uidcount = iAudioPlayUidArray.Count();
	    		TUid playcontrollerUid = TUid::Null();
	    		if(uidcount)
		    		{
		    		playcontrollerUid = iAudioPlayUidArray[0];
		    		}
 		    	if(iFileReplace)
		    		{
		    		fs.Delete(iDestFilename);
		    		}
		    	iAudioRecordAgent->CreateNewFileL(iDestFilename,iControllerUid,playcontrollerUid);	
		    	iLogger.Write(_L("Opened file for Audio Record"));
		    	iRecFName.Copy(iDestFilename);
	        	iButtons->DimCommand(EMVSCmdRecord, EFalse);
	        	iButtons->DrawNow();
		    	}
		    	    	
        	else if(iMode==EVideoRecord)
	    		{
	    		if(iFileReplace)
		    		{
		    		fs.Delete(iDestFilename);
		    		}
	    		TInt err = iVideoRecordAgent->OpenFileL(iDestFilename,iControllerUid);
	       		if(err != KErrNone)
	      			{
	       			iLogger.Write(_L("ECam Object creation failed"));
	       			User::InfoPrint(KVideoError);
	       			}
	    		else
	       			{
	       			iLogger.Write(_L("Opened file for Video Record"));
	       			iRecFName.Copy(iDestFilename);
        			iButtons->DimCommand(EMVSCmdRecord, EFalse);
        			iButtons->DrawNow();	
	       			}
	       		}
        	}
		}
	else
		{
		iMode = EIdle;
		UpdateStateChange(ENotReady , KErrNone);
		}
    }
	
//The state change calls from the agents
void CMVSAppUi::UpdateStateChange(TMVSState aState, TInt aError)
	{
	TInt error = aError;
	//if state changes from EVideoRecording and view finder is running, stop it.
	if(iState == EVideoRecording && aState != EVideoRecording)
		{
		StopViewFinder();
		}
	//if playback stops, redraw the app view
	if(iState == EVideoPlaying && aState != EVideoPlaying)
		{
		// the last frame is still on the display, but nothing is in charge
		// of redrawing it. this would otherwise lead to graphical glitches
		// as the frame is partially erased over time.
		iAppView->iMainWindowControl->DrawNow();
		}
	iState=aState;
	if(error == KErrNone)
		{
		if(aState == EAudioOpened)
			{
			iAppView->ResetProgressBar();
	   		iButtons->DimCommand(EMVSCmdPlay,EFalse);
	   		iButtons->DrawNow();
	   		if(iMode == EAudioPlay)
	   			{
	   			iButtons->DimCommand(EMVSCmdRecord,ETrue);	
 				iDuration = iAudioPlayAgent->Duration();
 				if((iStart == 0) && (iEnd == 0))
		   			{
		   			iStart = 0;
		   			iEnd = iDuration;	
		   			}
	   			}
         	if(iButtons->IsCommandDimmed(EMVSCmdSetRepeats))
         		{
         		iButtons->DimCommand(EMVSCmdSetRepeats,EFalse);	
         		iButtons->DrawNow();
         		}
         	if(iButtons->IsCommandDimmed(EMVSCmdSetPlayBalance))
         		{
         		iButtons->DimCommand(EMVSCmdSetPlayBalance,EFalse);	
         		iButtons->DrawNow();
         		}
	   		//Provided to automatically start playing, when the play button is selected 
			//without opening a file through the file menu.
	   		if(iAutoplay)
	   			{
	   			iAudioPlayAgent->Play();
	   			iAutoplay=EFalse;
	   			}
	   		if(iMode == EAudioRecord && !iOpeningForRecord)
	   			{
	   			iButtons->DimCommand(EMVSCmdRecord,EFalse);
	   			iButtons->DimCommand(EMVSCmdPlay,EFalse);	
	   			iButtons->DrawNow();
	   			}
	   		//Pop the Audio Record Settings Dialogue
	   		if(iMode == EAudioRecord && iOpeningForRecord && iSelectedPluginIdx !=-2)
	   			{
	   			iButtons->DimCommand(EMVSCmdSetPlayBalance,EFalse);
	   			if(iButtons->IsCommandDimmed(EMVSCmdRecord))
         			{
         			iButtons->DimCommand(EMVSCmdRecord,EFalse);
		 			iButtons->DrawNow();	
         			}
	   			//if appending to an existing file no need to re-enter the controller configuration.
	   			if(iFileReplace)
	   				{
	   				if(!iMediaType && iSelectedPluginIdx !=-1)
	         			{
				 		TRAP(error, iConfigFormatDialog->SetupDialogLD(this,iAudioUidArray[iSelectedPluginIdx],iSettingManager) );
				 		iOpeningForRecord = EFalse;
				 	  	}
	   				}
	   			}
         	}
		else if(aState == EAudioPlaying)
			{
			iButtons->DimCommand(EMVSCmdPlay, ETrue);
			iButtons->DimCommand(EMVSCmdRecord,ETrue);
			iButtons->DimCommand(EMVSCmdPause, EFalse);
			iButtons->DimCommand(EMVSCmdStop, EFalse);
			iButtons->DrawNow();
	   		}
		else if(aState == ENotReady)
			{
			iButtons->DimCommand(EMVSCmdSetPlayBalance,EFalse);
			iButtons->DimCommand(EMVSCmdSetVolume, ETrue);
            iButtons->DimCommand(EMVSCmdRecord, ETrue);
            iButtons->DimCommand(EMVSCmdPlay, EFalse);
            iButtons->DrawNow();

			// File has been closed.  Remove the display window if it was added.
			if (iDisplayAdded)
				{
				TRAP(error, iVideoPlayAgent->RemoveDisplayWindowL(iVideoCtl->ControlWindow()));
				TRAP(error, iVideoPlayAgent->RemoveDisplayWindowL(iVideoCtl->Pip().ControlWindow()));
				iVideoPlayAgent->RemoveDisplay(iVideoCtl->ScreenNumber());
				iVideoCtl->Pip().Clear();
				iDisplayAdded = EFalse;
				}
            }
		else if(aState == EAudioOpening)
			{
			iButtons->DimCommand(EMVSCmdSetVolume,EFalse);
	   		}
		else if(aState == EAudioPaused)
			{
			iButtons->DimCommand(EMVSCmdPause, ETrue);
	   		iButtons->DrawNow();
	   		}
		else if(aState == EAudioStopped)
			{
			iAppView->ResetProgressBar();
			iButtons->DimCommand(EMVSCmdStop, ETrue);
			iButtons->DimCommand(EMVSCmdPause, ETrue);
	   		iButtons->DrawNow();
			}
		else if(aState == EVideoPaused)
			{
			iButtons->DimCommand(EMVSCmdPause,ETrue);
			iButtons->DrawNow();
			}
		else if(aState == EVideoStopped)
			{
			iButtons->DimCommand(EMVSCmdStop,ETrue);
			iButtons->DimCommand(EMVSCmdPause, ETrue);
			iButtons->DrawNow();
			}
		else if(aState == EVideoPlaying)
			{
			iButtons->DimCommand(EMVSCmdPlay, ETrue);
			iButtons->DimCommand(EMVSCmdRecord,ETrue);
			iButtons->DimCommand(EMVSCmdPause,ETrue);
			iButtons->DimCommand(EMVSCmdStop,EFalse);
			iButtons->DrawNow();
			}
		else if(aState == EVideoRecording)
			{
			iButtons->DimCommand(EMVSCmdPlay, ETrue);
			iButtons->DimCommand(EMVSCmdRecord, ETrue);
			iButtons->DimCommand(EMVSCmdPause,EFalse);
			iButtons->DimCommand(EMVSCmdSetRepeats,ETrue);
			iButtons->DrawNow();
			}
		
		else if(aState == EAudioRecording)
			{
			iButtons->DimCommand(EMVSCmdPlay, ETrue);
			iButtons->DimCommand(EMVSCmdRecord,ETrue);
			iButtons->DimCommand(EMVSCmdPause,ETrue);
			iButtons->DrawNow();
			}
		else if(aState == EVideoOpened )
			{
			//Pop the Video Record Settings Dialogue
			iButtons->DimCommand(EMVSCmdPlay,EFalse);
         	iButtons->DimCommand(EMVSCmdPause,ETrue);
         	iButtons->DimCommand(EMVSCmdSetRepeats,ETrue);
         	iButtons->DrawNow();
			if(iButtons->IsCommandDimmed(EMVSCmdSetPlayBalance))
         		{
         		iButtons->DimCommand(EMVSCmdSetPlayBalance,EFalse);	
         		iButtons->DrawNow();
         		}
			if(iMode==EVideoRecord && iOpeningForRecord)
				{
				iButtons->DimCommand(EMVSCmdSetPlayBalance,ETrue);
				if(iFileReplace)
	   				{
				if(iMediaType && iSelectedPluginIdx !=-1)
	        			{
		     			TRAP(error, iVideoFormatDialog->SetupDialogLD(this,iVideoUidArray[iSelectedPluginIdx],iSettingManager) );
		     			iOpeningForRecord=EFalse;
	         			}
	   				}
				}
			}
		if(error != KErrNone)		
			{
			LogErrors(error);		
			}
		}
	else if ((error == KErrEof) && (aState == EAudioStopped))
		{
		iButtons->DimCommand(EMVSCmdPlay, EFalse);
		iButtons->DimCommand(EMVSCmdRecord,ETrue);
		iButtons->DimCommand(EMVSCmdPause,EFalse);
		iButtons->DimCommand(EMVSCmdStop,EFalse);
		iButtons->DrawNow();
		}
	else	
		{
		LogErrors(error);
		}
	
	//As the state changes the display windows(views) contents also needs to be changed
	TRAPD(err,UpdateDisplayL(iState));
	if(err)
		{
		LogErrors(err);	
		iState = ENotReady;
		FileClose();
		}
	}

void CMVSAppUi::MvsResourceNotification(const TDesC8& /*aNotificationData*/)
	{
	if(iMode == EVideoPlay)
		{
		iVideoPlayAgent->Play();
		}
	}
//Return the current mode of the App
void CMVSAppUi::Reset()
	{
	iMode =EIdle;
	iState =ENotReady;
	iBitRate = 0;
	iSampleRate = 0;
	iChannels =0;
	iAutoplay = EFalse;
	iStart = 0;
	iEnd = 0;
	iDuration = 0;
	}


//GetCropStart()
TTimeIntervalMicroSeconds CMVSAppUi::GetCropStart() const
    {
    return iCropStart;
    }


// Returns the cropping end-point in microseconds
TTimeIntervalMicroSeconds CMVSAppUi::GetCropEnd() const
    {
    return iCropEnd;
    }

//Volume setting
void CMVSAppUi::SetVolumeL(TInt aVolume, TTimeIntervalMicroSeconds aRamp)
    {
    //Initialise the volume settings based on the parameters
    iVolumeOrGain = aVolume;
    iRamp = aRamp;
	switch(iMode)
		{
	 	case EAudioPlay:
     		{
     		//And set the maximum volume based on the device's audio capabilities
     		iMaxVolume = iAudioPlayAgent->MaxVolume();
	    	//Set the volume, and volume ramps.
     		iAudioPlayAgent->SetVolume((iVolumeOrGain*iMaxVolume)/100, iRamp);	
     		}
     		break;
	 	
	 	case EAudioRecord:
	 		{
	 		//And set the maximum volume based on the device's audio capabilities
     		iMaxVolume = iAudioRecordAgent->MaxGain();
	    	//Set the volume, and volume ramps.
     		iAudioRecordAgent->SetGain((iVolumeOrGain*iMaxVolume)/100);	
	 		}
	 		break;
     	
	 	case EVideoPlay:
	 		{
	 		iMaxVolume = iVideoPlayAgent->MaxVolume();
	 		iVideoPlayAgent->SetVolumeL((iVolumeOrGain*iMaxVolume)/100);	
	 		}
	 		break;
	 	
	 	case EVideoRecord:
	 		{
	 		iMaxVolume = iVideoRecordAgent->MaxGainL();
			iVideoRecordAgent->SetGainL((iVolumeOrGain*iMaxVolume)/100); 		
	 		}
			break;
		
		case EIdle:
			break;
			
		default:
			ASSERT(EFalse);
			break;
	 	}
    }
 
 
//Set repeats
void CMVSAppUi::SetRepeats(TInt aNoRepeats, TTimeIntervalMicroSeconds aDelay)
    {
    //Initialise the repeat variables based on the parameters
    iNoRepeats = aNoRepeats;
    iRepeatDelay = aDelay;
	switch(iMode)
	    {
	    case EAudioPlay:
	 		//Set the repeat settings based on these values
     		iAudioPlayAgent->SetRepeats(iNoRepeats, iRepeatDelay);
			break;
		
	 	case EAudioRecord:
	 		//Set the repeat settings based on these values
     		iAudioRecordAgent->SetRepeats(iNoRepeats, iRepeatDelay);
	 		break;
	 	default:
	 		break;
		}
    }

//Balance settings
//
//
//
// Sets the balance, based on the argument. This value is translated to both
// the playback AND recording balance for the device.
//
void CMVSAppUi::SetBalanceL(TInt aBalance)
	{
    iBalance = aBalance;
    switch(iMode)
        {
     	case EAudioPlay:
     		iAudioPlayAgent->SetBalance(iBalance);
     		break;
     
     	case EAudioRecord:
     		iAudioRecordAgent->SetRecordBalance(iBalance);
     		break;
     		
        case EVideoPlay:
        	iVideoPlayAgent->SetBalanceL(iBalance);
        	break;
        
        case EVideoRecord:
          	User::InfoPrint(KNotsupported);
        	break;
        	
        case EIdle:
        	break;
        	
        default:
        	ASSERT(EFalse);
        	break;
     		
     	}
    }

     
//
//SetPriorityL(...)
//
// Sets the audio priority, and the priority preference, based on the
// arguments.
//
void CMVSAppUi::SetPriorityL(TInt aPriority, 
                                 TMdaPriorityPreference aPriorityPreference)
    {
    iPriority = aPriority;
    iPriorityPreference = aPriorityPreference;
    switch(iMode)
	    {
	    case EAudioPlay:
		    iAudioPlayAgent->SetPriority(iPriority,iPriorityPreference);	
		    break;
	    
	    case EVideoPlay:
		    iVideoPlayAgent->SetPriorityL(iPriority,iPriorityPreference);		
		    break;
	    
	    case EAudioRecord:
	    	iAudioRecordAgent->SetPriority(iPriority,iPriorityPreference);	
		    break;	
		
	    case EVideoRecord:
	    	iVideoRecordAgent->SetPriorityL(iPriority,iPriorityPreference);		
		    break;
	    
	    case EIdle:
	    	break;
		
		default:
			ASSERT(EFalse);
			break;
	    }
    }


//
//SetPlayWindow(...)
//
// Sets the play window, based on the
// arguments.
// return value indicates if the operation is successful or not.
TBool CMVSAppUi::SetPlayWindow(TTimeIntervalMicroSeconds aStart,
                                 TTimeIntervalMicroSeconds aEnd)
    {
    iStart = aStart;
    iEnd = aEnd;
    TInt err = iAudioPlayAgent->SetPlayWindow(iStart,iEnd);
    if(err != KErrNone)
    	{
    	LogErrors(err);
    	iStart = 0;
    	iEnd = iDuration;
    	return EFalse;
		}
    else
    	{
    	return ETrue;
    	}
    }


//
//SetMetaDataL(...)
//
// Sets the MetaData, based on the arguments
//
void CMVSAppUi::SetMetaDataL(RPointerArray<CMMFMetaDataEntry> aMetaArray)
	{
	switch(iMode)
		{
		case EAudioRecord:
			iAudioRecordAgent->SetMetaDataL(aMetaArray);
			break;
			
		case EVideoRecord:
			iVideoRecordAgent->SetMetaDataL(aMetaArray);
			break;
		default:
			ASSERT(EFalse);
			break;
		}
	
	}
	

//
//GetMetaDataL(...)
//
// Sets the MetaData, based on the arguments
//
void CMVSAppUi::GetMetaDataL(RPointerArray<CMMFMetaDataEntry>& aMetaArray)
	{
	switch(iMode)
		{
		case EAudioPlay:
			iAudioPlayAgent->GetMetaArrayL(aMetaArray);
			break;
			
		case EVideoPlay:
			iVideoPlayAgent->GetMetaDataArrayL(aMetaArray);
			break;
		default:
			ASSERT(EFalse);
			break;
		}
	
	}



//
//SetCrop(...)
//
// Sets the crop window, and also whether to crop from begining 
// or end, based on the arguments passed
//
void CMVSAppUi::SetCropL(TTimeIntervalMicroSeconds aCropStart,TBool aCropFromBeginning)
	{
	iAudioRecordAgent->CropClipL(aCropStart,aCropFromBeginning);
	iCropStart=aCropStart;
	iCropFromBeginning=aCropFromBeginning;
	}
	

void CMVSAppUi::SetBitRateL(TUint aBitRate)
	{
	TRAPD(err,iAudioRecordAgent->SetDestinationBitRateL(aBitRate));
	if(err != KErrNone)
		{
		iLogger.Write(_L("SetDestinationBitRateL returned err"));
		}
	else
		{
		iBitRate = aBitRate;
		}
	}


void CMVSAppUi::SetSampleRateL(TUint aSampleRate)
	{
	iAudioRecordAgent->SetDestinationSampleRateL(aSampleRate);
	iSampleRate = aSampleRate;
	}

void CMVSAppUi::SetChannelsL(TUint aNumberOfChannels)
	{
	iAudioRecordAgent->SetDestinationNumberOfChannelsL(aNumberOfChannels);
	iChannels = aNumberOfChannels;
	}
	
void CMVSAppUi::SetCodecsL(TFourCC aDataType)
	{
	iAudioRecordAgent->SetDestinationDataTypeL(aDataType);	
	}

	
	
void CMVSAppUi::InitializeMetaArrayL()
    {
    //Reset the meta array
    iMetaArray.Reset();
    if(iMetaTitle != NULL)
        {
        iMetaArray.AppendL(iMetaTitle);
        }

    if(iMetaAuthor != NULL)
        {
        iMetaArray.AppendL(iMetaAuthor);
        }

    if(iMetaCopyright != NULL)
        {
        iMetaArray.AppendL(iMetaCopyright);
        }

    if(iMetaRevision != NULL)
        {
        iMetaArray.AppendL(iMetaRevision);
        }

    if(iMetaCategory != NULL)
        {
        iMetaArray.AppendL(iMetaCategory);
        }

    if(iMetaComments != NULL)
        {
        iMetaArray.AppendL(iMetaComments);
        }
    }
 
//
//DynInitMenuPaneL(...)            *** This method can LEAVE ***
//
// Initialises the availibility of controls on the menu bar depending on the
// current state of the MVSApp. This allows the application to 
// constrain the availibility of controls which are inappropriate in certain
// states.
//
     
void CMVSAppUi::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
    {
    switch(iMode)
    	{
        case (EAudioRecord):
            //File menu          
            if(aResourceId == R_MVS_FILE_MENU)
            	{
                aMenuPane->SetItemDimmed(EMVSCmdNew,ETrue);
                aMenuPane->SetItemDimmed(EMVSCmdOpen,ETrue);
                aMenuPane->SetItemDimmed(EMVSCmdClose,EFalse);
                aMenuPane->SetItemDimmed(EMVSCmdSystemInfo,EFalse);
                aMenuPane->SetItemDimmed(EMVSCmdExit,EFalse);
				}

            //Edit Menu
            if(aResourceId == R_MVS_EDIT_MENU)
                {
                aMenuPane->SetItemDimmed(EMVSCmdSetCropWindow,EFalse);
                aMenuPane->SetItemDimmed(EMVSCmdSetPlayWindow,ETrue);
                aMenuPane->SetItemDimmed(EMVSCmdClearPlayWindow,ETrue);
                }

            //Settings menu > 
            if(aResourceId == R_MVS_SETTINGS_MENU)
                {
                aMenuPane->SetItemDimmed(EMVSCmdAudioSetting,EFalse);
                aMenuPane->SetItemDimmed(EMVSVideoCmdSettings,ETrue);
                aMenuPane->SetItemDimmed(EMVSCmdEditMetadata,EFalse);
                aMenuPane->SetItemDimmed(EMVSCmdClearLog,ETrue);
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
         		aMenuPane->SetItemDimmed(EMVSCmdEnableSubtitles, ETrue);
         		aMenuPane->SetItemDimmed(EMVSCmdDisableSubtitles, ETrue);
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
                }
			
			if(iState == EAudioRecording)
				{
				//Controls menu 
	            if(aResourceId == R_MVS_CONTROLS_MENU)
	            	{
		            aMenuPane->SetItemDimmed(EMVSCmdPlay,ETrue);
		            aMenuPane->SetItemDimmed(EMVSCmdPause,ETrue);
		            aMenuPane->SetItemDimmed(EMVSCmdStop,EFalse);
		            aMenuPane->SetItemDimmed(EMVSCmdRewind,ETrue);
		            aMenuPane->SetItemDimmed(EMVSCmdFastForward,ETrue);
		            aMenuPane->SetItemDimmed(EMVSCmdRecord,ETrue);
		            }          	
				}
			else
				{
				if(aResourceId == R_MVS_CONTROLS_MENU)
	            	{
		            aMenuPane->SetItemDimmed(EMVSCmdPlay,ETrue);
		            aMenuPane->SetItemDimmed(EMVSCmdPause,ETrue);
		            aMenuPane->SetItemDimmed(EMVSCmdStop,ETrue);
		            aMenuPane->SetItemDimmed(EMVSCmdRewind,ETrue);
		            aMenuPane->SetItemDimmed(EMVSCmdFastForward,ETrue);
		            aMenuPane->SetItemDimmed(EMVSCmdRecord,EFalse);
		            }
				}
            break;
            
            
        case (EVideoRecord):
        	//File menu
            if(aResourceId == R_MVS_FILE_MENU)
            	{
                aMenuPane->SetItemDimmed(EMVSCmdNew,ETrue);
                aMenuPane->SetItemDimmed(EMVSCmdOpen,ETrue);
                aMenuPane->SetItemDimmed(EMVSCmdClose,EFalse);
                aMenuPane->SetItemDimmed(EMVSCmdSystemInfo,EFalse);
                aMenuPane->SetItemDimmed(EMVSCmdExit,EFalse);
				}

            //Edit Menu
            if(aResourceId == R_MVS_EDIT_MENU)
                {
                aMenuPane->SetItemDimmed(EMVSCmdSetCropWindow,ETrue);
                aMenuPane->SetItemDimmed(EMVSCmdSetPlayWindow,ETrue);
                aMenuPane->SetItemDimmed(EMVSCmdClearPlayWindow,ETrue);
                }

            //Settings menu > 
            if(aResourceId == R_MVS_SETTINGS_MENU)
                {
                aMenuPane->SetItemDimmed(EMVSCmdAudioSetting,ETrue);
                aMenuPane->SetItemDimmed(EMVSVideoCmdSettings,ETrue);
                aMenuPane->SetItemDimmed(EMVSCmdEditMetadata,EFalse);
                aMenuPane->SetItemDimmed(EMVSCmdClearLog,ETrue);
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
         		aMenuPane->SetItemDimmed(EMVSCmdEnableSubtitles, ETrue);
         		aMenuPane->SetItemDimmed(EMVSCmdDisableSubtitles, ETrue);
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
                }

            if(iState == EVideoRecording)
				{
				//File menu
            	if(aResourceId == R_MVS_FILE_MENU)
	            	{
	                aMenuPane->SetItemDimmed(EMVSCmdNew,ETrue);
	                aMenuPane->SetItemDimmed(EMVSCmdOpen,ETrue);
	                aMenuPane->SetItemDimmed(EMVSCmdClose,EFalse);
	                aMenuPane->SetItemDimmed(EMVSCmdSystemInfo,EFalse);
	                aMenuPane->SetItemDimmed(EMVSCmdExit,EFalse);
					}
				
				//Controls menu 
	            if(aResourceId == R_MVS_CONTROLS_MENU)
	            	{
		            aMenuPane->SetItemDimmed(EMVSCmdPlay,ETrue);
		            aMenuPane->SetItemDimmed(EMVSCmdPause,ETrue);
		            aMenuPane->SetItemDimmed(EMVSCmdStop,EFalse);
		            aMenuPane->SetItemDimmed(EMVSCmdRewind,ETrue);
		            aMenuPane->SetItemDimmed(EMVSCmdFastForward,ETrue);
		            aMenuPane->SetItemDimmed(EMVSCmdRecord,ETrue);
		            }          	
				}
			else
				{
				if(aResourceId == R_MVS_CONTROLS_MENU)
	            	{
		            aMenuPane->SetItemDimmed(EMVSCmdPlay,ETrue);
		            aMenuPane->SetItemDimmed(EMVSCmdPause,ETrue);
		            aMenuPane->SetItemDimmed(EMVSCmdStop,ETrue);
		            aMenuPane->SetItemDimmed(EMVSCmdRewind,ETrue);
		            aMenuPane->SetItemDimmed(EMVSCmdFastForward,ETrue);
		            aMenuPane->SetItemDimmed(EMVSCmdRecord,EFalse);
		            }
				}
			if(iState == EVideoOpened)
				{
				//File menu
            	if(aResourceId == R_MVS_FILE_MENU)
	            	{
	                aMenuPane->SetItemDimmed(EMVSCmdNew,ETrue);
	                aMenuPane->SetItemDimmed(EMVSCmdOpen,ETrue);
	                aMenuPane->SetItemDimmed(EMVSCmdClose,EFalse);
	                aMenuPane->SetItemDimmed(EMVSCmdSystemInfo,EFalse);
	                aMenuPane->SetItemDimmed(EMVSCmdExit,EFalse);
					}
				}
			if(iState == EVideoStopped)
				{
				//File menu
            	if(aResourceId == R_MVS_FILE_MENU)
	            	{
	                aMenuPane->SetItemDimmed(EMVSCmdNew,ETrue);
	                aMenuPane->SetItemDimmed(EMVSCmdOpen,ETrue);
	                aMenuPane->SetItemDimmed(EMVSCmdClose,EFalse);
	                aMenuPane->SetItemDimmed(EMVSCmdSystemInfo,EFalse);
	                aMenuPane->SetItemDimmed(EMVSCmdExit,EFalse);
					}
				}
            break;

        case (EAudioPlay):
                       
            if(iState == EAudioOpened)
            	{
            	//File menu
	            if(aResourceId == R_MVS_FILE_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdNew,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdOpen,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdClose,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdSystemInfo,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdExit,EFalse);
	               	}

	             //Edit Menu
	            if(aResourceId == R_MVS_EDIT_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdSetCropWindow,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdSetPlayWindow,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdClearPlayWindow,EFalse);
	               	}

	            //Settings menu 
	            if(aResourceId == R_MVS_SETTINGS_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdAudioSetting,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSVideoCmdSettings,ETrue);
	                //aMenuPane->SetItemDimmed(EMVSCmdEditMetadata,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdClearLog,EFalse);
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
             		aMenuPane->SetItemDimmed(EMVSCmdEnableSubtitles, ETrue);
             		aMenuPane->SetItemDimmed(EMVSCmdDisableSubtitles, ETrue);
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	               	}

	            //Controls menu 
	            if(aResourceId == R_MVS_CONTROLS_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdPlay,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdPause,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdStop,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdRewind,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdFastForward,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdRecord,ETrue);
	               	}	
            	}
            
            else if(iState == EAudioPlaying)
            	{
            	//File menu
	            if(aResourceId == R_MVS_FILE_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdNew,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdOpen,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdClose,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdSystemInfo,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdExit,EFalse);
	               	}

	             //Edit Menu
	            if(aResourceId == R_MVS_EDIT_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdSetCropWindow,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdSetPlayWindow,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdClearPlayWindow,ETrue);
	               	}

	            //Settings menu 
	            if(aResourceId == R_MVS_SETTINGS_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdAudioSetting,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSVideoCmdSettings,ETrue);
	                aMenuPane->SetItemDimmed(EMVSCmdEditMetadata,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdClearLog,EFalse);
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
             		aMenuPane->SetItemDimmed(EMVSCmdEnableSubtitles, ETrue);
             		aMenuPane->SetItemDimmed(EMVSCmdDisableSubtitles, ETrue);
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	               	}

	            //Controls menu 
	            if(aResourceId == R_MVS_CONTROLS_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdPlay,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdPause,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdStop,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdRewind,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdFastForward,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdRecord,ETrue);
	               	}	
            	}
            
            else if(iState == EAudioPaused)
            	{
            	//File menu
	            if(aResourceId == R_MVS_FILE_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdNew,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdOpen,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdClose,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdSystemInfo,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdExit,EFalse);
	               	}

	             //Edit Menu
	            if(aResourceId == R_MVS_EDIT_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdSetCropWindow,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdSetPlayWindow,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdClearPlayWindow,EFalse);
	               	}

	            //Settings menu 
	            if(aResourceId == R_MVS_SETTINGS_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdAudioSetting,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSVideoCmdSettings,ETrue);
	                aMenuPane->SetItemDimmed(EMVSCmdEditMetadata,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdClearLog,EFalse);
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
             		aMenuPane->SetItemDimmed(EMVSCmdEnableSubtitles, ETrue);
             		aMenuPane->SetItemDimmed(EMVSCmdDisableSubtitles, ETrue);
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	               	}

	            //Controls menu 
	            if(aResourceId == R_MVS_CONTROLS_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdPlay,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdPause,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdStop,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdRewind,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdFastForward,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdRecord,ETrue);
	               	}
            	}
            else if(iState == EAudioStopped)
            	{
            	//File menu
	            if(aResourceId == R_MVS_FILE_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdNew,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdOpen,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdClose,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdSystemInfo,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdExit,EFalse);
	               	}

	             //Edit Menu
	            if(aResourceId == R_MVS_EDIT_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdSetCropWindow,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdSetPlayWindow,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdClearPlayWindow,EFalse);
	               	}

	            //Settings menu 
	            if(aResourceId == R_MVS_SETTINGS_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdAudioSetting,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSVideoCmdSettings,ETrue);
	                aMenuPane->SetItemDimmed(EMVSCmdEditMetadata,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdClearLog,EFalse);
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
             		aMenuPane->SetItemDimmed(EMVSCmdEnableSubtitles, ETrue);
             		aMenuPane->SetItemDimmed(EMVSCmdDisableSubtitles, ETrue);
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	               	}

	            //Controls menu 
	            if(aResourceId == R_MVS_CONTROLS_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdPlay,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdPause,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdStop,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdRewind,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdFastForward,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdRecord,ETrue);
	               	}
            	}
            break;
         
    	case (EVideoPlay):
    		if(iState == EVideoOpened)
            	{
            	//File menu
	            if(aResourceId == R_MVS_FILE_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdNew,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdOpen,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdClose,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdSystemInfo,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdExit,EFalse);
	               	}

	             //Edit Menu
	            if(aResourceId == R_MVS_EDIT_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdSetCropWindow,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdSetPlayWindow,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdClearPlayWindow,ETrue);
	               	}

	            //Settings menu 
	            if(aResourceId == R_MVS_SETTINGS_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdAudioSetting,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSVideoCmdSettings,EFalse);
	                aMenuPane->SetItemDimmed(EMVSCmdEditMetadata,ETrue);
	                aMenuPane->SetItemDimmed(EMVSCmdMultiScreen,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdClearLog,EFalse);
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
             		TBool subAvail = iVideoPlayAgent->SubtitlesAvailable();
             		aMenuPane->SetItemDimmed(EMVSCmdEnableSubtitles, !subAvail);
             		aMenuPane->SetItemDimmed(EMVSCmdDisableSubtitles, !subAvail);
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	               	}

	            //Controls menu 
	            if(aResourceId == R_MVS_CONTROLS_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdPlay,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdPause,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdStop,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdRewind,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdFastForward,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdRecord,ETrue);
	               	}	
            	}
            
    		else if(iState == EVideoPlaying)
            	{
            	//File menu
	            if(aResourceId == R_MVS_FILE_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdNew,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdOpen,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdClose,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdSystemInfo,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdExit,EFalse);
	               	}

	             //Edit Menu
	            if(aResourceId == R_MVS_EDIT_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdSetCropWindow,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdSetPlayWindow,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdClearPlayWindow,ETrue);
	               	}

	            //Settings menu 
	            if(aResourceId == R_MVS_SETTINGS_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdAudioSetting,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSVideoCmdSettings,EFalse);
	                aMenuPane->SetItemDimmed(EMVSCmdEditMetadata,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdClearLog,EFalse);
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
             		TBool subAvail = iVideoPlayAgent->SubtitlesAvailable();
             		aMenuPane->SetItemDimmed(EMVSCmdEnableSubtitles, !subAvail);
             		aMenuPane->SetItemDimmed(EMVSCmdDisableSubtitles, !subAvail);
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	               	}

	            //Controls menu 
	            if(aResourceId == R_MVS_CONTROLS_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdPlay,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdPause,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdStop,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdRewind,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdFastForward,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdRecord,ETrue);
	               	}	
            	}
            else if(iState == EVideoPaused)
            	{
            	//File menu
	            if(aResourceId == R_MVS_FILE_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdNew,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdOpen,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdClose,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdSystemInfo,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdExit,EFalse);
	               	}

	             //Edit Menu
	            if(aResourceId == R_MVS_EDIT_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdSetCropWindow,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdSetPlayWindow,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdClearPlayWindow,ETrue);
	               	}

	            //Settings menu 
	            if(aResourceId == R_MVS_SETTINGS_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdAudioSetting,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSVideoCmdSettings,EFalse);
	                aMenuPane->SetItemDimmed(EMVSCmdEditMetadata,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdClearLog,EFalse);
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
             		TBool subAvail = iVideoPlayAgent->SubtitlesAvailable();
             		aMenuPane->SetItemDimmed(EMVSCmdEnableSubtitles, !subAvail);
             		aMenuPane->SetItemDimmed(EMVSCmdDisableSubtitles, !subAvail);
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	               	}

	            //Controls menu 
	            if(aResourceId == R_MVS_CONTROLS_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdPlay,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdPause,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdStop,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdRewind,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdFastForward,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdRecord,ETrue);
	               	}
            	}
            else if(iState == EVideoStopped)
            	{
            	//File menu
	            if(aResourceId == R_MVS_FILE_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdNew,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdOpen,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdClose,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdSystemInfo,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdExit,EFalse);
	               	}

	             //Edit Menu
	            if(aResourceId == R_MVS_EDIT_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdSetCropWindow,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdSetPlayWindow,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdClearPlayWindow,ETrue);
	               	}

	            //Settings menu 
	            if(aResourceId == R_MVS_SETTINGS_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdAudioSetting,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSVideoCmdSettings,EFalse);
	                aMenuPane->SetItemDimmed(EMVSCmdEditMetadata,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdClearLog,EFalse);
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
             		TBool subAvail = iVideoPlayAgent->SubtitlesAvailable();
             		aMenuPane->SetItemDimmed(EMVSCmdEnableSubtitles, !subAvail);
             		aMenuPane->SetItemDimmed(EMVSCmdDisableSubtitles, !subAvail);
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
	               	}

	            //Controls menu 
	            if(aResourceId == R_MVS_CONTROLS_MENU)
	            	{
	               	aMenuPane->SetItemDimmed(EMVSCmdPlay,EFalse);
	               	aMenuPane->SetItemDimmed(EMVSCmdPause,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdStop,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdRewind,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdFastForward,ETrue);
	               	aMenuPane->SetItemDimmed(EMVSCmdRecord,ETrue);
	               	}
            	}
            break;
    
    	case (EIdle):
       		//File menu
            if(aResourceId == R_MVS_FILE_MENU)
            	{
                aMenuPane->SetItemDimmed(EMVSCmdNew,EFalse);
                aMenuPane->SetItemDimmed(EMVSCmdOpen,EFalse);
                aMenuPane->SetItemDimmed(EMVSCmdClose,ETrue);
                aMenuPane->SetItemDimmed(EMVSCmdSystemInfo,EFalse);
                aMenuPane->SetItemDimmed(EMVSCmdExit,EFalse);
				}

            //Edit Menu
            if(aResourceId == R_MVS_EDIT_MENU)
                {
                aMenuPane->SetItemDimmed(EMVSCmdSetCropWindow,ETrue);
                aMenuPane->SetItemDimmed(EMVSCmdSetPlayWindow,ETrue);
                aMenuPane->SetItemDimmed(EMVSCmdClearPlayWindow,ETrue);
                }

            //Settings menu > 
            if(aResourceId == R_MVS_SETTINGS_MENU)
                {
                aMenuPane->SetItemDimmed(EMVSCmdAudioSetting,ETrue);
                aMenuPane->SetItemDimmed(EMVSVideoCmdSettings,ETrue);
                aMenuPane->SetItemDimmed(EMVSCmdEditMetadata,ETrue);
                aMenuPane->SetItemDimmed(EMVSCmdMultiScreen,EFalse);
                aMenuPane->SetItemDimmed(EMVSCmdClearLog,EFalse);
#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
         		aMenuPane->SetItemDimmed(EMVSCmdEnableSubtitles, ETrue);
         		aMenuPane->SetItemDimmed(EMVSCmdDisableSubtitles, ETrue);
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
                }

            //Controls menu 
            if(aResourceId == R_MVS_CONTROLS_MENU)
            	{
	            aMenuPane->SetItemDimmed(EMVSCmdPlay,ETrue);
	            aMenuPane->SetItemDimmed(EMVSCmdPause,ETrue);
	            aMenuPane->SetItemDimmed(EMVSCmdStop,ETrue);
	            aMenuPane->SetItemDimmed(EMVSCmdRewind,ETrue);
	            aMenuPane->SetItemDimmed(EMVSCmdFastForward,ETrue);
	            aMenuPane->SetItemDimmed(EMVSCmdRecord,ETrue);
	            }
            break;

        default:
            ASSERT(EFalse);
            break;
    	}
    }


void CMVSAppUi::InternalizeL(RReadStream& aStream)
	{
	iVolumeOrGain = aStream.ReadInt32L();
	iBalance = aStream.ReadInt32L();
	iNoRepeats = aStream.ReadInt32L();
	iRepeatDelay = aStream.ReadReal64L();
	}

void CMVSAppUi::ExternalizeL(RWriteStream& aStream)
	{
	aStream.WriteInt32L(iVolumeOrGain);
	aStream.WriteInt32L(iBalance);
	aStream.WriteInt32L(iNoRepeats);
	aStream.WriteReal64L(iRepeatDelay.Int64());
	}

//Returns the selected controller plugin from the UI
void CMVSAppUi::SelectedPluginIdx(TInt aIdx)
	{
	iSelectedPluginIdx = aIdx;
	}
	
//Returns the selected Extension from the UI	
void CMVSAppUi::SelectedExtension(TDesC16& aDes)
	{
	iControllerExt = aDes;
	}


TInt CMVSAppUi::GetExtensionListL(CDesCArrayFlat* aExtArray,
								  TBool aMediaType)
	{
 	return iPluginInfo->GetExtensionListL(aMediaType,aExtArray);	
	}


TInt CMVSAppUi::GetPluginListL(TInt8 aExtIdx,CDesCArrayFlat* aExtArray,CDesCArrayFlat* aPluginArray,TBool aMediaType)
	{
	iControllerExt.Copy(aExtArray->operator[](aExtIdx));
	iAudioUidArray.Reset();
	iAudioPlayUidArray.Reset();
	iVideoUidArray.Reset();
	if(!aMediaType)
		{
		return iPluginInfo->GetAudioPluginListL(aPluginArray,iAudioUidArray,iAudioPlayUidArray,&iControllerExt);	
		} 
	else
		{
		return iPluginInfo->GetVideoPluginListL(aPluginArray,iVideoUidArray,&iControllerExt);	
		}
	}
	
TInt CMVSAppUi::GetSystemInfoL(CDesCArrayFlat* aPluginArray)
	{
	return iPluginInfo->GetPluginListL(aPluginArray,iUidArray);
	}

//Returnd the selected media type, ie Audio or Video
void CMVSAppUi::SelectedMedia(TBool aMediaType)
	{
	iMediaType = aMediaType;	
	}


void CMVSAppUi::GetSupportedBitRatesL(RArray <TUint>& aBitRateArray) 
	{
	iAudioRecordAgent->GetSupportedBitRatesArrayL(aBitRateArray);
	}

void CMVSAppUi::GetSupportedSampleRatesArrayL(RArray<TUint>& aSampleRatesArray)
	{
	iAudioRecordAgent->GetSupportedSampleRatesArrayL(aSampleRatesArray);
	}

void CMVSAppUi::GetSupportedNoChannelsArrayL(RArray<TUint>& aNoChannelsArray)
	{
	iAudioRecordAgent->GetSupportedNoChannelsArrayL(aNoChannelsArray);
	}

void CMVSAppUi::GetSupportedCodecsArrayL(RArray<TFourCC>& aCodecsArray)
	{
	iAudioRecordAgent->GetSupportedCodecsArrayL(aCodecsArray);
	}

void CMVSAppUi::GetSupportedFrameRatesL(RArray <TReal32>& aFrameRateArray) 
	{
	iVideoRecordAgent->GetSupportedFrameRatesArrayL(aFrameRateArray);
	}

void CMVSAppUi::GetSupportedFrameSizeArrayL(RArray <TSize>& aFrameSizeArray) 
	{
	iVideoRecordAgent->GetSupportedFrameSizesArrayL(aFrameSizeArray);
	}
	
void CMVSAppUi::SetVideoFrameRateL(TReal32 aFrameRate)
	{
	TRAPD(err,iVideoRecordAgent->SetVideoFrameRateL(aFrameRate));
	if(err != KErrNone)
		{
		iLogger.Write(_L("SetVideoFrameRateL returned err"));
		}
	else
		{
		iVideoFrameRate = aFrameRate;
		}
	}

void CMVSAppUi::SetVideoFrameSizeL(TSize aFrameSize)
	{
	TRAPD(err,iVideoRecordAgent->SetVideoFrameSizeL(aFrameSize));
	if(err != KErrNone)
		{
		iLogger.Write(_L("SetVideoFrameRateL returned err"));
		}
	else
		{
		iFrameSize = aFrameSize;
		}
	}
	
void CMVSAppUi::SetAudioEnabledL(TBool aAudioEnabled)
	{
	iAudioEnabled = aAudioEnabled;
	iVideoRecordAgent->SetAudioEnabledL(iAudioEnabled);
	iLogger.Write(_L("Audio Enabled for Video"));
	}

void CMVSAppUi::GetSupportedVideoTypesL(CDesC8ArrayFlat& aMimeArray)
	{
	iVideoRecordAgent->GetSupportedVideoTypesL(aMimeArray);
	}

void CMVSAppUi::SetVideoTypeL(const TDesC8 &aType)
	{
	iVideoRecordAgent->SetVideoTypeL(aType);
	}
	
void CMVSAppUi::WriteAudioDataL(CMVSConfigAudioFormatDialog* aAudioFormatDlg,
					  const TUid& aUid)
	{
	iSettingManager->WriteAudioDataL(aAudioFormatDlg,aUid);
	}

void CMVSAppUi::WriteVideoDataL(CMVSConfigVideoFormatDialog* aVideoFormatDlg,
						  const TUid& aUid)
	{
	iSettingManager->WriteVideoDataL(aVideoFormatDlg,aUid);
	}

TReal32 CMVSAppUi::GetVideoFrameRateL()
	{
	if(iMode == EVideoRecord)
		{
		iVideoFrameRate=iVideoRecordAgent->VideoFrameRateL();	
		}
	else if(iMode == EVideoPlay)
		{
		iVideoFrameRate=iVideoPlayAgent->VideoFrameRateL();
		}
	return iVideoFrameRate;	
	}
	
TVideoRotation CMVSAppUi::GetRotationL() 
	{
	iVideoRotation = iVideoPlayAgent->RotationL();
	return iVideoRotation;
	}

void CMVSAppUi::SetRotationL(TVideoRotation aRotation)
	{
	TRAPD(err, iVideoPlayAgent->SetRotationL(aRotation));
	if (err == KErrNotSupported)
		{
		User::InfoPrint(KNotsupported);
		iLogger.Write(_L("Error,This feature not supported by the controller"));
		}
	else if (err == KErrNotReady)
		{
		iLogger.Write(_L("Error, Videoplayer not yet created in the agent"));
		}
	else
		{
		iRotation = aRotation;
		}
	}

void CMVSAppUi::GetScaleFactorL(TReal32& aScaleWidth, TReal32& aScaleHeight, TBool& aAntiAliasFiltering)
	{
	TRAPD(err, iVideoPlayAgent->GetScaleFactorL(aScaleWidth, aScaleHeight, aAntiAliasFiltering));
	if (err == KErrNotSupported)
		{
		User::InfoPrint(KNotsupported);
		}
	else if (err == KErrNotReady)
		{
		iLogger.Write(_L("Error, Videoplayer not yet created in the agent"));
		}
	}

void CMVSAppUi::SetScaleFactorL(TReal32 aScaleWidth,TReal32 aScaleHeight,TBool aAntiAliasFiltering)
	{
	TRAPD(err, iVideoPlayAgent->SetScaleFactorL(aScaleWidth,aScaleHeight,aAntiAliasFiltering));
	if (err == KErrNotSupported)
		{
		User::InfoPrint(KNotsupported);
		}
	else if (err == KErrNotReady)
		{
		iLogger.Write(_L("Error, Videoplayer not yet created in the agent"));
		}
	}

void CMVSAppUi::SetCropRegionL(const TRect& aCropRegion)
	{
	TRAPD(err, iVideoPlayAgent->SetCropRegionL(aCropRegion));
	if (err == KErrNotSupported)
		{
		User::InfoPrint(KNotsupported);
		}
	else if (err == KErrNotReady)
		{
		iLogger.Write(_L("Error, Videoplayer not yet created in the agent"));
		}
	}
	
void CMVSAppUi::GetCropRegionL(TRect& aCropRegion)
	{
	TRAPD(err, iVideoPlayAgent->GetCropRegionL(aCropRegion));
	if (err == KErrNotSupported)
		{
		User::InfoPrint(KNotsupported);
		}
	else if (err == KErrNotReady)
		{
		iLogger.Write(_L("Error, Videoplayer not yet created in the agent"));
		}
	}

void CMVSAppUi::SetVideoExtent(const TRect& aVideoExtent)
	{
	TRAPD(err, iVideoPlayAgent->SetVideoExtentL(aVideoExtent));
    if (err != KErrNone)
		{
		TBuf<50> text;
		text.AppendFormat(_L("SetVideoExtentL - %d"), err);
		User::InfoPrint(text);
		return;
		}
    iVideoExtentSet = ETrue;
    iVideoExtent = aVideoExtent;       			
	}

void CMVSAppUi::SetWindowClippingRect(const TRect& aWindowClipRect)
	{
	TRAPD(err, iVideoPlayAgent->SetWindowClipRectL(aWindowClipRect));
    if (err != KErrNone)
		{
		TBuf<50> text;
		text.AppendFormat(_L("SetWindowClippingRectL - %d"), err);
		User::InfoPrint(text);
		return;
		}
    
    iWindowClipRectSet = ETrue;
    iWindowClipRect = aWindowClipRect;                   			
	}
	
void CMVSAppUi::SetOverlayTextL(const TDesC& aOverlayText)
	{
	iOverlayText.Close();
	iOverlayText.CreateL(aOverlayText);	
	
	iAppView->iMainWindowControl->SetOverlayTextL(aOverlayText);
	}

const TDesC& CMVSAppUi::OverlayText() const
	{
	return iOverlayText;
	}

void CMVSAppUi::SetAutoScaleL(TAutoScaleType aScaleType, TInt aHorizPos, TInt aVertPos)
	{
	iVideoPlayAgent->SetAutoScaleL(aScaleType, aHorizPos, aVertPos);
	iAutoScaleType = aScaleType;
	iAutoScaleHorizPos = aHorizPos;
	iAutoScaleVertPos = aVertPos;
	}

void CMVSAppUi::GetAutoScale(TAutoScaleType& aScaleType, TInt& aHorizPos, TInt& aVertPos)
	{
	aScaleType = iAutoScaleType;
	aHorizPos = iAutoScaleHorizPos;
	aVertPos = iAutoScaleVertPos;
	}

void CMVSAppUi::SetOutputScreen(TInt aScreenNumber)
	{
	iScreenNumber = aScreenNumber;
	}
	
void CMVSAppUi::RegisterForNotification(TBool aRegistered)
	{
	if(iRegistered)
		{
		iAlreadyRegistered = ETrue;	
		}
	iRegistered = aRegistered;
    if(iMode == EVideoPlay)
		{
		if(iRegistered && iAlreadyRegistered == EFalse)
			{
			TInt regerr = iVideoPlayAgent->RegisterForNotification();	
			if(regerr == KErrNone)
				{
				User::InfoPrint(KRegisterSuccess);
				}
			else
				{
				User::InfoPrint(KRegisterFailure);	
				}
			}
		else if(iAlreadyRegistered && iRegistered == EFalse)
			{
			TInt cancelerr = iVideoPlayAgent->CancelNotification();
			if(cancelerr == KErrNone)
				{
				iAlreadyRegistered = EFalse;	
				User::InfoPrint(KRegisterCancelation);
				}
			else
				{
				User::InfoPrint(KCancelationFailure);	
				}
			}		
		}
	else if(iMode == EAudioPlay && iRegistered)
		{
		}
	}

void CMVSAppUi::SetPip(TBool aPip)
	{
	iPip = aPip;
	}

void CMVSAppUi::SetCrp(TBool aCrp)
	{
	iCrp = aCrp;
	}
/*
UpdteDisplay(....)
Used to populate all the view windows,based
on the arguments passed this includes updating the time bar,the file name
and the state of the app.
*/
void CMVSAppUi::UpdateDisplayL(TMVSState aState)
	{
    TBuf<KTimeValueLength> disTime;
    TFileName disFName;
    TBuf<32> disState;
    disTime.Append(iDisplayTime);
    disFName.Append(iDisplayFName);
    disState.Append(iDisplayState);

    iDisplayFName.Zero();
    iDisplayState.Zero();
    iDisplayTime.Zero();
     
    switch(aState)
    	{
        case(ENotReady):
        	iCoeEnv->ReadResourceL(iDisplayFName, R_NOFILE);
            iCoeEnv->ReadResourceL(iDisplayState, R_IDLE);
            iCoeEnv->ReadResourceL(iDisplayTime, R_ZEROTIME);
            break;
        case(EAudioOpened):
        case(EVideoOpened):    
          	if(iMode == EAudioPlay || iMode == EVideoPlay)
          		{
          	   	iDisplayFName.Append(iSrcFName);
          	   	}
           	if(iMode == EAudioRecord || iMode == EVideoRecord)
           	   	{
           	   	iDisplayFName.Append(iRecFName);
           	   	}
          	iCoeEnv->ReadResourceL(iDisplayState, R_OPEN);
       	    break;
          	   
        case(EAudioStopped):
        case(EVideoStopped):
            if(iMode == EAudioPlay || iMode == EVideoPlay)
            	{
            	iDisplayFName.Append(iSrcFName);
               	} 
            if(iMode == EAudioRecord || iMode == EVideoRecord)
               	{
               	iDisplayFName.Append(iRecFName);
               	}
			iCoeEnv->ReadResourceL(iDisplayState, R_READY);
            break;

        case(EAudioPlaying):
        case(EVideoPlaying):
            if(iMode == EAudioPlay || iMode == EVideoPlay)
            	{
               	iDisplayFName.Append(iSrcFName);
               	} 
            if(iMode == EAudioRecord || iMode == EVideoRecord)
               	{
               	iDisplayFName.Append(iRecFName);
               	}
            iCoeEnv->ReadResourceL(iDisplayState, R_PLAYING);
            break;

        case(EAudioRecording):
            iDisplayFName.Append(iRecFName);
            iCoeEnv->ReadResourceL(iDisplayState, R_RECORDING);
            ConvertTimeToDes(iAudioRecordAgent->Position(), iDisplayTime);
            break;
            
        case(EVideoRecording):
        	iDisplayFName.Append(iRecFName);
            iCoeEnv->ReadResourceL(iDisplayState, R_RECORDING);
            ConvertTimeToDes(iVideoRecordAgent->DurationL(), iDisplayTime);
            break;

        case(EAudioPaused):
        case(EVideoPaused):
            if(iMode == EAudioPlay || iMode == EVideoPlay)
            	{
               	iDisplayFName.Append(iSrcFName);
               	} 
            if(iMode == EAudioRecord || iMode == EVideoRecord)
               	{
               	iDisplayFName.Append(iRecFName);
               	}
            iCoeEnv->ReadResourceL(iDisplayState, R_PAUSED);
            break;
          
        default:
            iCoeEnv->ReadResourceL(iDisplayFName, R_NOFILE);
            iCoeEnv->ReadResourceL(iDisplayState, R_IDLE);
            iCoeEnv->ReadResourceL(iDisplayTime, R_ZEROTIME);
            break;
        }
     
    	if (iMode == EVideoPlay)
    		{
    		if (aState != EVideoPlaying && iVideoPlayAgent->SupportVideoPlayerUtility2() && iDisplayAdded)
    			{
    			iVideoCtl->Pip().Clear();
    			PrepareVideoControlsL();
       			}
    		}    		
    
     PopulateInfoWindowDataL(aState);

     if(disFName.CompareF(iDisplayFName) != 0)
     	{
        iAppView->iStatusWindowFNameControl->SetTextL(iDisplayFName);
        iAppView->iStatusWindowFNameControl->DrawNow();
        }
     if(disTime.CompareF(iDisplayTime) != 0)
     	{
        iAppView->iStatusWindowTimeControl->SetTextL(iDisplayTime);
        iAppView->iStatusWindowTimeControl->DrawNow();
        }
     if(disState.CompareF(iDisplayState) != 0)
        {
        iAppView->iStatusWindowStateControl->SetTextL(iDisplayState);
        iAppView->iStatusWindowStateControl->DrawNow();
        }
     
     TBuf<256> dollarDes;
     BuildDollarDesFromArrayL(dollarDes);
     if(iDollarDes.CompareF(dollarDes) != 0)
        {
        iDollarDes.Zero();
        iDollarDes.Append(dollarDes);
        iAppView->iInfoWindowControl->SetTextL(dollarDes);
        iAppView->iInfoWindowControl->DrawNow();
        }
     }
     


//
//PopulateMainWindowData(...)         ***TRAPPED ERRORS***
//
// Adds data to the array used to hold playback or recording data
// such as the volume, cropping window and balance. This is then used to
// build a $$-separated descriptor which is sent to the view to be
// displayed as lines of text within the main window.
//
void CMVSAppUi::PopulateInfoWindowDataL(TMVSState aState)
	{
    TInt balance;
    TBuf<256> infoBuf;
    TBuf<75> welcome;
    iCoeEnv->ReadResourceL(welcome, R_WELCOME);

    TBuf<32> format;
    iCoeEnv->ReadResourceL(format, R_FORMAT);
    
    TBuf<10> left;
    iCoeEnv->ReadResourceL(left, R_LEFT);

    TBuf<10> right;
    iCoeEnv->ReadResourceL(right, R_RIGHT);

    TBuf<1> close;
    iCoeEnv->ReadResourceL(close, R_CLOSE);

    TBuf<56> noCrop;
    iCoeEnv->ReadResourceL(noCrop, R_NOCROP);
     
    TBuf<KLabelCaptionMaxLength> center;
    iCoeEnv->ReadResourceL(center, R_CENTER);

    TBuf<KLabelCaptionMaxLength> volume;
    iCoeEnv->ReadResourceL(volume, R_VOLUME);

    TBuf<KLabelCaptionMaxLength> ramp;
    iCoeEnv->ReadResourceL(ramp, R_RAMP);

    TBuf<KLabelCaptionMaxLength> balanceDes;
    iCoeEnv->ReadResourceL(balanceDes, R_BALANCE);

    TBuf<KLabelCaptionMaxLength> cropStart;
    iCoeEnv->ReadResourceL(cropStart, R_CROPSTART);

    TBuf<KLabelCaptionMaxLength> cropEnd;
    iCoeEnv->ReadResourceL(cropEnd, R_CROPEND);

    TBuf<KLabelCaptionMaxLength> noRepeats;
    iCoeEnv->ReadResourceL(noRepeats, R_NOREPEATS);

    TBuf<32> audioPriority;
    iCoeEnv->ReadResourceL(audioPriority, R_AUDIOPRIORITY);

    TBuf<KLabelCaptionMaxLength> gain;
    iCoeEnv->ReadResourceL(gain, R_GAIN);

    TBuf<KLabelCaptionMaxLength> clipLength;
    iCoeEnv->ReadResourceL(clipLength, R_CLIPLENGTH);
    
    TBuf<KLabelCaptionMaxLength> bitRate;
    iCoeEnv->ReadResourceL(bitRate, R_BITRATE);
    
    TBuf<KLabelCaptionMaxLength> sampleRate;
    iCoeEnv->ReadResourceL(sampleRate, R_SAMPLERATE);
    
    TBuf<KLabelCaptionMaxLength> channels;
    iCoeEnv->ReadResourceL(channels,R_CHANNELS);
    
    TBuf<KLabelCaptionMaxLength> AudioEnabled;
    iCoeEnv->ReadResourceL(AudioEnabled, R_AUDIOENABLED);
    
    TBuf<KLabelCaptionMaxLength> AudioNotEnabled;
    iCoeEnv->ReadResourceL(AudioNotEnabled, R_AUDIONOTENABLED);

    _LIT(KPercent, "%");

    TBuf<KTimeValueLength> clipLengthDes;

    //If the window text has anything in it, clear it out
    if(iMainWindowText.Count() > 0)
    	{
        iMainWindowText.Reset();
        }
    TTimeIntervalMicroSeconds duration;
    TInt64 clipLengthVal =0;
    TUint bitrateValue = 0;
    TInt frameHeight = 0;
    TInt frameWidth = 0;
    TInt frameRate = 0;
    //Populate the array based on the current state
    switch(aState)
    	{
        //Idle, the main window is all but empty
        case(ENotReady):
        	iMainWindowText.AppendL(welcome);
           	if(iButtons->IsCommandDimmed(EMVSCmdPause))
         		{
         		iButtons->DimCommand(EMVSCmdPause,EFalse);
         		iButtons->DrawNow();	
         		}
         	if(iButtons->IsCommandDimmed(EMVSCmdPlay))
         		{
         		iButtons->DimCommand(EMVSCmdPlay,EFalse);
         		iButtons->DrawNow();	
         		}
         	if(iButtons->IsCommandDimmed(EMVSCmdStop))
         		{
         		iButtons->DimCommand(EMVSCmdStop,EFalse);
         		iButtons->DrawNow();	
         		}	
           	break;

        //Recording: show the gain, balance and crop window.
        case(EAudioRecording):
            if(iButtons->IsCommandDimmed(EMVSCmdStop))
            	{
            	iButtons->DimCommand(EMVSCmdStop,EFalse);
				iButtons->DrawNow();	
           	 	}
            infoBuf.Append(KAudioRecording);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            infoBuf.Append(gain);
  			infoBuf.AppendNum(iVolumeOrGain);
  			infoBuf.Append(KPercent);
  			iMainWindowText.AppendL(infoBuf);
  			infoBuf.Zero();
            infoBuf.Append(balanceDes);
            //Convert the balance to a number between 0 and 100
            iAudioRecordAgent->GetRecordBalance(iBalance);
            balance = (iBalance/2) + 50;
            //Translate this figure into 'Left', 'Right' or 'Center'
            if(balance < 50)
            	{
                infoBuf.Append(left);
                infoBuf.AppendNum(balance);
                infoBuf.Append(close);
                }
            else if(balance == 50)
                {
                infoBuf.Append(center);
                }
            else
                {
                infoBuf.Append(right);
                infoBuf.AppendNum(balance);
                infoBuf.Append(close);
                }
  			iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            infoBuf.Append(bitRate);
            if(iBitRate == 0)
            	{
            	infoBuf.Append(KNotSupported);
            	}
            else
	            {
	            infoBuf.AppendNum(iBitRate);	
	            }        
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            infoBuf.Append(sampleRate);
            infoBuf.AppendNum(iSampleRate);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            if(iChannels == 1)
            	{
            	infoBuf.Append(channels);
            	infoBuf.Append(KMono);
            	iMainWindowText.AppendL(infoBuf);
            	infoBuf.Zero();
            	}
            else if(iChannels == 2)
            	{
            	infoBuf.Append(channels);
            	infoBuf.Append(KStereo);
            	iMainWindowText.AppendL(infoBuf);
            	infoBuf.Zero();
            	}
            break;
        
        case(EVideoRecording):
            if(iButtons->IsCommandDimmed(EMVSCmdStop))
            	{
            	iButtons->DimCommand(EMVSCmdStop,EFalse);
				iButtons->DrawNow();	
           	 	}
            infoBuf.Append(KVideoRecording);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();	
            if(iVideoRecordAgent->AudioEnabledL())
  					{
  					infoBuf.Zero();
  					infoBuf.Append(AudioEnabled);
  					iMainWindowText.AppendL(infoBuf);
  					infoBuf.Zero();
  					infoBuf.Append(gain);
  					infoBuf.AppendNum(iVolumeOrGain);
  					infoBuf.Append(KPercent);
  					iMainWindowText.AppendL(infoBuf);
  					infoBuf.Zero();
  					}
  				else
  					{
  					infoBuf.Zero();
  					infoBuf.Append(AudioNotEnabled);
  					iMainWindowText.AppendL(infoBuf);
  					infoBuf.Zero();
  					}
  			infoBuf.Append(KFrameSize);
            frameHeight = iFrameSize.iHeight;
            frameWidth = iFrameSize.iWidth;
            infoBuf.AppendNum(frameWidth);
            infoBuf.Append(KComma);
            infoBuf.AppendNum(frameHeight);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            infoBuf.Append(KFrameRate);
            frameRate = TInt(iVideoFrameRate);
            infoBuf.AppendNum(frameRate);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            break;
 
        //The default state displays all playback information such as
        //Volume, balance, cropping points, audio priority & the number of
        //repeats.
    	case(EAudioOpened):
    		if(iMode == EAudioPlay)
    			{
    			infoBuf.Append(volume);	
    			}
    		if(iMode == EAudioRecord)
    			{
    			infoBuf.Append(KGain);
    			}
            infoBuf.AppendNum(iVolumeOrGain);
            infoBuf.Append(KPercent);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            infoBuf.Append(balanceDes);
            balance = (iBalance/2) + 50;
            if(balance < 50)
            	{
                infoBuf.Append(left);
                infoBuf.AppendNum(balance);
                infoBuf.Append(close);
                }
            else if(balance == 50)
                {
                infoBuf.Append(center);
                }
            else
                {
                infoBuf.Append(right);
                infoBuf.AppendNum(balance);
                infoBuf.Append(close);
                }          
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            infoBuf.Append(noRepeats);
            infoBuf.AppendNum(iNoRepeats);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            //Audio Priority
            infoBuf.Append(audioPriority);
            infoBuf.AppendNum(iPriority);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            infoBuf.Append(KDuration);
  		    if(iMode == EAudioPlay)
  		    	{
  		    	duration = iAudioPlayAgent->Duration();	
  		    	}
  		    else if(iMode == EAudioRecord)
  		    	{
  		    	duration = iAudioRecordAgent->Duration();
  		    	if(duration > 0)
	  		    	{
	  		    	iButtons->DimCommand(EMVSCmdPlay,EFalse);	
	  		    	}
  		    	else
  		    		{
  		    		iButtons->DimCommand(EMVSCmdPlay,ETrue);	
  		    		}
  		    	iButtons->DimCommand(EMVSCmdPause,ETrue);
  		    	iButtons->DimCommand(EMVSCmdStop,ETrue);
				iButtons->DrawNow();
  		    	}
  		    clipLengthVal = duration.Int64();
  		    infoBuf.AppendNum(clipLengthVal);
  		    infoBuf.Append(KMs);
  		    iMainWindowText.AppendL(infoBuf);
  		    infoBuf.Zero();
  		    infoBuf.Append(KBitRate);
  		    if(iMode == EAudioPlay)
  		    	{
  		    	iAudioPlayAgent->GetBitRate(bitrateValue);	
  		    	infoBuf.AppendNum(bitrateValue);
  		    	}
  		    else if(iMode == EAudioRecord)
	  		    {
  		    	if(iBitRate == 0)
		  		    {
		  		    infoBuf.Append(KNotSupported);
		  		    }
	  		    else
		  		    {
		  		    infoBuf.AppendNum(iBitRate);	
		  		    }
	  		    }
  		    iMainWindowText.AppendL(infoBuf);
  		    infoBuf.Zero();
            break;
    	
    	case(EAudioStopped):
            infoBuf.Append(KStopped);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
    	    infoBuf.Append(volume);
            infoBuf.AppendNum(iVolumeOrGain);
            infoBuf.Append(KPercent);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            infoBuf.Append(balanceDes);
            balance = (iBalance/2) + 50;
            if(balance < 50)
            	{
                infoBuf.Append(left);
                infoBuf.AppendNum(balance);
                infoBuf.Append(close);
                }
            else if(balance == 50)
                {
                infoBuf.Append(center);
                }
            else
                {
                infoBuf.Append(right);
                infoBuf.AppendNum(balance);
                infoBuf.Append(close);
                }          
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            infoBuf.Append(noRepeats);
            infoBuf.AppendNum(iNoRepeats);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            //Audio Priority
            infoBuf.Append(audioPriority);
            infoBuf.AppendNum(iPriority);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            infoBuf.Append(KDuration);
  		    if(iMode == EAudioPlay)
  		    	{
  		    	duration = iAudioPlayAgent->Duration();	
  		    	}
  		    else if(iMode == EAudioRecord)
  		    	{
  		    	duration = iAudioRecordAgent->Duration();
  		    	}
  		    clipLengthVal = duration.Int64();
  		    infoBuf.AppendNum(clipLengthVal);
  		    infoBuf.Append(KMs);
  		    iMainWindowText.AppendL(infoBuf);
  		    infoBuf.Zero();
  		    infoBuf.Append(KBitRate);
  		    if(iMode == EAudioPlay)
  		    	{
  		    	iAudioPlayAgent->GetBitRate(bitrateValue);	
  		    	infoBuf.AppendNum(bitrateValue);
  		    	}
  		    else if(iMode == EAudioRecord)
	  		    {
  		    	if(iBitRate == 0)
		  		    {
		  		    infoBuf.Append(KNotSupported);
		  		    }
	  		    else
		  		    {
		  		    infoBuf.AppendNum(iBitRate);	
		  		    }
	  		    }
  		    iMainWindowText.AppendL(infoBuf);
  		    infoBuf.Zero();
            break;
            
        case(EAudioPlaying):
            infoBuf.Append(KAudioPlaying);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            infoBuf.Append(volume);
            infoBuf.AppendNum(iVolumeOrGain);
            infoBuf.Append(KPercent);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            infoBuf.Append(balanceDes);
            balance = (iBalance/2) + 50;
            if(balance < 50)
            	{
                infoBuf.Append(left);
                infoBuf.AppendNum(balance);
                infoBuf.Append(close);
                }
            else if(balance == 50)
                {
                infoBuf.Append(center);
                }
            else
                {
                infoBuf.Append(right);
                infoBuf.AppendNum(balance);
                infoBuf.Append(close);
                }          
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            infoBuf.Append(noRepeats);
            infoBuf.AppendNum(iNoRepeats);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            //Audio Priority
            infoBuf.Append(audioPriority);
            infoBuf.AppendNum(iPriority);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            infoBuf.Append(KDuration);
  		    if(iMode == EAudioPlay)
  		    	{
  		    	duration = iAudioPlayAgent->Duration();	
  		    	}
  		    else if(iMode == EAudioRecord)
  		    	{
  		    	duration = iAudioRecordAgent->Duration();
  		    	}
  		    clipLengthVal = duration.Int64();
  		    infoBuf.AppendNum(clipLengthVal);
  		    infoBuf.Append(KMs);
  		    iMainWindowText.AppendL(infoBuf);
  		    infoBuf.Zero();
  		    infoBuf.Append(KBitRate);
  		    if(iMode == EAudioPlay)
  		    	{
  		    	iAudioPlayAgent->GetBitRate(bitrateValue);	
  		    	infoBuf.AppendNum(bitrateValue);
  		    	}
  		    else if(iMode == EAudioRecord)
	  		    {
  		    	if(iBitRate == 0)
		  		    {
		  		    infoBuf.Append(KNotSupported);
		  		    }
	  		    else
		  		    {
		  		    infoBuf.AppendNum(iBitRate);	
		  		    }
	  		    }
  		    iMainWindowText.AppendL(infoBuf);
  		    infoBuf.Zero();
            break;    
    	
    	case(EAudioPaused):
            infoBuf.Append(KPaused);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
    	    infoBuf.Append(volume);
            infoBuf.AppendNum(iVolumeOrGain);
            infoBuf.Append(KPercent);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            infoBuf.Append(balanceDes);
            balance = (iBalance/2) + 50;
            if(balance < 50)
            	{
                infoBuf.Append(left);
                infoBuf.AppendNum(balance);
                infoBuf.Append(close);
                }
            else if(balance == 50)
                {
                infoBuf.Append(center);
                }
            else
                {
                infoBuf.Append(right);
                infoBuf.AppendNum(balance);
                infoBuf.Append(close);
                }          
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            infoBuf.Append(noRepeats);
            infoBuf.AppendNum(iNoRepeats);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            //Audio Priority
            infoBuf.Append(audioPriority);
            infoBuf.AppendNum(iPriority);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            infoBuf.Append(KDuration);
  		    if(iMode == EAudioPlay)
  		    	{
  		    	duration = iAudioPlayAgent->Duration();	
  		    	}
  		    else if(iMode == EAudioRecord)
  		    	{
  		    	duration = iAudioRecordAgent->Duration();
  		    	}
  		    clipLengthVal = duration.Int64();
  		    infoBuf.AppendNum(clipLengthVal);
  		    infoBuf.Append(KMs);
  		    iMainWindowText.AppendL(infoBuf);
  		    infoBuf.Zero();
  		    infoBuf.Append(KBitRate);
  		    if(iMode == EAudioPlay)
  		    	{
  		    	iAudioPlayAgent->GetBitRate(bitrateValue);	
  		    	infoBuf.AppendNum(bitrateValue);
  		    	}
  		    else if(iMode == EAudioRecord)
	  		    {
  		    	if(iBitRate == 0)
		  		    {
		  		    infoBuf.Append(KNotSupported);
		  		    }
	  		    else
		  		    {
		  		    infoBuf.AppendNum(iBitRate);	
		  		    }
	  		    }
  		    iMainWindowText.AppendL(infoBuf);
  		    infoBuf.Zero();
            break;
    	
    	case(EVideoOpened):
  			if(iMode == EVideoPlay)
  				{
  				infoBuf.Append(KVideoOpened);
            	iMainWindowText.AppendL(infoBuf);
            	infoBuf.Zero();
  				if(iVideoPlayAgent->AudioEnabledL())
  					{
  					infoBuf.Zero();
  					infoBuf.Append(AudioEnabled);
  					iMainWindowText.AppendL(infoBuf);
  					infoBuf.Zero();
  					infoBuf.Append(volume);
  					infoBuf.AppendNum(iVolumeOrGain);
  					infoBuf.Append(KPercent);
  					iMainWindowText.AppendL(infoBuf);
  					infoBuf.Zero();
  					if(iButtons->IsCommandDimmed(EMVSCmdSetVolume))
  						{
  						iButtons->DimCommand(EMVSCmdSetVolume,EFalse);
						iButtons->DrawNow();	
  						}
  					}
  				else
  					{
  					infoBuf.Zero();
  					infoBuf.Append(AudioNotEnabled);
  					iMainWindowText.AppendL(infoBuf);
  					infoBuf.Zero();
  					}
  				infoBuf.Append(KDuration);
  		    	duration = iVideoPlayAgent->DurationL();
  		    	clipLengthVal = duration.Int64();
  		    	infoBuf.AppendNum(clipLengthVal);
  		    	iMainWindowText.AppendL(infoBuf);
  		    	infoBuf.Zero();
            	iVideoPlayAgent->VideoFrameSizeL(iFrameSize);
            	frameHeight = iFrameSize.iHeight;
            	frameWidth = iFrameSize.iWidth;
            	infoBuf.Append(KFrameSize);
            	infoBuf.AppendNum(frameWidth);
            	infoBuf.Append(KComma);
            	infoBuf.AppendNum(frameHeight);
            	iMainWindowText.AppendL(infoBuf);
            	infoBuf.Zero();
            	iVideoFrameRate = GetVideoFrameRateL();
            	}
            else if(iMode == EVideoRecord)
            	{
            	iButtons->DimCommand(EMVSCmdPlay,ETrue);
  		    	iButtons->DimCommand(EMVSCmdPause,ETrue);
  		    	iButtons->DimCommand(EMVSCmdStop,ETrue);
				iButtons->DrawNow();
            	infoBuf.Append(KVideoOpenedForRecord);
            	iMainWindowText.AppendL(infoBuf);
            	if(iVideoRecordAgent->AudioEnabledL())
  					{
  					iButtons->DimCommand(EMVSCmdSetVolume, EFalse);
	    			iButtons->DrawNow();
  					infoBuf.Zero();
  					infoBuf.Append(AudioEnabled);
  					iMainWindowText.AppendL(infoBuf);
  					infoBuf.Zero();
  					infoBuf.Append(gain);
  					infoBuf.AppendNum(iVolumeOrGain);
  					infoBuf.Append(KPercent);
  					iMainWindowText.AppendL(infoBuf);
  					infoBuf.Zero();
  					}
  				else
  					{
  					infoBuf.Zero();
  					infoBuf.Append(AudioNotEnabled);
  					iMainWindowText.AppendL(infoBuf);
  					infoBuf.Zero();
  					}
            	infoBuf.Append(KFrameSize);
            	frameHeight =iFrameSize.iHeight;
            	frameWidth =iFrameSize.iWidth;
            	infoBuf.AppendNum(frameWidth);
            	infoBuf.Append(KComma);
            	infoBuf.AppendNum(frameHeight);
            	iMainWindowText.AppendL(infoBuf);
            	infoBuf.Zero();
            	}
            infoBuf.Zero();
            infoBuf.Append(KFrameRate);
            frameRate = TInt(iVideoFrameRate);
            infoBuf.AppendNum(frameRate);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
            break;    	
            	
  		case(EVideoPlaying):
    	    infoBuf.Append(KVideoPlaying);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
    	    if(	iVideoPlayAgent->AudioEnabledL())
  				{
  				infoBuf.Zero();
  				infoBuf.Append(AudioEnabled);
  				iMainWindowText.AppendL(infoBuf);
  				infoBuf.Zero();
  				infoBuf.Append(volume);
  				infoBuf.AppendNum(iVolumeOrGain);
  				infoBuf.Append(KPercent);
  				iMainWindowText.AppendL(infoBuf);
  				infoBuf.Zero();
  				}
  			else
  				{
  				infoBuf.Zero();
  				infoBuf.Append(AudioNotEnabled);
  				iMainWindowText.AppendL(infoBuf);
  				infoBuf.Zero();
  				}
  			infoBuf.Append(KDuration);
  		    duration = iVideoPlayAgent->DurationL();
  		    clipLengthVal = duration.Int64();
  		    infoBuf.AppendNum(clipLengthVal);
  		    iMainWindowText.AppendL(infoBuf);
  		    infoBuf.Zero();
    	    iVideoPlayAgent->VideoFrameSizeL(iFrameSize);
            frameHeight = iFrameSize.iHeight;
            frameWidth = iFrameSize.iWidth;
            infoBuf.Append(KFrameSize);
            infoBuf.AppendNum(frameWidth);
            infoBuf.Append(KComma);
            infoBuf.AppendNum(frameHeight);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
    	    infoBuf.Append(KFrameRate);
    	    iVideoFrameRate = GetVideoFrameRateL();
            frameRate = TInt(iVideoFrameRate);
            infoBuf.AppendNum(frameRate);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
    	    break;
    	
    	case(EVideoPaused):
    	    infoBuf.Append(KPaused);
            iMainWindowText.AppendL(infoBuf);
            if(iMode == EVideoRecord)
            	{
            	if(iVideoRecordAgent->AudioEnabledL())	
            		{
            		infoBuf.Zero();
	  				infoBuf.Append(AudioEnabled);
	  				iMainWindowText.AppendL(infoBuf);
	  				infoBuf.Zero();
            		}
            	else
            		{
            		infoBuf.Zero();
	  				infoBuf.Append(AudioNotEnabled);
	  				iMainWindowText.AppendL(infoBuf);
	  				infoBuf.Zero();
            		}
            	infoBuf.Append(KDuration);
	  		    duration = iVideoRecordAgent->DurationL();
	  		    clipLengthVal = duration.Int64();
	  		    infoBuf.AppendNum(clipLengthVal);
	  		    iMainWindowText.AppendL(infoBuf);
	  		    infoBuf.Zero();
	    	    
            	}
            if(iMode == EVideoPlay)
            	{
	            if(iVideoPlayAgent->AudioEnabledL())
	  				{
	  				infoBuf.Zero();
	  				infoBuf.Append(AudioEnabled);
	  				iMainWindowText.AppendL(infoBuf);
	  				infoBuf.Zero();
	  				infoBuf.Append(volume);
	  				infoBuf.AppendNum(iVolumeOrGain);
	  				infoBuf.Append(KPercent);
	  				iMainWindowText.AppendL(infoBuf);
	  				infoBuf.Zero();
	  				}
	  			else
	  				{
	  				infoBuf.Zero();
	  				infoBuf.Append(AudioNotEnabled);
	  				iMainWindowText.AppendL(infoBuf);
	  				infoBuf.Zero();
	  				}
	  			infoBuf.Append(KDuration);
	  		    duration = iVideoPlayAgent->DurationL();
	  		    clipLengthVal = duration.Int64();
	  		    infoBuf.AppendNum(clipLengthVal);
	  		    iMainWindowText.AppendL(infoBuf);
	  		    infoBuf.Zero();
	    	    iVideoPlayAgent->VideoFrameSizeL(iFrameSize);
	            frameHeight = iFrameSize.iHeight;
	            frameWidth = iFrameSize.iWidth;
	            infoBuf.Append(KFrameSize);
	            infoBuf.AppendNum(frameWidth);
	            infoBuf.Append(KComma);
	            infoBuf.AppendNum(frameHeight);
	            iMainWindowText.AppendL(infoBuf);
	            infoBuf.Zero();
	    	    infoBuf.Append(KFrameRate);
	            iVideoFrameRate = GetVideoFrameRateL();
	            frameRate = TInt(iVideoFrameRate);
	            infoBuf.AppendNum(frameRate);
	            iMainWindowText.AppendL(infoBuf);
	            infoBuf.Zero();
            	}
            break;
    	    
    	case(EVideoStopped):
    	    infoBuf.Append(KStopped);
            iMainWindowText.AppendL(infoBuf);
            if(iMode == EVideoPlay)
  				{
	  			if(iVideoPlayAgent->AudioEnabledL())
	  				{
	  				infoBuf.Zero();
	  				infoBuf.Append(AudioEnabled);
	  				iMainWindowText.AppendL(infoBuf);
	  				infoBuf.Zero();
	  				infoBuf.Append(volume);
	  				infoBuf.AppendNum(iVolumeOrGain);
	  				infoBuf.Append(KPercent);
	  				iMainWindowText.AppendL(infoBuf);
	  				infoBuf.Zero();
	  				}
	  			else
	  				{
	  				infoBuf.Zero();
	  				infoBuf.Append(AudioNotEnabled);
	  				iMainWindowText.AppendL(infoBuf);
	  				infoBuf.Zero();
	  				}	
	  			infoBuf.Append(KDuration);
  		    	duration = iVideoPlayAgent->DurationL();
  		    	clipLengthVal = duration.Int64();
  		    	infoBuf.AppendNum(clipLengthVal);
  		    	iMainWindowText.AppendL(infoBuf);
  		    	infoBuf.Zero();
	  			iVideoPlayAgent->VideoFrameSizeL(iFrameSize);
            	frameHeight = iFrameSize.iHeight;
            	frameWidth = iFrameSize.iWidth;
            	infoBuf.Append(KFrameSize);
            	infoBuf.AppendNum(frameWidth);
            	infoBuf.Append(KComma);
            	infoBuf.AppendNum(frameHeight);
            	iMainWindowText.AppendL(infoBuf);
            	infoBuf.Zero();
	  			iVideoFrameRate = GetVideoFrameRateL();
	  			}
  			else if(iMode == EVideoRecord)
  				{
  				if(iVideoRecordAgent->AudioEnabledL())
	  				{
	  				infoBuf.Zero();
	  				infoBuf.Append(AudioEnabled);
	  				iMainWindowText.AppendL(infoBuf);
	  				infoBuf.Zero();
	  				infoBuf.Append(gain);
	  				infoBuf.AppendNum(iVolumeOrGain);
	  				infoBuf.Append(KPercent);
	  				iMainWindowText.AppendL(infoBuf);
	  				}
	  			else
	  				{
	  				infoBuf.Zero();
	  				infoBuf.Append(AudioNotEnabled);
	  				iMainWindowText.AppendL(infoBuf);
	  				infoBuf.Zero();
	  				}
	  			infoBuf.Zero();	
  				infoBuf.Append(KDuration);
  		    	duration = iVideoRecordAgent->DurationL();
  		    	clipLengthVal = duration.Int64();
  		    	infoBuf.AppendNum(clipLengthVal);
  		    	iMainWindowText.AppendL(infoBuf);
  		    	infoBuf.Zero();
  				infoBuf.Append(KFrameSize);
            	frameHeight =iFrameSize.iHeight;
            	frameWidth =iFrameSize.iWidth;
            	infoBuf.AppendNum(frameWidth);
            	infoBuf.Append(KComma);
            	infoBuf.AppendNum(frameHeight);
            	iMainWindowText.AppendL(infoBuf);
            	infoBuf.Zero();
  				}
  			infoBuf.Zero();
            infoBuf.Append(KFrameRate);
            frameRate = TInt(iVideoFrameRate);
            infoBuf.AppendNum(frameRate);
            iMainWindowText.AppendL(infoBuf);
            infoBuf.Zero();
  			break;
        }
        TInt clipLen = I64INT(clipLengthVal);
        iAppView->SetClipLength(clipLen);
    }

void CMVSAppUi::BuildDollarDesFromArrayL(TDes& aDollarDes)
	{
    //take the CDesCArrayFlat and build a dollar separated descriptor
    TInt counter;
    TBuf<2> dollar;
    for(counter = 0; counter < iMainWindowText.Count(); ++counter)
    	{
        aDollarDes.Append(iMainWindowText[counter]);
        iCoeEnv->ReadResourceL(dollar, R_DOLLARDOLLAR);
        aDollarDes.Append(dollar);
        }
    return;
    }

//
//ConvertTimeToDes(...)
//
// Converts the time to a  descriptor for display purposes. The display is
// in the format HH:MM:SS:SS.
//
void CMVSAppUi::ConvertTimeToDes(TTimeIntervalMicroSeconds aPosition,
                                         TDes& aDes)
	{
    _LIT(KColon, ":");
    _LIT(KZeroDes, "0");
    RArray<TInt> addZero; //array to record whether we need to append a '0'
                          //to each value in the times array.
    RArray<TInt> times;   //The array of times, stored hour, minute, second...
    
    TInt64 milliseconds = I64INT(aPosition.Int64())/1000;
    TInt seconds = milliseconds/1000;
    milliseconds = milliseconds - (seconds * 1000);
    
    TInt hundreds = milliseconds/10; //hundreds of seconds

    TInt minutes = seconds/60;
    seconds = seconds - minutes*60; //seconds

    TInt hours = minutes/60; //hours
    minutes = minutes - hours*60; //minutes

    times.Append(hours);
    times.Append(minutes);
    times.Append(seconds);
    times.Append(hundreds); //fill the array

    TBuf<256> timeDes; //descriptor holding each element of the times array
                     //in turn.

    TInt counter = 0; //counter
    for(counter = 0; counter < 4; ++counter) 
    	{
        timeDes.Zero();
        timeDes.AppendNum(times[counter]);
     
        if(timeDes.Length() == 1)
        	{
            //If the time value is a single digit, we need to add a '0'
            addZero.Append(1);
            }
        else
            {
            //Otherwise, we do not.
            addZero.Append(0);
            }
        }

    //Add zeros where required.
    if(addZero[0])
    	aDes.Append(KZeroDes);
    aDes.AppendNum(hours);
    aDes.Append(KColon);
    if(addZero[1])
    	aDes.Append(KZeroDes);
    aDes.AppendNum(minutes);
    aDes.Append(KColon);
    if(addZero[2])
    	aDes.Append(KZeroDes);
    aDes.AppendNum(seconds);
    aDes.Append(KColon);
    if(addZero[3])
    	aDes.Append(KZeroDes);
    aDes.AppendNum(hundreds);

    //Close the RArrays and return
    addZero.Close();
    times.Close();
    return;
    }

//
//UpdatePositionViewL()
//
// Updates the view if the state is recording or playing. Otherwise, this is
// unnecessary.
//
void CMVSAppUi::UpdatePositionViewL()
	{
	TBuf<64> dispStr1;
	TBuf<256> temp;
	TInt64 currentposition = 0;
    if(iState == EAudioRecording || iState == EAudioPlaying || iState == EVideoRecording 
       || iState == EVideoPlaying || iState == EAudioPaused || iState == EVideoPaused 
       || iState == EVideoStopped || iState == EAudioStopped)
    	{
        iDisplayTime.Zero();
        if(iMode==EAudioRecord)
        	{
        	TBuf<64> dispStr2;
        	TBuf<32> recTimeLeft;
        	TTimeIntervalMicroSeconds timeLeft = iAudioRecordAgent->RecordTimeAvailable();
        	if(iRecordLimitSet && iState == EAudioRecording)
        		{
        		ConvertTimeToDes(timeLeft, recTimeLeft);
	        	dispStr2.Copy(_L("Time Left:"));
	        	dispStr2.Append(recTimeLeft);
	        	iAppView->iStatusWindowTimeControl->SetText2L(dispStr2);
	        	dispStr1.Copy(_L("Rec. Time:"));	
        		}
        	}
        if(iMode==EAudioRecord && iState==EAudioRecording)
	    	{
	        iClipLength = iAudioRecordAgent->Duration();
	        iTimeElapsed = iAudioRecordAgent->Position();
	        TInt64 duration = iClipLength.Int64();
	        TInt64 position = iTimeElapsed.Int64();
	        currentposition = duration + position;
	        iTimeElapsed = TTimeIntervalMicroSeconds(currentposition);
	        }
	    else if(iMode==EAudioRecord && iState==EAudioPlaying)
	    	{
	    	iTimeElapsed=iAudioRecordAgent->Position();
	    	iAppView->UpdatePlayProgress(iTimeElapsed);
	    	}
		else if(iMode==EAudioPlay)
			{
		   	iAudioPlayAgent->GetPosition(iTimeElapsed);	
		   	iAppView->UpdatePlayProgress(iTimeElapsed);
		  	}
		else if(iMode==EVideoPlay)
			{
			iTimeElapsed = iVideoPlayAgent->PositionL();
			}
		else if(iMode==EVideoRecord)
			{
			iTimeElapsed = iVideoRecordAgent->DurationL();
			}
		temp.AppendNum(currentposition);
		
        ConvertTimeToDes(iTimeElapsed, iDisplayTime);
        dispStr1.Append(iDisplayTime);
        iAppView->iStatusWindowTimeControl->SetTextL(dispStr1);
        iAppView->iStatusWindowTimeControl->DrawNow();
        }
    }


void CMVSAppUi::SetMaxFileSize(TInt& aVal)
	{
	if(aVal>0)
		{
		iRecordLimitSet = ETrue;
		iAudioRecordAgent->SetMaxWriteLength(aVal);
		}
	else
		{
		iRecordLimitSet = EFalse;
		iAudioRecordAgent->SetMaxWriteLength(aVal);
		TRAP_IGNORE(iAppView->iStatusWindowTimeControl->ResetText2L());
		}
	}

//Handling the errors and logging it to the 
//log file
void CMVSAppUi::LogErrors(TInt aError)
	{
	TBuf<KErrorTextLength> erorrDes;
	CEikonEnv::Static()->GetErrorText(erorrDes,aError);
	CEikonEnv::Static()->AlertWin(erorrDes);
	iLogger.Write(erorrDes);	 
	}
	

void CMVSAppUi::DetermineFinalFNameL()
     {
     //check whether the extension and the controller 
     //has been selected or not.If not selected proceed
     //to the next dialog, nothing to be appended here to 
     //the filename.Else append the selected extension to
     //the filename.
     if(iSelectedPluginIdx == KControllerNotSelected)
     	{
     	iProceed = ETrue;
     	}
     TFileName givenExt;
     TParsePtr parser(iDestFilename);
     TPtrC extensionval = parser.Ext();
     if(extensionval.Length() == 0)
     	{
        iDestFilename.Append(iControllerExt);
        iProceed = ETrue;
        return;
        }
     else
     	{
        //If the extension is too long, report an error.
        if(extensionval.Length() > 4)
        	{
            _LIT(KExtTooLong, "Extension TOO long");
            User::InfoPrint(KExtTooLong);
            iProceed = EFalse;
            }

        //Otherwise record the extension given in the filename
        if(extensionval.CompareF(iControllerExt)==0)
        	{
            iProceed = ETrue;
            return;
            }

        //Otherwise, append the right extension to the filename and return.
        else
          	{
            _LIT(KWrongExt, "Wrong Extension");
            _LIT(KWrongExtBody, "Extension mismatch,Please re-enter");
            CEikonEnv::Static()->InfoWinL(KWrongExt, KWrongExtBody);
            SaveAsDialogL();
            iProceed = EFalse;
            }
        }
     }

TPoint CMVSAppUi::DrawBorders(const TSize& aSize) const
	{
	CWindowGc& gc = iAppView->iMainWindowControl->SystemGc();
	gc.Activate(iAppView->iMainWindowControl->ControlWindow());
	TRect rect(iAppView->iMainWindowControl->Size());
	TPoint pos( rect.iTl.iX + rect.Width()/2 - aSize.iWidth/2,
				rect.iTl.iY + rect.Height()/2 - aSize.iHeight/2);
	TRect drawRect(pos,aSize);
	gc.SetPenColor(KRgbBlack);
	drawRect.Grow(1,1);
	gc.DrawRect(drawRect);
	gc.Deactivate();
	iAppView->iMainWindowControl->ControlEnv()->WsSession().Flush();
	return pos;
	}
	
void CMVSAppUi::StopViewFinder()
	{
	if(!iVideoRecordAgent)
		{
		return;			
		}
	CCamera *camera = iVideoRecordAgent->Camera();
	if (camera->ViewFinderActive())
		{
		camera->StopViewFinder();
		iAppView->iMainWindowControl->DrawNow();
		}
	}
