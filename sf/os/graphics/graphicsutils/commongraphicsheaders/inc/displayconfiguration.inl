// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @prototype
*/

#ifndef _DISPLAYCONFIGURATION_INL__INCLUDED_
#define _DISPLAYCONFIGURATION_INL__INCLUDED_

//
//
// Complete implementation of Base class
//
//
inline TInt TDisplayConfigurationBase::Version() const
	{
	return iVersion;
	}

inline void TDisplayConfigurationBase::Clear(TAttribute aAttribute)
	{
	iDefined.Clear(+aAttribute);
	}

inline void TDisplayConfigurationBase::ClearAll()
	{
	iDefined.ClearAll();
	}

inline void TDisplayConfigurationBase::Panic(TInt aPanicCode)
	{
	_LIT(KCategory,"DISPLAYCONFIG");
	User::Panic(KCategory,aPanicCode);
	}

inline TBool TDisplayConfigurationBase::IsDefined(TAttribute aAttribute) const
	{
	//If we have more than 32 attributes then this code may need a rewrite.
	__ASSERT_COMPILE(EAttributeMax<=32);
	return  iDefined.IsSet(+aAttribute);
	}

inline TDisplayConfigurationBase::TDisplayConfigurationBase(TInt aVersion)
	{
	if (aVersion < sizeof(*this))
		{
		Panic(EPanicConfigInvalid);
		}
	iVersion=aVersion;
	//default constructor for iDefined sets all-zero.
	//Members do not need zapping to zero because they are disabled in iDefined
	}


inline TBool TDisplayConfigurationBase::operator == (const TDisplayConfigurationBase& aRhs) const
	{
	return (aRhs.Version()== Version()) && (const_cast<TBitFlags32&>(aRhs.iDefined) == iDefined );
	}
//
//
// Complete implementation of V1 class
//
//
inline TDisplayConfiguration1::TDisplayConfiguration1():TDisplayConfigurationBase(sizeof(*this))
		,iResolution(TSize::EUninitialized)
	{}

inline TDisplayConfiguration1::TDisplayConfiguration1(TInt aVersion):TDisplayConfigurationBase(aVersion)
		,iResolution(TSize::EUninitialized)
	{
	if (aVersion > sizeof(TDisplayConfigurationBase) &&
			aVersion < sizeof(*this))
		{
		Panic(EPanicConfigInvalid);
		}
	}

inline void TDisplayConfiguration1::SetResolution(const TSize& aSize)
	{
	if (MemberAccessible(iResolution))
		{
		if (aSize.iWidth<0 || aSize.iHeight<0)
			{
			Panic(EPanicNegResolution);
			}
		if ((aSize.iWidth>0 && aSize.iHeight==0)||
				(aSize.iWidth==0 && aSize.iHeight>0))
			{
			Panic(EPanicSemiZeroResolution);
			}
		iDefined.Set(+EResolution);
		iResolution=aSize;
		}
	}

inline TBool TDisplayConfiguration1::GetResolution(TSize& aSize) const
	{
	if (MemberAccessible(iResolution) && IsDefined(EResolution))
		{
		aSize=iResolution;
		return ETrue;
		}		
	return  EFalse;
	}

//Accessor for rotation member (index)
inline void TDisplayConfiguration1::SetRotation(TRotation aOrientation)
	{
	if (MemberAccessible(iRotation))
		{
		if ((TUint)aOrientation>=(TUint)ERotationIllegal)
			{
			Panic(EPanicIllegalRotation);
			}
		iDefined.Set(+ERotation);
		iRotation=aOrientation;
		}
	
	}
//Accessor for rotation member (index)
inline TBool TDisplayConfiguration1::GetRotation(TRotation& aOrientation)const
	{
	if (MemberAccessible(iRotation) && IsDefined(ERotation))
		{
		aOrientation=static_cast<TRotation>(iRotation);
		return ETrue;
		}
	return EFalse;
	}
//Accessor for resolution value
inline void TDisplayConfiguration1::SetResolutionTwips(const TSize& aSize)
	{
	if (MemberAccessible(iTwipsSize))
		{
		if (aSize.iWidth<0 || aSize.iHeight<0)
			{
			Panic(EPanicNegTwips);
			}
		if ((aSize.iWidth>0 && aSize.iHeight==0)||
				(aSize.iWidth==0 && aSize.iHeight>0))
			{
			Panic(EPanicSemiZeroTwips);
			}
		iDefined.Set(+EResolutionTwips);
		iTwipsSize=aSize;
		}
	}
//Accessor for resolution value
inline TBool TDisplayConfiguration1::GetResolutionTwips(TSize& aSize) const
	{
	if (MemberAccessible(iTwipsSize) && IsDefined(EResolutionTwips))
		{
		aSize=iTwipsSize;
		return ETrue;
		}
	return EFalse;
	}

inline TBool TDisplayConfiguration1::operator == (const TDisplayConfiguration1& aRhs)const
	{
	if (!TDisplayConfigurationBase::operator ==(aRhs))
		return EFalse;	//Check for earlier version compatability
	if (Version()<sizeof(*this))
		return ETrue;	//If the objects are smaller than this then the check is complete
	
	TBool isAttrEqual = ETrue;
	if(IsDefined(EResolution))
		{
		isAttrEqual = isAttrEqual&(aRhs.iResolution == iResolution);
		}
	if(IsDefined(ERotation))
		{
		isAttrEqual = isAttrEqual&(aRhs.iRotation == iRotation);
		}
	if(IsDefined(EResolutionTwips))
		{
		isAttrEqual = isAttrEqual&(aRhs.iTwipsSize == iTwipsSize);
		}
	
	return isAttrEqual;
	}

//
//
// Complete implementation of v2 class (goes here)
//
//


//
//
// Complete implementation of Wrapper class
//
//
inline TDisplayConfiguration::TDisplayConfiguration(const TDisplayConfiguration& aDisplayConfiguration):
	TDisplayConfigurationTop(TDisplayConfiguration().Version())
	{
	TInt tempSize = aDisplayConfiguration.Version();
	if (tempSize > TDisplayConfiguration().Version())
		{
		tempSize = TDisplayConfiguration().Version();
		}
	Mem::Copy(this, &aDisplayConfiguration, tempSize);
	//This has copied size for higher version features...
	iVersion = tempSize;
	//This has copied set flags for higher version features...
	TInt flags=iDefined.Value();
	//If we have more than 32 attributes then this code will need a rewrite.
	__ASSERT_COMPILE(EAttributeMax<=32);
	flags&=(1<<EAttributeMax)-1;
	iDefined.SetValue(flags);
	}

inline TDisplayConfiguration::TDisplayConfiguration(TInt aVersion):
	TDisplayConfigurationTop(aVersion)
	{
	if (TDisplayConfiguration().Version() > aVersion)
		{
		iVersion = aVersion;
		}
	}

#endif // _DISPLAYCONFIGURATION__INCLUDED_
