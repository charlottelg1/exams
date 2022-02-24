#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

typedef struct line {
    char    t;
    float   x;
    float   y;
    float   r;
    char    c;
} line;

int     W;
int     H;
char    BG;
char    **TAB;

int err_msg(FILE *fd, int err)
{
    int i;

    if (err == 2 && (err = 1))
        write(1, "Error: Operation file corrupted\n", 32);
    else if (err == 1)
        write(1, "Error: argument\n", 16);
    else
    {
        i = 0;
        while (i < H)
        {
            write(1, TAB[i], W);
            write(1, "\n", 1);
            i++;
        }
    }
    if (fd)
        fclose(fd);
    return (err);
}

int main(int ac, char **av)
{
    FILE    *fd;
    int     res;
    int     i;
    line    line;
    int     row;
    int     col;
    float   sqr;

    fd = NULL;
    if (ac != 2)
        return (err_msg(fd, 1));
    if ((fd = fopen(av[1], "r")))
    {
        if ((res = fscanf(fd, "%d %d %c", &W, &H, &BG)) == 3)
        {
            if (W > 0 && W <= 300 && H > 0 && H <= 300)
            {
                TAB = malloc(sizeof(char *) * H);
                if (TAB == 0)
                    return (err_msg(fd, 0));
                i = 0;
                while (i < H)
                {
                    TAB[i] = malloc(sizeof(char) * W);
                    if (TAB[i] == 0)
                        return (err_msg(fd, 0));
                    memset(TAB[i], BG, W);
                    i++;
                }
                while (1)
                {
                    res = fscanf(fd, "\n%c %f %f %f %c", &line.t, &line.x, &line.y, &line.r, &line.c);
                    if (res == -1)
                        return (err_msg(fd, 0));
                    else if (res != 5 || line.r <= 0 || (line.t != 'c' && line.t != 'C'))
                        break ;
                    row = 0;
                    while (row < H)
                    {
                        col = 0;
                        while (col < W)
                        {
                            sqr = sqrtf((powf(col - line.x, 2)) + (powf(row - line.y, 2)));
                            if (sqr <= line.r)
                                if ((sqr + 1 > line.r && line.t =='c') || line.t == 'C')
                                    TAB[row][col] = line.c;
                            col++;
                        }
                        row++;
                    }
                }
            }
        }
    }
    return (err_msg(fd, 2));
}
