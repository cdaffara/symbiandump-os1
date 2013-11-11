/* Copyright (c) 2009-2010 The Khronos Group Inc.
 * Portions copyright (c) 2009-2010  Nokia Corporation and/or its subsidiary(-ies)
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



#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "owfattributes.h"
#include "owfmemory.h"
#include "owfdebug.h"

#define OWF_ATTRIB_RANGE_START            (0)
#define OWF_ATTRIB_RANGE_UNINITIALIZED    (-1)

static OWFint OWF_Attribute_Commit(OWF_ATTRIBUTE* aAttr, OWFint aDirtyFlag, OWFint aCopyTo,OWFint aCopyFrom);
 
  /*
  This attribute class is not used for WFC element attributes because elements are 
  completely cloned in the committed scene.
  [This class could be replaced with 3 copies of a much simpler writable attributes raw 
  structure with simple data members, and the whole structure copied each commit.] 
  Normal attribute values have three pointers indexed via an array:
     COMMITTED_ATTR_VALUE_INDEX:
         Attribute values used by the scene 
             - points to named variables directly used by the compositor
     WORKING_ATTR_VALUE_INDEX:
         Attribute values that may be set by the client, if they are not read-only.
     SNAPSHOT_ATTR_VALUE_INDEX
         A copy of the client-set attribute values following a client call to wfcCommit
         The copy is then protected against further modification by the client until 
         the committed scene is updated and displayed.
  The Working and Snapshot writable attributes require additional cache storage, 
  which is managed by the lifetime of the attribute list.
  Read-only attributes point all three pointers at the named compositor variables.
  Currently, there are relatively few writable attributes so it is reasonable 
  to individually dynamically allocate each cache. It would be better to allocate 
  a single block sized after the attributes have been registered.  
  
  Internal code is expected to read or write to member variables that are abstracted 
  by read-only attributes. However they must not write directly to member variables 
  masked by writable attributes after the initial "commit" to working. The code does 
  not currently use const instances to enforce this behavior.
 */
#define COND_FAIL_NR(ctx, condition, error) \
    if (!(condition)) { \
        if (ctx) { \
            (ctx)->last_error = error; \
        } \
        return; \
    }

#define COND_FAIL(ctx, condition, error, r) \
    if (!(condition)) { \
        if (ctx) { \
            (ctx)->last_error = error; \
        } \
        return r; \
    }
	
// NS here means No Set as we are not setting the last_error member of the context.
// These are used when we are testing the context itself so setting the last_error
// member is itself is an error
#define COND_FAIL_NR_NS(condition) \
    if (!(condition)) { \
        return; \
    }

#define COND_FAIL_NS(condition, r) \
    if (!(condition)) { \
        return r; \
    }

#define CHECK_INDEX_NR(ctx, index, error) \
    if (index < (ctx)->range_start || index > (ctx)->range_end) { \
        (ctx)->last_error = error; \
        return; \
    }

#define CHECK_INDEX(ctx, index, error, r) \
    if (index < (ctx)->range_start || index > (ctx)->range_end) { \
        (ctx)->last_error = error; \
        return r; \
    }

#define CHECK_BAD_NR(ctx, index) \
    CHECK_INDEX_NR(ctx, index, ATTR_ERROR_INVALID_ATTRIBUTE); \
    if ((ctx)->attributes[index-(ctx)->range_start].attr_info.type == AT_UNDEFINED) { \
        (ctx)->last_error = ATTR_ERROR_INVALID_ATTRIBUTE; \
        return; \
    }

#define CHECK_BAD(ctx, index, r) \
    CHECK_INDEX(ctx, index, ATTR_ERROR_INVALID_ATTRIBUTE, r); \
    if ((ctx)->attributes[index-(ctx)->range_start].attr_info.type == AT_UNDEFINED) { \
        (ctx)->last_error = ATTR_ERROR_INVALID_ATTRIBUTE; \
        return r; \
    }

#define SET_ERROR(ctx, err) \
    if ((ctx)->last_error == ATTR_ERROR_NONE) { \
        (ctx)->last_error = err; \
    }


/*
=============================================================================
ATTRIBUTE CONTEXT MANAGEMENT FUNCTIONS
=============================================================================
*/

