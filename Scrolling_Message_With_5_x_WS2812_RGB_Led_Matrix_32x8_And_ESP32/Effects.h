

#ifndef Effects_H
#define Effects_H

#define fontbyte(x) Font[x] 

uint32_t noise_x = 200;
uint32_t noise_y;
uint32_t noise_z;
uint32_t noise_scale_x;
uint32_t noise_scale_y;
uint8_t noise[kMatrixWidth][kMatrixHeight];

#define SWAPint(X,Y) { \
        int temp = X ; \
        X = Y ; \
        Y = temp ; \
    }
   
uint16_t XY( uint8_t x, uint8_t y)
{
  if( x >= kMatrixWidth || x < 0 || y >= kMatrixHeight || y < 0) return -1;
  uint16_t i;
  if( x & 0x01) {
      uint8_t reverseY = (kMatrixHeight - 1) - y;
      i = (x * kMatrixHeight) + reverseY;
  } else {
      i = (x * kMatrixHeight) + y;
  }
  return i;
}

CRGB leds[kMatrixWidth * kMatrixHeight];

class Effects {
public:

  
  void NoiseVariablesSetup() {
    noise_x = random16(2000) - 1000; 
    noise_y = random16(2000) - 1000;
    noise_z = random16(2000) - 1000;
    noise_scale_x = random16(10000) + 2000;
    noise_scale_y = random16(10000) + 2000;
  }
  

  void FillNoise() {
    for (uint8_t i = 0; i < 80; i++) {
      uint32_t ioffset = noise_scale_x * (i - 40);

      for (uint8_t j = 0; j < 4; j++) {
        uint32_t joffset = noise_scale_y * (j - 4);

        byte data = inoise16(noise_x + ioffset, noise_y + joffset, noise_z) >> 8;

        uint8_t olddata = noise[i][j];
        uint8_t newdata = scale8(olddata, 200) + scale8(data, 56);
        data = newdata;

        noise[i][j] = noise[kMatrixWidth - 1 - i][j] = noise[i][kMatrixHeight - 1 - j] = noise[kMatrixWidth - 1 - i][kMatrixHeight - 1 - j] = data;
      }
    }
  }
  
void Pixel(int x, int y) {
  if( x >= kMatrixWidth || x < 0 || y >= kMatrixHeight || y < 0) return;
  uint8_t index = noise[x][y] * 4;
  leds[XY(x, y)] = ColorFromPalette(RainbowColors_p, index, 255, NOBLEND);
}
  
void Pixel(int x, int y, uint8_t index) {
  if( x >= kMatrixWidth || x < 0 || y >= kMatrixHeight || y < 0) return;
  leds[XY(x, y)] = ColorFromPalette(RainbowColors_p, index, 255, NOBLEND);
}
  

void fillRect(int x, int y, int w, int h, uint8_t c) {
  if (x < 0) {
    w += x;
    x = 0;
    }
  if (y < 0) {
    h += y;
    y = 0;
  }
  if (x + w > kMatrixWidth)
    w = kMatrixWidth - x;
  if (y + h > kMatrixHeight)
    h = kMatrixHeight - y;
  for (int j = y; j < y + h; j++)
    for (int i = x; i < x + w; i++)
      Pixel(i, j, c);
}

void drawLine(int x0, int y0, int x1, int y1, uint8_t c)
{
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy, e2;
  for (;;) {
    Pixel(x0, y0, c);
    if (x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if (e2 > dy) {
      err += dy;
      x0 += sx;
    }
    if (e2 < dx) {
      err += dx;
      y0 += sy;
    }
  }
}
void xLine(int x0, int x1, int y, uint8_t c)
  {
    if (x0 > x1)
    {
      int xb = x0;
      x0 = x1;
      x1 = xb;
    }
    if (x0 < 0)
      x0 = 0;
    if (x1 > kMatrixWidth)
      x1 = kMatrixWidth;
    for (int x = x0; x < x1; x++)
      Pixel(x, y, c);
  }

void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint8_t c) {
  drawLine(x0, y0, x1, y1, c);
  drawLine(x1, y1, x2, y2, c);
  drawLine(x2, y2, x0, y0, c);
}

void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint8_t c) {
  int16_t a, b, y, last;
  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    SWAPint(y0, y1);
    SWAPint(x0, x1);
  }
  if (y1 > y2) {
    SWAPint(y2, y1);
    SWAPint(x2, x1);
  }
  if (y0 > y1) {
    SWAPint(y0, y1);
    SWAPint(x0, x1);
  }

  if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if (x1 < a)
      a = x1;
    else if (x1 > b)
      b = x1;
    if (x2 < a)
      a = x2;
    else if (x2 > b)
      b = x2;
    drawFastHLine(a, y0, b - a + 1, c);
    return;
  }
  int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
          dx12 = x2 - x1, dy12 = y2 - y1;
  int32_t sa = 0, sb = 0;
  if (y1 == y2)
    last = y1; // Include y1 scanline
  else
    last = y1 - 1; // Skip it
  for (y = y0; y <= last; y++) {
    a = x0 + sa / dy01;
    b = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    if (a > b)
      SWAPint(a, b);
    drawFastHLine(a, y, b - a + 1, c);
  }
  sa = (int32_t)dx12 * (y - y1);
  sb = (int32_t)dx02 * (y - y0);
  for (; y <= y2; y++) {
    a = x1 + sa / dy12;
    b = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    if (a > b)
      SWAPint(a, b);
    drawFastHLine(a, y, b - a + 1, c);
  }
}

