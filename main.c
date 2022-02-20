//#include "headers.h"
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"

#define MAX_STRING_LENGTH 100

int H;
int A;
int N;

int spectate;

int H_occupied = 0;
int A_occupied = 0;
int N_occupied = 0;

pthread_mutex_t mutex_seat = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_goal = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_goal = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutex_spectate = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_spectate = PTHREAD_COND_INITIALIZER;

pthread_mutex_t goal_lock = PTHREAD_MUTEX_INITIALIZER;

int num_groups;
int num_of_goal_scoring_chances;

int h_goals = 0;
int a_goals = 0;

time_t tic;
time_t toc;

struct person
{
    int person_id;
    char name[MAX_STRING_LENGTH];
    char type;
    time_t reached;
    int patience;
    int enraged;
    int seated; // 1 is seated
    time_t tic;
    time_t toc;
    char zone;

    struct timespec timeout;
};

struct group
{
    int num_of_people;
    struct person *people;
};

struct chance_details
{
    char team;
    int time;
    float probablity;
};

struct thread_for_person
{
    struct person *myperson;
};
struct goals
{
    struct chance_details *chances;
};
void *person_function(void *thread_input);
void *goal_scoring_function(void *thread_input);

int main()
{
    srand(time(NULL));
    scanf("%d %d %d", &H, &A, &N);

    scanf("%d", &spectate);

    scanf("%d", &num_groups);
    struct group groups[num_groups];

    int count = 0;
    for (int i = 0; i < num_groups; i++)
    {
        int num_of_people;
        scanf("%d", &num_of_people);
        groups[i].num_of_people = num_of_people;
        groups[i].people = (struct person *)malloc(sizeof(struct person) * num_of_people);

        for (int j = 0; j < num_of_people; j++)
        {
            count++;
            groups[i].people[j].person_id = j;
            scanf("%s", groups[i].people[j].name);
            getchar();
            scanf("%c", &groups[i].people[j].type);
            scanf("%ld", &groups[i].people[j].reached);
            scanf("%d", &groups[i].people[j].patience);
            scanf("%d", &groups[i].people[j].enraged);
            groups[i].people[j].seated = 0;

            // groups[i].people[j].timeout = (struct timespec *)malloc(sizeof(struct timespec));
            // clock_gettime(CLOCK_REALTIME, groups[i].people[j].timeout);
        }
    }

    scanf("%d", &num_of_goal_scoring_chances);
    struct chance_details chances[num_of_goal_scoring_chances];
    for (int i = 0; i < num_of_goal_scoring_chances; i++)
    {
        getchar();
        scanf("%c", &chances[i].team);
        scanf("%d", &chances[i].time);
        scanf("%f", &chances[i].probablity);
    }

    tic = time(NULL);
    pthread_t threads[count];

    struct goals *goals = (struct goals *)malloc(sizeof(struct goals));
    goals->chances = chances;

    pthread_t goal_scoring_thread;
    pthread_create(&goal_scoring_thread, NULL, goal_scoring_function, goals);

    count = 0;
    for (int i = 0; i < num_groups; i++)
    {
        for (int j = 0; j < groups[i].num_of_people; j++)
        {

            struct thread_for_person *thread_input = (struct thread_for_person *)(malloc(sizeof(struct thread_for_person)));
            thread_input->myperson = &groups[i].people[j];
            pthread_create(&threads[count], NULL, person_function, (void *)(thread_input));
            count++;
        }
    }
    pthread_join(goal_scoring_thread, NULL);
    for (int i = 0; i < num_groups; i++)
    {
        for (int j = 0; j < groups[i].num_of_people; j++)
        {
            pthread_join(threads[count], NULL);
        }
    }
}

