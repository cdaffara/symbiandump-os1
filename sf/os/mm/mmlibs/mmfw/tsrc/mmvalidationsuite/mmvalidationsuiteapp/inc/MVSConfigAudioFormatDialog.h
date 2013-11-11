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

#ifndef MVSCONFIGAUDIOFORMATDIALOG_H
#define MVSCONFIGAUDIOFORMATDIALOG_H

#include <techview/eikdialg.h>
#include <techview/eikchkbx.h>
#include <mmf/common/mmfutilities.h>


class CMVSAppUi;
class CSettingsManager;
class CMVSConfigAudioFormatDialog : public CEikDialog
	{
public:
    ~CMVSConfigAudioFormatDialog();
    void SetupDialogLD(CMVSAppUi* aAppUi,TUid aControllerId,CSettingsManager* aSettingsManager); 
    void PreLayoutDynInitL();    //seeds dialog controls
    void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream);
	void ConstructL();
	void HandleControlEventL(CCoeControl *aControl, TCoeEvent aEventType);
private:
    CMVSConfigAudioFormatDialog(CMVSAppUi* aAppUi,TUid aControllerId);
    TBool OkToExitL(TInt aButtonId); //handles button presses for this dialog
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
private:
    TUid iContollerUid;
    TInt8 iBitRateIdx;
    TInt8 iSampleRateIdx;
    TInt8 iChannelListIdx;
    TInt8 iFormatListIdx;
    TInt8 iCodecListIdx;
    CMVSAppUi* iAppUi;
    CDesCArrayFlat* iArrSampleRates; 
    CDesCArrayFlat* iArrBitRates; 
    CDesCArrayFlat* iArrChannels; 
    CDesCArrayFlat* iArrCodecs;
    CEikCheckBox* iSettings;
public:
	RArray<TUint> iBitRatesArray;
    RArray<TUint> iSampleRatesArray;
    RArray<TUint> iChannelsArray;
    RArray<TFourCC> iCodecsArray;
    TBool iRestoreSettings; 
    };

#endif MVSCONFIGAUDIOFORMATDIALOG_H
