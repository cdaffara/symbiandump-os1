// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __GRAPHICSACCELERATOR_H__
#define __GRAPHICSACCELERATOR_H__

#include <e32std.h>
#include <gdi.h>

//
// Bitmaps
//

// Forward references
class CFbsBitmap;
class TAcceleratedBitmapSpec;

/**
A data structure that holds the information needed to directly access a bitmap.

The bitmap can be a hardware bitmap (RHardwareBitmap), or an ordinary bitmap 
(CFbsBitmap). An object of this class is filled by calling TAcceleratedBitmapSpec::GetInfo().

@see RHardwareBitmap
@see CFbsBitmap
@see TAcceleratedBitmapSpec::GetInfo() 
@publishedAll
@released
*/
class TAcceleratedBitmapInfo
	{
public:
	/** The display mode of the bitmap. */
	TDisplayMode	iDisplayMode;

	/** The address of the start of the bitmap. */
	TUint8*			iAddress;
	
	/** The width and height of the bitmap in pixels. */
	TSize			iSize;
	
	/** The address offset (in bytes) between successive lines in a bitmap.
	If the bitmap is compressed the line pitch has no meaning so this data
	member is set to the negation of the compression type. In the case of
	an extended bitmap it is -EProprietaryCompression. */
	TInt			iLinePitch;
	
	/** The shift required to obtain the number of bits needed to represent one pixel
	in the bitmap. The number of bits per pixel is calculated as 1 << iPixelShift.
	In the case of an extended bitmap this data member is set to the bitmap type UID. */
	TInt			iPixelShift;
	
	union
		{
		/** The physical address of the start of the bitmap. This is the address which a
		hardware graphics accelerator will use and is zero if the bitmap is not accessible
		to hardware graphics accelerators. Invalid in the case of an extended bitmap. */
		TUint8*		iPhysicalAddress;
		/** In the case of an extended bitmap, the size of the raw bitmap data. */
		TInt		iDataSize;
		};
	};

/**
The interface to a hardware bitmap.

This is a bitmap that can be drawn to by graphics acceleration hardware. It 
is stored in a contiguous area of physical memory.

After creating the hardware bitmap, it can be passed to CHardwareGraphicsAccelerator::NewL().

@see CHardwareGraphicsAccelerator::NewL() 
@publishedAll
@released
*/
class RHardwareBitmap
	{
	friend class CBitwiseBitmap;
	friend class CFbsScreenDevice;
public:

    /** Default constructor. */
	inline RHardwareBitmap();

    /** Constructor taking the handle of an existing RHardwareBitmap to duplicate it. */
	inline RHardwareBitmap(TInt aHandle);
	
	/** 
	Gets the information needed for accessing a bitmap directly into TAcceleratedBitmapInfo structure.

	@param aInfo On return, holds the information needed to directly access the	bitmap.
	@return KErrNone if sucessful, otherwise one of the system error codes, including 
	KErrUnknown if the object's type is ENoBitmap. 
	*/
	IMPORT_C TInt GetInfo(TAcceleratedBitmapInfo& aInfo) const;
private:
	IMPORT_C TInt SetAsScreenReference(TInt aScreen=-1);
	IMPORT_C TInt Create(TDisplayMode aDisplayMode, TSize aSize, TUid aCreatorUid);
	IMPORT_C void Destroy();
public:
	
	/** The bitmap's handle; assigned during construction. This is used to identify 
	the bitmap. */
	TInt iHandle;	// Must be only member data
	};

	/** Default constructor. Initialises the handle to zero. */
inline RHardwareBitmap::RHardwareBitmap()
	: iHandle(0)
	{}

	/** Constructor taking the handle of an existing RHardwareBitmap to duplicate.
	@param aHandle The RHardwareBitmap handle to duplicate. */
inline RHardwareBitmap::RHardwareBitmap(TInt aHandle)
	: iHandle(aHandle)
	{}

/**
Maintains a count of the number of locks made on a bitmap through a TAcceleratedBitmapSpec 
object.

Passed as a parameter to TAcceleratedBitmapSpec::Lock() and TAcceleratedBitmapSpec::Unlock().

@see TAcceleratedBitmapSpec 
@publishedAll
@released
*/
class TBitmapLockCount
	{
	friend class TAcceleratedBitmapSpec;
public:
	
	/** Default constructor. Initialises the lock count to zero. */
	inline TBitmapLockCount() : iCount(0) {}
private:
	inline TInt Inc() { return iCount++; }
	inline TInt Dec() { return --iCount; }
private:
	TInt iCount;
	};


/**
A utility class that provides access to the contents of a bitmap.

The bitmap can be a hardware bitmap (RHardwareBitmap), or an ordinary bitmap 
(CFbsBitmap). An object of this class is used as a parameter by several accelerated 
graphics operations, e.g. TGopBitBlt, to specify the source bitmap for the 
operation. 
@publishedAll
@released
*/
class TAcceleratedBitmapSpec
	{
public:
	// Constructors
	inline TAcceleratedBitmapSpec();
	IMPORT_C TAcceleratedBitmapSpec(CFbsBitmap* aBitmap);
	IMPORT_C TAcceleratedBitmapSpec(RHardwareBitmap aBitmap);
	// Bitmap access (use with caution, see documentation)
	
	IMPORT_C TInt GetInfo(TAcceleratedBitmapInfo& aInfo) const;
	inline void Lock(TBitmapLockCount& aCount);
	inline void Lock(TBitmapLockCount& aCount,TAcceleratedBitmapInfo& aInfo);
	inline void	Unlock(TBitmapLockCount& aCount);

	// enums
	/** Identifies the type of the bitmap.

	Type() returns this value.

	@see CFbsBitmap */
	enum TAcceleratedBitmapType
		{
	/** The object was created using the default constructor, and has no type. */
		ENoBitmap,
	
	/** The bitmap is of type CFbsBitmap.
	
	@see CFbsBitmap */
		EFbsBitmap,
	
	/** The bitmap is of type RHardwareBitmap.
	
	@see RHardwareBitmap */
		EHardwareBitmap,
		};
	enum TAcceleratedBitmapLock
		{
		EBitmapIsStatic,
		EBitmapNeedsLocking,
		};
	// Getters
	inline TAcceleratedBitmapType	Type() const;
	inline TInt						Handle() const;
private:
	IMPORT_C void DoLock(TBitmapLockCount& aCount);
	IMPORT_C void DoLock(TBitmapLockCount& aCount,TAcceleratedBitmapInfo& aInfo);
	IMPORT_C void DoUnlock(TBitmapLockCount& aCount);
private:
	TUint8	iType;			// TAcceleratedBitmapType
	TUint8	iLockStatus;	// TAcceleratedBitmapLock
	TUint8	iSpare1;
	TUint8	iSpare2;
	TInt	iHandle;
	};

	/** Default constructor. 
	Use one of the other constructor overloads instead. */
inline TAcceleratedBitmapSpec::TAcceleratedBitmapSpec()
	: iType(ENoBitmap), iLockStatus(EBitmapIsStatic)
	{}

	/** Prevents a bitmap from moving in memory. Lock() should be called before accessing 
	the bitmap and Unlock() immediately afterwards. Although it is not necessary 
	to lock and unlock some types of bitmap, it is a small overhead, and it is 
	recommended that you always do it.
	
	If a bitmap is already locked, all uses of the Lock() and Unlock() methods 
	within the same thread must use the same TBitmapLockCount object, even if 
	Lock() and Unlock() are called by different instances of TAcceleratedBitmapSpec.
	
	@param aCount Maintains a count of the number of locks made on the bitmap. */
inline void TAcceleratedBitmapSpec::Lock(TBitmapLockCount& aCount)
	{ if(iLockStatus==EBitmapNeedsLocking) DoLock(aCount); }

	/** Prevents a bitmap from moving in memory. Lock() should be called before accessing 
	the bitmap and Unlock() immediately afterwards. Although it is not necessary 
	to lock and unlock some types of bitmap, it is a small overhead, and it is 
	recommended that you always do it. Also updates a TAcceleratedBitmapInfo structure 
	with any information that may have changed, (typically the bitmap's memory 
	address).
	
	If a bitmap is already locked, all uses of the Lock() and Unlock() methods 
	within the same thread must use the same TBitmapLockCount object, even if 
	Lock() and Unlock() are called by different instances of TAcceleratedBitmapSpec.
	
	@param aCount Maintains a count of the number of locks made on the bitmap.
	@param aInfo On return, contains the new address of the start of the bitmap. */
