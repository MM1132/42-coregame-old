#include "../include/my_core_bot.h"

void	ft_init_func(void *data);
void	ft_user_loop(void *data);

const time_t FINISH_TIME = 235;

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
	start_time = time(NULL);
	// ft_print_all();
	init_my_info();
}

void    ft_user_loop(void *data)
{
    (void)data;


	update_my_info();
    // get all units of own team
    t_obj **units = ft_get_my_units();

    // get the first opponent core there is

	if (my_worker_num() < 5 && time(NULL) - start_time < FINISH_TIME)  {
	    ft_create_type_id(UNIT_WORKER); // try to create a warrior
	} else {
		ft_create_type_id(UNIT_WARRIOR);
	}
    int i = -1;
    while (units[++i]) // loop through every of our units
    {
        t_obj *curr = units[i];
        if (curr->s_unit.type_id == UNIT_WARRIOR) // if the unit is a warrior
        {
			warrior_move(curr);
            // t_obj *war = ft_get_nearest_opponent_unit(curr); // try to get the closest core to current unit
            // if (war)
            //     ft_travel_attack(curr, war); // travel and then attack to the obj
            // else
            //     ft_travel_attack(curr, enemy_core);
        } else {
			worker_move(curr);
		}
    }

    free(units);
}

void warrior_move(t_obj* self) {
	time_t time_passed = time(NULL) - start_time;
	if (time_passed > FINISH_TIME) {
		if (attack_core_if_close(self) == false) {
			attack_unit_if_close(self);
		}
		ft_travel_to(self, ft_get_first_opponent_core()->x, ft_get_first_opponent_core()->y);
		return;
	}
	attack_unit_if_close(self);
	int stack_x = get_stack_x();
	int stack_y = get_stack_y();
	t_obj *war = ft_get_nearest_opponent_unit(self);
    t_obj *enemy_core = ft_get_first_opponent_core();
	if (ft_distance(self, war) < 1200) {
		ft_travel_attack(self, war);
	} else {
		if (my_warrior_num() - opponent_warrior_num() > 5) {
			ft_travel_attack(self, enemy_core);
		} else {
			ft_travel_to(self, stack_x, stack_y);
		}
	}
}

void worker_move(t_obj* self) {
	if (attack_unit_if_close(self) == false) {
		attack_resource_if_close(self);
	}
	{
		t_obj *enemy = ft_get_nearest_opponent_unit(self);
		if (ft_distance(self, enemy) < 1000) {
			bool x_outside = (int)self->x + (int)self->x - (int)enemy->x < 0 || (int)self->x + (int)self->x - (int)enemy->x > (int)game.config.width;
			bool y_outside = (int)self->y + (int)self->y - (int)enemy->y < 0 || (int)self->y + (int)self->y - (int)enemy->y > (int)game.config.height;
			if (x_outside && y_outside) {
				ft_travel_to(self, ft_get_my_core()->x, ft_get_my_core()->y);
				return;
			}
			if (x_outside) {
				ft_travel_dir(self, 0, (long)self->y - (long)enemy->y);
				return;
			}
			if (y_outside) {
				ft_travel_dir(self, (long)self->x - (long)enemy->x, 0);
				return;
			}
			ft_travel_dir(self, (long)self->x - (long)enemy->x, (long)self->y - (long)enemy->y);
			return;
		}
	}
	t_obj *sorted = resource_by_priority(self);
	for (int i = 0; i < info->resource_size; i++) {
		if (resource_avaliable_by_id(sorted[i].id) && *resource_avaliable_by_id(sorted[i].id) == true) {
			*resource_avaliable_by_id(sorted[i].id) = false;
			ft_travel_attack(self, sorted + i);
			return;
		}
	}
}

t_obj *resource_by_priority(t_obj* worker) {
	t_obj *sorted = malloc(sizeof(t_obj) * info->resource_size);
	for (int i = 0; i < info->resource_size; i++) {
		sorted[i] = game.resources[i];
	}
	for (int iter = 0; iter < info->resource_size; iter++) {
		for (int i = 0; i < info->resource_size - 1; i++) {
			if (!is_greater_priority(worker, sorted[i], sorted[i + 1])) {
				t_obj temp = sorted[i];
				sorted[i] = sorted[i + 1];
				sorted[i + 1] = temp;
			}
		}
	}
	return sorted;
}

bool is_greater_priority(t_obj* worker, t_obj res1, t_obj res2) {
	// return ft_distance(worker, &res1) < ft_distance(worker, &res2)
	return priority(worker, res1) > priority(worker, res2);
}

