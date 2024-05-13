/* How does raymarching work?
 * 1. Find distance to nearest object from the POV.
 * 2. Goto <point> with that distance.
 * 3. Get next nearest object.
 * 3. Continue going to <point> by distance to nearest object.
 * 4. Distance for next nearest object is less than threshold? if no, goto step 3
 * 5. If you are here, then you have hit an object, or have come close enough to count it as hit.
 * 6. Color pixel to a colour of designated object.
 */
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

typedef struct Vec3 {
    float x, y, z;
} Vec3;

typedef struct Color {
    char r,g,b;
} Color;

typedef struct Sphere {
    float rad;
    Color col;
    Vec3 pos;
} Sphere;


#define k 10
Vec3 screen = {10*k,50*k,50*k};

float GetVec3Dist(Vec3 a, Vec3 b){
    float dx, dy, dz, dist;
    dx = a.x-b.x;
    dy = a.y-b.y;
    dz = a.z - b.z;
    dist = sqrt(dx*dx+dy*dy+dz*dz);
    return dist;
}

Vec3 Vec3Add(Vec3 a, Vec3 b){
    return (Vec3){a.x+b.x, a.y+b.x, a.z+b.z};
}

void printVec3(Vec3 a){
    printf("{%.2f, %.2f, %.2f}\n", a.x, a.y, a.z);
}

enum HitObj {Floor, Ceil, Wall, None};

int main(int argc, char **argv){
    (void) argc;
    (void) argv;
    FILE *file = fopen("file.ppm", "w");
    fprintf(file, "P6\n%d %d 255\n", (int)screen.y, (int)screen.z);
    Sphere sphere1;
    Vec3 camera = {-60, screen.y/2, screen.z/2};
#define SPHERES 3
    Sphere balls[SPHERES] = {
        /* {screen.y/3, (Color){255, 0, 0}, (Vec3){screen.x*2, screen.y/2-30, screen.z/2}} */
        {6*k, (Color){255, 0, 0}, (Vec3){20*k, screen.y*2/3, 10*k}},
        {30*k,  (Color){0, 255, 0}, (Vec3){69*k, k*5,  10*k}},
        {3*k,  (Color){0, 0, 255}, (Vec3){30*k, k*40, 25*k}},
    };
    for (int z = screen.z-1; z >=0; z--) {
        for (int y = 0; y < screen.y; y++) {
            sphere1 = balls[0];
            for (int i = 0; i < SPHERES; i++) {
                if(GetVec3Dist(sphere1.pos, camera)-sphere1.rad>GetVec3Dist(balls[i].pos, camera)-balls[i].rad){
                    sphere1 = balls[i];
                }
            }
            Vec3 screen_pixel_pos = {screen.x, y, z};
            Vec3 cur_pos = camera;
            float dist_cam = GetVec3Dist(cur_pos, screen_pixel_pos);
            float dist = GetVec3Dist(cur_pos, sphere1.pos)-sphere1.rad;
            cur_pos.x = camera.x + dist * (screen_pixel_pos.x-camera.x)/dist_cam;
            cur_pos.y = camera.y + dist * (screen_pixel_pos.y-camera.y)/dist_cam;
            cur_pos.z = camera.z + dist * (screen_pixel_pos.z-camera.z)/dist_cam;
            bool is_hit=false;
            enum HitObj ho = None;
            int iterations = 0;
            while(ho==None && !is_hit && iterations++<256){
                if(cur_pos.z<=0){ho=Floor; break;}
                for (int i = 0; i < SPHERES; i++) {
                    if(GetVec3Dist(sphere1.pos, cur_pos)-sphere1.rad>GetVec3Dist(balls[i].pos, cur_pos)-balls[i].rad){
                        sphere1 = balls[i];
                    }
                }
                dist_cam = GetVec3Dist(cur_pos, camera);
                dist = GetVec3Dist(cur_pos, sphere1.pos)-sphere1.rad;
                cur_pos.x = (dist + dist_cam )/ dist_cam * (cur_pos.x - camera.x) + camera.x;
                cur_pos.y = (dist + dist_cam )/ dist_cam * (cur_pos.y - camera.y) + camera.y;
                cur_pos.z = (dist + dist_cam )/ dist_cam * (cur_pos.z - camera.z) + camera.z;
                if(dist<=3){is_hit=true;break;}
                if(cur_pos.z<=0){ho=Floor; break;}
                /* if(cur_pos.z>=250){ho=Ceil; break;} */
            }
            switch(ho){
                case Floor:
                    /* fprintf(file, "%c%c%c", ((int)cur_pos.x+(int)cur_pos.y)%2*255, ((int)cur_pos.x+(int)cur_pos.y)%2*255, ((int)cur_pos.x+(int)cur_pos.y)%2*255); */
                    fprintf(file, "%c%c%c", (z+y)%2*255, (z+y)%2*255, (z+y)%2*255);
                    goto nextPixel;
                case Ceil:
                    fprintf(file, "%c%c%c", 255, 0, 255);
                    goto nextPixel;
                default: break;
            }
            if(is_hit){
                Color col = sphere1.col;
                fprintf(file, "%c%c%c", col.r, col.g, col.b);
            } else {
                fprintf(file, "%c%c%c", 0, 0, 0);
            }
nextPixel:
            continue;
        }
        /* fprintf(file, "\n"); */
    }
    return 0;
}
