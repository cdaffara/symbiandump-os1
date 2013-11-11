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
//


#ifndef TSU_MMF_AVICTRLPLAY_H
#define TSU_MMF_AVICTRLPLAY_H

#include "tsu_mmf_avictrl_teststep.h"

#ifdef SYMBIAN_BUILD_GCE
#include <mmf/common/mmfvideosurfacecustomcommands.h>
#endif // SYMBIAN_BUILD_GCE
#define KMaxChunks 50 //The maximum chunks that can be allocated
					  //Has been adjusted such that we dont need higher than this

/**
This class tests playing an avi file completely and checks for 
playcompletion event with KErrNone from the aviplaycontroller.
*/ 
class RTestStepAviPlayCtrlPlay : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlPlay(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL(  );
	};
/**
This class tests setting the volume on AviPlayController in Stopped, Primed and Playing states
and checks if the volume set is same as the volume got.
*/  
class RTestStepAviPlayCtrlVolume : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlVolume(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL(  );
	};

/**
This class tests setting the PlayBalance on AviPlayController in Stopped, Primed and Playing  states
and checks if the PlayBalance set is same as the PlayBalance got.
*/  
class RTestStepAviPlayCtrlPlayBalance : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlPlayBalance(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};
	
/**
This class tests the duration of the file using AviPlayController.
*/  
class RTestStepAviPlayCtrlDuration : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlDuration(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL(  );
	};
	
 /**
This class tests pause functionality of AviPlayController in Stopped, Primed and Playing  states and checks
 if the play starts from the current position during playing state.
*/ 
class RTestStepAviPlayCtrlPause : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlPause(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};

/**
This class tests stop functionality of AviPlayController in Stopped, Primed and Playing 
states and checks if the play starts from the initial position during playing state
*/  
class RTestStepAviPlayCtrlStop : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlStop(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};
	
/**
This class tests to set and get the crop region using AviPlayController.
*/ 
class RTestStepAviPlayCtrlSetGetCropRegion : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlSetGetCropRegion(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};

/**
This class tests to set and get the framerate of the file using AviPlayController.
 */ 
class RTestStepAviPlayCtrlSetGetFrameRate : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlSetGetFrameRate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};

/**
This class tests to get the frame size using AviPlayController.
*/ 
class RTestStepAviPlayCtrlGetFrameSize : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlGetFrameSize(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};

/**
This class tests to get the audio codec installed in the file using AviPlayController.
*/  
class RTestStepAviPlayCtrlGetAudioDataType : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlGetAudioDataType(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};

/**
This class tests to get the video codec installed in the file using AviPlayController.
*/ 
class RTestStepAviPlayCtrlGetVideoDataType : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlGetVideoDataType(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};

/**
This class tests metadata support of the AviPlayController. 
*/  
class RTestStepAviPlayCtrlMetaData : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlMetaData(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};

/**
This class tests Rotation api of the AviPlayController. 
*/  
class RTestStepAviPlayCtrlSetGetRotation : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlSetGetRotation(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};
/**
This class tests Rotation api of the AviPlayController. 
*/ 
class RTestStepAviPlayCtrlSetGetScaling : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlSetGetScaling(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};

/**
This class tests Position api of the AviPlayController. 
*/  
class RTestStepAviPlayCtrlSetPosition : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlSetPosition(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};

/**
This class tests getting video bit rate of the AviPlayController. 
*/  
class RTestStepAviPlayCtrlGetVideoBitrate : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlGetVideoBitrate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};

/**
This class tests getting audio bit rate of the AviPlayController. 
*/  
class RTestStepAviPlayCtrlGetAudioBitrate : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlGetAudioBitrate(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL( );
	};	
	
/**
This class tests for direct screen access of aviplaycontroller
*/  
class RTestStepAviCtrlDirectScreenAccess : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviCtrlDirectScreenAccess(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL(void);
	};
	
/**
This class tests for Play window property for .avi play controller
*/ 	
class RTestStepAviCtrlSetPlayWindow : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviCtrlSetPlayWindow(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL(void);
	};

//Negative Tests

/**
This class tests opening an invalid file using AviPlayController.
*/ 
class RTestStepAviCtrlPlayInvalid : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviCtrlPlayInvalid(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TVerdict  DoTestStepL(void);
	};

/**
This class tests for volume and balance when audio is not enabled in the file.
*/ 
class RTestStepAviCtrlNotAudioEnabled : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviCtrlNotAudioEnabled(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict  DoTestStepL(void);
	};

/**
This class tests playing an avi file on primary or secondary screen depending on the screen number set during its  
construction and checks for playcompletion event with KErrNone from the aviplaycontroller.
*/ 
class RTestStepAviPlayCtrlSetScrnPlay : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlSetScrnPlay(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid, TInt aScreenNumber);
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TVerdict  DoTestStepL(void);
protected:
	//from MMMFControllerEventMonitorObserver
	void HandleEvent(const TMMFEvent& aEvent);
