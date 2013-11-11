/* Copyright (c) 2009 The Khronos Group Inc.
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

#ifndef OWFATTRIBUTES_H_
#define OWFATTRIBUTES_H_

#include "owftypes.h"
#include "owfutils.h"


#ifdef __cplusplus
extern "C"
{
#endif


/*!
 *  \file Attribute interface
 *
 *  Attribute interface provides mechanism for controlled reading/writing
 *  access of different object properties in WFC and WFD APIs by proxying
 *  object instances' properties.
 *
 */
#define RANGE_UNDEFINED     0xFFFFFFFF
#define ATTR_LENGTH_BITS    20
#define MAX_ATTR_LENGTH     (1 << ATTR_LENGTH_BITS)-1
#define NUM_ATTR_VALUE_COPIES 3
#define COMMITTED_ATTR_VALUE_INDEX 0
#define WORKING_ATTR_VALUE_INDEX 1
#define SNAPSHOT_ATTR_VALUE_INDEX 2
#define COMMIT_ATTR_DIRECT_FROM_WORKING -1

typedef enum {
    ATTR_ERROR_NONE             = 0,
    ATTR_ERROR_NOT_FOUND        = -1,
    ATTR_ERROR_ALREADY_IN_USE   = -2,
    ATTR_ERROR_INVALID_ATTRIBUTE= -3,
    ATTR_ERROR_INVALID_VALUE    = -4,
    ATTR_ERROR_INVALID_ARGUMENT = -5,
    ATTR_ERROR_INVALID_CONTEXT  = -6,
    ATTR_ERROR_NO_MEMORY        = -7,
    ATTR_ERROR_INSANE           = -8,
    ATTR_ERROR_ACCESS_DENIED    = -9,
    ATTR_ERROR_INVALID_TYPE     = -10,
    ATTR_ERROR_CANT_HANDLE      = -11
} OWF_ATTRIBUTE_LIST_STATUS;

/* Attribute value element types */
typedef enum {
    AT_UNDEFINED = 0,
    AT_INTEGER = 1,
    AT_FLOAT = 2,
    AT_BOOLEAN = 3
} OWF_ATTRIBUTE_TYPE;


/*
 * Attribute information (header)
 *
 * Dirty field is internal. Do not fiddle with it.
 * 
 * Comment on use of bitfields: 
 * 1: It stops generic use of split dirty flag
 * 2: Some compilers will not reorder members to merge length into earier fields.
 */
typedef struct {
    OWFuint                 type: 2;
    OWFuint                 dirty: 1;
    OWFuint                 dirtysnapshot: 1;
    OWFuint                 readonly: 1;
    OWFuint                 size;                       /* Size of one primitive */
    OWFuint                 length: ATTR_LENGTH_BITS;   /* Number of primitives in vector */
} OWF_ATTRIBUTE_INFO;

/*
 * Attribute may be a scalar (1-element) or vector (n-element),
 * containing a reference to either integer, boolean or float value(s).
 * Attribute type cannot be changed after it has been initialized.
 *
 * Attributes don't own the data they refer to; attributes only
 * control the reading and writing of the data they refer to.
 */

typedef OWFfloat*           OWF_FLOAT_REF;
typedef OWFint*             OWF_INT_REF;
typedef OWFboolean*         OWF_BOOL_REF;
typedef OWFfloat*           OWF_FLOAT_VECTOR_REF;
typedef OWFint*             OWF_INT_VECTOR_REF;

typedef struct {
    OWF_ATTRIBUTE_INFO      attr_info;
    union {
        OWF_INT_REF             int_value;
        OWF_FLOAT_REF           float_value;
        void*                   gen_ptr;
        /*
        OWF_INT_VECTOR_REF      int_vector_value;
        OWF_FLOAT_VECTOR_REF    float_vector_value;
        */
    } attr_value[NUM_ATTR_VALUE_COPIES];
} OWF_ATTRIBUTE;

/*
 * Attribute list/context. Container for attributes.
 *
 * One list/context can contain constant number of
 * attributes (amount of which is defined at construction
 * time)
 */
typedef struct {
    OWFint                  range_start;
    OWFint                  range_end;
    OWF_ATTRIBUTE*          attributes;
    OWF_ATTRIBUTE_LIST_STATUS  last_error;
} OWF_ATTRIBUTE_LIST;

/*
 * \brief Initializes attribute context
 *
 * \param aContext Attribute context to initialize
 * \param aStart Attribute range start
 * \param aEnd Attribute range end. Must be greater than range start.
 *
 * \return ERR_INVALID_ARGUMENT
 * ERR_NO_MEMORY
 */
OWF_API_CALL void
OWF_AttributeList_Create(OWF_ATTRIBUTE_LIST* aContext,
                         OWFint aStart,
                         OWFint aEnd);

