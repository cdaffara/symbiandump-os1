// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Part of the MVS Application for TechView
//

#include  <w32std.h>

#include "MVSApp.h"
#include "MVSAppUI.h"
#include "mvsvideocontrol.h"

CMVSAppView* CMVSAppView::NewL(const TRect& aRect, CMVSVideoPlayAgent& aVideoPlayAgent)
	{
  	CMVSAppView * self = new(ELeave) CMVSAppView;
 	CleanupStack::PushL(self);
  	self->ConstructL(aRect, aVideoPlayAgent);
  	CleanupStack::Pop();
  	return self;
  	}


CMVSAppView::CMVSAppView()
    {
    }


CMVSAppView::~CMVSAppView()
	{
  	//Remove our controls
  	delete iMainWindowControl;
  	delete iStatusWindowFNameControl;
  	delete iStatusWindowStateControl;
  	delete iStatusWindowTimeControl;
  	delete iInfoWindowControl;
  	delete iProgress;
  	
  	CloseWindow();
  	}


void CMVSAppView::ConstructL(const TRect& aRect, CMVSVideoPlayAgent& aVideoPlayAgent)
	{
  	//we need a window in which to place controls, so take ownership of one
  	CreateWindowL();
  
  	//Set the extent of the control.
  	SetRect(aRect);
  	TRect fullRectWindow = Rect();
  	//vertical point co-ordinate
  	TInt statusPaneHeight = 4*(fullRectWindow.iTl.iY + fullRectWindow.iBr.iY)/5;
  	TInt halfLength = (fullRectWindow.iTl.iX + fullRectWindow.iBr.iX)/2;
  	TInt threeQuarterLength= 3*(fullRectWindow.iTl.iX + fullRectWindow.iBr.iX)/4;

	//set the dimensions of our main window
  	TRect mainWindow(Rect());
  	mainWindow.iTl.iX = halfLength-115;
  	mainWindow.iBr.iY = statusPaneHeight;
  
	iMainWindowControl = CMVSVideoControl::NewL(aVideoPlayAgent, 0, Window());
	iMainWindowControl->SetExtent(mainWindow.iTl, mainWindow.Size());

  	//set the dimensions of our status window
  	TRect statusWindowFName = fullRectWindow;
  	statusWindowFName.iTl.iY = statusPaneHeight;
  	statusWindowFName.iBr.iX = halfLength;

  	//now construct our status window filename control
  	iStatusWindowFNameControl = CMVSFileControl::NewL(*this, statusWindowFName,
                                                     KNullDesC);
   	//set the dimensions of our state window
  	TRect statusWindowState = fullRectWindow;
  	statusWindowState.iTl.iX = halfLength;
  	statusWindowState.iTl.iY = statusPaneHeight;
  	statusWindowState.iBr.iX = threeQuarterLength;

  	//now construct our status window state control
  	iStatusWindowStateControl = CMVSStateControl::NewL(*this, statusWindowState, 
                                                     KNullDesC);
  
  	//set the dimensions of our timer window
  	TRect statusWindowTime = fullRectWindow;
  	statusWindowTime.iTl.iX = threeQuarterLength;
  	statusWindowTime.iTl.iY = statusPaneHeight;
  
  	//now construct our status window timer control
  	iStatusWindowTimeControl = CMVSTimeControl::NewL(*this, statusWindowTime, 
                                                    KNullDesC);
    TRect infoWindow = fullRectWindow;
  	infoWindow.iTl.iX=0;
  	infoWindow.iBr.iX= halfLength-115;
  	infoWindow.iBr.iY =statusPaneHeight-15;
  	iInfoWindowControl = CMVSInfoControl::NewL(*this,infoWindow,KNullDesC);
  	iInfoWindowControl->SetParent(this);
  	//now activate the control.
  	
  	// construct and draw the progress bar 
  	iProgress = new (ELeave) CEikProgressInfo();
  	
  	TResourceReader reader;
  	iEikonEnv->CreateResourceReaderLC(reader, R_CLIP_PROGRESSINFO);
	iProgress->ConstructFromResourceL(reader);
	iProgress->SetContainerWindowL(*this);
	iProgress->SetExtent(TPoint(fullRectWindow.iTl.iX+ (fullRectWindow.iBr.iX/20),statusPaneHeight-10),TSize((fullRectWindow.iBr.iX - fullRectWindow.iTl.iX - (fullRectWindow.iBr.iX/10) ),10));
	iProgress->SetAndDraw(0);
	CleanupStack::PopAndDestroy();	
	
  	ActivateL();
  	}

