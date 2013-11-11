// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

/*
 * Portions of this code, in particular the fghCircleTable function and the code to 
 * calculate the vertices for the solid sphere, is ported from freeglut_geometry.cpp 
 * which is distributed under the following terms:
 * 
 ****************************************************************** 
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: Fri Dec 3 1999
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *********************************************************************
 * Apart from porting to use Open GL ES instead of Open GL the other major change is 
 * to separate out the code that calculates vertices from the code that draws the shape.   
 */


#include "geometrystructs.h"

#include <GLES/gl.h>
#include <e32math.h>


float sin(float x)
	{
	double t = x;
	double r;
	TInt error = Math::Sin(r,t);
	ASSERT(error==KErrNone);
	return r;
	}

float cos(float x)
	{
	double t = x;
	double r;
	TInt error = Math::Cos(r,t);
	ASSERT(error==KErrNone);
	return r;
	}

int abs(int x)
	{
	return Abs(x);
	}

Vertex3F::Vertex3F(float x, float y, float z)
	:iX(x), iY(y), iZ(z)
	{
	}

Vertex3F::Vertex3F()
:iX(0), iY(0), iZ(0)
	{
	}

int fghCircleTable(float **sint,float **cost,const int n)
{
    int i;

    /* Table size, the sign of n flips the circle direction */
    const int size = abs(n);

    /* Determine the angle between samples */
    const float angle = 2*KPi/(float)( ( n == 0 ) ? 1 : n );

    /* Allocate memory for n samples, plus duplicate of first entry at the end */
    *sint = new float[sizeof(float) * size+1];
    
    if(*sint==NULL)
		{
		return KErrNoMemory;
		}
		
    *cost = new float[sizeof(float) * size+1];
    
    if(*cost==NULL)
		{
		delete[] sint;
		return KErrNoMemory;
		}
    
    /* Compute cos and sin around the circle */
    (*sint)[0] = 0.0;
    (*cost)[0] = 1.0;

    for (i=1; i<size; i++)
    {
    (*sint)[i] = sin(angle*i);
    (*cost)[i] = cos(angle*i);
    }

    /* Last sample is duplicate of the first */

    (*sint)[size] = (*sint)[0];
    (*cost)[size] = (*cost)[0];
    return KErrNone;
}

CSolidSphere* CSolidSphere::NewLC(TReal aRadius, TInt aSlices, TInt aStacks)
	{
	CSolidSphere* self = new(ELeave) CSolidSphere(aSlices, aStacks);
	CleanupStack::PushL(self);
	self->ConstructL(aRadius);
	return self;
	}

CSolidSphere::~CSolidSphere()
	{
	delete[] iTopVertices;
	delete[] iTopNormals;
	delete[] iBottomVertices;
	delete[] iBottomNormals;
	delete[] iStackVertices;
	delete[] iStackNormals;
	}

void CSolidSphere::Draw() const
	{
//top fan
	glVertexPointer(3, GL_FLOAT, 0, iTopVertices);
	glNormalPointer(GL_FLOAT, 0, iTopNormals);
	glDrawArrays(GL_TRIANGLE_FAN, 0, iSlices+2);
//stacks, one at a time	
	glVertexPointer(3, GL_FLOAT, 0, iStackVertices);
	glNormalPointer(GL_FLOAT, 0, iStackNormals);
	TInt offset = 0;
	TInt verticesPerStack = (iSlices+1)*2;
	for(TInt i=1; i<iStacks-1; i++ )
		{
		glDrawArrays(GL_TRIANGLE_STRIP, offset, verticesPerStack);
		offset+=verticesPerStack;
		}
//bottom fan
	glVertexPointer(3, GL_FLOAT, 0, iBottomVertices);
	glNormalPointer(GL_FLOAT, 0, iBottomNormals);
	glDrawArrays(GL_TRIANGLE_FAN, 0, iSlices+2);
	}

CSolidSphere::CSolidSphere(TInt aSlices, TInt aStacks)
	:iSlices(aSlices), iStacks(aStacks)
	{
	}

