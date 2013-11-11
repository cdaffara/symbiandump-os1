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
// Implementation of the FontInjector DLL, only for the emulator. This DLL has
// writeable static data and should use the EPOCALLOWDLLDATA keyword in the MMP
// file, but it does not because by default the EKA2 emulator allows global data
// as long as the constructors do not make executive calls and, in some test
// cases, there is more than one instance of the test server with this DLL loaded,
// although it should be in active use by only one process at a time. For more
// information see the Symbian Developer Library > Symbian OS Guide > Essential
// idioms > Static data.
//

#include "fontinjector.h"

#ifdef __WINS__

template<class C> EXPORT_C XVtableInjector<C>::XVtableInjector()
	{
	ASSERT(gObjects.Count() == 0);
	ASSERT(!gShellcode);
	ASSERT(!gOriginalVtable);
	}

template<class C> EXPORT_C XVtableInjector<C>::~XVtableInjector()
	{
	for (TInt i = 0; i < gObjects.Count(); ++i)
		{
		*reinterpret_cast<TAny**>(gObjects[i]) = gOriginalVtable;
		}
	gObjects.Close();
	gShellcode = NULL;
	gOriginalVtable = NULL;
	}

template<class C> EXPORT_C TInt XVtableInjector<C>::InjectShellcode(C* aObject, MShellcode* aShellcode)
	{
	if (gShellcode && gShellcode != aShellcode)
		{
		return KErrArgument;
		}
	TInt err = KErrNone;
	if (gObjects.FindInAddressOrder(aObject) == KErrNotFound)
		{
		if (gOriginalVtable && gOriginalVtable != *reinterpret_cast<TAny**>(aObject))
			{
			return KErrArgument;
			}
		err = gObjects.InsertInAddressOrder(aObject);
		if (err == KErrNone)
			{
			gShellcode = aShellcode;
			gOriginalVtable = *reinterpret_cast<TAny**>(aObject);
			*reinterpret_cast<TAny**>(aObject) = *reinterpret_cast<TAny**>(this);
			}
		}
	return err;
	}

template<> EXPORT_C void XVtableInjector<CFont>::GetVirtualFunctionName(TInt aIndex, TDes& aName)
	{
	switch (aIndex)
		{
	case 0:
		aName.Copy(_S("CFont::~CFont()"));
		break;
	case 1:
		aName.Copy(_S("CFont::Extension_(TUint, TAny*&, TAny*)"));
		break;
	case 2:
		aName.Copy(_S("CFont::DoTypeUid()"));
		break;
	case 3:
		aName.Copy(_S("CFont::DoHeightInPixels()"));
		break;
	case 4:
		aName.Copy(_S("CFont::DoAscentInPixels()"));
		break;
	case 5:
		aName.Copy(_S("CFont::DoDescentInPixels()"));
		break;
	case 6:
		aName.Copy(_S("CFont::DoCharWidthInPixels(TChar)"));
		break;
	case 7:
		aName.Copy(_S("CFont::DoTextWidthInPixels(const TDesC&)"));
		break;
	case 8:
		aName.Copy(_S("CFont::DoBaselineOffsetInPixels()"));
		break;
	case 9:
		aName.Copy(_S("CFont::DoTextCount(const TDesC&, TInt)"));
		break;
	case 10:
		aName.Copy(_S("CFont::DoTextCount(const TDesC&, TInt, TInt&)"));
		break;
	case 11:
		aName.Copy(_S("CFont::DoMaxCharWidthInPixels()"));
		break;
	case 12:
		aName.Copy(_S("CFont::DoMaxNormalCharWidthInPixels()"));
		break;
	case 13:
		aName.Copy(_S("CFont::DoFontSpecInTwips()"));
		break;
	case 14:
		aName.Copy(_S("CFont::DoGetCharacterData(TUint, TOpenFontCharMetrics&, const TUint8*&, TSize&)"));
		break;
	case 15:
		aName.Copy(_S("CFont::DoGetCharacterPosition(CFont::TPositionParam&)"));
		break;
	case 16:
		aName.Copy(_S("CFont::DoExtendedFunction(TUid, TAny*)"));
		break;
	default:
		aName.Copy(_S("CFont::<Unknown Virtual Function>"));
		}
	}

template<> EXPORT_C void XVtableInjector<COpenFont>::GetVirtualFunctionName(TInt aIndex, TDes& aName)
	{
	switch (aIndex)
		{
	case 0:
		aName.Copy(_S("COpenFont::~COpenFont()"));
		break;
	case 1:
		aName.Copy(_S("COpenFont::Extension_(TUint, TAny*&, TAny*)"));
		break;
	case 2:
		aName.Copy(_S("COpenFont::RasterizeL(TInt, TOpenFontGlyphData*)"));
		break;
	case 3:
		aName.Copy(_S("COpenFont::ExtendedInterface(TUid, TAny*&)"));
		break;
	default:
		aName.Format(_L("COpenFont::<Extended Virtual Function %03d>"), aIndex - 3);
		}
	}

template<class C> void XVtableInjector<C>::ObjectDestroyed(TAny* aObject)
	{
	gObjects.Remove(gObjects.FindInAddressOrder(aObject));
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function000()
	{
	asm { push ecx }
	/* cdecl calling convention */
	asm { push ecx }
	asm { call ObjectDestroyed }
	asm { add esp, 4 }
	/* thiscall calling convention */
	asm { mov ecx, gShellcode }
	asm { push 0 }
	asm { mov eax, [ecx] }
	asm { call [eax] }
	/* continue to original function */
	asm { pop ecx }
	asm { mov eax, gOriginalVtable }
	asm { jmp [eax] }
	}

#define DISPATCH_NTH_VIRTUAL_FUNCTION(n) \
	asm { push ecx } \
	/* thiscall calling convention */ \
	asm { mov ecx, gShellcode } \
	asm { push n } \
	asm { mov eax, [ecx] } \
	asm { call [eax] } \
	/* continue to original function */ \
	asm { pop ecx } \
	asm { mov eax, gOriginalVtable } \
	asm { jmp [eax + (n * 4)] }

template<class C> void __declspec(naked) XVtableInjector<C>::Function001()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(1)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function002()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(2)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function003()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(3)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function004()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(4)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function005()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(5)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function006()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(6)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function007()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(7)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function008()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(8)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function009()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(9)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function010()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(10)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function011()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(11)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function012()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(12)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function013()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(13)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function014()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(14)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function015()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(15)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function016()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(16)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function017()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(17)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function018()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(18)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function019()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(19)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function020()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(20)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function021()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(21)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function022()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(22)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function023()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(23)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function024()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(24)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function025()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(25)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function026()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(26)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function027()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(27)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function028()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(28)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function029()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(29)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function030()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(30)
	}

template<class C> void __declspec(naked) XVtableInjector<C>::Function031()
	{
	DISPATCH_NTH_VIRTUAL_FUNCTION(31)
	}

template<class C> RPointerArray<TAny> XVtableInjector<C>::gObjects;
template<class C> MShellcode* XVtableInjector<C>::gShellcode = NULL;
template<class C> TAny* XVtableInjector<C>::gOriginalVtable = NULL;

// Explicit instantiation of the injector classes.
template class XVtableInjector<CFont>;
template class XVtableInjector<COpenFont>;

#endif // __WINS__