/*!
 * \brief Initializes attribute context
 *
 * \param aContext Attribute context to initialize
 * \param aStart Attribute range start
 * \param aEnd Attribute range end. Must be greater than range start.
 *
 * \return ATTR_ERROR_INVALID_ARGUMENT
 * ATTR_ERROR_NO_MEMORY
 */
OWF_API_CALL void
OWF_AttributeList_Create(OWF_ATTRIBUTE_LIST* aContext,
                         OWFint aStart,
                         OWFint aEnd)
{
    OWF_ATTRIBUTE*          temp = NULL;

    COND_FAIL_NR_NS(aContext);
    COND_FAIL_NR(aContext, aEnd >= 0, ATTR_ERROR_INVALID_ARGUMENT);
    COND_FAIL_NR(aContext, aEnd >= aStart, ATTR_ERROR_INVALID_ARGUMENT);

    aContext->range_start = OWF_ATTRIB_RANGE_START;
    aContext->range_end = OWF_ATTRIB_RANGE_UNINITIALIZED;

    temp = (OWF_ATTRIBUTE*) xalloc(aEnd - aStart + 1, sizeof(OWF_ATTRIBUTE));
    COND_FAIL_NR(aContext, temp, ATTR_ERROR_NO_MEMORY);

    memset(aContext, 0, sizeof(OWF_ATTRIBUTE_LIST));

    aContext->range_start = aStart;
    aContext->range_end = aEnd;
    aContext->attributes = temp;

    SET_ERROR(aContext, ATTR_ERROR_NONE);
    return;
}

/*!
 * \brief Destroy attribute context and free any resources (memory
 * blocks) allocated to it. All attributes are destroyed.
 *
 * \param aContext Attribute context to destroy
 *
 * \return ATTR_ERROR_INVALID_ARGUMENT
 * ATTR_ERROR_INVALID_CONTEXT
 */
OWF_API_CALL void
OWF_AttributeList_Destroy(OWF_ATTRIBUTE_LIST* aContext)
{
    OWFint                 count = 0;
    OWFint                 at = 0;
    OWFint                 cache = 0;

    COND_FAIL_NR_NS(aContext);
    COND_FAIL_NR(aContext, aContext->attributes, ATTR_ERROR_INVALID_CONTEXT);

    count = aContext->range_end - aContext->range_start;
    for (at = 0; at <= count; at++) {

        OWF_ATTRIBUTE* attr = &aContext->attributes[at];
        if (!attr->attr_info.readonly)
            {
            for (cache=0;cache<NUM_ATTR_VALUE_COPIES;cache++)
                {
                if (cache!=COMMITTED_ATTR_VALUE_INDEX)
                    {
                    xfree(attr->attr_value[cache].gen_ptr);
                    }
                }
            }
    }

    xfree(aContext->attributes);
    memset(aContext, 0, sizeof(OWF_ATTRIBUTE_LIST));
    SET_ERROR(aContext, ATTR_ERROR_NONE);
    return;
}

/*


 */
OWF_API_CALL OWF_ATTRIBUTE_LIST_STATUS
OWF_AttributeList_GetError(OWF_ATTRIBUTE_LIST* aContext)
{
    OWF_ATTRIBUTE_LIST_STATUS   error;

    if (!aContext) {
        return ATTR_ERROR_INVALID_ARGUMENT;
    }
    error = aContext->last_error;
    aContext->last_error = ATTR_ERROR_NONE;
    return error;
}

/*
=============================================================================
INITIALIZATION FUNCTIONS
=============================================================================
*/

