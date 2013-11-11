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
//

#ifndef TESTSTEPOGGCTRLPLAY_H
#define TESTSTEPOGGCTRLPLAY_H

#include "tsu_mmf_oggctrl_teststep.h"

/**
 *
 * This class tests playing an ogg file completely and checks for playcompletion
 * event with KErrNone from the oggplaycontroller.
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlPlay : public RTestStepOggCtrlPlayBase
	{
public:
	RTestStepOggCtrlPlay(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};

/**
 *
 * This class tests setting the volume on OggPlayController in Stopped, Primed and Playing 
 * states and checks if the volume set is same as the volume got.
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlVolume : public RTestStepOggCtrlPlayBase
	{
public:
	RTestStepOggCtrlVolume(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};

/**
 *
 * This class tests setting the PlayBalance on OggPlayController in Stopped, Primed and Playing 
 * states and checks if the PlayBalance set is same as the PlayBalance got.
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlPlayBalance : public RTestStepOggCtrlPlayBase
	{
public:
	RTestStepOggCtrlPlayBalance(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};

/**
 *
 * This class tests setting the Position on OggPlayController in Stopped, Primed and Playing 
 * states and checks if the Position set is same as the Position got.
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlPosition : public RTestStepOggCtrlPlayBase
	{
public:
	RTestStepOggCtrlPosition(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};

/**
 *
 * This class tests pause functionality of OggPlayController in Stopped, Primed and Playing 
 * states and checks if the play starts from the current position during playign state
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlPause : public RTestStepOggCtrlPlayBase
	{
public:
	RTestStepOggCtrlPause(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};

/**
 *
 * This class tests stop functionality of OggPlayController in Stopped, Primed and Playing 
 * states and checks if the play starts from the initial position during playing state
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlStop : public RTestStepOggCtrlPlayBase
	{
public:
	RTestStepOggCtrlStop(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};

/**
 *
 * This class tests metadata support of the OggPlayController. 
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlMetaData : public RTestStepOggCtrlPlayBase
	{
public:
	RTestStepOggCtrlMetaData(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};

/**
 *
 * This class tests PlayWindow support of the OggPlayController.
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlPlayWindow : public RTestStepOggCtrlPlayBase
	{
public:
	RTestStepOggCtrlPlayWindow(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};

/**
 *
 * This class tests Getting the audio configuratin of the file using OggPlayController.
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlGetSetConfig : public RTestStepOggCtrlPlayBase
	{
public:
	RTestStepOggCtrlGetSetConfig(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};

/**
 *
 * This class tests Getting the duration of the file using OggPlayController.
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlDuration : public RTestStepOggCtrlPlayBase
	{
public:
	RTestStepOggCtrlDuration(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};

//Negative Tests
/**
 *
 * This class tests opening an invalid file using OggPlayController.
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlPlayInvalid : public RTestStepOggCtrlPlayBase
	{
public:
	RTestStepOggCtrlPlayInvalid(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TVerdict  DoTestStepL();
	};

/**
 *
 * This class tests setting an invalid sourcedatatype on OggPlayController.
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlSetInvalidConfig : public RTestStepOggCtrlPlayBase
	{
public:
	RTestStepOggCtrlSetInvalidConfig(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};

/**
 *
 * This class tests opening an invalid file using OggPlayController.
 *
 * @lib "TSU_MMF_OGGCTRL_01.lib"
 *
 */
class RTestStepOggCtrlPositionNegative : public RTestStepOggCtrlPlayBase
	{
public:
	RTestStepOggCtrlPositionNegative(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL();
	};
#endif //(TESTSTEPOGGCTRLPLAY_H)