inline void TAcceleratedBitmapSpec::Lock(TBitmapLockCount& aCount,TAcceleratedBitmapInfo& aInfo)
	{ if(iLockStatus==EBitmapNeedsLocking) DoLock(aCount,aInfo); }

	/** Frees a bitmap after a call to Lock().  A call to Unlock() must be made for each corresponding
	call to Lock(). This function should be called as soon as any bitmap access has finished. If, after
	the Unlock() operation, no more calls to Lock() are outstanding on the bitmap, the bitmap is free to
	be moved in memory again.
	
	If a bitmap is already locked, all uses of the Lock() and Unlock() methods 
	within the same thread must use the same TBitmapLockCount object, even if 
	Lock() and Unlock() are called by different instances of TAcceleratedBitmapSpec.
	
	@param aCount Maintains a count of the number of locks made on the bitmap. */
inline void	TAcceleratedBitmapSpec::Unlock(TBitmapLockCount& aCount)
	{ if(iLockStatus==EBitmapNeedsLocking) DoUnlock(aCount); }

	/** Returns the type of the bitmap. The type is assigned during construction.
	
	@return The type of bitmap. */
inline TAcceleratedBitmapSpec::TAcceleratedBitmapType TAcceleratedBitmapSpec::Type() const
	{ return (TAcceleratedBitmapSpec::TAcceleratedBitmapType)iType; }

	/** Returns the handle to the bitmap.
	
	@return The handle to the bitmap. */
inline TInt TAcceleratedBitmapSpec::Handle() const
	{ return iHandle; }

//
// Accelerator capabilities
//


/**
Enumerates the four transparency types.

ETransparentPixel and ETransparentColor are used with a pixel value or a TRgb 
respectively.

@see TGopTransparency
@see TGraphicsAcceleratorCaps::iTransparency 
@publishedAll
@released
*/
enum TTransparencyType
	{
	
	/** Any pixel that has all bits equal to zero is treated as transparent. */
	ETransparentPixelZero,
	
	/** Any pixel that is equal to the pixel value passed to the TGopTransparency constructor 
	is treated as transparent. */
	ETransparentPixel,
	
	/** Any pixel that is equal to the TRgb value passed to the TGopTransparency constructor 
	is treated as transparent. */
	ETransparentColor,
	
	/** In 16 bits per pixel display mode, which uses 5 bits each for red, green and 
	blue, the most significant bit is an Alpha value. Alpha=0 means the pixel 
	is transparent, Alpha=1 means the pixel is fully opaque. */
	ETransparent1555,
	};

/** 
Stores the capabilities of a graphics accelerator.

All of the member enums except TMaskBitmapCaps define flags that are stored 
as public data of type TUint. Only TMaskBitmapCaps takes sequential values, 
so its values are mutually exclusive.

An object of this class is returned by CGraphicsAccelerator::Capabilities() 
or by GenericCapabilities(), which is implemented by CSoftwareGraphicsAccelerator 
and CHardwareGraphicsAccelerator.

@see CGraphicsAccelerator::Capabilities() 
@publishedAll
@released
*/
class TGraphicsAcceleratorCaps
	{
public:
    /** Clipping capabilities. Used by the iClipping member.

    @see CGraphicsAccelerator::Operation() */
	enum TClipCaps	// Bit flags
		{
		EClipToBitmap = 1,	// Will always clip all operations to the bitmap
	
	    /** The accelerator supports the Operation() methods which take clipping rectangles 
	    as parameters.
	
	    @see CGraphicsAccelerator::Operation() */
		EClipping = 2		// Is able to clip operations to a region
		};

/** Enumerates the capabilities relating to operations taking a bitmap mask parameter, 
for instance TGopBitBltMasked. These are mutually exclusive values used by 
the iMaskType member. */
	enum TMaskBitmapCaps	// Enum
		{
	/** No masked operations are supported. */
		EMaskBitmapNone = 0,
	
	/** The mask bitmap can be in any display mode at all. */
		EMaskBitmapAnyDisplayMode,
	
	/** The mask bitmap must be in the same display mode as the destination bitmap. */
		EMaskBitmapMatchingDisplayMode,
	
	/** The mask bitmap must be in EGray2 display mode. */
		EMaskBitmapGray2,
		};

/** Bit flags for the capabilities relating to operations that use an alpha channel 
(TGopBitBltAlphaChannel and TGopScaledBitBltAlphaChannel). These flags are 
used by the iAlphaChannel member. */
	enum TAlphaChannelCaps	//Bit flags
    	{
	/** The accelerator can draw bitmaps with 4 bits each for the alpha value and the 
	red, green and blue components. */
		EAlpha4444 = 1,	// Bitmaps with 4 bits for Alpha value and Red, Green, Blue components
	
	/** The accelerator can draw bitmaps with 8 bits each for the alpha value and the 
	red, green and blue components. */
		EAlpha8888 = 2, // Bitmaps with 8 bits for Alpha value and Red, Green, Blue components
	
	/** The accelerator can draw bitmaps with 1 bit for the alpha value and and 5 bits 
	for the red, green and blue components. */
		EAlpha1555 = 4, // Bitmaps with 1 bit for Alpha value and 5 bits for Red, Green, and Blue
		};

/** Bit flags for the capabilities relating to operations which take an alpha bitmap 
parameter, for instance TGopBitBltAlphaBitmap. These flags are used by the 
iAlphaBitmap member. */
	enum TAlphaBitmapCaps	//Bit flags
    	{
	/** For 256 greyscale bitmaps, the value of each pixel in the alpha bitmap (from 
	0 to 255) is used as the alpha value. */
		EAlphaBitmapGray256 = 1,
	
	/** An EColor16M bitmap may be used as the alpha bitmap. The red, green and blue 
	values for each pixel in this bitmap are used as the alpha values for the 
	red, green and blue components of the corresponding pixel in the source bitmap. */
		EAlphaBitmapColor16M = 2,
	
	/** The alpha bitmap must have the same display mode as the source bitmap. */
		EAlphaBitmapMatchingMode = 4,	// Alpha bitmap must be same mode as source
		};

/** Indicates whether there is a restriction on the sizes of bitmaps that can be 
used in bitmap patterns.

This is one of the possible values for the iPatternSizes member.

@see TGopFillPattern */
	enum TPatternSizeCaps	//Bit flags
		{
	/** There is no restriction on the dimensions of bitmap patterns. */
		EPatternSizeAny = 0xFFFFFFFF,
		};

/** Bit flags for the capabilities relating to operations that draw a fill pattern 
using a bitmap, for instance TGopFilledRectWithPatern. They are used in the 
iPattern member. */
	enum TPatternCaps	//Bit flags
		{
	/** The pattern bitmap can be in any display mode. */
		EPatternAnyDisplayMode = 1,			// Patterns can be in any supported display mode
	
	/** The pattern bitmap must be in the same display mode as the destination. */
		EPatternMatchingDisplayMode = 2,	// Pattern must be in same displ mode as target
	
	/** The pattern bitmap must be square (width==height). */
		EPatternMustBeSquare = 4,			// The pattern must be square (width==height)
		};
		
/** Bit flags for how self-crossing polygons are filled.

@see CGraphicsContext::TFillRule */
	enum TPolygonCaps	// Bit flags for fill rules (see CGraphicsContext::TFillRule)
		{
	/** Only areas with odd winding numbers are filled. */
		EPolygonFillAlternate = 1,
	
	/** All areas with a winding number greater than zero are filled.
	
	@see CGraphicsContext::TFillRule */
		EPolygonFillWinding = 2,
		};

	 		
/** Bit flags for the specifying the supported rendering orientations. 
@see  CFbsBitGc::TGraphicsOrientation */
 	enum TOrientationCaps
 		{
 		/** Normal orientation is supported. */
 		EOrientationCapNormal = 1,
 		/** A 90 degree rotation is supported. */
 		EOrientationCapRotated90 = 2,
 		/** A 180 degree rotation is supported. */
 		EOrientationCapRotated180 = 4,
 		/** A 270 degree rotation is supported. */
 		EOrientationCapRotated270 = 8,
 		/** All orientations are supported. */ 
 		EOrientationCapAll = EOrientationCapNormal|EOrientationCapRotated90|EOrientationCapRotated180|EOrientationCapRotated270
 		};

 	/** The size of this class in bytes. */
	TInt			iStructureSize;	// The size of this class
	
	/** The version number of the API. */
	TInt			iVersion;		// == 1 to specify current API
	
	/** Optional UID to identify the vendor of the graphics accelerator. This UID can 
	be used to recognise a particular accelerator, enabling code to use any custom 
	graphics operations and capabilities that it knows the accelerator provides. */
	TUid			iVendorUid;		// Optional ID
	
	/** A bit mask of the supported display modes for the bitmap passed to the graphics 
	accelerator's NewL(). Uses the least significant 11 bits as flags for each 
	TDisplayMode supported. For instance, to check whether the EColor256 display 
	mode is available, use the expression iDisplayModes & (1 << EColor256).
	
	@see TDisplayMode */
	TUint			iDisplayModes;	// One bit for each TDisplayMode enumeration
	
	/** Indicates whether the Operation() methods which take clipping rectangles as 
	parameters are supported.
	
	@see TClipCaps */
	TUint			iClipping;		// TClipCaps bit flags
	
	/** Specifies the display mode restrictions for bitmap masks. These are mutually 
	exclusive values.
	
	@see TMaskBitmapCaps */
	TMaskBitmapCaps	iMaskType;		// Mask type used
	
	/** Specifies the transparency types supported. Uses a bit flag for each TTransparencyType 
	supported.
	
	@see TTransparencyType */
	TUint			iTransparency;	// Bit flag for each TTransparencyType supported
	
	/** Specifies the capabilities relating to operations that use an alpha channel. Uses a bit flag for
	each TAlphaChannelCaps supported.
	
	@see TAlphaChannelCaps */
	TUint			iAlphaChannel;	// TAlphaChannelCaps bit flags
	
	/** Specifies the supported alpha bitmap types. Uses a bit flag for each TAlphaBitmapCaps 
	supported.
	
	@see TAlphaBitmapCaps */
	TUint			iAlphaBitmap;	// TAlphaBitmapCaps bit flags
	
	/** Specifies the sizes of bitmaps that can be used in bitmap patterns.
	
	This is a bitmask for each power of 2, or EPatternSizeAny. For example, if 
	bitmaps used in patterns can only have a width or height of 16 pixels then 
	this value should be set to 16. If patterns can have dimensions of 16, 32, 
	64, 128 or 256, then this value would equal the sum of these, (i.e. bits 4, 
	5, 6, 7 and 8 would be set).  If this value is equal to EPatternSizeAny, there
	are no restrictions on the size of patterns that can be used.
	
	@see TPatternSizeCaps */
	TUint			iPatternSizes;	// a mask bit for each power of 2, or EPatternSizeAny
	
	/** Specifies the supported bitmap types for fill patterns. Uses a bit flag for 
	each TPatternCaps supported.
	
	@see TPatternCaps */
	TUint			iPattern;		// TPatternCaps bit flags
	
	/** Specifies the supported fill rules for self crossing polygons. Uses a bit flag 
	for each TPolygonCaps supported.
	
	@see TPolygonCaps */
	TUint			iPolygon;		// TPolygonCaps bit flags
	
 	/** 
 	iReserved[0] specifies the supported rendering orientations.Uses a bit flags
 	for each TOrientationCaps supported.	
 	@see TOrientationCaps 
 	iReserved[1]-iReserved[3] are reserved for future use. All should be set to zero.
 	*/
 	TUint			iReserved[4];
	};


