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
 * @file rtsrc/ASN1TObjId.h 
 * ASN.1 object identifier class definition.
 */

/**
@file
@internalTechnology
*/

#ifndef _ASN1TOBJID_H_
#define _ASN1TOBJID_H_

#include "rtsrc/asn1type.h"

/** 
 * @ingroup cppruntime 
 */
/**
 * @addtogroup asn1data
 * @{
 */
/** 
 * Object identifier. This is the base class for generated C++ data type
 * classes for object identifier values.
 */
struct EXTRTCLASS ASN1TObjId : public ASN1OBJID {
   /**
    * The default constructor creates an empty object identifier value.
    */
   ASN1TObjId () { numids = 0; }

   /**
    * The Virtual Destructor 
    */
   virtual EXTRTMETHOD ~ASN1TObjId ();

   /**
    * This constructor initializes the object identifer to contain the given
    * data values.
    *
    * @param _numids      - Number of subidentifiers in the OID.
    * @param _subids      - Array of subidentifier values.
    */
   EXTRTMETHOD ASN1TObjId (OSOCTET _numids, const OSUINT32* _subids);

   /**
    * This constructor initializes the object identifier to contain the given
    * data values. This can be used to set the value to a compiler-generated
    * OID value.
    *
    * @param oid          - C object identifier value.
    */
   EXTRTMETHOD ASN1TObjId (const ASN1OBJID& oid);

   /**
    * The copy constructor.
    *
    * @param oid          - C++ object identifier value.
    */
   EXTRTMETHOD ASN1TObjId (const ASN1TObjId& oid);

   /**
    * Construct an OID from a dotted string.
    *
    * @param dotted_oid_string - for example "1.3.1.6.1.10"
    */
   EXTRTMETHOD ASN1TObjId (const char *dotted_oid_string);
  
   /**
    * Assignment from a string.
    *
    * @param dotted_oid_string - New value (for example "1.3.6.1.6.0");
    */
   EXTRTMETHOD ASN1TObjId& operator=(const char *dotted_oid_string);

   /**
    * This assignment operator sets the object identifier to contain the OID in
    * the given C structure. This can be used to set the value to a
    * compiler-generated OID value.
    *
    * @param rhs          - C object identifier value.
    */
   EXTRTMETHOD void operator= (const ASN1OBJID& rhs);

   /**
    * This assignment operator sets the object identifier to contain the OID in
    * the given C++ structure.
    *
    * @param rhs          - C++ object identifier value.
    */
   EXTRTMETHOD void operator= (const ASN1TObjId& rhs);

   /**
    * Overloaded  += operator.
    * This operator allows subidentifiers in the form of a dotted OID 
    * string ("n.n.n") to be appended to an existing OID object.
    *
    * @param dotted_oid_string - C++ object identifier value.
    * @return             - True if values are equal.
    */
   EXTRTMETHOD ASN1TObjId& operator+=(const char *dotted_oid_string);

   /**
    * Overloaded  += operator.
    * This operator allows a single subidentifier in the form of an 
    * integer value to be appended to an existing OID object.
    *
    * @param i            - Subidentifier to append.
    * @return             - True if values are equal.
    */
   EXTRTMETHOD ASN1TObjId& operator+=(const OSUINT32 i);

   /**
    * Overloaded  += operator.
    * This operator allows one object identifier to be appended to 
    * another object identifier.
    *
    * @param o            - C++ object identifier value.
    * @return             - True if values are equal.
    */
   EXTRTMETHOD ASN1TObjId& operator+=(const ASN1TObjId &o);
   
   /**
    * Get a printable ASCII string of a part of the value.
    *
    * @param pctxt        - Pointer to a context structure.
    * @return             - Dotted OID string (for example "3.6.1.6")
    */
   EXTRTMETHOD const char *toString (OSCTXT* pctxt) const;

   /**
    * Sets the data of an object identifier using a pointer and a length.
    *
    * @param raw_oid      - Pointer to an array of subidentifier values.
    * @param oid_len      - Number of subids in the array,
    */
   EXTRTMETHOD void set_data (const OSUINT32 *raw_oid, OSUINT32 oid_len);

   /**
    * Compare the first n sub-ids(left to right) of two object identifiers.
    *
    * @param n            - Number of subid values to compare.
    * @param o            - OID to compare this OID with.
    * @return             - 0 if OID's are equal, -1 if this OID less than 
    *                       given OID, +1 if this OID > given OID.
    */
   EXTRTMETHOD int nCompare(const OSUINT32 n, const ASN1TObjId &o) const;

   /**
    * Compare the last n sub-ids(right to left) of two object identifiers.
    *
    * @param n            - Number of subid values to compare.
    * @param o            - OID to compare this OID with.
    * @return             - 0 if OID's are equal, -1 if this OID less than 
    *                       given OID, +1 if this OID > given OID.
    */
   EXTRTMETHOD int RnCompare(const OSUINT32 n, const ASN1TObjId &o) const;