public:
	TInt iScreenNumber;
	};

/**
This class tests setting an invalid screen id on AviPlayController.
*/ 
class RTestStepAviPlayCtrlSetInvalidScrn : public RTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlSetInvalidScrn(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid, TInt aScreenNumber);
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TVerdict  DoTestStepL(void);
public:
	TInt iScreenNumber;
	};

/**
This class tests setting the invalid screen id after adding the datasink to the AviPlayController.
It resets the controller and adds the datasink for the secondtime and expects KErrNotSupported.
*/ 
class RTestStepAviPlayCtrlAddDataSinkTwice : public RTestStepAviPlayCtrlSetScrnPlay
	{
public:
	RTestStepAviPlayCtrlAddDataSinkTwice(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid, TInt aScreenNumber);
	virtual TVerdict DoTestStepPreambleL(void);
	virtual TVerdict  DoTestStepL(void);
	};


/**
This class tests playing an avi file when the xvid decoder finds low memory
conditions. The codec will inform back to the controlller that a fatal error
occured. After that, the memory conditions are returned back to normal and the
video playback is restarted to demonstrate that DevVideo is recreated after the
failure and that the video is played succesfully.
*/
class RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap :
	public RAsyncTestStepAviCtrlPlayBase,
	public MMMFControllerEventMonitorObserver

	{
public:
	static const TUint KMaxFreeRamForTest = 0x4fff00; // 5-32k MB, this is adjusted
													  // to leave just enough
													  // memory for the xvid
													  // decoder to fail

public:
	RTestStepAviPlayNotEnoughMemoryForXvidCodecHeap(
		const TDesC& aTestName, const TDesC& aSectName,
		const TDesC& aKeyName, TUid aControllerUid);

	// from RAsyncTestStep
	void KickoffTestL();
	void CloseTest();

protected:

	// from MMMFControllerEventMonitorObserver
	void HandleEvent(const TMMFEvent& aEvent);

	// Call backs
	static TInt AllocChunkCallBack(TAny* aPtr);
	void DoAllocChunkCallBack();
	static TInt PrimeControllerCallBack(TAny* aPtr);
	void DoPrimeControllerCallBack();
	static TInt PlayControllerCallBack(TAny* aPtr);
	void DoPlayControllerCallBack();
	static TInt ReturnChunckAndStopControllerCallBack(TAny* aPtr);
	void DoReturnChunckAndStopControllerCallBack();


protected:

	// Set of states for simple state machine
	enum TTestState
		{
		EStateDefault,
		EStateLoadKickoffTestLComplete,
		EStateAllocChunkCallBackComplete,
		EStatePrimeControllerCallBackComplete1,
		EStatePlayControllerCallBackComplete1,
		EStateReturnChunckAndStopControllerCallBackComplete,
		EStatePrimeControllerCallBackComplete2,
		EStatePlayControllerCallBackComplete2
		};

	TTestState iTestState;
	RChunk iRChunk[KMaxChunks];
	TInt iNumAllocatedChunks;
	CAsyncCallBack* iKickoff01;
	CAsyncCallBack* iKickoff02;
	CAsyncCallBack* iKickoff03;
	CAsyncCallBack* iKickoff04;
	CAsyncCallBack* iKickoff05;
	CAsyncCallBack* iKickoff06;
	};

