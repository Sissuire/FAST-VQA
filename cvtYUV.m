function [outfile, height, width, noof, flag] = cvtYUV(file, height, width, noof)
%% convert .mp4/.avi to .yuv and save it.

if file(end) == 'v' && file(end-1) == 'u' && file(end-2) == 'y'
    outfile = file;
    flag = 0;
else
    v = VideoReader(file);    
    outfile = [file(1:end-3), 'yuv'];
    height = v.Height;
    width = v.Width;
    
    noof = 0;
    fid = fopen(outfile, 'w');
    while hasFrame(v)
        frame = readFrame(v);
        
        yuv = rgb2ycbcr(frame);
        savedata(fid, yuv);    
        noof = noof + 1;
    end
    fclose(fid); 
    flag = 1;
end

end

function savedata(fid, yuv)
%subsampling of U and V | YUV420p

y = yuv(:, :, 1);
u = double(yuv(:, :, 2));
v = double(yuv(:, :, 3));

u = 0.25*(u(1:2:end, 1:2:end) + u(2:2:end, 1:2:end) + u(1:2:end, 2:2:end) + u(2:2:end, 2:2:end));
v = 0.25*(v(1:2:end, 1:2:end) + v(2:2:end, 1:2:end) + v(1:2:end, 2:2:end) + v(2:2:end, 2:2:end));

fwrite(fid, uint8(y'),'uchar'); %writes Y-Data
fwrite(fid, uint8(u'),'uchar');
fwrite(fid, uint8(v'),'uchar');
end