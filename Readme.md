# Glotter

Glotter is a self-hostable translation server.
This server uses firefox translation models [see](https://github.com/mozilla/firefox-translations-models).
_Kindly note that glotter is not affiliated with The Mozilla Foundation in any way._

The implementation is heavily inspired from our other app [LocalTranslate](https://github.com/terslang/LocalTranslate)

The API provides 5 endpoints where only 3 of them are relevant for most of the usecases.
* */translate* - Takes text, from and to language codes and translates the text.
* */transliterate* - Romanizes text, useful for romanizing non-latin languages
* */languages* - Returns a list of languages and their ISO codes that are supported by this API
* */version* - Responds with the current version of the API
* */source-code* - Redirects to this github page

**Just hitting the root '/' from a browser window gives a basic UI for translations**

> Please refer to the openapi.yaml file for api specification.

## Here are a couple of examples

<img width="1127" height="577" alt="Image" src="https://github.com/user-attachments/assets/960955ae-7632-46b4-becd-6e6063697fa8" />

<img width="1248" height="548" alt="Image" src="https://github.com/user-attachments/assets/3902df17-f481-4c45-8893-ecd486e7de23" />

## How to run docker image?

`glotter` docker image is available in [ghcr](https://github.com/terslang/glotter/pkgs/container/glotter) packages.

Pull the image
```bash
docker pull ghcr.io/terslang/glotter:latest
```

Run the container
```bash
docker run -d -p 9107:9107 --name glotter ghcr.io/terslang/glotter
```

I recommend using a reverse proxy with SSL/TLS for https when self hosting.

## Built with (Thanks to)
- [terslang/kotki](https://github.com/terslang/kotki) (fork of [kroketio/kotki](https://github.com/kroketio/kotki))
- [terslang/marian-lite](https://github.com/terslang/marian-lite) (fork of [kroketio/marian-lite](https://github.com/kroketio/marian-lite))
- [mozilla/firefox-translations-models](https://github.com/mozilla/firefox-translations-models)
- [MeCab](https://github.com/taku910/mecab)
- [libICU](https://github.com/unicode-org/icu)
- [cpp-httplib](https://github.com/yhirose/cpp-httplib)
