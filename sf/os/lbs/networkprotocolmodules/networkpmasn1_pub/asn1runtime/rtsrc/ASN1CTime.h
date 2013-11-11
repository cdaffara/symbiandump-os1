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

#ifndef __ASN1CTIME_H__
#define __ASN1CTIME_H__

#ifndef _WIN32_WCE
#include <time.h>
#endif
#include "rtsrc/asn1CppTypes.h"
#include "rtsrc/ASN1TTime.h"

/**
 * @file rtsrc/ASN1CTime.h 
 * ASN1CTime abstract class definition. This is used
 * as the base class for other ASN.1 time class definitions.
 */
/**
 * @addtogroup asn1ctrl 
 * @{
 */
/**
 * @defgroup asn1ctime Date and Time Runtime Classes 
 * The date and time classes contain methods for doing date/time calculations
 * for the various ASN.1 time types including GeneralizedTime and UTCTime.
 *
 * @{
 */

//
//
// ASN1CTime
// 
// ISO 8601 time and date for ITU-T X.680. 
//
// Author Artem Bolgar.
//
#ifndef _NO_UTILS_CLASSES

#define LOG_TMERR(pctxt,stat) ((pctxt != 0) ? LOG_RTERR (pctxt, stat) : stat)

/** 
 * ASN.1 Time control base class. 
 * The ASN1CTime class is derived from the ASN1CType base class. It is used as
 * the abstract base class for generated control classes for the ASN.1
 * Generalized Time ([UNIVERSAL 24] IMPLICIT VisibleString) types and Universal
 * Time ([UNIVERSAL 23] IMPLICIT VisibleString) types. This class provides
 * utility methods for operating on the time information referenced by the
 * generated class. This class can also be used inline to operate on the times
 * within generated time string elements in a SEQUENCE, SET, or CHOICE
 * construct. The time string are generally formatted according to ISO 8601
 * format with some exceptions (X.680).
 */
class EXTRTCLASS ASN1CTime : public ASN1CType {
 protected:
   OSBOOL parsed;
   OSBOOL derRules;
   
   char*& timeStr;
   int strSize;

   EXTRTMETHOD void checkCapacity();

   /* 
   * Returns time string pointer. NULL, if error occurred.
   */
   EXTRTMETHOD char*& getTimeStringPtr ();

   virtual ASN1TTime& getTimeObj () = 0;
   virtual const ASN1TTime& getTimeObj () const = 0;

   EXTRTMETHOD ASN1CTime(char*& buf, int bufSize, OSBOOL useDerRules); 
   EXTRTMETHOD ASN1CTime(ASN1VisibleString& buf,  OSBOOL useDerRules); 
 private:
   char timeStrBuf[MAX_TIMESTR_SIZE];
   char* pTimeStr; // placeholder

   EXTRTMETHOD void privateInit ();

 public:
   enum {
      January = 1,   Jan = 1,
      February = 2,  Feb = 2,
      March = 3,     Mar = 3,
      April = 4,     Apr = 4,
      May = 5,
      June = 6,      Jun = 6,
      July = 7,      Jul = 7,
      August = 8,    Aug = 8,
      September = 9, Sep = 9,
      October = 10,  Oct = 10,
      November = 11, Nov = 11,
      December = 12, Dec = 12
   }; 

  /**
   * This constructor creates a time string from buffer.
   *
   * It does not deep-copy the data; it just assigns the passed array to an
   * internal reference variable. The object will then directly operate on the
   * given data variable.
   *
   * @param msgBuf       Reference to an OSRTMessage buffer derived object (for
   *                       example, \c ASNBEREncodeBuffer).
   * @param buf          Reference to a pointer to a time string buffer.
   * @param bufSize      Size of buffer in bytes.
   * @param useDerRules  Use the Distinguished Encoding Rules (DER) to operate
   *                       on this time value.
   */
   EXTRTMETHOD ASN1CTime (OSRTMessageBufferIF& msgBuf, char*& buf, int bufSize, 
              OSBOOL useDerRules); 

