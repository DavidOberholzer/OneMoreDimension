#include <iostream>
#include "worldmath.h"
#include "data_io.h"
#include "structures.h"

using namespace std;

#define MAX_VERTICES 12

int dotProduct(int x1, int y1, int z1, int x2, int y2, int z2)
{
    return x1 * x2 + y1 * y2 + z1 * z2;
}

bool triangleOrientation(Point3D points[3])
{
    float x1 = points[2].getX() - points[0].getX();
    float y1 = points[2].getY() - points[0].getY();
    float x2 = points[1].getX() - points[0].getX();
    float y2 = points[1].getY() - points[0].getY();
    return ((x1 * y2 - x2 * y1) >= 0);
}

void drawScanLine(Gradients gradients, Edge left, Edge right, int y, float zBuffer[WIDTH * HEIGHT], int tex)
{
    struct texture *texture;
    if (tex >= 0)
    {
        texture = &textures[tex - 1];
    }
    int xMin = (int)ceil(left.x);
    int xMax = (int)ceil(right.x);
    float xPrestep = xMin - left.x;
    float u = left.u + gradients.uXStep * xPrestep;
    float v = left.v + gradients.vXStep * xPrestep;
    float ooz = left.ooz + gradients.oozXStep * xPrestep;
    float zMin = left.z;
    float zMax = right.z;

    for (int x = xMin; x < xMax; x++)
    {
        float t = (x - xMin) / (float)(xMax - xMin);

        float z = zMin * (1 - t) + zMax * t;
        int index = y * WIDTH + x;
        if (zBuffer[index] > z)
        {
            zBuffer[index] = z;
            if (tex >= 0)
            {
                float wz = 1 / ooz;
                int xPix = (u * wz) * (texture->width - 1);
                int yPix = (v * wz) * (texture->height - 1);
                int tindex = (yPix * texture->width) + xPix;
                graphicsDrawPoint(x, y, texture->pixels[tindex].R, texture->pixels[tindex].G, texture->pixels[tindex].B);
            }
        }
        u += gradients.uXStep;
        v += gradients.vXStep;
        ooz += gradients.oozXStep;
    }
}

void scanEdge(Gradients gradients, Edge *A, Edge *B, bool orientation, float zBuffer[WIDTH * HEIGHT], int tex)
{
    Edge *left = A;
    Edge *right = B;
    if (orientation)
    {
        swap(left, right);
    }
    for (int y = B->yStart; y < B->yEnd; y++)
    {
        if (y > 0 || y < HEIGHT)
            drawScanLine(gradients, *left, *right, y, zBuffer, tex);
        left->step();
        right->step();
    }
}

void fillTriangle(Point3D points[3], Point3D texels[3], float zBuffer[WIDTH * HEIGHT], int tex)
{
    Gradients gradients = Gradients(points, texels);
    Edge *topToBottom = new Edge(gradients, points[0], points[2], texels[0]);
    Edge *topToMiddle = new Edge(gradients, points[0], points[1], texels[0]);
    Edge *middleToBottom = new Edge(gradients, points[1], points[2], texels[1]);

    bool orientation = triangleOrientation(points);
    scanEdge(gradients, topToBottom, topToMiddle, orientation, zBuffer, tex);
    scanEdge(gradients, topToBottom, middleToBottom, orientation, zBuffer, tex);
    delete topToBottom;
    delete topToMiddle;
    delete middleToBottom;
}

void sortTriangle(Point3D nPoints[3], Point3D texels[3], float zBuffer[WIDTH * HEIGHT], int tex)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = i; j < 3; j++)
        {
            if (nPoints[j].getY() < nPoints[i].getY())
            {
                swap(nPoints[j], nPoints[i]);
                swap(texels[j], texels[i]);
            }
        }
    }
    fillTriangle(nPoints, texels, zBuffer, tex);
}

