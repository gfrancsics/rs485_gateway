/* 
 * File:   statemachine.h
 * Author: Francsics Gergely
 *
 * Created on 2021. január 5., 17:33
 */

#ifndef STATEMACHINE_H
#define	STATEMACHINE_H

#ifdef	__cplusplus
extern "C" {
#endif
    
typedef struct input_st
{
    unsigned int mode_active:1;
    unsigned int enable_active:1;
    unsigned char direction:1;
    unsigned int potmeter_value;
}input_st;

typedef struct state_t
{
	unsigned char id;
	void (*funcp_entry_state)(struct input_st, unsigned char);
	struct state_t* (*funcp_state)(struct input_st, unsigned char);
	void (*funcp_exit_state)(struct input_st, unsigned char);
} state_t;

static state_t* current_state_p;

void run_state_machine(input_st input, unsigned char failure);
void initialize_state_machine(state_t* p_state);
state_t* register_state(unsigned char id, void (*func_entry)(input_st,unsigned char), state_t* (*func)(input_st,unsigned char), void (*func_exit)(input_st,unsigned char));

#ifdef	__cplusplus
}
#endif

#endif	/* STATEMACHINE_H */

