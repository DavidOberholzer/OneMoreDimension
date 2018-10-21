#include <iostream>
#include "worldmath.h"
#include "data_io.h"

using namespace std;

// bool pointInside(int x, int y, Point3D *A, Vector3D *j, Vector3D *k)
// {
//     Vector3D *i = new Vector3D(x, y, 0, A->getX(), A->getY(), 0);
//     int ij = dotProduct(i->getVX(), i->getVY(), 0, j->getVX(), j->getVY(), 0);
//     int ik = dotProduct(i->getVX(), i->getVY(), 0, k->getVX(), k->getVY(), 0);
//     int jj = dotProduct(j->getVX(), j->getVY(), 0, j->getVX(), j->getVY(), 0);
//     int jk = dotProduct(j->getVX(), j->getVY(), 0, k->getVX(), k->getVY(), 0);
//     int kk = dotProduct(k->getVX(), k->getVY(), 0, k->getVX(), k->getVY(), 0);
//     float denominator = (jk * jk) - (kk * jj);
//     if (denominator == 0)
//     {
//         cout << "Denominator equals 0";
//     }
//     else
//     {
//         float u = -((ik * jk) - (ij * kk)) / denominator;
//         float v = -((ij * jk) - (ik * jj)) / denominator;
//         return u >= 0 && v >= 0 && (u + v) <= 1;
//     }
// }

int dotProduct(int x1, int y1, int z1, int x2, int y2, int z2)
{
    return x1 * x2 + y1 * y2 + z1 * z2;
}

void drawTriangle(Color color, Matrix *P, Matrix *V, Point3D points[3])
{
    Matrix projectedPoints[3];
    Point3D nPoints[3];
    bool draw = true;
    float x, y, z;
    for (int i = 0; i < 3; i++)
    {
        Point3D p = V->xPoint(points[i]);
        projectedPoints[i] = P->projectPoint(p, false);
        if (fabs(projectedPoints[i].getValue(0)) > fabs(p.getZ()) ||
            fabs(projectedPoints[i].getValue(4)) > fabs(p.getZ()) ||
            fabs(projectedPoints[i].getValue(8)) > fabs(p.getZ()))
        {
            draw = false;
            break;
        }
        else
        {
            x = projectedPoints[i].getValue(0) / projectedPoints[i].getValue(12);
            y = projectedPoints[i].getValue(4) / projectedPoints[i].getValue(12);
            z = projectedPoints[i].getValue(8) / projectedPoints[i].getValue(12);
            nPoints[i].setX((x + 1) * WIDTH / 2);
            nPoints[i].setY((y + 1) * HEIGHT / 2);
            nPoints[i].setZ(z);
        }
    }
    if (draw)
    {
        graphicsDrawLine(
            (int)nPoints[0].getX(), (int)nPoints[0].getY(),
            (int)nPoints[1].getX(), (int)nPoints[1].getY(),
            color);
        graphicsDrawLine(
            (int)nPoints[1].getX(), (int)nPoints[1].getY(),
            (int)nPoints[2].getX(), (int)nPoints[2].getY(),
            color);
        graphicsDrawLine(
            (int)nPoints[2].getX(), (int)nPoints[2].getY(),
            (int)nPoints[0].getX(), (int)nPoints[0].getY(),
            color);
    }
}

// void drawVector(Color color, Matrix *P, Point3D points[2])
// {
//     Matrix projectedPoints[2];
//     Point3D nPoints[2];
//     bool draw = true;
//     float x, y, z;
//     for (int i = 0; i < 2; i++)
//     {
//         Point3D *p = &points[i];
//         projectedPoints[i] = P->projectPoint(p, false);
//         if (fabs(projectedPoints[i].getValue(0)) > fabs(p->getZ()) ||
//             fabs(projectedPoints[i].getValue(4)) > fabs(p->getZ()) ||
//             fabs(projectedPoints[i].getValue(8)) > fabs(p->getZ()))
//         {
//             draw = false;
//             break;
//         }
//         else
//         {
//             x = projectedPoints[i].getValue(0) / projectedPoints[i].getValue(12);
//             y = projectedPoints[i].getValue(4) / projectedPoints[i].getValue(12);
//             z = projectedPoints[i].getValue(8) / projectedPoints[i].getValue(12);
//             nPoints[i].setX((x + 1) * WIDTH / 2);
//             nPoints[i].setY((y + 1) * HEIGHT / 2);
//             nPoints[i].setZ(z);
//         }
//     }
//     if (draw)
//     {
//         graphicsDrawLine(
//             (int)nPoints[0].getX(), (int)nPoints[0].getY(),
//             (int)nPoints[1].getX(), (int)nPoints[1].getY(),
//             color);
//     }
// }

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

Point3D::Point3D()
{
    x = 0;
    y = 0;
    z = 0;
}