  /**
   * This constructor creates a time string from an \c ASN1VisibleString
   * object.
   *
   * It does not deep-copy the data; it just assigns the passed object to an
   * internal reference variable. The object will then directly operate on the
   * given data variable.
   *
   * @param msgBuf       Reference to an OSRTMessage buffer derived object (for
   *                       example, \c ASNBEREncodeBuffer).
   * @param buf          Reference to a visible string object to hold the time
   *                       data.
   * @param useDerRules  Use the Distinguished Encoding Rules (DER) to operate
   *                       on this time value.
   */
   EXTRTMETHOD ASN1CTime (OSRTMessageBufferIF& msgBuf, ASN1VisibleString& buf, 
              OSBOOL useDerRules); 
   
   EXTRTMETHOD ASN1CTime (OSRTContext& ctxt, char*& buf, int bufSize, 
              OSBOOL useDerRules); 

   EXTRTMETHOD ASN1CTime (OSRTContext& ctxt, ASN1VisibleString& buf, 
              OSBOOL useDerRules); 
   
   /**
    * The copy constructor. This does not deep-copy the original value.
    * Instead, it assigns references to the internal components.
    *
    * @param original     The original time string object value.
    */
   EXTRTMETHOD ASN1CTime (const ASN1CTime& original); 

   EXTRTMETHOD ~ASN1CTime();

  /**
   * This method returns the year component of the time value.
   *
   * Note that the return value may differ for different inherited ASN1CTime
   * classes.
   *
   * @param             - none
   * @return             Year component (full 4 digits) is returned if the
   *                       operation is successful. If the operation fails, a
   *                       negative value is returned.
   */
   virtual EXTRTMETHOD int getYear();

  /**
   * This method returns the month number component of the time value.
   *
   * The number of January is 1, February 2, ... up to December 12. You may
   * also use enumerated valued for decoded months: ASN1CTime::January,
   * ASN1CTime::February, etc. Also short aliases for months can be used:
   * ASN1CTime::Jan, ASN1CTime::Feb, etc. Note that the return value may differ
   * for different inherited ASN1CTime classes.
   *
   * @param             - none
   * @return             Month component (1 - 12) is returned if operation is
   *                       successful. If the operation fails, a negative value
   *                       is returned.
   */
   virtual EXTRTMETHOD int getMonth();

  /**
   * This method returns the day of month number component of the time value.
   *
   * The number of the first day in the month is 1; the number of the last day
   * may be in the interval from 28 to 31. Note that the return value may be
   * differ for different inherited ASN1CTime classes.
   *
   * @param             - none
   * @return             Day of month component (1 - 31) is returned if the
   *                       operation is successful. If the operation fails, a
   *                       negative value is returned.
   */
   virtual EXTRTMETHOD int getDay();

  /**
   * This method returns the hour component of the time value.
   *
   * As the ISO 8601 is based on the 24-hour timekeeping system, hours are
   * represented by two-digit values from 00 to 23. Note that the return value
   * may differ from different inherited ASN1CTime classes.
   *
   * @param             - none
   * @return             Hour component (0 - 23) is returned if the operation
   *                       is successful. If the operation fails, a negative
   *                       value is returned.
   */
   virtual EXTRTMETHOD int getHour();

  /**
   * This method returns the minute component of the time value.
   *
   * Minutes are represented by the two digits from 00 to 59. Note that the
   * return value may differ from different inherited ASN1CTime classes.
   *
   * @param             - none
   * @return             Minute component (0 - 59) is returned if the operation
   *                       is successful. If the operation fails, a negative
   *                       value is returned.
   */
   virtual EXTRTMETHOD int getMinute();

  /**
   * This method returns the second component of the time value.
   *
   * Seconds are represented by two digits from 00 to 59. Note that the return
   * value may differ from different inherited ASN1CTime classes.
   *
   * @param             - none
   * @return             Second component (0 - 59) is returned if the operation
   *                       is successful. If the operation fails, a negative
   *                       value is returned.
   */
   virtual EXTRTMETHOD int getSecond();

  /**
   * This method returns the second's decimal component of the time value.
   *
   * Second's decimal fraction is represented by one or more digits from 0 to 9. Note
   * that the return value may differ for different inherited ASN1CTime
   * classes.
   *
   * @param             - none
   * @return             Second's decimal fraction component (0 - 9) is
   *                       returned if operation is successful. If the
   *                       operation fails, a negative value is returned.
   */
   virtual EXTRTMETHOD int getFraction();