//
// TGraphicsOperation
//

/**
Abstract base class for all graphics operations.

Derived classes encapsulate all the arguments needed by a given graphics operation. 
An object of one of the derived classes is passed as a parameter to CGraphicsAccelerator::Operation(). 
The member functions and enum defined in this class are not used directly 
in third party code.

@see CGraphicsAccelerator::Operation() 
@publishedAll
@released
*/
class TGraphicsOperation
	{
public:
	enum TGopFunction
		{								// Require arguments commented below here

		EFilledRect,					// (TRect,TRgb)
		EFilledRectUsingDrawMode,		// (TRect,TRgb,CGraphicsContext:TDrawMode)
		EFilledRectWithPattern,			// (TRect,TGopFillPattern)
		EInvertRect,					// (TRect)
		EFadeRect,						// (TRect,TGopFadeParams)

		EBitBlt,						// (TPoint,TAcceleratedBitmapSpec,TRect&)
		EBitBltMasked,					// (TPoint,TAcceleratedBitmapSpec,TRect&,TAcceleratedBitmapSpec aMask)
		EBitBltTransparent,				// (TPoint,TAcceleratedBitmapSpec,TRect&,TGopTransparency)
		EBitBltAlphaChannel,			// (TPoint,TAcceleratedBitmapSpec,TRect&)
		EBitBltAlphaBitmap,				// (TPoint,TAcceleratedBitmapSpec,TRect&,TAcceleratedBitmapSpec aAlpha)

		EScaledBitBlt,					// (TRect,TAcceleratedBitmapSpec,TRect&)
		EScaledBitBltMasked,			// (TRect,TAcceleratedBitmapSpec,TRect&,TAcceleratedBitmapSpec aMask)
		EScaledBitBltTransparent,		// (TRect,TAcceleratedBitmapSpec,TRect&,TGopTransparency)
		EScaledBitBltAlphaChannel,		// (TRect,TAcceleratedBitmapSpec,TRect&)
		EScaledBitBltAlphaBitmap,		// (TRect,TAcceleratedBitmapSpec,TRect&,TAcceleratedBitmapSpec aAlpha)

		EFilledPolygon,					// (TRGb aColor,CGraphicsContext::TFillRule aFillRule,TInt aNumPoints,TPoint[])
		EFilledPolygonWithPattern,		// (TGopFillPattern,CGraphicsContext::TFillRule aFillRule,TInt aNumPoints,TPoint[])
		EAlphaBlendTwoBitmaps,			// (TPoint,TAcceleratedBitmapSpec aSrce1,TAcceleratedBitmapSpec aSrce2,TRect&,TAcceleratedBitmapSpec aAlpha)
		EAlphaBlendOneBitmap,			// (TPoint,TAcceleratedBitmapSpec aSrce,TRect&,TAcceleratedBitmapSpec aAlpha)
		EChunkTest,
		EVirtualAddressTest,
		};
public:
	// Getters
	inline TGopFunction Function() const	{ return iFunction; }
	inline TInt Size() const				{ return iSize; }
	// Utility functions 
	inline TGraphicsOperation* Next() const;
	inline void Append(TInt aNumBytes,TAny* aData);
protected:
	inline TGraphicsOperation(TGopFunction aFunction, TInt aArgSize);
	inline TGraphicsOperation() {}
protected:
	TGopFunction iFunction;
	TInt iSize;  // Total size of derived class
	};

inline TGraphicsOperation::TGraphicsOperation(TGopFunction aFunction, TInt aSize)
	: iFunction(aFunction) , iSize(aSize) {}

inline TGraphicsOperation* TGraphicsOperation::Next() const
	{ return (TGraphicsOperation*)((TUint8*)this+iSize); }

inline void TGraphicsOperation::Append(TInt aNumBytes,TAny* aData)
	{
	Mem::Copy(Next(),aData,aNumBytes);
	iSize += aNumBytes;
	}


//
// Graphics accelerator
//

/**
Abstract base class for 2D graphics accelerators.

This class can be derived from to provide accelerated implementations of some 
common 2D graphics algorithms. Support for accelerated 2D graphics has been 
integrated into existing classes in the Graphics API for instance CFbsBitGc, 
so that existing code does not need to be altered, but a graphics accelerator 
can be used directly by applications. The accelerated 2D graphics operations 
may be implemented in software, hardware, or both. 
@publishedAll
@released
*/
class CGraphicsAccelerator : public CBase
	{
public:
	// Return the capabilities of this accelerator
	
	/** Returns the capabilities of the graphics accelerator.
	
	@return The capabilities of the accelerator. */
	virtual const TGraphicsAcceleratorCaps* Capabilities() = 0;

	// Perform a graphics operation
	
	/** Requests the graphics accelerator to perform a single graphics operation.
	
	@param aOperation An instance of a TGraphicsOperation-derived class that identifies 
	the graphics operation to be performed.
	@return KErrNone if successful, otherwise one of the system error codes. The 
	function should return KErrNotSupported if the accelerator does not support 
	the requested operation. */
	virtual TInt Operation(const TGraphicsOperation& aOperation) = 0;
	
	/** Requests the graphics accelerator perform a single graphics operation within 
	a clipping region.  This version is of Operation() is only usable if the 
	accelerator capabilities returned by Capabilities() indicate that clipping to a region
	is supported.
	
	@param aOperation An instance of a TGraphicsOperation-derived class that identifies 
	the graphics operation to be performed.
	@param aNumClipRects The number of rectangles in the clipping region.
	@param aClipRects A pointer to the first rectangle in the clipping region.
	@return KErrNone if successful, otherwise one of the system error codes. The 
	function should return KErrNotSupported if the accelerator does not support 
	the requested operation.
	@see TGraphicsAcceleratorCaps::iClipping */
	virtual TInt Operation(const TGraphicsOperation& aOperation,TInt aNumClipRects,TRect* aClipRects) = 0;
	
	// Process a buffer of TGraphicsOperation. (Each operation immediately follows the
	// one preceding it in the buffer)
	
	/** Requests the graphics accelerator perform one or more graphics operations contained 
	in a buffer.
	
	The underlying implementation may be able to process a group of graphics operations 
	more efficiently than if Operation() was called for each individually.
	
	This function should be implemented as if Operation() was called in turn for 
	each operation contained in the buffer. Each operation should be carried out 
	immediately after the one preceding it. If a method returns an error, the 
	length of aBuffer should be set to indicate the number of operations that 
	have been successfully processed. In this case, the operation in which the 
	error occurred will be indicated by the memory address &aBuffer[aBuffer.Length()].
	
	@param aBuffer A descriptor which holds a concatenation of graphics operations 
	(TGraphicsOperation-derived objects).
	@return KErrNone if successful, otherwise one of the system error codes. The 
	function should return KErrNotSupported if the accelerator does not support 
	any of the requested operations. */
	virtual TInt Operation(TDes8& aBuffer) = 0;
	
	/** Requests the graphics accelerator perform one or more graphics operations within 
	a clipping region.  This version is of Operation() is only usable if the 
	accelerator capabilities returned by Capabilities() indicate that clipping to a region
	is supported.
	
	The underlying implementation may be able to process a group of graphics operations 
	more efficiently than if Operation() was called for each individually.
	
	This function should be implemented as if Operation() was called in turn for 
	each operation contained in the buffer. Each operation should be carried out 
	immediately after the one preceding it. If a method returns an error, the 
	length of aBuffer should be set to indicate the number of operations that 
	have been successfully processed. In this case, the operation in which the 
	error occurred will be indicated by the memory address &aBuffer[aBuffer.Length()].
	
	@param aBuffer A descriptor which holds a concatenation of graphics operations 
	(TGraphicsOperation objects).
	@param aNumClipRects The number of rectangles in the clipping region.
	@param aClipRects A pointer to the first rectangle in the clipping region.
	@return KErrNone if successful, otherwise one of the system error codes. The 
	function should return KErrNotSupported if the accelerator does not support 
	any of the requested operations.
	@see TGraphicsAcceleratorCaps::iClipping */
	virtual TInt Operation(TDes8& aBuffer,TInt aNumClipRects,TRect* aClipRects) = 0;
public:
	// Reserved virtual functions for future use
	virtual void Reserved_1() = 0;
	virtual void Reserved_2() = 0;
	virtual void Reserved_3() = 0;
	virtual void Reserved_4() = 0;
	};



