#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include "Base/Vectorf.h"
#include "Base/Time.h"
#include "Base/Input.h"
#include "Base/Screen.h"

#define WINDOW_WIDTH 1300
#define WINDOW_HEIGHT 800
#define PIXEL_SCALE 1

struct Math
{
private:
    static double Dsin[36000];
    static double Dcos[36000];
public:
    static const double pi;
    static void InitSinAndCos()
    {
        for (int i = 0; i < 36000; i++)
        {
            Dsin[i] = sin(i / 180.0 * pi);
            Dcos[i] = cos(i / 180.0 * pi);
        }
    }
    static inline Vector2d RotateAroundPoint2d(const Vector2d& point, const Vector2d& aroundwhat, float a)
    {
        float CS = Math::GetCos(a);
        float SN = Math::GetSin(a);
        Vector2d vec = point - aroundwhat;
        return Vector2d(vec.x * CS - vec.y * SN + aroundwhat.x, vec.x * SN + vec.y * CS + aroundwhat.y);
    }
    static inline Vector3d RotateAroundPoint3d(const Vector3d& point, const Vector3d& aroundwhat, float a, float l = 0.0f)
    {
        Vector2d vec = RotateAroundPoint2d(Vector2d(point.x, point.z), Vector2d(aroundwhat.x, aroundwhat.z), a);
        float x = vec.x;
        vec = RotateAroundPoint2d(Vector2d(vec.y, point.y), Vector2d(aroundwhat.z, aroundwhat.y), l);
        return Vector3d(x, vec.y, vec.x);
    }
    static inline float Interpolate(float x1, float y1, float x2, float y2, float x)
    {
        if (x1 - x2 == 0)
            return 0;
        return y2 + (y1 - y2) * (x - x2) / (x1 - x2);
    }
    static double GetSin(float angle) { return Dsin[(int)(angle * 100) % 36000]; }
    static double GetCos(float angle) { return Dcos[(int)(angle * 100) % 36000]; }
};
const double Math::pi = 3.14159265359;
double Math::Dsin[36000];
double Math::Dcos[36000];

class Camera
{
public:
    float fov = 400.0;
    Vector3d pos = Vector3d::ZERO;
    Screen* scr;
    float a;
    Camera(Screen* scr)
    { 
        this->scr = scr;
        this->pos = Vector3d::ZERO;
    }
    void WorldPointToScreen(Vector3d point, int& x, int& y, bool rotated = false, bool displaced = false)
    {
        if (!rotated)
            point = Math::RotateAroundPoint3d(point, pos, a);
        if (!displaced)
            point = point - pos;

        float a = fov;
        if (point.z != 0)
            a = fov / point.z;
        x = (point.x * a) + scr->GetScreenWidth() / 2;
        y = (-1 * point.y * a) + scr->GetScreenHeight() / 2;
    }
};
Camera* mcam;

class Player
{
public:
    float speed;
    float rotate_speed;
    Vector3d position;
    float a;
    float l;
    Player()
    {
        speed = 2;
        rotate_speed = 100;
        position.x = 0;
        position.y = 0;
        position.z = 0;
        a = 0;
        l = 0;
    }
    Player(const Vector3d& pos, float a, float l, float speed = 2, float rotate_speed = 2)
    {
        this->speed = speed;
        this->rotate_speed = rotate_speed;
        this->position = pos;
        this->a = a;
        this->l = l;
    }
    void RotateA(float angle)
    {
        a += angle;
        if(a >= 360)
            a -= 360;
        if (a < 0)
            a += 360;
    }
    inline Vector3d GetViewDirection()
    {
        Vector3d vec = Vector3d(Math::GetSin(this->a), Math::GetSin(this->l), Math::GetCos(this->a));
        return vec;
    }
    void Move()
    {
        float speed = this->speed;
        if (Input::Get_key_down(SDL_SCANCODE_LSHIFT))
            speed *= 2;
        RotateA(Input::Get_delta_mouse_x() * Time::Delta_time() * rotate_speed);

        if (Input::Get_key_down(SDL_SCANCODE_SPACE))
            this->position.y += speed * Time::Delta_time() / 1.3;
        if (Input::Get_key_down(SDL_SCANCODE_LCTRL))
            this->position.y -= speed * Time::Delta_time() / 1.3;

        Vector3d vec;
        Vector3d look = this->GetViewDirection();
        Vector3d par = Math::RotateAroundPoint3d(Vector3d::LEFT, Vector3d::ZERO, 360 - this->a);

        if (Input::Get_key_down(SDL_SCANCODE_A)) { vec = vec + par; }
        if (Input::Get_key_down(SDL_SCANCODE_D)) { vec = vec + par * -1; }
        if (Input::Get_key_down(SDL_SCANCODE_S)) { vec = vec + look * -1; }
        if (Input::Get_key_down(SDL_SCANCODE_W)) { vec = vec + look; }

        this->position = this->position + vec.Normalize() * speed * Time::Delta_time();
    }
};
Player* p = new Player(Vector3d(0, 0.5, 0), 0, 0, 2.0f, 0.3f / PIXEL_SCALE);

