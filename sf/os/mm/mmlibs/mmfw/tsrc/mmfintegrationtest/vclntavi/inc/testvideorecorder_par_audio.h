// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: Video Recorder Extra Tests.
// 
//

#ifndef TESTVIDEORECORDER_PAR_AUDIO_H
#define TESTVIDEORECORDER_PAR_AUDIO_H

#include "tsi_mmf_vclntavi_stepbase.h"
#include "testvideorecorder.h"


// POSITIVE TESTS

/**
 * Set and Get video pixel aspect ratio for recording and record.
 * MM-MMF-VCLNTAVI-I-0300, MM-MMF-VCLNTAVI-I-0315
 *
 * RTestVclntSetGetPARRec
 *
 */
class RTestVclntSetGetPARRec : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntSetGetPARRec* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError,TInt aNumerator=1, TInt aDenominator=1);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
    virtual void FinishedL();
private:
    RTestVclntSetGetPARRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aNumerator, TInt aDenominator);
    };

/**
 * Get supported video pixel aspect ratioes for recording.
 * MM-MMF-VCLNTAVI-I-0301
 *
 * RTestVclntGetSuppPAR
 *
 */
class RTestVclntGetSuppPAR : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntGetSuppPAR* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
    virtual void FinishedL();
private:
    RTestVclntGetSuppPAR(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

/**
 * Set and Get PAR [1:1] (in invalid state) after open and prepare and then record.
 * MM-MMF-VCLNTAVI-I-0302
 *
 * RTestVclntSetGetPARAftOpenPrepThenRec
 *
 */
class  RTestVclntSetGetPARAftOpenPrepThenRec : public RTestVclntRecordAviFile
    {
public:
    static  RTestVclntSetGetPARAftOpenPrepThenRec* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
	virtual void FinishedL();
private:
	RTestVclntSetGetPARAftOpenPrepThenRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

/**
 * Set video PAR (in invalid state) after opening the video file for recording (without setting video frame size, video codec and audio codec.
 * MM-MMF-VCLNTAVI-I-0303
 *
 * RTestVclntSetPARAftOpenWoutSetVidAudCodec
 *
 */
class RTestVclntSetPARAftOpenWoutSetVidAudCodec : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntSetPARAftOpenWoutSetVidAudCodec* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
    virtual void FinishedL();
private:
    RTestVclntSetPARAftOpenWoutSetVidAudCodec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

/**
 * Set and Get the number of audio channels for recording and record.
 * RTestVclntSetGetAudChanRec
 * MM-MMF-VCLNTAVI-I-0304, MM-MMF-VCLNTAVI-I-0318
 */
class RTestVclntSetGetAudChanRec : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntSetGetAudChanRec* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aAudioChannel=2);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
    virtual void FinishedL();
private:
    RTestVclntSetGetAudChanRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aAudioChannel=2);
    };

/**
 * Get supported number of audio channels for recording.
 *
 * MM-MMF-VCLNTAVI-I-0305
 * RTestVclntGetSuppAudChan
 *
 */
class RTestVclntGetSuppAudChan : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntGetSuppAudChan* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
    virtual void FinishedL();
private:
    RTestVclntGetSuppAudChan(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

/**
 * Set and Get valid audio channel (in invalid state) after open and prepare and then record.
 * MM-MMF-VCLNTAVI-I-0306
 *
 * RTestVclntSetGetAudChanAftOpenPrepThenRec
 *
 */
class RTestVclntSetGetAudChanAftOpenPrepThenRec : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntSetGetAudChanAftOpenPrepThenRec* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
	virtual void FinishedL();
private:
    RTestVclntSetGetAudChanAftOpenPrepThenRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

/**
 * Get the list of supported channels, Set and Get the valid audio channel (in invalid state) before selecting the audio codec and start recording.
 * MM-MMF-VCLNTAVI-I-0307
 *
 * RTestVclntGetSuppAudChanSetGetAudChanBefAudCodRec
 *
 */
class RTestVclntGetSuppAudChanSetGetAudChanBefAudCodRec : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntGetSuppAudChanSetGetAudChanBefAudCodRec* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
	virtual void FinishedL();
private:
    RTestVclntGetSuppAudChanSetGetAudChanBefAudCodRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

/**
 * Set and Get the audio sample rate for recording.
 * MM-MMF-VCLNTAVI-I-0308, MM-MMF-VCLNTAVI-I-0321
 *
 * RTestVclntSetGetAudSRRec
 *
 */
class RTestVclntSetGetAudSRRec : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntSetGetAudSRRec* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aAudioSampleRate=22050);
    virtual TVerdict DoTestStepL();
	virtual void FsmL(TVclntTestRecordEvents aEventCode);
    virtual void FinishedL();
private:
    RTestVclntSetGetAudSRRec(const TDesC& aTestName, const TDesC& SectName,const TDesC& aKeyName, TInt aExpectedError, TInt aAudioSampleRate=22050);
    };

/**
 * Set and Get the audio sample rate for recording and record.
 * MM-MMF-VCLNTAVI-I-0309
 *
 * RTestVclntGetSuppAudSR
 *
 */
class RTestVclntGetSuppAudSR : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntGetSuppAudSR* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
	virtual void FsmL(TVclntTestRecordEvents aEventCode);
    virtual void FinishedL();
