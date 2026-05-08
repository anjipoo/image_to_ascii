import cv2
import os
import time

gradient = " .'`^,:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$"

width=100

def brightness(pxl):
    b,g,r=pxl
    return int((0.299 * r) + (0.587 * g) + (0.114 * b))

def getchar(gray):
    idx=gray*(len(gradient)-1)//255
    return gradient[idx]

cam=cv2.VideoCapture(0)

if not cam.isOpened():
    print("cant access camera")
    exit()

print("\033[2J")

while True:
    ret, frame=cam.read()
    if not ret:
        break

    frame=cv2.flip(frame,1)

    h,w,_=frame.shape

    outputheight=int((h*width/w)/2)

    resized = cv2.resize(frame, (width, outputheight))

    ascii_frame = ""

    for row in resized:
        for pixel in row:
            gray=brightness(pixel)
            ch=getchar(gray)
            b,g,r=pixel
            ascii_frame+=(f"\033[38;2;{r};{g};{b}m{ch}")

        ascii_frame+="\033[0m\n"

    print("\033[H"+ascii_frame, end="")
    time.sleep(0.03)

cam.release()
print("\033[0m")