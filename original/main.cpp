// Written by Mark Bayazit (darkzerox)
// March 23, 2009

#include <GL/glfw.h>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <FTGL/ftgl.h>
#include <limits>
#include "Point.h"

typedef vector<Point> Polygon;

int winWidth = 800, winHeight = 600;
Polygon incPoly;
int mouse_x, mouse_y;
bool polyComplete = false;
vector<Polygon> polys;
vector<Point> steinerPoints, reflexVertices;

bool isReflex(const Polygon &p, const int &i);
void makeCCW(Polygon &poly);
void initGraphics();
void decomposePoly(Polygon poly);
void GLFWCALL mouseButton(int button, int action);
void GLFWCALL mousePos(int x, int y);
void GLFWCALL keyPress(int key, int action);
void glColor1i(int c, GLubyte a = 0xff);

int main(int argc, char** argv) {
    initGraphics();
    FTTextureFont font = "FreeSans.ttf";
    font.FaceSize(20);
    char buf[80];
    srand((unsigned) time(0));
    int colors[] = {0xff0000, 0x00ff00, 0x0000ff, 0xffff00, 0xff00ff, 0x00ffff, 0xff8800};
    int nColors = sizeof (colors) / sizeof (int);

    while (1) {
        if (glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS || !glfwGetWindowParam(GLFW_OPENED))
            break;

        // render
        glClear(GL_COLOR_BUFFER_BIT);

        if (!polyComplete) {
            if (incPoly.size() > 0) {
                glColor3f(1, 1, 1);
                glPointSize(12);
                glLineWidth(3);

                glBegin(GL_LINE_STRIP);
                for (int i = 0; i < incPoly.size(); ++i) {
                    glVertex2f(incPoly[i].x, incPoly[i].y);
                }
                glEnd();

                glBegin(GL_POINTS);
                for (int i = 0; i < incPoly.size(); ++i) {
                    glVertex2f(incPoly[i].x, incPoly[i].y);
                }
                glEnd();

                // line to cursor
                glLineWidth(1.5);
                glColor4f(1, 1, 1, .5);
                glBegin(GL_LINES);
                glVertex2f(incPoly.back().x, incPoly.back().y);
                glVertex2f(mouse_x, mouse_y);
                glEnd();

            }
        } else {

            // colored polygons
            for (int i = 0; i < polys.size(); ++i) {
                glColor1i(colors[i % nColors], 64);
                glBegin(GL_POLYGON);
                for (int j = 0; j < polys[i].size(); ++j) {
                    glVertex2f(polys[i][j].x, polys[i][j].y);
                }
                glEnd();


            }

            // polygon outlines (thin)
            for (int i = 0; i < polys.size(); ++i) {
                glColor3f(1, 1, 1);
                glLineWidth(1.5);
                glBegin(GL_LINE_LOOP);
                for (int j = 0; j < polys[i].size(); ++j) {
                    glVertex2f(polys[i][j].x, polys[i][j].y);
                }
                glEnd();
            }


            // original polygon and points
            glColor3f(1, 1, 1);
            glPointSize(12);
            glLineWidth(3);

            glBegin(GL_LINE_LOOP);
            for (int i = 0; i < incPoly.size(); ++i) {
                glVertex2f(incPoly[i].x, incPoly[i].y);
            }
            glEnd();

            glBegin(GL_POINTS);
            for (int i = 0; i < incPoly.size(); ++i) {
                glVertex2f(incPoly[i].x, incPoly[i].y);
            }
            glEnd();

            // reflex vertices
            glColor3f(0, 0, 1);
            glPointSize(8);
            glBegin(GL_POINTS);
            for (int i = 0; i < reflexVertices.size(); ++i) {
                glVertex2f(reflexVertices[i].x, reflexVertices[i].y);
            }
            glEnd();

            // steiner points
            glColor3f(.8, 0, 0);
            glPointSize(8);
            glBegin(GL_POINTS);
            for (int i = 0; i < steinerPoints.size(); ++i) {
                glVertex2f(steinerPoints[i].x, steinerPoints[i].y);
            }
            glEnd();
        }


        for (int i = 0; i < incPoly.size(); ++i) {
            sprintf(buf, "%d", i);
            glColor3f(0, 0, 0);
            font.Render(buf, -1, FTPoint(incPoly[i].x + 6, incPoly[i].y + 4));
            glColor3f(1, 1, 0);
            font.Render(buf, -1, FTPoint(incPoly[i].x + 5, incPoly[i].y + 5));
        }

        glColor3f(1, 1, 1);
        sprintf(buf, "Mouse: (%d, %d)", mouse_x, mouse_y);
        font.Render(buf, -1, FTPoint(5, 5));

        glfwSwapBuffers();
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}

void initGraphics() {
    GLFWvidmode dvm; // desktop video mode

    // initialize glfw
    glfwInit();
    glfwGetDesktopMode(&dvm);
    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
    glfwOpenWindow(winWidth, winHeight, dvm.RedBits, dvm.GreenBits, dvm.BlueBits, 0, 0, 0, GLFW_WINDOW);
    glfwGetWindowSize(&winWidth, &winHeight); // created window may be different than requested size
    glfwSetWindowPos((dvm.Width - winWidth) / 2, (dvm.Height - winHeight) / 2); // center the window
    glfwSetWindowTitle("Convex Decomposition");
    glfwEnable(GLFW_STICKY_KEYS);
    glfwDisable(GLFW_KEY_REPEAT);
    glfwSetMouseButtonCallback(mouseButton);
    glfwSetMousePosCallback(mousePos);
    glfwSetKeyCallback(keyPress);

    // initialize opengl
    glViewport(0, 0, winWidth, winHeight);
    glLineWidth(1.5);
    glPointSize(5);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, winWidth, 0, winHeight); // set origin to bottom left
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0, 0, 0, 0);
    glColor3f(1, 1, 1);
}

