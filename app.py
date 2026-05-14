from flask import Flask, render_template, request, send_file
from flask_socketio import SocketIO, emit
import subprocess
import os
import cv2
import base64
import numpy as np
from PIL import Image, ImageDraw, ImageFont
from io import BytesIO
import platform

EXECUTABLE = './image.exe'

if platform.system() != 'Windows':
    EXECUTABLE = './image'

import eventlet
eventlet.monkey_patch()

app = Flask(__name__)
socketio = SocketIO(app)

BASE_DIR = os.path.dirname(os.path.abspath(__file__))

UPLOAD_FOLDER = os.path.join(BASE_DIR, "uploads")

OUTPUT_FOLDER = os.path.join(BASE_DIR, "output")

os.makedirs(UPLOAD_FOLDER, exist_ok=True)
os.makedirs(OUTPUT_FOLDER, exist_ok=True)

gradient = r" .'`^,:;Il!i~+_-?][}{1)(|\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$"

last_ascii = ""

@app.route('/')
def home():
    return render_template('index.html')

@app.route('/image', methods=['POST'])
def image_ascii():

    global last_ascii

    file = request.files['file']

    width = request.form.get('width', '100')

    color = request.form.get('color')

    mode = 'color'

    if color != 'on':
        mode = 'bw'

    filename = os.path.basename(file.filename)

    path = os.path.join(
        UPLOAD_FOLDER,
        filename
    )

    file.save(path)

    result = subprocess.run(
        [EXECUTABLE, path, width, mode, 'web'],
        capture_output=True,
        text=True,
        encoding='utf-8'
    )

    ascii_output = result.stdout

    last_ascii = ascii_output

    return ascii_output

@app.route('/download_txt')
def download_txt():

    global last_ascii

    clean = last_ascii

    import re

    clean = re.sub(r'<span[^>]*>', '', clean)
    clean = clean.replace('</span>', '')
    clean = clean.replace('<br>', '')

    with open('output.txt', 'w', encoding='utf-8') as f:
        f.write(clean)

    return send_file('output.txt', as_attachment=True)

@app.route('/download_png')
def download_png():

    global last_ascii

    import re

    clean = re.sub(r'<span[^>]*>', '', last_ascii)
    clean = clean.replace('</span>', '')
    clean = clean.replace('<br>', '')

    lines = clean.split('\n')

    width = max(len(line) for line in lines) * 8
    height = len(lines) * 12

    image = Image.new('RGB', (width, height), 'black')

    draw = ImageDraw.Draw(image)

    y = 0

    for line in lines:
        draw.text((0, y), line, fill='white')
        y += 12

    buf = BytesIO()

    image.save(buf, format='PNG')

    buf.seek(0)

    return send_file(buf,
                     mimetype='image/png',
                     as_attachment=True,
                     download_name='ascii.png')


@socketio.on('webcam_frame')
def webcam_frame(data):

    frame_data = data.split(',')[1]

    img_bytes = base64.b64decode(
        frame_data
    )

    np_arr = np.frombuffer(
        img_bytes,
        np.uint8
    )

    frame = cv2.imdecode(
        np_arr,
        cv2.IMREAD_COLOR
    )

    frame = cv2.flip(frame, 1)

    h, w, _ = frame.shape

    WIDTH = 120

    output_height = int(
        (h * WIDTH / w) / 2
    )

    resized = cv2.resize(
        frame,
        (WIDTH, output_height)
    )

    # Chars in the gradient that would break inline HTML
    html_escape = {'&': '&amp;', '<': '&lt;', '>': '&gt;'}

    frame_chars = []

    for row in resized:

        for pixel in row:

            b, g, r = pixel

            gray = int(
                (0.299 * r) +
                (0.587 * g) +
                (0.114 * b)
            )

            idx = (
                gray *
                (len(gradient)-1)
            ) // 255

            ch = gradient[idx]
            ch = html_escape.get(ch, ch)

            # Emit RGB HTML spans instead of plain chars
            frame_chars.append(
                f"<span style='color:rgb({int(r)},{int(g)},{int(b)})'>{ch}</span>"
            )

        frame_chars.append('<br>')

    ascii_frame = ''.join(
        frame_chars
    )

    emit(
        'ascii_frame',
        ascii_frame
    )

if __name__ == '__main__':
    socketio.run(app, debug=True, use_reloader=False, allow_unsafe_werkzeug=True)