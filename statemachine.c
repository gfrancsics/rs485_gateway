#include "statemachine.h"

static unsigned char state_counter = 0;
static state_t states[10];

void initialize_state_machine(state_t* p_state)
{
    current_state_p = p_state;
}

state_t* register_state(unsigned char id, void (*func_entry)(input_st,unsigned char), state_t* (*func)(input_st,unsigned char), void (*func_exit)(input_st,unsigned char))
{
    state_t* p_state;
    states[state_counter].id = id;
    states[state_counter].funcp_entry_state = func_entry;
    states[state_counter].funcp_state = func;
    states[state_counter].funcp_exit_state = func_exit;
    p_state = &states[state_counter];
    state_counter++;
    return p_state;
}

void run_state_machine(input_st input, unsigned char failure)
{
	state_t* next_state_p;

	//main state evaluation
	next_state_p = current_state_p->funcp_state(input, failure);

	if(next_state_p != current_state_p)
	{
		//exit state of the current state if exist
		if(current_state_p->funcp_exit_state != 0)
		{
			current_state_p->funcp_exit_state(input, failure);
		}
	
		//entry state of the next state if exist
		if(next_state_p->funcp_entry_state != 0)
		{
			next_state_p->funcp_entry_state(input, failure);
		}
	}
	current_state_p = next_state_p;
}