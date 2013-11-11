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
 * @file asn1ErrCodes.h
 * List of numeric status codes that can be returned by ASN1C run-time 
 * functions and generated code.
 */

/**
@file
@internalTechnology
*/

#ifndef _ASN1ERRCODES_H_
#define _ASN1ERRCODES_H_

/**
 * @defgroup asn1ErrCodes Run-time error status codes.
 * This is a list of status codes that can be returned by the ASN1C 
 * run-time functions and generated code.  In many cases, additional 
 * information and parameters for the different errors are stored in 
 * the context structure at the time the error in raised.  This 
 * additional information can be output using the \c rtxErrPrint or 
 * \c rtxErrLogUsingCB run-time functions.
 * @{
 */

/**
 * Fragment decode success status.  This is returned when decoding is 
 * successful but only a fragment of the item was decoded.  User should 
 * repeat the decode operation in order to fully decode message.
 */
#define ASN_OK_FRAG     2

/**
 * Error base.  ASN.1 specific errors start at this base number to 
 * distinguish them from common and other error types.
 */
#define ASN_E_BASE      -100

/**
 * Invalid object identifier.  This error code is returned when an 
 * object identifier is encountered that is not valid.  Possible reasons 
 * for being invalid include invalid first and second arc identifiers 
 * (first must be 0, 1, or 2; second must be less than 40), not enough 
 * subidentifier values (must be 2 or more), or too many arc values 
 * (maximum number is 128).
 */
#define ASN_E_INVOBJID  (ASN_E_BASE)

/**
 * Invalid length.  This error code is returned when a length value is
 * parsed that is not consistent with other lengths in a BER/DER message. 
 * This typically happens when an inner length within a constructed type 
 * is larger than the outer length value.
 */
#define ASN_E_INVLEN    (ASN_E_BASE-1)

/**
 * Bad tag value.  This error code is returned when a tag value is
 * parsed with an identifier code that is too large to fit in a 32-bit 
 * integer variable.
 */
#define ASN_E_BADTAG    (ASN_E_BASE-2)

/**
 * Invalid binary string.  This error code is returned when decoding XER
 * data and a bit string value is received that contains something other
 * than '1' or '0' characters.
 */
#define ASN_E_INVBINS   (ASN_E_BASE-3)

/**
 * Invalid table constraint index.  This error code is returned when a 
 * value is provided to index into a table and the value does not match 
 * any of the defined indexes.
 */
#define ASN_E_INVINDEX  (ASN_E_BASE-4)

/**
 * Invalid table constraint value.  This error code is returned when a 
 * the value for an element in a table-constrained message instance does 
 * not match the value for the element defined in the table.
 */
#define ASN_E_INVTCVAL  (ASN_E_BASE-5)

/**
 * Concurrent list modification error.  This error is returned from 
 * within a list iterator when it is detected that the list was 
 * modified outside the control of the iterator.
 */
#define ASN_E_CONCMODF  (ASN_E_BASE-6)

/**
 * Illegal state for operation.  This error is returned in places where 
 * an operation is attempted but the object is not in a state that would 
 * allow the operation to be completed.  One example is in a list iterator 
 * class when an attempt is made to remove a node but the node does 
 * not exist.
 */
#define ASN_E_ILLSTATE  (ASN_E_BASE-7)

/**
 * Encode/Decode method called for non-PDU. This error is returned when
 * an Encode or Decode method is called on a non-PDU. Only PDUs have
 * implementations of these methods.
 */
#define ASN_E_NOTPDU    (ASN_E_BASE-8)

/**
 * Element type could not be resolved at run-time.  This error is returned 
 * when the run-time parser modules is used (Asn1RTProd) to decode a type 
 * at run-time and the type of the element could not be resolved.
 */
#define ASN_E_UNDEFTYP  (ASN_E_BASE-9)

/**
 * @} asn1ErrCodes
 */
#endif
