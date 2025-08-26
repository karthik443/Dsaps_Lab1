#include<iostream>
#include<vector>
#include <cstring>
#include <string>
using namespace std;

struct Node
{
    bool end = false;
    Node * arr[26];
    bool vis = false;
};

class Trie{

    
    public:
    Node *root;
    
    Trie(){
        cout<<"Trie initialized!"<<endl;
        root = new Node();
    }
    void insertWord(string word){
        int idx=0;
        int size = word.size();
        Node * curr = root;
        while(idx!=size){
            
                int charIdx= word[idx]-'a';
                if(curr->arr[charIdx]){
                    curr=curr->arr[charIdx];
                }else{
                    Node *temp = new Node();
                    curr->arr[charIdx] = temp;
                    curr=curr->arr[charIdx];
                }
                idx++;
            
        }
        curr->end = true;

    }
    vector<string> searchWord(string word){
        int idx=0;
        int size = word.size();
        Node * curr = root;
        while(idx!=size){
            
                int charIdx= word[idx]-'a';
                if(curr->arr[charIdx]){
                    curr=curr->arr[charIdx];
                }else{
                    
                   return {"0"};
                }
                idx++;
            
        }
        if(curr->end){
            return {"1"};
        }
        else{
            return {"0"};
        }
    }
    vector<string> getsuggestions(string prefix){
        vector<string>suggestions;
        int idx=0;
        Node * curr = root;
        while(idx<prefix.size()){
            int charIdx= prefix[idx]-'a';
            if(!curr->arr[charIdx]){
                //cout<<"I'm not available";
                return suggestions;
            }
            else{
                curr=curr->arr[charIdx];
            }
            idx++;
        }
        doTraversal(curr,prefix,suggestions);
        for(auto word:suggestions){
            cout<<word<<endl;
        }
        return suggestions;
    }
    void doTraversal(Node *head,string word,vector<string>&suggestions){
        for(int i=0;i<26;i++){
            if(head->arr[i]){
                char ch = 'a'+i;
                doTraversal(head->arr[i],word+ch,suggestions);
            }
            
        }
        if(head->end){
            suggestions.push_back(word);
        }
    }
    void levDistanceHelper(int idx,const char *input,string res,Node * curr,int N,int remaining,vector<string>&nearStrings){
        if(idx>N || remaining<0){ 
            // cout<<idx<<" - "<<remaining<<endl;
            return ;
        }
        if(idx==N && curr->end && !curr->vis){
            curr->vis = true;
            nearStrings.push_back(res);
           // cout<<res<<endl;
        }
        for(int i=0;i<26;i++){
            if(curr->arr[i]){
                char ch = 'a'+i;
                Node *inside = curr->arr[i];
                if(idx<N && input[idx]==ch){
                        levDistanceHelper(idx+1,input,res+ch,inside,N,remaining,nearStrings);
                }
                 //replace
                    levDistanceHelper(idx+1,input,res+ch,inside,N,remaining-1,nearStrings);
                     //insert 
                    levDistanceHelper(idx,input,res+ch,inside,N,remaining-1,nearStrings);  
            }
        }
         //delete
        levDistanceHelper(idx+1,input,res,curr,N,remaining-1,nearStrings);
                   
    }
    void exploreAllStrings(Node * curr){
        curr->vis=false;
        for(int i=0;i<26;i++){
            if(curr->arr[i]){
                exploreAllStrings(curr->arr[i]);
            }
        }
    }
    vector<string> levDistance(int idx,const string input,string res,Node * curr,int remaining){
        
        vector<string>nearStrings;
        // cout<<"hi!"<<endl;
        levDistanceHelper( idx,input.c_str(), res, curr,input.size(), remaining,nearStrings);
        exploreAllStrings(this->root);
        return nearStrings;
        
    }


};



int main(){
    Trie t;
    int n,q;
    cin>>n>>q;
    vector<string>ans;
    for(int i=0;i<n;i++){
        string s;
        cin>>s;
        t.insertWord(s);
    }
    vector<string>temp;
    for(int i=0;i<q;i++){
        int flag;
        string word;
        cin>>flag;
        cin>>word;
        if(flag==1){
            temp = t.searchWord(word);
        }else if(flag==2){
            
            temp = t.getsuggestions(word);
            ans.push_back(to_string(temp.size()));
        }else if(flag==3){
            
            temp = t.levDistance(0,word,"",t.root,3);
            ans.push_back(to_string(temp.size()));
        }
        ans.insert(ans.end(),temp.begin(),temp.end());
    }
    for(string word:ans){
        cout<<word<<endl;
    }
    
    
}




//   t.insertWord("consider");
   
//     t.insertWord("entitled");
    
//     // t.insertWord("litter");
//     t.insertWord("dames");
//     t.insertWord("camel");
//     t.insertWord("filling");
//     t.insertWord("grasses");
//     t.insertWord("fitter");
//     t.insertWord("filters");
//     t.insertWord("tilers");
//     t.insertWord("filers");
//     // cout<<t.searchWord("tilers")<<endl;
//     // cout<<t.searchWord("dame")<<endl;

//     // t.gesuggestions("con");
//     t.levDistance(0,"filter","",t.root,3);
//      t.levDistance(0,"dam","",t.root,3);