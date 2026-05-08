#include<bits/stdc++.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

class ASCII {
    private:
    vector<char> charset={ '@', '#', 'S', '%', '?', '*', '+', ';', ':', ',', '.'};
    int outputwidth;

    public:
    ASCII(int w) {
        outputwidth=w;

    }

    int brightness(unsigned char r, unsigned char g, unsigned char b) {
        return (0.299 * r)+(0.587 * g)+(0.114 * b);
    }

    char picchar(int gray) {
        int idx=gray*(charset.size()-1)/255;
        return charset[idx];
    }

    void printpxl(unsigned char r, unsigned char g, unsigned char b, char sym) {
        cout<<"\033[38;2;"<< (int)r << ";"<< (int)g << ";"<< (int)b << "m"<< sym;
    }

    void render(string filename) {
        int width, height, channels;
        unsigned char *img=stbi_load(filename.c_str(), &width, &height, &channels,3);

        if(!img) return;

        int outputheight=(height*outputwidth/width)/2;

        for(int i=0; i<outputheight; i++) {
            for(int j=0; j<outputwidth; j++) {
                int px=(j*width)/outputwidth;
                int py=(i*height)/outputheight;

                int pxlidx=(py*width+px)*3;

                int r=img[pxlidx];
                int g=img[pxlidx+1];
                int b=img[pxlidx+2];

                int gray=brightness(r,g,b);
                char symbol=picchar(gray);
                printpxl(r,g,b,symbol);
            }

            cout<<"\033[0m\n";
        }

        cout<<"\033[0m";

        stbi_image_free(img);
    }
};

int main(int argc, char* argv[]) {
    if(argc<2) {
        cout << "Usage: ./ascii image.jpg\n";
        return 1;
    }

    ASCII art(80);
    art.render(argv[1]);
    return 0;
}