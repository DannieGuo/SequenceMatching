/*
    author：郭丹琪 2018202067
*/
#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <time.h>
#include <math.h>
using namespace std;

//长串切割长度
#define CUT_LEN 1000000

//短串长度
#define SUB_LEN 200

//SA间隔
#define SA_CKPT_LEN 100

//count_num 间隔
#define COUNT_CKPT_LEN 100

//非精确匹配部分允许缺失的K值
#define K 3


int len = CUT_LEN + SUB_LEN + 1;
int num_a = 0, num_c = 0, num_g = 0, num_t = 0; //各字符出现次数
vector<vector<int>>count_num(4, vector<int>(CUT_LEN + SUB_LEN + 1, 0)); //索引
vector<int> SA(CUT_LEN + SUB_LEN + 2, 0);   //后缀数组
vector<int> Rank(CUT_LEN + SUB_LEN + 2, 0); //名次数组
vector<int> Second(CUT_LEN + SUB_LEN + 2, 0);   //记录对第二关键字排的序
vector<int> tmp(CUT_LEN + SUB_LEN + 2, 0);  //计算Rank时的临时存储
vector<int> cnt(CUT_LEN + SUB_LEN + 2, 0);  
string str; //编码部分的原串
string L; //匹配时读入的L

//倍增算法计算SA
void Suffix(){
    //初始化
    str = "0" + str;
    for(int i = 1; i <= len; i++){
        Rank[i] = str[i];
        SA[i] = i;
    }

    //排序
    for(int l = 0 , pos = 0 , sig = 255; pos < len; sig = pos) {
        //第二关键字排序
        pos = 0;
        for(int i = len - l + 1; i <= len; i++)
            Second[++pos] = i;

        for(int i = 1; i <= len; i++)
            if(SA[i] > l) Second[ ++pos ] = SA[i] - l;
          
        //第一关键字排序
        cnt.assign(sig + 2, 0);
        for(int i = 1; i <= len; i++)
            ++cnt[Rank[i]];
        for(int i = 1; i <= sig; i++)
            cnt[i] += cnt[i - 1];

        //计算SA
        for(int i = len; i > 0; i--)
            SA[cnt[Rank[Second[i]]]--] = Second[i];

        //更新Rank
        pos = 0;
        for(int i = 1; i <= len; i++)
            tmp[SA[i]] = (Rank[SA[i]] == Rank[SA[i - 1]] && Rank[SA[i] + l] == Rank[SA[i - 1] + l])? 
                            pos : ++pos;
        for(int i = 1; i <= len; i++)
            Rank[i] = tmp[i];
        l = !l? 1 : l << 1; 
    }
    return;
}

//BWT编码及索引计算
string Encode() {
    str = str + '$';
    len = str.size();

    Suffix();

    num_a = 0, num_c = 0, num_g = 0, num_t = 0;
    for(int i = 0; i < 4; i++)
        count_num[i].assign(len, 0);

    //由后缀数组得到L并计算索引
    string L(len, ' ');
    for(int i = 0; i < len; i++) {
        if(SA[i + 1] == 1) {
            L[i] = '$';
        }
        else {
            L[i] = str[SA[i + 1] - 1];
            count_num[0][i] = num_a;
            count_num[1][i] = num_c;
            count_num[2][i] = num_g;
            count_num[3][i] = num_t;

            if(L[i] == 'A') num_a++;
            else if(L[i] == 'C') num_c++;
            else if(L[i] == 'G') num_g++;
            else if(L[i] == 'T') num_t++;
        }
    }
    return L;
}

//计算对应索引
int Count_Num(char target, int now_i) {
    int char_num = -1;
    if(target == 'A')
        char_num = 0;
    else if(target == 'C')
        char_num = 1;
    else if(target == 'G')
        char_num = 2;
    else if(target == 'T')
        char_num = 3;
    
    int pre_i = now_i / COUNT_CKPT_LEN;
    int count = count_num[char_num][pre_i]; //前面最近的checkpoint位置上的值
    pre_i = pre_i * COUNT_CKPT_LEN; //前面最近的checkpoint位置
    for(int i = pre_i; i < now_i; i++) { //从checkpoint到该位前的对应字符计数
        if(L[i] == target)
            count++;        
    }
    return count;
}

