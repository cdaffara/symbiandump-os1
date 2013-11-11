// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#ifndef __EGLTEST_PARAMETERS_H__
#define __EGLTEST_PARAMETERS_H__


#include <e32base.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>


//This table defines the candidate constants that could be used as bad
//parameters. They are candidate becuase they will be evaluated at runtime and
//if any conflict with known good params, they will be excluded. When creating
//the dynamic list, non static params are added such as [validParam] + 1.
const TInt KCandidateBadParams[] =
    {
    0,
    4,
    5,
    0xBADFEE0,
    0xBADFEED
    };
const TInt KNumCandidateBadParams = sizeof(KCandidateBadParams)/sizeof(KCandidateBadParams[0]);


template <typename T>
class CParameters : public CBase
    {
public:
    static CParameters* NewLC(TBool aCreateBadParams, const RArray<T>& aValidParamsArray, TInt aIndex);
    static CParameters* NewLC(TBool aCreateBadParams, const T* aValidParamsArray, TInt aCount, TInt aIndex);
    static CParameters* NewLC(TBool aCreateBadParams, T aValidParam);
    ~CParameters();
    TInt Count() const;
    T& operator[](TInt aIndex);
    const T& operator[](TInt aIndex) const;

private:
    CParameters();
    void ConstructL(TBool aCreateBadParams, const RArray<T>& aValidParamsArray, TInt aIndex);
    void ConstructL(TBool aCreateBadParams, const T* aValidParamsArray, TInt aCount, TInt aIndex);
    void ConstructL(TBool aCreateBadParams, T aValidParam);

    void MakeBadParamsL(const RArray<T>& aValidParamsArray);
    void MakeGoodParamsL(const RArray<T>& aValidParamsArray, TInt aIndex);

    TBool IsErrorValue(const T aValue) const;
private:
    RArray<T> iParams;
    };


typedef CParameters<EGLDisplay> CDisplayParams;
typedef CParameters<EGLEndpointNOK> CEndpointParams;
typedef CParameters<EGLImageKHR> CImageParams;
typedef CParameters<EGLenum> CEnumParams;
typedef CParameters<EGLint> CIntParams;
typedef CParameters<TSurfaceId> CSurfaceIdParams;
typedef CParameters<TRequestStatus *> CSyncParams;

template <typename T>
CParameters<T>* CParameters<T>::NewLC(TBool aCreateBadParams, const RArray<T>& aValidParamsArray, TInt aIndex)
    {
    CParameters<T>* obj = new (ELeave) CParameters();
    CleanupStack::PushL(obj);
    obj->ConstructL(aCreateBadParams, aValidParamsArray);
    return obj;
    }


template <typename T>
CParameters<T>* CParameters<T>::NewLC(TBool aCreateBadParams, const T* aValidParamsArray, TInt aCount, TInt aIndex)
    {
    CParameters<T>* obj = new (ELeave) CParameters();
    CleanupStack::PushL(obj);
    obj->ConstructL(aCreateBadParams, aValidParamsArray, aCount, aIndex);
    return obj;
    }


template <typename T>
CParameters<T>* CParameters<T>::NewLC(TBool aCreateBadParams, T aValidParam)
    {
    CParameters<T>* obj = new (ELeave) CParameters();
    CleanupStack::PushL(obj);
    obj->ConstructL(aCreateBadParams, aValidParam);
    return obj;
    }

template <typename T>
CParameters<T>::CParameters()
    {
    }


template <typename T>
void CParameters<T>::ConstructL(TBool aCreateBadParams, const RArray<T>& aValidParamsArray, TInt aIndex)
    {
    if(aCreateBadParams)
        {
        MakeBadParamsL(aValidParamsArray);
        }
    else
        {
        MakeGoodParamsL(aValidParamsArray, aIndex);
        }
    }


template <typename T>
void CParameters<T>::ConstructL(TBool aCreateBadParams, const T* aValidParamsArray, TInt aCount, TInt aIndex)
    {
    RArray<T> validParamsArray;
    CleanupClosePushL(validParamsArray);
    for(TInt i=0; i < aCount; i++)
        {
        validParamsArray.AppendL(aValidParamsArray[i]);
        }
    ConstructL(aCreateBadParams, validParamsArray, aIndex);
    CleanupStack::PopAndDestroy(1);
    }


template <typename T>
void CParameters<T>::ConstructL(TBool aCreateBadParams, T aValidParam)
    {
    ConstructL(aCreateBadParams, &aValidParam, 1, 0);
    }


template <typename T>
void CParameters<T>::MakeGoodParamsL(const RArray<T>& aValidParamsArray, TInt aIndex)
    {
    ASSERT(iParams.Count() == 0);
    iParams.AppendL(aValidParamsArray[aIndex]);
    }


