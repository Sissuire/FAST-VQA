#include "FAST_def.h"
#include "FAST_fun.h"

void FAST(const VideoInfo &videoInfo, std::vector<double> &details, std::vector<double> &quality)
// void FAST(const VideoInfo &videoInfo, std::vector<std::vector<double>> &quality)
{
    /*
     * args:
     * - videoInfo	: IN : a data structure containing basic information about the videos.(width, height, file name, ...)
     * - details    : OUT: to save the details during computation. 
	 * - quality    : OUT: to save the final quality: q_ovearll, q_spatial, q_opticalFlow, q_spatio-temporal
     */
    
    std::list<Track> tracker;
    
    int trajLength = traj_length;
    
    int initGap = int((float)trajLength * init_gap_r); // gap for initialize points
    
    std::vector<cv::Mat> Orig, Dist;
    GetVideos(videoInfo.file_Orig, Orig, videoInfo.width, videoInfo.height, videoInfo.frameNo, 0);
    GetVideos(videoInfo.file_Dist, Dist, videoInfo.width, videoInfo.height, videoInfo.frameNo, 0);
    
    cv::Mat flowOrig, flowDist;
    
    /* initialize info.(about tracking, hof, and flow) */
    TrackInfo trackInfo(trajLength, initGap, init_size);
    DescInfo hofInfo(patch_size, patch_size, nxy_cell, nxy_cell, histNBins);
    FlowInfo flowInfo(fb_pyr_scale, fb_levels, fb_winsize, fb_iterations, fb_poly_n, fb_pol_sigma);
    
    cv::Mat mask(Orig[0].size(), CV_32FC1);
    CenterBias(mask, .9, 0.);
    
    /* sample points for the first frame, saving in tracker */
    DenseSample(Dist[0], mask, tracker, trackInfo, hofInfo, sample_rate, 0);
    
    std::list<double> sQualityInList; // frame-by-frame spatial quality in each subsequence
    std::list<double> startList, endList; // start_ and end_ indices for each subsequence
    std::vector<std::list<double>> mvQualityInList(2), // motion velocity degradation along trajectories (0: mean, 1:stdev)
								   mcQualityInList(2); // motion content degradation along trajectories (0: mean, 1:stdev)
    int dims = int(Orig.size());
    std::vector<double> spatials(dims, 0.);
    
    int start_ = 0, end_ = 0;
    int width = 0;
    int height = 0;
    
    bool flag_st = true;
    int init_counter = 0; // counter for re-sampling points
    
    for (int frame_num = 1; frame_num < dims - 1; ++frame_num)
    {
        ComputeGMSD(Orig[frame_num], Dist[frame_num], spatials, frame_num);
		
        /* compute optical flow */
        CalcFlowWithFarneback(Orig[frame_num], Orig[frame_num + 1], flowOrig, flowInfo);
        CalcFlowWithFarneback(Dist[frame_num], Dist[frame_num + 1], flowDist, flowInfo);

        width = flowOrig.cols;
        height = flowOrig.rows;
        ++init_counter;
        
        /* Compute Integral Histograms for Optical Flow */
        DescMat hofOrig(width + 1, height + 1, hofInfo.nBins);
        DescMat hofDist(width + 1, height + 1, hofInfo.nBins);
        HofComp(flowOrig, hofOrig.desc, hofInfo);
        HofComp(flowDist, hofDist.desc, hofInfo);
        
        std::list<double> tMV, tMC;
        
        bool valid_flag = false; // set it 'True' if tracking finished.
        int index = 0;
        int x, y;
        float dx, dy;
        cv::Rect rect;
        std::list<std::vector<cv::Point>> region_inhibit; // for removing two near-by trajectories.
        
        for (auto iTrack = tracker.begin(); iTrack != tracker.end();)
        {
            index = iTrack->index;
            cv::Point prevPoint = iTrack->point[index], currPoint;
            
            /* previous point */
            x = std::min<int>(std::max<int>(prevPoint.x, 0), width - 1);
            y = std::min<int>(std::max<int>(prevPoint.y, 0), height - 1);
            
            /* flow offset */
            dx = flowDist.ptr<float>(y)[2 * x];
            dy = flowDist.ptr<float>(y)[2 * x + 1];
            
            /* predicting current point */
            currPoint.x = prevPoint.x + cvRound(dx);
            currPoint.y = prevPoint.y + cvRound(dy);
            
            /* remove point out of boundary, or with a large displacement */
            if (currPoint.x <= 0 || currPoint.y <= 0 || currPoint.x >= width || currPoint.y >= height
                    || dx > max_dis || dx < -max_dis || dy > max_dis || dy < -max_dis)
            {
                iTrack = tracker.erase(iTrack);
                continue;
            }
            
            /* add displacement if current point is valid */
            iTrack->disp[index].x = dx;
            iTrack->disp[index].y = dy;
            
            /* add Hof features */
            GetRect(prevPoint, rect, width, height, hofInfo);
            GetDesc(hofOrig, rect, hofInfo, iTrack->OrigHof, index);
            GetDesc(hofDist, rect, hofInfo, iTrack->DistHof, index);
            
            /* push rect */
            iTrack->rects.push_back(rect);
            
            /* add current point */
            iTrack->addPoint(currPoint);
            
            /* check if tracking finished */
            if (iTrack->index == trackInfo.length)
            {
                if (IsValid(iTrack->point, iTrack->disp)) // False, if displacement of trajectories are random, or the trajectories are static.
                {
                    if (IsOverlapping(iTrack->point, region_inhibit, trackInfo.length)) // False, if the trajectory is close to some one already existing.
                    {
                        valid_flag = true;
                        start_ = iTrack->start_frame;
                        end_ = iTrack->end_frame;
                        
                        std::vector<cv::Mat> OrigRect, DistRect;
                        OrigRect.reserve(trackInfo.length);
                        DistRect.reserve(trackInfo.length);
                        GetCube(Orig, OrigRect, iTrack->rects, start_, end_);
                        GetCube(Dist, DistRect, iTrack->rects, start_, end_);
                        GetScore_st(OrigRect, DistRect, tMC);
                        
                        GetScore_t(iTrack->OrigHof, iTrack->DistHof, hofInfo, trackInfo, tMV); // compute dissimilarity between two Hof features.
                        
                    }
                }
                iTrack = tracker.erase(iTrack); // if calculate the dissimilarity of the trajectory, then, remove it.
                continue;
            }
            ++iTrack;
        }
        
        
        if (valid_flag) // if there's temporal quality computation, i.e., some trajectories finished, we convert these dissimilarity into spatial/temporal/combing quality.
        {
            //mvQualityInList.push_back(SimpleTemporalPooling(tMV)); // temporal pooling
            SimpleTemporalPooling(mvQualityInList, tMV);
            GetScore_s(sQualityInList, spatials, start_ + 1, end_ + 1); // get the spatial quality
            
            //mcQualityInList.push_back(SimpleTemporalPooling(tMC));
            SimpleTemporalPooling(mcQualityInList, tMC);
            
            startList.push_back(double(start_ + 2));
            endList.push_back(double(end_ + 1));
        }
        
        region_inhibit.clear();
        hofOrig.Release();
        hofDist.Release();
        tMV.clear();
        tMC.clear();
        
        if ((init_counter == trackInfo.gap) || (tracker.size() < trackInfo.thre_size)) // re-sample points
        {
            DenseSample(Dist[frame_num], mask, tracker, trackInfo, hofInfo, sample_rate, frame_num);
            init_counter = 0;
        }
    }

    /* output the intermediate variables */
	/* combine spatial, temporal as well as spatio-temporal qualities */
    size_t outDims = sQualityInList.size();
    assert(outDims == mcQualityInList[0].size() && outDims == mvQualityInList[0].size());
    
	double sizes = double(outDims);
	double sq, tq, stq, score;
	sq = tq = stq = score = 0.;
    
	outDims *= 7;
	details.resize(outDims);
    
    auto iter_s = sQualityInList.begin();
    auto iter_startIdx = startList.begin(), iter_endIdx = endList.begin();
    auto iter_mvm = mvQualityInList[0].begin(), iter_mvs = mvQualityInList[1].begin(),
            iter_mcm = mcQualityInList[0].begin(), iter_mcs = mcQualityInList[1].begin();

    for(size_t j = 0; j < outDims;
            ++iter_s, ++iter_mvm, ++iter_mvs, ++iter_mcm, ++iter_mcs, ++iter_startIdx, ++iter_endIdx) 
    {
		details[j++] = (*iter_s);
		details[j++] = (*iter_mvm);
		details[j++] = (*iter_mvs);
		details[j++] = (*iter_mcm);
		details[j++] = (*iter_mcs);
		details[j++] = (*iter_startIdx);
		details[j++] = (*iter_endIdx);

        sq    += (*iter_s);		// spatial
        tq    += (*iter_mvm + *iter_mvs); // motion velocity - temporal
        stq   += (*iter_mcm + *iter_mcs); // motion content - spatiotemporal
        score += (10000 * (*iter_s) * (*iter_mvm + *iter_mvs) * (*iter_mcm + *iter_mcs)); // since the multiplication result is too small, 10000x it.
    }

	sq		  /= sizes;
	tq		  /= sizes;
	stq		  /= sizes;
	score	  /= sizes;

	quality.resize(4);
	quality[0] = score;
	quality[1] = sq;
	quality[2] = tq;
	quality[3] = stq;

    return;
}