%% Depth Estimation from Single Image using Bluring

%% init evvironment
close all;
clear all;
clc;

% read in imag
img=im2double(imread('new_sponge_n.bmp'));
%img = rgb2gray(img);
%% Blur the image with Gaussian kernal of diff sigma
% sigmaA = 5, 9x9 Gaussian kernel
sigmaA = 5;
imgBlrA = filter2(fspecial('gaussian', [9,9], sigmaA), img);

% sigmaB = 10, 9x9 Gaussian kernel
sigmaB = 10;
imgBlrB = filter2(fspecial('gaussian', [9,9], sigmaB), img);

% sigmaC = 5, 11x11 Gaussian kernel
sigmaC = 5;
imgBlrC = filter2(fspecial('gaussian', [11,11], sigmaC), img);

% display
%figure, subplot(2,2,1), imshow(img,[]), xlabel('Orignal image');
%hold on, subplot(2,2,2),imshow(imgBlrA, []), xlabel('Blured with 9x9 Gaussian, Sigma = 5');
%hold on, subplot(2,2,3), imshow(imgBlrB, []), xlabel('Blured with 11x11 Gaussian, Sigma = 10');
%hold on, subplot(2,2,4), imshow(imgBlrC, []), xlabel('Blured with 11x11 Gaussian, Sigma = 5');
%hold on, title('images filtered by different Gaussian kernels'), hold off;

%% Elemant-wise Ratio

% take out one row of these images and show then in 1-D
r = 240; % 240th row
imgRow = img(r,:);
imgBlrARow = imgBlrA(r,:);
imgBlrBRow = imgBlrB(r,:);

% show these rows in the same figure and compare the amptitules
%figure, plot(imgRow, 'b');
%hold on, plot(imgBlrARow, 'r');
%hold on, plot(imgBlrBRow, 'k');
%legend('imgRow', 'imgBlrARow', 'imgBlrBRow'),
%title('1-D slice of images');
%hold off;

% to make the blur independent of amplitude and offset, define a ratio
ratioNumerator = imgBlrA - imgBlrB;
ratioDenominator = (imgBlrA + imgBlrB);
ratio = ratioNumerator ./ ratioDenominator; % Note here use element-wise division
ratio = filter2(fspecial('average',11), ratio);
% 1-D slice of these images above
%figure, plot(ratioNumerator(r,:), 'b');
%hold on, plot(ratioDenominator(r,:), 'r');
%hold on, plot(ratio(r,:), 'k');
%legend('ratioNumerator', 'ratioDenominator', 'ratio');
%title('1-D slice of ratio maps');
%hold off;

%% Estimate the variance Sigma of the Gaussian kernel, which is used for modeling the blur
% Sigma = ((sigmaA * sigmaB) / (sigmaB - sigmaA)ratio_max + sigmaB)
% ratio_max is found to be at the location of edges
% image size is 480x640, and set the resolution to be 16x16, we find 30x40 max ratio for each 16x16 patch
% img = double(imread('output8.bmp'));
% [cols,rows] = size(img);
% ratio = zeros(cols,rows);
% for i = 1:(cols/8)
%     for j = 1:(rows/8)
%         window = img((i*8-7):(i*8),(j*8-7):(j*8));
%         max_num = max(window(:));
%         window(:,:) = max_num;
%         ratio((i*8-7):(i*8),(j*8-7):(j*8)) = window;
%     end
% end
patchSize = 8;
for i = 1: (size(img,1)/patchSize)
  for j = 1: (size(img,2)/patchSize)
    ratioMax = max(max(ratio((patchSize*(i-1)+1):patchSize*i, (patchSize*(j-1)+1):patchSize*j)));
    ratioNumeratorPatch = imgBlrA((patchSize*(i-1)+1):patchSize*i, (patchSize*(j-1)+1):patchSize*j) .* imgBlrB((patchSize*(i-1)+1):patchSize*i, (patchSize*(j-1)+1):patchSize*j);
    ratioDenominatorPatch = (imgBlrB((patchSize*(i-1)+1):patchSize*i, (patchSize*(j-1)+1):patchSize*j) - imgBlrA((patchSize*(i-1)+1):patchSize*i, (patchSize*(j-1)+1):patchSize*j)) .* ratio((patchSize*(i-1)+1):patchSize*i, (patchSize*(j-1)+1):patchSize*j) + imgBlrB((patchSize*(i-1)+1):patchSize*i, (patchSize*(j-1)+1):patchSize*j);
    ratioPatch = ratioNumeratorPatch ./ ratioDenominatorPatch;
    Sigma((patchSize*(i-1)+1):patchSize*i, (patchSize*(j-1)+1):patchSize*j) = abs(ratioPatch);
  end
end
%DepthMap1 = filter2(fspecial('average',3), Sigma);
DepthMap = medfilt2(Sigma, [11,11], 'symmetric');
DepthMap1 = filter2(fspecial('average',11), DepthMap);
DepthMap2 = filter2(fspecial('gaussian', [9,9], sigmaA), DepthMap);
mesh(DepthMap);
% display
%figure, subplot(2,2,1), imshow(ratioNumerator, []), xlabel('map of ratioNumerator');
%hold on, subplot(2,2,2), imshow(ratioDenominator, []), xlabel('map of ratioDenominator');
%hold on, subplot(2,2,3), imshow(double(ratio), []), xlabel('map of the ratio');
%hold on, subplot(2,2,4), mesh(DepthMap), title('DepthMap');
%hold on, title('The Ratio which is invariant to amplitude and offset
%edges'), hold off;

