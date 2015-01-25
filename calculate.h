#ifndef _CALCULATE_H_

#include "Tool.h"

typedef pair <pair < pair<int, int> , pair<int, int> > , pair < pair<int, int> , pair<int, int> > > PP;

class Cal {
private:
    int **sValue, **tValue;
    int ****F, ****av;
    int sLength, tLength;
    void Solution(PP&s, int i, int j, int k, int l, int ii, int jj, int kk, int ll) {
        s.first.first.first = i;
        s.first.first.second = j;
        s.first.second.first = k;
        s.first.second.second = l;
        s.second.first.first = ii;
        s.second.first.second = jj;
        s.second.second.first = kk;
        s.second.second.second = ll;
    }
public:
    PP ****S;
    vector < pair< int, int > > segment[2];
    vector < pair< pair <int, int>, pair<int, int> > > alignment;
    Cal (int slen, int tlen, int sv[100][100], int tv[100][100]):sLength(slen), tLength(tlen) {
        ++slen, ++tlen;
        av = new int***[slen];
        F = new int***[slen];
        S = new PP***[slen];
        for (int i = 0; i < slen; ++i) {
            av[i] = new int**[tlen];
            F[i] = new int**[tlen];
            S[i] = new PP**[tlen];
            for (int j = 0; j < tlen; ++j) {
                av[i][j] = new int*[slen];
                F[i][j] = new int*[slen];
                S[i][j] = new PP*[slen];
                for (int k = 0; k < slen; ++k) {
                    av[i][j][k] = new int[tlen];
                    F[i][j][k] = new int[tlen];
                    S[i][j][k] = new PP[tlen];
                }
            }
        }

        int i, j, k, l, jj, ll;

        for (i = 0; i < slen; ++i)
            for (j = 0; j < slen; ++j)
                sValue[i][j] = sv[i][j];
        for (i = 0; i < tlen; ++i)
            for (j = 0; j < tlen; ++j)
                tValue[i][j] = tv[i][j];

        memset(F, -1, sizeof(F));
        F[0][0][0][0] = 0;

        for (int scnt = 1; scnt < slen; ++scnt)
            for (int tcnt = 1; tcnt < tlen; ++tcnt)
                for (i = 0; i+scnt < slen; ++i)
                    for (k = 0; k+tcnt < tlen; ++k) {
                        j = i+scnt;
                        l = k+tcnt;
                        F[i][j][k][l] = sValue[i][j] + tValue[k][l] + av[i][j][k][l];
                        Solution(S[i][j][k][l], -1, -1, -1, -1, -1, -1, -1, -1);
                        if (i+1 < j && F[i][j][k][l] < F[i+1][j][k][l]) {
                            F[i][j][k][l] = F[i+1][j][k][l];
                            S[i][j][k][l] = S[i+1][j][k][l];
                        }
                        if (i < j-1 && F[i][j][k][l] < F[i][j-1][k][l]) {
                            F[i][j][k][l] = F[i][j-1][k][l];
                            S[i][j][k][l] = S[i][j-1][k][l];
                        }
                        if (k+1 < l && F[i][j][k][l] < F[i][j][k+1][l]) {
                            F[i][j][k][l] = F[i][j][k+1][l];
                            S[i][j][k][l] = S[i][j][k+1][l];
                        }
                        if (k < l-1 && F[i][j][k][l] < F[i][j][k][l-1]) {
                            F[i][j][k][l] = F[i][j][k][l-1];
                            S[i][j][k][l] = S[i][j][k][l-1];
                        }
                        for (jj = j+1; jj < k; ++jj) {
                            for (ll = k+1; ll < l; ++ll) {
                                if (F[i][jj][k][ll]!=-1 && F[jj+1][j][ll+1][l]!=-1 &&F[i][j][k][l] <= F[i][jj][k][ll] + F[jj+1][j][ll+1][l]) {
                                    F[i][j][k][l] = F[i][jj][k][ll] + F[jj+1][j][ll+1][l];
                                    Solution(S[i][j][k][l], i, jj, k, ll, jj+1, j, ll+1, l);
                                }
                                if (F[i][jj][ll+1][l]!=-1 && F[jj+1][j][k][ll]!=-1 &&F[i][j][k][l] <= F[i][jj][ll+1][l] + F[jj+1][j][k][ll]) {
                                    F[i][j][k][l] = F[i][jj][ll+1][l] + F[jj+1][j][k][ll];
                                    Solution(S[i][j][k][l], i, jj, ll+1, l, jj+1, j, k, ll);
                                }
                            }
                        }
                    }
        getMsg(0, 0, slen-1, tlen-1);
        sort(alignment.begin(), alignment.end());
        sort(segment[0].begin(), segment[0].end());
        sort(segment[1].begin(), segment[1].end());
    }

    void getMsg(int i, int j, int k, int l) {
        if (S[i][j][k][l].first.first.first == -1) {
            pair< pair <int, int>, pair<int, int> > pp;
            pp.first.first = i;
            pp.first.second = j;
            pp.second.first = k;
            pp.second.second = l;
            alignment.push_back(pp);
            pair <int, int> t(i,j);
            segment[0].push_back(t);
            t.first = k, t.second = l;
            segment[1].push_back(t);
            return ;
        }
        int a1, b1, c1, d1, a2, b2, c2, d2;
        a1 = S[i][j][k][l].first.first.first;
        b1 = S[i][j][k][l].first.first.second;
        c1 = S[i][j][k][l].first.second.first;
        d1 = S[i][j][k][l].first.second.second;
        getMsg(a1, b1, c1, d1);
        a2 = S[i][j][k][l].second.first.first;
        b2 = S[i][j][k][l].second.first.second;
        c2 = S[i][j][k][l].second.second.first;
        d2 = S[i][j][k][l].second.second.second;
        getMsg(a2, b2, c2, d2);

        if (a1 > a2) {swap(a1, a2); swap(b1, b2);}
        if (c1 > c2) {swap(c1, c2); swap(d1, d2);}
        pair <int, int> t;
        t.first = a1; t.second = b1;
        segment[0].push_back(t);
        t.first = a2; t.second = b2;
        segment[0].push_back(t);
        t.first = c1; t.second = d1;
        segment[1].push_back(t);
        t.first = c2; t.second = d2;
        segment[1].push_back(t);

        if (a1 > i) {
            t.first = i;
            t.second = a1;
            segment[0].push_back(t);
        }
        if (b1 < a2) {
            t.first = b1;
            t.second = a2;
            segment[0].push_back(t);
        }
        if (b2 < j) {
            t.first = b2;
            t.second = j;
            segment[0].push_back(t);
        }
        if (k < c1) {
            t.first = k;
            t.second = c1;
            segment[1].push_back(t);
        }
        if (d1 < c2) {
            t.first = d1;
            t.second = c2;
            segment[1].push_back(t);
        }
        if (d2 < l) {
            t.first = d2;
            t.second = l;
            segment[1].push_back(t);
        }
    }

    ~Cal() {
        delete(F);
        delete(av);
        delete(S);
    }
};

#define _CALCULATE_H_
#endif