static void OWF_Attribute_Init(OWF_ATTRIBUTE_LIST* aContext,
                               OWFint aName,
                               OWF_ATTRIBUTE_TYPE aType,
                               OWFint aLength,
                               void* aValue,
                               OWFboolean aRdOnly)
{
    OWF_ATTRIBUTE*          attr = NULL;
    void*                   cache = NULL;
    OWFint                  itemSize;
    OWFint                  arraySize;
    OWFint                  copy;
    OWFint                  index;
	
	COND_FAIL_NR_NS(aContext);
	CHECK_INDEX_NR(aContext, aName, ATTR_ERROR_INVALID_ATTRIBUTE);
    COND_FAIL_NR(aContext, aContext->attributes, ATTR_ERROR_INVALID_CONTEXT);
    COND_FAIL_NR(aContext, aLength < MAX_ATTR_LENGTH, ATTR_ERROR_CANT_HANDLE);
    COND_FAIL_NR(aContext, aType != AT_UNDEFINED, ATTR_ERROR_INVALID_ARGUMENT);
	
	index = aName - aContext->range_start;

    attr = &aContext->attributes[index];
    
    memset(attr, 0, sizeof(OWF_ATTRIBUTE));

    /* when allocin', size DOES matter */
    if (aType == AT_INTEGER || aType == AT_BOOLEAN) {
        itemSize = sizeof(OWFint);
    } else {
        itemSize = sizeof(OWFfloat);
    }
    arraySize=itemSize*aLength;

    /* don't allocate cache for read-only 'butes */
    attr->attr_info.type        = aType;
    attr->attr_info.length      = aLength;
    attr->attr_info.readonly    = aRdOnly;
    attr->attr_info.size        = itemSize;
    if (aRdOnly)
        {
        for (copy=0;copy<NUM_ATTR_VALUE_COPIES;copy++)
            {
            attr->attr_value[copy].gen_ptr = aValue;
            }
        }
    else
        {
        for (copy=0;copy<NUM_ATTR_VALUE_COPIES;copy++)
            {
            if (copy==COMMITTED_ATTR_VALUE_INDEX)
                {
                 attr->attr_value[COMMITTED_ATTR_VALUE_INDEX].gen_ptr = aValue;
                }
            else
                {
                cache = xalloc(arraySize,1);
                COND_FAIL_NR(aContext, NULL != cache, ATTR_ERROR_NO_MEMORY);
                attr->attr_value[copy].gen_ptr = cache;
                }
             }
       }

    SET_ERROR(aContext, ATTR_ERROR_NONE);

}

/*
 * \brief Intialize integer attribute
 *
 * \param aContext Attibute context
 * \param aName Attribute name
 * \param aValue Attribute initial value
 * \param aRdOnly Read-only flag
 *
 * \return ATTR_ERROR_INVALID_ARGUMENT
 * ATTR_ERROR_INVALID_ATTRIBUTE
 * ATTR_ERROR_INVALID_CONTEXT
 */
OWF_API_CALL void
OWF_Attribute_Initi(OWF_ATTRIBUTE_LIST* aContext,
                    OWFint aName,
                    OWF_INT_REF aValue,
                    OWFboolean aRdOnly)
{
    OWF_Attribute_Init(aContext, aName, AT_INTEGER, 1, aValue, aRdOnly);
}

/*
 * \brief Initialize float attribute
 *
 * \param aContext Attribute context
 * \param aName Attribute name
 * \param aValue Initial value for attribute
 * \param aRdOnly Read-only flag
 *
 * \return ATTR_ERROR_INVALID_ARGUMENT
 * ATTR_ERROR_INVALID_ATTRIBUTE
 * ATTR_ERROR_INVALID_CONTEXT
 */
OWF_API_CALL void
OWF_Attribute_Initf(OWF_ATTRIBUTE_LIST* aContext,
                    OWFint aName,
                    OWF_FLOAT_REF aValue,
                    OWFboolean aRdOnly)
{
    OWF_Attribute_Init(aContext, aName, AT_FLOAT, 1, aValue, aRdOnly);
}

/*
 * \brief Initialize boolean attribute
 *
 * \param aContext Attribute context
 * \param aName Attribute name
 * \param aValue Initial value for attribute
 * \param aRdOnly Read-only flag
 *
 * \return ATTR_ERROR_INVALID_ARGUMENT
 * ATTR_ERROR_INVALID_ATTRIBUTE
 * ATTR_ERROR_INVALID_CONTEXT
 */
OWF_API_CALL void
OWF_Attribute_Initb(OWF_ATTRIBUTE_LIST* aContext,
                    OWFint aName,
                    OWF_BOOL_REF aValue,
                    OWFboolean aRdOnly)
{
    OWF_Attribute_Init(aContext, aName, AT_BOOLEAN, 1, aValue, aRdOnly);
}

/*
 * \brief Initialize vector attribute
 *
 * \param aContext Attribute context
 * \param aName Attribute name
 * \param aLength Attribute (vector) length
 * \param aValues Initial value(s) for attribute
 * \param aRdOnly Read-only flag
 *
 * \return ATTR_ERROR_INVALID_ARGUMENT
 * ATTR_ERROR_INVALID_ATTRIBUTE
 * ATTR_ERROR_INVALID_CONTEXT
 * ATTR_ERROR_CANT_HANDLE
 */
