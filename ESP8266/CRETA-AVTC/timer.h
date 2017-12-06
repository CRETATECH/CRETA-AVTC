#ifndef TIMER_H
#define TIMER_H

#define SW_TIMER_MAX_NUM   5

void Timer_Init (void);
int createSWTimer (uint32_t interval, void (*Callback)(void), void* param);
int runSWTimer (int id);
void haltSWTimer (int id);
int deleteSWTimer (int id);

#endif /* TIMER_H */
