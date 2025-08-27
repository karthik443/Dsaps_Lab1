#include<iostream>
#include <climits>
#include <cmath>
using namespace std;

//------------------------------------------------>stored every query in below structure 
struct Query{
    int type;
    int st;
    int end;
    int height;
    Query(){
        type=1;
        st=0,end=0,height=0;
    }
    Query(int _st,int _end,int _height){
        type=0;
        st=_st;
        end = _end;
        height = _height;
    }
};
//------------------------------------------------>binary search used for segment tree compression
int binarySearch(int l , int r,int val,int *arr){
    int m = l+(r-l)/2;
    if(arr[m]==val)return m;
    if(val>arr[m]){
        return binarySearch(m+1,r,val,arr);
    }else{
        return binarySearch(l,m-1,val,arr);
    }
}
//------------------------------------------------->code for lazy update in seg tree
void lazyupdate(int idx,int l, int r,int searchl, int searchr,int * &segtree,int val, int * &lazybucket){
    int lidx = 2*idx+1;
    int ridx = 2*idx+2;
    if(lazybucket[idx]!=0){
        segtree[idx] = lazybucket[idx];
        if(l!=r){
            lazybucket[lidx] = lazybucket[idx];
            lazybucket[ridx] = lazybucket[idx];
        }
        lazybucket[idx]=0;
    }
    if(searchl>r || searchr<l) return ;
    if(searchl<=l && searchr>=r){
        if(l!=r){
            lazybucket[lidx] = val;
            lazybucket[ridx] = val;
        }
        segtree[idx] = val;
        return;
    }
    int mid = (l+r)/2;
    lazyupdate(lidx,l,mid,searchl,searchr,segtree,val,lazybucket);
    lazyupdate(ridx,mid+1,r,searchl,searchr,segtree,val,lazybucket);
     segtree[idx] = max(segtree[lidx], segtree[ridx]);
}
//---------------------------------------------------> traverse entire tree leafs to search for change in height difference
void printLeafs(int idx,int l , int r, int * &segtree,int * &lazybucket,int &prevHeight,int * points){

    int lidx = 2*idx+1;
    int ridx = 2*idx+2;
    if(lazybucket[idx]!=0){
        segtree[idx] = lazybucket[idx];
        if(l!=r){
            lazybucket[lidx] = lazybucket[idx];
            lazybucket[ridx] = lazybucket[idx];
        }
        lazybucket[idx]=0;
    }

    if(l==r){
        if(prevHeight!=segtree[idx]){
        cout<<points[l]<<" "<<segtree[idx]<<endl;
        prevHeight = segtree[idx];
        }else{
        }
        return ;
    }
    int mid = (l+r)/2;
    printLeafs(lidx,l,mid,segtree,lazybucket,prevHeight,points);
    printLeafs(ridx,mid+1,r,segtree,lazybucket,prevHeight,points);
   
}
//------------------------------------------------------> sort and remove input point duplicates.
int sortAndUniqueArray(int *&arr,int N){
    for(int i=0;i<N-1;i++){
        for(int j=0;j<N-i-1;j++){
            if(arr[j]>arr[j+1]){
                int temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
    int idx=0;
    for(int i=0;i<N;i++){
        if(i==0 || arr[i]!=arr[i-1]){
            arr[idx++] = arr[i];
        }
    }
    return idx;
}
int main(){
    int  q;
    cin>>q;
    Query * inputs[q];
    int *points = new int[2*q];     //storing all points sent as input
    int pointIdx=0;
    for(int i=0;i<q;i++){
        int type;
        cin>>type;
        Query *newQuery;
        if(type==0){
            int st,end,height;
            cin>>st>>end>>height;
            points[pointIdx++] = st;
            points[pointIdx++] = end;
            newQuery = new Query(st,end,height);
        }else{
            newQuery = new Query();
        }
        inputs[i] = newQuery;
    }
    int size = sortAndUniqueArray(points,2*q);   


    int st = 0;
    int end = size-1;
    int * lazybucket = new int[4*size];
    int * segTree = new int[4*size];
    int prevHeight = 0;


    for(auto input:inputs){
        if(input->type==0){ // type 0 for tree updation
            int startpoint =  binarySearch(0,size-1,input->st,points);
            int endpoint  = binarySearch(0,size-1,input->end,points);
            lazyupdate(0,st,end,startpoint,endpoint-1,segTree,input->height,lazybucket);
        }else{              // type 1 for point printing
            prevHeight=0;
            printLeafs(0,st,end,segTree,lazybucket,prevHeight,points);
        }
    }
}