//FM字符串匹配
vector<int> FM(string &sub, int file_num) {
    int begin_a = 1, begin_c = num_a + 1, begin_g = 1 + num_a + num_c, begin_t = 1 + num_a + num_c + num_g;
    int begin = 0, end = L.size() - 1;

    //最后一个字符
    char now = sub[sub.size() - 1];
    if(now == 'A') {
        begin = begin_a;
        end = begin_c - 1;
    } else if(now == 'C') {
        begin = begin_c;
        end = begin_g - 1; 
    } else if(now == 'G') {
        begin = begin_g;
        end = begin_t - 1;
    } else if(now == 'T') {
        begin = begin_t;
        end = L.size() - 1;
    }

    //从后往前匹配
    for(int i = sub.size() - 2; i >= 0; i--) {
        now = sub[i]; 
        int lbegin;
        if((lbegin = L.find(now, begin)) == string::npos)
            return vector<int>(0);
        if(lbegin > end)
            return vector<int>(0);
        int lend = L.rfind(now, end);

        //映射到F中
        if(now == 'A') {
            begin = begin_a + Count_Num(now, lbegin);
            end = begin_a + Count_Num(now, lend);
        } else if(now == 'C') { 
            begin = begin_c + Count_Num(now, lbegin);
            end = begin_c + Count_Num(now, lend);
        } else if(now == 'G') { 
            begin = begin_g + Count_Num(now, lbegin);
            end = begin_g + Count_Num(now, lend);
        } else if(now == 'T') { 
            begin = begin_t + Count_Num(now, lbegin);
            end = begin_t + Count_Num(now, lend);
        }
    }

    //由后缀数组SA计算匹配位置在S中的下标
    vector<int>ans;
    for(int i = begin; i <= end; i++) {
        if(i + 1 == 1 || ((i + 1) % SA_CKPT_LEN == 0)) { //要找的刚好是有记录的checkpoint上的值
            ans.push_back(SA[(i + 1) / SA_CKPT_LEN + 1] - 1 + file_num * CUT_LEN);
        } else {
            int pre = 0; //记录往前查找的次数
            int now_i = i;
            //利用 F 和 L 的关系，一直往前找，直到找到有记录的checkpoint的位置
            while(now_i + 1 != 1 && ((now_i + 1) % SA_CKPT_LEN != 0)) {
                char temp = L[now_i];
                if(temp == 'A')
                    now_i = begin_a + Count_Num(temp, now_i);
                else if(temp == 'C')
                    now_i = begin_c + Count_Num(temp, now_i);
                else if(temp == 'G')
                    now_i = begin_g + Count_Num(temp, now_i);
                else if(temp == 'T')
                    now_i = begin_t + Count_Num(temp, now_i);
                else if(temp == '$') {
                    now_i = -1;
                    break;
                }
                pre++;
            }
            if(now_i != -1)
                ans.push_back(SA[(now_i + 1) / SA_CKPT_LEN + 1] - 1 + pre + file_num * CUT_LEN);
            else
                ans.push_back(0 + pre);
        }
    }
    sort(ans.begin(), ans.end());
    return ans;
}

//字符串匹配
//对每个短串,遍历所有长串文件
vector<int> Find(string &sub, string long_file_name) {
    int file_num = (long_file_name == "SRR00001_rows_ATCG")? 130 : 939;
    string file_path = "long/" + long_file_name + "_cut_encode/" + long_file_name + "_cut_";
    
    vector<int>ans; 
    for(int i = 0; i < file_num; i++) { //遍历长串分治出的所有子串
        //子串的L及索引读入
        string file_name = file_path + to_string(i) + ".txt";
        ifstream Lfile(file_name);
        Lfile >> L;
        Lfile.close();

        ifstream index_file(file_path + "index_" + to_string(i) + ".txt");
        for(int i = 1; i <= ((L.size()) / SA_CKPT_LEN) + 1; i++)
            index_file >> SA[i];

        for(int i = 0; i < 4; i++)
            for(int j = 0; j <= (L.size() / COUNT_CKPT_LEN); j++)
                index_file >> count_num[i][j];

        index_file >> num_a >> num_c >> num_g >> num_t;
        index_file.close();

        //匹配
        vector<int>sub_ans = FM(sub, i);

        if(ans.size() > 0 && sub_ans.size() > 0 && ans[ans.size() - 1] == sub_ans[0])
            ans.pop_back();
        ans.insert(ans.end(), sub_ans.begin(), sub_ans.end());
    }
    return ans;
}

