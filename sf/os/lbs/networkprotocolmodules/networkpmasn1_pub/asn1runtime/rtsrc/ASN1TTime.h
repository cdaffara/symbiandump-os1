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

#ifndef __ASN1TTIME_H__
#define __ASN1TTIME_H__

#ifndef _WIN32_WCE
#include <time.h>
#endif
#include "rtsrc/asn1CppTypes.h"

#define MAX_TIMESTR_SIZE 64

#define LOG_TTMERR(stat) (mStatus = stat, stat)


//
//
// ASN1TTime
// 
// ISO 8601 time and date for ITU-T X.680. 
//
// Author Artem Bolgar.
//

/**
 * @file rtsrc/ASN1TTime.h 
 */
/** 
 * @ingroup cppruntime 
 */
/**
 * @addtogroup asn1data
 * @{
 */
/**
 * @addtogroup asn1ctime
 * @{
 */
/** 
 * ASN.1 Time utility base class. 
 */
class EXTRTCLASS ASN1TTime {
 public:
   short mYear;
   short mMonth;
   short mDay;
   short mHour;
   short mMinute;
   short mSecond;
   short mDiffHour;
   short mDiffMin;
   int   mSecFraction;
   int   mSecFracLen;
   int   mStatus;
   OSBOOL mbUtcFlag;
   OSBOOL mbDerRules;
   
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

   EXTRTMETHOD ASN1TTime (); 

  /**
   * This constructor creates an empty time class.
   *
   * @param useDerRules  Use the Distinguished Encoding Rules (DER) to operate
   *                       on this time value.
   */
   EXTRTMETHOD ASN1TTime (OSBOOL useDerRules); 
   
   
   /**
    * The copy constructor.
    *
    * @param original     The original time string object value.
    */
   EXTRTMETHOD ASN1TTime (const ASN1TTime& original); 

   /**
    * The destructor.
    */
   virtual EXTRTMETHOD ~ASN1TTime();

  /**
   * This method returns the year component of the time value.
   *
   * Note that the return value may differ for different inherited ASN1TTime
   * classes.
   *
   * @return             Year component (full 4 digits) is returned if the
   *                       operation is successful. If the operation fails, a
   *                       negative value is returned.
   */
   virtual EXTRTMETHOD int getYear() const;

  /**
   * This method returns the month number component of the time value.
   *
   * The number of January is 1, February 2, ... up to December 12. You may
   * also use enumerated valued for decoded months: ASN1TTime::January,
   * ASN1TTime::February, etc. Also short aliases for months can be used:
   * ASN1TTime::Jan, ASN1TTime::Feb, etc. Note that the return value may differ
   * for different inherited ASN1TTime classes.
   *
   * @return             Month component (1 - 12) is returned if operation is
   *                       successful. If the operation fails, a negative value
   *                       is returned.
   */
   virtual EXTRTMETHOD int getMonth() const;

  /**
   * This method returns the day of month number component of the time value.
   *
   * The number of the first day in the month is 1; the number of the last day
   * may be in the interval from 28 to 31. Note that the return value may be
   * differ for different inherited ASN1TTime classes.
   *
   * @return             Day of month component (1 - 31) is returned if the
   *                       operation is successful. If the operation fails, a
   *                       negative value is returned.
   */
   virtual EXTRTMETHOD int getDay() const;

  /**
   * This method returns the hour component of the time value.
   *
   * As the ISO 8601 is based on the 24-hour timekeeping system, hours are
   * represented by two-digit values from 00 to 23. Note that the return value
   * may differ from different inherited ASN1TTime classes.
   *
   * @return             Hour component (0 - 23) is returned if the operation
   *                       is successful. If the operation fails, a negative
   *                       value is returned.
   */
   virtual EXTRTMETHOD int getHour() const;

  /**
   * This method returns the minute component of the time value.
   *
   * Minutes are represented by the two digits from 00 to 59. Note that the
   * return value may differ from different inherited ASN1TTime classes.
   *
   * @return             Minute component (0 - 59) is returned if the operation
   *                       is successful. If the operation fails, a negative
   *                       value is returned.
   */
   virtual EXTRTMETHOD int getMinute() const;

  /**
   * This method returns the second component of the time value.
   *
   * Seconds are represented by two digits from 00 to 59. Note that the return
   * value may differ from different inherited ASN1TTime classes.
   *
   * @return             Second component (0 - 59) is returned if the operation
   *                       is successful. If the operation fails, a negative
   *                       value is returned.
   */
   virtual EXTRTMETHOD int getSecond() const;

