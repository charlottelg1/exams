#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int H;
int W;
char C;
char **TAB;

typedef struct line {
	char m;
	float x;
	float y;
	float r;
	char c;
} t_line;

int	err_msg(FILE *fd, int err)
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

int	main(int ac, char **av)
{
	int res;
	int i;
	FILE *fd;
	t_line line;
	int sqr;
	int col;
	int row;

	fd = NULL;
	if (ac != 2)
		return (err_msg(fd, 1));
	if ((fd = fopen(av[1], "r")))
	{
		if ((res = fscanf(fd, "%d %d %c", &W, &H, &C)) == 3)
		{
			if (W >0 && W <= 300 && H > 0 && H <= 300)
			{
				TAB = (char **)malloc(sizeof(char *) * H);
				if (!TAB)
					return (err_msg(fd, 0));
				i = 0;
				while (i < H)
				{
					TAB[i] = (char *)malloc(sizeof(char) * W);
					if (!TAB[i])
						return (err_msg(fd, 0));
					memset(TAB[i], C, W);
					i++;
				}
				while (1)
				{
					res = fscanf(fd, "\n%c %f %f %f %c", &line.m, &line.x, &line.y, &line.r, &line.c);
					if (res == -1)
						return (err_msg(fd, 0));
					if (res != 5 || line.r <0 || (line.m != 'c' && line.m != 'C'))
						break;
					row = 0;
					while (row < H)
					{
						col = 0;
						while (col < W)
						{
							sqr = sqrtf((powf(col - line.x, 2)) + (powf(row - line.y,2)));
							if (sqr <= line.r)
								if (((sqr + 1 > line.r && line.m == 'c') || line.m == 'C'))
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