OWF_API_CALL void
OWF_Attribute_Initiv(OWF_ATTRIBUTE_LIST* aContext,
                     OWFint aName,
                     OWFint aLength,
                     OWF_INT_VECTOR_REF aValues,
                     OWFboolean aRdOnly)
{
    OWF_Attribute_Init(aContext, aName, AT_INTEGER, aLength, aValues, aRdOnly);
}

/*
 * \brief Initialize vector attribute
 *
 * \param aContext Attribute context
 * \param aName Attribute name
 * \param aLength Attribute (vector) length
 * \param aValues Initial value(s) for attributes
 * \param aRdOnly Read-only flag
 *
 * \return
 */
OWF_API_CALL void
OWF_Attribute_Initfv(OWF_ATTRIBUTE_LIST* aContext,
                     OWFint aName,
                     OWFint aLength,
                     OWF_FLOAT_VECTOR_REF aValues,
                     OWFboolean aRdOnly)
{
    OWF_Attribute_Init(aContext, aName, AT_FLOAT, aLength, aValues, aRdOnly);
}

/*
=============================================================================
GETTER FUNCTIONS
=============================================================================
*/

/*
 * \brief Get attribute integer value.
 *
 * \param aContext Attribute context
 * \param aName Attribute name
 *
 * \return Attribute integer value (floats are floor()ed). For vector
 * attributes the return value will be error ATTR_ERROR_INVALID_TYPE.
 * ATTR_ERROR_INVALID_ATTRIBUTE
 * ATTR_ERROR_INVALID_CONTEXT
 */
OWF_API_CALL OWFint
OWF_Attribute_GetValuei(OWF_ATTRIBUTE_LIST* aContext,
                        OWFint aName)
{
    OWFint                  index = 0;
    OWF_ATTRIBUTE*          attr = 0;
    OWFint                  result = 0;

    COND_FAIL_NS(aContext, 0); 
	COND_FAIL(aContext,
              aContext->attributes,
              ATTR_ERROR_INVALID_CONTEXT,
              0);
    CHECK_BAD(aContext, aName, 0);


    index = aName - aContext->range_start;
    attr = &aContext->attributes[index];

    COND_FAIL(aContext,
              1 == attr->attr_info.length,
              ATTR_ERROR_INVALID_TYPE,
              0);

    SET_ERROR(aContext, ATTR_ERROR_NONE);

    switch (attr->attr_info.type) {
        case AT_FLOAT: {
            result = floor(attr->attr_value[WORKING_ATTR_VALUE_INDEX].float_value[0]);
            break;
        }

        case AT_INTEGER:
        case AT_BOOLEAN: {
            result = attr->attr_value[WORKING_ATTR_VALUE_INDEX].int_value[0];
            break;
        }

        default: {
            SET_ERROR(aContext, ATTR_ERROR_INVALID_TYPE);
            break;
        }
    }
    return result;
}

/*
 * \brief Return boolean attribute value
 *
 * \param aContext Attribute context
 * \param aName Attribute name
 *
 * \return Attribute value
 * ATTR_ERROR_INVALID_ATTRIBUTE
 * ATTR_ERROR_INVALID_TYPE
 */
OWF_API_CALL OWFboolean
OWF_Attribute_GetValueb(OWF_ATTRIBUTE_LIST* aContext,
                        OWFint aName)
{
    /* boolean is stored as int, must cast */
    return (OWFboolean) OWF_Attribute_GetValuei(aContext, aName);
}

/*
 * \brief Get attribute float value
 *
 * \param aContext
 * \param aName
 * \param aValue
 *
 * \return Attribute
 */
