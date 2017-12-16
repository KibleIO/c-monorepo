#ifndef GRAPHICS_CPP_
#define GRAPHICS_CPP_

#include "Graphics.h"

Graphics::Graphics(char *b, int w, int h, int s_w, int s_h) :
	buffer(b), width(w), height(h), screen_w(s_w), screen_h(s_h) {
	buffer_i = (int*) buffer;
	ContourX = new long[height * 2];
	setClip(-1, -1, -1, -1);
}

void Graphics::Swapbuffers(char *buff) {
	buffer   = buff;
	buffer_i = (int*) buff;
}

void Graphics::setClip(int x, int y, int w, int h) {
	//this is a bit flawed as we do not want to draw any pixels on the screen if the clipped area is
    //outside of the screen
    if (x < 0 || x >= width || y < 0 || y >= height) {
		x_c      = 0;
		y_c      = 0;
		width_c  = width;
		height_c = height;
    } else {
	    if ((w + x) > width) {
			w = (width - x);
	    }
	    if ((h + y) > height) {
			h = (height - y);
	    }
	    x_c      = x;
	    y_c      = y;
	    width_c  = w + x;
	    height_c = h + y;
    }
}

void Graphics::ScanLine(long x1, long y1, long x2, long y2) {
  long sx, sy, dx1, dy1, dx2, dy2, x, y, m, n, k, cnt;

  sx = x2 - x1;
  sy = y2 - y1;

  if ((sy < 0) || (sy == 0 && sx < 0))
  {
    k = x1; x1 = x2; x2 = k;
    k = y1; y1 = y2; y2 = k;
    sx = -sx;
    sy = -sy;
  }

  if (sx > 0) dx1 = 1;
  else if (sx < 0) dx1 = -1;
  else dx1 = 0;

  if (sy > 0) dy1 = 1;
  else if (sy < 0) dy1 = -1;
  else dy1 = 0;

  m = ABS(sx);
  n = ABS(sy);
  dx2 = dx1;
  dy2 = 0;

  if (m < n)
  {
    m = ABS(sy);
    n = ABS(sx);
    dx2 = 0;
    dy2 = dy1;
  }

  x = x1; y = y1;
  cnt = m + 1;
  k = n / 2;

  while (cnt--)
  {
    if ((y >= 0) && (y < width))
    {
      if (x < ContourX[y * height + 0]) ContourX[y * height + 0] = x;
      if (x > ContourX[y * height + 1]) ContourX[y * height + 1] = x;
    }

    k += n;
    if (k < m)
    {
      x += dx2;
      y += dy2;
    }
    else
    {
      k -= m;
      x += dx1;
      y += dy1;
    }
  }
}

void Graphics::fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, Color c) {
	long y;

  for (y = 0; y < height; y++)
  {
    ContourX[y * height + 0] = LONG_MAX; // min X
    ContourX[y * height + 1] = LONG_MIN; // max X
  }

  ScanLine(x0, y0, x1, y1);
  ScanLine(x1, y1, x2, y2);
  ScanLine(x2, y2, x0, y0);

  for (y = 0; y < height; y++)
  {
    if (ContourX[y * height + 1] >= ContourX[y * height + 0])
    {
      long x = ContourX[y * height + 0];
      long len = 1 + ContourX[y * height + 1] - ContourX[y * height + 0];

      // Can draw a horizontal line instead of individual pixels here
      while (len--)
      {
        drawPoint(x++, y, c);
      }
    }
  }
}

void Graphics::drawImage(int x, int y, Image * i) {
	for (int X = 0; X < i->getWidth(); X++) {
		for (int Y = 0; Y < i->getHeight(); Y++) {
			drawPoint(X + x, Y + y, i->getARGB(X,Y));
		}
	}
}

void Graphics::drawImage2(int x, int y, Image * i) {
	int temp;
        for (int X = 0; X < i->getWidth(); X++) {
                for (int Y = 0; Y < i->getHeight(); Y++) {
			temp = i->getARGB(X,Y);
			if (temp != -1) drawPoint(X + x, Y + y, temp);
                }
        }
}

void Graphics::drawPoint(int x, int y, Color c) {
	if (x > width_c - 1 || x < x_c || y > height_c - 1 || y < y_c){
		return;
	} else {
 		buffer_i[(x + y * width)] = c;
	}
}

void Graphics::drawPoint_wa(int x, int y, Color c) {
	if (x > width_c - 1 || x < x_c || y > height_c - 1 || y < y_c){
		return;
	} //implied else
	char frontA = (c >> 24) & 0xff;
	if (frontA == 0) {
		return;
	} else if (frontA == 0xff) {
		buffer_i[(x + y * width)] = c;
		return;
	}
	char frontR = (c >> 16) & 0xff;
	char frontG = (c >> 8) & 0xff;
	char frontB = (c >> 0) & 0xff;
	char backR = 0;
        char backG = 0;
        char backB = 255;
	if (buffer_i[(x + y * width)] != -1) {
		backR = (buffer_i[(x + y * width)] >> 16) & 0xff;
		backG = (buffer_i[(x + y * width)] >> 8) & 0xff;
		backB = (buffer_i[(x + y * width)] >> 0) & 0xff;
	}
	frontR = ((0xFF * frontR) / frontA);
	frontG = ((0xFF * frontG) / frontA);
	frontB = ((0xFF * frontB) / frontA);
	frontR = ((frontR * frontA) + (backR * (255 - frontA))) / 255;
	frontG = ((frontG * frontA) + (backG * (255 - frontA))) / 255;
	frontB = ((frontB * frontA) + (backB * (255 - frontA))) / 255;
 	buffer_i[(x + y * width)] = (0xFF << 24) | ((frontR & 0xFF) << 16) | ((frontG & 0xFF) << 8) | (frontB & 0xFF);
}

