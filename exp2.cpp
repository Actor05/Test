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
    ofstream oo("out.txt"), o1("Log.txt"), dd("dd.txt"), dx("dx.txt"), xd("xd.txt"), xx("xx.txt");
    if (!in) cerr << "Error\n";
    string line, chn, eng, mamual, nb;
    getline(in, line);
    int minesame, same, minepc, pc, minerc, rc;    //召回率和准确率计算
    minesame = same = 0, minepc = pc = 0, minerc = rc = 0;
    int count1 = 0, count2 = 0, count3 = 0, count4 = 0, count5 = 0, count6 = 0, count7 = 0;

    int countdd[10], countdx[10], countxd[10], countxx[10], sum[10];
    int dddd=0,ddxx=0,xxdd=0,xxxx=0;
    memset(countdd, 0, sizeof(countdd));
    memset(countdx, 0, sizeof(countdx));
    memset(countxd, 0, sizeof(countxd));
    memset(countxx, 0, sizeof(countxx));
    memset(sum, 0, sizeof(sum));
    int mysum = 0, sentenseNumber = 0;;
    while ((getline(in, chn) && getline(in, eng)) ) {
        sentenseNumber++;
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

        /*Mine*/
        memset(sValue, 0, sizeof(sValue));
        memset(av, 0, sizeof(av));
        memset(align, 0, sizeof(align));
        memset(ss, 0, sizeof(ss));

        for (int i = 0; i < 10; ++i)
            for (int j = 0; j < ci[i].size(); ++j) {
                sValue[ci[i][j].first][ci[i][j].second] += 5;
                //ss[i] += 10 - i;
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
        int minemax = -1, uu;
        for (int i = 1; i < 10; ++i) {
            int temp = 0;
            for (int j = 0; j < ci[i].size(); ++j)
                temp += sValue[ci[i][j].first][ci[i][j].second] + align[ci[i][j].first][ci[i][j].second];
            if (minemax < temp) {
                minemax = temp;
                uu = i;
            }
        }
        mysum += ci[uu].size();

        int mmax = -1, utemp;
        for (int i = 1; i < 10; ++i) {
            sum[i] = sum[i] + ci[i].size();
            int j = 0, k = 0, l = 0, ccnt = 0, ccnt2 = 0;
            for (j = 0; j < sci.size(); ++j) {
                for (k = 0; k < ci[i].size(); ++k) {
                    if (sci[j] == ci[i][k]) {
                        for (l = 0; l < ci[0].size(); ++l)
                            if (sci[j] == ci[0][l]) {
                                countdd[i]++;
                                if (uu == i)dddd++;
                                ccnt++;ccnt2++;
                                dd << i << " "<< sc[j] << endl;
                                break;
                            }
                        if (l == ci[0].size()) {
                            countxd[i]++;
                            if (uu == i)xxdd++;
                            ccnt ++;//count the xd number.
                            xd << i <<" " << sc[j];
                            for (l = 0; l < ci[0].size(); ++l) {
                                if (ci[0][l].first < sci[j].second && ci[0][l].second > sci[j].first)
                                    xd << " " << c[0][l];
                            }
                            xd << endl;
                        }
                        break;
                    }
                }
                if (k == ci[i].size()) {
                    for (l = 0; l < ci[0].size(); ++l) {
                        if (sci[j] == ci[0][l]) {
                            countdx[i]++;
                            if (uu == i)ddxx++;
                            ccnt2++;
                            dx << i << " " << sc[j];
                            for (k = 0; k < ci[i].size(); ++k) {
                                if (ci[i][k].first < sci[j].second && ci[i][k].second > sci[j].first)
                                    dx << " " << c[i][k];
                            }
                            dx << endl;
                            break;
                        }
                    }
                    if (l == ci[0].size()) {
                        countxx[i]++;
                        if (uu == i)xxxx++;
                        /**xx**/
                        xx <<"--------------------------\n" << sc[j]<< endl;
                        for (k = 0; k < ci[i].size(); ++k) {
                            if (ci[i][k].first < sci[j].second && sci[j].first < ci[i][k].second)
                                xx << c[i][k] << " ";
                        }
                        xx << endl;
                        for (l = 0; l < ci[0].size(); ++l) {
                            if (ci[0][l].first < sci[j].second && sci[j].first < ci[0][l].second)
                                xx << c[0][l] << " ";
                        }
                        xx <<"\n--------------------------\n";
                    }
                }
            }
            if (mmax < ccnt) {
                mmax = ccnt;
                utemp = i;
            }
            if (mmax < ccnt2) {
                mmax = ccnt2;
                utemp = 0;
            }
        }
        count5 += mmax;
        count6 += ci[utemp].size();

    }
    cerr << "sentenseNumber:" << sentenseNumber <<endl;
    for (int i = 1; i < 10; ++i) {
        count1 += countdd[i];
        count2 += countdx[i];
        count3 += countxd[i];
        count4 += countxx[i];
        cout << i << " " << countdd[i] << " " << countdx[i] << " " << countxd[i] << " " << countxx[i];
        double same = countdd[i] + countxd[i];
        double p = same*100 / sum[i];
        double r = same*100 / 10440;
        double F = same*200 / (sum[i] + 10440);
        cout << " P:" << p << " R:" << r << " F:" << F << endl;
    }
    cerr << count1 << "; "<< count2 << ";" <<count3 << ";" << count4<< ";" << count5 <<";"<<count6<<";"<<count7<<endl;
    cout <<"Mine"<< dddd<< ";"<< ddxx << ";" << xxdd <<";" << xxxx <<";"<< mysum <<endl;
}

int main() {

    d = new Dict("ldc_ec_dict.2.0.txt");
    //memset(cnt, 0, sizeof(cnt));
    exp1();
    return 0;
}
