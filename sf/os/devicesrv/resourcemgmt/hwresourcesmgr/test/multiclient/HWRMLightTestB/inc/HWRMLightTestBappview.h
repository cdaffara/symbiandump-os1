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



#ifndef __HWRMLightTestB_APPVIEW_H__
#define __HWRMLightTestB_APPVIEW_H__


#include <coecntrl.h>

/** 
  @class CHWRMLightTestBAppView
  
  An instance of CHWRMLightTestBAppView is the Application View object
  for the HWRMLightTestB example application
  * * Nokia Core OS *
  */
class CHWRMLightTestBAppView : public CCoeControl
    {
public:

/**
  NewL
   
  Create a CHWRMLightTestBAppView object, which will draw itself to aRect
  @param aRect The rectangle this view will be drawn to
  @return A pointer to the created instance of CHWRMLightTestBAppView
  */
    static CHWRMLightTestBAppView* NewL(const TRect& aRect);

/**
  NewLC
   
  Create a CHWRMLightTestBAppView object, which will draw itself to aRect
  @param aRect The rectangle this view will be drawn to
  @return A pointer to the created instance of CHWRMLightTestBAppView
  */
    static CHWRMLightTestBAppView* NewLC(const TRect& aRect);


/**
  ~CHWRMLightTestBAppView
  
  Destroy the object
  */
     ~CHWRMLightTestBAppView();


public:  // from CCoeControl
/**
  Draw
  
  Draw this CHWRMLightTestBAppView to the screen
  @param aRect The rectangle of this view that needs updating
  */
    void Draw(const TRect& aRect) const;
  

private:

/**
  ConstructL
  
   Perform the second phase construction of a CHWRMLightTestBAppView object
  @param aRect The rectangle this view will be drawn to
  */
    void ConstructL(const TRect& aRect);

/**
  CHWRMLightTestBAppView
  
  Perform the first phase of two phase construction 
  */
    CHWRMLightTestBAppView();
    };


#endif // __HWRMLightTestB_APPVIEW_H__
