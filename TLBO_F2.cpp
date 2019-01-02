/*Is RANDOM leagle?*/
#include <iostream>
#include <iomanip>
#include <time.h>
#include <cmath>
using namespace std;

#define PS 50 //population size
#define maxCycle 2000
#define D 7 //7 designed variables
#define LOWER_BOUND -10
#define UPPER_BOUND 10                           //i = 1,2,3,..,7
#define RANDOM (double)rand() / (RAND_MAX + 1.0) //[0,1)

double population[PS][D];
double teach_population[PS][D];
double learn_population[PS][D];
double f[PS]; //fitness of population
double f_teach[PS];
double f_learner[PS];
double globalMin[2] = {};  //global best obj value
double globalObjParams[D]; //global best obj params
double teacher[D];         //teacher
double mean[D];            //mean of population
double sum_var[D] = {};    //sumary of every designed variable
double fitness_sum = 0;

void calculateFitness(int index, double population[][D], double f[]);
int subject_to_constraints(int index, double population[][D]);

void learning()
{
    double fit = 0;

    for (int i = 0; i < PS; i++)
    {
        int threshold = 0;
        double R = RANDOM;
        int J = rand() % PS;
        if (i != J || i == 0)
        {
            if (f[i] < f[J])
            {
                for (int j = 0; j < D; j++)
                {
                    double x_new;
                    x_new = population[i][j] + R * (population[i][j] - population[J][j]);
                    learn_population[i][j] = x_new;
                }
            }
            else
            {
                for (int j = 0; j < D; j++)
                {
                    double x_new;
                    x_new = population[i][j] + R * (population[J][j] - population[i][j]);
                    learn_population[i][j] = x_new;
                }
            }

            threshold = subject_to_constraints(i, learn_population);
            if (threshold == 1)
            {
                calculateFitness(i, learn_population, f_learner);
            }
            else
                i--;
        }
        else
            i--;
    }

    for (int i = 0; i < PS; i++)
    {
        fit += f_learner[i];
    }

    if (fit < fitness_sum)
    {
        for (int i = 0; i < PS; i++)
        {
            for (int j = 0; j < D; j++)
            {
                population[i][j] = learn_population[i][j];
            }
            f[i] = f_learner[i];
        }
        fitness_sum = fit;
    }
}

void teaching()
{
    double fit = 0;
    for (int i = 0; i < PS; i++)
    {
        int threshold = 0;
        double R = RANDOM;
        int T = 1 + round(RANDOM);

        for (int j = 0; j < D; j++)
        {
            double x_new;
            x_new = population[i][j] + R * (teacher[j] - T * mean[j]);
            teach_population[i][j] = x_new;
        }

        threshold = subject_to_constraints(i, teach_population);
        if (threshold == 1)
        {
            calculateFitness(i, teach_population, f_teach);
        }
        else
        {
            i--;
        }
    }

    for (int i = 0; i < PS; i++)
    {
        fit += f_teach[i];
    }

    if (fit < fitness_sum)
    {
        for (int i = 0; i < PS; i++)
        {
            for (int j = 0; j < D; j++)
            {
                population[i][j] = teach_population[i][j];
            }
            f[i] = f_teach[i];
        }
        fitness_sum = fit;
    }
}

void iter_of_teacher()
{
    double iter_fitness = 10000;
    int teacher_num;

    for (int i = 0; i < PS; i++)
    {
        if (f[i] < iter_fitness)
        {
            iter_fitness = f[i];
            teacher_num = i;
        }
    }
    for (int i = 0; i < D; i++)
    {
        teacher[i] = population[teacher_num][i];
    }
}

void calculate_mean()
{
    /*sumary every designed variable*/
    for (int j = 0; j < PS; j++)
    {
        for (int k = 0; k < D; k++)
        {
            sum_var[k] += population[j][k];
        }
    }

    /*calculate mean*/
    for (int i = 0; i < D; i++)
    {

        mean[i] = sum_var[i] / PS;
        sum_var[i] = 0; // -> ZERO
    }
}