OWF_API_CALL OWFfloat
OWF_Attribute_GetValuef(OWF_ATTRIBUTE_LIST* aContext,
                        OWFint aName)
{
    OWFint                 index = 0;
    OWF_ATTRIBUTE*          attr = NULL;
    OWFfloat                result = 0.f;

    COND_FAIL_NS(aContext, 0);
    COND_FAIL(aContext,
              aContext->attributes,
              ATTR_ERROR_INVALID_CONTEXT,
              0);
    CHECK_BAD(aContext, aName, 0);


    index = aName - aContext->range_start;
    attr = &aContext->attributes[index];

    COND_FAIL(aContext,
              1 == attr->attr_info.length,
              ATTR_ERROR_INVALID_TYPE,
              0);

    SET_ERROR(aContext, ATTR_ERROR_NONE);
    result = 0.f;

    switch (attr->attr_info.type) {
        case AT_FLOAT: {
            result = attr->attr_value[WORKING_ATTR_VALUE_INDEX].float_value[0];
            break;
        }

        case AT_INTEGER:
        case AT_BOOLEAN: {
            result = (OWFfloat) attr->attr_value[WORKING_ATTR_VALUE_INDEX].int_value[0];
            break;
        }

        default: {
            SET_ERROR(aContext, ATTR_ERROR_INVALID_TYPE);
            break;
        }
    }
    return result;
}

/*
 * \brief
 *
 * \param aContext
 * \param aName
 * \param aSize
 * \param aValue
 *
 * \return
 */
OWF_API_CALL OWFint
OWF_Attribute_GetValueiv(OWF_ATTRIBUTE_LIST* aContext,
                         OWFint aName,
                         OWFint aLength,
                         OWFint* aValue)
{
    OWFint                 index = 0;
    OWF_ATTRIBUTE*          attr = NULL;
    OWFint                 count = 0;

    COND_FAIL_NS(aContext, 0);
    COND_FAIL(aContext,
              aContext->attributes,
              ATTR_ERROR_INVALID_CONTEXT,
              0);
    CHECK_BAD(aContext, aName, 0);


    index = aName - aContext->range_start;
    attr = &aContext->attributes[index];

    COND_FAIL(aContext,
              attr->attr_info.length >= 1,
              ATTR_ERROR_INVALID_TYPE,
              0);

    if (!aValue) {
        /* fetch size only */
        return attr->attr_info.length;
    }

    count = min(aLength, attr->attr_info.length);

    SET_ERROR(aContext, ATTR_ERROR_NONE);

    switch (attr->attr_info.type) {
        case AT_FLOAT: {
            OWFint i;
            OWFfloat* v = attr->attr_value[WORKING_ATTR_VALUE_INDEX].float_value;
            for (i = 0; i < count; i++) {
                aValue[i] = floor(v[i]);
            }
            break;
        }

        case AT_BOOLEAN:
        case AT_INTEGER: {
            memcpy(aValue,
                   attr->attr_value[WORKING_ATTR_VALUE_INDEX].int_value,
                   count * attr->attr_info.size);
            break;
        }

        default: {
            SET_ERROR(aContext, ATTR_ERROR_INVALID_TYPE);
            break;
        }
    }
    return count;
}

/*
 * \brief
 *
 * \param aContext
 * \param aName
 * \param aSize
 * \param aValue
 *
 * \return
 */
OWF_API_CALL OWFint
OWF_Attribute_GetValuefv(OWF_ATTRIBUTE_LIST* aContext,
                         OWFint aName,
                         OWFint aLength,
                         OWFfloat* aValue)
{
    OWFint                 index = 0;
    OWF_ATTRIBUTE*          attr = NULL;
    OWFint                 count = 0;

    COND_FAIL_NS(aContext, 0);
    COND_FAIL(aContext,
              aContext->attributes,
              ATTR_ERROR_INVALID_CONTEXT,
              0);
    CHECK_BAD(aContext, aName, 0);


    index = aName - aContext->range_start;
    attr = &aContext->attributes[index];

    COND_FAIL(aContext,
              attr->attr_info.length >= 1,
              ATTR_ERROR_INVALID_TYPE, 0);

    if (!aValue) {
        /* fetch size only */
        return attr->attr_info.length;
    }

    count = min(aLength, attr->attr_info.length);

    SET_ERROR(aContext, ATTR_ERROR_NONE);

    switch (attr->attr_info.type) {
        case AT_FLOAT: {
            memcpy(aValue,
                   attr->attr_value[WORKING_ATTR_VALUE_INDEX].float_value,
                   count * attr->attr_info.size);
            break;
        }

        case AT_BOOLEAN:
        case AT_INTEGER: {
            OWFint i;
            OWFint* v = attr->attr_value[WORKING_ATTR_VALUE_INDEX].int_value;
            for (i = 0; i < count; i++) {
                aValue[i] = (OWFfloat) v[i];
            }
            break;
        }

        default: {
            SET_ERROR(aContext, ATTR_ERROR_INVALID_TYPE);
            break;
        }
    }
    return count;
}

