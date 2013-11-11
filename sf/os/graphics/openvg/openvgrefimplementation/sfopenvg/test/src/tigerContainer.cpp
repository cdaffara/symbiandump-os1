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
* Implementation of CTigerContainer class
*/


// INCLUDE FILES
#include "TigerContainer.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CTigerContainer::ConstructL(const TRect& aRect)
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CTigerContainer::ConstructL(const TRect& /*aRect*/)
    {
//    iOpenGlInitialized = EFalse;
    CreateWindowL();

    SetExtentToWholeScreen();                // Take the whole screen into use
    ActivateL();
            
    TSize size;
    size = this->Size();

    iTiger = CTiger::NewL(size.iWidth, size.iHeight ); // Create an instance of Tiger
    
    iTiger->AppInit(Window());
    
    return;
    }

// Destructor
CTigerContainer::~CTigerContainer()
    {
    /* AppExit call is made to release
       any allocations made in AppInit. */
    if ( iTiger )
        {
        iTiger->AppExit();
        delete iTiger;
        }
    }

// ---------------------------------------------------------
// CTigerContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CTigerContainer::SizeChanged()
    {
    if( iTiger )
        {
        TSize size;
        size = this->Size();
        iTiger->SetScreenSize( size.iWidth, size.iHeight );
        }
    }


// ---------------------------------------------------------
// CTigerContainer::HandleResourceChange(
//     TInt aType)
// Dynamic screen resize changes by calling the
// SetExtentToWholeScreen() method again.
// ---------------------------------------------------------
//
 void CTigerContainer::HandleResourceChange(TInt aType)
    {
	switch( aType )
    	{
	    case KEikDynamicLayoutVariantSwitch:
		    SetExtentToWholeScreen();
		    break;
	    }
    }

// ---------------------------------------------------------
// CTigerContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CTigerContainer::CountComponentControls() const
    {
    return 0;
    }

// ---------------------------------------------------------
// CTigerContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CTigerContainer::ComponentControl(TInt /*aIndex*/ ) const
    {
    return NULL;
    }

// ---------------------------------------------------------
// CTigerContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CTigerContainer::Draw(const TRect& aRect ) const
    {
    
    CWindowGc& gc = SystemGc();
    TRgb color(KRgbRed);
    gc.SetBrushColor(color);
	gc.SetPenStyle(CGraphicsContext::ENullPen);
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc.DrawRect(Rect());
	
	//set up destination CFbsBitmap object
	CFbsBitmap* bitmap=NULL;
	TRAPD(err, bitmap = new(ELeave)CFbsBitmap;);
	if(bitmap)
		{
		const TDisplayMode dispMode = Window().DisplayMode();
			//copy from source (EGL) to target(CFbsBitmap) buffer
		iTiger->AppRender(aRect, bitmap);		
		RenderBitmap(gc, bitmap);
		delete bitmap;
		}
	}

void  CTigerContainer::RenderBitmap(CWindowGc& aGc, CFbsBitmap* aBitmap) const
	{
	//draw bitmap
	// calculate position for top left of bitmap so it is centered
	TSize bmpSizeInPixels=aBitmap->SizeInPixels();
	TRect rect=Rect(); // a centered rectangle of the default size
	TInt xDelta=(rect.Width()-bmpSizeInPixels.iWidth)/2;
	TInt yDelta=(rect.Height()-bmpSizeInPixels.iHeight)/2;
	TPoint pos=TPoint(xDelta,yDelta); // displacement vector
	pos+=rect.iTl; // bitmap top left corner position
	aGc.BitBlt(pos, aBitmap); // CWindowGc member function	
	}



// ---------------------------------------------------------
// CTigerContainer::HandleControlEventL(
//     CCoeControl* aControl,TCoeEvent aEventType)
// ---------------------------------------------------------
//
void CTigerContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/)
    {
    }

// End of File
