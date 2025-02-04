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

int count_unit_by_type(t_obj **units, long unsigned int type)
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

#include <stdio.h>

typedef struct s_unit_action {
    unsigned long   id;
    int             current_action;
    int             movement_counter;
} t_unit_action;

/*
The idea is:
Always save the list of all the resources

Get the list of all resources
Get the closest one to you
Check how many workers are already mining that resource
Essentially each worker has a struct
Which saves the resource ID that this worker is currently working on

How do you store the number of workers already assigned to a resource
Each worker is always assigned to a resource

How about:
1. Get the nearest resource and try to go to mine it
2. If there is already another worker in the vicinity of that resouce, repeat the process with the next closest resource
3. If it goes through all the resources and cannot find a single "free" one, just go mine the very closest resource

*/


int count_units(t_obj **units)
{
    int counter = 0;
    while (units[counter] != NULL)
    {
        counter++;
    }
    return (counter);
}

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

// this function is called every time new data is recieved
void    ft_user_loop(void *data)
{
    static int full_fight_mode_activated = false;

    (void)data;

    t_obj **units = ft_get_my_units();

    // CREATING
    int worker_count = count_unit_by_type(units, UNIT_WORKER);
    int warrior_count = count_unit_by_type(units, UNIT_WARRIOR);

    t_obj **opponent_units = ft_get_opponent_units();
    int opponent_warriors_count = 0;
    int opponent_index = 0;
    while (opponent_units[opponent_index] != NULL)
    {
        if (opponent_units[opponent_index]->s_unit.type_id == UNIT_WARRIOR)
        {
            opponent_warriors_count++;
        }
        opponent_index++;
    }

    // Change mode! 
    if (full_fight_mode_activated == false && worker_count > 20)
    {
        full_fight_mode_activated = true;
    }
    else if (full_fight_mode_activated == true && worker_count <= 10)
    {
        full_fight_mode_activated = false;
    }

    if ((worker_count > 5 && opponent_warriors_count >= warrior_count) || worker_count > 15)
    {
        ft_create_type_id(UNIT_WARRIOR);
    }
    else
    {
        ft_create_type_id(UNIT_WORKER);
    }

    // DO THINGS
    int i = 0;
    while (units[i] != NULL)
    {
        t_obj *curr = units[i];
        if (curr->s_unit.type_id == UNIT_WORKER)
        {
            

            // const all_friendly_workers = get_all_friendly_workers();

            // const sorted_resources = get_resources_sorted_by_distance(curr);
            // const resources_count = count_resources(sorted_resources);

            // for (int i = 0; i < resources_count; i++)
            // {
            //     const worker_count = count_friendly_workers_in_radius(sorted_resources[i], all_friendly_workers);
            // }


            // Check if collective close HP is less
            // Then just move towards the core and attack closest enemy
            int collective_hp_less = is_collective_hp_less(curr, units, opponent_units);

            t_obj *nearest_resource = ft_get_nearest_resource(curr);
            t_obj *nearest_enemy = ft_get_nearest_opponent_unit(curr);
            t_obj *core = ft_get_my_core();
            t_obj *opponent_core = ft_get_first_opponent_core();
            
            double to_resource = ft_distance(curr, nearest_resource);
            // double to_opponent_core = ft_distance(curr, opponent_core);
            double to_enemy = ft_distance(curr, nearest_enemy);
            if (full_fight_mode_activated)
            {
                ft_travel_attack(curr, opponent_core);
            }
            else if (collective_hp_less)
            {
                ft_travel_to_obj(curr, core);
                ft_attack(curr, nearest_enemy);
            }
            else if ((to_enemy < to_resource))
            {
                ft_travel_attack(curr, nearest_enemy);
            }
            else
            {
                ft_travel_attack(curr, nearest_resource);
            }
        }
        else if (curr->s_unit.type_id == UNIT_WARRIOR)
        {
            int collective_hp_less = is_collective_hp_less(curr, units, opponent_units);

            t_obj *nearest_opponent = ft_get_nearest_opponent_unit(curr);
            t_obj *opponent_core = ft_get_first_opponent_core();
            t_obj *my_core = ft_get_my_core();

            double to_opponent = ft_distance(curr, nearest_opponent);
            double to_opponent_core = ft_distance(curr, opponent_core);
            double closest_distance = to_opponent < to_opponent_core ? to_opponent : to_opponent_core;

            if (full_fight_mode_activated)
            {
                ft_travel_attack(curr, opponent_core);
            }
            else if (collective_hp_less)
            {
                ft_travel_to_obj(curr, my_core);
                ft_attack(curr, nearest_opponent);
                i++;
                continue;
            }

            if (closest_distance < 1500)
            {
                if ((to_opponent + 50) < to_opponent_core)
                {
                    ft_travel_attack(curr, nearest_opponent);
                }
                else
                {
                    ft_travel_attack(curr, opponent_core);
                }

                i++;
                continue;
            }

            int warrior_count = count_unit_by_type(units, UNIT_WARRIOR);
            if (warrior_count <= 3)
            {
                t_obj *our_core = ft_get_my_core();

                ft_travel_to(curr, our_core->x, our_core->y);
                i++;
                continue;
            }

            if (to_opponent < to_opponent_core)
            {
                ft_travel_attack(curr, nearest_opponent);
            }
            else
            {
                ft_travel_attack(curr, opponent_core);
            }
        }
        i++;
    }

    free(units);
}
