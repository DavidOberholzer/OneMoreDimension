#include <iostream>
#include "worldmath.h"

using namespace std;

bool pointInside(int x, int y, Point3D *A, Vector3D *j, Vector3D *k)
{
    Vector3D *i = new Vector3D(x, y, 0, A->getX(), A->getY(), 0);
    int ij = dotProduct(i->getVX(), i->getVY(), 0, j->getVX(), j->getVY(), 0);
    int ik = dotProduct(i->getVX(), i->getVY(), 0, k->getVX(), k->getVY(), 0);
    int jj = dotProduct(j->getVX(), j->getVY(), 0, j->getVX(), j->getVY(), 0);
    int jk = dotProduct(j->getVX(), j->getVY(), 0, k->getVX(), k->getVY(), 0);
    int kk = dotProduct(k->getVX(), k->getVY(), 0, k->getVX(), k->getVY(), 0);
    float denominator = (jk * jk) - (kk * jj);
    if (denominator == 0)
    {
        cout << "Denominator equals 0";
    }
    else
    {
        float u = -((ik * jk) - (ij * kk)) / denominator;
        float v = -((ij * jk) - (ik * jj)) / denominator;
        return u >= 0 && v >= 0 && (u + v) <= 1;
    }
}

int dotProduct(int x1, int y1, int z1, int x2, int y2, int z2)
{
    return x1 * x2 + y1 * y2 + z1 * z2;
}

int Point3D::counter = 0;

Point3D::Point3D()
{
    ID = counter++;
    x = 0;
    y = 0;
    z = 0;
}

Point3D::Point3D(int x, int y, int z)
{
    ID = counter++;
    this->x = x;
    this->y = y;
    this->z = z;
}

void Point3D::setX(int x)
{
    this->x = x;
}

void Point3D::setY(int y)
{
    this->y = y;
}

void Point3D::setZ(int z)
{
    this->z = z;
}

int Vector3D::counter = 0;

Vector3D::Vector3D()
{
    ID = counter++;
    p1 = new Point3D;
    p2 = new Point3D;
    vx = p2->getX() - p1->getX();
    vy = p2->getY() - p1->getY();
    vz = p2->getZ() - p1->getZ();
}

Vector3D::Vector3D(int x1, int y1, int z1, int x2, int y2, int z2)
{
    ID = counter++;
    p1 = new Point3D(x1, y1, z1);
    p2 = new Point3D(x2, y2, z2);
    vx = p2->getX() - p1->getX();
    vy = p2->getY() - p1->getY();
    vz = p2->getZ() - p1->getZ();
}

Vector3D::~Vector3D()
{
    delete p1;
    delete p2;
}

void Vector3D::setP1(int x, int y, int z)
{
    p1->setX(x);
    p1->setY(y);
    p1->setZ(z);
    vx = p2->getX() - p1->getX();
    vy = p2->getY() - p1->getY();
    vz = p2->getZ() - p1->getZ();
}

void Vector3D::setP2(int x, int y, int z)
{
    p2->setX(x);
    p2->setY(y);
    p2->setZ(z);
    vx = p2->getX() - p1->getX();
    vy = p2->getY() - p1->getY();
    vz = p2->getZ() - p1->getZ();
}

void Vector3D::draw(Color color)
{
    Point3D *camera = new Point3D(0, 0, 1);
    int x1 = ((p1->getX() - camera->getX()) / (p1->getZ() - camera->getZ())) + WIDTH / 2;
    int y1 = ((p1->getY() - camera->getY()) / (p1->getZ() - camera->getZ())) + HEIGHT / 2;
    int x2 = ((p2->getX() - camera->getX()) / (p2->getZ() - camera->getZ())) + WIDTH / 2;
    int y2 = ((p2->getY() - camera->getY()) / (p2->getZ() - camera->getZ())) + HEIGHT / 2;
    delete camera;
    graphicsDrawLine(x1, y1, x2, y2, color);
}

int Vector3D::operator*(Vector3D *v2)
{
    return dotProduct(this->getVX(), this->getVY(), this->getVZ(), v2->getVX(), v2->getVY(), v2->getVZ());
}

Triangle3D::Triangle3D(Point3D *p1, Point3D *p2, Point3D *p3)
{
    if (p1->getY() > p2->getY())
        swap(p2, p1);
    if (p1->getY() > p3->getY())
        swap(p3, p1);
    if (p2->getY() > p3->getY())
        swap(p3, p2);
    lines->setP1(p1->getX(), p1->getY(), p1->getZ());
    lines->setP2(p2->getX(), p2->getY(), p2->getZ());
    (lines + 1)->setP1(p2->getX(), p2->getY(), p2->getZ());
    (lines + 1)->setP2(p3->getX(), p3->getY(), p3->getZ());
    (lines + 2)->setP1(p3->getX(), p3->getY(), p3->getZ());
    (lines + 2)->setP2(p1->getX(), p1->getY(), p1->getZ());
}

