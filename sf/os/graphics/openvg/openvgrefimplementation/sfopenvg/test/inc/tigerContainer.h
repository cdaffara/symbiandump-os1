/*
* Copyright (c) 2009 Symbian Foundation Ltd
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Symbian Foundation Ltd - initial contribution.
* 
* Contributors:
*
* Description:
* Tiger app container class
*/
#ifndef TIGERCONTAINER_H
#define TIGERCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include "tiger.h"
#include <akndef.h>

#include <eglosnativewindowtype.h>

// CLASS DECLARATION

/**
 * Container control class that handles the OpenGL ES initialization and deinitializations.
 * Also uses the CTiger class to do the actual OpenGL ES rendering.
 */
class CTicker;
class CTigerContainer : public CCoeControl, MCoeControlObserver
    {
    public: // Constructors and destructor

        /**
         * EPOC default constructor. Initializes the OpenGL ES for rendering to the window surface.
         * @param aRect Screen rectangle for container.
         */
        void ConstructL(const TRect& aRect);

        /**
         * Destructor. Destroys the CPeriodic, CTiger and uninitializes OpenGL ES.
         */
        virtual ~CTigerContainer();

    private: // Functions from base classes

        /**
         * Method from CoeControl that gets called when the display size changes.
         * If OpenGL has been initialized, notifies the renderer class that the screen
         * size has changed.
         */
        void SizeChanged();

        /**
         * Handles a change to the control's resources. This method
         * reacts to the KEikDynamicLayoutVariantSwitch event (that notifies of
         * screen size change) by calling the SetExtentToWholeScreen() again so that
         * this control fills the new screen size. This will then trigger a call to the
         * SizeChanged() method.
         * @param aType Message UID value, only KEikDynamicLayoutVariantSwitch is handled by this method.
         */
        void HandleResourceChange(TInt aType);

        /**
         * Method from CoeControl. Does nothing in this implementation.
         */
        TInt CountComponentControls() const;

        /**
         * Method from CCoeControl. Does nothing in this implementation.
         */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
         * Method from CCoeControl. Does nothing in this implementation.
         * All rendering is done in the DrawCallBack() method.
         */
        void Draw(const TRect& aRect) const;

        /**
         * Method from MCoeControlObserver that handles an event from the observed control.
         * Does nothing in this implementation.
		 * @param aControl   Control changing its state.
		 * @param aEventType Type of the control event.
         */
        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);
        
        void  RenderBitmap(CWindowGc& aGc, CFbsBitmap* aBitmap) const;
        void  CopyAndRender(CWindowGc& aGc) const;
        TInt FlipVertical(CFbsBitmap& aBitmap) const;

    private:  //data

        CTiger* iTiger;
    };

#endif

// End of File
