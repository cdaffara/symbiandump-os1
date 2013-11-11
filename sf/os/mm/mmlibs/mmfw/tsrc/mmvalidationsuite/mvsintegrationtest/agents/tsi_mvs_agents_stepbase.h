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

#ifndef TSI_MVS_AGENT_STEP_H
#define TSI_MVS_AGENT_STEP_H


#include <e32base.h>
#include <badesca.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <utf.h>

#include <testframework.h>
#include <mdaaudiosampleplayer.h>
#include <mdaaudiosampleeditor.h>
#include <mdaaudiotoneplayer.h>
#include <mdaaudiooutputstream.h>
#include <mmfformatimplementationuids.hrh>
#include <mmfcontrollerimplementationuids.hrh>
#include <mda/common/resource.h>
#include <mda/common/audio.h>
#include <mda/common/gsmaudio.h>

#include "tsi_mvs_agents_suite.h"
#include <mvs/agentsutility.h>
#include <mvs/audioplayagent.h>
#include <mvs/audiorecordagent.h>
#include <mvs/videoplayagent.h>
#include <mvs/videorecordagent.h>
#include "u32hal.h"


// Constant declarations

_LIT(KMVSAgentsLogPath, "c:\\Logs\\LogMVSappUi\\LogFile.txt");

const TInt KOneSecond        = 1000000;    // Used in a time out function, 1 second (in microSeconds)#
const TInt KTwoSeconds        = KOneSecond*2;
const TInt KTenSeconds        = 10000000;    // Used in a time out function, 10 seconds (in microSeconds)
const TInt KFiveSeconds        = 5000000;    // 5 seconds (in microSeconds)
const TInt KFifteenSeconds    = 15000000;    // 15 seconds (in microSeconds)
const TInt KExpectedDeviation= 3000000; // 1.5 second tolerance for timed tests
const TInt KFilNameBuf        = 256;        // Max size for file name
const TInt KIniNameBuf        = 25;        // Max size for INI file section and key names
const TInt KMaxPlugin        = 4;        // Max number of plug-ins in the system
const TInt KMaxDateLength    = 25;        // Max size for date time string to check in log-file (20 is enough)
const TInt KBufMimeTypeGranularity = 4; // Max size for Mime types
const TInt KNameBufSize            = 25;

// Uids of controllers for test use.
const TUid KTestOggPlayControllerUid = { 0x01004304 };
const TUid KTestOggRecordControllerUid = { 0x10273771 };
const TUid KTestMmfAudioControllerUid = { 0x101F5022 };
const TUid KTestScbVideoControllerUid = { 0x101FE477 };
const TUid KTestAviPlayControllerUid = { 0x102737A5 };
const TUid KTestAviRecordControllerUid = { 0x102737E1 };


// Display names of play controllers for test
_LIT(KTestOggPlayControllerName, "OggPlayController");
_LIT(KTestOggRecordControllerName, "OggRecordController");
_LIT(KTestAviPlayControllerName, "AviVideoPlayController");
_LIT(KTestAviRecordControllerName, "AviVideoRecordController");
_LIT(KTestScbVideoControllerName, "SCB Video Controller");

// Forward class declarations.
class CTestSuite;
class CTestSuiteMVSAgent;

// Enum to decide Agent has generated the event.
enum TMvsTestAgents
    {
    ETestAudioPlay = 0,
    ETestAudioRecord,
    ETestVideoPlay,
	ETestVideoPlay2,        
    ETestVideoRecord
    };

// Enum to decide the asynchronous events
enum TMvsTestAgentEvents
    {
    EMvsIdle = 0,
    EAudPlayerOpenComplete,
    EAudPlayerPlayComplete,
    EAudPlayerPlayComplete1,
    EAudRecorderOpenComplete,
    EAudRecorderPlayProgress,
    EAudRecorderPlayComplete,
    EAudRecorderRecordProgress,
    EAudRecorderRecordComplete,
    EVidPlayerOpening,
    EVidPlayerOpenComplete,
    EVidPlayerPlayComplete,
    EVidPlayerPlayPaused,
    EVidRecorderOpening,
    EVidRecorderOpenComplete,
    EVidRecorderRecording,
    EVidRecorderPaused,
    EVidRecorderRecordComplete,
    EErrInUseVideoPlayer,
    EErrInUseVideoPlayer1,
    EVidGeneralErr,
    EVidPlayerOpening2,
    EVidPlayerOpenComplete2,
    EVidPlayerPlayComplete2,
    EVidPlayerPlayPaused2,
    EVidPlayerNoNotificationReceived,
   	EVidPlayerResourceAvailable,
   	EVidPlayerResourceAvailable2
    };
      
