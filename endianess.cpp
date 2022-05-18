#include <bits/stdc++.h>

using namespace std;

int main() {
    int num = 0xF42A;

    for(int i = 0; i < 4; i++) {
        cout << hex << (int)((unsigned char*)&num)[i] << endl;
    }

    //cout << hex << (int)*(unsigned char*)&num << " " << hex << (int)*(((unsigned char*)&num)+1) << endl;
    return 0;
}