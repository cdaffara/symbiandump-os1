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

#ifndef __ASN1CUTCTIME_H__
#define __ASN1CUTCTIME_H__

#include "rtsrc/ASN1CTime.h"

//
//
// ASN1CUTCTime
// 
// ISO 8601 time and date for ITU-T X.680. 
//
// Author Artem Bolgar.
// version 3.00   03 Sep, 2004 (refactored to use ASN1TUTCTime)
//
// version 2.13   11 Jul, 2003 (2038 year problem fixed + some other fixes)
// version 1.11   3 Aug, 2002
//

/**
 * @file rtsrc/ASN1CUTCTime.h
 * ASN1CUTCTime control class definition.
 */
/**
 * @addtogroup asn1ctrl 
 * @{
 */
/**
 * @addtogroup asn1ctime 
 * @{
 */

#ifndef _NO_UTILS_CLASSES
/**
 * ASN.1 UTCTime control class. 
 * The ASN1CUTTime class is derived from the ASN1CTime base class. It used as
 * the bass class for generated control classes for the ASN.1 Universal Time
 * ([UNIVERSAL 23] IMPLICIT VisibleString) type. This class provides utility
 * methods for operating on the time information referenced by the generated
 * class. This class can also be used inline to operate on the time within
 * generated time string elements in a SEQUENCE, SET, or CHOICE construct. The
 * string generally is encoded according to ISO 8601 format with some
 * exceptions (see X.680).
 */
class EXTRTCLASS ASN1CUTCTime : public ASN1CTime {
 protected:
   ASN1TUTCTime timeObj;

   virtual ASN1TTime& getTimeObj () { return timeObj; }
   virtual const ASN1TTime& getTimeObj () const { return timeObj; }

   EXTRTMETHOD ASN1CUTCTime(char*& buf, int bufSize, OSBOOL useDerRules = FALSE); 
   EXTRTMETHOD ASN1CUTCTime(ASN1UTCTime& buf, OSBOOL useDerRules = FALSE); 

 public:
  /**
   * This constructor creates a time string from a buffer.
   *
   * It does not deep-copy the data, it just assigns the passed array to an
   * internal reference variable. The object will then directly operate on the
   * given data variable.
   *
   * @param msgBuf       Reference to an ASN1MEssage buffer derived object (for
   *                       example, an ASN1BEREncodeBuffer).
   * @param buf          Reference to a pointer to a time string buffer.
   * @param bufSize      Size of passed buffer, in bytes.
   * @param useDerRules  Use the Distinguished Encoding Rules to encode or
   *                       decode the value,
   */
   EXTRTMETHOD ASN1CUTCTime(OSRTMessageBufferIF& msgBuf, char*& buf, int bufSize,
      OSBOOL useDerRules = FALSE); 

  /**
   * This constructor creates a time string suing the ASN1UTCTime argument. c
   * The constructor does not deep-copy the variable, it assigns a referene to
   * it to an internal variable. The object will then directly operate on the
   * given data variable. This form of the constructor is used with a
   * compiler-generated time string variable.
   *
   * @param msgBuf       Reference to an ASN1MEssage buffer derived object (for
   *                       example, an ASN1BEREncodeBuffer).
   * @param buf          Reference to a time string structure.
   * @param useDerRules  Use the Distinguished Encoding Rules to encode or
   *                       decode the value,
   */
   EXTRTMETHOD ASN1CUTCTime(OSRTMessageBufferIF& msgBuf, ASN1UTCTime& buf,
      OSBOOL useDerRules = FALSE); 

   EXTRTMETHOD ASN1CUTCTime(OSRTContext& ctxt, char*& buf, int bufSize,
      OSBOOL useDerRules = FALSE); 

   EXTRTMETHOD ASN1CUTCTime(OSRTContext& ctxt, ASN1UTCTime& buf,
      OSBOOL useDerRules = FALSE); 

   // copy ctor

   ASN1CUTCTime (const ASN1CUTCTime& original) :
      ASN1CTime (original) {}

  /**
   * Converts time_t to time string.
   *
   * @param time         time to convert,
   * @param diffTime     TRUE means the difference between local time and UTC
   *                       will be calculated; in other case only local time
   *                       will be stored.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   EXTRTMETHOD int setTime(time_t time, OSBOOL diffTime);

   const ASN1CUTCTime& operator = (const ASN1CUTCTime& tm) {
      return (ASN1CUTCTime&) ASN1CTime::operator = (tm);
   }

 protected:
   /*
   * Compiles new time string accoring X.680 (clause 42) and ISO 8601.
   * Returns 0, if successful, or error code, if error.
   */
   EXTRTMETHOD int compileString();

   /* 
   * Gets fraction of second, 0..9. 
   * Returns: Zero.
   */
   EXTRTMETHOD int getFraction();
   
   /*
   * This method is not applicable for ASN1CUTCTime.
   * Returns RTERR_NOTSUPP.
   */
   EXTRTMETHOD int setFraction(int fraction);

} ;
#else
typedef class _ASN1CUTCTime : public _ASN1CTime {
 protected:
   _ASN1CUTCTime(char*& buf, int bufSize, OSBOOL useDerRules) {}
   _ASN1CUTCTime(ASN1VisibleString& buf,  OSBOOL useDerRules) {}
 public:
   _ASN1CUTCTime (OSRTMessageBufferIF& msgBuf, char*& buf, int sz,
      OSBOOL useDerRules = FALSE) :
      _ASN1CTime (msgBuf, buf, sz, useDerRules) {} 
   _ASN1CUTCTime (OSRTMessageBufferIF& msgBuf, ASN1VisibleString& buf,
      OSBOOL useDerRules = FALSE) :
      _ASN1CTime (msgBuf, buf, useDerRules) {} 
   _ASN1CUTCTime(OSRTContext& ctxt, char*& buf, int bufSize, 
      OSBOOL useDerRules) : _ASN1CTime (ctxt, buf, bufSize, useDerRules) {}  
   _ASN1CUTCTime(OSRTContext& ctxt, ASN1VisibleString& buf, 
      OSBOOL useDerRules) : _ASN1CTime (ctxt, buf, bufSize, useDerRules) {}  
} ASN1CUTCTime;
#endif // _NO_UTILS_CLASSES
#endif //__ASN1CUTCTIME_H__

/**
 * @} 
 */
/**
 * @} 
 */