void CSolidSphere::ConstructL(TReal aRadius)
	{
	//* Pre-computed circle 
	float* sint1 = NULL;
	float* cost1 = NULL;
	float* sint2 = NULL;
	float* cost2 = NULL;
	
	
	fghCircleTable(&sint1,&cost1,-iSlices);
	CleanupArrayDeletePushL(sint1);
	CleanupArrayDeletePushL(cost1);
		
	fghCircleTable(&sint2,&cost2,iStacks*2);
	CleanupArrayDeletePushL(sint2);
	CleanupArrayDeletePushL(cost2);

	//* The top stack is covered with a triangle fan 
	int i,j;
	//* Adjust z and radius as stacks are drawn. 
	float z0,z1;
	float r0,r1;
	
	z0 = 1.0;
	z1 = cost2[(iStacks>0)?1:0];
	r0 = 0.0;
	r1 = sint2[(iStacks>0)?1:0];
	
	iTopVertices = new (ELeave) Vertex3F[iSlices+2];
	iTopNormals = new (ELeave) Vertex3F[iSlices+2];
	TInt topVerticesIndex = 0;
	iTopVertices[topVerticesIndex] = Vertex3F(0,0,aRadius);
	iTopNormals[topVerticesIndex] = Vertex3F(0,0,1);
	for(j=iSlices; j>=0; j--)
	  	{
	  	topVerticesIndex++;
	    Vertex3F vertex(cost1[j]*r1*aRadius, sint1[j]*r1*aRadius, z1*aRadius);
	    iTopVertices[topVerticesIndex] = vertex;
	    Vertex3F normal(cost1[j]*r1,        sint1[j]*r1,        z1       );
	    iTopNormals[topVerticesIndex] = normal;
	    }
	
	//*calculate the vertices for each stack
	TInt stackVertexCount = ((iSlices+1)*2) *iStacks; 
	iStackVertices = new (ELeave) Vertex3F[stackVertexCount];
	iStackNormals = new (ELeave) Vertex3F[stackVertexCount];	
	TInt stackIndex = 0;
	for( i=1; i<iStacks-1; i++ )
	    {
	    z0 = z1; z1 = cost2[i+1];
	    r0 = r1; r1 = sint2[i+1];
	    for(j=0; j<=iSlices; j++)
	       {
	       Vertex3F v1(cost1[j]*r1*aRadius, sint1[j]*r1*aRadius, z1*aRadius);
	       Vertex3F v2(cost1[j]*r0*aRadius, sint1[j]*r0*aRadius, z0*aRadius);
	       iStackVertices[stackIndex*2] = v1;
	       iStackVertices[(stackIndex*2)+1] = v2;	                 
		   Vertex3F n1(cost1[j]*r1,        sint1[j]*r1,        z1);
		   Vertex3F n2(cost1[j]*r0,        sint1[j]*r0,        z0);
		   iStackNormals[stackIndex*2] = n1;
		   iStackNormals[(stackIndex*2)+1] = n2;
		   stackIndex++;
		   }
	    }
	//* The bottom stack is covered with a triangle fan 
	z0 = z1;
	r0 = r1;

	iBottomVertices = new (ELeave) Vertex3F[iSlices+2];
	iBottomNormals = new (ELeave) Vertex3F[iSlices+2];
	
	iBottomVertices[0] = Vertex3F(0,0,-aRadius);
	iBottomNormals[0] = Vertex3F(0,0,-1);
    for(j=0; j<=iSlices; j++)
    	{
	    Vertex3F vertex(cost1[j]*r0*aRadius, sint1[j]*r0*aRadius, z0*aRadius);
	    iBottomVertices[j+1] = vertex;
	    Vertex3F normal(cost1[j]*r0, sint1[j]*r0, z0);
	    iBottomNormals[j+1] = normal;
	    }
    //* Release sin and cos tables 
    CleanupStack::PopAndDestroy(4);
	}