void GLFWCALL mouseButton(int button, int action) {
    if (action != GLFW_PRESS)
        return;
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            if (incPoly.size() < 3 || sqdist(Point(mouse_x, mouse_y), incPoly[0]) > 100) {
                if (!polyComplete) incPoly.push_back(Point(mouse_x, mouse_y));
                break;
            }
        case GLFW_MOUSE_BUTTON_RIGHT:
            polyComplete = true;
            makeCCW(incPoly);
            decomposePoly(incPoly);
            break;
    }
}

void GLFWCALL keyPress(int key, int action) {
    if (action != GLFW_PRESS)
        return;
    switch (key) {
        case 'C':
            incPoly.clear();
            polys.clear();
            polyComplete = false;
            steinerPoints.clear();
            reflexVertices.clear();
            printf("---\n");
            break;
    }
}

void GLFWCALL mousePos(int x, int y) {
    mouse_x = x;
    mouse_y = winHeight - y;
}

void glColor1i(int c, GLubyte a) {
    glColor4ub((c >> 16) & 0xff, (c >> 8) & 0xff, c & 0xff, a);
}

void makeCCW(Polygon &poly) {
    int br = 0;

    // find bottom right point
    for (int i = 1; i < poly.size(); ++i) {
        if (poly[i].y < poly[br].y || (poly[i].y == poly[br].y && poly[i].x > poly[br].x)) {
            br = i;
        }
    }

    // reverse poly if clockwise
    if (!left(at(poly, br - 1), at(poly, br), at(poly, br + 1))) {
        reverse(poly.begin(), poly.end());
    }
}

bool isReflex(const Polygon &poly, const int &i) {
    return right(at(poly, i - 1), at(poly, i), at(poly, i + 1));
}

Point intersection(const Point &p1, const Point &p2, const Point &q1, const Point &q2) {
    Point i;
    Scalar a1, b1, c1, a2, b2, c2, det;
    a1 = p2.y - p1.y;
    b1 = p1.x - p2.x;
    c1 = a1 * p1.x + b1 * p1.y;
    a2 = q2.y - q1.y;
    b2 = q1.x - q2.x;
    c2 = a2 * q1.x + b2 * q1.y;
    det = a1 * b2 - a2*b1;
    if (!eq(det, 0)) { // lines are not parallel
        i.x = (b2 * c1 - b1 * c2) / det;
        i.y = (a1 * c2 - a2 * c1) / det;
    }
    return i;
}

void swap(int &a, int &b) {
    int c;
    c = a;
    a = b;
    b = c;
}

