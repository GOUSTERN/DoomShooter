#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include "Base/Vectorf.h"
#include "Base/Time.h"
#include "Base/Input.h"
#include "Base/Screen.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define PIXEL_SCALE 2

struct Math
{
private:
    static double Dsin[3600];
    static double Dcos[3600];
public:
    static const double pi;
    static void InitSinAndCos()
    {
        for (int i = 0; i < 3600; i++)
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
    static double GetSin(float angle) { return Dsin[(int)(angle * 10) % 3600]; }
    static double GetCos(float angle) { return Dcos[(int)(angle * 10) % 3600]; }
};
const double Math::pi = 3.14159265359;
double Math::Dsin[3600];
double Math::Dcos[3600];

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
Camera* mcam = new Camera(Screen::scr);

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
    Player(const Vector3d& pos, float a, float l, float speed = 2, float rotate_speed = 100)
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
        RotateA(Input::Get_delta_mouse_x() * Time::Delta_time() * 5.0f);

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
Player* p = new Player(Vector3d(0, 0.5, 0), 0, 0, 2.0f, 4.0f);

class Minimap
{
public:
    int w, h;
    float zoom;
    Player* p;
    Screen* scr;
    std::vector<Vector3d*> points;
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

        for (Vector3d* i : points)
        {
            Vector3d ni = Math::RotateAroundPoint3d(*i, p->position, p->a);
            x = ni.x * zoom - p->position.x * zoom + w / 2 + 0.5;
            y = -1 * (ni.z * zoom - p->position.z * zoom) + h / 2 + 0.5;
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

class Wall
{
public:
    Vector3d point1, point2, point3, point4;
    Color col;
    float height;
    static Camera* cam;
    Wall(const Vector3d& point1, const Vector3d& point2, float height, Color col)
    {
        init(point1, point2, height, col);
    }
    Wall()
    {
        this->point1 = Vector3d::ZERO;
        this->point2 = Vector3d::ZERO;
        this->point3 = Vector3d::ZERO;
        this->point4 = Vector3d::ZERO;
        this->height = 0;
    }
    void init(const Vector3d& point1, const Vector3d& point2, float height, Color col)
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
    inline float GetDistanseToCamera()
    {
        return Vector3d::Magnitude(this->point1 - p->position);
    }
    void Draw()
    {
        Vector3d point1, point2, point3, point4;
        point1 = Math::RotateAroundPoint3d(this->point1, cam->pos, cam->a);
        point2 = Math::RotateAroundPoint3d(this->point2, cam->pos, cam->a);
        point3 = Math::RotateAroundPoint3d(this->point3, cam->pos, cam->a);
        point4 = Math::RotateAroundPoint3d(this->point4, cam->pos, cam->a);
        point1 = point1 - cam->pos;
        point2 = point2 - cam->pos;
        point3 = point3 - cam->pos;
        point4 = point4 - cam->pos;

        if (point2.z <= 0 && point1.z <= 0)
            return;

        if (point2.z <= 0 || point1.z <= 0)
            ClipWall(point1, point2, point3, point4);

        float dst = Vector3d::Magnitude(point1) * 20;
        char r, g, b;
        r = this->col.r - dst < 0 ? 0 : this->col.r - dst;
        g = this->col.g - dst < 0 ? 0 : this->col.g - dst;
        b = this->col.b - dst < 0 ? 0 : this->col.b - dst;

        Color col = Color(r, g, b);

        int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
        int x3 = 0, y3 = 0, x4 = 0, y4 = 0;
        cam->WorldPointToScreen(point1, x1, y1, true, true);
        cam->WorldPointToScreen(point2, x2, y2, true, true);
        cam->WorldPointToScreen(point3, x3, y3, true, true);
        cam->WorldPointToScreen(point4, x4, y4, true, true);

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
        
        if (x1 < 0) { y1 = y1 + aB * (0 - x1); x1 = 0; }
        if (x2 < 0) { y2 = y2 + aB * (0 - x2); x2 = 0; }
        if (x3 < 0) { y3 = y3 + aH * (0 - x3); x3 = 0; }
        if (x4 < 0) { y4 = y4 + aH * (0 - x4); x4 = 0; }

        if (x1 > cam->scr->GetScreenWidth()) x1 = cam->scr->GetScreenWidth();
        if (x2 > cam->scr->GetScreenWidth()) x2 = cam->scr->GetScreenWidth();
        if (x3 > cam->scr->GetScreenWidth()) x3 = cam->scr->GetScreenWidth();
        if (x4 > cam->scr->GetScreenWidth()) x4 = cam->scr->GetScreenWidth();

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
Camera* Wall::cam = mcam;

bool comp(Wall wall1, Wall wall2)
{
    return wall1.GetDistanseToCamera() > wall2.GetDistanseToCamera();
}

void SortWalls(Wall* wall, int size)
{
    std::sort(wall, wall + size, comp);

    //////////////BOOBLE SORT/////////////////
    // 
    //for (int i = 0; i < size - 1; i++)
    //    for (int j = 0; j < size - i - 1; j++)
    //        if (wall[j].GetDistanseToCamera() < wall[j + 1].GetDistanseToCamera())
    //            std::swap(wall[j], wall[j + 1]);
}

bool InitApp()
{
    srand(time(0));
    Math::InitSinAndCos();
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        return 1;
    
    Screen::scr->CreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, PIXEL_SCALE, SDL_WINDOW_FULLSCREEN);
    Time::Set_target_framerate(60);
    Input::Lock_cursor(Screen::scr->window);
    SDL_ShowCursor(0);
}

int main(int argc, char** argv)
{
    InitApp();

    Minimap* mp = new Minimap(100, 60, 4, p, Screen::scr);

    /*Vector3d point1(0, -0.5, -3), point2(0, -0.5, 3);
    Wall wall1(point1, point2, 2, Color(255, 255, 0));
    mp->points.push_back(&point1);
    mp->points.push_back(&point2);
    Wall::mp = mp;*/
    
    const int size = 20;
    Vector3d point[size];
    Wall wall[size];
    point[0] = Vector3d(0.5, 0, 1);
    point[1] = Vector3d(1, 0, 0.5);
    point[2] = Vector3d(2, 0, 0.5);
    point[3] = Vector3d(2, 0, 2);
    point[4] = Vector3d(3, 0, 2);
    point[5] = Vector3d(3.5, 0, 2.5);
    point[6] = Vector3d(4.5, 0, 2.5);
    point[7] = Vector3d(5, 0, 2);
    point[8] = Vector3d(6, 0, 2);
    point[9] = Vector3d(6, 0, -1);
    point[10] = Vector3d(5, 0, -2);
    point[11] = Vector3d(3, 0, -2);
    point[12] = Vector3d(2, 0, -1);
    point[13] = Vector3d(2, 0, -0.5);
    point[14] = Vector3d(1, 0, -0.5);
    point[15] = Vector3d(0.5, 0, -1);
    point[16] = Vector3d(-1, 0, -1);
    point[17] = Vector3d(-1.5, 0, -0.5);
    point[18] = Vector3d(-1.5, 0, 0.5);
    point[19] = Vector3d(-1, 0, 1);
    for (int i = 0; i < size; i++)
    {
        mp->points.push_back(&point[i]);
        if(i + 1 >= size)
            wall[i].init(point[size - 1], point[0], 1.5, Color(255, 153, 102));
        else
            wall[i].init(point[i], point[i + 1], 1.5, Color(255, 153, 102));
    }

    bool quit = false;
    bool lockedcursor = true;
    while (!quit)
    {
        Input::Handle_events();
        quit = Input::Qiut();
        SDL_SetRenderTarget(Screen::scr->ren, Screen::scr->screen_tex);
        SDL_SetRenderDrawColor(Screen::scr->ren, 0, 0, 0, 255);
        SDL_RenderClear(Screen::scr->ren);

        //////////////////////////////////UPDATE//////////////////////////////////

        SortWalls(wall, size);

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

        for (int i = 0; i < size; i++)
            wall[i].Draw();

        mp->Draw();
        //wall1.Draw();

        mcam->fov += Input::Get_scroll_whell() * Time::Delta_time() * 500;
        std::cout << 1.0f / Time::Delta_time() << '\n';

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