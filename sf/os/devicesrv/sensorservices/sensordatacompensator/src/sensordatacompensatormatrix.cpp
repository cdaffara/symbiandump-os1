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


#include "sensordatacompensatormatrix.h"
#include "trace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// TSensorDataCompensatorMatrix::TSensorDataCompensatorMatrix
// ---------------------------------------------------------------------------
//
TSensorDataCompensatorMatrix::TSensorDataCompensatorMatrix()
    {
    FUNC_LOG;
    SetIdentity();
    }

// ---------------------------------------------------------------------------
// TSensorDataCompensatorMatrix::SetIdentity
// ---------------------------------------------------------------------------
//
void TSensorDataCompensatorMatrix::SetIdentity()
    {
    FUNC_LOG;
    const FValue KOne(1);
    const FValue KZero(0);

    iMatrix[ EMatrixItem11 ] = KOne;
    iMatrix[ EMatrixItem12 ] = KZero;
    iMatrix[ EMatrixItem13 ] = KZero;

    iMatrix[ EMatrixItem21 ] = KZero;
    iMatrix[ EMatrixItem22 ] = KOne;
    iMatrix[ EMatrixItem23 ] = KZero;

    iMatrix[ EMatrixItem31 ] = KZero;
    iMatrix[ EMatrixItem32 ] = KZero;
    iMatrix[ EMatrixItem33 ] = KOne;
    }

