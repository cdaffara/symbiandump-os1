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
// Interface for Window Server Scene
//
//

/**
 @publishedPartner
 @prototype
*/

#ifndef WSSCENE_H
#define WSSCENE_H

#include <babitflags.h>
#include <w32std.h>
#include <graphics/wsgraphicdrawerinterface.h>

class MWsElement;

/**
 * @brief Window Server Scene Interface Definition.
 * 
 * MWsScene is a CWsRenderStage interface extension.  An understanding
 * of the Render Stage Plug-in architecture, and the CWsRenderStage API
 * is required in order to use and understand this interface.
 * 
 * A MWsScene manages a collection of MWsElements that define
 * what is shown to a User on a particular target display, or off-screen
 * buffer.
 * 
 * A MWsScene constitute of MWsElements.  Each MWsElement represents
 * a rectangular region of the screen.  MWsElements have meta-data which
 * describe the region, such as the image orientation, extent, and
 * alpha-blending characteristics.  MWsElements also have surface-data,
 * which provides the pixel data to be shown in the given area of the
 * screen.  
 * 
 * The purpose of an MWsScene is to manage the relative ordinal positions
 * of scene elements.  Typically elements representing the camera view-
 * finder, or video frames, is placed at the bottom, and the user interface
 * element is placed above.  Semi-transparency is often used to blend
 * such elements into a visually appealing scene, and transparent holes
 * in the user interface element allow elements below to poke through.
 * 
 * The goal of this interface is to allow suitable platforms to implement
 * this API using composition hardware.  This is to reduce power and
 * CPU load, so a long movie might be played, for example.
 * 
 * A secondary goal of this interface is to enable transition effects
 * to be efficiently implemented.  This is because MWsScenes can be
 * chained together in a Render Stage pipeline.  Element meta-data
 * can be manipulated by transition effects render stages before actual
 * rendering.
 * 
 * <h1>Examples of Elements</h1>
 * @li Camera view-finder
 * @li Video frame
 * @li User interface
 * 
 * There can be two types of user interface element: directly rendered
 * and indirectly rendered.  
 * 
 * Indirect rendering is the normal case.  Here the client provides
 * CWindowGc commands, which Window Server passes as MWsGraphicsContext
 * API calls to the Render Stage.
 * 
 * Direct rendering is where Window Server does not see the graphics
 * commands, as is the case for Direct Screen Access clients.
 * 
 * <h1>Use Cases</h1>
 * 
 * <h2>Instantiation</h2>
 * 
 * During system-startup Window Server needs to obtain an instance of
 * the MWsScene interface in order to place MWsElements in the scene
 * for rendering.
 * 
 * MWsScene is a CWsRenderStage interface extension, accessed by
 * @code
 * CWsRenderStage::ResolveObjectInterface(KMWsScene);
 * @endcode
 * 
 * The initial values are:
 * @li No rotation is applied
 * @li No Elements are in the Scene
 * @li Scene is connected to same display as its CWsRenderStage
 * 
 * Note Elements may be added/inserted/removed subject to implementation-
 * defined constraints; the connected display is immutable.
 * 
 * <h2>Rendering</h2>
 * 
 * When the Scene changes, Window Server needs to communicate the changes
 * and then render the new Scene.  This can happen when a video player
 * application is launched, whereby a new User Interface control is placed
 * on the screen, and a new Element representing the Video Surface is added.
 * 
 * The Window Server must use the CWsRenderStage interface to provide
 * updates to the Scene.
 * @code
 * CWsRenderStage::Begin(regionOfScreenWithVideoPlayer);
 * drawing ops sent to MWsGraphicsContext for UI
 * drawing ops to setup a transparent hole for video surface to poke
 * through
 * .
 * .
 * 
 * sceneElement = CreateSceneElementL();
 * sceneElement->ConnectSurface(videoSurfaceAlreadySetup);
 * InsertSceneElement(sceneElement, NULL);
 * CWsRenderStage::End(asyncCallbackNotification);
 * @endcode
 * 
 * When Scene updates, such as in the example above, are done there is
 * a concept of the "Committed Scene".  All Scene updates are considered
 * to be "Pending" until a CWsRenderStage::End() has been called.  Then
 * the pending changes are applied atomically.  Either the previously
 * Committed Scene is re-instated or all the Pending changes are accepted
 * to form the newly Committed Scene.  This depends on platform-specific
 * limitations, and system-wide resource availability.
 * 
 * <h2>Orientation Changes to Elements</h2>
 * 
 * Suppose we have an MWsElement representing people as seen through a video
 * camera view finder in landscape orientation, with the handset also in
 * landscape orientation.  The handset is rotated into portrait mode.
 * 
 * We desire the view finder to be rotated in the opposite
 * sense to the rest of the User Interface.  This is to allow people
 * previously vertical in the view finder to remain vertical.
 * 
 * For this use case, a MWsElement::SetSourceRotation() must be called.
 * 
 * The changes are visible after the next @c CWsRenderStage::End() call.
 * 
 * <h2>Orientation Changes to Scenes</h2>
 * 
 * Suppose the entire Scene comprises a single User Interface element.
 * The handset is then rotated from landscape to portrait mode.
 * The whole scene must be rotated to compensate.  Here 
 * MWsScene::SetSceneRotation() must be called.
 * 
 * The changes are visible after the next @c CWsRenderStage::End() call.
 * 
 * <h2>Leveraging external APIs for Transition Effects</h2>
 *
 * Suppose we want to manipulate the Scene to achieve a transition
 * effect.  The Scene as presented by Window Server comprises one
 * UI Element, and one External Surface Element.  We have an external
 * APIs, such as OpenGL or OpenWF, available to deliver a transition effect
 * into a different External Surface.
 * 
 * This task is accomplished by using the output Surface of OpenGL as
 * the connected Image Source for the one Element which comprises the Scene
 * presented on the Screen.
 * The Elements inserted into the Scene by Window Server are instead given
 * to OpenGL as textures.  When the Screen needs to be rendered, OpenGL
 * is first used to render to its output Surface, and this is taken
 * as-is for the Scene Element that appears on the Screen.
 * 
 * A combination of Rendering APIs may be employed if they are available
 * to the Transition Effects Render Stage -- to combine the power of
 * OpenVG, OpenGL, and OpenWF, as an example.
 *
 * <h2>Managing the life-cycle of Surfaces from Window Server</h2>
 *
 * Suppose we want to maintain the life-cycle of a given Surface outside
 * that needed by the Scene.  For example, suppose we have a video player
 * application whose picture is frozen to a particular frame.  Then the
 * application switches to some other view, no longer showing the video
 * frame.  Finally, the application switches back to the original video
 * frame so that the video clip can be resumed.
 *
 * In such a scenario, we would want the contents of the Surface to
 * remain constant event though at one point the Surface is not part
 * of the Scene.  This is so that the user does not see anything
 * inconsistent appear with the video frame between switching from and
 * to the video frame.
 *
 * Another scenario where the Surface needs to have a life-cycle beyond
 * that of the Scene is when there is a large start-up cost to populating
 * the Surface with initial data.  For example, a computer game application
 * might take some seconds to construct the textures for an initial game
 * position.  The game would want to construct the Surface, and populate
 * its contents before supplying the Surface as a Background Surface for
 * a given window.  This is so that the user does not see the game building
 * up the Surface contents during game start-up.
 *
 * To facilitate the above use case, a call to RegisterSurface and a 
 * corresponding call to UnregisterSurface must be made.  Note, Window
 * Server should provide an UnregisterSurface call if the client program
 * dies or exits abruptly before it could call UnregisterSurface.
 */