template <typename T>
void CParameters<T>::MakeBadParamsL(const RArray<T>& aValidParamsArray)
    {
    RArray<T> candidateParams;
    CleanupClosePushL(candidateParams);

    //Pack the static bad params into an candidateParams.
    for(TInt i=0; i < KNumCandidateBadParams; i++)
        {
        candidateParams.AppendL((T)KCandidateBadParams[i]);
        }

    //Pack the dynamic bad params (ie validParam+1 for each valid param).
    for(TInt i=0; i < aValidParamsArray.Count(); i++)
        {
        candidateParams.AppendL((T)((TInt)aValidParamsArray[i] + 1));
        }

    //Iterate over candidateParams and add them to the final
    //bad params array if they don't clash with any valid params.
    for(TInt i=0; i < candidateParams.Count(); i++)
        {
        TBool useCurParam = ETrue;
        for(TInt n=0; n < aValidParamsArray.Count(); n++)
            {
            if(!IsErrorValue(aValidParamsArray[n]) && candidateParams[i] == aValidParamsArray[n])
                {
                useCurParam = EFalse;
                break;
                }
            }
        // Check if it's already in the list - no point in testing the same value more than once.
        for(TInt j = 0; useCurParam && j < iParams.Count(); j++)
            {
            if (candidateParams[i] == iParams[j])
                {
                useCurParam = EFalse;
                }
            }

        if(useCurParam)
            {
            iParams.AppendL(candidateParams[i]);
            }
        }

    //Final bad params are in iParams. Delete candidateParams.
    CleanupStack::PopAndDestroy(1);
    }

template <typename T>
TBool CParameters<T>::IsErrorValue(const T aValue) const
    {
    return aValue == 0;   // Default value, eg. EGL_NO_DISPLAY
    }

template <>
TBool CParameters<TSurfaceId>::IsErrorValue(const TSurfaceId aValue) const
    {
    return aValue.IsNull();
    }

template <typename T>
CParameters<T>::~CParameters()
    {
    iParams.Close();
    }


template <typename T>
TInt CParameters<T>::Count() const
    {
    return iParams.Count();
    }


template <typename T>
T& CParameters<T>::operator[](TInt aIndex)
    {
    return iParams[aIndex];
    }


template <typename T>
const T& CParameters<T>::operator[](TInt aIndex) const
    {
    return iParams[aIndex];
    }

template<>
void CParameters<TSurfaceId>::MakeBadParamsL(const RArray<TSurfaceId>& aValidParamsArray)
    {
    RArray<TSurfaceId> candidateParams;
    CleanupClosePushL(candidateParams);

    static const TSurfaceId KCandidateSurfaceBadParams[]=
    {
            { 0 },
            { 12346, 18129, 192121, 98198 },
            { 11111, 22222, 222222, 33333 },
            { -1, -1, -1, -1 },
            { 0x80000000, 0x80000000, 0x80000000, 0x80000000 },
            { 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF },
    };
    //Pack the static bad params into an candidateParams.
    for(TInt i=0; i < KNumCandidateBadParams; i++)
        {
        candidateParams.AppendL(KCandidateSurfaceBadParams[i]);
        }

    //Pack the dynamic bad params (ie validParam+1 for each valid param).
    for(TInt i=0; i < aValidParamsArray.Count(); i++)
        {
        TSurfaceId id = aValidParamsArray[i];
        id.iInternal[3]++;
        candidateParams.AppendL(id);
        }

    RSurfaceManager surfManager;
    User::LeaveIfError(surfManager.Open());
    CleanupClosePushL(surfManager);
    //Iterate over candidateParams and add them to the final
    //bad params array if they don't clash with any valid params.
    for(TInt i=0; i < candidateParams.Count(); i++)
        {
        TBool useCurParam = ETrue;
        for(TInt n=0; n < aValidParamsArray.Count(); n++)
            {
            TInt err = surfManager.OpenSurface(candidateParams[i]);
            if(err == KErrNone && !IsErrorValue(aValidParamsArray[n]) && candidateParams[i] == aValidParamsArray[n])
                {
                // It is unlikely we get here for ANY reason, but we always close the surface -
                // it may not have successfully opened, but it's fine to do even if we didn't successfully open
                // the surface. Just ignore any error.
                surfManager.CloseSurface(candidateParams[i]);
                useCurParam = EFalse;
                break;
                }
            }
        if(useCurParam)
            {
            iParams.AppendL(candidateParams[i]);
            }
        }
    //Final bad params are in iParams. Delete candidateParams, and close surfManager.
    CleanupStack::PopAndDestroy(2);
    }

template<>
void CParameters<TRequestStatus *>::MakeBadParamsL(const RArray<TRequestStatus *>& /* aValidParamsArray */)
    {
    // Currently, the only TRequestStatus that we can test for is NULL.
    // Since TRequestStatus can be any valid memory, it is impossible to
    // test for "bad" values by any simple measure - the app will panic
    // if random address is passed along, but there is no easy way to determine
    // if it's a valid or invalid address.
    iParams.AppendL(0);
    }

#endif
