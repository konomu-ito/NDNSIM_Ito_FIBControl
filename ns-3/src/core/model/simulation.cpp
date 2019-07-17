#include <stdio.h>
#include <stdlib.h>

#define Rand() ((doule)rand()/RAND_MAx)
#define END_TIME 1000.0
#define eran(num)
#define P1 10.0
#define P2 5.0

typedrf enum{
	E_ARRIVAL = 100,
	E_DEPARTURE
} event_type;

typedef struct _event {
	event_type type;
	double time;
	struct _event *next;
} event;

void e_arrival();
void e_departure();
void schedule(event_type type, int interval);

double time;
double l_time;
int total_queue;
int queue;
event top;

int main()
{
	event *cuurent;

	time = 0.0;
	queue = 0.0;
	total_queue = 0;

	schedule(E_ARRIVAL, P1);

	while(time < END_TIME) {
		cuurent = top.next;
		top.next = cuurent->next;

		l_time = time;
		time = cuurent->time;
		total_queue += (time - l_time) * queue;

		switch (current->type){
			case E_ARRIVAL:
				e_arrival();
		    	break;
		    case E_DEPARTURE:
		    	e_departure();
		    	break;
		    default:
		    	break;
		}
		free(cuurent);
	}
	printf("Averge = %lf")
}