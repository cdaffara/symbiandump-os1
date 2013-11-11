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

//
// CHANGE LOG
// Date         Init    Description
//
//
/** 
 * @file rtsrc/ASN1TOctStr.h 
 * ASN.1 OCTET string class definition.
 */

/**
@file
@internalTechnology
*/

#ifndef _ASN1TOCTSTR_H_
#define _ASN1TOCTSTR_H_

#include "rtsrc/asn1type.h"
/** 
 * @ingroup cppruntime 
 */
/**
 * @addtogroup asn1data
 * @{
 */
/** 
 * Dynamic octet string. This is the base class for generated C++ data type
 * classes for unsized OCTET string's.
 */
struct EXTRTCLASS ASN1TDynOctStr : public ASN1DynOctStr {
   /**
    * The default constructor creates an empty octet string.
    */
   ASN1TDynOctStr () { numocts = 0; }

   /**
    * This constructor initializes the octet string to contain the given data
    * values.
    *
    * @param _numocts     - Number of octet in the octet string.
    * @param _data        - The binary octet data values.
    */
   ASN1TDynOctStr (OSUINT32 _numocts, const OSOCTET* _data) {
      numocts = _numocts;
      data = _data;
   }

   /**
    * This constructor initializes the octet string to contain the given data
    * values.
    *
    * @param _os          - C octet string structure.
    */
   ASN1TDynOctStr (ASN1DynOctStr& _os) {
      numocts = _os.numocts;
      data = _os.data;
   }

   /**
    * This constructor initializes the octet string to contain the given data
    * values. In this case, it is initializes the string to contain the 
    * characters in a null-terminated C character string.
    *
    * @param cstring      - C null-terminated string.
    */
   ASN1TDynOctStr (const char* cstring) {
      if (strlen(cstring) > 0) {
         numocts = (OSUINT32)strlen(cstring) + 1;
         data = (const OSOCTET*) cstring;
      }
      else numocts = 0;
   }

   /**
    * This assignment operator sets the octet string to contain the characters
    * in a null-terminated C character string. For example, <code>myOctStr = "a
    * char string";</code>
    *
    * @param cstring      - C null-terminated string.
    */
   ASN1TDynOctStr& operator= (const char* cstring) {
      if (strlen(cstring) > 0) {
         numocts = (OSUINT32)strlen(cstring) + 1;
         data = (const OSOCTET*) cstring;
      }
      else numocts = 0;
      return *this;
   }

   /**
    * This assignment operator sets the octet string to contain the characters
    * from the given C++ octet string object.
    *
    * @param octet      - Octet string object reference
    */
   EXTRTMETHOD ASN1TDynOctStr& operator=(const ASN1TDynOctStr& octet);

   /**
    * This method converts the binary octet string to a human-readable 
    * representation.  The string is first checked to see if it contains 
    * all printable characters.  If this is the case, the characters in 
    * the string are returned; otherwise, the string contents are converted 
    * into a hexadecimal character string.
    *
    * @param pctxt      - Pointer to a context structure.
    */
   EXTRTMETHOD const char* toString (OSCTXT* pctxt) const;

   /**
    * This method converts the binary octet string to a hexadecimal 
    * string representation.
    *
    * @param pctxt      - Pointer to a context structure.
    */
   EXTRTMETHOD const char* toHexString (OSCTXT* pctxt) const;

   /**
    * This method compares the first n octets of this octet string with 
    * the given octet string.
    *
    * @param n          - Number of octets to compare
    * @param o          - Octet string for comparison
    * @return           - 0 if strings are equal, -1 if this octet string is
    *                     less than the given string, +1 if this string > 
    *                     given string.
    */
   EXTRTMETHOD int nCompare (OSUINT32 n, const ASN1TDynOctStr &o) const;

} ;

//operator==

EXTERNRT int operator==(const ASN1TDynOctStr &lhs,const ASN1TDynOctStr &rhs);
EXTERNRT int operator==(const ASN1TDynOctStr &lhs,const char *string);
EXTERNRT int operator==(const ASN1DynOctStr &lhs,const ASN1DynOctStr &rhs);
EXTERNRT int operator==(const ASN1DynOctStr &lhs,const char *string);

//operator !=

EXTERNRT int operator!=(const ASN1TDynOctStr &lhs,const ASN1TDynOctStr &rhs);
EXTERNRT int operator!=(const ASN1TDynOctStr &lhs,const char *string);
EXTERNRT int operator!=(const ASN1DynOctStr &lhs,const ASN1DynOctStr &rhs);
EXTERNRT int operator!=(const ASN1DynOctStr &lhs,const char *string);

//operator <

EXTERNRT int operator<(const ASN1TDynOctStr &lhs,const ASN1TDynOctStr &rhs);
EXTERNRT int operator<(const ASN1TDynOctStr &lhs,const char *string);
EXTERNRT int operator<(const ASN1DynOctStr &lhs,const ASN1DynOctStr &rhs);
EXTERNRT int operator<(const ASN1DynOctStr &lhs,const char *string);

//operator <=

EXTERNRT int operator<=(const ASN1TDynOctStr &lhs,const ASN1TDynOctStr &rhs);
EXTERNRT int operator<=(const ASN1TDynOctStr &lhs,const char *string);
EXTERNRT int operator<=(const ASN1DynOctStr &lhs,const ASN1DynOctStr &rhs);
EXTERNRT int operator<=(const ASN1DynOctStr &lhs,const char *string);

 //operator >

EXTERNRT int operator>(const ASN1TDynOctStr &lhs,const ASN1TDynOctStr &rhs);
EXTERNRT int operator>(const ASN1TDynOctStr &lhs,const char *string);
EXTERNRT int operator>(const ASN1DynOctStr &lhs,const ASN1DynOctStr &rhs);
EXTERNRT int operator>(const ASN1DynOctStr &lhs,const char *string);

//operator >=

EXTERNRT int operator>=(const ASN1TDynOctStr &lhs,const ASN1TDynOctStr &rhs);
EXTERNRT int operator>=(const ASN1TDynOctStr &lhs,const char *string);
EXTERNRT int operator>=(const ASN1DynOctStr &lhs,const ASN1DynOctStr &rhs);
EXTERNRT int operator>=(const ASN1DynOctStr &lhs,const char *string);

#endif

/**
 * @}
 */