void createTriangles(Point3D nPoints[MAX_VERTICES], Point3D texels[MAX_VERTICES], int count, float zBuffer[WIDTH * HEIGHT], int tex)
{
    float x, y, z;
    for (int i = 0; i < count; i++)
    {
        x = ((nPoints[i].getX() / nPoints[i].getW()) + 1) * WIDTHD2;
        y = ((nPoints[i].getY() / nPoints[i].getW()) + 1) * HEIGHTD2;
        z = nPoints[i].getZ() / nPoints[i].getW();
        nPoints[i].setX(x);
        nPoints[i].setY(y);
        nPoints[i].setZ(z);
    }
    Point3D start = nPoints[0];
    Point3D startTex = texels[0];
    for (int i = 1; i < count - 1; i++)
    {
        cout << "count: " << count << endl;
        cout << "i: " << i << endl;
        Point3D group[3] = {
            Point3D(start.getX(), start.getY(), start.getZ(), start.getW()),
            Point3D(nPoints[i].getX(), nPoints[i].getY(), nPoints[i].getZ(), nPoints[i].getW()),
            Point3D(nPoints[i + 1].getX(), nPoints[i + 1].getY(), nPoints[i + 1].getZ(), nPoints[i + 1].getW())};
        Point3D texGroup[3] = {
            Point3D(startTex.getX(), startTex.getY(), startTex.getZ(), startTex.getW()),
            Point3D(texels[i].getX(), texels[i].getY(), texels[i].getZ(), texels[i].getW()),
            Point3D(texels[i + 1].getX(), texels[i + 1].getY(), texels[i + 1].getZ(), texels[i + 1].getW())};
        sortTriangle(group, texGroup, zBuffer, tex);
    }
}

int clipPointComponent(Point3D vertices[MAX_VERTICES], Point3D texels[MAX_VERTICES], int count, int component, float componentFactor, Point3D results[MAX_VERTICES], Point3D texelResults[MAX_VERTICES])
{
    int resultCount = 0;
    Point3D previousPoint = vertices[count - 1];
    Point3D previousTexel = texels[count - 1];
    Point3D *p, *tp;
    float previousComponent = previousPoint.getComponent(component) * componentFactor;
    bool previousNotClipped = previousComponent <= previousPoint.getW();
    for (int i = 0; i < count; i++)
    {
        bool added = false;
        Point3D currentPoint = vertices[i];
        Point3D currentTexel = texels[i];
        float currentComponent = currentPoint.getComponent(component);
        bool currentNotClipped = (currentComponent <= currentPoint.getW());
        if (currentNotClipped ^ previousNotClipped)
        {
            added = true;
            float ratio = (previousPoint.getW() - previousComponent) /
                          ((previousPoint.getW() - previousComponent) -
                           (currentPoint.getW() - currentComponent));
            resultCount++;
            if (resultCount > MAX_VERTICES)
            {
                cout << count << endl;
                cout << "Too many vertices!" << endl;
                exit(1);
            }
            p = &results[resultCount - 1];
            Point3D newPoint = lerpPoint(currentPoint, previousPoint, ratio);
            p->setX(newPoint.getX());
            p->setY(newPoint.getY());
            p->setZ(newPoint.getZ());
            p->setW(newPoint.getW());

            tp = &texelResults[resultCount - 1];
            newPoint = lerpPoint(currentTexel, previousTexel, ratio);
            tp->setX(newPoint.getX());
            tp->setY(newPoint.getY());
        }

        if (currentNotClipped)
        {
            resultCount++;
            if (resultCount > MAX_VERTICES)
            {
                cout << count << endl;
                cout << "Too many vertices!" << endl;
                exit(1);
            }
            p = &results[resultCount - 1];
            p->setX(currentPoint.getX());
            p->setY(currentPoint.getY());
            p->setZ(currentPoint.getZ());
            p->setW(currentPoint.getW());

            tp = &texelResults[resultCount - 1];
            tp->setX(currentTexel.getX());
            tp->setY(currentTexel.getY());
        }
        previousPoint = currentPoint;
        previousTexel = currentTexel;
        previousComponent = currentComponent;
        previousNotClipped = currentNotClipped;
    }
    return resultCount;
}

