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

#ifndef TESTVIDEOPLAYER_SUBTITLES_H
#define TESTVIDEOPLAYER_SUBTITLES_H

#include "tsi_mmf_vclntavi_stepbase.h"
#include "u32hal.h"

#define LOG_CALL(f) INFO_PRINTF1(_L(#f)); f

class RTestVclnt2Subtitle;

class CPlayTimer : public CTimer
	{
public:
	static CPlayTimer* NewL(RTestVclnt2Subtitle& aTestStep);
	
protected:
	void RunL();
	TInt RunError(TInt aError);
	
private:
	CPlayTimer(RTestVclnt2Subtitle& aTestStep);
	
private:
	RTestVclnt2Subtitle& iTestStep;
	};

/**
 * MM-MMF-VCLNTAVI-I-0600-CP
 * MM-MMF-VCLNTAVI-I-0700-CP
 */
class RTestVclnt2Subtitle : public RTestVclnt2AviPlayerStep
	{
public:
	static RTestVclnt2Subtitle* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	
	virtual void DoBeforePlaybackActionsL();
	virtual void DoDuringPlaybackActionsL();
	
	void FailTest(TInt aError);
	
protected:
	RTestVclnt2Subtitle(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	
	// from RTestStep
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepPostambleL();
	virtual TInt SetCacheSize();
	virtual TInt CheckCacheError(TInt aError);
	virtual TInt RevertCacheSize();
	
	void FsmL(TVclntTestPlayEvents aEventCode);	
protected:
	CPlayTimer* iPlayTimer;
	};

/**
 * MM-MMF-VCLNTAVI-I-0602-CP
 */
class RTestVclnt2SubtitleEnableDuringPlayback : public RTestVclnt2Subtitle
	{
public:
	static RTestVclnt2SubtitleEnableDuringPlayback* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	void DoBeforePlaybackActionsL();
	void DoDuringPlaybackActionsL();
	
private:
	RTestVclnt2SubtitleEnableDuringPlayback(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	};

/**
 * MM-MMF-VCLNTAVI-I-0603-CP
 */
class RTestVclnt2SubtitleDisableDuringPlayback : public RTestVclnt2Subtitle
	{
public:
	static RTestVclnt2SubtitleDisableDuringPlayback* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	void DoDuringPlaybackActionsL();
	
private:
	RTestVclnt2SubtitleDisableDuringPlayback(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	};

/**
 * MM-MMF-VCLNTAVI-I-0605-CP
 */
class RTestVclnt2SubtitleDisableTwice : public RTestVclnt2Subtitle
	{
public:
	static RTestVclnt2SubtitleDisableTwice* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	void DoBeforePlaybackActionsL();
	void DoDuringPlaybackActionsL();
	
private:
	RTestVclnt2SubtitleDisableTwice(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	};
	
/**
 * MM-MMF-VCLNTAVI-I-0606-CP
 */
class RTestVclnt2SubtitleWindowClipBefore : public RTestVclnt2Subtitle
	{
public:
	static RTestVclnt2SubtitleWindowClipBefore* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	void DoBeforePlaybackActionsL();
	
private:
	RTestVclnt2SubtitleWindowClipBefore(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	};

/**
 * MM-MMF-VCLNTAVI-I-0607-CP
 */
class RTestVclnt2SubtitleWindowClipDuring : public RTestVclnt2Subtitle
	{
public:
	static RTestVclnt2SubtitleWindowClipDuring* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	void DoDuringPlaybackActionsL();
	
private:
	RTestVclnt2SubtitleWindowClipDuring(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	};

/**
 * MM-MMF-VCLNTAVI-I-0608-CP
 */
class RTestVclnt2SubtitleRotateBefore : public RTestVclnt2Subtitle
	{
public:
	static RTestVclnt2SubtitleRotateBefore* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	void DoBeforePlaybackActionsL();
	
private:
	RTestVclnt2SubtitleRotateBefore(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	};

/**
 * MM-MMF-VCLNTAVI-I-0609-CP
 */
class RTestVclnt2SubtitleRotateDuring : public RTestVclnt2Subtitle
	{
public:
	static RTestVclnt2SubtitleRotateDuring* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	void DoDuringPlaybackActionsL();
	
private:
	RTestVclnt2SubtitleRotateDuring(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	};

/**
 * MM-MMF-VCLNTAVI-I-0613-CP
 */
class RTestVclnt2SubtitleRemoveDisplayDuring : public RTestVclnt2Subtitle
	{
public:
	static RTestVclnt2SubtitleRemoveDisplayDuring* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	void DoDuringPlaybackActionsL();
	
private:
	RTestVclnt2SubtitleRemoveDisplayDuring(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	};

/**
 * MM-MMF-VCLNTAVI-I-0701-CP
 */
class RTestVclnt2SubtitleNotAvailable : public RTestVclnt2Subtitle
	{
public:
	static RTestVclnt2SubtitleNotAvailable* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	void DoBeforePlaybackActionsL();
	
private:
	RTestVclnt2SubtitleNotAvailable(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	};

/**
 * MM-MMF-VCLNTAVI-I-0705-CP
 */
class RTestVclnt2SubtitleLanguage : public RTestVclnt2Subtitle
	{
public:
	static RTestVclnt2SubtitleLanguage* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	void DoBeforePlaybackActionsL();
	
private:
	RTestVclnt2SubtitleLanguage(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	};

/**
 * MM-MMF-VCLNTAVI-I-0610-CP
 */
class RTestVclnt2SubtitleRedraw: public RTestVclnt2Subtitle
	{
public:
	static RTestVclnt2SubtitleRedraw* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, TBool aEnableSubtitles=ETrue);
	void DoBeforePlaybackActionsL();
	void DoDuringPlaybackActionsL();
	
private:
	RTestVclnt2SubtitleRedraw(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError, TBool aEnableSubtitles);

private:
	TBool iEnableSubtitles;
	};

/**
 * MM-MMF-VCLNTAVI-I-0611-CP
 */
class RTestVclnt2SubtitleRotateClipRedraw: public RTestVclnt2Subtitle
	{
public:
	static RTestVclnt2SubtitleRotateClipRedraw* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	void DoBeforePlaybackActionsL();
	void DoDuringPlaybackActionsL();
	
private:
	RTestVclnt2SubtitleRotateClipRedraw(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	};

/**
 * MM-MMF-VCLNTAVI-I-0708-CP
 */
class RTestVclnt2SubtitleMultipleWindows: public RTestVclnt2Subtitle
	{
public:
	static RTestVclnt2SubtitleMultipleWindows* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	void DoBeforePlaybackActionsL();
	
protected:
	// from RTestStep
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepPostambleL();
	
private:
	RTestVclnt2SubtitleMultipleWindows(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	
private:
	RWindow* iSecondaryWin;
	};

/**
 * MM-MMF-VCLNTAVI-I-0707-CP
 */
class RTestVclnt2SubtitleEnableTwice: public RTestVclnt2Subtitle
	{
public:
	static RTestVclnt2SubtitleEnableTwice* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	void DoBeforePlaybackActionsL();
	
private:
	RTestVclnt2SubtitleEnableTwice(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	};

/**
 * MM-MMF-VCLNTAVI-I-0612-CP
 */
class RTestVclnt2SubtitleEnableDisableStress: public RTestVclnt2Subtitle
	{
public:
	static RTestVclnt2SubtitleEnableDisableStress* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	void DoBeforePlaybackActionsL();
	void DoDuringPlaybackActionsL();
	
private:
	RTestVclnt2SubtitleEnableDisableStress(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);

private:
	TBool iDisableNext;
	TBool iExpectAvailable;
	};

/**
 * MM-MMF-VCLNTAVI-I-0706-CP
 */
class RTestVclnt2SubtitleAlloc: public RTestVclnt2Subtitle
	{
public:
	static RTestVclnt2SubtitleAlloc* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepPostambleL();
	TVerdict DoTestStepL();

private:
	RTestVclnt2SubtitleAlloc(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	TVerdict PerformTestL();
	};

#endif //TESTVIDEOPLAYER_SUBTITLES_H