/*
=============================================================================
SETTER FUNCTIONS
=============================================================================
*/

/*
 * \brief
 *
 * \param aContext
 * \param aName
 * \param aValue
 *
 * \return
 */
OWF_API_CALL void
OWF_Attribute_SetValuei(OWF_ATTRIBUTE_LIST* aContext,
                        OWFint aName,
                        OWFint aValue)
{
    OWFint                 index = 0;
    OWF_ATTRIBUTE*          attr = NULL;

    COND_FAIL_NR_NS(aContext);
    COND_FAIL_NR(aContext, aContext->attributes, ATTR_ERROR_INVALID_CONTEXT);
    CHECK_BAD_NR(aContext, aName);


    index = aName - aContext->range_start;
    attr = &aContext->attributes[index];

    COND_FAIL_NR(aContext,
                 1 == attr->attr_info.length,
                 ATTR_ERROR_INVALID_TYPE);
    COND_FAIL_NR(aContext, !attr->attr_info.readonly, ATTR_ERROR_ACCESS_DENIED);

    SET_ERROR(aContext, ATTR_ERROR_NONE);

    attr->attr_info.dirty = 1;

    switch (attr->attr_info.type) {
        case AT_FLOAT: {
            attr->attr_value[WORKING_ATTR_VALUE_INDEX].float_value[0] = aValue;
            break;
        }

        case AT_INTEGER:
        case AT_BOOLEAN: {
            attr->attr_value[WORKING_ATTR_VALUE_INDEX].int_value[0] = aValue;
            break;
        }

        default: {
            SET_ERROR(aContext, ATTR_ERROR_INVALID_TYPE);
            break;
        }
    }
}

/*
 * \brief
 *
 * \param aContext
 * \param aName
 * \param aValue
 *
 * \return
 */
OWF_API_CALL void
OWF_Attribute_SetValuef(OWF_ATTRIBUTE_LIST* aContext,
                        OWFint aName,
                        OWFfloat aValue)
{
    OWFint                 index = 0;
    OWF_ATTRIBUTE*          attr = NULL;

    COND_FAIL_NR_NS(aContext);
    COND_FAIL_NR(aContext, aContext->attributes, ATTR_ERROR_INVALID_CONTEXT);
    CHECK_BAD_NR(aContext, aName);


    index = aName - aContext->range_start;
    attr = &aContext->attributes[index];

    COND_FAIL_NR(aContext,
                 1 == attr->attr_info.length,
                 ATTR_ERROR_INVALID_TYPE);
    COND_FAIL_NR(aContext,
                 !attr->attr_info.readonly,
                 ATTR_ERROR_ACCESS_DENIED);

    SET_ERROR(aContext, ATTR_ERROR_NONE);

    attr->attr_info.dirty = 1;

    switch (attr->attr_info.type) {
        case AT_FLOAT: {
            attr->attr_value[WORKING_ATTR_VALUE_INDEX].float_value[0] = aValue;
            break;
        }

        case AT_INTEGER:
        case AT_BOOLEAN: {
            attr->attr_value[WORKING_ATTR_VALUE_INDEX].int_value[0] = floor(aValue);
            break;
        }

        default: {
            SET_ERROR(aContext, ATTR_ERROR_INVALID_TYPE);
            break;
        }
    }
}

/*
 * \brief
 *
 * \param
 * \param
 * \param
 *
 * \return
 */
OWF_API_CALL void
OWF_Attribute_SetValueb(OWF_ATTRIBUTE_LIST* aContext,
                        OWFint aName,
                        OWFboolean aValue)
{
    OWF_Attribute_SetValuei(aContext, aName, (OWFint) aValue);
}

/*
 * \brief
 *
 * \param
 * \param
 * \param
 *
 * \return
 */
