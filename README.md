GDI++
=====

*Note: this repository is an unofficial mirror of GDI++. The original project is located [here](https://goo.gl/Oa1egm). The project has been dead for quite some time, and was superseded by [GDIPP](https://github.com/CoolOppo/GDIPP). This repository is only here for historical purposes.*

### Introduction

GDI++ is a replacement for the Windows default font rasterizer that provides font smoothing (i.e. anti-aliasing) like OS X. It hacks one of the most important core DLLs for graphics, `gdi32.dll`.

### Disclaimer

This software is naturally dangerous and causes a heavy CPU load. You should understand how it works before using it. It may contain a bunch of bugs, so please use it at your own risk and back up the files if need be. You need to have Windows XP to run this software, possibly Windows 2000 is okay as well.

### How it works

- ExtTextOut() has been called
- Multiply the font size integrally (2x, 3x, 4x)
- Draw
- Resample
- Transfer If the background mode of the DC is transparent, we leave a messy alpha-blending thing to the font rasterizer by stretching the background image and transfer it to the buffer preliminarily.

Usage
-----

Just drag and drop an application onto gdi++.exe.

### Options

You can configure the rendering option by placing a file named `gdi++.ini` in the directory.

```ini
[General]
Quality=1
Weight=0
Enhance=0
UseSubPixel=0
SubPixelDirection=0
MaxHeight=0
ForceAntialiasedQuality=0
[Exclude]
FixedSys
Marlett
```

#### `[General]` section

- `Quality`
	- Choose the rendering quality.
	- `0`: disabled, `1`: twice, `2`: thrice, `3`: quadruple
- `Weight`
	- Choose how bold the font will be drawn. The text will be drawn (n + `1`) times repeatedly.
- `Enhance`
	- Enhance horizontal/vertical edges.
	- `0`: disabled (default), `1`: a bit enhanced, `2`: normal, `3`: super enhanced, `4`: extremely enhanced
- `UseSubPixel`
	- Choose whether simplified sub-pixel rendering (fake ClearType) is enabled. (still glitchy)
	- `0`: disabled (default), `1`: enabled
- `SubPixelDirection`
	- Choose the sub-pixel direction of your LCD screen. Most screens have a RGB striping order.
	- `0`: RGB (default), `1`: BGR
- `MaxHeight`
	- Specify the maximum font size (in pixels) to smooth (disable with `0`)
	- `0`: disabled (default)
- `ForceAntialiasedQuality`
	- Never use ClearType when drawing texts on Windows XP. It may speed up the entire performance.
	- `0`: Normal (default), `1`: Never draw texts with ClearType

#### `[Exclude]` section

Specify the font names you want to exclude on each line. If the font name is on this list, GDI++ will bypass the processing to the default rasterizer. This option is good for bitmap fonts or outline fonts optimized for ClearType, i.e. Meiryo.

###### Contacts *(some links are possibly dead now, so find new ones from Google or something)*

http://drwatson.nobody.jp/gdi++/

http://pc8.2ch.net/test/read.cgi/win/1158937333/

http://pc8.2ch.net/test/read.cgi/win/1153828837/
