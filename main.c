/* How does raymarching work?
 * 1. Find distance to nearest object from the POV.
 * 2. Goto <point> with that distance.
 * 3. Get next nearest object.
 * 3. Continue going to <point> by distance to nearest object.
 * 4. Distance for next nearest object is less than threshold? if no, goto step 3
 * 5. If you are here, then you have hit an object, or have come close enough to count it as hit.
 * 6. Color pixel to a colour of designated object.
 */
/*         |
 *         |
 *         |
 *         |
 *         |
 *         |
 *         |
 *         |
 *         |
 *         |
 *         |
 *         |
 *         |
 *         |(0,0)___________________________________
 *        /                                         /
 *       /             cam(5,5,5)                  /
 *      /                  |                      /
 *     /                   |                     /
 *    /                    |                    /
 *   /                     |                   /
 *  /                      |                  /
 * /_______________________|_________________/
 */
#include <stdio.h>

int main(int argc, char **argv){
    (void) argc;
    (void) argv;
    printf("Hello World!\n");
    return 0;
}
