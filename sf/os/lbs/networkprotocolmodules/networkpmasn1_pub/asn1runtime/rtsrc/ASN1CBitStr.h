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
@file
@internalTechnology
*/

#ifndef _ASN1CBITSTR_H_
#define _ASN1CBITSTR_H_

#include "rtsrc/asn1CppTypes.h"
/**
 * @file rtsrc/ASN1CBitStr.h 
 * Bit string control class definitions.
 */
/**
 * @addtogroup asn1ctrl 
 * @{
 */
//
//
// ASN1CBitStr
// 
// Bit string implementation. This class provides all functionalty
// necessary for bit strings. 
//
// Author Artem Bolgar.
// version 1.52   21 Feb, 2003
//
#ifndef _NO_UTILS_CLASSES

/**
 * ASN.1 bit string control class. 
 * The ASN1CBitStr class is derived from the ASN1CType base class. It is used
 * as the base class for generated control classes for the ASN.1 BIT STRING
 * type. This class provides utility methods for operating on the bit string
 * referenced by the generated class. This class can also be used inline to
 * operate on bits within generated BIT STRING elements in a SEQUENCE, SET, or
 * CHOICE construct.
 */
class EXTRTCLASS ASN1CBitStr : public ASN1CType {
 private:
   OSUINT32    _numbits; // placeholder
   OSOCTET*    _units;   // placeholder
 protected:
   OSOCTET**   mpUnits;
   OSUINT32    mMaxNumBits;
   OSUINT32*   mpNumBits; 
   int         mUnitsUsed;
   int         mUnitsAllocated;
   OSBOOL      mDynAlloc;
 private:
   EXTRTMETHOD OSOCTET* allocateMemory(int sz);

   EXTRTMETHOD OSOCTET* reallocateMemory(OSOCTET* old, int oldBufSz, int newBufSz);

   EXTRTMETHOD void freeMemory(OSOCTET* mem);
   /*
   * Set the field mUnitsUsed with the logical size in units of the bit
   * set.  WARNING:This function assumes that the number of units actually
   * in use is less than or equal to the current value of mUnitsUsed!
   */
   EXTRTMETHOD void recalculateUnitsUsed();

   /*
   * Ensures that the bit string can hold enough units.
   * param	unitsRequired the minimum acceptable number of units.
   */
   EXTRTMETHOD int checkCapacity(int unitsRequired);

   
   /* 
   * Returns the unit of this bitset at index j as if this bit string had an
   * infinite amount of storage.
   */
   EXTRTMETHOD OSOCTET getBits(int j);

   EXTRTMETHOD void privateInit (OSUINT32 nbits);
   EXTRTMETHOD void privateInit (OSOCTET* bitStr, OSUINT32 maxNumbits_);
   EXTRTMETHOD void privateInit (ASN1TDynBitStr& bitStr);
 protected:
   /*
   * Creates an empty bit string. The string should be additionaly initialized 
   * by call to 'init' method.
   */
   EXTRTMETHOD ASN1CBitStr (OSRTMessageBufferIF& msgBuf);

   /*
   * Default protected ctor. mpContext should be set and the string should be 
   * additionaly initialized by call to 'init' method.
   */
   EXTRTMETHOD ASN1CBitStr();

   /*
    * This constructor creates an empty bit string.
    *
    * @param ctxt - Reference to reference-counted context object.
    */
   EXTRTMETHOD ASN1CBitStr (OSRTContext& ctxt);

   /*
    * This constructor initializes the internal bit string to the 
    * given value.
    *
    * @param pBits - Pointer to binary bit string data.
    * @param numbits - Reference to counter to hold number of bits.
    * @param maxNumbits - Maximum size (in bits) of this bit string.
    */
   EXTRTMETHOD ASN1CBitStr (OSOCTET* pBits, OSUINT32& numbits, OSUINT32 maxNumbits);

