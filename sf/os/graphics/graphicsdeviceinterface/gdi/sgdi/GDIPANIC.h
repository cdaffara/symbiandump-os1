// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @internalComponent
*/

#ifndef __BITPANIC_H__
#define __BITPANIC_H__

#ifdef __GDI_H__
void Panic(TGdiPanic aPanicCode);
void PanicWithCondAndInfo(TGdiPanic aPanic, const TDesC& aCondition, const TDesC& aFileName, const TDesC& aPanicName, TInt aLine);
#endif 

void PanicLogWithInfo(const TDesC& aCommand, const TDesC& aCondition, const TDesC& aFileName, TInt aLine);

// helper macros
#define GDI_ASSERT_FILENAME(f)	_LIT(KPanicFileName, f);
#define GDI_ASSERT_PANICNAME(p)	_LIT(KPanicEnum ## p, # p);
#define GDI_ASSERT_PANICCOMMAND(a)	_LIT(KPanicCommand, a);
#define GDI_ASSERT_CONDITION(c)	_LIT(KPanicCondition, c);

/* Use these macros for GDI Asserts & Panics with diagnostic logging:
   GDI_ASSERT_ALWAYS, GDI_ASSERT_ALWAYS_GENERAL, GDI_ASSERT_DEBUG
   and GDI_ASSERT_DEBUG_GENERAL
   Note that the "do { ... } while (0)" construction forces the invocation to have a ';'
   and makes the macro expansion safe in nested "if ... else ..." clauses that forget to use the
   { braces } in the Coding Standard.
 */
 
#define GDI_ASSERT_ALWAYS(c, p) \
		do  { \
			if (!(c)) \
				{ \
				GDI_ASSERT_FILENAME(__FILE__); \
				GDI_ASSERT_PANICNAME(p); \
				GDI_ASSERT_CONDITION(#c); \
				PanicWithCondAndInfo(p, KPanicCondition, KPanicFileName, KPanicEnum ## p, __LINE__); \
				} \
			} while (0)
	
#define GDI_ASSERT_ALWAYS_GENERAL(c, a) \
		do  { \
			if (!(c)) \
				{ \
				GDI_ASSERT_FILENAME(__FILE__); \
				GDI_ASSERT_PANICCOMMAND(#a); \
				GDI_ASSERT_CONDITION(#c); \
				PanicLogWithInfo(KPanicCondition, KPanicCommand, KPanicFileName, __LINE__); \
				a ; \
				} \
			} while (0)

#if defined(_DEBUG)
#define GDI_ASSERT_DEBUG(c, p) \
		do  { \
			if (!(c)) \
				{ \
				GDI_ASSERT_FILENAME(__FILE__); \
				GDI_ASSERT_PANICNAME(p); \
				GDI_ASSERT_CONDITION(#c); \
				PanicWithCondAndInfo(p, KPanicCondition, KPanicFileName, KPanicEnum ## p, __LINE__); \
				} \
			} while (0)

#define GDI_ASSERT_DEBUG_GENERAL(c, a) \
		do  { \
			if (!(c)) \
				{ \
				GDI_ASSERT_FILENAME(__FILE__); \
				GDI_ASSERT_PANICCOMMAND(#a); \
				GDI_ASSERT_CONDITION(#c); \
				PanicLogWithInfo(KPanicCondition, KPanicCommand, KPanicFileName, __LINE__); \
				a ; \
				} \
			} while (0)
			
#else
#define GDI_ASSERT_DEBUG(c, p)
#define GDI_ASSERT_DEBUG_GENERAL(c, p)

#endif

#endif
