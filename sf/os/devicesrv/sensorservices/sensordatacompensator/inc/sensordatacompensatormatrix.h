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
* Description:  Matrix operations
*
*/


#ifndef T_SENSORDATACOMPENSATORMATRIX_H
#define T_SENSORDATACOMPENSATORMATRIX_H

#include "sensordatacompensatormathoperations.h"

/**
 *  Matrix item definitions used as indices to access the items.
 *
 *  | EMatrixItem11 EMatrixItem12 EMatrixItem13 |
 *  | EMatrixItem21 EMatrixItem22 EMatrixItem23 |
 *  | EMatrixItem31 EMatrixItem32 EMatrixItem33 |
 */
enum TSensorDataCompensatorMatrixItem
    {
    EMatrixItem11 = 0,
    EMatrixItem12,
    EMatrixItem13,
    EMatrixItem21,
    EMatrixItem22,
    EMatrixItem23,
    EMatrixItem31,
    EMatrixItem32,
    EMatrixItem33,
    EMatrixItemCount
    };

/**
 *  Class implements matrix calculations
 *
 *  @lib sensordatacompensator.lib
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( TSensorDataCompensatorMatrix )
    {
public:
    /**
    * C++ default constructor. Sets the identity values by default.
    */
    TSensorDataCompensatorMatrix();

    /**
    * Sets the identity values. Can be used for resetting the matrix.
    */
    void SetIdentity();

#ifdef _DEBUG
    /**
    * Multiplies two matrices
    *
    * @since S60 5.0
    * @param aMatrix Matrix to multiply with
    * @return Result matrix
    */
    inline TSensorDataCompensatorMatrix operator*( const TSensorDataCompensatorMatrix& aMatrix ) const
        { return Multiply( aMatrix ); }
#endif // _DEBUG

    /**
    * Multiplies two matrices and stores the result matrix
    *
    * @since S60 5.0
    * @param aMatrix Matrix to multiply with
    * @return Result matrix
    */
    inline TSensorDataCompensatorMatrix& operator*=( const TSensorDataCompensatorMatrix& aMatrix )
        { *this = Multiply( aMatrix ); return *this; }

#ifdef _DEBUG
    /**
    * Multiplies matrix by value
    *
    * @since S60 5.0
    * @param aValue Value to multiply with
    * @return Result matrix
    */
    TSensorDataCompensatorMatrix operator*( const FValue& aValue ) const;

    /**
    * Multiplies matrix by value and stores the result matrix
    *
    * @since S60 5.0
    * @param aValue Value to multiply with
    * @return Result matrix
    */
    TSensorDataCompensatorMatrix& operator*=( const FValue& aValue );
#endif // _DEBUG

    /**
    * Sets the values from rotation around x-axis. Existing values are overwritten.
    *
    * @since S60 5.0
    * @param aXAngle Angle to rotate in degrees
    */
    void SetRotateX( TInt aXAngle );

    /**
    * Sets the values from rotation around y-axis. Existing values are overwritten.
    *
    * @since S60 5.0
    * @param aYAngle Angle to rotate in degrees
    */
    void SetRotateY( TInt aYAngle );

    /**
    * Sets the values from rotation around z-axis. Existing values are overwritten.
    *
    * @since S60 5.0
    * @param aZAngle Angle to rotate in degrees
    */
    void SetRotateZ( TInt aZAngle );

    /**
    * Transforms x,y and z values using the matrix values
    *
    * @since S60 5.0
    * @param aOutputX Transformed x value
    * @param aOutputY Transformed x value
    * @param aOutputZ Transformed x value
    * @param aInputX X value to be transformed
    * @param aInputY Y value to be transformed
    * @param aInputZ Z value to be transformed
    */
    void Transform( TInt& aOutputX, TInt& aOutputY, TInt& aOutputZ,
                    TInt aInputX, TInt aInputY, TInt aInputZ );

    /**
    * Returns matrix item from given index.
    * See TSensorDataCompensatorMatrixItem for valid item indices.
    *
    * @since S60 5.0
    * @param aIndex Item index
    */
    inline FValue operator[](TInt aIndex) const { return iMatrix[ aIndex ]; }

    /**
    * Returns matrix item from given index.
    * See TSensorDataCompensatorMatrixItem for valid item indices.
    *
    * @since S60 5.0
    * @param aIndex Item index
    */
    inline FValue& operator[](TInt aIndex) { return iMatrix[ aIndex ]; }

#ifdef _DEBUG
    /**
    * Checks if rotation matrix is valid
    *
    * @since S60 5.0
    * @return ETrue if valid. Otherwise EFalse.
    */
    TBool IsValidRotationMatrix() const;
#endif // _DEBUG

private:
    TSensorDataCompensatorMatrix Multiply(
        const TSensorDataCompensatorMatrix& aMatrix ) const;

#ifdef _DEBUG
    static TBool Compare( const TSensorDataCompensatorMatrix& aLeft,
                          const TSensorDataCompensatorMatrix& aRight );

    TSensorDataCompensatorMatrix Transpose() const;

    TSensorDataCompensatorMatrix Inverse() const;

    static TBool Equals( FValue aLeft, FValue aRight, FValue aTolerance );

    FValue Det3() const;

    static FValue Det2( FValue a11, FValue a12, FValue a21, FValue a22);
#endif // _DEBUG

private: // Data
    /**
     * Stores the matrix items
     */
#ifdef _DEBUG
    TFixedArray<FValue, EMatrixItemCount> iMatrix; // For debug access
#else // _DEBUG
    FValue iMatrix[ EMatrixItemCount ]; // For fast access
#endif // _DEBUG

    };


#endif // T_SENSORDATACOMPENSATORMATRIX_H