   /*
    * This constructor initializes the internal bit string to the 
    * given value.
    *
    * @param bitstr - Reference to a bit string variable.
    */
   EXTRTMETHOD ASN1CBitStr (ASN1TDynBitStr& bitStr);

   EXTRTMETHOD void init (OSOCTET* pBits, OSUINT32& numbits, OSUINT32 maxNumbits);
   EXTRTMETHOD void init (ASN1TDynBitStr& bitStr);

 public:
   /**
    * This constructor creates an empty bit string. If the \c nbits argument is
    * zero, the bit string is set to be dynamic; otherwise, the capacity is set
    * to \c nbits.
    *
    * @param msgbuf       - ASN.1 message buffer or stream object.
    * @param nbits        - Number of bits this bit string can contain (zero if
    *                       unbounded.
    */
   EXTRTMETHOD ASN1CBitStr (OSRTMessageBufferIF& msgbuf, OSUINT32 nbits);
             
   /**
    * This constructor creates a bit string from an array of bits. The
    * constructor does not copy the bit string data, it just references the
    * given data variables. All operations on the bit string cause the
    * referenced items to be updated directly.
    *
    * @param msgbuf       - ASN.1 message buffer or stream object.
    * @param bitStr       - Pointer to static byte array
    * @param numbits      - Reference to length of bit string (in bits)
    * @param maxNumbits_  - sets maximum length in bits
    */
   EXTRTMETHOD ASN1CBitStr (OSRTMessageBufferIF& msgbuf, OSOCTET* bitStr, 
                OSUINT32& numbits, OSUINT32 maxNumbits_); 

   /*
   * Creates bits string from ASN1TDynBitStr. Ctor does not copy bytes,
   * just assign array (data member of ASN1TDynBitStr) to units.
   * This string will be extendable.
   */
   EXTRTMETHOD ASN1CBitStr(OSRTMessageBufferIF& msgBuf, ASN1TDynBitStr& bitStr);

   /*
   * Creates empty bit string. If nbits == 0 then bit string is dynamic; 
   * else capacity will be fixed to nbits.
   */
   EXTRTMETHOD ASN1CBitStr(OSRTContext& ctxt, OSUINT32 nbits);
             
   /*
   * Creates bits string from array of bits. Ctor does not copy bytes,
   * just assign passed array to units.
   * Param bitStr - pointer to static byte array
   * Param numbits - reference to length of bit string (in bits);
   * Param maxNumbits_  - sets maximum length in bits
   */
   EXTRTMETHOD ASN1CBitStr(OSRTContext& ctxt, OSOCTET* bitStr, 
      OSUINT32& octsNumbits, OSUINT32 maxNumbits_); 

   /*
   * Creates bits string from ASN1TDynBitStr. Ctor does not copy bytes,
   * just assign array (data member of ASN1TDynBitStr) to units.
   * This string will be extendable.
   */
   EXTRTMETHOD ASN1CBitStr(OSRTContext& ctxt, ASN1TDynBitStr& bitStr);

   /*
   * Copy constructor.
   */
   EXTRTMETHOD ASN1CBitStr(const ASN1CBitStr& bitStr);

   /*
   * Copy constructor, but allows to specify 'extendable' parameter
   */
   EXTRTMETHOD ASN1CBitStr(const ASN1CBitStr& bitStr, OSBOOL extendable);


   EXTRTMETHOD ~ASN1CBitStr();
  /**
   * This version of the set method sets the given bit in the target string.
   *
   * @param bitIndex     Relative index of the bit to set in the string. The
   *                       bit idex 0 refers to the MS bit (bit 8) in the first
   *                       octet. The index values then progress from the left
   *                       to right (MS to LS).
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   /*
   * Sets the bit at the specified index to 1.
   *
   * Returns: 0 - if succeed, or 
   *          RTERR_OUTOFBND - if 'bitIndex' is out of bounds, or
   *          other error codes (see asn1type.h).
   */
   EXTRTMETHOD int set(OSUINT32 bitIndex);