//字符串匹配
//对每个长串文件,遍历所有短串
vector<vector<int>> Find_All_Short(string long_file_name) {
    int file_num = (long_file_name == "SRR00001_rows_ATCG")? 130 : 939;
    string file_path = "long/" + long_file_name + "_cut_encode/" + long_file_name + "_cut_";
    
    vector<vector<int>>ans; 
    for(int i = 0; i < file_num; i++) {
        //读长串文件
        string file_name = file_path + to_string(i) + ".txt";
        ifstream Lfile(file_name);
        Lfile >> L;
        Lfile.close();

        ifstream index_file(file_path + "index_" + to_string(i) + ".txt");
        for(int i = 1; i <= ((L.size()) / SA_CKPT_LEN) + 1; i++)
            index_file >> SA[i];

        for(int i = 0; i < 4; i++)
            for(int j = 0; j <= (L.size() / COUNT_CKPT_LEN); j++)
                index_file >> count_num[i][j];

        index_file >> num_a >> num_c >> num_g >> num_t;
        index_file.close();

        //读入各短串
        //ifstream short_file("short/search_SRR00001.txt");
        ifstream short_file("short/search_SRR163132.txt");
        string short_string;
        int count = 0; //第count个子串
        while(short_file >> short_string) {
            //匹配
            vector<int>sub_ans = FM(short_string, i); 
            if(ans.size() > count && ans[count].size() > 0 && sub_ans.size() > 0 && ans[count][ans[count].size() - 1] == sub_ans[0])
                ans[count].pop_back();
            if(i > 0)
                ans[count].insert(ans[count].end(), sub_ans.begin(), sub_ans.end());
            else
                ans.push_back(sub_ans);
            count++;
        }
    }
    return ans;
}

//长串预处理
void Cut_And_Encode(string file_name) {
    string file_path = "long/" + file_name + ".txt";
    ifstream file(file_path);
    string long_string;
    string temp;
    file >> temp;
    file >> long_string;
    
    for(int i = 0; i <= ceil(long_string.size() / CUT_LEN); i++) {
        str = long_string.substr(i * CUT_LEN, CUT_LEN + SUB_LEN); //切割
        string cut_file_path = "long/" + file_name + "_cut_encode/" + file_name + "_cut_";

        //计算对应子串的L和索引,并存储
        ofstream cut_file(cut_file_path + to_string(i) + ".txt");
        string L = Encode();
        cut_file << L;
        cut_file.close();

        ofstream index_file(cut_file_path + "index_" + to_string(i) + ".txt");
        index_file << SA[1] <<" ";

        for(int i = SA_CKPT_LEN; i <= len; i += SA_CKPT_LEN)
                index_file << SA[i] <<" ";

        for(int i = 0; i < 4; i++)
            for(int j = 0; j < len; j += COUNT_CKPT_LEN)
                index_file << count_num[i][j] <<" ";

        index_file << num_a <<" "<< num_c <<" "<< num_g <<" "<< num_t;
        index_file.close();
    } 
}

