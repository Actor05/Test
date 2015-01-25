#include "dictModule.h"
#include "tool.h"
#include <string.h>
Dict *d;
int sValue[100][100] ;//cnt[10];
int av[100][100][100], ss[10], align[100][100];

int pow(int i, int j) {
    int ans = 1;
    while (j) {
        if (j&1) ans = ans*i;
        i = i*i;
        j >>= 1;
    }
    return ans;
}
int exp1() {
    //cerr << "step 0 - OK";
    ifstream in("n-best300.txt");
    ofstream oo("out.txt"), o1("Log.txt");
    if (!in) cerr << "Error\n";
    string line, chn, eng, mamual, nb;
    getline(in, line);
    int psum = 0, rsum = 0, fenzi = 0;
    int correct[10], error[10];
    memset(correct, 0, sizeof(correct));
    memset(error, 0, sizeof(error));
    while ((getline(in, chn) && getline(in, eng)) ) {
        getline(in, mamual);getline(in, nb);
        string_UTF8* su = new string_UTF8(chn);//存放中文句子
        vector <string> e, c[10], sc, ng;//e->英文句子; sc->中文句子标准切分;
        vector < pair <int, int> > ci[10], sci, dp, ngi;//sci->中文句子标准切分下标
        pair <int, int> pp;
        string tmp;
        char *p = strtok((char*)eng.c_str(), " ");
        if ( p ) {
            tmp.assign(p);
            e.push_back(p);
            while ( p = strtok(NULL, " ") ) {
                tmp.assign(p);
                e.push_back(p);
            }
        }

        int start = 0, end = 0, state = 0;
        for (int i = 0; i < mamual.size(); ++i) {
            if (mamual[i] == ' ') {
                tmp = "";
                for (int j = start;j < end; ++j)
                    tmp += su->WordAt(j);
                sc.push_back(tmp);
                pp.first = start, pp.second = end;
                sci.push_back(pp);
                start = end = 0;
                state = 0;
            } else if (mamual[i] == '-') {
                state = 1;
            } else {
                if (state == 0) start = start*10+mamual[i]-'0';
                else end = end*10 + mamual[i]-'0';
                if (i == mamual.size()-1) {
                    tmp = "";
                    for (int j = start;j < end; ++j)
                        tmp += su->WordAt(j);
                    sc.push_back(tmp);
                    pp.first = start, pp.second = end;
                    sci.push_back(pp);
                }
            }
        }

        vector < pair <int, string> >tt;
        p = strtok((char*)nb.c_str(), " ");
        tmp.assign(p);
        pair <int , string> ttt;
        ttt.second = tmp;
        ttt.first = 0;
        for (int i = 0; i < tmp.size(); ++i)
            if (tmp[i] == '-') break;
            else ttt.first = ttt.first*10+tmp[i]-'0';
        tt.push_back(ttt);
        //cout << p << endl;
        while (p = strtok(NULL, " ")) {
            tmp.assign(p);
            ttt.second = tmp;
            ttt.first = 0;
            for (int i = 0; i < tmp.size(); ++i)
                if (tmp[i] == '-') break;
                else ttt.first = ttt.first*10+tmp[i]-'0';
            tt.push_back(ttt);
            //cout << p << endl;
        }
        sort (tt.begin(), tt.end());
        nb = "";
        for (int i = 0; i < tt.size(); ++i)
        {
            nb += (tt[i].second + " ");
            //cout << tt[i].second <<endl;
        }

        state = 0;
        start = end = 0;
        for (int i = 0; i < nb.size(); ++i) {
            if (nb[i] == ' ') {
                state = 0;
                start = end = 0;
                continue;
            } else if (state == 0 && nb[i] >= '0' && nb[i] <= '9') {
                start = start*10+nb[i]-'0';
            } else if (state == 0 && nb[i] == '-') {
                state = 1;
            } else if (state == 1 && nb[i] >= '0' && nb[i] <= '9') {
                end = end*10+nb[i] - '0';
            } else if (state == 1 && nb[i] == ',') {
                state = 2;
                tmp = "";
                for (int j = start; j < end; ++j)
                    tmp += su->WordAt(j);
            } else if (state == 2 && nb[i] != ',') {
                c[nb[i]-'0'].push_back(tmp);
                pp.first = start, pp.second = end;
                ci[nb[i]-'0'].push_back(pp);
            }
        }
        //cout << "step 1 -OK\n";

        int i, j, k, l, number=0;

        for (i = 0; i < ci[0].size(); ++i) {
            for (j = 0; j < sci.size(); ++j)
                if (sci[j] == ci[0][i]) {
                    number = 0;
                    for (k = 1; k < 10; ++k)
                        for (l = 0; l < ci[k].size(); ++l) {
                            if (ci[k][l] == ci[0][i]) {
                                number ++;
                                break;
                            }
                        }
                    correct[number] ++;
                    break;
                }
            if (j == sci.size()) {
                number = 0;
                for (k = 1; k < 10; ++k)
                    for (l = 0; l < ci[k].size(); ++l) {
                        if (ci[k][l] == ci[0][i]) {
                            number++;
                            break;
                        }
                    }
                error[number] ++;
                if (number == 9) {
                    oo << c[0][i] << " VS ";
                    for (j = 0; j < sci.size(); ++j)
                        if (sci[j].first < ci[0][i].second && sci[j].second > ci[0][i].first)
                            oo << sc[j] << " ";
                    oo << endl;
                }
            }
        }

    }
    int ssss = 0;
    for (int i = 0; i < 10; ++i) {
        cerr << correct[i] <<";" << error[i] << endl;
        ssss = ssss + correct[i]+error[i];
    }
    cerr << ssss <<endl;
}

int main() {

    d = new Dict("ldc_ec_dict.2.0.txt");
    //memset(cnt, 0, sizeof(cnt));
    exp1();
    return 0;
}
