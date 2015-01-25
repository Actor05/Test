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
    int minesame, same, minepc, pc, minerc, rc;    //召回率和准确率计算
    minesame = same = 0, minepc = pc = 0, minerc = rc = 0;
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
        /* calculate */

        memset(sValue, 0, sizeof(sValue));
        memset(av, 0, sizeof(av));
        memset(align, 0, sizeof(align));
        memset(ss, 0, sizeof(ss));

        for (int i = 0; i < 10; ++i)
            for (int j = 0; j < ci[i].size(); ++j) {
                sValue[ci[i][j].first][ci[i][j].second] += 10-i;
                ss[i] += 10 - i;
            }
        /* n-gram */
        for (int i = 1; i <= 5; ++i)
            for (int j = 0; j+i < su->NumberOfWord(); ++j) {
                pp.first = j, pp.second = j+i;
                string tmp = "";
                for (int k = j; k < i+j; ++k)
                    tmp += su->WordAt(k);
                ng.push_back(tmp);
                ngi.push_back(pp);
            }
        /*查词典*/
        for (int i = 0, id = -1; i < e.size(); ++i) {
            if ((id = d->B_Search(e[i])) != -1) {
                for (int j = 0; j < ng.size(); ++j)
                    for (int k = 0; k < d->item[id].second.size(); ++k) {
                        if (d->item[id].second[k].first == ng[j]) {
                            if (e[i] == "alanine") oo << e[i]+" "<< ngi[j].first << " " << ngi[j].second<<endl;
                            align[ngi[j].first][ngi[j].second] += pow(2, (ngi[j].second-ngi[j].first))*20;
                            ss[i] += 50;
                        } else {
                            /* 此处添加一些优化方案 */
                        }
                    }
            }
        }


        //cerr << "step 2:";
        /* 计算F-score */
        //cerr << align[15][18];
        int f[1<<10], s[1<<10];
        memset(f, -1, sizeof(f));
        f[0] = 0;
        s[0] = -1;
        for (int i = 1; i <= su->NumberOfWord(); ++i) {
            for (int j = i-1; j >= 0; --j)
                if (f[j] > -1 && f[i] < f[j]+sValue[j][i]+align[j][i]) {
                    f[i] = f[j]+sValue[j][i]+align[j][i];
                    s[i] = j;
                }
        }
        oo << f[19] << " " << f[20] <<" "<< f[21] <<endl;
        oo << sValue[19][22] << " " << sValue[20][22] <<" " <<sValue[21][22] << endl;
        oo << align[19][22] << " " << align[20][22] << " " <<align[21][22] << endl;
        end = su->NumberOfWord();
        cerr << "step1\n";
        while (s[end] != -1) {
            pp.second = end;
            pp.first = s[end];
            dp.push_back(pp);
            end = s[end];
        }
        cerr << "step2\n";
        sort(dp.begin(), dp.end());

        oo << chn+"\n"+eng+"\n";
        int cnt = 0;
        for (int j = 0; j < dp.size(); ++j)
            for (int k = 0; k < sci.size(); ++k) {
                if (dp[j] == sci[k]) cnt++;
                else if ( dp[j].first < sci[k].second && sci[k].first < dp[j].second ){
                    for (int l = dp[j].first; l < dp[j].second; ++l)
                        o1 << su->WordAt(l);
                    o1 << " ";
                    for (int l = sci[k].first; l < sci[k].second; ++l)
                        o1 << su->WordAt(l);
                    o1 << " ";
                    o1 << endl;
                }
            }
        for (int i = 0; i < dp.size(); ++i) {
            for (int j = dp[i].first; j < dp[i].second; ++j)
                oo << su->WordAt(j);
            oo << " ";
        }
        minesame += cnt; minepc += dp.size(); minerc += sci.size();
        oo << "\nPrecision :" << 1.0*cnt/dp.size() << " Recall : " << 1.0*cnt/sci.size() ;
        oo << "F-score : " << 2.0*cnt/(dp.size()+sci.size()) <<endl;
        for (int i = 0; i < 10; ++i) {
            int cnt = 0;
            for (int j = 0; j < ci[i].size(); ++j) {
                oo << c[i][j] + " ";
                for (int k = 0; k < sci.size(); ++k)
                    if (ci[i][j] == sci[k]) cnt ++;
            }
            if (i == 0) {
                same += cnt;
                pc += ci[i].size();
                rc += sci.size();
            }
            oo << "\nPrecision :" << 1.0*cnt/ci[i].size() << " Recall : " << 1.0*cnt/sci.size() ;
            oo << "F-score : " << 2.0*cnt/(ci[i].size()+sci.size()) << " score : " << ss[i]<<endl;
        }
    }
    cerr << "mine P : " << 1.0*minesame/minepc << " R: " << 1.0*minesame/minerc << " F : " << 2.0*minesame/(minepc+minerc);
    cerr << "\nbest P : " << 1.0*same/pc << " R: " << 1.0*same/rc << " F : " << 2.0*same/(pc+rc)<<endl;;
    cerr << "same : " << same << " pc : " << pc << " R : " << rc << endl;
}

int main() {

    d = new Dict("ldc_ec_dict.2.0.txt");
    //memset(cnt, 0, sizeof(cnt));
    exp1();
    return 0;
}
