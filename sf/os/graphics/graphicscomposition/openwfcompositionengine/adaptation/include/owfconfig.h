/* Copyright (c) 2009 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 */

/*!
 * \file owfconfig.h
 *
 */

#ifndef OWFCONFIG_H_
#define OWFCONFIG_H_

#include "owftypes.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef void* OWF_CONF_DOCUMENT;
typedef void* OWF_CONF_GROUP;
typedef void* OWF_CONF_ELEMENT;

OWF_API_CALL OWF_CONF_DOCUMENT
OWF_Conf_GetGetDocument(const char* str);

OWF_API_CALL OWF_CONF_GROUP
OWF_Conf_GetRoot(OWF_CONF_DOCUMENT doc, const char* name);

OWF_API_CALL OWF_CONF_ELEMENT
OWF_Conf_GetElement(const OWF_CONF_GROUP cur,
                    const char* elementName);

OWF_API_CALL OWFint
OWF_Conf_GetNbrElements(const OWF_CONF_GROUP cur,
                        const char* elementName);

OWF_API_CALL OWF_CONF_ELEMENT
OWF_Conf_GetNextElement(const OWF_CONF_ELEMENT cur,
                        const char* elementName);

OWF_API_CALL OWFint
OWF_Conf_GetContenti(const OWF_CONF_ELEMENT cur, OWFint deflt);

OWF_API_CALL OWFfloat
OWF_Conf_GetContentf(const OWF_CONF_ELEMENT cur, OWFfloat deflt);

OWF_API_CALL char*
OWF_Conf_GetContentStr(const OWF_CONF_ELEMENT cur,
                       char* deflt);

OWF_API_CALL OWFint
OWF_Conf_GetElementContenti(const OWF_CONF_GROUP cur,
                            const char* elementName,
                            OWFint deflt);

OWF_API_CALL OWFfloat
OWF_Conf_GetElementContentf(const OWF_CONF_GROUP cur,
                            const char* elementName,
                            OWFfloat deflt);

/* \brief Get string element content
 *
 * String must be free by  OWF_Conf_FreeContent() call;
 */
OWF_API_CALL char*
OWF_Conf_GetElementContentStr(const OWF_CONF_GROUP cur,
                              const char* elementName,
                              char* deflt);

OWF_API_CALL void
OWF_Conf_Cleanup(OWF_CONF_DOCUMENT doc);

OWF_API_CALL void
OWF_Conf_FreeContent(char* str);

#ifdef __cplusplus
}
#endif



#endif /* OWFCONFIG_H_ */
