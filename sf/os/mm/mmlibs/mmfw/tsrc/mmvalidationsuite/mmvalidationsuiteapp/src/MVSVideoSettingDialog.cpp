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

#include <e32math.h>
#include <techview/eikfpne.h>

#include "MVSVideoSettingDialog.h"
#include "MVSApp.hrh"

void CMVSVideoSettingDialog::SetupDialogLD(TVideoRotation aRotation, 
                                           CMVSAppUi* aAppUi,
                                           const TRect& aVideoSize,
                                           const TRect& aWindowRect,
                                           TBool aSupportVideoPlayerUtility2,
                                           TBool aPip,
										   TBool aCrp)
	{
    CMVSVideoSettingDialog* dialog = new (ELeave) CMVSVideoSettingDialog(aRotation,
                                                                         aAppUi,
                                                                         aVideoSize,
                                                                         aWindowRect,
                                                                         aSupportVideoPlayerUtility2,
                                                                         aPip, 
																		 aCrp);
    dialog->ExecuteLD(R_MVS_VIDEO_DIALOG_VIDEOSETTING);
    }


void CMVSVideoSettingDialog::PreLayoutDynInitL()
    {
    // Get a downcasted pointer to the controls
    CEikChoiceList* rotation = static_cast<CEikChoiceList*>(Control(EMVSVideoRotationSettings));    
    if(rotation)
	    {
	    rotation->SetCurrentItem(iRotation);
	    }
	        
    // default the scaling factor to the current scaling factor as used by the video playback
    // utility    	
    iAppUi->GetScaleFactorL(iScaleWidth, iScaleHeight, iAntiAliasFiltering);
    
    CEikCheckBox* antiAliasFiltering = static_cast<CEikCheckBox*>(Control(EMVSAntiAliasFiltering));
    if(antiAliasFiltering)
    	{
    	iAntiAliasFilteringCtl = antiAliasFiltering;
    	antiAliasFiltering->SetObserver(this);
    	antiAliasFiltering->SetState(
    	    iAntiAliasFiltering ? CEikButtonBase::ESet : CEikButtonBase::EClear);            
    	}
        
    CEikFloatingPointEditor* editor = static_cast<CEikFloatingPointEditor*> (Control(EMVSCmdScaleWidth));
    TReal temp(iScaleWidth);
    editor->SetValueL(&temp);
    
    temp = iScaleHeight;
    editor = static_cast<CEikFloatingPointEditor*> (Control(EMVSCmdScaleHeight));
    editor->SetValueL(&temp);
    	
    // default the crop region to the current crop region values as used by the video playback
    // utility
    iAppUi->GetCropRegionL(iCropRegion);
    
    static_cast<CEikNumberEditor*>
        (Control(EMVSCmdSetTopLeftx))->SetNumber(iCropRegion.iTl.iX);
    static_cast<CEikNumberEditor*>
        (Control(EMVSCmdSetTopLefty))->SetNumber(iCropRegion.iTl.iY);
    static_cast<CEikNumberEditor*>
        (Control(EMVSCmdSetBottomRightx))->SetNumber(iCropRegion.iBr.iX);
    static_cast<CEikNumberEditor*>
        (Control(EMVSCmdSetBottomRighty))->SetNumber(iCropRegion.iBr.iY);    	
        
    if (!iSupportVideoPlayerUtility2)
        {
        // hide the video extent & window clipping rect UI elements if CVideoPlayerUtility2 
        // is not in effect
        for (TInt i = EMVSCmdSetVideoExtentTopLeftx; i <= EMVSCmdSetOverlayText; i++)
            {
            SetLineDimmedNow(i, ETrue);
            SetLineNonFocusing(i);
            }
        }
    else
        {
        // default the video extent to values currently set in the application
        static_cast<CEikNumberEditor*>
            (Control(EMVSCmdSetVideoExtentTopLeftx))->SetNumber(iVideoExtent.iTl.iX);
        static_cast<CEikNumberEditor*>
            (Control(EMVSCmdSetVideoExtentTopLefty))->SetNumber(iVideoExtent.iTl.iY);
        static_cast<CEikNumberEditor*>
            (Control(EMVSCmdSetVideoExtentBottomRightx))->SetNumber(iVideoExtent.iBr.iX);
        static_cast<CEikNumberEditor*>
            (Control(EMVSCmdSetVideoExtentBottomRighty))->SetNumber(iVideoExtent.iBr.iY);

        // default the window clipping rect to values currently set in the application
        static_cast<CEikNumberEditor*>
            (Control(EMVSCmdSetWindowClipTopLeftx))->SetNumber(iWindowClip.iTl.iX);
        static_cast<CEikNumberEditor*>
            (Control(EMVSCmdSetWindowClipTopLefty))->SetNumber(iWindowClip.iTl.iY);
        static_cast<CEikNumberEditor*>
            (Control(EMVSCmdSetWindowClipBottomRightx))->SetNumber(iWindowClip.iBr.iX);
        static_cast<CEikNumberEditor*>
            (Control(EMVSCmdSetWindowClipBottomRighty))->SetNumber(iWindowClip.iBr.iY);
            
        if (iAppUi->OverlayText().Length() > 0)
            {
            static_cast<CEikCheckBox*>
                (Control(EMVSCmdSetDisplayOverlayText))->SetState(CEikButtonBase::ESet);            
            static_cast<CEikEdwin*>
                (Control(EMVSCmdSetOverlayText))->SetTextL(&iAppUi->OverlayText());                
            }
        else
            {
            static_cast<CEikCheckBox*>
                (Control(EMVSCmdSetDisplayOverlayText))->SetState(CEikButtonBase::EClear);            
            static_cast<CEikEdwin*>
                (Control(EMVSCmdSetOverlayText))->SetTextL(NULL);                                
            SetLineDimmedNow(EMVSCmdSetOverlayText, ETrue);                            
            }
        }

#ifdef SYMBIAN_BUILD_GCE    
    TAutoScaleType autoScaleType;
    TInt autoScaleHorizPos;
    TInt autoScaleVertPos;
    iAppUi->GetAutoScale(autoScaleType, autoScaleHorizPos, autoScaleVertPos);
    
	CEikChoiceList* autoScaleTypeList = static_cast<CEikChoiceList*>(Control(EMVSAutoScaleType));    
    switch (autoScaleType)
    	{
        case EAutoScaleNone:
        	autoScaleTypeList->SetCurrentItem(0);
          	break;
        case EAutoScaleBestFit:
        	autoScaleTypeList->SetCurrentItem(1);
          	break;
        case EAutoScaleClip:
        	autoScaleTypeList->SetCurrentItem(2);
         	break;
    	case EAutoScaleStretch:
    		autoScaleTypeList->SetCurrentItem(3);
    		break;
        }	
	    
	CEikChoiceList* autoScaleHoriz = static_cast<CEikChoiceList*>(Control(EMVSAutoScaleHoriz));
	switch (autoScaleHorizPos)
		{
		case EHorizontalAlignLeft:
			autoScaleHoriz->SetCurrentItem(0);
			static_cast<CEikNumberEditor*>(Control(EMVSAutoScaleHorizPos))->SetNumber(0);
			break;
		case EHorizontalAlignCenter:
			autoScaleHoriz->SetCurrentItem(1);
			static_cast<CEikNumberEditor*>(Control(EMVSAutoScaleHorizPos))->SetNumber(0);
			break;
		case EHorizontalAlignRight:
			autoScaleHoriz->SetCurrentItem(2);
			static_cast<CEikNumberEditor*>(Control(EMVSAutoScaleHorizPos))->SetNumber(0);
			break;
		default: 
			autoScaleHoriz->SetCurrentItem(3);
			static_cast<CEikNumberEditor*>(Control(EMVSAutoScaleHorizPos))->SetNumber(autoScaleHorizPos);
			break;
		}
	
	CEikChoiceList* autoScaleVert = static_cast<CEikChoiceList*>(Control(EMVSAutoScaleVert));
	switch (autoScaleVertPos)
		{
		case EVerticalAlignTop:
			autoScaleVert->SetCurrentItem(0);
			static_cast<CEikNumberEditor*>(Control(EMVSAutoScaleVertPos))->SetNumber(0);
			break;
		case EVerticalAlignCenter:
			autoScaleVert->SetCurrentItem(1);
			static_cast<CEikNumberEditor*>(Control(EMVSAutoScaleVertPos))->SetNumber(0);
			break;
		case EVerticalAlignBottom:
			autoScaleVert->SetCurrentItem(2);
			static_cast<CEikNumberEditor*>(Control(EMVSAutoScaleVertPos))->SetNumber(0);
			break;
		default: 
			autoScaleVert->SetCurrentItem(3);
			static_cast<CEikNumberEditor*>(Control(EMVSAutoScaleVertPos))->SetNumber(autoScaleVertPos);
			break;
		}
	
	CEikCheckBox* pip = static_cast<CEikCheckBox*>(Control(EMVSPIP));
	
	if(pip)
    	{
    	pip->SetObserver(this);
    	pip->SetState(iPip ? CEikButtonBase::ESet : CEikButtonBase::EClear);            
    	}

	CEikCheckBox* crp = static_cast<CEikCheckBox*>(Control(EMVSCRP));

	if(crp)
		{
		crp->SetObserver(this);
		crp->SetState(iCrp ? CEikButtonBase::ESet : CEikButtonBase::EClear);
		}
	
#endif // SYMBIAN_BUILD_GCE
    }


