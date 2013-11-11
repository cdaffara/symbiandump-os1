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

#ifndef MVSCCONFIGVIDEOFORMATDIALOG_H
#define MVSCCONFIGVIDEOFORMATDIALOG_H

#include <techview/eikdialg.h>
#include <mda/common/controller.h>
#include <mmf/common/mmfutilities.h>
#include <techview/eikchkbx.h>
#include "MVSApp.h"

class CSettingsManager;
class CMVSAppUi;
class CMVSConfigVideoFormatDialog : public CEikDialog
	{
public:
    ~CMVSConfigVideoFormatDialog();
    void SetupDialogLD(CMVSAppUi* aAppUi,TUid aControllerId,CSettingsManager* aSettingsManager);
    void PreLayoutDynInitL();    //seeds dialog controls
    void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream);
	void HandleControlEventL(CCoeControl *aControl, TCoeEvent aEventType);
	void ConstructL();	
private:
    CMVSConfigVideoFormatDialog(CMVSAppUi* aAppUi , TUid aControllerId);
    TBool OkToExitL(TInt aButtonId); //handles button presses for this dialog
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    CEikCheckBox* iSettings;
    CEikCheckBox* iAudioEnableCtl;
private:
	TUid iContollerUid;
	CMVSAppUi* iAppUi;
	TBool iRestoreSettings;
	TInt8 iFrameRateIdx;
    TInt8 iFrameSizeIdx;
    TInt8 iMimeTypeIdx;
    CDesCArrayFlat* iArrFrameRate; 
    CDesCArrayFlat* iArrFrameSize;
    CDesCArrayFlat* iArrVideoMimeType;
    CDesC8ArrayFlat* iMimeTypeArray;
   
public:
	RArray<TReal32> iFrameRateArray;
    RArray<TSize> iFrameSizeArray;
    TBool iAudioEnable;
	};

#endif MVSCCONFIGVIDEOFORMATDIALOG_H