  /**
   * This version of teh set method sets the bits from the specified fromIndex
   * (inclusive) to the specified toIndex (exclusive) to one.
   *
   * @param fromIndex    Relative start index (inclusive) of bits in the
   *                       string. The bit index 0 refers to the MS bit (bit 8)
   *                       in the first octet. The index values then progress
   *                       from the left to right (MS to LS).
   * @param toIndex      Relative end index (exclusive) of bits in the string.
   *                       The bit index 0 refers to the MS bit (bit 8) in the
   *                       first octet. The index values then progress from the
   *                       left to right (MS to LS).
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   /*
   * Sets the bits from the specified fromIndex(inclusive) to the
   * specified toIndex(exclusive) to 1.
   *
   * Returns: 0 - if succeed, or 
   *          RTERR_OUTOFBND - if one of indexes is out of bounds, or
   *          RTERR_RANGERR - if fromIndex > toIndex, or
   *          other error codes (see asn1type.h).
   */
   EXTRTMETHOD int set(OSUINT32 fromIndex, OSUINT32 toIndex);

   /**
    * Changes the bit at the specified index to the specified value.
    *
    * @param bitIndex     Relative index of bit to set in string. Bit index 0
    *                       refers to the MS bit (bit 8) in the first octet.
    *                       The index values then progress from left to right
    *                       (MS to LS bits).
    * @param value        Boolean value to which the bit is to be set.
    * @return             Completion status of operation: 0 - if succeed
    *                       - 0 (0) = success
    *                       - negative return value is error.
   */          
   /*Returns (details): RTERR_OUTOFBND - if 'bitIndex' is out of bounds, or
   *          other error codes (see asn1type.h).
   */
   inline int change(OSUINT32 bitIndex, OSBOOL value) {
      if(value)
         return set(bitIndex);
      else
         return clear(bitIndex);
   }

  /** 
   * This version of the clear method sets the given bit in the target string
   * to zero.
   *
   * @param bitIndex     Relative index of bit in string. Bit index 0 refers to
   *                       the MS bit (bit 8) in the first octet. The index
   *                       values then progress from left to right (MS to LS
   *                       bits).
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   /*
   * Sets the bit specified by the index to 0.
   *
   * Returns: 0 - if succeed, or 
   *          RTERR_OUTOFBND - if 'bitIndex' is out of bounds, or
   *          other error codes (see asn1type.h).
   */
   EXTRTMETHOD int clear(OSUINT32 bitIndex);

  /**
   * This version of the clear method sets the bits from the specified
   * fromIndex (inclusive) to the specified toIndex (exclusive) to zero.
   *
   * @param fromIndex    Relative start index (inclusive) of bits in string.
   *                       Bit index 0 refers to the MS bit (bit 8) in the
   *                       first octet. The index values then progress from
   *                       left to right (MS to LS bits).
   * @param toIndex      Relative end index (exclusive) of bits in string. Bit
   *                       index 0 refers to the MS bit (bit 8) in the first
   *                       octet. The index values then progress from left to
   *                       right (MS to LS bits).
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   /*
   * Sets the bits from the specified fromIndex(inclusive) to the
   * specified toIndex(exclusive) to 0.
   *
   * Returns: 0 - if succeed, or 
   *          RTERR_OUTOFBND - if one of indexes is out of bounds, or
   *          RTERR_RANGERR - if fromIndex > toIndex, or
   *          other error codes (see asn1type.h).
   */
   EXTRTMETHOD int clear(OSUINT32 fromIndex, OSUINT32 toIndex);

  /**
   * This version of the clear method sets all bits in the bit string to zero.
   *
   * @param             - none
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   /*
   * Sets all of the bits in this bit string to 0.
   */
   EXTRTMETHOD void clear();

