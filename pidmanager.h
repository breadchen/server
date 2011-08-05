#ifndef _PID_MANAGER_H
#define _PID_MANAGER_H

extern void init_manager();
extern int can_add();
extern int add_pid(int pid);
extern int rm_pid(int pos);
extern int get_pos(int pid);
extern int* rm_all(int* counter);

#endif // if (!_PID_MANAGER_H)