class Minimap
{
public:
    int w, h;
    float zoom;
    Player* p;
    Screen* scr;
    std::vector<Vector2d*> points;
    Minimap(int w, int h, float zoom, Player* p, Screen* scr)
    {
        this->zoom = zoom;
        this->p = p;
        this->scr = scr;
        this->w = w;
        this->h = h;
    }
    Vector2d WorldPointToScreen(const Vector2d& point)
    {
        return Vector2d(point.x * zoom + w / 2, h / 2 - point.y * zoom);
    }
    Vector2d WorldPointToScreen(const Vector3d& point)
    {
        return Vector2d(point.x * zoom + w / 2, h / 2 - point.z * zoom);
    }
    void Draw()
    {
        scr->DrawRect(0, 0, w, h , Color(255, 255, 255));
        scr->DrawRect(1, 1, w - 2, h - 2, Color(0, 0, 0));

        Vector2d rotc = Math::RotateAroundPoint2d(Vector2d::ZERO, Vector2d(p->position.x, p->position.z), p->a);
        rotc = this->WorldPointToScreen(rotc);
        scr->DrawPixel(rotc.x - p->position.x * zoom, rotc.y + p->position.z * zoom, Color(50, 50, 50));

        scr->DrawPixel(w / 2, h / 2, Color(0, 255, 0));
        scr->DrawPixel(w / 2, h / 2 - 2, Color(0, 100, 0));

        int x, y;

        Vector2d ni;

        for (Vector2d* i : points)
        {
            ni = Math::RotateAroundPoint2d(*i, Vector2d(p->position.x, p->position.z), p->a);
            x = ni.x * zoom - p->position.x * zoom + w / 2 + 0.5;
            y = -1 * (ni.y * zoom - p->position.z * zoom) + h / 2 + 0.5;
            if(x >= 1 && x <= w - 2)
                if(y >= 1 && y <= h - 2)
                    scr->DrawPixel(x, y, Color(0, 0, 255));
        }
    }
    void DrawPointOnMinimap(Vector3d point)
    { 
        scr->DrawPixel(point.x * zoom + w / 2, -1 * point.z * zoom + h / 2, Color(0, 0, 255));
    }
};
Minimap* mp = NULL;