void Graphics::drawLine(int x1, int y1, int x2, int y2, Color c) {
	int dx, dy, inx, iny, e;
    dx = x2 - x1;
    dy = y2 - y1;
    inx = dx > 0 ? 1 : -1;
    iny = dy > 0 ? 1 : -1;

    dx = abs(dx);
    dy = abs(dy);

    if(dx >= dy) {
        dy <<= 1;
        e = dy - dx;
        dx <<= 1;
        while (x1 != x2) {
            drawPoint(x1, y1, c);
            if(e >= 0) {
                y1 += iny;
                e-= dx;
            }
            e += dy; x1 += inx;
        }
    } else {
        dx <<= 1;
        e = dx - dy;
        dy <<= 1;
        while (y1 != y2) {
            drawPoint(x1, y1, c);
            if(e >= 0) {
                x1 += inx;
                e -= dy;
            }
            e += dx; y1 += iny;
        }
    }
    drawPoint(x1, y1, c);
}

void Graphics::drawLineHeight(int x, int y, int height, Color c) {
	for (int i = 0; i < height; i++) {
		drawPoint(x, y + i, c);
	}
}

void Graphics::drawLineWidth(int x, int y, int width, Color c) {
	for (int i = 0; i < width; i++) {
		drawPoint(x + i, y, c);
	}
}

void Graphics::drawSquare(int x, int y, int w, int h, Color c) {
	drawLineWidth(x, y, w, c);
	drawLineHeight(x, y, h, c);
	drawLineWidth(x, y + h, w, c);
	drawLineHeight(x + w, y, h, c);
}

void Graphics::fillSquare(int x, int y, int w, int h, Color c) {
	for (int i = 0; i < h; i++) {
		drawLineWidth(x, y + i, w, c);
	}
}

void Graphics::drawCircle(int x0, int y0, int radius, Color c) {
	int x = radius, y = 0;
	int radiusError = 1 - x;

	while(x >= y) {
		drawPoint(x + x0, y + y0, c);
		drawPoint(-x + x0, y + y0, c);
		
		drawPoint(y + x0, x + y0, c);
		drawPoint(-y + x0, x + y0, c);
		
		drawPoint(-x + x0, -y + y0, c);
		drawPoint(x + x0, -y + y0, c);
		
		drawPoint(-y + x0, -x + y0, c);
		drawPoint(y + x0, -x + y0, c);
		
		y++;
		if (radiusError < 0) {
			radiusError += 2 * y + 1;
		} else {
			x--;
			radiusError += 2 * (y - x + 1);
		}
	}
}

void Graphics::fillCircle(int x0, int y0, int radius, Color c) {
	int x = radius, y = 0;
	int radiusError = 1 - x;

	while(x >= y) {
		drawLine(x + x0, y + y0, -x + x0, y + y0, c);
		drawLine(y + x0, x + y0, -y + x0, x + y0, c);
		drawLine(-x + x0, -y + y0, x + x0, -y + y0, c);
		drawLine(-y + x0, -x + y0, y + x0, -x + y0, c);
		y++;
		if (radiusError < 0) {
			radiusError += 2 * y + 1;
		} else {
			x--;
			radiusError += 2 * (y - x + 1);
		}
	}
}

void Graphics::fillPolygon(Polygon p, Color c) {
	int  nodes, pixelY, i, j, swap;
	int nodeX[10];
	for (pixelY = p.top; pixelY < p.bottom; pixelY++) {
		nodes = 0;
		j= p.corners - 1;
		for (i = 0; i < p.corners; i++) {
			if ((p.GetY(i) < (double) pixelY && p.GetY(j) >= (double) pixelY) ||  (p.GetY(j) < (double) pixelY && p.GetY(i) >= (double) pixelY)) {
				nodeX[nodes++] = (int) (p.GetX(i) + (pixelY - p.GetY(i)) / (p.GetY(j) - p.GetY(i)) * (p.GetX(j) - p.GetX(i)));
			}
			j = i;
		}
		i = 0;
		while (i < nodes - 1) {
			if (nodeX[i]>nodeX[i+1]) {
				swap = nodeX[i];
				nodeX[i] = nodeX[i + 1];
				nodeX[i + 1] = swap;
				if (i) i--;
			} else {
				i++;
			}
		}
		for (i = 0; i < nodes; i += 2) {
			if   (nodeX[i  ] >= p.right) break;
			if   (nodeX[i+1] > p.left) {
				if (nodeX[i    ] < p.left ) nodeX[i    ] = p.left;
				if (nodeX[i + 1] > p.right) nodeX[i + 1] = p.right;
				for (j = nodeX[i]; j < nodeX[i + 1]; j++) {
					drawPoint(j, pixelY, c);
				}
			}
		}
	}
}

#endif
