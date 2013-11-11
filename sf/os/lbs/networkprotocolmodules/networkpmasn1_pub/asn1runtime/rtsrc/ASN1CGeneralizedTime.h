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

#ifndef __ASN1CGENTIME_H__
#define __ASN1CGENTIME_H__

#include "rtsrc/ASN1CTime.h"

/**
 * @file rtsrc/ASN1CGeneralizedTime.h 
 * GeneralizedTime control class definition.
 */
/**
 * @addtogroup asn1ctrl 
 * @{
 */
/**
 * @addtogroup asn1ctime 
 * @{
 */
//
//
// ASN1CGeneralizedTime
// 
// ISO 8601 time and date for ITU-T X.680. 
//
// Author Artem Bolgar.
// version 3.00   03 Sep, 2004 (refactored to use ASN1TGeneralizedTime)
//
// version 2.13   11 Jul, 2003 (2038 year problem fixed + some other fixes)
// version 1.11   03 Aug, 2002
//
#ifndef _NO_UTILS_CLASSES

/**
 * ASN.1 GeneralizedTime control class. 
 * The ASN1CGeneralizedTime class is derived from the ASN1CTime base class. It
 * is used as the base class for generated control classes for the ASN.1
 * Generalized Time ([UNIVERSAL 24] IMPLICIT VisibleString) type. This class
 * provides utility methods for operating on the time information referenced by
 * the generated class. This class can also be used inline to operate on the
 * times within generated time string elements in a SEQUENCE, SET, or CHOICE
 * construct. The time string generally is encoded according to ISO 8601 format
 * with some exceptions (see X.680).
 */
class EXTRTCLASS ASN1CGeneralizedTime : public ASN1CTime {
 protected:
   ASN1TGeneralizedTime timeObj;

   virtual ASN1TTime& getTimeObj () { return timeObj; }
   virtual const ASN1TTime& getTimeObj () const { return timeObj; }

   EXTRTMETHOD ASN1CGeneralizedTime(char*& buf, int bufSize, OSBOOL useDerRules = FALSE);

   EXTRTMETHOD ASN1CGeneralizedTime(ASN1GeneralizedTime& buf,
      OSBOOL useDerRules = FALSE); 
 public:
 
/**
 * This constructor creates a time string from a buffer. It does not deep-copy
 * the data, it just assigns the passed array to an internal reference
 * variable. The object will then directly operate on the given data variable.
 *
 * @param msgBuf       Reference to an OSRTMessage buffer derived object (for
 *                       example, an ASN1BEREncodeBuffer).
 * @param buf          A reference pointer to the time string buffer.
 * @param bufSize      The size of the passed buffer, in bytes.
 * @param useDerRules  An OSBOOL value.
 */
   EXTRTMETHOD ASN1CGeneralizedTime(OSRTMessageBufferIF& msgBuf, char*& buf, int bufSize, 
      OSBOOL useDerRules = FALSE); 

/**
 * This constructor creates a time string using the ASN1GeneralizedTime
 * argument. The constructor does not deep-copy the variable, it assigns a
 * reference to it to an internal variable. The object will then directly
 * operate on the given data variable. This form of the constructor is used
 * with a complier-generated time string variable.
 *
 * @param msgBuf       Reference to an OSRTMessage buffer derived object (for
 *                       example, an ASN1BEREncodeBuffer).
 * @param buf          A reference pointer to the time string buffer.
 * @param useDerRules  An OSBOOL value.
 */
   EXTRTMETHOD ASN1CGeneralizedTime(OSRTMessageBufferIF& msgBuf, ASN1GeneralizedTime& buf,
      OSBOOL useDerRules = FALSE); 

   EXTRTMETHOD ASN1CGeneralizedTime(OSRTContext& ctxt, char*& buf, int bufSize, 
      OSBOOL useDerRules = FALSE); 

   EXTRTMETHOD ASN1CGeneralizedTime(OSRTContext& ctxt, ASN1GeneralizedTime& buf,
      OSBOOL useDerRules = FALSE); 

   // copy ctor

   ASN1CGeneralizedTime (const ASN1CGeneralizedTime& original) :
      ASN1CTime (original) {}

  /**
   * This method returns the centry part (first two digits) of the year
   * component of the time value.
   *
   * @param             - none
   * @return             Century part (first two digits) of the year component
   *                       is returned if the operation is sucessful. If the
   *                       operation fails, one of the negative status codes is
   *                       returned.
   */
   /* 
   * Gets century part of the year (first two digits of the year). 
   * Returns: first 2 digits of the year - if success;
   *          error code - if parsed value is invalid.
   */
   EXTRTMETHOD int getCentury();

  /**
   * This method sets the centry part (first two digits) of the year component
   * of the time value.
   *
   * @param century      Century part (first two digits) of the year component.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   /*
   * Sets century part of the year (first 2 digits).
   * Returns 0, if succeed, or error code, if error.
   */
   EXTRTMETHOD int setCentury(int century);

  /**
   * This converts the value of the C built-in type time_t to a time string.
   *
   * The value is the number of seconds from January 1, 1970. Note that the
   * action of this method may differ for different inherited ASN1CTime
   * Classes.
   *
   * @param time         The time value, expressed as a number of seconds from
   *                       January 1, 1970.
   * @param diffTime     TRUE means the difference between local time and UTC
   *                       time will be calculated; in other case, only local
   *                       time will be stored.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   EXTRTMETHOD int setTime(time_t time, OSBOOL diffTime);

   const ASN1CGeneralizedTime& operator = (const ASN1CGeneralizedTime& tm) {
      return (const ASN1CGeneralizedTime&) ASN1CTime::operator = (tm);
   }
 protected:
   /*
   * Compiles new time string accoring X.680 (clause 41) and ISO 8601.
   * Returns 0, if succeed, or error code, if error.
   */
   EXTRTMETHOD int compileString();
} ;
#else
typedef class _ASN1CGeneralizedTime : public _ASN1CTime {
 protected:
   _ASN1CGeneralizedTime(char*& buf, int bufSize, OSBOOL useDerRules) {}
   _ASN1CGeneralizedTime(ASN1VisibleString& buf,  OSBOOL useDerRules) {}
 public:
   _ASN1CGeneralizedTime (OSRTMessageBufferIF& msgBuf, char*& buf, int sz,
      OSBOOL useDerRules = FALSE) :
      _ASN1CTime (msgBuf, buf, sz, useDerRules) {} 
   _ASN1CGeneralizedTime (OSRTMessageBufferIF& msgBuf, ASN1VisibleString& buf,
      OSBOOL useDerRules = FALSE) :
      _ASN1CTime (msgBuf, buf, useDerRules) {} 
   _ASN1CGeneralizedTime(OSRTContext& ctxt, char*& buf, int bufSize, 
      OSBOOL useDerRules) : _ASN1CTime (ctxt, buf, bufSize, useDerRules) {}  
   _ASN1CGeneralizedTime(OSRTContext& ctxt, ASN1VisibleString& buf, 
      OSBOOL useDerRules) : _ASN1CTime (ctxt, buf, bufSize, useDerRules) {}  
} ASN1CGeneralizedTime;
#endif // _NO_UTILS_CLASSES

/**
 * @}ASN1CGeneralizedTime
 */
/**
 * @} asn1ctrl
 */

#endif //__ASN1CGENTIME_H__
