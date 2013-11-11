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

/**
 @file
 @internalComponent
*/

#ifndef MMFSUBTITLEGRAPHICDRAWER_H
#define MMFSUBTITLEGRAPHICDRAWER_H
  
#include <bitstd.h>
#include <graphics/wsgraphicdrawer.h>
#include <graphics/wsgraphicdrawerinterface.h>

#include "mmfsubtitlegraphicmessage.h"

/**
 * Forward declarations
 */
class TSubtitleGraphicState;

/** 
Defines the MMF subtitle graphic drawer CRP component.  Used to display subtitles 
over video content.  
*/
NONSHARABLE_CLASS(CMMFSubtitleGraphicDrawer): public CWsGraphicDrawer
    {  
public:
    enum {EImplUid = 0x10285C9C};
                
public:
	virtual ~CMMFSubtitleGraphicDrawer();
    static CMMFSubtitleGraphicDrawer* NewL();
   
	// From CwsGraphicDrawer
    void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId, MWsClient& aOwner, const TDesC8& aData);
    virtual void HandleMessage(const TDesC8& aData);
    virtual void DoDraw(MWsGc& aGc, const TRect& aRect, const TDesC8& aData) const;

private:
	// Message handling..
	void ProcessMessageInit(TSubtitleCrpMsgInit& aMessage);
	void ProcessMessageDrawFrame(TSubtitleCrpMsgDrawFrame& aMessage);
	void ProcessMessageSwapFrame(TSubtitleCrpMsgRenderSwapFrame& aMessage);
	void DoBitBlt(MWsGc& aGc, const TRect& aRect) const;
	
private:
    enum TSubtitleGraphicState
		{
		ESubtitleGraphicStateWaiting    			= 0,
		ESubtitleGraphicStateInit 					= 10,
		ESubtitleGraphicStateInitSimple 			= 20,
		ESubtitleGraphicStateDrawFrame 				= 30,
		ESubtitleGraphicStateRefreshContent			= 40,
		ESubtitleGraphicStateSwapFrame 				= 50,
		ESubtitleGraphicStateClear					= 60
		};
		
	enum TSubtitleGraphicFrame
		{
		ESubtitleGraphicFrame1 = 1,
		ESubtitleGraphicFrame2 = 2,
		ESubtitleGraphicFrame3 = 3
		};
      
private:
	/**
	Handles to subtitle frames.
	Used for double-buffered rendering approach 
	@see CWsInterleaveGraphicDrawer::ProcessMessageInit()
	*/
    CFbsBitmap* iBitmap1;
    CFbsBitmap* iBitmap2; 
    
    /**
    Used to store one-off subtitle rendering requests
    @see CWsInterleaveGraphicDrawer::ProcessMessageDrawFrame()
    */
    CFbsBitmap* iTempBitmap;
    
    /**
    Status of current bitmap handles.  Stored here for effeciency as they are 
    checked every DoDraw() call.
    */
    TBool iBitmap1Valid;
    TBool iBitmap2Valid;
    TBool iTempBitmapValid;
    
   
    /**
    Indicates the current frame.  This is the buffer that will be drawn
    on the next call to @see SwapFrame() or when WServ invalidates a sub-portion
    of the screen
    ESubtitleGraphicFrame1: @see iBitmapHandle1
    ESubtitleGraphicFrame2: @see iBitmapHandle2
    ESubtitleGraphicFrame3: @see iTempBitmapHandle
    */
    TSubtitleGraphicFrame iCurrentFrame;
    	
	/**
	Indicates the region of the current bitmap that contains new image data
	*/
	TRect iDirtyRegion;
	
	/**
	Stores the previous frames dirty region. 
	*/
	TRect iOldDirtyRegion;
	
	/**
	The current CRP region stored on the first call to DoDraw()
	*/
	mutable TRect iSubtitleRegion;
	
	/**
	Indicates how long the current frame should be displayed for
	*/
	TTimeIntervalMicroSeconds iDisplayDuration;
	
	/**
	Indicates the system time that the current frame expires.   Calculated
	from the current system time and the display duration given on a 
	ESubtitleGraphicStateDrawFrame/ESubtitleGraphicStateSwapFrame frame message.
	*/
	mutable TTime iDisplayClearDue;
	
	/**
	The current CRP state
	*/
	mutable TSubtitleGraphicState iState;
	
	mutable TBool iCaptureRegion;
};

#endif
