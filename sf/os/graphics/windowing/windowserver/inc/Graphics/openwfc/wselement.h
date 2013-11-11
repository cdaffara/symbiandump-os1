// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Interface for Scene Elements
// 
//

/**
 @publishedPartner
 @prototype
*/

#ifndef WSELEMENT_H
#define WSELEMENT_H

#include <w32std.h>
#include <graphics/wsgraphicdrawerinterface.h>
#include <graphics/surface.h>

/**
 * @brief Window Server Scene Element Interface Definition
 *
 * This interface allows Image Sources to be associated with meta-data
 * to define how the Image Source must be presented within a Scene.
 *
 * For a conceptual overview of Scenes, refer to the @c MWsScene Interface.
 *
 * The following aspects of Image Source presentation may be controlled:
 * @li Source rectangular region
 * @li Destination rectangular region
 * @li Rotation and flipping
 * @li Opacity
 * @li Relative z-order (ordinal position)
 * @li Tags to descriminate the UI from other Elements
 *
 * All co-ordinates are pixels offsets from (0,0) representing the
 * top level of the Image Source or Destination Target (the screen
 * or an off-screen buffer representing the screen), moving rightwards
 * and downwards in x and y co-ordinates respectively.
 *
 * Elements can be marked with flags.  There are two different consumers
 * of flags: Target Renderers and Render Stage Plug-ins.
 * 
 * Target Renderers are sub-systems which actually do composition and
 * rendering on a particular display device or off-screen buffer.
 * The EElementTransparencySource flag tells the Target Renderer that 
 * alpha blending must use the alpha channel in the Surface connected to
 * the Element.
 * 
 * On the other hand, Render Stage Plug-ins need flags to implement
 * specific policies such as "UI always on top".  To facilitate this,
 * Elements have a EElementIsIndirectlyRenderedUserInterface or a
 * EElementIsDirectlyRenderedUserInterface optionally set.  This
 * flag allows Render Stage Plug-ins to keep the relative z-order
 * of Elements aligned to this policy.
 * 
 * To allow compatible updates to be made to flags, all Render Stage
 * Plug-ins must preserve the flags in Elements which they do not operate
 * upon.  Target Renderers may ignore the flags in Elements which they
 * do not operate on.
 *
 * <h2>Intial State</h2>
 * 
 * The initial state for an Element is:
 * @li Fully opaque
 * @li No rotation, nor flipped
 * @li Not part of the Committed Scene
 * @li No connected Surface
 * @li All flags reset
 * @li Local error value set as KErrNone
 * @li Source Rectangle and Destination Rectangle is (0,0,0,0)
 * @il Destination Clipping Rectangle is (0,0,0,0)
 * 
 * When an Image Source is connected, the initial state becomes:
 * @li Source Rectangle is the extent of the Image Source
 * @li Destination Rectangle is set at origin (0,0), whose sized equals the
 *     Image Source size truncated the the target screen size.
 * @li Destination Clipping Rectangle is (0,0,0,0) to represent the entire
 *     area of the target screen.
 * 
 * <h2>Special Rectangle Values</h2>
 * 
 * Sometimes it is desirable to reset a Source, Destination, or Clipping
 * Rectangle so that the actual value used matches the corresponding Source
 * Image or Target Screen size.  In such cases rectangle (0,0,0,0) is used to
 * represent this.
 * 
 * <h2>Extending this interface</h2>
 * 
 * This interface can be extended in two different ways
 * @li Creating optional extension interfaces with 
 *       MWsElement::MWsObjectProvider::ResolveObjectInterface()
 * @li Defining new TElementFlags flag settings; Render Stage Plug-ins
 *       preserve the value of these across the Render Stage Pipeline.
 *
 * <h2>Committed Elements versus Pending Elements</h2>
 * 
 * This interface allows Elements to be modified in terms of their
 * own flag settings, and rotation, as well as their relative position
 * with other Elements.  Queries return the pending values for such
 * Elements.  Any modifications are marked Pending, and affect the Scene
 * according to the rules described by "Rendering" in @c MWsScene.
 * 
 * <h2>Order in which transformations are applied</h2>
 * 
 * A number of different transformations may be specified for an Element.
 * The order in which these are applied to arrive at the view seen in
 * the target is:
 * -# SetSourceRectangle              (Cropping)
 * -# SetSourceFlipping               (Flipping)
 * -# SetSourceRotation               (Rotation)
 * -# SetDestinationRectangle         (Scaling and Positioning)
 * -# SetDestinationClippingRectangle (Scaling and Positioning)
 * -# SetGlobalAlpha                  (Blending)
 */