  /**
   * This version of the invert method inverts the given bit in the target
   * string.
   *
   * If the bit in the bit string is a zero, it will be set to 1; if the bit is
   * a one, it will be set to 0.
   *
   * @param bitIndex     Relative index of bit in string. Bit index 0 refers to
   *                       the MS bit (bit 8) in the first octet. The index
   *                       values then progress from left to right (MS to LS
   *                       bits).
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   /*
   * Sets the bit at the specified index to to the complement of its
   * current value.
   *
   * Returns: 0 - if succeed, or 
   *          RTERR_OUTOFBND - if 'bitIndex' is out of bounds, or
   *          other error codes (see asn1type.h).
   */
   EXTRTMETHOD int invert(OSUINT32 bitIndex);

   /**
    * This version inverts the bits from the specified fromIndex (inclusive) to
    * the specified toIndex (exclusive).
    *
    * If the bit in the bit string is a zero, it will be set to 1; if the bit
    * is a one, it will be set to 0.
    *
    * @param fromIndex    Relative start index (inclusive) of bits in string.
    *                       Bit index 0 refers to the MS bit (bit 8) in the
    *                       first octet. The index values then progress from
    *                       left to right (MS to LS bits).
    * @param toIndex      Relative end index (exclusive) of bits in string. Bit
    *                       index 0 refers to the MS bit (bit 8) in the first
    *                       octet. The index values then progress from left to
    *                       right (MS to LS bits).
    * @return             Completion status of operation:
    *                       - 0 (0) = success,
    *                       - negative return value is error.
   */
   /*
   * Sets each bit from the specified fromIndex(inclusive) to the
   * specified toIndex(exclusive) to the complement of its current
   * value.
   *
   * Returns: 0 - if succeed, or 
   *          RTERR_OUTOFBND - if one of indexes is out of bounds, or
   *          RTERR_RANGERR - if fromIndex > toIndex, or
   *          other error codes (see asn1type.h).
   */
   EXTRTMETHOD int invert(OSUINT32 fromIndex, OSUINT32 toIndex);

  /**
   * This method returns the value of the bit with the specified index.
   *
   * @param bitIndex     Relative index of bit in string. Bit index 0 refers to
   *                       the MS bit (bit 8) in the first octet. The index
   *                       values then progress from left to right (MS to LS
   *                       bits).
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   /*
   * Returns the value of the bit with the specified index.
   */
   EXTRTMETHOD OSBOOL get(OSUINT32 bitIndex);

  /**
   * This method is the same as ASN1CBitStr::get.
   *
   * @see get           (OSUINT32 bitIndex)
   */
   /*
   * Returns TRUE if the value of the bit with the specified index is set to 1.
   */
   inline OSBOOL isSet(OSUINT32 bitIndex) { return get(bitIndex); }

  /**
   * This method returns TRUE if this bit string contains no bits that are set
   * to 1.
   *
   * @param             - none
   * @return             TRUE, if the bit string contains no bits that are set
   *                       to 1.
   */
   /*
   * Returns true if this bit string contains no bits that are set
   * to 1.
   */
   inline OSBOOL isEmpty() { return (mUnitsUsed == 0); }

  /**
   * This method returns the number of bytes of space actually in use by this
   * bit string to represent bit values.
   *
   * @param             - none
   * @return             Number of bytes of space actually in use by this bit
   *                       string to represent bit values.
   */
   /*
   * Returns the number of bytes of space actually in use by this 
   * bit string to represent bit values. 
   */
   EXTRTMETHOD int size() const;

  /**
   * This method Calculates the "logical size" of the bith string.
   *
   * The "logical size" is caluculated by noting the index of the highest set
   * bit in the bit string plus one. Zero will be returned if the bit string
   * contains no set bits. The highest bit in the bit string is the LS bit in
   * the last octet set to 1.
   *
   * @param             - none
   * @return             Returns the "logical size" of this bit string.
   */
   /*
   * Returns the "logical size" of this bit string: the index of
   * the highest set bit in the bit string plus one. Returns zero
   * if the bit string contains no set bits.
   */
   EXTRTMETHOD OSUINT32 length() const;

