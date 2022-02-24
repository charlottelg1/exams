#include "libc.h"

typedef struct line
{
    char    t;
    float   x;
    float   y;
    float   w;
    float   h;
    char    c;
}line;

int     W;
int     H;
char    BG;
char    **TAB;

int main(int ac, char **av)
{
    FILE    *fd;
    line    line;
    int     res;
    int     i;
    int     row;
    int     col;
    int     rect;

    fd = NULL;
    if (ac != 2)
        return (err_msg(fd, 1));
    if ((fd = fopen(av[1], "r")))
    {
        if ((res = fscanf(fd, "%d %d %c", &W, &C, &BG)) == 3)
        {
            if (W > 0 && W <= 300 && H > 0 && H <= 300)
            {
                TAB = malloc(sizeof(char *) * H);
                i = 0
                while (i < H)
                {
                    TAB[i] = malloc(sizeof(char *) * W);
                    if (TAB[i] == 0)
                        return (err_msg(fd, 0))
                    memset(TAB[i], BG, W);
                    i++;
                }
                while (1)
                {
                    res = fscanf(fd, "\n%c %f %f %f %f %c", &line.t, &line.x, &line.y, &line.w, &line.h, &line.c);
                    if (res == -1)
                        return(err_msg(fd, 0));
                    else if (res != 6 || line.w)
                }
            }
        }
    }
}