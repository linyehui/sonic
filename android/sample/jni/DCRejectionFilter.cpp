//
//  DCRejectionFilter.cpp
//
//
//  Created by linyehui on 2014-04-10.
//  Copyright (c) 2014年 linyehui. All rights reserved.
//

#include "DCRejectionFilter.h"

const float DCRejectionFilter::kDefaultPoleDist = 0.975f;

//////////////////////////////////////////////////////////

DCRejectionFilter::DCRejectionFilter(float poleDist)
{
	Reset();
}

void DCRejectionFilter::Reset()
{
	mY1 = mX1 = 0;
}

void DCRejectionFilter::InplaceFilter(float* ioData, unsigned long numFrames)
{
	for (unsigned long i=0; i < numFrames; i++)
	{
        float xCurr = ioData[i];
		ioData[i] = ioData[i] - mX1 + (kDefaultPoleDist * mY1);
        mX1 = xCurr;
        mY1 = ioData[i];
	}
}