  /**
   * This method returns the second's decimal component of the time value.
   *
   * Second's decimal fraction is represented by one or more digits from 0 to 9. 
   *
   * @return             Second's decimal fraction component is
   *                       returned if operation is successful. If the
   *                       operation fails, a negative value is returned.
   */
   virtual EXTRTMETHOD int getFraction() const;

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
   virtual EXTRTMETHOD double getFractionAsDouble() const;

  /**
   * This method returns the second's decimal component of the time value.
   * Second's fraction will be represented as string w/o integer part
   * and decimal point.
   *
   * @return             Length of the fraction string returned in pBuf, 
   *                       if operation is successful. If the
   *                       operation fails, a negative value is returned.
   */
   virtual EXTRTMETHOD int getFractionStr(char* const pBuf, size_t bufSize) const;

  /**
   * This method returns the number of digits in second's decimal component 
   * of the time value.
   *
   * @return             Second's decimal fraction's length is
   *                       returned if operation is successful. If the
   *                       operation fails, a negative value is returned.
   */
   virtual EXTRTMETHOD int getFractionLen() const;

  /**
   * This method returns the hour component of the difference between the time
   * zone of the object and the Coordinated Universal Time (UTC).
   *
   * The UTC time is the sum of the local time and a positive of negative time
   * difference. Note that the return value may differ for different inherited
   * ASN1TTime classes.
   *
   * @return             The negative or positive hour component of the
   *                       difference between the time zone of the object and
   *                       the UTC time (-12 - +12) is returned if the
   *                       operation is successful. If the operation fails, a
   *                       negative value is returned.
   */
   virtual EXTRTMETHOD int getDiffHour() const;

  /**
   * This method returns the minute component of the difference between the
   * time zone of the object and the Coordinated Universal Time (UTC).
   *
   * The UTC time is the sum of the local time and a positive of negative time
   * difference. Note that the return value may differ for different inherited
   * ASN1TTime classes.
   *
   * @return             The negative or positive minute component of the
   *                       difference between the time zone of the object and
   *                       the UTC time (-59 - +59) is returned if the
   *                       operation is successful. If the operation fails, a
   *                       negative value is returned.
   */
   virtual EXTRTMETHOD int getDiffMinute() const;

  /**
   * This method returns the difference between the time zone of the object and
   * the Coordinated Universal Time (UTC).
   *
   * The UTC time is the sum of the local time and a positive of negative time
   * difference. Note that the return value may differ for different inherited
   * ASN1TTime classes.
   *
   * @return             The negative or positive minute component of the
   *                       difference between the time zone of the object and
   *                       the UTC time (-12*60 - +12*60) is returned if the
   *                       operation is successful. If the operation fails, a
   *                       negative value is returned.
   */
   virtual EXTRTMETHOD int getDiff() const;

  /**
   * This method returns the UTC flag state.
   *
   * If the UTC flag is TRUE, then the time is a UTC time and symbol Z is added
   * at the end of the time string. Otherwise, it is local time.
   *
   * @return             UTC flag state is returned.
   */
   virtual EXTRTMETHOD OSBOOL getUTC() const;

  /**
   * This method converts the time string to a value of the built-in C type
   * time_t.
   *
   * The value is the number of seconds from January 1, 1970. If the time is
   * represned as UTC time plus or minus a time difference, then the resulting
   * value will be recalculated as local time. For example, if the time string
   * is "19991208120000+0930", then this string will be converted to
   * "19991208213000" and then converted to a time_t value. Note that the
   * return value may differ for different inherited ASN1TTime classes.
   *
   * @return             The time value, expressed as a number of seconds from
   *                       January 1, 1970. If the operation fails, a negative
   *                       value is returned.
   */
   virtual EXTRTMETHOD time_t getTime() const;

   /** 
    * This method sets the 'use DER' flag which enforces the DER rules 
    * when time strings are constructed or parsed.
    */
   void setDER (OSBOOL bvalue) { mbDerRules = bvalue; }

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
   * ASN1TTime classes.
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
   * use enumerated values for months encoding: ASN1TTime::January,
   * ASN1TTime::February, etc. Also you can use short aliases for months:
   * ASN1TTime::Jan, ASN1TTime::Feb, etc. Note that the action of this method
   * may differ for different inherited ASN1TTime classes.
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
   * this method may differ for different inherited ASN1TTime classes.
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
   * method may differ for different inherited ASN1TTime classes.
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
   * of this method may differ for different inherited ASN1TTime classes.
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
   * of this method may differ form different inherited ASN1TTime classes.
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
   * ASN1TTime classes.
   *
   * @param fraction     Second's decimal fraction component (0 - 9).
   * @param fracLen      Optional parameter specifies number of digits in
   *                       second's fraction.
   * @return             Completion status of operation:
   *                       - 0 (ASN_OK) = success,
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
   *                       - 0 (0) = success,
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
   * action of this method may differ for different inherited ASN1TTime
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
   * inherited ASN1TTime classes.
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
   * inherited ASN1TTime classes.
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
   * inherited ASN1TTime classes.
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
   * differ for different inherited ASN1TTime classes.
   *
   * @param string       The time string value to be parsed.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   virtual int parseString (const char* string) = 0;

  /**
   * This method clears the time object.
   *
   * Note the action of this method may differ for different inherited
   * ASN1TTime classes.
   */
   virtual EXTRTMETHOD void clear();

  /**
   * Compiles new time string accoring X.680 and ISO 8601.
   * Returns 0, if succeed, or error code, if error.
   *
   * @param pbuf         A pointer to destination buffer.
   * @param bufsize      A size of destination buffer.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   virtual int compileString (char* pbuf, size_t bufsize) const = 0;

  /**
   * This method compares times.
   *
   */
   virtual EXTRTMETHOD int equals (const ASN1TTime&) const;

   /**
    * Get a printable ASCII string of the time value into the specified buffer.
    *
    * @param pbuf         Pointer to a destination buffer.
    * @param bufsize      Size of destination buffer.
    * @return             Compiled time string. NULL, if error occurs.
    */
   EXTRTMETHOD const char* toString (char* pbuf, size_t bufsize) const;

   /**
    * Get a printable ASCII string of the time value.
    *
    * @param pctxt        Pointer to a context structure.
    * @return             Compiled time string. NULL, if error occurs.
    */
   EXTRTMETHOD const char* toString (OSCTXT* pctxt) const;

   /**
    * Get a printable ASCII string of the time value. Memory will be 
    * allocated using new[] operator. User is responsible to free it
    * using delete[].
    *
    * @return             Compiled time string. NULL, if error occurs.
    */
   EXTRTMETHOD const char* toString () const;

   EXTRTMETHOD const ASN1TTime& operator = (const ASN1TTime& );
   virtual EXTRTMETHOD OSBOOL operator == (const ASN1TTime&) const;
   virtual EXTRTMETHOD OSBOOL operator != (const ASN1TTime&) const;
   virtual EXTRTMETHOD OSBOOL operator > (const ASN1TTime&) const;
   virtual EXTRTMETHOD OSBOOL operator < (const ASN1TTime&) const;
   virtual EXTRTMETHOD OSBOOL operator >= (const ASN1TTime&) const;
   virtual EXTRTMETHOD OSBOOL operator <= (const ASN1TTime&) const;

 protected:
   EXTRTMETHOD static int checkDate (int day, int month, int year);
   EXTRTMETHOD void privateInit ();

   EXTRTMETHOD static void addMilliseconds (int deltaMs, 
                                short& year, short& month, short& day,
                                short& hour, short& minute, 
                                short& second, int& secFraction,
                                int secFracLen);
   EXTRTMETHOD static void addDays (int deltaDays, short& year, short& month, short& day);

   EXTRTMETHOD int getDaysNum () const;
   EXTRTMETHOD long getMillisNum () const;

   EXTRTMETHOD static short daysInMonth(int i);
   EXTRTMETHOD static int daysAfterMonth(int i);
 
} ;

/**
 * ASN.1 GeneralizedTime utility class. 
 * The ASN1TGeneralizedTime class is derived from the ASN1TTime base class.
 */
class EXTRTCLASS ASN1TGeneralizedTime : public ASN1TTime {
 public:

   /**
    * A default constructor.
    */
   inline ASN1TGeneralizedTime () {}
    
   /**
    * This constructor creates a time object using the specified time string.
    *
    * @param buf          A pointer to the time string to be parsed.
    * @param useDerRules  An OSBOOL value.
    */
   EXTRTMETHOD ASN1TGeneralizedTime(const char* buf, OSBOOL useDerRules = FALSE); 

   /**
    * This constructor creates an empty time object.
    *
    * @param useDerRules  An OSBOOL value.
    */
   inline ASN1TGeneralizedTime(OSBOOL useDerRules) : 
      ASN1TTime (useDerRules) {}

   /**
    * A copy constructor.
    */
   inline ASN1TGeneralizedTime (const ASN1TGeneralizedTime& original) :
      ASN1TTime (original) {}

   /**
    * This method returns the centry part (first two digits) of the year
    * component of the time value.
    *
    * @return             Century part (first two digits) of the year component
    *                       is returned if the operation is sucessful. If the
    *                       operation fails, one of the negative status codes is
    *                       returned.
    */
   EXTRTMETHOD int getCentury() const;

   /**
    * This method sets the centry part (first two digits) of the year component
    * of the time value.
    *
    * @param century      Century part (first two digits) of the year component.
    * @return             Completion status of operation:
    *                       - 0 (0) = success,
    *                       - negative return value is error.
    */
   EXTRTMETHOD int setCentury(int century);

   /**
    * This converts the value of the C built-in type time_t to a time string.
    *
    * The value is the number of seconds from January 1, 1970.
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

   /**
    * Parses sting.
    *
    * @return             Completion status of operation:
    *                       - 0 (0) = success,
    *                       - negative return value is error.
    */
   EXTRTMETHOD int parseString (const char* string);

   inline const ASN1TGeneralizedTime& operator = 
      (const ASN1TGeneralizedTime& tm) 
   {
      return (const ASN1TGeneralizedTime&) ASN1TTime::operator = (tm);
   }

   /**
    * Compiles new time string accoring X.680 and ISO 8601.
    * Returns 0, if succeed, or error code, if error.
    *
    * @param pbuf         A pointer to destination buffer.
    * @param bufsize      A size of destination buffer.
    * @return             Completion status of operation:
    *                       - 0 (0) = success,
    *                       - negative return value is error.
    */
   EXTRTMETHOD int compileString (char* pbuf, size_t bufsize) const;
} ;

/**
 * ASN.1 UTCTime utility class. 
 * The ASN1TUTTime class is derived from the ASN1TTime base class.
 */
class EXTRTCLASS ASN1TUTCTime : public ASN1TTime {
 public:

   /**
    * A default constructor.
    */
   EXTRTMETHOD ASN1TUTCTime ();

   /**
    * This constructor creates a time object using the specified time string.
    *
    * @param timeStr      A pointer to the time string to be parsed.
    * @param useDerRules  Create object using DER rules.
    */
   EXTRTMETHOD ASN1TUTCTime(const char* timeStr, OSBOOL useDerRules = FALSE); 

   /**
    * This constructor creates an empty time object.
    *
    * @param useDerRules  An OSBOOL value.
    */
   EXTRTMETHOD ASN1TUTCTime(OSBOOL useDerRules); 

   /**
    * A copy constructor.
    */
   ASN1TUTCTime (const ASN1TUTCTime& original) :
      ASN1TTime (original) {}

  /**
   * This method sets the year component of the time value.
   *
   * The year parameter can be either the two last digits of the year
   * (00 - 99) or the full four digits (0 - 9999). Note: the getYear method
   * returns the year in the full four digits, independent of the format of the
   * year parameter used in this method.
   *
   * @param year_        Year component (full four digits or only last two
   *                       digits).
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   EXTRTMETHOD int setYear(int year_);

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

  /** 
   * This method sets the UTC flag state.
   *
   * If the UTC flag is TRUE, then the time is a UTC time and symbol 'Z' is
   * added to the end of the string. Otherwise, it is a local time.
   *
   * @param utc          UTC flag state.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   EXTRTMETHOD int setUTC (OSBOOL utc);

  /**
   * Clears out the time object.
   */
   EXTRTMETHOD void clear();

  /**
   * Compiles new time string accoring X.680 and ISO 8601.
   * Returns 0, if succeed, or error code, if error.
   *
   * @param pbuf         A pointer to destination buffer.
   * @param bufsize      A size of destination buffer.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   EXTRTMETHOD int compileString (char* pbuf, size_t bufsize) const;

  /**
   * Parses the given time string.  The string is assumed to be in standard 
   * UTC time format.
   *
   * @param string      UTC time string to be parsed.
   * @return             Completion status of operation:
   *                       - 0 (0) = success,
   *                       - negative return value is error.
   */
   EXTRTMETHOD int parseString(const char* string);

   const ASN1TUTCTime& operator = (const ASN1TUTCTime& tm) {
      return (ASN1TUTCTime&) ASN1TTime::operator = (tm);
   }

 protected:
   /* 
   * Gets fraction of second, 0..9. 
   * Returns: Zero.
   */
   EXTRTMETHOD int getFraction() const;
   
   /*
   * This method is not applicable for ASN1TUTCTime.
   * Returns RTERR_NOTSUPP.
   */
   EXTRTMETHOD int setFraction (int fraction, int fracLen = -1);

} ;
#endif //__ASN1TTIME_H__

/**
 * @}
 */
/**
 * @}
 */
