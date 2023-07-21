#include <iostream>
#include <cmath>
#include <vector>
#include "Base/Vectorf.h"
#include "Base/Time.h"
#include "Base/Input.h"
#include "Base/Screen.h"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
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
    static double GetSin(float angle) { return Dsin[(int)(angle * 10) % 3600]; }
    static double GetCos(float angle) { return Dcos[(int)(angle * 10) % 3600]; }
};
const double Math::pi = 3.14159265359;
double Math::Dsin[3600];
double Math::Dcos[3600];

class Camera
{
public:
    float fov = 200.0;
    Vector3d pos = Vector3d::ZERO;
    Screen* scr;
    float a;
    Camera(Screen* scr)
    { 
        this->scr = scr;
        this->pos = Vector3d::ZERO;
    }
    void WorldPointToScreen(Vector3d point, int& x, int& y)
    {
        point = Math::RotateAroundPoint3d(point, pos, a);
        point = point - pos;

        if (point.z <= 0)
            return;

        float a = fov;
        if (point.z != 0)
            a = fov / point.z;
        x = (point.x * a) + scr->GetScreenWidth() / 2;
        y = (-1 * point.y * a) + scr->GetScreenHight() / 2;
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
        if (Input::Get_key_down(SDL_SCANCODE_LEFT))
            RotateA(-1 * rotate_speed * Time::Delta_time());
        if (Input::Get_key_down(SDL_SCANCODE_RIGHT))
            RotateA(rotate_speed * Time::Delta_time());

        if (Input::Get_key_down(SDL_SCANCODE_SPACE))
            this->position.y += speed * Time::Delta_time() / 1.3;
        if (Input::Get_key_down(SDL_SCANCODE_LCTRL))
            this->position.y -= speed * Time::Delta_time() / 1.3;

        Vector3d vec;
        Vector3d look = this->GetViewDirection();
        //std::cout << look.x << " " << look.y << " " << look.z << '\n';
        Vector3d par = Math::RotateAroundPoint3d(Vector3d::LEFT, Vector3d::ZERO, 360 - this->a);

        if (Input::Get_key_down(SDL_SCANCODE_A)) { vec = vec + par; }
        if (Input::Get_key_down(SDL_SCANCODE_D)) { vec = vec + par * -1; }
        if (Input::Get_key_down(SDL_SCANCODE_S)) { vec = vec + look * -1; }
        if (Input::Get_key_down(SDL_SCANCODE_W)) { vec = vec + look; }

        this->position = this->position + vec.Normalize() * speed * Time::Delta_time();

        /*std::cout << "a = " << a << '\n';
        std::cout << "l = " << l << '\n';
        std::cout << "x = " << position.x << '\n';
        std::cout << "y = " << position.y << '\n' << '\n';*/
    }
};
Player* p = new Player(Vector3d(0, 0.5, 0), 0, 0, 2.0f, 15.0f);

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
    void Draw()
    {
        int x1, y1, x2, y2;
        int x3, y3, x4, y4;
        cam->WorldPointToScreen(point1, x1, y1);
        cam->WorldPointToScreen(point2, x2, y2);
        cam->WorldPointToScreen(point3, x3, y3);
        cam->WorldPointToScreen(point4, x4, y4);

        cam->scr->DrawLine(x1, y1, x2, y2, col);
        cam->scr->DrawLine(x3, y3, x4, y4, col);
        cam->scr->DrawLine(x1, y1, x3, y3, col);
        cam->scr->DrawLine(x2, y2, x4, y4, col);
    }
};
Camera* Wall::cam = mcam;
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
        scr->DrawLine(0, 0, w, 0, Color(255, 255, 255));
        scr->DrawLine(0, 0, 0, h, Color(255, 255, 255));
        scr->DrawLine(w, 0, w, h, Color(255, 255, 255));
        scr->DrawLine(0, h, w, h, Color(255, 255, 255));
        scr->DrawRect(1, 1, w, h, Color(0, 0, 0));

        scr->DrawPixel(w / 2 - p->position.x + 0.5, h / 2 - p->position.z + 0.5, Color(50, 50, 50));

        Vector3d look = p->GetViewDirection() * 3;
        scr->DrawPixel(p->position.x * zoom + w / 2 + 0.5, -1 * p->position.z * zoom + h / 2 + 0.5, Color(0, 255, 0));
        scr->DrawPixel(p->position.x * zoom + look.x + w / 2 + 0.5, -1 * (p->position.z * zoom + look.z) + h / 2 + 0.5, Color(0, 100, 0));

        for (Vector3d* i : points)
            scr->DrawPixel(i->x * zoom + w / 2 + 0.5, -1 * i->z * zoom + h / 2 + 0.5, Color(0, 0, 255));
    }
    void Draw2()
    {
        scr->DrawLine(0, 0, w, 0, Color(255, 255, 255));
        scr->DrawLine(0, 0, 0, h, Color(255, 255, 255));
        scr->DrawLine(w, 0, w, h, Color(255, 255, 255));
        scr->DrawLine(0, h, w, h, Color(255, 255, 255));
        scr->DrawRect(1, 1, w, h, Color(0, 0, 0));

        Vector2d rotc = Math::RotateAroundPoint2d(Vector2d::ZERO, Vector2d(p->position.x, p->position.z), p->a);
        rotc = this->WorldPointToScreen(rotc);
        scr->DrawPixel(rotc.x - p->position.x * zoom, rotc.y + p->position.z * zoom, Color(50, 50, 50));

        scr->DrawPixel(w / 2, h / 2, Color(0, 255, 0));
        scr->DrawPixel(w / 2, h / 2 - 2, Color(0, 100, 0));

        for (Vector3d* i : points)
        {
            Vector3d ni = Math::RotateAroundPoint3d(*i, p->position, p->a);
            scr->DrawPixel(ni.x * zoom - p->position.x * zoom + w / 2 + 0.5, -1 * (ni.z * zoom - p->position.z * zoom) + h / 2 + 0.5, Color(0, 0, 255));
        }
    }
};

bool InitApp()
{
    srand(time(0));

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        return 1;
}

int main(int argc, char** argv)
{
    Screen::scr->CreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, PIXEL_SCALE);
    Math::InitSinAndCos();
    Time::Set_target_framerate(120);

    Minimap* mp = new Minimap(100, 60, 4, p, Screen::scr);

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
            wall[i].init(point[i], point[0], 1.5, Color(255, 255, 50));
        else
            wall[i].init(point[i], point[i + 1], 1.5, Color(255, 255, 50));
    }

    while (!Input::Qiut())
    {
        Input::Handle_events();
        SDL_SetRenderTarget(Screen::scr->ren, Screen::scr->screen_tex);
        SDL_SetRenderDrawColor(Screen::scr->ren, 0, 0, 0, 255);
        SDL_RenderClear(Screen::scr->ren);

        //////////////////////////////////UPDATE//////////////////////////////////

        p->Move();
        mcam->pos = p->position;
        mcam->a = p->a;

        for (int i = 0; i < size; i++)
        {
            wall[i].Draw();
        }

        mp->zoom += Input::Get_scroll_whell() * Time::Delta_time() * 10;
        std::cout << 1.0f / Time::Delta_time() << '\n';

        mp->Draw2();

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