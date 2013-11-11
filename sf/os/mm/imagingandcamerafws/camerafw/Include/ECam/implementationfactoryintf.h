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
 @publishedPartner
 @prototype
*/

#ifndef  IMPLEMENTATIONFACTORYINTF_H
#define  IMPLEMENTATIONFACTORYINTF_H

/**
Union used to provide future flexibility if extra details are needed by the abstract factory methods.

@publishedPartner
@prototype
*/
union TECamImplFactoryParam
    {
public:
	/** explicit constructor when the union is used to represent a TAny* parameter */
    explicit TECamImplFactoryParam(TAny* aPtrParam);
    
    /** explicit constructor when the union is used to represent a TInt parameter */
    explicit TECamImplFactoryParam(TInt aIntParam);
    
    /** explicit constructor when the union is used to represent a TUint parameter */
    explicit TECamImplFactoryParam(TUint aUintParam);

public:
	/** union may represent a TAny* parameter in order to provide extra information for creating concrete implementation. */
    TAny*   iPtrParam;
    /** union may represent a TInt parameter in order to provide extra information for creating concrete implementation. */
    TInt    iIntParam;
    /** union may represent a TUint parameter in order to provide extra information for creating concrete implementation. */
    TUint   iUintParam;
    };

/**
Abstract Factory Class in order to derive different concrete factory class for image processing implementation specific to 
VideoCapture and specific Viewfinder. 

Also used to derive concrete factory class for snapshot implementation specific to image capture and video capture.

Also used to derive concrete factory class for histogram implementation specific to still image, video, snapshot or specific viewfinder. 

This may be used in other possible cases as well.

@publishedPartner
@prototype
*/	
class MImplementationFactory
	{
public:

	/** 
	Releases the interface. 
	*/
	virtual void Release() =0;
	
	/**
	Provides implementation handle for different implementation products.
		
	@param aIfPtr
		   Retrieves pointer to specifc interface implementation as identified by the interface uid.
	
	@param aIfaceUid
		   The interface uid.
		   
	@return any defined error code.
	
	@note  KErrNone should be returned only when a valid interface pointer is there. Any other error code implies that
		   interface implementation pointer is NULL.
	
	@note  KErrNotSupported should be returned when the concrete interface implementation is not present.
	
	@note  The inferface pointer is retrieved only on the basis of interface uid passed.
		   Examples of 'interface implementation pointer' - 'interface uid' are as follows:-
		   MCameraImageProcessing* - KECamMCameraImageProcessingUid; 
		   MCameraImageProcessing2* - KECamMCameraImageProcessing2Uid;
		   MCameraImageProcessing3* - KECamMCameraImageProcessing3Uid;
		   
		   MCameraSnapshot* - KECamMCameraSnapshotUid;
		   MCameraSnapshot2* - KECamMCameraSnapshot2Uid;
		   
		   MCameraV2Histogram* - KECamMCameraV2HistogramUid;
		   
		   Many other pairs may be possible in future. For example when image processing API/ camera snapshot API/ 
		   histogram API is further extended. 
		   Or when same concept is being used on other ECam related sub-component.
	*/
	virtual TInt GetImpl(TAny*& aIfPtr, TUid aIfaceUid) const=0;

	/**
	Overloaded method which helps in retrieving the appropriate implementation pointer
	based on extra information provided through a single 'TECamImplFactoryParam'.
	
	Provides implementation handle for different implementation products.
	
	@param aIfPtr
		   Retrieves pointer to specifc interface implementation as identified by the interface uid.
	
	@param aIfaceUid
		   The interface uid.
		   
	@param aParam1
		   union used to provide extra information which could a TAny*, TInt or TUint.
		   
	@return error code
	
	@note  The inferface pointer is retrieved only on the basis of interface uid passed.
		   Examples of 'interface implementation pointer' - 'interface uid' are as follows:-
		   MCameraImageProcessing* - KECamMCameraImageProcessingUid; 
		   MCameraImageProcessing2* - KECamMCameraImageProcessing2Uid;
		   MCameraImageProcessing3* - KECamMCameraImageProcessing3Uid;
		   
		   MCameraSnapshot* - KECamMCameraSnapshotUid;
		   MCameraSnapshot2* - KECamMCameraSnapshot2Uid;
		   
		   MCameraV2Histogram* - KECamMCameraV2HistogramUid;
		   
		   Many other pairs may be possible in future. For example when image processing API/ camera snapshot API/ 
		   histogram API is further extended. 
		   Or when same concept is being used on other ECam related sub-component.
	*/
	virtual TInt GetImpl1(TAny*& aIfPtr, TUid aIfaceUid, TECamImplFactoryParam aParam1) const=0;
	
	/**
	Overloaded method which helps in retrieving the appropriate implementation pointer
	based on extra information provided through two different 'TECamImplFactoryParam'.
	
	Provides implementation handle for different implementation products.
	
	@param aIfPtr
		   Retrieves pointer to specifc interface implementation as identified by the interface uid.
	
	@param aIfaceUid
		   The interface uid.
		   
	@param aParam1
		   union used to provide extra information which could a TAny*, TInt or TUint.
		   
	@param aParam2
		   union used to provide extra information which could a TAny*, TInt or TUint.
		   
	@return error code
	
	@note  The inferface pointer is retrieved only on the basis of interface uid passed.
		   Examples of 'interface implementation pointer' - 'interface uid' are as follows:-
		   MCameraImageProcessing* - KECamMCameraImageProcessingUid; 
		   MCameraImageProcessing2* - KECamMCameraImageProcessing2Uid;
		   MCameraImageProcessing3* - KECamMCameraImageProcessing3Uid;
		   
		   MCameraSnapshot* - KECamMCameraSnapshotUid;
		   MCameraSnapshot2* - KECamMCameraSnapshot2Uid;
		   
		   MCameraV2Histogram* - KECamMCameraV2HistogramUid;
		   
		   Many other pairs may be possible in future. For example when image processing API/ camera snapshot API/ 
		   histogram API is further extended. 
		   Or when same concept is being used on other ECam related sub-component.
	*/
    virtual TInt GetImpl2(TAny*& aIfPtr, TUid aIfaceUid, TECamImplFactoryParam aParam1, TECamImplFactoryParam aParam2) const=0;
	};

inline
TECamImplFactoryParam::TECamImplFactoryParam(TAny* aPtrParam):iPtrParam(aPtrParam)
    {
    }

inline
TECamImplFactoryParam::TECamImplFactoryParam(TInt aIntParam):iIntParam(aIntParam)
    {
    }

inline
TECamImplFactoryParam::TECamImplFactoryParam(TUint aUintParam):iUintParam(aUintParam)
    {
    }

	
#endif // IMPLEMENTATIONFACTORYINTF_H
