/*to run:
cd /c/Users/anjan/Downloads/cvt_to_ascii
g++ video.cpp -o video $(pkg-config --cflags --libs opencv4)
./video video.mp4
*/

#include<bits/stdc++.h>
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class VideoASCII {
    private:

    string gradient=" .'`^,:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";

    int outputwidth;

    public:
    VideoASCII(int w) {outputwidth=w;}

    int brightness(Vec3b pixel) {return (0.299 * pixel[2])+(0.587 * pixel[1])+(0.114 * pixel[0]);}

    char getChar(int gray) {
        int idx=gray*(gradient.size()-1)/255;
        return gradient[idx];
    }

    void printPixel(Vec3b pixel, char ch) {cout << "\033[38;2;"<< (int)pixel[2] << ";"<< (int)pixel[1] << ";"<< (int)pixel[0] << "m"<< ch;}

    void render(string filename) {
        VideoCapture video(filename);

        if(!video.isOpened()) {
            cout << "Cannot open video\n";
            return;
        }

        Mat frame;

        while(video.read(frame)) {
            int width=frame.cols;
            int height=frame.rows;

            int outputheight=(height * outputwidth/width)/2;

            Mat resized;

            resize(frame, resized, Size(outputwidth, outputheight));

            cout << "\033[H";

            for(int i = 0; i < resized.rows; i++) {
                for(int j = 0; j < resized.cols; j++) {
                    Vec3b pixel=resized.at<Vec3b>(i, j);

                    int gray=brightness(pixel);

                    char ch=getChar(gray);

                    printPixel(pixel, ch);
                }

                cout << "\033[0m\n";
            }

            this_thread::sleep_for(
                chrono::milliseconds(30)
            );
        }

        cout << "\033[0m";
    }

};

int main(int argc, char* argv[]) {

    if(argc < 2) return 1;

    cout << "\033[2J";

    VideoASCII player(120);

    player.render(argv[1]);

    return 0;
}