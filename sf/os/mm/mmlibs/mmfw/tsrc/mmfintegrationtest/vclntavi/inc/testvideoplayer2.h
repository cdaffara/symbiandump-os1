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
// Header file: Video Player 2 Tests.
// 
//

#ifndef TESTVIDEOPLAYER2_H
#define TESTVIDEOPLAYER2_H

#include "tsi_mmf_vclntavi_stepbase.h"
#include <surfaceeventhandler.h>

/**
 * Load and play a video file using graphics surface
 *
 * RTestVclnt2PlayFile
 *
 */
class RTestVclnt2PlayFile : public RTestVclnt2AviPlayerStep
    {
public:
    static RTestVclnt2PlayFile* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
        
protected:    
    virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict DoTestStepPostambleL();

    virtual void FsmL(TVclntTestPlayEvents aEventCode);
    
    virtual void HandleIdleL();
    virtual void HandleOpenCompleteL();
    virtual void HandlePrepareCompleteL();
    virtual void HandlePlayCompleteL();
    
    // called by HandlePrepareCompleteL to trigger the video playback
    virtual void StartPlayback();    

    // constructor    
    RTestVclnt2PlayFile(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };
#ifdef SYMBIAN_BUILD_GCE
/**
 * Test CMediaClientVideoDisplay.This test makes sure that the new dll 
 * (mediaclientvideodisplay) is loaded and used.
 *
 * RTestMediaClientVideoDisplay
 *
 */
class RTestMediaClientVideoDisplay : public RTestVclnt2AviPlayerStep
    {
public:
    static RTestMediaClientVideoDisplay* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
        
protected:    
    virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict DoTestStepPostambleL();

    virtual void FsmL(TVclntTestPlayEvents aEventCode);
    
    virtual void HandleIdleL();
    virtual void HandleOpenCompleteL();
    virtual void HandlePrepareCompleteL();
    virtual void HandlePlayCompleteL();
    
    // called by HandlePrepareCompleteL to trigger the video playback
    virtual void StartPlayback();    

    // constructor    
    RTestMediaClientVideoDisplay(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };
    
#endif


/**
 * Load and play a video file using graphics surface with default window position, video
 * extent and clip rect, then during playback, perform some actions specified within the 
 * implementation of the function DoThisActionDuringPlaybackL().  Before exiting 
 * DoThisActionDuringPlaybackL, SetActionPerformed() should be called to signal that 
 * all user action to be performed during video playback is completed.
 *
 * RTestVclnt2ActionDuringVideoPlayback 
 *
 */
class RTestVclnt2PerformActionDuringVideoPlayback : public RTestVclnt2PlayFile
    {
protected:    
    RTestVclnt2PerformActionDuringVideoPlayback(const TDesC& aTestName,
                                                const TDesC& aSectName,
                                                const TDesC& aKeyName, 
                                                TInt aExpectedError);
    
    // overriden in this class to trigger start video playback and then proceed to
    // perform the action detailed within DoThisActionDuringPlaybackL
    virtual void StartPlayback();    
    
    virtual void HandlePlayCompleteL();    
    virtual void DoThisActionDuringPlaybackL(CVideoPlayerUtility2& aPlayer) = 0;
    
private:
    TInt iActionError;
    };


/**
 * Load and play a video file using graphics surface with specific window position, video
 * extent and clip rect
 *
 * RTestVclnt2AddWin
 *
 */
class RTestVclnt2AddWin : public RTestVclnt2PlayFile
    {
public:
    static RTestVclnt2AddWin* NewL(const TDesC& aTestName, 
                                   const TDesC& aSectName,
                                   const TDesC& aKeyName, 
                                   TInt aExpectedError);
    
protected:    
    // constructor
    RTestVclnt2AddWin(const TDesC& aTestName,
                      const TDesC& aSectName,
                      const TDesC& aKeyName, 
                      TInt aExpectedError);

    virtual void HandlePrepareCompleteL();
    };


/**
 * Load and play a video file using graphics surface, then change the display window during playback
 *
 * RTestVclnt2RemoveWin
 *
 */
class RTestVclnt2RemoveWin : public RTestVclnt2PerformActionDuringVideoPlayback
    {
public:
    static RTestVclnt2RemoveWin* NewL(const TDesC& aTestName, 
                                      const TDesC& aSectName,
                                      const TDesC& aKeyName, 
                                      TInt aExpectedError,
                                      TBool aAddBack);
    
protected:    
    virtual void HandlePrepareCompleteL();
    virtual void DoThisActionDuringPlaybackL(CVideoPlayerUtility2& aPlayer);
    
private:
    // constructor
    RTestVclnt2RemoveWin(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError, TBool aAddBack);
    
private:
	/** If true, Add the display back again after removing it. */ 
	TBool iAddBack;
    };


/**
 * Load and play a video file using graphics surface, then change the window position during playback
 *
 * RTestVclnt2WinPos
 *
 */
class RTestVclnt2WinPos : public RTestVclnt2PerformActionDuringVideoPlayback
    {
    public:
        static RTestVclnt2WinPos* NewL(const TDesC& aTestName, 
                                       const TDesC& aSectName,
                                       const TDesC& aKeyName, 
                                       TInt aExpectedError);
    
    protected:    
        virtual void DoThisActionDuringPlaybackL(CVideoPlayerUtility2& aPlayer);

    private:
        RTestVclnt2WinPos(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };


/**
 * Load and play a video file using graphics surface, then change the video extent during playback
 *
 * RTestVclnt2VideoExt
 *
 */
class RTestVclnt2VideoExt : public RTestVclnt2PerformActionDuringVideoPlayback
    {
    public:
        static RTestVclnt2VideoExt* NewL(const TDesC& aTestName, 
                                         const TDesC& aSectName,
                                         const TDesC& aKeyName, 
                                         TInt aExpectedError);
    
    protected:    
        virtual void DoThisActionDuringPlaybackL(CVideoPlayerUtility2& aPlayer);

    private:
        // constructor
        RTestVclnt2VideoExt(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };


/**
 * Load and play a video file using graphics surface, then change the clipping rect during playback
 *
 * RTestVclnt2ClipRect
 *
 */
class RTestVclnt2ClipRect : public RTestVclnt2PerformActionDuringVideoPlayback
    {
    public:
        static RTestVclnt2ClipRect* NewL(const TDesC& aTestName, 
                                         const TDesC& aSectName,
                                         const TDesC& aKeyName, 
                                         TInt aExpectedError);
    
    protected:    
        virtual void DoThisActionDuringPlaybackL(CVideoPlayerUtility2& aPlayer);

    private:
        RTestVclnt2ClipRect(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };


/**
 * Load and play a video file using graphics surface, then change the scale factor during playback
 *
 * RTestVclnt2Scale
 *
 */
class RTestVclnt2Scale : public RTestVclnt2PerformActionDuringVideoPlayback
    {
    public:
        static RTestVclnt2Scale* NewL(const TDesC& aTestName, 
                                      const TDesC& aSectName,
                                      const TDesC& aKeyName, 
                                      TInt aExpectedError);
    
    protected:    
        virtual void DoThisActionDuringPlaybackL(CVideoPlayerUtility2& aPlayer);
        
        virtual void HandlePrepareCompleteL();

    private:
        // constructor
        RTestVclnt2Scale(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };
    
/**
 * Load a video file, set the crop region and then play video using graphics surface with non-default window
 * clipping rect or video extent
 *
 * RTestVclnt2Viewport
 *
 */    
class RTestVclnt2Viewport : public RTestVclnt2PlayFile
    {
public:
    static RTestVclnt2Viewport* NewL(const TDesC& aTestName, 
                                     const TDesC& aSectName,
                                     const TDesC& aKeyName, 
                                     const TSize& aViewportSize,
                                     TInt aExpectedError = KErrNone);

protected:    
	virtual void HandlePrepareCompleteL();

private:
    void HandlePrepareCompleteL(const TRect& aCropRegion, const TRect& aVideoExtent, const TRect& aClipRect);
    
    // constructor
    RTestVclnt2Viewport(const TDesC& aTestName,
                        const TDesC& aSectName,
                        const TDesC& aKeyName, 
                        const TSize& aViewportSize,
                        TInt aExpectedError);

protected:
    TSize iViewportSize;
    };
    
/**
 * Load a video file, set an invalid crop region and then play video using graphics surface 
 *
 * RTestVclnt2InvalidViewport
 *
 */    
class RTestVclnt2InvalidViewport : public RTestVclnt2PlayFile
    {
public:
    static RTestVclnt2InvalidViewport* NewL(const TDesC& aTestName, 
                                     const TDesC& aSectName,
                                     const TDesC& aKeyName, 
                                     TInt aExpectedError = KErrNone);
    
protected:    
    virtual void HandlePrepareCompleteL();

private:
    // constructor
    RTestVclnt2InvalidViewport(const TDesC& aTestName,
                               const TDesC& aSectName,
                               const TDesC& aKeyName, 
                               TInt aExpectedError);    
    };    

/**
 * Load and play a video file using graphics surface with rotation set
 *
 * RTestVclnt2Rotation
 *
 */
class RTestVclnt2Rotation : public RTestVclnt2PlayFile
    {
public:
    static RTestVclnt2Rotation* NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    
protected:    
    virtual void HandlePrepareCompleteL();
    
private:
    // constructor
    RTestVclnt2Rotation(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError) ;    
    };
    

/**
 * Load and play a video file using graphics surface with specific crop region, autoscale, 
 * video extent and clip rect settings.
 *
 * RTestVclnt2Settings
 *
 */
class RTestVclnt2Settings : public RTestVclnt2PlayFile
    {
public:
    static RTestVclnt2Settings* NewL(const TDesC& aTestName, 
                                     const TDesC& aSectName,
                                     const TDesC& aKeyName, 
                                     TInt aExpectedError = KErrNone);
    
protected:    
    virtual void HandlePrepareCompleteL();
    
private:
    // constructor
    RTestVclnt2Settings(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError) ;    
    };   

/**
 * Load and play a video file using graphics surface with specific crop region, content alignment, 
 * scale factor, video extent and clip rect settings.  The video clip will be playbacked multiple times, 
 * each with a different combination of the settings.
 *
 * RTestVclnt2SettingsAndAlignments
 *
 */
class RTestVclnt2SettingsAndAlignments : public RTestVclnt2PlayFile
    {
public:
    static RTestVclnt2SettingsAndAlignments* NewL(const TDesC& aTestName, 
                                                  const TDesC& aSectName,
                                                  const TDesC& aKeyName, 
                                                  TInt aExpectedError = KErrNone);
    
protected:    
    virtual void HandlePrepareCompleteL();
    virtual void HandlePlayCompleteL();    
    virtual TVerdict DoTestStepPreambleL();
    
private:    
    // constructor
    RTestVclnt2SettingsAndAlignments(const TDesC& aTestName,
                                     const TDesC& aSectName,
                                     const TDesC& aKeyName, 
                                     TInt aExpectedError);   

    // helpers
    void AddDisplayWindowL(const TRect& aCropRegion);
    void SetCropRegionL(TRect& aCropRegion);
    void SetContentOffsetL();
    void SetScaleFactorL();
    void SetContentAlignmentL();
    
private:
    TInt iCount;
    };


/**
 * Load and play a video file using graphics surface on a second display
 *
 * RTestVclnt2SecDisplay
 *
 */
class RTestVclnt2SecDisplay : public RTestVclnt2PlayFile
    {
public:
    static RTestVclnt2SecDisplay* NewL(const TDesC& aTestName, 
                                       const TDesC& aSectName,
                                       const TDesC& aKeyName, 
                                       TInt aExpectedError = KErrNone);
    
protected:    
    // test framework
    virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict DoTestStepPostambleL();
    
    virtual void HandlePrepareCompleteL();    
    
private:
    // constructor
    RTestVclnt2SecDisplay(const TDesC& aTestName, 
                          const TDesC& aSectName, 
                          const TDesC& aKeyName, 
                          TInt aExpectedError);
    
private:
    CWsScreenDevice* iSecondScreen;    
    };


/**
 * Load and play a video file using graphics surface, then pause the video and attempt
 * to change the crop region and continue to playback the video.
 *
 * RTestVclnt2CropRectPause
 *
 */
class RTestVclnt2CropRectPause : public RTestVclnt2PerformActionDuringVideoPlayback
    {
    public:
        static RTestVclnt2CropRectPause* NewL(const TDesC& aTestName, 
                                              const TDesC& aSectName,
                                              const TDesC& aKeyName, 
                                              TInt aExpectedError = KErrNone);
    
    protected:    
        virtual void DoThisActionDuringPlaybackL(CVideoPlayerUtility2& aPlayfser);

    private:
        RTestVclnt2CropRectPause(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);        
    };


/**
 * Load and play a video file using graphics surface with overlay graphics on top of video picture
 *
 * RTestVclnt2Overlay
 *
 */
class RTestVclnt2Overlay : public RTestVclnt2PlayFile
    {
public:
    static RTestVclnt2Overlay* NewL(const TDesC& aTestName, 
                                    const TDesC& aSectName,
                                    const TDesC& aKeyName, 
                                    TInt aExpectedError);
    
protected:    
    // constructor
    RTestVclnt2Overlay(const TDesC& aTestName,
                       const TDesC& aSectName,
                       const TDesC& aKeyName, 
                       TInt aExpectedError);

    virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict DoTestStepPostambleL();

    virtual void HandlePrepareCompleteL();
    
private:
    CFont*      iFont;    
    };

/**
 * Load and play a video file using graphics surface and attempting to add the same display window twice
 * using default values
 *
 * RTestVclnt2AddWin2
 *
 */
class RTestVclnt2AddWin2 : public RTestVclnt2PlayFile
    {
public:
    static RTestVclnt2AddWin2* NewL(const TDesC& aTestName, 
                                    const TDesC& aSectName,
                                    const TDesC& aKeyName, 
                                    TInt aExpectedError);
    
protected:    
    // constructor
    RTestVclnt2AddWin2(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);

    virtual void HandlePrepareCompleteL();
    };

/**
 * Load and play a video file using graphics surface and attempting to add the display window with specific
 * settings then add the same display window using default values again 
 *
 * RTestVclnt2AddWin2WithSettings
 *
 */
class RTestVclnt2AddWin2WithSettings : public RTestVclnt2PlayFile
    {
    public:
        static RTestVclnt2AddWin2WithSettings* NewL(const TDesC& aTestName, 
                                                    const TDesC& aSectName,
                                                    const TDesC& aKeyName, 
                                                    TInt aExpectedError);
        
    protected:    
        // constructor
        RTestVclnt2AddWin2WithSettings(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);

        // state machine
        virtual void HandlePrepareCompleteL();
    };

/**
 * Load and play a video file using graphics surface and set content offset and/or content alignment
 *
 * RTestVclnt2ContOffset
 *
 */
class RTestVclnt2ContOffset : public RTestVclnt2PlayFile
	{
    public:
    	static RTestVclnt2ContOffset* NewL(const TDesC& aTestName, 
    	                                   const TDesC& aSectName, 
    	                                   const TDesC& aKeyName, 
    	                                   TInt aExpectedError);
    	
    protected:
        // state machine
        virtual void HandlePrepareCompleteL();
    	
    private:
        // constructor
    	RTestVclnt2ContOffset(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	};
	
/**
 * Load and play a video file using graphics surface and set content offset during playback
 *
 * RTestVclnt2ContOffsetAtPlay
 *
 */	
class RTestVclnt2ContOffsetAtPlay : public RTestVclnt2PerformActionDuringVideoPlayback
	{
    public:
    	static RTestVclnt2ContOffsetAtPlay* NewL(const TDesC& aTestName, 
    	                                         const TDesC& aSectName, 
    	                                         const TDesC& aKeyName, 
    	                                         TInt aExpectedError);
    	
    protected:    
        virtual void DoThisActionDuringPlaybackL(CVideoPlayerUtility2& aPlayer);

    private:
        // constructor
    	RTestVclnt2ContOffsetAtPlay(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	};

/**
 * Load and play a video file using graphics surface and set content aligment during playback
 *
 * RTestVclnt2Align
 *
 */	
class RTestVclnt2Align : public RTestVclnt2PerformActionDuringVideoPlayback
	{
    public:
    	static RTestVclnt2Align* NewL(const TDesC& aTestName, 
    	                              const TDesC& aSectName, 
    	                              const TDesC& aKeyName, 
    	                              TInt aExpectedError);
    	
    protected:
        virtual void HandlePrepareCompleteL();
        virtual void DoThisActionDuringPlaybackL(CVideoPlayerUtility2& aPlayer);	
    	
    private:
        // constructor
    	RTestVclnt2Align(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	};
	
/**
 * Load and play a video file using graphics surface and set auto scale
 *
 * RTestVclnt2AutoScale
 *
 */	
class RTestVclnt2AutoScale : public RTestVclnt2PerformActionDuringVideoPlayback
	{
    public:
    	static RTestVclnt2AutoScale* NewL(const TDesC& aTestName, 
    	                                  const TDesC& aSectName, 
    	                                  const TDesC& aKeyName, 
    	                                  TInt aExpectedError);
	
    protected:
        virtual void HandlePrepareCompleteL();
        virtual void DoThisActionDuringPlaybackL(CVideoPlayerUtility2& aPlayer);	
	
    private:
    	RTestVclnt2AutoScale(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError);
	};

/**
 * Load and play a video file using graphics surface
 *
 * RTestVclnt2PlayFile
 *
 */
class RTestVclnt2OldController : public RTestVclnt2PlayFile
    {
public:
    static RTestVclnt2OldController* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
        
protected:
    virtual void HandleIdleL();
    virtual void HandlePrepareCompleteL();
    
private:
	RTestVclnt2OldController(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

/**
 * Without opening a file, try doing surface related functions
 *
 * RTestVclnt2NoFile
 *
 */
class RTestVclnt2NoFile : public RTestVclnt2PlayFile
	{
public:
	static RTestVclnt2NoFile* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);

protected:
	virtual void HandleIdleL();

private:
	RTestVclnt2NoFile(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
	};

/**
 * Attempt to play video without display
 *
 * RTestVclnt2PlayAfterRemoveWin
 *
 */
class RTestVclnt2PlayAfterRemoveWin : public RTestVclnt2PlayFile
	{
public:
	static RTestVclnt2PlayAfterRemoveWin* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);

protected:
	virtual void HandlePrepareCompleteL();
	virtual void HandlePlayCompleteL();

private:
	RTestVclnt2PlayAfterRemoveWin(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);

private:
	TInt iPlayAttempt;
	};
	
/**
 * Use surface utility API without GCE support
 *
 * RTestVclnt2NoGce
 *
 */
class RTestVclnt2NoGce : public RTestVclnt2PlayFile
	{
public:
    static RTestVclnt2NoGce* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
        
protected:
    virtual void HandleIdleL();
    virtual void HandleOpenCompleteL();
    
private:
	RTestVclnt2NoGce(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

/**
 * Alloc tests (using Client heap) for Video Player utility 2.
 *
 * RTestVclnt2Alloc
 *
 */
class RTestVclnt2Alloc : public RTestVclnt2PlayFile
	{
public:
    static RTestVclnt2Alloc* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    virtual TVerdict DoTestStepL();
   
protected:
	virtual TVerdict DoTestStepPreambleL();
    virtual TVerdict PerformTestL();
    
private:
	RTestVclnt2Alloc(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
    };

class RTestVclnt2InvalidScaleFactor : public RTestVclnt2PlayFile
	{
public:
	static RTestVclnt2InvalidScaleFactor* NewL(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
	
protected:
	// From RTestVclnt2PlayFile
	void HandlePrepareCompleteL();
	
private:
	RTestVclnt2InvalidScaleFactor(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName, TInt aExpectedError);
	};

#endif // TESTVIDEOPLAYER2_H