  /**
   * This method calculates the cardinality of the target bit string.
   *
   * Cardinality of the bit string is the number of bits set to 1.
   *
   * @param             - none
   * @return             The number of bytes of space actually in use by this
   *                       bit string to represent the bit values.
   */
   /*
   * Returns the number of bits set to 1 in this
   * bit string.
   */
   EXTRTMETHOD int cardinality() const;

  /**
   * This method copies the bit string to the given buffer.
   *
   * @param pBuf         Pointer to the destination buffer where bytes will be
   *                       copied.
   * @param bufSz        Size of the destination buffer. If the size of 
   *                       the buffer is not large enough to receive the 
   *                       entire bit string, a negative status value 
   *                       (RTERR_BUFOVFLOW) will be returned.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   /*
   * Copies bit string to buffer (pBuf).
   */
   EXTRTMETHOD int getBytes(OSOCTET* pBuf, int bufSz);

  /**
   * This version of the get method copies the bit string composed of bits from
   * this bit string from the specified fromIndex (inclusive) to the specified
   * toIndex (exclusive) into the given buffer.
   *
   * @param fromIndex    Relative start index (inclusive) of bits in string.
   *                       Bit index 0 refers to the MS bit (bit 8) in the
   *                       first octet. The index values then progress from
   *                       left to right (MS to LS bits).
   * @param toIndex      Relative end index (exclusive) of bits in string. Bit
   *                       index 0 refers to the MS bit (bit 8) in the first
   *                       octet. The index values then progress from left to
   *                       right (MS to LS bits).
   * @param pBuf         Pointer to destination buffer where bytes will be
   *                       copied.
   * @param bufSz        Size of the destination buffer. If the size of 
   *                       the buffer is not large enough to receive the 
   *                       entire bit string, a negative status value 
   *                       (RTERR_BUFOVFLOW) will be returned.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - RTERR_OUTOFBND index value is out of bounds
   *                       - RTERR_RANGERR fromIndex > toIndex
   *                       - other error codes (see asn1type.h).
   */
   EXTRTMETHOD int get(OSUINT32 fromIndex, OSUINT32 toIndex, OSOCTET* pBuf, int bufSz);

  /**
   * This method provides the logical AND of the target bit string with the
   * argument bit string.
   *
   * @param pOctstr      A pointer to octets of another bit string for
   *                       performing logical operation.
   * @param octsNumbits  A number of bits in arguent bit string.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   /*
   * Performs a logical AND of this target bit set with the 
   * argument bit set.
   *
   * Returns: 0 - if succeed, or 
   *          ASN_E_INVLEN - if 'octsNumbits' is negative, or
   *          RTERR_INVPARAM - if pOctstr is the same bit string as this or null, or
   *          other error codes (see asn1type.h).
   */
   EXTRTMETHOD int doAnd(const OSOCTET* pOctstr, OSUINT32 octsNumbits);
   
  /**
   * This method performs a logical AND of the target bit string with the
   * argument bit string.
   *
   * @param bitStr       A reference t another bit string represented by
   *                       ASN1TDynBitStr type for performing logical
   *                       operation.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   inline int doAnd(const ASN1TDynBitStr& bitStr) {
      return doAnd(bitStr.data, bitStr.numbits);
   }
 
  /**  
   * This method performs a logical AND of the target bit string with the
   * argument bit string.
   *
   * @param bitStr       A reference to another bit string represented by
   *                       ASN1CBitStr type for performing logical operation.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   inline int doAnd(const ASN1CBitStr& bitStr) {
      return doAnd(*bitStr.mpUnits, bitStr.length());
   }


  /**
   * This method preforms a logical OR of the target bit string with the
   * argument bit string.
   *
   * @param pOctstr      A pointer to octets of another bit string for
   *                       performing logical operation.
   * @param octsNumbits  A number of bits in arguent bit string.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   /*
   * Performs a logical OR of this target bit set with the 
   * argument bit set.
   *
   * Returns: 0 - if succeed, or 
   *          ASN_E_INVLEN - if 'octsNumbits' is negative, or
   *          RTERR_INVPARAM - if pOctstr is the same bit string as this or null, or
   *          other error codes (see asn1type.h).
   */
   EXTRTMETHOD int doOr(const OSOCTET* pOctstr, OSUINT32 octsNumbits);
   
