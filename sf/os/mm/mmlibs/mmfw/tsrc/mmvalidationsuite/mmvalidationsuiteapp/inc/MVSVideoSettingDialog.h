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

#ifndef CMVSVIDEOSETTINGDIALOG_H
#define CMVSVIDEOSETTINGDIALOG_H

#include <techview/eikdialg.h>
#include <techview/eikchkbx.h>
#include "MVSAppUI.h"

class CMVSAppUi;

class CMVSVideoSettingDialog : public CEikDialog
    {
public:
    //constructs and launches dialog
    static void SetupDialogLD(TVideoRotation aRotation,
                              CMVSAppUi* aAppUi,
                              const TRect& aVideoSize,
                              const TRect& aWindowRect,
                              TBool aSupportVideoPlayerUtility2,
                              TBool aPip,
							  TBool aCrp); 

    void PreLayoutDynInitL();    //seeds dialog controls
    void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);

private:
    //standard C++ constructor
    CMVSVideoSettingDialog(TVideoRotation aRotation,
                           CMVSAppUi* aAppUi,
                           const TRect& aVideoSize,
                           const TRect& aWindowRect,
                           TBool aSupportVideoPlayerUtility2,
                           TBool aPip,
						   TBool aCrp); 

    TBool OkToExitL(TInt aButtonId); //handles button presses for this dialog
    void HandleUpdateL();

    void RetrieveVideoExtentFieldValues(TRect& aVideoExtentValues) const;
    void RetrieveWindowClipRectFieldValues(TRect& aNewVideoExtent) const;


private:
    CEikCheckBox* iAntiAliasFilteringCtl;
    TVideoRotation iRotation;
    CMVSAppUi* iAppUi;
    TReal32 iScaleWidth;
    TReal32 iScaleHeight;
    TBool iAntiAliasFiltering;
    TInt iAx;
    TInt iAy;
    TInt iBx;
    TInt iBy;
    TRect iCropRegion;
    TRect iVideoExtent;
    TRect iWindowClip;
    TBool iSupportVideoPlayerUtility2;
    TBool iPip;
	TBool iCrp;
    };

#endif CMVSVIDEOSETTINGDIALOG_H

