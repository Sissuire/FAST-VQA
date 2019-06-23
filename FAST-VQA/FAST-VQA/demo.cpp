/*
* =============================================================================================
* FAST - Full-reference Assessor along Salient Trajectories for video
* ----------------------------------------------------------------------
* Copyright(c) 2019 Jinjian Wu, Yongxu Liu, Weisheng Dong, Guangming Shi, and Weisi Lin
* All Rights Reserved.
* ----------------------------------------------------------------------
* Permission to use, copy, or modify this software and its documentation
* for educational and research purposes only and without fee is here
* granted, provided that this copyright notice and the original authors'
* names appear on all copies and supporting documentation. This program
* shall not be used, rewritten, or adapted as the basis of a commercial
* software or hardware product without first obtaining permission of the
* authors. The authors make no representations about the suitability of
* this software for any purpose. It is provided "as is" without express
* or implied warranty.
* ----------------------------------------------------------------------
*
* This is an implementation of the algorithm for creating the
* Full-reference Assessor along Salient Trajectories (FAST) VQA method
*
* Please contact `yongxu.liu@stu.xidian.edu.cn` if any issue with the code.
*
* Please refer to the following paper
*
* Jinjian Wu, Yongxu Liu, Weisheng Dong, Guangming Shi, and Weisi Lin
* "Quality Assessment for Video with Degradation Along Salient Trajectories"
* IEEE TMM, 2019.
* -----------------------------------------------------------------------
* Input:
* Orig           - reference video file.
* Dist           - disorted video file.
* width          - width of each frame.
* height         - height of each frame.
* Noof           - the whole number of frames.
*
* Output:
* FAST_score     - the quality score of testing video.
* details        - more information during the computation (see `demo.m`).
* -----------------------------------------------------------------------
* 
* This code depends on the OpenCV library. Please ensure the OpenCV library
* is configured. Further, as the code is tested on OpenCV 2.4.13, other 
* versions may result in some different performance.(just a bit)
* 
* -----------------------------------------------------------------------
*/

#include "FAST_def.h"
#include "FAST_fun.h"

void main()
{
	/* 
	-----------------------------------------------------------
	| NOTE:													  |
	| ---------------------------------------------------------
	| only yuv420p is used.									  |
	| if the input format is not yuv420p,					  |
	| please change the code of "ReadYUV420p" in 'FAST_fun.h' |
	| it's easy enough for the converting.					  |
	-----------------------------------------------------------
	*/
	std::string ref = "J:/_VideoDatabase/LIVE/ref/pa1_768x432_25fps.yuv";  // set your reference video
	std::string dst = "J:/_VideoDatabase/LIVE/all-videos/pa12_768x432_25fps.yuv"; // set your test video

	int width		= 768;  
	int height		= 432;
	int noof		= 250; // no. of frames

	clock_t tic = clock();

	VideoInfo videoInfo(ref, dst, width, height, noof);
	
	std::vector<double> details, FAST_score(4, 0.);
	FAST(videoInfo, details, FAST_score);

	clock_t toc = clock();

	std::cout << std::fixed << std::setprecision(7);
	std::cout << "____________________________________" << std::endl;
	std::cout << "|-----------------------------------" << std::endl;
	std::cout << "| Video Quality |\t" << FAST_score[0] << std::endl;
	std::cout << "|-----------------------------------" << std::endl;
	std::cout << "| spatial score |\t" << FAST_score[1] << std::endl;
	std::cout << "| optical flow  |\t" << FAST_score[2] << std::endl;
	std::cout << "| sptiotemporal |\t" << FAST_score[3] << std::endl;
	std::cout << "|-----------------------------------" << std::endl;
	std::cout << "| time elapsed:\t" << double(toc - tic) / CLOCKS_PER_SEC << " seconds" << std::endl;
	std::cout << "------------------------------------" << std::endl << std::endl;

	system("pause");
	return;
}