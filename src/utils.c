#include "../include/my_core_bot.h"

int count_units_by_type(t_obj **units, long unsigned int type)
{
	int index = 0;
	int counter = 0;
	while (units[index] != NULL)
	{
		if (units[index]->s_unit.type_id == type)
		{
			counter++;
		}
		index++;
	}
	return (counter);
}

// We expect that the passed things are already of unit type
t_obj **filter_units_by_type(t_obj **units, unsigned long type)
{
	int unit_type_count = count_units_by_type(units, type);

	t_obj **filtered_units = malloc(sizeof(t_obj *) * (unit_type_count + 1));

	// Loop through all the units
	int index = 0;
	int filtered_units_index = 0;
	while (units[index] != NULL)
	{
		// printf("----------------\n");
		// printf("Unit: %p\n", units[index]);
		// printf("Unit type: %lu\n", units[index]->s_unit.type_id);
		// printf("Position: %lu, %lu\n", units[index]->x, units[index]->y);
		
		if (units[index]->s_unit.type_id == type)
		{
			filtered_units[filtered_units_index] = units[index];
			filtered_units_index++;
		}

		index++;
	}
	filtered_units[filtered_units_index] = NULL;
	return (filtered_units);
}

t_obj **get_resources_sorted_by_distance(t_obj *resources, t_obj *unit)
{
	t_obj *first_res_point = resources;

	int resource_count = 0;
	while (resources->id != 0)
	{
		resources++;
		resource_count++;
	}
	resources = first_res_point;
	
	t_obj **sorted_resources = malloc(sizeof(t_obj *) * (resource_count + 1));
	
	int index = 0;
	while (index < resource_count)
	{
		sorted_resources[index] = resources;

		resources++;
		index++;
	}
	sorted_resources[index] = NULL;

	// Sort by distance here
	// 6 1 8 3
	index = 0;
	while (index < resource_count - 1)
	{
		int index2 = index + 1;
		while (index2 < resource_count)
		{
			double first_distance = ft_distance(sorted_resources[index], unit);
			double second_distance = ft_distance(sorted_resources[index2], unit);

			if (second_distance < first_distance)
			{
				t_obj *tmp = sorted_resources[index];
				sorted_resources[index] = sorted_resources[index2];
				sorted_resources[index2] = tmp;
			}

			index2++;
		}

		index++;
	}
	return (sorted_resources);
}

// Return the count of `t_obj` that are within the radius of the passed `t_obj`
int count_units_in_radius(t_obj *obj, t_obj **objects, double radius)
{
	int counter = 0;

	int index = 0;
	while (objects[index] != NULL)
	{
		double distance = ft_distance(obj, objects[index]);
		if (distance <= radius)
			counter++;

		index++;
	}

	return (counter);
}

t_obj **get_units_in_radius(t_obj *obj, t_obj **objects, double radius)
{
	int units_in_radius_count = count_units_in_radius(obj, objects, radius);

	t_obj **units_in_radius = malloc(sizeof(t_obj *) * (units_in_radius_count + 1));
	if (!units_in_radius)
		return (NULL);
	int index = 0;
	int add_index = 0;
	while (objects[index] != NULL)
	{
		double distance = ft_distance(obj, objects[index]);
		if (distance < radius)
		{
			units_in_radius[add_index] = objects[index];
			add_index++;
		}
		index++;
	}
	objects[index] = NULL;
	return (units_in_radius);
}

t_obj *get_closest_unit(t_obj *obj, t_obj **objects)
{
	if (objects[0] == NULL)
		return (NULL);
	int index = 0;
	t_obj *closest_unit = objects[index];
	double closest_distance = ft_distance(obj, closest_unit);
	while (objects[index] != NULL)
	{
		double distance = ft_distance(obj, objects[index]);
		if (distance < closest_distance)
		{
			closest_distance = distance;
			closest_unit = objects[index];
		}
		index++;
	}
	return (closest_unit);
}

t_obj **get_units_except(t_obj **units, t_obj *unit)
{
	if (units[0] == NULL)
		return (NULL);
	int units_count = 0;
	while (units[units_count] != NULL)
		units_count++;
	
	t_obj **filtered_units = malloc(sizeof(t_obj *) * (units_count));
	if (!filtered_units)
		return (NULL);
	int index = 0;
	int added_count = 0;
	while (index < units_count)
	{
		if (units[index]->id != unit->id)
		{
			filtered_units[added_count] = units[index];
			added_count++;
		}
		index++;
	}
	filtered_units[added_count] = NULL;
	return (filtered_units);
}