Point3D::Point3D(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
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

void Point3D::rotateQ(float pitch, float yaw, float angle)
{
    // Quarternion Rotation
    Quarternion q = Quarternion(pitch, yaw, angle);
    Point3D r = q * Point3D(this->x, this->y, this->z);
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
    return Point3D(this->x * scale, this->y * scale, this->z * scale);
}

Point3D Point3D::cross(Point3D p)
{
    Point3D r;

    r.setX(this->y * p.getZ() - this->z * p.getY());
    r.setY(this->z * p.getX() - this->x * p.getZ());
    r.setZ(this->x * p.getY() - this->y * p.getX());

    return r;
}

void Point3D::print()
{
    printf("Point\n");
    printf("x: %.2f\ny: %.2f\nz: %.2f\n", this->x, this->y, this->z);
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

// bool Triangle3D::pointInTriangle(int x, int y)
// {
//     Vector3D *j = new Vector3D(
//         this->points[0].getX(), this->points[0].getY(), this->points[0].getZ(),
//         this->points[2].getX(), this->points[2].getY(), this->points[2].getZ());
//     Vector3D *k = new Vector3D(
//         this->points[0].getX(), this->points[0].getY(), this->points[0].getZ(),
//         this->points[1].getX(), this->points[1].getY(), this->points[1].getZ());
//     return pointInside(x, y, this->points, j, k);
// }

// /* Barycentric Triangle filling!!! */

// void Triangle3D::draw(Color color)
// {
//     Point3D *points[3] = {lines[0].getP1(), lines[0].getP2(), lines[2].getP1()};
//     int yBox[2] = {max(points[0]->getY(), 0), min(points[2]->getY(), HEIGHT)};
//     if (yBox[0] < yBox[1])
//     {
//         int xBox[2] = {WIDTH, 0};
//         for (int i = 0; i < 3; i++)
//         {
//             xBox[0] = max(0, min(xBox[0], points[i]->getX()));
//             xBox[1] = min(WIDTH, max(xBox[1], points[i]->getX()));
//         }
//         if (xBox[0] >= xBox[1])
//         {
//             cout << "Triangle is outside of screen!" << endl;
//         }
//         else
//         {
//             for (int i = xBox[0]; i <= xBox[1]; i++)
//             {
//                 for (int j = yBox[0]; j < yBox[1]; j++)
//                 {
//                     bool draw = this->pointInTriangle(i, j);
//                     if (draw)
//                     {
//                         graphicsDrawPoint(i, j, color);
//                     }
//                 }
//             }
//         }
//     }
//     else
//     {
//         cout << "Triangle is outside of screen!" << endl;
//     }
// }

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
        printf("Invalid Matrix size (valid: 2, 3, 4)!");
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

void Matrix::printMatrix()
{
    printf("Matrix\n");
    if (this->onebyX)
    {
        int length = this->two ? 2 : this->three ? 3 : 4;
        for (int i = 0; i < length; i++)
        {
            printf("%.2f\n", this->values[i * 4]);
            if (i + 1 < length)
            {
                printf("----\n");
            }
        }
    }
    if (this->two)
    {
        printf("%.2f | %.2f\n", this->values[0], this->values[1]);
        printf("-----------\n");
        printf("%.2f | %.2f\n", this->values[4], this->values[5]);
    }
    if (this->three)
    {
        printf("%.2f | %.2f | %.2f\n", this->values[0], this->values[1], this->values[2]);
        printf("------------------\n");
        printf("%.2f | %.2f | %.2f\n", this->values[4], this->values[5], this->values[6]);
        printf("------------------\n");
        printf("%.2f | %.2f | %.2f\n", this->values[8], this->values[9], this->values[10]);
    }
    if (this->four)
    {
        printf("%.2f | %.2f | %.2f | %.2f\n", this->values[0], this->values[1], this->values[2], this->values[3]);
        printf("-------------------------\n");
        printf("%.2f | %.2f | %.2f | %.2f\n", this->values[4], this->values[5], this->values[6], this->values[7]);
        printf("-------------------------\n");
        printf("%.2f | %.2f | %.2f | %.2f\n", this->values[8], this->values[9], this->values[10], this->values[11]);
        printf("-------------------------\n");
        printf("%.2f | %.2f | %.2f | %.2f\n", this->values[12], this->values[13], this->values[14], this->values[15]);
    }
    printf("Matrix End\n\n");
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
        printf("Matrix sizes differ! Cannot multiply.");
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
        Point3D p1 = Point3D(this->x, this->y, this->z);
        Point3D p2 = Point3D(q.getX(), q.getY(), q.getZ());
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
    // Quarternion pq;
    // pq.setW(0);
    // pq.setX(p.getX());
    // pq.setY(p.getY());
    // pq.setZ(p.getZ());
    // Quarternion q = (*this);
    // Quarternion qt = q * pq * q.inverse();
    // Point3D result = Point3D(qt.getX(), qt.getY(), qt.getZ());

    // return result;
    if (this->w)
    {
        Point3D pq = Point3D(this->x, this->y, this->z);
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

    // Point3D cross1 = pq.cross(p);
    // Point3D cross2 = pq.cross(cross1);

    // result.setX(p.getX() + 2.0 * this->w * cross1.getX() + 2.0 * cross2.getX());
    // result.setY(p.getY() + 2.0 * this->w * cross1.getY() + 2.0 * cross2.getY());
    // result.setZ(p.getZ() + 2.0 * this->w * cross1.getZ() + 2.0 * cross2.getZ());
    return result;
}

void Quarternion::print()
{
    cout << "w: " << this->w << endl;
    cout << "x: " << this->x << endl;
    cout << "y: " << this->y << endl;
    cout << "z: " << this->z << endl;
}