void CMVSAppView::SetClipLength(TInt& aClipLen)
	{
	iClipLength = aClipLen;
	}

//
//CountComponentControls()
//
// Implemented by a view with more than one control so that a call to
// DrawNow successfully draws all four component controls.
//
TInt CMVSAppView::CountComponentControls() const
	{
	return 6; //we have six controls
	}
	
//
// To handle the Pointer events on the Progress Bar
//
void CMVSAppView::HandlePointerEventL(const TPointerEvent &aPointerEvent)
	{
	TInt clipPos;
	TRect rect = iProgress->Rect();
	if(	(aPointerEvent.iPosition.iX>=rect.iTl.iX && aPointerEvent.iPosition.iX<=rect.iBr.iX) && 
		(aPointerEvent.iPosition.iY>=rect.iTl.iY && aPointerEvent.iPosition.iY<=rect.iBr.iY)	)
		{
		TInt pos = aPointerEvent.iPosition.iX - rect.iTl.iX;
		switch(aPointerEvent.iType)
			{
			case TPointerEvent::EButton1Down:
				break;
			case TPointerEvent::EDrag:
				{
				iProgress->SetAndDraw(pos);
				break;
				}
			case TPointerEvent::EButton1Up:
				{
				TInt progToDraw = pos* (static_cast<double>(200)/(rect.iBr.iX -rect.iTl.iX));
				iProgress->SetAndDraw(progToDraw+1);
				clipPos = (progToDraw+1) *(static_cast<double>(iClipLength)/200);
				TTimeIntervalMicroSeconds clipTime(clipPos);	
				static_cast<CMVSAppUi*>(CEikonEnv::Static()->EikAppUi())->SetPosition(clipTime);
				break;
				}
				
			default:
				break;
			}
		}
	CCoeControl::HandlePointerEventL(aPointerEvent);
	
	}
	
	
//
// Updates the Audion play progress 
//
void CMVSAppView::UpdatePlayProgress(TTimeIntervalMicroSeconds& aPos)
	{
	TInt curPos = I64INT(aPos.Int64());
	TInt percent = curPos*(static_cast<double>(100)/iClipLength);
	TInt progressPos = percent*2;
	iProgress->SetAndDraw(progressPos+1);
	}
	
//	
// Reset the Progress to ) and redraw	
//
void CMVSAppView::ResetProgressBar()
	{
	iProgress->SetAndDraw(0);
	}
//
//ComponentControl(...)
//
// Returns the control by index. The counting sequence goes left to right, top
// to bottom.
//
CCoeControl* CMVSAppView::ComponentControl(TInt aIndex) const
	{
	switch (aIndex)
		{
	 	case 0: return iMainWindowControl;
	 	case 1: return iStatusWindowFNameControl;
     	case 2: return iStatusWindowStateControl;
     	case 3: return iStatusWindowTimeControl;
	 	case 4: return iInfoWindowControl;
	 	case 5: return iProgress;
	 	default: return 0;
		}
	}



//
//
// class CMVSFileControl
//
//



//
//NewL(...)              *** This method can LEAVE ***
//
// Factory contructor, initialises a control based on the rectangle provided.
//
CMVSFileControl* CMVSFileControl::NewL(const CCoeControl& aContainer, 
                                         const TRect& aRect, 
                                         const TDesC& aText)
	{
	CMVSFileControl* self=new(ELeave) CMVSFileControl;
	CleanupStack::PushL(self);
	self->ConstructL(aContainer, aRect, aText);
	CleanupStack::Pop();
	return self;
	}


//
//ContructL(...)              *** This method can LEAVE ***
//
//
void CMVSFileControl::ConstructL(const CCoeControl& aContainer, 
                                  const TRect& aRect, const TDesC& aText)
    {
	SetContainerWindowL(aContainer);
	SetRect(aRect);
    iWindow = aRect;
	SetTextL(aText);
    //Activate the control
	ActivateL();
	}


