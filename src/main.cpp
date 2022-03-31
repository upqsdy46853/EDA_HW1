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

double GetTime(void) // get the current time
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
    implicant[0][cnt].push_back(pair<string, int>(s, false));
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
    for(int j=0;j<m;j++){
        std::sort(implicant[0][j].begin(), implicant[0][j].end());
        auto it = std::unique(implicant[0][j].begin(), implicant[0][j].end());
        implicant[0][j].resize(distance(implicant[0][j].begin(),it));
    }

    //primary implicant generation 
    double t = GetTime();
    for(int i=0;i<=m;i++){
        //get column i
        //set<string> merged;
        for(int j=0;j<m;j++){
            //set<string> next_merged;
            for(auto str1=implicant[i][j].begin();str1!=implicant[i][j].end();str1++){
                //bool merge = false;
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
                        //next_merged.insert(*str2);
                        //merge = true;
                        (*str1).second = true; 
                        (*str2).second = true; 
                    }
                }
                //get prime implicant
                //if(!merge && merged.find(*str1)==merged.end()){
                //    prime_implicant.push_back(*str1);
                //}
            }
            for(auto p:implicant[i][j]){
                if(p.second==false){
                    prime_implicant.push_back(p.first);
                }
            }
            std::sort(implicant[i+1][j].begin(), implicant[i+1][j].end());
            auto it = std::unique(implicant[i+1][j].begin(), implicant[i+1][j].end());
            implicant[i+1][j].resize(distance(implicant[i+1][j].begin(),it));
            //std::unique(implicant[i+1][j].begin(), implicant[i+1][j].end());

            //merged = next_merged;
            if(i!=0)
                implicant[i][j].clear();
        }
    }
    cout<<"Time:"<<GetTime()-t<<endl;
    //test prime
    //for(auto i:prime_implicant){
    //    cout<<i<<endl;
    //}
    cout<<prime_implicant.size()<<endl;

    //create table
    for(int j=0;j<=m;j++){
        for(auto it=implicant[0][j].begin(); it!=implicant[0][j].end();it++){
            maxterm[stoi((*it).first, 0, 2)] = set<string>();
            //cout<<*it<<":"<<stoi(*it, 0, 2)<<endl;
        }
    }

    for(auto str=prime_implicant.begin();str!=prime_implicant.end();str++){
        string tmp = *str;
        getSOP(tmp, 0, 0);
    }

    for(auto i:maxterm){
        cnt_set.insert(pair<int, int>(i.second.size(), i.first));
    }

    //test cnt_set
    //for(auto i:cnt_set){
    //    cout<<i.first<<","<<i.second<<endl;
    //}

    //test maxterm
    //for(auto i:maxterm){
    //    cout<<i.first<<":";
    //    cout<<"{";
    //    for(auto j:i.second)
    //        cout<<j<<",";
    //    cout<<"}\n";
    //}

    //test P
    //for(auto i:P){
    //    cout<<i.first<<":";
    //    cout<<"{";
    //    for(auto j:i.second)
    //        cout<<j<<",";
    //    cout<<"}\n";
    //}


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
            //string str = *(tmp.begin());
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
    fhdr<<num_literals<<endl;
    fhdr<<ans.size()<<endl;
    for(auto str:ans){
        fhdr<<str<<endl;
    }
    fhdr.close();
    //output
    //int num_literals = 0;
    //for(auto str=prime_implicant.begin();str!=prime_implicant.end();str++){
    //    for(int i=0;i<(*str).size();i++){
    //        if((*str)[i]=='1'||(*str)[i]=='0')
    //        num_literals++;
    //    }
    //}
    //ofstream fhdr;
    //fhdr.open(argv[2]);
    //fhdr<<num_literals<<endl;
    //fhdr<<prime_implicant.size()<<endl;
    //for(auto i=prime_implicant.begin();i!=prime_implicant.end();i++){
    //    fhdr<<*i<<endl;
    //}
    //fhdr.close();
    return 0;
}
