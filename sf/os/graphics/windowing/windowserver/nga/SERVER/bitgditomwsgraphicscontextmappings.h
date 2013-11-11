// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BITGDITOMWSGRAPHICSCONTEXTMAPPINGS_H_
#define BITGDITOMWSGRAPHICSCONTEXTMAPPINGS_H_

#include <gdi.h>
#include <graphics/wsgraphicscontext.h>

class BitGdiToMWsGraphicsContextMappings
	{
public:
	static MWsGraphicsContext::TFillRule Convert(CGraphicsContext::TFillRule aFillRule) { return (MWsGraphicsContext::TFillRule)aFillRule; }
	static MWsGraphicsContext::TPenStyle Convert(CGraphicsContext::TPenStyle aPenStyle) { return (MWsGraphicsContext::TPenStyle)aPenStyle; }
	static MWsGraphicsContext::TBrushStyle Convert(CGraphicsContext::TBrushStyle aBrushStyle) { return (MWsGraphicsContext::TBrushStyle)aBrushStyle; }
	static MWsGraphicsContext::TTextAlign Convert(CGraphicsContext::TTextAlign aTextAlign) { return (MWsGraphicsContext::TTextAlign)aTextAlign; }
	static MWsGraphicsContext::TFontUnderline Convert(TFontUnderline aFontUnderline) { return (MWsGraphicsContext::TFontUnderline)aFontUnderline; }
	static MWsGraphicsContext::TFontStrikethrough Convert(TFontStrikethrough aFontStrikethrough) { return (MWsGraphicsContext::TFontStrikethrough)aFontStrikethrough; }
	static const MWsGraphicsContext::TTextParameters* Convert(CGraphicsContext::TTextParameters* aParam) { return reinterpret_cast<MWsGraphicsContext::TTextParameters*>(aParam); }
	static MWsGraphicsContext::TDrawMode LossyConvert(CGraphicsContext::TDrawMode aDrawMode)
		{
		if(aDrawMode == CGraphicsContext::EDrawModePEN)
			return MWsGraphicsContext::EDrawModePEN;
		if(aDrawMode == CGraphicsContext::EDrawModeWriteAlpha)
			return MWsGraphicsContext::EDrawModeWriteAlpha;
		return MWsGraphicsContext::EDrawModePEN;
		}
	};


template<class T>
class TArrayWrapper : public TArray<T>
	{
public:
	TArrayWrapper(const T* aArray, TInt aCount);
private:
	static TInt Count(const CBase* aPtr);
	static const TAny* At(const CBase* aPtr, TInt aIndex);
private:
	const T* iArray;
	const TInt iCount;
	};

template<class T>
TArrayWrapper<T>::TArrayWrapper(const T* aArray, TInt aCount)
	: TArray<T>(TArrayWrapper::Count, TArrayWrapper::At, reinterpret_cast<const CBase*>(this)), iArray(aArray), iCount(aCount)
	{
	//reinterpret_cast above since this class doesn't derive from CBase but TArray is
	//only silly requiring CBase as opposed to TAny, so this is safe
	ASSERT(iArray);
	}

template<class T>
TInt TArrayWrapper<T>::Count(const CBase* aPtr)
	{
	//reinterpret_cast since this class doesn't derive from CBase but TArray is
	//only silly requiring CBase as opposed to TAny, so this is safe
	const TArrayWrapper* self = reinterpret_cast<const TArrayWrapper*>(aPtr);
	return self->iCount;
	}

template<class T>
const TAny* TArrayWrapper<T>::At(const CBase* aPtr, TInt aIndex)
	{
	//reinterpret_cast since this class doesn't derive from CBase but TArray is
	//only silly requiring CBase as opposed to TAny, so this is safe
	const TArrayWrapper* self = reinterpret_cast<const TArrayWrapper*>(aPtr);
	if(!Rng(0, aIndex, self->iCount - 1))
		User::Panic(_L("USER"), 130); //out of bounds (RArray/RPointerArray)
	return self->iArray + aIndex;
	}

#endif //BITGDITOMWSGRAPHICSCONTEXTMAPPINGS_H

