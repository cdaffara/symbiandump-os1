// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32hal.h>
#include <fbs.h>
#include "UTILS.H"
#include <linkedfonts.h>
#include "FbsMessage.H"

GLREF_C void Panic(TFbsPanic aPanic);

CFbsTypefaceStore::CFbsTypefaceStore(CGraphicsDevice* aDevice):
	CTypefaceStore(),
	iDevice(aDevice),
	iTwipsCache(NULL)
	{
	}

/** 
@publishedAll 
@released
*/
EXPORT_C CFbsTypefaceStore::~CFbsTypefaceStore()
	{
	if(iTwipsCache)
		{
		ReleaseTwipsCache();
		delete iTwipsCache;
		}
	}

/** Allocates and constructs a CFbsTypefaceStore, specifying a graphics device.
@param aDevice A pointer to a graphics device.
@return A pointer to the newly created typeface store. 
@publishedAll 
@released
*/
EXPORT_C CFbsTypefaceStore* CFbsTypefaceStore::NewL(CGraphicsDevice* aDevice)
	{
	CFbsTypefaceStore* thisptr=new(ELeave) CFbsTypefaceStore(aDevice);
	CleanupStack::PushL(thisptr);
	thisptr->ConstructL();
	CleanupStack::Pop();
	return(thisptr);
	}


void CFbsTypefaceStore::ConstructL()
	{
	// get the session info from the Tls
	iFbs = RFbsSession::GetSession();
	if (!iFbs)
		{
		User::Leave(KErrCouldNotConnect);
		}
	CTypefaceStore::ConstructL();
	iTwipsCache = new(ELeave) CFontCache;
	}


/** Installs a font store file into the typeface store.
All the fonts in added and installed font files are available to the
GetNearestFont...() family of functions.
Additionally Bitmap Fonts are also available to GetFontById().

Installed files remain in the typeface store even after the client which added 
them is destroyed. They can be removed using RemoveFile().
@param aName The name of the file to be installed.
@param aId On return, contains the id of the installed file.
@return KErrNone if successful, otherwise another of the system-wide error 
codes.
@see AddFile()
@see RemoveFile()
@see GetFontById()
@see GetNearestFontToDesignHeightInTwips()
@see GetNearestFontToDesignHeightInPixels()
@see GetNearestFontToMaxHeightInTwips()
@see GetNearestFontToMaxHeightInPixels()
@publishedAll 
@released
*/
EXPORT_C TInt CFbsTypefaceStore::InstallFile(const TDesC& aName,TInt& aId)
	{
	TPckgBuf<TIntParcel> ip;
	aId=0;
	TIpcArgs args(&aName,aName.Length(),&ip);
	TInt ret=iFbs->SendCommand(EFbsMessInstallFontStoreFile,args);
	if(ret==KErrNone)
		aId=ip().iInt;
	return(ret);
	}

/** Adds a font store file to the typeface store.
All the fonts in added and installed font files are available to the
GetNearestFont...() family of functions.
Additionally Bitmap Fonts are also available to GetFontById().

This function adds the typeface to a reference counted list of fonts. Each 
client that adds the typeface to the store increases the reference count. 
The count is decremented when a client using the typeface is destroyed or 
calls the RemoveFile() function. The typeface is removed from the store only 
when the the reference count is zero (it is not being used by any clients).
The InstallFile() function is similar, except that the typeface is not reference 
counted, and is hence not removed when all the clients using it are destroyed.
@param aName A descriptor containing the filename of the typeface store
@param aId On return, contains the id of the typeface.
@return KErrNone if successful, otherwise another of the system-wide error 
codes.
@see InstallFile()
@see RemoveFile()
@see GetFontById()
@see GetNearestFontToDesignHeightInTwips()
@see GetNearestFontToDesignHeightInPixels()
@see GetNearestFontToMaxHeightInTwips()
@see GetNearestFontToMaxHeightInPixels()
@publishedAll 
@released
*/
EXPORT_C TInt CFbsTypefaceStore::AddFile(const TDesC& aName,TInt& aId)
	{
	TPckgBuf<TIntParcel> ip;
	aId=0;
	TIpcArgs args(&aName,aName.Length(),&ip);
	TInt ret=iFbs->SendCommand(EFbsMessAddFontStoreFile,args);
	if(ret==KErrNone)
		aId=ip().iInt;
	return(ret);
	}