class Wall
{
public:
    Vector3d point1, point2, point3, point4;
    Color col;
    float height;
    static Camera* cam;
    int x1, y1, x2, y2;
    int x3, y3, x4, y4;
    bool flipped = false;
    Wall(const Vector3d& point1, const Vector3d& point2, float height, Color col)
    {
        Init(point1, point2, height, col);
    }
    Wall()
    {
        this->point1 = Vector3d::ZERO;
        this->point2 = Vector3d::ZERO;
        this->point3 = Vector3d::ZERO;
        this->point4 = Vector3d::ZERO;
        this->height = 0;
    }
    void Init(const Vector3d& point1, const Vector3d& point2, float height, Color col)
    {
        this->point1 = point1;
        this->point2 = point2;
        this->point3 = point1;
        this->point3.y += height;
        this->point4 = point2;
        this->point4.y += height;
        this->height = height;
        this->col = col;
    }
    static void ClipWall(Vector3d& point1, Vector3d& point2, Vector3d& point3, Vector3d& point4)
    {
        if (point1.z <= 0)
        {
            point1.x = Math::Interpolate(point1.z, point1.x, point2.z, point2.x, 0);
            point1.z = 0.0001;

            point3.x = point1.x;
            point3.z = point1.z;
        }
        if (point2.z <= 0)
        {
            point2.x = Math::Interpolate(point1.z, point1.x, point2.z, point2.x, 0);
            point2.z = 0.0001;

            point4.x = point2.x;
            point4.z = point2.z;
        }
    }
    static float GetDistanseToCamera(const Wall& wall)
    {
        return Vector3d::Magnitude(wall.point1 - Wall::cam->pos);
    }
    void Draw(Vector3d add = Vector3d::ZERO)
    {
        x1 = 0; y1 = 0; x2 = 0; y2 = 0;
        x3 = 0; y3 = 0; x4 = 0; y4 = 0;
        Vector3d point1, point2, point3, point4;
        point1 = Math::RotateAroundPoint3d(this->point1 + add, cam->pos, cam->a);
        point2 = Math::RotateAroundPoint3d(this->point2 + add, cam->pos, cam->a);
        point3 = Math::RotateAroundPoint3d(this->point3 + add, cam->pos, cam->a);
        point4 = Math::RotateAroundPoint3d(this->point4 + add, cam->pos, cam->a);
        point1 = point1 - cam->pos;
        point2 = point2 - cam->pos;
        point3 = point3 - cam->pos;
        point4 = point4 - cam->pos;

        if (point2.z <= 0 && point1.z <= 0)
            return;

        if (point2.z <= 0 || point1.z <= 0)
            ClipWall(point1, point2, point3, point4);

        float dst = Vector3d::Magnitude(point1) * 7;
        char r, g, b;
        r = this->col.r - dst < 0 ? 0 : this->col.r - dst;
        g = this->col.g - dst < 0 ? 0 : this->col.g - dst;
        b = this->col.b - dst < 0 ? 0 : this->col.b - dst;

        Color col = Color(r, g, b);
        
        cam->WorldPointToScreen(point1, x1, y1, true, true);
        cam->WorldPointToScreen(point2, x2, y2, true, true);
        cam->WorldPointToScreen(point3, x3, y3, true, true);
        cam->WorldPointToScreen(point4, x4, y4, true, true);

        if (x3 > x4)
        {
            std::swap(x3, x4);
            std::swap(y3, y4);
            std::swap(x1, x2);
            std::swap(y1, y2);
            flipped = true;
        }
        else
            flipped = false;
        int dyH = y4 - y3;
        int dxH = x4 - x3;
        if (dxH == 0)
            dxH = 1;

        int dyB = y2 - y1;
        int dxB = x2 - x1;
        if (dxB == 0)
            dxB = 1;

        float aH = dyH / (float)dxH;
        float aB = dyB / (float)dxB;
        int yH = 0, yB = 0;

        if (x1 < 0) { y1 = y1 + aB * (0 - x1) + 0.5; x1 = 0; }
        if (x2 < 0) { y2 = y2 + aB * (0 - x2) + 0.5; x2 = 0; }
        if (x3 < 0) { y3 = y3 + aH * (0 - x3) + 0.5; x3 = 0; }
        if (x4 < 0) { y4 = y4 + aH * (0 - x4) + 0.5; x4 = 0; }

        if (x1 > cam->scr->GetScreenWidth()) { y1 = y1 + aB * (cam->scr->GetScreenWidth() - x1) + 0.5; x1 = cam->scr->GetScreenWidth(); }
        if (x2 > cam->scr->GetScreenWidth()) { y2 = y2 + aB * (cam->scr->GetScreenWidth() - x2) + 0.5; x2 = cam->scr->GetScreenWidth(); }
        if (x3 > cam->scr->GetScreenWidth()) { y3 = y3 + aH * (cam->scr->GetScreenWidth() - x3) + 0.5; x3 = cam->scr->GetScreenWidth(); }
        if (x4 > cam->scr->GetScreenWidth()) { y4 = y4 + aH * (cam->scr->GetScreenWidth() - x4) + 0.5; x4 = cam->scr->GetScreenWidth(); }

        if (!flipped)
            for (int x = x3; x < x4; x++)
            {
                yH = aH * (x - x1) + y3;
                yB = aB * (x - x1) + y1;

                if (yH < 0) yH = 0;
                if (yB < 0) yB = 0;
                if (yH > cam->scr->GetScreenHeight()) yH = cam->scr->GetScreenHeight();
                if (yB > cam->scr->GetScreenHeight()) yB = cam->scr->GetScreenHeight();

                cam->scr->DrawLine(x, yH, x, yB, col);
            }
    }
};
Camera* Wall::cam = NULL;

