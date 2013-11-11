/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#ifndef __HWRMLightTestF_APPVIEW_H__
#define __HWRMLightTestF_APPVIEW_H__


#include <coecntrl.h>

/** 
  @class CHWRMLightTestFAppView
  
  An instance of CHWRMLightTestFAppView is the Application View object
  for the HWRMLightTestF example application
  * * Nokia Core OS *
  */
class CHWRMLightTestFAppView : public CCoeControl
    {
public:

/**
  NewL
   
  Create a CHWRMLightTestFAppView object, which will draw itself to aRect
  @param aRect The rectangle this view will be drawn to
  @return A pointer to the created instance of CHWRMLightTestFAppView
  */
    static CHWRMLightTestFAppView* NewL(const TRect& aRect);

/**
  NewLC
   
  Create a CHWRMLightTestFAppView object, which will draw itself to aRect
  @param aRect The rectangle this view will be drawn to
  @return A pointer to the created instance of CHWRMLightTestFAppView
  */
    static CHWRMLightTestFAppView* NewLC(const TRect& aRect);


/**
  ~CHWRMLightTestFAppView
  
  Destroy the object
  */
     ~CHWRMLightTestFAppView();


public:  // from CCoeControl
/**
  Draw
  
  Draw this CHWRMLightTestFAppView to the screen
  @param aRect The rectangle of this view that needs updating
  */
    void Draw(const TRect& aRect) const;
  

private:

/**
  ConstructL
  
   Perform the second phase construction of a CHWRMLightTestFAppView object
  @param aRect The rectangle this view will be drawn to
  */
    void ConstructL(const TRect& aRect);

/**
  CHWRMLightTestFAppView
  
  Perform the first phase of two phase construction 
  */
    CHWRMLightTestFAppView();
    };


#endif // __HWRMLightTestF_APPVIEW_H__
