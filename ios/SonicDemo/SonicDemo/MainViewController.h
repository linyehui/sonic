//
//  MainViewController.h
//  SonicDemo
//
//  Created by linyehui on 14-2-20.
//  Copyright (c) 2014年 linyehui. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface MainViewController : UIViewController
{
    NSTimer *animationTimer;
    NSTimeInterval animationInterval;
}

- (void)runComputeWave;
- (IBAction)genWaveDataClicked:(id)sender;
- (IBAction)computerWaveClicked:(id)sender;

@end
