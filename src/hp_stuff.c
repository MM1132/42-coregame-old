#include "../include/my_core_bot.h"

int is_collective_hp_less(t_obj *curr, t_obj **my_units, t_obj **enemy_units)
{
    int friendly_hp = 0;
    int enemy_hp = 0;

    int index = 0;
    while (my_units[index] != NULL)
    {
        double distance = ft_distance(curr, my_units[index]);
        if (distance < 1000)
        {
            friendly_hp += my_units[index]->hp;
        }
        index++;
    }

    index = 0;
    while (enemy_units[index] != NULL)
    {
        double distance = ft_distance(curr, enemy_units[index]);
        if (distance < 1000)
        {
            enemy_hp += enemy_units[index]->hp;
        }
        index++;
    }

    return ((friendly_hp - 1) < enemy_hp);
}
