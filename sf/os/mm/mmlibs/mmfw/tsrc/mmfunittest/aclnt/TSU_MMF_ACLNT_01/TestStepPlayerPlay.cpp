// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains an example Test step implementation 
// This demonstrates the various functions provided
// by the CTestStep base class which are available within
// a test step 
// 
//

// EPOC includes
#include <e32base.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <testframework.h>

#include <mdaaudiosampleplayer.h>
//#include <MdaAudioSampleEditor.h>
//#include <MdaAudioTonePlayer.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepPlayerNew.h"

#include "mmfclientaudioplayer.h"

// Path where the test files reside
_LIT(KFilePath, "C:\\mm\\mmf\\testfiles\\aclnt\\");

// --------------------------------------------

/**
 *
 * Static constructor for CTestStepPlayerPlay.
 *
 *
 * @return   "CTestStepPlayerPlay*"
 *         The constructed CTestStepPlayerPlay
 *
 * @xxxx
 * 
 */
CTestStepPlayerPlay* CTestStepPlayerPlay::NewL()
   {
   CTestStepPlayerPlay* self = new(ELeave) CTestStepPlayerPlay;
   return self;
   }

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepPlayerPlay::CTestStepPlayerPlay() : iHasPlayed(EFalse)
   {
   iTestStepResult = EPass;
   // store the name of this test case
   // this is the name that is used by the script file
   iTestStepName = _L("MM-MMF-ACLNT-U-0010-CP");
   }

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerPlay::~CTestStepPlayerPlay()
   {
   }
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerPlay::MapcInitComplete(TInt aError, 
                        const TTimeIntervalMicroSeconds &/*aDuration*/)
   {
   iError = aError;
   CActiveScheduler::Stop();
   }

void CTestStepPlayerPlay::MapcPlayComplete(TInt aError)
   {
   iError = aError;
   if (iError == KErrNone)
      iHasPlayed = ETrue;
   CActiveScheduler::Stop();
   }

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepPlayerPlay::DoTestStepPreambleL(void)
   {
    enum TVerdict verdict;
    // this installs the scheduler
    verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

   // Printing to the console and log file
    INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0010-CP"));
    INFO_PRINTF1(_L("this test is playing a sound file after openning it. Play()"));

    iError = KErrNone;

    if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName))
      {
       INFO_PRINTF2(_L("file name %S not found..."), &iFileName);
       return EInconclusive;
      }

	// 23_01_06 This test case has been modified in order to provide regression testing for improved CAF file sharing.
	// See defect DEF076592.  This test case ensures that a file can be played that has already been opened for reading/writing.
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	RFile temp;
	temp.Open(fs, iFileName, EFileRead|EFileWrite|EFileShareReadersOnly);
	CleanupClosePushL(temp);
	
    iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
    CActiveScheduler::Start();
    User::After(1000000);
    
    CleanupStack::PopAndDestroy(2, &fs);

    if (iPlayer == NULL ||
       iError != KErrNone )
       return EInconclusive;

    return verdict;
   }

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepPlayerPlay::DoTestStepPostambleL(void)
   {
   delete iPlayer;
   iPlayer = NULL;
   //[ Destroy the scheduler ]
   return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
   }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return   "TVerdict"
 *         The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepPlayerPlay::DoTestStepL()
	{
	//__MM_HEAP_MARK;

   // check if player openned the file successfully
   TInt err1 = KErrNone;
   TInt state = 0;
   err1 = GetAudPlayerState(iPlayer, state);
   if ( state != CMMFMdaAudioPlayerUtility::EStopped )
       return EInconclusive;

   TInt err2 = iPlayer->SetThreadPriority(EPriorityAbsoluteHigh);
   if (err2 != KErrNone)
       {
       INFO_PRINTF1(_L("Failed to set the player's thread priority"));
   	   return EFail;
   	   }

   iPlayer->Play();
   CActiveScheduler::Start();

   User::After(500000);

   iPlayer->Pause(); //JPW


   User::After(2000000);

   err2 = KErrNone;
   TMMFMessageDestination* srcHandle = NULL;
   err1 = GetAudPlayerState(iPlayer, state);
   err2 = GetAudPlayerSourceHandle(iPlayer, srcHandle);
   if ( err1 != KErrNone ||
       err2 != KErrNone ||
       &srcHandle == NULL ||
       state != CMMFMdaAudioPlayerUtility::EStopped ||
       !iHasPlayed ||
       iError != KErrNone)
       iTestStepResult = EFail;


	//__MM_HEAP_MARKEND;

   INFO_PRINTF1(_L("finished with this test step"));
   // test steps return a result
   return iTestStepResult;
   }