CMVSVideoSettingDialog::CMVSVideoSettingDialog(TVideoRotation aRotation,
                                               CMVSAppUi* aAppUi,
                                               const TRect& aVideoSize,
                                               const TRect& aWindowRect,
                                               TBool aSupportVideoPlayerUtility2,
                                               TBool aPip,
											   TBool aCrp)
    : iRotation(aRotation),
      iAppUi(aAppUi),
      iVideoExtent(aVideoSize),
      iWindowClip(aWindowRect),
      iSupportVideoPlayerUtility2(aSupportVideoPlayerUtility2),
      iPip(aPip),
	  iCrp(aCrp)
    {
    //Nothing to do here - all done in initialisation list
    }


TBool CMVSVideoSettingDialog::OkToExitL(TInt aButtonId)
    {
    switch(aButtonId)
    	{
        case EMVSButtonUpdate:
            {          
            HandleUpdateL();
            break;
            }
            
        case EMVSButtonCancel:
        default: //cancel
            break;
        }
        
    return ETrue;                    
    }                        

void CMVSVideoSettingDialog::HandleUpdateL()
    {
    // retrieve crop region value
    iAx = static_cast<CEikNumberEditor*>
                        (Control(EMVSCmdSetTopLeftx))->Number();
    
    iAy = static_cast<CEikNumberEditor*>
                        (Control(EMVSCmdSetTopLefty))->Number();
    
    iBx = static_cast<CEikNumberEditor*>
                        (Control(EMVSCmdSetBottomRightx))->Number();
    
    iBy = static_cast<CEikNumberEditor*>
                        (Control(EMVSCmdSetBottomRighty))->Number();    
    TRect newCrop(iAx, iAy, iBx, iBy);   
    if (newCrop != iCropRegion)
        {
        iAppUi->SetCropRegionL(newCrop);
        }
    
    // retrieve scale factor
    TReal32 scaleWidth = TReal32(static_cast<CEikFloatingPointEditor*>
                        (Control(EMVSCmdScaleWidth))->Value());    
    TReal32 scaleHeight = TReal32(static_cast<CEikFloatingPointEditor*>
                        (Control(EMVSCmdScaleHeight))->Value());                        
    TBool antiAlias = (iAntiAliasFilteringCtl->State() == CEikButtonBase::ESet);

    if (iScaleWidth != scaleWidth || iScaleHeight != scaleHeight || antiAlias != iAntiAliasFiltering)
        {            
        iAppUi->SetScaleFactorL(scaleWidth, scaleHeight, antiAlias);
        }
            
    // retrieve rotation value
    TVideoRotation newRotation(EVideoRotationNone);
    CEikChoiceList* rotation = static_cast<CEikChoiceList*>(Control(EMVSVideoRotationSettings));    
    if(rotation)
	    {
	    switch (rotation->CurrentItem())
	    	{
	        case 0:
	        	newRotation = EVideoRotationNone;
	          	break;

	        case 1:
	        	newRotation = EVideoRotationClockwise90;
	          	break;

	        case 2:
	        	newRotation = EVideoRotationClockwise180;
	         	break;
	         	
	        case 3:
	        	newRotation = EVideoRotationClockwise270;
	         	break;

	        default:
	        	newRotation = EVideoRotationNone;
	         	break;
	        }	
	    }
    if (iRotation != newRotation)
        {            
        iAppUi->SetRotationL(newRotation);
        iRotation = newRotation;
        }
            
#ifdef SYMBIAN_BUILD_GCE
    // retrieves window clipping rect
    TRect newWinClipRect;
    RetrieveWindowClipRectFieldValues(newWinClipRect);
    
    if (newWinClipRect != iWindowClip)
        {
        iAppUi->SetWindowClippingRect(newWinClipRect);
        }


    // retrieves video extent properties
    TRect newVideoExtent;
    RetrieveVideoExtentFieldValues(newVideoExtent);

    if (newVideoExtent != iVideoExtent)
        {
        iAppUi->SetVideoExtent(newVideoExtent);
        }

    // retrieve overlay text
    HBufC* overlayText = NULL;                  
    if (static_cast<CEikCheckBox*>(Control(EMVSCmdSetDisplayOverlayText))->State() 
        == CEikButtonBase::ESet)
        {
        overlayText 
            = static_cast<CEikEdwin*>(Control(EMVSCmdSetOverlayText))->GetTextInHBufL();
        }
     
    if (overlayText)
        {
        iAppUi->SetOverlayTextL(*overlayText);
        delete overlayText;                                
        }
    else
        {
        iAppUi->SetOverlayTextL(KNullDesC);
        }

	// retrieve autoscale values
    TAutoScaleType newAutoScaleType = EAutoScaleNone;
    TInt newAutoScaleHorizPos = 0;
    TInt newAutoScaleVertPos = 0;
    
    CEikChoiceList* autoScaleType = static_cast<CEikChoiceList*>(Control(EMVSAutoScaleType));
    
    if (autoScaleType)
	    {
	    switch (autoScaleType->CurrentItem())
	    	{
	        case 0:
	        	newAutoScaleType = EAutoScaleNone;
	          	break;
	        case 1:
	        	newAutoScaleType = EAutoScaleBestFit;
	          	break;
	        case 2:
	        	newAutoScaleType = EAutoScaleClip;
	         	break;
	    	case 3:
	    		newAutoScaleType = EAutoScaleStretch;
	    		break;
	    	default:
	    		newAutoScaleType = EAutoScaleNone;
	    		break;
	        }	
	    }
	
	CEikChoiceList* autoScaleHoriz = static_cast<CEikChoiceList*>(Control(EMVSAutoScaleHoriz));
	switch (autoScaleHoriz->CurrentItem())
		{
		case 0:
			newAutoScaleHorizPos = EHorizontalAlignLeft;
			break;
		case 1:
			newAutoScaleHorizPos = EHorizontalAlignCenter;
			break;
		case 2:
			newAutoScaleHorizPos = EHorizontalAlignRight;
			break;
		default:
			newAutoScaleHorizPos = static_cast<CEikNumberEditor*>(Control(EMVSAutoScaleHorizPos))->Number();
			break;
		}
	
	CEikChoiceList* autoScaleVert = static_cast<CEikChoiceList*>(Control(EMVSAutoScaleVert));
	switch (autoScaleVert->CurrentItem())
		{
		case 0:
			newAutoScaleVertPos = EVerticalAlignTop;
			break;
		case 1:
			newAutoScaleVertPos = EVerticalAlignCenter;
			break;
		case 2:
			newAutoScaleVertPos = EVerticalAlignBottom;
			break;
		default:
			newAutoScaleVertPos = static_cast<CEikNumberEditor*>(Control(EMVSAutoScaleVertPos))->Number();
			break;
		}

	iAppUi->SetAutoScaleL(newAutoScaleType, newAutoScaleHorizPos, newAutoScaleVertPos);
	
	CEikButtonBase::TState state = static_cast<CEikCheckBox*>(Control(EMVSPIP))->State();
	iAppUi->SetPip(CEikButtonBase::ESet == state);

	state = static_cast<CEikCheckBox*>(Control(EMVSCRP))->State();
	iAppUi->SetCrp(CEikButtonBase::ESet == state);	
#endif  // SYMBIAN_BUILD_GCE
    }
            

