// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This material, including documentation and any related
// computer programs, is protected by copyright controlled by
// Nokia. All rights are reserved. Copying, including
// reproducing, storing, adapting or translating, any
// or all of this material requires the prior written consent of
// Nokia. This material also contains confidential
// information which may not be disclosed to others without the
// prior written consent of Nokia.
//
// Description:
// Render Orientation Tracking and Publication
// 

#ifndef renderorientationtracker_h
#define renderorientationtracker_h

#include <e32std.h>
#include <e32base.h>
#include <e32property.h>
#include <w32std.h>
#include <wspublishandsubscribedata.h>

// Values for the device orientation that we receive via P&S from the Theme Server
// Todo FIX THIS Category UID!!!
const TUid  KThemeOrientationCategory   = {0x20022E82}; // == KHbPsHardwareCoarseOrientationCategoryUid 
const TUint KThemeOrientationKey        = 0x4F726965; // == KHbPsHardwareCoarseOrientationKey 

typedef TRenderOrientation TRenderOrientationTrackingType; 

class CWsWindowGroup;

class CWsRenderOrienationTracker : public CActive
    {
public:  
    static CWsRenderOrienationTracker* NewL();
    ~CWsRenderOrienationTracker();

    void CheckRenderOrientation();

private:   
    CWsRenderOrienationTracker();
    void ConstructL();
    
    void RequestDeviceOrientationNotification();
    void CancelDeviceOrientationNotification();
    
    // CActive
    void RunL();
    void DoCancel();
    
    TInt DoOrientationTracking();
    TInt GetThemeOrientation(TRenderOrientation& aThemeOrientation);
    TInt PublishOrientation(const TRenderOrientation aRenderOrientation);
    TInt DoPublishOrientation(const TRenderOrientation aRenderOrientation);
    TInt GetIndicatedOrientation(TRenderOrientationTrackingType& aOrientationTrackingType);
    TInt CheckWindowGroupOrientation(const CWsWindowGroup& aWinGroup, TRenderOrientationTrackingType& aOrientationTrackingType);
    TBool UseableGroupWindow(const CWsWindowGroup& aWinGroup) const;
    TInt GetFocusWindowOrientation(TRenderOrientationTrackingType& aOrientationTrackingType);
    TInt FindOrientationFromWindowTree(TRenderOrientationTrackingType& aOrientationTrackingType);    
    void SetHALOrientation(const TRenderOrientation aRenderOrientation);
    
private:    
    TRenderOrientationTrackingType  iRenderOrientationTrackingType;    
    TRenderOrientation              iPublishedRenderOrientation;
    RProperty                       iThemeOrientationProperty;
    RProperty                       iRenderOrientationPublisher;
    };

#endif