#ifdef _DEBUG
// ---------------------------------------------------------------------------
// TSensorDataCompensatorMatrix::operator*
// ---------------------------------------------------------------------------
//
TSensorDataCompensatorMatrix TSensorDataCompensatorMatrix::operator*(
        const FValue& aValue ) const
    {
    FUNC_LOG;
    TSensorDataCompensatorMatrix ret;
    for ( TInt i( 0 ) ; i < EMatrixItemCount ; ++i )
        {
        ret.iMatrix[ i ] = iMatrix[ i ] * aValue;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// TSensorDataCompensatorMatrix::operator*=
// ---------------------------------------------------------------------------
//
TSensorDataCompensatorMatrix& TSensorDataCompensatorMatrix::operator*=(
        const FValue& aValue )
    {
    FUNC_LOG;
    for ( TInt i( 0 ) ; i < EMatrixItemCount ; ++i )
        {
        iMatrix[ i ] *= aValue;
        }
    return *this;
    }
#endif // _DEBUG

// ---------------------------------------------------------------------------
// TSensorDataCompensatorMatrix::SetRotateX
// ---------------------------------------------------------------------------
void TSensorDataCompensatorMatrix::SetRotateX( TInt aXAngle )
    {
    FUNC_LOG;
    FValue s = FSin( aXAngle );
    FValue c = FCos( aXAngle );

    SetIdentity();

    iMatrix[ EMatrixItem22 ] = c;
    iMatrix[ EMatrixItem23 ] = -s;
    iMatrix[ EMatrixItem32 ] = s;
    iMatrix[ EMatrixItem33 ] = c;
    }

// ---------------------------------------------------------------------------
// TSensorDataCompensatorMatrix::SetRotateY
// ---------------------------------------------------------------------------
void TSensorDataCompensatorMatrix::SetRotateY( TInt aYAngle )
    {
    FUNC_LOG;
    FValue s = FSin( aYAngle );
    FValue c = FCos( aYAngle );

    SetIdentity();

    iMatrix[ EMatrixItem11 ] = c;
    iMatrix[ EMatrixItem13 ] = s;
    iMatrix[ EMatrixItem31 ] = -s;
    iMatrix[ EMatrixItem33 ] = c;
    }

// ---------------------------------------------------------------------------
// TSensorDataCompensatorMatrix::SetRotateZ
// ---------------------------------------------------------------------------
void TSensorDataCompensatorMatrix::SetRotateZ( TInt aZAngle )
    {
    FUNC_LOG;
    FValue s = FSin( aZAngle );
    FValue c = FCos( aZAngle );

    SetIdentity();

    iMatrix[ EMatrixItem11 ] = c;
    iMatrix[ EMatrixItem12 ] = -s;
    iMatrix[ EMatrixItem21 ] = s;
    iMatrix[ EMatrixItem22 ] = c;
    }

// ---------------------------------------------------------------------------
// TSensorDataCompensatorMatrix::Transform
// ---------------------------------------------------------------------------
void TSensorDataCompensatorMatrix::Transform( TInt& aOutputX, 
                                              TInt& aOutputY,
                                              TInt& aOutputZ,
                                              TInt aInputX, 
                                              TInt aInputY,
                                              TInt aInputZ )
    {
    FUNC_LOG;

    FValue x(aInputX);
    FValue y(aInputY);
    FValue z(aInputZ);
    aOutputX = FRoundToInt( iMatrix[EMatrixItem11] * x + 
                            iMatrix[EMatrixItem12] * y + 
                            iMatrix[EMatrixItem13] * z );
    aOutputY = FRoundToInt( iMatrix[EMatrixItem21] * x + 
                            iMatrix[EMatrixItem22] * y + 
                            iMatrix[EMatrixItem23] * z );
    aOutputZ = FRoundToInt( iMatrix[EMatrixItem31] * x + 
                            iMatrix[EMatrixItem32] * y + 
                            iMatrix[EMatrixItem33] * z );
    }

#ifdef _DEBUG
// ---------------------------------------------------------------------------
// TSensorDataCompensatorMatrix::IsValidRotationMatrix
// ---------------------------------------------------------------------------
//
TBool TSensorDataCompensatorMatrix::IsValidRotationMatrix() const
    {
    FUNC_LOG;
    TBool ret( ETrue );

    //Check if determinant is +1
    FValue det( Det3() );
    if ( !( Equals( static_cast<FValue>( 1 ), det, 
                    static_cast<FValue>( 0.01 ) ) ) )
        {

        ret = EFalse;
        }
    else
        {
        TSensorDataCompensatorMatrix trans( Transpose() );
        TSensorDataCompensatorMatrix inv( Inverse() );

        // Check if iMatrix' transpose is it's inverse
        if ( EFalse != Compare( trans, inv ) )
            {
            INFO( "iMatrix transpose != iMatrix inverse -> not a valid rotation iMatrix" );

            ret = EFalse;
            }
        }
    return ret;
    }
#endif // _DEBUG

// ---------------------------------------------------------------------------
// TSensorDataCompensatorMatrix::Multiply
// ---------------------------------------------------------------------------
//
TSensorDataCompensatorMatrix TSensorDataCompensatorMatrix::Multiply(
        const TSensorDataCompensatorMatrix& aMatrix ) const
    {
    FUNC_LOG;
    TSensorDataCompensatorMatrix ret;

    // 1st row
    ret.iMatrix[EMatrixItem11] =
        iMatrix[EMatrixItem11] * aMatrix.iMatrix[EMatrixItem11] +
        iMatrix[EMatrixItem12] * aMatrix.iMatrix[EMatrixItem21] +
        iMatrix[EMatrixItem13] * aMatrix.iMatrix[EMatrixItem31];

    ret.iMatrix[EMatrixItem12] =
        iMatrix[EMatrixItem11] * aMatrix.iMatrix[EMatrixItem12] +
        iMatrix[EMatrixItem12] * aMatrix.iMatrix[EMatrixItem22] +
        iMatrix[EMatrixItem13] * aMatrix.iMatrix[EMatrixItem32];

    ret.iMatrix[EMatrixItem13] =
        iMatrix[EMatrixItem11] * aMatrix.iMatrix[EMatrixItem13] +
        iMatrix[EMatrixItem12] * aMatrix.iMatrix[EMatrixItem23] +
        iMatrix[EMatrixItem13] * aMatrix.iMatrix[EMatrixItem33];

    // 2nd row
    ret.iMatrix[EMatrixItem21] =
        iMatrix[EMatrixItem21] * aMatrix.iMatrix[EMatrixItem11] +
        iMatrix[EMatrixItem22] * aMatrix.iMatrix[EMatrixItem21] +
        iMatrix[EMatrixItem23] * aMatrix.iMatrix[EMatrixItem31];

    ret.iMatrix[EMatrixItem22] =
        iMatrix[EMatrixItem21] * aMatrix.iMatrix[EMatrixItem12] +
        iMatrix[EMatrixItem22] * aMatrix.iMatrix[EMatrixItem22] +
        iMatrix[EMatrixItem23] * aMatrix.iMatrix[EMatrixItem32];

    ret.iMatrix[EMatrixItem23] =
        iMatrix[EMatrixItem21] * aMatrix.iMatrix[EMatrixItem13] +
        iMatrix[EMatrixItem22] * aMatrix.iMatrix[EMatrixItem23] +
        iMatrix[EMatrixItem23] * aMatrix.iMatrix[EMatrixItem33];

    // 3rd row
    ret.iMatrix[EMatrixItem31] =
        iMatrix[EMatrixItem31] * aMatrix.iMatrix[EMatrixItem11] +
        iMatrix[EMatrixItem32] * aMatrix.iMatrix[EMatrixItem21] +
        iMatrix[EMatrixItem33] * aMatrix.iMatrix[EMatrixItem31];

    ret.iMatrix[EMatrixItem32] =
        iMatrix[EMatrixItem31] * aMatrix.iMatrix[EMatrixItem12] +
        iMatrix[EMatrixItem32] * aMatrix.iMatrix[EMatrixItem22] +
        iMatrix[EMatrixItem33] * aMatrix.iMatrix[EMatrixItem32];

    ret.iMatrix[EMatrixItem33] =
        iMatrix[EMatrixItem31] * aMatrix.iMatrix[EMatrixItem13] +
        iMatrix[EMatrixItem32] * aMatrix.iMatrix[EMatrixItem23] +
        iMatrix[EMatrixItem33] * aMatrix.iMatrix[EMatrixItem33];

    return ret;    
    }

#ifdef _DEBUG

// ---------------------------------------------------------------------------
// TSensorDataCompensatorMatrix::Compare
// ---------------------------------------------------------------------------
//
TBool TSensorDataCompensatorMatrix::Compare(
    const TSensorDataCompensatorMatrix& aLeft, const TSensorDataCompensatorMatrix& aRight )
    {
    FUNC_LOG;
    const FValue KMatrixTolerance( 0.01 );
    TBool status( ETrue );
    TInt x( EMatrixItemCount );
    while ( x && status )
        {
        --x;
        status = Equals( aLeft[x], aRight[x], KMatrixTolerance );
        }
    return status;
    }

// ---------------------------------------------------------------------------
// TSensorDataCompensatorMatrix::Transpose
// ---------------------------------------------------------------------------
//
TSensorDataCompensatorMatrix TSensorDataCompensatorMatrix::Transpose() const
    {
    FUNC_LOG;
    TSensorDataCompensatorMatrix trans;

    trans[EMatrixItem11] = iMatrix[EMatrixItem11];
    trans[EMatrixItem21] = iMatrix[EMatrixItem12];
    trans[EMatrixItem31] = iMatrix[EMatrixItem13];

    trans[EMatrixItem12] = iMatrix[EMatrixItem21];
    trans[EMatrixItem22] = iMatrix[EMatrixItem22];
    trans[EMatrixItem32] = iMatrix[EMatrixItem23];

    trans[EMatrixItem13] = iMatrix[EMatrixItem31];
    trans[EMatrixItem23] = iMatrix[EMatrixItem32];
    trans[EMatrixItem33] = iMatrix[EMatrixItem33];

    return trans;
    }

// ---------------------------------------------------------------------------
// TSensorDataCompensatorMatrix::Inverse
// ---------------------------------------------------------------------------
//
TSensorDataCompensatorMatrix TSensorDataCompensatorMatrix::Inverse() const
    {
    FUNC_LOG;
    //Calculate the adjugate iMatrix and determinate
    //Inversion iMatrix is adjugate * (1/determinate)

    TSensorDataCompensatorMatrix adj;

    FValue det( Det3() );

    const FValue KDetermTolerance( 0.01 );
    if ( !( Equals( static_cast<FValue>( 0 ), det, KDetermTolerance ) ) )
        {
        return adj;
        }

    adj[ EMatrixItem11 ] = Det2( iMatrix[ EMatrixItem22 ],
                                        iMatrix[ EMatrixItem32 ],
                                        iMatrix[ EMatrixItem23 ],
                                        iMatrix[ EMatrixItem33 ] );
    
    adj[ EMatrixItem12 ] =-Det2( iMatrix[ EMatrixItem12 ],
                                        iMatrix[ EMatrixItem32 ],
                                        iMatrix[ EMatrixItem13 ],
                                        iMatrix[ EMatrixItem33 ] );
    
    adj[ EMatrixItem13 ] = Det2( iMatrix[ EMatrixItem12 ],
                                        iMatrix[ EMatrixItem22 ],
                                        iMatrix[ EMatrixItem13 ],
                                        iMatrix[ EMatrixItem23 ] );

    adj[ EMatrixItem21 ] =-Det2( iMatrix[ EMatrixItem21 ],
                                        iMatrix[ EMatrixItem31 ],
                                        iMatrix[ EMatrixItem23 ],
                                        iMatrix[ EMatrixItem33 ] );
    
    adj[ EMatrixItem22 ] = Det2( iMatrix[ EMatrixItem11 ],
                                        iMatrix[ EMatrixItem31 ],
                                        iMatrix[ EMatrixItem13 ],
                                        iMatrix[ EMatrixItem33 ] );
    
    adj[ EMatrixItem23 ] =-Det2( iMatrix[ EMatrixItem11 ],
                                        iMatrix[ EMatrixItem21 ],
                                        iMatrix[ EMatrixItem13 ],
                                        iMatrix[ EMatrixItem23 ] );

    adj[ EMatrixItem31 ] = Det2( iMatrix[ EMatrixItem21 ],
                                        iMatrix[ EMatrixItem31 ],
                                        iMatrix[ EMatrixItem12 ],
                                        iMatrix[ EMatrixItem32 ] );
    
    adj[ EMatrixItem32 ] =-Det2( iMatrix[ EMatrixItem11 ],
                                        iMatrix[ EMatrixItem31 ],
                                        iMatrix[ EMatrixItem12 ],
                                        iMatrix[ EMatrixItem32 ] );
    
    adj[ EMatrixItem33 ] = Det2( iMatrix[ EMatrixItem11 ],
                                        iMatrix[ EMatrixItem21 ],
                                        iMatrix[ EMatrixItem12 ],
                                        iMatrix[ EMatrixItem22 ] );

    return ( adj * ( ( static_cast<FValue>( 1 ) ) / det) );
    }

// ---------------------------------------------------------------------------
// TSensorDataCompensatorMatrix::Equals
// ---------------------------------------------------------------------------
//
TBool TSensorDataCompensatorMatrix::Equals( FValue aLeft, FValue aRight, FValue aTolerance )
    {
    FUNC_LOG;
    return ( FAbs( aLeft - aRight ) < aTolerance );
    }

// ---------------------------------------------------------------------------
// TSensorDataCompensatorMatrix::Det3
// ---------------------------------------------------------------------------
//
FValue TSensorDataCompensatorMatrix::Det3() const
    {
    FUNC_LOG;
    // Use Sarrus's rule
    FValue ret( iMatrix[EMatrixItem11] * 
                iMatrix[EMatrixItem22] * 
                iMatrix[EMatrixItem33] + 
                iMatrix[EMatrixItem21] * 
                iMatrix[EMatrixItem32] * 
                iMatrix[EMatrixItem13] +
                iMatrix[EMatrixItem31] * 
                iMatrix[EMatrixItem12] * 
                iMatrix[EMatrixItem23] -
                iMatrix[EMatrixItem31] * 
                iMatrix[EMatrixItem22] * 
                iMatrix[EMatrixItem13] - 
                iMatrix[EMatrixItem21] * 
                iMatrix[EMatrixItem12] * 
                iMatrix[EMatrixItem33] - 
                iMatrix[EMatrixItem11] * 
                iMatrix[EMatrixItem32] * 
                iMatrix[EMatrixItem23] );
    return ret;
    }

// ---------------------------------------------------------------------------
// TSensorDataCompensatorMatrix::Det2
// ---------------------------------------------------------------------------
//
FValue TSensorDataCompensatorMatrix::Det2( FValue a11, FValue a12, FValue a21, FValue a22 )
    {
    FUNC_LOG;
    FValue ret ( a11 * a22 - a12 * a21 );
    return ret;
    }

#endif // _DEBUG
