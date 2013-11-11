// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __BITPANIC_H__
#define __BITPANIC_H__

#include <e32cmn.h>
/**
@internalTechnology
*/
enum TBitgdiPanic
	{
	EBitgdiPanicNoDevicePresent=1,
	EBitgdiPanicNoFBSConnection,
	EBitgdiPanicNoFontSelected,
	EBitgdiPanicNoSpriteSet,
	EBitgdiPanicInvalidScreenDeviceLibrary,
	EBitgdiPanicInvalidParameter,
	EBitgdiPanicInvalidColor,
	EBitgdiPanicInvalidDrawMode,
	EBitgdiPanicInvalidDisplayMode,
	EBitgdiPanicInvalidRegion,
	EBitgdiPanicInvalidWindowHandle,
	EBitgdiPanicInvalidFont,
	EBitgdiPanicInvalidBitmap,
	EBitgdiPanicOutOfBounds,
	EBitgdiPanicReadOnly,
	EBitgdiPanicPolygonFiller,
	EBitgdiPanicZeroLength,
	EBitgdiPanicNullPointer,
	EBitgdiPanicInvalidPointer,
	EBitgdiPanicCharacterTooBig,
	EBitgdiPanicInvalidHalValue,
	EBitgdiPanicInvalidArg,
	EBitgdiPanicNegativeShift,
	EBitgdiPanicInvalidInterfaceHandle,		//This panic may occur if MOutlineAndShadowBlend interface is not implemented.
	EBitgdiPanicInvalidBrushStyle,
	};

/**
@internalComponent
*/
void Panic(TBitgdiPanic aPanicCode);
void PanicWithInfo(TBitgdiPanic aPanic, const TDesC& aFileName, const TDesC& aPanicName, TInt aLine);
void PanicWithCondAndInfo(TBitgdiPanic aPanic, const TDesC& aCondition, const TDesC& aFileName, const TDesC& aPanicName, TInt aLine);
void InvariantWithCondAndInfo(const TDesC& aCondition, const TDesC& aFileName, TInt aLine);

// helper macros
#define BG_ASSERT_FILENAME(f)	_LIT(KPanicFileName, f);
#define BG_ASSERT_PANICNAME(p)	_LIT(KPanicEnum ## p, # p);
#define BG_ASSERT_CONDITION(c)	_LIT(KPanicCondition, c);

/* Use these macros for BitGdi server Asserts & Panics with diagnostic logging:
   BG_ASSERT_ALWAYS, BG_PANIC_ALWAYS, BG_ASSERT_DEBUG, BG_PANIC_DEBUG
   BG_ASSERT_ALWAYS_INVARIANT and BG_ASSERT_DEBUG_INVARIANT
   Note that the "do { ... } while (0)" construction forces the invocation to have a ';'
   and makes the macro expansion safe in nested "if ... else ..." clauses that forget to use the
   { braces } in the Coding Standard.
 */
 
#define BG_ASSERT_ALWAYS(c, p) \
		do  { \
			if (!(c)) \
				{ \
				BG_ASSERT_FILENAME(__FILE__); \
				BG_ASSERT_PANICNAME(p); \
				BG_ASSERT_CONDITION(#c); \
				PanicWithCondAndInfo(p, KPanicCondition, KPanicFileName, KPanicEnum ## p, __LINE__); \
				} \
			} while (EFalse)

#define BG_PANIC_ALWAYS(p) \
		do  { \
			BG_ASSERT_FILENAME(__FILE__); \
			BG_ASSERT_PANICNAME(p); \
			PanicWithInfo(p, KPanicFileName, KPanicEnum ## p, __LINE__); \
			} while (EFalse)
			
#define BG_ASSERT_ALWAYS_INVARIANT(c) \
		do  { \
			if (!(c)) \
				{ \
				BG_ASSERT_FILENAME(__FILE__); \
				BG_ASSERT_CONDITION(#c); \
				InvariantWithCondAndInfo(KPanicCondition, KPanicFileName, __LINE__); \
				} \
			} while (EFalse)

#if defined(_DEBUG)
#define BG_ASSERT_DEBUG(c, p) \
		do  { \
			if (!(c)) \
				{ \
				BG_ASSERT_FILENAME(__FILE__); \
				BG_ASSERT_PANICNAME(p); \
				BG_ASSERT_CONDITION(#c); \
				PanicWithCondAndInfo(p, KPanicCondition, KPanicFileName, KPanicEnum ## p, __LINE__); \
				} \
			} while (EFalse)

#define BG_PANIC_DEBUG(p) \
		do  { \
			BG_ASSERT_FILENAME(__FILE__); \
			BG_ASSERT_PANICNAME(p); \
			PanicWithInfo(p, KPanicFileName, KPanicEnum ## p, __LINE__); \
			} while (EFalse)
			
#define BG_ASSERT_DEBUG_INVARIANT(c) \
		do  { \
			if (!(c)) \
				{ \
				BG_ASSERT_FILENAME(__FILE__); \
				BG_ASSERT_CONDITION(#c); \
				InvariantWithCondAndInfo(KPanicCondition, KPanicFileName, __LINE__); \
				} \
			} while (EFalse)
			
#else
#define BG_ASSERT_DEBUG(c, p)
#define BG_PANIC_DEBUG(p)
#define BG_ASSERT_DEBUG_INVARIANT(c)
#endif

/**
Required to ensure BC between NGage and 7.0S platforms.  
Functions are exported at ordinal corresponding to where NGage platform
has extended this library and must not be moved.
@since 7.0S
@internalComponent
*/
IMPORT_C void DummyReserved1();
IMPORT_C void DummyReserved2();
IMPORT_C void DummyReserved3();
IMPORT_C void DummyReserved4();
IMPORT_C void DummyReserved5();
IMPORT_C void DummyReserved6();
IMPORT_C void DummyReserved7();
IMPORT_C void DummyReserved8();
IMPORT_C void DummyReserved9();
IMPORT_C void DummyReserved10();

#endif

