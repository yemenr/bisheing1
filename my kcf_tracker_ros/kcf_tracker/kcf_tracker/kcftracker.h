/*

Tracker based on Kernelized Correlation Filter (KCF) [1] and Circulant Structure with Kernels (CSK) [2].
CSK is implemented by using raw gray level features, since it is a single-channel filter.
KCF is implemented by using HOG features (the default), since it extends CSK to multiple channels.

[1] J. F. Henriques, R. Caseiro, P. Martins, J. Batista,
"High-Speed Tracking with Kernelized Correlation Filters", TPAMI 2015.

[2] J. F. Henriques, R. Caseiro, P. Martins, J. Batista,
"Exploiting the Circulant Structure of Tracking-by-detection with Kernels", ECCV 2012.

Authors: Joao Faro, Christian Bailer, Joao F. Henriques
Contacts: joaopfaro@gmail.com, Christian.Bailer@dfki.de, henriques@isr.uc.pt
Institute of Systems and Robotics - University of Coimbra / Department Augmented Vision DFKI


Constructor parameters, all boolean:
hog: use HOG features (default), otherwise use raw pixels
fixed_window: fix window size (default), otherwise use ROI size (slower but more accurate)
multiscale: use multi-scale tracking (default; cannot be used with fixed_window = true)

Default values are set for all properties of the tracker depending on the above choices.
Their values can be customized further before calling init():
interp_factor: linear interpolation factor for adaptation
sigma: gaussian kernel bandwidth
lambda: regularization
cell_size: HOG cell size
padding: horizontal area surrounding the target, relative to its size
output_sigma_factor: bandwidth of gaussian target
template_size: template size in pixels, 0 to use ROI size
scale_step: scale step for multi-scale estimation, 1 to disable it
scale_weight: to downweight detection scores of other scales for added stability

For speed, the value (template_size/cell_size) should be a power of 2 or a product of small prime numbers.

Inputs to init():
image is the initial frame.
roi is a cv::Rect with the target positions in the initial frame

Inputs to update():
image is the current frame.

Outputs of update():
cv::Rect with target positions for the current frame


By downloading, copying, installing or using the software you agree to this license.
If you do not agree to this license, do not download, install,
copy or use the software.


License Agreement
For Open Source Computer Vision Library
(3-clause BSD License)

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

* Neither the names of the copyright holders nor the names of the contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

This software is provided by the copyright holders and contributors "as is" and
any express or implied warranties, including, but not limited to, the implied
warranties of merchantability and fitness for a particular purpose are disclaimed.
In no event shall copyright holders or contributors be liable for any direct,
indirect, incidental, special, exemplary, or consequential damages
(including, but not limited to, procurement of substitute goods or services;
loss of use, data, or profits; or business interruption) however caused
and on any theory of liability, whether in contract, strict liability,
or tort (including negligence or otherwise) arising in any way out of
the use of this software, even if advised of the possibility of such damage.
*/

#pragma once


#include "tracker.h"

#ifndef _OPENCV_KCFTRACKER_HPP_
#define _OPENCV_KCFTRACKER_HPP_
#endif

//�̳�Tracker
class KCFTracker : public Tracker
{
public:
	// Constructor  ����KCF����������
	KCFTracker(bool hog = true,   //ʹ��HOG����
		bool fixed_window = true,   //ʹ�ù̶�����
		bool multiscale = true,    //ʹ�ö�߶�
		bool lab = true);    //ʹ��Lab��ɫ�ռ�����

	
	// Initialize tracker 
	//��ʼ����������roi��Ŀ���ʼ������ã�image�ǽ�����ٵĵ�һ��ͼ��
	virtual void init(const cv::Rect &roi, cv::Mat image);

	
	// Update position based on the new frame
	//������һ֡��ͼ�����Ŀ��λ�ã�image����һ֡ͼ��
	//��߶ȶ��������ͨ�����һ������һ��С��ĳ߶ȣ��Ƚ�������ֵ�Ľ�������ж�߶ȵ���Ӧ
	virtual cv::Rect update(cv::Mat image);  // cv::Rect�Ƿ�������


