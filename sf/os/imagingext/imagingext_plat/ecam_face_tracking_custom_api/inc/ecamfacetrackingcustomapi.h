/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  This Custom API controls the Face Tracking features.
*
*/


#ifndef ECAMFACETRACKINGCUSTOMAPI_H
#define ECAMFACETRACKINGCUSTOMAPI_H

#include <e32base.h>

// CONSTANTS

const TUid KCameraFaceTrackingUid = { 0x2001E2AF };

// CLASS DECLARATION

/**
* Custom interface for Face Tracking setting.
*
* @since S60 v5.1
*/
class MCameraFaceTracking
    {

public: 
    
    /**
    * List of supported face tracking settings
    */
    enum TFaceTrackingSettings
        {
        EFaceTrackingSupported      = 0x1,  /// @see SetFaceTrackingL()
        EFaceIndicatorsSupported    = 0x2,  /// @see EnableFaceIndicatorsL()
        EFaceSelectionSupported     = 0x4,  /// @see SetFaceSelectionPointL()
        EFaceRectanglesSupported    = 0x8   /// @see GetFaceRectanglesL()
        };
    
    /**
    * Returns a bitfield of supported face tracking settings. The bitfield
    * is a combination of TFaceTrackingSettings enumeration values.
    *
    * @since S60 v5.1
    * @return a bitfield that contains list of supported face tracking settings
    */
    virtual TUint32 SupportedFaceTrackingSettings() = 0;

    /**
    * Enables or disables face tracking.
    *
    * @since S60 v5.1
    * @param aFaceTrackingOn face tracking switch
    * @leave KErrNotSupported if face tracking is not supported
    * @leave KErrInUse if camera is not reserved
    * @leave KErrNotReady if camera power is not on
    */
    virtual void SetFaceTrackingL( TBool aFaceTrackingOn ) = 0;

    /**
    * Returns current face tracking status.
    *
    * @since S60 v5.1
    * @return ETrue if face tracking is on 
    */
    virtual TBool FaceTrackingOn() const = 0;

    /**
    * Sets drawing of face indicators on or off. If drawing is off or not
    * supported, application should draw face indicators itself. In this
    * situation application should also do face selection itself.
    *
    * @since S60 v5.1
    * @param aIndicatorsOn indicators switch
    * @leave KErrNotSupported if drawing of face indicators is not supported
    * @leave KErrInUse if camera is not reserved
    * @leave KErrNotReady if camera power is not on
    */
    virtual void EnableFaceIndicatorsL( TBool aIndicatorsOn ) = 0;

    /**
    * Returns current status of face indicators
    *
    * @since S60 v5.1
    * @return ETrue if face indicators are drawn 
    */
    virtual TBool FaceIndicatorsOn() const = 0;

    /**
    * Sets a point that is used for selecting or deselecting the primary
    * face among all detected faces.
    *
    * The given point is used to select or deselect (in case the face is
    * already selected) a primary face. In case drawing of face indicators
    * is on, it is up to the implementation to decide which face is
    * selected/deselected using the point coordinates provided by this method.
    * In case drawing of face indicators is off, application should do face
    * selection itself and use this method to pass the point where the camera
    * adjustment should be made (e.g., focus) i.e. center of currently
    * selected face.
    * 
    * Reference size defines the coordinate space where the point
    * coordinates reside. Thus the point coordinates are within the
    * rectangle [[0,0], [aReferenceSize.iWidth,aReferenceSize.iHeight]].
    * Usually the reference size is the same as viewfinder size.
    *
    * @since S60 v5.1
    * @param aFaceSelectionPoint coordinates of the selection point
    * @param aReferenceSize reference size for the selection point coordinates
    * @leave KErrNotSupported if face selection is not supported
    * @leave KErrInUse if camera is not reserved
    * @leave KErrNotReady if camera power is not on or face tracking is not on
    */
    virtual void SetFaceSelectionPointL(
        const TPoint& aFaceSelectionPoint,
        const TSize& aReferenceSize ) = 0;

    /**
    * Returns rectangles of faces in the given array and a reference size that
    * defines the limits for the coordinate space where the rectangle
    * coordinates reside.
    *
    * This method should be called only when drawing of face indicators is off.
    * Application can draw face indicators and do face selection using these
    * rectangles. The array is empty if no faces were detected.
    *
    * @since S60 v5.1
    * @param aRectangles array where rectangles of the faces are returned
    * @return reference size for the face rectangles
    * @leave KErrNotSupported if returning face rectangles is not supported
    * @leave KErrInUse if camera is not reserved
    * @leave KErrNotReady if camera power is not on or face tracking is not on
    */
    virtual TSize GetFaceRectanglesL( RArray<TRect>& aRectangles ) = 0;

    };

#endif //ECAMFACETRACKINGCUSTOMAPI_H
