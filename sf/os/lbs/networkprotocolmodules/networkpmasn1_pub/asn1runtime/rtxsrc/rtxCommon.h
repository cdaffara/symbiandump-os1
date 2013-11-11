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

/*The following is the text for the main C Runtime index page.*/
/*! \mainpage
 * <H1>C Common Runtime Library Functions</H1>
 *
 * The <B>C run-time common library</B> contains common C functions used by  
 * the low-level encode/decode functions. These functions are identified 
 * by their <I>rtx</I> prefixes. 
 *
 * The categories of functions provided are as follows:<UL>
 * <LI>Context management functions handle the allocation, 
 * initialization, and destruction of context variables 
 * (variables of type OSCTXT) that handle the working data used 
 * during the encoding or decoding of a message.</LI>
 * <LI>Memory allocation macros and functions provide an optimized memory 
 * management interface.</LI> 
 * <LI>Doubly linked list (DList) functions are used to manipulate linked 
 * list structures that are used to model repeating XSD types and 
 * elements.</LI>
 * <LI>UTF-8 and Unicode character string functions provide support 
 * for conversions to and from these formats in C or C++.</LI>
 * <LI>Date/time conversion functions provide utilities for converting 
 * system and structured numeric date/time values to XML schema standard 
 * string format.</LI>
 * <LI>Pattern matching function compare strings against patterns specified 
 * using regular expressions (regexp's).</LI>
 * <LI>Diagnostic trace functions allow the output of trace messages
 * to standard output.</LI> 
 * <LI>Error formatting and print functions allow information about 
 * encode/decode errors to be added to a context block structure 
 * and printed out.</LI>
 * <LI>Memory buffer management functions handle the allocation, 
 * expansion, and de-allocation of dynamic memory buffers used by some
 * encode/decode functions.</LI> 
 * <LI>Formatted print functions allow binary data to be
 * formatted and printed to standard output and other output devices.</LI>
 * <LI>Big Integer helper functions are arbitrary-precision integer 
 * manipulating functions used to maintain big integers.</LI>
 * </UL>
 *
 */ 
/** 
 * @file rtxCommon.h 
 * Common runtime constants, data structure definitions, and run-time functions
 * to support various data encoding standards.
 */

/**
@file
@internalTechnology
*/

#ifndef _RTXCOMMON_H_
#define _RTXCOMMON_H_

#include "rtxsrc/osSysTypes.h"
#include "rtxsrc/osMacros.h"
#include "rtxsrc/rtxExternDefs.h"
#include "rtxsrc/rtxBigInt.h"
#include "rtxsrc/rtxBitString.h"
#include "rtxsrc/rtxBuffer.h"
#include "rtxsrc/rtxCharStr.h"
#include "rtxsrc/rtxCommonDefs.h"
#include "rtxsrc/rtxDateTime.h"
#include "rtxsrc/rtxDiag.h"
#include "rtxsrc/rtxEnum.h"
#include "rtxsrc/rtxError.h"
#include "rtxsrc/rtxFile.h"
#include "rtxsrc/rtxMemory.h"
#include "rtxsrc/rtxPattern.h"
#include "rtxsrc/rtxReal.h"
#include "rtxsrc/rtxUTF8.h"
#include "rtxsrc/rtxUtil.h"

#endif