/**
A factory for creating 2D graphics accelerator objects whose graphics operations 
are implemented in software.

Objects of derived classes can write to all types of bitmap, not just hardware 
bitmaps. Note that graphics accelerators may support only a subset of all 
graphics operations. 
@publishedAll
@released
*/
class CSoftwareGraphicsAccelerator : public CGraphicsAccelerator
	{
public:
	// Create a new CSoftwareGraphicsAccelerator for use with a given bitmap
	IMPORT_C static CSoftwareGraphicsAccelerator* NewL(CFbsBitmap* aBitmap);

	// Get the non-bitmap-specific capabilities of the hardware accelerator.
	IMPORT_C static const TGraphicsAcceleratorCaps* GenericCapabilities();
public:
	// From CGraphicsAccelerator
	virtual const TGraphicsAcceleratorCaps* Capabilities() = 0;
	virtual TInt Operation(const TGraphicsOperation& aOperation) = 0;
	virtual TInt Operation(const TGraphicsOperation& aOperation,TInt aNumClipRects,TRect* aClipRects) = 0;
	virtual TInt Operation(TDes8& aBuffer) = 0;
	virtual TInt Operation(TDes8& aBuffer,TInt aNumClipRects,TRect* aClipRects) = 0;
	// From CGraphicsAccelerator
	virtual void Reserved_1() = 0;
	virtual void Reserved_2() = 0;
	virtual void Reserved_3() = 0;
	virtual void Reserved_4() = 0;
	};


/**
A factory for creating 2D graphics accelerator objects whose graphics operations 
are implemented in hardware, software or a mixture of both.

Objects of derived classes can only write to hardware bitmaps (RHardwareBitmap). 
Note that graphics accelerators may support only a subset of all graphics 
operations.

@see RHardwareBitmap 
@publishedAll
@released
*/
class CHardwareGraphicsAccelerator : public CGraphicsAccelerator
	{
public:
	/**
	Create a new CHardwareGraphicsAccelerator for use with a given hardware bitmap.
	
	Do not use, link against scdv.lib.
	
	@param aBitmap A bitmap that can be drawn by graphics acceleration hardware. 
	               It can be any bitmap.
	@return Reference to hardware graphics accelerator object.
	*/
	IMPORT_C static CHardwareGraphicsAccelerator* NewL(RHardwareBitmap aBitmap);
	
	/**
	Gets the generic capabilities of the accelerator, including the supported display modes 
	for the bitmap passed to NewL().
	
	Do not use, link against scdv.lib.
	
	@return Generic capabilities for software graphics accelerators.	
	*/
	IMPORT_C static const TGraphicsAcceleratorCaps* GenericCapabilities();
public:
	// From CGraphicsAccelerator
	virtual const TGraphicsAcceleratorCaps* Capabilities() = 0;
	virtual TInt Operation(const TGraphicsOperation& aOperation) = 0;
	virtual TInt Operation(const TGraphicsOperation& aOperation,TInt aNumClipRects,TRect* aClipRects) = 0;
	virtual TInt Operation(TDes8& aBuffer) = 0;
	virtual TInt Operation(TDes8& aBuffer,TInt aNumClipRects,TRect* aClipRects) = 0;
	// From CGraphicsAccelerator
	virtual void Reserved_1() = 0;
	virtual void Reserved_2() = 0;
	virtual void Reserved_3() = 0;
	virtual void Reserved_4() = 0;
	};

//
// Classes used as arguments to graphics operations
//

/**
A pattern represented by a bitmap that is used by a graphics accelerator to 
fill a rectangle or polygon.

An object of this class is specified when constructing a TGopFilledRectWithPattern 
or TGopFilledPolygonWithPattern. The types and sizes of fill pattern bitmaps 
supported by the accelerator are given by TGraphicsAcceleratorCaps::iPattern 
and TGraphicsAcceleratorCaps::iPatternSizes respectively.

@see TGopFilledRectWithPattern
@see TGopFilledPolygonWithPattern
@see TGraphicsAcceleratorCaps::iPatternSizes
@see TGraphicsAcceleratorCaps::iPattern 
@publishedAll
@released
*/
class TGopFillPattern
	{
public:
	
	/** Provides a handle to the bitmap, and other information needed to draw it. */
	TAcceleratedBitmapSpec	iBitmap;
	
	/** The origin of the pattern. This is the position at which to draw the pixel at 
	the top left hand corner of the bitmap around which copies of the bitmap are 
	"tiled" to form the pattern. It is relative to the top left hand corner of 
	the rectangle being filled, so specify 0,0 if you want the bitmaps drawn flush 
	with the top and left hand sides of the rectangle. */
	TPoint					iOrigin;
	};

/**
Specifies the amount of fading for all the pixels in a rectangular area.

Fading changes colours so that they are closer to white or closer to black. 
To make colours whiter, increase iOffset; to use a smaller range of colours, 
reduce iScale. Fading uses the following formula (where C is a red, green 
or blue value in a TRgb):

colour component C = ( ( iScale * C ) / 256 )+iOffset;

For example:
- to fade to white, specify iScale=128, iOffset=128
- to fade to black, specify iScale=128, iOffset=0
- for no change, specify iScale=256, iOffset=0

An object of this class is specified when constructing a TGopFadeRect.

@see TGopFadeRect
@publishedAll
@released
*/
class TGopFadeParams	// color component C = ( ( iScale * C ) >> 8 )+iOffset;
	{
public:
	
	/** Specifies the degree of fading, maximum=256. */
	TInt iScale;
	
	/** The fading offset. Specifies whether to fade to black or to white. */
	TInt iOffset;
	};

/**
Specifies which pixels should be treated as transparent in a bitblt operation 
that supports transparency.

This is used by the TGopBitBltTransparent and TGopScaledBitBltTransparent 
graphics operations.

For the possible transparency types, see the TTransparencyType enumeration.

An object of this class is specified when constructing a TGopBitBltTransparent or
TGopScaledBitBltTransparent.

@see TTransparencyType
@see TGopBitBltTransparent
@see TGopScaledBitBltTransparent 
@publishedAll
@released
*/
class TGopTransparency
	{
public:
	
	/** Constructor with a transparency type. iParam is initialised to zero.
	
	@param aType The transparency type. */
	inline TGopTransparency(TTransparencyType aType)	: iType(aType), iParam(0) {}
	
	/** Constructor with a pixel value. The type is initialised to ETransparentPixel. 
	Any pixel that has a value equal to aPixelValue is treated as transparent. 
	aPixelValue is the bit pattern of the pixel as stored in the bitmap.
	
	@param aPixelValue The pixel value. */
	inline TGopTransparency(TInt aPixelValue)			: iType(ETransparentPixel), iParam(aPixelValue) {}
	
	/** Constructor with a TRgb value. The type is initialised to ETransparentColor. 
	Any pixel that has a color of aRgb is treated as transparent.
	
	@param aRgb The TRgb value. */
	inline TGopTransparency(TRgb aRgb)					: iType(ETransparentColor), iParam(aRgb.Value()) {}
	
	/** Gets the colour that is treated as transparent. This is the value of iParam 
	as a TRgb.
	
	@return The colour that is treated as transparent. */
	inline TRgb Color()	const							{ return TRgb(iParam); }
	
	/** Gets the value of the colour as a TInt that is treated as transparent. This 
	is the value of iParam.
	
	@return The colour that is treated as transparent. This is the bit pattern 
	of the colour as stored in the bitmap. */
	inline TInt Pixel()	const							{ return iParam; }
public:
	
	/** The transparency type. */
	TTransparencyType	iType;
	
	/** Holds the value of the colour/pixel that is treated as transparent. */
	TUint32				iParam;
	};