const int MY_INT_MIN = -2147483648;
int priority(t_obj* worker, t_obj res) {
	if (ft_distance(ft_get_nearest_opponent_unit(&res), &res) < 1000) {
		return MY_INT_MIN;
	}
	const int DISTANCE_COEF = -10;
	const int CORE_DISTANCE_COEF = -20;
	return CORE_DISTANCE_COEF * ft_distance(worker, ft_get_my_core())  + DISTANCE_COEF * ft_distance(worker, &res);
}

void init_my_info() {
	t_my_info *i = malloc(sizeof(t_my_info));
	i->resource_size = 0;
	i->resource_avaliable = NULL;
	info = i;
}

void update_my_info() {
	info->resource_size = obj_size(game.resources);
	if (info->resource_avaliable != NULL) {
		free(info->resource_avaliable);
	}
	info->resource_avaliable = malloc(info->resource_size * sizeof(bool));
	for (int i = 0; i < info->resource_size; i++) {
		info->resource_avaliable[i] = true;
	}
}

int obj_size(const t_obj *objs) {
	int i = 0;
	while (objs[i++].id != 0) {}
	return i;
}

bool *resource_avaliable_by_id(unsigned long id) {
	t_obj *resources = game.resources;
	for (int i = 0; resources[i].id != 0; i++) {
		if (resources[i].id == id) {
			return info->resource_avaliable + i;
		}
	}
	return NULL;
}

int my_worker_num() {
	int workers = 0;
	t_obj** units = ft_get_my_units();
	for (int i = 0; units[i]; i++) {
		if (units[i]->type == OBJ_UNIT && units[i]->s_unit.type_id == UNIT_WORKER) {
			workers++;
		}
	}
	free(units);
	return workers;
}

int my_warrior_num() {
	int warriors = 0;
	t_obj** units = ft_get_my_units();
	for (int i = 0; units[i]; i++) {
		if (units[i]->type == OBJ_UNIT && units[i]->s_unit.type_id == UNIT_WARRIOR) {
			warriors++;
		}
	}
	free(units);
	return warriors;
}

int opponent_worker_num() {
	int workers = 0;
	t_obj** units = ft_get_opponent_units();
	for (int i = 0; units[i]; i++) {
		if (units[i]->type == OBJ_UNIT && units[i]->s_unit.type_id == UNIT_WORKER) {
			workers++;
		}
	}
	free(units);
	return workers;
}

int opponent_warrior_num() {
	int warriors = 0;
	t_obj** units = ft_get_opponent_units();
	for (int i = 0; units[i]; i++) {
		if (units[i]->type == OBJ_UNIT && units[i]->s_unit.type_id == UNIT_WARRIOR) {
			warriors++;
		}
	}
	free(units);
	return warriors;
}

int get_stack_x() {
	if (ft_get_my_core()->x > 9000 ) {
		return 9000;
	} else {
		return 1000;
	}
}

int get_stack_y() {
	if (ft_get_my_core()->y > 9000) {
		return 9000;
	} else {
		return 1000;
	}
}

const int WORKER_RANGE = 250;
const int WARRIOR_RANGE = 500;

bool attack_resource_if_close(t_obj* attacker) {
	if (attacker->type != OBJ_UNIT) {
		return false;
	}
	int range;
	if (attacker->s_unit.type_id == UNIT_WORKER) {
		range = WORKER_RANGE;
	} else {
		range = WARRIOR_RANGE;
	}
	t_obj* closest = ft_get_nearest_resource(attacker);
	if (ft_distance(attacker, closest) < range) {
		ft_attack(attacker, closest);
		return true;
	}
	return false;
}
bool attack_unit_if_close(t_obj* attacker) {
	if (attacker->type != OBJ_UNIT) {
		return false;
	}
	int range;
	if (attacker->s_unit.type_id == UNIT_WORKER) {
		range = WORKER_RANGE;
	} else {
		range = WARRIOR_RANGE;
	}
	t_obj* closest = ft_get_nearest_opponent_unit(attacker);
	if (ft_distance(attacker, closest) < range) {
		ft_attack(attacker, closest);
		return true;
	}
	return false;
}

bool attack_core_if_close(t_obj* attacker) {
	if (attacker->type != OBJ_UNIT) {
		return false;
	}
	int range;
	if (attacker->s_unit.type_id == UNIT_WORKER) {
		range = WORKER_RANGE;
	} else {
		range = WARRIOR_RANGE;
	}
	t_obj* closest = ft_get_first_opponent_core();
	if (ft_distance(attacker, closest) < range) {
		ft_attack(attacker, closest);
		return true;
	}
	return false;
}