void *goal_scoring_function(void *thread_input)
{
    struct goals *goals = ((struct goals *)thread_input);
    struct chance_details *chances = goals->chances;

    for (int i = 0; i < num_of_goal_scoring_chances; i++)
    {
        toc = time(NULL);
        int diff = toc - tic;
        if (diff < chances[i].time)
        {
            sleep(chances[i].time - diff);
        }
        float prob = (float)rand() / (float)RAND_MAX;
        if (chances[i].team == 'H' && chances[i].probablity > prob)
        {
            pthread_mutex_lock(&goal_lock);
            h_goals += 1;
            pthread_mutex_unlock(&goal_lock);

            if (h_goals == 1)
            {
                printf(CYAN "Team H have scored their %dst goal\n" RESET, h_goals);
                pthread_mutex_lock(&mutex_goal);
                pthread_cond_broadcast(&cond_goal);
                pthread_mutex_unlock(&mutex_goal);
            }
            else if (h_goals == 2)
            {
                printf(CYAN "Team H have scored their %dnd goal\n" RESET, h_goals);
                pthread_mutex_lock(&mutex_goal);
                pthread_cond_broadcast(&cond_goal);
                pthread_mutex_unlock(&mutex_goal);
            }
            else if (h_goals == 3)
            {
                printf(CYAN "Team H have scored their %drd goal\n" RESET, h_goals);
                pthread_mutex_lock(&mutex_goal);
                pthread_cond_broadcast(&cond_goal);
                pthread_mutex_unlock(&mutex_goal);
            }
            else
            {
                printf(CYAN "Team H have scored their %dth goal\n" RESET, h_goals);
                pthread_mutex_lock(&mutex_goal);
                pthread_cond_broadcast(&cond_goal);
                pthread_mutex_unlock(&mutex_goal);
            }
        }
        else if (chances[i].team == 'A' && chances[i].probablity > prob)
        {
            pthread_mutex_lock(&goal_lock);
            a_goals += 1;
            pthread_mutex_unlock(&goal_lock);

            if (a_goals == 1)
            {
                printf(CYAN "Team A have scored their %dst goal\n", a_goals);
                pthread_mutex_lock(&mutex_goal);
                pthread_cond_broadcast(&cond_goal);
                pthread_mutex_unlock(&mutex_goal);
            }
            else if (a_goals == 2)
            {
                printf(CYAN "Team A have scored their %dnd goal\n", a_goals);
                pthread_mutex_lock(&mutex_goal);
                pthread_cond_broadcast(&cond_goal);
                pthread_mutex_unlock(&mutex_goal);
            }
            else if (a_goals == 3)
            {
                printf(CYAN "Team A have scored their %drd goal\n", a_goals);
                pthread_mutex_lock(&mutex_goal);
                pthread_cond_broadcast(&cond_goal);
                pthread_mutex_unlock(&mutex_goal);
            }
            else
            {
                printf(CYAN "Team A have scored their %dth goal\n", a_goals);
                pthread_mutex_lock(&mutex_goal);
                pthread_cond_broadcast(&cond_goal);
                pthread_mutex_unlock(&mutex_goal);
            }
        }
        else if (chances[i].team == 'H' && chances[i].probablity <= prob)
        {
            if (h_goals == 0)
            {
                printf(CYAN "Team H missed the chance to score their %dst goal\n", h_goals + 1);
            }
            else if (h_goals == 1)
            {
                printf(CYAN "Team H missed the chance to score their %dnd goal\n", h_goals + 1);
            }
            else if (h_goals == 2)
            {
                printf(CYAN "Team H missed the chance to score their %drd goal\n", h_goals + 1);
            }
            else
            {
                printf(CYAN "Team H missed the chance to score their %dth goal\n", h_goals);
            }
        }
        else if (chances[i].team == 'A' && chances[i].probablity <= prob)
        {
            if (a_goals == 0)
            {
                printf(CYAN "Team A missed the chance to score their %dst goal\n", a_goals + 1);
            }
            else if (a_goals == 1)
            {
                printf(CYAN "Team A missed the chance to score their %dnd goal\n", a_goals + 1);
            }
            else if (a_goals == 2)
            {
                printf(CYAN "Team A missed the chance to score their %drd goal\n", a_goals + 1);
            }
            else
            {
                printf(CYAN "Team A missed the chance to score their %dth goal\n", a_goals);
            }
        }
    }
    //printf((CYAN"Match Over\n"));
    return NULL;
}