/**
 *
 * Static constructor for CTestStepPlayerPlay.
 *
 *
 * @return   "CTestStepPlayerPlay*"
 *         The constructed CTestStepPlayerPlay
 *
 * @xxxx
 * 
 */
CTestStepPlayerPlayTwice* CTestStepPlayerPlayTwice::NewL()
   {
   CTestStepPlayerPlayTwice* self = new(ELeave) CTestStepPlayerPlayTwice;
   return self;
   }

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepPlayerPlayTwice::CTestStepPlayerPlayTwice()
   : iHasPlayed(EFalse)
   {
   iTestStepResult = EPass;
   // store the name of this test case
   // this is the name that is used by the script file
   iTestStepName = _L("MM-MMF-ACLNT-U-0012-CP");
   }

/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerPlayTwice::MapcInitComplete(TInt aError, 
                                    const TTimeIntervalMicroSeconds &/*aDuration*/)
   {
   iError = aError;
   CActiveScheduler::Stop();
   }

void CTestStepPlayerPlayTwice::MapcPlayComplete(TInt aError)
   {
   iError = aError;
   if (iError == KErrNone)
      iHasPlayed = ETrue;
   CActiveScheduler::Stop();
   }

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepPlayerPlayTwice::DoTestStepPreambleL(void)
   {
    enum TVerdict verdict;
    // this installs the scheduler
    verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

   // Printing to the console and log file
    INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0012-CP"));
    INFO_PRINTF1(_L("This test is calling Play() twice"));

    iError = KErrNone;

    if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName))
      {
       //INFO_PRINTF2(_L("file name %s not found..."), fileptr);
       return EInconclusive;
      }

	// This test case has been modified in order to provide regression testing for improved CAF file sharing
	// See defect DEF076592.  This test case now checks that a file can be played from z:\resource
	_LIT(KResFile, "z:\\resource\\small.wav");
	TBuf<100> temp;
	temp.Copy(KResFile);
	iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(temp, *this);
    CActiveScheduler::Start();
    User::After(1000000);

    if (iPlayer == NULL ||
       iError != KErrNone )
       return EInconclusive;

    return verdict;
   }

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
TVerdict CTestStepPlayerPlayTwice::DoTestStepPostambleL(void)
   {
   delete iPlayer;
   iPlayer = NULL;
   //[ Destroy the scheduler ]
   return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
   }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return   "TVerdict"
 *         The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepPlayerPlayTwice::DoTestStepL()
	{
	//__MM_HEAP_MARK;

   // check if player openned the file successfully
   TInt err1 = KErrNone;
   TInt state = 0;
   err1 = GetAudPlayerState(iPlayer, state);
   if ( state != CMMFMdaAudioPlayerUtility::EStopped )
      return EInconclusive;

   // used to measure the time it takes to play
   TTime timeStart;
   TTime timeEnd;

   // start the timer
   timeStart.UniversalTime();

   // Play
   iPlayer->Play();
   CActiveScheduler::Start();

   // stop the timer
   timeEnd.UniversalTime();
   TTimeIntervalMicroSeconds playDuration1 = 
      timeEnd.MicroSecondsFrom(timeStart);

   User::After(2000000);

   TInt err2 = KErrNone;
   TMMFMessageDestination* srcHandle = NULL;
   err1 = GetAudPlayerState(iPlayer, state);
   err2 = GetAudPlayerSourceHandle(iPlayer, srcHandle);
   if ( err1 != KErrNone ||
       err2 != KErrNone ||
       &srcHandle == NULL ||
       state != CMMFMdaAudioPlayerUtility::EStopped ||
       !iHasPlayed ||
       iError != KErrNone)
      {
      iTestStepResult = EFail;
      }

   // start the timer
   timeStart.UniversalTime();

   // Play again
   iPlayer->Play();
   CActiveScheduler::Start();

   // stop the timer
   timeEnd.UniversalTime();
   TTimeIntervalMicroSeconds playDuration2 = 
      timeEnd.MicroSecondsFrom(timeStart);
   
   // calculate the difference (hopefully pretty small)
   TInt64 playDurationDiff = 
      playDuration2.Int64() - playDuration1.Int64();
   // make sure it's a positive number
   if (playDurationDiff < 0)
      playDurationDiff = -playDurationDiff;

   INFO_PRINTF2(_L("Duration #1 = %d microseconds"), (long)(playDuration1.Int64()));
   INFO_PRINTF2(_L("Duration #2 = %d microseconds"), (long)(playDuration2.Int64()));
   INFO_PRINTF2(_L("Difference  = %d microseconds"), (long)playDurationDiff);
   if (playDurationDiff > 250000)
      {
      INFO_PRINTF1(_L("Error: Difference is too high"));
   #ifdef __WINS__
      // under WINS these values can't be trusted due to both
      // background processing and timer resolution
      // report the difference as an error but still allow
      // the test to pass.
      INFO_PRINTF1(_L("This is Emulator so allowing test to still pass..."));
   #else
      iTestStepResult = EFail;
   #endif
      }

   User::After(2000000);

   err2 = KErrNone;
   srcHandle = NULL;
   err1 = GetAudPlayerState(iPlayer, state);
   err2 = GetAudPlayerSourceHandle(iPlayer, srcHandle);
   if ( err1 != KErrNone ||
       err2 != KErrNone ||
       &srcHandle == NULL ||
       state != CMMFMdaAudioPlayerUtility::EStopped ||
       !iHasPlayed ||
       iError != KErrNone)
      {
      INFO_PRINTF1(_L("Invalid player state"));
      iTestStepResult = EFail;
      }

	//__MM_HEAP_MARKEND;

   INFO_PRINTF1(_L("finished with this test step"));
   // test steps return a result
   return iTestStepResult;
   }