//
// Wrapper classes for graphics operation arguments
//

#ifdef __WINS__
#pragma warning(disable : 4355) // Disable warning - 'this' : used in base member initializer list
#endif

/**
An accelerated graphics operation that fills a rectangular area with a colour.

The data members are all initialised on construction. Objects of this class 
can be passed to a graphics accelerator's Operation() function either individually, 
or in a buffer. 
@publishedAll
@released
*/
class TGopFilledRect : public TGraphicsOperation
	{
public:
	/** Constructor with a rectangle and a colour.
	@param aRect The rectangle to fill.
	@param aColor The fill colour. */
	inline TGopFilledRect(const TRect& aRect,TRgb aColor)
			: TGraphicsOperation(EFilledRect,sizeof(*this)), iRect(aRect) , iColor(aColor) {}
public:
	
	/** The rectangle to fill. */
	TRect	iRect;
	
	/** The fill colour. */
	TRgb	iColor;
	};

/**
An accelerated graphics operation that fills a rectangular area with a colour, 
whilst performing a bitwise logical operation with the pixels in the region, 
for instance AND, OR, Exclusive OR.

The bitwise logical operation is specified in the draw mode. The data members 
are all initialised on construction. Objects of this class can be passed to 
a graphics accelerator's Operation() function either individually, or in a 
buffer.
@publishedAll
@released
*/
class TGopFilledRectUsingDrawMode : public TGraphicsOperation
	{
public:
	/** Constructor with a rectangle, a colour and a draw mode.
	@param aRect The rectangle to fill.
	@param aColor The fill colour.
	@param aDrawMode The draw mode. */
	inline TGopFilledRectUsingDrawMode(const TRect& aRect,TRgb aColor,CGraphicsContext::TDrawMode aDrawMode)
		: TGraphicsOperation(EFilledRectUsingDrawMode,sizeof(*this)), iRect(aRect) , iColor(aColor) , iDrawMode(aDrawMode) {}
public:
	
	/** The rectangle to fill. */
	TRect						iRect;
	
	/** The fill colour. */
	TRgb						iColor;
	
	/** The draw mode. */
	CGraphicsContext::TDrawMode	iDrawMode;
	};

/**
An accelerated graphics operation that fills a rectangular area with a pattern.

The pattern consists of multiple copies of a bitmap, drawn tiled around an 
origin. Objects of this class can be passed to a graphics accelerator's Operation() 
function either individually, or in a buffer. 

@see TGopFillPattern
@publishedAll
@released
*/
class TGopFilledRectWithPattern : public TGraphicsOperation
	{
public:
	/** Constructor with a rectangle and a pattern.
	@param aRect The rectangle to fill.
	@param aPattern Specifies the handle to the bitmap to use for the pattern, 
	and the origin for the pattern. */
	inline TGopFilledRectWithPattern(const TRect& aRect,TGopFillPattern aPattern)
		: TGraphicsOperation(EFilledRectWithPattern,sizeof(*this)), iRect(aRect) , iPattern(aPattern) {}
public:
	
	/** The rectangle to fill. */
	TRect			iRect;
	
	/** Specifies the handle to the bitmap to use for the pattern and the origin for 
	the pattern. */
	TGopFillPattern iPattern;
	};

/**
An accelerated graphics operation that inverts the colour of all pixels in 
a rectangular area.

Objects of this class can be passed to a graphics accelerator's Operation() 
function either individually, or in a buffer. 
@publishedAll
@released
*/
class TGopInvertRect : public TGraphicsOperation
	{
public:
	/** Constructor with a rectangle.
	@param aRect The rectangle in which to invert the colours. */
	inline TGopInvertRect(const TRect& aRect)
		: TGraphicsOperation(EInvertRect,sizeof(*this)), iRect(aRect) {}
public:
	
	/** The rectangle in which to invert the colours. */
	TRect	iRect;
	};

/**
An accelerated graphics operation that fades the pixels in a rectangular area.

Objects of this class can be passed to a graphics accelerator's Operation() 
function either individually, or in a buffer. 
@publishedAll
@released
*/
class TGopFadeRect : public TGraphicsOperation
	{
public:
	/** Constructor with a rectangle and fade parameters.
	@param aRect The rectangle to fade.
	@param aFade The fade parameters. */
	inline TGopFadeRect(const TRect& aRect, const TGopFadeParams aFade)
		: TGraphicsOperation(EFadeRect,sizeof(*this)), iRect(aRect), iFade(aFade) {}
public:
	
	/** The rectangle to fade. */
	TRect			iRect;
	
	/** The fade parameters. */
	TGopFadeParams	iFade;
	};

/**
An accelerated graphics operation that copies a rectangular region of one bitmap 
into another.

The data members are all initialised on construction. Objects of this class 
can be passed to a graphics accelerator's Operation() function either individually, 
or in a buffer. 
@publishedAll
@released
*/
class TGopBitBlt : public TGraphicsOperation
	{
public:
	/** Constructor with a position, a source bitmap handle and a rectangle.
	@param aDestination The destination for the top left hand corner of the portion 
	of the source bitmap.
	@param aSourceBitmap A handle to the source bitmap, and other information needed 
	to draw it.
	@param aSourceRect A rectangle within the source bitmap. Its coordinates are relative 
	to the top left of the source bitmap. Defines the part of the source bitmap to be copied. */
	inline TGopBitBlt(const TPoint& aDestination, TAcceleratedBitmapSpec aSourceBitmap, TRect& aSourceRect)
		: TGraphicsOperation(EBitBlt,sizeof(*this)), iDestination(aDestination), iSourceBitmap(aSourceBitmap), iSourceRect(aSourceRect) {}
public:
	
	/** The destination for the top left hand corner of the portion of the source bitmap. */
	TPoint					iDestination;
	
	/** A handle to the source bitmap, and other information needed to draw it. */
	TAcceleratedBitmapSpec	iSourceBitmap;
	
	/** A rectangle defining all or a part of the bitmap to be copied. */
	TRect					iSourceRect;
	};

/**
An accelerated graphics operation that copies a rectangular region of one bitmap 
into another, using a third bitmap as a mask.

The mask must be the same size as the source bitmap. The parts of the source 
bitmap that are drawn are the areas that are black in the mask.

The data members are all initialised on construction. Objects of this class 
can be passed to a graphics accelerator's Operation() function either individually, 
or in a buffer.

@see TGraphicsAcceleratorCaps::iMaskType 
@publishedAll
@released
*/
class TGopBitBltMasked : public TGraphicsOperation
	{
public:
	/** Constructor with a position, a source bitmap handle, a rectangle and a mask bitmap handle.
	@param aDestination The destination for the top left hand corner of the portion 
	of the source bitmap.
	@param aSourceBitmap A handle to the source bitmap, and other information needed 
	to draw it.
	@param aSourceRect A rectangle within the source bitmap. Its coordinates are relative 
	to the top left of the bitmap. Defines the part of the source bitmap to be copied.
	@param aMask A handle to the mask bitmap. The parts of the source bitmap 
	that are drawn are the areas that are black in the mask bitmap. */
	inline TGopBitBltMasked(const TPoint& aDestination, TAcceleratedBitmapSpec aSourceBitmap, TRect& aSourceRect, TAcceleratedBitmapSpec aMask)
		: TGraphicsOperation(EBitBltMasked,sizeof(*this)), iDestination(aDestination), iSourceBitmap(aSourceBitmap), iSourceRect(aSourceRect), iMask(aMask) {}
public:
	
	/** The destination for the top left hand corner of the portion of the bitmap. */
	TPoint					iDestination;
	
	/** A handle to the source bitmap, and other information needed to draw it. */
	TAcceleratedBitmapSpec	iSourceBitmap;
	
	/** A rectangle defining all or a part of the bitmap to be copied. */
	TRect					iSourceRect;
	
	/** A handle to the source bitmap mask. */
	TAcceleratedBitmapSpec	iMask;
	};

