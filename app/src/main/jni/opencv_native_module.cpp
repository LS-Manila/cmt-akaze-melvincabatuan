#include <jni.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <vector>

#include "CMT.h"




using namespace std;
using namespace cv;

extern "C" {


JNIEXPORT void JNICALL Java_ph_edu_dlsu_cmtakaze_CameraActivity_FindFeatures(JNIEnv*, jobject, jlong addrGray, jlong addrRgba);
JNIEXPORT void JNICALL Java_ph_edu_dlsu_cmtakaze_CameraActivity_OpenCMT(JNIEnv*, jobject, jlong addrGray, jlong addrRgba,jlong x, jlong y, jlong width, jlong height);
JNIEXPORT void JNICALL Java_ph_edu_dlsu_cmtakaze_CameraActivity_ProcessCMT(JNIEnv *env, jobject instance, jlong addrGray, jlong addrRgba);
JNIEXPORT jintArray JNICALL Java_ph_edu_dlsu_cmtakaze_CameraActivity_CMTgetRect(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL Java_ph_edu_dlsu_cmtakaze_CameraActivity_CMTSave(JNIEnv *env, jobject instance, jstring path);


// Global variables

bool CMTinitiated=false;
CMT  *cmt = new CMT();
long rect[4];



JNIEXPORT void JNICALL Java_ph_edu_dlsu_cmtakaze_CameraActivity_FindFeatures(JNIEnv*, jobject, jlong addrGray, jlong addrRgba)
{
    Mat& mGr  = *(Mat*)addrGray;
    Mat& mRgb = *(Mat*)addrRgba;
    vector<KeyPoint> v;

    Ptr<FeatureDetector> detector = FastFeatureDetector::create(50);
    detector->detect(mGr, v);
    for( unsigned int i = 0; i < v.size(); i++ )
    {
        const KeyPoint& kp = v[i];
        circle(mRgb, Point(kp.pt.x, kp.pt.y), 10, Scalar(255,0,0,255));
    }
}



JNIEXPORT void JNICALL
Java_ph_edu_dlsu_cmtakaze_CameraActivity_OpenCMT(JNIEnv*, jobject, jlong addrGray, jlong addrRgba,jlong x, jlong y, jlong width, jlong height) {

    Mat& im_gray  = *(Mat*)addrGray;
    Point p1(x,y);
    Point p2(x+width,y+height);

    CMTinitiated=false;
    if (cmt->initialise(im_gray, p1, p2))
        CMTinitiated=true;

}



JNIEXPORT void JNICALL
Java_ph_edu_dlsu_cmtakaze_CameraActivity_ProcessCMT(JNIEnv *env, jobject instance, jlong addrGray, jlong addrRgba) {

    if (!CMTinitiated)
        return;
    Mat& img  = *(Mat*)addrRgba;
    Mat& im_gray  = *(Mat*)addrGray;

    cmt->processFrame(im_gray);

    //for(int i = 0; i<cmt->trackedKeypoints.size(); i++)
             //cv::circle(img, cmt->trackedKeypoints[i].first.pt, 3, cv::Scalar(255,255,255));

    cv::line(img, cmt->topLeft, cmt->topRight, cv::Scalar(255,255,255));
    cv::line(img, cmt->topRight, cmt->bottomRight, cv::Scalar(255,255,255));
    cv::line(img, cmt->bottomRight, cmt->bottomLeft, cv::Scalar(255,255,255));
    cv::line(img, cmt->bottomLeft, cmt->topLeft, cv::Scalar(255,255,255));


}



JNIEXPORT jintArray JNICALL
Java_ph_edu_dlsu_cmtakaze_CameraActivity_CMTgetRect(JNIEnv *env, jclass type) {

    if (!CMTinitiated)
        return NULL;

    jintArray result;
    result = env->NewIntArray(8);



    jint fill[8];

    {

        fill[0]=cmt->topLeft.x;
        fill[1]=cmt->topLeft.y;
        fill[2]=cmt->topRight.x;
        fill[3]=cmt->topRight.y;
        fill[4]=cmt->bottomLeft.x;
        fill[5]=cmt->bottomLeft.y;
        fill[6]=cmt->bottomRight.x;
        fill[7]=cmt->bottomRight.y;
        env->SetIntArrayRegion(result, 0, 8, fill);
        return result;
    }

    return NULL;

}




JNIEXPORT void JNICALL
Java_ph_edu_dlsu_cmtakaze_CameraActivity_CMTSave(JNIEnv *env, jobject instance, jstring path) {

    if (!CMTinitiated)
        return ;

    const char *str = env->GetStringUTFChars(path, 0);

    cmt->Save(str);

    env->ReleaseStringUTFChars( path, str);

}



JNIEXPORT void JNICALL
Java_ph_edu_dlsu_cmtakaze_CameraActivity_CMTLoad(JNIEnv *env, jobject instance, jstring path) {

    const char *str = env->GetStringUTFChars(path, 0);

    cmt=new CMT();

    cmt->Load(str);

    env->ReleaseStringUTFChars( path, str);

    CMTinitiated=true;

}


}