  /** 
   * This method performs a logical OR of the target bit string with the
   * argument bit string.
   *
   * @param bitStr       A reference t another bit string represented by
   *                       ASN1TDynBitStr type for performing logical
   *                       operation.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   inline int doOr(const ASN1TDynBitStr& bitStr) {
      return doOr(bitStr.data, bitStr.numbits);
   }
   
  /**  
   * This method performs a logical OR of the target bit string with the
   * argument bit string.
   *
   * @param bitStr       A reference to another bit string represented by
   *                       ASN1CBitStr type for performing logical operation.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   inline int doOr(const ASN1CBitStr& bitStr) {
      return doOr(*bitStr.mpUnits, bitStr.length());
   }

   /**
    * This method provides the logical XOR of the target bit string with the
    * argument bit string.
    *
    * @param pOctstr      A pointer to octets of another bit string for
    *                       performing logical operation.
    * @param octsNumbits  A number of bits in arguent bit string.
    * @return             Completion status of operation:
    *                       - 0 (0) = success,
    *                       - negative return value is error.
   */
   /*
   * Performs a logical XOR of this target bit set with the 
   * argument bit set.
   *
   * Returns: 0 - if succeed, or 
   *          ASN_E_INVLEN - if 'octsNumbits' is negative, or
   *          RTERR_INVPARAM - if pOctstr is null, or
   *          other error codes (see asn1type.h).
   */
   EXTRTMETHOD int doXor(const OSOCTET* pOctstr, OSUINT32 octsNumbits);
 
  /** 
   * This method performs a logical XOR of the target bit string with the
   * argument bit string.
   *
   * @param bitStr       A reference t another bit string represented by
   *                       ASN1TDynBitStr type for performing logical
   *                       operation.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */  
   inline int doXor(const ASN1TDynBitStr& bitStr) {
      return doXor(bitStr.data, bitStr.numbits);
   }

  /**  
   * This method performs a logical OR of the target bit string with the
   * argument bit string.
   *
   * @param bitStr       A reference to another bit string represented by
   *                       ASN1CBitStr type for performing logical operation.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   inline int doXor(const ASN1CBitStr& bitStr) {
      return doXor(*bitStr.mpUnits, bitStr.length());
   }

  /**
   * This method performs a logical ANDNOT of the target bit string with the
   * argument bit string.
   *
   * Logical ANDNOT clars all of the bits in this bit string whose
   * corresponding bit is set in the specified bit string.
   *
   * @param pOctstr      A pointer to octets of another bit string for
   *                       performing logical operation.
   * @param octsNumbits  A number of bits in arguent bit string.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   /*
   * Clears all of the bits in this bit string whose corresponding
   * bit is set in the specified bit string.
   *
   * Returns: 0 - if succeed, or 
   *          ASN_E_INVLEN - if 'octsNumbits' is negative, or
   *          RTERR_INVPARAM - if pOctstr is null, or
   *          other error codes (see asn1type.h).
   */
   EXTRTMETHOD int doAndNot(const OSOCTET* pOctstr, OSUINT32 octsNumbits);
   
