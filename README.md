# webui-mini-preview
Hack to get a preview of the images on a small ESP32 device while they are generating.

[![Youtube link](https://img.youtube.com/vi/d8oRyAS-5Qw/0.jpg)](https://www.youtube.com/watch?v=d8oRyAS-5Qw)

Small hack to see if I could get the live preview in the stable diffusion webui to show on a M5Stack ESP32 Basic Core.

Do not expect too much. There is still work to do but "it works for me".

# Setup

Flash `Arduino/preview.ino` to a M5Stack Core. Make sure to change some settings to fit your WiFi and webui.

```
#define MY_SSID "WiFi SSID"
#define MY_PASS "WiFi password"
#define PREVIEWURL "http://192.168.1.10:7860/minip/run"
```

Install this repo as an extension. This will set up an api entrypoint `/minip/run` that will return a shrunk down 320x240 jpeg of the preview image. This is done to get a response small enough to be easy for the ESP32 to handle.

You need to run the webui with `--listen` (in theory `--share` would work too, but you would have to re-flash for every startup to get the correct url.)