/*
 * \brief Destroy attribute context and free any resources (memory
 * blocks) allocated to it. All attributes are destroyed.
 *
 * \param aContext Attribute context to destroy
 *
 * \return ERR_INVALID_ARGUMENT
 * ERR_INVALID_CONTEXT
 */
OWF_API_CALL void
OWF_AttributeList_Destroy(OWF_ATTRIBUTE_LIST* aContext);

/*
 * \brief Gets the last error from the attribute.
 * Resets the error state.
 *
 * \return	Error code.
 *
 */
OWF_API_CALL OWF_ATTRIBUTE_LIST_STATUS
OWF_AttributeList_GetError(OWF_ATTRIBUTE_LIST* aContext);

/*
 * \brief Intialize integer attribute
 *
 * \param aContext Attibute context
 * \param aName Attribute name
 * \param aValue Attribute initial value
 * \param aRdOnly Read-only flag
 *
 * \return ERR_INVALID_ARGUMENT
 * ERR_INVALID_ATTRIBUTE
 * ERR_INVALID_CONTEXT
 */
OWF_API_CALL void
OWF_Attribute_Initi(OWF_ATTRIBUTE_LIST* aContext,
                    OWFint aName,
                    OWF_INT_REF aValue,
                    OWFboolean aRdOnly);

/*
 * \brief Initialize float attribute
 *
 * \param aContext Attribute context
 * \param aName Attribute name
 * \param aValue Initial value for attribute
 * \param aRdOnly Read-only flag
 *
 * \return ERR_INVALID_ARGUMENT
 * ERR_INVALID_ATTRIBUTE
 * ERR_INVALID_CONTEXT
 */
OWF_API_CALL void
OWF_Attribute_Initf(OWF_ATTRIBUTE_LIST* aContext,
                    OWFint aName,
                    OWF_FLOAT_REF aValue,
                    OWFboolean aRdOnly);

/*
 * \brief Initialize boolean attribute
 *
 * \param aContext Attribute context
 * \param aName Attribute name
 * \param aValue Initial value for attribute
 * \param aRdOnly Read-only flag
 *
 * \return ERR_INVALID_ARGUMENT
 * ERR_INVALID_ATTRIBUTE
 * ERR_INVALID_CONTEXT
 */
OWF_API_CALL void
OWF_Attribute_Initb(OWF_ATTRIBUTE_LIST* aContext,
                    OWFint aName,
                    OWF_BOOL_REF aValue,
                    OWFboolean aRdOnly);

/*
 * \brief Initialize vector attribute
 *
 * \param aContext Attribute context
 * \param aName Attribute name
 * \param aLength Attribute (vector) length
 * \param aValues Initial value(s) for attribute
 * \param aRdOnly Read-only flag
 *
 * \return ERR_INVALID_ARGUMENT
 * ERR_INVALID_ATTRIBUTE
 * ERR_INVALID_CONTEXT
 * ERR_CANT_HANDLE
 */
OWF_API_CALL void
OWF_Attribute_Initiv(OWF_ATTRIBUTE_LIST* aContext,
                     OWFint aName,
                     OWFint aLength,
                     OWF_INT_VECTOR_REF aValues,
                     OWFboolean aRdOnly);

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
                     OWFboolean aRdOnly);

/*
 * \brief Get attribute integer value.
 *
 * \param aContext Attribute context
 * \param aName Attribute name
 *
 * \return Attribute integer value (floats are floor()ed). For vector
 * attributes the return value will be error ERR_INVALID_TYPE.
 * ERR_INVALID_ATTRIBUTE
 * ERR_INVALID_CONTEXT
 */
OWF_API_CALL OWFint
OWF_Attribute_GetValuei(OWF_ATTRIBUTE_LIST* aContext,
                        OWFint aName);

/*
 * \brief Return boolean attribute value
 *
 * \param aContext Attribute context
 * \param aName Attribute name
 *
 * \return Attribute value
 * ERR_INVALID_ATTRIBUTE
 * ERR_INVALID_TYPE
 */
OWF_API_CALL OWFboolean
OWF_Attribute_GetValueb(OWF_ATTRIBUTE_LIST* aContext,
                        OWFint aName);

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
                        OWFint aName);

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
                         OWFint* aValue);

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
                         OWFfloat* aValue);

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
                        OWFint aValue);

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
                        OWFfloat aValue);

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
                        OWFboolean aValue);

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
                         const OWFint* aValue);

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
                         const OWFfloat* aValue);


OWF_API_CALL void
OWF_AttributeList_Commit(OWF_ATTRIBUTE_LIST* aContext, OWFint aStart, OWFint aEnd, OWFint aCopyTo );


#ifdef __cplusplus
}
#endif

#endif /* ATTRIBUTES_H_ */
