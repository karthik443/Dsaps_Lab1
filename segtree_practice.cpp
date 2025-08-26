#include<iostream>
#include<vector>
#include <climits>
using namespace std;

void lazyupdate(int idx,int l, int r,int searchl, int searchr,vector<int>&segtree,int val,vector<int>&lazybucket){
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
    segtree[idx] =max( segtree[lidx],segtree[ridx]);
}
int lazyQuery(int idx,int l, int r,int searchl, int searchr,vector<int>&segtree,vector<int>&lazybucket){
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
    if(searchl>r || searchr<l) return INT_MIN;
    if(searchl<=l && searchr>=r){
        return segtree[idx];
    }
    int mid = (l+r)/2;
    int left = lazyQuery(lidx,l,mid,searchl,searchr,segtree,lazybucket);
    int right = lazyQuery(ridx,mid+1,r,searchl,searchr,segtree,lazybucket);
    return max(left,right);
}
void buildTree(int idx,int l, int r,vector<int>&tree,vector<int>&input){
    if(l==r){
        tree[idx] = input[l];
        return ;
    }
    int lidx = 2*idx+1;
    int ridx= 2*idx+2;

    int mid =( l+r)/2;
    buildTree(lidx,l,mid,tree,input);
    buildTree(ridx,mid+1,r,tree,input);
    tree[idx] = max(tree[lidx],tree[ridx]);
}
int searchTree(int idx,int l, int r,int searchl,int searchr,vector<int>&tree,vector<int>&input){

    if(searchr<l || searchl>r){
        return INT_MIN;
    }
    else if(searchl<=l && searchr>=r){
        return tree[idx];
    }
    else{
        int mid = (l+r)/2;
        int lidx = 2*idx+1;
        int ridx = 2*idx+2;
        int a = searchTree(lidx,l,mid,searchl,searchr,tree,input);
        int b = searchTree(ridx,mid+1,r,searchl,searchr,tree,input);
        return max(a,b);
    }
    

}
void pointUpdate(int idx, int l , int r, int val , int pointIdx,vector<int>&segTree){
    if(l==r && l==pointIdx){
        segTree[idx] = val;
        return ;
    }
    int mid= (l+r)/2;
    int lidx = 2*idx+1;
    int ridx = 2*idx+2;
    if(pointIdx<=mid){
        pointUpdate(lidx,l,mid,val,pointIdx,segTree);
    }else{
        pointUpdate(ridx,mid+1,r,val,pointIdx,segTree);
    }
    segTree[idx] = max(segTree[lidx],segTree[ridx]);
}
int main(){
    vector<int>arr = {8,2,5,1,4,5,3,9,6,10};
    vector<int>segTree(4*arr.size());
    buildTree(0,0,arr.size()-1,segTree,arr);
   cout<< searchTree(0,0,arr.size()-1,0,arr.size()-1,segTree,arr);
}