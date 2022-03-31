#include<iostream>
#include<fstream>
#include<set>
#include<map>
#include<vector>
#include <sys/time.h>
using namespace std;

int m;
vector<pair<string, bool>> implicant[27][27];
vector<string> prime_implicant;
set<pair<int, int>> cnt_set;
map<int, set<string>> maxterm;
map<string, set<int>> P;
vector<string> ans;

double GetTime(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + 1e-6 * tv.tv_usec;
}

void append(string s, int cnt, int n){
    for(int i=n;i<m;i++){
        if(s[i]=='1'){
            cnt++;
        }
        else if(s[i]=='-'){
            s[i] = '0';
            append(s, cnt, i);
            s[i] = '1';
            append(s, cnt, i);
            return;
        }
    }
    implicant[0][cnt].push_back(pair<string, bool>(s, false));
}

void getSOP(string &s, int cnt, int val){
    for(int i=cnt;i<m;i++){
        val <<= 1;
        if(s[i]=='1'){
            val+=1;
        }
        else if(s[i]=='-'){
            getSOP(s, i+1, val);
            getSOP(s, i+1, val+1);
            return;
        }
    }
    auto it = maxterm.find(val);
    if(it!=maxterm.end()){
        (it->second).insert(s);
        P[s].insert(val);
    }
}

int main(int argc, char* argv[]){
    //double t = GetTime();
    string line;
    ifstream fptr(argv[1]);
    int _;
    fptr>>m;
    fptr>>_;
    getline(fptr, line);
    //get all minterms
    while(getline(fptr, line)){
        if(line.size())
            append(line, 0, 0);
    }
    //delete repeat 
    for(int j=0;j<m;j++){
        std::sort(implicant[0][j].begin(), implicant[0][j].end());
        auto it = std::unique(implicant[0][j].begin(), implicant[0][j].end());
        implicant[0][j].resize(distance(implicant[0][j].begin(),it));
    }

    //primary implicant generation 
    for(int i=0;i<=m;i++){
        //get column i
        for(int j=0;j<m;j++){
            for(auto str1=implicant[i][j].begin();str1!=implicant[i][j].end();str1++){
                for(auto str2=implicant[i][j+1].begin();str2!=implicant[i][j+1].end();str2++){
                    int diff_idx = -1;
                    int diff_cnt = 0;
                    for(int n=0;n<=m;n++){
                        if((*str1).first[n]!=(*str2).first[n]){
                            diff_cnt++;
                            if(diff_cnt>1)
                                break;
                            diff_idx = n;
                        }
                    }
                    if(diff_cnt==1){
                        string tmp = (*str1).first;
                        tmp[diff_idx] = '-';
                        implicant[i+1][j].push_back(pair<string ,bool>(tmp, false));
                        (*str1).second = true; 
                        (*str2).second = true; 
                    }
                }
            }
            //get prime implicant
            for(auto p:implicant[i][j]){
                if(p.second==false){
                    prime_implicant.push_back(p.first);
                }
            }
            //delete repeat
            std::sort(implicant[i+1][j].begin(), implicant[i+1][j].end());
            auto it = std::unique(implicant[i+1][j].begin(), implicant[i+1][j].end());
            implicant[i+1][j].resize(distance(implicant[i+1][j].begin(),it));
            if(i!=0)
                implicant[i][j].clear();
        }
    }

    //create table
    for(int j=0;j<=m;j++){
        for(auto it=implicant[0][j].begin(); it!=implicant[0][j].end();it++){
            maxterm[stoi((*it).first, 0, 2)] = set<string>();
        }
    }

    for(auto str=prime_implicant.begin();str!=prime_implicant.end();str++){
        string tmp = *str;
        getSOP(tmp, 0, 0);
    }

    for(auto i:maxterm){
        cnt_set.insert(pair<int, int>(i.second.size(), i.first));
    }

    for(auto p:cnt_set){
        if(maxterm.size()==0)
            break;
        auto it = maxterm.find(p.second);
        if(it != maxterm.end()){
            set<string> tmp = it->second;
            int max = 0;
            string max_str = *tmp.begin();
            for(auto str:tmp){
                if(P[str].size()>max){
                    max = P[str].size();
                    max_str = str;
                }
            }
            for(auto col:P[max_str]){
                for(auto str:maxterm[col]){
                    if(str!=max_str)
                        P[str].erase(col);
                }
                maxterm.erase(col);
            }
            ans.push_back(max_str);
        }
    }

    int num_literals = 0;
    for(auto str:ans){
        for(auto c:str){
            if(c=='1'||c=='0')
                num_literals++;
        }
    }
    ofstream fhdr;
    fhdr.open(argv[2]);
    fhdr<<num_literals<<"\n";
    fhdr<<ans.size()<<"\n";
    for(auto str:ans){
        fhdr<<str<<"\n";
    }
    fhdr.close();
    //cout<<"Time:"<<GetTime()-t<<"\n";
    return 0;
}