void SortWalls(Wall* wall, int size);
bool wallsortcomp(const Wall& wall1, const Wall& wall2);

class Sector
{
public:
    Wall* walls;
    int wallsnum;
    float height, bottom;
    Color bottomcol;
    Color surfcol;
    Sector()
    {
        walls = NULL;
        wallsnum = -1;
        height = 0;
        bottom = 0;
        bottomcol = Color();
        surfcol = Color();
    }
    void Init(Vector2d* points, int pointsnum, float wallheight, float wallbottom, Color col, Color surfcol, Color bottomcol)
    {
        if (walls != NULL)
            Clean();
        walls = new Wall[pointsnum]();

        this->surfcol = surfcol;
        this->bottomcol = bottomcol;
        this->height = wallheight;
        this->bottom = wallbottom;
        this->wallsnum = pointsnum;
        for (int i = 0; i < pointsnum; i++)
        {
            if (i + 1 >= pointsnum)
                walls[i].Init(Vector3d(points[pointsnum - 1].x, bottom, points[pointsnum - 1].y), Vector3d(points[0].x, bottom, points[0].y), height, col);
            else
                walls[i].Init(Vector3d(points[i].x, bottom, points[i].y), Vector3d(points[i + 1].x, bottom, points[i + 1].y), height, col);
        }
    }
    void Draw()
    {
        SortWalls(walls, wallsnum);
        char surf = 0;//0 - normal, 1 - height, 2 - bottom
        if (Wall::cam->pos.y >= walls[0].point1.y && Wall::cam->pos.y <= walls[0].point3.y)
            surf = 0;
        else if (Wall::cam->pos.y > walls[0].point3.y)
            surf = 1;
        else  surf = 2;

        int minx = 10000, maxx = -1;
        for (int i = 0; i < wallsnum; i++)
        {
            walls[i].Draw();
            if (walls[i].x3 < minx)
                minx = walls[i].x3;
            if (walls[i].x4 > maxx)
                maxx = walls[i].x4;
        }
        if (minx > Wall::cam->scr->GetScreenWidth() || maxx < 0) return;
        if (minx < 0) minx = 0;
        if (maxx > Wall::cam->scr->GetScreenWidth()) maxx = Wall::cam->scr->GetScreenWidth();

        int deltax = maxx - minx;
        int* fpx = new int[deltax];
        int* spx = new int[deltax];
        int n = 0;
        int n2 = 0;
        for (int i = 0; i < wallsnum; i++)
        {
            switch (surf)
            {
            case 1:
                if (walls[i].flipped)
                {
                    n2 += walls[i].x4 - walls[i].x3;
                    for (int x = walls[i].x3 - minx; x < walls[i].x4 - minx; x++)
                    {
                        fpx[x] = Math::Interpolate(walls[i].x3, walls[i].y3, walls[i].x4, walls[i].y4, x + minx);
                        if (fpx[x] > Wall::cam->scr->GetScreenHeight())
                            fpx[x] = Wall::cam->scr->GetScreenHeight();
                        else if (fpx[x] < 0)
                            fpx[x] = 0;
                    }
                }
                else
                {
                    n += walls[i].x4 - walls[i].x3;
                    for (int x = walls[i].x3 - minx; x < walls[i].x4 - minx; x++)
                    {
                        spx[x] = Math::Interpolate(walls[i].x3, walls[i].y3, walls[i].x4, walls[i].y4, x + minx);
                        if (spx[x] > Wall::cam->scr->GetScreenHeight())
                            spx[x] = Wall::cam->scr->GetScreenHeight();
                        else if (spx[x] < 0)
                            spx[x] = 0;
                    }
                }
                break;
            case 2:
                if (walls[i].flipped)
                {
                    n2 += walls[i].x2 - walls[i].x1;
                    for (int x = walls[i].x1 - minx; x < walls[i].x2 - minx; x++)
                    {
                        fpx[x] = Math::Interpolate(walls[i].x1, walls[i].y1, walls[i].x2, walls[i].y2, x + minx);
                        if (fpx[x] > Wall::cam->scr->GetScreenHeight())
                            fpx[x] = Wall::cam->scr->GetScreenHeight();
                        else if (fpx[x] < 0)
                            fpx[x] = 0;
                    }
                }
                else
                {
                    n += walls[i].x2 - walls[i].x1;
                    for (int x = walls[i].x1 - minx; x < walls[i].x2 - minx; x++)
                    {
                        spx[x] = Math::Interpolate(walls[i].x1, walls[i].y1, walls[i].x2, walls[i].y2, x + minx);
                        if (spx[x] > Wall::cam->scr->GetScreenHeight())
                            spx[x] = Wall::cam->scr->GetScreenHeight();
                        else if (spx[x] < 0)
                            spx[x] = 0;
                    }
                }
                break;
            }
        }

        for (int i = 0; i < deltax; i++)
        {
            if (n == 0 && n2 != 0)
            {
                if(surf == 1)
                    Wall::cam->scr->DrawLine(i + minx, fpx[i], i + minx, Wall::cam->scr->GetScreenHeight(), surfcol);
                else if(surf == 2)
                    Wall::cam->scr->DrawLine(i + minx, fpx[i], i + minx, 0, bottomcol);
            }
            else
                Wall::cam->scr->DrawLine(i + minx, fpx[i], i + minx, spx[i], surf == 1 ? surfcol : bottomcol);

        }
        delete[] fpx;
        delete[] spx;
    }
    void Clean()
    {
        delete[] walls;
    }
};

