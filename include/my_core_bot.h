#ifndef MY_CORE_BOT_H
#define MY_CORE_BOT_H

#include "/core/con_lib.h"
#include <stdio.h>

// your defines here
int		count_units_by_type(t_obj **units, long unsigned int type);
t_obj	**filter_units_by_type(t_obj **units, long unsigned int type);
t_obj	**get_resources_sorted_by_distance(t_obj *resources, t_obj *unit);
int		count_units_in_radius(t_obj *obj, t_obj **objects, double radius);
t_obj	**get_units_in_radius(t_obj *obj, t_obj **objects, double radius);
t_obj	*get_closest_unit(t_obj *obj, t_obj **objects);
t_obj	**get_units_except(t_obj **units, t_obj *unit);
int		is_collective_hp_less(t_obj *curr, t_obj **my_units, t_obj **enemy_units);

#endif