void drawFastHLine(int x, int y, int w, uint8_t c) {
  drawLine(x, y, x + w - 1, y, c);
}
void drawFastVLine(int x, int y, int h, uint8_t c) {
  drawLine(x, y, x, y + h - 1, c);
}

void fillEllipse(int x, int y, int rx, int ry, uint8_t c) {
  if(ry == 0)
    return;
  float f = float(rx) / ry;
  f *= f;   
  for(int i = 0; i < ry + 1; i++) {
    float s = rx * rx - i * i * f;
    int xr = (int)sqrt(s <= 0 ? 0 : s);
    xLine(x - xr, x + xr + 1, y + i, c);
    if(i) 
      xLine(x - xr, x + xr + 1, y - i, c);
    }
  }
  
void print(String st)
{
  char buf[st.length()+1];
  st.toCharArray(buf, st.length()+1);
  print(buf);
}

void print(char *st)
{
  int x = cursor_x;
  int y = cursor_y;
  int stl = strlen(st);
  for (int cnt=0; cnt<stl; cnt++){
    printchar(*st++, x + (cnt*(x_size)), y);
  }
}

void printchar(unsigned char c, int x, int y)
{
   int font_idx = ((c - offset)*((x_size*y_size/8)))+4;
    int cbyte=fontbyte(font_idx);
    int cbit=7;
    for (int cy=0; cy<y_size; cy++)
    {
      for (int cx=0; cx<x_size; cx++)
      {
        if ((cbyte & (1<<cbit)) != 0)
          Pixel(x+cx, y+cy);
        cbit--;
        if (cbit<0)
        {
          cbit=7;
          font_idx++;
          cbyte=fontbyte(font_idx);
        }
      }
    }
}

void setCursor(int x, int y) 
{
    cursor_x = x;
    cursor_y = y;
}  

void setFont(uint8_t* font_c)
{
  Font=font_c;
  x_size=fontbyte(0);
  y_size=fontbyte(1);
  offset=fontbyte(2);
  numchars=fontbyte(3);
}

protected:
  uint8_t* Font;
  uint8_t x_size;
  uint8_t y_size;
  uint8_t offset;
  uint8_t numchars;
  int cursor_x;     ///< x location to start print()ing text
  int cursor_y;     ///< y location to start print()ing text

};

#endif