   /**
    * Trim the given number of rightmost sub elements from this OID.
    *
    * @param n - number of subids to trim from OID
    */
   EXTRTMETHOD void trim(const OSUINT32 n);
   
};

/**
 * This comparison operator allows for comparison of equality of two C-based
 * object identifier structures.
 *
 * @param lhs          - C object identifier value.
 * @param rhs          - C object identifier value.
 * @return             - True if values are equal.
 */
EXTERNRT int operator== (const ASN1OBJID& lhs, const ASN1OBJID& rhs);

/**
 * This comparison operator allows for comparison of equality of two C++-based
 * object identifier structures.
 *
 * @param lhs          - C++ object identifier value.
 * @param rhs          - C++ object identifier value.
 * @return             - True if values are equal.
 */
EXTERNRT int operator==(const ASN1TObjId &lhs, const ASN1TObjId &rhs);

/**
 * This comparison operator allows for comparison of equality of a C-based
 * object identifier structure and a dotted string.
 *
 * @param lhs          - C object identifier value.
 * @param dotted_oid_string - String containing OID value to compare.
 * @return             - True if values are equal.
 */
EXTERNRT int operator==(const ASN1OBJID &lhs, const char *dotted_oid_string);

/**
 * This comparison operator allows for comparison of equality of  C++ based
 * object identifier structure and a dotted string.
 *
 * @param lhs          - C++ object identifier value.
 * @param dotted_oid_string - String containing OID value to compare.
 * @return             - True if values are equal.
 */
EXTERNRT int operator==(const ASN1TObjId &lhs, const char *dotted_oid_string);
  
/**
 * Overloaded not equal operator.
 * This comparison operator allows for comparison of not equality of  C++ based
 * object identifier structure and a dotted string.
 *
 * @param lhs          - C++ object identifier value.
 * @param rhs          - C++ object identifier value
 * @return             - True if values are equal.
 */
EXTERNRT int operator!=(const ASN1TObjId &lhs, const ASN1TObjId &rhs);

/**
 * Overloaded not equal operator.
 * This comparison operator allows for comparison of not equality of  C based
 * object identifier structure and a dotted string.
 *
 * @param lhs          - C object identifier value.
 * @param rhs          - C object identifier value
 * @return             - True if values are equal.
 */
EXTERNRT int operator!=(const ASN1OBJID &lhs, const ASN1OBJID &rhs);

/**
 * Overloaded not equal operator.
 * This comparison operator allows for comparison of not equality of  C based
 * object identifier structure and a dotted string.
 *
 * @param lhs          - C object identifier value.
 * @param dotted_oid_string - String containing OID value to compare.
 * @return             - True if values are equal.
 */
EXTERNRT int operator!=(const ASN1OBJID &lhs, const char *dotted_oid_string);

/**
 * Overloaded not equal operator.
 * This comparison operator allows for comparison of not equality of  C++ based
 * object identifier structure and a dotted string.
 *
 * @param lhs          - C++ object identifier value.
 * @param dotted_oid_string - String containing OID value to compare.
 * @return             - True if values are equal.
 */
EXTERNRT int operator!=(const ASN1TObjId &lhs, const char *dotted_oid_string);

/**
 * Overloaded less than < operator.
 * This comparison operator allows for comparison of less than of  C++ based
 * object identifier structure and a dotted string.
 *
 * @param lhs          - C++ object identifier value.
 * @param rhs          - C++ object identifier value.
 * @return             - True if values are equal.
 */
EXTERNRT int operator<(const ASN1TObjId &lhs, const ASN1TObjId &rhs);

/**
 * Overloaded less than < operator.
 * This comparison operator allows for comparison of less than of  C based
 * object identifier structure and a dotted string.
 *
 * @param lhs          - C object identifier value.
 * @param rhs          - C object identifier value.
 * @return             - True if values are equal.
 */
EXTERNRT int operator<(const ASN1OBJID &lhs, const ASN1OBJID &rhs);

/**
 * Overloaded less than < operator.
 * This comparison operator allows for comparison of less than of  C based
 * object identifier structure and a dotted string.
 *
 * @param lhs          - C object identifier value.
 * @param dotted_oid_string - String containing OID value to compare.
 * @return             - True if values are equal.
 */
EXTERNRT int operator<(const ASN1OBJID &lhs, const char *dotted_oid_string);

/**
 * Overloaded less than < operator.
 * This comparison operator allows for comparison of less than of  C++ based
 * object identifier structure and a dotted string.
 *
 * @param lhs          - C++ object identifier value.
 * @param dotted_oid_string - String containing OID value to compare.
 * @return             - True if values are equal.
 */
EXTERNRT int operator<(const ASN1TObjId &lhs, const char *dotted_oid_string);

/**
 * Overloaded less than <= operator.
 * This comparison operator allows for comparison of less than of  C++ based
 * object identifier structure and a dotted string.
 *
 * @param lhs          - C++ object identifier value.
 * @param rhs          - C++ object identifier value
 * @return             - True if values are equal.
 */