void *person_function(void *thread_input)
{
    struct person *myperson = ((struct thread_for_person *)thread_input)->myperson;

    sleep(myperson->reached);
    myperson->tic = time(NULL);

    printf(RED "%s has reached the stadium\n", myperson->name);
    fflush(stdout);
    while (1)
    {
        myperson->toc = time(NULL);
        int time_elapsed = (myperson->toc - myperson->tic);
        if (time_elapsed >= myperson->patience)
        {
            myperson->seated = 0;
            break;
        }

        if (myperson->type == 'H')
        {
            pthread_mutex_lock(&mutex_seat);
            if (H_occupied < H)
            {
                H_occupied++;
                myperson->seated = 1;
                myperson->zone = 'H';
            }
            else if (N_occupied < N)
            {
                N_occupied++;
                myperson->seated = 1;
                myperson->zone = 'N';
            }
            pthread_mutex_unlock(&mutex_seat);
        }
        else if (myperson->type == 'A')
        {
            pthread_mutex_lock(&mutex_seat);
            if (A_occupied < A)
            {
                A_occupied++;
                myperson->seated = 1;
                myperson->zone = 'A';
            }
            pthread_mutex_unlock(&mutex_seat);
        }
        else if (myperson->type == 'N')
        {
            pthread_mutex_lock(&mutex_seat);
            if (H_occupied < H)
            {
                H_occupied++;
                myperson->seated = 1;
                myperson->zone = 'H';
            }
            else if (A_occupied < A)
            {
                A_occupied++;
                myperson->seated = 1;
                myperson->zone = 'A';
            }
            else if (N_occupied < N)
            {
                N_occupied++;
                myperson->seated = 1;
                myperson->zone = 'N';
            }
            pthread_mutex_unlock(&mutex_seat);
        }

        if (myperson->seated == 1)
        {
            myperson->tic = time(NULL); // Saving the time the specific person has got a seat
            clock_gettime(CLOCK_REALTIME, &myperson->timeout);
            myperson->timeout.tv_sec += myperson->patience;
            printf(MAGENTA "%s has got a seat in zone %c\n", myperson->name, myperson->zone);
            fflush(stdout);
            break;
        }
        //Waiting for a seat to be available
        pthread_mutex_lock(&mutex_spectate);
        pthread_cond_wait(&cond_spectate, &mutex_spectate);
        pthread_mutex_unlock(&mutex_spectate);
    }
    if (myperson->seated == 0)
    {
        printf(MAGENTA "%s couldn’t get a seat\n", myperson->name);
        fflush(stdout);
    }
    else
    {
        while (1)
        {
            myperson->toc = time(NULL);
            int time_elapsed = (myperson->toc - myperson->tic);

            if (time_elapsed >= spectate)
            {
                myperson->seated = 0;
                printf(GREEN "%s watched the match for %d seconds and is leaving\n", myperson->name, spectate);
                fflush(stdout);
                pthread_mutex_lock(&mutex_seat);
                if (myperson->zone == 'H')
                {
                    H_occupied--;
                }
                else if (myperson->zone == 'N')
                {
                    N_occupied--;
                }
                else if (myperson->zone == 'A')
                {
                    A_occupied--;
                }
                pthread_mutex_unlock(&mutex_seat);
                pthread_mutex_lock(&mutex_spectate);
                pthread_cond_broadcast(&cond_spectate);
                pthread_mutex_unlock(&mutex_spectate);
                break;
            }
            if (myperson->type == 'H')
            {
                if (myperson->enraged <= a_goals)
                {
                    printf(GREEN "%s is leaving due to the bad defensive performance of his team\n", myperson->name);
                    fflush(stdout);
                    pthread_mutex_lock(&mutex_seat);
                    if (myperson->zone == 'H')
                    {
                        H_occupied--;
                    }
                    else if (myperson->zone == 'N')
                    {
                        N_occupied--;
                    }
                    else if (myperson->zone == 'A')
                    {
                        A_occupied--;
                    }
                    pthread_mutex_unlock(&mutex_seat);
                    pthread_mutex_lock(&mutex_spectate);
                    pthread_cond_broadcast(&cond_spectate);
                    pthread_mutex_unlock(&mutex_spectate);
                    break;
                }
            }
            else if (myperson->type == 'A')
            {
                if (myperson->enraged <= h_goals)
                {
                    printf(GREEN "%s is leaving due to the bad defensive performance of his team\n", myperson->name);
                    fflush(stdout);
                    pthread_mutex_lock(&mutex_seat);
                    if (myperson->zone == 'H')
                    {
                        H_occupied--;
                    }
                    else if (myperson->zone == 'N')
                    {
                        N_occupied--;
                    }
                    else if (myperson->zone == 'A')
                    {
                        A_occupied--;
                    }
                    pthread_mutex_unlock(&mutex_seat);
                    pthread_mutex_lock(&mutex_spectate);
                    pthread_cond_broadcast(&cond_spectate);
                    pthread_mutex_unlock(&mutex_spectate);
                    break;
                }
            }
            pthread_mutex_lock(&mutex_goal);
            pthread_cond_timedwait(&cond_goal, &mutex_goal, &myperson->timeout);
            pthread_mutex_unlock(&mutex_goal);
            // sleep(1);
        }
    }
    printf(RESET"%s is leaving for dinner\n", myperson->name);
    fflush(stdout);
    return NULL;
}