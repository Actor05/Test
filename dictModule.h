#ifndef _DICTMODULE_H_

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <strstream>
#include <map>

using namespace std;

#define SENTENCE_MAX_LENGTHEN 500
typedef pair <string, double> P1;
typedef pair <string, vector < pair <string, double> > > P2;

int cmp(const P2 &a, const P2 &b) {
    return a.first < b.first;
}

class Dict {
private:
    //map <string, double> item;
    P1 curExp;
    P2 curItem;
    vector < P2 >::iterator it;
    int cnnt;
public:
    vector < P2 > item;
    double value[SENTENCE_MAX_LENGTHEN][SENTENCE_MAX_LENGTHEN];
    Dict (string filePath) {
        cnnt = 0;
        ifstream in(filePath.c_str());
        string line, word;
        item.clear();
        if (!in) cerr << "Loading dictionary Error !!!\n";
        getline(in, line);
        while (getline(in, line)) {

            curItem.second.clear();

            istrstream ii(line.c_str());
            if (ii>>word) {
                curItem.first = word;
                bool state = 0;
                while (ii >> word) {
                    if (word == "/") {
                        if (state == 1) {
                            curExp.second = -1.0;
                            curItem.second.push_back(curExp);
                            state = 0;
                        }
                        continue;
                    }
                    if (state == 0) {
                        curExp.first = word;
                        state = 1;
                    }
                    else {
                        double sum = 0;
                        int gd = 0;
                        for (unsigned int i = 0; i < word.size(); ++i) {
                            if (word[i] == '.') { gd = 1; continue;}
                            if (word[i] >= '0' && word[i] <= '9') {
                                sum = sum*10 + word[i]-'0';
                                gd *= 10;
                            }
                        }
                        curExp.second = sum / gd;
                        curItem.second.push_back(curExp);
                        state = 0;
                    }
                }
            }
            item.push_back(curItem);
        }
        //cerr << item[0].first << endl;
        sort(item.begin(), item.end(), cmp);
        cerr << "dictionary size = " << item.size() << endl;
        //cerr << item[0].first << endl;
    }

    int B_Search(string words) {
        int left = 0, right = item.size()-1;
        while (left <= right) {
            int mid = (left + right)>>1;
            if (item[mid].first < words) left = mid + 1;
            else if (item[mid].first > words) right = mid-1;
            else {
                return mid;
            }
        }
        return -1;
    }

    bool Binary_Search(string words) {
        int left = 0, right = item.size()-1;
        while (left <= right) {
            int mid = (left + right)>>1;
            if (item[mid].first < words) left = mid + 1;
            else if (item[mid].first > words) right = mid-1;
            else {
                it = item.begin()+mid;
                return true;
            }
        }
        return false;
    }

    void getGraph(vector<string> s1, vector<string> s2) {
        unsigned int i, j, k;
        for (i = 0; i < s1.size(); ++i)
            for (j = 0; j < s2.size(); ++j)
                value[i][j] = 0;
        for (i = 0; i < s1.size(); ++i) {
            if (Binary_Search(s1[i])==false) {
                cerr << "Warning: words <"+s1[i]+"> can't find in dictionary\n";
                continue;
            }
            for (j = 0;j < s2.size(); ++j)
                for (k = 0; k < (*it).second.size(); ++k)
                {
                    if ((*it).second[k].first == s2[j]) {
                        //cnnt ++;
                        //cout <<"@@@:"<< cnnt << endl;
                        if ((*it).second[k].second < 0)
                            value[i][j] = 100.0 / (*it).second.size();
                        else
                            value[i][j] = (*it).second[k].second;
                        //cerr << s1[i] << s2[j] << value[i][j] << endl;
                        break;
                    }
                }
        }
        //return value;
    }
    ~Dict() {
        item.clear();
    }
};

#define _DICTMODULE_H_
#endif