OWF_API_CALL void
OWF_Attribute_SetValueiv(OWF_ATTRIBUTE_LIST* aContext,
                         OWFint aName,
                         OWFint aLength,
                         const OWFint* aValue)
{
    OWFint                 index = 0;
    OWF_ATTRIBUTE*          attr = NULL;
    OWFint                 count = 0;

    COND_FAIL_NR_NS(aContext);
    COND_FAIL_NR(aContext, aValue, ATTR_ERROR_INVALID_ARGUMENT);
    COND_FAIL_NR(aContext, aContext->attributes, ATTR_ERROR_INVALID_CONTEXT);
    CHECK_BAD_NR(aContext, aName);

    index = aName - aContext->range_start;
    attr = &aContext->attributes[index];
/*
    COND_FAIL_NR(aContext,
               attr->attr_info.length >= 1,
               ATTR_ERROR_INVALID_TYPE);
*/
    COND_FAIL_NR(aContext,
                 aLength > 0 && aLength <= attr->attr_info.length,
                 ATTR_ERROR_INVALID_ARGUMENT);

    COND_FAIL_NR(aContext,
                 !attr->attr_info.readonly,
                 ATTR_ERROR_ACCESS_DENIED);

    count = min(aLength, attr->attr_info.length);

    SET_ERROR(aContext, ATTR_ERROR_NONE);

    switch (attr->attr_info.type) {
        case AT_FLOAT: {
            OWFint i = 0;
            OWFfloat* v = attr->attr_value[1].float_value;
            for (i = 0; i < count; i++) {
                v[i] = floor(aValue[i]);
            }
            break;
        }

        case AT_BOOLEAN:
        case AT_INTEGER: {
            memcpy(attr->attr_value[1].int_value,
                   aValue,
                   count * attr->attr_info.size);
            break;
        }

        default: {
            SET_ERROR(aContext, ATTR_ERROR_INVALID_TYPE);
            break;
        }
    }
    aContext->attributes[index].attr_info.dirty = 1;
}

/*
 * \brief
 *
 * \param
 * \param
 * \param
 *
 * \return
 */
OWF_API_CALL void
OWF_Attribute_SetValuefv(OWF_ATTRIBUTE_LIST* aContext,
                         OWFint aName,
                         OWFint aLength,
                         const OWFfloat* aValue)
{
    OWFint                 index = 0;
    OWF_ATTRIBUTE*          attr = NULL;
    OWFint                 count = 0;

    COND_FAIL_NR_NS(aContext);
    COND_FAIL_NR(aContext, aValue, ATTR_ERROR_INVALID_ARGUMENT);
    COND_FAIL_NR(aContext, aContext->attributes, ATTR_ERROR_INVALID_CONTEXT);
    CHECK_BAD_NR(aContext, aName);

    index = aName - aContext->range_start;
    attr = &aContext->attributes[index];

/*
    COND_FAIL_NR(aContext,
               attr->attr_info.length >= 1,
               ATTR_ERROR_INVALID_TYPE);
*/

    COND_FAIL_NR(aContext,
                 aLength > 0 && aLength <= attr->attr_info.length,
                 ATTR_ERROR_INVALID_ARGUMENT);

    COND_FAIL_NR(aContext,
                 !attr->attr_info.readonly,
                 ATTR_ERROR_ACCESS_DENIED);

    count = min(aLength, attr->attr_info.length);

    SET_ERROR(aContext, ATTR_ERROR_NONE);

    switch (attr->attr_info.type) {
        case AT_FLOAT: {
            memcpy(attr->attr_value[1].float_value,
                   aValue,
                   count * attr->attr_info.size);
            break;
        }

        case AT_BOOLEAN:
        case AT_INTEGER: {
            OWFint i;
            OWFint* v = attr->attr_value[1].int_value;
            for (i = 0; i < count; i++) {
                 v[i] = floor(aValue[i]);
            }
            break;
        }

        default: {
            SET_ERROR(aContext, ATTR_ERROR_INVALID_TYPE);
            break;
        }
    }
    aContext->attributes[index].attr_info.dirty = 1;
}