void CMVSVideoSettingDialog::RetrieveVideoExtentFieldValues(TRect& aVideoExtentValues) const
    {
    aVideoExtentValues.iTl.iX 
        = static_cast<CEikNumberEditor*>(Control(EMVSCmdSetVideoExtentTopLeftx))->Number();        

    aVideoExtentValues.iTl.iY 
        = static_cast<CEikNumberEditor*>(Control(EMVSCmdSetVideoExtentTopLefty))->Number();        

    aVideoExtentValues.iBr.iX 
        = static_cast<CEikNumberEditor*>(Control(EMVSCmdSetVideoExtentBottomRightx))->Number();        

    aVideoExtentValues.iBr.iY 
        = static_cast<CEikNumberEditor*>(Control(EMVSCmdSetVideoExtentBottomRighty))->Number();                
    }

void CMVSVideoSettingDialog::RetrieveWindowClipRectFieldValues(TRect& aNewVideoExtent) const
    {
    aNewVideoExtent.iTl.iX 
        = static_cast<CEikNumberEditor*>(Control(EMVSCmdSetWindowClipTopLeftx))->Number();        

    aNewVideoExtent.iTl.iY 
        = static_cast<CEikNumberEditor*>(Control(EMVSCmdSetWindowClipTopLefty))->Number();        

    aNewVideoExtent.iBr.iX 
        = static_cast<CEikNumberEditor*>(Control(EMVSCmdSetWindowClipBottomRightx))->Number();        

    aNewVideoExtent.iBr.iY 
        = static_cast<CEikNumberEditor*>(Control(EMVSCmdSetWindowClipBottomRighty))->Number();                
    }

void CMVSVideoSettingDialog::HandleControlEventL(CCoeControl *aControl, TCoeEvent aEventType)
	{
	CEikDialog::HandleControlEventL(aControl,aEventType);
    if (aControl == Control(EMVSCmdSetDisplayOverlayText) && aEventType == EEventStateChanged)
        {
        CEikCheckBox* overlayCheckBox 
            = static_cast<CEikCheckBox*>(Control(EMVSCmdSetDisplayOverlayText));
            
        TBool disableOverlayText = (overlayCheckBox->State() != CEikButtonBase::ESet);                    
        SetLineDimmedNow(EMVSCmdSetOverlayText, disableOverlayText);                                    
        }
	}