bool sectorssortcomp(const Sector& sec1, const Sector& sec2)
{
    return Wall::GetDistanseToCamera(sec1.walls[0]) > Wall::GetDistanseToCamera(sec2.walls[0]);
}

bool wallsortcomp(const Wall& wall1, const Wall& wall2)
{
    return Wall::GetDistanseToCamera(wall1) > Wall::GetDistanseToCamera(wall2);
}

void SortWalls(Wall* wall, int size)
{
    std::sort(wall, wall + size, wallsortcomp);
}

bool InitApp()
{
    srand(time(0));
    Math::InitSinAndCos();
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        return 1;
    
    Screen::scr->CreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, PIXEL_SCALE);
    mp = new Minimap(200 / PIXEL_SCALE, 120 / PIXEL_SCALE, 4, p, Screen::scr);
    mcam = new Camera(Screen::scr);
    Wall::cam = mcam;
    Time::Set_target_framerate(120);
    Input::Lock_cursor(Screen::scr->window);
    SDL_ShowCursor(0);
    return 0;
}

int main(int argc, char** argv)
{
    if (InitApp())
        return 0;

    const int size = 6;
    Vector2d point[size];
    point[0] = Vector2d(-1, 0);
    point[1] = Vector2d(-0.5, -1);
    point[2] = Vector2d(0.5, -1);
    point[3] = Vector2d(1, 0);
    point[4] = Vector2d(0.5, 1);
    point[5] = Vector2d(-0.5, 1);
    Sector sec1;
    sec1.Init(point, size, 0.5, 0, Color(125, 56, 101), Color(125, 56, 101), Color(125, 56, 101));
    for (int i = 0; i < size; i++)
        mp->points.push_back(&point[i]);

    const int size2 = 6;
    Vector2d point2[size];
    point2[0] = Vector2d(-4, 0);
    point2[1] = Vector2d(-3.5, -1);
    point2[2] = Vector2d(-2.5, -1);
    point2[3] = Vector2d(-2, 0);
    point2[4] = Vector2d(-2.5, 1);
    point2[5] = Vector2d(-3.5, 1);
    Sector sec2;
    sec2.Init(point2, size2, 1, 0, Color(200, 150, 3), Color(200, 150, 3), Color(200, 150, 3));
    for (int i = 0; i < size2; i++)
        mp->points.push_back(&point2[i]);

    const int size3 = 6;
    Vector2d point3[size];
    point3[0] = Vector2d(-4, 3);
    point3[1] = Vector2d(-3.5, 2);
    point3[2] = Vector2d(-2.5, 2);
    point3[3] = Vector2d(-2, 3);
    point3[4] = Vector2d(-2.5, 4);
    point3[5] = Vector2d(-3.5, 4);
    Sector sec3;
    sec3.Init(point3, size3, 1.5, 0, Color(148, 146, 23), Color(148, 146, 23), Color(148, 146, 23));
    for (int i = 0; i < size3; i++)
        mp->points.push_back(&point3[i]);

    const int size4 = 6;
    Vector2d point4[size];
    point4[0] = Vector2d(-1, 3);
    point4[1] = Vector2d(-0.5, 2);
    point4[2] = Vector2d(0.5, 2);
    point4[3] = Vector2d(1, 3);
    point4[4] = Vector2d(0.5, 4);
    point4[5] = Vector2d(-0.5, 4);
    Sector sec4;
    sec4.Init(point4, size4, 2, 0, Color(15, 162, 169), Color(15, 162, 169), Color(15, 162, 169));
    for (int i = 0; i < size4; i++)
        mp->points.push_back(&point4[i]);

    Sector sectors[4];
    sectors[0] = sec1;
    sectors[1] = sec2;
    sectors[2] = sec3;
    sectors[3] = sec4;

    /*const int size = 20;
    Vector2d point[size];
    point[0] = Vector2d(0.5, 1);
    point[1] = Vector2d(1, 0.5);
    point[2] = Vector2d(2, 0.5);
    point[3] = Vector2d(2, 2);
    point[4] = Vector2d(3, 2);
    point[5] = Vector2d(3.5, 2.5);
    point[6] = Vector2d(4.5, 2.5);
    point[7] = Vector2d(5, 2);
    point[8] = Vector2d(6, 2);
    point[9] = Vector2d(6, -1);
    point[10] = Vector2d(5, -2);
    point[11] = Vector2d(3, -2);
    point[12] = Vector2d(2, -1);
    point[13] = Vector2d(2, -0.5);
    point[14] = Vector2d(1, -0.5);
    point[15] = Vector2d(0.5, -1);
    point[16] = Vector2d(-1, -1);
    point[17] = Vector2d(-1.5, -0.5);
    point[18] = Vector2d(-1.5, 0.5);
    point[19] = Vector2d(-1, 1);
    Sector sec1;
    sec1.Init(point, size, 1, 0, Color(255, 153, 102));
    for (int i = 0; i < size; i++)
        mp->points.push_back(&point[i]);


    const int size2 = 4;
    Vector2d point2[size2];
    point2[0] = Vector2d(3.5, 0.5);
    point2[1] = Vector2d(3.5, -0.5);
    point2[2] = Vector2d(4.5, -0.5);
    point2[3] = Vector2d(4.5, 0.5);
    Sector sec2;
    sec2.Init(point2, size2, 0.3, 0, Color(51, 204, 102));
    for (int i = 0; i < size2; i++)
        mp->points.push_back(&point2[i]);*/

    bool quit = false;
    bool lockedcursor = true;
    while (!quit)
    {
        Input::Handle_events();
        quit = Input::Qiut();
        SDL_SetRenderTarget(Screen::scr->ren, Screen::scr->screen_tex);
        SDL_SetRenderDrawColor(Screen::scr->ren, 102, 102, 153, 255);
        SDL_RenderClear(Screen::scr->ren);

        //////////////////////////////////UPDATE//////////////////////////////////

        if (Input::Get_key_down(SDL_SCANCODE_ESCAPE))
            quit = true;
        if (Input::Get_key_down(SDL_SCANCODE_F1))
        {
            lockedcursor = !lockedcursor;
            if (lockedcursor)
                Input::Lock_cursor(Screen::scr->window);
            else
                Input::Unlock_cursor();
        }

        p->Move();
        mcam->pos = p->position;
        mcam->a = p->a;

        std::sort(sectors, sectors + 4, sectorssortcomp);
        for (int i = 0; i < 4; i++)
            sectors[i].Draw();
        //sec1.Draw();
        mp->Draw();

        mcam->fov += Input::Get_scroll_whell() * Time::Delta_time() * 500;
        //std::cout << 1.0f / Time::Delta_time() << '\n';

        //////////////////////////////////UPDATE//////////////////////////////////

        SDL_SetRenderTarget(Screen::scr->ren, NULL);
        SDL_RenderCopy(Screen::scr->ren, Screen::scr->screen_tex, NULL, NULL);
        SDL_RenderPresent(Screen::scr->ren);
        Time::Tick();

    }
    Screen::scr->ApplicationQuit();
    delete(Screen::scr);
    return 0;
}