static OWFint OWF_Attribute_Commit(OWF_ATTRIBUTE* aAttr, 
                            OWFint aDirtyFlag, 
                            OWFint aCopyTo,
                            OWFint aCopyFrom )
    {
	OWF_ASSERT(aCopyTo >= 0);
	OWF_ASSERT(aCopyTo < NUM_ATTR_VALUE_COPIES);
	OWF_ASSERT(aCopyFrom >= 0);
	OWF_ASSERT(aCopyFrom < NUM_ATTR_VALUE_COPIES);
    /* if type is undefined, it means there're gaps in the attribute
       range (e.g. reservations for future use and such.) ignore them. */
    if (aAttr->attr_info.type != AT_UNDEFINED && aDirtyFlag) 
        {
        /* poor-man's commit */
        memcpy(aAttr->attr_value[aCopyTo].gen_ptr,
                aAttr->attr_value[aCopyFrom].gen_ptr,
                aAttr->attr_info.size * aAttr->attr_info.length);
        return 0;
        }
    else
        {
        return aDirtyFlag;
        }
    }


OWF_API_CALL void
OWF_AttributeList_Commit(OWF_ATTRIBUTE_LIST* aContext,
                     OWFint aStart,
                     OWFint aEnd,
		     OWFint aCopyTo )
{
    OWFint                 index = 0;
    /* Attribute commit works like the element list commit
     * by forward-copying the "working" attributes to the snapshot  
     * during client invoked commit,
     * then copying the snapshot to the commited scene during the docommit job.
     * This requires the same wait-for-the-previous-commit-job strategy used in the element commit.
     * Could in future use copy-back technique to avoid having to wait substantially, 
     * in which case the index of the working attribute set would switch after each invoked commit,
     * instead of being a constant.
     *
     * The same number of copies would still need to take place  
     * but would not need exclusive access to the list.
     */

    COND_FAIL_NR_NS(aContext);
    COND_FAIL_NR(aContext, aStart <= aEnd, ATTR_ERROR_INVALID_ARGUMENT);
    COND_FAIL_NR(aContext, aContext->attributes, ATTR_ERROR_INVALID_CONTEXT);
    CHECK_BAD_NR(aContext, aStart);
    CHECK_BAD_NR(aContext, aEnd);

    switch (aCopyTo)
        {
        case COMMITTED_ATTR_VALUE_INDEX: /* Used in composition thread to set displayed scene attributes */
            for (index = aStart; index <= aEnd; index++) 
                {
                OWF_ATTRIBUTE* attr = &aContext->attributes[index - aContext->range_start];
                attr->attr_info.dirtysnapshot=
                    OWF_Attribute_Commit(attr,attr->attr_info.dirtysnapshot,
                            COMMITTED_ATTR_VALUE_INDEX,SNAPSHOT_ATTR_VALUE_INDEX);
                }
            break;
        case SNAPSHOT_ATTR_VALUE_INDEX: /* Used in API threads to make a snapshot of the client attributes */
             for (index = aStart; index <= aEnd; index++) 
                 {
                 OWF_ATTRIBUTE* attr = &aContext->attributes[index - aContext->range_start];
                 OWFuint oldDirty=attr->attr_info.dirty;
                 attr->attr_info.dirtysnapshot=oldDirty;
                 attr->attr_info.dirty=
                     OWF_Attribute_Commit(attr,oldDirty,
                             SNAPSHOT_ATTR_VALUE_INDEX,WORKING_ATTR_VALUE_INDEX);
                 }
             break;
        case WORKING_ATTR_VALUE_INDEX:   /* Used in initialisation to copy displayed attributes to client copies */
             for (index = aStart; index <= aEnd; index++) 
                 {
                 OWF_ATTRIBUTE* attr = &aContext->attributes[index - aContext->range_start];
                 OWF_Attribute_Commit(attr,!attr->attr_info.readonly,
                         WORKING_ATTR_VALUE_INDEX,COMMITTED_ATTR_VALUE_INDEX);
                 }
             break;
	case COMMIT_ATTR_DIRECT_FROM_WORKING: /* Used in WFD to commit new working values directly in 1 step. */
            for (index = aStart; index <= aEnd; index++) 
                {
                OWF_ATTRIBUTE* attr = &aContext->attributes[index - aContext->range_start];
                attr->attr_info.dirty=
                    OWF_Attribute_Commit(attr,attr->attr_info.dirty,
                            COMMITTED_ATTR_VALUE_INDEX,WORKING_ATTR_VALUE_INDEX);
                }
            break;
	default:
			COND_FAIL_NR(aContext, 0, ATTR_ERROR_INVALID_ARGUMENT);
          }
    
    SET_ERROR(aContext, ATTR_ERROR_NONE);
}

#ifdef __cplusplus
}
#endif
