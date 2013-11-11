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

#ifndef __MWSGRAPHICSCONTEXTTODIRECTGDIMAPPINGS_H__
#define __MWSGRAPHICSCONTEXTTODIRECTGDIMAPPINGS_H__

#include <gdi.h>
#include <graphics/wsgraphicscontext.h>
#include <graphics/directgditypes.h>

class MWsGraphicsContextToDirectGdiMappings
	{
public:
	static DirectGdi::TFillRule Convert(MWsGraphicsContext::TFillRule aFillRule) { return (DirectGdi::TFillRule)aFillRule; }
	static DirectGdi::TPenStyle Convert(MWsGraphicsContext::TPenStyle aPenStyle) { return (DirectGdi::TPenStyle)aPenStyle; }
	static DirectGdi::TBrushStyle Convert(MWsGraphicsContext::TBrushStyle aBrushStyle) { return (DirectGdi::TBrushStyle)aBrushStyle; }
	static DirectGdi::TTextAlign Convert(MWsGraphicsContext::TTextAlign aTextAlign) { return (DirectGdi::TTextAlign)aTextAlign; }
	static DirectGdi::TFontUnderline Convert(MWsGraphicsContext::TFontUnderline aFontUnderline) { return (DirectGdi::TFontUnderline)aFontUnderline; }
	static DirectGdi::TFontStrikethrough Convert(MWsGraphicsContext::TFontStrikethrough aFontStrikethrough) { return (DirectGdi::TFontStrikethrough)aFontStrikethrough; }
	static const DirectGdi::TTextParameters* Convert(const MWsGraphicsContext::TTextParameters* aParam) { return reinterpret_cast<const DirectGdi::TTextParameters*>(aParam); }
	static DirectGdi::TDrawMode LossyConvert(MWsGraphicsContext::TDrawMode aDrawMode)
		{return (aDrawMode==MWsGraphicsContext::EDrawModeWriteAlpha ? DirectGdi::EDrawModeWriteAlpha : DirectGdi::EDrawModePEN);}
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


#endif	//__MWSGRAPHICSCONTEXTTODIRECTGDIMAPPINGS_H__
