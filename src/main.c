#include "../include/my_core_bot.h"

void	ft_init_func(void *data);
void	ft_user_loop(void *data);

int	main(int argc, char **argv)
{
	// ft_enable_debug(); // uncomment this to show more debug information in the console when running a game
	ft_init_con(NULL, &argc, argv);
	ft_loop(&ft_init_func, &ft_user_loop, NULL);
	ft_close_con();
	return (0);
}

// this function is called once at the start of the game
void	ft_init_func(void *data)
{
	(void)data;

	printf("Init CORE Bot\n");
}

#include <stdio.h>

void	ft_user_loop(void *data)
{
	(void)data;

	// Definitions
	t_obj **my_units = ft_get_my_units();
	t_obj **my_workers = filter_units_by_type(my_units, UNIT_WORKER);
	t_obj **my_warriors = filter_units_by_type(my_units, UNIT_WARRIOR);

	t_obj **opponent_units = ft_get_opponent_units();

	t_obj *my_core = ft_get_my_core();
	t_obj *opponent_core = ft_get_first_opponent_core();

	int my_worker_count = count_units_by_type(my_units, UNIT_WORKER);
	int my_warrior_count = count_units_by_type(my_units, UNIT_WARRIOR);
	int opponent_warrior_count = count_units_by_type(opponent_units, UNIT_WARRIOR);

	// CREATE UNITS
	if (my_warrior_count < opponent_warrior_count || my_worker_count >= 20)
		ft_create_type_id(UNIT_WARRIOR);
	else
		ft_create_type_id(UNIT_WORKER);

	// LOGIC FOR WORKERS
	int worker_index = 0;
	while (my_workers[worker_index] != NULL)
	{
		t_obj *current_worker = my_workers[worker_index];
		t_obj **other_workers = get_units_except(my_workers, current_worker);

		t_obj **sorted_resources = get_resources_sorted_by_distance(game.resources, current_worker);

		// The attacking vs escaping logic
		t_obj *nearest_opponent_unit = ft_get_nearest_opponent_unit(current_worker);
		double distance_to_nearest_opponent = ft_distance(current_worker, nearest_opponent_unit);
		if (distance_to_nearest_opponent < 800)
		{
			// Go help your ally! 
			int ally_units_in_radius_count = count_units_in_radius(nearest_opponent_unit, other_workers, 200);
			if (ally_units_in_radius_count > 0)
			{
				ft_travel_to_obj(current_worker, nearest_opponent_unit);
				ft_attack(current_worker, nearest_opponent_unit);
				worker_index++;
				continue;
			}
			// Escape
			if (is_collective_hp_less(current_worker, my_units, opponent_units))
			{
				ft_travel_to_obj(current_worker, my_core);
				ft_attack(current_worker, nearest_opponent_unit);
				worker_index++;
				continue;
			}
			// Go for the kill
			else
			{
				ft_travel_to_obj(current_worker, nearest_opponent_unit);
				ft_attack(current_worker, nearest_opponent_unit);
				worker_index++;
				continue;
			}
		}

		// RESOURCE MINING LOGIC
		int resource_index = 0;
		while (sorted_resources[resource_index] != NULL)
		{
			t_obj *current_resource = sorted_resources[resource_index];

			int other_workers_count_on_resource = count_units_in_radius(current_resource, other_workers, 800);
			
			t_obj *closest_worker = get_closest_unit(current_resource, my_workers);
			int i_am_the_closest = closest_worker->id == current_worker->id;
			if (i_am_the_closest)
			{
				ft_travel_attack(current_worker, current_resource);
				break;
			}
			if (ft_distance(current_worker, current_resource) < 150)
			{
				ft_travel_attack(current_worker, current_resource);
				break;
			}
			if (other_workers_count_on_resource == 0)
			{
				ft_travel_attack(current_worker, current_resource);
				break;
			}

			resource_index++;
		}

		worker_index++;
		free(other_workers);
		free(sorted_resources);
	}

	// LOGIC FOR WARRIORS
	int warrior_index = 0;
	while (my_warriors[warrior_index] != NULL)
	{
		t_obj *current_warrior = my_warriors[warrior_index];
		t_obj *nearest_opponent_unit = ft_get_nearest_opponent_unit(current_warrior);

		double distance_to_opponent_core = ft_distance(current_warrior, opponent_core);
		double distance_to_nearest_opponent = ft_distance(current_warrior, nearest_opponent_unit);

		// If there aren't any opponents left, just simply go attack the core
		if (nearest_opponent_unit == NULL || \
			distance_to_opponent_core < distance_to_nearest_opponent)
		{
			ft_travel_to_obj(current_warrior, opponent_core);
			ft_attack(current_warrior, opponent_core);
			warrior_index++;
			continue;
		}

		// If the opponent is so close that we cannot escape anymore, just fuck it and attack them
		if (distance_to_nearest_opponent < 200)
		{
			ft_travel_to_obj(current_warrior, nearest_opponent_unit);
			ft_attack(current_warrior, nearest_opponent_unit);
			warrior_index++;
			continue;
		}
		if (distance_to_nearest_opponent < 1000)
		{
			// Now here we are deciding whether or not to go for the kill
			if (is_collective_hp_less(current_warrior, my_units, opponent_units))
			{
				ft_travel_to_obj(current_warrior, my_core);
				ft_attack(current_warrior, nearest_opponent_unit);
				warrior_index++;
				continue;
			}
			else
			{
				ft_travel_to_obj(current_warrior, nearest_opponent_unit);
				ft_attack(current_warrior, nearest_opponent_unit);
				warrior_index++;
				continue;
			}
		}

		// AWESOME ATTACKING SPREADING
		// t_obj **sorted_opponent_units = get_resources_sorted_by_distance(*opponent_units, current_warrior);
		// int opponent_index = 0;
		// while (sorted_opponent_units[opponent_index] != NULL)
		// {
		// 	t_obj *current_opponent = sorted_opponent_units[opponent_index];

		// 	int other_workers_count_on_resource = count_units_in_radius(current_opponent, current_warrior, 800);
			
		// 	t_obj *closest_worker = get_closest_unit(current_opponent, my_workers);
		// 	int i_am_the_closest = closest_worker->id == current_warrior->id;
		// 	if (i_am_the_closest)
		// 	{
		// 		ft_travel_attack(current_warrior, current_opponent);
		// 		break;
		// 	}
		// 	if (ft_distance(current_warrior, current_opponent) < 150)
		// 	{
		// 		ft_travel_attack(current_warrior, current_opponent);
		// 		break;
		// 	}
		// 	if (other_workers_count_on_resource == 0)
		// 	{
		// 		ft_travel_attack(current_warrior, current_opponent);
		// 		break;
		// 	}

		// 	opponent_index++;
		// }

		ft_travel_attack(current_warrior, nearest_opponent_unit);

		warrior_index++;
	}

	free(my_units);
	free(my_workers);
	free(my_warriors);
	free(opponent_units);
}