const TInt KMaxEvents = 2;
//Recent change for writable static data.

//
// Array to display which state change event occurred.
const TText* const KTestMvsAgents[] = 
    { 
    _S("AudioPlayAgent"), 
    _S("AudioRecordAgent"),
    _S("VideoPlayAgent"), 
    _S("VideoPlayAgent2"),
    _S("VideoRecordAgent")
    };
                             
// Array to display which state change event occurred.
const TText* const KTestMvsStates[] =
    {
    _S("ENotReady"),
    _S("EAudioOpening"),
    _S("EAudioOpened"),
    _S("EAudioPlaying"),
    _S("EAudioRecording"),
    _S("EAudioStopped"),
    _S("EVideoOpening"),
    _S("EVideoOpened"),
    _S("EVideoPlaying"),
    _S("EVideoRecording"),
    _S("EVideoStopped"),
    _S("EVideoPaused"),
    _S("EAudioPaused")    
    };
        
class CTestMVSCallbackSupport;
/**
 * 
 * MTestMVSClientObserver
 * Observer class to identify and pass events to the test step class
 *
 */
class MTestMVSClientObserver
    {
public :
    virtual void UpdateStateChangeTest(TMVSState aState, TInt aError, CTestMVSCallbackSupport& aTestMVSCallbackSupport) = 0;
    virtual void MvsResourceNotificationTest(const TDesC8& aNotificationData, CTestMVSCallbackSupport& aTestMVSCallbackSupport)=0;
    };

/**
 * 
 * CTestMVSCallbackSupport
 * Support class to pass the events to the test step class using the MTestMVSClientObserver
 *
 */
class CTestMVSCallbackSupport : public CBase, public MMVSClientObserver
    {
public :
    static CTestMVSCallbackSupport* NewL(MTestMVSClientObserver& aCallback);
    virtual void UpdateStateChange(TMVSState aState, TInt aErrorCode);
   	virtual void MvsResourceNotification(const TDesC8& aNotificationData);

protected:
    CTestMVSCallbackSupport(MTestMVSClientObserver& aCallback);
private :
    MTestMVSClientObserver& iCallback;
    };

/**
 * CStopActiveSchedulerTimer
 * Class used by the test harness to stop the active scheduler 
 * after a timeout period
 */
class CStopActiveSchedulerTimer : public CTimer
    {
public:
    static CStopActiveSchedulerTimer* NewL();
    void StopAfter(TTimeIntervalMicroSeconds32 aTimeInterval);
private:
    CStopActiveSchedulerTimer();
    void ConstructL();
    virtual void RunL();
    };

/**
 *
 * RTestStepMVSBase
 * Description - Test class that enables MVS Audio Agent tests.
 *               It's a base class for all Audio test steps.
 *
 */
class RTestStepMVSBase : public RTestStep
    {
public:
    void SetTestSuite(const CTestSuiteMVSAgent* aTestSuite);

protected:
    //Preamble and Postamble
    virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict DoTestStepPostambleL();
    //RTestStep
    virtual TVerdict DoTestStepL() = 0;
    // Constructor
    RTestStepMVSBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    // Function to get the drive which is used for test
    void GetDriveName(TDes& aFileName);
    // Function to Create a Dir
    TInt MakeDir(const TDesC &aDirName);
    // Function to delete a file
    TInt DeleteFileL(const TDesC &aFileName);
    // Function to compare the log file
    TBool CheckLogFileL(const TDesC& aOrigLogFile, const TDesC8 &aCheckString8, const TDesC &aCheckString);
protected:    
    // Pointer to suite which owns this test
    const CTestSuiteMVSAgent*     iMVSAgentTestSuite;
    CActiveScheduler*            iAS;                // Active Scheduler
    CMVSControllerPluginInfo*    iMVSControllerPluginInfo;            // MVSControllerPluginInfo
    // INI Values
    TBuf<KIniNameBuf>            iSectName;            // Section name for retrieving filename
    TBuf<KIniNameBuf>            iKeyName;            // Key name for retrieving filename
    TFileName                    iFilename;
    };

