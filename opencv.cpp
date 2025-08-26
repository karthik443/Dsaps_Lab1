#include <opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;

int main() {
    Mat img = Mat::zeros(400, 400, CV_8UC3);
    putText(img, "Karthik!", Point(50,200), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,255,0), 2);
    imshow("Hello Karthik", img);
    waitKey(0);
    return 0;
}