void decomposePoly(Polygon poly) {
    Point upperInt, lowerInt, p, closestVert;
    Scalar upperDist, lowerDist, d, closestDist;
    int upperIndex, lowerIndex, closestIndex;
    Polygon lowerPoly, upperPoly;

    for (int i = 0; i < poly.size(); ++i) {
        if (isReflex(poly, i)) {
            reflexVertices.push_back(poly[i]);
            upperDist = lowerDist = numeric_limits<Scalar>::max();
            for (int j = 0; j < poly.size(); ++j) {
                if (left(at(poly, i - 1), at(poly, i), at(poly, j))
                        && rightOn(at(poly, i - 1), at(poly, i), at(poly, j - 1))) { // if line intersects with an edge
                    p = intersection(at(poly, i - 1), at(poly, i), at(poly, j), at(poly, j - 1)); // find the point of intersection
                    if (right(at(poly, i + 1), at(poly, i), p)) { // make sure it's inside the poly
                        d = sqdist(poly[i], p);
                        if (d < lowerDist) { // keep only the closest intersection
                            lowerDist = d;
                            lowerInt = p;
                            lowerIndex = j;
                        }
                    }
                }
                if (left(at(poly, i + 1), at(poly, i), at(poly, j + 1))
                        && rightOn(at(poly, i + 1), at(poly, i), at(poly, j))) {
                    p = intersection(at(poly, i + 1), at(poly, i), at(poly, j), at(poly, j + 1));
                    if (left(at(poly, i - 1), at(poly, i), p)) {
                        d = sqdist(poly[i], p);
                        if (d < upperDist) {
                            upperDist = d;
                            upperInt = p;
                            upperIndex = j;
                        }
                    }
                }
            }

            // if there are no vertices to connect to, choose a point in the middle
            if (lowerIndex == (upperIndex + 1) % poly.size()) {
                printf("Case 1: Vertex(%d), lowerIndex(%d), upperIndex(%d), poly.size(%d)\n", i, lowerIndex, upperIndex, (int) poly.size());
                p.x = (lowerInt.x + upperInt.x) / 2;
                p.y = (lowerInt.y + upperInt.y) / 2;
                steinerPoints.push_back(p);

                if (i < upperIndex) {
                    lowerPoly.insert(lowerPoly.end(), poly.begin() + i, poly.begin() + upperIndex + 1);
                    lowerPoly.push_back(p);
                    upperPoly.push_back(p);
                    if (lowerIndex != 0) upperPoly.insert(upperPoly.end(), poly.begin() + lowerIndex, poly.end());
                    upperPoly.insert(upperPoly.end(), poly.begin(), poly.begin() + i + 1);
                } else {
                    if (i != 0) lowerPoly.insert(lowerPoly.end(), poly.begin() + i, poly.end());
                    lowerPoly.insert(lowerPoly.end(), poly.begin(), poly.begin() + upperIndex + 1);
                    lowerPoly.push_back(p);
                    upperPoly.push_back(p);
                    upperPoly.insert(upperPoly.end(), poly.begin() + lowerIndex, poly.begin() + i + 1);
                }
            } else {
                // connect to the closest point within the triangle
                printf("Case 2: Vertex(%d), closestIndex(%d), poly.size(%d)\n", i, closestIndex, (int) poly.size());

                if (lowerIndex > upperIndex) {
                    upperIndex += poly.size();
                }
                closestDist = numeric_limits<Scalar>::max();
                for (int j = lowerIndex; j <= upperIndex; ++j) {
                    if (leftOn(at(poly, i - 1), at(poly, i), at(poly, j))
                            && rightOn(at(poly, i + 1), at(poly, i), at(poly, j))) {
                        d = sqdist(at(poly, i), at(poly, j));
                        if (d < closestDist) {
                            closestDist = d;
                            closestVert = at(poly, j);
                            closestIndex = j % poly.size();
                        }
                    }
                }

                if (i < closestIndex) {
                    lowerPoly.insert(lowerPoly.end(), poly.begin() + i, poly.begin() + closestIndex + 1);
                    if (closestIndex != 0) upperPoly.insert(upperPoly.end(), poly.begin() + closestIndex, poly.end());
                    upperPoly.insert(upperPoly.end(), poly.begin(), poly.begin() + i + 1);
                } else {
                    if (i != 0) lowerPoly.insert(lowerPoly.end(), poly.begin() + i, poly.end());
                    lowerPoly.insert(lowerPoly.end(), poly.begin(), poly.begin() + closestIndex + 1);
                    upperPoly.insert(upperPoly.end(), poly.begin() + closestIndex, poly.begin() + i + 1);
                }
            }

            // solve smallest poly first
            if (lowerPoly.size() < upperPoly.size()) {
                decomposePoly(lowerPoly);
                decomposePoly(upperPoly);
            } else {
                decomposePoly(upperPoly);
                decomposePoly(lowerPoly);
            }
            return;
        }
    }
    polys.push_back(poly);
}
