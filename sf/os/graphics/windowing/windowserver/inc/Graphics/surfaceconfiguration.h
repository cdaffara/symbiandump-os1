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

#ifndef SURFACECONFIGURATION_H
#define SURFACECONFIGURATION_H

#include <e32base.h>
#include <graphics/surface.h>
#include <bitstd.h>

/** 
The base class for the surface configuration containing the size of the surface configuration.  
Provides a Size function to help identify which attributes are available.

@publishedPartner
@prototype
*/
class TSurfaceConfigurationSize
{
public:
	inline TInt Size() const;
protected:
	inline TSurfaceConfigurationSize(TInt aSize);
	template <class Member> TBool MemberAccessible (const Member& aMember) const
		{
		return iSize>=sizeof(Member)+TInt(&aMember)-TInt(this);
		}
	inline TBool operator == (const TSurfaceConfigurationSize& aRhs)const;
private:	
	inline TSurfaceConfigurationSize();
	inline TSurfaceConfigurationSize(const TSurfaceConfigurationSize& /*aSurfaceConfigurationSize*/);
	inline TSurfaceConfigurationSize operator = (const TSurfaceConfigurationSize& /*aRhs*/)const;
protected:
	TInt iSize;
};

/** 
The first set of surface configuration attributes containing the surface id, viewport, extent
and orientation of the surface, along with a SupportsAllMembers function to help identify which 
attributes are available.

@publishedPartner
@prototype
*/
class TSurfaceConfiguration1: public TSurfaceConfigurationSize
{
public:
	inline TSurfaceConfiguration1();
	inline TInt SetSurfaceId (const TSurfaceId& aSurfaceId);
	inline void GetSurfaceId (TSurfaceId& aSurfaceId) const;
	inline TInt SetViewport (const TRect& aViewport);
	inline void GetViewport (TRect& aViewport) const;
	inline TInt SetOrientation (CFbsBitGc::TGraphicsOrientation aOrientation);
	inline CFbsBitGc::TGraphicsOrientation Orientation() const;
	inline TInt SetExtent(const TRect& aExtent);
	inline void GetExtent(TRect& aExtent) const;
	inline TBool SupportsAllMembers();
protected:
	inline TSurfaceConfiguration1(TInt aSize);
	inline TBool operator == (const TSurfaceConfiguration1& aRhs)const;
private:
	inline TSurfaceConfiguration1(const TSurfaceConfiguration1& /*aSurfaceConfiguration1*/);
	inline TSurfaceConfiguration1 operator = (const TSurfaceConfiguration1& /*aRhs*/)const;
private:
	TSurfaceId iSurfaceId;
	CFbsBitGc::TGraphicsOrientation iOrientation;
	TRect iExtent;
	TRect iViewport;
};

/** 
The second set of surface configuration attributes containing the flip attribute of the surface, along with a SupportsAllMembers function to help identify which 
attributes are available.

@publishedPartner
@prototype
*/
class TSurfaceConfiguration2: public TSurfaceConfiguration1
{
public:
    inline TSurfaceConfiguration2();
    inline TInt SetFlip(TBool aFlip);
    inline TBool Flip()const;
    inline TBool SupportsAllMembers();
protected:
    inline TSurfaceConfiguration2(TInt aSize);
    inline TBool operator == (const TSurfaceConfiguration2& aRhs)const;
private:
    inline TSurfaceConfiguration2(const TSurfaceConfiguration2& /*aSurfaceConfiguration2*/);
    inline TSurfaceConfiguration2 operator = (const TSurfaceConfiguration2& /*aRhs*/)const;
    //iFlip is a x-axis flip
    TBool iFlip;
};

/** 
TSurfaceConfiguration is provided as a chain of derived classes representing the growing 
history of the class.

@publishedPartner
@prototype
*/
class TSurfaceConfiguration: public TSurfaceConfiguration2
{
	typedef	TSurfaceConfiguration2	TBiggestConfiguration;
public:
	inline TSurfaceConfiguration();
	inline TSurfaceConfiguration(const TSurfaceConfiguration& aSurfaceConfiguration);
	inline TSurfaceConfiguration(TInt aPreferredMaxSize);
	inline TBool operator == (const TSurfaceConfiguration& aRhs)const
		{
		return TSurfaceConfiguration2::operator==(aRhs);
		}
private:
	inline TSurfaceConfiguration operator = (const TSurfaceConfiguration& /*aRhs*/);
};
#include <graphics/surfaceconfiguration.inl>
#endif // SURFACECONFIGURATION_H