/**
This class tests OOM situation on several controller functions.
(e.g. AddSource/Sink)
*/ 
class RTestStepAviPlayCtrlMemAlloc1 : public RTestStepAviCtrlBase
	{
public:
	RTestStepAviPlayCtrlMemAlloc1(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict DoTestStepL(  );
protected:
	//from MMMFControllerEventMonitorObserver
	void HandleEvent(const TMMFEvent& aEvent);
	};

/**
This class tests OOM situation on several controller functions.
(e.g. Prime)
*/ 
class RTestStepAviPlayCtrlMemAlloc2 : public RTestStepAviCtrlBase
	{
public:
	RTestStepAviPlayCtrlMemAlloc2(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict DoTestStepL(  );
protected:
	//from MMMFControllerEventMonitorObserver
	void HandleEvent(const TMMFEvent& aEvent);
	};

/**
This class tests OOM situation on several controller functions.
(e.g. Prime)
*/ 
class RTestStepAviPlayCtrlMemAlloc3 : public RTestStepAviCtrlBase
	{
public:
	RTestStepAviPlayCtrlMemAlloc3(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	virtual TVerdict DoTestStepL(  );
protected:
	//from MMMFControllerEventMonitorObserver
	void HandleEvent(const TMMFEvent& aEvent);
};

#ifdef SYMBIAN_BUILD_GCE
// Surface support positive tests:

/**
Test surface support in the AVI play controller.  It enables surfaces, adds a rendering 
target display, gets the surface parameter, updates the display and finally removes the display.
*/
class RTestStepAviPlayCtrlSurfaceSupport : public RAsyncTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlSurfaceSupport(const TDesC& aTestName, const TDesC& aSecName, const TDesC& aKeyName, TUid aControllerUid);
	
	// from RAsyncTestStep
	virtual void KickoffTestL();
	virtual void CloseTest();
	
	// from MMMFControllerEventMonitorObserver
	virtual void HandleEvent(const TMMFEvent& aEvent);

private:
	virtual TVerdict CheckTestResult();
	
	// Call back functions
	static TInt UseSurfacesCallBack(TAny* aPtr);
	void DoUseSurfacesCallBack();
	
	static TInt PrimeControllerCallBack(TAny* aPtr);
	void DoPrimeControllerCallBack();
	
	static TInt PlayControllerCallBack(TAny* aPtr);
	void DoPlayControllerCallBack();
	
private:
	TBool iSurfaceCreated;
	TSurfaceId iSurfaceId;
	
	CAsyncCallBack* iKickoffUseSurfaces;
	CAsyncCallBack* iKickoffUseSurfaces2;
	CAsyncCallBack* iKickoffPrimeController;
	CAsyncCallBack* iKickoffPlayController;
	
	RMMFVideoPlaySurfaceSupportCustomCommands iVpsCommands;
	};

// Surface support negative tests:

/**
Test surface support in the AVI play controller.  We expect a failure when we call surface removed 
without first callng UseSurfaces.
*/
class RTestStepAviPlayCtrlSurfaceRemovedNoUS : public RAsyncTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlSurfaceRemovedNoUS(const TDesC& aTestName, const TDesC& aSecName, const TDesC& aKeyName, TUid aControllerUid);
	
	// from RAsyncTestStep
	virtual void KickoffTestL();
	virtual void CloseTest();

private:
	// Call back functions
	static TInt SurfaceRemovedCallBack(TAny* aPtr);
	void DoSurfaceRemovedCallBack();
	
private:
	CAsyncCallBack* iKickoffSurfaceRemoved;

	TSurfaceId iSurfaceId;
	
	RMMFVideoPlaySurfaceSupportCustomCommands iVpsCommands;
	};

/**
Test surface support in the AVI play controller.  We expect a failure when we get the surface parameters 
without first callng UseSurfaces.
*/
class RTestStepAviPlayCtrlGetSurfaceParametersNoUS : public RAsyncTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlGetSurfaceParametersNoUS(const TDesC& aTestName, const TDesC& aSecName, const TDesC& aKeyName, TUid aControllerUid);
	
	// from RAsyncTestStep
	virtual void KickoffTestL();
	virtual void CloseTest();

private:
	// Call back functions
	static TInt GetSurfaceParametersCallBack(TAny* aPtr);
	void DoGetSurfaceParametersCallBack();
	
private:
	CAsyncCallBack* iKickoffGetSurfaceParameters;
	
	RMMFVideoPlaySurfaceSupportCustomCommands iVpsCommands;
	};

/**
Test surface support in the AVI play controller.  We expect a failure when we call surface removed 
after callng UseSurfaces with a null surface Id
*/
class RTestStepAviPlayCtrlSurfaceRemovedNullSurfaceId : public RAsyncTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlSurfaceRemovedNullSurfaceId(const TDesC& aTestName, const TDesC& aSecName, const TDesC& aKeyName, TUid aControllerUid);
	
	// from RAsyncTestStep
	virtual void KickoffTestL();
	virtual void CloseTest();

private:
	// Call back functions
	static TInt SurfaceRemovedCallBack(TAny* aPtr);
	void DoSurfaceRemovedCallBack();
	static TInt UseSurfacesCallBack(TAny* aPtr);
	void DoUseSurfacesCallBack();
	
private:
	CAsyncCallBack* iKickoffSurfaceRemoved;
	CAsyncCallBack* iKickoffUseSurfaces;
	
	TSurfaceId iSurfaceId;
	
	RMMFVideoPlaySurfaceSupportCustomCommands iVpsCommands;
	};

