1.8'' TFT LCD Screen ST7735R
============================

This is a library for 1.8'' TFT LCD screen for Texas Instruments C2000
Piccolo F28027 LaunchPad.

WARNING: library is not tested yet and some functions are not written!

List of functions and description:
----------------------------------

-   *color(int red, int green, int blue)*: returns a combined value for
    RGB 16 bit color mode
-   *fillRect(int x, int y, int w, int h, int color)*: draws and fills
    rectangle with width *w* and height *h* with color *color*. Its
    starting point is top left corner of rectangle with coordinates *x*
    and *y*.
-   *fillScreen(int color)*: sets backgroud color
-   *drawPixel(int x, int y, int color)*: sets color of pixel on
    coordinates *x* and *y*.
-   *drawArrow(char C, int x, int y, int b, int col, int size)*: draws
    an arrow with color *col* on a backgroud with color *b*. *x* and *y*
    are coordinates of top left corner of box around the arrow. With
    character *C* you choose in which way the arrow points. *'U'* is for
    up, *'D'* down, *'R'* right and *'L'* left. Mind the capital
    letters!
-   *writeWord(String str, int x, int y, int b, int col, int size)*:
    writes word *str* with color *col* in a background with color *b*.
    Coordinates *x* and *y* are top left corner of box around the text.
-   *drawHeart(int x, int y, int b, int col, int size)*: draws heart.
    Other parameters are the same as in commands above.
-   *drawCircle(int x, int y, int r, int e, int rot)*: draws ellipse
    with center in point defined with *x* and *y*. *r* is radius or
    length of major semi-axis and *e* is eccentricity (for circle:
    *e=0*). You can rotate ellipse for angle *rot* (in degrees) from
    x-axis.
-   *fillCircle(int x, int y, int r, int e, int rot, int color)*: same
    as drawCircle, but filled with color *color*.
-   *drawArc(int x, int y, int r, int e, int rot, int s, int f)*: same
    as drawCircle, but you define starting point *s* as angle from
    rotated x-axis and stopping point *f* as angle from rotated x-axis.
    Angles should be in degrees.
-   *fillArc(int x, int y, int r, int e, int rot, int s, int f, int
    color)*: same as drawArc, but filled with color *color*.

Images of used screen:
----------------------

![image](http://i.imgsafe.org/e1daa8ed51.jpg)

![image](http://i.imgsafe.org/e1daa1033c.jpg)

![image](http://i.imgsafe.org/e1daa4aa8f.jpg)