/**
 *
 * Static constructor for CTestStepPlayerPlayThreeTimes.
 *
 *
 * @return   "CTestStepPlayerPlayThreeTimes*"
 *         The constructed CTestStepPlayerPlayThreeTimes
 *
 * @xxxx
 * 
 */
CTestStepPlayerPlayThreeTimes* CTestStepPlayerPlayThreeTimes::NewL()
   {
   CTestStepPlayerPlayThreeTimes* self = new(ELeave) CTestStepPlayerPlayThreeTimes;
   return self;
   }

CTestStepPlayerPlayThreeTimes::CTestStepPlayerPlayThreeTimes()
   : iHasPlayed(EFalse)
   {
   // store the name of this test case
   // this is the name that is used by the script file
   iTestStepName = _L("MM-MMF-ACLNT-U-0013-CP");
   }

/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerPlayThreeTimes::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
   {
   iError = aError;
   CActiveScheduler::Stop();
   }

void CTestStepPlayerPlayThreeTimes::MapcPlayComplete(TInt aError)
   {
   iError = aError;
   if (iError == KErrNone)
      iHasPlayed = ETrue;
   CActiveScheduler::Stop();
   }

TVerdict CTestStepPlayerPlayThreeTimes::DoTestStepPreambleL(void)
   {
   TVerdict verdict;
   // this installs the scheduler
   verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

   // Printing to the console and log file
   INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0013-CP"));
   INFO_PRINTF1(_L("This test is calling Play() three times"));

   iError = KErrNone;

   if (!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName2"), iFileName))
      {
      return EInconclusive;
      }

   iPlayer = CMMFMdaAudioPlayerUtility::NewFilePlayerL(iFileName, *this);
   CActiveScheduler::Start();
   User::After(1000000);

   if (iPlayer == NULL || iError != KErrNone )
       return EInconclusive;

   return verdict;
   }

TVerdict CTestStepPlayerPlayThreeTimes::DoTestStepPostambleL(void)
   {
   delete iPlayer;
   iPlayer = NULL;
   //[ Destroy the scheduler ]
   return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
   }

TVerdict CTestStepPlayerPlayThreeTimes::DoTestStepL()
   {
   // check if player openned the file successfully
//   TInt err1 = KErrNone;   // EABI warning removal
   TInt state = 0;
//   err1 = GetAudPlayerState(iPlayer, state);
   GetAudPlayerState(iPlayer, state);   // EABI warning removal
   if ( state != CMMFMdaAudioPlayerUtility::EStopped )
      return EInconclusive;

   TCallBack cb(PlayThreeTimesCallBack, this);
   iTimer = CPeriodic::NewL(CActive::EPriorityLow);

   // Play
   INFO_PRINTF1(_L("Playing for first time"));
   iPlayer->Play();
   iTimer->Start(1500000, 3000000, cb);
   CActiveScheduler::Start();
   if (iError != KErrNone || iHasPlayed)
      {
      INFO_PRINTF3(_L("Error playing first time  iError = %d, iHasPlayed = %d"), iError, iHasPlayed);
      return EFail;
      }

   User::After(500000);

   // Play again
   INFO_PRINTF1(_L("Playing for second time"));
   iPlayer->Play();
   CActiveScheduler::Start();
   if (iError != KErrNone || !iHasPlayed)
      {
      INFO_PRINTF3(_L("Error playing second time  iError = %d, iHasPlayed = %d"), iError, iHasPlayed);
      return EFail;
      }

   User::After(2000000);

   // Play third time
   INFO_PRINTF1(_L("Playing for third time"));
   iPlayer->Play();
   CActiveScheduler::Start();
   if (iError != KErrNone || !iHasPlayed)
      {
      INFO_PRINTF3(_L("Error playing third time  iError = %d, iHasPlayed = %d"), iError, iHasPlayed);
      return EFail;
      }

   INFO_PRINTF1(_L("finished with this test step"));

   return iTestStepResult;
   }

TInt CTestStepPlayerPlayThreeTimes::PlayThreeTimesCallBack(TAny* aPtr)
   {
   CTestStepPlayerPlayThreeTimes* self = static_cast<CTestStepPlayerPlayThreeTimes*>(aPtr);
   self->DoPlayThreeTimesCallBack();
   return EFalse;
   }

void CTestStepPlayerPlayThreeTimes::DoPlayThreeTimesCallBack()
   {
   iPlayer->Stop();
   iTimer->Cancel();
   CActiveScheduler::Stop();
   }

/**
 *
 * Static constructor for CTestStepPlayerClosePlay.
 *
 *
 * @return   "CTestStepPlayerPlay*"
 *         The constructed CTestStepPlayerPlay
 *
 * @xxxx
 * 
 */
CTestStepPlayerClosePlay* CTestStepPlayerClosePlay::NewL()
   {
   CTestStepPlayerClosePlay* self = new(ELeave) CTestStepPlayerClosePlay;
   return self;
   }

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepPlayerClosePlay::CTestStepPlayerClosePlay()
   {
   // store the name of this test case
   // this is the name that is used by the script file
   iTestStepName = _L("MM-MMF-ACLNT-U-0015-CP");
   }

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerClosePlay::~CTestStepPlayerClosePlay()
   {
   }
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerClosePlay::MapcInitComplete(TInt aError, 
                        const TTimeIntervalMicroSeconds &/*aDuration*/)
   {
   iError = aError;
   CActiveScheduler::Stop();
   }

void CTestStepPlayerClosePlay::MapcPlayComplete(TInt aError)
   {
   iError = aError;
   CActiveScheduler::Stop();
   }

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
TVerdict CTestStepPlayerClosePlay::DoTestStepPreambleL(void)
   {
    // this installs the scheduler
    TVerdict verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

   // Printing to the console and log file
    INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0015-CP"));
    INFO_PRINTF1(_L("This test will call Play()->Close()->Play() and checks if the audioplayerutility is in stopped state after calling Close()"));

    iError = KErrNone;

    if(!GetStringFromConfig(_L("SectionOne"), _L("AudioPlayFName1"), iFileName))
      {
       return EInconclusive;
      }
    return verdict;
   }

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
TVerdict CTestStepPlayerClosePlay::DoTestStepPostambleL(void)
   {
   //[ Destroy the scheduler ]
   return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
   }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return   "TVerdict"
 *         The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepPlayerClosePlay::DoTestStepL()
   {
   TInt err = KErrNone;
   TInt state = 0;

   CMMFMdaAudioPlayerUtility* player = CMMFMdaAudioPlayerUtility::NewL(*this);

   if (player == NULL ||
       iError != KErrNone)
      {
       return EInconclusive;
      }

   err = GetAudPlayerState(player, state);
   if ( err != KErrNone || 
       state != CMMFMdaAudioPlayerUtility::EStopped )
      {
      return EInconclusive;
      }

   CleanupStack::PushL(player);

   TRAP(err,player->OpenFileL(iFileName));
   if (err != KErrNone)
      {
      CleanupStack::PopAndDestroy(player);
      return EFail;
      }
   CActiveScheduler::Start();

   INFO_PRINTF1(_L("Calling Play()"));
   player->Play();

   INFO_PRINTF1(_L("Calling Close()"));
   player->Close(); 

   // AudioPlayerUtility should be in EStopped state after calling Close().
   err = GetAudPlayerState(player, state);
   if ( err != KErrNone || 
       state != CMMFMdaAudioPlayerUtility::EStopped )
      {
      CleanupStack::PopAndDestroy(player);
      return EFail;
      }

   TRAP(err,player->OpenFileL(iFileName));
   if (err != KErrNone)
      {
      CleanupStack::PopAndDestroy(player);
      return EFail;
      }
   CActiveScheduler::Start();

   INFO_PRINTF1(_L("Calling Play()"));
   player->Play();

   // AudioPlayerUtility should now be in EPlaying state.
   err = GetAudPlayerState(player, state);
   CleanupStack::PopAndDestroy(player);
   if ( err != KErrNone || 
       state != CMMFMdaAudioPlayerUtility::EPlaying )
      {
      return EFail;
      }

   INFO_PRINTF1(_L("finished with this test step"));
   
   // test steps return a result
   return EPass;
   }



//----- Test for WAV  IMAADCPM Formats -----

/**
 *
 * Static constructor for CTestStepPlayerPlayWav.
 *
 *
 * @return   "CTestStepPlayerPlayWav*"
 *         The constructed CTestStepPlayerPlayWav
 *
 * @xxxx
 * 
 */
CTestStepPlayerPlayWav* CTestStepPlayerPlayWav::NewL()
   {
   CTestStepPlayerPlayWav* self = new(ELeave) CTestStepPlayerPlayWav;
   return self;
   }

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepPlayerPlayWav::CTestStepPlayerPlayWav() : iHasPlayed(EFalse)
   {
   // store the name of this test case
   // this is the name that is used by the script file
   iTestStepName = _L("MM-MMF-ACLNT-U-0854-HP");
   }

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerPlayWav::~CTestStepPlayerPlayWav()
   {
   }
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerPlayWav::MapcInitComplete(TInt aError, 
                        const TTimeIntervalMicroSeconds &/*aDuration*/)
   {
   iError = aError;
   CActiveScheduler::Stop();
   }

void CTestStepPlayerPlayWav::MapcPlayComplete(TInt aError)
   {
   iError = aError;
   if (iError == KErrNone)
      {
      iHasPlayed = ETrue;
      }
   CActiveScheduler::Stop();
   }

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return   "TVerdict"
 *         The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepPlayerPlayWav::DoTestStepL()
   {
   // Printing to the console and log file
   INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0854-HP"));
   INFO_PRINTF1(_L("This test will play the Wav IMAADPCM formats."));
   
   TInt err = KErrNone;
   TInt state = 0;
   TVerdict result = EPass;
      
   RArray<TPtrC> array;
   array.Append(_L("IMAADPCM4Bit8KHzMono.wav"));
   array.Append(_L("IMAADPCM 4-Bit,8KHz,Mono"));
   array.Append(_L("IMAADPCM4Bit11KHzMono.wav"));
   array.Append(_L("IMAADPCM 4-Bit,11KHz,Mono"));
   array.Append(_L("IMAADPCM4Bit22KHzMono.wav"));
   array.Append(_L("IMAADPCM 4-Bit,22KHz,Mono"));
   array.Append(_L("IMAADPCM4Bit8KHzMono512Block.wav"));
   array.Append(_L("IMAADPCM 4-Bit,8KHz,Mono"));
         
   CMMFMdaAudioPlayerUtility* player = CMMFMdaAudioPlayerUtility::NewL(*this);

   if (player == NULL ||
       iError != KErrNone)
      {
       return EInconclusive;
      }
   CleanupStack::PushL(player);
   TInt count = array.Count();
   for (TInt iter=0; iter<count; iter+=2)
      {
      TBuf<256> fileBuf;
      fileBuf.Append(KFilePath);
      // Append the file name
      fileBuf.Append(array[iter]);
      
      INFO_PRINTF2(_L("Opening file = %S"), &fileBuf);
      INFO_PRINTF2(_L("File info = %S"), &array[iter+1]);
   
      TRAP(err,player->OpenFileL(fileBuf));
      // If the opening fails, save the test result as fail and continue looping
      if (err != KErrNone)
         {
         INFO_PRINTF3(_L("Opening file %S failed,error = %d"),&fileBuf,err);
         result = EFail;
         continue;
         }
      CActiveScheduler::Start();

      player->Play();
      
      CActiveScheduler::Start();
      
      // Close the file before opening the second file
      player->Close();
      
      err = GetAudPlayerState(player, state);
      if (err != KErrNone ||
         state != CMMFMdaAudioPlayerUtility::EStopped ||
          !iHasPlayed ||
         iError != KErrNone)
         {
         INFO_PRINTF4(_L("Playing file %S failed,error = %d iHasPlayed = %d"),&fileBuf,err,iHasPlayed);
         result = EFail;
         }   
      }
   CleanupStack::PopAndDestroy(player);
   
   INFO_PRINTF1(_L("Finished with this test step"));
   // test steps return a result
   return result;
   }

RAudioPlayCompleteTest* RAudioPlayCompleteTest::NewL(const TDesC& aStepName, const TDesC& aFileName)
	{
	RAudioPlayCompleteTest* self = new(ELeave)RAudioPlayCompleteTest(aStepName, aFileName);
	return self;
	}

RAudioPlayCompleteTest::RAudioPlayCompleteTest(const TDesC& aStepName, const TDesC& aFileName):
	iFileName (aFileName), iUtility(NULL), iPlayStartTime(0)
	{
	iTestStepName = aStepName;	
	}
	
void RAudioPlayCompleteTest::KickoffTestL()
	{
	INFO_PRINTF1(_L("INC103000:V&V_PATOS_3_W950i_error when audio file is finished"));
	iUtility = CMdaAudioPlayerUtility::NewL(*this);
	iUtility->OpenFileL(iFileName);	
	}

void RAudioPlayCompleteTest::CloseTest()
	{
	delete iUtility;
	iUtility = NULL;
	}

void RAudioPlayCompleteTest::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration)
	{
	if(aError != KErrNone)
		{
		StopTest(aError);		
		}
	else
		{
		iDuration = aDuration;
		iPlayStartTime.UniversalTime();
		iUtility->Play();
		}
	}