/** Decrements the reference count of a file which was added using 
AddFile(), and removes it from the store if the reference count reaches zero. 
If the font was not found in the list of reference-counted files (see AddFile()) 
it is assumed to be an installed file (see InstallFile()) and an attempt is 
made to remove it anyway.

If the id given is 0, an attempt is made to remove all font objects from the 
font store provided none of the fonts in the store are currently accessed,
otherwise it has no effect.

Note:
The id passed to this function has a different meaning depending on whether 
or not the file is a Symbian-format bitmap file. If it is a Symbian-format 
bitmap file the id is a UID, and is the same from one session to the next. 
If it is an Open Font System file (e.g., a TrueType file) the id is an arbitrary 
number. Consequently the id may vary from one session to the next, and should 
not be saved in a file: however it may be kept and used by a client as 
long as the client is running.

@param aId The id of the file to be removed/decremented, set by AddFile() 
or InstallFile().
@see AddFile()
@see InstallFile()
@publishedAll 
@released
*/
EXPORT_C void CFbsTypefaceStore::RemoveFile(TInt aId)
	{
	iFbs->SendCommand(EFbsMessRemoveFontStoreFile,aId);
	}

/** Gets the number of typefaces supported by this store.
@return The number of supported typefaces. 
@see CTypefaceStore::NumTypefaces()
@publishedAll 
@released
*/
EXPORT_C TInt CFbsTypefaceStore::NumTypefaces() const
	{
	return(iFbs->SendCommand(EFbsMessNumTypefaces));
	}

/**
Gets the font which is the nearest to the given font specification.

When the font is no longer needed, call @c ReleaseFont().

Note that this deprecated function is replaced by the new @c GetNearestFontToDesignHeightInTwips() 
yielding (virtually) the same result. However clients are strongly encouraged to use the new
@c GetNearestFontToMaxHeightInTwips() function instead. This will guarantee that every 
character within any given text string will fit within the given amount of twips, whereas the design 
height is an aesthetic unit decided by the font designer without strict physical meaning, which 
may result in cropped characters.

Chooses from the fonts loaded at system startup or through the AddFile()
or InstallFile() APIs.

@param aFont On return, contains a pointer to the nearest font.
@param aFontSpec The specification of the font to be matched.
@return KErrNone if successful; a system-wide error code otherwise.
@publishedAll
@see GetNearestFontToDesignHeightInTwips()
@see GetNearestFontToMaxHeightInTwips()
@see AddFile()
@see InstallFile()
*/
EXPORT_C TInt CFbsTypefaceStore::GetNearestFontInTwips(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	return GetNearestFontToDesignHeightInTwips(aFont, aFontSpec);
	}

/**
Gets the font which is the nearest to the given font specification.

When the font is no longer needed, call @c ReleaseFont().

Note that this deprecated function is replaced by the new @c GetNearestFontToDesignHeightInPixels() 
yielding (virtually) the same result. However clients are strongly encouraged to use the new
@c GetNearestFontToMaxHeightInPixels() function instead. This will guarantee that every 
character within any given text string will fit within the given amount of pixels, whereas the design 
height is an aesthetic unit decided by the font designer without strict physical meaning, which 
may result in cropped characters.

Chooses from the fonts loaded at system startup or through the AddFile()
or InstallFile() APIs.

@param aFont On return, contains a pointer to the nearest font.
@param aFontSpec The specification of the font to be matched.
@return KErrNone if successful; a system-wide error code otherwise.
@publishedAll
@deprecated Use GetNearestFontToDesignHeightInPixels
@see GetNearestFontToDesignHeightInPixels()
@see GetNearestFontToMaxHeightInPixels()
@see AddFile()
@see InstallFile()
*/
EXPORT_C TInt CFbsTypefaceStore::GetNearestFontInPixels(CFont*& aFont,const TFontSpec& aFontSpec)
	{
	return GetNearestFontToDesignHeightInPixels(aFont, aFontSpec);
	}

