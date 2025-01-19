#ifndef MY_CORE_BOT_H
#define MY_CORE_BOT_H

#include "/core/con_lib.h"
#include <time.h>
#include <stdio.h>

// your defines here
void warrior_move(t_obj* war);
void worker_move(t_obj* war);



typedef struct s_my_info {
    bool* resource_avaliable; // array with resource ids to their avaliablility, same structure as game.resources
    int resource_size;
} t_my_info;

t_my_info *info;
time_t start_time;

void update_my_info();
int obj_size(const t_obj *objs);
void init_my_info();
bool *resource_avaliable_by_id(unsigned long id); // pointer to bool representing if resource if avaliable
t_obj *resource_by_priority(t_obj* worker); // sort resource by priority for worker
bool is_greater_priority(t_obj* worker, t_obj res1, t_obj res2); // true if res1 has greater priority, if id == 0, always returns the other one
t_obj *resource_by_priority(t_obj* worker);
int priority(t_obj* worker, t_obj res);
int my_warrior_num();
int my_worker_num();
int opponent_warrior_num();
int opponent_worker_num();
int get_stack_x();
int get_stack_y();
bool attack_unit_if_close(t_obj* attacker);
bool attack_resource_if_close(t_obj* attacker);
bool attack_core_if_close(t_obj* attacker);


#endif