/**
Test surface support in the AVI play controller.  We expect a failure when we call surface removed 
after calling UseSurfaces with an invalid surface Id
*/
class RTestStepAviPlayCtrlSurfaceRemovedInvalidSurfaceId : public RAsyncTestStepAviCtrlPlayBase
	{
public:
	RTestStepAviPlayCtrlSurfaceRemovedInvalidSurfaceId(const TDesC& aTestName, const TDesC& aSecName, const TDesC& aKeyName, TUid aControllerUid);
	
	// from RAsyncTestStep
	virtual void KickoffTestL();
	virtual void CloseTest();

private:
	// Call back functions
	static TInt SurfaceRemovedCallBack(TAny* aPtr);
	void DoSurfaceRemovedCallBack();
	static TInt UseSurfacesCallBack(TAny* aPtr);
	void DoUseSurfacesCallBack();
	
private:
	CAsyncCallBack* iKickoffSurfaceRemoved;
	CAsyncCallBack* iKickoffUseSurfaces;
	
	TSurfaceId iSurfaceId;
	
	RMMFVideoPlaySurfaceSupportCustomCommands iVpsCommands;
	};

#endif // SYMBIAN_BUILD_GCE

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

/**
 * MM-MMF-AVICTRL-U-0100-HP
 */
class RAsyncTestSubtitlePlay : public RAsyncTestStepSubtitleBase
	{
public:
	RAsyncTestSubtitlePlay(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	
private:
	// From RAsyncTestStepSubtitleBase
	void OnOpenCompleteL();
	void OnPlayCompleteL();
	};

/**
 * MM-MMF-AVICTRL-U-0101-HP
 */
class RAsyncTestSubtitleUpdateConfig : public RAsyncTestStepSubtitleBase
	{
public: 
	RAsyncTestSubtitleUpdateConfig(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);

private:
	// From RAsyncTestStepSubtitleBase
	void OnOpenCompleteL();
	void OnPlayCompleteL();
	};

class CCallBackTimer : public CTimer
	{
public:
	static CCallBackTimer* NewL(const TCallBack& aCallBack, TInt aPriority);
	
protected:
	void RunL();

private:
	CCallBackTimer(const TCallBack& aCallBack, TInt aPriority);

private:
	TCallBack iCallBack;
	};
	
/**
 * MM-MMF-AVICTRL-U-0102-HP
 */
class RAsyncTestSubtitleUpdateDuring : public RAsyncTestStepSubtitleBase
	{
public:
	RAsyncTestSubtitleUpdateDuring(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	
	//From RAsyncTestStep
	void KickoffTestL();
	void CloseTest();
	
private:
	// From RAsyncTestStepSubtitleBase
	void OnOpenCompleteL();
	void OnPlayCompleteL();

	static TInt DuringPlaybackCallBack(TAny* aPtr);
	void OnDuringPlayback();

private:
	CCallBackTimer* iCallBackTimer;
	TBool iHasUpdated;
	};
	
/**
 * MM-MMF-AVICTRL-U-0103-HP
 */
class RAsyncTestSubtitleRemoveAdd : public RAsyncTestStepSubtitleBase
	{
public: 
	RAsyncTestSubtitleRemoveAdd(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);

private:
	// From RAsyncTestStepSubtitleBase
	void OnOpenCompleteL();
	void OnPlayCompleteL();
	};
	
/**
 * MM-MMF-AVICTRL-U-0200-HP
 */
class RAsyncTestSubtitleNotEnabled : public RAsyncTestStepSubtitleBase
	{
public:
	RAsyncTestSubtitleNotEnabled(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	
private:
	// From RAsyncTestStepSubtitleBase
	void OnOpenCompleteL();
	void OnPlayCompleteL();
	};

/**
 * MM-MMF-AVICTRL-U-0201-HP
 */
class RAsyncTestSubtitleNotAvailable : public RAsyncTestStepSubtitleBase
	{
public:
	RAsyncTestSubtitleNotAvailable(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	
private:
	// From RAsyncTestStepSubtitleBase
	void OnOpenCompleteL();
	void OnPlayCompleteL();
	};
	
/**
 * MM-MMF-AVICTRL-U-0202-HP
 */
class RAsyncTestSubtitleMismatchedId : public RAsyncTestStepSubtitleBase
	{
public:
	RAsyncTestSubtitleMismatchedId(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	
private:
	// From RAsyncTestStepSubtitleBase
	void OnOpenCompleteL();
	};

/**
 * MM-MMF-AVICTRL-U-0203-HP
 */
class RAsyncTestSubtitleDoubleCall : public RAsyncTestStepSubtitleBase
	{
public:
	RAsyncTestSubtitleDoubleCall(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);

private:
	// From RAsyncTestStepSubtitleBase
	void OnOpenCompleteL();
	void OnPlayCompleteL();
	};

/**
 * MM-MMF-AVICTRL-U-0204-HP
 */
class RAsyncTestSubtitleMultipleConfigs : public RAsyncTestStepSubtitleBase
	{
public:
	RAsyncTestSubtitleMultipleConfigs(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TUid aControllerUid);
	
private:
	// From RAsyncTestStepSubtitleBase
	void OnOpenCompleteL();
	};

#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
#endif //(tsu_mmf_avictrlplay.h)