//缺失K个字符的匹配
vector<int> FM_MissingK(string &sub, int file_num) {
    int begin_a = 1, begin_c = num_a + 1, begin_g = 1 + num_a + num_c, begin_t = 1 + num_a + num_c + num_g; 
    char now = sub[sub.size() - 1];

    //匹配记录初始化
    vector<pair<int, int>>temp_find(L.size(), pair<int, int>(0, 1));
    for(int i = 0; i < L.size(); i++) {
        temp_find[i].first = i;
        if(now == 'A') {
            if(i >= begin_a && i < begin_c)
                temp_find[i].second = 0;
        } else if(now == 'C') {
            if(i >= begin_c && i < begin_g)
                temp_find[i].second = 0;
        } else if(now == 'G') {
            if(i >= begin_g && i < begin_t)
                temp_find[i].second = 0;
        } else if(now == 'T') {
            if(i >= begin_t)
                temp_find[i].second = 0;
        }
    }

    //遍历短串的每一位进行匹配
    for(int i = sub.size() - 2; i >= 0; i--) {
        now = sub[i]; 
        int j = 0;
        while(j < temp_find.size()) {
            char temp_now = L[temp_find[j].first];
            if(temp_now != now) //该位缺失匹配
                temp_find[j].second++;
            if(temp_find[j].second > K) {   //删除缺失字符已经超过K的
                temp_find.erase(temp_find.begin() + j);
                continue;
            }
            //映射到F上
            if(temp_now == 'A') {
                temp_find[j].first = begin_a + Count_Num(temp_now, temp_find[j].first);
            } else if(temp_now == 'C') {
                temp_find[j].first = begin_c + Count_Num(temp_now, temp_find[j].first);
            } else if(temp_now == 'G') {
                temp_find[j].first = begin_g + Count_Num(temp_now, temp_find[j].first);
            } else if(temp_now == 'T') {
                temp_find[j].first = begin_t + Count_Num(temp_now, temp_find[j].first);
            }
            j++; 
        }
    }

    //此时temp_find里剩的就是可以满足最多缺失K个字符的匹配位置
    //同样在后缀数组里查找的操作
    vector<int>ans;
    for(int i = 0; i < temp_find.size(); i++) {
        int now_index = temp_find[i].first;
        if(now_index + 1 == 1 || ((now_index + 1) % SA_CKPT_LEN == 0)) {
            ans.push_back(SA[(now_index + 1) / SA_CKPT_LEN + 1] - 1 + file_num * CUT_LEN);
        } else {
            int pre = 0;
            int now_i = now_index;
            while(now_i + 1 != 1 && ((now_i + 1) % SA_CKPT_LEN != 0)) {
                char temp = L[now_i];
                if(temp == 'A')
                    now_i = begin_a + Count_Num(temp, now_i);
                else if(temp == 'C')
                    now_i = begin_c + Count_Num(temp, now_i);
                else if(temp == 'G')
                    now_i = begin_g + Count_Num(temp, now_i);
                else if(temp == 'T')
                    now_i = begin_t + Count_Num(temp, now_i);
                else if(temp == '$') {
                    now_i = -1;
                    break;
                }
                pre++;
            }
            if(now_i != -1)
                ans.push_back(SA[(now_i + 1) / SA_CKPT_LEN + 1] - 1 + pre + file_num * CUT_LEN);
            else
                ans.push_back(0 + pre);
        }
    }
    sort(ans.begin(), ans.end());
    return ans;
}



int main() {
    // 长串预处理
    // clock_t start, end;
    // start = clock();    
    // // string file_name = "SRR00001_rows_ATCG";
    // string file_name = "SRR311115_rows_ATCG";
    // Cut_And_Encode(file_name);
    // end = clock();
    // cout<<"time = "<<double(end-start)/CLOCKS_PER_SEC<<"s"<<endl;

    //匹配测试
    ifstream short_file("short/search_SRR163132.txt");
    string short_string;
    ofstream answer_file("answer/SRR311115_SRR163132_answer.txt");
    clock_t total_start, total_end;
    total_start = clock();

    //读入短串，进行匹配
    while(short_file >> short_string) {
        vector<int>ans = Find(short_string, "SRR311115_rows_ATCG");
        for(int i = 0; i < ans.size(); i++)
            answer_file << ans[i] <<" ";
        answer_file << endl;
    }

    total_end = clock();
    cout<<"time = "<<double(total_end - total_start) / CLOCKS_PER_SEC<<"s"<<endl;
    cout<<"SRR311115_rows_ATCG find done"<<endl;
    short_file.close();
    answer_file.close();
    return 0;
}