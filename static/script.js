/* ================================
   ASCIIfy — script.js
   Handles: tabs, drag-drop, image upload,
            webcam feed via WebSocket
   ================================ */

const socket = io();

/* ── Tab Switching ── */
document.querySelectorAll('.tab').forEach(tab => {
  tab.addEventListener('click', () => {
    document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
    document.querySelectorAll('.panel').forEach(p => p.classList.remove('active'));
    tab.classList.add('active');
    document.getElementById('panel-' + tab.dataset.tab).classList.add('active');
  });
});

/* ── Width Slider ── */
const widthRange = document.getElementById('width-range');
const widthVal   = document.getElementById('width-val');
widthRange.addEventListener('input', () => {
  widthVal.textContent = widthRange.value;
});

/* ── Drag & Drop ── */
const dropzone  = document.getElementById('dropzone');
const fileInput = document.getElementById('file-input');
const preview   = document.getElementById('upload-preview');

dropzone.addEventListener('dragover', e => {
  e.preventDefault();
  dropzone.classList.add('drag-over');
});

dropzone.addEventListener('dragleave', () => {
  dropzone.classList.remove('drag-over');
});

dropzone.addEventListener('drop', e => {
  e.preventDefault();
  dropzone.classList.remove('drag-over');
  const file = e.dataTransfer.files[0];
  if (file && file.type.startsWith('image/')) {
    showPreview(file);
  }
});

dropzone.addEventListener('click', e => {
  if (!e.target.classList.contains('file-browse')) {
    fileInput.click();
  }
});

fileInput.addEventListener('change', () => {
  const file = fileInput.files[0];
  if (file) showPreview(file);
});

function showPreview(file) {
  const reader = new FileReader();
  reader.onload = e => {
    preview.innerHTML = `<img src="${e.target.result}" alt="preview" />`;
  };
  reader.readAsDataURL(file);
}

/* ── Image Convert ── */
const btnConvert    = document.getElementById('btn-convert');
const colorToggle   = document.getElementById('color-toggle');
const outputSection = document.getElementById('output-section');
const asciiOutput   = document.getElementById('ascii-output');
const loader        = document.getElementById('loader');

btnConvert.addEventListener('click', async () => {
  const file = fileInput.files[0];
  if (!file) {
    alert('Please select an image first.');
    return;
  }

  const formData = new FormData();
  formData.append('file', file);
  formData.append('width', widthRange.value);
  formData.append('color', colorToggle.checked ? 'on' : 'off');

  outputSection.style.display = 'none';
  loader.style.display        = 'flex';
  btnConvert.disabled         = true;

  try {
    const res  = await fetch('/image', { method: 'POST', body: formData });
    const html = await res.text();

    asciiOutput.innerHTML   = html;
    outputSection.style.display = 'block';
  } catch (err) {
    alert('Error converting image: ' + err.message);
  } finally {
    loader.style.display    = 'none';
    btnConvert.disabled     = false;
  }
});

/* ── Webcam ── */
const btnStart       = document.getElementById('btn-webcam-start');
const btnStop        = document.getElementById('btn-webcam-stop');
const video          = document.getElementById('webcam-video');
const canvas         = document.getElementById('webcam-canvas');
const asciiWC        = document.getElementById('ascii-webcam');
const webcamIdle     = document.getElementById('webcam-idle');
const webcamFullscreen = document.getElementById('webcam-fullscreen');

let stream     = null;
let animFrame  = null;
let streaming  = false;

btnStart.addEventListener('click', async () => {
  try {
    stream = await navigator.mediaDevices.getUserMedia({ video: true });
    video.srcObject = stream;
    await video.play();

    streaming = true;
    webcamIdle.style.display      = 'none';
    webcamFullscreen.style.display = 'flex';

    sendFrame();
  } catch (err) {
    alert('Could not access webcam: ' + err.message);
  }
});

btnStop.addEventListener('click', stopWebcam);

function stopWebcam() {
  streaming = false;
  if (animFrame) cancelAnimationFrame(animFrame);
  if (stream) stream.getTracks().forEach(t => t.stop());
  video.srcObject = null;
  stream = null;

  webcamFullscreen.style.display = 'none';
  webcamIdle.style.display       = 'flex';
  asciiWC.innerHTML = '';
}

function sendFrame() {
  if (!streaming) return;

  const ctx = canvas.getContext('2d');
  canvas.width  = video.videoWidth;
  canvas.height = video.videoHeight;
  ctx.drawImage(video, 0, 0);

  const dataURL = canvas.toDataURL('image/jpeg', 0.6);
  socket.emit('webcam_frame', dataURL);

  setTimeout(sendFrame, 60);
}

// Server now sends HTML with <span> color tags — use innerHTML
socket.on('ascii_frame', data => {
  asciiWC.innerHTML = data;
});