#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INPUT "beh.txt"
#define OUTPUT "vysledkova_listina.txt"
#define SIZE 200

typedef struct {
    int number;
    char surname[30];
    char name[30];
    int birthYear;
    char country[5];
    char club[50];
    int hour;
    int min;
    int sec;
    int totalSeconds;
    int age;
    int timeLoss;
} RUNNER;

int timeToSeconds(int h, int m, int s) {
    return h * 3600 + m * 60 + s;
}

void secondsToTime(int total, int *h, int *m, int *s) {
    *h = total / 3600;
    *m = (total % 3600) / 60;
    *s = total % 60;
}

int calculateAge(int birthYear) {
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    return local->tm_year + 1900 - birthYear;
}

RUNNER *readFile(int *count) {
    FILE *fr = fopen(INPUT, "r");
    if (!fr) {
        printf("Nelze otevřít vstupní soubor %s\n", INPUT);
        return NULL;
    }

    RUNNER *runners = NULL;
    char line[SIZE];
    int i = 0;

    while (fgets(line, SIZE, fr)) {
        RUNNER *temp = realloc(runners, (i + 1) * sizeof(RUNNER));
        if (!temp) {
            free(runners);
            printf("Chyba alokace paměti\n");
            fclose(fr);
            return NULL;
        }
        runners = temp;

        sscanf(line, "%d %s %s %d %s %[^\n] %d:%d:%d",
               &runners[i].number,
               runners[i].surname,
               runners[i].name,
               &runners[i].birthYear,
               runners[i].country,
               runners[i].club,
               &runners[i].hour,
               &runners[i].min,
               &runners[i].sec);

        runners[i].totalSeconds = timeToSeconds(runners[i].hour, runners[i].min, runners[i].sec);
        runners[i].age = calculateAge(runners[i].birthYear);
        i++;
    }

    fclose(fr);
    *count = i;
    return runners;
}

void printStartList(RUNNER *runners, int count) {
    printf("S T A R T O V N Í   L I S T I N A – M A R A T O N\n");
    printf("-------------------------------------------------------------\n");
    printf("číslo  příjmení     jméno        naroz.  stát  klub                     čas\n");
    printf("-------------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%5d  %-12s %-12s %4d   %-4s  %-25s %02d:%02d:%02d\n",
               runners[i].number,
               runners[i].surname,
               runners[i].name,
               runners[i].birthYear,
               runners[i].country,
               runners[i].club,
               runners[i].hour,
               runners[i].min,
               runners[i].sec);
    }
}

int countCzechs(RUNNER *runners, int count) {
    int czechs = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(runners[i].country, "CZE") == 0)
            czechs++;
    }
    return czechs;
}

RUNNER findOldest(RUNNER *runners, int count) {
    int oldest = 0;
    for (int i = 1; i < count; i++) {
        if (runners[i].birthYear < runners[oldest].birthYear)
            oldest = i;
    }
    return runners[oldest];
}

float averageAge(RUNNER *runners, int count) {
    int sum = 0;
    for (int i = 0; i < count; i++) {
        sum += runners[i].age;
    }
    return (float)sum / count;
}

int countSurnameStartsWith(RUNNER *runners, int count, char letter, int ageLimit) {
    int total = 0;
    for (int i = 0; i < count; i++) {
        if (runners[i].surname[0] == letter && runners[i].age > ageLimit)
            total++;
    }
    return total;
}

void sortByTime(RUNNER *runners, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (runners[i].totalSeconds > runners[j].totalSeconds) {
                RUNNER temp = runners[i];
                runners[i] = runners[j];
                runners[j] = temp;
            }
        }
    }
}

void calculateLosses(RUNNER *runners, int count) {
    int leaderTime = runners[0].totalSeconds;
    for (int i = 0; i < count; i++) {
        runners[i].timeLoss = runners[i].totalSeconds - leaderTime;
    }
}

void saveResults(RUNNER *runners, int count) {
    FILE *fw = fopen(OUTPUT, "w");
    if (!fw) {
        printf("Nelze otevřít výstupní soubor %s\n", OUTPUT);
        return;
    }

    fprintf(fw, "V Ý S L E D K O V Á   L I S T I N A – M A R A T O N\n");
    fprintf(fw, "-------------------------------------------------------------\n");
    fprintf(fw, "pořadí | číslo | příjmení | jméno | narozen | stát | klub | čas | ztráta\n");
    fprintf(fw, "-------------------------------------------------------------\n");

    for (int i = 0; i < count; i++) {
        int h, m, s;
        secondsToTime(runners[i].totalSeconds, &h, &m, &s);
        int lh, lm, ls;
        secondsToTime(runners[i].timeLoss, &lh, &lm, &ls);

        fprintf(fw, "%6d | %5d | %-10s | %-10s | %4d | %-4s | %-20s | %02d:%02d:%02d | +%02d:%02d:%02d\n",
                i + 1,
                runners[i].number,
                runners[i].surname,
                runners[i].name,
                runners[i].birthYear,
                runners[i].country,
                runners[i].club,
                h, m, s,
                lh, lm, ls);
    }

    fclose(fw);
}

int main(void) {
    int count;
    RUNNER *runners = readFile(&count);
    if (!runners) return 1;

    printStartList(runners, count);
    printf("\nCelkový počet závodníků: %d\n", count);
    printf("Počet českých závodníků: %d\n", countCzechs(runners, count));

    RUNNER oldest = findOldest(runners, count);
    printf("Nejstarší závodník je %s %s narozen v roce %d. Je mu %d let.\n",
           oldest.surname, oldest.name, oldest.birthYear, oldest.age);

    printf("Průměrný věk závodníků: %.2f\n", averageAge(runners, count));
    printf("Počet závodníků s příjmením na 'K': %d\n", countSurnameStartsWith(runners, count, 'K', 0));
    printf("Počet závodníků s příjmením na 'K' a věkem > 40: %d\n", countSurnameStartsWith(runners, count, 'K', 40));

    sortByTime(runners, count);
    calculateLosses(runners, count);
    saveResults(runners, count);

    free(runners);
    return 0;
}