private:
    RTestVclntGetSuppAudSR(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

/**
 * Set and Get valid sample rate (in invalid state) after open and prepare and then record.
 * MM-MMF-VCLNTAVI-I-0310
 *
 * RTestVclntSetGetSRAftOpenPrepRec
 *
 */
class RTestVclntSetGetSRAftOpenPrepRec : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntSetGetSRAftOpenPrepRec* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
    virtual void FinishedL();
private:
    RTestVclntSetGetSRAftOpenPrepRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

/**
 * Get the list of supported sample rates, Set and Get the valid sample rates (in invalid state) before selecting the audio codec and then record.
 * MM-MMF-VCLNTAVI-I-0311
 *
 * RTestVclntGetSuppSRSetGetSRBefAudCodRec
 *
 */
class RTestVclntGetSuppSRSetGetSRBefAudCodRec : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntGetSuppSRSetGetSRBefAudCodRec* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
	virtual void FinishedL();
private:
    RTestVclntGetSuppSRSetGetSRBefAudCodRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

/**
 * Get default PAR,audio sample rate and audio channel after open the target but before prepare has been called.
 * MM-MMF-VCLNTAVI-I-0312
 *
 * RTestVclntGetDefPARAudSRAudChanAftOpenBefPrep
 *
 */
class RTestVclntGetDefPARAudSRAudChanAftOpenBefPrep : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntGetDefPARAudSRAudChanAftOpenBefPrep* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
	virtual void FsmL(TVclntTestRecordEvents aEventCode);
	virtual void FinishedL();
private:
    RTestVclntGetDefPARAudSRAudChanAftOpenBefPrep(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

/**
 * Set Audio/Video Bit Rate, Audio Gain and PAR before Prepare and do recording  while calling Pause in between  the recording and Get the Audio/Video Bit Rate,  Audio Gain and PAR after Stop.
 * MM-MMF-VCLNTAVI-I-0313
 * RTestVclntSetGetPARAudVidBRAudGain
 *
 */
class RTestVclntSetGetPARAudVidBRAudGain : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntSetGetPARAudVidBRAudGain* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
	virtual void FsmL(TVclntTestRecordEvents aEventCode);
	virtual void FinishedL();
private:
    RTestVclntSetGetPARAudVidBRAudGain(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    TVclntTestRecordEvents 		iCurrentEvent;
    TInt 						iGain;
    };

/**
 * Set and Get audio sample rate and no. of channels for video only file [audio disabled] and then record.
 * MM-MMF-VCLNTAVI-I-0314
 *
 * RTestVclntSetValidSetGetAudSRChanAudDisable
 *
 */
class RTestVclntSetGetAudSRAudChanWithAudDisable : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntSetGetAudSRAudChanWithAudDisable* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
	virtual void FinishedL();
private:
    RTestVclntSetGetAudSRAudChanWithAudDisable(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

// NEGATIVE TESTS
/**
 * Get supported list of PAR,Set and Get PAR [1:1] (in invalid state) before Open File. Then Open and Record.
 * MM-MMF-VCLNTAVI-I-0316
 *
 * RTestVclntGetSuppPARSetGetPARBefOpenThenOpenRec
 *
 */
class RTestVclntGetSuppPARSetGetPARBefOpenThenOpenRec : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntGetSuppPARSetGetPARBefOpenThenOpenRec* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
	virtual void FsmL(TVclntTestRecordEvents aEventCode);
	virtual void FinishedL();
private:
    RTestVclntGetSuppPARSetGetPARBefOpenThenOpenRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

/**
 * Set and Get PAR [1:1] (in invalid state) in recording state.
 * MM-MMF-VCLNTAVI-I-0317
 *
 * RTestVclntSetGetValidPARWhileRec
 *
 */
class RTestVclntSetGetValidPARWhileRec : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntSetGetValidPARWhileRec* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
    virtual void FinishedL();
private:
    RTestVclntSetGetValidPARWhileRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

/**
 * Get the list of supported channels, Set and Get valid audio channel (in invalid state) before open the target and then start recording after open the file.
 * MM-MMF-VCLNTAVI-I-0319
 *
 * RTestVclntGetSuppChanSetGetAudChanBefOpenThenOpenRec
 *
 */
class RTestVclntGetSuppChanSetGetAudChanBefOpenThenOpenRec : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntGetSuppChanSetGetAudChanBefOpenThenOpenRec* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
    virtual void FinishedL();
private:
   RTestVclntGetSuppChanSetGetAudChanBefOpenThenOpenRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

/**
 * Set and Get valid audio channel (in invalid state) in recording state.
 * MM-MMF-VCLNTAVI-I-0320
 *
 * RTestVclntSetGetValidAudChanWhileRec
 *
 */
class RTestVclntSetGetValidAudChanWhileRec : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntSetGetValidAudChanWhileRec* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
	virtual void FinishedL();
private:
    RTestVclntSetGetValidAudChanWhileRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

/**
 * Get supported list of sample rates, Set and Get valid sample rate (in invalid state) before openfile. Then open and record.
 * MM-MMF-VCLNTAVI-I-0322
 *
 * RTestVclntGetSuppSRSetGetSRBefOpenThenOpenRec
 *
 */
class RTestVclntGetSuppSRSetGetSRBefOpenThenOpenRec : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntGetSuppSRSetGetSRBefOpenThenOpenRec* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
	virtual void FinishedL();
private:
    RTestVclntGetSuppSRSetGetSRBefOpenThenOpenRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

/**
 * Set and Get valid sample rate (in invalid state) during recording.
 * MM-MMF-VCLNTAVI-I-0323
 *
 * RTestVclntSetGetValidSRWhileRec
 *
 */
class RTestVclntSetGetValidSRWhileRec : public RTestVclntRecordAviFile
    {
public:
    static RTestVclntSetGetValidSRWhileRec* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestRecordEvents aEventCode);
	virtual void FinishedL();
private:
    RTestVclntSetGetValidSRWhileRec(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

#endif // TESTVIDEORECORDER_PAR_AUDIO_H
