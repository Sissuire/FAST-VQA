#include "FAST_def.h"
#include "FAST_fun.h"
#include "mex.h"

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
 * Please contact `yongxu.liu@stu.xidian.edu.cn` if any issue with the code.
 *
 * -----------------------------------------------------------------------
 */

void mexFunction (int nlhs, mxArray *plhs[], int nrhs,const mxArray *prhs[])
{
    // check input and output arguments.
    if( nrhs != 5 )
        mexErrMsgTxt("Error: Input Arguments.\n");
    if( nlhs > 2 )
        mexErrMsgTxt("Error: Output Arguments.\n");
    
    // Get input data.
    std::string orig(mxArrayToString(prhs[0]));
    std::string dist(mxArrayToString(prhs[1]));
    
    int width   = int(mxGetScalar(prhs[2]));
    int height  = int(mxGetScalar(prhs[3]));
    int Noof    = int(mxGetScalar(prhs[4]));
    
     // run FAST Model   
    std::vector<double> details, FAST_score;
    VideoInfo videoInfo(orig, dist, width, height, Noof);
	FAST(videoInfo, details, FAST_score);
  
     // Output.        
    int nDims = (int)details.size();
    assert((nDims % 7) == 0);
    
    int nCols = nDims / 7;
	plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
    plhs[1] = mxCreateDoubleMatrix(7, nCols, mxREAL);
    
	*mxGetPr(plhs[0]) = FAST_score[0];
	memcpy(mxGetPr(plhs[1]), &details[0], nDims*sizeof(double));
}