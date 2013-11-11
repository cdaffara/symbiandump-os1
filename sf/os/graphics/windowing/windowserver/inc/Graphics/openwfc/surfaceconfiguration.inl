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
// Sets all members to default values. Initializes the size field to match the object size.
// 
//
/**
@file
@publishedpartner 
@released
*/

inline TSurfaceConfiguration::TSurfaceConfiguration()
	{}

/**
Copy constructor. This constructor will read the size field of the source and target 
objects and only copy the minimum set of fields indicated. Any remaining fields in the target 
object will be set to default values. 

@param aSurfaceConfiguration Config to copy
*/
inline TSurfaceConfiguration::TSurfaceConfiguration(const TSurfaceConfiguration& aSurfaceConfiguration):
	TSurfaceConfiguration2(0)
	{
	TInt tempSize = aSurfaceConfiguration.Size();
	if (tempSize > sizeof(*this))
		{
		tempSize = sizeof(*this);
		}
	Mem::Copy(this, &aSurfaceConfiguration, tempSize);
	iSize = tempSize;
	}

/**
This constructor will take any value, but it is designed to be used with
a value returned by RWsSession::PreferredSurfaceConfigurationSize, 
and initializes the members to represent an object compatible with that.

If the aPreferredMaxSize is larger than the actual size of TSurfaceConfiguration, indicating 
that the server supports a newer version of the interface than the caller is aware of, then 
the actual size is used.

If the aPreferredMaxSize is smaller than the actual size of TSurfaceConfiguration, indicating 
that the server supports an earlier version of the interface, then this value is used. It must 
match the Size() member of one of the defined hierarchy of TSurfaceConfiguration classes.

@param aPreferredMaxSize User defined max size
@see RWsSession::PreferredSurfaceConfigurationSize
*/
inline TSurfaceConfiguration::TSurfaceConfiguration(TInt aPreferredMaxSize):
	TSurfaceConfiguration2(sizeof(*this))
	{
	if (sizeof(*this) > aPreferredMaxSize)
		{
		iSize = aPreferredMaxSize;
		}
	}

/** Default constructor for configuration 1
*/
inline TSurfaceConfiguration1::TSurfaceConfiguration1():TSurfaceConfigurationSize(sizeof(*this))
		,iExtent(TRect::EUninitialized), iViewport(TRect::EUninitialized)
	{}

/**
Constructor for configuration 1 passing through a size already calculated

@param aSize Size of the class already calculated and being passed up through the classes
*/
inline TSurfaceConfiguration1::TSurfaceConfiguration1(TInt aSize):TSurfaceConfigurationSize(aSize)
		,iExtent(TRect::EUninitialized), iViewport(TRect::EUninitialized)
	{}

/**
The method sets the surface ID to be set as the background surface. 

The default initial value held in this class for surface ID is the null surface ID.

Note that SetBackgroundSurface() will not accept a null surface ID, so this parameter must always be set.

@param aSurfaceId Surface ID to set
@return KErrNone on success or a system-wide error code
	- KErrNotSupported if not supported.
@post The Surface Id values are updated in the object of this class if supported. The 
SetBackgroundSurface API must be called passing object of this class as parameter to 
get the desired effect.
*/
inline TInt TSurfaceConfiguration1::SetSurfaceId (const TSurfaceId& aSurfaceId)
	{
	if (MemberAccessible(iSurfaceId))
		{
		iSurfaceId = aSurfaceId;
		return KErrNone;
		}	
	return KErrNotSupported;
	}

/**
The method gets the surface ID stored in the object of this class.

@param aSurfaceId User surface id to be set
@post The aSurface parameter is filled in.
@post If the instance of TSurfaceConfiguration is too small, the null surface ID will be set.
*/
inline void TSurfaceConfiguration1::GetSurfaceId (TSurfaceId& aSurfaceId) const
	{
	if (MemberAccessible(iSurfaceId))
		{
		aSurfaceId = iSurfaceId;
		}
	else
		{
		aSurfaceId.CreateNullId();
		}
	}

/**
The method sets the area of the surface that should be mapped to the specified window area. 
The viewport is in surface coordinates.

In SetBackgroundSurface(), if not set or if the set viewport is empty, the size of the whole surface 
is used as the viewport. If the surface is changed then the viewport will track the size of 
the surface. 

Setting any ordinate of the viewport outside the surface size is not required specified behavior for 
this interface. The call will not fail if any ordinates are out of range, and the internal NGA Composition 
handling of this case will produce a representative image. This current behavior is that the NGA Composition 
will clamp any out-of-range ordinate to the size of the surface.

@param aViewport A rectangle of the area, in relative coordinates to the surface
@return KErrNone on success or a system-wide error code
	- KErrNotSupported if not supported.
@post The viewport values are updated in the object of this class. This does not imply that the 
viewport is set for the surface. The SetBackgroundSurface API must be called passing an object of 
this class as parameter to get the desired effect.
*/
inline TInt TSurfaceConfiguration1::SetViewport (const TRect& aViewport)
	{
	if (MemberAccessible(iViewport))
		{
		iViewport = aViewport;
		return KErrNone;
		}
	return KErrNotSupported;
	}