//
//CMVSControl()
//
// Constructor, does nothing. Private to prevent it being called.
//
CMVSFileControl::CMVSFileControl()
     {
     }


//
//~CMVSControl()
//
// Destructor.
//
CMVSFileControl::~CMVSFileControl()
     {
     delete iText;
     }


//Draw the FileName Display Window
void CMVSFileControl::Draw(const TRect& /*aRect*/) const
	{
	//Establish a Graphics Context
	CWindowGc& gc=SystemGc();

    //Establish a drawing rectangle
	TRect rect=Rect();

    //Move in 2 pixels each side to give a white border
	rect.Shrink(2,2);

    //Set-up a pen
	gc.SetPenStyle(CGraphicsContext::ENullPen);
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc.SetBrushColor(KRgbDarkGray);

    //Draw a blank rectangle
    gc.DrawRect(rect);

    //Fill in the border-regions
	DrawUtils::DrawBetweenRects(gc, Rect(), rect); 

    //Change the pen colour to black
    gc.SetPenStyle(CGraphicsContext::ESolidPen);
	gc.SetPenColor(KRgbWhite);
	
	//Set the fill colour to 'no fill'
    gc.SetBrushStyle(CGraphicsContext::ENullBrush);

    //Draw a rectangle (transparent with a black border)
	gc.DrawRect(rect);
	rect.Shrink(1,1);
     
    const CFont* appFont = iEikonEnv->AnnotationFont();
    DrawOtherWindows(gc,rect,appFont);
    return;
	}



//
//DrawOtherWindows(...)
//
// Draw the text in a central position in the window.
//
void CMVSFileControl::DrawOtherWindows(CGraphicsContext& aGc, 
                                        const TRect& aDeviceRect, 
                                        const CFont* aFont) const
     {
     //Set up the pen and brush colours
     aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	 aGc.SetBrushColor(KRgbGray);

	 aGc.SetPenStyle(CGraphicsContext::ESolidPen);
	 aGc.SetPenColor(KRgbWhite);
	
     //Set-up a font
     aGc.UseFont(aFont);

     //Set the baseline to be half the height of the rectangle + half
     //the height of the font
	 TInt baseline=aDeviceRect.Height()/2 + aFont->AscentInPixels()/2;

     //Draw the text
     //__ASSERT_ALWAYS(iText != NULL, User::Panic(KNullPtr, KNAPanicNullPointer));
	 aGc.DrawText(*iText, aDeviceRect, baseline, CGraphicsContext::ECenter);

     //Done with the font
	 aGc.DiscardFont();
     }

void CMVSFileControl::SetTextL(const TDesC& aText)
     {
     HBufC* text=aText.AllocL();
	 delete iText;
	 iText=text;
     }

//
//Window()
//
// Returns the window defined by this object.
TRect& CMVSFileControl::Window()
     {
     return iWindow;
     }
     
        

//
//
// class CMVSStateControl
//
//



//
//NewL(...)              *** This method can LEAVE ***
//
// Factory contructor, initialises a control based on the rectangle provided.
//
CMVSStateControl* CMVSStateControl::NewL(const CCoeControl& aContainer, 
                                         const TRect& aRect, 
                                         const TDesC& aText)
	{
	CMVSStateControl* self=new(ELeave) CMVSStateControl;
	CleanupStack::PushL(self);
	self->ConstructL(aContainer, aRect, aText);
	CleanupStack::Pop();
	return self;
	}



//
//ContructL(...)              *** This method can LEAVE ***
//
//
void CMVSStateControl::ConstructL(const CCoeControl& aContainer, 
                                  const TRect& aRect, const TDesC& aText)
    {
	SetContainerWindowL(aContainer);
	SetRect(aRect);
    iWindow = aRect;
	SetTextL(aText);
	
    //Activate the control
	ActivateL();
	}



//
//CMVSControl()
//
// Constructor, does nothing. Private to prevent it being called.
//
CMVSStateControl::CMVSStateControl()
     {
     }


