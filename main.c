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

#define MAX(a,b) (((a)>(b))?(a):(b))

typedef struct Vec3 {
    float x, y, z;
} Vec3;

typedef struct Color {
    float r,g,b;
} Color;

typedef struct Sphere {
    float rad;
    Color col;
    Vec3 pos;
    float arc, drc, srt;
    // ambient_reflection_coefficient, diffuse_reflection_coefficient, specular_reflection_coefficient
} Sphere;

#define k (2000/50)
Vec3 screen = {10*k,50*k,50*k};

float GetVec3Dist(Vec3 a, Vec3 b){
    float dx, dy, dz, dist;
    dx = a.x-b.x;
    dy = a.y-b.y;
    dz = a.z - b.z;
    dist = sqrt(dx*dx+dy*dy+dz*dz);
    return dist;
}

Vec3 Vec3Normalise(Vec3 a){
    float dist = GetVec3Dist(a, (Vec3){0,0,0});
    return (Vec3){a.x/dist, a.y/dist, a.z/dist};
}

float Vec3Dot(Vec3 a, Vec3 b){
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vec3 Vec3Add(Vec3 a, Vec3 b){
    return (Vec3){a.x+b.x, a.y+b.y, a.z+b.z};
}
Vec3 Vec3Mul(Vec3 a, float kk){
    return (Vec3){a.x*kk, a.y*kk, a.z*kk};
}
Vec3 Vec3Sub(Vec3 a, Vec3 b){
    return (Vec3){a.x-b.x, a.y-b.y, a.z-b.z};
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
        {5*k , (Color){255, 0, 0}, (Vec3){20*k, 25*k, 10*k}, .75, 1.0, 1},
        {15*k, (Color){0, 255, 0} , (Vec3){30*k, 10*k, 10*k}, .75, 1.0, 1},
        {25*k , (Color){0, 0, 255}, (Vec3){60*k, 60*k, 25*k}, 0.75, 1, 1},
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
            while(ho==None && !is_hit && iterations++<100){
                if(cur_pos.z<=0){ho=Floor; break;}
                for (int i = 0; i < SPHERES; i++) {
                    if(GetVec3Dist(sphere1.pos, cur_pos) - sphere1.rad > GetVec3Dist(balls[i].pos, cur_pos) - balls[i].rad){
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
                    /* fprintf(file, "%c%c%c", (z+y)%2*255, (z+y)%2*255, (z+y)%2*255); */
                    fprintf(file, "%c%c%c", 0x30, 0x30, 0x30);
                    goto nextPixel;
                case Ceil:
                    fprintf(file, "%c%c%c", 150, 150, 255);
                    goto nextPixel;
                default: break;
            }
            if(is_hit){
                Vec3 light_pos = {0, screen.y*2, screen.z*2};
                Color col = sphere1.col;
                // Ambient lighting
                float ambient_light_intensity = 1;
                float ambient_light = ambient_light_intensity*sphere1.arc;
                // Diffuse lighting
                float light_intensity = 1;
                Vec3 surface_normal = Vec3Normalise(Vec3Sub(cur_pos,sphere1.pos));
                Vec3 light_direction = Vec3Normalise(Vec3Sub(light_pos, cur_pos));
                float diffuse_light = light_intensity * sphere1.drc *
                                        MAX(Vec3Dot(light_direction, surface_normal), 0);
                // Total lighting
                float total_lighting = (ambient_light + diffuse_light)/2; //+ specular_light;
                fprintf(file, "%c%c%c",
                            (char)((float)col.r*total_lighting),
                            (char)((float)col.g*total_lighting),
                            (char)((float)col.b*total_lighting));
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
