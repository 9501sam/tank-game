#include <stdlib.h> 

#include "map.h"

map *newmap(void)
{
    map *ret = (map *)malloc(sizeof(map));
    if (ret == NULL)
        err_exit("newmap");
    for (int i = 0; i < MAP_HEIGHT; i++)
        for (int j = 0; j < MAP_WIDTH; j++)
            ret->m[i][j] = '0';
    return ret;
}
