// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file: Video Player Tests.
// 
//

#ifndef TESTVIDEOPLAYER_SECDISP_H
#define TESTVIDEOPLAYER_SECDISP_H

#include "tsi_mmf_vclntavi_stepbase.h"

class CTestVclntAviPlayTimer;

/**
 *
 * RTestVclntSetScrIdAndPlayBase
 *
 */
class RTestVclntSetScrIdAndPlayBase : public RTestVclntAviPlayerStep
    {
public:
    RTestVclntSetScrIdAndPlayBase(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber);
    
    virtual void FsmL(TVclntTestPlayEvents aEventCode) = 0;
    // timer handling
    virtual void FinishedL();
    // Just to handle the behaviour of ARMV5
    virtual void MvpuoOpenComplete(TInt aError);
protected:
    TInt iScreenNumber; // Stores screen number
    TInt iCurrentScreenNumber; // Stores currently active screen number
    };

/**
 * Set the screen id and Play the video file.
 * Set the invalid screen id and Play the video file.
 *
 * RTestVclntSetScrIdAndPlayAviFile
 * MM-MMF-VCLNTAVI-I-0201-CP, MM-MMF-VCLNTAVI-I-0251-CP
 *
 */
class RTestVclntSetScrIdAndPlayAviFile : public RTestVclntSetScrIdAndPlayBase
    {
public:
    static RTestVclntSetScrIdAndPlayAviFile* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestPlayEvents aEventCode);
private:
    RTestVclntSetScrIdAndPlayAviFile(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber);
    };


/**
 * Set the screen id and Play video descriptor.
 *
 * RTestVclntSetScrIdAndPlayAviDes
 * MM-MMF-VCLNTAVI-I-0202-CP
 *
 */
class RTestVclntSetScrIdAndPlayAviDes : public RTestVclntSetScrIdAndPlayBase
    {
public:
    static RTestVclntSetScrIdAndPlayAviDes* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber);
    virtual TVerdict DoTestStepL();
    virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict DoTestStepPostambleL();
    virtual void FsmL(TVclntTestPlayEvents aEventCode);
protected:
    HBufC8* iVideo;                        // Buffer for holding video descriptor data
private:
    RTestVclntSetScrIdAndPlayAviDes(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber) ;    
    } ;

/**
 * Set the screen id and Play video from URL.
 *
 * RTestVclntSetScrIdAndPlayAviUrl
 * MM-MMF-VCLNTAVI-I-0203-CP
 *
 */
 
class RTestVclntSetScrIdAndPlayAviUrl : public RTestVclntSetScrIdAndPlayBase
    {
public:
    static RTestVclntSetScrIdAndPlayAviUrl* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestPlayEvents aEventCode);
private:
    RTestVclntSetScrIdAndPlayAviUrl(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber);    
    };
    
/**
 * Set the screen id and play video file, Stop.Again set the screen id and play.
 * Set the screen id after stop and play again.
 *
 * RTestVclntSetScrIdAndPlayStopAviFile
 * MM-MMF-VCLNTAVI-I-0204-CP, MM-MMF-VCLNTAVI-I-0206-CP
 *
 */
class RTestVclntSetScrIdAndPlayStopAviFile : public RTestVclntSetScrIdAndPlayBase
    {
public:
    static RTestVclntSetScrIdAndPlayStopAviFile* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber, TInt aScreenNumber2);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestPlayEvents aEventCode);
    
    // timer handling
    void FinishedL();
protected:
    CTestVclntAviPlayTimer* iPlayTimer;
    TBool iRestarted;
    TInt iScreenNumber2; // Stores screen number
    TInt iCountNumber;
    TFileName iFilename2;
private:
    RTestVclntSetScrIdAndPlayStopAviFile(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber, TInt aScreenNumber2);    
    };
    
/**
 * Set the screen id and play video file, Pause.Again set the screen id and play.
 * Set the screen ID after Pause and play again.
 *
 * RTestVclntSetScrIdAndPlayPauseAviFile
 * MM-MMF-VCLNTAVI-I-0205-CP, MM-MMF-VCLNTAVI-I-0207-CP
 *
 */
class RTestVclntSetScrIdAndPlayPauseAviFile : public RTestVclntSetScrIdAndPlayBase
    {
public:
    static RTestVclntSetScrIdAndPlayPauseAviFile* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber, TInt aScreenNumber2);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestPlayEvents aEventCode);
    
    // timer handling
    void FinishedL();
protected:
    CTestVclntAviPlayTimer* iPlayTimer;
    TBool iRestarted;
    TInt iScreenNumber2; // Stores screen number
    TInt iCountNumber;
    TFileName iFilename2;
private:
    RTestVclntSetScrIdAndPlayPauseAviFile(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber, TInt aScreenNumber2);    
    };
    
/**
 * Set screen ID after play complete first time and before open the second time.
 *
 * RTestVclntSetScrIdAfterPlayAndPlayAviFile
 * MM-MMF-VCLNTAVI-I-0208-CP
 *
 */
class RTestVclntSetScrIdAfterPlayAndPlayAviFile : public RTestVclntSetScrIdAndPlayBase
    {
public:
    static RTestVclntSetScrIdAfterPlayAndPlayAviFile* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestPlayEvents aEventCode);
protected:
    TInt iCountNumber;
private:
    RTestVclntSetScrIdAfterPlayAndPlayAviFile(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber);    
    };
    
/**
 * Set screen ID after Open but before play
 *
 * RTestVclntSetScrIdAfterOpenAndPlayAviFile
 * MM-MMF-VCLNTAVI-I-0209-CP
 *
 */
class RTestVclntSetScrIdAfterOpenAndPlayAviFile : public RTestVclntSetScrIdAndPlayBase
    {
public:
    static RTestVclntSetScrIdAfterOpenAndPlayAviFile* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestPlayEvents aEventCode);
protected:
    TInt iCountNumber;
private:
    RTestVclntSetScrIdAfterOpenAndPlayAviFile(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber);    
    };
    
/**
 * Set screen ID during Play
 *
 * RTestVclntSetScrIdDuringPlayAndPlayAviFile
 * MM-MMF-VCLNTAVI-I-0211-CP
 *
 */
class RTestVclntSetScrIdDuringPlayAndPlayAviFile : public RTestVclntSetScrIdAndPlayBase
    {
public:
    static RTestVclntSetScrIdDuringPlayAndPlayAviFile* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber);
    virtual TVerdict DoTestStepL();
    virtual void FsmL(TVclntTestPlayEvents aEventCode);
protected:
    TInt iCountNumber;
private:
    RTestVclntSetScrIdDuringPlayAndPlayAviFile(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TInt aScreenNumber);    
    };
    
/**
 * 
 * CTestVclntAviPlayTimer 
 * - Timer for those test classes requiring timed stop
 *
 */
class CTestVclntAviPlayTimer : public CTimer
    {
public:
    static CTestVclntAviPlayTimer* NewL(RTestVclntSetScrIdAndPlayBase* aParent, TTimeIntervalMicroSeconds32 aWaitTime);
    void Start();
protected:
    TInt RunError(TInt aError);
    void RunL();
private:
    CTestVclntAviPlayTimer(RTestVclntSetScrIdAndPlayBase* aParent, TTimeIntervalMicroSeconds32 aWaitTime);
    RTestVclntSetScrIdAndPlayBase* iParent;
    TTimeIntervalMicroSeconds32 iWaitTime;
    };                                              

#endif //TESTVIDEOPLAYER_SECDISP_H