  /**
   * This method returns the second's decimal component of the time value.
   * Second's fraction will be represented as double value more than 0 and
   * less than 1.
   *
   * Second's decimal fraction is represented by one or more digits from 0 to 9. 
   *
   * @return             Second's decimal fraction component is
   *                       returned if operation is successful. If the
   *                       operation fails, a negative value is returned.
   */
   virtual EXTRTMETHOD double getFractionAsDouble();

  /**
   * This method returns the second's decimal component of the time value.
   * Second's fraction will be represented as string w/o integer part
   * and decimal point.
   *
   * @return             Length of the fraction string returned in pBuf, 
   *                       if operation is successful. If the
   *                       operation fails, a negative value is returned.
   */
   virtual EXTRTMETHOD int getFractionStr(char* const pBuf, size_t bufSize);

  /**
   * This method returns the number of digits in second's decimal component 
   * of the time value.
   *
   * @return             Second's decimal fraction's length is
   *                       returned if operation is successful. If the
   *                       operation fails, a negative value is returned.
   */
   virtual EXTRTMETHOD int getFractionLen();

  /**
   * This method returns the hour component of the difference between the time
   * zone of the object and the Coordinated Universal Time (UTC).
   *
   * The UTC time is the sum of the local time and a positive of negative time
   * difference. Note that the return value may differ for different inherited
   * ASN1CTime classes.
   *
   * @param             - none
   * @return             The negative or positive hour component of the
   *                       difference between the time zone of the object and
   *                       the UTC time (-12 - +12) is returned if the
   *                       operation is successful. If the operation fails, a
   *                       negative value is returned.
   */
   virtual EXTRTMETHOD int getDiffHour();

  /**
   * This method returns the minute component of the difference between the
   * time zone of the object and the Coordinated Universal Time (UTC).
   *
   * The UTC time is the sum of the local time and a positive of negative time
   * difference. Note that the return value may differ for different inherited
   * ASN1CTime classes.
   *
   * @param             - none
   * @return             The negative or positive minute component of the
   *                       difference between the time zone of the object and
   *                       the UTC time (-59 - +59) is returned if the
   *                       operation is successful. If the operation fails, a
   *                       negative value is returned.
   */
   virtual EXTRTMETHOD int getDiffMinute();

  /**
   * This method returns the difference between the time zone of the object and
   * the Coordinated Universal Time (UTC).
   *
   * The UTC time is the sum of the local time and a positive of negative time
   * difference. Note that the return value may differ for different inherited
   * ASN1CTime classes.
   *
   * @param             - none
   * @return             The negative or positive minute component of the
   *                       difference between the time zone of the object and
   *                       the UTC time (-12*60 - +12*60) is returned if the
   *                       operation is successful. If the operation fails, a
   *                       negative value is returned.
   */
   virtual EXTRTMETHOD int getDiff();

  /**
   * This method returns the UTC flag state.
   *
   * If the UTC flag is TRUE, then the time is a UTC time and symbol Z is added
   * at the end of the time string. Otherwise, it is local time.
   *
   * @param             - none
   * @return             UTC flag state is returned.
   */
   virtual EXTRTMETHOD OSBOOL getUTC();

  /**
   * This method converts the time string to a value of the built-in C type
   * time_t.
   *
   * The value is the number of seconds from January 1, 1970. If the time is
   * represned as UTC time plus or minus a time difference, then the resulting
   * value will be recalculated as local time. For example, if the time string
   * is "19991208120000+0930", then this string will be converted to
   * "19991208213000" and then converted to a time_t value. Note that the
   * return value may differ for different inherited ASN1CTime classes.
   *
   * @param             - none
   * @return             The time value, expressed as a number of seconds from
   *                       January 1, 1970. If the operation fails, a negative
   *                       value is returned.
   */
   virtual EXTRTMETHOD time_t getTime();

   /** 
    * This method sets the 'use DER' flag which enforces the DER rules 
    * when time strings are constructed or parsed.
    */
   void setDER (OSBOOL bvalue) { derRules = bvalue; }

