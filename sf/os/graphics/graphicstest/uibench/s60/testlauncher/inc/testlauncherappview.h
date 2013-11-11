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
 //

#ifndef __HELLOWORLDBASICAPPVIEW_H__
#define __HELLOWORLDBASICAPPVIEW_H__


#include <coecntrl.h>


class CEikGlobalTextEditor;


class CHelloWorldBasicAppView : public CCoeControl
    {
    public:
        /**
        * Create a CHelloWorldBasicAppView object, which will draw itself to aRect.
        * @param aRect The rectangle this view will be drawn to.
        * @return a pointer to the created instance of CHelloWorldBasicAppView.
        */
        static CHelloWorldBasicAppView* NewL(const TRect& aRect);

        /**
        * Create a CHelloWorldBasicAppView object, which will draw itself
        * to aRect.
        * @param aRect Rectangle this view will be drawn to.
        * @return A pointer to the created instance of CHelloWorldBasicAppView.
        */
        static CHelloWorldBasicAppView* NewLC( const TRect& aRect );

        ~CHelloWorldBasicAppView();

    public:
        /**
        * From CCoeControl
        * Draw this CHelloWorldBasicAppView to the screen.
        * If the user has given a text, it is also printed to the center of
        * the screen.
        * @param aRect the rectangle of this view that needs updating
        */
        void Draw( const TRect& aRect ) const;

    private:
        CHelloWorldBasicAppView();
        void ConstructL(const TRect& aRect);
        
    private:
		mutable TInt iDir; // mutable because it gets changed in constant draw method
		CFont* iFont;
    };

#endif // __HELLOWORLDBASICAPPVIEW_H__