/**
An accelerated graphics operation that copies a rectangular region of one bitmap 
into another, with some transparent pixels in the bitmap.

The data members are all initialised on construction. Objects of this class 
can be passed to a graphics accelerator's Operation() function either individually, 
or in a buffer.

@see TGraphicsAcceleratorCaps::iTransparency 
@see TGopTransparency
@publishedAll
@released
*/
class TGopBitBltTransparent : public TGraphicsOperation
	{
public:
	/** Constructor with a destination, a handle to the source bitmap, a rectangle 
	and a specification for which pixels should be treated as transparent.
	
	@param aDestination The destination for the top left hand corner of the portion 
	of the source bitmap.
	@param aSourceBitmap A handle to the source bitmap, and other information needed 
	to draw it.
	@param aSourceRect A rectangle within the source bitmap. Its coordinates are 
	relative to the top left of the bitmap. Defines the part of the source bitmap to 
	be copied.
	@param aTransparency A specification for which pixels in the source bitmap should
	be treated as transparent. */
	inline TGopBitBltTransparent(const TPoint& aDestination, TAcceleratedBitmapSpec aSourceBitmap, TRect& aSourceRect, TGopTransparency aTransparency)
		: TGraphicsOperation(EBitBltTransparent,sizeof(*this)), iDestination(aDestination), iSourceBitmap(aSourceBitmap), iSourceRect(aSourceRect), iTransparency(aTransparency) {}
public:
	
	/** The destination for the top left hand corner of the portion of the bitmap. */
	TPoint					iDestination;
	
	/** A handle to the source bitmap, and other information needed to draw it. */
	TAcceleratedBitmapSpec	iSourceBitmap;
	
	/** A rectangle defining all or a part of the bitmap to be copied. */
	TRect					iSourceRect;
	
	/** A specification for which pixels should be treated as transparent. */
	TGopTransparency		iTransparency;
	};

/**
An accelerated graphics operation that copies a rectangular region of one bitmap 
into another, using alpha blending. 

The alpha value is part of each pixel in the source bitmap. For instance, 
a 32 bits per pixel bitmap may have 8 bits for each of the alpha, red, green 
and blue values.

Supported bitmap formats with an alpha-channel are given in by
TGraphicsAcceleratorCaps::iAlphaChannel.

The data members are all initialised on construction. Objects of this class 
can be passed to a graphics accelerator's Operation() function either individually, 
or in a buffer.

@see TGraphicsAcceleratorCaps::iAlphaChannel 
@publishedAll
@released
*/
class TGopBitBltAlphaChannel : public TGraphicsOperation
	{
public:
	/** Constructor with a position, a bitmap handle and a rectangle.
	@param aDestination The destination for the top left hand corner of the portion 
	of the source bitmap.
	@param aSourceBitmap A handle to the source bitmap, and other information needed 
	to draw it.
	@param aSourceRect A rectangle within the source bitmap. Its coordinates are relative 
	to the top left of the source bitmap. Defines the part of the source bitmap to be copied. */
	inline TGopBitBltAlphaChannel(const TPoint& aDestination, TAcceleratedBitmapSpec aSourceBitmap, TRect& aSourceRect)
		: TGraphicsOperation(EBitBltAlphaChannel,sizeof(*this)), iDestination(aDestination), iSourceBitmap(aSourceBitmap), iSourceRect(aSourceRect) {}
public:
	
	/** The destination for the top left hand corner of the portion of the bitmap. */
	TPoint					iDestination;
	
	/** A handle to the source bitmap, and other information needed to access it. */
	TAcceleratedBitmapSpec	iSourceBitmap;
	
	/** A rectangle defining all or a part of the bitmap to be copied. */
	TRect					iSourceRect;
	};

/**
An accelerated graphics operation that copies a rectangular region of one bitmap 
into another using alpha blending values provided in a third bitmap.

The way alpha blending works is as follows: if the alpha value is the maximum, 
the source pixel is opaque, in other words, the full colour of the pixel is 
written to the destination. If the alpha value is zero, the source pixel is 
fully transparent, and the destination is left unaltered. Values in-between 
cause blending with the following formula:

Destination = Source*Alpha/max_Alpha + Destination*(max_Alpha-Alpha)/max_Alpha

Colour alpha-bitmaps specify red, green and blue alpha values for each pixel, 
greyscale bitmaps specify a single alpha value for each pixel. The maximum 
alpha value depends on the bitmap's display mode. For example, 255 is the 
maximum for an EGray256 or EColor16M bitmap. The maximum is less for bitmaps 
which use fewer bits per colour component.

Supported bitmap formats than can be used as alpha bitmaps are given in 
TGraphicsAcceleratorCaps::iAlphaBitmap.

Objects of this class can be passed to a graphics accelerator's Operation() 
function either individually, or in a buffer.

@see TGraphicsAcceleratorCaps::iAlphaBitmap 
@publishedAll
@released
*/
class TGopBitBltAlphaBitmap : public TGraphicsOperation
	{
public:
	/** Constructor with a position, two bitmap specs and a rectangle.
	@param aDestination The destination for the top left hand corner of the portion 
	of the source bitmap.
	@param aSourceBitmap A handle to the source bitmap, and other information needed 
	to draw it.
	@param aSourceRect A rectangle within the source bitmap. Its coordinates are relative 
	to the top left of the source bitmap. Defines the part of the source bitmap to be copied.
	@param aAlphaBitmap A handle to the alpha bitmap, the bitmap that contains 
	alpha blending values. */
	inline TGopBitBltAlphaBitmap(const TPoint& aDestination, TAcceleratedBitmapSpec aSourceBitmap, TRect& aSourceRect, TAcceleratedBitmapSpec aAlphaBitmap)
		: TGraphicsOperation(EBitBltAlphaBitmap,sizeof(*this)), iDestination(aDestination), iSourceBitmap(aSourceBitmap), iSourceRect(aSourceRect), iAlphaBitmap(aAlphaBitmap) {}
public:
	
	/** The destination for the top left hand corner of the portion of the source bitmap. */
	TPoint					iDestination;
	
	/** A handle to the source bitmap, and other information needed to access it. */
	TAcceleratedBitmapSpec	iSourceBitmap;
	
	/** A rectangle defining the part of the source bitmap to be copied. */
	TRect					iSourceRect;
	
	/** A handle to the alpha bitmap, the bitmap that contains alpha blending values. */
	TAcceleratedBitmapSpec	iAlphaBitmap;
	};

/**
An accelerated graphics operation that copies a rectangular region of two bitmaps 
to a destination, using alpha blending values provided in a third bitmap to blend the 
corresponding entries in the first and second bitmaps.

The way alpha blending works is as follows: if the alpha value is the maximum, 
the pixel from the first source is opaque, in other words, the full colour of 
the pixel is written to the destination. If the alpha value is zero, the pixel 
from the first source is fully transparent, in other words, the full colour of
the pixel in the second source is used. Values in-between cause blending with 
the following formula:

Destination = Source1*Alpha/max_Alpha + Source2*(max_Alpha-Alpha)/max_Alpha

Colour alpha bitmaps specify red, green and blue alpha values for each pixel, 
greyscale bitmaps specify a single alpha value for each pixel. The maximum 
alpha value depends on the bitmap's display mode. For example, 255 is the 
maximum for an EGray256 or EColor16M bitmap. The maximum is less for bitmaps 
which use fewer bits per colour component.

Supported bitmap formats than can be used as alpha bitmaps are given in 
TGraphicsAcceleratorCaps::iAlphaBitmap.

Objects of this class can be passed to a graphics accelerator's Operation() 
function either individually, or in a buffer.

@see TGraphicsAcceleratorCaps::iAlphaBitmap 
@publishedAll
@released
*/
class TGopAlphaBlendTwoBitmaps : public TGraphicsOperation
	{
public:
	/** 
	Constructor with a position, three bitmap specs and a rectangle.
	@param aDestination The destination for the top left hand corner of the portion 
	of the source bitmaps.
	@param aSourceBmp1 A handle to the first of the source bitmaps, and other information 
	needed to draw it.
	@param aSourceBmp2 A handle to the second of the source bitmaps, and other information 
	needed to draw it.
	@param aSourceRect A rectangle within the source bitmaps. Its coordinates are relative 
	to the top left of the bitmap. Defines the part of the bitmap to be copied.
	@param aSourcePt2 The point in the second source bitmap from which we take pixels to blend
	@param aAlphaBitmap A handle to the alpha bitmap, the bitmap that contains 
	alpha blending values.
	@param aAlphaPt The point in the alpha bitmap from which we take pixels to blend
	 */	
	inline TGopAlphaBlendTwoBitmaps(const TPoint& aDestination,TAcceleratedBitmapSpec aSourceBmp1,TAcceleratedBitmapSpec aSourceBmp2,TRect& aSourceRect,const TPoint& aSrcPt2,TAcceleratedBitmapSpec aAlphaBmp, const TPoint& aAlphaPt)
		: TGraphicsOperation(EAlphaBlendTwoBitmaps,sizeof(*this)), iDestination(aDestination), iSourceBmp1(aSourceBmp1), iSourceBmp2(aSourceBmp2), iSourceRect(aSourceRect), iSrcPt2(aSrcPt2), iAlphaBmp(aAlphaBmp), iAlphaPt(aAlphaPt) {}
public:
	
	/** The destination for the top left hand corner of the portion of the source bitmaps. */
	TPoint					iDestination;
	
	/** A handle to the first source bitmap, and other information needed to access it. */
	TAcceleratedBitmapSpec	iSourceBmp1;
	
	/** A handle to the second source bitmap, and other information needed to access it. */
	TAcceleratedBitmapSpec	iSourceBmp2;
	
	/** A rectangle defining the part of the source bitmaps to be copied. */
	TRect					iSourceRect;
	
	/** The point in the second source bitmap from which we take pixels to blend. */ 
	TPoint					iSrcPt2;
	
	/** A handle to the alpha bitmap, the bitmap that contains alpha blending values. */
	TAcceleratedBitmapSpec	iAlphaBmp;
	
	/** The point in the alpha bitmap from which we take pixels to blend. */ 
	TPoint					iAlphaPt;
	};
	
