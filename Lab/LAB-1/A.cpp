#include <bits/stdc++.h>
#include <unistd.h>

using namespace std;

int main(){
    cout<<"Process A runs\n";
    char ch;
    while(1){
        if(!read(0, &ch, sizeof(char)))break;
        sleep(1);
        cout<<"Process A: "<<ch<<"\n";
    }
    return 0;
}