TInt CFbsTypefaceStore::GetNearestFontInTwipsAndCreateFont(
	CFont*&				aFont,
	TInt				aFbsMessage, // a TFbsMessage
	const TFontSpec&	aFontSpec,
	TInt				aMaxHeight)
	{
	aFont = iTwipsCache->Search(aFontSpec);
	if (aFont)
		{
		if (IncrementFontCount(aFont))
			{
			return KErrNone;
			}
		Panic(EFbsTypefaceStoreError);
		}

	TInt ret = SendGetNearestFontCommandNCreateFont(
		aFont, aFbsMessage, aFontSpec, aMaxHeight);
	if (KErrNone != ret)
		{
		return ret;
		}

	CFont* discard = NULL;
	/* We are deliberately storing in the cache the TFontSpec requested by the client 
	 * and not the REAL TFontSpec of the font returned by the font matcher.
	 * We are doing this for performance reasons: to use the Font Cache as much as possible. 
	 * Unless the requested font spec is really complete, including all the different flags 
	 * and sub fields, there is only a very small chance that the TFontSpec requested 
	 * by the client would be exactly the same as the best match returned by the server.
	 * Since some flags (iFontStyle.iFlags and iTypeFace.iFlags) are quite complex and 
	 * in general the client don't set up all of them perfectly.
	 * So in order to decrease the number of entries in the cache AND also the requests
	 * to the server, it's better to associate the TFontSpec asked for by the client 
	 * with the CFont found so that next time we ask for it, it will be in the cache.
	 * 
	 * If we request 2 different font specs that both have the same real font as their 
	 * best match then each will appear in the cache but the CFont pointer in both entries
	 * will point to the same real CFont (no copies)
	 * 
	 * PS: a problem is known because of doing this:
	 * if we try to get a Font which is not in the system, we obtain the best match
	 * then if we add this Font in the system (AddFile) and ask again for this Font,
	 * we will still have the previous one which is in the Cache and not the one added.
	 * This problem is the result of "bad" programming/testing, in general when we use a Font,
	 * we know it's here...
	 */
	TRAP(ret, discard = iTwipsCache->AddEntryL(aFont,aFontSpec));
	if (KErrNone == ret)
		{ 
		// Font has been added to cache. Increment reference count, so that this font
		// will only be destroyed once all client handles to it are released, AND it can 
		// not fit in the cache. Even if no clients have a handle to this font, the object
		// will still persist until other fonts force it out of the cache.
		IncrementFontCount(aFont);
		}
	if (discard)
		{ // a font was bumped out of the cache
		ReleaseFont(discard);
		}
	return KErrNone;
	}


/**
 get pixel size in Twips * 1000
 @internalComponent
 */
void CFbsTypefaceStore::GetPixelSizeInTwips(TSize& aSize) const
	{
	if(iDevice)
		{
		aSize.iWidth = iDevice->HorizontalPixelsToTwips(1000);
		aSize.iHeight = iDevice->VerticalPixelsToTwips(1000);
		}
	if(aSize.iWidth==0 || aSize.iHeight==0)
		{
		TMachineInfoV1Buf mibuf;
		UserHal::MachineInfo(mibuf);
		TSize twipsize = mibuf().iPhysicalScreenSize;
		TSize pixelsize = mibuf().iDisplaySizeInPixels;
		aSize.iWidth = twipsize.iWidth*1000 / pixelsize.iWidth;
		aSize.iHeight = twipsize.iHeight*1000 / pixelsize.iHeight;
		}
	}


TInt CFbsTypefaceStore::SendGetNearestFontCommandNCreateFont(
	CFont*&				aFont,
	TInt				aFbsMessage, // a TFbsMessage
	const TFontSpec&	aFontSpec,
	TInt				aMaxHeight)
	{
	TPckgBuf<TFontSpec> pckgFontSpec(aFontSpec);
	TSize pixelSize;
	GetPixelSizeInTwips(pixelSize);
	TSizeInfo info(aMaxHeight, pixelSize);
	TPckgBuf<TSizeInfo>		pckgMaxHeight(info);
	TPckgBuf<TFontInfo> pckgFontInfo;
	TIpcArgs args(&pckgFontSpec, &pckgFontInfo, &pckgMaxHeight);
	const TInt ret = iFbs->SendCommand(aFbsMessage, args);
	if (KErrNone != ret)
		{
		return ret;
		}
	return CreateFont(aFont, pckgFontInfo());
	}

/**
Gets the font which is the nearest to the given font specification.

When the font is no longer needed, call @c ReleaseFont().

This new function replaces the deprecated @c GetNearestFontInTwips() yielding (virtually) the 
same result. However clients are strongly encouraged to use the new
@c GetNearestFontToMaxHeightInTwips() function instead. This will guarantee that every 
character within any given text string will fit within the given amount of twips, whereas the design 
height is an aesthetic unit decided by the font designer without strict physical meaning, which 
may result in cropped characters.

Chooses from the fonts loaded at system startup or through the AddFile()
or InstallFile() APIs.

@param aFont On return, contains a pointer to the nearest font.
@param aFontSpec The specification of the font to be matched.
@return KErrNone if successful; a system-wide error code otherwise.
@publishedAll
@released
@see GetNearestFontToMaxHeightInTwips()
@see AddFile()
@see InstallFile()
*/
EXPORT_C TInt CFbsTypefaceStore::GetNearestFontToDesignHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	return GetNearestFontInTwipsAndCreateFont(aFont, EFbsMessGetNearestFontToDesignHeightInTwips, aFontSpec);
	}

