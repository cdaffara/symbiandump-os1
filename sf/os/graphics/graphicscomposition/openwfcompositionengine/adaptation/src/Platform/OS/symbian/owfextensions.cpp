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
// Tables of OpenWF-C strings and extensions, plus access for EGL.
//
//

#include <string.h>
#include <e32def.h>
#include <WF/wfc.h>
#include <KHR/khrplatform.h>
#include "owfextensions.h"
#include "owftypes.h"

/*!
 * Vendor, renderer and version strings
 */
const char* wfc_strings[] =
{
    "Nokia",
    "Sample Implementation",
    "1.0",
};

/*
 * Table of extensions supported, terminated by a NULL.
 */
const char *wfc_extensions[] =
{
    NULL
};

#ifdef __cplusplus
typedef void (*OWFExtensionFunction)(...);
#else
typedef void (*OWFExtensionFunction)();
#endif

typedef struct OWFExtension_tag
{
    const char *name;
    OWFExtensionFunction function;
} OWFExtension;

/*
 * Table of extension function name/pointer pairs, terminated with a NULL name
 * pair (pointer not used for terminating entry).
 */
static OWFExtension OWF_ExtensionFunctionTable[] =
{
    { NULL, NULL }
};

OWF_API_CALL OWFExtensionFunction
owfGetProcAddress(const char *procName)
{
    WFCint ii;
    
    if (!procName)
        {
        return NULL;
        }
    
    for (ii=0; OWF_ExtensionFunctionTable[ii].name != NULL; ii++)
    {
       if (strcmp(procName, OWF_ExtensionFunctionTable[ii].name)==0)
       {
           return OWF_ExtensionFunctionTable[ii].function;
       }
    }
    
    return NULL;
}

/*
 * eglGetProcAddress uses export 1 (implemented as GetExtensionFunctionTable) to
 * fill in a function pointer table. The first entry is the function that looks up
 * a function pointer for a given name. Other entries could be added in the
 * future if coordinated with the EGL implementation.
 */

typedef struct
{
    OWFExtensionFunction (*owfGetProcAddressFn) (const char* procname);
} owfCFunctions;

extern "C" WFC_API_CALL void WFC_APIENTRY
GetExtensionFunctionTable(owfCFunctions* fnTable) WFC_APIEXIT
{
    fnTable->owfGetProcAddressFn = owfGetProcAddress;
}