/**
An accelerated graphics operation that copies a rectangular region of a bitmap blended
with the screen image to the screen, using alpha blending values provided in an alpha bitmap 
to blend the corresponding entries in the bitmap and on the screen.

The way alpha blending works is as follows: if the alpha value is the maximum, 
the pixel from the source bitmap is opaque, in other words, the full colour of 
the pixel is written to the destination. If the alpha value is zero, the pixel 
from the source bitmap is fully transparent, in other words, the full colour of
the pixel on the screen is used. Values in-between cause blending with the 
following formula:

Destination = Source*Alpha/max_Alpha + Screen*(max_Alpha-Alpha)/max_Alpha

Colour alpha bitmaps specify red, green and blue alpha values for each pixel, 
greyscale bitmaps specify a single alpha value for each pixel. The maximum 
alpha value depends on the bitmap's display mode. For example, 255 is the 
maximum for an EGray256 or EColor16M bitmap. The maximum is less for bitmaps 
which use fewer bits per colour component.

Supported bitmap formats than can be used as alpha bitmaps are given in 
TGraphicsAcceleratorCaps::iAlphaBitmap.

Objects of this class can be passed to a graphics accelerator's Operation() 
function either individually, or in a buffer.

@see TGraphicsAcceleratorCaps::iAlphaBitmap 
@publishedAll
@released
*/
class TGopAlphaBlendOneBitmap : public TGraphicsOperation
	{
public:
	/** 
	Constructor with a position, two bitmap specs and a rectangle.
	@param aDestination The destination for the top left hand corner of the portion 
	of the source bitmap.
	@param aSourceBmp A handle to the source bitmap, and other information needed 
	to draw it.
	@param aSourceRect A rectangle within the source bitmap. Its coordinates are relative 
	to the top left of the source bitmap. Defines the part of the source bitmap to be copied.
	@param aAlphaBitmap A handle to the alpha bitmap, the bitmap that contains 
	alpha blending values.
	@param aAlphaPt The point in the alpha bitmap from which we take pixels to blend
	 */
	
	
	inline TGopAlphaBlendOneBitmap(const TPoint& aDestination,TAcceleratedBitmapSpec aSourceBmp,TRect& aSourceRect,TAcceleratedBitmapSpec aAlphaBmp, const TPoint& aAlphaPt)
		: TGraphicsOperation(EAlphaBlendOneBitmap,sizeof(*this)), iDestination(aDestination), iSourceBmp(aSourceBmp), iSourceRect(aSourceRect), iAlphaBmp(aAlphaBmp), iAlphaPt(aAlphaPt) {}
public:
	
	/** The destination for the top left hand corner of the portion of the source bitmap. */
	TPoint					iDestination;
	
	/** A handle to the source bitmap, and other information needed to access it. */
	TAcceleratedBitmapSpec	iSourceBmp;
	
	/** A rectangle defining the part of the bitmap to be copied. */
	TRect					iSourceRect;
	
	/** A handle to the alpha bitmap, the bitmap that contains alpha blending values. */
	TAcceleratedBitmapSpec	iAlphaBmp;
	
	/** Position of the first pixel in the alpha bitmap to be used for alpha blending. */ 
	TPoint					iAlphaPt;
	};

/**
An accelerated graphics operation that copies a rectangular region of one bitmap 
into a different sized region of another.

The data members are all initialised on construction. Objects of this class 
can be passed to a graphics accelerator's Operation() function either individually, 
or in a buffer. 
@publishedAll
@released
*/
class TGopScaledBitBlt : public TGraphicsOperation
	{
public:
	/** Constructor with a destination rectangle, a handle to the source bitmap and 
	a source rectangle.
	@param aDestination The destination for the portion of the source bitmap. If necessary, 
	the source bitmap portion is resized to fit into this rectangle.
	@param aSourceBitmap A handle to the source bitmap, and other information needed 
	to draw it.
	@param aSourceRect A rectangle within the source bitmap. Its coordinates are relative 
	to the top left of the source bitmap. Defines the part of the source bitmap to be copied. */
	inline TGopScaledBitBlt(const TRect& aDestination, TAcceleratedBitmapSpec aSourceBitmap, TRect& aSourceRect)
		: TGraphicsOperation(EScaledBitBlt,sizeof(*this)), iDestination(aDestination), iSourceBitmap(aSourceBitmap), iSourceRect(aSourceRect) {}
public:

	/** The destination rectangle for the portion of the source bitmap. */
	TRect					iDestination;
	
	/** A handle to the source bitmap. */
	TAcceleratedBitmapSpec	iSourceBitmap;
	
	/** A rectangle defining all or a part of the source bitmap to be copied. */
	TRect					iSourceRect;
	};

/**
An accelerated graphics operation that copies a rectangular region of one bitmap 
into a different sized region of another, using a third bitmap as a mask. 

The mask must be the same size as the source bitmap. The parts of the source 
bitmap that are drawn are the areas that are black in the mask.

The data members are all initialised on construction. Objects of this class 
can be passed to a graphics accelerator's Operation() function either individually, 
or in a buffer.

@see TGraphicsAcceleratorCaps::iMaskType 
@publishedAll
@released
*/
class TGopScaledBitBltMasked : public TGraphicsOperation
	{
public:
	/** Constructor with a source and destination rectangle, and handles to the source 
	and mask bitmaps.
	
	@param aDestination The destination for the portion of the source bitmap. If necessary, 
	the source bitmap portion is resized to fit into this rectangle.
	@param aSourceBitmap A handle to the source bitmap, and other information needed 
	to draw it.
	@param aSourceRect A rectangle within the source bitmap. Its coordinates are relative 
	to the top left of the source bitmap. Defines the part of the source bitmap to be copied.
	@param aMask A handle to the mask bitmap. */
	inline TGopScaledBitBltMasked(const TRect& aDestination, TAcceleratedBitmapSpec aSourceBitmap, TRect& aSourceRect, TAcceleratedBitmapSpec aMask)
		: TGraphicsOperation(EScaledBitBltMasked,sizeof(*this)), iDestination(aDestination), iSourceBitmap(aSourceBitmap), iSourceRect(aSourceRect), iMask(aMask) {}
public:
	
	/** The destination rectangle for the portion of the bitmap. */
	TRect					iDestination;
	
	/** A handle to the source bitmap. */
	TAcceleratedBitmapSpec	iSourceBitmap;
	
	/** A rectangle defining all or a part of the source bitmap to be copied. */
	TRect					iSourceRect;
	
	/** A handle to the source bitmap mask. */
	TAcceleratedBitmapSpec	iMask;
	};

