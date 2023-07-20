#include <iostream>
#include <cmath>
#include <vector>
#include "Base/Vectorf.h"
#include "Base/Time.h"
#include "Base/Input.h"
#include "Base/Screen.h"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define PIXEL_SCALE 1

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
    Camera(Screen* scr)
    { 
        this->scr = scr;
        this->pos = Vector3d::ZERO;
    }

    void W_to_S(Vector3d point, int& x, int& y)
    {
        point = point - pos;
        if (point.z <= 0)
        {
            x = 0;
            y = 0;
            return;
        }
        //std::cout << "point.x = " << point.x << '\n';
        //std::cout << "point.y = " << point.y << '\n';
        //std::cout << "point.z = " << point.z << '\n';
        //std::cout << "point.x * fov / point.y = " << point.x * fov / point.y << '\n' << '\n';
        float a = fov; 
        if (point.z != 0)
            a = fov / point.z;
        x = (point.x * a) + scr->GetScreenWidth() / 2;
        y = (-1 * point.y * a) + scr->GetScreenHight() / 2;
    }
private:
    Screen* scr;
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
    void RotateL(float angle)
    {
        l += angle;
        if (l >= 360)
            l -= 360;
        if (l < 0)
            l += 360;
    }
    inline Vector3d GetViewDirection() { return Math::RotateAroundPoint3d(Vector3d::FORWARD, Vector3d::ZERO, 360 - this->a, this->l); }
    void Move()
    {
        float speed = this->speed;
        if (Input::Get_key_down(SDL_SCANCODE_LSHIFT))
            speed *= 2;
        if (Input::Get_key_down(SDL_SCANCODE_LEFT))
            RotateA(-1 * rotate_speed * Time::Delta_time());
        if (Input::Get_key_down(SDL_SCANCODE_RIGHT))
            RotateA(rotate_speed * Time::Delta_time());
        if (Input::Get_key_down(SDL_SCANCODE_UP))
            RotateL(rotate_speed * Time::Delta_time() / 2);
        if (Input::Get_key_down(SDL_SCANCODE_DOWN))
            RotateL(-1 * rotate_speed * Time::Delta_time() / 2);

        if (Input::Get_key_down(SDL_SCANCODE_SPACE))
            this->position.y += speed * Time::Delta_time() / 1.3;
        if (Input::Get_key_down(SDL_SCANCODE_LCTRL))
            this->position.y -= speed * Time::Delta_time() / 1.3;

        Vector3d vec;
        Vector3d look = this->GetViewDirection();
        std::cout << look.x << " " << look.y << " " << look.z << '\n';
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
Player* p = new Player(Vector3d::ZERO, 0, 0, 2.0f, 15.0f);

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
    Vector2d W_to_S(const Vector2d& point)
    {
        return Vector2d(point.x * zoom + w / 2, h / 2 - point.y * zoom);
    }
    Vector2d W_to_S(const Vector3d& point)
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
        rotc = this->W_to_S(rotc);
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

    int x1, y1;
    int x2, y2;
    int x3, y3;
    int x4, y4;
    Minimap* mp = new Minimap(100, 60, 4, p, Screen::scr);

    Vector3d point1 = Vector3d(0, -0.5, 1);
    Vector3d point2 = Vector3d(0, 1, 1);
    Vector3d point3 = Vector3d(1, -0.5, 1);
    Vector3d point4 = Vector3d(1, 1, 1);
    Vector3d point5 = Vector3d(-1, -0.5, 0);
    Vector3d point6 = Vector3d(-1, 1, 0);

    mp->points.push_back(&point1);
    mp->points.push_back(&point2);
    mp->points.push_back(&point3);
    mp->points.push_back(&point4);
    mp->points.push_back(&point5);
    mp->points.push_back(&point6);

    while (!Input::Qiut())
    {
        Input::Handle_events();
        SDL_SetRenderTarget(Screen::scr->ren, Screen::scr->screen_tex);
        
        SDL_SetRenderDrawColor(Screen::scr->ren, 0, 0, 0, 255);
        SDL_RenderClear(Screen::scr->ren);

        p->Move();

        mcam->pos = p->position;

        mcam->W_to_S(Math::RotateAroundPoint3d(point1, p->position, p->a, 360 - p->l), x1, y1);
        mcam->W_to_S(Math::RotateAroundPoint3d(point2, p->position, p->a, 360 - p->l), x2, y2);
        mcam->W_to_S(Math::RotateAroundPoint3d(point3, p->position, p->a, 360 - p->l), x3, y3);
        mcam->W_to_S(Math::RotateAroundPoint3d(point4, p->position, p->a, 360 - p->l), x4, y4);
        Screen::scr->DrawLine(x1, y1, x2, y2, Color(255, 0, 0));
        Screen::scr->DrawLine(x3, y3, x4, y4, Color(255, 0, 0));
        Screen::scr->DrawLine(x1, y1, x3, y3, Color(255, 0, 0));
        Screen::scr->DrawLine(x2, y2, x4, y4, Color(255, 0, 0));
        mcam->W_to_S(Math::RotateAroundPoint3d(point5, p->position, p->a, 360 - p->l), x3, y3);
        mcam->W_to_S(Math::RotateAroundPoint3d(point6, p->position, p->a, 360 - p->l), x4, y4);
        Screen::scr->DrawLine(x1, y1, x2, y2, Color(255, 0, 0));
        Screen::scr->DrawLine(x3, y3, x4, y4, Color(255, 0, 0));
        Screen::scr->DrawLine(x1, y1, x3, y3, Color(255, 0, 0));
        Screen::scr->DrawLine(x2, y2, x4, y4, Color(255, 0, 0));
        /*Screen::scr->DrawPixel(x, y, Color(255, 0, 0));
        mcam->W_to_S(Math::RotateAroundPoint(point2, p->position, p->a), x, y);
        Screen::scr->DrawPixel(x, y, Color(255, 0, 0));
        mcam->W_to_S(Math::RotateAroundPoint(point3, p->position, p->a), x, y);
        Screen::scr->DrawPixel(x, y, Color(255, 0, 0));
        mcam->W_to_S(Math::RotateAroundPoint(point4, p->position, p->a), x, y);
        Screen::scr->DrawPixel(x, y, Color(255, 0, 0));
        mcam->W_to_S(Math::RotateAroundPoint(point5, p->position, p->a), x, y);
        Screen::scr->DrawPixel(x, y, Color(255, 0, 0));
        mcam->W_to_S(Math::RotateAroundPoint(point6, p->position, p->a), x, y);
        Screen::scr->DrawPixel(x, y, Color(255, 0, 0));*/
        mp->zoom += Input::Get_scroll_whell() * Time::Delta_time() * 10;
        //std::cout << mcam->fov << '\n';
        /*std::cout << "x cam = " << mcam->pos.x << '\n';
        std::cout << "y cam = " << mcam->pos.y << '\n';
        std::cout << "z cam = " << mcam->pos.z << '\n';
        std::cout << "x1 = " << x1 << '\n';
        std::cout << "y1 = " << y1 << '\n';
        std::cout << "x2 = " << x2 << '\n';
        std::cout << "y2 = " << y2 << '\n';*/

        std::cout << 1.0f / Time::Delta_time() << '\n';

        mp->Draw2();

        SDL_SetRenderTarget(Screen::scr->ren, NULL);

        SDL_RenderCopy(Screen::scr->ren, Screen::scr->screen_tex, NULL, NULL);
        SDL_RenderPresent(Screen::scr->ren);

        Time::Tick();
    }
    Screen::scr->ApplicationQuit();
    delete(Screen::scr);
    return 0;
}