void RAudioPlayCompleteTest::MapcPlayComplete(TInt aError)
	{
	if(aError != KErrNone)	
		{
		INFO_PRINTF1(_L("Got play error"));
		StopTest(aError);
		}
	else
		{
		TTime curTime;
		curTime.UniversalTime();
		//add margin of 1 sec
		curTime += TTimeIntervalMicroSeconds(1000000);
		TTimeIntervalMicroSeconds playDuration(curTime.MicroSecondsFrom(iPlayStartTime));
		if(playDuration < iDuration)
			{
			INFO_PRINTF3(_L("Clip is not played till the end - playDuration %d iDuration %d"),
						 TInt(playDuration.Int64()), TInt(iDuration.Int64()));
			StopTest(EFail);
			}
		else
			{
			StopTest(EPass);
			}
		}		
	}
	
	
// -------------------- Open a corrupt wav file, which has sampling rate 0xFFFFFFFF --------------------

RAudioOpenCorrupt* RAudioOpenCorrupt::NewL(const TDesC& aStepName, const TDesC& aFileName)
	{
	RAudioOpenCorrupt* self = new(ELeave)RAudioOpenCorrupt(aStepName, aFileName);
	return self;
	}

RAudioOpenCorrupt::RAudioOpenCorrupt(const TDesC& aStepName, const TDesC& aFileName):
	iFileName (aFileName), iUtility(NULL)
	{
	iTestStepName = aStepName;	
	}
	
