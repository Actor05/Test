#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <string>

using namespace std;

vector <string> chn,eng;
vector <string> mamual, nb;

int main() {
    ifstream in("ch300.txt"), in1("chinese.doc"), in2("english_GENIA_UTF-8.txt"), ii("N_Best.txt");
    ofstream oo("n-best300.txt"), error("e.txt");
    string line1, line2;
    if ( !ii ) cerr << "File N_Best.txt can't open\n";
    if ( !in ) cerr << "File chn300.txt can't open\n";
    if (!in1) cerr << "File chinese.txt can't open\n";
    if (!in2) cerr << "File english_GENIA_UTF-8.txt can't open\n";

    getline(in, line1);
    getline(in1, line1);
    getline(in2, line1);
    int cnt = 0;
    while (getline(in1, line1)&&getline(in2, line2)) {
        ++cnt;
        char p[1<<14];
        int i , j;
        for (i = 0, j = 0; i < line1.size(); ++i)
            if (line1[i] != ' ') p[j++] = line1[i];
        p[j] = 0;
        line1.assign(p);
        chn.push_back(line1);
        eng.push_back(line2);
    }
    cerr << "total senetense " << cnt << endl;
    getline(ii, line1);
    cnt = 0;
    while (getline(ii, line1)) {
        getline(ii, line1);
        mamual.push_back(line1);
        getline(ii, line1);
        nb.push_back(line1);
        getline(ii, line1);
        ++cnt;
    }
    cerr << cnt << endl;
    cnt = 0;
    while (getline(in, line1)) {
        int i;
        for (i = 0; i < chn.size(); ++i)
            if (line1 == chn[i]) {
                oo << line1 << endl;
                oo << eng[i]+"\n"+mamual[cnt]+"\n"+nb[cnt]+"\n";
                break;
            }
        if (i == chn.size()) error << line1+"\n";
        cnt ++;
    }
    return 0;
}
