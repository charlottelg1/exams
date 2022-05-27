#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

typedef struct line {
    char    t;
    float   x;
    float   y;
    float   w;
    float   h;
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

int in_rect(float row, float col, line *line)
{
    if (col < line->x || line->x + line->w < col || row < line->y || line->y + line->h < row)
        return (0);
    if ((col - line->x < 1.00000000 || line->x + line->w - col < 1.00000000 || row - line->y < 1.00000000 || line->y + line->h - row< 1.00000000))
        return (1);
    return (2);
}

int main(int ac, char **av)
{
    FILE    *fd;
    int     res;
    int     i;
    line    line;
    int     row;
    int     col;
    int     rect;

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
                    res = fscanf(fd, "\n%c %f %f %f %f %c", &line.t, &line.x, &line.y, &line.w, &line.h, &line.c);
                    if (res == -1)
                        return (err_msg(fd, 0));
                    else if (res != 6 || line.w <= 0 || line.h <= 0 || (line.t != 'r' && line.t != 'R'))
                        break ;
                    row = 0;
                    while (row < H)
                    {
                        col = 0;
                        while (col < W)
                        {
                            rect = in_rect(row, col, &line);
                            if ((rect == 1 && line.t == 'r') || ((rect == 1 || rect == 2) && line.t == 'R'))
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
