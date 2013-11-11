// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <gdi.h>
#include "GDIPANIC.h"

//
// CPalette
//

_LIT(KGdiCPalettePanicCategory,"CPalette");

EXPORT_C CPalette::CPalette():
	CBase(),
	iArray(NULL),
	iNumEntries(0)
	{}


EXPORT_C CPalette::~CPalette()
/** Destructor. */
	{
	delete [] iArray;
	}


EXPORT_C void CPalette::Clear()
/** Clears all the entries in the palette to TRgb(0). */
	{
	TRgb blank(0);
	for(TInt count=0;count<iNumEntries;count++)
		iArray[count]=blank;
	}


EXPORT_C CPalette* CPalette::NewL(TInt aNumberOfEntries)
/** Creates a new palette with the specified number of entries. 

@param aNumberOfEntries The number of entries in the palette being created.
@return The newly created palette. */
	{
	CPalette* thisptr=new(ELeave) CPalette;
	CleanupStack::PushL(thisptr);
	thisptr->ConstructL(aNumberOfEntries);
	CleanupStack::Pop();
	return(thisptr);
	}


EXPORT_C CPalette* CPalette::NewDefaultL(TDisplayMode aDispMode)
/** Creates a new default palette for the specified display mode. The default palette 
for a particular display mode has one entry for each possible colour in that 
display mode (2 entries for EGray2, 16 entries for EColor16 etc.); the colour 
of each index p in the default palette is set to its default value according 
to its display mode (e.g. if the mode is EColor16 then palette[p]==TRgb::Color16(p); 
if the mode is EGray4 then palette[p]==TRgb::_Gray4(p)). 

@param aDispMode The display mode for which the palette is to be created. 
@return The newly created palette */
	{
	TInt numentries=0;
	switch(aDispMode)
		{
	case EGray2:
		numentries=2;
		break;
	case EGray4:
		numentries=4;
		break;
	case EGray16:
	case EColor16:
		numentries=16;
		break;
	case EGray256:
	case EColor256:
		numentries=256;
		break;
	default:
		User::Leave(KErrNotSupported);
		};
	CPalette* thisptr=new(ELeave) CPalette;
	CleanupStack::PushL(thisptr);
	thisptr->ConstructL(numentries);
	TInt count=0;
	switch(aDispMode)
		{
	case EGray2:
		thisptr->iArray[0]=TRgb::_Gray2(0);
		thisptr->iArray[1]=TRgb::_Gray2(1);
		break;
	case EGray4:
		for(;count<numentries;count++)
			thisptr->iArray[count]=TRgb::_Gray4(count);
		break;
	case EGray16:
		for(;count<numentries;count++)
			thisptr->iArray[count]=TRgb::_Gray16(count);
		break;
	case EColor16:
		for(;count<numentries;count++)
			thisptr->iArray[count]=TRgb::Color16(count);
		break;
	case EGray256:
		for(;count<numentries;count++)
			thisptr->iArray[count]=TRgb::_Gray256(count);
		break;
	case EColor256:
		for(;count<numentries;count++)
			thisptr->iArray[count]=TRgb::Color256(count);
		break;
	default:
		User::Leave(KErrNotSupported);
		}
	CleanupStack::Pop();
	return(thisptr);
	}

void CPalette::ConstructL(TInt aNumberOfEntries)
	{
	if(aNumberOfEntries<=0) User::Leave(KErrArgument);
	iArray=new(ELeave) TRgb[aNumberOfEntries];
	iNumEntries=aNumberOfEntries;
	}


EXPORT_C TRgb CPalette::GetEntry(TInt aIndex) const
/** Gets the RGB value of the palette entry at aPaletteIndex.
	
@param aPaletteIndex The index of the palette entry to get. 
@return The RGB value of the palette entry */
	{
	GDI_ASSERT_ALWAYS_GENERAL(aIndex<iNumEntries,User::Panic(KGdiCPalettePanicCategory,KErrTooBig));
	if(aIndex<iNumEntries)
		return(iArray[aIndex]);
	TRgb defaultcol(0,0,0);
	return(defaultcol);
	}


EXPORT_C TRgb CPalette::NearestEntry(const TRgb& aColor) const
/** Gets the colour in the palette which is closest to the specified colour.

@param aColor The colour to find.
@return The colour in the palette which is closest to the specified colour. */
	{
	return(iArray[NearestIndex(aColor)]);
	}


EXPORT_C TInt CPalette::NearestIndex(const TRgb& aColor) const
/** Gets the index of the colour in the palette which is closest to the specified 
colour.

@param aColor The colour to find.
@return The index of the colour in the palette which is closest to the specified 
colour. */
	{
	TRgb* entry = iArray;
	TRgb* entryLimit = entry+iNumEntries;
	TRgb* lowest = entry;
	TInt mindif = KMaxTInt;

	while(entry<entryLimit)
		{
		TInt value = entry->Internal();

		TInt difference = Abs((TInt)(aColor.Internal()&0xFF)-(TInt)(value&0xFF))+
						  (Abs((TInt)(aColor.Internal()&0xFF00)-(TInt)(value&0xFF00))>>8)+
						  (Abs((TInt)(aColor.Internal()&0xFF0000)-(TInt)(value&0xFF0000))>>16);

		if(difference<mindif)
			{
			lowest=entry;
			mindif=difference;
			if(difference==0)
				break;
			}
		entry++;
		}
	return(lowest-iArray);
	}


EXPORT_C void CPalette::SetEntry(TInt aIndex,const TRgb& aColor)
/** Sets the palette entry at aPaletteIndex to the RGB value aPaletteEntry.
	
@param aPaletteIndex The index of the palette entry to be set.
@param aPaletteEntry The RGB value to set that entry to. */
	{
	GDI_ASSERT_ALWAYS_GENERAL(aIndex<iNumEntries,User::Panic(KGdiCPalettePanicCategory,KErrTooBig));
	iArray[aIndex]=aColor;
	}


EXPORT_C void CPalette::GetDataPtr(TInt aFirstColor,TInt aNumColors,TPtr8& aPtr)
/** Returns a descriptor over the palette entries for the specifed colors. It is 
designed so that the descriptor can be passed to another thread and that thread 
copy the relevant entries.

@param aFirstColor The first colour.
@param aNumColors Number of colours.
@param aPtr Descriptor. */
	{
	TInt size=sizeof(TRgb)*aNumColors;
	aPtr.Set(REINTERPRET_CAST(TUint8*,&iArray[aFirstColor]),size,size);
	}
