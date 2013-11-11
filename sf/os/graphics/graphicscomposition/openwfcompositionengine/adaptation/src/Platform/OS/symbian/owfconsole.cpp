// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and/or associated documentation files (the
// "Materials"), to deal in the Materials without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Materials, and to
// permit persons to whom the Materials are furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Materials.
//
// THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
//
// Description:
// Console adaptation for OpenWFC conformance tests
//
//

#include "owfdebug.h"

#include <e32base.h>
#include <e32cmn.h>
#include <e32debug.h>
#include <e32property.h>
#include <stdio.h>
#include <string.h>

static const TInt KOpenWfcImplCleanupKey = 0x10286FC4;  // same as DLL UID3

/**
Re-directs vprintf to RDebug so the console doesn't trash the visual tests.

Not exported. The CT should not depend on functions provided by the SI adaptation.
*/
extern "C" int xvprintf(const char* aFormat, va_list aArgs)
	{
#define SAFE_STRING_SIZE 2040
	char buffer[SAFE_STRING_SIZE+4];
    buffer[SAFE_STRING_SIZE]=0;
    buffer[SAFE_STRING_SIZE+1]=0;
    buffer[SAFE_STRING_SIZE+2]=0;
	// Symbian format syntax is different so convert to a buffer using P.I.P.S first
	vsnprintf(buffer,SAFE_STRING_SIZE, aFormat, aArgs);
	TPtrC8 ptr(reinterpret_cast<unsigned char*>(buffer), strlen(buffer));
	RDebug::RawPrint(ptr);
	return ptr.Length();
	}

/**
Re-directs printf to RDebug so the console doesn't trash the visual tests.

Not exported. The CT should not depend on functions provided by the SI adaptation.
*/
extern "C" void xprintf(const char* aFormat, ...)
    {
    va_list list; 
    va_start(list, aFormat);
	// Disabled Coverity warning, since it does not support vararg and throws a warning 
	// coverity[uninit_use_in_call]
    xvprintf(aFormat, list);
    va_end(list);
    }

static void (*gAtExitFunc)(void) = NULL;

static int DoAtExit(void* aData)
/**
 * This function is required because the signature of atexit callback is 
 * different to TCallBack func. Additionaly, this function checks the heap
 * supplied to the callback is the heap used for the singelton.
 * 
 * It is intended but not guaranteed that WServ will invoke this callback at
 * shutdown. The primary purpose is to support detection of memory leaks so
 * production code should not depend on this function being called. 
 * 
 * @param   aData   The data parameter for the callback which must be a pointer
 *                  to the heap that the singleton was created on.
 */
    {
    // Blank the property for this callback
    RThread t;
    RProperty prop;
    TCallBack cb(NULL, NULL);
    TPckgC<TCallBack> cbPckg(cb);
    prop.Set(TUid::Uid(t.SecureId().iId), KOpenWfcImplCleanupKey, cbPckg);
    prop.Close();
    t.Close();
    
    if (aData == &User::Heap() && gAtExitFunc)
        {
        gAtExitFunc();
        return 1;
        }
    return 0;
    }

extern "C" void RegisterCleanup(TInt (*aFunction)(void* aPtr))
/**
 * Register the cleanup function in a property defined by WServ. 
 * @param   aFunction   A pointer to the function that WServ should use to 
 *                      cleanup singletons on exit.
 */
    {   
    RThread t;      
    TUid category = {t.SecureId().iId};
    RProperty prop;
    TCallBack cb(aFunction, &User::Heap());
    TPckgC<TCallBack> cbPckg(cb);
    
    // If the property cannot be set the assumption is that the cleanup is not needed
    (void) prop.Set(category, KOpenWfcImplCleanupKey, cbPckg);
    prop.Close();
    t.Close();        
    }

extern "C" int xatexit(void (*func)(void))
/**
 * To minimise changes to the SI code the function is similar to the standard
 * atexit function. However, instead of providing a general purpose cleanup
 * operation this implementaion simply allows WServ to trigger the cleanup
 * of singleton objects during memory leak tests.
 * 
 * This is done purely to simplify the implementation.
 * 
 * @param func  A pointer to the function responsible for deleting singletons
 *              owned by OpenWFC.
 */
    {
    __ASSERT_ALWAYS( ( ! gAtExitFunc ) || ( gAtExitFunc == func ), User::Invariant());
    gAtExitFunc = func;
    RegisterCleanup( DoAtExit );
    return 0;
    }