/**
An accelerated graphics operation that copies a rectangular region of one bitmap 
into a different sized region of another, with some transparent pixels in 
the source bitmap.

The data members are all initialised on construction. Objects of this class 
can be passed to a graphics accelerator's Operation() function either individually, 
or in a buffer.

@see TGraphicsAcceleratorCaps::iTransparency 
@see TGopTransparency 
@publishedAll
@released
*/
class TGopScaledBitBltTransparent : public TGraphicsOperation
	{
public:
	/** Constructor with destination and source rectangles, a handle to the source 
	bitmap and a specification for which pixels should be treated as transparent.
	
	@param aDestination The destination for the portion of the source bitmap. If necessary, 
	the source bitmap portion is resized to fit into this rectangle.
	@param aSourceBitmap A handle to the source bitmap, and other information needed 
	to draw it.
	@param aSourceRect A rectangle within the source bitmap. Its coordinates are 
	relative to the top left of the source bitmap. Defines the part of the source bitmap to 
	be copied.
	@param aTransparency A specification for which pixels in the source bitmap should be treated as 
	transparent. */
	inline TGopScaledBitBltTransparent(const TRect& aDestination, TAcceleratedBitmapSpec aSourceBitmap, TRect& aSourceRect, TGopTransparency aTransparency)
		: TGraphicsOperation(EScaledBitBltTransparent,sizeof(*this)), iDestination(aDestination), iSourceBitmap(aSourceBitmap), iSourceRect(aSourceRect), iTransparency(aTransparency) {}
public:
	
	/** The destination rectangle for the portion of the source bitmap. */
	TRect					iDestination;
	
	/** A handle to the source bitmap. */
	TAcceleratedBitmapSpec	iSourceBitmap;
	
	/** A rectangle defining all or a part of the source bitmap to be copied. */
	TRect					iSourceRect;
	
	/** A specification for which pixels in the source bitmap should be treated as transparent. */
	TGopTransparency		iTransparency;
	};

/**
An accelerated graphics operation that copies a rectangular region of one bitmap 
into a different sized region of another using alpha blending. The alpha value 
is part of each pixel in the source bitmap.

Supported bitmap formats with an alpha-channel are given in by
TGraphicsAcceleratorCaps::iAlphaChannel.

The data members are all initialised on construction. Objects of this class 
can be passed to a graphics accelerator's Operation() function either individually, 
or in a buffer.

@see TGraphicsAcceleratorCaps::iAlphaChannel 
@publishedAll
@released
*/
class TGopScaledBitBltAlphaChannel : public TGraphicsOperation
	{
public:
	/** Constructor with a destination rectangle, a handle to the source bitmap and 
	a source rectangle.
	
	@param aDestination The destination for the portion of the source bitmap. If necessary, 
	the source bitmap portion is resized to fit into this rectangle.
	@param aSourceBitmap A handle to the source bitmap, and other information needed 
	to draw it.
	@param aSourceRect A rectangle within the source bitmap. Its coordinates are relative 
	to the top left of the source bitmap. Defines the part of the source bitmap to be copied. */
	inline TGopScaledBitBltAlphaChannel(const TRect& aDestination, TAcceleratedBitmapSpec aSourceBitmap, TRect& aSourceRect)
		: TGraphicsOperation(EScaledBitBltAlphaChannel,sizeof(*this)), iDestination(aDestination), iSourceBitmap(aSourceBitmap), iSourceRect(aSourceRect) {}
public:
	
	/** The destination for the portion of the source bitmap. */
	TRect					iDestination;
	
	/** A handle to the source bitmap, and other information needed to draw it. */
	TAcceleratedBitmapSpec	iSourceBitmap;
	
	/** A rectangle defining the part of the source bitmap to be copied. */
	TRect					iSourceRect;
	};

/**
An accelerated graphics operation that copies a rectangular region of one bitmap 
into a different sized region of another using alpha blending values provided 
in a third bitmap.

The data members are all initialised on construction. Objects of this class 
can be passed to a graphics accelerator's Operation() function either individually, 
or in a buffer.

@see TGraphicsAcceleratorCaps::iAlphaBitmap 
@publishedAll
@released
*/
class TGopScaledBitBltAlphaBitmap : public TGraphicsOperation
	{
public:
	/** Constructor with a source and destination rectangle and two bitmap handles.
	
	@param aDestination The destination for the portion of the source bitmap. If necessary, 
	the source bitmap portion is resized to fit into this rectangle.
	@param aSourceBitmap A handle to the source bitmap, and other information needed 
	to draw it.
	@param aSourceRect A rectangle within the source bitmap. Its coordinates are relative 
	to the top left of the source bitmap. Defines the part of the source bitmap to be copied.
	@param aAlphaBitmap A handle to the bitmap that contains alpha blending values. */
	inline TGopScaledBitBltAlphaBitmap(const TRect& aDestination, TAcceleratedBitmapSpec aSourceBitmap, TRect& aSourceRect, TAcceleratedBitmapSpec aAlphaBitmap)
		: TGraphicsOperation(EScaledBitBltAlphaBitmap,sizeof(*this)), iDestination(aDestination), iSourceBitmap(aSourceBitmap), iSourceRect(aSourceRect), iAlphaBitmap(aAlphaBitmap) {}
public:
	
	/** The destination for the portion of the bitmap. */
	TRect					iDestination;
	
	/** A handle to the source bitmap, and other information needed to draw it. */
	TAcceleratedBitmapSpec	iSourceBitmap;
	
	/** A rectangle defining the part of the source bitmap to be copied. */
	TRect					iSourceRect;
	
	/** A handle to the bitmap that contains alpha blending values. */
	TAcceleratedBitmapSpec	iAlphaBitmap;
	};

/**
An accelerated graphics operation that fills a polygon with a colour.

AddPoints() must be called to specify the polygon to be filled. Objects of 
this class can be passed to a graphics accelerator's Operation() function 
either individually, or in a buffer.

How a graphics accelerator can fill polygons is given by TGraphicsAcceleratorCaps::iPolygon.

@see TGraphicsAcceleratorCaps::iPolygon 
@publishedAll
@released
*/
class TGopFilledPolygon : public TGraphicsOperation
	{
public:
	/** Constructor with a fill rule and a fill colour. The number of points is initialised 
	to zero.
 	
	@param aColor The fill colour.
	@param aFillRule Bit flags for how self-crossing polygons are filled. */
	inline TGopFilledPolygon(TRgb aColor, CGraphicsContext::TFillRule aFillRule)
		: TGraphicsOperation(EFilledPolygon,sizeof(*this)), iColor(aColor), iFillRule(aFillRule), iNumPoints(0) {}
	inline void AddPoints(TInt aNumPoints, TPoint* aPoints);
public:
	
	/** The fill colour. */
	TRgb						iColor;
	
	/** Bit flags for how self-crossing polygons are filled.
	
	@see CGraphicsContext::TFillRule */
	CGraphicsContext::TFillRule iFillRule;
	
	/** The number of points in the polygon. */
	TInt						iNumPoints;
	};

/** Specifies the polygon to be filled as a number of 2D point coordinates.

AddPoints() should only be called once the TGopFilledPolygon object has been stored
into a buffer.  There must be enough room in the buffer after the TGopFilledPolygon
object to hold aNumPoints TPoint sized structures.  This is because the points are
copied into the memory space directly following the TGopFilledPolygon object.

@param aNumPoints The number of points in the polygon.
@param aPoints Pointer to the first point in the polygon. */
inline void TGopFilledPolygon::AddPoints(TInt aNumPoints, TPoint* aPoints)
	{ Append(aNumPoints*sizeof(TPoint),aPoints); iNumPoints += aNumPoints; }

/** 
An accelerated graphics operation that fills a polygon with a pattern held 
in another bitmap.

AddPoints() must be called to specify the polygon to be filled. Objects of 
this class can be passed to a graphics accelerator's Operation() function 
either individually, or in a buffer.

@see TGraphicsAcceleratorCaps::iPolygon 
@see TGopFillPattern
@publishedAll
@released
*/
class TGopFilledPolygonWithPattern : public TGraphicsOperation
	{
public:
	/** Constructor with a fill pattern and a fill rule. The number of points is initialised 
	to zero.
 	
	@param aPattern The fill pattern.
	@param aFillRule Bit flags for how self-crossing polygons are filled. */
	inline TGopFilledPolygonWithPattern(TGopFillPattern aPattern, CGraphicsContext::TFillRule aFillRule)
		: TGraphicsOperation(EFilledPolygonWithPattern,sizeof(*this)), iPattern(aPattern), iFillRule(aFillRule), iNumPoints(0) {}
	inline void AddPoints(TInt aNumPoints, TPoint* aPoints);
public:
	
	/** The pattern of bitmaps that is used to fill the polygon. */
	TGopFillPattern				iPattern;
	
	/** Bit flags for how self-crossing polygons are filled.
	
	@see CGraphicsContext::TFillRule */
	CGraphicsContext::TFillRule iFillRule;
	
	/** The number of points in the polygon. */
	TInt						iNumPoints;
	};

/** Specifies the polygon to be filled as a number of 2D point coordinates.

AddPoints() should only be called once the TGopFilledPolygonWithPattern object has been stored
into a buffer.  There must be enough room in the buffer after the TGopFilledPolygonWithPattern
object to hold aNumPoints TPoint sized structures.  This is because the points are
copied into the memory space directly following the TGopFilledPolygonWithPattern object.

@param aNumPoints The number of points in the polygon.
@param aPoints Pointer to the first point in the polygon. */
inline void TGopFilledPolygonWithPattern::AddPoints(TInt aNumPoints, TPoint* aPoints)
	{ Append(aNumPoints*sizeof(TPoint),aPoints); iNumPoints += aNumPoints; }



#endif