EXTERNRT int operator<=(const ASN1TObjId &lhs, const ASN1TObjId &rhs);

/**
 * Overloaded less than <= operator.
 * This comparison operator allows for comparison of less than of  C based
 * object identifier structure and a dotted string.
 *
 * @param lhs          - C object identifier value.
 * @param rhs          - C object identifier value
 * @return             - True if values are equal.
 */
EXTERNRT int operator<=(const ASN1OBJID &lhs, const ASN1OBJID &rhs);

/**
 * Overloaded less than <= operator.
 * This comparison operator allows for comparison of less than of a C++ based
 * object identifier structure and a dotted string.
 *
 * @param lhs          - C++ object identifier value.
 * @param dotted_oid_string - String containing OID value to compare.
 * @return             - True if values are equal.
 */

EXTERNRT int operator<=(const ASN1TObjId &lhs, const char *dotted_oid_string);

/**
 * Overloaded less than <= operator.
 * This comparison operator allows for comparison of less than or equal of  
 * a C based object identifier structure and a dotted string.
 *
 * @param lhs          - C object identifier value.
 * @param dotted_oid_string - String containing OID value to compare.
 * @return             - True if values are equal.
 */
EXTERNRT int operator<=(const ASN1OBJID &lhs, const char *dotted_oid_string);


/**
 * Overloaded greater than > operator.
 * This comparison operator allows for comparison of greater than of C++ based
 * object identifier structures
 *
 * @param lhs          - C++ object identifier value.
 * @param rhs          - C++ object identifier value.
 * @return             - True if values are equal.
 */
EXTERNRT int operator>(const ASN1TObjId &lhs, const ASN1TObjId &rhs);

/**
 * Overloaded greater than > operator.
 * This comparison operator allows for comparison of greater than of a C++ 
 * based object identifier structure and a dotted string.
 *
 * @param lhs          - C++ object identifier value.
 * @param dotted_oid_string - String containing OID value to compare.
 * @return             - True if values are equal.
 */
EXTERNRT int operator>(const ASN1TObjId &lhs, const char *dotted_oid_string);

/**
 * Overloaded greater than > operator.
 * This comparison operator allows for comparison of greater than of C based
 * object identifier structures.
 *
 * @param lhs          - C object identifier value.
 * @param rhs          - C object identifier value.
 * @return             - True if values are equal.
 */
EXTERNRT int operator>(const ASN1OBJID &lhs, const ASN1OBJID &rhs);

/**
 * Overloaded greater than > operator.
 * This comparison operator allows for comparison of greater than of a C based
 * object identifier structure and a dotted string.
 *
 * @param lhs          - C object identifier value.
 * @param dotted_oid_string - String containing OID value to compare.
 * @return             - True if values are equal.
 */
EXTERNRT int operator>(const ASN1OBJID &lhs, const char *dotted_oid_string);

/**
 * Overloaded greater than equal >= operator.
 * This comparison operator allows for comparison of greater than or equal 
 * of C++ based object identifier structures.
 *
 * @param lhs          - C++ object identifier value.
 * @param rhs          - C++ object identifier value.
 * @return             - True if values are equal.
 */
EXTERNRT int operator>=(const ASN1TObjId &lhs, const ASN1TObjId &rhs);

/**
 * Overloaded greater than equal >= operator.
 * This comparison operator allows for comparison of greater than or equal 
 * of a C++ based object identifier structure and a dotted string.
 *
 * @param lhs          - C++ object identifier value.
 * @param dotted_oid_string - String containing OID value to compare.
 * @return             - True if values are equal.
 */
EXTERNRT int operator>=(const ASN1TObjId &lhs, const char *dotted_oid_string);

/**
 * Overloaded greater than equal >= operator.
 * This comparison operator allows for comparison of greater than or equal 
 * of C based object identifier structures.
 *
 * @param lhs          - C object identifier value.
 * @param rhs          - C object identifier value.
 * @return             - True if values are equal.
 */
EXTERNRT int operator>=(const ASN1OBJID &lhs, const ASN1OBJID &rhs);


/**
 * Overloaded greater than equal >= operator.
 * This comparison operator allows for comparison of greater than or equal 
 * of a C based object identifier structure and a dotted string.
 *
 * @param lhs          - C object identifier value.
 * @param dotted_oid_string - String containing OID value to compare.
 * @return             - True if values are equal.
 */
EXTERNRT int operator>=(const ASN1OBJID &lhs, const char *dotted_oid_string);

/**
 * Overloaded append + operator.
 * This operator allows two Object Identifier values to be concatanated.
 *
 * @param lhs          - C++ object identifier value.
 * @param rhs          - C++ object identifier value.
 * 
 */
EXTERNRT ASN1TObjId operator +(const ASN1TObjId &lhs, const ASN1TObjId &rhs);

#endif

/**
 * @}
 */
