% 
% Full-reference Assessor along Salient Trajectories (FAST)
% Version 1.0, Copyright(c) 2019
% 
% Jinjian Wu, Yongxu Liu, Weisheng Dong, Guangming Shi, and Weisi Lin
% All Rights Reserved.
% ----------------------------------------------------------------------
% Permission to use, copy, or modify this software and its documentation  
% for educational and research purposes only and without fee is here  
% granted, provided that this copyright notice and the original authors'  
% names appear on all copies and supporting documentation. This program 
% shall not be used, rewritten, or adapted as the basis of a commercial 
% software or hardware product without first obtaining permission of the 
% authors. The authors make no representations about the suitability of 
% this software for any purpose. It is provided "as is" without express 
% or implied warranty.
% ----------------------------------------------------------------------
% This is an implementation of the algorithm for 
% Full-reference Assessor along Salient Trajectories (FAST) VQA method
% 
% **Please refer to the following papers**
% Jinjian Wu, Yongxu Liu, Weisheng Dong, Guangming Shi, and Weisi Lin, 
% "Quality Assessment for Video with Degradation Along Salient Trajectories",
% IEEE TMM, 2019. 
%
% Please contact `yongxu.liu@xidian.edu.cn` if any issue with the code.
% -----------------------------------------------------------------------
% 

clear
close all
% clc

%% Compile the Code
%### set your path of OpenCV Library in the function.
% CompileMex();

%% Provide testing video information
% Note: ONLY YUV420p is supported correctedly.
% Conversion is needed if the format is YUV422 or others.

% reference    = 'J:/_VideoDatabase/LIVE/ref/pa1_768x432_25fps.yuv';
% distorted    = 'J:/_VideoDatabase/LIVE/all-videos/pa2_768x432_25fps.yuv';
reference = 'B001.mp4';
distorted = 'B001.mp4';

width        = 768;   % ignore these parameters if the input videos are .mp4
height       = 432;
no_of_frames = 300;


%% Converting the format if the video is not based on YUV420p
% To support more format like .mp4, you should revise the code in `FAST_fun.cpp` and compile the code. 
[reference, height, width, noof1, flag_r] = cvtYUV(reference, height, width, no_of_frames);
[distorted, ~, ~, noof2, flag_d] = cvtYUV(distorted, height, width, no_of_frames);

no_of_frames = min(noof1, noof2);

%% Running the VQA model

%
% FAST_score is the final video quality through FAST computation, which can
% be obtained via `outInfo` also. 
%
% outInfo is 7xN matrix, which contains
%           1. mean of subsequence-based spatial quality ( S )
%           2. mean of subsequence-based motion velocity quality (MV-m)
%           3. stdev of subsequence-based motion velocity quality (MV-s)
%           4. mean of subsequence-based motion content quality (MC-m)
%           5. stdev of subsequence-based motion content quality (MC-s)
%           6. the start_index for each subsequence
%           7. the end_index for each subsequence
%
%   indices of starting and ending positions can be used to substitute the
%   original IQA metric (GMSD) in this work, through calculating the mean
%   value of j-th subsequence as:
%      Spatial_Score(j) = mean( IQA_Score( start_index(j), end_index(j) ) );
%

tic

outInfo = FAST_matlab(reference, distorted, width, height, no_of_frames);

toc

if flag_r
    delete(reference);
end

if flag_d
    delete(distorted);
end

%% Getting results
QualitySm   = outInfo(1, :);
QualityMVm  = outInfo(2, :);
QualityMVs  = outInfo(3, :);
QualityMCm  = outInfo(4, :);
QualityMCs  = outInfo(5, :);
startIndex  = outInfo(6, :);
endIndex    = outInfo(7, :);

% as the multiplication of several decimals is too small, 
% a constant is used to elevate the final quality here.
Qs  = QualitySm;
Qt  = QualityMVm + QualityMVs;
Qst = QualityMCm + QualityMCs;

% use `sqrt` to condense the scale. (tricky)
VideoQuality = sqrt(mean(10000 * Qs .* Qt .* Qst));  

fprintf('The quality is %.4f.\n', VideoQuality)
