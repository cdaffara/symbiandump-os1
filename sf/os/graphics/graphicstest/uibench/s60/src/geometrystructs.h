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
 Function to calculate sine and cosine values for points of a circle
 composed of n segments.
 The function is taken from freeglut_geometry.cpp, which is distributed
 under the following terms:
 * 
 *  ****************************************************************** 
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
 * ********************************************************************
 *   
 */


#ifndef GEOMETRYSTRUCTS_H_
#define GEOMETRYSTRUCTS_H_


#include <e32base.h>


float sin(float x);
float cos(float x);

struct Vertex3F
	{
	Vertex3F(float x, float y, float z);
	Vertex3F();
	float iX;
	float iY;
	float iZ;
	};

int fghCircleTable(float **sint,float **cost,const int n);


class CSolidSphere : public CBase
	{
public:
	static CSolidSphere* NewLC(TReal aRadius, TInt aSlices, TInt aStacks);
	~CSolidSphere();
	void Draw() const;
	
private:
	CSolidSphere(TInt aSlices, TInt aStacks);
	void ConstructL(TReal aRadius);
	
private:
	TInt iSlices;
	TInt iStacks;
	Vertex3F* iTopVertices;
	Vertex3F* iTopNormals;
	Vertex3F* iBottomVertices;
	Vertex3F* iBottomNormals;
	Vertex3F* iStackVertices;
	Vertex3F* iStackNormals;
	};


#endif /* GEOMETRYSTRUCTS_H_ */