/**
Gets the font which is the nearest to the given font specification.

When the font is no longer needed, call @c ReleaseFont().

This new function replaces the deprecated @c GetNearestFontInPixels() yielding (virtually) the 
same result. However clients are strongly encouraged to use the new
@c GetNearestFontToMaxHeightInPixels() function instead. This will guarantee that every 
character within any given text string will fit within the given amount of pixels, whereas the design 
height is an aesthetic unit decided by the font designer without strict physical meaning, which 
may result in cropped characters.

Chooses from the fonts loaded at system startup or through the AddFile()
or InstallFile() APIs.

@param aFont On return, contains a pointer to the nearest font.
@param aFontSpec The specification of the font to be matched.
@return KErrNone if successful; a system-wide error code otherwise.
@publishedAll
@released
@see GetNearestFontToMaxHeightInPixels()
@see AddFile()
@see InstallFile()
*/
EXPORT_C TInt CFbsTypefaceStore::GetNearestFontToDesignHeightInPixels(CFont*& aFont, const TFontSpec& aFontSpec)
	{
	return SendGetNearestFontCommandNCreateFont(aFont, EFbsMessGetNearestFontToDesignHeightInPixels, aFontSpec);
	}

/**
Gets the font which is the nearest to the given font specification.

When the font is no longer needed, call @c ReleaseFont().

The font and bitmap server returns a pointer to the nearest matching font 
from those available. Matches to max height of font - this does its best 
to return a font that will fit within the maximum height specified (but 
note that variations due to hinting algorithms may rarely result in this 
height being exceeded by up to one pixel). Problems can also be 
encountered with bitmap fonts where the typeface exists but doesn't have 
a font small enough.

Chooses from the fonts loaded at system startup or through the AddFile()
or InstallFile() APIs.

@param aFont On return, contains a pointer to the nearest font.
@param aFontSpec The specification of the font to be matched.
@param aMaxHeight The maximum height within which the font must fit.
This overrides the height specified in aFontSpec.
@return KErrNone if successful; a system-wide error code otherwise.
@publishedAll
@released
@see GetNearestFontToDesignHeightInTwips()
@see AddFile()
@see InstallFile()
*/
EXPORT_C TInt CFbsTypefaceStore::GetNearestFontToMaxHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec, TInt aMaxHeight)
	{
	return GetNearestFontInTwipsAndCreateFont(aFont, EFbsMessGetNearestFontToMaxHeightInTwips, aFontSpec, aMaxHeight);
	}

/**
Gets the font which is the nearest to the given font specification.

When the font is no longer needed, call @c ReleaseFont().

The font and bitmap server returns a pointer to the nearest matching font 
from those available. Matches to max height of font - this does its best 
to return a font that will fit within the maximum height specified (but 
note that variations due to hinting algorithms may rarely result in this 
height being exceeded by up to one pixel). Problems can also be 
encountered with bitmap fonts where the typeface exists but doesn't have 
a font small enough.

Chooses from the fonts loaded at system startup or through the AddFile()
or InstallFile() APIs.

@param aFont On return, contains a pointer to the nearest font.
@param aFontSpec The specification of the font to be matched.
@param aMaxHeight The maximum height within which the font must fit.
This overrides the height specified in aFontSpec.
@return KErrNone if successful; a system-wide error code otherwise.
@publishedAll
@released
@see GetNearestFontToDesignHeightInPixels()
@see AddFile()
@see InstallFile()
*/
EXPORT_C TInt CFbsTypefaceStore::GetNearestFontToMaxHeightInPixels(CFont*& aFont, const TFontSpec& aFontSpec, TInt aMaxHeight)
	{
	return SendGetNearestFontCommandNCreateFont(aFont, EFbsMessGetNearestFontToMaxHeightInPixels, aFontSpec, aMaxHeight);
	}