  /** 
   * This method sets teh UTC flag state.
   *
   * If the UTC flag is TRUE, then the time is a UTC time and symbol 'Z' is
   * added to the end of the string. Otherwise, it is a local time.
   *
   * @param utc          UTC flag state.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   virtual EXTRTMETHOD int setUTC(OSBOOL utc);

  /**
   * This method sets the year component of the time value.
   *
   * Note that the action of this method may differ for different inherited
   * ASN1CTime classes.
   *
   * @param year_        Year component (full 4 digits).
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   virtual EXTRTMETHOD int setYear(int year_);

  /**
   * This method sets the month number component of the time value.
   *
   * The number of January is 1, February 2, ..., through December 12. You may
   * use enumerated values for months encoding: ASN1CTime::January,
   * ASN1CTime::February, etc. Also you can use short aliases for months:
   * ASN1CTime::Jan, ASN1CTime::Feb, etc. Note that the action of this method
   * may differ for different inherited ASN1CTime classes.
   *
   * @param month_       Month component (1 - 12).
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   virtual EXTRTMETHOD int setMonth(int month_);

  /**
   * This method sets the day of month number component of the time value.
   *
   * The number of the first day in the month is 1; the number of the last day
   * in the month may be in the interval from 28 to 31. Note that the action of
   * this method may differ for different inherited ASN1CTime classes.
   *
   * @param day_         Day of month component (1 - 31).
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   virtual EXTRTMETHOD int setDay(int day_);

  /**
   * This method sets the hour component of the time value.
   *
   * As the ISO 8601 is based on the 24-hour timekeeping system, hours are
   * represented by two digits from 00 to 23. Note that the action of this
   * method may differ for different inherited ASN1CTime classes.
   *
   * @param hour_        Hour component (0 - 23).
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   virtual EXTRTMETHOD int setHour(int hour_);

  /**
   * This method sets the minute component of the time value.
   *
   * Minutes are represented by two digits from 00 to 59. Note that the action
   * of this method may differ for different inherited ASN1CTime classes.
   *
   * @param minute_      Minute component (0 - 59).
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   virtual EXTRTMETHOD int setMinute(int minute_);

  /**
   * This method sets the second component of the time value.
   *
   * Seconds are represented by two digits from 00 to 59. Note that the action
   * of this method may differ form different inherited ASN1CTime classes.
   *
   * @param second_      Second component (0 - 59).
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   virtual EXTRTMETHOD int setSecond(int second_);

  /**
   * This method sets the second's decimal fraction component of the time
   * value.
   *
   * Second's decimal fraction is represented by one or more digits from 0 to 9. Note
   * that the action of this method may differ for different inherited
   * ASN1CTime classes.
   *
   * @param fraction     Second's decimal fraction component (0 - 9).
   * @param fracLen      Optional parameter specifies number of digits in
   *                       second's fraction.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   virtual EXTRTMETHOD int setFraction(int fraction, int fracLen = -1);

  /**
   * This method sets the second's decimal fraction component of the time
   * value. Double value must be greater or equal 0 and less than 1.
   *
   * @param frac         Second's decimal fraction component.
   * @param fracLen      Specifies number of digits in
   *                       second's fraction.
   * @return             Completion status of operation:
   *                       - 0 (ASN_OK) = success,
   *                       - negative return value is error.
   */
   virtual EXTRTMETHOD int setFraction (double frac, int fracLen);

  /**
   * This method sets the second's decimal fraction component of the time
   * value. Double value must be greater or equal 0 and less than 1.
   *
   * @param frac         Second's decimal fraction component.
   * @return             Completion status of operation:
   *                       - 0 (ASN_OK) = success,
   *                       - negative return value is error.
   */
   virtual EXTRTMETHOD int setFraction (char const* frac);

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
   virtual int setTime(time_t time, OSBOOL diffTime) = 0;