class MWsScene : public MWsObjectProvider
    {
public:
    DECLARE_WS_TYPE_ID(KMWsScene)
    
public:
    
    /**
     * Amount of rotation of pixel data in the target display or
     * target surface for the entire scene.
     */
    enum TSceneRotation
        {
        ESceneAntiClockwise0 = 0,   /** No rotation */
        ESceneAntiClockwise90 = 1,  /** 90 Degrees Anti-clockwise in target */
        ESceneAntiClockwise180 = 2, /** 180 Degrees Anti-clockwise in target */
        ESceneAntiClockwise270 = 3  /** 270 Degrees Anti-clockwise in target */
        };
       
    /**
     * @brief   Update the rotation setting for the entire scene.
     * @param aSceneRotation  The new rotation setting
     * @note    The changes are visible after the next @c CWsRenderStage::End()
     *          or @c ScreenSnapshot() call.
     * @return  One of the system-wide error codes.  KErrArgument means
     *          aSceneRotation is out-of-range, KErrNotSupported means the
     *          setting is not supported.  KErrNone is returned upon success.
     */
    virtual TInt SetSceneRotation(const TSceneRotation aSceneRotation) = 0;
    
    /**
     * @brief   Obtain the current rotation setting for the entire scene
     * @return  The currently committed scene rotation setting.
     */
    virtual TSceneRotation SceneRotation() const = 0;
    
    /**
     * @brief Create a Scene Element for this scene.
     * 
     * Create a new Scene Element for use in the Scene.  The Scene Element
     * is immediately available to the caller, but is not in the Scene.
     * It must be inserted into the Scene, at which time is is considered
     * a Pending Scene change.  Once @c CWsRenderStage::End() has been called
     * the Element is rendered in the target and becomes part of the Committed
     * Scene.
     * 
     * @return  The Scene Element
     * @leave   KErrNoMemory There are insufficient memory resources
     */
    virtual MWsElement* CreateSceneElementL() = 0;
    
    /**
     * @brief Destroy Scene Element
     * 
     * Remove the specified element from the scene, and then destroy
     * resources associated with it.
     * 
     * @note  The update is seen after the next @c CWsRenderStage::End()
     *        call.
     * @param aElement  The element to destroy.  If NULL, the scene is
     *                  unmodified.
     */
    virtual void DestroySceneElement(MWsElement* aElement) = 0;
    
    /**
     * @brief Insert or Re-insert the Scene Element into the Scene
     * 
     * Insert the Scene Element into the Scene above the aSubordinateElement,
     * or re-insert the existing Element but into a new position in the Scene.
     * 
     * @note    The same element cannot be in the scene at more than one
     *          z-order position.
     * @pre     The element must have previously been created @c 
     *          CreateSceneElementL()
     * @return  One of the system-wide error codes.  KErrArgument means
     *          aElement or aSubordinateElement is not a valid element for this
     *          Scene.  KErrNone is returned upon success.
     * @param aInsertElement        The element to add; cannot be NULL
     * @param aSubordinateElement   The element which will be below 
     *                              aInsertElement when the scene is committed,
     *                              or NULL to place at it the bottom of
     *                              the scene.
     */
    virtual TInt InsertSceneElement(MWsElement* aInsertElement,
                                    MWsElement* aSubordinateElement) = 0;
    
    /**
     * @brief Remove an Element from the Scene
     * 
     * Remove the specified Element from the Scene, without destroying
     * it.  
     * 
     * @note   The update is seen after the next @c CWsRenderStage::End()
     *         call. 
     * @param aRemoveElement  The element to be removed from the scene.
     *                        If NULL, no action is taken.
     * @pre     The supplied element, if non-NULL, must be in the Scene;
     *          see @c InsertSceneElement()
     * @post    The removed element is no longer in the Scene, but is still
     *          valid object; it may receive further method calls.
     * @return  One of the system-wide error codes.  KErrArgument means
     *          aRemoveElement was not in this Scene.  KErrNone is returned
     *          upon success.             
     */
    virtual TInt RemoveSceneElement(MWsElement* aRemoveElement) = 0;
    
    /**
     * @brief Compose pending Scene into an off-screen target
     *
     * Compose the pending Scene into an off-screen target.  This
     * method is an asynchronous call, and signals the optionally provided
     * TRequestStatus, either immediately from some error cases, or later
     * when the supplied off-screen target has been populated.
     * 
     * However, the pending scene changes for the target screen are not
     * committed by this function call.
     * 
     * The TRequestStatus status value is updated by this function to one of
     * the system-wide error codes.  KErrArgument is set if the supplied surface
     * or TRequestStatus is in error.  KErrNoMemory is set for memory or other
     * resource failures.  KErrInUse is set if the surface is in use or
     * otherwise inaccessible.  KErrNotSupported is set if the supplied surface
     * is incompatible with the screen.
     *
     * @param aOffScreenTarget  Surface to be updated with pending
     *                          Scene
     * @param aCompleted        When non-NULL, is used to signal the
     *                          client when completed
     * @post    Either the supplied surface is updated with new contents
     *          and the aCompleted status is set to KErrNone, or an error value
     *          is set in the aCompleted status.
     * @post    Its guaranteed that the supplied surface is not accessed
     *          by MWsScene after aCompleted has been signaled.
     */
    virtual void ComposePendingScene(TSurfaceId& aOffScreenTarget,
                                        TRequestStatus* aCompleted) = 0;
    
    /**
     * @brief Register the Window Server use of a Surface
     *
     * Register the Window Server use of a Surface for this Scene.
     * The Scene abstraction already manages its use of Surfaces without
     * this method call.  The purpose of this method is to allow Window
     * Server, on behalf of its clients, to mark the given Surface as
     * being used by such clients.  This allows the Surface to maintain
     * a lifecycle beyond that which would normally be needed by the
     * Scene, and allows early indication of incompatibility between
     * the Surface and the Scene.
     *
     * @param aSurface  Surface to be marked as in use by the caller
     * @post    Either the supplied surface accepted as potentially useable
     *          by this Scene, or the local error value is set.
     * @return  One of the system-wide error codes.  KErrNotSupported means the
     *          Surface is incompatible with this Scene.  KErrNoMemory means
     *          there was a memory problem.  KErrArgument means the supplied
     *          Surface is not valid.  KErrServerBusy means the Surface is
     *          currently used by another Scene.  KErrNone is returned upon
     *          success.
     */
    virtual TInt RegisterSurface(const TSurfaceId& aSurface) = 0;
    
    /**
     * @brief Unregister the Window Server use of a Surface
     *
     * Unregister the Window Server use of a Surface with this Scene.
     * The Scene abstraction already manages its use of Surfaces without
     * this method call.  The purpose of the method is to allow Window
     * Server, on behalf of its clients, to mark the given Surface as
     * no longer in use by such clients.  This ends the lifecycle of
     * the Surface from the point of view of such clients.
     *
     * @param aSurface  Surface to be marked as no longer in use by the
     *                  caller
     * @return  One of the system-wide error codes.  KErrBadHandle means the 
     *          supplied Surface is not known.  KErrNone is returned upon 
     *          success.
     */
    virtual TInt UnregisterSurface(const TSurfaceId& aSurface) = 0;
    };
#endif // WSSCENE_H