/** Gets a Bitmap Font by unique identifier and algorithmic drawing style.
Chooses from the Bitmap fonts loaded at system startup or through the AddFile()
or InstallFile() APIs.

@param aFont On return, contains a pointer to the retrieved font.
@param aUid The unique identifier of the font to be retrieved.
@param aAlgStyle Algorithmic style to be applied. e.g. Sets things like algorithmic 
bolding, or slant for pseudo-italics.
@return KErrNone if successful, otherwise another of the system-wide error 
codes.
@publishedAll 
@released
@see GetNearestFontToMaxHeightInTwips()
@see GetNearestFontToDesignHeightInTwips()
@see GetNearestFontToMaxHeightInPixels()
@see GetNearestFontToDesignHeightInPixels()
@see AddFile()
@see InstallFile()
*/
EXPORT_C TInt CFbsTypefaceStore::GetFontById(CFont*& aFont,TUid aUid,const TAlgStyle& aAlgStyle)
	{
	TPckgBuf<TFontInfo> tfpckg;
	TSize pixelSize;
	GetPixelSizeInTwips(pixelSize);
	TPckgBuf<TSize> sizePkg(pixelSize);
	TPckgBuf<TAlgStyle> stylepckg(aAlgStyle);
	TIpcArgs args(&tfpckg,&stylepckg,aUid.iUid, &sizePkg);
	TInt ret=iFbs->SendCommand(EFbsMessGetFontById,args);
	if(ret!=KErrNone) return(ret);
	return(CreateFont(aFont,tfpckg()));
	}

TInt CFbsTypefaceStore::CreateFont(CFont*& aFont, const TFontInfo& aFontInfo)
	{
	if (!aFontInfo.iHandle)
		{
		Panic(EFbsFontCreateFailed);
		}
	if (IsFontLoaded(aFont, aFontInfo))
		{
		// By now, a new server-side font object has been created for the requested 
		// TFontSpec. However IsFontLoaded() is true, meaning the client already has a 
		// font at this address. This can happen if a closely matching (but not exact, 
		// otherwise it would be found in the twipscache already) TFontSpec is sent to 
		// the server. 
		// This means the new server-side font is a duplicate another server-side font. 
		// Therefore tell the server to destroy the one just created, and return the 
		// font that was already created.
		iFbs->SendCommand(EFbsMessClose, aFontInfo.iHandle);
		return KErrNone;
		}

	CFbsFont* font = new CFbsFont;
	if (!font)
		{
		iFbs->SendCommand(EFbsMessClose, aFontInfo.iHandle);
		return KErrNoMemory;
		}
	font->iHandle = aFontInfo.iHandle;
	font->iServerHandle = aFontInfo.iServerHandle;
	font->iAddressPointer = (CBitmapFont*)(iFbs->HeapBase() + aFontInfo.iAddressOffset);
	TRAPD(ret, AddFontL(font));
	if (KErrNone == ret)
		{
		aFont = font;
		}
	else
		{
		delete font;
		}
	return ret;
	}

TBool CFbsTypefaceStore::IsFontLoaded(CFont*& aFont, const TFontInfo& aFontInfo) const
/**
@see CFontStore::IsFontLoaded
@see CPdrTypefaceStore::IsFontLoaded
*/
	{
	const TInt count = iFontAccess->Count();
	for (TInt i = 0; i < count; i++)
		{
		CFont* font = (*iFontAccess)[i].iFont;
		if (((CFbsFont*)font)->iAddressPointer ==
			(CBitmapFont*)(iFbs->HeapBase() + aFontInfo.iAddressOffset))
			{
			(*iFontAccess)[i].iAccessCount++;
			aFont = font;
			return ETrue;
			}
		}
	return EFalse;
	}

/** Gets typeface information for a specified typeface index.
This information is returned in aTypefaceSupport, and includes the typeface 
name and typeface attributes, the number of font heights, the maximum and 
minimum font heights, and whether it is a scaleable typeface.

Returns benignly with an empty TTypefaceSupport if the index is too high; 
this can happen if another process removes a typeface after the first process 
has already got the number of typefaces. However, if the aTypefaceIndex<0 
the function panics with EFbsTypefaceIndexOutOfRange.

@param aTypefaceSupport On return, if the function executed successfully, 
this object contains the typeface information. 
@param aTypefaceIndex A typeface index number, in the range: zero to (NumTypefaces() - 1). 
@see CTypefaceStore::TypefaceSupport() 
@publishedAll 
@released
*/
EXPORT_C void CFbsTypefaceStore::TypefaceSupport(TTypefaceSupport& aTypefaceSupport,TInt aTypefaceIndex) const
	{
	__ASSERT_ALWAYS( aTypefaceIndex >= 0, Panic(EFbsTypefaceIndexOutOfRange) );
	TSize pixelSize;
	GetPixelSizeInTwips(pixelSize);
	TPckgBuf<TSize> sizePkg(pixelSize);
	TPckgBuf<TTypefaceSupport> tfi;
	TIpcArgs args(aTypefaceIndex,&tfi, &sizePkg);
	iFbs->SendCommand(EFbsMessTypefaceSupport,args);
	aTypefaceSupport=tfi();
	}

