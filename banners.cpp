#include<iostream>
#include <climits>
#include <cmath>
using namespace std;
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
void printLeafs(int idx,int l , int r, int * &segtree,int * &lazybucket,int &prevHeight,int offset){

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
        cout<<l+offset<<" "<<segtree[idx]<<endl;
        prevHeight = segtree[idx];
        }else{
        }
        return ;
    }
    int mid = (l+r)/2;
    printLeafs(lidx,l,mid,segtree,lazybucket,prevHeight,offset);
    printLeafs(ridx,mid+1,r,segtree,lazybucket,prevHeight,offset);
   
}
int main(){

    int  q;
    cin>>q;
    Query * inputs[q];
    int minIdx = INT_MAX,maxIdx=INT_MIN;
    for(int i=0;i<q;i++){
        int type;
        cin>>type;
        Query *newQuery;
        if(type==0){
            int st,end,height;
            cin>>st>>end>>height;
            minIdx = min(minIdx,st);
            maxIdx = max(maxIdx,end);
            newQuery = new Query(st,end,height);
        }else{
            newQuery = new Query();
        }
        inputs[i] = newQuery;
    }
    int offset = minIdx;
    int st = 0;
    int end = maxIdx-minIdx;
    int limit = end;
    limit = min(limit,(int)pow(10,5)+1);
    int * lazybucket = new int[4*limit];
    int * segTree = new int[4*limit];
    int prevHeight = -1;
    
    for(auto input:inputs){
        if(input->type==0){
            lazyupdate(0,st,end,input->st-offset,input->end-offset-1,segTree,input->height,lazybucket);
        }else{
            prevHeight=-1;
            printLeafs(0,st,end,segTree,lazybucket,prevHeight,offset);
            cout<<endl;
        }
    }
}