void RAudioOpenCorrupt::KickoffTestL()
	{
	INFO_PRINTF1(_L("PDEF131534 SPPR_PERF: Music player crashes when opening corrupted wav file"));
	iUtility = CMdaAudioPlayerUtility::NewL(*this);
	iUtility->OpenFileL(iFileName);	
	}

void RAudioOpenCorrupt::CloseTest()
	{
	delete iUtility;
	iUtility = NULL;
	}

void RAudioOpenCorrupt::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	if(aError != KErrNotSupported)
		{
		if(aError==KErrNone)
			{
			INFO_PRINTF1(_L("Received KErrNone while expecting KErrNotSupported"));
			StopTest(EFail);
			}
		else
			{
			StopTest(aError);
			}
		}
	else
		{
		INFO_PRINTF2(_L("Recevied %d as expected"),aError);
		StopTest(EPass);
		}
	}

void RAudioOpenCorrupt::MapcPlayComplete(TInt /*aError*/)
	{
	ASSERT(0);
	}


RAudioPlayPanicTest* RAudioPlayPanicTest::NewL(const TDesC& aStepName, const TDesC& aFileName)
	{
	RAudioPlayPanicTest* self = new(ELeave)RAudioPlayPanicTest(aStepName, aFileName);
	return self;
	}

RAudioPlayPanicTest::RAudioPlayPanicTest(const TDesC& aStepName, const TDesC& aFileName):
	RAudioPlayCompleteTest(aStepName, aFileName)
	{
	
	}
	
void RAudioPlayPanicTest::KickoffTestL()
	{
	INFO_PRINTF1(_L("DEF127630:Fix for INC108007 appears to close "));
	iUtility = CMdaAudioPlayerUtility::NewL(*this);
	iUtility->OpenFileL(iFileName);
	}
	
void RAudioPlayPanicTest::MapcPlayComplete(TInt aError)
	{
	if(aError != KErrServerTerminated)
		{
		INFO_PRINTF2(_L("Not received KErrDied as expected[%d]"), aError);
		StopTest(aError);
		}
	else
		{
		StopTest(EPass);
		}		
	}