  /**
   * This method performs a logical ANDNOT of the target bit string with the
   * argument bit string.
   *
   * Logical ANDNOT clears all of the bits in this bit string whose
   * corresponding bit is set in the specified bit string.
   *
   * @param bitStr       A reference t another bit string represented by
   *                       ASN1TDynBitStr type for performing logical
   *                       operation.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   inline int doAndNot(const ASN1TDynBitStr& bitStr) {
      return doAndNot(bitStr.data, bitStr.numbits);
   }

  /**
   * This method performs a logical ANDNOT of the target bit string with the
   * argument bit string.
   *
   * Logical ANDNOT clears all of the bits in this bit string whose
   * corresponding bit is set in the specified bit string.
   *
   * @param bitStr       A reference to another bit string represented by
   *                       ASN1CBitStr type for performing logical operation.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */   
   inline int doAndNot(const ASN1CBitStr& bitStr) {
      return doAndNot(*bitStr.mpUnits, bitStr.length());
   }

  /**
   * This method shifts all bits to the left by the number of specified in the
   * shift operand.
   *
   * If the bit string can dynamically grow, then the length of the bit string
   * will be decreased by shift bits. Otherwise, bits that are shifted into the
   * bitstring are filled with zeros from the right. Most left bits are lost.
   *
   * @param shift        Number of bits to be shifted.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   /*
   * Shifts all bits to the left by 'shift' bits.
   */
   EXTRTMETHOD int shiftLeft(OSUINT32 shift);

  /**
   * This method shifts all bits to the right by the number of specified in the
   * shift operand.
   *
   * If the bit string can dynamically grow, then the length of the bit string
   * will be decreased by shift bits. Otherwise, bits that are shifted into the
   * bitstring are filled with zeros from the left. Most right bits are lost.
   *
   * @param shift        Number of bits to be shifted.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   /*
   * Shifts all bits to the right by 'shift' bits.
   */
   EXTRTMETHOD int shiftRight(OSUINT32 shift);

  /**
   * This method returns the number of unused bits in the last octet.
   *
   * @param             - none
   * @return             Number of bits in teh last octet. It equals to
   *                       length() % 8.
   */
   /* 
   * Returns number of unused bits in last unit. 
   */
   EXTRTMETHOD int unusedBitsInLastUnit();

  /**
   * This method returns a filled ANSDITDynBitStr variable.
   *
   * Memory is not allocated when calling this method; only a pointer is
   * assigned. Thus, the ASN1TDynBitStr variable is only valid while this
   * ASN1CBitStr is in scope.
   *
   * @param             - none
   * @return             Filled ASN1TDynBitStr.
   */
   /*
   * Returns filled ASN1TDynBitStr. Memory for data is not
   * reallocated, thus, this ASN1TDynBitStr will be eligible while
   * this ASN1CBitStr is in scope.
   */
   EXTRTMETHOD operator ASN1TDynBitStr();
   
  /**
   * This method returns a pointer to the filled ANSDITDynBitStr variable.
   *
   * Memory for the ASN1DynBitStr variable is alloced using memory memAlloc and
   * bits are copied into it.
   *
   * @param             - none
   * @return             Pointer to a filled ASN1TDynBitStr.
   */
   /*
   * Returns pointer to filled ASN1TDynBitStr. In this method,
   * memory is allocated, and bits are copied to it.
   */
   EXTRTMETHOD operator ASN1TDynBitStr*();
} ;
#else
typedef class _ASN1CBitStr : public ASN1CType {
 public:
   _ASN1CBitStr (OSRTMessageBufferIF& msgBuf, OSUINT32 nbits) :
      ASN1CType (msgBuf) {}

   _ASN1CBitStr (OSRTMessageBufferIF& msgBuf, OSOCTET* bitStr, 
      OSUINT32& octsNumbits, OSUINT32 maxNumbits_) :
      ASN1CType (msgBuf) {}

   _ASN1CBitStr (OSRTMessageBufferIF& msgBuf, ASN1TDynBitStr& bitStr) :
      ASN1CType (msgBuf) {}
} ASN1CBitStr;
#endif // _NO_UTILS_CLASSES
#endif // _ASN1CBITSTR_H_
/** 
 * @}asn1cb 
 */
