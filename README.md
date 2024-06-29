# Full-reference Assessor along Salient Trajectories (FAST)

----------------------------------------------------------------------

This is an implementation of the algorithm for 
Full-reference Assessor along Salient Trajectories (FAST) VQA method

**Please refer to the following papers**

> Jinjian Wu, Yongxu Liu, Weisheng Dong, Guangming Shi, and Weisi Lin, "Quality Assessment for Video with Degradation Along Salient Trajectories" ,  IEEE TMM, 2019. 

-----------------------------------------------------------------------

======================== **Usage** ==========================

**upate** 2024.06.29

We found that the code would encouter ERRORS when the input videos are NOT based on `.yuv` (some other formats like `.mp4/.avi`). The best way is to check the code in  `FAST_fun.cpp` [Line 523](https://github.com/Sissuire/FAST-VQA/blob/aec21fbfa7a6d2c590c2688d6f5ce63e6e2a2c34/FAST-VQA/FAST-VQA/FAST_fun.cpp#L523), debug and re-compile the code. 

An alternative solution is to convert the `.mp4` videos to `.yuv` (YUV420p) files. We add a new file `cvtYUV.m` to do so, and this would definitely slow down the runtime, but I donot have much energy to maintain the whole code. Sorry for that. A more efficient way is to use `ffmpeg` to convert the file format quickly. `ffmpeg -i input_file.mp4 -f rawvideo -pix_fmt yuv420p output_file.yuv`


**upate**

Now, the compiled files **.MEXW/MEXA** are available, which can be used directly in MATLAB if you are not interested in the detailed implementation in the source code. **Run `demo.m`** for an example.

---------

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