	float interp_factor; // linear interpolation factor for adaptation
						//����Ӧ�����Բ�ֵ���ӣ�����ΪHOG,Lab��ѡ��仯
	float sigma; // gaussian kernel bandwidth
				//��˹�����˴���������ΪHOG,Lab��ѡ��仯
	float lambda; // regularization
					//���򻯲�����0.0001
	int cell_size; // HOG cell size 
					//ϸ����Ԫ�ߴ�
	int cell_sizeQ; // cell size^2, to avoid repeated operations
					//ϸ����Ԫ�����أ�����6*6��������Ŀ
	float padding; // extra area surrounding the target
				//Ŀ����չ����������
	float output_sigma_factor; // bandwidth of gaussian target
							//��˹Ŀ��Ĵ���
	int template_size; // template size
	                //ģ���С���ڼ���_tmpl_szʱ��  
                    // �ϴ��ɱ���һ��96������С�߳���������С 
	float scale_step; // scale step for multi-scale estimation
					//��߶ȹ��Ƶĳ߶Ȳ���
	float scale_weight;  // to downweight detection scores of other scales for added stability
						//Ϊ�����������߶ȼ��ʱ���ȶ��ԣ����������һ����˥���ȣ�Ϊԭ����0.95
protected:
	// Detect object in the current frame.
	//��⵱ǰ֡��Ŀ��
	//z��ǰһ֡��ѵ�������ǵ�һ֡�ĳ�ʼ�������x�ǵ�ǰ֡�߶��µ�������peak_value�Ǽ������ֵ
	cv::Point2f detect(cv::Mat z, cv::Mat x, float &peak_value);

	
	// train tracker with a single image
	//ʹ�õ�ǰͼ��ļ��������ѵ����x�ǵ�ǰ֡�߶��µ�������train_interp_factor��interp_factor
	void train(cv::Mat x, float train_interp_factor);

	// Evaluates a Gaussian kernel with bandwidth SIGMA for all relative shifts between input images X and Y, 
	//which must both be MxN. They must also be periodic (ie., pre-processed with a cosine window).
	//ʹ�ô���SIGMA�����˹��������������ͼ��x��y֮������λ�ƣ�
	//������M*N��С�����߱����������Եģ�ͨ��һ��cos(����)���ڽ���Ԥ������
	cv::Mat gaussianCorrelation(cv::Mat x1, cv::Mat x2);

	
	// Create Gaussian Peak. Function called only in the first frame.
	//������˹��ֵ�������˺���ֻ�ڵ�һ֡ʱ����
	cv::Mat createGaussianPeak(int sizey, int sizex);

	// Obtain sub-window from image, with replication-padding and extract features
	//��ͼ��õ�2.5���Ӵ��ڣ���������ͨ����ֵ��䲢��֤
	cv::Mat getFeatures(const cv::Mat & image, bool inithann, float scale_adjust = 1.0f);

	// Initialize Hanning window. Function called only in the first frame.
	//��ʼ��Hanning���ڣ�����
	void createHanningMats();

	// Calculate sub-pixel peak for one dimension
	//����һά�����ط�ֵ��������������֮�仹��sub-pixel
	float subPixelPeak(float left, float center, float right);

	cv::Mat _alphaf;  //��ʼ��/ѵ�����alpha,���ڼ�ⲿ���н���ļ��㣬����������֤���˰���ĵ�alpha
	cv::Mat _prob;   //��ʼ��������ʣ����ø��ģ�����ѵ������Ԥ�⵽��Ŀ��λ��
	cv::Mat _tmpl;  //��ʼ��/ѵ���Ľ��������detect��z���������е�����ͼ��z
	
	cv::Mat _num;   //
	cv::Mat _den;
	
	cv::Mat _labCentroids; //Lab��������

private:
	int size_patch[3];   //HOG������sizeY��sizeX��numFeatures��ά����
	cv::Mat hann;    //createHanningMats()�ļ�����
	cv::Size _tmpl_sz;  //HOGϸ��Ԫ��Ӧ�������С
	float _scale;  //������_tmpl_sz��ĳ߶ȴ�С���߶Ȳ���ָ����ĳߴ��С����ĳһ�����Ĳ���
	
	int _gaussian_size; //����õ��ĸ�˹��С
	
	bool _hogfeatures;   //HOG��־λ
	bool _labfeatures;  //lab��־λ
};