/**
This method gets the viewport stored in the object of this class. The viewport is in surface 
coordinates.

If viewport was not set or an empty viewport was set, the value returned is an empty viewport. This 
is an indication that the viewport used by the server is equal to the size of the surface. The 
actual size of the surface is not returned.

@param aViewport User viewport to be set.
@post The aViewport rectangle is filled in.
@post If the instance of TSurfaceConfiguration is too small, the empty viewport rectangle will be returned.
*/
inline void TSurfaceConfiguration1::GetViewport (TRect& aViewport) const
	{
	if (MemberAccessible(iViewport))
		{
		aViewport = iViewport;
		}
	else
		{
		aViewport = TRect();
		}
	}

/**
This method sets the relative orientation of viewport to window extent. The default value is 
EGraphicsOrientationNormal.

The rotation effect is applied to the input viewport area, around the centre-point of the viewport.

@param aOrientation User orientation to be set
@return KErrNone on success or a system-wide error code
	- KErrNotSupported if not supported.
	- KErrArgument if aOrientation is out of range
@post The orientation is updated in the object of this class. This does not imply that the orientation 
is set for the surface content. The SetBackgroundSurface API must be called passing an object of 
this class as parameter to get the desired effect.
*/
inline TInt TSurfaceConfiguration1::SetOrientation (CFbsBitGc::TGraphicsOrientation aOrientation)
	{
	if (MemberAccessible(iOrientation))
		{
		__ASSERT_COMPILE(CFbsBitGc::EGraphicsOrientationNormal==0 && 
						CFbsBitGc::EGraphicsOrientationRotated270 == 3);
		if(aOrientation < CFbsBitGc::EGraphicsOrientationNormal || 
				aOrientation > CFbsBitGc::EGraphicsOrientationRotated270)
			{
			return KErrArgument;
			}
		iOrientation = aOrientation;
		return KErrNone;
		}
	return KErrNotSupported;
	}

/**
The method gets the surface orientation angle stored in the object of this class. It is the 
relative orientation of viewport to window extent. If orientation was not set, the value returned 
will be EGraphicsOrientationNormal.

@return orientation in this object
@post If the instance of TSurfaceConfiguration is too small, the default EGraphicsOrientationNormal 
setting will be returned.
*/
inline CFbsBitGc::TGraphicsOrientation TSurfaceConfiguration1::Orientation() const
	{
	if (MemberAccessible(iOrientation))
		{
		return iOrientation;
		}
	else
		{
		return CFbsBitGc::EGraphicsOrientationNormal;
		}
	}

/**
This method sets the area of the window where the surface content is to be placed. The extent is 
in window coordinates.

If not set or if the set extent is empty, the size of the entire window is used as the default extent. 
The default extent changes according to the change in the window size. The empty extent is an indication 
to the server that the extent should fill the entire window without the client tracking the change in 
the window size.

In SetBackgroundSurface() it is legal to set the extent larger than the window, or the window may 
become smaller than the extent. In these cases the output will be transformed on the basis of this 
larger extent, but then clipped to the window outline.

@param aExtent Area on the window where the surface contents specified by viewport
should be mapped
@return KErrNone on success or a system-wide error code
	- KErrNotSupported if not supported.
@post The extent values are updated in the object of this class. This does not imply that the 
extent is set for the surface. The SetBackgroundSurface API must be called passing an object of 
this class as parameter to get the desired effect.
*/
inline TInt TSurfaceConfiguration1::SetExtent(const TRect& aExtent)
	{
	if (MemberAccessible(iExtent))
		{
		iExtent = aExtent;
		return KErrNone;
		}
	return KErrNotSupported;
	}

/**
The method gets the extent stored in the object of this class. The extent is in window coordinates.

If extent was not set or an empty extent was set, the value returned will be an empty extent. This 
is an indication that the extent used by the server is equal to the size of the window. The actual 
size of the window is not returned.

@param aExtent User extent to be set
@post The aExtent parameter is filled in
@post If the instance of TSurfaceConfiguration is too small, the empty extent rectangle will be returned.
*/
inline void TSurfaceConfiguration1::GetExtent(TRect& aExtent) const
	{
	if (MemberAccessible(iExtent))
		{
		aExtent = iExtent;
		}
	else
		{
		aExtent = TRect();
		}
	}

