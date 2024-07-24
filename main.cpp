#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
#include <emscripten.h>

class Screen
{
private:
    SDL_Event e;
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::vector<SDL_FPoint> points;

public:
    Screen()
    {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_CreateWindowAndRenderer(600, 600, 0, &window, &renderer);
        SDL_RenderSetScale(renderer, 2, 2);
    }

    void pixel(float x, float y)
    {
        SDL_FPoint point;
        point.x = x;
        point.y = y;
        points.emplace_back(point);
    }

    void show()
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (auto& point : points)
        {
            SDL_RenderDrawPointF(renderer, point.x, point.y);
        }

        SDL_RenderPresent(renderer);
    }

    void clear()
    {
        points.clear();
    }

    void input()
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                SDL_Quit();
                exit(0);
            }
        }
    }
};

struct vec3
{
    float x, y, z;
};

struct connection
{
    int a, b;
};

struct context {
    Screen screen;
    std::vector<vec3> points;
    std::vector<connection> connections;
    vec3 centroid;
};

void rotate(vec3& point, float x = 1, float y = 1, float z = 1)
{
    float rad = 0;

    rad = x;
    point.y = std::cos(rad) * point.y - std::sin(rad) * point.z;
    point.z = std::sin(rad) * point.y + std::cos(rad) * point.z;

    rad = y;
    point.x = std::cos(rad) * point.x - std::sin(rad) * point.z;
    point.z = std::sin(rad) * point.x + std::cos(rad) * point.z;

    rad = z;
    point.x = std::cos(rad) * point.x - std::sin(rad) * point.y;
    point.y = std::sin(rad) * point.x + std::cos(rad) * point.y;
}

void line(Screen& screen, float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;

    float length = std::sqrt(dx * dx + dy * dy);
    float angle = std::atan2(dy, dx);

    for (int i = 0; i < length; i++)
    {
        screen.pixel(
                x1 + std::cos(angle) * i,
                y1 + std::sin(angle) * i
        );
    }
}

void mainloop(void* arg)
{
    auto ctx = static_cast<context*>(arg);

    for (auto& p : ctx->points)
    {
        p.x -= ctx->centroid.x;
        p.y -= ctx->centroid.y;
        p.z -= ctx->centroid.z;
        rotate(p, 0.006, 0.003, 0.012);
        p.x += ctx->centroid.x;
        p.y += ctx->centroid.y;
        p.z += ctx->centroid.z;
        ctx->screen.pixel(p.x, p.y);
    }

    for (auto& conn : ctx->connections)
    {
        line(ctx->screen,
             ctx->points[conn.a].x,
             ctx->points[conn.a].y,
             ctx->points[conn.b].x,
             ctx->points[conn.b].y
        );
    }

    ctx->screen.show();
    ctx->screen.clear();
    ctx->screen.input();
    SDL_Delay(30);
}

int main() {
    Screen screen;
    std::vector<vec3> points
    {
            {100, 100, 100},
            {200, 100, 100},
            {200, 200, 100},
            {100, 200, 100},

            {100, 100, 200},
            {200, 100, 200},
            {200, 200, 200},
            {100, 200, 200},
    };
    std::vector<connection> connections
    {
            {0, 4},
            {1, 5},
            {2, 6},
            {3, 7},

            {0, 1},
            {1, 2},
            {2, 3},
            {3, 0},

            {4, 5},
            {5, 6},
            {6, 7},
            {7, 4},
    };

    // Calculate centriod
    vec3 c{0,0,0};
    for (auto& p : points)
    {
        c.x += p.x;
        c.y += p.y;
        c.z += p.z;
    }
    c.x /= points.size();
    c.y /= points.size();
    c.z /= points.size();

    // Create context
    context ctx;
    ctx.screen = screen;
    ctx.points = points;
    ctx.connections = connections;
    ctx.centroid = c;

    emscripten_set_main_loop_arg(mainloop, &ctx, -1, 1);
}
