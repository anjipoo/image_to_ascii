#include<bits/stdc++.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using namespace std;

string ascii_chars="@%#*+=-:. ";

int main() {
    string filename="face.jpeg";
    int w, h, c;
    unsigned char *img=stbi_load(filename.c_str(), &w, &h, &c, 0);

    if(img==nullptr) {
        cout<<"no img loaded"<<endl;
        return 1;

    }

    int nw=50, nh=(h*nw/w)*0.5;
    for(int i=0; i<nh; i++) {
        for(int j=0; j<nw; j++) {
            int px=j*w/nw;
            int py=i*h/nh;

            int idx=(py*w+px)*c;

            int r=img[idx];
            int g=img[idx+1];
            int b=img[idx+2];

            int gray= 0.299*r + 0.587*g + 0.114*b;

            int asciidx=gray*(ascii_chars.size()-1)/255;

            // cout<<ascii_chars[asciidx];

            cout<<"\033[38;2;"<< r << ";"<< g << ";"<< b << "m"<< ascii_chars[asciidx];
        }

        cout << "\033[0m\n";
    }
    cout << "\033[0m";

    stbi_image_free(img);

    return 0;
}