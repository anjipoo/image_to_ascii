#include<bits/stdc++.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

class ASCII {
    private:
    string gradient=" .'`^,:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
    int outputwidth;

    public:
    ASCII(int w) {
        outputwidth=w;

    }

    int brightness(unsigned char r, unsigned char g, unsigned char b) {
        return (0.299 * r)+(0.587 * g)+(0.114 * b);
    }

    char picchar(int gray) {
        int idx = gray * (gradient.size()-1) / 255;
        return gradient[idx];
    }

    void printpxl(unsigned char r, unsigned char g, unsigned char b, char sym, bool colour=true) {
        if(colour) {
            cout << "\033[38;2;"<< (int)r << ";"<< (int)g << ";"<< (int)b << "m"<< sym;
        } else {
            cout << sym;
        }
    }

    void printHTML(unsigned char r, unsigned char g, unsigned char b, char sym) {
        cout << "<span style='color:rgb("<< (int)r << ","<< (int)g << ","<< (int)b << ")'>"<< sym<< "</span>";
    }

    void render(string filename, bool colour=true, bool web=false) {
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
                if(web) printHTML(r,g,b,symbol);
                else printpxl(r,g,b,symbol,colour);
            }

            if(web) cout << "<br>";
            else cout << "\033[0m\n";
        }

        if(!web) cout << "\033[0m";

        stbi_image_free(img);
    }
};

int main(int argc, char* argv[]) {
    if(argc < 2) {
        cout << "Usage:\n";
        cout << "./image image.jpg [width] [bw]\n";
        return 1;
    }

    int width = 100;
    bool colour = true;
    bool web = false;

    if(argc >= 3) width = stoi(argv[2]);

    if(argc >= 4) {
        string mode = argv[3];
        if(mode == "bw") colour = false;
    }
    if(argc >= 5) {
        string mode = argv[4];
        if(mode == "web") web = true;
    }

    ASCII art(width);
    art.render(argv[1], colour, web);
    return 0;
}