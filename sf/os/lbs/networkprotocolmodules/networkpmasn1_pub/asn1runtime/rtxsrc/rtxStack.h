// Copyright (c) 2001-2009 Objective Systems, Inc. (http://www.obj-sys.com) 
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available 
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Objective Systems, Inc - initial contribution.
//
// Contributors:
//
// Description:
// 

/**
 * @file rtxStack.h
 * Simple FIFO stack for storing void pointers to any type of data.
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXSTACK_H_
#define _RTXSTACK_H_

#include "rtxsrc/rtxContext.h"

/**
 * @defgroup ccfStack Stack Utility Functions
 *
 * This is a simple stack structure with supporting push and pop functions.
 * Different initialization routines are provided to permit different 
 * memory management schemes.
 * 
 *@{
 */
/**
 * @struct _OSRTStack
 * This is the main stack structure.  It uses a linked list structure.
 */
typedef struct _OSRTStack {
   OSCTXT*   pctxt;
   OSRTDList dlist;
} OSRTStack;

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * This function creates a new stack structure. It allocates memory for the
 * structure and calls rtxStackInit to initialize the structure.
 *
 * @param pctxt        A pointer to the context with which the stack is 
 *                      associated.
 * @return             A pointer to an allocated stack structure.
 */
EXTERNRT OSRTStack* rtxStackCreate (OSCTXT* pctxt);

/** 
 * This function initializes a stack structure. It sets the number of elements
 * to zero and sets all internal pointer values to NULL.
 *
 * @param pctxt        A pointer to the context with which the stack is
 *                      associated.
 * @param pStack       A pointer to a stack structure to be initialized.
 */
EXTERNRT void  rtxStackInit (OSCTXT* pctxt, OSRTStack* pStack);

/** 
 * This macro tests if the stack is empty.
 *
 * @param stack        Stack structure variable to be tested.
 */
#define rtxStackIsEmpty(stack) (OSBOOL)((stack).dlist.count == 0) 

/** 
 * This function pops an item off the stack.
 *
 * @param pStack       The pointer to the stack structure from which the value
 *                       is to be popped. Pointer to the updated stack
 *                       structure.
 * @return             The pointer to the item popped from the stack
 */
EXTERNRT void* rtxStackPop (OSRTStack* pStack);

/**
 * This function pushes an item onto the stack.
 *
 * @param pStack       A pointer to the structure onto which the data item is
 *                       to be pushed. The pointer updated to the stack
 *                       structure
 * @param pData        A pointer to the data item to be pushed on the stack.
 * @return             Completion status of operation:
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtxStackPush (OSRTStack* pStack, void* pData);

/**
 * This functions returns the data item on the top of the stack.
 *
 * @param pStack       A pointer to the structure onto which the data item is
 *                       to be pushed. The pointer updated to the stack
 *                       structure
 * @return             Pointer to data item at top of stack or NULL if 
 *                       stack empty.
 *                       - 0 (0) = success,
 *                       - negative return value is error.
 */
EXTERNRT void* rtxStackPeek (OSRTStack* pStack);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif
