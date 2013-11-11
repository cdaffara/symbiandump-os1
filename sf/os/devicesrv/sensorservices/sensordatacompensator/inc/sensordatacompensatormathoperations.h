/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Math operations
*
*/


#ifndef SENSORDATACOMPENSATORMATHOPERATIONS_H
#define SENSORDATACOMPENSATORMATHOPERATIONS_H

#include <e32std.h>

#if !( defined( SDCOMP_USE_FIXED_POINT ) || defined( SDCOMP_USE_FIXED_POINT64 ) )

#include <e32math.h>

// Floating point for testing, best accuracy and worst performance
typedef TReal FValue;

/**
* Returns sine value from given angle
*
* @since S60 5.0
* @param aDeg Angle in degrees
* @return Sine value
*/
inline FValue FSin( TInt aDeg )
    {
    FValue ret;
    Math::Sin( ret, static_cast<TReal>(aDeg) * KDegToRad );
    return ret;
    }

/**
* Returns cosine value from given angle
*
* @since S60 5.0
* @param aDeg Angle in degrees
* @return Cosine value
*/
inline FValue FCos( TInt aDeg )
    {
    FValue ret;
    Math::Cos( ret, static_cast<TReal>(aDeg) * KDegToRad );
    return ret;
    }

/**
* Rounds the given value and returns it as integer
*
* @since S60 5.0
* @param aValue Value to round
* @return Rounded value
*/
inline TInt FRoundToInt( FValue aValue )
    {
    // If the rounding does not succeed, the assigment to integer
    // cuts off the decimals anyway.
    FValue ret;
    Math::Round( ret, aValue, 0 );
    return ret;
    }

/**
* Returns absolute value from given value
*
* @since S60 5.0
* @param aValue The value
* @return Absolute value
*/
inline FValue FAbs( FValue aValue )
    {
    return Abs(aValue);
    }

#else // !( SDCOMP_USE_FIXED_POINT || SDCOMP_USE_FIXED_POINT64 )

/**
* Returns fixed point sine value from given angle
*
* @since S60 5.0
* @param aDeg Angle in degrees
* @param aDecBits Number of bits used to store decimal part by fixed point
* @return Sine value as fixed point value
*/
TInt FixedPointSin( TInt aDeg, TInt aDecBits );

/**
 *  Class implements fixed point type for fast calculations as light weight template
 *
 *  @lib sensordatacompensator.lib
 *  @since S60 5.0
 */
template <class T, class DT, TInt Q>
class TFixedPoint
    {
public:
    /**
    * Default constructor
    * @since S60 5.0
    */
    inline TFixedPoint() {};

    /**
    * Constructor
    * @since S60 5.0
    * @param a Initial value as floating point
    */
    inline TFixedPoint( TReal a )
        {
        iFp = static_cast<T>( a * ( 1<<Q ) );
        }

    /**
    * Assigns floating point to fixed point
    * @since S60 5.0
    * @param a Value to assign
    * @return Fixed point value
    */
    inline TFixedPoint& operator=( TReal a )
        {
        iFp = static_cast<T>( a * ( 1<<Q ) );
        return *this;
        }

    /**
    * Constructor
    * @since S60 5.0
    * @param a Initial value as integer
    */
    inline TFixedPoint( TInt a )
        {
        iFp = static_cast<T>( a ) << Q;
        }

    /**
    * Assigns integer to fixed point
    * @since S60 5.0
    * @param a Value to assign
    * @return Fixed point value
    */
    inline TFixedPoint& operator=( TInt a )
        {
        iFp = static_cast<T>( a ) << Q;
        return *this;
        }

    /**
    * Constructor
    * @since S60 5.0
    * @param a Initial value as 64-bit integer
    */
    inline TFixedPoint( const TInt64& a )
        {
        iFp = static_cast<T>( a ) << Q;
        }

    /**
    * Assigns 64-bit integer to fixed point
    * @since S60 5.0
    * @param a Value to assign
    * @return Fixed point value
    */
    inline TFixedPoint& operator=( const TInt64& a )
        {
        iFp = static_cast<T>( a ) << Q;
        return *this;
        }

    /**
    * Casts fixed point to floating point value
    * @since S60 5.0
    * @return Floating point value
    */
    inline operator TReal() const
        {
        return static_cast<TReal>( iFp ) / ( 1<<Q );
        }

    /**
    * Casts fixed point to integer value
    * @since S60 5.0
    * @return Integer value
    */
    inline operator TInt() const
        {
        return iFp >> Q;
        }

    /**
    * Compares if values are equal
    * @since S60 5.0
    * @param a Value to compare with
    * @return ETrue if values are equal. Otherwise EFalse.
    */
    inline TBool operator==( const TFixedPoint& a ) const
        {
        return iFp == a.iFp;
        }

    /**
    * Compares if values are inequal
    * @since S60 5.0
    * @param a Value to compare with
    * @return ETrue if values are inequal. Otherwise EFalse.
    */
    inline TBool operator!=( const TFixedPoint& a ) const
        {
        return iFp != a.iFp;
        }

    /**
    * Compares if value is greater
    * @since S60 5.0
    * @param a Value to compare with
    * @return ETrue if value is greater. Otherwise EFalse.
    */
    inline TBool operator>( const TFixedPoint& a ) const
        {
        return iFp > a.iFp;
        }

    /**
    * Compares if value is smaller
    * @since S60 5.0
    * @param a Value to compare with
    * @return ETrue if value is smaller. Otherwise EFalse.
    */
    inline TBool operator<( const TFixedPoint& a ) const
        {
        return iFp < a.iFp;
        }

    /**
    * Compares if value is greater or equal
    * @since S60 5.0
    * @param a Value to compare with
    * @return ETrue if value is greater or equal. Otherwise EFalse.
    */
    inline TBool operator>=( const TFixedPoint& a ) const
        {
        return iFp >= a.iFp;
        }

    /**
    * Compares if value is smaller or equal
    * @since S60 5.0
    * @param a Value to compare with
    * @return ETrue if value is smaller or equal. Otherwise EFalse.
    */
    inline TBool operator<=( const TFixedPoint& a ) const
        {
        return iFp <= a.iFp;
        }

    /**
    * Adds values
    * @since S60 5.0
    * @param a Value to add with
    * @return Result value
    */
    inline TFixedPoint operator+( const TFixedPoint& a ) const
        {
        TFixedPoint tmp;
        tmp.iFp = iFp + a.iFp;
        return tmp;
        }

    /**
    * Subtracts values
    * @since S60 5.0
    * @param a Value to subtracts with
    * @return Result value
    */
    inline TFixedPoint operator-( const TFixedPoint& a ) const
        {
        TFixedPoint tmp;
        tmp.iFp = iFp - a.iFp;
        return tmp;
        }

    /**
    * Changes sign of the value
    * @since S60 5.0
    * @return Result value
    */
    inline TFixedPoint operator-() const
        {
        TFixedPoint tmp;
        tmp.iFp = -iFp;
        return tmp;
        }

    /**
    * Adds values and stores result
    * @since S60 5.0
    * @param a Value to add with
    * @return Result value
    */
    inline TFixedPoint& operator+=( const TFixedPoint& a )
        {
        iFp += a.iFp;
        return *this;
        }

    /**
    * Subtracts values and stores result
    * @since S60 5.0
    * @param a Value to subtract with
    * @return Result value
    */
    inline TFixedPoint& operator-=( const TFixedPoint& a )
        {
        iFp -= a.iFp;
        return *this;
        }

    /**
    * Multiplies values
    * @since S60 5.0
    * @param a Value to multiply with
    * @return Result value
    */
    inline TFixedPoint operator*( const TFixedPoint& a ) const
        {
        TFixedPoint tmp;
        tmp.iFp = Mul( iFp, a.iFp );
        return tmp;
        }

    /**
    * Divides value by given value
    * @since S60 5.0
    * @param a Value to divide with
    * @return Result value
    */
    inline TFixedPoint operator/( const TFixedPoint& a ) const
        {
        TFixedPoint tmp;
        tmp.iFp = Div( iFp, a.iFp );
        return tmp;
        }

    /**
    * Multiplies values and stores result
    * @since S60 5.0
    * @param a Value to multiply with
    * @return Result value
    */
    inline TFixedPoint& operator*=( const TFixedPoint& a )
        {
        iFp = Mul( iFp, a.iFp );
        return *this;
        }

    /**
    * Divides value by given value and stores result
    * @since S60 5.0
    * @param a Value to divide with
    * @return Result value
    */
    inline TFixedPoint& operator/=( const TFixedPoint& a )
        {
        iFp = Div( iFp, a.iFp );
        return *this;
        }

    /**
    * Rounds the given value and returns it as integer
    *
    * @since S60 5.0
    * @param a Value to round
    * @return Rounded value
    */
    static inline TInt RoundToInt( const TFixedPoint& a )
        {
        // Round up to next integer
        return ( a.iFp + (( 1<<Q ) >> 1 )) >> Q;
        }

    /**
    * Returns absolute value from given value
    *
    * @since S60 5.0
    * @param a The value
    * @return Absolute value
    */
    static inline TFixedPoint Abs( const TFixedPoint& a )
        {
        return ( a.iFp < 0 ? -a.iFp : a.iFp );
        }

    /**
    * Returns sine value from given value
    *
    * @since S60 5.0
    * @param a Angle in degrees
    * @return Sine value
    */
    static inline TFixedPoint Sin( TInt a )
        {
        TFixedPoint tmp;
        tmp.iFp = FixedPointSin( a, Q );
        return tmp;
        }

private:
    static inline T Mul( DT a, T b )
        {
        return static_cast<T>( ( a * b ) >> Q );
        }

    static inline T Div( DT a, T b )
        {
        return b ? ( static_cast<T>( ( a << Q ) / b ) ) : 0;
        }

private: // Data
    // Stores the fixed point value
    T iFp;

    };

#ifdef SDCOMP_USE_FIXED_POINT64
 typedef TFixedPoint<TInt64, TInt64, 16> FValue; // 48.16 fixed point
#else //SDCOMP_USE_FIXED_POINT64
 typedef TFixedPoint<TInt, TInt64, 13> FValue; // 19.13 fixed point
#endif // SDCOMP_USE_FIXED_POINT64

/**
* Returns sine value from given angle
*
* @since S60 5.0
* @param aDeg Angle in degrees
* @return Sine value
*/
inline FValue FSin( TInt aDeg )
    {
    return FValue::Sin( aDeg );
    }

/**
* Returns cosine value from given angle
*
* @since S60 5.0
* @param aDeg Angle in degrees
* @return Cosine value
*/
inline FValue FCos( TInt aDeg )
    {
    return FSin( aDeg + 90 ); // Adjust 90 degree phase difference
    }

/**
* Rounds the given value and returns it as integer
*
* @since S60 5.0
* @param aValue Value to round
* @return Rounded value
*/
inline TInt FRoundToInt( FValue aValue )
    {
    return FValue::RoundToInt(aValue);
    }

/**
* Returns absolute value from given value
*
* @since S60 5.0
* @param aValue The value
* @return Absolute value
*/
inline FValue FAbs( FValue aValue )
    {
    return FValue::Abs(aValue);
    }

#endif // !( SDCOMP_USE_FIXED_POINT || SDCOMP_USE_FIXED_POINT64 )

#endif // SENSORDATACOMPENSATORMATHOPERATIONS_H
