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
* Description:  Declares view class for application.
*
*/


#ifndef __AUDIOCLIENTSTUBAPPVIEW_h__
#define __AUDIOCLIENTSTUBAPPVIEW_h__

// INCLUDES
#include <coecntrl.h>

// CLASS DECLARATION
class CaudioclientstubAppView : public CCoeControl
    {
public:
    // New methods

    /**
     * NewL.
     * Two-phased constructor.
     * Create a CaudioclientstubAppView object, which will draw itself to aRect.
     * @param aRect The rectangle this view will be drawn to.
     * @return a pointer to the created instance of CaudioclientstubAppView.
     */
    static CaudioclientstubAppView* NewL(const TRect& aRect);

    /**
     * NewLC.
     * Two-phased constructor.
     * Create a CaudioclientstubAppView object, which will draw itself
     * to aRect.
     * @param aRect Rectangle this view will be drawn to.
     * @return A pointer to the created instance of CaudioclientstubAppView.
     */
    static CaudioclientstubAppView* NewLC(const TRect& aRect);

    /**
     * ~CaudioclientstubAppView
     * Virtual Destructor.
     */
    virtual ~CaudioclientstubAppView();

public:
    // Functions from base classes

    /**
     * From CCoeControl, Draw
     * Draw this CaudioclientstubAppView to the screen.
     * @param aRect the rectangle of this view that needs updating
     */
    void Draw(const TRect& aRect) const;

    /**
     * From CoeControl, SizeChanged.
     * Called by framework when the view size is changed.
     */
    virtual void SizeChanged();

private:
    // Constructors

    /**
     * ConstructL
     * 2nd phase constructor.
     * Perform the second phase construction of a
     * CaudioclientstubAppView object.
     * @param aRect The rectangle this view will be drawn to.
     */
    void ConstructL(const TRect& aRect);

    /**
     * CaudioclientstubAppView.
     * C++ default constructor.
     */
    CaudioclientstubAppView();

    };

#endif // __AUDIOCLIENTSTUBAPPVIEW_h__
// End of File