/**
 *
 * RTestStepMVSAgent
 * Description - Test class that enables MVS Agent tests.
 *               It's a base class for all Audio / Video test steps.
 *
 */
class RTestStepMVSAgent : public RTestStepMVSBase,
                          public MTestMVSClientObserver
    {
protected:
    // Constructor
    RTestStepMVSAgent(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    //Preamble and Postamble
    virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict DoTestStepPostambleL();
    //MMVSClientObserver
    virtual void UpdateStateChangeTest(TMVSState aState, TInt aErrorCode, CTestMVSCallbackSupport& aTestMVSCallbackSupport);
    virtual void MvsResourceNotificationTest(const TDesC8& aNotificationData, CTestMVSCallbackSupport& aTestMVSCallbackSupport);    
    //RTestStep
    virtual TVerdict DoTestStepL() = 0;
    virtual TVerdict PerformTestL() = 0;
    //State Machine
    virtual void FsmL(TMvsTestAgentEvents aEventCode)=0;
    void ResetState();
    void PrepareState(TMVSState aExpectedState, TInt aExpectedError = KErrNone);
    // Time comparision with expected deviation
    TBool TimeComparison(const TUint aActual, const TUint aExpected, const TUint aDeviation);
    
protected:
    // Callback handler class instances
    CTestMVSCallbackSupport* iAudioPlayAgentCallback;
    CTestMVSCallbackSupport* iAudioRecordAgentCallback;
    CTestMVSCallbackSupport* iVideoPlayAgentCallback;
    CTestMVSCallbackSupport* iVideoRecordAgentCallback;
    // Hold the type of agent
    TMvsTestAgents iActiveAgent;
    TMvsTestAgents iCallbackAgent;
    // State and Error Values
    TInt                        iCallbackError;
    TMVSState                    iPreviousState;
    TMVSState                    iCallbackState;
    TMvsTestAgentEvents            iCallbackEvent;
    TInt                        iExpectedError;
    TMVSState                    iExpectedState;
    TInt                        iError;
    // Test specific flag variables
    TBool iEndOfPlay;
    };

/**
 *
 * RTestStepMVSPluginInfo
 * Description - Test class that enables MVS Agent tests.
 *               It's a base class for all test steps.
 *
 */
class RTestStepMVSPluginInfo : public RTestStepMVSBase
    {
protected:
    //Preamble and Postamble
    virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict DoTestStepPostambleL();
    //RTestStep
    virtual TVerdict DoTestStepL() = 0;
    virtual TVerdict PerformTestL() = 0;
    // Constructor
    RTestStepMVSPluginInfo(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid aExpectedMediaType, const TUid aControllerUid);
    
protected:
    // Arrays to hold plug-in information
    CDesCArrayFlat* iPluginArray; //Array of supported plugins
    CDesCArrayFlat* iExtArray;
    CDesCArrayFlat* iCodecArray;
    RArray<TUid> iUidArray;
    
    // Hold Error Information
    TInt iExpectedError;
    TInt iError;
    TUid iExpectedMediaType;
    TUid iControllerUid;
    TBool iMediaType; 
    };


/**
 *
 * RTestStepMVSAudioAgent
 * Description - Test class that enables MVS Audio Agent tests.
 *               It's a base class for all Audio test steps.
 *
 */
class RTestStepMVSAudioAgent : public RTestStepMVSAgent
    {
protected:
    //Preamble and Postamble
    virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict DoTestStepPostambleL();
    //RTestStep
    virtual TVerdict DoTestStepL();
    //State Machine
    virtual void FsmL(TMvsTestAgentEvents aEventCode) = 0;
    // Constructor
    RTestStepMVSAudioAgent(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    
protected:
    CMVSAudioPlayAgent*        iMVSAudioPlayAgent;        // Audio Play Agent
    CMVSAudioRecordAgent*    iMVSAudioRecordAgent;    // Audio Record Agent
    };

/**
 *
 * RTestStepMVSVideoAgent
 * Description - Test class that enables MVS Video Agent tests.
 *               It's a base class for all Video test steps.
 *
 */
class RTestStepMVSVideoAgent : public RTestStepMVSAgent
    {

public:
    // timer handling
    virtual void Finished();
    
protected:
    //Preamble and Postamble
    virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict DoTestStepPostambleL();
    //RTestStep
    virtual TVerdict DoTestStepL();
    //State Machine
    virtual void FsmL(TMvsTestAgentEvents aEventCode) = 0;

    void OpenFileL(CMVSVideoPlayAgent& aVideoPlayAgent, const TDesC& aFileName);
    virtual void PlayL(CMVSVideoPlayAgent& aVideoPlayAgent);        
    virtual void PlayL(CMVSVideoPlayAgent& aVideoPlayAgent,
                       const TRect& aVideoExtent,
                       const TRect& aWindowClipping);    

    // Constructor
    RTestStepMVSVideoAgent(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName);
    
protected:
    CMVSVideoPlayAgent*     iMVSVideoPlayAgent;     // Video Play Agent
    CMVSVideoRecordAgent*   iMVSVideoRecordAgent;   // Video Record Agent
    TBool                   iUseNonGceController;    
    
//#ifdef __WINS__
    // TO create the Window Server related objects
    void InitWservL();
    void DeInitWserv();
    // Window Server related objects
    CWsScreenDevice* iScreen;
    RWindow* iWindow;
    RWsSession iWs;
    RWindowGroup iRootWindow;
//#endif // __WINS__
    };
    
/**
 *
 * RTestMVSVideoAgentSetScrIdAndPlayBase
 * Description - Test class that enables MVS video Agent tests.
 *               It's a base class for Secondary video test steps.
 *
 */
class RTestMVSVideoAgentSetScrIdAndPlayBase : public RAsyncTestStep,
												public MTestMVSClientObserver
{
public:
    RTestMVSVideoAgentSetScrIdAndPlayBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError,TInt aScreenNumber);
    void SetTestSuite(const CTestSuiteMVSAgent* aTestSuite);

   // from RAsyncTestStep
	void KickoffTestL();
	virtual void DoKickoffTestL() = 0;  //-- for the derived class to implement
	void CloseTest();
	void Finished();

	//State Machine Functions
	virtual void FsmL(TMvsTestAgentEvents aEventCode) = 0;
	TBool ValidateEvent();
	void PrepareState(TMVSState aExpectedState, TInt aExpectedError);

	//MMVSClientObserver
	virtual void UpdateStateChangeTest(TMVSState aState, TInt aErrorCode, CTestMVSCallbackSupport& aTestMVSCallbackSupport);
	virtual void MvsResourceNotificationTest(const TDesC8& aNotificationData, CTestMVSCallbackSupport& aTestMVSCallbackSupport);

protected:

 	// TO create the Window Server related objects
    void InitWservL();
    void DeInitWserv();

    void OpenFileL(CMVSVideoPlayAgent& aVideoPlayAgent, const TDesC& aFileName);
    void PlayL(CMVSVideoPlayAgent& aVideoPlayAgent);        

    // Window Server related objects
    CWsScreenDevice* 			iScreen;
    RWindow* 					iWindow;
    RWsSession 					iWs;
    RWindowGroup 				iRootWindow;
  	// Window Screen End   
  
 	// Function to get the drive which is used for test
    void GetDriveName(TDes& aFileName);
    
    TInt 						iScreenNumber; // Stores screen number
    TInt 						iCurrentScreenNumber; // Stores currently active screen number
    
    TBuf<KNameBufSize> 			iSectName;            // Section name for retrieving filename
    TBuf<KNameBufSize> 			iKeyName;            // Key name for retrieving filename
    TFileName   				iFileName; 
    TInt 						iTestExpectedError;
    TInt 						iExpectedError;
    const CTestSuiteMVSAgent*   iMVSAgentTestSuite;
    CMVSVideoPlayAgent*        	iMVSVideoPlayAgent; 
    TBool                       iUseNonGceController;
    CTestMVSCallbackSupport* 	iVideoPlayAgentCallback;
    TMvsTestAgentEvents         iCallbackEvent;
    TMvsTestAgentEvents         iExpectedEvent;
    TMVSState                   iExpectedState;
    TMVSState                   iPreviousState;
    TMVSState                   iCallbackState;
    RFs 						iFs;
    TInt                        iError;	
	};
    

/**
 *
 * RTestStepMVSAgentsAPRBase
 * Description - Test class that enables MVS video Agent tests.
 *               It's a base class for APR test steps.
 *
 */
class RTestStepMVSAgentsAPRBase : public RAsyncTestStep,
							 public MTestMVSClientObserver
    {
public:
	RTestStepMVSAgentsAPRBase(const TDesC& aStepName, const TDesC& aSectionName);
	void SetTestSuite(const CTestSuiteMVSAgent* aTestSuite);
	
	// from RAsyncTestStep
	void KickoffTestL();
	virtual void DoKickoffTestL() = 0;  //-- for the derived class to implement
	void CloseTest();
	
	//State Machine Functions
	virtual void FsmL(TMvsTestAgentEvents aEventCode, TInt aError) = 0;
	TBool ValidateEvent(TMvsTestAgentEvents aEventCode, TInt aError);
	TInt AnalyseInput(TMvsTestAgentEvents aEventCode, TInt aError);
	void PrepareState(TMVSState aExpectedState,TMvsTestAgentEvents aEventCode, TInt aExpectedError, TMvsTestAgents aActiveAgent, TInt aEventIndex=1);
	//MMVSClientObserver
	virtual void UpdateStateChangeTest(TMVSState aState, TInt aErrorCode, CTestMVSCallbackSupport& aTestMVSCallbackSupport);
	virtual void MvsResourceNotificationTest(const TDesC8& aNotificationData, CTestMVSCallbackSupport& aTestMVSCallbackSupport);
	virtual TInt SetCacheSize();
	virtual TInt CheckCacheError(TInt aError);
	virtual TInt RevertCacheSize();

protected:

    // TO create the Window Server related objects
	void InitWservL();
	void DeInitWserv();

    void OpenFileL(CMVSVideoPlayAgent& aVideoPlayAgent, const TDesC& aFileName);
    void PlayL(CMVSVideoPlayAgent& aVideoPlayAgent);        

	// Window Server related objects
	CWsScreenDevice* 			iScreen;
	RWindow* 					iWindow;
	RWsSession 					iWs;
	RWindowGroup 				iRootWindow;
    // Window Screen End   
    
	// Function to get the drive which is used for test
    void GetDriveName(TDes& aFileName);
    // Function to Create a Dir
    TInt MakeDir(const TDesC &aDirName);
    // Function to delete a file
    TInt DeleteFileL(const TDesC &aFileName);
    // Function to compare the log file
    TBool CheckLogFileL(const TDesC& aOrigLogFile, const TDesC8 &aCheckString8, const TDesC &aCheckString);
    
    // Pointer to suite which owns this test
    const CTestSuiteMVSAgent*   iMVSAgentTestSuite;
    TBool                       iUseNonGceController;
    CMVSVideoPlayAgent*        	iMVSVideoPlayAgent1; 
    CMVSVideoPlayAgent*        	iMVSVideoPlayAgent2;
    CMVSAudioPlayAgent*        	iMVSAudioPlayAgent;
    
    CTestMVSCallbackSupport* 	iAudioPlayAgentCallback;
    CTestMVSCallbackSupport* 	iVideoPlayAgentCallback1;
    CTestMVSCallbackSupport* 	iVideoPlayAgentCallback2;
        
    // Hold the type of agent			
    TMvsTestAgents 				iActiveAgent;
    TMvsTestAgents 				iCallbackAgent;
    
    // INI Values
    TFileName        			iFilename1;
    TFileName        			iFilename2;
    TFileName        			iFilename3;
    // State and Error Values
    TInt                    	iCallbackError;
    TMVSState                   iPreviousStateAP;
    TMVSState                   iPreviousStateVP1;
    TMVSState                   iPreviousStateVP2;
    TMVSState                   iCallbackStateAP;
    TMVSState                   iCallbackStateVP1;
    TMVSState                   iCallbackStateVP2;
    TMvsTestAgentEvents         iCallbackEvent;
    TMvsTestAgentEvents			iExpectedEvent[KMaxEvents];
    
    TInt                        iExpectedError[KMaxEvents];
    TMVSState                   iExpectedState;
    TInt                        iError;
    TMVSState					iState;		
public:
	static TInt TimerCallback(TAny*);
	virtual void DoTimerCallback();
	CPeriodic					*iTimer;
	TFileName 					iSectionName;	
	TInt						iTestExpectedError;
	RFs 						iFs;
	TInt 						iExpEventCount;
	TInt 						iActEventCount;
	TBool 						iCanStateChange;	
	TBool 						iRegistered;
	TBool 						iSetPriority;
	TBool 						iFlag;
	TBool						iSecPlay;
    };

#endif