/**
This method identifies which attributes are available to use.

Generally, any version of the server will accept any version of TSurfaceConfiguration, ignoring 
newer version attributes it does not recognize and substituting defaults for attributes not present 
in older version.

However, by using this method in conjunction with GetBackgroundSurface() the intelligent client can 
identify which blocks of attributes are actually present and supported after a call to 
GetBackgroundSurface, avoiding preparing complex attributes, or reducing user-interface control 
gadgets.

@return ETrue if all these attributes are available to use; otherwise EFalse
*/
inline TBool TSurfaceConfiguration1::SupportsAllMembers()
	{
	if (Size() >= sizeof(*this))
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

/**
Constructs the TSurfaceConfigurationSize class, enters the size field and blanks everything else

@param aSize The size to set for the configuration
@return ETrue If all these attributes are available to use; otherwise EFalse
*/
inline TSurfaceConfigurationSize::TSurfaceConfigurationSize(TInt aSize)
	{
	Mem::FillZ(this,aSize);
	iSize = aSize;
	}

/**
Reads the size field. The size is set automatically in the constructor, or by the 
GetBackgroundSurface method to indicate the sender’s supported structure size, and imply a 
compatible version identifier. The size reported after GetBackgroundSurface will match 
the expected size of one of the hierarchy of TSurfaceConfiguration classes documented in the 
header, and will never exceed the constructed size.

@return ETrue if all these attributes are available to use; otherwise EFalse
*/
inline TInt TSurfaceConfigurationSize::Size() const
	{
	return iSize;
	}

inline TBool TSurfaceConfigurationSize::operator == (const TSurfaceConfigurationSize& aRhs)const
	{
	return aRhs.Size()==Size();	//If the two objects are not the same version then we can't compare them!
	}
inline TBool TSurfaceConfiguration1::operator == (const TSurfaceConfiguration1& aRhs)const
	{
	if (!TSurfaceConfigurationSize::operator ==(aRhs))
		return EFalse;	//Check for earlier version compatability
	if (Size()<sizeof(*this))
		return ETrue;	//If the objects are smaller than this then the check is complete
	return 	(iSurfaceId==aRhs.iSurfaceId)
		&&	(iOrientation==aRhs.iOrientation)
		&&	(iExtent==aRhs.iExtent)
		&&	(iViewport==aRhs.iViewport)
		;
	}

/** Default constructor for configuration 2
*/
inline TSurfaceConfiguration2::TSurfaceConfiguration2():TSurfaceConfiguration1(sizeof(*this))
        ,iFlip(EFalse)
    {}

/**
Constructor for configuration 2 passing through a size already calculated

@param aSize Size of the class already calculated and being passed up through the classes
*/
inline TSurfaceConfiguration2::TSurfaceConfiguration2(TInt aSize):TSurfaceConfiguration1(aSize)
        ,iFlip(EFalse)
    {}

/**
This method sets the flip attribute to the surface. The default value is 
EFalse.

@param aFlip When ETrue enalbes flip in the x-axis
@post The flip attribute is updated in the object of this class. This does not imply that the flip 
is set for the surface content. The SetBackgroundSurface API must be called passing an object of 
this class as parameter to get the desired effect.
@return KErrNone on success 
    - KErrNotSupported if flip is not supported. 
*/
inline TInt TSurfaceConfiguration2::SetFlip(TBool aFlip)
    {
    if (MemberAccessible(iFlip))
        {
        iFlip = aFlip;
        return KErrNone;
        }
     return KErrNotSupported;
    }

/**
The method gets the flip attribute of the surface stored in the object of this class. If the flip was not set, the value returned 
will be EFalse.

@return ETrue if flip is enabled
@post If the instance of TSurfaceConfiguration is too small, the default EFalse 
setting will be returned.
*/
inline TBool TSurfaceConfiguration2::Flip() const
    {
    if (MemberAccessible(iFlip))
        {
        return iFlip;
        }
    else
        {
        return EFalse;
        }
    }

inline TBool TSurfaceConfiguration2::SupportsAllMembers()
    {
    if (Size() >= sizeof(*this))
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

inline TBool TSurfaceConfiguration2::operator == (const TSurfaceConfiguration2& aRhs)const
    {
    if (!TSurfaceConfiguration1::operator ==(aRhs))
        return EFalse;  //Check for earlier version compatability
    if (Size()<sizeof(*this))
        return ETrue;   //If the objects are smaller than this then the check is complete
    return  iFlip==aRhs.iFlip;
    }
