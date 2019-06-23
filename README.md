# Full-reference Assessor along Salient Trajectories (FAST)

----------------------------------------------------------------------

Version 1.0, Copyright(c) 2019

Jinjian Wu, Yongxu Liu, Weisheng Dong, Guangming Shi, and Weisi Lin

All Rights Reserved.

----------------------------------------------------------------------

Permission to use, copy, or modify this software and its documentation  for educational and research purposes only and without fee is here  granted, provided that this copyright notice and the original authors'  names appear on all copies and supporting documentation. This program shall not be used, rewritten, or adapted as the basis of a commercial software or hardware product without first obtaining permission of the authors. The authors make no representations about the suitability of this software for any purpose. It is provided "as is" without express or implied warranty.

----------------------------------------------------------------------

This is an implementation of the algorithm for 
Full-reference Assessor along Salient Trajectories (FAST) VQA method

**Please refer to the following papers**

> Jinjian Wu, Yongxu Liu, Weisheng Dong, Guangming Shi, and Weisi Lin, "Quality Assessment for Video with Degradation Along Salient Trajectories" ,  IEEE TMM, 2019. 

-----------------------------------------------------------------------

======================== **Usage** ==========================

1. **OpenCV Library** is required. **OpenCV 2.4.13** is suggested, and other versions may obtain a bit different result.

2. ​
 - with MATLAB, **Run `CompileMex.m`** to configure your own OpenCV path and compile the source code. Then **Run `demo.m`** for an example.

- with VisualStudio or VSCode, just configure your OpenCV Library and **Run `demo.cpp`**.

The source code is tested with VisualStudio 2013 with OpenCV 2.4.13, and also MATLAB R2016a. 

-----------------------------------------------------------------------

NOTE：

To output some variables during computation, the code is a little different from the one used for running time testing. However, there's only serveral hundreds milliseconds at most in terms of computational time, which is up to your machine. The additional computational time can be ignored in most cases, since many other VQA methods require far more. 

-----------------------------------------------------------------------

Please contact `yongxu.liu@stu.xidian.edu.cn` if any issue with the code.

