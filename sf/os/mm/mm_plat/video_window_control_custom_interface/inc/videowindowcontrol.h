/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Defines a custom interface to permit the MDF client to 
*              position the video outside the confines of the physical 
*              display.   
*
*/





#ifndef __VIDEOWINDOWCONTROL_H__
#define __VIDEOWINDOWCONTROL_H__

#include <e32base.h>


/** Video window control extension UID */
const TUid KUidMdfVideoWindowControl = { 0x10282fb0 };

/**
 * DevVideoPlay video window control extension. This extension lets the client
 * control video position using a logical video window, and specify the exact
 * location of the picture within that window. The logical video window may
 * be partially or completely off-screen.
 */
class MMdfVideoWindowControl
    {
public:
    /**
     * Horizontal alignment contants.
     */
    enum THorizAlign
        {
        EHorizAlignCenter, /// The picture is horizontally centered
        EHorizAlignLeft,   /// The picture is left-aligned
        EHorizAlignRight   /// The picture is right-aligned
        };

    /**
     * Vertical alignment contants.
     */
    enum TVertAlign
        {
        EVertAlignCenter,  /// The picture is vertically centered
        EVertAlignTop,     /// The picture is top aligned
        EVertAlignBottom   /// The picture is bottom aligned
        };

    /** 
     * Sets the logical video window to use for positioning the video
     * picture on screen. By default the logical video window is the same
     * as the display video window set with StartDirectScreenAccessL().
     * 
     * @param aWindow Logical window rectangle, relative to the device display
     */
    virtual void MmvwcSetLogicalVideoWindow(const TRect& aWindow) = 0;

    /**
     * Sets the offset where the video picture will be located in the
     * logical video window. By default the picture is centered in the window
     * if it content and window sizes do not match.
     * 
     * @param aOffset Offset, the position of the upper left hand corner of the
     *                picture relative to the upper left hand corner of the
     *                logical window. The offset must not be negative, and the
     *                picture must be fully contained within the window.
     */
    virtual void MmvwcSetContentOffset(const TPoint &aOffset) = 0;

    /**
     * Sets video picture alignment within the logical video window.
     * By default the picture is centered in the window
     * if it content and window sizes do not match.
     *
     * This method and MmvwcSetContentOffsetL() are mutually exclusive, and
     * only one of them should be used. Calling one will override the settings
     * made with the other.
     *
     * @param aHorizAlign Horizontal alignment within the logical window
     * @param aVertAlign Vertical alignment within the logical window
     */
    virtual void MmvwcSetContentAlignment(THorizAlign aHorizAlign,
                                          TVertAlign aVertAlign) = 0;
    };

#endif