//
//~CMVSControl()
//
// Destructor.
//
CMVSStateControl::~CMVSStateControl()
     {
     delete iText;
     }




//For Displaying the state
void CMVSStateControl::Draw(const TRect& /*aRect*/) const
	{
	//Establish a Graphics Context
	CWindowGc& gc=SystemGc();

    //Establish a drawing rectangle
	TRect rect=Rect();

    //Move in 2 pixels each side to give a white border
	rect.Shrink(2,2);

    //Set-up a pen
	gc.SetPenStyle(CGraphicsContext::ENullPen);
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc.SetBrushColor(KRgbDarkGray);

    //Draw a blank rectangle
    gc.DrawRect(rect);

    //Fill in the border-regions
	DrawUtils::DrawBetweenRects(gc, Rect(), rect); 

    //Change the pen colour to black
    gc.SetPenStyle(CGraphicsContext::ESolidPen);
	gc.SetPenColor(KRgbWhite);
	
	//Set the fill colour to 'no fill'
    gc.SetBrushStyle(CGraphicsContext::ENullBrush);

    //Draw a rectangle (transparent with a black border)
	gc.DrawRect(rect);
	rect.Shrink(1,1);

    const CFont* appFont = iEikonEnv->AnnotationFont();
          DrawOtherWindows(gc,rect,appFont);
    return;
	}




//
//DrawOtherWindows(...)
//
// Draw the text in a central position in the window.
//
void CMVSStateControl::DrawOtherWindows(CGraphicsContext& aGc, 
                                        const TRect& aDeviceRect, 
                                        const CFont* aFont) const
    {
    //Set up the pen and brush colours
    aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.SetBrushColor(KRgbGray);

	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
	aGc.SetPenColor(KRgbWhite);
	
    //Set-up a font
    aGc.UseFont(aFont);

    //Set the baseline to be half the height of the rectangle + half
    //the height of the font
	TInt baseline=aDeviceRect.Height()/2 + aFont->AscentInPixels()/2;

    //Draw the text
    //__ASSERT_ALWAYS(iText != NULL, User::Panic(KNullPtr, KNAPanicNullPointer));
	aGc.DrawText(*iText, aDeviceRect, baseline, CGraphicsContext::ECenter);

    //Done with the font
	aGc.DiscardFont();
    }

void CMVSStateControl::SetTextL(const TDesC& aText)
    {
    HBufC* text=aText.AllocL();
	delete iText;
	iText=NULL;
	iText=text;
    }

//
//Window()
//
// Returns the window defined by this object.
TRect& CMVSStateControl::Window()
    {
    return iWindow;
    }
    
    
     
//
//
// class CMVSTimeControl
//
//



//
//NewL(...)              *** This method can LEAVE ***
//
// Factory contructor, initialises a control based on the rectangle provided.
//
CMVSTimeControl* CMVSTimeControl::NewL(const CCoeControl& aContainer, 
                                         const TRect& aRect, 
                                         const TDesC& aText)
	{
	CMVSTimeControl* self=new(ELeave) CMVSTimeControl;
	CleanupStack::PushL(self);
	self->ConstructL(aContainer, aRect, aText);
	CleanupStack::Pop();
	return self;
	}



//
//ContructL(...)              *** This method can LEAVE ***
//
//
void CMVSTimeControl::ConstructL(const CCoeControl& aContainer, 
                                  const TRect& aRect, const TDesC& aText)
    {
	SetContainerWindowL(aContainer);
	SetRect(aRect);
    iWindow = aRect;
    SetTextL(aText);
    iText2 = NULL;
    //Activate the control
	ActivateL();
	}



//
//CMVSControl()
//
// Constructor, does nothing. Private to prevent it being called.
//
CMVSTimeControl::CMVSTimeControl()
    {
    }


//
//~CMVSControl()
//
// Destructor.
//
CMVSTimeControl::~CMVSTimeControl()
    {
    delete iText;
    if(iText2)
    	{
    	delete iText2;
    	}
    }