Triangle3D::~Triangle3D()
{
    delete[] lines;
}

Vector3D *Triangle3D::getVector(int index)
{
    if (index < 3 && index >= 0)
    {
        return lines + index;
    }
    cout << "Index out of triange side range!" << endl;
}

bool Triangle3D::pointInTriangle(int x, int y)
{
    Point3D *p1 = lines[0].getP1();
    Vector3D *j = new Vector3D(
        lines[2].getP2()->getX(), lines[2].getP2()->getY(), lines[2].getP2()->getZ(),
        lines[2].getP1()->getX(), lines[2].getP1()->getY(), lines[2].getP1()->getZ());
    return pointInside(x, y, p1, j, lines);
}

/* Barycentric Triangle filling!!! */

void Triangle3D::draw(Color color)
{
    Point3D *points[3] = {lines[0].getP1(), lines[0].getP2(), lines[2].getP1()};
    int yBox[2] = {max(points[0]->getY(), 0), min(points[2]->getY(), HEIGHT)};
    if (yBox[0] < yBox[1])
    {
        int xBox[2] = {WIDTH, 0};
        for (int i = 0; i < 3; i++)
        {
            xBox[0] = max(0, min(xBox[0], points[i]->getX()));
            xBox[1] = min(WIDTH, max(xBox[1], points[i]->getX()));
        }
        if (xBox[0] >= xBox[1])
        {
            cout << "Triangle is outside of screen!" << endl;
        }
        else
        {
            for (int i = xBox[0]; i <= xBox[1]; i++)
            {
                for (int j = yBox[0]; j < yBox[1]; j++)
                {
                    bool draw = this->pointInTriangle(i, j);
                    if (draw)
                    {
                        graphicsDrawPoint(i, j, color);
                    }
                }
            }
        }
    }
    else
    {
        cout << "Triangle is outside of screen!" << endl;
    }
}

/* Simple Triangle line filling!!! */

// void Triangle3D::draw(Color color)
// {
//     Point3D *point1 = lines[0].getP1();
//     Point3D *point2 = lines[0].getP2();
//     Point3D *point3 = lines[2].getP1();

//     int totalYDiff = point3->getY() - point1->getY();
//     int totalXDiff = point3->getX() - point1->getX();
//     int segmentDiffTop = point2->getY() - point1->getY();
//     int segmentDiffBottom = point3->getY() - point2->getY();
//     for (int y = point1->getY(); y <= point2->getY(); y++)
//     {
//         float totalRatio = (float)(y - point1->getY()) / totalYDiff;
//         float segmentRatio = (float)(y - point1->getY()) / segmentDiffTop;
//         int x1 = point1->getX() + totalXDiff * totalRatio;
//         int x2 = point1->getX() + (point2->getX() - point1->getX()) * segmentRatio;
//         graphicsDrawLine(x1, y, x2, y, white);
//     }
//     for (int y = point2->getY(); y <= point3->getY(); y++)
//     {
//         float totalRatio = (float)(y - point1->getY()) / totalYDiff;
//         float segmentRatio = (float)(y - point2->getY()) / segmentDiffBottom;
//         int x1 = point1->getX() + totalXDiff * totalRatio;
//         int x2 = point2->getX() + (point3->getX() - point2->getX()) * segmentRatio;
//         graphicsDrawLine(x1, y, x2, y, white);
//     }
// }

Shape3D::Shape3D(int sides, int *points)
{
    this->sides = sides;
    lines = new Vector3D[sides];
    for (int i = 0; i < sides; i++)
    {
        int multiplier = i * 3;
        lines[i].setP1(*(points + multiplier), *(points + 1 + multiplier), *(points + 2 + multiplier));
        if (i + 1 == sides)
        {
            lines[i].setP2(*(points), *(points + 1), *(points + 2));
        }
        else
        {
            lines[i].setP2(*(points + 3 + multiplier), *(points + 4 + multiplier), *(points + 5 + multiplier));
        }
    }
}

Shape3D::~Shape3D()
{
    delete[] lines;
}

Vector3D *Shape3D::getVector(int index)
{
    if (index >= sides && index >= 0)
    {
        return (lines + index);
    }
    cout << "Out of bounds of shape sides!" << endl;
}

void Shape3D::draw(Color color)
{
    for (int i = 0; i < sides; i++)
    {
        lines[i].draw(color);
    }
}