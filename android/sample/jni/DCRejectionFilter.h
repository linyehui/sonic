//
//  DCRejectionFilter.h
//
//
//  Created by linyehui on 2014-04-10.
//  Copyright (c) 2014年 linyehui. All rights reserved.
//

#ifndef __DC_REJECTION_FILTER_H__
#define __DC_REJECTION_FILTER_H__

class DCRejectionFilter
{
public:
	DCRejectionFilter(float poleDist = DCRejectionFilter::kDefaultPoleDist);

	void InplaceFilter(float* ioData, unsigned long numFrames);
	void Reset();

protected:

	// State variables
	float mY1;
	float mX1;

	static const float kDefaultPoleDist;
};

#endif // __DC_REJECTION_FILTER_H__