//For TimeDisplay
void CMVSTimeControl::Draw(const TRect& /*aRect*/) const
	{
     //Establish a Graphics Context
	CWindowGc& gc=SystemGc();

     //Establish a drawing rectangle
	TRect rect=Rect();

    //Move in 2 pixels each side to give a white border
	rect.Shrink(2,2);

    //Set-up a pen
	gc.SetPenStyle(CGraphicsContext::ENullPen);
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc.SetBrushColor(KRgbDarkGray);

    //Draw a blank rectangle
    gc.DrawRect(rect);

    //Fill in the border-regions
	DrawUtils::DrawBetweenRects(gc, Rect(), rect); 

    //Change the pen colour to black
    gc.SetPenStyle(CGraphicsContext::ESolidPen);
	gc.SetPenColor(KRgbWhite);
	
	//Set the fill colour to 'no fill'
    gc.SetBrushStyle(CGraphicsContext::ENullBrush);

    //Draw a rectangle (transparent with a black border)
	gc.DrawRect(rect);
	rect.Shrink(1,1);

    const CFont* appFont = iEikonEnv->AnnotationFont();
    DrawOtherWindows(gc,rect,appFont);
    return;
	}




//
//DrawOtherWindows(...)
//
// Draw the text in a central position in the window.
//
void CMVSTimeControl::DrawOtherWindows(CGraphicsContext& aGc, 
                                        const TRect& aDeviceRect, 
                                        const CFont* aFont) const
    {
    //Set up the pen and brush colours
    aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.SetBrushColor(KRgbGray);
	
	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
	aGc.SetPenColor(KRgbWhite);
	
    //Set-up a font
    aGc.UseFont(aFont);
	
	
	if(iText2)
		{
		TInt rWidth =  aDeviceRect.iBr.iX - aDeviceRect.iTl.iX;
		TInt rHeight = aDeviceRect.iBr.iY - aDeviceRect.iTl.iY;
		
		TRect rect1(aDeviceRect.iTl,TSize(rWidth, rHeight/2));
		TRect rect2(TPoint(aDeviceRect.iTl.iX, aDeviceRect.iTl.iY + rHeight/2),TSize(rWidth, rHeight/2));
		
		TInt baseline = rect1.Height();
	    //Draw the text 1
	    aGc.DrawText(*iText, rect1, baseline - 2, CGraphicsContext::ECenter);	
	    //Draw the text 2
	    aGc.DrawText(*iText2, rect2, baseline - 3 , CGraphicsContext::ECenter);	
		}
    else
    	{
    	//Set the baseline to be half the height of the rectangle + half
	    //the height of the font
		TInt baseline=aDeviceRect.Height()/2 + aFont->AscentInPixels()/2;

	    //Draw the text
	    aGc.DrawText(*iText, aDeviceRect, baseline, CGraphicsContext::ECenter);	
    	}

    //Done with the font
	aGc.DiscardFont();
    }

void CMVSTimeControl::SetTextL(const TDesC& aText)
    {
    HBufC* text=aText.AllocL();
	delete iText;
	iText=NULL;
	iText=text;
    }
   
void CMVSTimeControl::SetText2L(const TDesC& aText) 
	{
	HBufC* text=aText.AllocL();
	delete iText2;
	iText2=NULL;
	iText2=text;
	}
	
void CMVSTimeControl::ResetText2L()	
	{
	delete iText2;
	iText2=NULL;
	}
//
//Window()
//
// Returns the window defined by this object.
TRect& CMVSTimeControl::Window()
    {
    return iWindow;
    }
    
     
//
//
// class CMVSInfoControl
//
//



//
//NewL(...)              *** This method can LEAVE ***
//
// Factory contructor, initialises a control based on the rectangle provided.
//
CMVSInfoControl* CMVSInfoControl::NewL(const CCoeControl& aContainer, 
                                         const TRect& aRect, 
                                         const TDesC& aText)
	{
	CMVSInfoControl* self=new(ELeave) CMVSInfoControl;
	CleanupStack::PushL(self);
	self->ConstructL(aContainer, aRect, aText);
	CleanupStack::Pop();
	return self;
	}



//
//ContructL(...)              *** This method can LEAVE ***
//
//
void CMVSInfoControl::ConstructL(const CCoeControl& aContainer, 
                                  const TRect& aRect, const TDesC& aText)
    {
	SetContainerWindowL(aContainer);
	SetRect(aRect);
    iWindow = aRect;
    SetTextL(aText);
    //Activate the control
	ActivateL();
	}