int clipPointAxis(Point3D vertices[MAX_VERTICES], Point3D texels[MAX_VERTICES], int component, int nVertices)
{
    Point3D auxVertices[MAX_VERTICES];
    Point3D auxTexels[MAX_VERTICES];
    int count1 = clipPointComponent(vertices, texels, nVertices, component, 1.0, auxVertices, auxTexels);

    if (count1 == 0)
    {
        return 0;
    }

    int count2 = clipPointComponent(auxVertices, auxTexels, count1, component, -1.0, vertices, texels);
    return count2;
}

void drawTriangle(int cIndex, Matrix *P, Matrix *V, Point3D points[3], float u[3], float v[3], int tex, float zBuffer[WIDTH * HEIGHT])
{
    float wz[3];
    Matrix projectedPoints[3];
    Point3D nPoints[3];
    Point3D pPoints[MAX_VERTICES];
    Point3D texels[MAX_VERTICES];
    bool outside[3] = {0, 0, 0};
    float x, y, z;
    for (int i = 0; i < 3; i++)
    {
        Point3D p = V->xPoint(points[i]);
        projectedPoints[i] = P->projectPoint(p, false);
        pPoints[i].setX(projectedPoints[i].getValue(0));
        pPoints[i].setY(projectedPoints[i].getValue(4));
        pPoints[i].setZ(projectedPoints[i].getValue(8));
        pPoints[i].setW(projectedPoints[i].getValue(12));
        bool xOut = fabs(pPoints[i].getX()) >= fabs(pPoints[i].getW());
        bool yOut = fabs(pPoints[i].getY()) >= fabs(pPoints[i].getW());
        bool zOut = fabs(pPoints[i].getZ()) >= fabs(pPoints[i].getW());
        if (xOut || yOut || zOut)
            outside[i] = true;
        texels[i].setX(u[i]);
        texels[i].setY(v[i]);
    }
    if (outside[0] && outside[1] && outside[2])
        return;
    int vCount = clipPointAxis(pPoints, texels, 0, 3);
    vCount = clipPointAxis(pPoints, texels, 1, vCount);
    vCount = clipPointAxis(pPoints, texels, 2, vCount);
    if (vCount > 2)
    {
        createTriangles(pPoints, texels, vCount, zBuffer, tex);
    }
}

Point3D lerpPoint(Point3D p2, Point3D p1, float t)
{
    float x = p2.getX() * t + p1.getX() * (1 - t);
    float y = p2.getY() * t + p1.getY() * (1 - t);
    float z = p2.getZ() * t + p1.getZ() * (1 - t);
    float w = p2.getW() * t + p1.getW() * (1 - t);
    return Point3D(x, y, z, w);
}

Matrix viewMatrix(Point3D U, Point3D R, Point3D D, float dx, float dy, float dz)
{
    float values[16];
    for (int i = 0; i < 16; i++)
        values[i] = 0;
    // Rotation
    values[0] = R.getX();
    values[1] = R.getY();
    values[2] = R.getZ();
    values[4] = U.getX();
    values[5] = U.getY();
    values[6] = U.getZ();
    values[8] = D.getX();
    values[9] = D.getY();
    values[10] = D.getZ();
    values[15] = 1;
    Matrix rotation = Matrix(values, 4);
    // Translation
    for (int i = 0; i < 16; i++)
        values[i] = 0;
    values[0] = 1;
    values[3] = -dx;
    values[5] = 1;
    values[7] = -dy;
    values[10] = 1;
    values[11] = -dz;
    values[15] = 1;
    Matrix translation = Matrix(values, 4);
    return rotation * translation;
}

Color::Color()
{
}

Color::Color(float R, float G, float B, float A)
{
    this->R = R;
    this->G = G;
    this->B = B;
    this->A = A;
}

Color Color::operator+(Color other)
{
    return Color(R + other.R, G + other.G, B + other.B, A + other.A);
}

Color Color::operator-(Color other)
{

    return Color(R - other.R, G - other.G, B - other.B, A - other.A);
}

Color Color::operator*(float value)
{
    return Color(R * value, G * value, B * value, A * value);
}