  /**
   * This method sets teh hour component of the difference between the time
   * zone of the object and the Coordinated Universal Time (UTC).
   *
   * The UTC time is the sum of the local time and a positive or negative time
   * difference. Note that the action of this method may differ from different
   * inherited ASN1CTime classes.
   *
   * @param dhour        The negative or positive hour component of the
   *                       difference between the time zone of the object and
   *                       the UTC time (-12 - +12) is returned if the
   *                       operation is successful. If the operation fails, a
   *                       negative value is returned.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   virtual EXTRTMETHOD int setDiffHour(int dhour);

  /**
   * This method sets the hours and the minute components of the difference
   * between the time zone of the object and Coordinated Universal Time (UTC).
   *
   * The UTC time is the sum of the local time and a positive or negative time
   * difference. Note that the action of this method may differ for different
   * inherited ASN1CTime classes.
   *
   * @param dhour        The negative or positive hour component of the
   *                       difference between the time zone of the object and
   *                       the UTC time (-12 - +12).
   * @param dminute      The negative or positive minute component of the
   *                       difference between the time zone of the object and
   *                       the UTC time (-59 - +59).
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   virtual EXTRTMETHOD int setDiff(int dhour, int dminute);

  /**
   * This method sets the difference between the time zone of the object and
   * Coordinated Universal Time (UTC), in minutes.
   *
   * The UTC time is the sum of the local time and a positive or negative time
   * difference. Note that the action of this method may differ for different
   * inherited ASN1CTime classes.
   *
   * @param inMinutes    The negative or positive difference, in minutes,
   *                       between the time zone of the object and the UTC time
   *                       (-12*60 - +12*60) is returned if the operation is
   *                       successful.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   virtual EXTRTMETHOD int setDiff(int inMinutes);

  /**
   * This method parses the given time string.
   *
   * The string is expected to be in the ASN.1 value notation format for the
   * given ASN.1 time string type. Note that the action of this method may
   * differ for different inherited ASN1CTime classes.
   *
   * @param string       The time string value to be parsed.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   virtual EXTRTMETHOD int parseString(const char* string);

  /**
   * This method clears the time string.
   *
   * Note the action of this method may differ for different inherited
   * ASN1CTime classes.
   *
   * @param             - none
   * @return             - none
   */
   virtual EXTRTMETHOD void clear();

  /**
   * This method compares times.
   *
   */
   virtual EXTRTMETHOD int equals (ASN1CTime&);

   /* Returns the length of compiled time string */
   EXTRTMETHOD size_t getTimeStringLen ();

   /* Copies the compiled time string into specified buffer */
   EXTRTMETHOD const char* getTimeString (char* pbuf, size_t bufsize);

   EXTRTMETHOD const ASN1CTime& operator = (const ASN1CTime& );
   virtual EXTRTMETHOD OSBOOL operator == (ASN1CTime&);
   virtual EXTRTMETHOD OSBOOL operator != (ASN1CTime&);
   virtual EXTRTMETHOD OSBOOL operator > (ASN1CTime&);
   virtual EXTRTMETHOD OSBOOL operator < (ASN1CTime&);
   virtual EXTRTMETHOD OSBOOL operator >= (ASN1CTime&);
   virtual EXTRTMETHOD OSBOOL operator <= (ASN1CTime&);

 protected:
   /*
   * Compiles new time string accoring X.680 and ISO 8601.
   * Returns 0, if succeed, or error code, if error.
   */
   virtual int compileString() = 0;

} ;

#else
typedef class _ASN1CTime : public ASN1CType {
 protected:
   _ASN1CTime(char*& buf, int bufSize, OSBOOL useDerRules) {}
   _ASN1CTime(ASN1VisibleString& buf,  OSBOOL useDerRules) {}
 public:
   _ASN1CTime (OSRTMessageBufferIF& msgBuf, char*& , int, OSBOOL) :
      ASN1CType (msgBuf) {} 
   _ASN1CTime (OSRTMessageBufferIF& msgBuf, ASN1VisibleString&, OSBOOL) :
      ASN1CType (msgBuf) {} 
   _ASN1CTime(OSRTContext& ctxt, char*& buf, int bufSize, 
      OSBOOL useDerRules) : ASN1CType (ctxt) {}  
   _ASN1CTime(OSRTContext& ctxt, ASN1VisibleString& buf, 
      OSBOOL useDerRules) : ASN1CType (ctxt) {}  
} ASN1CTime;

#endif // _NO_UTILS_CLASSES

/**
 * @} asn1ctime
 */
/**
 * @} asn1ctrl
 */

#endif //__ASN1CTIME_H__