//
//CMVSControl()
//
// Constructor, does nothing. Private to prevent it being called.
//
CMVSInfoControl::CMVSInfoControl()
    {
    }


//
//~CMVSControl()
//
// Destructor.
//
CMVSInfoControl::~CMVSInfoControl()
    {
    delete iText;
    }



//
//Draw(...)
//
void CMVSInfoControl::Draw(const TRect& /*aRect*/) const
	{
    //Establish a Graphics Context
	CWindowGc& gc=SystemGc();

    //Establish a drawing rectangle
	TRect rect=Rect();

    //Move in 2 pixels each side to give a white border
	rect.Shrink(2,2);

    //Set-up a pen
	gc.SetPenStyle(CGraphicsContext::ENullPen);
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc.SetBrushColor(KRgbDarkGray);

    //Draw a blank rectangle
    //gc.DrawRect(rect);
    
    //To cover Progress Bar area
    TRect ProgRect = rect;
    TInt statusPaneHeight = 4*(Parent()->Rect().iTl.iY + Parent()->Rect().iBr.iY)/5;
    ProgRect.iBr.iY = statusPaneHeight;	
    ProgRect.Grow(2,0);
    gc.DrawRect(ProgRect);
    
    //Fill in the border-regions
	DrawUtils::DrawBetweenRects(gc, Rect(), rect); 

    //Change the pen colour to black
    gc.SetPenStyle(CGraphicsContext::ESolidPen);
	gc.SetPenColor(KRgbWhite);
	
	//Set the fill colour to 'no fill'
    gc.SetBrushStyle(CGraphicsContext::ENullBrush);

    //Draw a rectangle (transparent with a black border)
	gc.DrawRect(rect);
	rect.Shrink(1,1);

    const CFont* appFont = iEikonEnv->AnnotationFont();
    DrawMainWindow(gc,rect,appFont);
    return;
	}



void CMVSInfoControl::DrawMainWindow(CGraphicsContext& aGc, 
                                      const TRect& /*aDeviceRect*/, 
                                      const CFont* aFont) const
    {
    //Set up a brush and pen
    aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.SetBrushColor(KRgbDarkGray);
	
	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
	aGc.SetPenColor(KRgbWhite);

    //Get a font
    aGc.UseFont(aFont);

    //The 'step' by which we move down to get to a fresh line in
    //the window
    TInt distToNextBaseline = (aFont->AscentInPixels()/2)*3;
	
    //The main window text
    TPtrC mainWindowText;
    mainWindowText.Set(iText->Des());

    //The escape sequence
    _LIT(KDollarDollar, "$$");

    TRect rect;
    TInt x = 10; //The left hand side of the rectangle
    TInt y = 10; //The top of the rectangle.
     
    //Whilst we can find a '$$' in the string
    while(mainWindowText.Find(KDollarDollar) != KErrNotFound)
    	{
       	//do text up to $$
        TInt pos = mainWindowText.Find(KDollarDollar);
        TPtrC text(mainWindowText.Mid(0,pos));
         
        //define the rectangle for this text
        TInt height = aFont->HeightInPixels();
	    TInt width = aFont->TextWidthInPixels( text );
	    rect.SetRect( x, y, x + width, y + height );
         
		//Draw the text
        aGc.DrawText(text, rect, height - aFont->DescentInPixels(), 
                     CGraphicsContext::ELeft);
	     
        //delete text upto and including '$$'
        text.Set(mainWindowText.Right(mainWindowText.Length() - (pos+2)));
        mainWindowText.Set(text);

		//adjust the baseline offset
        y+=distToNextBaseline;
        }

    //Done with our font
    aGc.DiscardFont();
    }


void CMVSInfoControl::SetTextL(const TDesC& aText)
    {
    HBufC* text=aText.AllocL();
	delete iText;
	iText=NULL;
	iText=text;
    }

//
//Window()
//
// Returns the window defined by this object.
TRect& CMVSInfoControl::Window()
    {
    return iWindow;
    }
