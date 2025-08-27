#include <iostream>
#include <cmath>
#include <cstring>
#include <limits>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
// to find seam with low energy cells
int * findingSeam(Mat & energy ){
    int r = energy.rows;
    int c = energy.cols;
    
    float** energyDp = new float*[r];
    int** parent = new int*[r];
    
    for (int i = 0; i < r; i++) {
        parent[i] = new int[c];
        energyDp[i] = new float[c];
    }
    for(int j=0;j<c;j++){
       energyDp[0][j] = energy.at<float>(0,j);

    }
    //-----------------------------------------------------Iterative dp to find the lowest energy path START

    for(int i=1;i<r;i++){
        for(int j=0;j<c;j++){

            float bestVal = energyDp[i-1][j];
            int bestIdx=j;

            if(j!=0 && energyDp[i-1][j-1]<bestVal){
                bestVal = energyDp[i-1][j-1];
               bestIdx = j-1;
            }

            if(j!=c-1 && energyDp[i-1][j+1]<bestVal){
                bestVal = energyDp[i-1][j+1];
                bestIdx = j+1;
            }

            parent[i][j] = bestIdx;
            energyDp[i][j] = energy.at<float>(i,j)+bestVal;
        }
    }
    //---------------------------------------------------energy dp is calculated. END

    //-------------------------------------------------checking the lowest energy cell in last row; 

    int minval=energyDp[r-1][0];
    int minJ=0;
    for(int j=1;j<c;j++){
        if(minval>energyDp[r-1][j]){
            minval = energyDp[r-1][j];
            minJ = j;
        }
    }
    //-------------------------------------------------backtrack for finding the seam.
    int *seam = new int[r];
    for(int i=r-1;i>=0;i--){
        seam[i] = minJ;
        minJ = parent[i][minJ];
    }
    //--------------------------------------------------free the memory allocated
    for (int i=0;i<r;i++) {
        delete[] energyDp[i];
        delete[] parent[i];
    }
    delete[] energyDp;
    delete[] parent;
    return seam;

}
//----------------------------------------------------- create and return a new matrix by removing lowest energy seam
Mat removeSeam(Mat & Image, int * seam){
    int rows = Image.rows;
    int cols = Image.cols;
    Mat newImageMat(rows,cols-1,Image.type());
    for(int i=0;i<rows;i++){
        int newJ=0;
        int seamCol = seam[i];
        for(int j=0;j<cols;j++){
            if(j==seamCol)continue;
            newImageMat.at<Vec3b>(i,newJ) = Image.at<Vec3b>(i,j);
            newJ++;
        }
    }
    return newImageMat;
}
//------------------------------------------------------------ finding the energy matrix from an image.
Mat computeEnergy(const cv::Mat &img) {
    if(img.empty()){
        return Mat();
    }
    Mat grayMatrix;
    cvtColor(img, grayMatrix, cv::COLOR_BGR2GRAY);
    Mat grayFloat;
    grayMatrix.convertTo(grayFloat, CV_32F);
    Mat gradX, gradY;
    Sobel(grayFloat, gradY, CV_32F, 0, 1, 3); 
    Sobel(grayFloat, gradX, CV_32F, 1, 0, 3); 
    Mat energy;
    magnitude(gradX, gradY, energy);
    return energy; 
}



int main(int argc, char **argv) {
    try{

    string inputfile;
    cin>>inputfile;
    Mat Image = imread(inputfile.c_str(),IMREAD_COLOR);
    int targetW;
    int targetH;
    cout<<"Image width and height are : "<<Image.cols<<" - "<<Image.rows<<endl;
    cout<<"Enter target width and target height->"<<endl;
    cin>>targetW;
    cin>>targetH;
    
    cout<<"Working on the image , kindly wait"<<endl;
    if(Image.empty()){
        throw runtime_error("Unable to open file");
    }
    if(targetW<=0 || targetH<=0 || targetH>Image.rows || targetW>Image.cols){
        throw runtime_error(" Invalid Height/Width passed");
       
    }
    while(Image.cols>targetW || Image.rows>targetH){
        int * seam;
        if(Image.cols>targetW){
            Mat energy = computeEnergy(Image);
            seam = findingSeam(energy);
            Image = removeSeam(Image,seam);
        }
        if(Image.rows>targetH){
            transpose(Image,Image);
            Mat energy = computeEnergy(Image);
            seam = findingSeam(energy);
            Image = removeSeam(Image,seam);
            transpose(Image,Image);
        }
        delete[] seam;  
    
    }
    // shrink Height
    
    cv::imwrite("resized_output.jpg", Image);
    //waitKey(0);
    cout<<"Output Generated as resized_output.jpg"<<endl;
}
catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}