/** Gets the height of the font with specified height and typeface indices, in 
twips.
The value returned is rounded up or down to the nearest font height in twips.

If aTypefaceIndex<0 the function panics with EFbsTypefaceIndexOutOfRange. 
If aTypefaceIndex is greater than the number of typefaces or aHeightIndex<0 
then the function returns 0. If aHeightIndex is greater than the number of 
heights then the function returns the biggest height. 

@param aTypefaceIndex A typeface index number, in the range: 0 to (NumTypefaces() - 1). 
@param aHeightIndex A font height index number, in the range: 0 to (TTypefaceSupport::iNumHeights - 1). 
Note: TTypefaceSupport::iNumHeights is returned by TypefaceSupport(). 
@return The height of the font, in twips.
@see CTypefaceStore::FontHeightInTwips()
@publishedAll 
@released
*/
EXPORT_C TInt CFbsTypefaceStore::FontHeightInTwips(TInt aTypefaceIndex,TInt aHeightIndex) const
	{
	return FontHeight(aTypefaceIndex,aHeightIndex,EFbsMessFontHeightInTwips);
	}

/** Gets the height of the font with specified height and typeface indices, in 
pixels.
The value returned is rounded up or down to the nearest font height in pixels.

If aTypefaceIndex<0 the function panics with EFbsTypefaceIndexOutOfRange. 
If aTypefaceIndex is greater than the number of typefaces or aHeightIndex<0 
then the function returns 0. If aHeightIndex is greater than the number of 
heights then the function returns the biggest height. 

@param aTypefaceIndex A typeface index number, in the range: 0 to (NumTypefaces() - 1). 
@param aHeightIndex A font height index number, in the range: 0 to (TTypefaceSupport::iNumHeights - 1). 
Note: TTypefaceSupport::iNumHeights is returned by TypefaceSupport(). 
@return The height of the font, in pixels.
@publishedAll 
@released
*/
EXPORT_C TInt CFbsTypefaceStore::FontHeightInPixels(TInt aTypefaceIndex,TInt aHeightIndex) const
	{
	return FontHeight(aTypefaceIndex,aHeightIndex,EFbsMessFontHeightInPixels);
	}

TInt CFbsTypefaceStore::FontHeight(TInt aTypefaceIndex,TInt aHeightIndex,TInt aMessage) const
	{
	__ASSERT_ALWAYS( aTypefaceIndex >= 0, Panic(EFbsTypefaceIndexOutOfRange) );
	TSize pixelSize;
	GetPixelSizeInTwips(pixelSize);
	TPckgBuf<TSize> sizePkg(pixelSize);
	TIpcArgs args(aTypefaceIndex, aHeightIndex, &sizePkg);
	return iFbs->SendCommand(aMessage, args);
	}

/** Gets the default anti-aliasing setting for scalable fonts.
@return Indicates whether or not scalable fonts should be drawn using 
anti-aliasing.
@publishedAll 
@released
*/
EXPORT_C TGlyphBitmapType CFbsTypefaceStore::DefaultBitmapType() const
	{
	return (TGlyphBitmapType)iFbs->SendCommand(EFbsMessGetDefaultGlyphBitmapType);
	}

/** Sets the default anti-aliasing setting for scalable fonts. Unless this 
default setting is overridden so that a font is explicitly requested with 
anti-aliasing turned on or off, (see TOpenFontSpec::SetBitmapType() or 
TFontStyle::SetBitmapType()), fonts will use the default setting. The default 
setting would typically only be changed via the Control Panel. The new setting 
affects fonts requested after the change has been made.
There is currently no anti-aliasing support for bitmapped fonts.
@param aType Indicates whether or not scalable fonts should be drawn using 
anti-aliasing.
@see TOpenFontSpec::SetBitmapType()
@see TFontStyle::SetBitmapType()
@publishedAll 
@released
*/
EXPORT_C void CFbsTypefaceStore::SetDefaultBitmapType(TGlyphBitmapType aType) const
	{
	iFbs->SendCommand(EFbsMessSetDefaultGlyphBitmapType,aType);
	}

