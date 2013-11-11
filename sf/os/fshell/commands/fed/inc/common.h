// common.h
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#ifndef COMMON_H_
#define COMMON_H_

#include <e32std.h>
#include <e32def.h>
#include <e32cmn.h>
#include <e32keys.h>

#ifdef ASSERT
#undef ASSERT
#endif
#define ASSERT(x) __ASSERT_ALWAYS(x, AssertionFail(#x, __FILE__, __LINE__))
void AssertionFail(const char* aAssertion, const char* aFile, TInt aLine);

#if defined(_DEBUG)
#define ASSERT_DEBUG(c,p) ASSERT(c)
#else
#define ASSERT_DEBUG(c,p)
#endif

#define CTRL(x) ((x)-'a'+1)

class DCommandWindow;

class TWindow
{
public:
	TWindow()
		: iX(0), iY(0), iWidth(0), iHeight(0) {}
	TWindow(TInt aPosX, TInt aPosY, TInt aWidth, TInt aHeight)
		: iX(aPosX), iY(aPosY), iWidth(aWidth), iHeight(aHeight) {}

	TBool operator==(const TWindow& aWindow) const
		{return iX == aWindow.iX && iY == aWindow.iY && iWidth == aWindow.iWidth && iHeight == aWindow.iHeight;}

	TPoint Relocate(const TPoint& aPos) const
		{return TPoint(iX + aPos.iX, iY + aPos.iY);}

	TInt NextX() const {return iX + iWidth;}
	TInt NextY() const {return iY + iHeight;}

	TPoint Origin() const { return TPoint(iX, iY); }

	TInt iX;
	TInt iY;
	TInt iWidth;
	TInt iHeight;
};

//Overloaded by a class which can draw on the console
class MViewController
{
public:
	/*
	 Activate the View and set the new position and size of the window (which may be not changed).
	 The window is visible so only the difference between the old and the new window has to be redrawn.
	 Used when the window needs to be resized or moved.*/
	virtual void ResizeL(const TWindow& aWindow) = 0;
	/*
	 Activate the View and set the new position and size of the window (which may be not changed).
	 The View is not visible so it has to redraw the whole window.
	 Used when switching views visible in the same window.*/
	virtual void RedrawL(const TWindow& aWindow) = 0;
	/*
	This view must stop drawing on the console, however It can still process data in the background.*/
	virtual void DeactivateL() = 0;
};

//Overloaded by a class which uses a memory cache shared by another class (i.e. by MShareCacheProvider)
class MSharedCacheClient
{
	//Gives the class, which owns the buffer, a chance to complete all writings using data in the buffer
	virtual void InvalidateBuffer(TRequestStatus& aStatus) = 0;
};

class TRange;

/*
 Overloaded by a class which provides shared access to its memory cache. The MSharedCacheClient class, which
 needs access to the memory cache provided by this class, supplies TPtrC8 descriptor pointer, which is being
 set to point to the existing data loaded to the memory cache and maintained by this class.
*/
class MSharedCacheProvider
	{
	//Called to add a new client to this file buffer
	virtual TInt RegisterClient(MSharedCacheClient& aClient, TPtrC& aDes, TRange& aRange, TInt& aRangeStartLineNumber) = 0;
	virtual void UnregisterClient(MSharedCacheClient& aClient) = 0;
	};

//Overloaded by a class which can receive key input from the user
class MKeyConsumer
{
public:
	//Returns EFalse if the key wasn't consumed
	virtual TBool ConsumeKeyL(const TKeyCode& aCode) = 0;
};

//Overloaded by a class which can defer closing the application
class MDeferredClose
{
public:
	/*
	 Called when the object is about to be closed, gives a chance to save changed data.
	 If the object can be closed immediately it should return ETrue, otherwise EFalse.
	 If any data or info should be shown to the user then a request should be registered with MRequestHandler*/
	virtual TBool TryCloseL() = 0;
};

class CConsoleBase;
class CColorConsoleBase;
class RFs;

class MConsoleProvider
	{
public:
	virtual CConsoleBase& Console() = 0;
	virtual CColorConsoleBase* ColorConsole() = 0;
	virtual RFs& Fs() = 0;
	virtual void WriteStatus(const TDesC& aNameToTruncate, TRefByValue<const TDesC> aFmt, ...) = 0;
	virtual void InfoPrint(TRefByValue<const TDesC> aFmt, ...) = 0;
	virtual TKeyCode Query(const TDesC& aPrompt, const TDesC& aValidKeys) = 0;
	virtual TBool QueryFilename(const TDesC& aPrompt, TFileName& aFileName) = 0;
	virtual TBool QueryText(const TDesC& aPrompt, TDes& aText) = 0;
	};

// General helper class, suprised we don't have anything else suitable...
class TRange
	{
public:
	inline TRange() : iLocation(0), iLength(0) {}
	inline TRange(TInt aLocation, TInt aLength)
		: iLocation(aLocation), iLength(aLength)
		{}
		
	inline TInt Max() const
		{
		return iLocation + iLength;
		}

	inline TBool Contains(TInt aLocation) const
		{
		return aLocation >= iLocation && aLocation < Max();
		}
	inline void Intersect(const TRange& aRange)
		{
		TInt end = Min(this->Max(), aRange.Max());
		iLocation = ::Max(iLocation, aRange.iLocation); // Have to use :: to say "look at global namespace implementation of Max()"
		iLength = end - iLocation;
		}
public:
	TInt iLocation;
	TInt iLength;
	};

extern TInt KConsoleWidthCorrection;
extern TInt KConsoleHeightCorrection;
TInt TabWidth();
void ShowHelpL();

#endif /*COMMON_H_*/