void memorize_best_solution()
{
    int best_solution_number, if_found = 0;

    /*find best fitness*/
    for (int j = 0; j < PS; j++)
    {
        if (f[j] < globalMin[0])
        {
            globalMin[0] = f[j];
            best_solution_number = j;
            if_found = 1;
        }
    }
    if (if_found == 1)
    {
        fitness_sum = 0;
        for (int i = 0; i < PS; i++)
        {
            fitness_sum += f[i];
        }
        for (int i = 0; i < D; i++)
        {
            globalObjParams[i] = population[best_solution_number][i];
        }
    }
}

void calculateFitness(int index, double population[][D], double f[])
{
    f[index] = 0.0;

    f[index] += (population[index][0] - 10) * (population[index][0] - 10) + 5 * (population[index][1] - 12) * (population[index][1] - 12);
    f[index] += population[index][2] * population[index][2] * population[index][2] * population[index][2];
    f[index] += 3 * (population[index][3] - 11) * (population[index][3] - 11);
    f[index] += 10 * population[index][4] * population[index][4] * population[index][4] * population[index][4] * population[index][4] * population[index][4];
    f[index] += 7 * population[index][5] * population[index][5];
    f[index] += population[index][6] * population[index][6] * population[index][6] * population[index][6];
    f[index] -= 4 * population[index][5] * population[index][6];
    f[index] -= 10 * population[index][5];
    f[index] -= 8 * population[index][6];

    /*cout << index << ": " << endl;
    cout << setprecision(9) << f[index] << endl;*/
}

int subject_to_constraints(int index, double population[][D])
{
    double g1 = 0, g2 = 0, g3 = 0, g4 = 0;
    g1 += -127 + 2 * population[index][0] * population[index][0] + 3 * (population[index][1] * population[index][1]) * (population[index][1] * population[index][1]) + population[index][2] + 4 * population[index][3] * population[index][3] + 5 * population[index][4];
    if (g1 > 0)
        return 0;

    g2 += -282 + 7 * population[index][0] + 3 * population[index][1] + 10 * population[index][2] * population[index][2] + population[index][3] - population[index][4];
    if (g2 > 0)
        return 0;

    g3 += -196 + 23 * population[index][0] + population[index][1] * population[index][1] + 6 * population[index][5] * population[index][5] - 8 * population[index][6];
    if (g3 > 0)
        return 0;

    g4 += 4 * population[index][0] * population[index][0] + population[index][1] * population[index][1] - 3 * population[index][0] * population[index][1] + 2 * population[index][2] * population[index][2] + 5 * population[index][5] - 11 * population[index][6];
    if (g4 > 0)
        return 0;
    return 1;
}

/* randomly generate 13 designed variables*/
void init(int index)
{
    for (int i = 0; i < D; i++)
    {
        population[index][i] = (UPPER_BOUND - LOWER_BOUND) * rand() / (RAND_MAX + 1.0) + LOWER_BOUND;
    }
}

void initialize()
{
    for (int i = 0; i < PS; i++)
    {
        int threshold = 0;
        init(i); /* randomly generate 7 designed variables*/
        threshold = subject_to_constraints(i, population);
        if (threshold == 1)
        {
            calculateFitness(i, population, f);
        }
        else
        {
            i--;
        }
    }
}

int main()
{
    srand(time(NULL));
    globalMin[0] = 10000;

    initialize();
    memorize_best_solution();

    for (int i = 0; i < maxCycle; i++)
    {
        // teacher_phase
        calculate_mean();
        iter_of_teacher();
        teaching();

        //learner_phase();
        learning();
        memorize_best_solution();
    }
    cout << "globalMin : " << globalMin[0] << endl;
    cout << "Params : ";
    for (int j = 0; j < D; j++)
    {
        cout << globalObjParams[j] << " ";
    }
    cout << endl;

    system("PAUSE");
    return 0;
}