/** Sets an alias for a font name.

If a requested font cannot be found and its name occurs in the alias list
then it will be searched for again using the font name corresponding to
that alias. If an empty font name is passed then the alias will be removed
the list.
@param  TDesC& The font name alias to set.
@param TDesC&  The actual font name to use for this alias. May be empty.
@publishedAll 
@released
*/
EXPORT_C void CFbsTypefaceStore::SetFontNameAliasL(const TDesC& aFontAlias,const TDesC& aFontName) const
	{
	TIpcArgs args(&aFontAlias,aFontAlias.Length(),&aFontName,aFontName.Length());
	User::LeaveIfError(iFbs->SendCommand(EFbsMessFontNameAlias,args));
	}

/** Specifies the default language with which font metrics calculation will be based on.
The default language will be used if none is set on the font specification.
@publishedAll 
@released
@see TFontSpec::SetScriptTypeForMetrics
*/
EXPORT_C void CFbsTypefaceStore::SetDefaultLanguageForMetrics(TLanguage aLanguage) const
	{
	iFbs->SendCommand(EFbsMessDefaultLanguageForMetrics, aLanguage);
	}

/**
Unload all fonts loaded from RAM or removable media
@internalTechnology
@deprecated
*/
EXPORT_C void CFbsTypefaceStore::RemoveFontFileLocksL()
	{
	User::Leave(KErrNotSupported);
	}