Point3D::Point3D()
{
    x = 0;
    y = 0;
    z = 0;
    w = 0;
}

Point3D::Point3D(float x, float y, float z, float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

void Point3D::setX(float x)
{
    this->x = x;
}

void Point3D::setY(float y)
{
    this->y = y;
}

void Point3D::setZ(float z)
{
    this->z = z;
}

void Point3D::setW(float w)
{
    this->w = w;
}

void Point3D::rotateQ(float pitch, float yaw, float angle)
{
    // Quarternion Rotation
    Quarternion q = Quarternion(pitch, yaw, angle);
    Point3D r = q * Point3D(this->x, this->y, this->z, 0);
    this->x = r.getX();
    this->y = r.getY();
    this->z = r.getZ();
}

void Point3D::rotateX(float angle)
{
    // Euler Theorem
    float oldY = this->y,
          oldZ = this->z;
    this->y = oldY * cos(angle) - oldZ * sin(angle);
    this->z = oldY * sin(angle) + oldZ * cos(angle);
}

void Point3D::rotateY(float angle)
{
    // Euler Theorem
    float oldX = this->x,
          oldZ = this->z;
    this->x = oldX * cos(angle) - oldZ * sin(angle);
    this->z = oldX * sin(angle) + oldZ * cos(angle);
}

void Point3D::rotateZ(float angle)
{
    // Euler Theorem
    float oldX = this->x,
          oldY = this->y;
    this->x = oldX * cos(angle) - oldY * sin(angle);
    this->y = oldX * sin(angle) + oldY * cos(angle);
}

void Point3D::translate(float x, float y, float z)
{
    this->x += x;
    this->y += y;
    this->z += z;
}

void Point3D::scale(float factor)
{
    this->x = this->x * factor;
    this->y = this->y * factor;
    this->z = this->z * factor;
}

float Point3D::dot(Point3D p)
{
    return this->x * p.getX() + this->y * p.getY() + this->z * p.getZ();
}

Point3D Point3D::scaledVector(float scale)
{
    return Point3D(this->x * scale, this->y * scale, this->z * scale, 0);
}

Point3D Point3D::cross(Point3D p)
{
    Point3D r;

    r.setX(this->y * p.getZ() - this->z * p.getY());
    r.setY(this->z * p.getX() - this->x * p.getZ());
    r.setZ(this->x * p.getY() - this->y * p.getX());

    return r;
}

float Point3D::getComponent(int index)
{
    switch (index)
    {
    case 0:
        return this->x;
        break;
    case 1:
        return this->y;
        break;
    case 2:
        return this->z;
        break;
    case 3:
        return this->w;
        break;
    }
}

void Point3D::print()
{
    printf("Point\n");
    printf("x: %.2f\ny: %.2f\nz: %.2f\n", this->x, this->y, this->z);
}

Gradients::Gradients(Point3D points[3], Point3D texels[3])
{
    float y02 = points[0].getY() - points[2].getY();
    float y12 = points[1].getY() - points[2].getY();
    float x02 = points[0].getX() - points[2].getX();
    float x12 = points[1].getX() - points[2].getX();

    float oodx = 1 / (x12 * y02 - x02 * y12);
    float oody = -oodx;

    // z gradient
    float c02 = (points[0].getZ()) - (points[2].getZ());
    float c12 = (points[1].getZ()) - (points[2].getZ());
    zXStep = (c12 * y02 - c02 * y12) * oodx;
    zYStep = (c12 * x02 - c02 * x12) * oody;

    // 1/z gradients
    float ooz0 = 1 / points[0].getW();
    float ooz1 = 1 / points[1].getW();
    float ooz2 = 1 / points[2].getW();

    c02 = ooz0 - ooz2;
    c12 = ooz1 - ooz2;
    oozXStep = (c12 * y02 - c02 * y12) * oodx;
    oozYStep = (c12 * x02 - c02 * x12) * oody;

    // u/z gradients
    c02 = (texels[0].getX() * ooz0) - (texels[2].getX() * ooz2);
    c12 = (texels[1].getX() * ooz1) - (texels[2].getX() * ooz2);
    uXStep = (c12 * y02 - c02 * y12) * oodx;
    uYStep = (c12 * x02 - c02 * x12) * oody;

    // v/z gradients
    c02 = (texels[0].getY() * ooz0) - (texels[2].getY() * ooz2);
    c12 = (texels[1].getY() * ooz1) - (texels[2].getY() * ooz2);
    vXStep = (c12 * y02 - c02 * y12) * oodx;
    vYStep = (c12 * x02 - c02 * x12) * oody;
}

Edge::Edge(Gradients gradients, Point3D minYPoint, Point3D maxYPoint, Point3D minYTexel)
{
    yStart = (int)ceil(minYPoint.getY());
    yEnd = (int)ceil(maxYPoint.getY());

    float dy = maxYPoint.getY() - minYPoint.getY();
    float dx = maxYPoint.getX() - minYPoint.getX();

    float yPrestep = yStart - minYPoint.getY();
    xStep = dx / dy;
    x = minYPoint.getX() + yPrestep * xStep;
    float xPrestep = x - minYPoint.getX();

    z = (minYPoint.getZ()) + gradients.zYStep * yPrestep + gradients.zXStep * xPrestep;
    zStep = gradients.zYStep + (gradients.zXStep * xStep);

    ooz = (1 / minYPoint.getW()) + gradients.oozYStep * yPrestep + gradients.oozXStep * xPrestep;
    oozStep = gradients.oozYStep + (gradients.oozXStep * xStep);

    u = (minYTexel.getX() / minYPoint.getW()) + gradients.uYStep * yPrestep + gradients.uXStep * xPrestep;
    uStep = gradients.uYStep + (gradients.uXStep * xStep);

    v = (minYTexel.getY() / minYPoint.getW()) + gradients.vYStep * yPrestep + gradients.vXStep * xPrestep;
    vStep = gradients.vYStep + (gradients.vXStep * xStep);
}

void Edge::step()
{
    x += xStep;
    z += zStep;
    ooz += oozStep;
    u += uStep;
    v += vStep;
}

Triangle3D::Triangle3D()
{
    this->points[0] = -1;
    this->points[1] = -1;
    this->points[2] = -1;
}

Triangle3D::~Triangle3D()
{
}

int Triangle3D::getPoint(int index)
{
    if (index < 0 || index > 2)
    {
        cout << "Triange point index out of range!" << endl;
        exit(1);
    }
    return this->points[index];
}

int Triangle3D::getP1()
{
    return this->points[0];
}

int Triangle3D::getP2()
{
    return this->points[1];
}

int Triangle3D::getP3()
{
    return this->points[2];
}

float Triangle3D::getUTexel(int index)
{
    if (index < 0 || index > 2)
    {
        cout << "U Texel Index OUT OF RANGE!" << endl;
        exit(1);
    }
    return this->uTexels[index];
}

float Triangle3D::getVTexel(int index)
{
    if (index < 0 || index > 2)
    {
        cout << "V Texel Index OUT OF RANGE!" << endl;
        exit(1);
    }
    return this->vTexels[index];
}

int Triangle3D::getTexture()
{
    return this->texture;
}

void Triangle3D::setP1(int p1)
{
    this->points[0] = p1;
}

void Triangle3D::setP2(int p2)
{
    this->points[1] = p2;
}

void Triangle3D::setP3(int p3)
{
    this->points[2] = p3;
}

void Triangle3D::setUTexel(int index, float value)
{
    if (index < 0 || index > 2)
    {
        cout << "U Texel Index OUT OF RANGE!" << endl;
        exit(1);
    }
    this->uTexels[index] = value;
}

void Triangle3D::setVTexel(int index, float value)
{
    if (index < 0 || index > 2)
    {
        cout << "V Texel Index OUT OF RANGE!" << endl;
        exit(1);
    }
    this->vTexels[index] = value;
}

void Triangle3D::setTexture(int index)
{
    this->texture = index;
}

void Matrix::clear()
{
    for (int i = 0; i < 16; i++)
    {
        this->values[i] = 0;
    }
}

void Matrix::set2D()
{
    this->onebyX = false;
    this->two = true;
    this->three = false;
    this->four = false;
}

void Matrix::set3D()
{
    this->two = false;
    this->three = true;
    this->four = false;
}

void Matrix::set4D()
{
    this->two = false;
    this->three = false;
    this->four = true;
}

void Matrix::setSize(int size)
{
    switch (size)
    {
    case 2:
        this->set2D();
        break;
    case 3:
        this->set3D();
        break;
    case 4:
        this->set4D();
        break;
    default:
        cout << "Invalid Matrix size (valid: 2, 3, 4)!" << endl;
        exit(1);
    }
}

float Matrix::getValue(int index)
{
    if (index < 0 || index > 15)
    {
        cout << "Invalid Matrix value index." << endl;
        exit(1);
    }
    return this->values[index];
}

Matrix::Matrix()
{
    this->set4D();
    this->clear();
}

Matrix::Matrix(float *values, int size)
{
    this->setSize(size);
    for (int i = 0; i < 16; i++)
    {
        this->values[i] = *(values + i);
    }
}

Matrix::Matrix(int near, int far, float fov, float aspect)
{
    this->set4D();
    float second = 1 / tan(fov / 2);
    float first = second * aspect;
    float diff = (float)(far - near);
    this->clear();
    this->values[0] = first;
    this->values[5] = second;
    this->values[10] = -(far + near) / diff;
    this->values[11] = -2 * far * near / diff;
    this->values[14] = -1;
}

Matrix::Matrix(Point3D *vertex)
{
    this->set4D();
    this->clear();
    this->values[0] = vertex->getX();
    this->values[5] = vertex->getY();
    this->values[10] = vertex->getZ();
    this->values[15] = 1;
}

int Matrix::getSize()
{
    if (this->two)
        return 2;
    if (this->three)
        return 3;
    if (this->four)
        return 4;
}

Point3D Matrix::xPoint(Point3D p)
{
    Point3D newPoint;
    newPoint.setX(this->values[0] * p.getX() + this->values[1] * p.getY() + this->values[2] * p.getZ() + this->values[3] * 1);
    newPoint.setY(this->values[4] * p.getX() + this->values[5] * p.getY() + this->values[6] * p.getZ() + this->values[7] * 1);
    newPoint.setZ(this->values[8] * p.getX() + this->values[9] * p.getY() + this->values[10] * p.getZ() + this->values[11] * 1);
    return newPoint;
}

Matrix Matrix::projectPoint(Point3D p, bool keepSize)
{
    Matrix newMatrix;
    if (keepSize)
    {
        newMatrix.values[0] = this->values[0] * p.getX() + this->values[1] * p.getY() + this->values[2] * p.getZ() + this->values[3] * 1;
        newMatrix.values[5] = this->values[4] * p.getX() + this->values[5] * p.getY() + this->values[6] * p.getZ() + this->values[7] * 1;
        newMatrix.values[10] = this->values[8] * p.getX() + this->values[9] * p.getY() + this->values[10] * p.getZ() + this->values[11] * 1;
        newMatrix.values[15] = this->values[12] * p.getX() + this->values[13] * p.getY() + this->values[14] * p.getZ() + this->values[15] * 1;
    }
    else
    {
        newMatrix.values[0] = this->values[0] * p.getX() + this->values[1] * p.getY() + this->values[2] * p.getZ() + this->values[3] * 1;
        newMatrix.values[4] = this->values[4] * p.getX() + this->values[5] * p.getY() + this->values[6] * p.getZ() + this->values[7] * 1;
        newMatrix.values[8] = this->values[8] * p.getX() + this->values[9] * p.getY() + this->values[10] * p.getZ() + this->values[11] * 1;
        newMatrix.values[12] = this->values[12] * p.getX() + this->values[13] * p.getY() + this->values[14] * p.getZ() + this->values[15] * 1;
    }
    return newMatrix;
}

Matrix Matrix::operator*(Matrix m2)
{
    if (this->getSize() != m2.getSize())
    {
        cout << "Matrix sizes differ! Cannot multiply." << endl;
        exit(1);
    }
    Matrix f = Matrix();
    f.values[0] = this->values[0] * m2.values[0] + this->values[1] * m2.values[4] + this->values[2] * m2.values[8] + this->values[3] * m2.values[12];
    f.values[1] = this->values[0] * m2.values[1] + this->values[1] * m2.values[5] + this->values[2] * m2.values[9] + this->values[3] * m2.values[13];
    f.values[2] = this->values[0] * m2.values[2] + this->values[1] * m2.values[6] + this->values[2] * m2.values[10] + this->values[3] * m2.values[14];
    f.values[3] = this->values[0] * m2.values[3] + this->values[1] * m2.values[7] + this->values[2] * m2.values[11] + this->values[3] * m2.values[15];

    f.values[4] = this->values[4] * m2.values[0] + this->values[5] * m2.values[4] + this->values[6] * m2.values[8] + this->values[7] * m2.values[12];
    f.values[5] = this->values[4] * m2.values[1] + this->values[5] * m2.values[5] + this->values[6] * m2.values[9] + this->values[7] * m2.values[13];
    f.values[6] = this->values[4] * m2.values[2] + this->values[5] * m2.values[6] + this->values[6] * m2.values[10] + this->values[7] * m2.values[14];
    f.values[7] = this->values[4] * m2.values[3] + this->values[5] * m2.values[7] + this->values[6] * m2.values[11] + this->values[7] * m2.values[15];

    f.values[8] = this->values[8] * m2.values[0] + this->values[9] * m2.values[4] + this->values[10] * m2.values[8] + this->values[11] * m2.values[12];
    f.values[9] = this->values[8] * m2.values[1] + this->values[9] * m2.values[5] + this->values[10] * m2.values[9] + this->values[11] * m2.values[13];
    f.values[10] = this->values[8] * m2.values[2] + this->values[9] * m2.values[6] + this->values[10] * m2.values[10] + this->values[11] * m2.values[14];
    f.values[11] = this->values[8] * m2.values[3] + this->values[9] * m2.values[7] + this->values[10] * m2.values[11] + this->values[11] * m2.values[15];

    f.values[12] = this->values[12] * m2.values[0] + this->values[13] * m2.values[4] + this->values[14] * m2.values[8] + this->values[15] * m2.values[12];
    f.values[13] = this->values[12] * m2.values[1] + this->values[13] * m2.values[5] + this->values[14] * m2.values[9] + this->values[15] * m2.values[13];
    f.values[14] = this->values[12] * m2.values[2] + this->values[13] * m2.values[6] + this->values[14] * m2.values[10] + this->values[15] * m2.values[14];
    f.values[15] = this->values[12] * m2.values[3] + this->values[13] * m2.values[7] + this->values[14] * m2.values[11] + this->values[15] * m2.values[15];

    f.setSize(this->getSize());
    return f;
}

Object::Object()
{
    this->name = (char *)"No Name";
    this->numVertices = 0;
    this->numTriangles = 0;
}

Object::Object(char *name)
{
    this->name = name;
    this->numVertices = 0;
    this->numTriangles = 0;
}

Object::~Object()
{
    delete this->vertices;
    delete this->triangles;
}

void Object::mallocVertices(Point3D *vertices)
{
    this->vertices = (Point3D *)malloc(this->numVertices * sizeof(*this->vertices));
    for (int i = 0; i < this->numVertices; i++)
    {
        this->vertices[i] = vertices[i];
    }
}

void Object::mallocTriangles(Triangle3D *triangles)
{
    this->triangles = (Triangle3D *)malloc(this->numTriangles * sizeof(*this->triangles));
    for (int i = 0; i < this->numTriangles; i++)
    {
        this->triangles[i] = triangles[i];
    }
}

void Object::drawObject(float pitch, float yaw, float rAngle, float dx, float dy, float dz, Matrix *P, Matrix *V, float zBuffer[WIDTH * HEIGHT])
{
    for (int i = 0; i < this->numTriangles; i++)
    {
        Triangle3D *t = &this->triangles[i];
        Point3D movedPoints[3];
        for (int j = 0; j < 3; j++)
        {
            Point3D p = this->vertices[t->getPoint(j) - 1];
            p.rotateQ(pitch, yaw, rAngle);
            p.translate(dx, dy, dz);
            movedPoints[j] = p;
        }
        drawTriangle(floor(i / 2.0), P, V, movedPoints, this->triangles[i].uTexels, this->triangles[i].vTexels, this->triangles[i].getTexture(), zBuffer);
    }
}

Quarternion::Quarternion()
{
    this->w = 0;
    this->x = 0;
    this->y = 0;
    this->z = 0;
}

Quarternion::Quarternion(Point3D p, float a)
{
    float angle = a / 2;
    float sAngle = sin(angle);
    this->w = cos(angle);
    this->x = p.getX() * sAngle;
    this->y = p.getY() * sAngle;
    this->z = p.getZ() * sAngle;
}

Quarternion::Quarternion(float pitch, float yaw, float a)
{
    float angle = a / 2;
    float sAngle = sin(angle);
    this->w = cos(angle);
    float h = cos(pitch);
    this->x = h * cos(yaw) * sAngle;
    this->y = sin(pitch) * sAngle;
    this->z = h * sin(yaw) * sAngle;
}

bool Quarternion::isValid()
{
    return !this->w ? false : true;
}

void Quarternion::setW(float w)
{
    this->w = w;
}

void Quarternion::setX(float x)
{
    this->x = x;
}

void Quarternion::setY(float y)
{
    this->y = y;
}

void Quarternion::setZ(float z)
{
    this->z = z;
}

Quarternion Quarternion::inverse()
{
    Quarternion result;
    result.setW(this->w);
    result.setX(-this->x);
    result.setY(-this->y);
    result.setZ(-this->z);
    return result;
}

Quarternion Quarternion::operator*(Quarternion q)
{
    Quarternion result;
    if (q.isValid() && this->isValid())
    {
        Point3D p1 = Point3D(this->x, this->y, this->z, 0);
        Point3D p2 = Point3D(q.getX(), q.getY(), q.getZ(), 0);
        Point3D cross = p1.cross(p2);

        result.setW(this->w * q.getW() + p1.dot(p2));
        result.setX(this->x * q.getW() + q.getX() * this->w + cross.getX());
        result.setY(this->y * q.getW() + q.getY() * this->w + cross.getY());
        result.setZ(this->z * q.getW() + q.getZ() * this->w + cross.getZ());
    }
    else if (q.isValid())
    {
        result.setW(q.getW());
        result.setX(q.getX());
        result.setY(q.getY());
        result.setZ(q.getZ());
    }
    else
    {
        result.setW(this->w);
        result.setX(this->x);
        result.setY(this->y);
        result.setZ(this->z);
    }

    return result;
}

Point3D Quarternion::operator*(Point3D p)
{
    Point3D result;
    // Only if a rotation angle is present.
    if (this->w)
    {
        Point3D pq = Point3D(this->x, this->y, this->z, 0);
        float dot1 = pq.dot(p);
        float dot2 = pq.dot(pq);
        Point3D cross = pq.cross(p);
        float w2 = this->w * this->w;
        result.setX(2.0 * dot1 * pq.getX() + (w2 - dot2) * p.getX() + 2.0 * this->w * cross.getX());
        result.setY(2.0 * dot1 * pq.getY() + (w2 - dot2) * p.getY() + 2.0 * this->w * cross.getY());
        result.setZ(2.0 * dot1 * pq.getZ() + (w2 - dot2) * p.getZ() + 2.0 * this->w * cross.getZ());
    }
    else
    {
        result = p;
    }
    return result;
}

void Quarternion::print()
{
    cout << "w: " << this->w << endl;
    cout << "x: " << this->x << endl;
    cout << "y: " << this->y << endl;
    cout << "z: " << this->z << endl;
}