class MWsElement : public MWsObjectProvider
    {
public:
    
    /**
     * @brief Flags used by Target Renderers.
     */
    enum TElementTargetRendererFlags
        {
        /**
         * Target renderer must treat the Element as Opaque.
         */
        EElementTransparencyNone                 = 0,
        /**
         * Target renderer must use the Global Alpha Transparency value
         * from the Element
         */
        EElementTransparencyGlobalAlpha          = (1 << 0),
        /**
         * Target renderer must use the Alpha Transparency Channel from
         * the Element
         */
        EElementTransparencySource               = (1 << 1),
        /**
         * Reserved flag value; this must not be set.
         */
        EElementTargetRendererReserved           = (1 << 2)
        };
    
    /**
     * @brief Flags used by Render Stages.
     */
    enum TElementRenderStageFlags
        {
        /**
         * No flag value set for Render Stage Plug-in
         */
        EElementRenderStageNone                   = 0,
        /**
         * Indirectly Rendered User Interface, i.e. rendering via 
         * MWsGraphicsContext calls originating from client CWindowGc
         * commands
         */
        EElementIsIndirectlyRenderedUserInterface = (1 << 0),
        /**
         * Directly Render User Interface, i.e. rendering where Window
         * Server does not see the graphics commands, as is the case for
         * Direct Screen Access clients.
         */
        EElementIsDirectlyRenderedUserInterface   = (1 << 1),
        /**
         * Reserved values for future Render Stage Plug-in usage
         */
        EElementRenderStageReserved               = (1 << 2)
        };
    
    /**
     * Amount of rotation of pixel data to apply to the Image Source
     * to obtain the Target Image
     */
    enum TElementRotation
        {
        EElementAntiClockwise0   = 0, /** No rotation */
        EElementAntiClockwise90  = 1, /** 90 Degrees Anti-clockwise in target */
        EElementAntiClockwise180 = 2, /** 180 Degrees Anti-clockwise in target */
        EElementAntiClockwise270 = 3  /** 270 Degrees Anti-clockwise in target */
        };
    
public:
    /**
     * @brief Set which portion of the image source is used.
     *
     * Set which rectangular portion of the connected image source
     * will be used when displaying this Element.
     *
     * @param aSrc  The source rectangle of pixels from the connected Image Source.
     *              This rectangle must not include any pixels outside the Image
     *              Source.  (0,0,0,0) means the entire size of the Image Source.
     * @pre     An Image Source must have been connected; @see ConnectSurface()
     * @return  One of the system-wide error codes.  
     *          KErrGeneral, if there is no image source connected.
     *          KErrArgument, if the source rectangle includes pixels not in the 
     *          connected image source, or has negative width or height.  
     *          KErrNone, if successful.
     */
    virtual TInt SetSourceRectangle(const TRect& aSrc) = 0;
    
    /**
     * @brief Get the portion of the Image Source being used.
     *
     * Get the portion of the connected Image Source being used.  The default
     * value is the entire extent of the connected Image Source.
     *
     * @param aSrc  The source rectangle is updated with the current rectangular
     *              region being used within the Image Source.
     * @pre     An Image Source must have been connected; @see ConnectSurface()
     * @return  One of the system-wide error codes.  
     *          KErrGeneral, if no Image Source has been connected.  
     *          KErrNone, if successful.
     */
    virtual TInt GetSourceRectangle(TRect& aSrc) = 0;
    
    /**
     * @brief Set the target area where the Image Source should appear.
     * 
     * Set the target area in the target screen (or off-screen buffer)
     * where the Image Source should appear.  Scaling may result if
     * the source and destination rectangles differ.  A platform specific
     * algorithm will be used to interpolate the scaled image.
     * 
     * @param aDest  The destination rectangle in the target.
     * @pre     An Image Source must have been connected; @see ConnectSurface()
     * @return  One of the system-wide error codes.  
     *          KErrGeneral, if there is no image source connected. 
     *          KErrArgument, if the destination rectangle has negative width or 
     *          height.  
     *          KErrNone, if successful.
     */
    virtual TInt SetDestinationRectangle(const TRect& aDest) = 0;
    
    /**
     * @brief Get the target area where the Image Source would appear.
     * 
     * Get the target area where the Image Source would appear.  The default
     * value is at (0,0) sized to the Image Source clipped to the size
     * of the target.
     * 
     * @param aDest  The destination rectangle is updated with the current
     *               region used in the target screen. 
     * @pre     An Image Source must have been connected; @see ConnectSurface()
     * @return  One of the system-wide error codes.  
     *          KErrGeneral, if no Image Source has been connected.  
     *          KErrNone, if successful.
     */
    virtual TInt GetDestinationRectangle(TRect& aDest) const = 0;
    
    /**
     * @brief Clip the target area where the Image Source would appear.
     * 
     * Set clipping area where the Image Source would appear.  The default
     * value is the area of the target screen.
     * 
     * @param aDestClipRect  The clipping rectangle in the target.  (0,0,0,0)
     *                       means the entire size of the Target Screen.
     * @return  One of the system-wide error codes.  
     *          KErrGeneral, if no Image Source has been connected. 
     *          KErrArgument, if the rectangle has negative width or height  
     *          KErrNone, if successful.
     */
    virtual TInt SetDestinationClippingRect(const TRect& aDestClipRect) = 0;
    
    /**
     * @brief Get the clipping area in the target.
     * 
     * Get the clipping area which limits where in the target screen the
     * element may appear.
     * 
     * @param aDestClipRect  The clipping rectangle is updated with the current
     *                       clipping area used in the target screen.
     * @return  One of the system-wide error codes.  KErrGeneral means no Image
     *          Source has been connected.  KErrNone is returned upon success.      
     */
    virtual TInt GetDestinationClippingRect(TRect& aDestClipRect) const = 0;
    
    /**
     * @brief Set the rotation that should appear in the target for the
     * Image Source.
     * 
     * Set the rotation which describes how the connected Image Source
     * should be rotated to appear in the target.
     * 
     * @param aElementRotation  The amount of rotation in the target.
     * @pre     An Image Source must have been connected; @see ConnectSurface()
     * @return  One of the system-wide error codes.  
     *          KErrGeneral, if no Image Source has been connected.  
     *          KErrArgument, if an out of bounds rotation value has been 
     *          supplied.  
     *          KErrNone, if successful.
     */
    virtual TInt SetSourceRotation(const TElementRotation aElementRotation) = 0;
    
    /**
     * @brief Get the pending rotation setting as it would appear in the target.
     * 
     * Get the pending rotation setting for the connected Image Source.
     * By default, the rotation is EElementAntiClockwise0 and this is returned
     * if there is no connected Image Source.
     * 
     * @return  The pending rotation setting.
     */
    virtual TElementRotation SourceRotation() const = 0 ;
    
    /**
     * @brief Set flipping about the image horizontal centerline.
     * 
     * Set the pending flipping setting for the connected Image Source.
     * When flipped, the Image Source appears in the target as inverted
     * about its horizontal centerline.  This effect can be combined
     * with rotation effects.
     * By default, the Image Source is not flipped.
     * 
     * @param aFlip  If ETrue, the connected Image is flipped, else
     *               it is not flipped.
     * @pre     An Image Source must have been connected; @see ConnectSurface()
     * @return  One of the system-wide error codes.  
     *          KErrGeneral, if no Image Source has been connceted.  
     *          KErrNotSupported, if the platform does not support the feature.  
     *          KErrNone, if successful.
     */
    virtual TInt SetSourceFlipping(const TBool aFlip) = 0;
    
    /**
     * @brief Get the pending flipping setting.
     * 
     * Get the pending flipping setting for the connected Image Source.
     * By default, the image is not flipped.
     * 
     * @return  The pending flipping setting.  If no Image is connected, return
     *          EFalse.
     */
    virtual TBool SourceFlipping() const = 0 ;
    
    /**
     * @brief Set Target Renderer Flags associated with this Element
     * 
     * Set the Target Renderer flags to be associated with this Element.
     * 
     * @note    Flags which are not recognised by an implementation must
     *          be kept to allow the interface to be extended in a 
     *          compatible manner.
     * @param aTargetRendererFlags  Flags to set for Target Renderer;
     *                              @see TElementTargetRendererFlags
     * @return  One of the system-wide error codes.  
     *          KErrNotSupported, if the platform does not support the given 
     *          flags.  
     *          KErrArgument, if incompatible flag settings have been made.  
     *          KErrNone, if successful.
     */
    virtual TInt SetTargetRendererFlags(const TUint32& aTargetRendererFlags) = 0;
    
    /**
     * @brief Set Render Stage Flags associated with this Element
     * 
     * Set the Render Stage flags to be associated with this Element.
     * 
     * @note    Flags which are not recognised by an implementation must
     *          be kept to allow the interface to be extended in a 
     *          compatible manner.
     * @param aRenderStageFlags  Flags to set for Render Stage Plug-ins;
     *                           @see TElementRenderStageFlags
     * @return  One of the system-wide error codes.  
     *          KErrNotSupported, if the platform does not support the given 
     *          flags.  
     *          KErrArgument, if incompatible flag settings have been requested.  
     *          KErrNone, if successful.
     */
    virtual TInt SetRenderStageFlags(const TUint32& aRenderStageFlags) = 0;
    
    /**
     * @brief Get Target Renderer Flags associated with this Element
     * 
     * Get the Target Renderer flags associated with this Element.
     *   
     * @note    Flags which are not recognised by an implementation must
     *          be kept to allow the interface to be extended in a 
     *          compatible manner.
     * @param aTargetRendererFlags  Variable to receive TElementTargetRendererFlags
     */
    virtual void GetTargetRendererFlags(TUint32& aTargetRendererFlags) const = 0;

    /**
      * @brief Get Render Stage Flags associated with this Element
      * 
      * Get the Render Stage flags associated with this Element.
      *   
      * @note    Flags which are not recognised by an implementation must
      *          be kept to allow the interface to be extended in a 
      *          compatible manner.
      * @param aRenderStageFlags  Variable to receive TElementRenderStageFlags
      */
     virtual void GetRenderStageFlags(TUint32& aRenderStageFlags) const = 0;   
     
    /**
     * @brief Get the Element above this Element.
     * 
     * Get the Element which is above this Element in the Pending Scene.
     * If the Element is the top-most element, NULL is returned.  If the
     * Element is not in the Scene, NULL is returned.
     * 
     * @return Element above, or NULL.
     */
    virtual MWsElement *ElementAbove() = 0;
    
    /**
     * @brief Get the Element below this Element.
     * 
     * Get the Element which is below this Element in the Pending Scene.
     * If the Element is the bottom-most element, NULL is returned.  If the
     * Element is not in the Scene, NULL is returned.
     * 
     * @return Element below, or NULL.
     */
    virtual MWsElement *ElementBelow() = 0;
    
    /**
     * @brief Connect or re-Connect a Surface to this Element.
     * 
     * Connect the given Surface to this Element, or re-Connect a new
     * Surface to this Element which already has a connected Surface.
     * The Element does not need to be in the Scene when this call is made.
     * 
     * @note    The Source Rectangle of the Element is updated to correspond
     *          to match the Source Rectangle of the Surface.  Thus any
     *          prior Source Rectangle setting will be overwritten.
     * 
     * @param aSurface  Surface to connect to.  If this is the Null
     *                  Surface, then the Source Rectangle of the
     *                  Element is set to (0,0,0,0).
     * @return  One of the system-wide error codes.  
     *          KErrArgument, if supplied Surface cannot be connected.  For 
     *          example the Element has a flag indicating it has an Alpha 
     *          Channel but the Surface does not.  
     *          KErrNone, if successful.
     */
    virtual TInt ConnectSurface(const TSurfaceId& aSurface) = 0;
    
    /**
     * @brief Get the Surface connected to this Element
     * 
     * @return  The Surface Identifier of the Connected Surface.  This
     *          identifier may represent the Null Surface Id if there
     *          was no Surface connected in the Pending Scene for this
     *          Element.
     */
    virtual const TSurfaceId& ConnectedSurface() const = 0;
    
    /**
     * @brief Set the Global Alpha Transparency value
     * 
     * Sets the Global Alpha Transparency value for this Element.  Elements
     * are by default fully opaque, represented by opacity 255.  On platforms
     * that support it, the Alpha Transparency value can be reduced down
     * to 0 representing fully transparent.  The purpose of this is to
     * allow the fading in and fading out of Elements into the Scene.
     * 
     * @param aAlpha  The Alpha Transparency value desired.  Values
     *                are clamped to be within the range [0, 255] where 0
     *                represents fully transparent, and 255 represents
     *                fully opaque.
     * @note    This setting has no effect unless the flag
     *          Target Renderer flag EElementTransparencyGlobalAlpha
     *          is set; @see SetTargetRendererFlags()
     * @note    Out of range values are clamped silently, and do not cause
     *          error conditions.
     * @return  One of the system-wide error codes.  
     *          KErrNotSupported, if the supplied value, once clamped, is not 
     *          supported on the platform.
     *          KErrNone, if successful.
     */
    virtual TInt SetGlobalAlpha(const TInt aAlpha) = 0;
    
    /**
     * @brief Get the Global Alpha Transparency value
     * 
     * Gets the Global Alpha Transparency value for this Element.  Elements
     * are by default fully opaque, represented by opacity 255.
     * 
     * @param aAlpha  Variable which receives the Global Alpha Transparency
     *                value.
     */
    virtual void GlobalAlpha(TInt& aAlpha) const = 0;
    };
#endif // WSELEMENT_H