/**
Unload all fonts (of specified type) loaded from named drive
@internalTechnology
*/
EXPORT_C void CFbsTypefaceStore::RemoveFontFileLocksL(const TDesC& /*aDrive*/, TBool /*aAllFonts*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
Unload the named font file
@internalTechnology
*/
EXPORT_C void CFbsTypefaceStore::RemoveFontFileLocksL(const TDesC& /*aFileName*/)
	{
	User::Leave(KErrNotSupported);
	}

/** Reset the twips cache.

The Typeface Store remembers font matches found through GetNearestFont...InTwips() family 
of functions in a cache for quicker matching.  This function empties the cache, and should be
called after the screen mode is changed as the Twips to pixels relationship may have changed.
@publishedAll
*/
EXPORT_C void CFbsTypefaceStore::ReleaseTwipsCache()
	{
	if(iTwipsCache)
		{
		CFont* font=iTwipsCache->RemoveFirstEntry();
		while(font!=NULL)
			{
			ReleaseFont(font);
			font=iTwipsCache->RemoveFirstEntry();
			}
		}
	}

/**
Sets the system default font typeface. This font will be used when finding the nearest font and the font specified is
an empty descriptor.
If the system default font is not set, then the default behaviour is to find the nearest match.

@capability WriteDeviceData
@param aFontTypefacename is the font typeface to use as the system default. A font alias cannot be used.
*/
EXPORT_C void CFbsTypefaceStore::SetSystemDefaultTypefaceNameL(const TDesC& aFontTypefaceName)
	{
	if (aFontTypefaceName.Length() <= KMaxTypefaceNameLength)
		{
		TIpcArgs args(&aFontTypefaceName);
		User::LeaveIfError(iFbs->SendCommand(EFbsSetSystemDefaultTypefaceName, args));		
		}
	else
		User::Leave(KErrTooBig);	// Typeface name is too large
	}

/**
Function to add a CLinkedTypefaceSpecification to the font and bitmap server typeface store.
@capability ECapabilityWriteDeviceData
@publishedPartner
@released
@param aLinkedTypefaceSpec  The typeface specification to be added.  Ownership is not transferred.
@param aId A unique identifier
@return A global error code
@see CLinkedTypefaceSpecification
@deprecated
*/
EXPORT_C TInt CFbsTypefaceStore::RegisterLinkedTypeface(const CLinkedTypefaceSpecification& /*aLinkedTypefaceSpec*/, TInt& /*aId*/)
	{
	return KErrNotSupported;
	}

/**
	Function to add a CLinkedTypefaceSpecification to the font and bitmap server typeface store.
	@capability ECapabilityWriteDeviceData
	@publishedPartner
	@released
	@param aLinkedTypefaceSpec.  The typeface specification to be added.  Ownership is not transferred.
	@return a global error code
	@see CLinkedTypefaceSpecification
	*/
EXPORT_C TInt CFbsTypefaceStore::RegisterLinkedTypeface(const CLinkedTypefaceSpecification& aLinkedTypefaceSpec)
	{
	__ASSERT_ALWAYS(iFbs,Panic(EFbsPanicNoConnection));
	
	// send the name
	TPckgBuf <TLinkedTypefaceSpecificationArgs> pckgbuf;
	TLinkedTypefaceSpecificationArgs &typefaceArgs=pckgbuf();
	
	typefaceArgs = aLinkedTypefaceSpec;

	TIpcArgs args(&pckgbuf,sizeof(TPckgBuf <TLinkedTypefaceSpecificationArgs>));
	
	return iFbs->SendCommand(EFbsMessRegisterLinkedTypeface,args);
	}
/**
Function to retrieve a linked typeface specification from the installed rasterizer.
If there is not a rasterizer present supporting font linking then KErrNotSupported will be returned.
The rasterizer is name specified within the passed specification and fills in the elements and
groups if the typeface exists.

	@param CLinkedTypefaceSpecificaion& The typeface Specification with the name set to be the typeface to be retrieved.
	
	@leave KErrNoMemory if there is insufficient memory available
	@leave KErrServerTerminated if the server no longer present 
	@leave KErrServerBusy if there are no message slots available  
 */
EXPORT_C void CFbsTypefaceStore::GetLinkedTypefaceL(CLinkedTypefaceSpecification& aLinkedTypefaceSpec)
	{
	TBuf<KMaxTypefaceNameLength> linkedName = aLinkedTypefaceSpec.Name();
	
	TLinkedTypefaceSpecificationArgs returnSpec;
	TPckgBuf<TLinkedTypefaceSpecificationArgs> specPkg;
	
	TIpcArgs args;
	args.Set(0,&linkedName);
	args.Set(2,&specPkg);

	User::LeaveIfError(iFbs->SendCommand(EFbsMessFetchLinkedTypeface, args));
	
	aLinkedTypefaceSpec.Clear();
	returnSpec = specPkg();
	
	TInt i;
	for (i = 0 ; i < returnSpec.iGroupSize ; i++)
		{
		CLinkedTypefaceGroup* grp = CLinkedTypefaceGroup::NewLC(returnSpec.iGroups[i].iGroupId);
		grp->SetBaselineShift(returnSpec.iGroups[i].iBaselineShift);
		grp->SetScalingOption(returnSpec.iGroups[i].iScalingOption);
		grp->SetBoldnessPercentage(returnSpec.iGroups[i].iBoldnessPercentage);
		grp->SetItalicAngle(returnSpec.iGroups[i].iItalicAngle);
		aLinkedTypefaceSpec.AddLinkedTypefaceGroupL(*grp);
		CleanupStack::Pop(grp);
		}
	for (i = 0 ; i < returnSpec.iSize ; i++)
		{
		CLinkedTypefaceElementSpec* ele = CLinkedTypefaceElementSpec::NewLC(returnSpec.iTypefaces[i].iName, returnSpec.iTypefaces[i].iGroupId);
		ele->SetCanonical(returnSpec.iTypefaces[i].iIsCanonical);
		aLinkedTypefaceSpec.AddTypefaceAtBackL(*ele);
		CleanupStack::Pop(ele);
		}
	}

/**
Function to update an existing linked typeface with a new specification. If successful a temporary file is generated and this will replace the
linked font after a reboot. Calls to FetchLinkedTypefaceSpecificationL will return the currently loaded linked font spec and not the
updated specification. 

@param aLinkedTypefaceSpec A new linked font specification to replace an existing file

@panic EFbsPanicNoConnection There is no connection to FontBitmap Server

@return TInt One of the system wide error codes
*/
EXPORT_C TInt CFbsTypefaceStore::UpdateLinkedTypeface(const CLinkedTypefaceSpecification& aLinkedTypefaceSpec)
	{
	__ASSERT_ALWAYS(iFbs,Panic(EFbsPanicNoConnection));

	TPckgBuf <TLinkedTypefaceSpecificationArgs> pckgbuf;
	TLinkedTypefaceSpecificationArgs &typefaceArgs=pckgbuf();
	
	typefaceArgs = aLinkedTypefaceSpec;

	TIpcArgs args(&pckgbuf,sizeof(TPckgBuf <TLinkedTypefaceSpecificationArgs>));
	
	return iFbs->SendCommand(EFbsMessUpdateLinkedTypeface,args);
	}

