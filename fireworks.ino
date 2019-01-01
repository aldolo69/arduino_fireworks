#include "LedControl.h"

LedControl lc = LedControl(12, 11, 10, 1);

class clDot
{
  public:
    int dx = 0; //ball slope
    int dy = 0;
    int x = 0;
    int y = 0;
    int eps = 0;
    int olddx = 9999;
    int olddy = 9999;
    int nextx;
    int nexty;

    void resetDot(int ix, int iy, int idx, int idy)
    {
      x = nextx = ix;
      y = nexty = iy;
      dx = idx;
      dy = idy;
      olddx = 9999;
      olddy = 9999;
      eps = 0;
    }
};




void displayPixel(int x, int y, boolean state)
{
  if (x < 0 || x > 7 || y < 0 || y > 7) return;
  lc.setLed(0, y, x, state);
}



/*
  how to use slope: 8 octane to apply Bresenham's line algorithm
  2 1
  3   0
  4   7
  5 6
  i need to move by 1 pixel, according to slope.
  whenever slope change the algorithm start from fresh
  if the slope stay constant then the algorithm goes on keeping the eps error running
*/

void move(int x, int y, int dx, int dy, int *eps, int *iNextX, int *iNextY, int addX, int addY)
{
  *eps += dy;
  (*iNextX) += addX;
  if ( ((*eps) << 1) >= dx )
  {
    (*iNextY) += addY;
    *eps -= dx;
  }
}



//compute next position 1 pixel away according to slope
void dotMove(clDot *d)//int * nextx,int * nexty)
{
  //slope according to dx/dy. which is the preset octane?
  char cSlope = 0;
  if (d->dx < 0)
  {
    //2,3,4,5
    if (d->dy < 0) //dx<0
    {
      //4,5
      if (d->dx < d->dy)
      {
        //4
        cSlope = 4;
      }
      else
      {
        //5
        cSlope = 5;
      }
    }
    else//dy>=0 dx<0
    {
      //2,3
      if ((- d->dx) > d->dy)
      {
        //3
        cSlope = 3;
      }
      else
      {
        //2
        cSlope = 2;
      }
    }
  }
  else //dx>=0
  {
    //0,1,7,6
    if (d->dy < 0) //dx>=0
    {
      //6,7
      if ((- d->dy ) > d->dx) //dx>=0
      {
        //6
        cSlope = 6;
      }
      else
      {
        //7
        cSlope = 7;
      }
    }
    else
    {
      //0,1
      if ( d->dy > d->dx) //dx>=0
      {
        //1
        cSlope = 1;
      }
      else
      {
        //0
        cSlope = 0;
      }
    }

  }


  /*
    for octane 0 it is possible to use:
    x,y=initial position
    dx,dy=delta
  */
  if (d->olddx != d->dx || d->olddy != d->dy)
  {
    //initialize algorithm
    d->olddx = d->dx;
    d->olddy = d->dy;
    d->eps = 0;
  }

  switch (cSlope)
  {
    case 0:
      //        move(x, y);
      move(d->x, d->y, d->dx, d->dy, &d->eps, &d->nextx, &d->nexty, 1, 1);
      break;
    case 1:
      //        move(y, x);
      move(d->y, d->x, d->dy, d->dx, &d->eps, &d->nexty, &d->nextx, 1, 1);
      break;
    case 2:
      //        move(y, -x);
      move(d->y, d->x, d->dy, -d->dx, &d->eps, &d->nexty, &d->nextx, 1, -1);
      break;
    case 3:
      //        move(-x, y);
      move(d->x, d->y, -d->dx, d->dy, &d->eps, &d->nextx, &d->nexty, -1, 1);
      break;
    case 4:
      //        move(-x, -y);
      move(d->x, d->y, -d->dx, -d->dy, &d->eps, &d->nextx, &d->nexty, -1, -1);
      break;
    case 5:
      //        move(-y, -x);
      move(d->y, d->x, -d->dy, -d->dx, &d->eps, &d->nexty, &d->nextx, -1, -1);
      break;
    case 6:
      //        move(-y, x);
      move(d->y, d->x, -d->dy, d->dx, &d->eps, &d->nexty, &d->nextx, -1, 1);
      break;
    case 7:
      //        move(x, -y);
      move(d->x, d->y, d->dx, -d->dy, &d->eps, &d->nextx, &d->nexty, 1, -1);
      break;
  }

}


void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 7);
  lc.clearDisplay(0);
}


#define NDOTS 6
void loop() {
  static clDot d[NDOTS];
  static char cStat = 0; //0 not started,1=fly,2=explosion
  static char clen;

  switch (cStat)
  {
    case 0:
      //choose starting point,direction,length
      lc.clearDisplay(0);
      clen = 5 + random(4);
      d[0].resetDot(2 + random(4), 8, -1 + random(3), -2);
      cStat = 1;
      break;
    case 1:

      //turnoff dot
      displayPixel(d[0].x, d[0].y , false);
      clen--;
      if (clen == 0)
      {
        //choose 4 point from the actual center, 4 direction and a len
        clen = 3 + random(2);

        for (int i = NDOTS - 1; i ; i--)
        {
          d[i].resetDot(d[0].x, d[0].y, -1 + random(3), -1 + random(3));
          displayPixel(d[i].x, d[i].y , true);
        }

        cStat = 2;
        break;
      }

      dotMove(&d[0]);
      d[0].x = d[0].nextx;
      d[0].y = d[0].nexty;
      displayPixel(d[0].x, d[0].y , true);
      break;

    case 2:
      for (int i = NDOTS - 1; i ; i--)
      {
        displayPixel(d[i].x, d[i].y , false);
      }

      clen--;

      if (clen == 0)
      {
        cStat = 0;
        break;
      }

      for (int i = NDOTS - 1; i ; i--)
      {
        dotMove(&d[i]);
        d[i].x = d[i].nextx;
        d[i].y = d[i].nexty;
        displayPixel(d[i].x, d[i].y , true);
      }

      break;
  }

  if (cStat == 2)
  {
    delay(100);
  } else {
    delay(200);
  }

}
